#ifndef SUBLIMINAL__FRAME_TRANSFORM_HPP
#define SUBLIMINAL__FRAME_TRANSFORM_HPP

#include <boost/gil/extension/numeric/sampler.hpp>
#include <boost/gil/extension/numeric/resample.hpp>

namespace subliminal {

class frame_transform {
  public:
    frame_transform(double x_shift) :
      x_shift_{x_shift},
      matrix_{
        boost::gil::matrix3x2<double>::get_translate(
          boost::gil::point2<double>(x_shift_, 0)
        )
      }
    {}

    template<typename In, typename Out>
    void operator()(In const& in, Out const& out) {
      resample_pixels(in, out, matrix_, boost::gil::bilinear_sampler());
    }
  private:
    double x_shift_;
    boost::gil::matrix3x2<double> matrix_;
};

}

#endif // SUBLIMINAL__FRAME_TRANSFORM_HPP

