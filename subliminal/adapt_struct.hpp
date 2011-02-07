#ifndef SUBLIMINAL__ADAPT_STRUCT_HPP
#define SUBLIMINAL__ADAPT_STRUCT_HPP

#include <boost/fusion/include/adapt_struct.hpp>

#define SUBLIMINAL_ADAPT_STRUCT_HELPER(r, class, member) \
  (decltype(class::member), member)

// Easier-to-use version of FUSION_ADAPT_STRUCT taking advantage of decltype
#define SUBLIMINAL_ADAPT_STRUCT(class, members) \
  BOOST_FUSION_ADAPT_STRUCT(                    \
      class,                                    \
      BOOST_PP_SEQ_FOR_EACH(SUBLIMINAL_ADAPT_STRUCT_HELPER, class, members) \
    )

#endif // SUBLIMINAL__ADAPT_STRUCT_HPP

