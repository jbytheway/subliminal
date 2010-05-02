#ifndef SUBLIMINAL__FLOOD_FILL_HPP
#define SUBLIMINAL__FLOOD_FILL_HPP

#include <boost/gil/typedefs.hpp>

namespace subliminal {

void flood_fill(
  boost::gil::gray8c_view_t const& src,
  boost::gil::gray8_view_t const& dst
);

}

#endif // SUBLIMINAL__FLOOD_FILL_HPP

