#include "gtk_feedback.hpp"

#include <boost/thread/thread.hpp>
#include <boost/filesystem/operations.hpp>

#include <gtkmm.h>
#include <libglademm.h>

#include <ffmsxx/video_dimensions.hpp>

#include "fatal.hpp"

namespace subliminal {

struct gtk_feedback::impl {
  impl(boost::filesystem::path dataPath, std::ostream& o) :
    out(o),
    thread_obj(std::move(dataPath), out),
    gtk_thread(thread_obj)
  {}

  ~impl()
  {
    gtk_thread.join();
  }

  struct ThreadObj {
    ThreadObj(boost::filesystem::path dataPath, std::ostream& o) :
      data_path(std::move(dataPath)),
      out(o)
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
      Gtk::Main::run(*window);
      out << "GTK thread completed" << std::endl;
    }

    boost::filesystem::path data_path;
    std::ostream& out;
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

void gtk_feedback::set_dimensions(ffmsxx::video_dimensions const& dims)
{
  impl_->out << "Set dimensions to " << dims << std::endl;
}

void gtk_feedback::show(ffmsxx::video_frame const&)
{
}

}

