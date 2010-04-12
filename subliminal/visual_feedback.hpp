#ifndef SUBLIMINAL__VISUAL_FEEDBACK_HPP
#define SUBLIMINAL__VISUAL_FEEDBACK_HPP

#include <boost/noncopyable.hpp>

#include <ffmsxx/fwd.hpp>

namespace subliminal {

class visual_feedback : boost::noncopyable {
  public:
    virtual ~visual_feedback() = 0;
    virtual void show(ffmsxx::video_frame const&, int image) = 0;
  protected:
    visual_feedback() = default;
};

}

#endif // SUBLIMINAL__VISUAL_FEEDBACK_HPP

