#ifndef FFMSXX__VIDEO_FRAME_HPP
#define FFMSXX__VIDEO_FRAME_HPP

#include <memory>

#include <ffmsxx/fwd.hpp>

namespace ffmsxx {

class video_frame {
  public:
    video_frame(video_frame&&);
    ~video_frame();

    video_dimensions encoded_dimensions() const;
  private:
    friend class video_source;

    video_frame(const void* frame);

    struct impl;
    std::unique_ptr<impl> impl_;
};

}

#endif // FFMSXX__VIDEO_FRAME_HPP

