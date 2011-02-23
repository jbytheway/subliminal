#include "conglomerate_image.hpp"

#include <numeric>

#include <boost/foreach.hpp>
#include <boost/gil/image.hpp>
#include <boost/gil/image_view.hpp>
#include <boost/spirit/home/phoenix/core.hpp>
#include <boost/spirit/home/phoenix/bind.hpp>
#include <boost/spirit/home/phoenix/operator.hpp>
#include <boost/spirit/home/phoenix/stl.hpp>
#include <boost/lexical_cast.hpp>

#include "delta_luminosity.hpp"
#include "means_cluster.hpp"

namespace subliminal {

namespace px = boost::phoenix;
using namespace px::arg_names;

namespace {

  struct identity {
    template<typename T>
    T operator()(T&& t) {
      return std::forward<T>(t);
    }
  };

  struct max {
    template<typename T>
    T operator()(T t1, T t2) {
      return std::max(std::move(t1), std::move(t2));
    }
  };

  struct max_delta_luminosity_p1 {
    int operator()(
      conglomerate_pixel const& c,
      boost::gil::rgb8_pixel_t const& p
    ) {
      if (p == boost::gil::rgb8_pixel_t(0, 0, 0)) return 0;
      int result = 0;
      half_difference_luminosity hdl;
      BOOST_FOREACH(auto const& option, c.options()) {
        result = std::max(result, abs(hdl(p, option))+1);
      }
      return result;
    }
  };

  struct destructive_merge {
    conglomerate_pixel operator()(
      conglomerate_pixel& p1,
      conglomerate_pixel& p2
    ) {
      p1.merge(std::move(p2));
      return std::move(p1);
    }
  };

  struct extract_representative_colour {
    typedef boost::gil::rgb8_pixel_t Pixel;
    Pixel operator()(conglomerate_pixel const& c) {
      if (c.options().empty()) return Pixel(0, 0, 0);
      return c.options().front();
    }
  };

  struct replace_black_with {
    typedef boost::gil::rgb8_pixel_t Pixel;
    replace_black_with(Pixel const& replacement) : replacement_(replacement) {}
    void operator()(Pixel& p) {
      if (p == Pixel(0, 0, 0)) p = replacement_;
    }
    Pixel replacement_;
  };

  struct pixel_to_point {
    Point<float> operator()(boost::gil::rgb8_pixel_t const& p) const {
      return Point<float>(p[0], p[1], p[2]);
    }
  };

  struct point_to_pixel {
    boost::gil::rgb8_pixel_t operator()(Point<float> const& p) const {
      return boost::gil::rgb8_pixel_t(p[0], p[1], p[2]);
    }
  };

  struct distance_from {
    typedef boost::gil::rgb8_pixel_t Pixel;
    distance_from(Pixel const& centre) : centre_(pixel_to_point()(centre)) {}
    int operator()(Pixel const& p) {
      if (p == Pixel(0, 0, 0)) return 255;
      double distance = (pixel_to_point()(p) - centre_).length();
      return std::min(distance*2, 255.0);
    }
    Point<float> centre_;
  };
}

conglomerate_image::conglomerate_image(
  int start_frame,
  boost::rational<int64_t> const& start_time,
  boost::gil::rgb8c_view_t const& v
) :
  start_frame_(start_frame),
  start_time_(start_time),
  dims_(v.dimensions())
{
  pixels_.reserve(v.width() * v.height());
  std::transform(
    v.begin(), v.end(), std::back_inserter(pixels_), identity()
  );
}

bool conglomerate_image::consistent_overlap(
  boost::gil::rgb8c_view_t const& v
) const
{
  assert(pixels_.size() == size_t(v.width() * v.height()));
  std::vector<int> delta_lums;
  auto max_delta = std::transform(
    pixels_.begin(), pixels_.end(),
    v.begin(),
    std::back_inserter(delta_lums),
    max_delta_luminosity_p1()
  );
  auto new_last = std::remove(delta_lums.begin(), delta_lums.end(), 0);
  delta_lums.erase(new_last, delta_lums.end());
  if (delta_lums.size() < 10) {
    // there was insufficient overlap
    return false;
  }
  auto nth_it = delta_lums.begin()+size_t(delta_lums.size()*0.9);
  std::nth_element(delta_lums.begin(), nth_it, delta_lums.end());
  if (*nth_it < 20) {
    // there was consistent overlap
    return true;
  } else {
    // there was inconsistent overlap
    return false;
  }
}

void conglomerate_image::merge(conglomerate_image&& other)
{
  assert(pixels_.size() == other.pixels_.size());
  start_frame_ = std::min(start_frame_, other.start_frame_);
  start_time_ = std::min(start_time_, other.start_time_);
  std::transform(
    pixels_.begin(), pixels_.end(),
    other.pixels_.begin(),
    pixels_.begin(),
    destructive_merge()
  );
}

size_t conglomerate_image::index_of_first_nonempty() const
{
  auto const it = std::find_if(
    pixels_.begin(), pixels_.end(),
    !px::bind(&conglomerate_pixel::empty, arg1)
  );
  return it - pixels_.begin();
}

void conglomerate_image::finalize(
  int frame,
  boost::rational<int64_t> const& time,
  output& out
)
{
  if (frame - start_frame_ <= 10) {
    // Only a few frames long; probably junk; ignore it
    return;
  }
  size_t num_pix = std::count_if(
    pixels_.begin(), pixels_.end(),
    !px::bind(&conglomerate_pixel::empty, arg1)
  );
  if (num_pix < 100) {
    // Only a few pixels large; probably junk; ignore it
    return;
  }
  typedef boost::gil::rgb8_pixel_t Pixel;
  std::vector<Pixel> all_pixels;
  std::for_each(
    pixels_.begin(), pixels_.end(),
    px::insert(
      px::ref(all_pixels), px::end(px::ref(all_pixels)),
      px::begin(px::bind(&conglomerate_pixel::options, arg1)),
      px::end(px::bind(&conglomerate_pixel::options, arg1))
    )
  );
  std::vector<Point<float>> all_points;
  std::transform(
    all_pixels.begin(), all_pixels.end(),
    std::back_inserter(all_points),
    pixel_to_point()
  );
  all_pixels.clear(); // Free up possibly-significant memory
  auto means = means_cluster(all_points, 2);
  boost::gil::rgb8_image_t flattened(dims_);
  std::transform(
    pixels_.begin(), pixels_.end(),
    view(flattened).begin(),
    extract_representative_colour()
  );
  std::vector<boost::gil::gray8_image_t> finals;
  BOOST_FOREACH(auto const& mean, means) {
    finals.emplace_back(dims_);
    transform_pixels(
      const_view(flattened), view(finals.back()),
      distance_from(point_to_pixel()(mean))
    );
  }
  auto note = "";
  out.save(start_time_, time, finals, note);
}

}

