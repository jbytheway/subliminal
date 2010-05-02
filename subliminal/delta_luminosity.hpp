#ifndef SUBLIMINAL__DELTA_LUMINOSITY_HPP
#define SUBLIMINAL__DELTA_LUMINOSITY_HPP

namespace subliminal {

  struct half_difference_luminosity {
    template<typename Pixel1, typename Pixel2>
    int operator()(Pixel1 in1, Pixel2 in2) const {
      boost::gil::color_convert_deref_fn<
        const Pixel1&,
        boost::gil::gray8_pixel_t
      > cc1;
      boost::gil::color_convert_deref_fn<
        const Pixel2&,
        boost::gil::gray8_pixel_t
      > cc2;
      return (cc1(in1) - cc2(in2)) / 2;
    }
  };

  template<typename InView1, typename InView2, typename OutView>
  void delta_luminosity(
    InView1 const& in1,
    InView2 const& in2,
    OutView const& out
  )
  {
    boost::gil::transform_pixels(in1, in2, out, half_difference_luminosity());
  }

}

#endif // SUBLIMINAL__DELTA_LUMINOSITY_HPP

