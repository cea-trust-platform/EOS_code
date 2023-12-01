#!/bin/bash
#
# T.M. Cathare2 source correction
#

# Icare gas Equations of State : remove empty files
rm C2_FHDIXAIC.f C2_FHGAXAIC.f C2_FHGAXIC.f C2_FHVAPAic.f C2_FHVAPPic.f C2_FHXSUMIC.f

ner=0
for file in `ls *.f`
do
   # no comment lines
   awk 'BEGIN{IGNORECASE=1} $0 !~ /^*|^C/ {print}' $file > filec$$
   # from string str80 to str120
   if [ `grep -ic "str80" $file` -ne 0 ]; then 
      awk 'BEGIN{IGNORECASE=1} \
           {if ($0 ~ /str80/) {gsub("80","120"); print} else print}' $file >tmp$$ ; mv tmp$$ $file
   fi
   # no  '90000 FORMAT'
   if [ `grep -ic "^ *90000 *FORMAT" $file` -ne 0 ]; then 
      awk 'BEGIN{IGNORECASE=1} \
           {if ($0 ~ /^ *90000 *FORMAT/) {sub("^","* "); print} else print}' $file >tmp$$ ; mv tmp$$ $file
   fi
   # replace "WRITE (str120,90000)"  by "WRITE (str120,'("name : ")')"
   if [ `grep -ic "^ *[0-9]* *WRITE.*str120.*90000" $file` -ne 0 ]; then 
      nsub=`echo $file|awk -F'.' '{print $1}'`
      awk -v nsub=$nsub -v ner=$ner -v proc=$$ 'BEGIN{IGNORECASE=1; nomf="/tmp/liste"proc; nomfe="/tmp/erreur"proc} \
          {if ($0 ~ /^ *[0-9]* *WRITE.*str120.*90000/) {ner++; c="+++"; rem=c"(\"" nsub " : "ner" : \")"c; \
           sub("90000",rem); print; print nsub" : "ner>> nomf} else print} END{print ner> nomfe}' $file |\
          sed -e s"/+++/'/g"  >tmp$$ ; mv tmp$$ $file
      ner=`cat /tmp/erreur$$`
   fi
   # repplace  "WRITE (IOSTER,90000)"  by  CHARACTER*120 str120 ; "WRITE (str120,'("name : ")') ; CALL C2_ERPILE (str120)"
   if [ `grep -ic "^ *[0-9]* *WRITE.*IOSTER.*90000" $file` -ne 0 ]; then 
      nsub=`echo $file|awk -F'.' '{print $1}'`
      awk -v nsub=$nsub -v ner=$ner -v proc=$$ 'BEGIN{IGNORECASE=1; nomf="/tmp/liste"proc; nomfe="/tmp/erreur"proc} \
          {if ($0 ~ /^ *[0-9]* *WRITE.*IOSTER.*90000/) {ner++; c="+++"; rem=c"(\"" nsub " : "ner" : \")"c; sub("90000",rem); sub("IOSTER","str120");\
            print ; print "      CALL C2_ERPILE (str120)";  print nsub" : "ner>> nomf} else print} END{print ner> nomfe}' $file |\
          sed -e s"/+++/'/g" >tmp$$ ; mv tmp$$ $file
      if [ `grep -ic "^ *CHARACTER.*120.*str120" $file` -eq 0 ]; then
         awk 'BEGIN{IGNORECASE=1; n=0} {if ($0 ~ /^ *INCLUDE/ && n==0) {n=1; print "      CHARACTER*120 str120"; print} else print}' $file  >tmp$$ ; mv tmp$$ $file
      fi
      ner=`cat /tmp/erreur$$`
   fi

   # label 8888
   if [ `grep -ic "8888" filec$$` -eq 1 ]; then
      sed -i -e 's/^ *8888/     /' $file 
   fi

   # label 9999
   if [ `grep -ic "9999" filec$$` -eq 1 ]; then
      sed -i -e 's/^ *9999/     /' $file 
   fi
   
done

# variable XLAST not used 
if [ -f C2_LUTIL.H ]; then
   sed -i -e 's/\(^ *DOUBLE PRECISION XLAST\)/*\1/I' C2_LUTIL.H 
fi

# variable I not used 
if [ -f C2_INIP2.f ]; then
   sed -i -e 's/\(^ *INTEGER *I\)/*\1/I' C2_INIP2.f 
fi
if [ -f C2_INIP5.f ]; then
   sed -i -e 's/\(^ *INTEGER *I\)/*\1/I' C2_INIP5.f 
fi
if [ -f C2_INIP6.f ]; then
   sed -i -e 's/\(^ *INTEGER *I\)/*\1/I' C2_INIP6.f 
fi
if [ -f C2_INIFLD.f ]; then
   sed -i -e 's/\(^ *INTEGER *I\)/*\1/I' C2_INIFLD.f 
fi
if [ -f C2_INI1P.f ]; then
   sed -i -e 's/\(^ *INTEGER *I\)/*\1/I' C2_INI1P.f 
fi
if [ -f C2_FI3.f ]; then
   sed -i -e 's/\(^ *INTEGER\) *I,/\1/I' C2_FI3.f 
fi

# variable D2HDP2 not used 
if [ -f C2_HCP3te1.f ]; then
   sed -i -e 's/\(^ *DOUBLE *PRECISION *D2HDP2\)/*\1/I' C2_HCP3te1.f 
fi
if [ -f C2_HCP3te2.f ]; then
   sed -i -e 's/\(^ *DOUBLE *PRECISION *D2HDP2\)/*\1/I' C2_HCP3te2.f 
fi

# variables TGiter, ... not used
if [ -f C2_FHVPNa.f ]; then
   sed -i -e 's/\(^ *DOUBLE *PRECISION *TGiter\)/*\1/I' C2_FHVPNa.f 
   sed -i -e 's/, *CPV0//I'                             C2_FHVPNa.f 
   sed -i -e 's/^ *DOUBLE *PRECISION.*DRV3iter//I'      C2_FHVPNa.f 
   sed -i -e 's/^ *DOUBLE *PRECISION.*DCPV3iter//I'     C2_FHVPNa.f 
   sed -i -e 's/^ *DOUBLE *PRECISION.*DHV1iter//I'      C2_FHVPNa.f 
   sed -i -e 's/, *DTV *, *RHS//I'                      C2_FHVPNa.f 
   sed -i -e 's/, *IER *, *ITEST//I'                    C2_FHVPNa.f 
   sed -i -e 's/MAXIT *, *IDIVERGE *, *ITER *,//I'      C2_FHVPNa.f 
   sed -i -e 's/^ *PARAMETER.*MAXIT=50)//I'             C2_FHVPNa.f 
fi

# variables I, ILL, ... not used 
if [ -f C2_FHVAPVDHO.f ]; then
   sed -i -e 's/, *I *, *ILL *, *IVSTAT//I'             C2_FHVAPVDHO.f 
   sed -i -e 's/, *ZQ0 *, *ZQH//I'                      C2_FHVAPVDHO.f 
fi

# variables DCP1, ... not used 
if [ -f C2_FHXCP.f ]; then
   sed -i -e 's/, DCP1 *, *DCP2 *, *DCP3//I'            C2_FHXCP.f 
   awk 'BEGIN{IGNORECASE=1} \
        {if ($0 ~ /END_*DOCUMENT/) {print ;\
         print "      XNORM=ZERO ; CPFIX=ZERO"} else print}' C2_FHXCP.f >tmp$$ ; mv tmp$$ C2_FHXCP.f
fi

# variables IER, ITEST  not used
if [ -f C2_FHVAPNa.f ]; then
   sed -i -e 's/, *IER//'    C2_FHVAPNa.f 
   sed -i -e 's/, *ITEST//'  C2_FHVAPNa.f 
fi



# variables not used de C2_FHVAPA
if [ -f C2_FHVAPA.f ]; then
   sed -i -e 's/^ *DOUBLE PRECISION TLAV0,DLAGT0,DLAVP0,DLAPT0//I'          C2_FHVAPA.f 
   sed -i -e 's/^ *1 *,TLAV1,DLAGT1,DLAVP1//I'                              C2_FHVAPA.f 
fi


# variables not used de C2_FHLIQDHO
if [ -f C2_FHLIQDHO.f ]; then
   sed -i -e 's/,NP,NHL//I'                                                 C2_FHLIQDHO.f 
   sed -i -e 's/^ *DOUBLE PRECISION DTL2S,DTL2S1,DTL2S2,DTL2S21,DTL2S22//I' C2_FHLIQDHO.f 
   sed -i -e 's/^ *1 *,TL3,TL4,DTL31,DTL41//I'                              C2_FHLIQDHO.f 
   sed -i -e 's/TSP0(1),HLSP0(1),//I'                                       C2_FHLIQDHO.f 
   sed -i -e 's/^ *DOUBLE PRECISION TLT,DTLT1,DTLT2,DTLT21,DTLT22//I'       C2_FHLIQDHO.f 
   sed -i -e 's/^ *DOUBLE PRECISION TLA,TLA1,TLA2,TLA21,TLA22,TLA221//I'    C2_FHLIQDHO.f 
   sed -i -e 's/^ *1 *,TL3,TL4,DTL31,DTL41//I'                              C2_FHLIQDHO.f 
   sed -i -e 's/^ *DOUBLE PRECISION RLT,DRLT1,DRLT2,RLS,DRLS1,DRLS2//I'     C2_FHLIQDHO.f 
   sed -i -e 's/^ *DOUBLE PRECISION RLA,RLA1,RLA2,RLA21//I'                 C2_FHLIQDHO.f 
   sed -i -e 's/DHLS,HLSM,P2,//I'                                           C2_FHLIQDHO.f 
   sed -i -e 's/,DCPL01,DCPL02//I'                                          C2_FHLIQDHO.f 
   sed -i -e 's/,DRLS21,DRLS22//I'                                          C2_FHLIQDHO.f 
   sed -i -e 's/,DTL021,DTL022//I'                                          C2_FHLIQDHO.f 
   sed -i -e 's/,DCPDTL,DTL21,DTL22,DCPL21//I'                              C2_FHLIQDHO.f 
   sed -i -e 's/^ *DOUBLE PRECISION K,KP,KHL,DKP1,DKHL2,DKP11,DKHL22//I'    C2_FHLIQDHO.f 
   sed -i -e 's/^ *DOUBLE PRECISION DLHL,DHL,HLAUX//I'                      C2_FHLIQDHO.f 
fi


# variables not used de C2_FHVAPADHO
if [ -f C2_FHVAPADHO.f ]; then
   sed -i -e 's/^ *DOUBLE PRECISION TLAV0,DLAGT0,DLAVP0,DLAPT0//I'       C2_FHVAPADHO.f 
   sed -i -e 's/^ *1 *,TLAV1,DLAGT1,DLAVP1//I'                           C2_FHVAPADHO.f 
fi

# variables not used de C2_FHVAPDHO
if [ -f C2_FHVAPDHO.f ]; then
   sed -i -e 's/\(^ *DOUBLE PRECISION DRV170H,DRV260H,DRV170S,DRV260S\)/*\1/I'              C2_FHVAPDHO.f 
   sed -i -e 's/\(^ *DOUBLE PRECISION *DRVHVS\)\(,DRVHHS,DRVHHP\)/\1/I'                     C2_FHVAPDHO.f 
   sed -i -e 's/\(^ *DOUBLE PRECISION K0,K1,RV170,RV260,P260,HVMHVS2\)/*\1/I'               C2_FHVAPDHO.f 
   sed -i -e 's/\(^ *DOUBLE PRECISION VID1,VID2,VID3,VID4,VID5,VID6,VID7,VID8,VID9\)/*\1/I' C2_FHVAPDHO.f 
fi


# variables DCPVSDTGS, J not used
if [ -f C2_FTVAPNa.f ]; then
   sed -i -e 's/, *DCPVSDTGS//I' C2_FTVAPNa.f 
   sed -i -e 's/, *J//I'         C2_FTVAPNa.f 
fi

# variables IDEBUT, IFIN not used
if [ -f C2_DENST3F.f ]; then
   sed -i -e 's/^ *INTEGER *IDEBUT, *IFIN//I' C2_DENST3F.f                
fi
if [ -f C2_DENST3I.f ]; then
   sed -i -e 's/^ *INTEGER *IDEBUT, *IFIN//I' C2_DENST3I.f
fi
if [ -f C2_DENST3J.f ]; then
   sed -i -e 's/^ *INTEGER *IDEBUT, *IFIN//I' C2_DENST3J.f
fi
if [ -f C2_DENST3K.f ]; then
   sed -i -e 's/^ *INTEGER *IDEBUT, *IFIN//I' C2_DENST3K.f
fi
if [ -f C2_DENST3P.f ]; then
   sed -i -e 's/^ *INTEGER *IDEBUT, *IFIN//I' C2_DENST3P.f
fi
if [ -f C2_DENST3R.f ]; then
   sed -i -e 's/^ *INTEGER *IDEBUT, *IFIN//I' C2_DENST3R.f
fi
if [ -f C2_DENST3U.f ]; then
   sed -i -e 's/^ *INTEGER *IDEBUT, *IFIN//I' C2_DENST3U.f
fi
if [ -f C2_DENST3V.f ]; then
   sed -i -e 's/^ *INTEGER *IDEBUT, *IFIN//I' C2_DENST3V.f
fi
if [ -f C2_DENST3W.f ]; then
   sed -i -e 's/^ *INTEGER *IDEBUT, *IFIN//I' C2_DENST3W.f
fi
if [ -f C2_DENST3X.f ]; then
   sed -i -e 's/^ *INTEGER *IDEBUT, *IFIN//I' C2_DENST3X.f
fi
if [ -f C2_DENST3Y.f ]; then
   sed -i -e 's/^ *INTEGER *IDEBUT, *IFIN//I' C2_DENST3Y.f
fi
if [ -f C2_DENST3Z.f ]; then
   sed -i -e 's/^ *INTEGER *IDEBUT, *IFIN//I' C2_DENST3Z.f
fi

# variables DTMUGAX, DUSTMUGBX not used
if [ -f C2_FHDIXA.f ]; then
   sed -i -e 's/ *DTMUGAX *,//I' C2_FHDIXA.f 
   sed -i -e 's/, *DUSTMUGBX//I' C2_FHDIXA.f 
fi

# variables IVSTAT, IDER not used
if [ -f C2_TSAT4.f ]; then
   sed -i -e 's/, *IVSTAT *, *IDER//I' C2_TSAT4.f 
fi

# variable T not used
if [ -f C2_TRANMS.f ]; then
   sed -i -e 's/^ *DOUBLE *PRECISION *T//I'                            C2_TRANMS.f 
   sed -i -e 's/^ *DOUBLE *PRECISION *RHO *, *DRHODT *, *DRHODP *,//I' C2_TRANMS.f 
   sed -i -e 's/^ *& *D2RHODT2 *, *D2RHODTDP *, *D2RHODP2//I'          C2_TRANMS.f 
fi

# variable DENSOLD not used
if [ -f C2_TEMPH3.f ]; then
   sed -i -e 's/, *DENSOLD//I' C2_TEMPH3.f 
fi

# variable PI2RED not used
if [ -f C2_TEMPH2A.f ]; then
   sed -i -e 's/PI2RED *,//I' C2_TEMPH2A.f 
fi

# variable FLUID not used
if [ -f C2_REGTPSV.f ]; then
   sed -i -e 's/^ *CHARACTER.*FLUID//I' C2_REGTPSV.f 
fi
if [ -f C2_REGTPMSV.f ]; then
   sed -i -e 's/^ *CHARACTER.*FLUID//I' C2_REGTPMSV.f 
fi


# variables not used
if [ -f C2_FHVAPV.f ]; then
   sed -i -e 's/I *, *ILL *, *IVSTAT *,//I' C2_FHVAPV.f 
   sed -i -e 's/, *ZQ0 *, *ZQH//I' C2_FHVAPV.f 
fi


# variables not used
if [ -f C2_FTLIQDHO.f ]; then
   sed -i -e 's/^ *DOUBLE *PRECISION *PH1 *, *PH2 *, *PH3 *,*PH4 *,*DPH2 *,*DPH3//I' C2_FTLIQDHO.f
   sed -i -e 's/^ *1 *,R0,R2,R3,R4/      DOUBLE PRECISION R0,R2,R3,R4/I'             C2_FTLIQDHO.f
fi

# variable HIMAX not used
if [ -f C2_REGHPSL.f ]; then
   sed -i -e 's/, *H1MAX//I' C2_REGHPSL.f 
fi

# variables INREG31, INREG32 not used
if [ -f C2_REGHPS.f ]; then
   sed -i -e 's/^ *LOGICAL.*INREG32//I' C2_REGHPS.f 
fi

# variables not used
if [ -f C2_REGHPMSV.f ]; then
   sed -i -e 's/^ *DOUBLE.*D2T32DP2//I' C2_REGHPMSV.f 
   sed -i -e 's/, *H3MAX//I'            C2_REGHPMSV.f 
fi

# variables not used
if [ -f C2_REGHPMSL.f ]; then
   sed -i -e 's/^ *INTEGER *I,/      INTEGER/I' C2_REGHPMSL.f 
   sed -i -e 's/, *H3MIN//I'                    C2_REGHPMSL.f 
fi


# variables not used
if [ -f C2_PSAT4.f ]; then
   sed -i -e 's/^ *&.*DPBOUNDDT//I'                C2_PSAT4.f 
   sed -i -e 's/^ *&.*DLIMTDP//I'                  C2_PSAT4.f 
   sed -i -e 's/, *D2TSATDP2 *, *$/, D2TSATDP2/I'  C2_PSAT4.f 
   sed -i -e 's/, *D2MAXTDP2 *, *$/, D2MAXTDP2/I'  C2_PSAT4.f 
fi

# variables not used
if [ -f C2_PBOUND23.f ]; then
   sed -i -e 's/^ *DOUBLE *.*D2LIMTDP2//I'     C2_PBOUND23.f 
fi


# l'argument B de la fonction NAND
if [ -f C2_NAND.f ]; then
   sed -i -e 's/(B)/()/I'                      C2_NAND.f 
   sed -i -e 's/DOUBLE *PRECISION *B//I'       C2_NAND.f 
fi


# variables not used
if [ -f C2_FHLIQLBe.f ]; then
   sed -i -e 's/, *D2RLDTL2//I'        C2_FHLIQLBe.f 
fi

# variables not used
if [ -f C2_FHLIQPb.f ]; then
   sed -i -e 's/, *D2RLDTL2//I'        C2_FHLIQPb.f 
fi

# variables not used
if [ -f C2_FHLIQPb.f ]; then
   sed -i -e 's/, *D2RLDTL2//I'        C2_FHLIQPb.f 
fi

# variables not used
if [ -f C2_FHVAP.f ]; then
   sed -i -e 's/, *DRVHHS *, *DRVHHP//I'      C2_FHVAP.f 
   sed -i -e 's/^ *DOUBLE.*DRV260S//I'        C2_FHVAP.f 
   sed -i -e 's/^ *DOUBLE.*HVMHVS2//I'        C2_FHVAP.f 
   sed -i -e 's/^ *DOUBLE.*VID9//I'           C2_FHVAP.f 
fi

# variables not used
if [ -f C2_FHVAPPDHO.f ]; then
   sed -i -e 's/, *DRVBHP//I'      C2_FHVAPPDHO.f 
   sed -i -e 's/, *HVMHA//I'       C2_FHVAPPDHO.f 
   sed -i -e 's/, *DTGBHP//I'      C2_FHVAPPDHO.f 
fi

# variables not used
if [ -f C2_FHVAPP.f ]; then
   sed -i -e 's/, *DRVBHP//I'      C2_FHVAPP.f 
   sed -i -e 's/, *DTGBHP//I'      C2_FHVAPP.f 
   sed -i -e 's/, *HVMHA//I'       C2_FHVAPP.f 
fi

# variables not used
if [ -f C2_INITP2te7.f ]; then
   sed -i -e 's/INTEGER *I *$//I'      C2_INITP2te7.f 
fi

# variables not used
if [ -f C2_HCPMS.f ]; then
   sed -i -e 's/DOUBLE *PRECISION *D2HDP2//I'  C2_HCPMS.f 
fi

# variables not used
if [ -f C2_FVAPAIAPWS.f ]; then
   sed -i -e 's/, *IENC//I'  C2_FVAPAIAPWS.f 
fi

# variables not used
if [ -f C2_GAM1.f ]; then
   sed -i -e 's/INTEGER *I *,/INTEGER/I'      C2_GAM1.f 
fi
if [ -f C2_GAM2.f ]; then
   sed -i -e 's/INTEGER *I *,/INTEGER/I'      C2_GAM2.f 
fi
if [ -f C2_GAM5.f ]; then
   sed -i -e 's/INTEGER *I *,/INTEGER/I'      C2_GAM5.f 
fi
if [ -f C2_GAM6.f ]; then
   sed -i -e 's/INTEGER *I *,/INTEGER/I'      C2_GAM6.f 
fi
if [ -f C2_GAM7.f ]; then
   sed -i -e 's/INTEGER *I *,/INTEGER/I'      C2_GAM7.f 
fi

# variables not used
if [ -f C2_INIT2.f ]; then
   sed -i -e 's/\( *INTEGER *IDEBUT *, *IFIN\).*/\1/I'     C2_INIT2.f 
   sed -i -e 's/POLY20JNEG *,//I'                          C2_INIT2.f 
   sed -i -e 's/POLY20JPOS *,//I'                          C2_INIT2.f 
   sed -i -e 's/DOUBLE *PRECISION *TEMP//I'                C2_INIT2.f 
fi
if [ -f C2_INIT3.f ]; then
   sed -i -e 's/\( *INTEGER *IDEBUT *, *IFIN\).*/\1/I'     C2_INIT3.f 
   sed -i -e 's/DOUBLE *PRECISION *TEMP//I'                C2_INIT3.f 
   sed -i -e 's/, *USWTAU *//I'                            C2_INIT3.f 
fi
if [ -f C2_INIT5.f ]; then
   sed -i -e 's/\( *INTEGER *IDEBUT *, *IFIN\).*/\1/I'     C2_INIT5.f 
   sed -i -e 's/POLY50JNEG *,//I'                          C2_INIT5.f 
   sed -i -e 's/POLY50JPOS *,//I'                          C2_INIT5.f 
   sed -i -e 's/DOUBLE *PRECISION *TEMP//I'                C2_INIT5.f 
fi
if [ -f C2_INIT6.f ]; then
   sed -i -e 's/\( *INTEGER *IDEBUT *, *IFIN\).*/\1/I'     C2_INIT6.f 
   sed -i -e 's/POLY60JNEG *,//I'                          C2_INIT6.f 
   sed -i -e 's/POLY60JPOS *,//I'                          C2_INIT6.f 
   sed -i -e 's/DOUBLE *PRECISION *TEMP//I'                C2_INIT6.f 
fi
if [ -f C2_INIT7.f ]; then
   sed -i -e 's/\( *INTEGER *IDEBUT *, *IFIN\).*/\1/I'     C2_INIT7.f 
   sed -i -e 's/POLY70JNEG *,//I'                          C2_INIT7.f 
   sed -i -e 's/POLY70JPOS *,//I'                          C2_INIT7.f 
   sed -i -e 's/DOUBLE *PRECISION *TEMP//I'                C2_INIT7.f 
fi

# variables not used
if [ -f C2_FTSATPDHO.f ]; then
   sed -i -e 's/^ *DOUBLE *PRECISION *A,B,C,DB,DC//I'                       C2_FTSATPDHO.f
   sed -i -e 's/^ *DOUBLE PRECISION PH1,PH2,PH3,PH4,CHA1,CHA2,DPH2,DPH3//I' C2_FTSATPDHO.f
   sed -i -e 's/^ *DOUBLE PRECISION CPLS0,DCPLS0,CPVS0,DCPVS0//I'           C2_FTSATPDHO.f
   sed -i -e 's/^ *DOUBLE PRECISION CPS1,DCPS1,CPVS1,DCPVS1//I'             C2_FTSATPDHO.f
   sed -i -e 's/^ *DOUBLE PRECISION HLS0,DHLS0,HVS0,DHVS0//I'               C2_FTSATPDHO.f
   sed -i -e 's/^ *DOUBLE PRECISION HLS1,DHLS1,HVS1//I'                     C2_FTSATPDHO.f
   sed -i -e 's/^ *DOUBLE PRECISION DP,DLP,TSP0,DTSP0//I'                   C2_FTSATPDHO.f
   sed -i -e 's/^ *DOUBLE PRECISION RLS0,DRLS0,RVS0,DRVS0//I'               C2_FTSATPDHO.f
   sed -i -e 's/^ *DOUBLE PRECISION RLS1,DRLS1,RVS1,DRVS1//I'               C2_FTSATPDHO.f
fi

# argument FLUID not used in  C2_INIHPMS0
if [ -f C2_INIHPMS0.f ]; then
   sed -i -e 's/, *FLUID *,/,/I'                        C2_INIHPMS0.f
   sed -i -e 's/^ *CHARACTER.*FLUID//I'                 C2_INIHPMS0.f
   sed -i -e '/^ *CALL *C2_INIHPMS0/s/, *FLUID *,/,/I'  C2_INIHPMS.f
fi

# argument FLUID not used in  C2_INITPMS0
if [ -f C2_INITPMS0.f ]; then
   sed -i -e 's/, *FLUID *,/,/I'                         C2_INITPMS0.f
   sed -i -e 's/^ *CHARACTER.*FLUID//I'                  C2_INITPMS0.f
   sed -i -e '/^ *CALL *C2_INITPMS0/s/, *FLUID *,/,/I'   C2_INITPMS.f
fi

# argument IDER not used in  C2_INITP3te1
if [ -f C2_INITP3te1.f ]; then
   sed -i -e 's/, *IDER//I'                             C2_INITP3te1.f
   sed -i -e '/^ *CALL *C2_INITP3te1/s/, *IDER *,/,/I'  C2_INIHPMS0.f
   sed -i -e '/^ *CALL *C2_INITP3te1/s/, *IDER *,/,/I'  C2_INITPMS0.f
   sed -i -e '/^ *CALL *C2_INITP3te1/s/, *IDER *,/,/I'  C2_TEMPH3te1.f

fi

# argument IDER not used in  C2_INITP3te2
if [ -f C2_INITP3te2.f ]; then
   sed -i -e 's/, *IDER//I'                             C2_INITP3te2.f
   sed -i -e '/^ *CALL *C2_INITP3te2/s/, *IDER *,/,/I'  C2_INIHPMS0.f
   sed -i -e '/^ *CALL *C2_INITP3te2/s/, *IDER *,/,/I'  C2_INITPMS0.f
   sed -i -e '/^ *CALL *C2_INITP3te2/s/, *IDER *,/,/I'  C2_TEMPH3te2.f
fi

# argument IENC not used in  C2_FTSATPPb
if [ -f C2_FTSATPPb.f ]; then
   sed -i -e '/ *, *IENC *)/Id'                             C2_FTSATPPb.f
   sed -i -e 's/\( *, *RVSV *, *DRVSVV *$\)/\1)/I'          C2_FTSATPPb.f
   sed -i -e 's/, *IENC *$//I'                              C2_FTSATPPb.f

   sed -i -e '/ *, *IENC *)/Id'                             C2_FPSATTPb.f
   sed -i -e 's/\(^ *[0-9]* *, *RVSP *, *DRVSP1 *$\)/\1)/I' C2_FPSATTPb.f
   sed -i -e 's/, *IENC *$//I'                              C2_FPSATTPb.f

   sed -i -e '/ *, *IENC *)/Id'                             C2_FHINITPb.f
   sed -i -e 's/\(^ *[0-9]* *, *RVSV *, *DRVSVV *$\)/\1)/I' C2_FHINITPb.f
   sed -i -e 's/, *IENC *$//I'                              C2_FHINITPb.f
fi

# argument IENC not used in  C2_FTSATPLBe
if [ -f C2_FTSATPLBe.f ]; then
   sed -i -e '/ *, *IENC *)/Id'                             C2_FTSATPLBe.f
   sed -i -e 's/\( *, *RVSV *, *DRVSVV *$\)/\1)/I'          C2_FTSATPLBe.f
   sed -i -e 's/, *IENC *$//I'                              C2_FTSATPLBe.f

   sed -i -e '/ *, *IENC *)/Id'                             C2_FPSATTLBe.f
   sed -i -e 's/\(^ *[0-9]* *, *RVSP *, *DRVSP1 *$\)/\1)/I' C2_FPSATTLBe.f
   sed -i -e 's/, *IENC *$//I'                              C2_FPSATTLBe.f

   sed -i -e '/ *, *IENC *)/Id'                             C2_FHINITLBe.f
   sed -i -e 's/\(^ *[0-9]* *, *RVSV *, *DRVSVV *$\)/\1)/I' C2_FHINITLBe.f
   sed -i -e 's/, *IENC *$//I'                              C2_FHINITLBe.f
fi

# argument IENC not used in  C2_FTSATPNa
if [ -f C2_FTSATPNa.f ]; then
   sed -i -e '/ *, *IENC *)/Id'                             C2_FTSATPNa.f
   sed -i -e 's/\( *, *RVSV *, *DRVSVV *$\)/\1)/I'          C2_FTSATPNa.f
   sed -i -e 's/, *IENC *$//I'                              C2_FTSATPNa.f

   sed -i -e '/^ *[0-9]* *, *IENC *) *$/Id'                 C2_FPSATTNa.f
   sed -i -e 's/, *IENC *) *$//I'                           C2_FPSATTNa.f
   sed -i -e 's/\(^ *4 *,.*RVSP *, *DRVSP1 *$\)/\1)/I'      C2_FPSATTNa.f
   sed -i -e 's/\(^ *[0-9]* *, *RVSP *, *DRVSP1 *$\)/\1)/I' C2_FPSATTNa.f
   sed -i -e '/^ *INTEGER *IENC *$/Id'                      C2_FPSATTNa.f

   sed -i -e '/^ *[0-9]* *, *IENC *) *$/Id'                 C2_FHINITNa.f
   sed -i -e 's/, *IENC *) *$/)/I'                          C2_FHINITNa.f
   sed -i -e 's/, *IENC *$//I'                              C2_FHINITNa.f
   sed -i -e 's/\(^ *[0-9]* *, *RVSV *, *DRVSVV *$\)/\1)/I' C2_FHINITNa.f
fi

# 2 arguments CPG , FLDR not used in  C2_FHDIGA
if [ -f C2_FHDIGA.f ]; then
   sed -i -e 's/ *, *FLDR *$//I'                             C2_FHDIGA.f
   sed -i -e 's/PRECISION *FLDR *,/PRECISION /I'             C2_FHDIGA.f
   sed -i -e 's/ *, *CPG *,/,/I'                             C2_FHDIGA.f
   sed -i -e 's/,CPG(NSCA),/,/I'                             C2_FHDIGA.f
fi

# 5 arguments CPG , DLAVPV , DLAXTG , DMUVPV , DMUXTG   not used in  C2_FHDIXA
if [ -f C2_FHDIXA.f ]; then
   sed -i -e 's/, *CPG *,/,/I'                               C2_FHDIXA.f
   sed -i -e 's/ *, *CPG(NSCA) *,/,/I'                       C2_FHDIXA.f
   sed -i -e 's/ *, *DLAVPV *,/,/I'                          C2_FHDIXA.f
   sed -i -e 's/ *, *DLAVPV(NSCA) *,/,/I'                    C2_FHDIXA.f
   sed -i -e 's/ *, *DLAXTG *$//I'                           C2_FHDIXA.f
   sed -i -e 's/ *, *DLAXTG(NSCA) *$//I'                     C2_FHDIXA.f
   sed -i -e 's/ *, *DMUVPV *,/,/I'                          C2_FHDIXA.f
   sed -i -e 's/ *, *DMUVPV(NSCA) *,/,/I'                    C2_FHDIXA.f
   sed -i -e 's/ *, *DMUXTG *$//I'                           C2_FHDIXA.f
   sed -i -e 's/ *, *DMUXTG(NSCA) *$//I'                     C2_FHDIXA.f
fi

# 3 arguments GAMTAU, GAMTAU2, GAMTAU3 not used in  C2_DENSGAM
if [ -f C2_DENSGAM.f ]; then
   sed -i -e '/^ *& *GAMTAU *, *GAMTAU2 *, *GAMTAU3 *, *$/Id'                   C2_DENSGAM.f
   sed -i -e 's/^ *DOUBLE *PRECISION *GAMTAU *, *GAMTAU2 *, *GAMTAU3 *, *$/      DOUBLE PRECISION /I'   C2_DENSGAM.f

   sed -i -e '/^ *& *GAM5TAU *, *GAM5TAU2 *, *GAM5TAU3 *, *$/Id'                C2_DENS5.f
   sed -i -e '/^ *& *GAM6TAU *, *GAM6TAU2 *, *GAM6TAU3 *, *$/Id'                C2_DENS60.f
   sed -i -e '/^ *& *GAM1TAU *, *GAM1TAU2 *, *GAM1TAU3 *, *$/Id'                C2_DENS10.f
   sed -i -e '/^ *& *GAM2TAU *, *GAM2TAU2 *, *GAM2TAU3 *, *$/Id'                C2_DENS20.f
   sed -i -e '/^ *& *GAM7TAU *, *GAM7TAU2 *, *GAM7TAU3 *, *$/Id'                C2_DENS70.f
fi

# argument  ILL  not used in  C2_FHGAQ
if [ -f C2_FHGAQ.f ]; then
   sed -i -e 's/ *, *ILL *,/,/I'                C2_FHGAQ.f
   sed -i -e 's/ *ILL *,/          /I'          C2_FHGAQ.f
fi
 
# 2 arguments DLAGAPV, DUSLAGBPV not used in  C2_FHGAXA
if [ -f C2_FHGAXA.f ]; then
   sed -i -e 's/ *, *DLAGAPV *,/,/I'            C2_FHGAXA.f
   sed -i -e 's/ *, *DLAGAPV(NSCA) *,/,/I'      C2_FHGAXA.f
   sed -i -e 's/ *, *DUSLAGBPV *,/,/I'          C2_FHGAXA.f
   sed -i -e 's/ *, *DUSLAGBPV(NSCA) *$//I'     C2_FHGAXA.f
fi

# 2 arguments XVAP, RNC not used in  C2_FHGAX
if [ -f C2_FHGAX.f ]; then
   sed -i -e 's/ *, *XVAP *,/                ,/I'          C2_FHGAX.f
   sed -i -e 's/ *PRECISION *XVAP(NSCA) *,/ PRECISION /I'  C2_FHGAX.f
   sed -i -e 's/ *, *RNC *,/,/I'                           C2_FHGAX.f
   sed -i -e 's/ *, *RNC(NSCA) *,/,/I'                     C2_FHGAX.f
fi
  
# argument  P  not used in  C2_FHLIQADHO
if [ -f C2_FHLIQADHO.f ]; then
   sed -i -e 's/ *, *P *,/,/I'                          C2_FHLIQADHO.f
   sed -i -e 's/ *PRECISION *P(NSCA) *,/ PRECISION /I'  C2_FHLIQADHO.f
fi

# argument  P  not used in  C2_FHLIQA
if [ -f C2_FHLIQA.f ]; then
   sed -i -e 's/ *, *P *,/,/I'                          C2_FHLIQA.f
   sed -i -e 's/ *PRECISION *P(NSCA) *,/ PRECISION /I'  C2_FHLIQA.f
fi
  
# 5 arguments  P , HL , RL , DRL1 , DRL2  not used in  C2_FHLIQALBe
if [ -f C2_FHLIQALBe.f ]; then
   sed -i -e 's/ *, *P *, *HL *$//I'                                C2_FHLIQALBe.f
   sed -i -e '/ * P(NSCA) *, *HL(NSCA) *$/Id'                       C2_FHLIQALBe.f
   sed -i -e 's/1 *,TL(NSCA)/ DOUBLE PRECISION TL(NSCA)/I'          C2_FHLIQALBe.f
   sed -i -e '/ *, *RL *, *DRL1 *, *DRL2 *$/Id'                     C2_FHLIQALBe.f
   sed -i -e '/ *, *RL(NSCA) *, *DRL1(NSCA) *, *DRL2(NSCA) *$/Id'   C2_FHLIQALBe.f
fi
  
# arguments  P , HL  not used in  C2_FHLIQANa
if [ -f C2_FHLIQANa.f ]; then
   sed -i -e 's/ *, *P *, *HL *$//I'                                C2_FHLIQANa.f
   sed -i -e '/ * P(NSCA) *, *HL(NSCA) *$/Id'                       C2_FHLIQANa.f
   sed -i -e 's/1 *,TL(NSCA)/ DOUBLE PRECISION TL(NSCA)/I'          C2_FHLIQANa.f
fi
  
# 5 arguments  P , HL , RL , DRL1 , DRL2  not used in  C2_FHLIQAPb
if [ -f C2_FHLIQAPb.f ]; then
   sed -i -e 's/ *, *P *, *HL *$//I'                                C2_FHLIQAPb.f
   sed -i -e '/ * P(NSCA) *, *HL(NSCA) *$/Id'                       C2_FHLIQAPb.f
   sed -i -e 's/1 *,TL(NSCA)/ DOUBLE PRECISION TL(NSCA)/I'          C2_FHLIQAPb.f
   sed -i -e '/ *, *RL *, *DRL1 *, *DRL2 *$/Id'                     C2_FHLIQAPb.f
   sed -i -e '/ *, *RL(NSCA) *, *DRL1(NSCA) *, *DRL2(NSCA) *$/Id'   C2_FHLIQAPb.f
fi
  
# argument  K  not used in  C2_FHSOLV
if [ -f C2_FHSOLV.f ]; then
   sed -i -e 's/ *, *K *,/,/I'                                C2_FHSOLV.f
   sed -i -e 's/INTEGER IFLUID,NSCA,K/INTEGER IFLUID,NSCA/I'  C2_FHSOLV.f
fi
  
# 9 arguments  TSP,DTSP1, HLSP,DHLSP1, CPLSP,DCLSP1, RLSP,DRLSP1, IENC 
# not used in  C2_FHLIQDHO
if [ -f C2_FHLIQDHO.f ]; then
   sed -i -e '/ *,TSP,DTSP1 *$/Id'                                C2_FHLIQDHO.f
   sed -i -e '/ *,HLSP,DHLSP1 *$/Id'                              C2_FHLIQDHO.f
   sed -i -e '/ *,CPLSP,DCLSP1 *$/Id'                             C2_FHLIQDHO.f
   sed -i -e '/ *,RLSP,DRLSP1 *$/Id'                              C2_FHLIQDHO.f
   sed -i -e '/ *TSP(NSCA),DTSP1(NSCA) *$/Id'                     C2_FHLIQDHO.f
   sed -i -e '/ *HLSP(NSCA),DHLSP1(NSCA) *$/Id'                   C2_FHLIQDHO.f
   sed -i -e '/ *CPLSP(NSCA),DCLSP1(NSCA) *$/Id'                  C2_FHLIQDHO.f
   sed -i -e '/ *RLSP(NSCA),DRLSP1(NSCA) *$/Id'                   C2_FHLIQDHO.f
   sed -i -e 's/,IENC//I'                                         C2_FHLIQDHO.f
fi

# argument  PV  not used in  C2_FHVAPALBe
if [ -f C2_FHVAPALBe.f ]; then
   sed -i -e 's/ *, *PV *,/                   ,/I'            C2_FHVAPALBe.f
   sed -i -e 's/PRECISION PV(NSCA),/PRECISION /I'             C2_FHVAPALBe.f
fi
  
# argument  PV  not used in  C2_FHVAPANa
if [ -f C2_FHVAPANa.f ]; then
   sed -i -e 's/ *, *PV *,/,/I'                               C2_FHVAPANa.f
   sed -i -e 's/PRECISION PV(NSCA),/PRECISION /I'             C2_FHVAPANa.f
fi
  
# argument  PV  not used in  C2_FHVAPAPb
if [ -f C2_FHVAPAPb.f ]; then
   sed -i -e 's/ *, *PV *,/,/I'                               C2_FHVAPAPb.f
   sed -i -e 's/PRECISION PV(NSCA),/PRECISION /I'             C2_FHVAPAPb.f
fi
          
# argument  KS  not used in  C2_FHVAPRO
if [ -f C2_FHVAPAPb.f ]; then
   sed -i -e 's/SUBROUTINE C2_FHVAPRO (KS/SUBROUTINE C2_FHVAPRO (/I'  C2_FHVAPRO.f
   sed -i -e 's/\(& *\),\(TS,DTSDP\)/\1 \2/I'                         C2_FHVAPRO.f
   sed -i -e 's/INTEGER KS, LIMITE/INTEGER LIMITE/I'                  C2_FHVAPRO.f

   sed -i -e 's/CALL C2_FHVAPRO (I/CALL C2_FHVAPRO (/I'               C2_FHVAPTG.f
   sed -i -e 's/\(& *\),\(TSPV\)/\1 \2/I'                             C2_FHVAPTG.f
fi
  
# 4 arguments  CPV,DCPVPV,DCPVHV , TGK not used in  C2_FHVAPVDHO
if [ -f C2_FHVAPVDHO.f ]; then
   sed -i -e '/ *,CPV *, *DCPVPV *, *DCPVHV *$/Id'          C2_FHVAPVDHO.f
   sed -i -e 's/ *, *TGK *,/                  ,/I'          C2_FHVAPVDHO.f
   sed -i -e 's/PRECISION TGK,/PRECISION /I'                C2_FHVAPVDHO.f

   sed -i -e '/ *8 *,CPV(I), *DCPVPV(I), *DCPVHV(I) *$/Id'  C2_FHVAPPDHO.f
   sed -i -e 's/ *, *TGK(I) *, */                  ,/I'     C2_FHVAPPDHO.f
fi
  
# 4 arguments  CPV,DCPVPV,DCPVHV , TGK not used in  C2_FHVAPV
if [ -f C2_FHVAPV.f ]; then
   sed -i -e '/ *,CPV *, *DCPVPV *, *DCPVHV *$/Id'          C2_FHVAPV.f
   sed -i -e 's/ *, *TGK *,/                  ,/I'          C2_FHVAPV.f
   sed -i -e 's/PRECISION TGK,/PRECISION /I'                C2_FHVAPV.f

   sed -i -e '/ *8 *,CPV(I), *DCPVPV(I), *DCPVHV(I) *$/Id'  C2_FHVAPP.f
   sed -i -e 's/ *, *TGK(I) *, */                  ,/I'     C2_FHVAPP.f
fi


# variable STR120 not used
if [ -f C2_FTSATGXIAPWS.f ]; then
   sed -i -e 's/\(^ *CHARACTER.*120\)/*\1/I' C2_FTSATGXIAPWS.f  # ! OK retrait de la variable STR120, pas d'erreur possible
fi
if [ -f C2_EPSTGXIAPWS.f ]; then
   sed -i -e 's/\(^ *CHARACTER.*120\)/*\1/I' C2_EPSTGXIAPWS.f   # ! OK retrait de la variable STR120, pas d'erreur possible
fi
if [ -f C2_FHVAPTG.f ]; then
   sed -i -e 's/\(^ *CHARACTER.*120\)/*\1/I' C2_FHVAPTG.f       # ! NOK pas de retrait de la variable STR120, utile pour message d'erreur
fi
if [ -f C2_FTVAPNa.f ]; then
   sed -i -e 's/\(^ *CHARACTER.*120\)/*\1/I' C2_FTVAPNa.f       # ! NOK pas de retrait de la variable STR120, utile pour message d'erreur
fi

# null values
if [ -f C2_FHLIQ.f ]; then
   awk 'BEGIN{IGNORECASE=1} \
        {if ($0 ~ /END_*DOCUMENT/) {print ;\
         print "      D2RLDHLP=ZERO; D2TLDHLP=ZERO; D2RLDHL2=ZERO; D2TLDHL2=ZERO"} else print}' C2_FHLIQ.f >tmp$$ ; mv tmp$$ C2_FHLIQ.f
fi

rm filec$$ /tmp/liste$$ /tmp/erreur$$

exit 0
