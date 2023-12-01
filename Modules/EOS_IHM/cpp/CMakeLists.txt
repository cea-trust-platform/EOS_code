option (WITH_GUI "Add GUI for EOS" ON)
message (STATUS "Info_global gui " ${WITH_GUI})

if (WITH_GUI)
  if(NOT BUILD_SHARED_LIB)
      message(WARNING "Warning, you deactivated shared libraries, but the GUI will build one. "
                      "If an error occurs, try the configure option --without-gui or the CMake option -DWITH_GUI=OFF")
  endif()

  include(EOS_Python)

#  -- eos_gui (install)     
  set (EOS_GUI_SRC ${CMAKE_SOURCE_DIR}/Modules/EOS_IHM/qt)
  set (EOS_GUI_BINARY_DIR ${CMAKE_BINARY_DIR}/Modules/EOS_IHM/qt)
  set (EOS_GUI_PY ${EOS_PYTHON_SITE_PACKAGES_DIR}/eosMain4.py)
  set (EOS_DOC_GUI_DIR ${EOS_DOC_DIR}/Modules/EOS_IHM)
  configure_file(${EOS_GUI_SRC}/eosPrint4.py.in
                 ${EOS_GUI_BINARY_DIR}/eosPrint4.py @ONLY)   
  configure_file(${EOS_GUI_SRC}/eosHelp4.py.in
                 ${EOS_GUI_BINARY_DIR}/eosHelp4.py @ONLY)   
  configure_file(${EOS_GUI_SRC}/eosGUI4a.py.in
                 ${EOS_GUI_BINARY_DIR}/eosGUI4a.py @ONLY)   
  set (EOS_LIB ${EOS_LIB_DIR})
  configure_file(${CMAKE_SOURCE_DIR}/Tools/eos_gui.in
                 ${CMAKE_BINARY_DIR}/Tools/eos_gui @ONLY)
  configure_file(${EOS_GUI_SRC}/eosAva.py.in
                 ${EOS_GUI_BINARY_DIR}/eosAva.py @ONLY)   


#  -- swig --
  message(STATUS " EOS GUI : configuration")
  find_package(SWIG)

  # Index ID_SWIG
  set(ID_SWIG 0)
  if (SWIG_FOUND)
     set(ID_SWIG 1)
  else (SWIG_FOUND)
     if (EXISTS ${SWIG_EXECUTABLE})
        set(ID_SWIG 1)
     endif (EXISTS ${SWIG_EXECUTABLE})
  endif (SWIG_FOUND)

  if (ID_SWIG)
     message(STATUS " - Swig executable is  ${SWIG_EXECUTABLE}")
  else (ID_SWIG)
     message(FATAL_ERROR "Swig is required to build GUI")
  endif (ID_SWIG)

  #
  # Shared library  ${NLIB_IHM}
  # compilation option
  if (EXISTS ${MED_LIB_DIR})
     add_definitions(-DWITHMED)
  endif (EXISTS ${MED_LIB_DIR})
  # include
  include_directories(${EOS_SOURCE_DIR}/Modules ${EOS_SOURCE_DIR}/Modules/EOS 
                      ${EOS_SOURCE_DIR}/Modules/Language 
                      ${GLOBALEXT_INC} ${Python_INCLUDE_DIRS})
  # source list
  set(SRCS ${CMAKE_CURRENT_SOURCE_DIR}/eosihmSrc1.cxx
           ${CMAKE_CURRENT_BINARY_DIR}/eosihm_wrap.cxx)
  if (EXISTS ${MED_LIB_DIR})
     list(APPEND SRCS ${CMAKE_CURRENT_SOURCE_DIR}/med/eos2med.f)
  endif (EXISTS ${MED_LIB_DIR})
  # objects list
  set(LSO "")
  foreach(TGT_OBJ ${LCOMM_TGT_OBJ} ${LLANG_TGT_OBJ} ${LTM_TGT_OBJ} ${LIPP_TGT_OBJ})
    set(LSO ${LSO} $<TARGET_OBJECTS:${TGT_OBJ}>)
  endforeach(TGT_OBJ)
  # library
  add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/eosihm_wrap.cxx ${CMAKE_CURRENT_BINARY_DIR}/eosihm.py
                     COMMAND ${SWIG_EXECUTABLE} ARGS -shadow -c++ -python
                                                     -outdir ${CMAKE_CURRENT_BINARY_DIR}
                                                     -o ${CMAKE_CURRENT_BINARY_DIR}/eosihm_wrap.cxx
                                                     ${CMAKE_CURRENT_SOURCE_DIR}/eosihm.i)
  if     (${RES_VAR})
    message(FATAL_ERROR " ! Problem SWIG. perhaps you may choose the option --without-gui")
  endif  (${RES_VAR})
  add_library(${NLIB_IHM} SHARED ${SRCS} ${LSO})
  target_link_libraries(${NLIB_IHM} PUBLIC ${GLOBALEXT_LIB} ${Python_LIBRARIES})
  target_include_directories(${NLIB_IHM} PUBLIC ${CMAKE_BINARY_DIR}/Modules/EOS/API ${CMAKE_BINARY_DIR}/Modules ${CMAKE_CURRENT_SOURCE_DIR})

else (WITH_GUI)
  configure_file(${CMAKE_SOURCE_DIR}/Tools/eos_gui.in2 ${CMAKE_BINARY_DIR}/Tools/eos_gui COPYONLY)
  set (EOS_GUI_SRC ${CMAKE_SOURCE_DIR}/Modules/EOS_IHM/qt)
  configure_file(${EOS_GUI_SRC}/eosAva.py.in ${CMAKE_BINARY_DIR}/Modules/EOS_IHM/qt/eosAva.py @ONLY)
endif (WITH_GUI)
