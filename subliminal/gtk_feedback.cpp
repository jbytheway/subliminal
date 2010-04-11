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
    gtk_thread.join();
  }

  struct ThreadObj : boost::noncopyable {
    ThreadObj(boost::filesystem::path dataPath, std::ostream& o) :
      initted{false},
      data_path(std::move(dataPath)),
      out(o),
      image1{}
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
      Gtk::Window* window = NULL;
      xml->get_widget("MainWindow", window);
      xml->get_widget("Image1", image1);
      assert(image1);
      pixbuf1_show_signal.connect(
        sigc::mem_fun(this, &ThreadObj::update_image)
      );
      end_signal.connect(
        sigc::mem_fun(this, &ThreadObj::close_window)
      );
      initted = true;
      Gtk::Main::run(*window);
      out << "GTK thread completed" << std::endl;
    }

    void show(ffmsxx::video_frame const& frame) {
      if (frame.converted_pixel_format() != ffmsxx::pixel_format("rgb24")) {
        SUBLIMINAL_FATAL("wrong colourspace");
      }
      if (frame.data_stride(0) == 0) {
        SUBLIMINAL_FATAL("no frame data");
      }
      /** \bug Maybe should use condition variable instead of spin-locking?
      */
      while (!initted) {}
      pixbuf1.create(
        frame.data(0),
        false /*has_alpha*/,
        frame.scaled_dimensions(),
        frame.data_stride(0),
        pixbuf1_show_signal
      );
    }

    void update_image() {
      out << "updating image\n";
      pixbuf1.make_image(*image1);
    }

    void end() {
      /** \bug Maybe should use condition variable instead of spin-locking? */
      while (!initted) {}
      end_signal();
    }

    void close_window() {
      Gtk::Main::quit();
    }

    bool initted;
    boost::filesystem::path data_path;
    std::ostream& out;
    Gtk::Image* image1;
    prepixbuf pixbuf1;
    Glib::Dispatcher pixbuf1_show_signal;
    Glib::Dispatcher end_signal;
  };

  std::ostream& out;
  ThreadObj thread_obj;
  boost::thread gtk_thread;
};

gtk_feedback::gtk_feedback(
  boost::filesystem::path dataPath, std::ostream& out
) :
  impl_(new impl(std::move(dataPath), out))
{
}

gtk_feedback::~gtk_feedback() = default;

void gtk_feedback::show(ffmsxx::video_frame const& frame)
{
  impl_->thread_obj.show(frame);
}

void gtk_feedback::end()
{
  impl_->thread_obj.end();
}

}

