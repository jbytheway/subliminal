#ifndef SUBLIMINAL__CHUNKIFY_HPP
#define SUBLIMINAL__CHUNKIFY_HPP

#include <vector>

#include <boost/gil/typedefs.hpp>

namespace subliminal {

std::vector<boost::gil::gray8_image_t>
chunkify(boost::gil::gray8c_view_t const&, int range);

}

#endif // SUBLIMINAL__CHUNKIFY_HPP

