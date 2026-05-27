#include <gtest/gtest.h>
#include "scheduler.hpp"
#include <iostream>


TEST(FutureTests, FutureMultiple) {
  TTaskScheduler TaskScheduler;
  auto& task1 = TaskScheduler.add([](int x){return x * 2;}, 12);
  auto task1_future_move = task1.getFutureResult<int>();
  auto& task2 = TaskScheduler.add([](int x){return x / 2;}, task1_future_move);
  auto result = task2.getResultSync<int>();
  EXPECT_THROW(task1_future_move.get(), MyAlreadyMovedException);
}

TEST(FutureTests, ConstRefFutureMultipleCalls) {
  TTaskScheduler scheduler;
  auto& task1 = scheduler.add([](int x){ return x * 3; }, 10);
  auto future = task1.getFutureResult<const int&>();
  const int& r1 = future.get();
  const int& r2 = future.get();
  EXPECT_EQ(r1, 30);
  EXPECT_EQ(r2, 30);
}

TEST(FutureTests, ConstRefFutureAsArgument) {
  TTaskScheduler scheduler;
  auto& task1 = scheduler.add([](int x){ return x + 5; }, 7);
  auto future = task1.getFutureResult<const int&>();
  auto& task2 = scheduler.add([](const int& x){ return x * 2; }, future);
  EXPECT_EQ(task2.getResultSync<int>(), 24);
}