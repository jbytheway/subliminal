#ifndef SUBLIMINAL_FIND_BEST_COLOUR_TRANSFORM_HPP
#define SUBLIMINAL_FIND_BEST_COLOUR_TRANSFORM_HPP

#include <ffmsxx/fwd.hpp>

#include "frame_colour_transform.hpp"
#include "visual_feedback.hpp"

namespace subliminal {

frame_colour_transform find_best_colour_transform(
  ffmsxx::video_frame const& from,
  ffmsxx::video_frame const& to,
  spatial_transform_params const& based_on,
  visual_feedback&
);

}

#endif // SUBLIMINAL_FIND_BEST_COLOUR_TRANSFORM_HPP

