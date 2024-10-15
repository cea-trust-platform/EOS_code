# (add executable EOSTest)
message(STATUS "Add test EOSTest")
add_executable (EOSTest "${CMAKE_CURRENT_SOURCE_DIR}/main.cxx")
target_include_directories(EOSTest PRIVATE ${EOS_BINARY_DIR}/Modules)
target_link_libraries(EOSTest PRIVATE ${NLIB_LANG} ${NLIB_TM} ${NLIB_IPP})
add_test(EOSTest bash -c "set -o pipefail ; ${CMAKE_CURRENT_BINARY_DIR}/EOSTest 2>&1 | tee ${CMAKE_CURRENT_BINARY_DIR}/output")
set_tests_properties(EOSTest PROPERTIES ENVIRONMENT "USER_EOS_DATA=${CMAKE_BINARY_DIR}/Modules/EOS/Data/"
                                        COST        5.)



# (add executable EOSTestVal)
message(STATUS "Add all tests EOSTestVal")
add_executable (EOSTestVal "${CMAKE_CURRENT_SOURCE_DIR}/main_val.cxx")
target_include_directories(EOSTestVal PRIVATE ${EOS_BINARY_DIR}/Modules)
target_link_libraries(EOSTestVal PRIVATE ${NLIB_LANG} ${NLIB_TM} ${NLIB_IPP})

#---------------------------------------------------------------------
# add a test test-val-${filevalname}
#  for each file .val
#---------------------------------------------------------------------
file(GLOB listval ${CMAKE_CURRENT_BINARY_DIR}/*.val)
foreach(fileval ${listval})
  get_filename_component(filevalname ${fileval} NAME_WE)
  message(STATUS "  |test-val-${filevalname}")
  add_test(test-val-${filevalname} ${CMAKE_CURRENT_BINARY_DIR}/EOSTestVal ${fileval} ${CMAKE_CURRENT_BINARY_DIR}/${filevalname}.valeos)
  set_tests_properties(test-val-${filevalname} PROPERTIES ENVIRONMENT "USER_EOS_DATA=${CMAKE_BINARY_DIR}/Modules/EOS/Data/"
                                                          DEPENDS index-eos)
endforeach(fileval)


# (add executable EOSTestIpp)
if (WITH_IPP)
  message(STATUS "Add test EOSTestIpp")
  add_executable (EOSTestIpp  "${CMAKE_CURRENT_SOURCE_DIR}/main_ipp.cxx")
  target_include_directories(EOSTestIpp PRIVATE ${EOS_BINARY_DIR}/Modules ${GLOBALEXT_INC})
  target_link_libraries(EOSTestIpp PRIVATE ${NLIB_LANG} ${NLIB_TM} ${NLIB_IPP})
  add_test(EOSTestIpp bash -c "set -o pipefail ; ${CMAKE_CURRENT_BINARY_DIR}/EOSTestIpp 2>&1 | tee ${CMAKE_CURRENT_BINARY_DIR}/output_ipp")
  set_tests_properties(EOSTestIpp PROPERTIES DEPENDS      EOSIGenTest
                                             COST         10.
                                             ENVIRONMENT "USER_EOS_DATA=${CMAKE_BINARY_DIR}/Modules/EOS/Data/")
endif (WITH_IPP)


# (add executable EOSTest)
if (USE_OPENMP)
  message(STATUS "Add test EOSTest (OpenMP version)")
  add_executable (EOSTest_OpenMP
	  main_OpenMP.cxx
	  dump.cxx
    arguments.cxx
	  )
  target_include_directories(EOSTest_OpenMP PRIVATE ${EOS_BINARY_DIR}/Modules)
  target_link_libraries(EOSTest_OpenMP PRIVATE ${NLIB_LANG} ${NLIB_TM} ${NLIB_IPP})
  add_test(EOSTest_OpenMP bash -c "set -o pipefail ; ${CMAKE_CURRENT_BINARY_DIR}/EOSTest_OpenMP 2>&1 | tee ${CMAKE_CURRENT_BINARY_DIR}/output_openmp")
  set_tests_properties(EOSTest PROPERTIES ENVIRONMENT "USER_EOS_DATA=${CMAKE_BINARY_DIR}/Modules/EOS/Data/;OMP_NUM_THREADS=3"
                                        COST        5.)
  install(TARGETS EOSTest_OpenMP DESTINATION .)
endif()
