#ifndef SUBLIMINAL_SPATIAL_TRANSFORM_PARAMS_HPP
#define SUBLIMINAL_SPATIAL_TRANSFORM_PARAMS_HPP

#include <cassert>
#include <iosfwd>

#include <boost/fusion/include/is_sequence.hpp>
#include <boost/fusion/include/at.hpp>
#include <boost/fusion/include/size.hpp>

#include "adapt_struct.hpp"

namespace subliminal {

struct spatial_transform_params {
  spatial_transform_params() :
    x_shift(0), x_scale(0),
    y_shift(0), y_scale(0)
  {
  }

  spatial_transform_params(
    double x_sh, double x_sc,
    double y_sh, double y_sc
  ) :
    x_shift(x_sh), x_scale(x_sc),
    y_shift(y_sh), y_scale(y_sc)
  {
    assert(x_scale);
    assert(y_scale);
  }

  double x_shift;
  double x_scale;
  double y_shift;
  double y_scale;
};

}

SUBLIMINAL_ADAPT_STRUCT(
  subliminal::spatial_transform_params,
  (x_shift)
  (x_scale)
  (y_shift)
  (y_scale)
)

#endif // SUBLIMINAL_SPATIAL_TRANSFORM_PARAMS_HPP

