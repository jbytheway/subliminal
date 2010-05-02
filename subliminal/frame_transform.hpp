#ifndef SUBLIMINAL__FRAME_TRANSFORM_HPP
#define SUBLIMINAL__FRAME_TRANSFORM_HPP

#include <boost/gil/extension/numeric/sampler.hpp>
#include <boost/gil/extension/numeric/resample.hpp>

#include "transform_params.hpp"

namespace subliminal {

class frame_transform {
  public:
    frame_transform(
      boost::gil::point2<ptrdiff_t> const& dims,
      double x_shift, double y_shift,
      double x_scale, double y_scale
    )
    {
      init(dims, x_shift, y_shift, x_scale, y_scale);
    }

    frame_transform(
      boost::gil::point2<ptrdiff_t> const& dims,
      transform_params const& params
    )
    {
      init(
        dims, params.x_shift, params.y_shift, params.x_scale, params.y_scale
      );
    }

    template<typename In, typename Out>
    void operator()(In const& in, Out const& out) {
      resample_pixels(in, out, matrix_, boost::gil::bilinear_sampler());
    }
  private:
    void init(
      boost::gil::point2<ptrdiff_t> const& dims,
      double x_shift, double y_shift,
      double x_scale, double y_scale
    ) {
      matrix_ =
        boost::gil::matrix3x2<double>::get_translate(
          boost::gil::point2<double>(-dims.x/2+x_shift, -dims.y/2+y_shift)
        ) *
        boost::gil::matrix3x2<double>::get_scale(
          boost::gil::point2<double>(x_scale, y_scale)
        ) *
        boost::gil::matrix3x2<double>::get_translate(
          boost::gil::point2<double>(dims.x/2, dims.y/2)
        );
    }

    boost::gil::matrix3x2<double> matrix_;
};

}

#endif // SUBLIMINAL__FRAME_TRANSFORM_HPP

