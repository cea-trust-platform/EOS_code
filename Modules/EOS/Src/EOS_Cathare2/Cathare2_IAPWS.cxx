/****************************************************************************
* Copyright (c) 2023, CEA
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
* 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
* 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*****************************************************************************/



#include "Cathare2_Parameters.hxx"
#include "Cathare2_IAPWS.hxx"
#include "Cathare2_IAPWS.f77.hxx"

namespace CATHARE2 {
  
  CATHARE2_IAPWS::CATHARE2_IAPWS(EOS_Fluid* the_fluid, domain phase) : 
    CATHARE2(the_fluid, phase, CATHARE2_LFLH2Oiapws)
  {}
  
  //
  int CATHARE2_IAPWS::fhpfld(int& ill, int& ivstat, int& ivalu, domain mode) 
  { (void)ivalu ;
    int itest ;
    ivstat = 0 ;
    F77DECLARE(c2_ftsatpiapws)(nsca, lp[0],
                            ltsp[0], ltsp1[0], l2tsdpvv[0],
                            lhlsp[0], lhlsp1[0],
                            lhvsp[0], lhvsp1[0],
                            lcplsp[0], lclsp1[0],
                            lcpvsp[0], lcvsp1[0],
                            lrlsp[0], lrlsp1[0],
                            lrvsp[0], lrvsp1[0],
                            7777) ;

    if ( (mode == liquid) || (mode == unknown) ) 
       { F77DECLARE(c2_fhliqiapws)(nsca, lp[0], lhl[0],
                                ltl[0], ltl1[0], ltl2[0],
                                lrl[0], lrl1[0], lrl2[0],
                                lcpl[0], lcpl1[0], lcpl2[0],
                                lbetal[0], lbetal1[0], lbetal2[0],
                                ill, ivstat, 7777) ;
         for (int i=1; i<=nsca; i++) 
             { if (ltl[i-1] > xtlp || ltl[i-1] < xtlm) 
                  { ill = i ;
                    ivstat = CATHARE2_ILLTL ;
                    return 0 ;
                  }
             }
         if (ivstat != 0) return 0 ;
         F77DECLARE(c2_epstliapws)(nsca, lp[0], ltsp[0], ltsp1[0],
                                lcpl[0], lcpl1[0], lcpl2[0],
                                epsl,
                                lepstliq[0], lepstliq1[0], lepstliq2[0],
                                lhlspsc[0], lhlspsc1[0], lhlspsc2[0],
                                7777) ;
       }

    if ( (mode == vapor) || (mode == unknown) ) 
       { F77DECLARE(c2_usetvd)(nsca, lxvap[0], un)   ;
         F77DECLARE(c2_usetvd)(nsca, lxnc[0],  zero) ;
         F77DECLARE(c2_usetvd)(nsca, lrnc[0],  zero) ;
         F77DECLARE(c2_usetvd)(nsca, lmnc[0],  zero) ;

         // call to liquid properties ...
         F77DECLARE(c2_fhliqiapws)(nsca, lp[0], lhl[0],
                                ltl[0], ltl1[0], ltl2[0],
                                lrl[0], lrl1[0], lrl2[0],
                                lcpl[0], lcpl1[0], lcpl2[0],
                                lbetal[0], lbetal1[0], lbetal2[0],
                                ill, ivstat, 7777) ;
         if (ivstat != 0) return 0 ;
         F77DECLARE(c2_epstliapws)(nsca, lp[0], ltsp[0], ltsp1[0],
                                lcpl[0], lcpl1[0], lcpl2[0],
                                epsl,
                                lepstliq[0], lepstliq1[0], lepstliq2[0],
                                lhlspsc[0], lhlspsc1[0], lhlspsc2[0],
                                7777) ;

         if (nincon == 0) 
            { // only fluid vapor
              for (int i=1; i<=nsca; i++)  lprgr[i-1] = fldr ;
              F77DECLARE(c2_fhvapiapws)(nsca, lp[0], lhg[0], lhvsp[0],
                                     ltg[0], ltgpv[0], ltghv[0],
                                     lcpv[0], lcpvpv[0], lcpvhv[0],
                                     lrv[0], lrvpv[0], lrvhv[0],
                                     ltgk[0], lhvmhs[0],
                                     ill, ivstat, 7777) ;
              if (ivstat != 0) return 0 ;
              for (int i=1; i<=nsca; i++) 
                 { if (ltg[i-1]>xtgp||ltg[i-1]<xtgm) 
                      { ill = i ;
                        ivstat = CATHARE2_ILLTG ;
                        return 0 ;
                      }
                 }
              for (int i=1; i<=nsca; i++) 
                 { if (lrv[i-1] < zero) 
                      { ill = i;
                        ivstat = CATHARE2_ILLRG ;
                        return 0 ;
                      }
                 }
              
              F77DECLARE(c2_usetvd)(nsca, lhlsvsc3[0], zero) ;
            }

         else
            { // incondensable gas
              F77DECLARE(c2_usetvd)(nsca, lxrsu[0],    zero)  ;
              F77DECLARE(c2_usetvd)(nsca, lxcpsu[0],   zero)  ;
              F77DECLARE(c2_usetvd)(nsca, lxcpsutg[0], zero)  ;
              F77DECLARE(c2_usetvd)(nsca, lhi7su[0],   zero)  ;
              F77DECLARE(c2_usetvd)(nsca, lvalp[0],    epspp) ;
              for (int j=1; j<=nincon; j++) 
                 { F77DECLARE(c2_fhxcp)(lfluid, nsca, ltgini[0], lprxcp0[j-1],
                                     lprxcp1[j-1], lprxcp2[j-1], lprxcp3[j-1],
                                     lprxcp4[j-1], lprxcp5[j-1], lprxcp6[j-1],
                                     lprxcp[j-1][0], lprxcptg[j-1][0]) ;
                   F77DECLARE(c2_fhxsum)(lfluid, nsca, 1, lx[j-1][0],
                                      lprxr[j-1], lprxcp[j-1][0], lprxcptg[j-1][0], lprxm[j-1],
                                      lxnc[0], lmnc[0], lxrsu[0], lxcpsu[0], lxcpsutg[0],
                                      lhi7su[0], lvalp[0]) ;
                 }
              F77DECLARE(c2_fhinitiapws)(nsca, fldr, lp[0], lhg[0], 
                                      lxnc[0], lxrsu[0], lxcpsu[0], lhi7su[0],
                                      lpv[0], lhv[0],
                                      lxvap[0], lrnc[0], lmnc[0], lprgr[0], lvalp[0], 7777) ;
              itest = 1 ;
              for (int k=1; k<=50; k++) 
                  { F77DECLARE(c2_fhvapiapws)(nsca, lpv[0], lhv[0], lhvsv[0],
                                           ltg[0], ltgpv[0], ltghv[0],
                                           lcpv[0], lcpvpv[0], lcpvhv[0],
                                           lrv[0], lrvpv[0], lrvhv[0], 
                                           ltgk[0], lhvmhs[0],
                                           ill, ivstat, 7777) ;
                    if (ivstat != 0) return 0 ;
                    F77DECLARE(c2_usetvd)(nsca, lxcpsu[0], zero)   ;
                    F77DECLARE(c2_usetvd)(nsca, lxcpsutg[0], zero) ;
                    F77DECLARE(c2_usetvd)(nsca, lhi7su[0], zero)   ;
                    for (int j=1; j<=nincon; j++) 
                        { F77DECLARE(c2_fhxcp)(lfluid, nsca, ltg[0], lprxcp0[j-1],
                                            lprxcp1[j-1], lprxcp2[j-1], lprxcp3[j-1],
                                            lprxcp4[j-1], lprxcp5[j-1], lprxcp6[j-1],
                                            lprxcp[j-1][0], lprxcptg[j-1][0]) ;
                          F77DECLARE(c2_fhxsum)(lfluid, nsca, 0, lx[j-1][0],
                                             lprxr[j-1], lprxcp[j-1][0], lprxcptg[j-1][0], lprxm[j-1],
                                             lxnc[0], lmnc[0], lxrsu[0], lxcpsu[0], lxcpsutg[0],
                                             lhi7su[0], lvalp[0]) ;
                        }
                    F77DECLARE(c2_fhsolv)(lfluid, nsca, tref,
                                       lp[0], lhg[0], 
                                       ltg[0], ltgpv[0], ltghv[0], ltgk[0],
                                       lrv[0], lrvpv[0], lrvhv[0],
                                       lxvap[0], lxrsu[0], lxcpsu[0], lxcpsutg[0],
                                       lhi7su[0], lvalp[0],
                                       itest, lpv[0], lhv[0]) ;
                    if ( (itest == 0) && (k >= litermin) ) 
                       break ;
                    else if (k == 50) 
                       { ill = itest ;
                         ivstat = CATHARE2_ILLCVR ;
                         return 0 ;
                       }
                    itest = 0 ;
                  }

              F77DECLARE(c2_ftsatpiapws)(nsca, lpv[0],
                                     ltspv[0], ltspvv[0], l2tsdpvv[0],
                                     lhlsv[0], lhlsvv[0],
                                     lhvsv[0], lhvsvv[0],
                                     lcplsv[0], lclsvv[0],
                                     lcpvsv[0], lcvsvv[0], 
                                     lrlsv[0], lrlsvv[0],
                                     lrvsv[0], lrvsvv[0], 
                                     7777) ;
              for (int i=1; i<=nsca; i++) 
                  { if ( lhv[i-1] > xhvp || lhv[i-1 ]< xhvm ) 
                       { ill = i ;
                         ivstat = CATHARE2_ILLHV ;
                         return 0 ;
                       }
                  }
              F77DECLARE(c2_fhvapiapws)(nsca, lpv[0], lhv[0], lhvsv[0],
                                     ltg[0], ltgpv[0], ltghv[0],
                                     lcpv[0], lcpvpv[0], lcpvhv[0],
                                     lrv[0], lrvpv[0], lrvhv[0], 
                                     ltgk[0], lhvmhs[0],
                                     ill, ivstat, 7777) ;
              if (ivstat != 0) return 0 ;
              F77DECLARE(c2_usetvd)(nsca, lxcpsu[0], zero)   ;
              F77DECLARE(c2_usetvd)(nsca, lxcpsutg[0], zero) ;
              F77DECLARE(c2_usetvd)(nsca, lhi7su[0], zero)   ;
              for (int j=1; j<=nincon; j++) 
                  { F77DECLARE(c2_fhxcp)(lfluid, nsca, ltg[0], lprxcp0[j-1], 
                                      lprxcp1[j-1], lprxcp2[j-1], lprxcp3[j-1],
                                      lprxcp4[j-1], lprxcp5[j-1], lprxcp6[j-1],
                                      lprxcp[j-1][0], lprxcptg[j-1][0]) ;
                    F77DECLARE(c2_fhxsum)(lfluid, nsca, 0, lx[j-1][0],
                                       lprxr[j-1], lprxcp[j-1][0], lprxcptg[j-1][0], lprxm[j-1],
                                       lxnc[0], lmnc[0], lxrsu[0], lxcpsu[0], lxcpsutg[0],
                                       lhi7su[0], lvalp[0]) ;
                  }
              for (int i=1; i<=nsca; i++) 
                  { if (ltg[i-1]>xtgp||ltg[i-1]<xtgm) 
                       { ill = i ;
                         ivstat = CATHARE2_ILLTG ;
                         return 0 ;
                       }
                  }
              for (int i=1; i<=nsca; i++) 
                  { if (lrv[i-1] < zero) 
                       { ill = i ;
                         ivstat = CATHARE2_ILLRG ;
                         return 0 ;
                       }
                  }
              F77DECLARE(c2_fhgas)(nsca, fldr, tref,
                                lp[0], lpv[0],
                                ltg[0], ltgpv[0], ltghv[0], ltgk[0],
                                lrv[0], lrvpv[0], lrvhv[0],
                                lxvap[0], lxrsu[0], lxcpsu[0], lxcpsutg[0], lprgr[0],
                                lpv1[0], lpv3[0], lhv1[0], lhv3[0],
                                ltg1[0], ltg3[0], lrv1[0], lrv3[0],
                                lrg[0], lrg1[0], lrg3[0],
                                lfdpv[0], lfdhv[0], lgdpv[0], lgdhv[0]) ;
              for (int j=1; j<=nincon; j++) 
                  { F77DECLARE(c2_fhgax)(nsca, fldr, href, trefk,
                                      lp[0], lpv[0], lhv[0], lx[j-1][0],
                                      ltgpv[0], ltghv[0], ltgk[0],
                                      lrv[0], lrvpv[0], lrvhv[0], lrg[0], 
                                      lxnc[0], lmnc[0],
                                      lprxr[j-1], lprxcp[j-1][0], lprxm[j-1], lprgr[0], 
                                      lfdpv[0], lfdhv[0], lgdpv[0], lgdhv[0],
                                      lpvx[j-1][0], lhvx[j-1][0], 
                                      ltgx[j-1][0], lrvx[j-1][0], lrgx[j-1][0], 
                                      lhx[j-1][0], lrncx[j-1][0], lmncx[j-1][0], lpx[j-1][0]) ;
                  }
              F77DECLARE(c2_ftsatliapws)(nsca, lpv[0], lpv1[0], lpv3[0],
                                      ltspv[0], ltspvv[0], 
                                      lepstliq[0], lepstliq1[0], lepstliq2[0], 
                                      lhlsvsc[0], lhlsvsc1[0], lhlsvsc2[0], lhlsvsc3[0],
                                      lhlsvscv[0], 7777) ;
              for (int j=1; j<=nincon; j++)
                  F77DECLARE(c2_umultvd)(nsca, lhlsvscv[0], lpvx[j-1][0], lhlsvscx[j-1][0]);
            } // end ... incondensable gas
       } // end ... fluid vapor

    return 0 ;
  }
 
//
  int CATHARE2_IAPWS::ftrafld(int& ill, int& ivstat, domain mode) 
     { ill = 0;
       ivstat = 0;
       if ( (mode == liquid) || (mode == unknown) ) 
          { F77DECLARE(c2_fliqaiapws)(nsca, lp[0],
                                   ltl[0], ltl1[0], ltl2[0], 
                                   ltlal[0], ltlal1[0], ltlal2[0],
                                   ltmul[0], ltmul1[0], ltmul2[0],
                                   ill, ivstat, 9999) ;
            if (ivstat != 0)  return 0 ;
          }

       if ( (mode == vapor) || (mode == unknown) ) 
          { F77DECLARE(c2_fvapaiapws)(nsca,
                                   lpv[0], ltg[0], 
                                   ltspv[0], ltspvv[0], 
                                   ltlav[0], llavpv[0], llavtg[0],
                                   ltmuv[0], lmuvpv[0], lmuvtg[0], 
                                   lsi[0], lsipv[0],
                                   ill, ivstat, 9999) ;
            if (ivstat != 0)  return 0 ;

            if (nincon == 0)
               { F77DECLARE(c2_fhbasa)(nsca, 
                                    ltg1[0], ltg3[0],
                                    llavpv[0], llavtg[0], lmuvpv[0], lmuvtg[0], 
                                    ltlag1[0], ltlag3[0], ltmug1[0], ltmug3[0]) ;
                 F77DECLARE(c2_usetvd)(nsca, ldncv[0],  zero) ;
                 F77DECLARE(c2_usetvd)(nsca, ldncv1[0], zero) ;
                 F77DECLARE(c2_usetvd)(nsca, ldncv3[0], zero) ;
               }
            else 
               { // incondensable gas
                 for (int j=1; j<=nincon; j++) 
                    { F77DECLARE(c2_fhxinit)(nsca, ltgk[0], lprxl0[j-1], lprxl1[j-1], lprxl2[j-1], 
                                          lprxm0[j-1], lprxm1[j-1], lprxm2[j-1],
                                          ltlax[j-1][0], llaxtg[j-1][0], ltmux[j-1][0], lmuxtg[j-1][0]) ;
                    }
                 if (llatypml == CATHARE2_LAVERAGE) 
                    { if (nincon == 4) 
                         F77DECLARE(c2_fhgaq)(nincon, nsca, 
                                           fldr, luserq, lcoefqv[0], lcoefq,
                                           lcoefqvx[0][0], lcoefqvx[1][0], lcoefqvx[2][0], 
                                           lcoefqvx[3][0], lzero[0], lx[0][0], lx[1][0],
                                           lx[2][0], lx[3][0], lprxr[0], lprxr[1],
                                           lprxr[2], lprxr[3], lprgr[0], ivstat) ;
                      else if (nincon == 3)  
                         F77DECLARE(c2_fhgaq)(nincon, nsca,
                                           fldr, luserq, lcoefqv[0], lcoefq,
                                           lcoefqvx[0][0], lcoefqvx[1][0], lcoefqvx[2][0],
                                           lbidon[0], lzero[0], lx[0][0], lx[1][0], 
                                           lx[2][0], lzero[0], lprxr[0], lprxr[1],
                                           lprxr[2], lzero[0], lprgr[0], ivstat) ;
                      else if (nincon == 2)  
                         F77DECLARE(c2_fhgaq)(nincon, nsca,
                                           fldr, luserq, lcoefqv[0], lcoefq,
                                           lcoefqvx[0][0], lcoefqvx[1][0], lbidon[0], 
                                           lbidon[0], lzero[0], lx[0][0], lx[1][0],
                                           lzero[0], lzero[0], lprxr[0], lprxr[1],
                                           lzero[0], lzero[0], lprgr[0], ivstat) ;
                      else if (nincon == 1)
                         F77DECLARE(c2_fhgaq)(nincon, nsca,
                                           fldr, luserq, lcoefqv[0], lcoefq,
                                           lcoefqvx[0][0], lbidon[0], lbidon[0],
                                           lbidon[0], lzero[0], lx[0][0], lzero[0],
                                           lzero[0], lzero[0], lprxr[0], lzero[0],
                                           lzero[0], lzero[0], lprgr[0], ivstat) ;
                      if (ivstat != 0) return 0 ;
                    }
                 if (lmutypml == CATHARE2_LWILKE || llatypml == CATHARE2_LMASSAX)
                    F77DECLARE(c2_fhgaphi)(nincon, nsca, fldr, fldm,
                                        lxvap[0], lzero[0], 
                                        lprxri[0], lprxmi[0], 
                                        lprgr[0], ltmuv[0], 
                                        lmuvpv[0], lmuvtg[0],
                                        lxi[0], ltmuxi[0], lmuxtgi[0],
                                        lphivmusutg[0], lphivmusupv[0],
                                        lphivmusu[0], lphivmusuxi[0]) ;
                 F77DECLARE(c2_fhgasa)(llatypml, lmutypml, 
                                    nsca, fldr, lxvap[0], 
                                    lcpv[0], ltlav[0], ltmuv[0], 
                                    llavtg[0], lmuvtg[0],
                                    llavpv[0], lmuvpv[0],
                                    lphivmusu[0], lphivmusupv[0], lphivmusutg[0],
                                    lcpg[0], ltlag[0], ltmug[0],
                                    lcpgtg[0], llagtg[0], lmugtg[0], 
                                    llagpv[0], lmugpv[0], 
                                    ltlaga[0], llagapv[0], llagatg[0],
                                    lustlagb[0], luslagbpv[0], luslagbtg[0]) ;
                 F77DECLARE(c2_usetvd)(nsca, ldncv[0], zero) ;
                 if (lmutypml == CATHARE2_LWILKE || llatypml == CATHARE2_LMASSAX)
                    F77DECLARE(c2_fhgaxphi)(nincon, nsca,
                                         fldr, fldm, 
                                         lxi[0], lxvap[0], 
                                         ltmuxi[0], lmuxtgi[0],
                                         lprxri[0], lprxmi[0],
                                         ltmuv[0], lmuvpv[0], lmuvtg[0],
                                         lprgr[0], lzero[0],
                                         lphixmusuxv[0], lphixmusui[0],
                                         lphixmusupvi[0], lphixmusutgi[0], 
                                         lphixmusuxii[0]) ;
                 for (int j=1; j<=nincon; j++) 
                    { F77DECLARE(c2_fhxcp)(lfluid, nsca, ltg[0], lprxcp0[j-1],
                                       lprxcp1[j-1], lprxcp2[j-1], lprxcp3[j-1],
                                       lprxcp4[j-1], lprxcp5[j-1], lprxcp6[j-1],
                                       lprxcp[j-1][0], lprxcptg[j-1][0]) ;
                      F77DECLARE(c2_fhgaxa)(llatypml, lmutypml,
                                         nsca, fldm, flddv,
                                         lp[0], ltgk[0], lx[j-1][0],
                                         lprxr[j-1], lprxcp[j-1][0], lprxcptg[j-1][0], lprxm[j-1], lprxdv[j-1],
                                         ltlax[j-1][0], llaxtg[j-1][0], 
                                         ltmux[j-1][0], lmuxtg[j-1][0],
                                         lphixmusu[j-1][0], lphixmusupv[j-1][0], lphixmusutg[j-1][0],
                                         lcpg[0], ltlag[0], ltmug[0],
                                         lcpgtg[0], llagtg[0], lmugtg[0], 
                                         llagpv[0], lmugpv[0],
                                         ltlaga[0], llagatg[0],
                                         lustlagb[0], luslagbtg[0],
                                         ldncvj[j-1][0], ldncv[0]) ;
                    }
                 F77DECLARE(c2_fhdiga)(llatypml, lmutypml, 
                                    nsca,
                                    lpv1[0], lpv3[0], lhv1[0], lhv3[0],
                                    lp[0], ltgk[0], ltg1[0], ltg3[0],
                                    lxnc[0], lxvap[0], lprgr[0],
                                    lcpvpv[0], lcpvhv[0], lsipv[0], 
                                    llagpv[0], llagtg[0], lmugpv[0], lmugtg[0], 
                                    ltlaga[0], llagapv[0], llagatg[0], 
                                    lustlagb[0], luslagbpv[0], luslagbtg[0],
                                    lcoefqv[0],
                                    lcpgtg[0], lcpg1[0], lcpg3[0],
                                    ltlag[0], ltlag1[0], ltlag3[0],
                                    ltmug[0], ltmug1[0], ltmug3[0],
                                    lsi1[0], lsi3[0], 
                                    ldncv[0], ldncv1[0], ldncv3[0]) ;
                 for (int j=1; j<=nincon; j++) 
                    { F77DECLARE(c2_fhdixa)(llatypml, lmutypml,
                                         nsca, fldr, 
                                         ltgk[0], lx[j-1][0],
                                         lpvx[j-1][0], lhvx[j-1][0], ltgx[j-1][0],
                                         lcpv[0], lcpvpv[0], lcpvhv[0],
                                         ltlav[0], llagtg[0],
                                         ltmuv[0], lmugtg[0],
                                         llagpv[0], lmugpv[0],
                                         ltlaga[0], llagapv[0], llagatg[0],
                                         lustlagb[0], luslagbpv[0], luslagbtg[0],
                                         lcoefqv[0], lcoefqvx[j-1][0], 
                                         lsipv[0], lxnc[0], lxvap[0], 
                                         ldncvj[j-1][0], ldncv[0], lprgr[0],
                                         lprxr[j-1], lprxcp[j-1][0], lcpgtg[0], 
                                         ltlax[j-1][0],
                                         ltmux[j-1][0],
                                         lphivmusu[0], lphivmusux[j-1][0],
                                         lphixmusu[j-1][0], lphixmusux[j-1][j-1][0],
                                         lcpgx[j-1][0], 
                                         ltlag[0], ltlagx[j-1][0], 
                                         ltmug[0], ltmugx[j-1][0], 
                                         lsix[j-1][0], ldncvx[j-1][0]) ;
                      for (int k=1; k<=nincon; k++) 
                         { if (k != j) 
                              F77DECLARE(c2_fhdixxa)(llatypml, lmutypml,
                                                  nsca, 
                                                  lx[k-1][0], lprxr[k-1], lprgr[0], 
                                                  ltlax[k-1][0], ltmux[k-1][0], 
                                                  lphixmusu[k-1][0], lphixmusux[k-1][j-1][0],
                                                  ltlagx[j-1][0], ltmugx[j-1][0]) ;
                         }
                    }
               } // end ... incondensable gaz

            if (liter == 1) F77DECLARE(c2_fhcomp)(lfluid, nsca, lp[0], ltmug[0], lcpg[0], ltlag[0], lprgr[0],
                                               lkiseng[0], lprandg[0]) ;
            F77DECLARE(c2_epstgiapws)(nsca, lp[0], ltsp[0], ltsp1[0],
                                  lcpg[0], lcpg1[0], lcpg3[0], 
                                  epsg, 
                                  lepstgas[0], lepstgas1[0], lepstgas3[0],
                                  lhvspsc[0], lhvspsc1[0], lhvspsc3[0], 
                                  lhvspsctg[0], 9999) ;
            if (nincon != 0) 
               { for (int j=1; j<=nincon; j++) 
                    F77DECLARE(c2_epstgxiapws)(nsca, lp[0], 
                                            lcpg[0], lcpgx[j-1][0], lhvspsctg[0],
                                            epsg, 
                                            lepstgasx[j-1][0], lhvspscx[j-1][0]) ;
                 F77DECLARE(c2_ftsatgiapws)(nsca, lpv[0], lpv1[0], lpv3[0],
                                         ltspv[0], ltspvv[0], 
                                         lepstgas[0], lepstgas1[0], lepstgas3[0],
                                         lhvsvsc[0], lhvsvsc1[0], lhvsvsc3[0], 
                                         lhvsvscpv[0], lhvsvsctg[0], 9999) ;
                 for (int j=1; j<=nincon; j++)
                    F77DECLARE(c2_ftsatgxiapws)(nsca, 
                                             ltspvv[0], lpvx[j-1][0], lepstgasx[j-1][0], 
                                             lhvsvscpv[0], lhvsvsctg[0], 
                                             lhvsvscx[j-1][0]) ;
               }
          }

       return 0 ;
     }
  
  //
  void CATHARE2_IAPWS::fpsattfld() 
  { for (int ill=0; ill<nsca; ill++)
       F77DECLARE(c2_fpsattiapws)(ltsp[ill],
                               lp[ill], ltsp1[ill], 
                               lhlsp[ill], lhlsp1[ill], lhvsp[ill], lhvsp1[ill], 
                               lcplsp[ill], lclsp1[ill], lcpvsp[ill], lcvsp1[ill], 
                               lrlsp[ill], lrlsp1[ill], lrvsp[ill], lrvsp1[ill], 0) ;
   }
  
  //
  void CATHARE2_IAPWS::ftsatpfld() 
  { F77DECLARE(c2_ftsatpiapws)(nsca, lp[0], 
                            ltsp[0], ltsp1[0], l2tsdpvv[0],
                            lhlsp[0], lhlsp1[0], 
                            lhvsp[0], lhvsp1[0], 
                            lcplsp[0], lclsp1[0], 
                            lcpvsp[0], lcvsp1[0],
                            lrlsp[0], lrlsp1[0],
                            lrvsp[0], lrvsp1[0], 
                            0) ;
  }
  
  //
  void CATHARE2_IAPWS::ftliqfld() 
  { int ill, ivstat ;
    F77DECLARE(c2_ftliqiapws)(nsca, lp[0], ltl[0],
                           lhlpt[0],  lhl1pt[0],  lhl2pt[0], 
                           lcplpt[0], lcpl1pt[0], lcpl2pt[0], 
                           lrlpt[0],  lrl1pt[0],  lrl2pt[0],
                           ill, ivstat, 0) ;
  }
  
  //
  void CATHARE2_IAPWS::ftvapfld() 
  { int ill, ivstat ;
    F77DECLARE(c2_ftvapiapws)(nsca, lp[0], ltg[0],
                           lhvpt[0], lhv1pt[0],
                           lcpvpt[0], lcpvpvpt[0], lcpgtgpt[0],
                           lrvpt[0], lrv1pt[0], lrv3pt[0],
                           lhvsp[0], lhvmhs[0],
                           ill, ivstat, 0) ;
   }
  
}
