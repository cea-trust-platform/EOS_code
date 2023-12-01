#
# object target = directory name : TGT_OBJ
STRING(REGEX REPLACE "(^.*/)(.*$)" "\\2" TGT_OBJ "${CMAKE_CURRENT_SOURCE_DIR}")

#  T.M. elementary library definition
felemlib(${TGT_OBJ} LTM_TGT_OBJ GLOB)
