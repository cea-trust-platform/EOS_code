# FindTIRPC
# ---------
#
# Find the native TIRPC includes and library.
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module will set the following variables in your project:
#
# ``RPC_INC``
#   where to find rpc.h, etc.
# ``RPC_LIB``
#   the shared libraries to link against to use TIRPC.
# ``RPC_STATIC_LIB``
#   the static libraries to link against to use TIRPC.
# ``TIRPC_VERSION``
#   the version of TIRPC found.
# ``TIRPC_FOUND``
#   true if the TIRPC headers and libraries were found.
#

find_package(PkgConfig QUIET)
pkg_check_modules(PC_TIRPC libtirpc)

find_path(RPC_INC
    NAMES netconfig.h
    PATH_SUFFIXES tirpc
    HINTS ${PC_RPC_INC}
)

set (CMAKE_FIND_LIBRARY_SUFFIXES .a)
find_library(RPC_STATIC_LIB
    NAMES tirpc
    HINTS ${PC_TIRPC_LIBRARY_DIRS}
)

set (CMAKE_FIND_LIBRARY_SUFFIXES .so)
find_library(RPC_LIB
    NAMES tirpc
    HINTS ${PC_TIRPC_LIBRARY_DIRS}
)

set(TIRPC_VERSION ${PC_TIRPC_VERSION})

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(TIRPC
    REQUIRED_VARS RPC_LIB RPC_STATIC_LIB RPC_INC
    VERSION_VAR TIRPC_VERSION
)
