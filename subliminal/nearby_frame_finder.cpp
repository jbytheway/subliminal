#include "nearby_frame_finder.hpp"

#include <ffmsxx/video_source.hpp>
#include <ffmsxx/video_frame.hpp>

namespace subliminal {

nearby_frame_finder::nearby_frame_finder(ffmsxx::video_source const& source) :
  source_(source),
  time_base_(source_.time_base()),
  frame_index_{0}
{
  fetch_next_frame();
}

std::list<ffmsxx::video_frame> const& nearby_frame_finder::get_frames(
      boost::rational<int64_t> const& start,
      boost::rational<int64_t> const& finish
    )
{
  // 1. Discard too-old frames we have
  while (!recent_frames_.empty() &&
    recent_frames_.front().time(time_base_) < start) {
    recent_frames_.pop_front();
  }

  // 2. Skip over too-old frames still to come
  if (recent_frames_.empty()) {
    while (next_frame_ && next_frame_->time(time_base_) < start) {
      fetch_next_frame();
    }
  }

  // 3. Store frames in correct time interval
  while (next_frame_ && next_frame_->time(time_base_) < finish) {
    recent_frames_.push_back(std::move(*next_frame_));
    fetch_next_frame();
  }

  return recent_frames_;
}

void nearby_frame_finder::fetch_next_frame() {
  if (frame_index_ >= source_.num_frames()) {
    next_frame_ = {};
  } else {
    next_frame_.reset(
      new ffmsxx::video_frame(source_.frame(frame_index_))
    );
    ++frame_index_;
  }
}

}
