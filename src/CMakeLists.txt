message(STATUS "Building and installing library.")

set(HEADER_FILES
  ${PROJECT_SOURCE_DIR}/include/fluentcpp/asserts.h
  ${PROJECT_SOURCE_DIR}/include/fluentcpp/query.h
  ${PROJECT_SOURCE_DIR}/include/fluentcpp/traits.h
  ${PROJECT_SOURCE_DIR}/include/fluentcpp/transforms.h)

add_library(fluentcpp STATIC ${HEADER_FILES} query.cpp transforms.cpp)
add_library(fluentcpp::fluentcpp ALIAS fluentcpp)

# Consumers include headers as <fluentcpp/query.h>, both in-tree and installed.
target_include_directories(fluentcpp
  PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_compile_features(fluentcpp PUBLIC cxx_std_20)

# EXPR and CR_EXPR count __VA_ARGS__, which MSVC's traditional preprocessor gets wrong.
# PUBLIC so every target that includes the headers uses the conforming preprocessor too.
target_compile_options(fluentcpp PUBLIC $<$<CXX_COMPILER_ID:MSVC>:/Zc:preprocessor>)

set_target_properties(fluentcpp
  PROPERTIES
  PUBLIC_HEADER "${HEADER_FILES}"
  VERSION ${PROJECT_VERSION})

install(TARGETS fluentcpp
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/fluentcpp COMPONENT lib
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/fluentcpp COMPONENT dev)
