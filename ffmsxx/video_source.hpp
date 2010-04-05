#ifndef FFMSXX__VIDEO_SOURCE_HPP
#define FFMSXX__VIDEO_SOURCE_HPP

#include <boost/noncopyable.hpp>
#include <boost/filesystem/path.hpp>

#include <ffmsxx/fwd.hpp>
#include <ffmsxx/seek.hpp>

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
  private:
    struct impl;
    std::unique_ptr<impl> impl_;
};

}

#endif // FFMSXX__VIDEO_SOURCE_HPP

