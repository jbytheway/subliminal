#include "extract_subtitles.hpp"

#include <boost/foreach.hpp>
#include <boost/mpl/vector_c.hpp>
#include <boost/gil/image_view_factory.hpp>
#include <boost/gil/image_view.hpp>
#include <boost/gil/image.hpp>
#include <boost/gil/typedefs.hpp>
#include <boost/format.hpp>
#include <boost/spirit/home/phoenix/core/argument.hpp>
#include <boost/spirit/home/phoenix/operator.hpp>

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

namespace subliminal {

namespace gil = boost::gil;

namespace {

  class closest_frame_finder {
    public:
      closest_frame_finder(ffmsxx::video_source const& source) :
        source_(source),
        time_base_(source_.time_base()),
        frame_index_{0},
        last_frame_{source.frame(frame_index_)},
        this_frame_{source.frame(++frame_index_)}
      {
      }

      bool get_closest_frame(
        boost::rational<int64_t> const& time,
        ffmsxx::video_frame const*& best_frame
      )
      {
        while (this_frame_.time(time_base_) < time) {
          last_frame_ = std::move(this_frame_);
          ++frame_index_;
          if (frame_index_ >= source_.num_frames()) {
            return false;
          }
          this_frame_ = source_.frame(frame_index_);
        }

        auto diff_to_last = abs(time-last_frame_.time(time_base_));
        auto diff_to_this = abs(time-this_frame_.time(time_base_));
        best_frame =
          &( diff_to_last < diff_to_this ? last_frame_ : this_frame_ );
        return true;
      }
    private:
      ffmsxx::video_source const& source_;
      boost::rational<int64_t> time_base_;
      int frame_index_;
      ffmsxx::video_frame last_frame_;
      ffmsxx::video_frame this_frame_;
  };

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
    transform_pixels(in, mask, out, copy_if());
  }

}

void extract_subtitles(
  ffmsxx::video_source& raw,
  ffmsxx::video_source& subs,
  extract_subtitles_options const& options,
  visual_feedback& feedback
)
{
  // We want both sources producing video at the same resolution and pixel
  // format.
  // For resolution we mimic the subs since they're what we're extracting
  // images from later, and don't want to lose quality there.
  // For pixel format we go with RGB, 8 bits each, because that's easy to
  // understand and work with.

  auto const subs_dims = subs.encoded_dimensions();
  ffmsxx::pixel_formats const formats(ffmsxx::pixel_format("rgb24"));

  raw.set_output_format(formats, subs_dims, ffmsxx::resizer::bicubic);
  subs.set_output_format(formats, subs_dims, ffmsxx::resizer::bicubic);

  auto subs_time_base = subs.time_base();

  std::unique_ptr<frame_transform> best_transform;
  {
    // Step 1: Determine the appropriate affine transformation to make the raw
    // frames line up with the subbed ones; will be stored in best_transform
    feedback.message("searching for good transform");

    closest_frame_finder raw_finder{raw};

    // Jump to a frame ~5 seconds in (blah blah framerate blah blah)
    int const sub_frame_index =
      ( options.alignment_frame ?
        *options.alignment_frame :std::min(25*5, subs.num_frames()/2) );
    auto subs_frame = subs.frame(sub_frame_index);
    ffmsxx::video_frame const* raw_frame = NULL;

    if (!raw_finder.get_closest_frame(
        subs_frame.time(subs_time_base), raw_frame
      )) {
      SUBLIMINAL_FATAL("videos seem to be of wildly different lengths");
    }

    best_transform.reset(new frame_transform(find_best_transform(
          *raw_frame, subs_frame, options.start_params, feedback
        )));
  }

  assert(best_transform);

  {
    // Step 2: Walk through the video, and do stuff
    closest_frame_finder raw_finder{raw};

    for (int sub_frame_index = 0; sub_frame_index < subs.num_frames();
        sub_frame_index += 10) {
      auto subs_frame = subs.frame(sub_frame_index);
      ffmsxx::video_frame const* raw_frame = NULL;

      if (!raw_finder.get_closest_frame(
          subs_frame.time(subs_time_base), raw_frame
        )) {
        return;
      }

      assert(raw_frame);

      feedback.show(*raw_frame, 0);
      feedback.show(subs_frame, 1);

      // Get Boost.GIL views on the images
      auto raw_view = make_gil_view(*raw_frame);
      auto subs_view = make_gil_view(subs_frame);

      auto dims = raw_view.dimensions();
      assert(dims == subs_view.dimensions());

      // Apply chosen best transform to raw
      boost::gil::gray8_image_t transformed(dims);
      (*best_transform)(raw_view, view(transformed));

      // Copmute the delta of the two images
      boost::gil::gray8s_image_t delta(dims);
      delta_luminosity(const_view(transformed), subs_view, view(delta));

      // Show the delta in slot 3
      feedback.show(const_view(delta), 2);

      // Pull the extreme values from the delta
      boost::gil::gray8_image_t extreme_values(dims);
      transform_pixels(
        const_view(delta), view(extreme_values), get_extreme_values(32)
      );

      // Flood fill the outside
      boost::gil::gray8_image_t outside_fill(dims);
      fill_pixels(view(outside_fill), 0);
      flood_fill(const_view(extreme_values), view(outside_fill));

      // Invert
      for_each_pixel(view(outside_fill), invert());

      // Delete isolated pixels
      for_each_pixel_position(
        subimage_view(view(outside_fill), 1, 1, dims.x-2, dims.y-2),
        delete_if_isolated()
      );

      feedback.show(const_view(outside_fill), 3);

      // Chunkify
      auto chunks =
        chunkify(const_view(outside_fill), options.chunking_threshold);
      feedback.messagef(boost::format("Got %1% chunks") % chunks.size());

      BOOST_FOREACH(auto const& chunk, chunks) {
        feedback.show(const_view(chunk), 4);

        // Extract this chunk from the subbed copy
        boost::gil::rgb8_image_t sub_chunk(dims);
        copy_under_mask(subs_view, const_view(chunk), view(sub_chunk));

        feedback.show(const_view(sub_chunk), 5);
      }

      feedback.progress(sub_frame_index, subs.num_frames());
    }
  }
}

}

