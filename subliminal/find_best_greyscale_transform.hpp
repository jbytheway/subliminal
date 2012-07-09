#ifndef SUBLIMINAL__FIND_BEST_GREYSCALE_TRANSFORM_HPP
#define SUBLIMINAL__FIND_BEST_GREYSCALE_TRANSFORM_HPP

#include <ffmsxx/fwd.hpp>

#include "frame_greyscale_transform.hpp"
#include "visual_feedback.hpp"

namespace subliminal {

std::tuple<greyscale_transform_params, frame_greyscale_transform, double>
find_best_greyscale_transform(
  ffmsxx::video_frame const& from,
  ffmsxx::video_frame const& to,
  greyscale_transform_params const& start_params,
  visual_feedback& feedback
);

}

#endif // SUBLIMINAL__FIND_BEST_GREYSCALE_TRANSFORM_HPP

