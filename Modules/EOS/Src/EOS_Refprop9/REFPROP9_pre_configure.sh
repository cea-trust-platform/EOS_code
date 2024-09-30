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

SOURCE=${BASH_SOURCE[0]}
while [ -L "$SOURCE" ]; do # resolve $SOURCE until the file is no longer a symlink
  SCRIPT_DIR=$( cd -P "$( dirname "$SOURCE" )" >/dev/null 2>&1 && pwd )
  SOURCE=$(readlink "$SOURCE")
  [[ $SOURCE != /* ]] && SOURCE=$SCRIPT_DIR/$SOURCE # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
done
SCRIPT_DIR=$( cd -P "$( dirname "$SOURCE" )" >/dev/null 2>&1 && pwd )

# ----------------------------------------
# read arguments
REFPROP9_ROOT_DIR=$1
BINARY_DIR=$2
EOS_BINARY_DIR=$BINARY_DIR/../..
ROOT_BINARY_DIR=$EOS_BINARY_DIR/../..
#Is there anything to do ?
if [ -f $EOS_BINARY_DIR/Tests/C++/Refprop9WaterVapor_pt.val ]; then exit 0; fi

# ----------------------------------------
# Sanity Check
if [ ! -d $REFPROP9_ROOT_DIR ] && [ ! -f $REFPROP9_ROOT_DIR ] ; then
    error 44 " T.M. Plugin EXT. not found --with-refprop9=$REFPROP9_ROOT_DIR"
fi

# Decompression if path is an archive
if [ -f $REFPROP9_ROOT_DIR ]; then
    mkdir -p $ROOT_BINARY_DIR/plugins_tmp
    tar xf $REFPROP9_ROOT_DIR -C $ROOT_BINARY_DIR/plugins_tmp
    REFPROP9_ROOT_DIR=$ROOT_BINARY_DIR/plugins_tmp/Refprop9
fi

# SRC, TCS and DATA directories in Plugin EXT.
PLUGINEXT_SRC=$REFPROP9_ROOT_DIR/SRC
PLUGINEXT_TCS=$REFPROP9_ROOT_DIR/TCS
PLUGINEXT_DAT=$REFPROP9_ROOT_DIR/DATA
# MV_FILE : T.M. and version File
MV_FILE=$REFPROP9_ROOT_DIR/model-version.txt

if [ ! -f $MV_FILE ] ; then
   error 45 " T.M. and version File not found  MV_FILE = '$MV_FILE'"
fi
#  SRC test files
if [ ! -d $PLUGINEXT_SRC ]; then
   error 47 "$PLUGINEXT_SRC directory not found"
fi
res=`ls $PLUGINEXT_SRC/*.FOR | wc -l`
if [ $res = 0 ] ; then
    error 48 "no SRC files in $PLUGINEXT_SRC"
fi
#  DATA test files
if [ ! -d $PLUGINEXT_DAT ]; then
   error 49 "$PLUGINEXT_DAT directory not found"
fi
if [ ! -d $PLUGINEXT_DAT/fluids ]; then
   error 50 "$PLUGINEXT_DAT/fluids directory not found"
fi
res=`ls $PLUGINEXT_DAT/fluids/*.FLD | wc -l`
if [ $res = 0 ] ; then
    error 51 "no DATA/fluids files in $PLUGINEXT_DAT"
fi
if [ ! -d $PLUGINEXT_DAT/mixtures ]; then
   error 52 "$PLUGINEXT_DAT/mixtures directory not found"
fi
res=`ls $PLUGINEXT_DAT/mixtures/*.MIX | wc -l`
if [ $res = 0 ] ; then
    error 53 "no DATA/mixtures files in $PLUGINEXT_DAT"
fi

# ----------------------------------------
# Source files
echo "Copy source files"
mkdir -p $BINARY_DIR/Refprop9
cp $PLUGINEXT_SRC/* $BINARY_DIR/Refprop9/.
mv $BINARY_DIR/Refprop9/COMMONS.FOR $BINARY_DIR/Refprop9/COMMONS.INC
mv $BINARY_DIR/Refprop9/COMTRN.FOR $BINARY_DIR/Refprop9/COMTRN.INC
sed "s/commons.for/COMMONS.INC/g" -i $BINARY_DIR/Refprop9/*
sed "s/comtrn.for/COMTRN.INC/g" -i $BINARY_DIR/Refprop9/*
# Patches
(cd $BINARY_DIR/Refprop9 ; ./REFPROP9_patch.sh)


# ----------------------------------------
# Data
# build  directory $EOS_DATA
EOS_DATA=$EOS_BINARY_DIR/Data/EOS_Refprop9/
mkdir -p $EOS_DATA
# TM_VE   : Thermodynamic Model version
TM_VE=`awk -F'-' '{print $2}' $MV_FILE | col -b`
echo "Refprop9 EOS_Refprop9 $TM_VE" > $EOS_DATA/info.tm
# index.eos T.M. file
INDEX_TMFILE=$EOS_DATA/index.eos
:> $INDEX_TMFILE
for fld in `cd $PLUGINEXT_DAT/fluids ; ls *.FLD`
do
    name=`echo ${fld%.*} | sed 's/\(.\)\(.*\)/\1\L\2/'`
    echo "Refprop9 $name EOS_Refprop9 $name Unknown 2 fluids/$fld fluid"                    >> $INDEX_TMFILE
    echo "Refprop9 ${name}Liquid EOS_Refprop9_liquid $name Unknown 2 fluids/$fld fluid"     >> $INDEX_TMFILE
    echo "Refprop9 ${name}Vapor EOS_Refprop9_vapor $name Unknown 2 fluids/$fld fluid"       >> $INDEX_TMFILE
done
for ppf in `cd $PLUGINEXT_DAT/fluids ; ls *.PPF`
do
    name=`echo ${ppf%.*} | sed 's/\(.\)\(.*\)/\1\L\2/'`_ppf
    echo "Refprop9 $name EOS_Refprop9 $name Unknown 2 fluids/$ppf fluid"                    >> $INDEX_TMFILE
    echo "Refprop9 ${name}Liquid EOS_Refprop9_liquid $name Unknown 2 fluids/$ppf fluid"     >> $INDEX_TMFILE
    echo "Refprop9 ${name}Vapor EOS_Refprop9_vapor $name Unknown 2 fluids/$ppf fluid"       >> $INDEX_TMFILE
done
for mix in `cd $PLUGINEXT_DAT/mixtures ; ls *.MIX`
do
    name=`echo ${mix%.*} | sed 's/\(.\)\(.*\)/\1\L\2/'`
    echo "Refprop9 $name EOS_Refprop9 $name Unknown 2 mixtures/$mix mixture"                >> $INDEX_TMFILE
    echo "Refprop9 ${name}Liquid EOS_Refprop9_liquid $name Unknown 2 mixtures/$mix mixture" >> $INDEX_TMFILE
    echo "Refprop9 ${name}Vapor EOS_Refprop9_vapor $name Unknown 2 mixtures/$mix mixture"   >> $INDEX_TMFILE
done

echo "Copy fluids, ppf and mixtures properties to $EOS_DATA"
cp -rpf $PLUGINEXT_DAT/fluids   $EOS_DATA/.
cp -rpf $PLUGINEXT_DAT/mixtures $EOS_DATA/.



# ----------------------------------------
# Doc
mkdir -p $ROOT_BINARY_DIR/Doc/Thermo_Models/EOS_Refprop9

#Add 'EOS T.M. Name' in index.html
#Add before <hr> tag
index=$ROOT_BINARY_DIR/Doc/Thermo_Models/index.html
line1="   <h2> For EOS_Refprop9 :</h2>\n"
line2="   <a href=\"EOS_Refprop9/doc_not_available.pdf\">Not available</a>\n"
if ! grep -q "EOS_Refprop9" $index ; then
    sed "/<hr>/i\ $line1" $index | sed "/<hr>/i\ $line2" > tmp.html
    mv tmp.html $index
else
    echo "PLUGIN Refprop9 already installed in $index"
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
   echo "WARNING : You don't have physics tests for Refprop9 model"
   echo "          Ask for those tests to the EOS administrator"
fi

# ----------------------------------------
exit 0
