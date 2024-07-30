#!/bin/bash
#
# T.M. Refprop9 source correction
#
set -o nounset -o errexit

# CORE_QUI.FOR
awk 'BEGIN{IGNORECASE=1; ind=0} \
     {if ($0 ~ /^ *function *PHIQUI/) {ind=1 ; print} \
      else if (ind == 1 && $0 ~ /^ *include/) {ind=0 ; print ; print "      betq=0.d0 ; betq10=0.d0 ; betq20=0.d0"} \
      else print}'  CORE_QUI.FOR >tmp$$ ; mv tmp$$ CORE_QUI.FOR

# FLSH_SUB.FOR
awk 'BEGIN{IGNORECASE=1; ind=0} \
     {if ($0 ~ /^ *subroutine *TPRHO/) {ind=1 ; print} \
      else if (ind == 1 && $0 ~ /^ *dimension *xdew/) {ind=0 ; print ; print "      rhom1=0.d0"} \
      else print}'  FLSH_SUB.FOR >tmp$$ ; mv tmp$$ FLSH_SUB.FOR


awk 'BEGIN{IGNORECASE=1; ind=0} \
     {if ($0 ~ /^ *subroutine *PBFLSH/) {ind=1 ; print} \
      else if (ind == 1 && $0 ~ /^ *bt=ab/) {ind=0 ; print ; print "      blbub=-1.d0"} \
      else print}'  FLSH_SUB.FOR >tmp$$ ; mv tmp$$ FLSH_SUB.FOR

# PROP_SUB.FOR
awk 'BEGIN{IGNORECASE=1; ind=0} \
     {if ($0 ~ /^ *subroutine *ENTHHC/) {ind=1 ; print} \
      else if (ind == 1 && $0 ~ /^ *dimension *xkhc/) {ind=0 ; print ; print "      INTEGER ntc ; ntc=0"} \
      else print}'  PROP_SUB.FOR >tmp$$ ; mv tmp$$ PROP_SUB.FOR

# SAT_SUB.FOR
awk 'BEGIN{IGNORECASE=1; ind1=0; ind2=0; ind3=0; ind4=0; ind5=0} \
     {if ($0 ~ /^ *subroutine *LGPHI/) {ind1=1 ; print} \
      else if (ind1 == 1 && $0 ~ /^ *character *herr/) {ind1=0 ; print ; print "      DOUBLE PRECISION pifactor ; pifactor=0.d0"} \
      else if ($0 ~ /^ *subroutine *SATGV/) {ind2=1 ; print} \
      else if (ind2 == 1 && $0 ~ /^ *common.*GVtemp/) {ind2=0 ; print ; print "      DOUBLE PRECISION s ; s=0.d0"} \
      else if ($0 ~ /^ *subroutine *EHMAXX/) {ind3=1 ; print} \
      else if (ind3 == 1 && $0 ~ /^ *dimension *x/) {ind3=0 ; print ; print "      DOUBLE PRECISION hold ; hold=0.d0"} \
      else if ($0 ~ /^ *subroutine *SATD/) {ind4=1 ; print} \
      else if (ind4 == 1 && $0 ~ /^ *dimension *tt/) {ind4=0 ; print ; print "      DOUBLE PRECISION f" ; print "      ft(1)=0.d0 ; ft(2)=0.d0 ; f=0.d0"} \
      else if ($0 ~ /^ *subroutine *SATSPLN/) {ind5=1 ; print} \
      else if (ind5 == 1 && $0 ~ /^ *common.*GVtemp/) {ind5=0 ; print ; print "      DOUBLE PRECISION s1 ; s1=0.d0"} \
      else print}'  SAT_SUB.FOR >tmp$$ ; mv tmp$$ SAT_SUB.FOR

# TRNS_VIS.FOR
awk 'BEGIN{IGNORECASE=1; ind=0} \
     {if ($0 ~ /^ *function *ETA4DG/) {ind=1 ; print} \
      else if (ind == 1 && $0 ~ /^ *i=icomp/) {ind=0 ; print "      INTEGER j" ;\
                                                       print "      DOUBLE PRECISION tau" ;\
                                                       print "      j = 0 ; tau = 0.d0" ;\
                                                       print} \
      else print}'  TRNS_VIS.FOR >tmp$$ ; mv tmp$$ TRNS_VIS.FOR

# TRNS_ECS.FOR
awk 'BEGIN{IGNORECASE=1; ind=0} \
     {if ($0 ~ /^ *function *DELHSV/) {ind=1 ; print} \
      else if (ind == 1 && $0 ~ /^ *parameter/) {ind=0 ; print ; print "      DO I=1,NX ; S(I) = 0.D0  ; ENDDO"} \
      else print}'  TRNS_ECS.FOR >tmp$$ ; mv tmp$$ TRNS_ECS.FOR


# Rename all functions to avoid conflicts with Refprop10 functions
cat *.FOR *.INC | awk '$0 ~ /^ *block *data/ || \
                       $0 ~ /^ *function/    || \
                       $0 ~ /^ *common/      || \
                       $0 ~ /^ *subroutine/ {sub(" *block *data",""); \
                      sub(" *function",""); \
                      sub(" *common */","");; sub("/.*",""); \
                      sub(" *subroutine",""); sub("\\(.*$",""); print}' | sort |uniq  > e_bfs$$

sed -i -e "s/ *//g" e_bfs$$
SUFFIX=_RP9
for nfunc in `cat e_bfs$$`
do
   sed -i -e "s/\(\b${nfunc}\)\(\b\)/\1${SUFFIX}\2/gI" *.FOR *.INC
   sed -i -e "s/\('\[${nfunc}\)${SUFFIX}/\1/gI" *.FOR
done

rm e_bfs$$

# Can't find out why the functions reseta and unsetaga are not taken into account... weird
# Rename global variables common to refprop 9 and 10
for COMM_VAR in REFSAV FLAGS RESETA TCXH2 BDSET UNSETAGA ; do
   sed -i -e "s/\(\b${COMM_VAR}\)\(\b\)/\1${SUFFIX}\2/g" *.FOR *.INC
done

# Previous sed made some un-wanted changes
sed -i -e "s?^\([^c].*'.*\)${SUFFIX}\(.*'\)?\1\2?gI" *
sed -i -e "s/'\(stn\)${SUFFIX}'/'\1'/gI" -e "s/'\(cp0\)${SUFFIX}/'\1/gI" SETUP2.FOR SETUP.FOR UTILITY.FOR

exit 0
