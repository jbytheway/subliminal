#ifndef SUBLIMINAL__EXTRACT_SUBTITLES_HPP
#define SUBLIMINAL__EXTRACT_SUBTITLES_HPP

#include <ffmsxx/fwd.hpp>

namespace subliminal {

void extract_subtitles(ffmsxx::video_source& raw, ffmsxx::video_source& subs);

}

#endif // SUBLIMINAL__EXTRACT_SUBTITLES_HPP

