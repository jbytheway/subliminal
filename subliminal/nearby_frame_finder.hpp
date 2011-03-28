#ifndef SUBLIMINAL__NEARBY_FRAME_FINDER_HPP
#define SUBLIMINAL__NEARBY_FRAME_FINDER_HPP

namespace subliminal {

class nearby_frame_finder {
  public:
    nearby_frame_finder(ffmsxx::video_source const& source) :
      source_(source),
      time_base_(source_.time_base()),
      frame_index_{0}
    {
      fetch_next_frame();
    }

    std::list<ffmsxx::video_frame> const& get_frames(
      boost::rational<int64_t> const& start,
      boost::rational<int64_t> const& finish
    )
    {
      while (next_frame_ &&
        next_frame_->time(time_base_) < finish) {
        recent_frames_.push_back(std::move(*next_frame_));
        fetch_next_frame();
      }

      while (!recent_frames_.empty() &&
        recent_frames_.front().time(time_base_) < start) {
        recent_frames_.pop_front();
      }

      return recent_frames_;
    }
  private:
    void fetch_next_frame() {
      if (frame_index_ >= source_.num_frames()) {
        next_frame_ = {};
      } else {
        next_frame_.reset(
          new ffmsxx::video_frame(source_.frame(frame_index_))
        );
        ++frame_index_;
      }
    }

    ffmsxx::video_source const& source_;
    boost::rational<int64_t> time_base_;
    int frame_index_;
    std::list<ffmsxx::video_frame> recent_frames_;
    // optional would be better, but not move-enabled
    std::unique_ptr<ffmsxx::video_frame> next_frame_;
};

}

#endif // SUBLIMINAL__NEARBY_FRAME_FINDER_HPP

