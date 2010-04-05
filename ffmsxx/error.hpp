#ifndef FFMSXX__ERROR_HPP
#define FFMSXX__ERROR_HPP

#include <stdexcept>

namespace ffmsxx {

class error : std::runtime_error {
  public:
    error(std::string const& m) : std::runtime_error(m) {}
};

}

#endif // FFMSXX__ERROR_HPP

