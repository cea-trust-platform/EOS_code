# Thermodynamic Model Plugins
# ---------------------------
#
#
message ("")
message (STATUS " --- Thermodynamic Model Plugins")

#
# object target
SET(TGT_OBJ "EOS_src")

#  T.M. elementary library definition
felemlib(${TGT_OBJ} LTM_TGT_OBJ GLOB)

#
# model-version.txt
set(FILEMV "${CMAKE_SOURCE_DIR}/Modules/EOS/model-version.txt")

add_subdirectory(EOS_PerfectGas)

add_subdirectory(EOS_StiffenedGas)

if (WITH_IPP)
  add_subdirectory(EOS_Ipp)
else (WITH_IPP)
  message (STATUS " > Interpolator  OFF")
endif (WITH_IPP)

if(WITH_PLUGIN_CATHARE)
  add_subdirectory(EOS_Cathare)
  execute_process(COMMAND ${CMAKE_COMMAND} -E echo "Done")
else (WITH_PLUGIN_CATHARE)
  message (STATUS " > Plug-Cathare    OFF")
endif(WITH_PLUGIN_CATHARE)

if(WITH_PLUGIN_CATHARE2)
  add_subdirectory(EOS_Cathare2)
  execute_process(COMMAND ${CMAKE_COMMAND} -E echo "Done")
else (WITH_PLUGIN_CATHARE2)
  message (STATUS " > Plug-Cathare2   OFF")
endif(WITH_PLUGIN_CATHARE2)

if(WITH_PLUGIN_REFPROP_9)
  add_subdirectory(EOS_Refprop9)
  execute_process(COMMAND ${CMAKE_COMMAND} -E echo "Done")
else (WITH_PLUGIN_REFPROP_9)
  message (STATUS " > Plug-Refprop9   OFF")
endif(WITH_PLUGIN_REFPROP_9)

if(WITH_PLUGIN_REFPROP_10)
  add_subdirectory(EOS_Refprop10)
  execute_process(COMMAND ${CMAKE_COMMAND} -E echo "Done")
else (WITH_PLUGIN_REFPROP_10)
  message (STATUS " > Plug-Refprop10  OFF")
endif(WITH_PLUGIN_REFPROP_10)

if(WITH_PLUGIN_THETIS)
  add_subdirectory(EOS_Thetis)
  execute_process(COMMAND ${CMAKE_COMMAND} -E echo "Done")
else (WITH_PLUGIN_THETIS)
  message (STATUS " > Plug-Thetis     OFF")
endif(WITH_PLUGIN_THETIS)

if(WITH_PLUGIN_HITEC)
  add_subdirectory(EOS_Hitec)
  execute_process(COMMAND ${CMAKE_COMMAND} -E echo "Done")
else (WITH_PLUGIN_HITEC)
  message (STATUS " > Plug-Hitec      OFF")
endif(WITH_PLUGIN_HITEC)

if(WITH_PLUGIN_NAK)
  add_subdirectory(EOS_Nak)
  execute_process(COMMAND ${CMAKE_COMMAND} -E echo "Done")
else (WITH_PLUGIN_NAK)
  message (STATUS " > Plug-Nak        OFF")
endif(WITH_PLUGIN_NAK)

if(WITH_PLUGIN_FLICA4)
  add_subdirectory(EOS_Flica4)
  execute_process(COMMAND ${CMAKE_COMMAND} -E echo "Done")
else (WITH_PLUGIN_FLICA4)
  message (STATUS " > Plug-Flica4     OFF")
endif(WITH_PLUGIN_FLICA4)

if(WITH_PLUGIN_COOLPROP)
  add_subdirectory(EOS_CoolProp)
  execute_process(COMMAND ${CMAKE_COMMAND} -E echo "Done")
else(WITH_PLUGIN_COOLPROP)
  message (STATUS " > Plug-CoolProp   OFF")
endif(WITH_PLUGIN_COOLPROP)

message ("")
