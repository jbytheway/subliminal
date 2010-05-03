#ifndef SUBLIMINAL__CHUNKIFY_HPP
#define SUBLIMINAL__CHUNKIFY_HPP

#include <vector>

#include <boost/fusion/include/vector.hpp>
#include <boost/gil/typedefs.hpp>

namespace subliminal {

std::vector<boost::gil::gray8_image_t>
chunkify(
  boost::gil::gray8c_view_t const&,
  boost::fusion::vector<int, int> const& range
);

}

#endif // SUBLIMINAL__CHUNKIFY_HPP

