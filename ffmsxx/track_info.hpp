#ifndef FFMSXX__TRACK_INFO_HPP
#define FFMSXX__TRACK_INFO_HPP

#include <ffmsxx/track_type.hpp>

namespace ffmsxx {

class track_info {
  public:
    track_info(track_type const type) : type_(type) {}

    bool is_video() { return type_ == track_type::video; }
  private:
    track_type type_;
};

}

#endif // FFMSXX__TRACK_INFO_HPP

