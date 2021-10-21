# HEADER-BEGIN

cmake_minimum_required(VERSION 3.16.3) # Change .lgtml.yml as well when this does.

include(GNUInstallDirs) # Make CMAKE_INSTALL_*DIR variables available.
set(CMAKE_CXX_STANDARD 20)

project(fluentcpp 
  VERSION 0.1
  DESCRIPTION 
    "Fluent functions for C++. Provides statically typed objects for easily found fluent functions.")  

# HEADER-END

include(assets/scripts/functions.cmake)

option(RUN_TESTS, "Run unit and benchmark tests." OFF)
option(BUILD_WEBSITE "Build Doxygen and product website." OFF)
option(RUN_BENCHMARKS "Run benchmark tests." OFF)
option(BUILD_EXAMPLES "Build code examples using the product." OFF)

# Make the src directory available for include lookup.
include_directories(src)

add_subdirectory(src)

# Must live in the top level file or tests won't be found.
# Options are evaluated in the subdirectory CMake file.
include(CTest)  # Automatically invokes enable_testing()
add_subdirectory(tests)

if (BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

if(BUILD_WEBSITE)
  add_subdirectory(assets/artwork)  # Distribute artwork first.
  add_subdirectory(assets/doxygen)  # Build API website.
endif()
