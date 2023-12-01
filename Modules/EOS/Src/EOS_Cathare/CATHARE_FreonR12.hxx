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



#ifndef CATHARE_FreonR12_HXX
#define CATHARE_FreonR12_HXX

#include <cmath>
#include <iostream>

#include "system/arch.h"
#include "EOS/API/EOS_Field.hxx"
#include "EOS/API/EOS_Fields.hxx"
#include "EOS/API/EOS_Error_Field.hxx"
#include "EOS/API/EOS_Error.hxx"
#include "EOS/API/EOS_properties.hxx"
#include "EOS/API/EOS_Fluid.hxx"

using namespace NEPTUNE ;

extern "C"
{
  // -- fpsatr12
  //    input(nsca,P)   output(Tsat,dTsatdp,iel,ivstat,terrc)
  void F77DECLARE(ca_fpsatr12)(int& nsca, double& tp, double& ttsp, double& ttsp1,
                            int& iel, int& ivstat, int& terrc) ;

  // -- frovlr12
  //    all : input(nsca,P,Tg  ,Tl  ,dTldp  )   output(rhov   ,rhol   ,drholdp   ,drhovdp   ,iel,ivstat,terrc)  unused(dTgdp)
  //    sat : input(nsca,P,Tsat,Tsat,dTsatdp)   output(rhovsat,rholsat,drholsatdp,drhovsatdp,iel,ivstat,terrc)  unused(dTsatdp)
  void F77DECLARE(ca_frovlr12)(int& nsca, double& tp, double& ttg, double& ttl, 
                            double& trg, double& trl,
                            double& ttl1, double& trl1, double& trg1,
                            int& iel, int& ivstat, int& terrc) ;

  // -- fhsatr12
  //    input(nsca,Tsat,rhovsat,rholsat,P,dTsatdp,drhovsatdp,drholsatdp)   output(hvsat,hlsat,dhvsatdp,dhlsatdp)
  void F77DECLARE(ca_fhsatr12)(int& nsca, double& ttsp, double& trvsp, double& trlsp, double& tp, 
                            double& thvsp, double& thlsp,
                            double& ttsp1, double& trvsp1, double& trlsp1, double& thvsp1, double& thlsp1) ;

  // -- fsigmar12
  //    input(nsca,Tsat,dTsatdp)   output(sigma,dsigmadp)
  void F77DECLARE(ca_fsigmar12)(int& nsca, double& ttsp, double& ttsp1,
                            double& tsi, double& tsipv) ;

  // -- ftgr12
  //    input(nsca,P,hv,Tsat,rhovsat)   output(Tg,dTgdp,dTgdh,rhog,drhogdp,drhogdh,iel,ivstat,terrc)
  void F77DECLARE(ca_ftgr12)(int& nsca, double& tp, double& thg, double& ttsp, double& trvsp1,
                          double& ttg, double& ttgpv, double& ttghv, 
                          double& trv, double& trvpv, double& trvhv, 
                          int& iel, int& ivstat, int& terrc) ;

  // -- fpthgr12
  //    input(nsca,P,Tg)   output(hg,iel,ivstat,terrc)
  void F77DECLARE(ca_fpthgr12)(int& nsca, double& tp, double& ttg, double& thg,
                            int& iel, int& ivstat, int& terrc) ;

  // -- fcpvr12
  //    input(nsca,Tg,dTgdp,dTgdh,rhog,drhogdp,drhogdh)   output(Cpv,dCpvdp,dCpvdh)
  void F77DECLARE(ca_fcpvr12)(int& nsca, double& ttg, double& ttgpv, double& ttghv, 
                           double& trv, double& trvpv, double& trvhv,
                           double& tcpv, double& tcpvpv, double& tcpvhv) ;

  // -- fmuvr12
  //    input(nsca,P,Tg,dTgdp,dTgdh)   output(mug,dmugdp,dmugdh)
  void F77DECLARE(ca_fmuvr12)(int& nsca, double& tp, double& ttg, double& ttgpv, double& ttghv, 
                           double& ttmuv, double& tmuvpv, double& tmuvhv) ;

  // -- fconvr12
  //    input(nsca,Tg,dTgdp,dTgdh)   output(lag,dlagdp,dlagdh)
  void F77DECLARE(ca_fconvr12)(int& nsca, double& ttg, double& ttgpv, double& ttghv, 
                            double& ttlav, double& tlavpv, double& tlavhv) ;

  // -- fpthlr12
  //    input(nsca,P,Tl)   output(hl,iel,ivstat,terrc)
  void F77DECLARE(ca_fpthlr12)(int& nsca, double& ttl, double& thl, 
                            int& iel, int& ivstat, int& terrc) ;

  // -- ftlr12
  //    input(nsca,P,Tsat,rhovsat,hvsat,hl)   output(Tl,rhol,iel,ivstat,terrc)
  void F77DECLARE(ca_ftlr12)(int& nsca, double& tp, double& ttsp, double& trvsp, double& thvsp, double& thl,
                          double& ttl, double& trl,
                          int& iel, int& ivstat, int& terrc) ;

  // -- fcplr12
  //    input(nsca,Tl)   output(Cpl,dTldp,dTldh,drholdp,drholdh,dcpldp,dcpldh,iel,ivstat,terrc)
  void F77DECLARE(ca_fcplr12)(int& nsca, double& ttl, 
                           double& tcpl, double& ttl1, double& ttl2,
                           double& trl1, double& trl2, double& tcpl1, double& tcpl2, 
                           int& iel, int& ivstat, int& terrc) ;

  // -- fmulr12
  //    input(nsca,Tl,dTldp,dTldh)    output(mul,dmuldp,dmuldh)
  void F77DECLARE(ca_fmulr12)(int& nsca, double& ttl, double& ttl1, double& ttl2, 
                           double& ttmul, double& ttmul1, double& ttmul2 ) ;

  // -- fconlr12
  //    input(nsca,P,Tl,dTldp,dTldh)    output(lal,dlaldp,dlaldh)
  void F77DECLARE(ca_fconlr12)(int& nsca, double& ttl, double& ttl1, double& ttl2, 
                            double& ttlal, double& ttlal1, double& ttlal2 ) ;
}



namespace NEPTUNE
{
  class EOS_Fluid;
};
namespace NEPTUNE_EOS
{ class EOS_Cathare_FreonR12 ;
  class EOS_Cathare_FreonR12Vapor ;
  class EOS_Cathare_FreonR12Liquid ;
};

namespace CATHARE_FREONR12
{
  //! Encapsulation of CATHARE-like functions for FreonR12.
  class CATHARE_FreonR12
  {
  public :
    friend class NEPTUNE_EOS::EOS_Cathare_FreonR12;
    friend class NEPTUNE_EOS::EOS_Cathare_FreonR12Liquid;
    friend class NEPTUNE_EOS::EOS_Cathare_FreonR12Vapor;

    // Limits min liquid and vapor
    double gl_Pmin() const ;
    double gl_hmin() const ;
    double gl_Tmin() const ;
    double gv_Pmin() const ;
    double gv_hmin() const ;
    double gv_Tmin() const ;

    // Limits max liquid and vapor
    double gl_Pmax() const ;
    double gl_hmax() const ;
    double gl_Tmax() const ;
    double gv_Pmax() const ;
    double gv_hmax() const ;
    double gv_Tmax() const ;

  private :
    CATHARE_FreonR12(NEPTUNE::EOS_Fluid* );
    CATHARE_FreonR12() ;
    const double tabsk;
    const double xpmr12,xppr12;
    const double xtlmr12,xtlpr12,xtgmr12,xtgpr12;
    const double xhlmr12,xhlpr12,xhvmr12,xhvpr12;

    EOS_Error calca_all_saturation(const NEPTUNE::EOS_Field& P, 
                                     EOS_Fields& r,
                                     EOS_Error_Field& err) const;
    EOS_Error calca_all_liquid(const EOS_Fields& input, 
                                 EOS_Fields& r,
                                 EOS_Error_Field& err) const ; 
    EOS_Error calca_all_vapor(const EOS_Fields& input, 
                                EOS_Fields& r,
                                EOS_Error_Field& err) const ;
    EOS_Error verify(const EOS_Fields& input, 
                     EOS_Error_Field& err,
                     int dom) const ;
    EOS_Error verify(const EOS_Field& input, 
                     EOS_Error_Field& err,
                     int dom) const ;
    //! Convert EOS_Cathare_FreonR12 partial error code to EOS internal code
    EOS_Internal_Error convert_eos_error(const int partial_code) const ;
    //! Convert ArrOfInt of EOS_Cathare_FreonR12 partial error code to EOS_Error_Field
    void convert_eos_error(const ArrOfInt& terrc, EOS_Error_Field& err) const ;
    NEPTUNE::EOS_Fluid* the_fluid ;
  };

  //
  inline double CATHARE_FreonR12::gl_Pmin() const
  { return xpmr12 ;
  }
  inline double CATHARE_FreonR12::gl_hmin() const
  { return xhlmr12 ;
  }
  inline double CATHARE_FreonR12::gl_Tmin() const
  { return xtlmr12 + tabsk ;
  }

  //
  inline double CATHARE_FreonR12::gv_Pmin() const
  { return xpmr12 ;
  }
  inline double CATHARE_FreonR12::gv_hmin() const
  { return xhvmr12 ;
  }
  inline double CATHARE_FreonR12::gv_Tmin() const
  { return xtgmr12 + tabsk ;
  }

  //
  inline double CATHARE_FreonR12::gl_Pmax() const
  { return xppr12 ;
  }
  inline double CATHARE_FreonR12::gl_hmax() const
  { return xhlpr12 ;
  }
  inline double CATHARE_FreonR12::gl_Tmax() const
  { return xtlpr12 + tabsk ;
  }

  //
  inline double CATHARE_FreonR12::gv_Pmax() const
  { return xppr12 ;
  }
  inline double CATHARE_FreonR12::gv_hmax() const
  { return xhvpr12 ;
  }
  inline double CATHARE_FreonR12::gv_Tmax() const
  { return xtgpr12 + tabsk ;
  }


  // calca_all_saturation

  inline EOS_Error CATHARE_FreonR12::calca_all_saturation (const NEPTUNE::EOS_Field& P, EOS_Fields& r, EOS_Error_Field& err) const
  {
    // -- init
    static int szmax = 1 ;

    // -- local ArrOfDouble ...
    // -- -- fpsatr12
    static ArrOfDouble locp(szmax)    ;
    static ArrOfDouble loctsp(szmax)  ;
    static ArrOfDouble loctsp1(szmax) ;
    // -- -- frovlr12
    static ArrOfDouble locrvsp(szmax) ;
    static ArrOfDouble locrlsp(szmax) ;
    static ArrOfDouble locrlsp1(szmax);
    static ArrOfDouble locrvsp1(szmax);
    // -- -- fhsatr12
    static ArrOfDouble lochvsp(szmax) ;
    static ArrOfDouble lochlsp(szmax) ;
    static ArrOfDouble lochvsp1(szmax);
    static ArrOfDouble lochlsp1(szmax); 
 
    // -- ArrOfDouble for compute ...
    // -- -- fpsatr12
    static ArrOfDouble tp; //(szmax);
    static ArrOfDouble ttsp; //(szmax);
    static ArrOfDouble ttsp1; //(szmax);
    // -- -- frovlr12
    static ArrOfDouble trvsp; //(szmax);
    static ArrOfDouble trlsp; //(szmax);
    static ArrOfDouble trlsp1; //(szmax);
    static ArrOfDouble trvsp1; //(szmax);
    // -- -- fhsatr12
    static ArrOfDouble thvsp; //(szmax);
    static ArrOfDouble thlsp; //(szmax);
    static ArrOfDouble thvsp1; //(szmax);
    static ArrOfDouble thlsp1; //(szmax); 

    // -- ArrOfInt for error
    static ArrOfInt    terrc(szmax);
    static ArrOfInt    internal_err_tmp(szmax);

    // -- -- resize if needed
    int sz = P.size() ;
    if (sz > szmax)
       { szmax = sz ;

         // -- local ArrOfDouble ...
         // -- -- fpsatr12
         locp.resize(sz)    ;
         loctsp.resize(sz)  ;
         loctsp1.resize(sz) ;
         // -- -- frovlr12
         locrvsp.resize(sz)  ;
         locrlsp.resize(sz)  ;
         locrlsp1.resize(sz) ;
         locrvsp1.resize(sz) ;
         // -- -- fhsatr12
         lochvsp.resize(sz)  ;
         lochlsp.resize(sz)  ;
         lochvsp1.resize(sz) ;
         lochlsp1.resize(sz) ; 

//       // -- ArrOfDouble for compute ...
//       // -- -- fpsatr12
//       tp.resize(sz);
//       ttsp.resize(sz);
//       ttsp1.resize(sz);
//       // -- -- frovlr12
//       trvsp.resize(sz);
//       trlsp.resize(sz);
//       trlsp1.resize(sz);
//       trvsp1.resize(sz);
//       // -- -- fhsatr12
//       thvsp.resize(sz);
//       thlsp.resize(sz);
//       thvsp1.resize(sz);
//       thlsp1.resize(sz); 
       }

    // -- -- resize (needed)
    // -- ArrOfInt for error
    terrc.resize(sz) ;
    internal_err_tmp.resize(sz) ;

  
    // -- -- t-->loc
    // -- -- fpsatr12
    tp.set_ptr(sz,locp.get_ptr())       ;
    ttsp.set_ptr(sz,loctsp.get_ptr())   ;
    ttsp1.set_ptr(sz,loctsp1.get_ptr()) ;
    // -- -- frovlr12
    trvsp.set_ptr(sz,locrvsp.get_ptr())   ;
    trlsp.set_ptr(sz,locrlsp.get_ptr())   ;
    trlsp1.set_ptr(sz,locrlsp1.get_ptr()) ;
    trvsp1.set_ptr(sz,locrvsp1.get_ptr()) ;
    // -- -- fhsatr12
    thvsp.set_ptr(sz,lochvsp.get_ptr())   ;
    thlsp.set_ptr(sz,lochlsp.get_ptr())   ;
    thvsp1.set_ptr(sz,lochvsp1.get_ptr()) ;
    thlsp1.set_ptr(sz,lochlsp1.get_ptr()) ;


    // --
    EOS_Error_Field err_tmp(internal_err_tmp) ;
    EOS_thermprop prop    ;
    EOS_saturprop propsat ;
    int comp_psatr12 = 0 ;
    int comp_rovlr12 = 0 ;
    int comp_hsatr12 = 0 ;
    int chg_T_sat    = 0 ;

    // -- init for error 
    int iel    = 0 ;
    int ivstat = 0 ;
    terrc      = 0 ;
    err     = EOS_Internal_Error::OK ;
    err_tmp = EOS_Internal_Error::OK ;

    
    // --  input
    prop = P.get_property() ;
    switch(prop)
       { case NEPTUNE::p :
            tp.set_ptr(sz, P.get_data().get_ptr()) ;
            break ;
         default :
           propsat = P.get_sat_property() ;
           switch(propsat)
              { case NEPTUNE::p_sat :
                   tp.set_ptr(sz, P.get_data().get_ptr()) ;
                   break ;
                default :
                   err = EOS_Internal_Error::NOT_IMPLEMENTED ;
                   return EOS_Error::error ;
                   break ;
              }
           break;
       }


    // -- output
    int nb_fields = r.size()    ;
    ArrOfInt not_def(nb_fields) ;
    int ind_not_def = -1        ;

    for (int i=0; i<nb_fields; i++)
       { propsat = r[i].get_sat_property() ;
         switch(propsat)
            { case NEPTUNE::T_sat :
                 ttsp.set_ptr(sz, r[i].get_data().get_ptr()) ;
                 comp_psatr12 = 1 ;
                 chg_T_sat = 1 ;
                 break ;
              case NEPTUNE::h_l_sat :
                 thlsp.set_ptr(sz, r[i].get_data().get_ptr()) ;
                 comp_hsatr12 = 1 ;
                 break ;
              case NEPTUNE::h_v_sat :
                 thvsp.set_ptr(sz, r[i].get_data().get_ptr()) ;
                 comp_hsatr12 = 1 ;
                 break ;
              case NEPTUNE::rho_l_sat :
                 trlsp.set_ptr(sz, r[i].get_data().get_ptr()) ;
                 comp_rovlr12 = 1 ;
                 break ;
              case NEPTUNE::rho_v_sat :
                 trvsp.set_ptr(sz, r[i].get_data().get_ptr()) ;
                 comp_rovlr12 = 1 ;
                 break ;
              case NEPTUNE::d_T_sat_d_p :
                 ttsp1.set_ptr(sz, r[i].get_data().get_ptr()) ;
                 comp_psatr12 = 1 ;
                 break ;
              case NEPTUNE::d_h_l_sat_d_p :
                 thlsp1.set_ptr(sz, r[i].get_data().get_ptr()) ;
                 comp_hsatr12 = 1 ;
                 break ;
              case NEPTUNE::d_h_v_sat_d_p :
                 thvsp1.set_ptr(sz, r[i].get_data().get_ptr()) ;
                 comp_hsatr12 = 1 ;
                 break ;
              case NEPTUNE::d_rho_l_sat_d_p :
                 trlsp1.set_ptr(sz, r[i].get_data().get_ptr()) ;
                 comp_rovlr12 = 1 ;
                 break ;
              case NEPTUNE::d_rho_v_sat_d_p :
                 trvsp1.set_ptr(sz, r[i].get_data().get_ptr()) ;
                 comp_rovlr12 = 1 ;
                 break ;
              case NEPTUNE::NotASatProperty :
                 err = EOS_Internal_Error::NOT_IMPLEMENTED ;
                 return EOS_Error::error ;
                 break ;
              default :
                 ind_not_def += 1 ;
                 not_def(ind_not_def) = i ;
                 break ;
            }
       }


    // -- compute using fortran if needed

    if (comp_hsatr12 == 1) comp_rovlr12 = 1 ;
    if (comp_rovlr12 == 1) comp_psatr12 = 1 ;
    
    // -- -- fpsatr12
    if (comp_psatr12 == 1)
      { F77DECLARE(ca_fpsatr12)(sz, tp[0], ttsp[0], ttsp1[0],
                          iel, ivstat, terrc[0]) ;
        convert_eos_error(terrc,err_tmp) ;
        err.set_worst_error(err_tmp) ;
      }

    // -- -- frovlr12 
    if (comp_rovlr12 == 1)
      { F77DECLARE(ca_frovlr12)(sz, tp[0], ttsp[0], ttsp[0],
                trvsp[0], trlsp[0],
                ttsp1[0], trlsp1[0], trvsp1[0],
                iel, ivstat, terrc[0]) ;
        convert_eos_error(terrc,err_tmp) ;
        err.set_worst_error(err_tmp) ;
      }

    // -- -- fhsatr12
    if (comp_hsatr12 == 1)
       { F77DECLARE(ca_fhsatr12)(sz, ttsp[0], trvsp[0], trlsp[0], tp[0],
                           thvsp[0], thlsp[0],
                           ttsp1[0], trvsp1[0], trlsp1[0], thvsp1[0], thlsp1[0]) ;
       }

    // -- -- others
    // -- -- (all not defined)
    for (int i=0; i<ind_not_def+1; i++)
       { the_fluid->EOS_Fluid::compute(P,r[not_def[i]],err_tmp) ;
         err.set_worst_error(err_tmp) ;   
       }

    // -- change unit if needed
    if (chg_T_sat == 1) ttsp += tabsk ;

    // -- end
    return err.find_worst_error().generic_error() ;

  }


  // calca_all_liquid

  inline EOS_Error CATHARE_FreonR12::calca_all_liquid (const EOS_Fields& input, EOS_Fields& r, EOS_Error_Field& err) const
  {
    // -- init
    static int szmax = 1 ;

    // -- local ArrOfDouble ...
    // -- -- fpsatr12
    static ArrOfDouble locp(szmax)    ;
    static ArrOfDouble loctsp(szmax)  ;
    static ArrOfDouble loctsp1(szmax) ;
    // -- -- frovlr12
    static ArrOfDouble locrvsp(szmax)  ;
    static ArrOfDouble locrlsp(szmax)  ;
    static ArrOfDouble locrlsp1(szmax) ;
    static ArrOfDouble locrvsp1(szmax) ;
    // -- -- fhsatr12
    static ArrOfDouble lochvsp(szmax)  ;
    static ArrOfDouble lochlsp(szmax)  ;
    static ArrOfDouble lochvsp1(szmax) ;
    static ArrOfDouble lochlsp1(szmax) ; 
    // -- -- fpthlr12
    static ArrOfDouble lochl(szmax) ;
    static ArrOfDouble loctl(szmax) ;
    static ArrOfDouble locrl(szmax) ;
    // -- -- fcplr12
    static ArrOfDouble loccpl(szmax)  ;
    static ArrOfDouble loctl1(szmax)  ;
    static ArrOfDouble loctl2(szmax)  ;
    static ArrOfDouble locrl1(szmax)  ;
    static ArrOfDouble locrl2(szmax)  ;
    static ArrOfDouble loccpl1(szmax) ;
    static ArrOfDouble loccpl2(szmax) ;
    // -- -- fmulr12
    static ArrOfDouble loctmul(szmax)  ;
    static ArrOfDouble loctmul1(szmax) ;
    static ArrOfDouble loctmul2(szmax) ;
    // -- -- fconlr12
    static ArrOfDouble loctlal(szmax)  ;
    static ArrOfDouble loctlal1(szmax) ;
    static ArrOfDouble loctlal2(szmax) ;
    // -- -- others
    static ArrOfDouble locc(szmax);

    // -- ArrOfDouble for compute ...
    // -- -- fpsatr12
    static ArrOfDouble tp     ; //(szmax);
    static ArrOfDouble ttsp   ; //(szmax);
    static ArrOfDouble ttsp1  ; //(szmax);
    // -- -- frovlr12
    static ArrOfDouble trvsp  ; //(szmax);
    static ArrOfDouble trlsp  ; //(szmax);
    static ArrOfDouble trlsp1 ; //(szmax);
    static ArrOfDouble trvsp1 ; //(szmax);
    // -- -- fhsatr12
    static ArrOfDouble thvsp  ; //(szmax);
    static ArrOfDouble thlsp  ; //(szmax);
    static ArrOfDouble thvsp1 ; //(szmax);
    static ArrOfDouble thlsp1 ; //(szmax); 
    // -- -- fpthlr12
    static ArrOfDouble thl    ; //(szmax);
    static ArrOfDouble ttl    ; //(szmax);
    static ArrOfDouble trl    ; //(szmax);
    // -- -- fcplr12
    static ArrOfDouble tcpl   ; //(szmax);
    static ArrOfDouble ttl1   ; //(szmax);
    static ArrOfDouble ttl2   ; //(szmax);
    static ArrOfDouble trl1   ; //(szmax);
    static ArrOfDouble trl2   ; //(szmax);
    static ArrOfDouble tcpl1  ; //(szmax);
    static ArrOfDouble tcpl2  ; //(szmax);
    // -- -- fmulr12
    static ArrOfDouble ttmul  ; //(szmax);
    static ArrOfDouble ttmul1 ; //(szmax);
    static ArrOfDouble ttmul2 ; //(szmax);
    // -- -- fconlr12
    static ArrOfDouble ttlal  ; //(szmax);
    static ArrOfDouble ttlal1 ; //(szmax);
    static ArrOfDouble ttlal2 ; //(szmax);
    // -- -- others
    static ArrOfDouble tc ; //(szmax);

    // -- ArrOfInt for error
    static ArrOfInt terrc(szmax) ;
    static ArrOfInt internal_err_tmp(szmax) ;


    // -- -- resize if needed
    int sz = input[0].size() ;
    if (sz > szmax)
       { szmax = sz ;

         // -- local ArrOfDouble ...
         // -- -- fpsatr12
         locp.resize(sz)    ;
         loctsp.resize(sz)  ;
         loctsp1.resize(sz) ;
         // -- -- frovlr12
         locrvsp.resize(sz)  ;
         locrlsp.resize(sz)  ;
         locrlsp1.resize(sz) ;
         locrvsp1.resize(sz) ;
         // -- -- fhsatr12
         lochvsp.resize(sz)  ;
         lochlsp.resize(sz)  ;
         lochvsp1.resize(sz) ;
         lochlsp1.resize(sz) ; 
         // -- -- fpthlr12
         lochl.resize(sz) ;
         loctl.resize(sz) ;
         locrl.resize(sz) ;
         // -- -- fcplr12
         loccpl.resize(sz)  ;
         loctl1.resize(sz)  ;
         loctl2.resize(sz)  ;
         locrl1.resize(sz)  ;
         locrl2.resize(sz)  ;
         loccpl1.resize(sz) ;
         loccpl2.resize(sz) ;
         // -- -- fmulr12
         loctmul.resize(sz)  ;
         loctmul1.resize(sz) ;
         loctmul2.resize(sz) ;
         // -- -- fconlr12
         loctlal.resize(sz)  ;
         loctlal1.resize(sz) ;
         loctlal2.resize(sz) ;
         // -- -- others
         locc.resize(sz) ;

//       // -- ArrOfDouble for compute ...
//       // -- -- fpsatr12
//       tp.resize(sz);
//       ttsp.resize(sz);
//       ttsp1.resize(sz);
//       // -- -- frovlr12
//       trvsp.resize(sz);
//       trlsp.resize(sz);
//       trlsp1.resize(sz);
//       trvsp1.resize(sz);
//       // -- -- fhsatr12
//       thvsp.resize(sz);
//       thlsp.resize(sz);
//       thvsp1.resize(sz);
//       thlsp1.resize(sz); 
//       // -- -- fpthlr12
//       thl.resize(sz);
//       ttl.resize(sz);
//       trl.resize(sz);
//       // -- -- fcplr12
//       tcpl.resize(sz);
//       ttl1.resize(sz);
//       ttl2.resize(sz);
//       trl1.resize(sz);
//       trl2.resize(sz);
//       tcpl1.resize(sz);
//       tcpl2.resize(sz);
//       // -- -- fmulr12
//       ttmul.resize(sz);
//       ttmul1.resize(sz);
//       ttmul2.resize(sz);
//       // -- -- fconlr12
//       ttlal.resize(sz);
//       ttlal1.resize(sz);
//       ttlal2.resize(sz);
//       // -- -- others
//       tc.resize(sz);
       }

    // -- -- resize (needed)
    // -- ArrOfInt for error
    terrc.resize(sz) ;
    internal_err_tmp.resize(sz) ;
    
 
    // -- -- t-->loc
    // -- -- fpsatr12
    tp.set_ptr(sz,locp.get_ptr())       ;
    ttsp.set_ptr(sz,loctsp.get_ptr())   ;
    ttsp1.set_ptr(sz,loctsp1.get_ptr()) ;
    // -- -- frovlr12
    trvsp.set_ptr(sz,locrvsp.get_ptr())   ;
    trlsp.set_ptr(sz,locrlsp.get_ptr())   ;
    trlsp1.set_ptr(sz,locrlsp1.get_ptr()) ;
    trvsp1.set_ptr(sz,locrvsp1.get_ptr()) ;
    // -- -- fhsatr12
    thvsp.set_ptr(sz,lochvsp.get_ptr())   ;
    thlsp.set_ptr(sz,lochlsp.get_ptr())   ;
    thvsp1.set_ptr(sz,lochvsp1.get_ptr()) ;
    thlsp1.set_ptr(sz,lochlsp1.get_ptr()) ;
    // -- -- fpthlr12
    thl.set_ptr(sz,lochl.get_ptr()) ;
    ttl.set_ptr(sz,loctl.get_ptr()) ;
    trl.set_ptr(sz,locrl.get_ptr()) ;
    // -- -- fcplr12
    tcpl.set_ptr(sz,loccpl.get_ptr())   ;
    ttl1.set_ptr(sz,loctl1.get_ptr())   ;
    ttl2.set_ptr(sz,loctl2.get_ptr())   ;
    trl1.set_ptr(sz,locrl1.get_ptr())   ;
    trl2.set_ptr(sz,locrl2.get_ptr())   ;
    tcpl1.set_ptr(sz,loccpl1.get_ptr()) ;
    tcpl2.set_ptr(sz,loccpl2.get_ptr()) ;
    // -- -- fmulr12
    ttmul.set_ptr(sz,loctmul.get_ptr())   ;
    ttmul1.set_ptr(sz,loctmul1.get_ptr()) ;
    ttmul2.set_ptr(sz,loctmul2.get_ptr()) ;
    // -- -- fconlr12
    ttlal.set_ptr(sz,loctlal.get_ptr())   ;
    ttlal1.set_ptr(sz,loctlal1.get_ptr()) ;
    ttlal2.set_ptr(sz,loctlal2.get_ptr()) ;
    // -- -- others
    tc.set_ptr(sz,locc.get_ptr()) ;

    // --  
    EOS_Error_Field err_tmp(internal_err_tmp) ;
    EOS_thermprop prop    ;
    EOS_saturprop propsat ;
    int comp_psatr12 = 0 ;
    int comp_rovlr12 = 0 ;
    int comp_hsatr12 = 0 ;
    int comp_pthlr12 = 0 ;
    int comp_tlr12   = 0 ;
    int comp_cplr12  = 0 ;
    int comp_mulr12  = 0 ;
    int comp_lalr12  = 0 ;
    int comp_c       = 0 ;
    int chg_T        = 0 ;
    int chg_T_sat    = 0 ;

    // -- init for error 
    int iel    = 0 ;
    int ivstat = 0 ;
    terrc = 0 ;
    err = EOS_Internal_Error::OK   ;
    err_tmp=EOS_Internal_Error::OK ;


    // -- -- input
    ArrOfDouble tintl;
    int v1=0;
    int v2=0;
    int nb_fields_in=input.size();

    for (int i=0; i<nb_fields_in; i++)
       { prop = input[i].get_property() ;
         switch(prop)
            { case NEPTUNE::p :
                 tp.set_ptr(sz, input[i].get_data().get_ptr());
                 v1=1;           
                 break;
              case NEPTUNE::h :
                 thl.set_ptr(sz, input[i].get_data().get_ptr());
                 v2=1;
                 break;
              case NEPTUNE::T :
                 tintl.set_ptr(sz, input[i].get_data().get_ptr());
                 v2=1;
                 comp_pthlr12=1;
                 break;
              default :
                 err = EOS_Internal_Error::NOT_IMPLEMENTED ;
                 return EOS_Error::error ;
                 break;
            }
       }


    // -- -- check input
    
    if ( (nb_fields_in != 2) || (v1*v2 != 1) )
       { err = EOS_Internal_Error::NOT_IMPLEMENTED ;
         return EOS_Error::error ;
       }

    // -- -- output
    int nb_fields = r.size() ;
    ArrOfInt not_def(nb_fields) ;
    int ind_not_def = -1 ;

    //
    for (int i=0; i<nb_fields; i++)
       { prop = r[i].get_property();
         switch(prop)
            { case NEPTUNE::h :
                 thl.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_pthlr12=1;
                 break;
              case NEPTUNE::T :
                 ttl.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_tlr12=1;
                 chg_T=1;
                 break;
              case NEPTUNE::rho :
                 trl.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_tlr12=1;
                 break;
              case NEPTUNE::cp :
                 tcpl.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_cplr12=1;
                 break;
              case NEPTUNE::lambda :
                 ttlal.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_lalr12=1;
                 break;
              case NEPTUNE::mu :
                 ttmul.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_mulr12=1;
                 break;
              case NEPTUNE::w :
                 tc.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_c=1;
                 break;
              // Derivatives
              case NEPTUNE::d_T_d_p_h  :
                 ttl1.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_cplr12=1;
                 break;
              case NEPTUNE::d_T_d_h_p :
                 ttl2.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_cplr12=1;
                 break;
              case NEPTUNE::d_rho_d_p_h  :
                 trl1.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_cplr12=1;
                 break;
              case NEPTUNE::d_rho_d_h_p :
                 trl2.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_cplr12=1;
                 break;
              case NEPTUNE::d_cp_d_p_h  :
                 tcpl1.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_cplr12=1;
                 break;
              case NEPTUNE::d_cp_d_h_p :
                 tcpl2.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_cplr12=1;
                 break;
              case NEPTUNE::d_lambda_d_p_h  :
                 ttlal1.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_lalr12=1;
                 break;
              case NEPTUNE::d_lambda_d_h_p :
                 ttlal2.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_lalr12=1;
                 break;
              case NEPTUNE::d_mu_d_p_h  :
                 ttmul1.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_mulr12=1;
                 break;
              case NEPTUNE::d_mu_d_h_p :
                 ttmul2.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_mulr12=1;
                 break ;
              case NEPTUNE::NotATProperty :
                 // Thermodynamic Properties at Saturation
                 { propsat = r[i].get_sat_property() ;
                   switch(propsat)
                      { case NEPTUNE::T_sat :
                           ttsp.set_ptr(sz, r[i].get_data().get_ptr());
                           comp_psatr12=1;
                           chg_T_sat=1;
                           break;
                        case NEPTUNE::h_l_sat :
                           thlsp.set_ptr(sz, r[i].get_data().get_ptr());
                           comp_hsatr12=1;
                           break;
                        case NEPTUNE::rho_l_sat :
                           trlsp.set_ptr(sz, r[i].get_data().get_ptr());
                           comp_rovlr12=1;
                           break;
                        case NEPTUNE::h_v_sat :
                           thvsp.set_ptr(sz, r[i].get_data().get_ptr());
                           comp_hsatr12=1;
                           break;
                        case NEPTUNE::rho_v_sat :
                           trvsp.set_ptr(sz, r[i].get_data().get_ptr());
                           comp_rovlr12=1;
                           break;
                        // Derivatives
                        case NEPTUNE::d_T_sat_d_p :
                           ttsp1.set_ptr(sz, r[i].get_data().get_ptr());
                           comp_psatr12=1;
                           break;
                        case NEPTUNE::d_h_l_sat_d_p :
                           thlsp1.set_ptr(sz, r[i].get_data().get_ptr());
                           comp_hsatr12=1;
                           break;
                        case NEPTUNE::d_rho_l_sat_d_p :
                           trlsp1.set_ptr(sz, r[i].get_data().get_ptr());
                           comp_rovlr12=1;
                           break;
                        case NEPTUNE::d_h_v_sat_d_p :
                           thvsp1.set_ptr(sz, r[i].get_data().get_ptr());
                           comp_hsatr12=1;
                           break;
                        case NEPTUNE::d_rho_v_sat_d_p :
                           trvsp1.set_ptr(sz, r[i].get_data().get_ptr());
                           comp_rovlr12=1;
                           break;
                        case NEPTUNE::NotASatProperty :
                           err = EOS_Internal_Error::NOT_IMPLEMENTED ;
                           return EOS_Error::error ;
                           break;
                        default :
                           ind_not_def += 1 ;
                           not_def(ind_not_def) = i ;
                           break ;
                      }
                 }
              default :
                ind_not_def += 1 ;
                not_def(ind_not_def) = i ;
                break ;
            }
       }


    // -- compute using fortran if needed
    if (comp_c       == 1 )  comp_cplr12  = 1 ;
    if (comp_lalr12  == 1 )  comp_cplr12  = 1 ;  
    if (comp_mulr12  == 1 )  comp_cplr12  = 1 ;  
    if (comp_cplr12  == 1 )  comp_tlr12   = 1 ;
    if (comp_tlr12   == 1 )  comp_hsatr12 = 1 ;
    if (comp_hsatr12 == 1 )  comp_rovlr12 = 1 ;
    if (comp_rovlr12 == 1 )  comp_psatr12 = 1 ;


    // -- -- fpsatr12
    if (comp_psatr12 == 1)
       { F77DECLARE(ca_fpsatr12)(sz, tp[0], ttsp[0], ttsp1[0],
                           iel, ivstat, terrc[0]) ;
         convert_eos_error(terrc,err_tmp) ;
         err.set_worst_error(err_tmp) ;
       }

    // -- -- frovlr12 
    if (comp_rovlr12 == 1)
       { F77DECLARE(ca_frovlr12)(sz, tp[0], ttsp[0], ttsp[0],
                           trvsp[0], trlsp[0],
                           ttsp1[0], trlsp1[0], trvsp1[0],
                           iel, ivstat, terrc[0]) ;
         convert_eos_error(terrc,err_tmp) ;
         err.set_worst_error(err_tmp) ;
       }

    // -- -- fhsatr12
    if (comp_hsatr12 == 1)
       { F77DECLARE(ca_fhsatr12)(sz, ttsp[0], trvsp[0], trlsp[0], tp[0],
                           thvsp[0], thlsp[0],
                           ttsp1[0], trvsp1[0], trlsp1[0], thvsp1[0], thlsp1[0]) ;
       }
    
    // -- fpthlr12
    if (comp_pthlr12 == 1)
       { ArrOfDouble ttlcelcius(sz) ;
         for(int i=0; i<sz; i++)
            ttlcelcius[i] = tintl[i]-tabsk ;
         F77DECLARE(ca_fpthlr12)(sz, ttlcelcius[0], thl[0],
                           iel, ivstat, terrc[0]) ;
         convert_eos_error(terrc,err_tmp) ;
         err.set_worst_error(err_tmp) ;
       }

    // -- ftlr12
    if (comp_tlr12 == 1)
       { F77DECLARE(ca_ftlr12)(sz, tp[0], ttsp[0], trvsp[0], thvsp[0], thl[0],
                         ttl[0], trl[0],
                         iel, ivstat, terrc[0]) ;
         convert_eos_error(terrc,err_tmp) ;
         err.set_worst_error(err_tmp) ;
       }

    // -- fcplr12
    if (comp_cplr12 == 1)
       { F77DECLARE(ca_fcplr12)(sz, ttl[0],
                          tcpl[0], ttl1[0], ttl2[0],
                          trl1[0], trl2[0], tcpl1[0], tcpl2[0],
                          iel, ivstat, terrc[0]) ;
         convert_eos_error(terrc,err_tmp);
         err.set_worst_error(err_tmp);
       }

    // -- fmulr12
    if (comp_mulr12 == 1)
       { F77DECLARE(ca_fmulr12)(sz, ttl[0], ttl1[0], ttl2[0],
                          ttmul[0], ttmul1[0], ttmul2[0]) ;
       }

    // -- fconlr12
    if (comp_lalr12 == 1)
       { F77DECLARE(ca_fconlr12)(sz, ttl[0], ttl1[0], ttl2[0],
                           ttlal[0], ttlal1[0], ttlal2[0]) ;
       }


    // -- -- others
    // -- -- (c)

    if (comp_c == 1)
       { double cc ;
         double m05 = -0.5 ;
         for(int i=0; i<sz; i++)
            { cc = trl1[i]+trl2[i]/trl[i] ;
              tc[i] = pow(cc,m05) ;
            }
       }
    
    // -- -- (all not defined)
    for (int i=0; i<ind_not_def+1; i++)
       { the_fluid->EOS_Fluid::compute(input[0],input[1],r[not_def[i]],err_tmp) ;
         err.set_worst_error(err_tmp) ;   
       }


    // -- change unit if needed
    if (chg_T_sat == 1)  ttsp += tabsk ;
    if (chg_T     == 1)  ttl  += tabsk ;

    // -- end
    return err.find_worst_error().generic_error() ;
  }


  // calca_all_vapor

  inline EOS_Error CATHARE_FreonR12::calca_all_vapor (const EOS_Fields& input, EOS_Fields& r, EOS_Error_Field& err) const
  {
    // -- init
    static int szmax=1;

    // -- local ArrOfDouble ...
    // -- -- fpsatr12
    static ArrOfDouble locp(szmax);
    static ArrOfDouble loctsp(szmax);
    static ArrOfDouble loctsp1(szmax);
    // -- -- frovlr12
    static ArrOfDouble locrvsp(szmax);
    static ArrOfDouble locrlsp(szmax);
    static ArrOfDouble locrlsp1(szmax);
    static ArrOfDouble locrvsp1(szmax);
    // -- -- fhsatr12
    static ArrOfDouble lochvsp(szmax);
    static ArrOfDouble lochlsp(szmax);
    static ArrOfDouble lochvsp1(szmax);
    static ArrOfDouble lochlsp1(szmax); 
    // -- -- fpthgr12
    static ArrOfDouble lochg(szmax);
    static ArrOfDouble loctg(szmax);
    // -- -- ftgr12
    static ArrOfDouble loctgpv(szmax);
    static ArrOfDouble loctghv(szmax);
    static ArrOfDouble locrv(szmax);
    static ArrOfDouble locrvpv(szmax);
    static ArrOfDouble locrvhv(szmax);
    // -- -- fcpvr12
    static ArrOfDouble loccpv(szmax);
    static ArrOfDouble loccpvpv(szmax);
    static ArrOfDouble loccpvhv(szmax);
    // -- -- fmuvr12
    static ArrOfDouble loctmuv(szmax);
    static ArrOfDouble locmuvpv(szmax);
    static ArrOfDouble locmuvhv(szmax);
    // -- -- fconvr12
    static ArrOfDouble loctlav(szmax);
    static ArrOfDouble loclavpv(szmax);
    static ArrOfDouble loclavhv(szmax);
    // -- -- fsigmar12
    static ArrOfDouble locsi(szmax);
    static ArrOfDouble locsipv(szmax);
    // -- -- others
    static ArrOfDouble locc(szmax);

    // -- ArrOfDouble for compute ...
    // -- -- fpsatr12
    static ArrOfDouble tp; //(szmax);
    static ArrOfDouble ttsp; //(szmax);
    static ArrOfDouble ttsp1; //(szmax);
    // -- -- frovlr12
    static ArrOfDouble trvsp; //(szmax);
    static ArrOfDouble trlsp; //(szmax);
    static ArrOfDouble trlsp1; //(szmax);
    static ArrOfDouble trvsp1; //(szmax);
    // -- -- fhsatr12
    static ArrOfDouble thvsp; //(szmax);
    static ArrOfDouble thlsp; //(szmax);
    static ArrOfDouble thvsp1; //(szmax);
    static ArrOfDouble thlsp1; //(szmax); 
    // -- -- fpthgr12
    static ArrOfDouble ttg; //(szmax);
    static ArrOfDouble thg; //(szmax);
    // -- -- ftgr12
    static ArrOfDouble ttgpv; //(szmax);
    static ArrOfDouble ttghv; //(szmax);
    static ArrOfDouble trv; //(szmax);
    static ArrOfDouble trvpv; //(szmax);
    static ArrOfDouble trvhv; //(szmax);
    // -- -- fcpvr12
    static ArrOfDouble tcpv; //(szmax);
    static ArrOfDouble tcpvpv; //(szmax);
    static ArrOfDouble tcpvhv; //(szmax);
    // -- -- fmuvr12
    static ArrOfDouble ttmuv; //(szmax);
    static ArrOfDouble tmuvpv; //(szmax);
    static ArrOfDouble tmuvhv; //(szmax);
    // -- -- fconvr12
    static ArrOfDouble ttlav; //(szmax);
    static ArrOfDouble tlavpv; //(szmax);
    static ArrOfDouble tlavhv; //(szmax);
    // -- -- fsigmar12
    static ArrOfDouble tsi; //(szmax);
    static ArrOfDouble tsipv; //(szmax);
    // -- -- others
    static ArrOfDouble tc; //(szmax);

    // -- ArrOfInt for error
    static ArrOfInt    terrc(szmax);
    static ArrOfInt    internal_err_tmp(szmax);

    // -- -- resize if needed
    int sz = input[0].size() ;
    if (sz > szmax)
       { szmax = sz ;

         // -- local ArrOfDouble ...
         // -- -- fpsatr12
         locp.resize(sz);
         loctsp.resize(sz);
         loctsp1.resize(sz);
         // -- -- frovlr12
         locrvsp.resize(sz);
         locrlsp.resize(sz);
         locrlsp1.resize(sz);
         locrvsp1.resize(sz);
         // -- -- fhsatr12
         lochvsp.resize(sz);
         lochlsp.resize(sz);
         lochvsp1.resize(sz);
         lochlsp1.resize(sz); 
         // -- -- fpthgr12
         lochg.resize(sz);
         loctg.resize(sz);
         // -- -- ftgr12
         loctgpv.resize(sz);
         loctghv.resize(sz);
         locrv.resize(sz);
         locrvpv.resize(sz);
         locrvhv.resize(sz);
         // -- -- fcpvr12
         loccpv.resize(sz);
         loccpvpv.resize(sz);
         loccpvhv.resize(sz);
         // -- -- fmuvr12
         loctmuv.resize(sz);
         locmuvpv.resize(sz);
         locmuvhv.resize(sz);
         // -- -- fconvr12
         loctlav.resize(sz);
         loclavpv.resize(sz);
         loclavhv.resize(sz);
         // -- -- fsigmar12
         locsi.resize(sz);
         locsipv.resize(sz);
         // -- -- others
         locc.resize(sz);

//       // -- ArrOfDouble for compute ...
//       // -- -- fpsatr12
//       tp.resize(sz);
//       ttsp.resize(sz);
//       ttsp1.resize(sz);
//       // -- -- frovlr12
//       trvsp.resize(sz);
//       trlsp.resize(sz);
//       trlsp1.resize(sz);
//       trvsp1.resize(sz);
//       // -- -- fhsatr12
//       thvsp.resize(sz);
//       thlsp.resize(sz);
//       thvsp1.resize(sz);
//       thlsp1.resize(sz); 
//       // -- -- fpthgr12
//       thg.resize(sz);
//       ttg.resize(sz);
//       // -- -- ftgr12
//       ttgpv.resize(sz);
//       ttghv.resize(sz);
//       trv.resize(sz);
//       trvpv.resize(sz);
//       trvhv.resize(sz);
//       // -- -- fcpvr12
//       tcpv.resize(sz);
//       tcpvpv.resize(sz);
//       tcpvhv.resize(sz);
//       // -- -- fmuvr12
//       ttmuv.resize(sz);
//       tmuvpv.resize(sz);
//       tmuvhv.resize(sz);
//       // -- -- fconvr12
//       ttlav.resize(sz);
//       tlavpv.resize(sz);
//       tlavhv.resize(sz);
//       // -- -- fsigmar12
//       tsi.resize(sz);
//       tsipv.resize(sz);
//       // -- -- others
//       tc.resize(sz);
       }
 
    // -- -- resize (needed)
    // -- ArrOfInt for error 
    terrc.resize(sz);
    internal_err_tmp.resize(sz);

    // -- -- t-->loc
    // -- -- fpsatr12
    tp.set_ptr(sz,locp.get_ptr());
    ttsp.set_ptr(sz,loctsp.get_ptr());
    ttsp1.set_ptr(sz,loctsp1.get_ptr());
    // -- -- frovlr12
    trvsp.set_ptr(sz,locrvsp.get_ptr());
    trlsp.set_ptr(sz,locrlsp.get_ptr());
    trlsp1.set_ptr(sz,locrlsp1.get_ptr());
    trvsp1.set_ptr(sz,locrvsp1.get_ptr());
    // -- -- fhsatr12
    thvsp.set_ptr(sz,lochvsp.get_ptr());
    thlsp.set_ptr(sz,lochlsp.get_ptr());
    thvsp1.set_ptr(sz,lochvsp1.get_ptr());
    thlsp1.set_ptr(sz,lochlsp1.get_ptr());
    // -- -- fpthgr12
    thg.set_ptr(sz,lochg.get_ptr());
    ttg.set_ptr(sz,loctg.get_ptr());
    // -- -- ftgr12
    ttgpv.set_ptr(sz,loctgpv.get_ptr());
    ttghv.set_ptr(sz,loctghv.get_ptr());
    trv.set_ptr(sz,locrv.get_ptr());
    trvpv.set_ptr(sz,locrvpv.get_ptr());
    trvhv.set_ptr(sz,locrvhv.get_ptr());
    // -- -- fcpvr12
    tcpv.set_ptr(sz,loccpv.get_ptr());
    tcpvpv.set_ptr(sz,loccpvpv.get_ptr());
    tcpvhv.set_ptr(sz,loccpvhv.get_ptr());
    // -- -- fmuvr12
    ttmuv.set_ptr(sz,loctmuv.get_ptr());
    tmuvpv.set_ptr(sz,locmuvpv.get_ptr());
    tmuvhv.set_ptr(sz,locmuvhv.get_ptr());
    // -- -- fconvr12
    ttlav.set_ptr(sz,loctlav.get_ptr());
    tlavpv.set_ptr(sz,loclavpv.get_ptr());
    tlavhv.set_ptr(sz,loclavhv.get_ptr());
    // -- -- fsigmar12
    tsi.set_ptr(sz,locsi.get_ptr());
    tsipv.set_ptr(sz,locsipv.get_ptr());
    // -- -- others
    tc.set_ptr(sz,locc.get_ptr());


    // -- 
    EOS_Error_Field err_tmp(internal_err_tmp);
    EOS_thermprop prop ;
    EOS_saturprop propsat ;
    int comp_psatr12=0;
    int comp_rovlr12=0;
    int comp_hsatr12=0;
    int comp_pthgr12=0;
    int comp_tgr12=0;
    int comp_cpvr12=0;
    int comp_muvr12=0;
    int comp_convr12=0;
    int comp_sigmar12=0;
    int comp_c=0;
    int chg_T_sat=0;
    int chg_T=0;

    // -- init for error 
    int iel = 0 ;
    int ivstat = 0 ;
    terrc = 0 ;
    err     = EOS_Internal_Error::OK ;
    err_tmp = EOS_Internal_Error::OK ;

    // -- -- input
    ArrOfDouble tintg;
    int v1 = 0 ;
    int v2 = 0 ;
    int nb_fields_in = input.size();

    for (int i=0; i<nb_fields_in; i++)
       { prop = input[i].get_property() ;
         switch(prop)
            { case NEPTUNE::p :
                 tp.set_ptr(sz, input[i].get_data().get_ptr());
                 v1=1;           
                 break ;
              case NEPTUNE::h :
                 thg.set_ptr(sz, input[i].get_data().get_ptr());
                 v2=1;
                 break ;
              case NEPTUNE::T :
                 tintg.set_ptr(sz, input[i].get_data().get_ptr());
                 v2=1;           
                 comp_pthgr12=1;
                 break ;
              default :
                 err = EOS_Internal_Error::NOT_IMPLEMENTED ;
                 return EOS_Error::error ;
                 break ;
            }
       }

    // -- -- check input
    
    if ( (nb_fields_in!=2) || (v1*v2!=1) )
       { err = EOS_Internal_Error::NOT_IMPLEMENTED ;
         return EOS_Error::error ;
       }

    // -- -- output
    int nb_fields = r.size() ;
    ArrOfInt not_def(nb_fields) ;
    int ind_not_def = -1 ;

    for (int i=0; i<nb_fields; i++)
       { prop = r[i].get_property() ;
         switch(prop)
            { case NEPTUNE::h :
                 thg.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_pthgr12=1;
                 break;
              case NEPTUNE::T :
                 ttg.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_tgr12=1;
                 chg_T=1;
                 break;
              case NEPTUNE::rho :
                 trv.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_tgr12=1;
                 break;
              case NEPTUNE::cp :
                 tcpv.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_cpvr12=1;
                 break;
              case NEPTUNE::lambda :
                 ttlav.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_convr12=1;
                 break;
              case NEPTUNE::mu :
                 ttmuv.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_muvr12=1;
                 break;
              case NEPTUNE::w :
                 tc.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_c=1;
                 break;
              case NEPTUNE::sigma :
                 tsi.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_sigmar12=1;
                 break;
              // Derivatives
              case NEPTUNE::d_T_d_p_h  :
                 ttgpv.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_tgr12=1;
                 break;
              case NEPTUNE::d_T_d_h_p :
                 ttghv.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_tgr12=1;
                 break;
              case NEPTUNE::d_rho_d_p_h  :
                 trvpv.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_tgr12=1;
                 break;
              case NEPTUNE::d_rho_d_h_p :
                 trvhv.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_tgr12=1;
                 break;
              case NEPTUNE::d_cp_d_p_h  :
                 tcpvpv.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_cpvr12=1;
                 break;
              case NEPTUNE::d_cp_d_h_p :
                 tcpvhv.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_cpvr12=1;
                 break;
              case NEPTUNE::d_lambda_d_p_h  :
                 tlavpv.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_convr12=1;
                 break;
              case NEPTUNE::d_lambda_d_h_p :
                 tlavhv.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_convr12=1;
                 break;
              case NEPTUNE::d_mu_d_p_h  :
                 tmuvpv.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_muvr12=1;
                 break;
              case NEPTUNE::d_mu_d_h_p :
                 tmuvhv.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_muvr12=1;
                 break;
              case NEPTUNE::d_sigma_d_p_h  :
                 tsipv.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_sigmar12=1;
                 break;
              case NEPTUNE::NotATProperty :
                 // Thermodynamic Properties at Saturation
                 propsat = r[i].get_sat_property();
                 switch(propsat)
                    { case NEPTUNE::T_sat :
                         ttsp.set_ptr(sz, r[i].get_data().get_ptr());
                         comp_psatr12=1;
                         chg_T_sat=1;
                         break;
                      case NEPTUNE::h_v_sat :
                         thvsp.set_ptr(sz, r[i].get_data().get_ptr());
                         comp_hsatr12=1;
                         break;
                      case NEPTUNE::rho_v_sat :
                         trvsp.set_ptr(sz, r[i].get_data().get_ptr());
                         comp_rovlr12=1;
                         break;
                      case NEPTUNE::h_l_sat :
                         thlsp.set_ptr(sz, r[i].get_data().get_ptr());
                         comp_hsatr12=1;
                         break;
                      case NEPTUNE::rho_l_sat :
                         trlsp.set_ptr(sz, r[i].get_data().get_ptr());
                         comp_rovlr12=1;
                         break;
                      // Derivatives
                      case NEPTUNE::d_T_sat_d_p :
                         ttsp1.set_ptr(sz, r[i].get_data().get_ptr());
                         comp_psatr12=1;
                         break;
                      case NEPTUNE::d_h_v_sat_d_p :
                         thvsp1.set_ptr(sz, r[i].get_data().get_ptr());
                         comp_hsatr12=1;
                         break;
                      case NEPTUNE::d_rho_v_sat_d_p :
                         trvsp1.set_ptr(sz, r[i].get_data().get_ptr());
                         comp_rovlr12=1;
                         break;
                      case NEPTUNE::d_h_l_sat_d_p :
                         thlsp1.set_ptr(sz, r[i].get_data().get_ptr());
                         comp_hsatr12=1;
                         break;
                      case NEPTUNE::d_rho_l_sat_d_p :
                         trlsp1.set_ptr(sz, r[i].get_data().get_ptr());
                         comp_rovlr12=1;
                         break;
                      case NEPTUNE::NotASatProperty :
                         err = EOS_Internal_Error::NOT_IMPLEMENTED ;
                         return EOS_Error::error ;
                      default :
                         ind_not_def += 1 ;
                         not_def(ind_not_def) = i ;
                         break ;
                    }
                 break ;
              default :
                 ind_not_def += 1 ;
                 not_def(ind_not_def) = i ;
                 break ;
            }
       }      

    // -- compute using fortran if needed
    if (comp_c        == 1)  comp_tgr12   = 1 ;
    if (comp_sigmar12 == 1)  comp_psatr12 = 1 ;
    if (comp_cpvr12   == 1)  comp_tgr12   = 1 ;
    if (comp_muvr12   == 1)  comp_tgr12   = 1 ;
    if (comp_convr12  == 1)  comp_tgr12   = 1 ;
    if (comp_tgr12    == 1)  comp_hsatr12 = 1 ;
    if (comp_hsatr12  == 1)  comp_rovlr12 = 1 ;
    if (comp_rovlr12  == 1)  comp_psatr12 = 1 ;
    
    // -- -- fpsatr12
    if (comp_psatr12 == 1)
       { F77DECLARE(ca_fpsatr12)(sz, tp[0], ttsp[0], ttsp1[0],
                          iel, ivstat, terrc[0]) ;
         convert_eos_error(terrc,err_tmp);
         err.set_worst_error(err_tmp);
       }

    // -- -- fsigmar12
    if (comp_sigmar12 == 1)
       { F77DECLARE(ca_fsigmar12)(sz, ttsp[0], ttsp1[0],
                            tsi[0], tsipv[0]) ;
       }

    // -- -- frovlr12 
    if (comp_rovlr12 == 1)
       { F77DECLARE(ca_frovlr12)(sz, tp[0], ttsp[0], ttsp[0],
                           trvsp[0], trlsp[0],
                           ttsp1[0], trlsp1[0], trvsp1[0], 
                           iel, ivstat, terrc[0]) ;
         convert_eos_error(terrc,err_tmp);
         err.set_worst_error(err_tmp);
       }

    // -- -- fhsatr12
    if (comp_hsatr12 == 1)
       { F77DECLARE(ca_fhsatr12)(sz, ttsp[0], trvsp[0], trlsp[0], tp[0],
                           thvsp[0], thlsp[0], 
                           ttsp1[0], trvsp1[0], trlsp1[0], thvsp1[0], thlsp1[0]) ;
       }

    // -- -- fpthgr12

    if (comp_pthgr12 == 1)
       { ArrOfDouble ttgcelcius(sz) ;
         for(int i=0; i<sz; i++)
            ttgcelcius[i] = tintg[i]-tabsk; 
         F77DECLARE(ca_fpthgr12)(sz, tp[0], ttgcelcius[0], thg[0],
                           iel, ivstat, terrc[0]) ;
         convert_eos_error(terrc,err_tmp) ;
         err.set_worst_error(err_tmp) ;
       }

    // -- -- ftgr12
    if (comp_tgr12 == 1)
       { F77DECLARE(ca_ftgr12)(sz, tp[0], thg[0], ttsp[0], trvsp1[0], 
                         ttg[0], ttgpv[0], ttghv[0],
                         trv[0], trvpv[0], trvhv[0], 
                         iel, ivstat, terrc[0]) ;
         convert_eos_error(terrc,err_tmp) ;
         err.set_worst_error(err_tmp) ;
       }

    // -- -- fcpvr12
    if (comp_cpvr12 == 1)
       { F77DECLARE(ca_fcpvr12) (sz, ttg[0], ttgpv[0], ttghv[0], 
                           trv[0], trvpv[0], trvhv[0], 
                           tcpv[0], tcpvpv[0], tcpvhv[0]) ;
       } 

    // -- -- fmuvr12
    if (comp_muvr12 == 1)
       { F77DECLARE(ca_fmuvr12)(sz, tp[0], ttg[0], ttgpv[0], ttghv[0],
                          ttmuv[0], tmuvpv[0], tmuvhv[0]) ;
       }

    // -- -- fconvr12
    if (comp_convr12 == 1)
       { F77DECLARE(ca_fconvr12)(sz, ttg[0], ttgpv[0], ttghv[0], 
                           ttlav[0], tlavpv[0], tlavhv[0]) ;
       }

    // -- -- others
    // -- -- (c)
    if (comp_c == 1)
       { double cc;
         double m05=-0.5;
         for(int i=0; i<sz; i++)
            { cc = trvpv[i] + trvhv[i]/trv[i] ;
              tc[i] = pow(cc,m05);
            }
       }
    // -- -- (all not defined)
    for (int i=0; i<ind_not_def+1; i++)
       { the_fluid->EOS_Fluid::compute(input[0], input[1], r[not_def[i]], err_tmp) ;
         err.set_worst_error(err_tmp) ; 
       }

    // -- change unit if needed
    if (chg_T_sat == 1)  ttsp += tabsk ;
    if (chg_T     == 1)  ttg  += tabsk ; 
 
    // -- end
    return err.find_worst_error().generic_error() ;
  }


  // verify for EOS_Field
  inline EOS_Error CATHARE_FreonR12::verify(const EOS_Field& input, EOS_Error_Field& err, int dom) const
  { EOS_Fields inputs(1) ;
    inputs[0] = input ;
    return verify(inputs, err, dom) ;
  }


  // verify for EOS_Fields
  inline EOS_Error CATHARE_FreonR12::verify(const EOS_Fields& input, EOS_Error_Field& err, int dom) const
  {
    // -- init
    err=EOS_Internal_Error::OK ;
    double p_max,p_min,T_max,T_min,h_max,h_min ;
    switch(dom)
      { // bounds : liquid + vapor
        case 0 :
           p_max = xppr12;
           p_min = xpmr12;
           h_max = xhlpr12;
           h_min = xhlmr12;
           T_max = xtlpr12+tabsk;
           T_min = xtlmr12+tabsk;
           break;

        // bounds : liquid
        case 1 :
           p_max = xppr12;
           p_min = xpmr12;
           h_max = xhlpr12;
           h_min = xhlmr12;
           T_max = xtlpr12+tabsk;
           T_min = xtlmr12+tabsk;
           break;

        // bounds : vapor
        case 2 :
           p_max = xppr12;
           p_min = xpmr12;
           h_max = xhvpr12;
           h_min = xhvmr12;
           T_max = xtgpr12+tabsk;
           T_min = xtgmr12+tabsk;  
           break;

        default :
           err = EOS_Internal_Error::NOT_IMPLEMENTED ;
           return EOS_Error::error ;
           break ;
      }

    int nb_fields = input.size() ;
    int sz ;
    EOS_thermprop prop ;
    for (int i=0; i<nb_fields; i++)
       { const NEPTUNE::EOS_Field& field_i=input[i] ;
         prop = field_i.get_property() ;
         const ArrOfDouble& values_i = field_i.get_data() ;
         switch(prop)
            { case NEPTUNE::p :
                 sz = values_i.size() ;
                 for (int j=0; j<sz; j++)
                    { if (values_i[j] < p_min)  err.set(j, EOS_Internal_Error(930, EOS_Error::bad)) ;
                      if (values_i[j] > p_max)  err.set(j, EOS_Internal_Error(931, EOS_Error::bad)) ;
                    }
                 break ;
                 
              case NEPTUNE::h :
                 sz = values_i.size() ;
                 for (int j=0; j<sz; j++)
                    { if (values_i[j] < h_min)  err.set(j, EOS_Internal_Error(920, EOS_Error::bad)) ;
                      if (values_i[j] > h_max)  err.set(j, EOS_Internal_Error(921, EOS_Error::bad)) ;
                    }
                 break ;
                 
              case NEPTUNE::T :
                 sz = values_i.size() ;
                 for (int j=0; j<sz; j++)
                    { if (values_i[j] < T_min)  err.set(j, EOS_Internal_Error(910, EOS_Error::bad)) ;
                      if (values_i[j] > T_max)  err.set(j, EOS_Internal_Error(911, EOS_Error::bad)) ;
                    }
                 break ;
                 
              default :
                 err = EOS_Internal_Error::NOT_IMPLEMENTED ;
                 break ;
            }
       }
    return err.find_worst_error().generic_error() ;
  };

  inline EOS_Internal_Error CATHARE_FreonR12::convert_eos_error(const int partial_code) const 
  { switch(partial_code)
      { case    0 : return EOS_Internal_Error::OK ;
        case 3800 : return EOS_Internal_Error(3800,         EOS_Error::bad) ;
        default   : return EOS_Internal_Error(partial_code, EOS_Error::bad) ;
      }
  };

  inline void CATHARE_FreonR12::convert_eos_error(const ArrOfInt& terrc, EOS_Error_Field& err) const
  { int sz = terrc.size() ;
    assert(err.size() == sz) ;
    EOS_Internal_Error internal_error=EOS_Internal_Error::OK ;
    for(int i=0; i<sz; i++)
       { internal_error = convert_eos_error(terrc[i]) ;
         err.set(i,internal_error.get_code()) ;
       }
  };
};
#endif
