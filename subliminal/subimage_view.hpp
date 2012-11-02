#ifndef SUBLIMINAL__SUBIMAGE_VIEW_HPP
#define SUBLIMINAL__SUBIMAGE_VIEW_HPP

#include "rectangle.hpp"

namespace subliminal {

template<typename View>
inline View subimage_view(View const& v, rectangle<ptrdiff_t> const& r) {
  return subimage_view(v, r.top_left(), r.dimensions());
}

}

#endif // SUBLIMINAL__SUBIMAGE_VIEW_HPP

