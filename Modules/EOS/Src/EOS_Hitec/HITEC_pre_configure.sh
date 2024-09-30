#!/bin/bash
#===========================================
# Shell script for T.M. plugin installation
#==========================================
set -o nounset -o errexit

# ----------------------------------------
# functions
error()
{ err=$1
  shift 1
  echo "Error ($script) : $*"
  exit $err
}
script=`basename $0`
SOURCE_DIR=`dirname $0`

HITEC_ROOT_DIR=$1
BINARY_DIR=$2
EOS_BINARY_DIR=$BINARY_DIR/../..
ROOT_BINARY_DIR=$EOS_BINARY_DIR/../..

# Is there something to do ?
if [ -f $EOS_BINARY_DIR/Tests/C++/HitecVapor.val ] ; then exit 0; fi

# Decompression if plugin is an archive
if [ -f $HITEC_ROOT_DIR ]; then
    mkdir -p $ROOT_BINARY_DIR/plugins_tmp
    tar xf $HITEC_ROOT_DIR -C $ROOT_BINARY_DIR/plugins_tmp
    HITEC_ROOT_DIR=$ROOT_BINARY_DIR/plugins_tmp/Hitec
fi

if [ ! -d $HITEC_ROOT_DIR ] ; then
    error 44 " T.M. Plugin Hitec not found, --with-hitec=$HITEC_ROOT_DIR"
fi

# ----------------------------------------
# T.M. and version
# MV_FILE : T.M. and version File
# TH_MO   : Thermodynamic Model Name
# EOS_TM  : EOS Thermodynamic Model Name
# TM_VE   : Thermodynamic Model version
MV_FILE=$HITEC_ROOT_DIR/model-version.txt
TH_MO=`awk -F'-' '{print $1}' $MV_FILE | col -b`
s1=`echo $TH_MO | cut -c 1  | tr '[a-z]' '[A-Z]'`
s2=`echo $TH_MO | cut -c 2- | tr '[A-Z]' '[a-z]'`
TH_MO="${s1}${s2}"
#
EOS_TM="EOS_$TH_MO"
#
TM_VE=`awk -F'-' '{print $2}' $MV_FILE | col -b`

#
# SRC, TCS and DATA directories in Plugin EXT.
PLUGINEXT_TCS=$HITEC_ROOT_DIR/TCS

# ----------------------------------------
# EOS Directories
EOS_DATTM=$EOS_BINARY_DIR/Data/$EOS_TM
mkdir -p $EOS_DATTM

# T.M. information file
INFO_TMFILE=$EOS_DATTM/info.tm
echo "$TH_MO $EOS_TM $TM_VE" > $INFO_TMFILE
# index.eos T.M. file
INDEX_TMFILE=$EOS_DATTM/index.eos
:> $INDEX_TMFILE
#
# Copy file from $HITEC_ROOT_DIR to $BINARY_DIR
echo "Copy plugin EOS_Hitec source files in EOS/Src"
for file in `find $HITEC_ROOT_DIR -name "*.?xx" -o -name "CMakeLists.txt"` ; do
    if [ -f $file ] ; then
	   cp -p $file $BINARY_DIR
    fi
done

patch -p1 -u < $SOURCE_DIR/EOS_Hitec.patch

if [ -f $INDEX_TMFILE ] ; then
   echo "Hitec  HitecLiquid  EOS_Hitec  Hitec  Unknown  1 Liquid" >> $INDEX_TMFILE
   echo "Hitec  HitecVapor   EOS_Hitec  Hitec  Unknown  1 Vapor" >> $INDEX_TMFILE
fi

# ----------------------------------------
# Doc
mkdir -p $ROOT_BINARY_DIR/Doc/Thermo_Models/$EOS_TM

#Add 'EOS T.M. Name' in index.html
#Add before <hr> tag
index=$ROOT_BINARY_DIR/Doc/Thermo_Models/index.html
line1="   <h2> For $EOS_TM :</h2>\n"
line2="   <a href=\"$EOS_TM/doc_not_available.pdf\">Not available</a>\n"
motif=$EOS_TM
if [ "`grep $motif $index`" = "" ] ; then
    sed "/<hr>/i\ $line1" $index | sed "/<hr>/i\ $line2" > tmp.html
    mv tmp.html $index
else
    echo "PLUGIN $TH_MO already installed in $index"
fi


# ----------------------------------------
# model-version.txt
cp $MV_FILE $BINARY_DIR
cat $MV_FILE | col -b | tr '[A-Z]' '[a-z]'  >> $EOS_BINARY_DIR/model-version.txt


# ----------------------------------------
# Tests
if [ -d $PLUGINEXT_TCS ] ; then
   mkdir -p $EOS_BINARY_DIR/Tests/C++/
   cp $PLUGINEXT_TCS/*.val $EOS_BINARY_DIR/Tests/C++/
else
   echo "WARNING : You don't have physics tests for Hitec model"
   echo "          Ask for those tests to the EOS administrator"
fi

 
# ----------------------------------------
exit 0
