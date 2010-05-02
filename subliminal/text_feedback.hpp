#ifndef SUBLIMINAL__TEXT_FEEDBACK_HPP
#define SUBLIMINAL__TEXT_FEEDBACK_HPP

#include <iosfwd>

#include "visual_feedback.hpp"

namespace subliminal {

class text_feedback : public visual_feedback {
  public:
    text_feedback(std::ostream& out) : out_(out) {}
    virtual void message(std::string const&);
    virtual void show_rgb(boost::gil::rgb8c_view_t const&, int image);
    virtual void progress(int pos, int outof);
  private:
    std::ostream& out_;
};

}

#endif // SUBLIMINAL__TEXT_FEEDBACK_HPP

