# CoolProp - Pre-configure
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

if(NOT ${CONFIGURE_RESULT} EQUAL 0)
  message(FATAL_ERROR "Error while configuring plugin CoolProp. "
                      "See ${CMAKE_BINARY_DIR}/CoolProp_pre_configure.log")
endif()

# object target = directory name
STRING(REGEX REPLACE "(^.*/)(.*$)" "\\2" TGT_OBJ "${CMAKE_CURRENT_SOURCE_DIR}")

# T.M. elementary library definition
felemlib(${TGT_OBJ} LTM_TGT_OBJ GLOB_RECURSE)

# -------------------------------------------------------------------
# The plugin is compiled in C++17 on the OBJECT target, which is the one
# actually compiled.
# -------------------------------------------------------------------
set_target_properties(${TGT_OBJ}
  PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# Include directories of the object target. The external headers are SYSTEM
# so that -Wextra does not report the warnings of CoolProp's AbstractState.h,
# as the top-level CMakeLists.txt does for the MED headers.
target_include_directories(${TGT_OBJ} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${TGT_OBJ} SYSTEM PRIVATE ${GLOBALEXT_INC})
