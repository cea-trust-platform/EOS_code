# Copy documentation sources (except Sphinx)
file(COPY
add_subdirectory(./GUIGuide)
  ${CMAKE_CURRENT_SOURCE_DIR}
add_subdirectory(./Environnement)
  DESTINATION ${CMAKE_BINARY_DIR}
add_subdirectory(./UsersGuide)
  PATTERN "Doc_EOS" EXCLUDE
add_subdirectory(./ImplementersGuide)
)
add_subdirectory(./DeveloppersGuide)
add_subdirectory(./Components)
add_subdirectory(GUIGuide)
add_subdirectory(./Interpolator)
add_subdirectory(Environnement)
add_subdirectory(UsersGuide)
add_subdirectory(ImplementersGuide)
add_subdirectory(DeveloppersGuide)
add_subdirectory(Components)
add_subdirectory(Interpolator)
# Sphinx documentation (Doc_EOS)
if (WITH_DOC_SPHINX)
  set(DOC_EOS_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Doc_EOS)
  set(DOC_EOS_BIN_DIR ${CMAKE_BINARY_DIR}/Doc/Doc_EOS)
  file(COPY
    ${DOC_EOS_SRC_DIR}
    DESTINATION ${CMAKE_BINARY_DIR}/Doc
  )
  add_subdirectory(Doc_EOS)
endif ()