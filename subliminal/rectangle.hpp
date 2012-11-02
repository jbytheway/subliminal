#ifndef SUBLIMINAL__RECTANGLE_HPP
#define SUBLIMINAL__RECTANGLE_HPP

#include <boost/gil/utilities.hpp>

namespace subliminal {

template<typename T>
class rectangle {
  public:
    typedef boost::gil::point2<T> point;

    template<typename U>
    rectangle(
      boost::gil::point2<U> const& tl,
      boost::gil::point2<U> const& br
    ) :
      top_left_(tl.x, tl.y),
      bottom_right_(br.x, br.y)
    {
      assert(tl.x <= br.x);
      assert(tl.y <= br.y);
    }

    point top_left() const { return top_left_; }
    point bottom_right() const { return bottom_right_; }
    point dimensions() const { return bottom_right_ - top_left_; }
  private:
    point top_left_;
    point bottom_right_;
};

template<typename T>
std::ostream& operator<<(std::ostream& o, rectangle<T> const& r) {
  auto const tl = r.top_left();
  auto const br = r.bottom_right();
  o << "[(" << tl.x << ", " << tl.y << "), (" << br.x << ", " << br.y << ")]";
  return o;
}

}

#endif // SUBLIMINAL__RECTANGLE_HPP

