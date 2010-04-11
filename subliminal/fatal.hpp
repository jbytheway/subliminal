#ifndef SUBLIMINAL__FATAL_HPP
#define SUBLIMINAL__FATAL_HPP

#include <cstdlib>
#include <ostream>

namespace subliminal {

extern std::ostream& error_stream;

}

#define SUBLIMINAL_FATAL(msg) \
  do { \
    subliminal::error_stream << __FILE__ << ":" << __LINE__ << ":" << \
      __PRETTY_FUNCTION__ <<  ": fatal error: " << msg << std::endl; \
    std::abort(); \
  } while(false)

#endif // SUBLIMINAL__FATAL_HPP


