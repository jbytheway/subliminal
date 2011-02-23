#ifndef SUBLIMINAL__CONGLOMERATE_IMAGE_HPP
#define SUBLIMINAL__CONGLOMERATE_IMAGE_HPP

#include <boost/rational.hpp>

#include "output.hpp"
#include "conglomerate_pixel.hpp"

namespace subliminal {

class conglomerate_image {
  public:
    conglomerate_image(
      int start_frame,
      boost::rational<int64_t> const& start_time,
      boost::gil::rgb8c_view_t const&
    );

    bool consistent_overlap(boost::gil::rgb8c_view_t const&) const;

    void merge(conglomerate_image&&);

    size_t index_of_first_nonempty() const;

    void finalize(
      int frame,
      boost::rational<int64_t> const& time,
      output&
    );
  private:
    int start_frame_;
    boost::rational<int64_t> start_time_;
    boost::gil::point2<ptrdiff_t> dims_;
    std::vector<conglomerate_pixel> pixels_;
};

}

#endif // SUBLIMINAL__CONGLOMERATE_IMAGE_HPP

