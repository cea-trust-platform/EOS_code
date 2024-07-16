#!/bin/bash
#
# T.M. source files : Correction
#
#

fSrc=`basename $1`

##### Patch Refprop 10 source files
echo "Patch Refprop 10 source file $fSrc"


if test $fSrc == "SETUP.FOR"
then
# file SETUP.FOR, subroutine SETFLD :
# change  call RFFILE (hFile(i),hreff)  to  call RFFILE (hreff,hFile(i))
  awk 'BEGIN{IGNORECASE=1; ind=0} \
     {if ($0 ~ /^ *subroutine *SETFLD/) {ind=1 ; print} \
      else if (ind == 1 && $0 ~ /hreff.*\.ne/)  {ind=2 ; print} \
      else if (ind == 2 && $0 ~ /call *RFFILE/) {ind=0 ; sub("\\(hFile\\(i\\),hreff\\)","(hreff,hFile(i))"); sub("!","! M.F. "); print} \
      else print}'  SETUP.FOR >tmp$$ ; mv tmp$$ SETUP.FOR

# file SETUP.FOR, modification  subroutine RFFILE
  :>setup.be
  :>setup.af
  awk 'BEGIN{IGNORECASE=1; ind=0} \
     {if ($0 ~ /^ *subroutine *RFFILE/) {ind=1} \
      else if (ind == 1 && $0 ~ /subroutine *RFFILE/)  {ind=2} \
      else if (ind == 0) {print >>  "setup.be"} \
      else if (ind == 2) {print >>  "setup.af"}}'  SETUP.FOR

  cat setup.be new_rffile setup.af > SETUP.FOR

  # file SETUP.FOR, subroutine SETMOD :
  # information -912 is now in a comment
  sed -i -e 's/^ *call *ERRNUM.*-912/* M.F. &/' SETUP.FOR
fi

if test $fSrc == "SAT_SUB.FOR"
then 
# file SAT_SUB.FOR, subroutine SATP : initialization T = 0.e0
  awk 'BEGIN{IGNORECASE=1; ind=0} \
     {if ($0 ~ /^ *subroutine *SATP/) {ind=1 ; print} \
      else if (ind == 1 && $0 ~ /iSatFlg1/)  {ind=0 ; print "      T = 0.e0 ;"; print} \
      else print}'  SAT_SUB.FOR >tmp$$ ; mv tmp$$ SAT_SUB.FOR
fi

if test $fSrc == "TRNSP.FOR"
then 
# file TRNSP.FOR, subroutine TRNECS : initialization T0 = 0.e0
awk 'BEGIN{IGNORECASE=1; ind=0} \
     {if ($0 ~ /^ *subroutine *TRNECS/) {ind=1 ; print} \
      else if (ind == 1 && $0 ~ /^ *call *TRNEC/)  {ind=0 ; print "      T0 = 0.e0 ;"; print} \
      else print}'  TRNSP.FOR >tmp$$ ; mv tmp$$ TRNSP.FOR
fi

fmt=`file $fSrc`
if [[ $fmt == *"CRLF"* ]]; then
  dos2unix $fSrc
fi


exit 0


# AFAIRE awk 'BEGIN{IGNORECASE=1; ind=0} {if ($0 ~ /^ *subroutine *SETAG *(/) ind=1; if (ind==1 && $0 ~ /^ *include.*RP10_COMMONS/) {print NR; exit}}' $coco
