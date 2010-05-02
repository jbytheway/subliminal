#ifndef SUBLIMINAL__FIND_BEST_TRANSFORM_HPP
#define SUBLIMINAL__FIND_BEST_TRANSFORM_HPP

#include <ffmsxx/fwd.hpp>

#include "frame_transform.hpp"
#include "visual_feedback.hpp"

namespace subliminal {

frame_transform find_best_transform(
  ffmsxx::video_frame const& from,
  ffmsxx::video_frame const& to,
  transform_params const& start_params,
  visual_feedback& feedback
);

}

#endif // SUBLIMINAL__FIND_BEST_TRANSFORM_HPP

