#ifndef FFMSXX__VIDEO_FRAME_HPP
#define FFMSXX__VIDEO_FRAME_HPP

#include <cstdint>
#include <memory>

#include <ffmsxx/fwd.hpp>

struct FFMS_VideoSource;

namespace ffmsxx {

class video_frame {
  public:
    video_frame(video_frame&&);
    ~video_frame();

    video_dimensions encoded_dimensions() const;
    video_dimensions scaled_dimensions() const;
    pixel_format converted_pixel_format() const;

    uint8_t const* data(int plane) const;
    int data_stride(int plane) const;
  private:
    friend class video_source;

    video_frame(FFMS_VideoSource*, int n);

    struct impl;
    std::unique_ptr<impl> impl_;
};

}

#endif // FFMSXX__VIDEO_FRAME_HPP

