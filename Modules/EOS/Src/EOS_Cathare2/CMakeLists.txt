# Cathare2 Version : VERSION_CATHARE2
# Pre-configure
execute_process(COMMAND ${CMAKE_COMMAND} -E echo_append "--  > Configuring plugin Cathare2  ... ")
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Cathare2/CATHARE2_patch.sh DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Cathare2)
execute_process(
    COMMAND           ${CMAKE_CURRENT_SOURCE_DIR}/CATHARE2_pre_configure.sh
                      ${WITH_PLUGIN_CATHARE2}
                      ${CMAKE_CURRENT_BINARY_DIR}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    OUTPUT_FILE       ${CMAKE_BINARY_DIR}/CATHARE2_pre_configure.log
    ERROR_FILE        ${CMAKE_BINARY_DIR}/CATHARE2_pre_configure.log
    RESULT_VARIABLE   CONFIGURE_RESULT
)

if(NOT ${CONFIGURE_RESULT} EQUAL 0)
    message(FATAL_ERROR "Error while configuring plugin Cathare2. See ${CMAKE_BINARY_DIR}/CATHARE2_pre_configure.log")
endif()


#
# object target = directory name : TGT_OBJ
STRING(REGEX REPLACE "(^.*/)(.*$)" "\\2" TGT_OBJ "${CMAKE_CURRENT_SOURCE_DIR}")

#  T.M. elementary library definition
felemlib(${TGT_OBJ} LTM_TGT_OBJ GLOB_RECURSE)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/Cathare2)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Cathare2)
