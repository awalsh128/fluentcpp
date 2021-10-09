message(STATUS "Building and installing library.")

add_library(fluentcpp STATIC  
  asserts.h
  traits.h
  transforms.h transforms.cpp
  query.h query.cpp)

set_target_properties(fluentcpp PROPERTIES
  VERSION ${PROJECT_VERSION}
  PUBLIC_HEADER query.h
  LINKER_LANGUAGE CXX
  CXX_STANDARD_REQUIRED 20)

install(TARGETS fluentcpp  
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT lib
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT lib
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/fluentcpp COMPONENT dev)
