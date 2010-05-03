#include "chunkify.hpp"

#include <stack>

#include <boost/foreach.hpp>
#include <boost/gil/image.hpp>
#include <boost/gil/image_view.hpp>

namespace subliminal {

std::vector<boost::gil::gray8_image_t>
chunkify(boost::gil::gray8c_view_t const& v, int range)
{
  assert(range > 0);
  typedef boost::gil::gray8c_view_t::point_t Point;
  typedef boost::gil::gray8c_view_t::locator Locator;

  std::vector<Point> diffs;
  for (int x = -range; x <= range; ++x) {
    for (int y = abs(x)-range; y <= range-abs(x); ++y) {
      if (x == 0 && y == 0) continue;
      diffs.push_back(Point(x, y));
    }
  }

  // Copy the pixels (we will be eliminating them as we chunk stuff up)
  auto const dims = v.dimensions();
  boost::gil::gray8_image_t copy(dims);
  auto const copy_view = view(copy);
  copy_pixels(v, copy_view);

  std::vector<boost::gil::gray8_image_t> result;

  // Big outer loop searching for new pixels
  for (ptrdiff_t y=0; y<copy.height(); ++y) {
    for (ptrdiff_t x=0; x<copy.width(); ++x) {
      Point start(x, y);
      if (*copy_view.xy_at(start)) {
        // We've found the start of a new chunk
        boost::gil::gray8_image_t chunk(dims);
        auto const chunk_view = view(chunk);
        fill_pixels(chunk_view, 0);
        std::stack<Point> pending;
        pending.push(start);

        while (!pending.empty()) {
          auto next = pending.top();
          pending.pop();
          BOOST_FOREACH(auto const& diff, diffs) {
            auto neighbour = next+diff;
            if (neighbour.x < 0 || neighbour.x >= dims.x ||
              neighbour.y < 0 || neighbour.y >= dims.y) {
              continue;
            }
            if (chunk_view.xy_at(neighbour)[0]) continue;
            if (copy_view.xy_at(neighbour)[0]) {
              chunk_view.xy_at(neighbour)[0] = 255;
              copy_view.xy_at(neighbour)[0] = 0;
              pending.push(neighbour);
            }
          }
        }

        result.push_back(std::move(chunk));
      }
    }
  }

  return result;
}

}

