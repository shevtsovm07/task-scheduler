#pragma once
#include "meta_utils.hpp"
#include <iostream>

class Any {

  struct Base {
    virtual ~Base() = default;
    virtual Base* GetCopy() const = 0;
  };

  template <typename T>
  struct Derived : public Base {
    T value_;
    Derived(const T& value) : value_(value) {}
    Derived(T&& value) : value_(myMove(value)) {}
    Base* GetCopy() const override {
      return new Derived(value_);
    }
  };

  Base* ptr_;

  template <typename U>
  friend U any_cast(Any& any);

  template <typename U>
  friend U any_cast(const Any& any);

  template <typename U>
  friend U any_cast(Any&& any);

public:

  Any() : ptr_(nullptr) {};

  template <typename T>
  Any(T&& value) : ptr_(new Derived<my_decay_t<T>>(myForward<T>(value))) {}

  Any(const Any& other) : ptr_(other.ptr_ ? other.ptr_->GetCopy() : nullptr) {}

  Any& operator=(const Any& other) {
    if (this != &other) {
      delete ptr_;
      ptr_ = other.ptr_ ? other.ptr_->GetCopy() : nullptr;
    }
    return *this;
  }

  Any(Any&& other) noexcept : ptr_(other.ptr_) {
    other.ptr_ = nullptr;
  }

  Any& operator=(Any&& other) noexcept {
    if (this != &other) {
      delete ptr_;
      ptr_ = other.ptr_;
      other.ptr_ = nullptr;
    }
    return *this;
  }

  ~Any() {
    delete ptr_;
  }

  bool has_value() const {
    if (ptr_ == nullptr) {
      return false;
    } return true;
  }

};

template <typename T>
T any_cast(Any& any) {
  auto* p = dynamic_cast<Any::Derived<my_decay_t<T>>*>(any.ptr_);
  if (!p) {
    throw MyBadAnyCast{};
  }
  return static_cast<T>(p->value_);
}

template <typename T>
T any_cast(const Any& any) {
  auto* p = dynamic_cast<Any::Derived<my_decay_t<T>>*>(any.ptr_);
  if (!p) {
    throw MyBadAnyCast{};
  }
  return static_cast<T>(p->value_);
}

template <typename T>
T any_cast(Any&& any) {
  auto* p = dynamic_cast<Any::Derived<my_decay_t<T>>*>(any.ptr_);
  if (!p) {
    throw MyBadAnyCast{};
  }
  return static_cast<T>(std::move(p->value_));
}