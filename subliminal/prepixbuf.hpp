#ifndef SUBLIMINAL__PREPIXBUF_HPP
#define SUBLIMINAL__PREPIXBUF_HPP

#include <boost/scoped_ptr.hpp>
#include <boost/scoped_array.hpp>
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
      size_t const data_size = rowstride*dims.height();
      boost::scoped_array<uint8_t> old(new uint8_t[data_size]);
      // Need to ensure that old data_ remains valid until image has been
      // reset, which is after barrier at bottom of this function
      swap(old, data_);
      {
        boost::lock_guard<boost::mutex> scoped_lock(predata_mutex_);
        memcpy(&data_[0], data, data_size);
        has_alpha_ = has_alpha;
        dims_ = dims;
        rowstride_ = rowstride;
      }
      barrier_.reset(new boost::barrier(2));
      signal();
      barrier_->wait();
      // If you delete the barrier too soon after waiting on it then sometimes
      // things break; use this mutex to wait a bit for the other thread
      boost::lock_guard<boost::mutex> scoped_lock(barrier_mutex_);
      barrier_.reset();
    }

    void make_image(Gtk::Image& image) {
      boost::lock_guard<boost::mutex> scoped_lock(predata_mutex_);
      auto pixbuf = Gdk::Pixbuf::create_from_data(
          &data_[0],
          Gdk::COLORSPACE_RGB,
          has_alpha_,
          8 /*bits per sample*/,
          dims_.width(),
          dims_.height(),
          rowstride_
        );
      image.set(pixbuf);
      boost::lock_guard<boost::mutex> scoped_lock2(barrier_mutex_);
      barrier_->wait();
    }
  private:
    boost::scoped_array<uint8_t> data_;
    bool has_alpha_;
    ffmsxx::video_dimensions dims_;
    int rowstride_;
    boost::mutex predata_mutex_;
    boost::mutex barrier_mutex_;
    boost::scoped_ptr<boost::barrier> barrier_;
};

}

#endif // SUBLIMINAL__PREPIXBUF_HPP

