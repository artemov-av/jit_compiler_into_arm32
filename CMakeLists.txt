cmake_minimum_required(VERSION 3.12)
project(run_tests)

set(CMAKE_FIND_ROOT_PATH ${PROJECT_SOURCE_DIR}/sysroot-glibc-linaro)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(CMAKE_CXX_FLAGS "-marm ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_STANDARD 17)

find_package(Threads REQUIRED)

add_executable(run_tests tests.cpp)
add_subdirectory(jit_compiler)
add_subdirectory(googletest)
target_link_libraries(run_tests jit_compiler_lib gmock gtest)
