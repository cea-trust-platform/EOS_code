#
# object target = directory name : TGT_OBJ
STRING(REGEX REPLACE "(^.*/)(.*$)" "\\2" TGT_OBJ "${CMAKE_CURRENT_SOURCE_DIR}")

#  T.M. elementary library definition
if(NOT WITH_PLUGIN_FLICA4)
    felemlib(${TGT_OBJ} LCOMM_TGT_OBJ GLOB)
else()
    felemlib(${TGT_OBJ} LCOMM_TGT_OBJ GLOB_RECURSE)
endif()

if(INSTALL_TIRPC)
    add_dependencies(${TGT_OBJ} libtirpc)
endif()

include_directories(${RPC_INC})
if(BUILD_SHARED_LIB)
  target_link_libraries(${TGT_OBJ}_shar PUBLIC ${RPC_LIB})
endif()
if(BUILD_STATIC_LIB)
  target_link_libraries(${TGT_OBJ}_stat PUBLIC ${RPC_STATIC_LIB} "-lpthread")
endif()
