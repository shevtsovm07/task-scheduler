cmake_minimum_required(VERSION 3.20)
project(task-scheduler)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(cpr REQUIRED)
find_package(nlohmann_json REQUIRED)

enable_testing()

add_subdirectory(lib)
add_subdirectory(app)
add_subdirectory(tests)
add_subdirectory(bin)