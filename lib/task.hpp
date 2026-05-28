#pragma once
#include "any.hpp"
#include "tuple.hpp"
#include "meta_utils.hpp"
#include "apply.hpp"
#include "future.hpp"
#include <iostream>

class TTaskScheduler;

enum Conditions {
  NotProcessed,
  Processing,
  Processed
};

struct TTaskBase {
  virtual ~TTaskBase() = default;
  virtual void execute() = 0;
  virtual Any& getResult() = 0;
};


template <typename Functor, typename... Args>
class TTask : public TTaskBase {
  Functor func_;
  MyTuple<Args...> args_;
  TTaskScheduler& scheduler_;
  Any result_;
  std::atomic<Conditions> condition_ = NotProcessed;
  using ResultType = decltype(myApply(myDeclVal<Functor>(), myDeclVal<MyTuple<Args...>>()));

public:

  template <typename UFunctor, typename... UArgs>
  TTask(UFunctor&& functor, TTaskScheduler& scheduler, UArgs&&... args) 
    : func_(myForward<UFunctor>(functor)),
      args_(myForward<UArgs>(args)...), 
      scheduler_(scheduler) {};

  void execute() override {
    Conditions expected = NotProcessed;
    if (!condition_.compare_exchange_strong(expected, Processing)) {
      if (expected == Processed) return;
      if (expected == Processing) throw MyGraphCycleDetected{};
    }
    if constexpr (my_is_void_v<ResultType>) {
      myApply(func_, args_);
    } else {
      result_ = myApply(func_, args_);
    }
    condition_ = Processed;
  } 

  Any& getResult() override {
    return result_;
  }

  template <typename T>
  T getResultSync() {
    execute();
    if constexpr (my_is_void_v<T>) {
      return;
    }
    return any_cast<T>(getResult());
  }

  template <typename T>
  TFuture<T> getFutureResult() {
    return TFuture<T>(this);
  }

  template <typename UFunc>
  decltype(auto) apply(UFunc&& func);

};

template <typename Type>
Type TFuture<Type>::get() {
  if (*moved_) {
    throw MyAlreadyMovedException{};
  }
  task_->execute();
  *(moved_) = true;
  return any_cast<Type>(task_->getResult());
}

template <typename Type>
const Type& TFuture<const Type&>::get() {
  task_->execute();
  return any_cast<const Type&>(task_->getResult());
}

template<typename Type>
Type TFuture<Type>::getInternal() {
  task_->execute();
  *(moved_) = true;
  return any_cast<Type>(task_->getResult());
}

template<typename Type>
const Type& TFuture<const Type&>::getInternal() {
  task_->execute();
  return any_cast<const Type&>(task_->getResult());
}