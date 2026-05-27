#pragma once
#include "meta_utils.hpp"
#include "tuple.hpp"
#include "future.hpp"

template<typename T>
class TFuture;

template<typename T>
class TFuture<const T&>;

template <typename T>
decltype(auto) resolve(T&& value) {
  return myForward<T>(value);
}

template <typename T>
decltype(auto) resolve(TFuture<T>& value) {
  return value.getInternal();
}

template <typename T>
decltype(auto) resolve(TFuture<T>&& value) {
  return myMove(value).getInternal();
}

template <typename Functor, typename Tuple, size_t First, size_t... Indexes>
decltype(auto) my_apply_method_ptr(Functor&& f, Tuple&& t, MyIndexSequence<First, Indexes...>) {
  decltype(auto) obj = MyGet<0>(myForward<Tuple>(t));
  return (obj.*f)(resolve(MyGet<Indexes>(myForward<Tuple>(t)))...);
}

template <typename Functor, typename Tuple, size_t... Indexes>
decltype(auto) my_apply_implementation(Functor&& f, Tuple&& t, MyIndexSequence<Indexes...>) {
  return myForward<Functor>(f)(resolve(MyGet<Indexes>(myForward<Tuple>(t)))...);
}


template <typename Functor, typename Tuple>
decltype(auto) myApply(Functor&& f, Tuple&& t) {
  constexpr size_t el_amount = MyTupleSize<my_decay_t<Tuple>>::value;
  using IndSeq = typename MyMakeIndexSequence<el_amount>::type;
  if constexpr (my_is_member_function_ptr_v<my_decay_t<Functor>>) {
    return my_apply_method_ptr(myForward<Functor>(f), myForward<Tuple>(t), IndSeq{});
  } else {
    return my_apply_implementation(myForward<Functor>(f), myForward<Tuple>(t), IndSeq{});
  }
}
