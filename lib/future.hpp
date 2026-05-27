#pragma once
#include "meta_utils.hpp"
#include <memory>

struct TTaskBase;

template <typename Type>
class TFuture {
  TTaskBase* task_;
  std::shared_ptr<bool> moved_;

public:
  explicit TFuture(TTaskBase* task) : task_(task), moved_(std::make_shared<bool>(false)) {};
  Type get();
  Type getInternal();
};

template <typename Type>
class TFuture<const Type&> {
  TTaskBase* task_;

public:
  explicit TFuture(TTaskBase* task) : task_(task) {};
  const Type& get();
  const Type& getInternal();
};