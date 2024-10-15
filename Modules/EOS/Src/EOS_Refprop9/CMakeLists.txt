# Pre-configure
execute_process(COMMAND ${CMAKE_COMMAND} -E echo_append "--  > Configuring plugin Refprop9  ... ")

if(ENABLE_OPENMP)
  set(USE_OPENMP "1")
else()
  set(USE_OPENMP "0")
endif()

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Refprop9 DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
execute_process(
    COMMAND           ${CMAKE_CURRENT_SOURCE_DIR}/REFPROP9_pre_configure.sh
                      ${WITH_PLUGIN_REFPROP_9}
                      ${CMAKE_CURRENT_BINARY_DIR}
                      ${USE_OPENMP}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    OUTPUT_FILE       ${CMAKE_BINARY_DIR}/REFPROP9_pre_configure.log
    ERROR_FILE        ${CMAKE_BINARY_DIR}/REFPROP9_pre_configure.log
    RESULT_VARIABLE   CONFIGURE_RESULT
)

if(NOT ${CONFIGURE_RESULT} EQUAL 0)
    message(FATAL_ERROR "Error while configuring plugin Refprop9. See ${CMAKE_BINARY_DIR}/REFPROP9_pre_configure.log")
endif()

include_directories(BEFORE ${CMAKE_CURRENT_BINARY_DIR}/Refprop9)

#
# object target = directory name : TGT_OBJ
STRING(REGEX REPLACE "(^.*/)(.*$)" "\\2" TGT_OBJ "${CMAKE_CURRENT_SOURCE_DIR}")

#  T.M. elementary library definition
felemlib(${TGT_OBJ} LTM_TGT_OBJ GLOB_RECURSE)
