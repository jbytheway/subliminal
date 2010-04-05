#ifndef FFMSXX__TRACK_TYPE_HPP
#define FFMSXX__TRACK_TYPE_HPP

namespace ffmsxx {

enum class track_type {
  video,
  audio,
};

track_type track_type_from_raw(int);

}

#endif // FFMSXX__TRACK_TYPE_HPP

