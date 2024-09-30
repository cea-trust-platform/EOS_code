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
   NEPTUNE_EOS_CATHARE2=$1
   BINARY_DIR=$2
   EOS_BINARY_DIR=$BINARY_DIR/../..
   ROOT_BINARY_DIR=$EOS_BINARY_DIR/../..
fi

# Is there something to do ?
if [ -f $EOS_BINARY_DIR/Tests/C++/C2DHOLiquid_1.val ]; then exit 0; fi

# ----------------------------------------
# check
if [[ -z $NEPTUNE_EOS_CATHARE2 ]] || [[ ! -d $NEPTUNE_EOS_CATHARE2 && ! -f $NEPTUNE_EOS_CATHARE2 ]] ; then
    eos_error 43 "unvalid path for --with-cathare2 option : $NEPTUNE_EOS_CATHARE2"
fi

# Unzip if path is an archive
if [ -f $NEPTUNE_EOS_CATHARE2 ]; then
   mkdir -p $ROOT_BINARY_DIR/plugins_tmp
   tar xf $NEPTUNE_EOS_CATHARE2 -C $ROOT_BINARY_DIR/plugins_tmp
   NEPTUNE_EOS_CATHARE2=`find $ROOT_BINARY_DIR/plugins_tmp -maxdepth 1 -name "C2*" |xargs realpath`
fi

if [ -d $NEPTUNE_EOS_CATHARE2/SRC ] ; then
   NEPTUNE_EOS_CATHARE2_SRC=$NEPTUNE_EOS_CATHARE2/SRC
else
   if [ -d $NEPTUNE_EOS_CATHARE2/Water ] ; then
       eos_error 44 "you are probably using an older version of the files from Cathare2 (the repertory SRC is not found in $NEPTUNE_EOS_CATHARE2)"
   else
       eos_error 45 "the repertory SRC is not found in $NEPTUNE_EOS_CATHARE2"
   fi
fi
if [ -d $NEPTUNE_EOS_CATHARE2/TCS ] ; then
   NEPTUNE_EOS_CATHARE2_TCS=$NEPTUNE_EOS_CATHARE2/TCS
else
   eos_error 46 "the repertory TCS is not found in $NEPTUNE_EOS_CATHARE2"
fi

echo "Copy Cathare2 source files 1, C2 fortran prefix"
for filet in `find $NEPTUNE_EOS_CATHARE2_SRC -name "*.f" -o -name "*.F" -o -name "*.H"`
do
  bfilet=`basename $filet`
  awk -v qt="'" 'BEGIN{IGNORECASE=1} \
                 {if($0 ~ /^ *INCLUDE/)    sub("INCLUDE *"qt,"INCLUDE "qt"C2_") ;\
                  if($0 ~ /^ *SUBROUTINE/) sub("SUBROUTINE *","SUBROUTINE C2_") ;\
                  if($0 ~ /^ *CALL/)       sub("CALL *","CALL C2_") ;\
                  print}'    $filet  >  $BINARY_DIR/Cathare2/C2_$bfilet
done

# Correction
mkdir -p $BINARY_DIR/Cathare2
#(cd Cathare2 ; ./CATHARE2_patch.sh)

#
# Build index.eos file
echo "Build index.eos file"
mkdir -p $EOS_BINARY_DIR/Data/EOS_Cathare2
index_eos=$EOS_BINARY_DIR/Data/EOS_Cathare2/index.eos
:> $index_eos

for filet in `find $NEPTUNE_EOS_CATHARE2_SRC/* -type d -exec basename {} \;`
do
   echo "${filet}Liquid ${filet}" | awk '{printf("Cathare2 %-14s EOS_Cathare2Liquid %-6s Unknown 1 %-6s\n"), $1,$2,$2}' >> $index_eos
   echo "${filet}Vapor  ${filet}" | awk '{printf("Cathare2 %-14s EOS_Cathare2Vapor  %-6s Unknown 1 %-6s\n"), $1,$2,$2}' >> $index_eos
done

echo "Cathare2 NitrogenIncondensable EOS_CathareIncondensableGas Nitrogen Unknown 1 Nitrogen" >> $index_eos
echo "Cathare2 HydrogenIncondensable EOS_CathareIncondensableGas Hydrogen Unknown 1 Hydrogen" >> $index_eos
echo "Cathare2 OxygenIncondensable   EOS_CathareIncondensableGas Oxygen   Unknown 1 Oxygen"   >> $index_eos
echo "Cathare2 AirIncondensable      EOS_CathareIncondensableGas Air      Unknown 1 Air"      >> $index_eos
echo "Cathare2 ArgonIncondensable    EOS_CathareIncondensableGas Argon    Unknown 1 Argon"    >> $index_eos
echo "Cathare2 HeliumIncondensable   EOS_CathareIncondensableGas Helium   Unknown 1 Helium"   >> $index_eos
echo "Cathare2 UserIncondensable     EOS_CathareIncondensableGas User     Unknown 1 User"     >> $index_eos

# model-version.txt
cp $NEPTUNE_EOS_CATHARE2/model-version.txt $BINARY_DIR
cat $NEPTUNE_EOS_CATHARE2/model-version.txt | col -b |\
   tr '[A-Z]' '[a-z]'  >> $EOS_BINARY_DIR/model-version.txt

# Tests
mkdir -p $EOS_BINARY_DIR/Tests/C++
find $NEPTUNE_EOS_CATHARE2_TCS -name "*.val" -exec cp {} $EOS_BINARY_DIR/Tests/C++ \;


# ----------------------------------------
# end
exit 0
