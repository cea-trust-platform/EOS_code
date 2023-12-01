if (WITH_DOC_DOXYGEN)
  add_custom_target(doc-doxygen ALL)
  add_custom_command(COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
                     OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Doc/html/index.html
                     WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
  add_custom_target(doc-doxygen-modules
                    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/Doc/html/index.html)
  add_dependencies(doc-doxygen doc-doxygen-modules)
endif (WITH_DOC_DOXYGEN)

if (WITH_IPP)
  add_subdirectory(EOS_IGen)
endif (WITH_IPP)

add_subdirectory(Common)
add_subdirectory(Functions)
add_subdirectory(Language)
add_subdirectory(EOS)
add_subdirectory(EOS_IHM)

