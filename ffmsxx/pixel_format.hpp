#ifndef FFMSXX__PIXEL_FORMAT_HPP
#define FFMSXX__PIXEL_FORMAT_HPP

#include <string>

namespace ffmsxx {

class pixel_format {
  public:
    explicit pixel_format(int const raw);
    explicit pixel_format(std::string const& name);

    int raw() const { return raw_; }
  private:
    int raw_;
};

inline bool operator==(pixel_format const& l, pixel_format const& r) {
  return l.raw() == r.raw();
}

inline bool operator!=(pixel_format const& l, pixel_format const& r) {
  return l.raw() != r.raw();
}

}

#endif // FFMSXX__PIXEL_FORMAT_HPP

