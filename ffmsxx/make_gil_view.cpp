#include <ffmsxx/make_gil_view.hpp>

#include <boost/gil/image_view_factory.hpp>
#include <boost/gil/image_view.hpp>

#include <ffmsxx/fatal.hpp>
#include <ffmsxx/pixel_format.hpp>
#include <ffmsxx/video_frame.hpp>
#include <ffmsxx/video_dimensions.hpp>

namespace ffmsxx {

boost::gil::rgb8c_view_t make_gil_view(ffmsxx::video_frame const& frame)
{
  static ffmsxx::pixel_format rgb24("rgb24");
  if (frame.converted_pixel_format() != rgb24) {
    FFMSXX_FATAL("wrong colourspace");
  }
  if (frame.data_stride(0) == 0) {
    FFMSXX_FATAL("no frame data");
  }
  auto dim = frame.scaled_dimensions();
  return boost::gil::interleaved_view(
    dim.width(), dim.height(),
    reinterpret_cast<boost::gil::rgb8_pixel_t const*>(frame.data(0)),
    frame.data_stride(0)
  );
}

}

