#ifndef SUBLIMINAL__CONGLOMERATE_PIXEL_HPP
#define SUBLIMINAL__CONGLOMERATE_PIXEL_HPP

#include <vector>

#include <boost/gil/typedefs.hpp>

namespace subliminal {

class conglomerate_pixel {
  public:
    typedef boost::gil::rgb8_pixel_t Pixel;
    typedef std::vector<Pixel> Options;

    conglomerate_pixel(Pixel const& p) {
      if (p != Pixel(0, 0, 0)) options_.push_back(p);
    }

    bool empty() const { return options_.empty(); }
    Options const& options() const { return options_; }
    void merge(conglomerate_pixel&& other) {
      if (empty()) {
        options_ = std::move(other.options_);
        return;
      }
      options_.insert(
        options_.end(), other.options_.begin(), other.options_.end()
      );
    }
  private:
    Options options_;
};

}

#endif // SUBLIMINAL__CONGLOMERATE_PIXEL_HPP

