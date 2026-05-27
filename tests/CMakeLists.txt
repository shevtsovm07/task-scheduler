find_package(GTest REQUIRED)

add_executable(scheduler-tests
    scheduler_test.cpp
    dependencies_test.cpp
    future_test.cpp
    apply_test.cpp
    callable_types_test.cpp
)

target_link_libraries(scheduler-tests PRIVATE
    scheduler-lib
    GTest::gtest
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(scheduler-tests)