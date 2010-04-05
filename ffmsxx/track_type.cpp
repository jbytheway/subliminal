#include <ffmsxx/track_type.hpp>

#include <ffms.h>

#include <ffmsxx/fatal.hpp>

namespace ffmsxx {

track_type track_type_from_raw(int raw)
{
  switch (raw) {
    case FFMS_TYPE_VIDEO:
      return track_type::video;
    case FFMS_TYPE_AUDIO:
      return track_type::audio;
    default:
      FFMSXX_FATAL("unexpected raw track type");
  }
}

}

