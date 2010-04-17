#include "gtk_feedback.hpp"

#include <boost/thread/thread.hpp>
#include <boost/filesystem/operations.hpp>

#include <gtkmm.h>
#include <libglademm.h>

#include <ffmsxx/video_dimensions.hpp>
#include <ffmsxx/video_frame.hpp>
#include <ffmsxx/pixel_format.hpp>

#include "fatal.hpp"
#include "prepixbuf.hpp"

namespace subliminal {

struct gtk_feedback::impl {
  impl(boost::filesystem::path dataPath, std::ostream& o) :
    out(o),
    thread_obj(std::move(dataPath), out),
    gtk_thread(boost::ref(thread_obj))
  {}

  ~impl()
  {
    thread_obj.end();
    gtk_thread.join();
  }

  struct ThreadObj : boost::noncopyable {
    static const int num_images = 3;

    ThreadObj(boost::filesystem::path dataPath, std::ostream& o) :
      initted{false},
      shutting_down{false},
      data_path(std::move(dataPath)),
      out(o),
      rgb24("rgb24"),
      window{NULL},
      images{ NULL, NULL, NULL }
    {}

    void operator()() {
      /** \bug Don't pass command line args through to here */
      int argc = 0;
      char* args[] = { NULL };
      char** argv = args;
      Gtk::Main kit(argc, argv);
      auto gladePath = data_path/"subliminal.glade";
      if (!boost::filesystem::exists(gladePath)) {
        SUBLIMINAL_FATAL("couldn't find " << gladePath);
      }
      Glib::RefPtr<Gnome::Glade::Xml> xml =
        Gnome::Glade::Xml::create(gladePath.file_string());
      xml->get_widget("MainWindow", window);
      BOOST_MPL_ASSERT_RELATION(num_images, ==, 3);
      xml->get_widget("Image0", images[0]);
      xml->get_widget("Image1", images[1]);
      xml->get_widget("Image2", images[2]);
      assert(images[0]);
      assert(images[1]);
      assert(images[2]);
      for (int i=0; i<num_images; ++i) {
        pixbuf_show_signals[i].connect(
          sigc::bind(sigc::mem_fun(this, &ThreadObj::update_image), i)
        );
      }
      end_signal.connect(
        sigc::mem_fun(this, &ThreadObj::close_window)
      );
      initted = true;
      window->show();
      Gtk::Main::run();
      out << "GTK thread completed" << std::endl;
    }

    void show(boost::gil::rgb8c_view_t const& view, int image) {
      assert(image >= 0);
      assert(image < num_images);
      /** \bug Maybe should use condition variable instead of spin-locking?
      */
      while (!initted) {}
      if (shutting_down || !window->get_visible()) return;
      pixbufs[image].create(
        reinterpret_cast<uint8_t const*>(&*view.row_begin(0)),
        false /*has_alpha*/,
        ffmsxx::video_dimensions(view.width(), view.height()),
        reinterpret_cast<uint8_t const*>(&*view.row_begin(1))-
          reinterpret_cast<uint8_t const*>(&*view.row_begin(0)),
        pixbuf_show_signals[image]
      );
    }

    void update_image(int image) {
      pixbufs[image].make_image(*images[image]);
    }

    void end() {
      /** \bug Maybe should use condition variable instead of spin-locking? */
      while (!initted) {}
      shutting_down = true;
      end_signal();
    }

    void close_window() {
      Gtk::Main::quit();
    }

    bool initted;
    bool shutting_down;
    boost::filesystem::path data_path;
    std::ostream& out;
    ffmsxx::pixel_format rgb24;
    Gtk::Window* window;
    Gtk::Image* images[num_images];
    prepixbuf pixbufs[num_images];
    Glib::Dispatcher pixbuf_show_signals[num_images];
    Glib::Dispatcher end_signal;
  };

  std::ostream& out;
  ThreadObj thread_obj;
  boost::thread gtk_thread;
};

gtk_feedback::gtk_feedback(
  boost::filesystem::path dataPath, std::ostream& out
) :
  text_feedback(out),
  impl_(new impl(std::move(dataPath), out))
{
}

gtk_feedback::~gtk_feedback() = default;

void gtk_feedback::show_rgb(boost::gil::rgb8c_view_t const& view, int image)
{
  impl_->thread_obj.show(view, image);
}

}

