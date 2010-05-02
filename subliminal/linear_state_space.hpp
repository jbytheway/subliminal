#ifndef SUBLIMINAL__LINEAR_STATE_SPACE_HPP
#define SUBLIMINAL__LINEAR_STATE_SPACE_HPP

namespace subliminal {

template<typename T>
class linear_state_space {
  public:
    typedef T state_type;

    linear_state_space(T min, T max, T init, T step) :
      min_(min), max_(max), init_(init), step_(step)
    {}

    T initial() const { return init_; }

    std::vector<T> neighbours(T const state) const {
      std::vector<T> result;
      if (state+step_ <= max_) result.push_back(state+step_);
      if (state-step_ >= min_) result.push_back(state-step_);
      return result;
    }
  private:
    T min_;
    T max_;
    T init_;
    T step_;
};

}

#endif // SUBLIMINAL__LINEAR_STATE_SPACE_HPP

