#include <ffmsxx/video_frame.hpp>

#include <ffms.h>

#include <ffmsxx/video_dimensions.hpp>
#include <ffmsxx/pixel_format.hpp>
#include <ffmsxx/detail/error_info.hpp>

namespace ffmsxx {

struct video_frame::impl {
  FFMS_Frame const* frame;
  FFMS_FrameInfo const* info;
};

video_frame::~video_frame() = default;

video_dimensions video_frame::encoded_dimensions() const
{
  return video_dimensions(
    impl_->frame->EncodedWidth, impl_->frame->EncodedHeight
  );
}

video_dimensions video_frame::scaled_dimensions() const
{
  return video_dimensions(
    impl_->frame->ScaledWidth, impl_->frame->ScaledHeight
  );
}

pixel_format video_frame::converted_pixel_format() const
{
  return pixel_format(impl_->frame->ConvertedPixelFormat);
}

uint8_t const* video_frame::data(int plane) const
{
  assert(plane >= 0);
  assert(plane < 4);
  return impl_->frame->Data[plane];
}

int video_frame::data_stride(int plane) const
{
  assert(plane >= 0);
  assert(plane < 4);
  return impl_->frame->Linesize[plane];
}

video_frame::video_frame(FFMS_VideoSource* video_source, int n) :
  impl_(new impl)
{
  detail::error_info e;
  impl_->frame = FFMS_GetFrame(video_source, n, e.raw());
  e.throw_if_null(impl_->frame);
  auto const track = FFMS_GetTrackFromVideo(video_source);
  impl_->info = FFMS_GetFrameInfo(track, n);
  if (impl_->info == NULL) {
    throw error("unknown error getingframe info");
  }
}

}

