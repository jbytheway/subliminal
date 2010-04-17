#ifndef FFMSXX__MAKE_GIL_VIEW_HPP
#define FFMSXX__MAKE_GIL_VIEW_HPP

#include <boost/gil/typedefs.hpp>

#include <ffmsxx/fwd.hpp>

namespace ffmsxx {

boost::gil::rgb8c_view_t make_gil_view(ffmsxx::video_frame const& frame);

}

#endif // FFMSXX__MAKE_GIL_VIEW_HPP

