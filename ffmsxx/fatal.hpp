#ifndef FFMSXX__FATAL_HPP
#define FFMSXX__FATAL_HPP

#include <cstdlib>
#include <ostream>

namespace ffmsxx {

extern std::ostream& error_stream;

}

#define FFMSXX_FATAL(msg) \
  do { \
    ffmsxx::error_stream << __FILE__ << ":" << __LINE__ << ":" << \
      __PRETTY_FUNCTION__ <<  ": fatal error: " << msg << std::endl; \
    std::abort(); \
  } while(false)

#endif // FFMSXX__FATAL_HPP

