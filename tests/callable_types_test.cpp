#include "scheduler.hpp"
#include <gtest/gtest.h>

struct ForTest {
  int operator()(int n, int m) {
    return n + m;
  }

  int multiply(int n, int m) {
    return n * m;
  }

  int divide(int n, int m) const {
    return n / m;
  }
};

TEST(NotLambdaFunction, FunctorAsFunction) {
  TTaskScheduler TaskScheduler;
  ForTest fortestclass;
  auto& task1 = TaskScheduler.add(fortestclass, 12, 13);
  auto result = task1.getResultSync<int>();
  EXPECT_EQ(result, 25);
}

TEST(NotLambdaFunction, ClassMethodAsFunction) {
  TTaskScheduler TaskScheduler;
  ForTest fortestclass;
  auto& task1 = TaskScheduler.add(&ForTest::multiply, fortestclass, 12, 13);
  auto result = task1.getResultSync<int>();
  EXPECT_EQ(result, 156);
}

TEST(NotLambdaFunction, ClassConstMethodAsFunction) {
  TTaskScheduler TaskScheduler;
  ForTest fortestclass;
  auto& task1 = TaskScheduler.add(&ForTest::divide, fortestclass, 12, 4);
  auto result = task1.getResultSync<int>();
  EXPECT_EQ(result, 3);
}