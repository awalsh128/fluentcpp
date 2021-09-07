cmake_minimum_required(VERSION 3.16.3) # Change .lgtml.yml as well when this does.

include(GNUInstallDirs) # Make CMAKE_INSTALL_*DIR variables available.

set(CMAKE_CXX_STANDARD 20)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

project(fluentcpp 
  VERSION 0.1
  DESCRIPTION 
    "Fluent functions for C++. Provides statically typed objects for easily found fluent functions.")

add_subdirectory(src)
add_subdirectory(tests)

if(CMAKE_BUILD_TYPE MATCHES Release)
  add_subdirectory(examples)
  add_subdirectory(configs/doxygen)  
endif(CMAKE_BUILD_TYPE MATCHES Release)