#pragma once
#include <cstddef>
#include "meta_utils.hpp"


template <typename... Types>
struct MyTuple{};

template <>
struct MyTuple<>{};

template <typename Head, typename... Tail>
struct MyTuple<Head, Tail...> : MyTuple<Tail...> {
  Head elem_;

  template <typename UHead, typename... UTail>
  MyTuple(UHead&& first, UTail&&... rest) : MyTuple<Tail...>(myForward<UTail>(rest)...), elem_(myForward<UHead>(first)) {}
};

template <size_t Index, typename Head, typename... Tail>
decltype(auto) MyGet(MyTuple<Head, Tail...>& t) {
  if constexpr (Index == 0) {
    return t.elem_;
  } else {
    return MyGet<Index - 1>(static_cast<MyTuple<Tail...>&>(t));
  }
};

template <size_t Index, typename Head, typename... Tail>
decltype(auto) MyGet(const MyTuple<Head, Tail...>& t) {
  if constexpr (Index == 0) {
    return t.elem_;
  } else {
    return MyGet<Index - 1>(static_cast<const MyTuple<Tail...>&>(t));
  }
};

template <size_t Index, typename Head, typename... Tail>
decltype(auto) MyGet(MyTuple<Head, Tail...>&& t) {
  if constexpr (Index == 0) {
    return myMove(t.elem_);
  } else {
    return MyGet<Index - 1>(static_cast<MyTuple<Tail...>&&>(t));
  }
};

template <typename... Args>
auto MyMakeTuple(Args&&... args) {
  return MyTuple<my_decay_t<Args>...>(myForward<Args>(args)...);
};

template <typename T>
struct MyTupleSize;

template <typename... Args>
struct MyTupleSize<MyTuple<Args...>> {
  static constexpr size_t value = sizeof...(Args);
};
