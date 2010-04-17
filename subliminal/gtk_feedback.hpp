#ifndef SUBLIMINAL__GTK_FEEDBACK_HPP
#define SUBLIMINAL__GTK_FEEDBACK_HPP

#include <memory>
#include <iosfwd>

#include <boost/filesystem/path.hpp>

#include "text_feedback.hpp"

namespace subliminal {

class gtk_feedback : public text_feedback {
  public:
    gtk_feedback(boost::filesystem::path dataPath, std::ostream&);
    ~gtk_feedback();
    virtual void show_rgb(boost::gil::rgb8c_view_t const&, int image);
  private:
    struct impl;
    std::unique_ptr<impl> impl_;
};

}

#endif // SUBLIMINAL__GTK_FEEDBACK_HPP

