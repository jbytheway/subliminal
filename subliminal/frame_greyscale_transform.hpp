#ifndef SUBLIMINAL__FRAME_GREYSCALE_TRANSFORM_HPP
#define SUBLIMINAL__FRAME_GREYSCALE_TRANSFORM_HPP

#include <boost/gil/extension/numeric/sampler.hpp>
#include <boost/gil/extension/numeric/resample.hpp>

#include "greyscale_transform_params.hpp"
#include "adjust_brightness_contrast.hpp"
#include "rectangle.hpp"
#include "subimage_view.hpp"

namespace subliminal {

class frame_greyscale_transform {
  public:
    frame_greyscale_transform(
      boost::gil::point2<ptrdiff_t> const& dims,
      double x_shift, double x_scale,
      double y_shift, double y_scale,
      double l_shift, double l_scale
    )
    {
      init(dims, x_shift, x_scale, y_shift, y_scale, l_shift, l_scale);
    }

    frame_greyscale_transform(
      boost::gil::point2<ptrdiff_t> const& dims,
      greyscale_transform_params const& params
    )
    {
      init(
        dims,
        params.x_shift, params.x_scale,
        params.y_shift, params.y_scale,
        params.l_shift, params.l_scale
      );
    }

    rectangle<ptrdiff_t> meaningful_area(
        boost::gil::point2<ptrdiff_t> in_dims,
        boost::gil::point2<ptrdiff_t> out_dims
      ) {
      auto top_left = boost::gil::point2<double>(0,0) * inverse_matrix_;
      top_left.x = std::ceil(std::max(top_left.x, 0.0));
      top_left.y = std::ceil(std::max(top_left.y, 0.0));
      auto bottom_right = in_dims * inverse_matrix_;
      bottom_right.x =
        std::floor(std::min<double>(bottom_right.x, out_dims.x));
      bottom_right.y =
        std::floor(std::min<double>(bottom_right.y, out_dims.y));
      return rectangle<ptrdiff_t>(top_left, bottom_right);
    }

    template<typename In, typename Out>
    rectangle<ptrdiff_t> operator()(In const& in, Out const& out) {
      typedef typename Out::value_type OutPixel;
      BOOST_MPL_ASSERT_RELATION(boost::gil::size<OutPixel>::value,==,1);
      resample_pixels(in, out, matrix_, boost::gil::bilinear_sampler());
      auto const area =
        meaningful_area(in.dimensions(), out.dimensions());
      for_each_pixel(
        subimage_view(out, area),
        adjust_brightness_contrast(l_shift_, real_l_scale_)
      );
      return area;
    }
  private:
    void init(
      boost::gil::point2<ptrdiff_t> const& dims,
      double x_shift, double x_scale,
      double y_shift, double y_scale,
      double l_shift, double l_scale
    ) {
      double const real_x_scale = (dims.x + x_scale)/dims.x;
      double const real_y_scale = (dims.y + y_scale)/dims.y;
      matrix_ =
        boost::gil::matrix3x2<double>::get_translate(
          boost::gil::point2<double>(-dims.x/2+x_shift, -dims.y/2+y_shift)
        ) *
        boost::gil::matrix3x2<double>::get_scale(
          boost::gil::point2<double>(real_x_scale, real_y_scale)
        ) *
        boost::gil::matrix3x2<double>::get_translate(
          boost::gil::point2<double>(dims.x/2, dims.y/2)
        );
      inverse_matrix_ =
        boost::gil::matrix3x2<double>::get_translate(
          boost::gil::point2<double>(-dims.x/2, -dims.y/2)
        ) *
        boost::gil::matrix3x2<double>::get_scale(
          boost::gil::point2<double>(1.0/real_x_scale, 1.0/real_y_scale)
        ) *
        boost::gil::matrix3x2<double>::get_translate(
          boost::gil::point2<double>(dims.x/2-x_shift, dims.y/2-y_shift)
        );
      l_shift_ = l_shift;
      real_l_scale_ = (256+l_scale)/256;
    }

    boost::gil::matrix3x2<double> matrix_;
    boost::gil::matrix3x2<double> inverse_matrix_;
    double l_shift_;
    double real_l_scale_;
};

}

#endif // SUBLIMINAL__FRAME_GREYSCALE_TRANSFORM_HPP

