#ifndef SUBLIMINAL__FIND_MINIMUM_GSL_HPP
#define SUBLIMINAL__FIND_MINIMUM_GSL_HPP

#include <array>

#include <boost/range/algorithm/copy.hpp>

#include <gsl/gsl_multimin.h>

#include "n_vector.hpp"

namespace subliminal {

template<size_t N, typename Range1, typename Range2, typename Scorer>
typename n_vector<double, N>::type find_minimum_gsl(
  Range1 const& startRange,
  Range2 const& stepRange,
  Scorer const& scorer,
  visual_feedback& feedback
)
{
  assert(boost::size(startRange) == N);
  assert(boost::size(stepRange) == N);
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
      BOOST_MPL_ASSERT_RELATION(N,==,6);
      transform_params const p(
        gsl_vector_get(x, 0),
        gsl_vector_get(x, 1),
        gsl_vector_get(x, 2),
        gsl_vector_get(x, 3),
        gsl_vector_get(x, 4),
        gsl_vector_get(x, 5)
      );
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
    status = gsl_multimin_test_size(characteristic_size, 0.1);
  } while (status == GSL_CONTINUE);

  // Turn the resulting gsl_vector back into a fusion::vector
  auto const x = gsl_multimin_fminimizer_x(&*minimizer);
  typedef typename n_vector<double, N>::type Result;
  BOOST_MPL_ASSERT_RELATION(N,==,6);
  return Result(
    gsl_vector_get(x, 0),
    gsl_vector_get(x, 1),
    gsl_vector_get(x, 2),
    gsl_vector_get(x, 3),
    gsl_vector_get(x, 4),
    gsl_vector_get(x, 5)
  );
}

}

#endif // SUBLIMINAL__FIND_MINIMUM_GSL_HPP

