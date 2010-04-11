#ifndef SUBLIMINAL__PREPIXBUF_HPP
#define SUBLIMINAL__PREPIXBUF_HPP

#include <boost/scoped_ptr.hpp>
#include <boost/thread/barrier.hpp>

namespace subliminal {

class prepixbuf {
  public:
    void create(
      const uint8_t* data,
      bool has_alpha,
      ffmsxx::video_dimensions dims,
      int rowstride,
      Glib::Dispatcher& signal
    ) {
      {
        boost::lock_guard<boost::mutex> scoped_lock(predata_mutex_);
        data_ = data;
        has_alpha_ = has_alpha;
        dims_ = dims;
        rowstride_ = rowstride;
      }
      barrier_.reset(new boost::barrier(2));
      signal();
      barrier_->wait();
      barrier_.reset();
    }

    void make_image(Gtk::Image& image) {
      boost::lock_guard<boost::mutex> scoped_lock(predata_mutex_);
      auto pixbuf = Gdk::Pixbuf::create_from_data(
          data_,
          Gdk::COLORSPACE_RGB,
          has_alpha_,
          8 /*bits per sample*/,
          dims_.width(),
          dims_.height(),
          rowstride_
        );
      image.set(pixbuf);
      barrier_->wait();
    }
  private:
    const uint8_t* data_;
    bool has_alpha_;
    ffmsxx::video_dimensions dims_;
    int rowstride_;
    boost::mutex predata_mutex_;
    boost::scoped_ptr<boost::barrier> barrier_;
};

}

#endif // SUBLIMINAL__PREPIXBUF_HPP

