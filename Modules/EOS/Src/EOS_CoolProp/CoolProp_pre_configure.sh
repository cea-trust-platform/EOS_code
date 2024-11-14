#!/bin/bash
#=============================================================================
# Shell script for plugin cathare2 installation
#=============================================================================
set -o nounset -o errexit

# ----------------------------------------
# functions
eos_error()
{

    err=$1
    shift 1
    echo "Error ($script) : $*"
    exit $err
}

# ----------------------------------------
# init
script=`basename $0`

# ----------------------------------------
# read arguments
if [ $# != 2 ] ; then
   eos_error 40 "wrong number of arguments ($# instead of 2)"
else
   NEPTUNE_EOS_COOLPROP=$1
   BINARY_DIR=$2
   EOS_BINARY_DIR=$BINARY_DIR/../..
   ROOT_BINARY_DIR=$EOS_BINARY_DIR/../..
fi

# ----------------------------------------
# check
if [ -z $NEPTUNE_EOS_COOLPROP ] || [ ! -d $NEPTUNE_EOS_COOLPROP && ! -f $NEPTUNE_EOS_COOLPROP ] ; then
    eos_error 43 "unvalid path for --with-coolprop option : $NEPTUNE_EOS_COOLPROP"
fi

# Copy index file
echo "Build index.eos file"
if [ -d $NEPTUNE_EOS_COOLPROP/DATA ]; then
  mkdir -p $EOS_BINARY_DIR/Data/EOS_CoolProp
  cp $NEPTUNE_EOS_COOLPROP/DATA/index.eos.coolprop $EOS_BINARY_DIR/Data/EOS_CoolProp/index.eos
else
  eos_error 47 "the folder DATA is not found in $NEPTUNE_EOS_COOLPROP"
fi

cp $NEPTUNE_EOS_COOLPROP/model-version.txt $BINARY_DIR
