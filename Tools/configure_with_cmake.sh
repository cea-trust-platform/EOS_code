#!/bin/bash
#=============================================================================
# EOS - Equation Of State - Neptune Project
#
# Shell script for EOS installation with cmake
# Do not use it. This shell is called by configure.
#
#=============================================================================


# define functions
# ----------------
info()
{
    echo    "  " $*
}
error()
{
    info "ERROR configure with cmake : " $*
    exit 1
}
usage()
{
    if [ -f $dirconfigure/configure ] ; then
        $dirconfigure/configure --help
    else
        error "file not found to give usage : $dirconfigure/configure"
    fi
    exit 1
}
unknown_option()
{
    info "error (unknown option : $*)"
    info "list of known options :"
    for opt in ${!listoptknown[@]} ; do
        echo "                        $opt"
    done
    for opt in ${!listopteqknown[@]} ; do
        echo "                        $opt=<path>"
    done
    exit 1
}

declare -A listoptknown
declare -A listopteqknown

add_opt()
{
    if [ "$#" != "2" ] ; then
        error "function add_opt takes 2 arguments and not $#"
    fi
    listoptknown["$1"]="$2"
}
add_opteq()
{
    if [ "$#" != "2" ] ; then
        error "function add_opteq takes 2 arguments and not $#"
    fi
    listopteqknown["$1"]="$2"
}

cmake_options="-DCMAKE_BUILD_TYPE:STRING=Release" #default option, can be overridden

fill_opt()
{
    if [ "$#" != "1" ] ; then
        error "function fill_opt takes 2 arguments and not $#"
    elif [ -z ${listoptknown[$1]} ] ; then
        error "option $1 is not a known option"
    fi
    cmake_options="${cmake_options} -D${listoptknown[$1]}"
}
fill_opt_with_value()
{
    if [ "$#" != "2" ] ; then
        error "function fill_opt_with_value takes 2 arguments and not $#"
    elif [ -z ${listopteqknown[$1]} ] ; then
        error "option $1 is not a known option"
    fi
    cmake_options="${cmake_options} -D${listopteqknown[$1]}=$2"
}

#=============================================================================
# define lists of links (configure option <--> cmake option)
# -----------------------------------------------

# option like --without-eos-gui  --> -DWITH_GUI=OFF
# ex : add_opt "--without-eos-gui" "WITH_GUI=OFF"
# ------------------------------------------

add_opt   "--release"                      "CMAKE_BUILD_TYPE=Release"
add_opt   "--debug"                        "CMAKE_BUILD_TYPE=Debug"
add_opt   "--with-gui"                     "WITH_GUI=ON"
add_opt   "--without-gui"                  "WITH_GUI=OFF"
add_opt   "--with-doc-doxygen"             "WITH_DOC_DOXYGEN=ON"
add_opt   "--without-doc-doxygen"          "WITH_DOC_DOXYGEN=OFF"
add_opt   "--with-doc-latex"               "WITH_DOC_LATEX=ON"
add_opt   "--without-doc-latex"            "WITH_DOC_LATEX=OFF"
add_opt   "--with-python-api"              "WITH_PYTHON_API=ON" 
add_opt   "--without-python-api"           "WITH_PYTHON_API=OFF"
add_opt   "--with-interpolator"            "WITH_IPP=ON"
add_opt   "--install-tirpc"                "INSTALL_TIRPC=ON"
add_opt   "--force-glibc-rpc"              "FORCE_GLIBC_RPC=ON"
add_opt   "--shared-only"                  "BUILD_STATIC_LIB=OFF"
add_opt   "--static-only"                  "BUILD_SHARED_LIB=OFF"
add_opt   "--with-openmp"                  "ENABLE_OPENMP=ON"
add_opt   "--without-openmp"               "ENABLE_OPENMP=OFF"

# option like --prefix=/home/myrep --> -DCMAKE_INSTALL_PREFIX=/home/myrep
# ex : add_opteq "--prefix" "CMAKE_INSTALL_PREFIX"
# ------------------------------------------

add_opteq "--prefix"                      "CMAKE_INSTALL_PREFIX"
add_opteq "--cxx"                         "CMAKE_CXX_COMPILER"
add_opteq "--fc"                          "CMAKE_Fortran_COMPILER"
add_opteq "--cc"                          "CMAKE_C_COMPILER"
add_opteq "--add_comp_flags"              "ADD_COMP_FLAGS"

add_opteq "--with-doxygen-exec"           "USER_DOXYGEN_EXECUTABLE"

add_opteq "--with-mpi"                    "USER_MPI_PATH"
add_opteq "--with-mpi-lib"                "USER_MPI_LIB_PATH"
add_opteq "--with-mpi-include"            "USER_MPI_INCLUDE_PATH"

add_opteq "--with-hdf5"                   "USER_HDF5_PATH"
add_opteq "--with-hdf5-lib"               "USER_HDF5_LIB_PATH"

add_opteq "--with-med"                    "USER_MED_PATH"
add_opteq "--with-med-lib"                "USER_MED_LIB_PATH"
add_opteq "--with-med-include"            "USER_MED_INCLUDE_PATH"

add_opteq "--with-python-exec"            "USER_PYTHON_EXECUTABLE"
add_opteq "--with-python-lib"             "USER_PYTHON_LIB"
add_opteq "--with-python-include"         "USER_PYTHON_INC"

add_opteq "--with-swig-exec"              "SWIG_EXECUTABLE"

add_opteq "--with-tirpc"                  "USER_TIRPC_PATH"
add_opteq "--with-tirpc-lib"              "USER_TIRPC_LIB_PATH"
add_opteq "--with-tirpc-include"          "USER_TIRPC_INCLUDE_PATH"
add_opteq "--with-tirpc-archive-path"     "TIRPC_ARCHIVE_PATH"

add_opteq "--with-cathare"                "WITH_PLUGIN_CATHARE"
add_opteq "--with-cathare2"               "WITH_PLUGIN_CATHARE2"
add_opteq "--with-hitec"                  "WITH_PLUGIN_HITEC"
add_opteq "--with-nak"                    "WITH_PLUGIN_NAK"
add_opteq "--with-thetis"                 "WITH_PLUGIN_THETIS"
add_opteq "--with-refprop9"               "WITH_PLUGIN_REFPROP_9"
add_opteq "--with-refprop10"              "WITH_PLUGIN_REFPROP_10"
add_opteq "--with-flica4"                 "WITH_PLUGIN_FLICA4"

#=============================================================================

# init
# ----------------
dirconfigure=$(dirname $0)
dirconfigure=$(cd $dirconfigure ; pwd)
dirconfigure=$(dirname $dirconfigure)

# translate options
# ----------------
cmake_exec="cmake"
cmake_binary_dir=`pwd`
for i in "$@" ; do
    case $i in
        --help | -h)
            usage
            ;;
        --user-env-file*)
            ;;
        --with-cmake-exec=*)
            cmake_exec="${i#*=}"
            ;;
        --execdir=*)
            cmake_binary_dir="${i#*=}"
            ;;
        --*=*)
            fill_opt_with_value "${i%=*}" "${i#*=}"
            ;;
        --*)
            echo $i
            fill_opt $i
            ;;
        *)  
            error "error (unknown option : $i)"
            ;;
    esac
done
# Build in-source is forbidden
[ ${cmake_binary_dir} == ${dirconfigure} ] && error "It is now forbidden to build in-source. You can either call the configure script from another directory or use the --execdir=<path> option."


# before cmake
# ----------------
cmake_options="${cmake_options} ${dirconfigure} -B${cmake_binary_dir}"

info "Execute cmake :"
info ${cmake_exec} ${cmake_options}
${cmake_exec} ${cmake_options} || exit 1

exit 0
