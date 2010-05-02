#ifndef SUBLIMINAL__VARIADIC_VECTOR_HPP
#define SUBLIMINAL__VARIADIC_VECTOR_HPP

#include <boost/fusion/include/as_vector.hpp>

namespace subliminal {

/** \bug Once g++ implements variadic templates fully this header will be
 * superfluous; it's a workaround for the fact that one can't expand a template
 * parameter pack into a fixed-length argument list. */
template<typename... Args>
struct variadic_sequence;

template<>
struct variadic_sequence<>
{
  typedef boost::mpl::vector<>::type type;
};

template<typename First, typename... More>
struct variadic_sequence<First, More...>
{
  typedef typename boost::mpl::push_front<
      typename variadic_sequence<More...>::type,
      First
    >::type type;
};

template<typename... Args>
struct variadic_vector {
  typedef typename boost::fusion::result_of::as_vector<
      typename variadic_sequence<Args...>::type
    >::type type;
};

}

#endif // SUBLIMINAL__VARIADIC_VECTOR_HPP

