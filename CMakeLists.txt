cmake_minimum_required(VERSION 3.14)
project(FixedPointDSP LANGUAGES CXX)

# Make googletest use the shared CRT (MD) to match our default /MD compile flags
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)


# ----------------------------
# C++ standard
# ----------------------------
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ----------------------------
# Include directories
# ----------------------------
include_directories(${PROJECT_SOURCE_DIR}/include)

# ----------------------------
# GoogleTest (external/googletest must be cloned)
# ----------------------------
add_subdirectory(external/googletest)

# ----------------------------
# Google Benchmark (external/benchmark must be cloned)
# ----------------------------
# Disable its own tests so it doesn’t recurse into googletest
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
add_subdirectory(external/benchmark)

# ----------------------------
# Unit‐test executable
# ----------------------------
add_executable(FixedPointTests
  tests/FixedPointTests.cpp
  tests/FIRFilterTests.cpp
  tests/ConvolutionTests.cpp 
  tests/DFTTests.cpp 
  tests/FFTTests.cpp
)

target_link_libraries(FixedPointTests
  PRIVATE
    GTest::gtest_main
)

target_include_directories(FixedPointTests PRIVATE
  ${PROJECT_SOURCE_DIR}/include
)

target_compile_definitions(FixedPointTests
  PRIVATE
    _SILENCE_NONFLOATING_COMPLEX_DEPRECATION_WARNING
)

# Discover and register with CTest
enable_testing()
include(GoogleTest)
gtest_discover_tests(FixedPointTests)

# ----------------------------
# FFT performance benchmark
# ----------------------------
add_executable(FFTBenchmark
  benchmarks/FFTBenchmark.cpp
)

# Link both the core library and the provided main()
target_link_libraries(FFTBenchmark
  PRIVATE
    benchmark::benchmark
    benchmark::benchmark_main
)

target_include_directories(FFTBenchmark PRIVATE
  ${PROJECT_SOURCE_DIR}/include
)

target_compile_definitions(FFTBenchmark
  PRIVATE
    _SILENCE_NONFLOATING_COMPLEX_DEPRECATION_WARNING
)

# Register benchmark as a CTest test, passing --benchmark_time_unit=ms
# so Test Explorer will show results in milliseconds
add_test(NAME FFTBenchmark COMMAND FFTBenchmark --benchmark_time_unit=ms)
set_tests_properties(FFTBenchmark PROPERTIES
  PASS_REGULAR_EXPRESSION "BM_"
)

# Optional custom target to build (but not run) your benchmark
add_custom_target(run_benchmark
  DEPENDS FFTBenchmark
  COMMENT "Build FFTBenchmark"
)
