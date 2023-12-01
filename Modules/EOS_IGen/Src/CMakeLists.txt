#
# T.M. elementary library definition
# ----------------------------------
#
# object target
set(TGT_OBJ "EOS_IGen_src")
#
include_directories(${EOS_SOURCE_DIR}/Modules/system        ${EOS_SOURCE_DIR}/Modules/Language 
                    ${EOS_SOURCE_DIR}/Modules/EOS_IGen      ${EOS_SOURCE_DIR}/Modules/EOS_IGen/Src
                    ${EOS_SOURCE_DIR}/Modules/Functions/Src ${GLOBALEXT_INC})
felemlib(${TGT_OBJ} LIPP_TGT_OBJ GLOB)


