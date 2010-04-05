#include <ffmsxx/video_frame.hpp>

#include <ffms.h>

#include <ffmsxx/video_dimensions.hpp>

namespace ffmsxx {

struct video_frame::impl {
  FFMS_Frame const* raw;
};

video_frame::~video_frame() = default;

video_dimensions video_frame::encoded_dimensions() const
{
  return video_dimensions(impl_->raw->EncodedWidth, impl_->raw->EncodedHeight);
}

video_frame::video_frame(const void* frame) :
  impl_(new impl{static_cast<FFMS_Frame const*>(frame)})
{}

}

