#ifndef SUBLIMINAL__CONSTRUCT_HPP
#define SUBLIMINAL__CONSTRUCT_HPP

namespace subliminal {

template<typename T>
struct construct {
  typedef T result_type;

  template<typename... U>
  T operator()(U&&... u) {
    return T(std::forward<U>(u)...);
  }
};

}

#endif // SUBLIMINAL__CONSTRUCT_HPP

