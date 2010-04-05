#ifndef FFMSXX__TRACK_MASK_HPP
#define FFMSXX__TRACK_MASK_HPP

namespace ffmsxx {

class track_mask {
  public:
    int raw;
};

static const track_mask no_tracks = { 0 };

}

#endif // FFMSXX__TRACK_MASK_HPP

