#ifndef SUBLIMINAL__FRAME_TRANSFORM_HPP
#define SUBLIMINAL__FRAME_TRANSFORM_HPP

#include <boost/gil/extension/numeric/sampler.hpp>
#include <boost/gil/extension/numeric/resample.hpp>

#include "transform_params.hpp"
#include "adjust_brightness_contrast.hpp"

namespace subliminal {

class frame_transform {
  public:
    frame_transform(
      boost::gil::point2<ptrdiff_t> const& dims,
      double x_shift, double x_scale,
      double y_shift, double y_scale,
      double l_shift, double l_scale
    )
    {
      init(dims, x_shift, x_scale, y_shift, y_scale, l_shift, l_scale);
    }

    frame_transform(
      boost::gil::point2<ptrdiff_t> const& dims,
      transform_params const& params
    )
    {
      init(
        dims,
        params.x_shift, params.x_scale,
        params.y_shift, params.y_scale,
        params.l_shift, params.l_scale
      );
    }

    template<typename In, typename Out>
    void operator()(In const& in, Out const& out) {
      typedef typename Out::value_type OutPixel;
      BOOST_MPL_ASSERT_RELATION(boost::gil::size<OutPixel>::value,==,1);
      resample_pixels(in, out, matrix_, boost::gil::bilinear_sampler());
      for_each_pixel(out, adjust_brightness_contrast(l_shift_, l_scale_));
    }
  private:
    void init(
      boost::gil::point2<ptrdiff_t> const& dims,
      double x_shift, double x_scale,
      double y_shift, double y_scale,
      double l_shift, double l_scale
    ) {
      assert(x_scale);
      assert(y_scale);
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
      l_shift_ = l_shift;
      l_scale_ = l_scale;
    }

    boost::gil::matrix3x2<double> matrix_;
    double l_shift_;
    double l_scale_;
};

}

#endif // SUBLIMINAL__FRAME_TRANSFORM_HPP

