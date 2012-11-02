#ifndef SUBLIMINAL_FIND_BEST_OFFSET_AND_TRANSFORM_HPP
#define SUBLIMINAL_FIND_BEST_OFFSET_AND_TRANSFORM_HPP

#include <boost/rational.hpp>

#include <ffmsxx/fwd.hpp>

#include "frame_colour_transform.hpp"
#include "visual_feedback.hpp"
#include "greyscale_transform_params.hpp"

namespace subliminal {

frame_colour_transform find_best_offset_and_transform(
  ffmsxx::video_source const& raw,
  ffmsxx::video_source const& subs,
  boost::rational<int64_t>& sync_bottom,
  boost::rational<int64_t>& sync_top,
  int const alignment_frame,
  greyscale_transform_params const& start_params,
  visual_feedback& feedback
);

}

#endif // SUBLIMINAL_FIND_BEST_OFFSET_AND_TRANSFORM_HPP

