#include "scheduler.hpp"
#include <gtest/gtest.h>

TEST(BaseTests, BaseSituationTwoTasks) {
  TTaskScheduler TaskScheduler;
  auto& task1 = TaskScheduler.add([](int x){return x * 2;}, 12);
  auto task1Future = task1.getFutureResult<int>();
  auto& task2 = TaskScheduler.add([](int x){return x / 2;}, task1Future);
  auto result = task2.getResultSync<int>();
  ASSERT_EQ(result, 12);
}

TEST(BaseTests, BaseSituationThreeTasks) {
  TTaskScheduler TaskScheduler;
  auto& task1 = TaskScheduler.add([](int x){return x * 2;}, 12);
  auto task1Future = task1.getFutureResult<int>();
  auto& task2 = TaskScheduler.add([](int x){return x / 2;}, task1Future);
  auto task2Future = task2.getFutureResult<int>();
  auto& task3 = TaskScheduler.add([](int x){return x * 2;}, task2Future);
  auto result = task3.getResultSync<int>();
  ASSERT_EQ(result, 24);
}
