#include "text_feedback.hpp"

#include <ostream>

#include <ffmsxx/video_dimensions.hpp>

namespace subliminal {

void text_feedback::show(ffmsxx::video_frame const&, int image)
{
  out_ << "[frame " << image << "]" << std::endl;
}

void text_feedback::end()
{
  out_ << "[end]" << std::endl;
}

}

