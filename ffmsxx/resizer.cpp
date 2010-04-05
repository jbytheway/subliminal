#include <ffmsxx/resizer.hpp>

#include <ffms.h>

#include <ffmsxx/fatal.hpp>

namespace ffmsxx {

int resizer_to_raw(resizer const r)
{
  switch (r) {
    case resizer::bicubic:
      return FFMS_RESIZER_BILINEAR;
    default:
      FFMSXX_FATAL("unexpected resizer");
  }
}

}

