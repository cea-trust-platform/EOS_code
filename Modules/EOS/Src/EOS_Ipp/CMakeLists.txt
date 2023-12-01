#
# T.M. elementary library definition
# ----------------------------------
message (STATUS " > Interpolator    ON")
#
# object target
set(TGT_OBJ "EOS_Ipp")
#
include_directories(${GLOBALEXT_INC})

felemlib(${TGT_OBJ} LTM_TGT_OBJ GLOB)
