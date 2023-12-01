message(STATUS "Add test LanguageTest")
add_executable (LanguageTest ${CMAKE_CURRENT_SOURCE_DIR}/main.cxx)
target_include_directories(LanguageTest PRIVATE ${EOS_BINARY_DIR}/Modules)
target_link_libraries(LanguageTest PRIVATE ${NLIB_LANG})
add_test(LanguageTest bash -c "set -o pipefail ; ${CMAKE_CURRENT_BINARY_DIR}/LanguageTest <${CMAKE_CURRENT_SOURCE_DIR}/input 2>&1 | tee ${CMAKE_CURRENT_BINARY_DIR}/output")

