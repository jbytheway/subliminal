#include "find_best_greyscale_transform.hpp"

#include <boost/spirit/home/phoenix/object/construct.hpp>
#include <boost/spirit/home/phoenix/core/argument.hpp>
#include <boost/assign/list_of.hpp>

#include "linear_state_space.hpp"
#include "product_state_space.hpp"
#include "find_minimum_on_graph.hpp"
#include "find_minimum_gsl.hpp"
#include "rms_delta_luminosity.hpp"
#include "subimage_view.hpp"

namespace subliminal {

namespace {

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
        boost::gil::gray8_image_t transformed(ref_view_.dimensions());
        auto meaningful_area = transf(transformee_view_, view(transformed));
        // Compute the delta of the transformed and other image
        return rms_delta_luminosity(
          subimage_view(const_view(transformed), meaningful_area),
          subimage_view(ref_view_, meaningful_area),
          feedback_
        );
      }
    private:
      TransformGen transform_gen_;
      View const& transformee_view_;
      View const& ref_view_;
      visual_feedback& feedback_;
  };

}

std::tuple<greyscale_transform_params, frame_greyscale_transform, double>
find_best_greyscale_transform(
  ffmsxx::video_frame const& from,
  ffmsxx::video_frame const& to,
  greyscale_transform_params const& start_params,
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
    px::construct<frame_greyscale_transform>(from_view.dimensions(), arg1);

  luminosity_match_scorer<decltype(transform_maker), decltype(to_view)>
    scorer(transform_maker, from_view, to_view, feedback);

  // Search using a GSL multidimensional minimizer (specifically: Nelder-Mead
  // Simplex algorithm)
  auto const best_state_and_score =
    find_minimum_gsl<greyscale_transform_params>(
    boost::assign::list_of
      (start_params.x_shift)
      (start_params.x_scale)
      (start_params.y_shift)
      (start_params.y_scale)
      (start_params.l_shift)
      (start_params.l_scale),
    boost::assign::list_of(8.0)(8.0)(8.0)(8.0)(8.0)(8.0),
    0.1, // this tells it how precisely we want to know the answer
    scorer,
    feedback
  );

  auto const best_state = std::get<0>(best_state_and_score);
  double const score = std::get<1>(best_state_and_score);

  feedback.messagef(
    boost::format("Final: score(%s) = %s\n") % best_state % score);

  return std::make_tuple(
    best_state, frame_greyscale_transform(from_view.dimensions(), best_state),
    score
  );
}

}

