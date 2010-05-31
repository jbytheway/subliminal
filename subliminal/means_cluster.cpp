#include "means_cluster.hpp"

#include <algorithm>
#include <random>

#include <boost/foreach.hpp>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/spirit/home/phoenix/core/argument.hpp>
#include <boost/spirit/home/phoenix/bind.hpp>
#include <boost/spirit/home/phoenix/operator.hpp>

namespace subliminal {

std::vector<Point<float>>
means_cluster(std::vector<Point<float>> const& data, size_t const num)
{
  if (data.size() < num) {
    SUBLIMINAL_FATAL("data too small");
  }

  typedef Point<float> Pt;
  typedef std::vector<Pt> Pts;

  Pts old_means;
  typedef std::mt19937 Engine;
  Engine engine;
  typedef std::variate_generator<Engine*, std::uniform_int<size_t>> Rng;
  Rng rng(&engine, std::uniform_int<size_t>(0, data.size()));

  size_t trials = 0;
  while (old_means.size() < num && trials < num*3) {
    Pt candidate = data[rng()];
    if (std::find(old_means.begin(), old_means.end(), candidate) !=
      old_means.end()) {
      continue;
    }
    old_means.push_back(candidate);
  }

  trials = 0;
  size_t const max_trials = 1000;
  while (trials < max_trials) {
    ++trials;
    using namespace boost::accumulators;
    typedef accumulator_set<Pt, stats<tag::mean> > Acc;
    std::vector<Acc> accumulators(num);

    BOOST_FOREACH(Pt const& datum, data) {
      std::vector<double> distances;
      using boost::phoenix::arg_names::arg1;
      std::transform(
        old_means.begin(), old_means.end(),
        std::back_inserter(distances),
        boost::phoenix::bind(&Pt::squareLength, arg1 - datum)
      );
      auto closest_mean = std::min_element(distances.begin(), distances.end());
      accumulators[closest_mean-distances.begin()](datum);
    }

    Pts new_means;
    BOOST_FOREACH(auto const& acc, accumulators) {
      new_means.push_back(mean(acc));
    }

    if (new_means == old_means) break;
    old_means = std::move(new_means);
  }

  if (trials == max_trials) {
    std::fprintf(stderr, "Warning: clustering algorithm failed to converge\n");
  }

  return old_means;
}

}

