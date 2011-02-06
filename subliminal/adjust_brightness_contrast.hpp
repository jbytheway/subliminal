#ifndef SUBLIMINAL__ADJUST_BRIGHTNESS_CONTRAST_HPP
#define SUBLIMINAL__ADJUST_BRIGHTNESS_CONTRAST_HPP

#include "clamp.hpp"

namespace subliminal {

class adjust_brightness_contrast {
  public:
    adjust_brightness_contrast(double l_shift, double l_scale) :
      l_shift_(l_shift), l_scale_(l_scale)
    {}

    template<typename P>
    void operator()(P& t) {
      BOOST_MPL_ASSERT_RELATION(boost::gil::size<P>::value,==,1);
      typedef typename std::remove_reference<decltype(t[0])>::type Value;
      t[0] = clamp<Value>(std::round(t[0]*l_scale_+l_shift_));
    }
  private:
    double l_shift_;
    double l_scale_;
};

}

#endif // SUBLIMINAL__ADJUST_BRIGHTNESS_CONTRAST_HPP

