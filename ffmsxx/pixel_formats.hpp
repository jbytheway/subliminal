#ifndef FFMSXX__PIXEL_FORMATS_HPP
#define FFMSXX__PIXEL_FORMATS_HPP

#include <ffmsxx/pixel_format.hpp>

namespace ffmsxx {

class pixel_formats {
  public:
    explicit pixel_formats(pixel_format const f) :
      raw_(uint64_t(1) << f.raw())
    {}

    uint64_t raw() const { return raw_; }
  private:
    uint64_t raw_;
};

}

#endif // FFMSXX__PIXEL_FORMATS_HPP

