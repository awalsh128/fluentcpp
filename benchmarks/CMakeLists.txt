include(FetchContent)

message(STATUS "Building benchmarks.")

# Skip building Google Benchmark's own tests, which would pull in googletest.
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
  googlebenchmark
  GIT_REPOSITORY https://github.com/google/benchmark.git
  GIT_TAG        v1.9.1)

FetchContent_MakeAvailable(googlebenchmark)

add_executable(benchmarks benchmarks.cpp benchmarks_common.h)
target_link_libraries(benchmarks PRIVATE benchmark::benchmark fluentcpp::fluentcpp)
add_test(NAME benchmarks COMMAND benchmarks)

add_executable(benchmarks_baseline benchmarks_baseline.cpp benchmarks_common.h)
target_link_libraries(benchmarks_baseline PRIVATE benchmark::benchmark fluentcpp::fluentcpp)
add_test(NAME benchmarks_baseline COMMAND benchmarks_baseline)

py3_build(benchmarks_report.py)

file(COPY benchmark.sh
  DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
  USE_SOURCE_PERMISSIONS)
