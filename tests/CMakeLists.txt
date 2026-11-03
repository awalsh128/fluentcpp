include(FetchContent)

message(STATUS "Building tests.")

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.7.1)

FetchContent_MakeAvailable(Catch2)

add_executable(tests tests.cpp models.h)
# Catch2 provides a main function as executable entry point.
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain fluentcpp::fluentcpp)
add_test(NAME tests COMMAND tests)
