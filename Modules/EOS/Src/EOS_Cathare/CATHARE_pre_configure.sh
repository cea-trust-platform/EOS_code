#!/bin/bash
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
   CATHARE_SOURCES=$1
   BINARY_DIR=$2  # = ${CMAKE_BINARY_DIR}/Modules/EOS/Src/EOS_Cathare
   EOS_BINARY_DIR=$BINARY_DIR/../..
   ROOT_BINARY_DIR=$EOS_BINARY_DIR/../..
fi

# Is there something to do ?
if [ -f $EOS_BINARY_DIR/model-version.txt ] && grep -q -e "^ca " $EOS_BINARY_DIR/model-version.txt ; then exit 0 ; fi


if [ ! -d $CATHARE_SOURCES && ! -f $CATHARE_SOURCES ] ; then
   eos_error 51 "Cathare plugin not found, option --with-cathare=$CATHARE_SOURCES"
fi

# Unzip if path is an archive
if [ -f $CATHARE_SOURCES ]; then
   mkdir -p $ROOT_BINARY_DIR/plugins_tmp
   tar xf $CATHARE_SOURCES -C $ROOT_BINARY_DIR/plugins_tmp
   CATHARE_SOURCES=`find $ROOT_BINARY_DIR/plugins_tmp/ -maxdepth 1 -name "cathare-*" |xargs realpath`
fi

# M.T. Name and prefix
# --------------------
MT="CA"
PREFIX="${MT}_"
prefix=`echo $PREFIX|tr '[A-Z]' '[a-z]'`



#
# Cathare source files in $BINARY_DIR/Cathare
# ====================
dirdest="$BINARY_DIR/Cathare"
mkdir -p $dirdest
echo " + Copy Cathare source files in $dirdest"



# Tools
# ------
# file awk n.1
fawk1=$dirdest/f1$$.awk
cat <<"eof" >$fawk1
BEGIN{IGNORECASE=1}
{ if ($0 ~ /^ *SUBROUTINE/)
     { cha = $0
       sub("SUBROUTINE","",cha)
       split(cha,tab," ")
       split(tab[1],tab2,"(")
       printf("CALL *%s|CALL %s%s\n",tab2[1],PREFIX,tab2[1])

       sub(tab2[1],PREFIX""tab2[1])
       print >> filer
     }
  else if ($0 ~ /^ *FUNCTION/)
     { cha = $0
       sub("FUNCTION","",cha)
       split(cha,tab," ")
       split(tab[1],tab2,"(")
       printf("%s|%s%s\n",tab2[1],PREFIX,tab2[1])

       sub(tab2[1],PREFIX""tab2[1])
       print >> filer
     }
  else if ($0 ~ /^ *[^*C][A-Z]* *FUNCTION/)
     { cha = $0
       sub(" *[A-Z]* *FUNCTION","",cha) ;
       split(cha,tab," ")
       split(tab[1],tab2,"(")
       printf("%s|%s%s\n",tab2[1],PREFIX,tab2[1])

       sub(tab2[1],PREFIX""tab2[1])
       print >> filer
     }
  else if ($0 ~ /^ *[^*C][A-Z]* *[A-Z]* *FUNCTION/)
     { cha = $0
       sub(" *[A-Z]* *[A-Z]* *FUNCTION","",cha)
       split(cha,tab," ")
       split(tab[1],tab2,"(")
       printf("%s|%s%s\n",tab2[1],PREFIX,tab2[1])

       sub(tab2[1],PREFIX""tab2[1])
       print >> filer
     }
  else if ($0 ~ /^ *PROGRAM/)
     { cha = $0
       sub("PROGRAM","",cha)
       split(cha,tab," ")
       split(tab[1],tab2,"(")
       printf("%s|%s%s\n",tab2[1],PREFIX,tab2[1])

       sub(tab2[1],PREFIX""tab2[1])
       print >> filer
     }
  else if ($0 ~ /^ *ENTRY/)
     { cha = $0
       sub("ENTRY","",cha)
       split(cha,tab," ")
       split(tab[1],tab2,"(")
       printf("%s|%s%s\n",tab2[1],PREFIX,tab2[1])

       sub(tab2[1],PREFIX""tab2[1])
       print >> filer
     }
  else if ($0 ~ /^ *BLOCK *DATA/)
     { cha = $0
       sub("BLOCK","",cha)
       sub("DATA","")
       split(cha,tab," ")
       split(tab[1],tab2,"(")
       printf("%s|%s%s\n",tab2[1],PREFIX,tab2[1])

       sub(tab2[1],PREFIX""tab2[1])
       print >> filer
     }
  else
     print >> filer
}
eof

# file awk n.2
fawk2=$dirdest/f2$$.awk
cat <<"eof" >$fawk2
BEGIN{IGNORECASE=1}
{ if ($0 ~ /^ *INCLUDE *'/)
     { sub("'","'"PREFIX)
       print
     }
  else if ($0 ~ /^ *INCLUDE *"/)
     { sub("\"","\""PREFIX)
       print
     }
  else
     print
}
eof



# . Copy and Rename fortran files
# . List of strings to remplace in fortran subroutine
#        file flist_procf :  'CALL {nomproc}|CALL {PREFIX}{nomproc}'
# . Replace fortran subroutine name in new fortran files
#   -------------------------------
flist_procf=$dirdest/ftmp_proc$$.txt
:> $flist_procf
for filef in `find $CATHARE_SOURCES -name "*.F"`
do
   filer=$dirdest/${PREFIX}$(basename $filef)
   awk -v filer=$filer -v PREFIX=$PREFIX -f $fawk1 $filef >> $flist_procf
done


# Replace fortran subroutine call
# -------------------------------
(cd $dirdest
 for filef in `find . -name "*.F"`
 do
    while read chain
    do
       ch1=`echo $chain| awk -F'|' '{print $1}'`
       ch2=`echo $chain| awk -F'|' '{print $2}'`
       awk -v ch1="$ch1" -v ch2="$ch2" \
           'BEGIN{IGNORECASE=1} {sub(ch1,ch2); print}' $filef > ftmp$$
       mv ftmp$$  $filef
    done < $flist_procf
 done
)


# . Copy and Rename include files
# . List of include files
#   ---------------------
list_includef=''
for fileh in `find $CATHARE_SOURCES -name "*.H"`
do
  cp $fileh $dirdest/${PREFIX}$(basename $fileh)
  list_includef="${list_includef} $(basename $fileh)"
done




# Replace include files call in new fortran files
# --------------------------
(cd $dirdest
 for filef in `find . -name "*.F"`
 do
    awk -v PREFIX=$PREFIX -f $fawk2 $filef > ftmp$$
    mv ftmp$$  $filef
 done
)



#
# EOS source files in $BINARY_DIR
# ================
dirdest="$BINARY_DIR"
echo " + Copy EOS source files in $dirdest"

# file awk n.3
fawk3=$dirdest/f3$$.awk
cat <<"eof" >$fawk3
BEGIN{IGNORECASE=1}
{ if ($0 ~ /F77DECLARE\(/)
     { sub("F77DECLARE\\(","F77DECLARE("prefix)
       print
     }
  else if ($0 ~ /F77NAME\(/)
     { sub("F77NAME\\(","F77NAME("prefix)
       print
     }
  else
     print
}
eof


#
# EOS_Cathare Data
# ================
dirdest="$EOS_BINARY_DIR/Data/EOS_Cathare"
echo " + Build EOS index.eos file in $dirdest"
mkdir -p $dirdest
(cd $dirdest
 :> index.eos
 echo "Cathare  WaterLiquid    EOS_CatharWaterLiquid      Water    2004-032.pdf  0" >> index.eos
 echo "Cathare  WaterVapor     EOS_CatharWaterVapor       Water    2004-032.pdf  0" >> index.eos
 echo "Cathare  R12Liquid      EOS_Cathare_FreonR12Liquid R12      Unkown        0" >> index.eos
 echo "Cathare  R12Vapor       EOS_Cathare_FreonR12Vapor  R12      Unkown        0" >> index.eos
)


rm $flist_procf $fawk1 $fawk2 $fawk3

# model-version.txt
cp $CATHARE_SOURCES/model-version.txt $BINARY_DIR
cat $CATHARE_SOURCES/model-version.txt | col -b |\
   tr '[A-Z]' '[a-z]'  >> $EOS_BINARY_DIR/model-version.txt

# ----------------------------------------
# end
exit 0
