set(RUN_ENV "USER_EOS_DATA=${CMAKE_BINARY_DIR}/Modules/EOS/Data/")

if (USE_OPENMP)
  set(RUN_ENV "${RUN_ENV};OMP_NUM_THREADS=3;OMP_STACKSIZE=16M")
endif()

message(STATUS "RUN_ENV: ${RUN_ENV}")

# (add executable EOSTest)
message(STATUS "Add test EOSTest")
add_executable (EOSTest "${CMAKE_CURRENT_SOURCE_DIR}/main.cxx")
target_include_directories(EOSTest PRIVATE ${EOS_BINARY_DIR}/Modules)
target_link_libraries(EOSTest PRIVATE ${NLIB_LANG} ${NLIB_TM} ${NLIB_IPP})
add_test(EOSTest bash -c "set -o pipefail ; OMP_NUM_THREADS=3 OMP_STACKSIZE=16M ${CMAKE_CURRENT_BINARY_DIR}/EOSTest 2>&1 | tee ${CMAKE_CURRENT_BINARY_DIR}/output")
set_tests_properties(EOSTest PROPERTIES ENVIRONMENT ${RUN_ENV}
                                        COST        5.)
                                        
                                        
                                        
                                        
                                        
# ( Add executable EOS_REFPROP9) 
message(STATUS "Add all tests EOSTestVal")
add_executable (EOSTestRefprop9 "${CMAKE_CURRENT_SOURCE_DIR}/main_Refprop9_Water_Liquid.cxx")
target_include_directories(EOSTestRefprop9 PRIVATE ${EOS_BINARY_DIR}/Modules)
target_link_libraries(EOSTestRefprop9 PRIVATE ${NLIB_LANG} ${NLIB_TM} ${NLIB_IPP})


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
  set_tests_properties(test-val-${filevalname} PROPERTIES ENVIRONMENT "OMP_NUM_THREADS=3;OMP_STACKSIZE=16M;${RUN_ENV}"
                                                          DEPENDS index-eos)
endforeach(fileval)


# (add executable EOSTestIpp)
if (WITH_IPP)
  message(STATUS "Add test EOSTestIpp")
  add_executable (EOSTestIpp  "${CMAKE_CURRENT_SOURCE_DIR}/main_ipp.cxx")
  target_include_directories(EOSTestIpp PRIVATE ${EOS_BINARY_DIR}/Modules ${GLOBALEXT_INC})
  target_link_libraries(EOSTestIpp PRIVATE ${NLIB_LANG} ${NLIB_TM} ${NLIB_IPP})
  add_test(EOSTestIpp bash -c "set -o pipefail ; OMP_NUM_THREADS=3 OMP_STACKSIZE=16M ${CMAKE_CURRENT_BINARY_DIR}/EOSTestIpp 2>&1 | tee ${CMAKE_CURRENT_BINARY_DIR}/output_ipp")
  set_tests_properties(EOSTestIpp PROPERTIES DEPENDS      EOSIGenTest
                                             COST         10.
                                             ENVIRONMENT ${RUN_ENV})

  message(STATUS "Add test EOSTestIppBicubic")
  add_executable (EOSTestIppBicubic  "${CMAKE_CURRENT_SOURCE_DIR}/main_ipp_bicubic.cxx")
  target_include_directories(EOSTestIppBicubic PRIVATE ${EOS_BINARY_DIR}/Modules ${GLOBALEXT_INC})
  target_link_libraries(EOSTestIppBicubic PRIVATE ${NLIB_LANG} ${NLIB_TM} ${NLIB_IPP})
  add_test(EOSTestIppBicubic bash -c "set -o pipefail ; OMP_NUM_THREADS=3 OMP_STACKSIZE=16M ${CMAKE_CURRENT_BINARY_DIR}/EOSTestIppBicubic 2>&1 | tee ${CMAKE_CURRENT_BINARY_DIR}/output_ipp_bicubic")
  set_tests_properties(EOSTestIppBicubic PROPERTIES DEPENDS      EOSIGenTest
                                                    COST         10.
                                                    ENVIRONMENT ${RUN_ENV})

  # EOS_Ipp tiled ("streaming") mode: self-contained (generates its own
  # fixtures at run time via EOS_Ipp_Tiler, cf. Modules/EOS_IGen/Src), so
  # unlike EOSTestIpp/EOSTestIppBicubic it does not DEPENDS on EOSIGenTest.
  message(STATUS "Add test EOSTestIppTiled")
  add_executable (EOSTestIppTiled  "${CMAKE_CURRENT_SOURCE_DIR}/main_ipp_tiled.cxx")
  target_include_directories(EOSTestIppTiled PRIVATE ${EOS_BINARY_DIR}/Modules ${GLOBALEXT_INC})
  target_link_libraries(EOSTestIppTiled PRIVATE ${NLIB_LANG} ${NLIB_TM} ${NLIB_IPP})
  add_test(EOSTestIppTiled bash -c "set -o pipefail ; OMP_NUM_THREADS=3 OMP_STACKSIZE=16M ${CMAKE_CURRENT_BINARY_DIR}/EOSTestIppTiled 2>&1 | tee ${CMAKE_CURRENT_BINARY_DIR}/output_ipp_tiled")
  set_tests_properties(EOSTestIppTiled PROPERTIES COST        5.
                                                   ENVIRONMENT ${RUN_ENV})

  # Continuity across hanging nodes: the property the local-refinement
  # algorithm exists to guarantee (cf. Doc/Interpolator, §Continuité and the
  # "Test de continuité" appendix, which demonstrated it on a plot and left it
  # unchecked). Self-contained: generates its own refined fixtures, from
  # EOS_Cathare2, so it is skipped when that plugin is not built.
  message(STATUS "Add test EOSTestIppContinuity")
  add_executable (EOSTestIppContinuity "${CMAKE_CURRENT_SOURCE_DIR}/main_ipp_continuity.cxx")
  target_include_directories(EOSTestIppContinuity PRIVATE ${EOS_BINARY_DIR}/Modules ${GLOBALEXT_INC})
  target_link_libraries(EOSTestIppContinuity PRIVATE ${NLIB_LANG} ${NLIB_TM} ${NLIB_IPP})
  add_test(EOSTestIppContinuity bash -c "set -o pipefail ; ${CMAKE_CURRENT_BINARY_DIR}/EOSTestIppContinuity 2>&1 | tee ${CMAKE_CURRENT_BINARY_DIR}/output_ipp_continuity")
  set_tests_properties(EOSTestIppContinuity PROPERTIES COST        20.
                                                       ENVIRONMENT ${RUN_ENV})
  if (NOT WITH_PLUGIN_CATHARE2)
    message(STATUS "EOSTestIppContinuity disabled: needs the Cathare2 plugin")
    set_tests_properties(EOSTestIppContinuity PROPERTIES DISABLED TRUE)
  endif (NOT WITH_PLUGIN_CATHARE2)

  # Performance bench and value-regression harness. Not registered as a ctest
  # test: it is a measuring instrument, run by hand around a change
  # (--dump before, --check after), and its numbers only mean something when
  # the machine is otherwise idle.
  message(STATUS "Add tool eos_ipp_bench")
  add_executable (eos_ipp_bench  "${CMAKE_CURRENT_SOURCE_DIR}/main_ipp_bench.cxx")
  target_include_directories(eos_ipp_bench PRIVATE ${EOS_BINARY_DIR}/Modules ${GLOBALEXT_INC})
  target_link_libraries(eos_ipp_bench PRIVATE ${NLIB_LANG} ${NLIB_TM} ${NLIB_IPP})

  # Concurrency regression: one EOS_Ipp instance per thread, single-file and
  # tiled, results compared against the serial run. Needs OpenMP to mean
  # anything (without it the test still builds and runs, serially).
  if (USE_OPENMP)
    message(STATUS "Add test EOSTestIppOpenMP")
    add_executable (EOSTestIppOpenMP  "${CMAKE_CURRENT_SOURCE_DIR}/main_ipp_openmp.cxx")
    target_include_directories(EOSTestIppOpenMP PRIVATE ${EOS_BINARY_DIR}/Modules ${GLOBALEXT_INC})
    target_link_libraries(EOSTestIppOpenMP PRIVATE ${NLIB_LANG} ${NLIB_TM} ${NLIB_IPP})
    add_test(EOSTestIppOpenMP bash -c "set -o pipefail ; OMP_NUM_THREADS=8 OMP_STACKSIZE=16M ${CMAKE_CURRENT_BINARY_DIR}/EOSTestIppOpenMP 2>&1 | tee ${CMAKE_CURRENT_BINARY_DIR}/output_ipp_openmp")
    set_tests_properties(EOSTestIppOpenMP PROPERTIES COST        5.
                                                     ENVIRONMENT ${RUN_ENV})
  endif (USE_OPENMP)
endif (WITH_IPP)


# (add executable EOSTest OpenMP version)
if (USE_OPENMP)
  message(STATUS "Add test EOSTest (OpenMP version)")
  add_executable (EOSTest_OpenMP
	  main_OpenMP.cxx
	  dump.cxx
          arguments.cxx
	  )
  target_include_directories(EOSTest_OpenMP PRIVATE ${EOS_BINARY_DIR}/Modules)
  target_link_libraries(EOSTest_OpenMP PRIVATE ${NLIB_LANG} ${NLIB_TM} ${NLIB_IPP})
  add_test(EOSTest_OpenMP bash -c "set -o pipefail ; \
  USER_EOS_DATA=${CMAKE_BINARY_DIR}/Modules/EOS/Data OMP_NUM_THREADS=3 OMP_STACKSIZE=16M ${CMAKE_CURRENT_BINARY_DIR}/EOSTest_OpenMP 2>&1 | tee ${CMAKE_CURRENT_BINARY_DIR}/output_openmp")

  set_tests_properties(EOSTest_OpenMP PROPERTIES ENVIRONMENT ${RUN_ENV}
                                                 COST        5.)
endif()


# (add executable EOSBench : performance benchmark, not part of ctest)
message(STATUS "Add benchmark EOSBench")
add_executable (EOSBench EXCLUDE_FROM_ALL "${CMAKE_CURRENT_SOURCE_DIR}/main_bench.cxx")
target_include_directories(EOSBench PRIVATE ${EOS_BINARY_DIR}/Modules)
target_link_libraries(EOSBench PRIVATE ${NLIB_LANG} ${NLIB_TM} ${NLIB_IPP})
