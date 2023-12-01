#
# T.M. elementary library definition
# ----------------------------------
#
# object target
set(TGT_OBJ "Language_api")
#
felemlib(${TGT_OBJ} LLANG_TGT_OBJ GLOB)


#
# T.M. general library definition  NLIB_LANG
# -------------------------------
#
SET(LSO "")
foreach(TGT_OBJ ${LLANG_TGT_OBJ})
  SET(LSO ${LSO} $<TARGET_OBJECTS:${TGT_OBJ}>)
endforeach(TGT_OBJ)

if(BUILD_SHARED_LIB)
  add_library(${NLIB_LANG}  SHARED  ${LSO})
else()
  add_library(${NLIB_LANG}  STATIC  ${LSO})
endif()

