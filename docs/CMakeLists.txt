message(STATUS "Building API documentation.")

set(DOXYGEN_OUTPUT_DIR ${PROJECT_BINARY_DIR}/website)

find_package(Doxygen REQUIRED)
find_package(Python3 REQUIRED COMPONENTS Interpreter)

# Doxyfile.in variable substitutions.
set(DOXYGEN_SRC_INPUT ${PROJECT_SOURCE_DIR}/include)
set(DOXYGEN_INDEX_FILE ${DOXYGEN_OUTPUT_DIR}/index.html)
set(DOXYGEN_THEME_DIR ${CMAKE_CURRENT_SOURCE_DIR}/theme)
set(DOXYGEN_LOGO_FILE ${PROJECT_SOURCE_DIR}/assets/artwork/fluentcpp-sm-light.png)
set(DOXYGEN_MAINPAGE_FILE ${CMAKE_CURRENT_BINARY_DIR}/mainpage.md)
set(DOXYGEN_HEADER_FILE ${CMAKE_CURRENT_BINARY_DIR}/header.html)
set(DOXYFILE_IN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
set(DOXYFILE_OUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
set(PREPARE_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/prepare.py)

configure_file(${DOXYFILE_IN_FILE} ${DOXYFILE_OUT_FILE} @ONLY)

add_custom_command(
  COMMENT "Building API documentation."
  COMMAND ${CMAKE_COMMAND} -E remove_directory ${DOXYGEN_OUTPUT_DIR}
  COMMAND ${CMAKE_COMMAND} -E make_directory ${DOXYGEN_OUTPUT_DIR}
  # The README is the landing page; convert its GitHub-only markup for Doxygen.
  COMMAND ${Python3_EXECUTABLE} ${PREPARE_SCRIPT} mainpage ${PROJECT_SOURCE_DIR}/README.md
          ${DOXYGEN_MAINPAGE_FILE}
  # Generate the stock header for this Doxygen version, then add the theme scripts to it.
  COMMAND ${DOXYGEN_EXECUTABLE} -w html ${DOXYGEN_HEADER_FILE} ${CMAKE_CURRENT_BINARY_DIR}/footer.html
          ${CMAKE_CURRENT_BINARY_DIR}/stock.css ${DOXYFILE_OUT_FILE}
  COMMAND ${Python3_EXECUTABLE} ${PREPARE_SCRIPT} header ${DOXYGEN_HEADER_FILE}
  COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT_FILE}
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  DEPENDS ${DOXYFILE_IN_FILE} ${PREPARE_SCRIPT} ${PROJECT_SOURCE_DIR}/README.md
  OUTPUT ${DOXYGEN_INDEX_FILE}
  VERBATIM)

add_custom_target(website_api ALL DEPENDS ${DOXYGEN_INDEX_FILE})
# Require source code compile to generate documentation from.
add_dependencies(website_api fluentcpp)
