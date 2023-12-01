#
# object target
SET(TGT_OBJ "Language_src")

#  T.M. elementary library definition
include_directories(${EOS_SOURCE_DIR}/Modules/Language/Src
                    ${EOS_SOURCE_DIR}/Modules/Language/Src/TypesHandling
                    ${EOS_SOURCE_DIR}/Modules/Language/Src/ObjectsHandling)
felemlib(${TGT_OBJ} LLANG_TGT_OBJ GLOB_RECURSE)
