#ifndef FFMSXX__DETAIL__ERROR_INFO_HPP
#define FFMSXX__DETAIL__ERROR_INFO_HPP

#include <boost/noncopyable.hpp>

#include <ffms.h>

namespace ffmsxx { namespace detail {

class error_info : boost::noncopyable {
  public:
    error_info();

    FFMS_ErrorInfo* raw() { return &raw_; }
    void throw_if(bool);
    void throw_if_null(void const*);
  private:
    char message[1024];
    FFMS_ErrorInfo raw_;
};

}}

#endif // FFMSXX__DETAIL__ERROR_INFO_HPP

