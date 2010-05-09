#include "output.hpp"

#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/gil/extension/io/png_io.hpp>

#include "fatal.hpp"

namespace subliminal {

namespace {

  double round_time(boost::rational<int64_t> const& t)
  {
    // t is in milliseconds; round down to centisecond
    return 0.01*floor(boost::rational_cast<double>(t/10));
  }

}

output::output(boost::filesystem::path const& root) :
  root_(root),
  txt_out_(root),
  index_{}
{
  if (!txt_out_.is_open()) {
    SUBLIMINAL_FATAL("couldn't open output file");
  }
}

void output::save(
  boost::rational<int64_t> const& begin_time,
  boost::rational<int64_t> const& end_time,
  std::vector<boost::gil::gray8_image_t> const& images,
  std::string const& other
)
{
  txt_out_ << round_time(begin_time) << ' ' << round_time(end_time);
  BOOST_FOREACH(auto const& image, images) {
    auto new_index = index_++;
    std::string uniq = (boost::format(".%04d") % new_index).str();
    auto image_name = root_.file_string()+uniq+".png";
    boost::gil::png_write_view(image_name, const_view(image));
    txt_out_ << ' ' << image_name;
  }
  if (!other.empty()) txt_out_ << ' ' << other;
  txt_out_ << std::endl;
}

}

