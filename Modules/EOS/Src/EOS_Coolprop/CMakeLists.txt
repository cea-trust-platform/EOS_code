# CoolProp version 6.4.3
# Pre-configure
#
execute_process(COMMAND ${CMAKE_COMMAND} -E echo_append "--  > Configuring plugin CoolProp  ...")
message(STATUS "PLUGIN: ${WITH_PLUGIN_COOLPROP}")
message(STATUS "PATH: ${CMAKE_CURRENT_SOURCE_DIR}")
execute_process(
    COMMAND           ${CMAKE_CURRENT_SOURCE_DIR}/CoolProp_pre_configure.sh
                      ${WITH_PLUGIN_COOLPROP}
                      ${CMAKE_CURRENT_BINARY_DIR}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    OUTPUT_FILE       ${CMAKE_BINARY_DIR}/CoolProp_pre_configure.log
    ERROR_FILE        ${CMAKE_BINARY_DIR}/CoolProp_pre_configure.log
    RESULT_VARIABLE   CONFIGURE_RESULT
)

if (NOT ${CONFIGURE_RESULT} EQUAL 0)
  message(FATAL_ERROR "Error while configuring plugin CoolProp. See ${CMAKE_BINARY_DIR}/CoolProp_pre_configure.log")
endif()

# object target = directory name : TGT_OBJ
STRING(REGEX REPLACE "(^.*/)(.*$)" "\\2" TGT_OBJ "${CMAKE_CURRENT_SOURCE_DIR}")

#  T.M. elementary library definition
felemlib(${TGT_OBJ} LTM_TGT_OBJ GLOB_RECURSE)

message (STATUS "TGB: ${TGT_OBJ}")
message (STATUS "LTM : ${LTM_TGT_OBJ}")
include_directories(${EOS_SOURCE_DIR}/Modules/EOS/Src/EOS_Coolprop ${LIBCOOLPROP_INC})

if(BUILD_SHARED_LIB)
  target_link_libraries(${TGT_OBJ}_shar PUBLIC ${LIBCOOLPROP_SHAREDLIB})
endif()
if(BUILD_STATIC_LIB)
  target_link_libraries(${TGT_OBJ}_stat PUBLIC ${LIBCOOLPROP_STATICLIB})
endif()
#target_link_libraries(${TGT_OBJ} CoolProp)
