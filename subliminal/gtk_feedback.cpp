#include "gtk_feedback.hpp"

#include <boost/thread/thread.hpp>
#include <boost/filesystem/operations.hpp>

#include <gtkmm.h>
#include <libglademm.h>

#include <ffmsxx/video_dimensions.hpp>
#include <ffmsxx/video_frame.hpp>
#include <ffmsxx/pixel_format.hpp>

#include "fatal.hpp"

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
      initted = true;
      Gtk::Main::run(*window);
      out << "GTK thread completed" << std::endl;
    }

    void show(ffmsxx::video_frame const& frame)
    {
      {
        boost::lock_guard<boost::mutex> scoped_lock(pixbuf1_mutex);
        if (frame.converted_pixel_format() != ffmsxx::pixel_format("rgb24")) {
          SUBLIMINAL_FATAL("wrong colourspace");
        }
        if (frame.data_stride(0) == 0) {
          SUBLIMINAL_FATAL("no frame data");
        }
        /** \bug Maybe should use condition variable instead of spin-locking?
         */
        while (!initted) {}
        pixbuf1 = Gdk::Pixbuf::create_from_data(
          frame.data(0),
          Gdk::COLORSPACE_RGB,
          false /*has_alpha*/,
          8 /*bits per sample*/,
          frame.scaled_dimensions().width() /*width*/,
          frame.scaled_dimensions().height() /*height*/,
          frame.data_stride(0) /*stride*/
        );
      }
      pixbuf1_show_signal();
    }

    void update_image()
    {
      out << "updating image\n";
      boost::lock_guard<boost::mutex> scoped_lock(pixbuf1_mutex);
      image1->set(pixbuf1);
    }

    bool initted;
    boost::filesystem::path data_path;
    std::ostream& out;
    Gtk::Image* image1;
    Glib::RefPtr<Gdk::Pixbuf> pixbuf1;
    boost::mutex pixbuf1_mutex;
    Glib::Dispatcher pixbuf1_show_signal;
  };

  void show(ffmsxx::video_frame const& frame)
  {
    thread_obj.show(frame);
  }

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

void gtk_feedback::set_dimensions(ffmsxx::video_dimensions const& dims)
{
  impl_->out << "Set dimensions to " << dims << std::endl;
}

void gtk_feedback::show(ffmsxx::video_frame const& frame)
{
  impl_->show(frame);
}

}

