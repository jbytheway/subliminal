#ifndef SUBLIMINAL__NEARBY_FRAME_FINDER_HPP
#define SUBLIMINAL__NEARBY_FRAME_FINDER_HPP

#include <list>
#include <memory>

#include <boost/rational.hpp>

#include <ffmsxx/fwd.hpp>

namespace subliminal {

class nearby_frame_finder {
  public:
    nearby_frame_finder(ffmsxx::video_source const& source);

    std::list<ffmsxx::video_frame> const& get_frames(
      boost::rational<int64_t> const& start,
      boost::rational<int64_t> const& finish
    );
  private:
    void fetch_next_frame();

    ffmsxx::video_source const& source_;
    boost::rational<int64_t> time_base_;
    int frame_index_;
    std::list<ffmsxx::video_frame> recent_frames_;
    // optional would be better, but not move-enabled
    std::unique_ptr<ffmsxx::video_frame> next_frame_;
};

}

#endif // SUBLIMINAL__NEARBY_FRAME_FINDER_HPP

