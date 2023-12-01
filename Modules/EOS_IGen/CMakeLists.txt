if (WITH_DOC_DOXYGEN)
  add_custom_command(COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
                     OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Doc/html/index.html
                     WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
  add_custom_target(doc-doxygen-eos_igen
                    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/Doc/html/index.html)
  add_dependencies(doc-doxygen doc-doxygen-eos_igen)
endif (WITH_DOC_DOXYGEN)

add_subdirectory(Src)
add_subdirectory(API)
add_subdirectory(Tests)
