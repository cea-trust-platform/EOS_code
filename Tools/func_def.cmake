#
#                                     Functions Definitions
#
#
#
# --- Elementary Library Definition : felemlib
#
#     input  : TGT_OBJ  : object library target
#              LTGT_OBJ : objects libraries targets list
#              PAREC    : source list parameter GLOB/GLOB_RECURSE
#              LIST_SO  : optionnel source or objects list 
#
#     output : LTGT_OBJ : objects libraries targets list
#              TGT_LIBS : static library target
#              TGT_LIBD : shared library target
#
function(felemlib)

   set(TGT_OBJ  ${ARGV0})
   set(LTGT_OBJ ${ARGV1})
   set(PAREC    ${ARGV2})
   if(${ARGC} GREATER 3)
      set(LIST_SO ${ARGV3})
   else(${ARGC} GREATER 3)
      set(LIST_SO "")
   endif(${ARGC} GREATER 3)
   
   # include files
   include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR})
   include_directories(BEFORE ${CMAKE_CURRENT_BINARY_DIR})

   # source list  SRCS
   if(NOT ${PAREC} STREQUAL GLOB  AND  NOT ${PAREC} STREQUAL GLOB_RECURSE)
      message (FATAL_ERROR " parameter PAREC GLOB/GLOB_RECURSE : ${PAREC}")
   endif(NOT ${PAREC} STREQUAL GLOB  AND  NOT ${PAREC} STREQUAL GLOB_RECURSE)
   # A file produced in the binary directory (by configure_file) shadows the
   # same-named file of the source directory : both map to the same object
   # name, so listing the two lets CMake keep only one of them - and it keeps
   # the source one, i.e. the unsubstituted template.
   file(${PAREC} SRCS_BIN ${CMAKE_CURRENT_BINARY_DIR}/*.cxx
                          ${CMAKE_CURRENT_BINARY_DIR}/*.c
                          ${CMAKE_CURRENT_BINARY_DIR}/*.f
                          ${CMAKE_CURRENT_BINARY_DIR}/*.F
                          ${CMAKE_CURRENT_BINARY_DIR}/*.FOR
   )
   file(${PAREC} SRCS_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cxx
                          ${CMAKE_CURRENT_SOURCE_DIR}/*.c
                          ${CMAKE_CURRENT_SOURCE_DIR}/*.f
                          ${CMAKE_CURRENT_SOURCE_DIR}/*.F
                          ${CMAKE_CURRENT_SOURCE_DIR}/*.FOR
   )

   set(SRCS_BIN_REL "")
   foreach(SRC_FILE ${SRCS_BIN})
      file(RELATIVE_PATH SRC_REL ${CMAKE_CURRENT_BINARY_DIR} ${SRC_FILE})
      list(APPEND SRCS_BIN_REL ${SRC_REL})
   endforeach(SRC_FILE)

   set(SRCS ${SRCS_BIN})
   foreach(SRC_FILE ${SRCS_SRC})
      file(RELATIVE_PATH SRC_REL ${CMAKE_CURRENT_SOURCE_DIR} ${SRC_FILE})
      if(NOT SRC_REL IN_LIST SRCS_BIN_REL)
         list(APPEND SRCS ${SRC_FILE})
      endif(NOT SRC_REL IN_LIST SRCS_BIN_REL)
   endforeach(SRC_FILE)

   # objects library
   add_library(${TGT_OBJ} OBJECT ${SRCS} ${LIST_SO})
   target_include_directories(${TGT_OBJ} PRIVATE ${CMAKE_BINARY_DIR}/Modules/EOS/API ${CMAKE_BINARY_DIR}/Modules)

   # static library  TGT_LIBS
   if(BUILD_STATIC_LIB)
     set(TGT_LIBS "${TGT_OBJ}_stat")
     add_library(${TGT_LIBS} STATIC $<TARGET_OBJECTS:${TGT_OBJ}>)
     target_include_directories(${TGT_LIBS} PRIVATE ${CMAKE_BINARY_DIR}/Modules/EOS/API ${CMAKE_BINARY_DIR}/Modules)
   endif(BUILD_STATIC_LIB)

   # shared library  TGT_LIBD
   if(BUILD_SHARED_LIB)
     set(TGT_LIBD "${TGT_OBJ}_shar")
     add_library(${TGT_LIBD} SHARED $<TARGET_OBJECTS:${TGT_OBJ}>)
     target_include_directories(${TGT_LIBD} PRIVATE ${CMAKE_BINARY_DIR}/Modules/EOS/API ${CMAKE_BINARY_DIR}/Modules)
   endif(BUILD_SHARED_LIB)

  
   # object library targets list  ${LTGT_OBJ}
   set(${LTGT_OBJ} ${${LTGT_OBJ}} ${TGT_OBJ} CACHE STRING ${LTGT_OBJ} FORCE)

endfunction()
