#ifndef SUBLIMINAL__N_VECTOR_HPP
#define SUBLIMINAL__N_VECTOR_HPP

#include <boost/fusion/include/transform.hpp>

namespace subliminal {

template<typename T, size_t N>
struct n_vector {
  typedef typename boost::fusion::result_of::as_vector<
    typename boost::mpl::fold<
      boost::mpl::range_c<size_t, 0, N>,
      boost::mpl::vector0<>,
      boost::mpl::push_back<
        boost::mpl::_1,
        T
      >
    >::type
  >::type type;
};

}

#endif // SUBLIMINAL__N_VECTOR_HPP

