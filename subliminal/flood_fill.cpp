#include "flood_fill.hpp"

#include <stack>

#include <boost/foreach.hpp>
#include <boost/gil/image_view.hpp>

namespace subliminal {

void flood_fill(
  boost::gil::gray8c_view_t const& src,
  boost::gil::gray8_view_t const& dst
)
{
  auto dims = src.dimensions();
  assert(dst.dimensions() == dims);
  typedef boost::gil::gray8c_view_t::point_t Point;
  Point start(0, 0);
  Point diffs[4] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
  auto colour = src.xy_at(start);
  std::stack<Point> pending;
  pending.push(start);

  while (!pending.empty()) {
    Point next = pending.top();
    pending.pop();
    BOOST_FOREACH(auto const& diff, diffs) {
      auto neighbour = next+diff;
      if (neighbour.x < 0 || neighbour.x >= dims.x ||
        neighbour.y < 0 || neighbour.y >= dims.y) {
        continue;
      }
      if (dst.xy_at(neighbour)[0]) continue;
      if (src.xy_at(neighbour)[0] == colour[0]) {
        dst.xy_at(neighbour)[0] = 255;
        pending.push(neighbour);
      }
    }
  }
}

}

