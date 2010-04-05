#ifndef FFMSXX__PIXEL_FORMAT_HPP
#define FFMSXX__PIXEL_FORMAT_HPP

#include <string>

namespace ffmsxx {

class pixel_format {
  public:
    explicit pixel_format(std::string const& name);

    int raw() const { return raw_; }
  private:
    int raw_;
};

}

#endif // FFMSXX__PIXEL_FORMAT_HPP

