cmake_minimum_required(VERSION 3.14)
project(FixedPointDSP LANGUAGES CXX)

# ----------------------------
# C++ standard
# ----------------------------
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ----------------------------
# Project structure
# ----------------------------
# Your fixed-point headers live here
include_directories(${PROJECT_SOURCE_DIR}/include)

# ----------------------------
# GoogleTest
# ----------------------------
# Assumes you've manually placed GoogleTest under external/googletest
add_subdirectory(external/googletest)

# ----------------------------
# Test executable
# ----------------------------
add_executable(FixedPointTests
  tests/FixedPointTests.cpp
)

# Link GoogleTest to the tests
target_link_libraries(FixedPointTests
  gtest_main
)

# Make sure the test target sees your include folder
target_include_directories(FixedPointTests PRIVATE
  ${PROJECT_SOURCE_DIR}/include
)

# ----------------------------
# Enable test discovery (for Visual Studio and ctest)
# ----------------------------
enable_testing()
include(GoogleTest)
gtest_discover_tests(FixedPointTests)
