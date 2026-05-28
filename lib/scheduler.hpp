#pragma once
#include "task.hpp"
#include "future.hpp"
#include <deque>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>


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

  void executeAllParallel() {
    uint16_t num_threads = std::thread::hardware_concurrency();
    std::mutex queue_mutex;
    std::deque<TTaskBase*> tasks_queue;
    std::vector<std::thread> workers;
    std::condition_variable cv;
    bool all_done = false;
    std::atomic<int> active_tasks = 0;

    for (auto& task : tasks_) {
      tasks_queue.push_back(task.get());
    }

    auto worker = [&]() {
    while (true) {
      TTaskBase* task = nullptr;
      {
        std::unique_lock<std::mutex> lock(queue_mutex);
        cv.wait(lock, [&](){return !tasks_queue.empty() || all_done;});
        if (tasks_queue.empty() && all_done) return;
        task = tasks_queue.front();
        tasks_queue.pop_front();
      }
      active_tasks++;
      task->execute();
      active_tasks--;
      cv.notify_all();
    }
    };

    for (int thrd_idx = 0; thrd_idx < num_threads; ++thrd_idx) {
      workers.emplace_back(worker);
    }

    {
      std::unique_lock<std::mutex> lock(queue_mutex);
      cv.wait(lock, [&](){return tasks_queue.empty() && active_tasks == 0;});
      all_done = true;
    }
    cv.notify_all();

    for (auto& w : workers) {
      w.join();
    }
}
};

template <typename Functor, typename... Args>
template <typename UFunc>
decltype(auto) TTask<Functor, Args...>::apply(UFunc&& func) {
  TFuture<ResultType> task_future = this->getFutureResult<ResultType>();
  return scheduler_.add(myForward<UFunc>(func), myMove(task_future));
}