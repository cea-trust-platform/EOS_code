message(STATUS "Add test LanguageTest-f77")
add_executable (LanguageTest-f77 ${CMAKE_CURRENT_SOURCE_DIR}/main.cxx ${CMAKE_CURRENT_SOURCE_DIR}/F77MAIN.F)
target_include_directories(LanguageTest-f77 PRIVATE ${EOS_BINARY_DIR}/Modules)
target_link_libraries(LanguageTest-f77 PRIVATE ${NLIB_LANG})
add_test(LanguageTest-f77 bash -c "set -o pipefail ; ${CMAKE_CURRENT_BINARY_DIR}/LanguageTest-f77  2>&1 | tee ${CMAKE_CURRENT_BINARY_DIR}/output")

