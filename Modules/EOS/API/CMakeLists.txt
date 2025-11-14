#
# T.M. elementary library definition
# ----------------------------------
#
# object target
set(TGT_OBJ "EOS_api")
#
felemlib(${TGT_OBJ} LTM_TGT_OBJ GLOB)


#
# T.M. general library definition  NLIB_TM
# -------------------------------
#
# include directories
include_directories(${GLOBALEXT_INC})
#
SET(LSO "")
foreach(TGT_OBJ  ${LCOMM_TGT_OBJ} ${LTM_TGT_OBJ})
  SET(LSO ${LSO} $<TARGET_OBJECTS:${TGT_OBJ}>)
endforeach(TGT_OBJ)

if(BUILD_SHARED_LIB)
  add_library(${NLIB_TM}  SHARED  ${LSO})
else()
  add_library(${NLIB_TM}  STATIC  ${LSO})
endif()

if(INSTALL_TIRPC)
   add_dependencies(${NLIB_TM} libtirpc)
endif()

target_link_libraries(${NLIB_TM} ${LIBCOOLPROP_SHAREDLIB} ${RPC_LIB})

