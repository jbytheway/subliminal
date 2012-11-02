#ifndef SUBLIMINAL__FRAME_GREYSCALE_TRANSFORM_HPP
#define SUBLIMINAL__FRAME_GREYSCALE_TRANSFORM_HPP

#include "greyscale_transform_params.hpp"
#include "adjust_brightness_contrast.hpp"
#include "subimage_view.hpp"
#include "frame_spatial_transform.hpp"
#include "shift_scale.hpp"

namespace subliminal {

class frame_greyscale_transform {
  public:
    frame_greyscale_transform(
      boost::gil::point2<ptrdiff_t> const& dims,
      double x_shift, double x_scale,
      double y_shift, double y_scale,
      double l_shift, double l_scale
    ) :
      spatial_(dims, x_shift, x_scale, y_shift, y_scale),
      luminosity_{l_shift, (256+l_scale)/256}
    {
    }

    frame_greyscale_transform(
      boost::gil::point2<ptrdiff_t> const& dims,
      greyscale_transform_params const& params
    ) :
      spatial_(
        dims, params.x_shift, params.x_scale, params.y_shift, params.y_scale
      ),
      luminosity_{params.l_shift, (256+params.l_scale)/256}
    {
    }

    template<typename In, typename Out>
    rectangle<ptrdiff_t> operator()(In const& in, Out const& out) {
      auto const area = spatial_(in, out);

      typedef typename Out::value_type OutPixel;
      BOOST_MPL_ASSERT_RELATION(boost::gil::size<OutPixel>::value,==,1);
      for_each_pixel(
        subimage_view(out, area),
        adjust_brightness_contrast<1>(luminosity_)
      );
      return area;
    }

    rectangle<ptrdiff_t> meaningful_area(
        boost::gil::point2<ptrdiff_t> in_dims,
        boost::gil::point2<ptrdiff_t> out_dims
      ) {
      return spatial_.meaningful_area(in_dims, out_dims);
    }
  private:
    frame_spatial_transform spatial_;
    shift_scale luminosity_;
};

}

#endif // SUBLIMINAL__FRAME_GREYSCALE_TRANSFORM_HPP

