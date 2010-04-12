#include <ffmsxx/video_frame.hpp>

#include <ffms.h>

#include <ffmsxx/video_dimensions.hpp>
#include <ffmsxx/pixel_format.hpp>
#include <ffmsxx/error.hpp>
#include <ffmsxx/detail/error_info.hpp>

namespace ffmsxx {

struct video_frame::impl {
  FFMS_Frame const* frame;
  FFMS_FrameInfo info;
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

int64_t video_frame::pts() const
{
  return impl_->info.PTS;
}

video_frame::video_frame(FFMS_VideoSource* video_source, int n) :
  impl_(new impl)
{
  detail::error_info e;
  impl_->frame = FFMS_GetFrame(video_source, n, e.raw());
  e.throw_if_null(impl_->frame);
  // It seems that both the following return pointers but retain ownership
  // thereof
  auto const track = FFMS_GetTrackFromVideo(video_source);
  auto const info = FFMS_GetFrameInfo(track, n);
  if (info == NULL) {
    throw error("unknown error geting frame info");
  }
  impl_->info = *info;
}

}

