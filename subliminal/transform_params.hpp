#ifndef SUBLIMINAL__TRANSFORM_PARAMS
#define SUBLIMINAL__TRANSFORM_PARAMS

#include <boost/fusion/include/is_sequence.hpp>
#include <boost/fusion/include/at.hpp>

namespace subliminal {

struct transform_params {
  template<typename Sequence>
  transform_params(Sequence const& s) {
    BOOST_MPL_ASSERT((
        typename boost::fusion::traits::is_sequence<Sequence>::type
      ));
    BOOST_MPL_ASSERT_RELATION(
      boost::fusion::result_of::size<Sequence>::type::value,==,4
    );
    x_shift = boost::fusion::at<boost::mpl::int_<0>>(s);
    y_shift = boost::fusion::at<boost::mpl::int_<1>>(s);
    x_scale = boost::fusion::at<boost::mpl::int_<2>>(s);
    y_scale = boost::fusion::at<boost::mpl::int_<3>>(s);
  }

  double x_shift;
  double y_shift;
  double x_scale;
  double y_scale;
};

}

#endif // SUBLIMINAL__TRANSFORM_PARAMS

