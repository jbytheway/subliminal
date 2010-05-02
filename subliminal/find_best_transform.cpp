#include "find_best_transform.hpp"

#include <boost/spirit/home/phoenix/object/construct.hpp>
#include <boost/spirit/home/phoenix/core/argument.hpp>

#include "linear_state_space.hpp"
#include "product_state_space.hpp"
#include "optimize.hpp"
#include "delta_luminosity.hpp"

namespace subliminal {

namespace gil = boost::gil;

namespace {

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
    return sqrt(gil::for_each_pixel(view, square_value_accumulator()).value());
  }

  template<typename TransformGen, typename View>
  class luminosity_match_scorer {
    public:
      luminosity_match_scorer(
        TransformGen transform_gen,
        View const& transformee_view,
        View const& ref_view,
        visual_feedback& feedback
      ) :
        transform_gen_(std::move(transform_gen)),
        transformee_view_(transformee_view),
        ref_view_(ref_view),
        feedback_(feedback)
      {}

      template<typename State>
      double operator()(State const& state) const {
        // Make the transformation function for this state
        auto transf = transform_gen_(state);
        // Transform one image
        boost::gil::gray8_image_t transformed(transformee_view_.dimensions());
        transf(transformee_view_, view(transformed));
        // Compute the delta of the transformed and other image
        boost::gil::gray8s_image_t delta(ref_view_.dimensions());
        delta_luminosity(const_view(transformed), ref_view_, view(delta));
        feedback_.show(const_view(delta), 2);
        // Score by the rms delta luminosity
        return -rms_value(view(delta));
      }
    private:
      TransformGen transform_gen_;
      View const& transformee_view_;
      View const& ref_view_;
      visual_feedback& feedback_;
  };

}

frame_transform find_best_transform(
  ffmsxx::video_frame const& from,
  ffmsxx::video_frame const& to,
  visual_feedback& feedback
)
{
  feedback.show(from, 0);
  feedback.show(to, 1);

  // Get Boost.GIL views on the images
  auto from_view = make_gil_view(from);
  auto to_view = make_gil_view(to);

  namespace px = boost::phoenix;
  using namespace px::arg_names;
  auto transform_maker =
    px::construct<frame_transform>(from_view.dimensions(), arg1);

  luminosity_match_scorer<decltype(transform_maker), decltype(to_view)>
    scorer(transform_maker, from_view, to_view, feedback);

  auto best_state = optimize(
    make_product_state_space(
      // x shift and scale
      linear_state_space<double>(-10, 10, 0, 1.0/4),
      linear_state_space<double>(0.95, 1.05, 1.0, 0.002),
      // y shift and scale
      linear_state_space<double>(-10, 10, 0, 1.0/4),
      linear_state_space<double>(0.95, 1.05, 1.0, 0.002),
      // l shift and scale
      linear_state_space<double>(-10, 10, 0, 1.0),
      linear_state_space<double>(0.9, 1.1, 1.0, 0.01)
    ),
    scorer,
    feedback
  );

  return frame_transform(from_view.dimensions(), best_state);
}

}

