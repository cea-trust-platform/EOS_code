#!/bin/sh

set -o nounset

eos_error()
{ err=$1
  shift 1
  echo "Error ($prog) : $*"
  exit $err
}
prog=`basename $0`


# ----------------------------------------
# read arguments
if [ $# != 2 ] ; then
   eos_error 40 "Wrong number of arguments ($# instead of 2)"
else
   FLICA4_ROOT=$1
   BINARY_DIR=$2  # = ${CMAKE_BINARY_DIR}/Modules/EOS/Src/EOS_Flica4
   EOS_BINARY_DIR=$BINARY_DIR/../..
   ROOT_BINARY_DIR=$EOS_BINARY_DIR/../..
fi

# Is there something to do ?
if [ -f ${EOS_BINARY_DIR}/Data/EOS_Flica4/index.eos ] ; then exit 0 ; fi


if [ ! -d $FLICA4_ROOT && ! -f $FLICA4_ROOT ] ; then
   eos_error 51 "Flica4 plugin not found, option --with-flica4=$FLICA4_ROOT"
fi

# Unzip if path is an archive
if [ -f $FLICA4_ROOT ]; then
   mkdir -p $ROOT_BINARY_DIR/plugins_tmp
   tar xf $FLICA4_ROOT -C $ROOT_BINARY_DIR/plugins_tmp
   FLICA4_ROOT=`find $ROOT_BINARY_DIR/plugins_tmp/ -maxdepth 1 -iname "*flica*" |xargs realpath`
fi

FLICA4_SOURCES=$FLICA4_ROOT/SRC_EOS
FLICA4_DATA=$FLICA4_ROOT/DATA

if [ ! -d ${FLICA4_SOURCES} ] ; then
    eos_error 52 "We expect to find ${FLICA4_SOURCES}"
fi

if [ ! -d ${FLICA4_DATA} ] ; then
    eos_error 53 "We expect to find ${FLICA4_DATA}"
fi



cp -r ${FLICA4_SOURCES}/* ${BINARY_DIR}/. || eos_error 54 "Error while copying Flica4 source code"

mkdir -p ${EOS_BINARY_DIR}/Data/EOS_Flica4 || eos_error 55 "Error while creating ${EOS_BINARY_DIR}/Data/EOS_Flica4 directory"
cp ${FLICA4_ROOT}/DATA/* ${EOS_BINARY_DIR}/Data/EOS_Flica4/. || eos_error 56 "Error while copying Flica4 data"