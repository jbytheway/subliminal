#ifndef SUBLIMINAL_FRAME_SPATIAL_TRANSFORM_HPP
#define SUBLIMINAL_FRAME_SPATIAL_TRANSFORM_HPP

#include <boost/gil/extension/numeric/sampler.hpp>
#include <boost/gil/extension/numeric/resample.hpp>

#include "rectangle.hpp"
#include "spatial_transform_params.hpp"
#include "invalid_transform.hpp"

namespace subliminal {

class frame_spatial_transform {
  public:
    // HACK: I have to move the default impl of the copy constructor into the
    // cpp file to avoid a really stupid compiler warning from gcc 4.6.2.
    // Hopefully this will go away with later versions.
    frame_spatial_transform(frame_spatial_transform const&);

    frame_spatial_transform(
      boost::gil::point2<ptrdiff_t> const& dims,
      double x_shift, double x_scale,
      double y_shift, double y_scale
    )
    {
      init(dims, x_shift, x_scale, y_shift, y_scale);
    }

    frame_spatial_transform(
      boost::gil::point2<ptrdiff_t> const& dims,
      spatial_transform_params const& params
    )
    {
      init(
        dims,
        params.x_shift, params.x_scale,
        params.y_shift, params.y_scale
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
      if (top_left.x > bottom_right.x ||
        top_left.y > bottom_right.y) {
        throw invalid_transform();
      }
      return rectangle<ptrdiff_t>(top_left, bottom_right);
    }

    template<typename In, typename Out>
    rectangle<ptrdiff_t> operator()(In const& in, Out const& out) {
      resample_pixels(in, out, matrix_, boost::gil::bilinear_sampler());
      return meaningful_area(in.dimensions(), out.dimensions());
    }

  private:
    void init(
      boost::gil::point2<ptrdiff_t> const& dims,
      double x_shift, double x_scale,
      double y_shift, double y_scale
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
    }

    boost::gil::matrix3x2<double> matrix_;
    boost::gil::matrix3x2<double> inverse_matrix_;
};

}

#endif // SUBLIMINAL_FRAME_SPATIAL_TRANSFORM_HPP

