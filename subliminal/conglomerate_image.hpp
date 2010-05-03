#ifndef SUBLIMINAL__CONGLOMERATE_IMAGE_HPP
#define SUBLIMINAL__CONGLOMERATE_IMAGE_HPP

#include <boost/rational.hpp>

#include "conglomerate_pixel.hpp"

namespace subliminal {

class conglomerate_image {
  public:
    conglomerate_image(boost::gil::rgb8c_view_t const&);

    bool consistent_overlap(boost::gil::rgb8c_view_t const&) const;

    void merge(conglomerate_image&&);

    void finalize(boost::rational<int64_t> const& time);
  private:
    std::vector<conglomerate_pixel> pixels_;
};

}

#endif // SUBLIMINAL__CONGLOMERATE_IMAGE_HPP

