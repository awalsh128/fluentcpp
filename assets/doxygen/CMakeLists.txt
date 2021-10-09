message(STATUS "Building API documentation.")

set(DOXYGEN_OUTPUT_DIR ${PROJECT_BINARY_DIR}/website)

find_package(Doxygen)
if (NOT DOXYGEN_FOUND)
  message(SEND_ERROR "Doxygen not found. Aborted building API documentation.")
  return()
endif(NOT DOXYGEN_FOUND)

# Clean and rebuild directory.
file(REMOVE RECURSE ${DOXYGEN_OUTPUT_DIR})
file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIR})

# Doxyfile.in variable substitutions.
set(DOXYGEN_SRC_INPUT ${PROJECT_SOURCE_DIR}/src)
set(DOXYGEN_INDEX_FILE ${DOXYGEN_OUTPUT_DIR}/index.html)
set(DOXYGEN_MDFILE ${PROJECT_SOURCE_DIR}/README.md)
set(DOXYFILE_IN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
set(DOXYFILE_OUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
set(DOXYGEN_FOOTER_FILE ${CMAKE_CURRENT_BINARY_DIR}/footer.html)
set(DOXYGEN_HEADER_FILE ${CMAKE_CURRENT_BINARY_DIR}/header.html)
set(DOXYGEN_CSS_FILE ${CMAKE_CURRENT_SOURCE_DIR}/doxygen.css)

# Replace variables inside @var@ with the current values
configure_file(${DOXYFILE_IN_FILE} ${DOXYFILE_OUT_FILE} @ONLY)

set(SCRIPT_FILE COMMAND ${PROJECT_SOURCE_DIR}/assets/scripts/expand_html_includes.sh)

set(DOXYGEN_SRC_FOOTER_FILE ${CMAKE_CURRENT_SOURCE_DIR}/footer.html)
set(DOXYGEN_SRC_HEADER_FILE ${CMAKE_CURRENT_SOURCE_DIR}/header.html)

add_custom_command(
  COMMENT "Building API documentation."
  # Expand out HTML files (i.e. replace href-include attributed elements).
  COMMAND ${SCRIPT_FILE} ${DOXYGEN_SRC_FOOTER_FILE} ${DOXYGEN_FOOTER_FILE}
  COMMAND ${SCRIPT_FILE} ${DOXYGEN_SRC_HEADER_FILE} ${DOXYGEN_HEADER_FILE}
  # Copy all web assets.
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/assets/website ${DOXYGEN_OUTPUT_DIR}
  # Run Doxygen and generate docs.
  COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT_FILE}
  # Update Doxyfile.in timestamp to avoid unchanged rebuild.
  COMMAND ${CMAKE_COMMAND} -E touch ${DOXYFILE_IN_FILE}
  WORKING_DIRECTORY ${DOXYGEN_OUTPUT_DIR}
  # Run condition based on state of Doxyfile.
  DEPENDS ${DOXYFILE_IN_FILE} ${DOXYGEN_SRC_FOOTER_FILE} ${DOXYGEN_SRC_HEADER_FILE}
  # HTML index file will be used as dependency artifact.
  OUTPUT ${DOXYGEN_INDEX_FILE}
  VERBATIM)

# HTML index file will be used as dependency artifact as noted above.
add_custom_target(website_api ALL DEPENDS ${DOXYGEN_INDEX_FILE})
# Require source code compile to generate documentation from.
add_dependencies(website_api fluentcpp)
