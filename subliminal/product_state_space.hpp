#ifndef SUBLIMINAL__PRODUCT_STATE_SPACE_HPP
#define SUBLIMINAL__PRODUCT_STATE_SPACE_HPP

#include <vector>

#include <boost/foreach.hpp>
#include <boost/fusion/include/transform.hpp>
#include <boost/fusion/include/front.hpp>
#include <boost/fusion/include/invoke_function_object.hpp>

#include "construct.hpp"
#include "variadic_vector.hpp"

namespace subliminal {

namespace detail {

struct get_initial {
  template<typename F>
  struct result;
  template<typename StateSpace>
  struct result<get_initial(StateSpace const&)> {
    typedef typename StateSpace::state_type type;
  };

  template<typename StateSpace>
  typename StateSpace::state_type operator()(StateSpace const& s) const {
    return s.initial();
  }
};

template<typename Constructed, typename Sequence, typename... Other>
Constructed construct_from_mix_reverse(Sequence const& s, Other const&... o)
{
  typedef typename variadic_vector<Other...>::type OtherSeq;
  OtherSeq other_seq(o...);
  auto joined_seq =
    boost::fusion::joint_view<OtherSeq const, Sequence const>(other_seq, s);
  return boost::fusion::invoke_function_object(
    construct<Constructed>(), joined_seq
  );
}

template<typename... Prodands>
struct add_neighbours;

template<typename Head, typename... Tail>
struct add_neighbours<Head, Tail...> {
  template<typename Result, typename... Prefix>
  void operator()(
    typename variadic_vector<
      typename Head::state_type, typename Tail::state_type...
    >::type const& state,
    Result& result,
    typename variadic_vector<Head, Tail...>::type const& prodands,
    Prefix const&... prefix
  )
  {
    Head const& head_prodand = boost::fusion::front(prodands);
    typename variadic_vector<Tail...>::type const& tail_prodands =
      boost::fusion::pop_front(prodands);
    typedef typename Head::state_type HeadState;
    HeadState const& head_state = boost::fusion::front(state);
    typedef typename variadic_vector<typename Tail::state_type...>::type
      TailState;
    TailState const& tail_state = boost::fusion::pop_front(state);
    auto prodand_neighbours = head_prodand.neighbours(head_state);
    BOOST_FOREACH(HeadState const& prodand_neighbour, prodand_neighbours) {
      result.push_back(
        construct_from_mix_reverse<typename Result::value_type>(
          tail_state, prefix..., prodand_neighbour
        )
      );
    }
    add_neighbours<Tail...>()(
      tail_state, result, tail_prodands, prefix..., head_state
    );
  }
};

template<>
struct add_neighbours<> {
  template<typename Result, typename... Prefix>
  void operator()(
    boost::fusion::vector<> const& /*state*/,
    Result& /*result*/,
    boost::fusion::vector<> const& /*prodands*/,
    Prefix const&... /*prefix*/
  )
  {
  }
};

}

template<typename... T>
class product_state_space {
  public:
    typedef typename variadic_vector<typename T::state_type...>::type
      state_type;

    product_state_space(T... prodands) :
      prodands_(std::move(prodands)...)
    {}

    state_type initial() const {
      return boost::fusion::transform(prodands_, detail::get_initial());
    }

    std::vector<state_type> neighbours(state_type const& state) const {
      std::vector<state_type> result;
      detail::add_neighbours<T...>()(state, result, prodands_);
      return result;
    }
  private:
    typename variadic_vector<T...>::type prodands_;
};

template<typename... T>
product_state_space<T...> make_product_state_space(T&&... prodands)
{
  return product_state_space<typename std::remove_reference<T>::type...>(
    std::forward<T>(prodands)...
  );
}

}

#endif // SUBLIMINAL__PRODUCT_STATE_SPACE_HPP

