cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
#
# Project name
project(EOS VERSION 2.0.0 LANGUAGES CXX C Fortran)
#
# Include CMAKE
# - path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/Tools")
# - include
include(func_def)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_INSTALL_MESSAGE LAZY)

find_package(Python QUIET REQUIRED COMPONENTS Interpreter)

# Initialisation
# --------------
# - set EOS version
set(EOSFILES Tools/eos.in Readme/en/Changelog.txt Readme/fr/Changelog.txt index.html Modules/index.html Modules/EOS/API/EOS.cxx)
file(GLOB_RECURSE DOXYFILES RELATIVE ${CMAKE_SOURCE_DIR} Doxyfile)
set(EOSFILES ${EOSFILES} ${DOXYFILES})
foreach(EOSFILE ${EOSFILES} )
  configure_file(${CMAKE_SOURCE_DIR}/${EOSFILE} ${CMAKE_BINARY_DIR}/${EOSFILE} @ONLY)
endforeach(EOSFILE)

if (ENABLE_OPENMP)
  find_package(OpenMP)
  if(OpenMP_CXX_FOUND AND OpenMP_Fortran_FOUND)
    set(USE_OPENMP ON)
    message(STATUS "Use OpenMP ${USE_OPENMP} (version ${OpenMP_CXX_VERSION})")
  else()
    set(USE_OPENMP OFF)
    message(STATUS "OpenMP not found")
  endif()
endif()

# - subdirectories
set(EOS_INSTALL_DIR ${CMAKE_INSTALL_PREFIX})
set(EOS_BIN_DIR     ${CMAKE_INSTALL_PREFIX}/bin)
set(EOS_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include)
set(EOS_LIB_DIR     ${CMAKE_INSTALL_PREFIX}/lib)
set(EOS_DATA_DIR    ${CMAKE_INSTALL_PREFIX}/data)
set(EOS_DOC_DIR     ${CMAKE_INSTALL_PREFIX}/doc)
set(EOS_DOC_TM_DIR  ${EOS_DOC_DIR}/Thermo_Models)
#
# Test with CTest
enable_testing()
#
#
message ("")
message (STATUS " --- EOS version          :  ${EOS_VERSION}")
message (STATUS " > Source    directory    :  ${CMAKE_SOURCE_DIR}")
message (STATUS " > Execution directory    :  ${CMAKE_BINARY_DIR}")
message (STATUS " > Installation directory :  ${EOS_INSTALL_DIR}")
message ("")
#
# Some CMAKE information
message (STATUS " --- CMAKE version        :  ${CMAKE_VERSION}")
message (STATUS " > O.S. version           :  ${CMAKE_SYSTEM_NAME} - ${CMAKE_SYSTEM_VERSION} - ${CMAKE_SYSTEM_PROCESSOR}")
message (STATUS " > C++ compiler           :  ${CMAKE_CXX_COMPILER}")
message (STATUS " > C   compiler           :  ${CMAKE_C_COMPILER}")
message (STATUS " > Fortran compiler       :  ${CMAKE_Fortran_COMPILER}")
message ("")


#
# Plugins
set(WITH_PLUGIN_CATHARE    "" CACHE PATH "Defines the root of Cathare plugin sources")
set(WITH_PLUGIN_CATHARE2   "" CACHE PATH "Defines the root of Cathare2 plugin sources")
set(WITH_PLUGIN_HITEC      "" CACHE PATH "Defines the root of Hitec plugin sources")
set(WITH_PLUGIN_NAK        "" CACHE PATH "Defines the root of Nak plugin sources")
set(WITH_PLUGIN_THETIS     "" CACHE PATH "Defines the root of Thetis plugin sources")
set(WITH_PLUGIN_REFPROP_9  "" CACHE PATH "Defines the root of Refprop9 plugin sources")
set(WITH_PLUGIN_REFPROP_10 "" CACHE PATH "Defines the root of Refprop10 plugin sources")
set(WITH_PLUGIN_FLICA4     "" CACHE PATH "Defines the root of Flica4 plugin sources")


# Prerequisites
# -------------
set(pc_libs_private "")
set(pc_req_private_list "")

#
# MPI
if (USER_MPI_PATH)
  message(STATUS "mpi path specified by user : ${USER_MPI_PATH}")
  set (MPI_INC ${USER_MPI_PATH}/include)
  set (MPI_LIB_DIR ${USER_MPI_PATH}/lib)
  set(ENV{LD_LIBRARY_PATH} ${USER_MPI_PATH}/lib/:$ENV{LD_LIBRARY_PATH} )
  message(STATUS "option --with-mpi=<path> adds ${USER_MPI_PATH}/lib to LD_LIBRARY_PATH")
endif (USER_MPI_PATH)
if (USER_MPI_INCLUDE_PATH)
  message(STATUS "mpi include path specified by user : ${USER_MPI_INCLUDE_PATH}")
  set (MPI_INC ${USER_MPI_INCLUDE_PATH})
endif (USER_MPI_INCLUDE_PATH)
if (USER_MPI_LIB_PATH)
  message(STATUS "mpi library path specified by user : ${USER_MPI_LIB_PATH}")
  set (MPI_LIB_DIR ${USER_MPI_LIB_PATH})
endif (USER_MPI_LIB_PATH)
if (MPI_LIB_DIR)
  find_library(MPI_LIB1  NAMES mpi     PATHS ${MPI_LIB_DIR} NO_DEFAULT_PATH)
  find_library(MPI_LIB2  NAMES mpi_cxx PATHS ${MPI_LIB_DIR} NO_DEFAULT_PATH)
  set (MPI_LIB ${MPI_LIB1} ${MPI_LIB2})
  if (NOT MPI_LIB)
     message(FATAL_ERROR "Not found MPI libraries after MPI user request")
  endif (NOT MPI_LIB)
  list(APPEND pc_req_private_list "ompi ompi-cxx")
endif (MPI_LIB_DIR)
#
# HDF5
if (USER_HDF5_PATH)
  message(STATUS "hdf5 path specified by user : ${USER_HDF5_PATH}")
  set (HDF5_INC ${USER_HDF5_PATH}/include)
  set (HDF5_LIB_DIR ${USER_HDF5_PATH}/lib)
  set(ENV{LD_LIBRARY_PATH} ${USER_HDF5_PATH}/lib/:$ENV{LD_LIBRARY_PATH} )
  message(STATUS "option --with-hdf5=<path> adds ${USER_HDF5_PATH}/lib to LD_LIBRARY_PATH")
endif (USER_HDF5_PATH)
if (USER_HDF5_LIB_PATH)
  message(STATUS "hdf5 library path specified by user : ${USER_HDF5_LIB_PATH}")
  set (HDF5_LIB_DIR ${USER_HDF5_LIB_PATH})
endif (USER_HDF5_LIB_PATH)
if (HDF5_LIB_DIR)
  set (CMAKE_FIND_LIBRARY_SUFFIXES .so)
  find_library(HDF5_SHAREDLIB  NAMES hdf5 PATHS ${HDF5_LIB_DIR} NO_DEFAULT_PATH)
  set (HDF5_SHAREDLIB ${HDF5_SHAREDLIB} -L${HDF5_LIB_DIR} -lhdf5)
  if (NOT HDF5_SHAREDLIB)
     message(FATAL_ERROR "Not found HDF5 shared library after HDF5 user request")
  endif (NOT HDF5_SHAREDLIB)

  set (CMAKE_FIND_LIBRARY_SUFFIXES .a)
  find_library(HDF5_STATICLIB  NAMES hdf5 PATHS ${HDF5_LIB_DIR} NO_DEFAULT_PATH)
  if (NOT HDF5_STATICLIB)
     # message(FATAL_ERROR "Not found HDF5 static library after HDF5 user request")
     set(HDF5_STATICLIB ${HDF5_SHAREDLIB})
  endif (NOT HDF5_STATICLIB)
  set (CMAKE_FIND_LIBRARY_SUFFIXES .so)
  list(APPEND pc_req_private_list "hdf5")
endif (HDF5_LIB_DIR)
#
# MED
if (USER_MED_PATH)
  message(STATUS "med path specified by user : ${USER_MED_PATH}")
  set (MED_INC ${USER_MED_PATH}/include)
  set (MED_LIB_DIR ${USER_MED_PATH}/lib)
  set(ENV{LD_LIBRARY_PATH} ${USER_MED_PATH}/lib/:$ENV{LD_LIBRARY_PATH} )
  message(STATUS "option --with-med=<path> adds ${USER_MED_PATH}/lib to LD_LIBRARY_PATH")
endif(USER_MED_PATH)
if (USER_MED_INCLUDE_PATH)
  message(STATUS "med include path specified by user : ${USER_MED_INCLUDE_PATH}")
  set (MED_INC ${USER_MED_INCLUDE_PATH})
endif (USER_MED_INCLUDE_PATH)
if (USER_MED_LIB_PATH)
    message(STATUS "med library path specified by user : ${USER_MED_LIB_PATH}")
    set (MED_LIB_DIR ${USER_MED_LIB_PATH})
endif (USER_MED_LIB_PATH)
if (MED_LIB_DIR)
  set (CMAKE_FIND_LIBRARY_SUFFIXES .a)
  find_library(MED_STATICLIB1 NAMES med       PATHS ${MED_LIB_DIR} NO_DEFAULT_PATH)
  find_library(MED_STATICLIB2 NAMES medC      PATHS ${MED_LIB_DIR} NO_DEFAULT_PATH)
  find_library(MED_STATICLIB3 NAMES medimport PATHS ${MED_LIB_DIR} NO_DEFAULT_PATH)
  set (MED_STATICLIB ${MED_STATICLIB1} ${MED_STATICLIB2} ${MED_STATICLIB3})
  if (NOT MED_STATICLIB)
     message(FATAL_ERROR "Not found MED static library after MED user request")
  endif (NOT MED_STATICLIB)

  set (CMAKE_FIND_LIBRARY_SUFFIXES .so)
  find_library(MED_SHAREDLIB1 NAMES med       PATHS ${MED_LIB_DIR} NO_DEFAULT_PATH)
  find_library(MED_SHAREDLIB2 NAMES medC      PATHS ${MED_LIB_DIR} NO_DEFAULT_PATH)
  find_library(MED_SHAREDLIB3 NAMES medimport PATHS ${MED_LIB_DIR} NO_DEFAULT_PATH)
  set (MED_SHAREDLIB ${MED_SHAREDLIB1} ${MED_SHAREDLIB2} ${MED_SHAREDLIB3})
  if (NOT MED_SHAREDLIB)
     message(FATAL_ERROR "Not found MED shared library after MED user request")
  endif (NOT MED_SHAREDLIB)
  list(APPEND pc_libs_private "-L${MED_LIB_DIR} -lmedimport -lmedC -lmed")
endif (MED_LIB_DIR)

#
# RPC
option(FORCE_GLIBC_RPC "Do not try to search tirpc, will only use rpc provided by glibc" OFF)
option(INSTALL_TIRPC   "Install Ti-RPC before compiling EOS"                             OFF)
set(USER_TIRPC_PATH         "" CACHE PATH "Path to tirpc root directory")
set(USER_TIRPC_LIB_PATH     "" CACHE PATH "Path to tirpc lib directory")
set(USER_TIRPC_INCLUDE_PATH "" CACHE PATH "Path to tirpc include directory")
if(WITH_PLUGIN_FLICA4)
  include(EOS_RPC)
endif()

#
# Error messages
if (WITH_IPP)
  if (NOT EXISTS ${MED_INC}) 
    message(FATAL_ERROR "MED include files are required for Interpolator. You must choose one option among : --with-med=<path> --with-med-include=<path>")
  endif (NOT EXISTS ${MED_INC})
  if (NOT EXISTS ${MED_LIB_DIR}) 
    message(FATAL_ERROR "MED library file is required for Interpolator. You must choose one option among : --with-med=<path> --with-med-lib=<path>")
  endif (NOT EXISTS ${MED_LIB_DIR})
  if (NOT EXISTS ${HDF5_LIB_DIR}) 
    message(FATAL_ERROR "HDF5 library file is required for Interpolator. You must choose one option among : --with-hdf5=<path> --with-hdf5-lib=<path>")
  endif (NOT EXISTS ${HDF5_LIB_DIR}) 
endif (WITH_IPP)
#
# Exterior include directories : GLOBALEXT_INC variable
set (GLOBALEXT_INC ${MPI_INC} ${HDF5_INC} ${MED_INC} ${RPC_INC})
#
# Exterior library files : GLOBALEXT_LIB variable
set (GLOBALEXT_LIB       ${MPI_LIB} ${HDF5_SHAREDLIB} ${MED_SHAREDLIB} ${RPC_LIB})
set (GLOBALEXT_STATICLIB ${MPI_LIB} ${HDF5_STATICLIB} ${MED_STATICLIB} ${RPC_STATIC_LIB})

# Configure parameters
# --------------------
#
# WITH_DOC_DOXYGEN
option (WITH_DOC_DOXYGEN "Build documentation with Doxygen" OFF)
message(STATUS "Option WITH_DOC_DOXYGEN : ${WITH_DOC_DOXYGEN}")
if (WITH_DOC_DOXYGEN)
  if (USER_DOXYGEN_EXECUTABLE)
    message(STATUS "Doxygen executable specified by user : ${USER_DOXYGEN_EXECUTABLE}")
    set (DOXYGEN_EXECUTABLE ${USER_DOXYGEN_EXECUTABLE})
  else (USER_DOXYGEN_EXECUTABLE)
    find_package(Doxygen) 
    if (NOT DOXYGEN_FOUND)
      message(STATUS "Not found Doxygen")
      message(FATAL_ERROR "Doxygen is required to build documentation. You must choose one option among :--without-doc-doxygen, --with-doxygen=<path>, --with-doxygen-exec=<path>/doxygen")
    endif (NOT DOXYGEN_FOUND)
  endif (USER_DOXYGEN_EXECUTABLE)
  if (NOT EXISTS ${DOXYGEN_EXECUTABLE})
     message(FATAL_ERROR "File not found : ${DOXYGEN_EXECUTABLE}")
  endif (NOT EXISTS ${DOXYGEN_EXECUTABLE})
endif (WITH_DOC_DOXYGEN)
#
# WITH_DOC_LATEX
option (WITH_DOC_LATEX "Build documentation with Latex" OFF)
message(STATUS "Option WITH_DOC_LATEX : ${WITH_DOC_LATEX}")
message(STATUS "Info_global with-doc-latex ${WITH_DOC_LATEX}")
if (WITH_DOC_LATEX)
  find_package(LATEX)
  if (NOT LATEX_COMPILER)
    message(FATAL_ERROR "Latex compiler is required to build documentation. You must use the option --without-doc-latex")
  endif (NOT LATEX_COMPILER)
endif (WITH_DOC_LATEX)
#
# WITH_PYTHON_API
option (WITH_PYTHON_API "Build EOS python API" OFF)
message(STATUS "Option WITH_PYTHON_API : ${WITH_PYTHON_API}")
if (WITH_PYTHON_API)
  message(FATAL_ERROR "Build EOS python API is not yet implemented. You must use the option --without-python-api")
endif (WITH_PYTHON_API)


# Compilation and Linking Definition
# ----------------------------------
#
#
# Initialisation  Objects libraries targets lists 
set(LCOMM_TGT_OBJ "" CACHE STRING LCOMM_TGT_OBJ FORCE)  # Common
set(LLANG_TGT_OBJ "" CACHE STRING LLANG_TGT_OBJ FORCE)  # EOS Language
set(LFUNC_TGT_OBJ "" CACHE STRING LFUNC_TGT_OBJ FORCE)  # EOS Functions
set(LTM_TGT_OBJ   "" CACHE STRING LTM_TGT_OBJ   FORCE)  # Thermodynamic Models
set(LIPP_TGT_OBJ  "" CACHE STRING LIPP_TGT_OBJ  FORCE)  # Interpolator
mark_as_advanced(LCOMM_TGT_OBJ LLANG_TGT_OBJ LFUNC_TGT_OBJ LTM_TGT_OBJ LIPP_TGT_OBJ)
#
# Temporary shared libraries
set(NLIB_LANG   "CCLanguageAPI") # EOS Language
set(NLIB_FUNC   "CCFunctions")   # EOS Functions
set(NLIB_TM     "CCEOSAPI")      # Thermodynamic Models
set(NLIB_IHM    "eosihmcmodule") # IHM
if (WITH_IPP)
  set(NLIB_IPP  "CCEOSIGenAPI")  # Interpolator
endif (WITH_IPP)
#
# EOS library name
set(EOS_LIB_NAME "eos")
option(BUILD_STATIC_LIB "Option to build static EOS library libeos.a" ON)
option(BUILD_SHARED_LIB "Option to build dynamic EOS library libeos.so" ON)

if(NOT BUILD_SHARED_LIB AND NOT BUILD_STATIC_LIB)
  message(FATAL_ERROR "Error, you can't de-activate the build of both shared and static libraries")
endif()


#
# General compilation options 
set(COMP_FLAGS  -DNO_PVM_ -DCN_RSIZE=8 -DCN_ISIZE=4 -D_COMPILE_AVEC_GCC_ -DF77CALLWITH_ -fPIC)
if (USER_MED_PATH OR USER_MED_INCLUDE_PATH)
  set(COMP_FLAGS "${COMP_FLAGS}  -DMED_API_23")
endif (USER_MED_PATH OR USER_MED_INCLUDE_PATH)
if (ADD_COMP_FLAGS)
  string(REGEX REPLACE "," " " ADD_COMP_FLAGS "${ADD_COMP_FLAGS}")
  set(COMP_FLAGS "${COMP_FLAGS}  ${ADD_COMP_FLAGS}")
endif (ADD_COMP_FLAGS)
#
add_definitions(${COMP_FLAGS})

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(CMAKE_CXX_FLAGS "-Wall -Wextra")
  set(CMAKE_C_FLAGS "-Wall -Wextra -Werror")
  set(CMAKE_Fortran_FLAGS "-Wall -ffixed-line-length-132 -Wno-unused-label")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "-Wall -Wextra")
  set(CMAKE_C_FLAGS "-Wall -Wextra -Werror")
  set(CMAKE_Fortran_FLAGS "-Wall -ffixed-line-length-132 -Wno-unused-label")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
  set(CMAKE_CXX_FLAGS "-Wall")
  set(CMAKE_C_FLAGS "-Wall")
  set(CMAKE_Fortran_FLAGS "-extend-source 132 -diag-disable=7713,7841")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "IntelLLVM")
  set(CMAKE_CXX_FLAGS "-Wall")
  set(CMAKE_C_FLAGS "-Wall")
  set(CMAKE_Fortran_FLAGS "-extend-source 132 -diag-disable=7713,7841")
endif()

SET(CMAKE_CXX_FLAGS_RELEASE "-O2 -fPIC ")
SET(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -fPIC ")
SET(CMAKE_C_FLAGS_RELEASE "-O2 -fPIC ")
SET(CMAKE_C_FLAGS_DEBUG "-g -O0 -fPIC ")
SET(CMAKE_Fortran_FLAGS_RELEASE "-O2 -fPIC ")
SET(CMAKE_Fortran_FLAGS_DEBUG "-g -O0 -fPIC ")

if (USE_OPENMP)
  set(CMAKE_CXX_FLAGS        "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
  set(CMAKE_C_FLAGS          "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set(CMAKE_Fortran_FLAGS    "${CMAKE_Fortran_FLAGS} ${OpenMP_Fortran_FLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")

  if (ENABLE_SANITIZE)
    SET(CMAKE_CXX_FLAGS_DEBUG     "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=thread")
    SET(CMAKE_C_FLAGS_DEBUG       "${CMAKE_C_FLAGS_DEBUG} -fsanitize=thread")
    SET(CMAKE_Fortran_FLAGS_DEBUG "${CMAKE_Fortran_FLAGS_DEBUG} -fsanitize=thread")
  endif()

else()
  if (ENABLE_SANITIZE)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fsanitize=address")
    set(CMAKE_Fortran_FLAGS_DEBUG "${CMAKE_Fortran_FLAGS_DEBUG} -fsanitize=address")
  endif()
endif()

#
# Include files
include_directories(BEFORE ${EOS_SOURCE_DIR}/Modules)

#
# use RPATH
set (CMAKE_INSTALL_RPATH ${EOS_LIB_DIR})
set (CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# Documentation, tests
# --------------------
#
add_subdirectory(Doc)
add_subdirectory(Modules)

#
# Define target doc-latex if not done before
if(NOT TARGET doc-latex)
  message(STATUS "Add target doc-latex")
  add_custom_target(doc-latex ALL
                    COMMENT "Nothing to do for doc-latex")
endif(NOT TARGET doc-latex)

# Code generation
# ---------------
#
# Configure EOS_Config.hxx
configure_file(Tools/EOS_Config.hxx.in
               Modules/EOS/API/EOS_Config.hxx)

# objects list
set(LSO "")
foreach(TGT_OBJ ${LCOMM_TGT_OBJ} ${LLANG_TGT_OBJ} 
                ${LFUNC_TGT_OBJ} ${LTM_TGT_OBJ} 
                ${LIPP_TGT_OBJ})
  set(LSO ${LSO} $<TARGET_OBJECTS:${TGT_OBJ}>)
endforeach(TGT_OBJ)


#
# EOS static library definition
if(BUILD_STATIC_LIB)
  set(EOS_LIB_STAT "static_${EOS_LIB_NAME}")
  add_library(${EOS_LIB_STAT}  STATIC  ${LSO})
  target_link_libraries(${EOS_LIB_STAT} PUBLIC ${GLOBALEXT_STATICLIB})
  target_include_directories(${EOS_LIB_STAT} PUBLIC $<INSTALL_INTERFACE:include/>)
  set_target_properties(${EOS_LIB_STAT} PROPERTIES OUTPUT_NAME ${EOS_LIB_NAME}
                                                   IMPORTED_LOCATION_RELEASE ${EOS_LIB_DIR})
endif(BUILD_STATIC_LIB)

#
# EOS shared library definition
if(BUILD_SHARED_LIB)
  set(EOS_LIB_SHAR "shared_${EOS_LIB_NAME}")
  add_library(${EOS_LIB_SHAR}  SHARED  ${LSO})
  target_link_libraries(${EOS_LIB_SHAR} PUBLIC ${GLOBALEXT_LIB})
  target_include_directories(${EOS_LIB_SHAR} PUBLIC $<INSTALL_INTERFACE:include/>)
  set_target_properties(${EOS_LIB_SHAR} PROPERTIES OUTPUT_NAME ${EOS_LIB_NAME}
                                                   VERSION ${EOS_VERSION}
                                                   SOVERSION ${EOS_VERSION}
                                                   IMPORTED_LOCATION_RELEASE ${EOS_LIB_DIR})
endif(BUILD_SHARED_LIB)

# Installation Definition
# -----------------------

#
# Libraries
if(BUILD_STATIC_LIB)
  install(TARGETS ${EOS_LIB_STAT}
          EXPORT EOSTargets
          ARCHIVE DESTINATION ${EOS_LIB_DIR})
endif(BUILD_STATIC_LIB)
if(BUILD_SHARED_LIB)
  install(TARGETS ${EOS_LIB_SHAR}
          EXPORT EOSTargets
          LIBRARY DESTINATION ${EOS_LIB_DIR}
          PERMISSIONS OWNER_READ    OWNER_WRITE   OWNER_EXECUTE
                      GROUP_READ                  GROUP_EXECUTE
                      WORLD_READ                  WORLD_EXECUTE)
endif(BUILD_SHARED_LIB)
#
# Executable files
install(PROGRAMS ${CMAKE_BINARY_DIR}/Tools/eos_gui         DESTINATION ${EOS_BIN_DIR})
install(PROGRAMS ${CMAKE_BINARY_DIR}/Tools/eos.in          DESTINATION ${EOS_BIN_DIR} RENAME eos)
install(PROGRAMS ${CMAKE_SOURCE_DIR}/Tools/eos_data        DESTINATION ${EOS_BIN_DIR})
install(PROGRAMS ${CMAKE_SOURCE_DIR}/Tools/eos_mec         DESTINATION ${EOS_BIN_DIR})
#
# Include files
set(HEADERS_TO_INSTALL ${CMAKE_SOURCE_DIR}/Modules/Common
                       ${CMAKE_BINARY_DIR}/Modules/Common
                       ${CMAKE_SOURCE_DIR}/Modules/Language
                       ${CMAKE_BINARY_DIR}/Modules/Language
                       ${CMAKE_SOURCE_DIR}/Modules/Functions
                       ${CMAKE_BINARY_DIR}/Modules/Functions
                       ${CMAKE_SOURCE_DIR}/Modules/EOS
                       ${CMAKE_BINARY_DIR}/Modules/EOS
                       ${CMAKE_SOURCE_DIR}/Modules/system
                       ${CMAKE_BINARY_DIR}/Modules/system
)
if (WITH_IPP)
   list(APPEND HEADERS_TO_INSTALL ${CMAKE_SOURCE_DIR}/Modules/EOS_IGen ${CMAKE_BINARY_DIR}/Modules/EOS_IGen)
endif (WITH_IPP)
if(WITH_PLUGIN_FLICA4)
  foreach(dir ${HEADERS_TO_INSTALL})
    install(DIRECTORY ${dir}
            DESTINATION ${EOS_INCLUDE_DIR}
            FILES_MATCHING PATTERN "*.hxx" PATTERN "*.h" PATTERN "*.H"
            PATTERN "CMake*" EXCLUDE PATTERN "Tests" EXCLUDE PATTERN "Doc"      EXCLUDE 
            PATTERN "Data"   EXCLUDE PATTERN "PyAPI" EXCLUDE)
  endforeach()
else(WITH_PLUGIN_FLICA4)
  foreach(dir ${HEADERS_TO_INSTALL})
    install(DIRECTORY ${dir}
            DESTINATION ${EOS_INCLUDE_DIR}
            FILES_MATCHING PATTERN "*.hxx" PATTERN "*.h" PATTERN "*.H"
            PATTERN "CMake*" EXCLUDE PATTERN "Tests" EXCLUDE PATTERN "Doc" EXCLUDE 
            PATTERN "Data"   EXCLUDE PATTERN "PyAPI" EXCLUDE PATTERN "*FLICA4*" EXCLUDE
            PATTERN "*ubicSpline*" EXCLUDE)
  endforeach()
endif(WITH_PLUGIN_FLICA4)
#
# Data and documentation
install(DIRECTORY ${CMAKE_BINARY_DIR}/Modules/EOS/Data/ DESTINATION ${EOS_DATA_DIR} 
        PATTERN "CMake*" EXCLUDE PATTERN "*.cmake" EXCLUDE PATTERN "Makefile" EXCLUDE)
if (WITH_DOC_DOXYGEN)
  install(DIRECTORY ${CMAKE_BINARY_DIR}/Modules/Doc
          DESTINATION ${EOS_DOC_DIR}/Modules)
  install(DIRECTORY ${CMAKE_BINARY_DIR}/Modules/Language/Doc
          DESTINATION ${EOS_DOC_DIR}/Modules/Language)
  install(DIRECTORY ${CMAKE_BINARY_DIR}/Modules/Functions/Doc
          DESTINATION ${EOS_DOC_DIR}/Modules/Functions)
  install(DIRECTORY ${CMAKE_BINARY_DIR}/Modules/EOS/Doc
          DESTINATION ${EOS_DOC_DIR}/Modules/EOS)
  if (WITH_IPP)
     install(DIRECTORY  ${CMAKE_BINARY_DIR}/Modules/EOS_IGen/Doc
             DESTINATION ${EOS_DOC_DIR}/Modules/EOS_IGen)
  endif (WITH_IPP)
endif (WITH_DOC_DOXYGEN)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/Readme ${CMAKE_BINARY_DIR}/Readme DESTINATION ${EOS_DOC_DIR})
install(FILES ${CMAKE_BINARY_DIR}/Modules/index.html DESTINATION ${EOS_DOC_DIR}/Modules)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/Doc ${CMAKE_BINARY_DIR}/Doc
        DESTINATION ${EOS_DOC_DIR}
        PATTERN "CMake*" EXCLUDE PATTERN "*.cmake" EXCLUDE PATTERN "Makefile" EXCLUDE)
#
# Tests files
set(EOS_TESTS_DIR ${CMAKE_INSTALL_PREFIX}/share/tests)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/Modules/EOS/Tests/ ${CMAKE_BINARY_DIR}/Modules/EOS/Tests/
        DESTINATION ${EOS_TESTS_DIR}/EOS
        FILES_MATCHING  PATTERN "*.cxx" PATTERN "*.f" PATTERN "*Test*" 
        PATTERN "output*" PATTERN "*.val" PATTERN "*.valeos*"
        PATTERN "CMake*" EXCLUDE PATTERN "*.cmake" EXCLUDE)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/Modules/Language/Tests/ ${CMAKE_BINARY_DIR}/Modules/Language/Tests/
        DESTINATION ${EOS_TESTS_DIR}/Language
        FILES_MATCHING  PATTERN "*.cxx" PATTERN "*.f" PATTERN "*Test*" 
        PATTERN "output*"  PATTERN "CMake*" EXCLUDE PATTERN "*.cmake" EXCLUDE)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/Modules/Functions/Tests/ ${CMAKE_BINARY_DIR}/Modules/Functions/Tests/
        DESTINATION ${EOS_TESTS_DIR}/Functions
        FILES_MATCHING  PATTERN "*.cxx" PATTERN "*Test*" PATTERN "output*"
        PATTERN "CMake*" EXCLUDE PATTERN "*.cmake" EXCLUDE)
if (WITH_IPP)
  install(DIRECTORY ${CMAKE_SOURCE_DIR}/Modules/EOS_IGen/Tests/ ${CMAKE_BINARY_DIR}/Modules/EOS_IGen/Tests/
          DESTINATION ${EOS_TESTS_DIR}/EOS_IGen
          FILES_MATCHING  PATTERN "*.cxx" PATTERN "*Test*" PATTERN "output*"
          PATTERN "CMake*" EXCLUDE PATTERN "*.cmake" EXCLUDE)
endif (WITH_IPP)
#
# GUI files
set(EOS_GUI_SRC ${CMAKE_SOURCE_DIR}/Modules/EOS_IHM/qt)
set(EOS_GUI_BINARY_DIR ${CMAKE_BINARY_DIR}/Modules/EOS_IHM)
if   (WITH_GUI)
  set(EOS_GUI_SR2 ${CMAKE_SOURCE_DIR}/Modules/EOS_IHM/cpp)
  file(GLOB files_py ${EOS_GUI_SRC}/*.py
                     ${EOS_GUI_BINARY_DIR}/qt/*.py)
  list(APPEND files_py ${EOS_GUI_BINARY_DIR}/cpp/eosihm.py)
  set(LIB_IHM $<TARGET_FILE:${NLIB_IHM}>)
  install(FILES ${LIB_IHM} DESTINATION ${EOS_LIB_DIR} RENAME _eosihm${CMAKE_SHARED_LIBRARY_SUFFIX}
          PERMISSIONS OWNER_READ    OWNER_WRITE   OWNER_EXECUTE
                      GROUP_READ                  GROUP_EXECUTE 
                      WORLD_READ                  WORLD_EXECUTE)
  install(PROGRAMS ${files_py} DESTINATION ${EOS_PYTHON_SITE_PACKAGES_DIR})
  file(GLOB files_txt ${EOS_GUI_SRC}/*.txt ${EOS_GUI_SRC}/*.png ${EOS_GUI_SR2}/../README)
  set(EOS_DOC_GUI_DIR ${EOS_DOC_DIR}/Modules/EOS_IHM)
  install(DIRECTORY DESTINATION ${EOS_DOC_GUI_DIR})
  install(FILES ${files_txt} DESTINATION ${EOS_DOC_GUI_DIR})
endif(WITH_GUI)
#
# Create refprop_key.txt for NIST-Refprop
if (WITH_PLUGIN_REFPROP_9 OR WITH_PLUGIN_REFPROP_10)
  set(RPKEY_FILE ${CMAKE_SOURCE_DIR}/Tools/refprop_key.sh)
  install(CODE "execute_process(COMMAND sh ${RPKEY_FILE} ${EOS_LIB_DIR})")
endif (WITH_PLUGIN_REFPROP_9 OR WITH_PLUGIN_REFPROP_10)


###############
## PACKAGING ##

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)
string(REPLACE ";" " " pc_req_private "${pc_req_private_list}")

configure_package_config_file(
  "${PROJECT_SOURCE_DIR}/Tools/EOSConfig.cmake.in"
  "${PROJECT_BINARY_DIR}/Tools/EOSConfig.cmake"
INSTALL_DESTINATION
  "${CMAKE_INSTALL_PREFIX}/lib/cmake/EOS-${EOS_VERSION}"
)

write_basic_package_version_file(
    "${PROJECT_BINARY_DIR}/Tools/EOSConfigVersion.cmake"
    VERSION ${EOS_VERSION}
    COMPATIBILITY ExactVersion
)

configure_file(
  "${PROJECT_SOURCE_DIR}/Tools/eos.pc.in"
  "${PROJECT_BINARY_DIR}/Tools/eos.pc"
  @ONLY
)
install(
  FILES
    "${PROJECT_BINARY_DIR}/Tools/eos.pc"
  DESTINATION
    "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig"
)

export(EXPORT EOSTargets
  FILE "${CMAKE_BINARY_DIR}/Tools/EOSTargets.cmake"
  NAMESPACE NEPTUNE::
)
install(EXPORT EOSTargets
  FILE "EOSTargets.cmake"
  NAMESPACE NEPTUNE::
  DESTINATION ${EOS_LIB_DIR}/cmake/EOS-${EOS_VERSION}
)
install(
  FILES
    "${PROJECT_BINARY_DIR}/Tools/EOSConfig.cmake"
    "${PROJECT_BINARY_DIR}/Tools/EOSConfigVersion.cmake"
    "${PROJECT_BINARY_DIR}/Tools/EOSTargets.cmake"
    "${PROJECT_SOURCE_DIR}/Tools/FindTIRPC.cmake"
  DESTINATION
    "${CMAKE_INSTALL_PREFIX}/lib/cmake/EOS-${EOS_VERSION}"
)
