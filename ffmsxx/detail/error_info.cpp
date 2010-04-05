#include <ffmsxx/detail/error_info.hpp>

#include <ffmsxx/error.hpp>

namespace ffmsxx { namespace detail {

error_info::error_info()
{
  raw_.Buffer = message;
  raw_.BufferSize = sizeof(message);
  raw_.ErrorType = FFMS_ERROR_SUCCESS;
  raw_.SubType = FFMS_ERROR_SUCCESS;
}

void error_info::throw_if_null(void* p)
{
  if (!p) {
    throw error(message);
  }
}

}}

