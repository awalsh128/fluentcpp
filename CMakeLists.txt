cmake_minimum_required(VERSION 3.16.3) # Change .lgtml.yml as well when this does.

include(GNUInstallDirs) # Make CMAKE_INSTALL_*DIR variables available.
set(CMAKE_CXX_STANDARD 20)

project(fluentcpp 
  VERSION 0.1
  DESCRIPTION 
    "Fluent functions for C++. Provides statically typed objects for easily found fluent functions.")  

# Make the src directory available for include lookup.
include_directories(src)

add_subdirectory(src)

# Must live in the top level file or tests won't be found.
include(CTest)  # Automatically invokes enable_testing()
add_subdirectory(tests)

if(CMAKE_BUILD_TYPE MATCHES Release)
  add_subdirectory(examples)
  add_subdirectory(configs)
endif(CMAKE_BUILD_TYPE MATCHES Release)