#include <ffmsxx/video_dimensions.hpp>

#include <ostream>

namespace ffmsxx {

std::ostream& operator<<(std::ostream& o, video_dimensions const& d)
{
  o << "[w=" << d.width() << ", h=" << d.height() << "]";
  return o;
}

}

