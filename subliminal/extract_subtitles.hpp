#ifndef SUBLIMINAL__EXTRACT_SUBTITLES_HPP
#define SUBLIMINAL__EXTRACT_SUBTITLES_HPP

#include <ffmsxx/fwd.hpp>

#include "visual_feedback.hpp"
#include "transform_params.hpp"

namespace subliminal {

struct extract_subtitles_options {
  boost::optional<int> alignment_frame;
  transform_params start_params;
};

void extract_subtitles(
  ffmsxx::video_source& raw,
  ffmsxx::video_source& subs,
  extract_subtitles_options const&,
  visual_feedback&
);

}

#endif // SUBLIMINAL__EXTRACT_SUBTITLES_HPP

