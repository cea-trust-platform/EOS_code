#!/bin/ksh
# Create refprop_key.txt for NIST-Refprop

ddist=$1
if [ ! -d $ddist ]; then
   echo " error :  $ddist  not a directory !"
   exit 1
fi
(cd $ddist ; touch refprop_key.txt ; chmod 640 refprop_key.txt)
exit 0