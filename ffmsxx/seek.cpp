#include <ffmsxx/seek.hpp>

#include <ffms.h>

#include <ffmsxx/fatal.hpp>

namespace ffmsxx {

int seek_to_raw(seek const s)
{
  switch (s) {
    case seek::normal:
      return FFMS_SEEK_NORMAL;
    default:
      FFMSXX_FATAL("unexpected seek");
  }
}

}

