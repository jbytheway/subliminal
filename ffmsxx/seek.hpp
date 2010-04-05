#ifndef FFMSXX__SEEK_HPP
#define FFMSXX__SEEK_HPP

namespace ffmsxx {

enum class seek {
  normal,
};

int seek_to_raw(seek const);

}

#endif // FFMSXX__SEEK_HPP

