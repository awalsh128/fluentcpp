include(FetchContent)

message(STATUS "Building tests.")

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.0.0-preview3)

FetchContent_MakeAvailable(Catch2)

file(GLOB TEST_FILES *.h *.cpp)

add_executable(query_tests ${TEST_FILES})

# Catch2 provides a main function as executable entry point.
# Private since there is no need to surface these libraries in the binary.
target_link_libraries(query_tests PUBLIC Catch2 Catch2WithMain fluentcpp)
add_test(NAME tests COMMAND query_tests)