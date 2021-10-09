message(STATUS "Building examples.")

add_executable(demo demo.cpp)
target_link_libraries(demo fluentcpp)