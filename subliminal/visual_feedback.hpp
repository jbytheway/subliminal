#ifndef SUBLIMINAL__VISUAL_FEEDBACK_HPP
#define SUBLIMINAL__VISUAL_FEEDBACK_HPP

#include <boost/noncopyable.hpp>
#include <boost/mpl/vector_c.hpp>
#include <boost/gil/image_view_factory.hpp>
#include <boost/gil/image_view.hpp>
#include <boost/gil/image.hpp>
#include <boost/format.hpp>

#include <ffmsxx/fwd.hpp>
#include <ffmsxx/make_gil_view.hpp>

namespace subliminal {

class visual_feedback : boost::noncopyable {
  public:
    virtual ~visual_feedback() = 0;
    virtual void message(std::string const&) = 0;
    virtual void messagef(boost::format const& f) { message(f.str()); }
    virtual void show_rgb(boost::gil::rgb8c_view_t const&, int image) = 0;
    virtual void progress(int pos, int outof) = 0;

    void show(ffmsxx::video_frame const& frame, int image) {
      show_rgb(make_gil_view(frame), image);
    }

    template<typename ImageView>
    void show(ImageView const& to_show, int image) {
      boost::gil::rgb8_image_t copy(to_show.dimensions());
      copy_pixels(
        boost::gil::color_converted_view<boost::gil::rgb8_pixel_t>(to_show),
        view(copy)
      );
      show_rgb(const_view(copy), image);
    }
  protected:
    visual_feedback() {};
};

}

#endif // SUBLIMINAL__VISUAL_FEEDBACK_HPP

