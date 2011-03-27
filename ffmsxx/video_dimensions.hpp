#ifndef FFMSXX__VIDEO_DIMENSIONS_HPP
#define FFMSXX__VIDEO_DIMENSIONS_HPP

#include <cassert>
#include <iosfwd>

#include <boost/gil/utilities.hpp>

namespace ffmsxx {

class video_dimensions {
  public:
    video_dimensions() : width_{}, height_{} {}
    video_dimensions(int width, int height) : width_(width), height_(height) {
      assert(width > 0);
      assert(height > 0);
    }
    int width() const { assert(width_ > 0); return width_; }
    int height() const { assert(height_ > 0); return height_; }

    boost::gil::point2<ptrdiff_t> as_point() const {
      return {width_, height_};
    }
  private:
    int width_;
    int height_;
};

std::ostream& operator<<(std::ostream&, video_dimensions const&);

}

#endif // FFMSXX__VIDEO_DIMENSIONS_HPP

