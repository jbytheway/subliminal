#ifndef SUBLIMINAL__TEXT_FEEDBACK_HPP
#define SUBLIMINAL__TEXT_FEEDBACK_HPP

#include <iosfwd>

#include "visual_feedback.hpp"

namespace subliminal {

class text_feedback : public visual_feedback {
  public:
    text_feedback(std::ostream& out) : out_(out) {}
    virtual void show(ffmsxx::video_frame const&, int image);
  private:
    std::ostream& out_;
};

}

#endif // SUBLIMINAL__TEXT_FEEDBACK_HPP

