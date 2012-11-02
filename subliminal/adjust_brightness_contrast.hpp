#ifndef SUBLIMINAL__ADJUST_BRIGHTNESS_CONTRAST_HPP
#define SUBLIMINAL__ADJUST_BRIGHTNESS_CONTRAST_HPP

#include <cmath>
#include <array>

#include <boost/mpl/vector_c.hpp> // GIL missing this include
#include <boost/gil/metafunctions.hpp>
#include <boost/gil/color_base_algorithm.hpp>

#include "clamp.hpp"
#include "shift_scale.hpp"

namespace subliminal {

namespace adjust_brightness_contrast_detail {
  struct adjust_channel {
    template<typename T>
    void operator()(T& t) const {
      BOOST_MPL_ASSERT((std::is_integral<T>));
      t = clamp<T>(std::round(t*params_.scale+params_.shift));
    }

    shift_scale params_;
  };
}

template<size_t Size>
class adjust_brightness_contrast {
  public:
    template<typename = void>
    adjust_brightness_contrast(shift_scale const& params) :
      params_{params}
    {}

    adjust_brightness_contrast(std::array<shift_scale, Size> const& params) :
      params_(params)
    {}

    template<typename P>
    void operator()(P& p) const {
      BOOST_MPL_ASSERT_RELATION(boost::gil::size<P>::value,==,Size);

      boost::gil::static_for_each(
        p, adjust_brightness_contrast_detail::adjust_channel{params_}
      );
    }
  private:
    std::array<shift_scale, Size> params_;
};

}

#endif // SUBLIMINAL__ADJUST_BRIGHTNESS_CONTRAST_HPP

