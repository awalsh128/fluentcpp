function(BIN_EXISTS NAME EXIT_CODE)
  execute_process(COMMAND which ${NAME} RESULT_VARIABLE EXIT_CODE)
endfunction()

function(PY3_BUILD FILENAME)
  find_program(PY3C python3 "Python 3 compiler executable.")
  if(NOT PY3C)
    message(FATAL_ERROR "Python 3 compiler not found.")
  endif()

  file(COPY ${FILENAME}
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR} 
    USE_SOURCE_PERMISSIONS)  

  execute_process(
    COMMAND ${PY3C} -m py_compile ${CMAKE_CURRENT_BINARY_DIR}/${FILENAME}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    RESULT_VARIABLE PY3_BUILD_RC)
  
  if (NOT PY3_BUILD_RC EQUAL 0)
    message(FATAL_ERROR 
      "Python 3 compile of ${FILENAME} exited with return code "
      "${PY3_BUILD_RC}.")
  else()
    message(STATUS "Built Python file ${FILENAME}")
  endif()
endfunction()
  