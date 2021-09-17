include(FetchContent)

message(STATUS "Building documentation.")

set(DOCS_DIR ${CMAKE_CURRENT_BINARY_DIR}/../../docs)
file(MAKE_DIRECTORY ${DOCS_DIR})

################################################################################
# Doxygen
################################################################################

FetchContent_Declare(
  Doxygen
  GIT_REPOSITORY https://github.com/doxygen/doxygen.git
  GIT_TAG        Release_1_9_2)
FetchContent_MakeAvailable(Doxygen)
find_package(Doxygen REQUIRED)

find_program(DOT_PATH dot PATH_SUFFIXES graphviz2.38/bin graphviz/bin)
if(NOT DOT_PATH)
    message(WARNING "Graphviz DOT not found, some UML diagrams will not be generated via Doxygen.")
endif()

set(DOXYGEN_OUTPUT_DIR ${DOCS_DIR}/api)
file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIR})

set(DOXYGEN_INPUT 
  "${PROJECT_SOURCE_DIR}/src \
  ${PROJECT_SOURCE_DIR}/README.md \
  ${PROJECT_SOURCE_DIR}/LICENSE")
set(DOXYGEN_INDEX_FILE ${DOXYGEN_OUTPUT_DIR}/index.html)
set(DOXYFILE_IN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
set(DOXYFILE_OUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
set(DOXYGEN_HEADER_FILE ${CMAKE_CURRENT_SOURCE_DIR}/header.html)
set(DOXYGEN_FOOTER_FILE ${CMAKE_CURRENT_SOURCE_DIR}/footer.html)
set(DOXYGEN_THEME_CSS_FILES 
  "${CMAKE_CURRENT_SOURCE_DIR}/doxygen-awesome.css \
  ${CMAKE_CURRENT_SOURCE_DIR}/doxygen-awesome-sidebar-only.css \
  ${CMAKE_CURRENT_SOURCE_DIR}/doxygen-awesome-sidebar-only-darkmode-toggle.css")
set(DOXYGEN_THEME_OTHER_FILES 
  "${CMAKE_CURRENT_SOURCE_DIR}/doxygen-awesome-darkmode-toggle.js")

# Replace variables inside @var@ with the current values
configure_file(${DOXYFILE_IN_FILE} ${DOXYFILE_OUT_FILE} @ONLY)

file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIR})

add_custom_command(
  COMMENT "Building API documentation."
  COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT_FILE}
  # Rely on this as the main artifact that gets generated.
  OUTPUT ${DOXYGEN_INDEX_FILE})
