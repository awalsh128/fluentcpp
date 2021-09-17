message(STATUS "Building and installing library.")

file(GLOB SRC_FILES *.h *.cpp)

add_library(fluentcpp STATIC ${SRC_FILES})

set_target_properties(fluentcpp PROPERTIES
  VERSION ${PROJECT_VERSION}
  PUBLIC_HEADER query.h
  LINKER_LANGUAGE CXX
  CXX_STANDARD_REQUIRED 20)

target_include_directories(fluentcpp PUBLIC .)

install(TARGETS fluentcpp DESTINATION lib)
install(FILES ${SRC_FILES} DESTINATION include)  
install(TARGETS fluentcpp
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
