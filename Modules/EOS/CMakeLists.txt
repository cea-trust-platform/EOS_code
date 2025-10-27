if (WITH_DOC_DOXYGEN)
  add_custom_command(COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
                     OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Doc/html/index.html
                     WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
  add_custom_target(doc-doxygen-eos
                    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/Doc/html/index.html)
  add_dependencies(doc-doxygen doc-doxygen-eos)
endif (WITH_DOC_DOXYGEN)

add_subdirectory(Src)
add_subdirectory(API)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
add_subdirectory(Data)
add_subdirectory(Tests)

if(WITH_PYTHON_API)
  add_subdirectory(PyAPI)
endif() 