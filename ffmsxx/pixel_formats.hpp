#ifndef FFMSXX__PIXEL_FORMATS_HPP
#define FFMSXX__PIXEL_FORMATS_HPP

#include <ffmsxx/pixel_format.hpp>

namespace ffmsxx {

class pixel_formats {
  public:
    explicit pixel_formats(pixel_format const f)
    {
      // Don't want to use the initializer list constructor because of
      // potential confusion with the (size, element) constructor
      raw_.insert(raw_.end(), {f.raw(), -1});
    }

    // FFMS2 representation is a pointer to a -1-terminated list.
    int const* raw() const { return raw_.data(); }
  private:
    std::vector<int> raw_;
};

}

#endif // FFMSXX__PIXEL_FORMATS_HPP

