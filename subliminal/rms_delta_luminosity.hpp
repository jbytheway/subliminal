#ifndef SUBLIMINAL__RMS_DELTA_LUMINOSITY_HPP
#define SUBLIMINAL__RMS_DELTA_LUMINOSITY_HPP

#include "delta_luminosity.hpp"

namespace subliminal {

namespace rms_delta_luminosity_detail {

  class square_value_accumulator {
    public:
      square_value_accumulator() : tally_(0), num_(0) {}

      template<typename P>
      void operator()(P const t) {
        BOOST_MPL_ASSERT_RELATION(boost::gil::size<P>::value,==,1);
        double v = t[0];
        tally_ += v*v;
        ++num_;
      }

      double value() { return tally_/num_; }
    private:
      double tally_;
      size_t num_;
  };

  template<typename View>
  double rms_value(View const& view)
  {
    return sqrt(
      boost::gil::for_each_pixel(view, square_value_accumulator()).value()
    );
  }

}

template<typename View1, typename View2>
double rms_delta_luminosity(
  View1 const& view1,
  View2 const& view2,
  visual_feedback& feedback
) {
  auto const dims = view1.dimensions();
  assert(view2.dimensions() == dims);
  boost::gil::gray8s_image_t delta(dims);
  delta_luminosity(view1, view2, view(delta));
  feedback.show(const_view(delta), 2);
  // Score by the rms delta luminosity
  return rms_delta_luminosity_detail::rms_value(view(delta));
}

}

#endif // SUBLIMINAL__RMS_DELTA_LUMINOSITY_HPP

