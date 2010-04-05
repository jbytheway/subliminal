#include "extract_subtitles.hpp"

#include <ffmsxx/video_source.hpp>
#include <ffmsxx/video_dimensions.hpp>

namespace subliminal {

void extract_subtitles(ffmsxx::video_source& raw, ffmsxx::video_source& subs)
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
}

}

