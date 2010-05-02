#include "extract_subtitles.hpp"

#include <boost/mpl/vector_c.hpp>
#include <boost/gil/image_view_factory.hpp>
#include <boost/gil/image_view.hpp>
#include <boost/gil/image.hpp>
#include <boost/gil/typedefs.hpp>

#include <ffmsxx/video_source.hpp>
#include <ffmsxx/video_dimensions.hpp>
#include <ffmsxx/video_frame.hpp>
#include <ffmsxx/make_gil_view.hpp>

#include "fatal.hpp"

namespace subliminal {

namespace gil = boost::gil;

namespace {

  struct half_difference {
    int operator()(gil::gray8_pixel_t in1, gil::gray8_pixel_t in2) const {
      return (in1 - in2) / 2;
    }
  };

  struct half_difference_luminosity {
    int operator()(gil::rgb8_pixel_t in1, gil::rgb8_pixel_t in2) const {
      boost::gil::color_convert_deref_fn<
        const boost::gil::rgb8c_pixel_t&,
        gil::gray8_pixel_t
      > cc;
      return (cc(in1) - cc(in2)) / 2;
    }
  };

  void delta(
    gil::gray8c_view_t const& in1,
    gil::gray8c_view_t const& in2,
    gil::gray8s_view_t const& out
  )
  {
    gil::transform_pixels(in1, in2, out, half_difference());
  }

  void delta_luminosity(
    gil::rgb8c_view_t const& in1,
    gil::rgb8c_view_t const& in2,
    gil::gray8s_view_t const& out
  )
  {
    gil::transform_pixels(in1, in2, out, half_difference_luminosity());
  }

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

}

void extract_subtitles(
  ffmsxx::video_source& raw,
  ffmsxx::video_source& subs,
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

  {
    // Step 1: Determine the appropriate affine transformation to make the raw
    // frames line up with the subbed ones
    feedback.message("searching for good transform");

    closest_frame_finder raw_finder{raw};

    // Jump to a frame ~5 seconds in (blah blah framerate blah blah)
    int const sub_frame_index = std::min(25*5, subs.num_frames()/2);
    auto subs_frame = subs.frame(sub_frame_index);
    ffmsxx::video_frame const* raw_frame = NULL;

    if (!raw_finder.get_closest_frame(
        subs_frame.time(subs_time_base), raw_frame
      )) {
      SUBLIMINAL_FATAL("videos seem to be of wildly different lengths");
    }

    feedback.show(*raw_frame, 0);
    feedback.show(subs_frame, 1);

    sleep(1);
  }

  {
    // Step 2: Walk through the video, and do stuff
    closest_frame_finder raw_finder{raw};

    for (int sub_frame_index = 0; sub_frame_index < subs.num_frames();
        ++sub_frame_index) {
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

      // Copmute the delta of the two images
      boost::gil::gray8s_image_t delta(subs_view.dimensions());
      delta_luminosity(raw_view, subs_view, view(delta));

      // Show the delta in slot 3
      feedback.show(view(delta), 2);

      feedback.progress(sub_frame_index, subs.num_frames());
    }
  }
}

}

