# Pre-configure
execute_process(COMMAND ${CMAKE_COMMAND} -E echo_append "--  > Configuring plugin Refprop10 ... ")
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Refprop10 DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
execute_process(
    COMMAND           ${CMAKE_CURRENT_SOURCE_DIR}/REFPROP10_pre_configure.sh
                      ${WITH_PLUGIN_REFPROP_10}
                      ${CMAKE_CURRENT_BINARY_DIR}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    OUTPUT_FILE       ${CMAKE_BINARY_DIR}/REFPROP10_pre_configure.log
    ERROR_FILE        ${CMAKE_BINARY_DIR}/REFPROP10_pre_configure.log
    RESULT_VARIABLE   CONFIGURE_RESULT
)

if(NOT ${CONFIGURE_RESULT} EQUAL 0)
    message(FATAL_ERROR "Error while configuring plugin Refprop10. See ${CMAKE_BINARY_DIR}/REFPROP10_pre_configure.log")
endif()


#
# object target = directory name : TGT_OBJ
STRING(REGEX REPLACE "(^.*/)(.*$)" "\\2" TGT_OBJ "${CMAKE_CURRENT_SOURCE_DIR}")

#  T.M. elementary library definition
felemlib(${TGT_OBJ} LTM_TGT_OBJ GLOB_RECURSE)
