#include "scheduler.hpp"
#include <iostream>
#include <gtest/gtest.h>

TEST(ApplyTest, ApplyTest) {
  testing::internal::CaptureStdout();
  TTaskScheduler TaskScheduler;
  auto& task1 = TaskScheduler.add([](int x){return x*2;}, 12);
  task1.apply([](int x){std::cout << x;});
  TaskScheduler.executeAllParallel();
  std::string output = testing::internal::GetCapturedStdout();
  EXPECT_EQ(output, "24");
}

TEST(ApplyTest, ApplyTestTwoTimes) {
  TTaskScheduler TaskScheduler;
  auto& task1 = TaskScheduler.add([](int x){return x*2;}, 12);
  auto& task2 = task1.apply([](int x){return x * 2;});
  auto& task3 = task2.apply([](int x) {return x / 4;});
  auto result = task3.getResultSync<int>();
  auto result2 = task2.getResultSync<int>();
  EXPECT_EQ(result, 12);
  EXPECT_EQ(result2, 48);
}

TEST(ApplyTest, ApplyResultSyncCorrect) {
  TTaskScheduler scheduler;
  auto& task1 = scheduler.add([](int x){ return x + 1; }, 9);
  auto& task2 = task1.apply([](int x){ return x * x; });
  EXPECT_EQ(task2.getResultSync<int>(), 100);
}

TEST(ApplyTest, ApplyLvalue) {
  TTaskScheduler scheduler;
  auto& task1 = scheduler.add([](int x){ return x * 2; }, 6);
  auto func = [](int x){ return x + 1; };
  auto& task2 = task1.apply(func);
  EXPECT_EQ(task2.getResultSync<int>(), 13);
}