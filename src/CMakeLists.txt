add_library(fluentcpp STATIC query.h)

set_target_properties(fluentcpp PROPERTIES
  VERSION ${PROJECT_VERSION}
  PUBLIC_HEADER query.h
  LINKER_LANGUAGE CXX
  CXX_STANDARD_REQUIRED 20)

target_include_directories(fluentcpp PUBLIC .)

message(STATUS "Building library.")
install(TARGETS fluentcpp DESTINATION lib)
install(FILES query.h query.cpp DESTINATION include)

message(STATUS "Installing library.")
install(TARGETS fluentcpp
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
