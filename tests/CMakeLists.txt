include(FetchContent)

include(CTest)
include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.0.0-preview3)
FetchContent_MakeAvailable(Catch2)

message(STATUS "Building tests.")
add_executable(query_test query_compound_test.cpp query_integer_test.cpp)
target_link_libraries(query_test PRIVATE fluentcpp)
target_link_libraries(query_test PRIVATE Catch2::Catch2WithMain)

add_test(NAME QueryTest COMMAND query_test)
