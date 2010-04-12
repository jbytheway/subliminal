#include "text_feedback.hpp"

#include <ostream>

#include <ffmsxx/video_dimensions.hpp>

namespace subliminal {

void text_feedback::show(ffmsxx::video_frame const&, int image)
{
  out_ << "\n[frame " << image << "]" << std::endl;
}

void text_feedback::progress(int pos, int outof)
{
  out_ << pos << '/' << outof << '\r' << std::flush;
}

}

