#
# T.M. elementary library definition
# ----------------------------------
#
# object target
set(TGT_OBJ "EOS_IGen_api")
#
felemlib(${TGT_OBJ} LIPP_TGT_OBJ GLOB)


#
# T.M. general library definition  NLIB_IPP
# -------------------------------
#
# include directories
include_directories(${EOS_SOURCE_DIR}/Modules/system        ${EOS_SOURCE_DIR}/Modules/Language
                    ${EOS_SOURCE_DIR}/Modules/EOS_IGen      ${EOS_SOURCE_DIR}/Modules/EOS_IGen/Src
                    ${EOS_SOURCE_DIR}/Modules/Functions/Src ${GLOBALEXT_INC})

SET(LSO "")
foreach(TGT_OBJ ${LIPP_TGT_OBJ})
  SET(LSO ${LSO} $<TARGET_OBJECTS:${TGT_OBJ}>)
endforeach(TGT_OBJ)

if(BUILD_SHARED_LIB)
  add_library(${NLIB_IPP}  SHARED  ${LSO})
else()
  add_library(${NLIB_IPP}  STATIC  ${LSO})
endif()

target_link_libraries(${NLIB_IPP} ${GLOBALEXT_LIB})
