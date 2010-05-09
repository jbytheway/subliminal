#ifndef SUBLIMINAL__POINT_HPP
#define SUBLIMINAL__POINT_HPP

#include <cmath>

#include "fatal.hpp"

namespace subliminal {

template<typename T>
class Point {
  public:
    typedef T element_type;

    T x;
    T y;
    T z;

    Point(): x(0), y(0), z(0) {}
    Point(T a, T b, T c): x(a), y(b), z(c) {}
    /** \brief Simple copy constructor for the sake of swig's sanity */
    Point(const Point<T>& p) : x(p.x), y(p.y), z(p.z) {}

    template <typename U>
    Point(const Point<U>& p) : x(T(p.x)), y(T(p.y)), z(T(p.z)) {}

    ~Point() {}

    inline T& operator[](const int index) {
      switch (index) {
        case 0:
          return x;
        case 1:
          return y;
        case 2:
          return z;
        default:
          SUBLIMINAL_FATAL("index " << index << " out of bounds");
      }
    }

    inline const T& operator[](const int index) const {
      switch (index) {
        case 0:
          return x;
        case 1:
          return y;
        case 2:
          return z;
        default:
          SUBLIMINAL_FATAL("index " << index << " out of bounds");
      }
    }

    template <typename U>
    inline bool operator==(const Point<U>& right) const {
      return x==right.x && y==right.y && z==right.z;
    }

    template <typename U>
    inline bool operator!=(const Point<U>& right) const {
      return x!=right.x || y!=right.y || z!=right.z;
    }

    /** \name Order operators
     *
     * \note These use the product order.  This is a partial
     * order only, not a total order (so don't use Point<T>s in an
     * ordered container) */
    //@{
    template <typename U>
    inline bool operator<=(const Point<U>& right) const {
      return x<=right.x && y<=right.y && z<=right.z;
    }

    template <typename U>
    inline bool operator<(const Point<U>& right) const {
      return (*this <= right) && (*this != right);
    }
    
    template <typename U>
    inline bool operator>=(const Point<U>& right) const {
      return x>=right.x && y>=right.y && z>=right.z;
    }

    template <typename U>
    inline bool operator>(const Point<U>& right) const {
      return (*this >= right) && (*this != right);
    }
    //@}

    /** \name Arithmetic operators */
    //@{
    inline Point<T> operator-() const {
      Point <T> result(-x,-y,-z);
      return result;
    }

    template <typename U>
    inline Point<T> operator+(const Point<U>& right) const {
      Point<T> result(x+right.x,y+right.y,z+right.z);
      return result;
    }

    template <typename U>
    inline void operator+=(const Point<U>& right) {
      x += right.x;
      y += right.y;
      z += right.z;
    }

    template <typename U>
    inline Point<T> operator-(const Point<U>& right) const {
      Point<T> result(x-right.x,y-right.y,z-right.z);
      return result;
    }

    template <typename U>
    inline void operator-=(const Point<U>& right) {
      x -= right.x;
      y -= right.y;
      z -= right.z;
    }

    inline Point<T> operator*(const T scale) const {
      return Point<T>(x*scale, y*scale, z*scale);
    }

    template <typename U>
    inline void operator*=(const U right) {
      x *= right;
      y *= right;
      z *= right;
    }

    inline Point<T>& operator/=(const T scale) {
      x /= scale; y /= scale; z /= scale;
      return *this;
    }

    inline Point<T> operator/(const T scale) const {
      return Point<T>(*this) /= scale;
    }
    //@}

    inline double squareLength(void) const {
      return double(x)*x + double(y)*y + double(z)*z;
    }

    inline double innerProduct(const Point<T>& right) const {
      return double(x)*right.x + double(y)*right.y + double(z)*right.z;
    }

    inline double length(void) const {
      return std::sqrt(static_cast<double>(squareLength()));
    }

    inline bool isZero(void) const {
      return x==T(0) && y==T(0) && z==T(0);
    }

    inline void zero(void) {
      x=T(0);
      y=T(0);
      z=T(0);
    }
};

template<typename T>
inline std::ostream& operator<<(std::ostream& out, const Point<T>& p) {
  out << "(" << p.x << ", " << p.y << ", " << p.z << ")";
  return out;
}

}

#endif // SUBLIMINAL__POINT_HPP

