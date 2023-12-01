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

THETIS_ROOT_DIR=$1
BINARY_DIR=$2
EOS_BINARY_DIR=$BINARY_DIR/../..
EOS_DATA_DIR=$EOS_BINARY_DIR/Data/EOS_Thetis
ROOT_BINARY_DIR=$EOS_BINARY_DIR/../..

# Is there something to do ?
if [ -f $EOS_BINARY_DIR/model-version.txt ] && grep -q -e "^th " $EOS_BINARY_DIR/model-version.txt ; then exit 0 ; fi


# ----------------------------------------
# check
if [ ! -d $THETIS_ROOT_DIR ] && [ ! -f $THETIS_ROOT_DIR ] || [ -z $THETIS_ROOT_DIR ] ; then
    eos_error 43 "unvalid path for --with-thetis option : $THETIS_ROOT_DIR"
fi

# Decompression if plugin is an archive
if [ -f $THETIS_ROOT_DIR ]; then
    mkdir -p $ROOT_BINARY_DIR/plugins_tmp
    tar xf $THETIS_ROOT_DIR -C $ROOT_BINARY_DIR/plugins_tmp
    THETIS_ROOT_DIR=`find $ROOT_BINARY_DIR/plugins_tmp -maxdepth 1 -name "*thetis*" |xargs realpath`
fi

if [ ! -d $THETIS_ROOT_DIR/DATA ] ; then
   error 51 "Directory DATA is not found : $THETIS_ROOT_DIR/DATA"
elif [ ! -d $THETIS_ROOT_DIR/SRC ] ; then
   error 51 "Directory SRC is not found : $THETIS_ROOT_DIR/SRC"
fi

mkdir -p $BINARY_DIR/Thetis
mkdir -p $EOS_DATA_DIR

echo "Make copy to Thetis files"
for filet in `find $THETIS_ROOT_DIR/SRC -name "*.c"`
do
   cp $filet $BINARY_DIR/Thetis/TH_$(basename $filet)
done

include=
for filet in `find $THETIS_ROOT_DIR -name "*.h"`
do
   cp $filet $BINARY_DIR/Thetis/TH_$(basename $filet)
   include="$include $(basename $filet)"
done

echo "Rename include in Thetis files"
for filet in $include 
do
   sed -i 's/'$filet'/TH_'$filet'/g' $BINARY_DIR/Thetis/*
done

echo "Copy Thetis data files, build index.eos"
# files copy
cp $THETIS_ROOT_DIR/DATA/* $EOS_DATA_DIR
# build index.eos file
cd $EOS_DATA_DIR
:> index.eos
echo "Thetis  AmmoniaLiquid   EOS_ThetisAmmoniaLiquid   Ammonia    Unknown  0" >> index.eos
echo "Thetis  AmmoniaVapor    EOS_ThetisAmmoniaVapor    Ammonia    Unknown  0" >> index.eos
echo "Thetis  WaterLiquid     EOS_ThetisWaterLiquid     Water      Unknown  0" >> index.eos
echo "Thetis  WaterVapor      EOS_ThetisWaterVapor      Water      Unknown  0" >> index.eos
echo "Thetis  WaterLiquidBMP  EOS_ThetisWaterBMPLiquid  Water      Unknown  0" >> index.eos
echo "Thetis  WaterVaporBMP   EOS_ThetisWaterBMPVapor   Water      Unknown  0" >> index.eos
echo "Thetis  WaterLiquidBP   EOS_ThetisWaterBPLiquid   Water      Unknown  0" >> index.eos
echo "Thetis  WaterVaporBP    EOS_ThetisWaterBPVapor    Water      Unknown  0" >> index.eos
echo "Thetis  WaterLiquidMP   EOS_ThetisWaterMPLiquid   Water      Unknown  0" >> index.eos
echo "Thetis  WaterVaporMP    EOS_ThetisWaterMPVapor    Water      Unknown  0" >> index.eos
echo "Thetis  R113Liquid      EOS_ThetisFreonR113Liquid R113       Unknown  0" >> index.eos
echo "Thetis  R113Vapor       EOS_ThetisFreonR113Vapor  R113       Unknown  0" >> index.eos
echo "Thetis  R114Liquid      EOS_ThetisFreonR114Liquid R114       Unknown  0" >> index.eos
echo "Thetis  R114Vapor       EOS_ThetisFreonR114Vapor  R114       Unknown  0" >> index.eos
echo "Thetis  R12Liquid       EOS_ThetisFreonR12Liquid  R12        Unknown  0" >> index.eos
echo "Thetis  R12Vapor        EOS_ThetisFreonR12Vapor   R12        Unknown  0" >> index.eos





# model-version.txt
cp $THETIS_ROOT_DIR/model-version.txt $BINARY_DIR
cat $THETIS_ROOT_DIR/model-version.txt | col -b |\
   tr '[A-Z]' '[a-z]'  >> $EOS_BINARY_DIR/model-version.txt

# ----------------------------------------
# end
exit 0
