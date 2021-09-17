message(STATUS "Building examples.")

add_executable(demo demo.cpp)
target_include_directories(demo PRIVATE ../src)
target_link_libraries(demo fluentcpp)