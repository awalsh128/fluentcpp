cmake_minimum_required(VERSION 3.16.3)

project(fluentcpp
  VERSION 0.2
  DESCRIPTION
    "Fluent functions for C++. Provides statically typed objects for easily found fluent functions."
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(GNUInstallDirs) # Make CMAKE_INSTALL_*DIR variables available.

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(functions)

option(BUILD_TESTS "Build unit tests." OFF)
option(BUILD_BENCHMARKS "Build benchmark tests." OFF)
option(BUILD_WEBSITE "Build Doxygen and product website." OFF)
option(BUILD_EXAMPLES "Build code examples using the product." OFF)

add_subdirectory(src)

# Must live in the top level file or tests won't be found.
include(CTest)  # Automatically invokes enable_testing()

if(BUILD_TESTS)
  add_subdirectory(tests)
endif()

if(BUILD_BENCHMARKS)
  add_subdirectory(benchmarks)
endif()

if(BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

if(BUILD_WEBSITE)
  add_subdirectory(assets/artwork)  # Distribute artwork first.
  add_subdirectory(docs)  # Build API website.
endif()
