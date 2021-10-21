include(FetchContent)

message(STATUS "Building tests.")

if (BUILD_TESTS)

  FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.0.0-preview3)

  FetchContent_MakeAvailable(Catch2)

  add_executable(tests tests.cpp)
  # Catch2 provides a main function as executable entry point.
  # Private since there is no need to surface these libraries in the binary.
  target_link_libraries(tests PUBLIC Catch2 Catch2WithMain fluentcpp)
  add_test(NAME tests COMMAND tests)

endif()

if (BUILD_BENCHMARKS)

  FetchContent_Declare(
    googlebenchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG master)

  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        release-1.11.0)  

  FetchContent_MakeAvailable(googletest googlebenchmark)

  add_executable(benchmarks benchmarks.cpp)
  target_link_libraries(benchmarks PUBLIC benchmark::benchmark fluentcpp)
  add_test(NAME benchmarks COMMAND tests)

  py3_build(benchmarks_delta.py)
  
  file(COPY benchmark.sh
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR} 
    USE_SOURCE_PERMISSIONS)

endif()