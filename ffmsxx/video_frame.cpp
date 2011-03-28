#include <ffmsxx/video_frame.hpp>

#include <cstring>

#include <boost/scoped_array.hpp>

#include <ffms.h>

#include <ffmsxx/video_dimensions.hpp>
#include <ffmsxx/pixel_format.hpp>
#include <ffmsxx/error.hpp>
#include <ffmsxx/detail/error_info.hpp>

namespace ffmsxx {

struct video_frame::impl {
  FFMS_Frame frame;
  boost::scoped_array<uint8_t> data[4];
  FFMS_FrameInfo info;
};

video_frame::video_frame(video_frame&&) = default;

video_frame& video_frame::operator=(video_frame&& r)
{
  impl_ = std::move(r.impl_);
  return *this;
}

video_frame::~video_frame() = default;

video_dimensions video_frame::encoded_dimensions() const
{
  return video_dimensions(
    impl_->frame.EncodedWidth, impl_->frame.EncodedHeight
  );
}

video_dimensions video_frame::scaled_dimensions() const
{
  return video_dimensions(
    impl_->frame.ScaledWidth, impl_->frame.ScaledHeight
  );
}

pixel_format video_frame::converted_pixel_format() const
{
  return pixel_format(impl_->frame.ConvertedPixelFormat);
}

uint8_t const* video_frame::data(int plane) const
{
  assert(plane >= 0);
  assert(plane < 4);
  return impl_->frame.Data[plane];
}

int video_frame::data_stride(int plane) const
{
  assert(plane >= 0);
  assert(plane < 4);
  return impl_->frame.Linesize[plane];
}

int64_t video_frame::pts() const
{
  return impl_->info.PTS;
}

boost::rational<int64_t>
video_frame::time(boost::rational<int64_t> const& time_base) const
{
  return time_base*pts();
}

video_frame::video_frame(FFMS_VideoSource* video_source, int n) :
  impl_(new impl)
{
  detail::error_info e;
  FFMS_Frame const* frame_shared = FFMS_GetFrame(video_source, n, e.raw());
  e.throw_if_null(frame_shared);
  // frame_shared points to reused memory, so we need to copy the contents
  // elsewhere to get something safe
  impl_->frame = *frame_shared;
  // And the same applies to the pixel data pointed to
  for (int i=0; i<4; ++i) {
    int64_t height = impl_->frame.ScaledHeight;
    if (height < 0) {
      height = impl_->frame.EncodedHeight;
    }
    assert(height >= 0);
    size_t const total_size =
      height * impl_->frame.Linesize[i];
    impl_->data[i].reset(new uint8_t[total_size]);
    uint8_t* p = impl_->data[i].get();
    std::memcpy(p, impl_->frame.Data[i], total_size);
    impl_->frame.Data[i] = p;
  }
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

