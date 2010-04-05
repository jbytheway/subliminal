#ifndef FFMSXX__RESIZER_HPP
#define FFMSXX__RESIZER_HPP

namespace ffmsxx {

enum class resizer {
  bicubic,
};

int resizer_to_raw(resizer const);

}

#endif // FFMSXX__RESIZER_HPP
