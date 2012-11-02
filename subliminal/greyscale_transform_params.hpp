#ifndef SUBLIMINAL__GREYSCALE_TRANSFORM_PARAMS
#define SUBLIMINAL__GREYSCALE_TRANSFORM_PARAMS

#include <cassert>
#include <iosfwd>

#include <boost/fusion/include/is_sequence.hpp>
#include <boost/fusion/include/at.hpp>
#include <boost/fusion/include/size.hpp>

#include "adapt_struct.hpp"
#include "spatial_transform_params.hpp"

namespace subliminal {

struct greyscale_transform_params {
  greyscale_transform_params() :
    x_shift(0), x_scale(0),
    y_shift(0), y_scale(0),
    l_shift(0), l_scale(0)
  {
  }

  template<typename Sequence>
  greyscale_transform_params(Sequence const& s) {
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

  greyscale_transform_params(
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

  spatial_transform_params spatial() const {
    return {x_shift, x_scale, y_shift, y_scale};
  }

  // FIXME: if it works, we should factor out a spatial_transform_params here
  double x_shift;
  double x_scale;
  double y_shift;
  double y_scale;
  double l_shift;
  double l_scale;
};

std::istream& operator>>(std::istream&, greyscale_transform_params&);
std::ostream& operator<<(std::ostream&, greyscale_transform_params const&);

}

SUBLIMINAL_ADAPT_STRUCT(
  subliminal::greyscale_transform_params,
  (x_shift)
  (x_scale)
  (y_shift)
  (y_scale)
  (l_shift)
  (l_scale)
)

#endif // SUBLIMINAL__GREYSCALE_TRANSFORM_PARAMS

