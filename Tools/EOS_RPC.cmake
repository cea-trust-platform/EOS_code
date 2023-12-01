#
#
#           Handles RPC prerequisite
#
#
#       RPC can either be 
#           1) specified by user
#               - by providing tirpc root dir
#               - by providing tirpc include dir and lib dir
#           2) installed during EOS build
#           4) ignore all tirpc libraries and force the use of RPC
#              provided by glibc (not available on recent systems)
#
#       By default, if none of the 4 options is provided, we search RPC
#       in GlibC, try a find package, and disable RPC if not found
#
#
#       Return values:
#       ^^^^^^^^^^^^^^
#
#         RPC_INC         : Path to RPC include directory
#         RPC_LIB         : Path to RPC shared library
#         RPC_STATIC_LIB  : Path to RPC static library
#


# Option 1
if (USER_TIRPC_PATH  OR USER_TIRPC_INCLUDE_PATH  OR USER_TIRPC_LIB_PATH)
  set(USER_RPC TRUE)
  if (INSTALL_TIRPC)
    message(FATAL_ERROR "You asked to compile Tirpc but also provided a path to RPC. Choose only one option")
  endif()
  if (FORCE_GLIBC_RPC)
    message(FATAL_ERROR "You asked to use glibc RPC but provided a path to tirpc. Choose only one option.")
  endif()

  set (CMAKE_FIND_LIBRARY_SUFFIXES .a)
  find_library(RPC_STATIC_LIB
      NAMES tirpc
      PATHS ${USER_TIRPC_LIB_PATH} "${USER_TIRPC_PATH}/lib"
      NO_DEFAULT_PATH
  )
  set (CMAKE_FIND_LIBRARY_SUFFIXES .so)
  find_library(RPC_LIB
      NAMES tirpc
      PATHS ${USER_TIRPC_LIB_PATH} "${USER_TIRPC_PATH}/lib"
      NO_DEFAULT_PATH
      REQUIRED
  )
  find_path(RPC_INC
      NAMES netconfig.h
      PATH_SUFFIXES tirpc
      PATHS ${USER_TIRPC_INCLUDE_PATH} "${USER_TIRPC_PATH}/include/tirpc"
      NO_DEFAULT_PATH
      REQUIRED
    )
  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(TIRPC
      REQUIRED_VARS RPC_LIB RPC_INC
      VERSION_VAR TIRPC_VERSION
  )
  if (NOT TIRPC_FOUND)
    message(FATAL_ERROR "We cannot find either TIRPC include directory or shared library in the path you provided.")
  endif()
  if(NOT RPC_STATIC_LIB)
    set(RPC_STATIC_LIB ${RPC_LIB}) # We don't want RPC_STATIC_LIB="NOTFOUND"
  endif()
  set(ENV{LD_LIBRARY_PATH} ${RPC_LIB}:$ENV{LD_LIBRARY_PATH} )
  message(STATUS "option --with-tirpc=<path> adds ${RPC_LIB} to LD_LIBRARY_PATH")
  list(APPEND pc_req_private_list "libtirpc")
endif(USER_TIRPC_PATH  OR USER_TIRPC_INCLUDE_PATH  OR USER_TIRPC_LIB_PATH)



# Option 2
if (INSTALL_TIRPC)
  if (FORCE_GLIBC_RPC)
    message(FATAL_ERROR "You asked to use glibc RPC but also asked to install embedded tirpc. Choose only one option.")
  endif()
  if(NOT TIRPC_ARCHIVE_PATH)
    set(TIRPC_ARCHIVE_PATH "https://downloads.sourceforge.net/libtirpc/libtirpc-1.3.3.tar.bz2")
  endif()
  message(STATUS "Tirpc will be installed into thirdpart")
  set(TIRPC_ROOT_DIR "${CMAKE_BINARY_DIR}/thirdpart/libtirpc")
  set(TIRPC_CONFIGURE_OPTIONS "--enable-gssapi=no")
  if(NOT BUILD_SHARED_LIB)
    set(TIRPC_CONFIGURE_OPTIONS "${TIRPC_CONFIGURE_OPTIONS} --enable-shared=no")
  elseif(NOT BUILD_STATIC_LIB)
    set(TIRPC_CONFIGURE_OPTIONS "${TIRPC_CONFIGURE_OPTIONS} --enable-static=no")
  endif()
  include(ExternalProject)
  ExternalProject_Add(
    libtirpc
    PREFIX            ${TIRPC_ROOT_DIR}
    URL               ${TIRPC_ARCHIVE_PATH}
    SOURCE_DIR        ${TIRPC_ROOT_DIR}/libtirpc
    BINARY_DIR        ${TIRPC_ROOT_DIR}/build
    INSTALL_DIR       ${TIRPC_ROOT_DIR}/install
    CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=<INSTALL_DIR> ${TIRPC_CONFIGURE_OPTIONS}
    BUILD_BYPRODUCTS <INSTALL_DIR>/lib/libtirpc.a
    BUILD_BYPRODUCTS <INSTALL_DIR>/lib/libtirpc.so
  )
  set(RPC_INC ${TIRPC_ROOT_DIR}/install/include/tirpc)
  add_library(RPC_LIB SHARED IMPORTED)
  add_library(RPC_STATIC_LIB STATIC IMPORTED)
  set_property(TARGET RPC_LIB PROPERTY IMPORTED_LOCATION "${TIRPC_ROOT_DIR}/install/lib/libtirpc.so")
  set_property(TARGET RPC_STATIC_LIB PROPERTY IMPORTED_LOCATION "${TIRPC_ROOT_DIR}/install/lib/libtirpc.a")
  install(DIRECTORY ${CMAKE_BINARY_DIR}/thirdpart/libtirpc/install/etc     DESTINATION ${CMAKE_INSTALL_PREFIX})
  install(DIRECTORY ${CMAKE_BINARY_DIR}/thirdpart/libtirpc/install/include DESTINATION ${CMAKE_INSTALL_PREFIX})
  install(DIRECTORY ${CMAKE_BINARY_DIR}/thirdpart/libtirpc/install/lib     DESTINATION ${CMAKE_INSTALL_PREFIX}
          PATTERN "libtirpc.pc" EXCLUDE)
  install(CODE "execute_process(COMMAND sed -e \"s?^prefix=.*?prefix=${CMAKE_INSTALL_PREFIX}?\" ${CMAKE_BINARY_DIR}/thirdpart/libtirpc/install/lib/pkgconfig/libtirpc.pc
                               OUTPUT_FILE ${CMAKE_INSTALL_PREFIX}/lib/pkgconfig/libtirpc.pc)")
  install(DIRECTORY ${CMAKE_BINARY_DIR}/thirdpart/libtirpc/install/share   DESTINATION ${CMAKE_INSTALL_PREFIX})

  list(APPEND pc_req_private_list "libtirpc")
endif(INSTALL_TIRPC)


# No option
if (NOT (USER_RPC OR INSTALL_TIRPC OR FORCE_GLIBC_RPC))
  find_package(TIRPC)
  if (NOT TIRPC_FOUND)
    message(STATUS "It's OK if the following tests pass")
  endif()
  if(NOT RPC_STATIC_LIB)
    set(RPC_STATIC_LIB ${RPC_LIB}) # We don't want RPC_STATIC_LIB="NOTFOUND"
  endif()
  if(NOT RPC_LIB OR NOT RPC_INC)
    set(RPC_INC "")
    set(RPC_LIB "")
    set(RPC_STATIC_LIB "")
  else()
    list(APPEND pc_req_private_list "libtirpc")
  endif()
endif(NOT (USER_RPC OR INSTALL_TIRPC OR FORCE_GLIBC_RPC))


# Check for rpc.h
if (NOT INSTALL_TIRPC)
    list(APPEND CMAKE_REQUIRED_INCLUDES ${RPC_INC})
    include(CheckIncludeFileCXX)
    CHECK_INCLUDE_FILE_CXX("rpc/rpc.h" RPC_INCLUDE_FOUND)
    CHECK_INCLUDE_FILE_CXX("rpc/xdr.h" XDR_INCLUDE_FOUND)
    if (NOT RPC_INCLUDE_FOUND OR NOT XDR_INCLUDE_FOUND)
      if(USER_RPC)
        message(FATAL_ERROR "We cannot find ${RPC_INC}/rpc/rpc.h or ${RPC_INC}/rpc/xdr.h\n"
                            "Please make sure ${RPC_INC} contains rpc header files,"
                            " ask for tirpc installation, or disable RPC."
        )
      elseif(FORCE_GLIBC_RPC)
        message(FATAL_ERROR "Your system is to recent to use FORCE_GLIBC_RPC option.\n"
                            "You must provide a path to tirpc, let cmake find it (with no option), "
                            "ask to install it, or disable RPC."
        )
      else()
        message(FATAL_ERROR "We cannot find tirpc and your glibc doesn't contain RPC.\n"
                            "Please provide a path to tirpc, ask for the installation "
                            "of embedded tirpc, or disable RPC and Flica4 plugin."
        )
      endif()
    endif()
    include(CheckSymbolExists)
    list(APPEND CMAKE_REQUIRED_LIBRARIES ${RPC_LIB})
    check_symbol_exists("xdr_float" "rpc/xdr.h" HAS_XDR_FLOAT)
    if (NOT HAS_XDR_FLOAT)
      if(USER_RPC)
        message(FATAL_ERROR "We found rpc.h and xdr.h, but they doesn't define xdr_float.\n"
                            "Please make sure your RPC library is correct, let CMake find tirpc, "
                            "ask the installation of embedded tirpc, or disable RPC."
        )
        elseif(FORCE_GLIBC_RPC)
          message(FATAL_ERROR "Your system is to recent to use FORCE_GLIBC_RPC option.\n"
                              "You must provide a path to tirpc, ask the installation of embedded tirpc, "
                              "let cmake find it (with no option), or disable RPC."
          )
      else()
        message(FATAL_ERROR "We cannot find tirpc and your glibc doesn't contain RPC.\n"
                            "Please provide a path to tirpc, ask for the installation "
                            "of embedded tirpc, or disable RPC and Flica4 plugin."
        )
      endif()
    endif()
endif(NOT INSTALL_TIRPC)
