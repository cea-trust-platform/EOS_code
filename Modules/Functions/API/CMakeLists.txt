#
# T.M. elementary library definition
# ----------------------------------
#
# object target
set(TGT_OBJ "Functions_api")
#
felemlib(${TGT_OBJ} LFUNC_TGT_OBJ GLOB)


#
# T.M. general library definition  NLIB_FUNC
# -------------------------------
#
SET(LSO "")
foreach(TGT_OBJ ${LFUNC_TGT_OBJ})
  SET(LSO ${LSO} $<TARGET_OBJECTS:${TGT_OBJ}>)
endforeach(TGT_OBJ)



if(BUILD_SHARED_LIB)
  add_library(${NLIB_FUNC}  SHARED  ${LSO})
else()
  add_library(${NLIB_FUNC}  STATIC  ${LSO})
  target_link_libraries(${NLIB_FUNC} PRIVATE ${NLIB_LANG})
endif()

