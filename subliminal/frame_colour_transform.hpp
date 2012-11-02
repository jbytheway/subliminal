#ifndef SUBLIMINAL_FRAME_COLOUR_TRANSFORM_HPP
#define SUBLIMINAL_FRAME_COLOUR_TRANSFORM_HPP

#include <array>

#include "rectangle.hpp"
#include "frame_spatial_transform.hpp"
#include "shift_scale.hpp"
#include "adjust_brightness_contrast.hpp"

namespace subliminal {

class frame_colour_transform {
  public:
    template<typename In, typename Out>
    rectangle<ptrdiff_t> operator()(In const& in, Out const& out) {
      auto const area = spatial_(in, out);

      typedef typename In::value_type InPixel;
      typedef typename Out::value_type OutPixel;
      BOOST_MPL_ASSERT_RELATION(
        boost::gil::size<OutPixel>::value,==,boost::gil::size<OutPixel>::value
      );
      for_each_pixel(
        subimage_view(out, area),
        adjust_brightness_contrast<3>(chromatic_)
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
    std::array<shift_scale, 3> chromatic_;
};

}

#endif // SUBLIMINAL_FRAME_COLOUR_TRANSFORM_HPP

