#pragma once
#include "task.hpp"
#include "future.hpp"
#include <deque>
#include <memory>


class TTaskScheduler {
  std::deque<std::unique_ptr<TTaskBase>> tasks_;

public:
  template <typename Functor, typename... Args>
  TTask<Functor, my_decay_t<Args>...>& add(Functor&& func, Args&&... args) {
    tasks_.push_back(std::make_unique<TTask<Functor, my_decay_t<Args>...>>(myForward<Functor>(func), *this, myForward<Args>(args)...));
    return static_cast<TTask<Functor, my_decay_t<Args>...>&>(*tasks_.back());
  }

  TTaskScheduler() = default;
  TTaskScheduler(const TTaskScheduler& other) = delete;
  TTaskScheduler(TTaskScheduler&& other) = delete;
  TTaskScheduler& operator=(const TTaskScheduler& other) = delete;
  TTaskScheduler& operator=(TTaskScheduler&& other) = delete;

  void executeAll() {
    for (auto&& task : tasks_) {
      task->execute();
    }
  }
};

template <typename Functor, typename... Args>
template <typename UFunc>
decltype(auto) TTask<Functor, Args...>::apply(UFunc&& func) {
  TFuture<ResultType> task_future = this->getFutureResult<ResultType>();
  return scheduler_.add(myForward<UFunc>(func), myMove(task_future));
}