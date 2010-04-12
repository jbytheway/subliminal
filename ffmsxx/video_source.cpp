#include <ffmsxx/video_source.hpp>

#include <ffmsxx/index.hpp>
#include <ffmsxx/video_dimensions.hpp>
#include <ffmsxx/video_frame.hpp>
#include <ffmsxx/detail/error_info.hpp>

namespace ffmsxx {

struct video_source::impl {
  ~impl() { FFMS_DestroyVideoSource(raw); }

  FFMS_VideoSource* raw;
};

video_source::video_source(
    boost::filesystem::path const& file,
    index const& i,
    int track,
    seek const seek
  )
{
  detail::error_info e;
  /** \todo Support multithreaded */
  FFMS_VideoSource* raw_source = FFMS_CreateVideoSource(
    file.file_string().c_str(), track, i.raw(), 1/*threads*/,
    seek_to_raw(seek), e.raw()
  );
  e.throw_if_null(raw_source);
  impl_.reset(new impl{raw_source});
}

video_source::~video_source() = default;

int video_source::num_frames() const
{
  return FFMS_GetVideoProperties(impl_->raw)->NumFrames;
}

video_frame video_source::frame(int n) const
{
  assert(n >= 0);
  assert(n < num_frames());
  return video_frame(impl_->raw, n);
}

video_dimensions video_source::encoded_dimensions() const
{
  assert(num_frames() > 0);
  auto f = frame(0);
  return f.encoded_dimensions();
}

boost::rational<int64_t> video_source::time_base() const
{
  auto const track = FFMS_GetTrackFromVideo(impl_->raw);
  auto const timeBase = FFMS_GetTimeBase(track);
  return boost::rational<int64_t>(timeBase->Num, timeBase->Den);
}

void video_source::set_output_format(
  pixel_formats const formats,
  video_dimensions const& dims,
  resizer const resize
)
{
  detail::error_info e;
  int ret = FFMS_SetOutputFormatV(
    impl_->raw, formats.raw(), dims.width(), dims.height(),
    resizer_to_raw(resize), e.raw()
  );
  e.throw_if(ret);
}

}

