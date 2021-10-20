function(BIN_EXISTS NAME EXIT_CODE)
  execute_process(COMMAND which ${NAME} RESULT_VARIABLE EXIT_CODE)
endfunction()

function(PY_COMPILE FILENAME)
  file(COPY ${FILENAME}
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR} 
    USE_SOURCE_PERMISSIONS)

  execute_process(
    COMMAND python3 -m py_compile ${CMAKE_CURRENT_BINARY_DIR}/${FILENAME}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    RESULT_VARIABLE PY_COMPILE_RC)
  
  if (NOT PY_COMPILE_RC EQUAL 0)
    message(FATAL_ERROR 
      "Python compile of ${FILENAME} exited with return code "
      "${PY_COMPILE_RC}.")
  endif()
endfunction()
  