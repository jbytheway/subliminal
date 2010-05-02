#ifndef SUBLIMINAL__OPTIMIZE_HPP
#define SUBLIMINAL__OPTIMIZE_HPP

#include <random>

#include <boost/foreach.hpp>
#include <boost/random/random_number_generator.hpp>

/** \bug Having this include here is a bit of a hack */
#include <boost/fusion/include/io.hpp>

namespace subliminal {

template<typename StateSpace, typename Scorer>
typename StateSpace::state_type optimize(
  StateSpace const& state_space,
  Scorer const& scorer,
  visual_feedback& feedback
)
{
  typedef std::mt19937 RandomEngine;
  RandomEngine random_engine;

  typedef typename StateSpace::state_type StateType;
  StateType current = state_space.initial();
  auto current_score = scorer(current);
  feedback.messagef(boost::format("score(%1%)=%2%") % current % current_score);

  bool at_maximum;
  do {
    auto neighbours = state_space.neighbours(current);
    boost::random_number_generator<RandomEngine> rng(random_engine);
    std::random_shuffle(neighbours.begin(), neighbours.end(), rng);
    at_maximum = true;
    BOOST_FOREACH(auto const& neighbour, neighbours) {
      auto const neighbour_score = scorer(neighbour);
      feedback.messagef(
        boost::format("score(%1%)=%2%") % neighbour % neighbour_score
      );
      if (neighbour_score > current_score) {
        feedback.messagef(boost::format("moving to %1%") % neighbour);
        at_maximum = false;
        current = neighbour;
        current_score = neighbour_score;
        break;
      }
    }
  } while (!at_maximum);

  return current;
}

}

#endif // SUBLIMINAL__OPTIMIZE_HPP

