#include <ffmsxx/pixel_format.hpp>

#include <cassert>

#include <ffms.h>

#include <ffmsxx/error.hpp>

namespace ffmsxx {

pixel_format::pixel_format(int const raw) :
  raw_{raw}
{
  assert(raw_ >= 0);
}

pixel_format::pixel_format(std::string const& name)
{
  raw_ = FFMS_GetPixFmt(name.c_str());
  if (raw_ == -1) {
    throw error("unrecognized pixel format");
  }
}

}

