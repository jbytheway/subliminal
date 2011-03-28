#ifndef SUBLIMINAL__FIND_MINIMUM_GSL_HPP
#define SUBLIMINAL__FIND_MINIMUM_GSL_HPP

#include <array>

#include <boost/range/algorithm/copy.hpp>
#include <boost/mpl/range_c.hpp>
#include <boost/fusion/include/for_each.hpp>

#include <gsl/gsl_multimin.h>

namespace subliminal {

namespace gsl_detail {

  template<typename Sequence>
  struct assign_to {
    Sequence& sequence;
    gsl_vector const* const vector;

    template<typename I>
    void operator()(I const) const {
      boost::fusion::at<I>(sequence) = gsl_vector_get(vector, I());
    }
  };

  template<typename Sequence>
  Sequence make_from_gsl_vector(gsl_vector const* x) {
    size_t const N = boost::fusion::result_of::size<Sequence>::type::value;
    Sequence result;
    boost::fusion::for_each(
      boost::mpl::range_c<size_t, 0, N>(), assign_to<Sequence>{result, x}
    );
    return result;
  }

}

template<typename StateType, typename Range1, typename Range2, typename Scorer>
std::tuple<StateType, double> find_minimum_gsl(
  Range1 const& startRange,
  Range2 const& stepRange,
  double const precision,
  Scorer const& scorer,
  visual_feedback& feedback
)
{
  const size_t N = boost::fusion::result_of::size<StateType>::type::value;
  assert(boost::size(startRange) == ptrdiff_t(N));
  assert(boost::size(stepRange) == ptrdiff_t(N));
  std::shared_ptr<gsl_multimin_fminimizer> minimizer(
    gsl_multimin_fminimizer_alloc(gsl_multimin_fminimizer_nmsimplex2, N),
    gsl_multimin_fminimizer_free
  );
  assert(minimizer);
  struct FParams {
    Scorer const& sc;
    visual_feedback& fb;
  } f_params = { scorer, feedback };
  struct F {
    static double f(const gsl_vector* x, void* params) {
      Scorer const& scorer = static_cast<FParams*>(params)->sc;
      visual_feedback& feedback = static_cast<FParams*>(params)->fb;
      auto const p = gsl_detail::make_from_gsl_vector<StateType>(x);
      double const score = scorer(p);
      feedback.messagef(boost::format("score(%1%)=%2%") % p % score);
      return score;
    }
  };
  gsl_multimin_function gsl_f = {
    &F::f,
    N,
    &f_params
  };
  std::array<double, N> startArray;
  boost::range::copy(startRange, startArray.begin());
  gsl_vector const startVector = { N, 1, startArray.data(), NULL, 0 };
  std::array<double, N> stepArray;
  boost::range::copy(stepRange, stepArray.begin());
  gsl_vector const stepVector = { N, 1, stepArray.data(), NULL, 0 };
  int status = gsl_multimin_fminimizer_set(
    &*minimizer, &gsl_f, &startVector, &stepVector
  );
  assert(!status);
  do {
    status = gsl_multimin_fminimizer_iterate(&*minimizer);
    assert(!status);
    double const characteristic_size =
      gsl_multimin_fminimizer_size(&*minimizer);
    status = gsl_multimin_test_size(characteristic_size, precision);
  } while (status == GSL_CONTINUE);

  // Turn the resulting gsl_vector back into a fusion::vector
  auto const x = gsl_multimin_fminimizer_x(&*minimizer);
  return std::make_tuple(
    gsl_detail::make_from_gsl_vector<StateType>(x),
    gsl_multimin_fminimizer_minimum(&*minimizer)
  );
}

}

#endif // SUBLIMINAL__FIND_MINIMUM_GSL_HPP

