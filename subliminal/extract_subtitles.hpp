#ifndef SUBLIMINAL__EXTRACT_SUBTITLES_HPP
#define SUBLIMINAL__EXTRACT_SUBTITLES_HPP

#include <ffmsxx/fwd.hpp>

#include "visual_feedback.hpp"

namespace subliminal {

void extract_subtitles(
  ffmsxx::video_source& raw,
  ffmsxx::video_source& subs,
  visual_feedback&
);

}

#endif // SUBLIMINAL__EXTRACT_SUBTITLES_HPP

