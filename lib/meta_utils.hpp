#pragma once
#include <cstddef>

struct my_true_type {static constexpr bool value = true;};
struct my_false_type {static constexpr bool value = false;};


template<typename T>
struct myRemoveReference { using type = T; };

template<typename T>
struct myRemoveReference<T&> { using type = T; };

template<typename T>
struct myRemoveReference<T&&> { using type = T; };

template<typename T>
struct myRemoveConst{ using type = T; };

template<typename T>
struct myRemoveConst<const T> { using type = T; };

template<typename T>
struct myRemoveVolatile{ using type = T; };

template<typename T>
struct myRemoveVolatile<volatile T> { using type = T; };

template <typename T>
using my_remove_const_t = typename myRemoveConst<T>::type;

template <typename T>
using my_remove_reference_t = typename myRemoveReference<T>::type;

template <typename T>
using my_remove_volatile_t = typename myRemoveVolatile<T>::type;

template <typename T>
struct myDecay {
  using NoRef = my_remove_reference_t<T>;
  using NoConst = my_remove_const_t<NoRef>;
  using Type = my_remove_volatile_t<NoConst>;
};

template <typename T>
using my_decay_t = myDecay<T>::Type;

template <typename T>
struct MyIsMemberFunctionPtr : my_false_type {};

template <typename RetType, typename Class, typename... Args>
struct MyIsMemberFunctionPtr<RetType(Class::*)(Args...)> : my_true_type {};

template <typename RetType, typename Class, typename... Args>
struct MyIsMemberFunctionPtr<RetType(Class::*)(Args...) const> : my_true_type {};

template <typename T>
inline constexpr bool my_is_member_function_ptr_v = MyIsMemberFunctionPtr<T>::value;

template <typename T>
T&& myForward(my_remove_reference_t<T>& t) {
  return static_cast<T&&>(t);
};

template <typename T>
T&& myForward(my_remove_reference_t<T>&& t) {
  return static_cast<T&&>(t);
};


template <typename T>
my_remove_reference_t<T>&& myMove(T&& arg) noexcept {
  return static_cast<my_remove_reference_t<T>&&>(arg);
}

template <bool B, typename T, typename F>
struct my_conditional { using type = T; };

template <typename T, typename F>
struct my_conditional<false, T, F> { using type = F; };

template <bool B, typename T, typename F>
using my_conditional_t = typename my_conditional<B, T, F>::type;

template <typename T>
struct my_is_void : my_false_type {}; 

template <>
struct my_is_void<void> : my_true_type {}; 
template <> 
struct my_is_void<const void> : my_true_type {};

template <typename T>
inline constexpr bool my_is_void_v = my_is_void<T>::value;

template <size_t... Indexes>
struct MyIndexSequence {};

template <size_t N, size_t... Indexes>
struct MyMakeIndexSequence : MyMakeIndexSequence<N - 1, N - 1, Indexes...> {};

template <size_t... Indexes>
struct MyMakeIndexSequence<0, Indexes...> {
  using type = MyIndexSequence<Indexes...>;
};

struct MyBadAnyCast {
  const char* what() const { return "bad any cast"; }
};

struct MyGraphCycleDetected {
  const char* what() const { return "graph contains cycle"; }
};

struct MyAlreadyMovedException {
  const char* what() const { return "value already moved"; }
};

template<typename T>
T&& myDeclVal() noexcept;