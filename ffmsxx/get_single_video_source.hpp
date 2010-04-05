#ifndef FFMSXX__GET_SINGLE_VIDEO_SOURCE_HPP
#define FFMSXX__GET_SINGLE_VIDEO_SOURCE_HPP

#include <boost/filesystem/path.hpp>

#include <ffmsxx/fwd.hpp>
#include <ffmsxx/video_source.hpp>

namespace ffmsxx {

video_source get_single_video_source(
  service const&,
  boost::filesystem::path const&
);

}

#endif // FFMSXX__GET_SINGLE_VIDEO_SOURCE_HPP

