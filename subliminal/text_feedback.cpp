#include "text_feedback.hpp"

#include <ostream>

#include <ffmsxx/video_dimensions.hpp>

namespace subliminal {

void text_feedback::set_dimensions(ffmsxx::video_dimensions const& dims)
{
  out_ << "dimensions set to " << dims << std::endl;
}

void text_feedback::show(ffmsxx::video_frame const&)
{
  out_ << "[frame]" << std::endl;
}

}

