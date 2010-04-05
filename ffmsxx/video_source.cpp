#include <ffmsxx/video_source.hpp>

#include <ffmsxx/index.hpp>
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

}

