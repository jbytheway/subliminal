#include <ffmsxx/video_frame.hpp>

#include <ffms.h>

#include <ffmsxx/video_dimensions.hpp>
#include <ffmsxx/pixel_format.hpp>

namespace ffmsxx {

struct video_frame::impl {
  FFMS_Frame const* raw;
};

video_frame::~video_frame() = default;

video_dimensions video_frame::encoded_dimensions() const
{
  return video_dimensions(impl_->raw->EncodedWidth, impl_->raw->EncodedHeight);
}

video_dimensions video_frame::scaled_dimensions() const
{
  return video_dimensions(impl_->raw->ScaledWidth, impl_->raw->ScaledHeight);
}

pixel_format video_frame::converted_pixel_format() const
{
  return pixel_format(impl_->raw->ConvertedPixelFormat);
}

uint8_t const* video_frame::data(int plane) const
{
  assert(plane >= 0);
  assert(plane < 4);
  return impl_->raw->Data[plane];
}

int video_frame::data_stride(int plane) const
{
  assert(plane >= 0);
  assert(plane < 4);
  return impl_->raw->Linesize[plane];
}

video_frame::video_frame(const void* frame) :
  impl_(new impl{static_cast<FFMS_Frame const*>(frame)})
{}

}

