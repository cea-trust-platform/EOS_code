message(STATUS "Add test EOSTest-f77")
add_executable (EOSTest-f77 ${CMAKE_CURRENT_SOURCE_DIR}/main.cxx ${CMAKE_CURRENT_SOURCE_DIR}/F77MAIN.f)
target_include_directories(EOSTest-f77 PRIVATE ${EOS_BINARY_DIR}/Modules ${EOS_BINARY_DIR}/Modules/Language)
target_link_libraries(EOSTest-f77 PRIVATE ${NLIB_LANG} ${NLIB_TM} ${NLIB_IPP})
add_test(EOSTest-f77 bash -c "set -o pipefail ; ${CMAKE_CURRENT_BINARY_DIR}/EOSTest-f77 2>&1 | tee ${CMAKE_CURRENT_BINARY_DIR}/output")
set_tests_properties(EOSTest-f77 PROPERTIES ENVIRONMENT "USER_EOS_DATA=${CMAKE_BINARY_DIR}/Modules/EOS/Data/")
