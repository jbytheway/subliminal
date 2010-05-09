#ifndef SUBLIMINAL__MEANS_CLUSTER_HPP
#define SUBLIMINAL__MEANS_CLUSTER_HPP

#include <vector>

#include "point.hpp"

namespace subliminal {

std::vector<Point<float>>
means_cluster(std::vector<Point<float>> const& data, size_t const num);

}

#endif // SUBLIMINAL__MEANS_CLUSTER_HPP

