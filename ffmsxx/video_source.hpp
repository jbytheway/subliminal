#ifndef FFMSXX__VIDEO_SOURCE_HPP
#define FFMSXX__VIDEO_SOURCE_HPP

#include <boost/noncopyable.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/rational.hpp>

#include <ffmsxx/fwd.hpp>
#include <ffmsxx/seek.hpp>
#include <ffmsxx/pixel_formats.hpp>
#include <ffmsxx/resizer.hpp>

namespace ffmsxx {

class video_source : boost::noncopyable {
  public:
    video_source(
      boost::filesystem::path const&,
      index const&,
      int track,
      seek const
    );
    video_source(video_source&&);
    ~video_source();

    int num_frames() const;
    video_frame frame(int) const;
    video_dimensions encoded_dimensions() const;
    boost::rational<int64_t> time_base() const;
    void set_output_format(
      pixel_formats const,
      video_dimensions const&,
      resizer const
    );
  private:
    struct impl;
    std::unique_ptr<impl> impl_;
};

}

#endif // FFMSXX__VIDEO_SOURCE_HPP

