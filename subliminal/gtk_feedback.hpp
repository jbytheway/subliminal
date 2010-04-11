#ifndef SUBLIMINAL__GTK_FEEDBACK_HPP
#define SUBLIMINAL__GTK_FEEDBACK_HPP

#include <memory>
#include <iosfwd>

#include <boost/filesystem/path.hpp>

#include "visual_feedback.hpp"

namespace subliminal {

class gtk_feedback : public visual_feedback {
  public:
    gtk_feedback(boost::filesystem::path dataPath, std::ostream&);
    ~gtk_feedback();
    virtual void show(ffmsxx::video_frame const&);
    virtual void end();
  private:
    struct impl;
    std::unique_ptr<impl> impl_;
};

}

#endif // SUBLIMINAL__GTK_FEEDBACK_HPP

