#ifndef SUBLIMINAL__CLAMP_HPP
#define SUBLIMINAL__CLAMP_HPP

namespace subliminal {

template<typename T, typename U>
inline T clamp(
  U val,
  T min = std::numeric_limits<T>::min(),
  T max = std::numeric_limits<T>::max()
) {
  if (val < min) return min;
  if (val > max) return max;
  return val;
}

}

#endif // SUBLIMINAL__CLAMP_HPP

