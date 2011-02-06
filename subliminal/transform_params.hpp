#ifndef SUBLIMINAL__TRANSFORM_PARAMS
#define SUBLIMINAL__TRANSFORM_PARAMS

#include <cassert>
#include <iosfwd>

#include <boost/fusion/include/is_sequence.hpp>
#include <boost/fusion/include/at.hpp>
#include <boost/fusion/include/size.hpp>

namespace subliminal {

struct transform_params {
  transform_params() :
    x_shift(0), x_scale(1),
    y_shift(0), y_scale(1),
    l_shift(0), l_scale(1)
  {
  }

  template<typename Sequence>
  transform_params(Sequence const& s) {
    BOOST_MPL_ASSERT((
        typename boost::fusion::traits::is_sequence<Sequence>::type
      ));
    BOOST_MPL_ASSERT_RELATION(
      boost::fusion::result_of::size<Sequence>::type::value,==,6
    );
    x_shift = boost::fusion::at<boost::mpl::int_<0>>(s);
    x_scale = boost::fusion::at<boost::mpl::int_<1>>(s);
    y_shift = boost::fusion::at<boost::mpl::int_<2>>(s);
    y_scale = boost::fusion::at<boost::mpl::int_<3>>(s);
    l_shift = boost::fusion::at<boost::mpl::int_<4>>(s);
    l_scale = boost::fusion::at<boost::mpl::int_<5>>(s);

    assert(x_scale);
    assert(y_scale);
  }

  transform_params(
    double x_sh, double x_sc,
    double y_sh, double y_sc,
    double l_sh, double l_sc
  ) :
    x_shift(x_sh), x_scale(x_sc),
    y_shift(y_sh), y_scale(y_sc),
    l_shift(l_sh), l_scale(l_sc)
  {
    assert(x_scale);
    assert(y_scale);
  }

  double x_shift;
  double x_scale;
  double y_shift;
  double y_scale;
  double l_shift;
  double l_scale;
};

std::istream& operator>>(std::istream&, transform_params&);
std::ostream& operator<<(std::ostream&, transform_params const&);

}

#endif // SUBLIMINAL__TRANSFORM_PARAMS

