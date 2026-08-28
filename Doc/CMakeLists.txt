# Copy documentation sources (except Sphinx)
file(COPY
  ${CMAKE_CURRENT_SOURCE_DIR}
  DESTINATION ${CMAKE_BINARY_DIR}
  PATTERN "sphinx" EXCLUDE
)

add_subdirectory(GUIGuide)
add_subdirectory(Environnement)
add_subdirectory(UsersGuide)
add_subdirectory(ImplementersGuide)
add_subdirectory(DeveloppersGuide)
add_subdirectory(Components)
add_subdirectory(Interpolator)

# Sphinx documentation
if (WITH_DOC_SPHINX)
  add_subdirectory(sphinx)
endif ()
