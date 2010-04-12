#include "extract_subtitles.hpp"

#include <ffmsxx/video_source.hpp>
#include <ffmsxx/video_dimensions.hpp>
#include <ffmsxx/video_frame.hpp>

namespace subliminal {

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

  auto raw_time_base = raw.time_base();
  auto subs_time_base = subs.time_base();

  int raw_frame_index = 0;
  auto last_raw_frame = raw.frame(raw_frame_index);
  auto this_raw_frame = raw.frame(++raw_frame_index);

  for (int sub_frame_index = 0; sub_frame_index < subs.num_frames();
      ++sub_frame_index) {
    auto subs_frame = subs.frame(sub_frame_index);
    bool done = false;
    while (this_raw_frame.time(raw_time_base) <
      subs_frame.time(subs_time_base)) {
      last_raw_frame = std::move(this_raw_frame);
      ++raw_frame_index;
      if (raw_frame_index >= raw.num_frames()) {
        done = true;
        break;
      }
      this_raw_frame = raw.frame(raw_frame_index);
    }
    if (done) break;

    feedback.show(this_raw_frame, 0);
    feedback.show(subs_frame, 1);
    feedback.progress(sub_frame_index, subs.num_frames());
  }

  sleep(1);
}

}

