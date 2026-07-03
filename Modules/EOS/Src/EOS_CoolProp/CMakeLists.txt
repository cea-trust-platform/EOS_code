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
# CoolProp requiert C++17 (std::string_view dans DataStructures.h)
# La compilation réelle se fait sur la cible OBJECT  c'est elle qu'on cible
# -------------------------------------------------------------------
set_target_properties(${TGT_OBJ}
  PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)
target_compile_definitions(${TGT_OBJ} PRIVATE COOLPROP_LIB_STATIC)

# Spécification des répertoires d'inclusion directement sur la cible d'objets
target_include_directories(${TGT_OBJ} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${GLOBALEXT_INC}
)
