#include "text_feedback.hpp"

#include <ostream>

#include <ffmsxx/video_dimensions.hpp>

namespace subliminal {

void text_feedback::show_rgb(boost::gil::rgb8c_view_t const&, int image)
{
  out_ << "\n[frame " << image << "]" << std::endl;
}

void text_feedback::progress(int pos, int outof)
{
  out_ << pos << '/' << outof << '\r' << std::flush;
}

}

