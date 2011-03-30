#include "extract_subtitles.hpp"

#include <set>

#include <boost/foreach.hpp>
#include <boost/mpl/vector_c.hpp>
#include <boost/gil/image_view_factory.hpp>
#include <boost/gil/image_view.hpp>
#include <boost/gil/image.hpp>
#include <boost/gil/typedefs.hpp>
#include <boost/format.hpp>
#include <boost/spirit/home/phoenix/core/argument.hpp>
#include <boost/spirit/home/phoenix/operator.hpp>
#include <boost/fusion/include/tuple.hpp>
#include <boost/range/algorithm/sort.hpp>

#include <ffmsxx/video_source.hpp>
#include <ffmsxx/video_dimensions.hpp>
#include <ffmsxx/video_frame.hpp>
#include <ffmsxx/make_gil_view.hpp>

#include "fatal.hpp"
#include "frame_transform.hpp"
#include "find_best_transform.hpp"
#include "delta_luminosity.hpp"
#include "flood_fill.hpp"
#include "chunkify.hpp"
#include "conglomerate_image.hpp"
#include "nearby_frame_finder.hpp"
#include "rms_delta_luminosity.hpp"

namespace subliminal {

namespace gil = boost::gil;

namespace {

  struct get_extreme_values {
    get_extreme_values(int thresh) : thresh_(thresh) {}

    int operator()(gil::gray8s_pixel_t in) {
      return ( abs(in) >= thresh_ ? 255 : 0 );
    }

    int thresh_;
  };

  struct invert {
    void operator()(gil::gray8_pixel_t& p) const {
      p[0] = ~p[0];
    }
  };

  struct delete_if_isolated {
    void operator()(const gil::gray8_loc_t& loc) const {
      typedef gil::gray8_loc_t::point_t Point;
      int count_neighbours = 0;
      auto up = loc - Point(0, 1);
      count_neighbours += !!up[-1] + !!up[0] + !!up[1];
      count_neighbours += !!loc[-1] + !!loc[1];
      auto down = loc + Point(0, 1);
      count_neighbours += !!down[-1] + !!down[0] + !!down[1];
      if (count_neighbours <= 1) *loc = 0;
    }
  };

  struct copy_if {
    template<typename InP>
    InP operator()(InP const& in, gil::gray8_pixel_t const& mask) {
      if (mask) {
        return in;
      } else {
        return InP(0, 0, 0);
      }
    }
  };

  template<typename In, typename Out>
  void copy_under_mask(
    In const& in,
    gil::gray8c_view_t const& mask,
    Out const& out
  )
  {
    assert(in.dimensions() == out.dimensions());
    transform_pixels(in, mask, out, copy_if());
  }

  template<typename View>
  size_t non_black_height(View const& chunk) {
    gil::gray8_pixel_t black(0);
    size_t first_black_row = chunk.height();
    size_t last_black_row = 0;
    for (size_t y=0; y<size_t(chunk.height()); ++y) {
      bool black_row = true;
      for (auto it = chunk.row_begin(y); it != chunk.row_end(y); ++it) {
        if ((*it) != black) {
          black_row = false;
          break;
        }
      }
      if (!black_row) {
        last_black_row = y;
        if (y < first_black_row) first_black_row = y;
      }
    }
    if (last_black_row < first_black_row) return 0;
    return last_black_row - first_black_row + 1;
  }

}

void extract_subtitles(
  ffmsxx::video_source& raw,
  ffmsxx::video_source& subs,
  extract_subtitles_options const& options,
  output& out,
  visual_feedback& feedback
)
{
  if (options.frame_interval <= 0) {
    SUBLIMINAL_FATAL("silly frame interval");
  }

  // We want both sources producing video at the same resolution and pixel
  // format.  *However*, we won't use ffmsxx's resizer (even though it's
  // probably better than ours) because we want more control than it offers.
  //
  // For pixel format we go with RGB, 8 bits each, because that's easy to
  // understand and work with.
  //
  // We verify that the widths are multiples of 8, because otherwise
  // ffmpegsource tends to throw a wobbly; could work around it, but too lazy.

  auto const subs_dims = subs.encoded_dimensions();
  if (subs_dims.width() % 8 != 0) {
    SUBLIMINAL_FATAL("width of subtitled video must be a multiple of 8");
  }

  auto const raw_dims = raw.encoded_dimensions();
  if (raw_dims.width() % 8 != 0) {
    SUBLIMINAL_FATAL("width of raw video must be a multiple of 8");
  }

  ffmsxx::pixel_formats const formats(ffmsxx::pixel_format("rgb24"));

  raw.set_output_format(formats, raw_dims, ffmsxx::resizer::bicubic);
  subs.set_output_format(formats, subs_dims, ffmsxx::resizer::bicubic);

  auto const subs_time_base = subs.time_base();
  auto const raw_time_base = raw.time_base();

  // Multiply sync allowance by 1000 to convert from seconds to milliseconds
  // (the latter is what the FFMS timestamps are reported in)
  boost::rational<int64_t> sync_bottom = -options.sync_allowance*1000;
  boost::rational<int64_t> sync_top = options.sync_allowance*1000;

  feedback.messagef(
    boost::format("Initial sync bounds (%s, %s)") %
      sync_bottom % sync_top
  );

  boost::optional<frame_transform> best_transform;
  {
    // Step 1: Determine the appropriate affine transformation to make the raw
    // frames line up with the subbed ones; will be stored in best_transform
    nearby_frame_finder raw_finder{raw};

    // Jump to a frame ~5 seconds in (blah blah framerate blah blah)
    int const sub_frame_index =
      ( options.alignment_frame ?
        *options.alignment_frame : std::min(25*5, subs.num_frames()/2) );
    auto subs_frame = subs.frame(sub_frame_index);
    auto subs_time = subs_frame.time(subs_time_base);

    auto const& potential_raw_frames = raw_finder.get_frames(
      subs_time+sync_bottom, subs_time+sync_top
    );

    feedback.messagef(
      boost::format("searching for good transform through %d frames near %s")
        % potential_raw_frames.size() % subs_time
    );

    if (potential_raw_frames.empty()) {
      SUBLIMINAL_FATAL("videos seem to be of wildly different lengths");
    }

    double best_score = 1.0/0.0;
    auto best_it = potential_raw_frames.begin();
    for (auto it = best_it; it != potential_raw_frames.end(); ++it) {
      auto const& raw_frame = *it;
      auto const raw_time = raw_frame.time(raw_time_base);
      feedback.messagef(
        boost::format("testing raw frame at %s") % raw_time
      );
      boost::optional<frame_transform> transform;
      double score;
      std::tie(transform, score) = find_best_transform(
        raw_frame, subs_frame, options.start_params, feedback
      );
      if (score < best_score) {
        best_score = score;
        best_transform = transform;
        best_it = it;
      }
    }

    // We have the best transform, but at the same time we need to store the
    // bounds we get which will inform later sync guesses
    if (best_it != potential_raw_frames.begin()) {
      sync_bottom = boost::prior(best_it)->time(raw_time_base) - subs_time;
    }
    if (boost::next(best_it) != potential_raw_frames.end()) {
      sync_top = boost::next(best_it)->time(raw_time_base) - subs_time;
    }

    feedback.messagef(
      boost::format("best frame at %s scored %f.  Sync bounds (%s, %s)") %
        best_it->time(raw_time_base) % best_score % sync_bottom % sync_top
    );
  }

  assert(best_transform);
  assert(sync_top > sync_bottom);

  {
    // Step 2: Walk through the video, and do stuff
    nearby_frame_finder raw_finder{raw};
    /** \bug Should be unique_ptr, but gcc libstdc++ not good enough yet */
    typedef std::set<std::shared_ptr<conglomerate_image>> Conglomerates;
    Conglomerates active_conglomerates;

    boost::optional<int> requested_begin_frame, requested_end_frame;
    std::tie(requested_begin_frame, requested_end_frame) = options.bounds;

    int const begin_frame = requested_begin_frame ? *requested_begin_frame : 0;
    int const end_frame = requested_end_frame ?
      std::min(*requested_end_frame, subs.num_frames()) : subs.num_frames();

    if (begin_frame < 0 || end_frame < 0 || end_frame < begin_frame) {
      SUBLIMINAL_FATAL("nonsensical frame bounds");
    }

    feedback.messagef(
      boost::format("analyzing frames from %d to %d") %
      begin_frame % end_frame
    );

    auto const meaningful_area = best_transform->meaningful_area(
      raw_dims.as_point(), subs_dims.as_point()
    );
    auto const subview_dims = meaningful_area.dimensions();

    for (int sub_frame_index = begin_frame; sub_frame_index < end_frame;
        sub_frame_index += options.frame_interval) {
      auto subs_frame = subs.frame(sub_frame_index);
      auto subs_time = subs_frame.time(subs_time_base);

      auto const& potential_raw_frames = raw_finder.get_frames(
        subs_time+sync_bottom, subs_time+sync_top
      );

      if (potential_raw_frames.empty()) {
        // No frame against which to compare (presumably the framerate of the
        // raws is lower or something) so just skip to the next one
        continue;
      }

      // Take various views on the subs
      auto const subs_view = make_gil_view(subs_frame);
      auto const subs_subview = subimage_view(subs_view, meaningful_area);

      feedback.show(subs_frame, 1);

      // Search through the potential raws for the best match
      auto raw_frame_it = potential_raw_frames.begin();
      double best_score = 1.0/0.0;

      if (potential_raw_frames.size() > 1) {
        for (auto it = raw_frame_it; it != potential_raw_frames.end(); ++it) {
          feedback.show(*it, 0);
          auto const raw_view = make_gil_view(*it);

          // Apply chosen best transform to raw
          boost::gil::gray8_image_t transformed(subs_dims.as_point());
          (*best_transform)(raw_view, view(transformed));

          // Take view of the meaningful area
          auto const raw_subview =
            subimage_view(const_view(transformed), meaningful_area);

          // Score this choice
          double const score =
            rms_delta_luminosity(raw_subview, subs_subview, feedback);

          if (score < best_score) {
            best_score = score;
            raw_frame_it = it;
          }
        }
      }

      feedback.show(*raw_frame_it, 0);

      // Apply chosen best transform to raw (again!)
      auto const raw_view = make_gil_view(*raw_frame_it);
      boost::gil::gray8_image_t transformed(subs_dims.as_point());
      (*best_transform)(raw_view, view(transformed));

      // Take view of the meaningful area
      auto const raw_subview =
        subimage_view(const_view(transformed), meaningful_area);

      // Copmute the delta of the two images
      boost::gil::gray8s_image_t delta(subview_dims);
      delta_luminosity(raw_subview, subs_subview, view(delta));

      // Show the delta in slot 3
      feedback.show(const_view(delta), 2);

      // Pull the extreme values from the delta
      boost::gil::gray8_image_t extreme_values(subview_dims);
      transform_pixels(
        const_view(delta), view(extreme_values), get_extreme_values(32)
      );

      // Flood fill the outside
      boost::gil::gray8_image_t outside_fill(subview_dims);
      fill_pixels(view(outside_fill), 0);
      flood_fill(const_view(extreme_values), view(outside_fill));

      // Invert
      for_each_pixel(view(outside_fill), invert());

      // Delete isolated pixels
      for_each_pixel_position(
        subimage_view(
          view(outside_fill), 1, 1, subview_dims.x-2, subview_dims.y-2
        ),
        delete_if_isolated()
      );

      feedback.show(const_view(outside_fill), 3);

      // Chunkify
      auto chunks =
        chunkify(const_view(outside_fill), options.chunking_threshold);
      if (!options.quiet) {
        feedback.messagef(boost::format("Got %1% chunks") % chunks.size());
      }
      /** \todo Expose this parameter on the command line */
      if (chunks.size() > 10) {
        feedback.message("implausibly many chunks; writing frame off");
        continue;
      }

      bool bad = false;
      BOOST_FOREACH(auto const& chunk, chunks) {
        if (non_black_height(const_view(chunk)) > size_t(subview_dims.y)/5) {
          feedback.message("implausibly high chunk; writing frame off");
          bad = true;
          break;
        }
      }
      if (bad) continue;

      Conglomerates new_conglomerates;

      BOOST_FOREACH(auto const& chunk, chunks) {
        feedback.show(const_view(chunk), 4);

        // Extract this chunk from the subbed copy
        boost::gil::rgb8_image_t sub_chunk(subview_dims);
        copy_under_mask(subs_subview, const_view(chunk), view(sub_chunk));

        feedback.show(const_view(sub_chunk), 5);

        Conglomerates::value_type new_conglomerate(
          new conglomerate_image(
            sub_frame_index, subs_time, const_view(sub_chunk)
          )
        );
        std::vector<Conglomerates::iterator> to_erase;
        for (auto it = active_conglomerates.begin();
          it != active_conglomerates.end(); ++it) {
          if ((*it)->consistent_overlap(const_view(sub_chunk))) {
            new_conglomerate->merge(std::move(**it));
            to_erase.push_back(it);
          }
        }
        BOOST_FOREACH(auto const it, to_erase) {
          active_conglomerates.erase(it);
        }

        new_conglomerates.insert(std::move(new_conglomerate));
      }

      // Conglomerates that have lost their activity indicate finished
      // subtitles.  We finalize them, but first we sort them so that the
      // output is more deterministic.
      std::vector<std::pair<size_t, Conglomerates::value_type>>
        finished_conglomerates;
      BOOST_FOREACH(auto const& conglomerate, active_conglomerates) {
        auto const index = conglomerate->index_of_first_nonempty();
        finished_conglomerates.push_back({index, std::move(conglomerate)});
      }
      boost::range::sort(finished_conglomerates);
      BOOST_FOREACH(auto const& conglomerate, finished_conglomerates) {
        conglomerate.second->finalize(sub_frame_index, subs_time, out);
      }

      active_conglomerates = std::move(new_conglomerates);

      feedback.progress(sub_frame_index, end_frame);
    }
  }
}

}

