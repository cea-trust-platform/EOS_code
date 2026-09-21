message(STATUS " > Tool eos_ipp_tiler")
add_executable(eos_ipp_tiler ${CMAKE_CURRENT_SOURCE_DIR}/eos_ipp_tiler.cxx)
target_include_directories(eos_ipp_tiler PRIVATE ${EOS_BINARY_DIR}/Modules ${GLOBALEXT_INC})
target_link_libraries(eos_ipp_tiler PRIVATE ${NLIB_IPP} ${NLIB_LANG} ${NLIB_TM})
# Installed alongside the other EOS tools: it was built but never installed, so
# it only existed inside a build tree.
install(TARGETS eos_ipp_tiler DESTINATION ${EOS_BIN_DIR})
