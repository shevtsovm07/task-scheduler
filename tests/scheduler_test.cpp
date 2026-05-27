#include "scheduler.hpp"
#include <string>
#include <gtest/gtest.h>


TEST(BaseTests, BaseSituationOneTask) {
  TTaskScheduler TaskScheduler;
  auto& task1 = TaskScheduler.add([](int x){return x * 2;}, 12);
  auto result = task1.getResultSync<int>();
  ASSERT_EQ(result, 24);
}

TEST(BaseTests, ExecuteAllTest) {
  testing::internal::CaptureStdout();
  TTaskScheduler TaskScheduler;
  auto& task1 = TaskScheduler.add([](){std::cout << "Execute all works right! ";});
  auto& task2 = TaskScheduler.add([](){std::cout << "Yes it is!";});
  TaskScheduler.executeAll();
  std::string output = testing::internal::GetCapturedStdout();
  EXPECT_EQ(output, "Execute all works right! Yes it is!");
}

TEST(BaseTests, GetResultSyncWithoutExecuteAll) {
  TTaskScheduler scheduler;
  auto& task1 = scheduler.add([](int x){ return x + 1; }, 41);
  EXPECT_EQ(task1.getResultSync<int>(), 42);
}

TEST(BaseTests, VoidTask) {
  int side_effect = 0;
  TTaskScheduler scheduler;
  auto& task1 = scheduler.add([&side_effect](){ side_effect = 42; });
  scheduler.executeAll();
  EXPECT_EQ(side_effect, 42);
}

TEST(BaseTests, VoidTaskWithDependency) {
  int result = 0;
  TTaskScheduler scheduler;
  auto& task1 = scheduler.add([](int x){ return x * 2; }, 10);
  auto future = task1.getFutureResult<const int&>();
  auto& task2 = scheduler.add([&result](const int& x){ result = x; }, future);
  scheduler.executeAll();
  EXPECT_EQ(result, 20);
}

TEST(BaseTests, TaskExecutedOnlyOnce) {
  int call_count = 0;
  TTaskScheduler scheduler;
  auto& task1 = scheduler.add([&call_count](){ call_count++; return 1; });
  auto f1 = task1.getFutureResult<const int&>();
  auto f2 = task1.getFutureResult<const int&>();
  auto& task2 = scheduler.add([](const int& a, const int& b){ return a + b; }, f1, f2);
  task2.getResultSync<int>();
  EXPECT_EQ(call_count, 1);
}

TEST(BaseTests, EmptyScheduler) {
  TTaskScheduler scheduler;
  EXPECT_NO_THROW(scheduler.executeAll());
}