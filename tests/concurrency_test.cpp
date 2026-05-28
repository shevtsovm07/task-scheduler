#include "scheduler.hpp"
#include <chrono>
#include <iostream>
#include <gtest/gtest.h>

void task_1() {
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));
  std::cout << "task 1" << "\n";
}

void task_2() {
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));
  std::cout << "task 2" << "\n";
}

void task_3() {
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));
  std::cout << "task 3" << "\n";
}

TEST(ConcurrencyTest, WorksFast) {
  TTaskScheduler TaskScheduler;

  auto& task1 = TaskScheduler.add(task_1);
  auto& task2 = TaskScheduler.add(task_2);
  auto& task3 = TaskScheduler.add(task_3);

  auto start = std::chrono::steady_clock::now();
  TaskScheduler.executeAllParallel();
  auto end = std::chrono::steady_clock::now();

  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

  EXPECT_LT(duration, 4000);
}