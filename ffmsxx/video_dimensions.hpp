#ifndef FFMSXX__VIDEO_DIMENSIONS_HPP
#define FFMSXX__VIDEO_DIMENSIONS_HPP

#include <cassert>

namespace ffmsxx {

class video_dimensions {
  public:
    video_dimensions(int width, int height) : width_(width), height_(height) {
      assert(width > 0);
      assert(height > 0);
    }
    int width() const { return width_; }
    int height() const { return height_; }
  private:
    int width_;
    int height_;
};

}

#endif // FFMSXX__VIDEO_DIMENSIONS_HPP

