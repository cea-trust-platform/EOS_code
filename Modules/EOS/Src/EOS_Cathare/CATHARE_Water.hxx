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



#ifndef CATHARE_WATER_HXX
#define CATHARE_WATER_HXX

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
{ void F77DECLARE(ca_fpsatte)(int& nsca, double& tp, double& ttsp, double& ttsp1, 
                           double& thlsp, double& thlsp1, double& thvsp,
                           double& thvsp1, double& tcplsp, double& tclsp1,
                           double& tcpvsp, double& tcvsp1, double& trlsp,
                           double& trlsp1, double& trvsp, double& trvsp1) ;

  void F77DECLARE(ca_fhliqr6te)(int& nsca, double& tp, double& thl, double& ttsp,
                             double& ttsp1, double& thlsp, double& thlsp1, 
                             double& tcplsp, double& tclsp1,double& trlsp,
                             double& trlsp1, double& ttl, double& ttl1, 
                             double& ttl2, double& tcpl, double& tcpl1,
                             double& tcpl2, double& trl, double& trl1, 
                             double& trl2, double& tbetal, double& tbetal1,
                             double& tbetal2) ;

  void F77DECLARE(ca_fhliqa)(int& nsca, double& thl, double& ttl, 
                          double& ttl1, double& ttl2, double& ttlal, double& ttlal1,
                          double& ttlal2, double& ttmul, double& ttmul1,
                          double& ttmul2) ;

  void F77DECLARE(ca_hlfptl)(int& nsca, double& tp, double& ttl, double& thl) ;

  void F77DECLARE(ca_fhvappte)(int& nsca, double& tp, double& thg, double& ttsp,
                            double& ttsp1, double& thvsp, double& thvsp1, 
                            double& tcpvsp, double& tcvsp1, double& trvsp,
                            double& trvsp1, double& ttg, double& ttgpv, 
                            double& ttghv, double& tcpv, double& tcpvpv,
                            double& tcpvhv, double& trv, double& trvpv,
                            double& trvhv, double& ttgk, double& thvmhs) ;

  void F77DECLARE(ca_fhvapate)(int& nsca, double& tp, double& ttg, double& ttspv,
                            double& ttspvv, double& ttlav, double& tlavpv,
                            double& tlavtg, double& ttmuv, double& tmuvpv, 
                            double& tmuvtg, double& tsi, double& tsipv) ;

  void F77DECLARE(ca_fhbasa)(int& nsca, double& ttg1, double& ttg3, double& tlavpv,
                          double& tlavtg, double& tmuvpv, double& tmuvtg,
                          double& ttlag1, double& ttlag3, double& ttmug1,
                          double& ttmug3) ;

  void F77DECLARE(ca_hgfptg)(int& nsca, double& tp, double& ttg, double& thg, int& terrc) ;

  void F77DECLARE(ca_fhmix)(int& nsca, int& nincon, int& nincx, 
                         double& tprxr, double& tprxcp, 
                         double& tp, double& thg, double& tx_1, double& tx_2, double& tx_3, double& tx_4,
                         double& ttsp, double& ttsp1, double& thlsp, double& thlsp1, double& thvsp, double& thvsp1,
                         double& tcplsp, double& tclsp1, double& tcpvsp, double& tcvsp1, double& trlsp, double& trlsp1, double& trvsp, double& trvsp1,
                         double& txvap, double& txrsu, double& txcpsu, double& txcpsutg, double& thi7su, double& tvalp, double& tprgr, 
                         double& tpv, double& thv, double& ttspv, double& ttspvv, double& t2tsdpvv, 
                         double& thlsv, double& thlsvv, double& thvsv, double& thvsvv,
                         double& ttg, double& ttgpv, double& ttghv, double& tcpv, double& tcpvpv, double& tcpvhv, 
                         double& trv, double& trvpv, double& trvhv, double& ttgk, double& thvmhs,
                         double& tpv1, double& tpv3, double& thv1, double& thv3, 
                         double& ttg1, double& ttg3, double& trv1, double& trv3, 
                         double& trg, double& trg1, double& trg3,
                         double& tfdpv, double& tfdhv, double& tgdpv, double& tgdhv, 
                         double& tpvx_1, double& thvx_1, double& ttgx_1, double& trvx_1, double& trgx_1, double& thx_1, double& trx_1, 
                         double& tpvx_2, double& thvx_2, double& ttgx_2, double& trvx_2, double& trgx_2, double& thx_2, double& trx_2,
                         double& tpvx_3, double& thvx_3, double& ttgx_3, double& trvx_3, double& trgx_3, double& thx_3, double& trx_3, 
                         double& tpvx_4, double& thvx_4, double& ttgx_4, double& trvx_4, double& trgx_4, double& thx_4, double& trx_4) ;

  void F77DECLARE(ca_hgfptgmix)(int& nsca, int& nincon, int& nincx, 
                             double& tprxr, double& tprxcp,
                             double& tp, double& thg, double& tx_1, double& tx_2, double& tx_3, double& tx_4,
                             double& ttsp, double& ttsp1, double& thlsp, double& thlsp1, double& thvsp, double& thvsp1, 
                             double& tcplsp, double& tclsp1, double& tcpvsp, double& tcvsp1, double& trlsp, double& trlsp1, double& trvsp, double& trvsp1,
                             double& txvap, double& txrsu, double& txcpsu, double& txcpsutg, double& thi7su, double& tvalp, double& tprgr, 
                             double& tpv, double& thv, double& ttspv, double& ttspvv, double& t2tsdpvv, 
                             double& thlsv, double& thlsvv, double& thvsv, double& thvsvv,
                             double& ttg, double& ttgpv, double& ttghv, double& tcpv, double& tcpvpv, double& tcpvhv, 
                             double& trv, double& trvpv, double& trvhv, double& ttgk, double& thvmhs,
                             double& tpv1, double& tpv3, double& thv1, double& thv3,
                             double& ttg1, double& trv1, double& trv3,
                             double& trg, double& trg1, double& trg3, 
                             double& tfdpv, double& tfdhv, double& tgdpv, double& tgdhv,
                             double& tpvx_1, double& thvx_1, double& ttgx_1, double& trvx_1, double& trgx_1, double& thx_1, double& trx_1,
                             double& tpvx_2, double& thvx_2, double& ttgx_2, double& trvx_2, double& trgx_2, double& thx_2, double& trx_2,
                             double& tpvx_3, double& thvx_3, double& ttgx_3, double& trvx_3, double& trgx_3, double& thx_3, double& trx_3, 
                             double& tpvx_4, double& thvx_4, double& ttgx_4, double& trvx_4, double& trgx_4, double& thx_4, double& trx_4) ;

  void F77DECLARE(ca_fhgasa)(int& sz, double& txvap, double& tcpv, double& ttlav,
                          double& ttmuv, double& tlavtg, double& tmuvtg,
                          double& tcpg, double& ttlag, double& ttmug,
                          double& tlagtg, double& tmugtg) ;

  void F77DECLARE(ca_fhgaxa)(int& sz, double& tx, double& ttgk, double& tprxr,
                          double& tprxcp, double& trprxl0, double& tprxl1,
                          double& tprxl2, double& tprxm0, double& tprxm1,
                          double& tprxm2, double& tcpg, double& ttlag,
                          double& ttmug, double& tlagtg, double& tmugtg) ;


  void F77DECLARE(ca_fhdiva)(int& sz, double& tpv1, double& tpv3, double& thv1,
                          double& thv3, double& ttg1, double& ttg3, double& txvap,
                          double& tprgr, double& tcpvpv, double& tcpvhv,
                          double& tsipv, double& tlavpv, double& tlagtg,
                          double& tmuvpv, double& tmugtg,
                          double& tcpg1, double& tcpg3, double& ttlag, 
                          double& ttlag1, double& ttlag3, double& ttmug,
                          double& ttmug1, double& ttmug3, double& tsi1,
                          double& tsi3) ;

  void F77DECLARE(ca_fhdixa)(int& sz, double& ttgk, double& tpvx,
                          double& thvx, double& ttgx, double& tcpv, double& tcpvpv,
                          double& tcpvhv, double& ttlav, double& tlavpv,
                          double& tlagtg, double& ttmuv, double& tmuvpv,
                          double& tmugtg, double& tsipv, double& txvap,
                          double& tprgr, double& tprxr, double& tprxcp,
                          double& tprxl0, double& tprxl1, double& tprxl2,
                          double& tprxm0, double& tprxm1, double& tprxm2,
                          double& tcpgx, double& ttlag,
                          double& ttlagx, double& ttmug, double& ttmugx,
                          double& tsix) ;
}

namespace NEPTUNE
{ class EOS_Fluid ;
};

namespace NEPTUNE_EOS
{ class EOS_Cathare_Water ;
  class EOS_Cathare_Steam ;
  class EOS_Cathare_LiquidWater ;
  class EOS_Mixing ; // provisoire
};

namespace CATHARE
{
  //! Encapsulation of the CATHARE functions for water.
  class CATHARE_Water
  { public :
      friend class NEPTUNE_EOS::EOS_Cathare_Water ;
      friend class NEPTUNE_EOS::EOS_Cathare_LiquidWater ;
      friend class NEPTUNE_EOS::EOS_Cathare_Steam ;
      friend class NEPTUNE_EOS::EOS_Mixing ; // provisoire

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
      CATHARE_Water(NEPTUNE::EOS_Fluid* ) ;
      CATHARE_Water() ;
      const double tabsk ;
      const double xtlm,xtlp,xtgm,xtgp ;
      const double xpmte,xppte ;
      const double xhlmte,xhlpte,xhvmte,xhvpte ;

      EOS_Error calca_all_saturation(const NEPTUNE::EOS_Field& P, 
                                       EOS_Fields& r,
                                       EOS_Error_Field& err) const ;
      EOS_Error calca_all_liquid(const EOS_Fields& input, 
                                   EOS_Fields& r,
                                   EOS_Error_Field& err) const ; 
      EOS_Error calca_all_steam(const EOS_Fields& input, 
                                  EOS_Fields& r,
                                  EOS_Error_Field& err) const ;
      //EOS_Error calca_all_mixing(const EOS_Fields& input, 
      //                           EOS_Fields& r,
      //                           EOS_Error_Field& err) const;
      EOS_Error calca_all_mixing(const EOS_Fields& input, 
                                   EOS_Fields& r,
                                   EOS_Error_Field& err,
                                   ArrOfDouble& tprxr,
                                   ArrOfDouble& tprxcp,
                                   ArrOfDouble& tprxl0,
                                   ArrOfDouble& tprxl1,
                                   ArrOfDouble& tprxl2,
                                   ArrOfDouble& tprxm0,
                                   ArrOfDouble& tprxm1,
                                   ArrOfDouble& tprxm2
                                   ) const ;
      EOS_Error verify(const EOS_Fields& input, 
                       EOS_Error_Field& err,
                       int dom) const ;
      EOS_Error verify(const EOS_Field& input, 
                       EOS_Error_Field& err,
                       int dom) const ;
      //! Convert partial error code to EOS internal code
      EOS_Internal_Error convert_eos_error(const int partial_code) const ;
      //! Convert ArrOfInt of partial error code to EOS_Error_Field
      void convert_eos_error(const ArrOfInt& terrc, EOS_Error_Field& err) const ;
      NEPTUNE::EOS_Fluid* the_fluid ;
  };

  //
  inline double CATHARE_Water::gl_Pmin() const
  { return xpmte ;
  }
  inline double CATHARE_Water::gl_hmin() const
  { return xhlmte ;
  }
  inline double CATHARE_Water::gl_Tmin() const
  { return xtlm + tabsk ;
  }

  //
  inline double CATHARE_Water::gv_Pmin() const
  { return xpmte ;
  }
  inline double CATHARE_Water::gv_hmin() const
  { return xhvmte ;
  }
  inline double CATHARE_Water::gv_Tmin() const
  { return xtgm + tabsk ;
  }

  //
  inline double CATHARE_Water::gl_Pmax() const
  { return xppte ;
  }
  inline double CATHARE_Water::gl_hmax() const
  { return xhlpte ;
  }
  inline double CATHARE_Water::gl_Tmax() const
  { return xtlp + tabsk ;
  }

  //
  inline double CATHARE_Water::gv_Pmax() const
  { return xppte ;
  }
  inline double CATHARE_Water::gv_hmax() const
  { return xhvpte ;
  }
  inline double CATHARE_Water::gv_Tmax() const
  { return xtgp + tabsk ;
  }


  // calca_all_mixing
  inline EOS_Error CATHARE_Water::calca_all_mixing (const EOS_Fields& input,
            EOS_Fields& r, EOS_Error_Field& err, ArrOfDouble& tprxr,
            ArrOfDouble& tprxcp, ArrOfDouble& tprxl0,
            ArrOfDouble& tprxl1, ArrOfDouble& tprxl2, ArrOfDouble& tprxm0, 
            ArrOfDouble& tprxm1, ArrOfDouble& tprxm2) const
  { // -- init
    int nincx = 4 ;

    // -- -- (HVPH.H)
    //
    ArrOfDouble tp(1)  ; 
    ArrOfDouble thg(1) ;
    //
    ArrOfDouble tx_0(1) ; 
    ArrOfDouble tx_1(1) ; 
    ArrOfDouble tx_2(1) ; 
    ArrOfDouble tx_3(1) ; 
    ArrOfDouble tx_4(1) ;
    //
    // -- -- (FEAUH.H)
    //
    ArrOfDouble ttg(1)   ;
    ArrOfDouble ttg1(1)  ; 
    ArrOfDouble ttg3(1)  ;  
    ArrOfDouble tcpg(1)  ; 
    ArrOfDouble tcpg1(1) ;  
    ArrOfDouble tcpg3(1) ; 
    ArrOfDouble trg(1)   ;
    ArrOfDouble trg1(1)  ;  
    ArrOfDouble trg3(1)  ;  
    ArrOfDouble tpv1(1)  ;  
    ArrOfDouble tpv3(1)  ;  
    ArrOfDouble thv1(1)  ;  
    ArrOfDouble thv3(1)  ;  
    ArrOfDouble txvap(1) ; 
    ArrOfDouble ttgk(1)  ;  
    //
    ArrOfDouble tpv(1)    ;
    ArrOfDouble thv(1)    ; 
    ArrOfDouble trv(1)    ;  
    ArrOfDouble trv1(1)   ;  
    ArrOfDouble trv3(1)   ;  
    ArrOfDouble ttgpv(1)  ; 
    ArrOfDouble ttghv(1)  ; 
    ArrOfDouble trvpv(1)  ;  
    ArrOfDouble trvhv(1)  ; 
    ArrOfDouble tcpv(1)   ;  
    ArrOfDouble tcpvpv(1) ; 
    ArrOfDouble tcpvhv(1) ; 
    //
    ArrOfDouble ttgx_1(1)  ;
    ArrOfDouble ttgx_2(1)  ; 
    ArrOfDouble ttgx_3(1)  ; 
    ArrOfDouble ttgx_4(1)  ; 
    ArrOfDouble tcpgx_1(1) ;
    ArrOfDouble tcpgx_2(1) ;
    ArrOfDouble tcpgx_3(1) ;
    ArrOfDouble tcpgx_4(1) ;
    ArrOfDouble trgx_1(1)  ;
    ArrOfDouble trgx_2(1)  ;
    ArrOfDouble trgx_3(1)  ;
    ArrOfDouble trgx_4(1)  ;
    ArrOfDouble tpvx_1(1)  ; 
    ArrOfDouble tpvx_2(1)  ; 
    ArrOfDouble tpvx_3(1)  ; 
    ArrOfDouble tpvx_4(1)  ; 
    ArrOfDouble thvx_1(1)  ; 
    ArrOfDouble thvx_2(1)  ; 
    ArrOfDouble thvx_3(1)  ; 
    ArrOfDouble thvx_4(1)  ; 
    ArrOfDouble trx_1(1)   ; 
    ArrOfDouble trx_2(1)   ; 
    ArrOfDouble trx_3(1)   ; 
    ArrOfDouble trx_4(1)   ; 
    ArrOfDouble thx_1(1)   ;  
    ArrOfDouble thx_2(1)   ;  
    ArrOfDouble thx_3(1)   ;  
    ArrOfDouble thx_4(1)   ;  
    //
    ArrOfDouble trvx_1(1) ; 
    ArrOfDouble trvx_2(1) ; 
    ArrOfDouble trvx_3(1) ; 
    ArrOfDouble trvx_4(1) ; 
    //
    // -- -- (FSATH.H)
    //
    ArrOfDouble ttsp(1)     ;   
    ArrOfDouble ttsp1(1)    ;  
    ArrOfDouble thlsp(1)    ;  
    ArrOfDouble thlsp1(1)   ;
    ArrOfDouble tcplsp(1)   ;  
    ArrOfDouble tclsp1(1)   ; 
    ArrOfDouble trlsp(1)    ; 
    ArrOfDouble trlsp1(1)   ; 
    ArrOfDouble thvsp(1)    ;   
    ArrOfDouble thvsp1(1)   ; 
    ArrOfDouble tcpvsp(1)   ;  
    ArrOfDouble tcvsp1(1)   ; 
    ArrOfDouble trvsp(1)    ;   
    ArrOfDouble trvsp1(1)   ; 
    ArrOfDouble tsi(1)      ;    
    ArrOfDouble tsi1(1)     ;    
    ArrOfDouble tsi3(1)     ;   
    ArrOfDouble t2tsdpvv(1) ;
    //
    ArrOfDouble ttspv(1)  ;  
    ArrOfDouble ttspvv(1) ;  
    ArrOfDouble thlsv(1)  ;  
    ArrOfDouble thlsvv(1) ;  
    ArrOfDouble thvsv(1)  ;  
    ArrOfDouble thvsvv(1) ;  
    ArrOfDouble tsipv(1)  ;  
    //
    ArrOfDouble tsix_1(1) ;
    ArrOfDouble tsix_2(1) ;
    ArrOfDouble tsix_3(1) ;
    ArrOfDouble tsix_4(1) ;
    //
    // -- -- (FTRANH.H)
    //
    ArrOfDouble ttlag(1)  ; 
    ArrOfDouble ttlag1(1) ; 
    ArrOfDouble ttlag3(1) ; 
    ArrOfDouble ttmug(1)  ;  
    ArrOfDouble ttmug1(1) ;
    ArrOfDouble ttmug3(1) ; 
    //
    ArrOfDouble ttlav(1)  ; 
    ArrOfDouble ttmuv(1)  ;  
    //
    ArrOfDouble ttlagx_1(1) ;
    ArrOfDouble ttlagx_2(1) ;
    ArrOfDouble ttlagx_3(1) ;
    ArrOfDouble ttlagx_4(1) ;
    ArrOfDouble ttmugx_1(1) ;
    ArrOfDouble ttmugx_2(1) ;
    ArrOfDouble ttmugx_3(1) ;
    ArrOfDouble ttmugx_4(1) ;
    //
    // -- -- (FEAUWH.H)
    //
    ArrOfDouble tlavpv(1)   ;   
    ArrOfDouble tmuvpv(1)   ;  
    ArrOfDouble tlagtg(1)   ;   
    ArrOfDouble tmugtg(1)   ;   
    ArrOfDouble txrsu(1)    ;   
    ArrOfDouble txcpsu(1)   ;   
    ArrOfDouble txcpsutg(1) ; 
    ArrOfDouble thi7su(1)   ;  
    ArrOfDouble tprgr(1)    ;    
    ArrOfDouble thvmhs(1)   ;  
    ArrOfDouble tvalp(1)    ;   
    ArrOfDouble tfdpv(1)    ;  
    ArrOfDouble tfdhv(1)    ;  
    ArrOfDouble tgdpv(1)    ;  
    ArrOfDouble tgdhv(1)    ;  
    //
    ArrOfDouble tlavtg(1)   ;
    ArrOfDouble tmuvtg(1)   ;

    // -- ArrOfInt for error
    int sz = input[0].size() ;
    ArrOfInt terrc(sz) ;
    ArrOfInt internal_err_tmp(sz) ;

    // -- -- others
    // ArrOfDouble tc ;

    // -- 
    EOS_Error_Field err_tmp(internal_err_tmp) ;
    EOS_Property  prop ;
    int comp_h = 0 ;
    //int comp_c=0;
    //int comp_sat=0;
    int comp_mix  = 1 ;
    int chg_T_sat = 0 ;
    //int comp_fhvap=0;
    int chg_T = 0 ;
    int comp_fhvapa = 0 ;
    int comp_fhgasa = 0 ;
    //int comp_fhbasa=0;
    int comp_fhdixa = 0 ;
    int comp_fhdiva = 0 ;
    int comp_fhgaxa = 0 ;

    // -- init for error 
    terrc = 0 ;
    err = EOS_Internal_Error::OK ;
    err_tmp = EOS_Internal_Error::OK ;

    // -- -- input
    ArrOfDouble tintg ;
    // int v1 = 0 ;
    // int v2 = 0 ;
    int nb_fields_in = input.size() ;
    int nincon = 0 ;
    for (int i=0; i<nb_fields_in; i++)
       { prop = input[i].get_property_number();
         switch(prop)
            { case NEPTUNE::p :
                 tp.set_ptr(sz, input[i].get_data().get_ptr());
                 // v1 = 1 ;
                 break;
              case NEPTUNE::h :
                 thg.set_ptr(sz, input[i].get_data().get_ptr());
                 // v2 = 1 ;
                 break;
              case NEPTUNE::T :
                 tintg.set_ptr(sz, input[i].get_data().get_ptr());
                 // v2 = 1 ;
                 comp_h = 1 ;
                 break;

              case NEPTUNE::c_0 :
                 tx_0.set_ptr(sz, input[i].get_data().get_ptr());
                 nincon += 1 ;
                 break;
              case NEPTUNE::c_1 :
                 tx_1.set_ptr(sz, input[i].get_data().get_ptr());
                 nincon += 1 ;
                 break;
              case NEPTUNE::c_2 :
                 tx_2.set_ptr(sz, input[i].get_data().get_ptr());
                 nincon += 1 ;
                 break;
              case NEPTUNE::c_3 :
                 tx_3.set_ptr(sz, input[i].get_data().get_ptr());
                 nincon += 1 ;
                 break;
              case NEPTUNE::c_4 :
                 tx_4.set_ptr(sz, input[i].get_data().get_ptr());
                 nincon += 1 ;
                 break;                  
              default :
                 err = EOS_Internal_Error::NOT_IMPLEMENTED;
                 return  EOS_Error::error ;
                 break;
            }
       }

    // -- -- check input
    nincon -= 1 ;
    /*if(nb_fields_in!=nb_fluids || v1*v2!=1)
      {
      cerr << "Sorry not yet implemented!" << endl;
      assert(0);
      exit(-1);
      return EOS_Error::error;
      }*/

    // -- -- output
    int nb_fields = r.size() ;
    ArrOfInt not_def(nb_fields) ;
    int ind_not_def = -1 ;

    for (int i=0; i<nb_fields; i++)
       { prop = r[i].get_property_number() ;
         switch(prop)
            { case NEPTUNE::h :
                 thg.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_h = 1 ;
                 break;
              case NEPTUNE::T :
                 ttg.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_mix = 1 ;
                 chg_T = 1 ;
                 break;
              case NEPTUNE::rho :
                 trg.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_mix = 1 ;
                 break;
              case NEPTUNE::cp :
                 tcpg.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_fhgasa = 1 ;
                 break;
              case NEPTUNE::lambda :
                 ttlag.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_fhdiva = 1 ;
                 break;
              case NEPTUNE::mu :
                 ttmug.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_fhdiva = 1 ;
                 break;
              case NEPTUNE::sigma :
                 tsi.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_fhvapa = 1 ;
                 break;
              // Derivatives
              case NEPTUNE::d_T_d_p_h :
                 ttg1.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_mix = 1 ;
                 break;
              case NEPTUNE::d_T_d_h_p :
                 ttg3.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_mix = 1 ;
                 break;
              case NEPTUNE::d_rho_d_p_h :
                 trg1.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_mix = 1 ;
                 break;
              case NEPTUNE::d_rho_d_h_p :
                 trg3.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_mix = 1 ;
                 break;
              case NEPTUNE::d_cp_d_p_h :
                 tcpg1.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_fhdiva = 1 ;
                 break;
              case NEPTUNE::d_cp_d_h_p :
                 tcpg3.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_fhdiva = 1 ;
                 break;
              case NEPTUNE::d_lambda_d_p_h :
                 ttlag1.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_fhdiva = 1 ;
                 break;
              case NEPTUNE::d_lambda_d_h_p :
                 ttlag3.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_fhdiva = 1 ;
                 break;
              case NEPTUNE::d_mu_d_p_h :
                 ttmug1.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_fhdiva = 1 ;
                 break ;
              case NEPTUNE::d_mu_d_h_p :
                 ttmug3.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_fhdiva = 1 ;
                 break ;
              case NEPTUNE::T_sat :
                 ttsp.set_ptr(sz, r[i].get_data().get_ptr());
                 chg_T_sat = 1 ;
                 comp_mix = 1 ;
                 break;
              case NEPTUNE::h_v_sat :
                 thvsp.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_mix = 1 ;
                 break;
              case NEPTUNE::rho_v_sat :
                 trvsp.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_mix = 1 ;
                 break;
              case NEPTUNE::h_l_sat :
                 thlsp.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_mix = 1 ;
                 break;
              case NEPTUNE::rho_l_sat :
                 trlsp.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_mix = 1 ;
                 break;
              // First Derivatives at Saturation
              case NEPTUNE::d_T_sat_d_p :
                 ttsp1.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_mix = 1 ;
                 break;
              case NEPTUNE::d_h_v_sat_d_p :
                 thvsp1.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_mix = 1 ;
                 break;
              case NEPTUNE::d_rho_v_sat_d_p :
                 trvsp1.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_mix = 1 ;
                 break;
              case NEPTUNE::d_h_l_sat_d_p :
                 thlsp1.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_mix = 1 ;
                 break;
              case NEPTUNE::d_rho_l_sat_d_p :
                 trlsp1.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_mix = 1 ;
                 break;
              case NEPTUNE::d_T_d_c_1_ph :
                 ttgx_1.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_mix = 1 ;
                 break;
              case NEPTUNE::d_T_d_c_2_ph :
                 ttgx_2.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_mix = 1 ;
                 break;
              case NEPTUNE::d_T_d_c_3_ph :
                 ttgx_3.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_mix = 1 ;
                 break;
              case NEPTUNE::d_T_d_c_4_ph :
                 ttgx_4.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_mix = 1 ;
                 break;
              case NEPTUNE::d_rho_d_c_1_ph :
                 trgx_1.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_mix = 1 ;
                 break;
              case NEPTUNE::d_rho_d_c_2_ph :
                 trgx_2.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_mix = 1 ;
                 break;
              case NEPTUNE::d_rho_d_c_3_ph :
                 trgx_3.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_mix = 1 ;
                 break;
              case NEPTUNE::d_rho_d_c_4_ph :
                 trgx_4.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_mix = 1 ;
                 break;
              case NEPTUNE::d_cp_d_c_1_ph :
                 tcpgx_1.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_fhdixa = 1 ;
                 break;
              case NEPTUNE::d_cp_d_c_2_ph :
                 tcpgx_2.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_fhdixa = 1 ;
                 break;
              case NEPTUNE::d_cp_d_c_3_ph :
                 tcpgx_3.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_fhdixa = 1 ;
                 break;
              case NEPTUNE::d_cp_d_c_4_ph :
                 tcpgx_4.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_fhdixa = 1 ;
                 break;
              case NEPTUNE::d_mu_d_c_1_ph :
                 ttmugx_1.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_fhdixa = 1 ;
                 break;
              case NEPTUNE::d_mu_d_c_2_ph :
                 ttmugx_2.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_fhdixa = 1 ;
                 break;
              case NEPTUNE::d_mu_d_c_3_ph :
                 ttmugx_3.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_fhdixa = 1 ;
                 break;
              case NEPTUNE::d_mu_d_c_4_ph :
                 ttmugx_4.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_fhdixa = 1 ;
                 break;
              case NEPTUNE::d_lambda_d_c_1_ph :
                 ttlagx_1.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_fhdixa = 1 ;
                 break;
              case NEPTUNE::d_lambda_d_c_2_ph :
                 ttlagx_2.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_fhdixa = 1 ;
                 break;
              case NEPTUNE::d_lambda_d_c_3_ph :
                 ttlagx_3.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_fhdixa = 1 ;
                 break;
              case NEPTUNE::d_lambda_d_c_4_ph :
                 ttlagx_4.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_fhdixa = 1 ;
                 break;
              case NEPTUNE::d_sigma_d_c_1_ph :
                 tsix_1.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_fhdixa = 1 ;
                 break;
              case NEPTUNE::d_sigma_d_c_2_ph :
                 tsix_2.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_fhdixa = 1 ;
                 break;
              case NEPTUNE::d_sigma_d_c_3_ph :
                 tsix_3.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_fhdixa = 1 ;
                 break;
              case NEPTUNE::d_sigma_d_c_4_ph :
                 tsix_4.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_fhdixa = 1 ;
                 break;
              case NEPTUNE::NotACamixProperty :
                 err = EOS_Internal_Error::NOT_IMPLEMENTED;
                 return EOS_Error::error ;
                 break ;
                 
              default :
                 ind_not_def += 1 ;
                 not_def(ind_not_def) = i ;
                 break;
            }
       }      


    // -- case nincon < 4
    if (tx_0.size() != sz) tx_0.resize(sz) ;    
    if (tx_1.size() != sz) tx_1.resize(sz) ;
    if (tx_2.size() != sz) tx_2.resize(sz) ;    
    if (tx_3.size() != sz) tx_3.resize(sz) ;    
    if (tx_4.size() != sz) tx_4.resize(sz) ;
    
    // -- compute using fortran if needed
    if (comp_fhdixa == 1) comp_fhdiva = 1 ;
    if (comp_fhdiva == 1) comp_fhgasa = 1 ;
    if (comp_fhgasa == 1) comp_fhvapa = 1 ;
    if (comp_fhgasa == 1 && nincon>0) comp_fhgaxa = 1 ;
    if (comp_fhdiva == 1 && nincon>0) comp_fhdixa = 1 ;

    // -- -- fhgfptgmix
    if (comp_h == 1)
       { ArrOfDouble ttgcelcius(sz) ;
         for (int i=0; i<sz; i++)
            ttgcelcius[i] = tintg[i] - tabsk ;
         if (thg.size()      != sz) thg.resize(sz) ;
         //
         if (ttsp.size()     != sz) ttsp.resize(sz);
         if (ttsp1.size()    != sz) ttsp1.resize(sz);
         if (thlsp.size()    != sz) thlsp.resize(sz);
         if (thlsp1.size()   != sz) thlsp1.resize(sz);
         if (thvsp.size()    != sz) thvsp.resize(sz);
         if (thvsp1.size()   != sz) thvsp1.resize(sz);
         if (tcplsp.size()   != sz) tcplsp.resize(sz);
         if (tclsp1.size()   != sz) tclsp1.resize(sz);
         if (tcpvsp.size()   != sz) tcpvsp.resize(sz);
         if (tcvsp1.size()   != sz) tcvsp1.resize(sz);
         if (trlsp.size()    != sz) trlsp.resize(sz);
         if (trlsp1.size()   != sz) trlsp1.resize(sz);
         if (trvsp.size()    != sz) trvsp.resize(sz);
         if (trvsp1.size()   != sz) trvsp1.resize(sz);
         //
         if (txvap.size()    != sz) txvap.resize(sz);  
         if (txrsu.size()    != sz) txrsu.resize(sz);
         if (txcpsu.size()   != sz) txcpsu.resize(sz);
         if (txcpsutg.size() != sz) txcpsutg.resize(sz);
         if (thi7su.size()   != sz) thi7su.resize(sz);
         if (tvalp.size()    != sz) tvalp.resize(sz);
         if (tprgr.size()    != sz) tprgr.resize(sz);  
         //
         if (tpv.size()      != sz) tpv.resize(sz);
         if (thv.size()      != sz) thv.resize(sz);
         if (ttspv.size()    != sz) ttspv.resize(sz);
         if (ttspvv.size()   != sz) ttspvv.resize(sz);
         if (t2tsdpvv.size() != sz) t2tsdpvv.resize(sz);
         if (thlsv.size()    != sz) thlsv.resize(sz);
         if (thlsvv.size()   != sz) thlsvv.resize(sz);
         if (thvsv.size()    != sz) thvsv.resize(sz);
         if (thvsvv.size()   != sz) thvsvv.resize(sz);
         //
         //if( ttg.size()==0) ttg.resize(sz);
         if (ttgpv.size()    != sz) ttgpv.resize(sz);
         if (ttghv.size()    != sz) ttghv.resize(sz);
         if (tcpv.size()     != sz) tcpv.resize(sz);
         if (tcpvpv.size()   != sz) tcpvpv.resize(sz);
         if (tcpvhv.size()   != sz) tcpvhv.resize(sz);
         if (trv.size()      != sz) trv.resize(sz);
         if (trvpv.size()    != sz) trvpv.resize(sz);
         if (trvhv.size()    != sz) trvhv.resize(sz);
         if (ttgk.size()     != sz) ttgk.resize(sz);
         if (thvmhs.size()   != sz) thvmhs.resize(sz);
         //
         if (tpv1.size()     != sz) tpv1.resize(sz);
         if (tpv3.size()     != sz) tpv3.resize(sz);
         if (thv1.size()     != sz) thv1.resize(sz);
         if (thv3.size()     != sz) thv3.resize(sz);
         if (ttg1.size()     != sz) ttg1.resize(sz);
         if (ttg3.size()     != sz) ttg3.resize(sz);
         if (trv1.size()     != sz) trv1.resize(sz);
         if (trv3.size()     != sz) trv3.resize(sz);
         if (trg.size()      != sz) trg.resize(sz);
         if (trg1.size()     != sz) trg1.resize(sz);
         if (trg3.size()     != sz) trg3.resize(sz);
         if (tfdpv.size()    != sz) tfdpv.resize(sz);
         if (tfdhv.size()    != sz) tfdhv.resize(sz);
         if (tgdpv.size()    != sz) tgdpv.resize(sz);
         if (tgdhv.size()    != sz) tgdhv.resize(sz);
         //
         if (tpvx_1.size()   != sz) tpvx_1.resize(sz);
         if (thvx_1.size()   != sz) thvx_1.resize(sz);
         if (ttgx_1.size()   != sz) ttgx_1.resize(sz);
         if (trvx_1.size()   != sz) trvx_1.resize(sz);
         if (trgx_1.size()   != sz) trgx_1.resize(sz);
         if (thx_1.size()    != sz) thx_1.resize(sz);
         if (trx_1.size()    != sz) trx_1.resize(sz);
         if (tpvx_2.size()   != sz) tpvx_2.resize(sz);
         if (thvx_2.size()   != sz) thvx_2.resize(sz);
         if (ttgx_2.size()   != sz) ttgx_2.resize(sz);
         if (trvx_2.size()   != sz) trvx_2.resize(sz);
         if (trgx_2.size()   != sz) trgx_2.resize(sz);
         if (thx_2.size()    != sz) thx_2.resize(sz);
         if (trx_2.size()    != sz) trx_2.resize(sz);
         if (tpvx_3.size()   != sz) tpvx_3.resize(sz);
         if (thvx_3.size()   != sz) thvx_3.resize(sz);
         if (ttgx_3.size()   != sz) ttgx_3.resize(sz);
         if (trvx_3.size()   != sz) trvx_3.resize(sz);
         if (trgx_3.size()   != sz) trgx_3.resize(sz);
         if (thx_3.size()    != sz) thx_3.resize(sz);
         if (trx_3.size()    != sz) trx_3.resize(sz);
         if (tpvx_4.size()   != sz) tpvx_4.resize(sz);
         if (thvx_4.size()   != sz) thvx_4.resize(sz);
         if (ttgx_4.size()   != sz) ttgx_4.resize(sz);
         if (trvx_4.size()   != sz) trvx_4.resize(sz);
         if (trgx_4.size()   != sz) trgx_4.resize(sz);
         if (thx_4.size()    != sz) thx_4.resize(sz);
         if (trx_4.size()    != sz) trx_4.resize(sz);
         F77DECLARE(ca_hgfptgmix)(sz,nincon,nincx,
                            tprxr[0],tprxcp[0],
                            tp[0],thg[0],tx_1[0],tx_2[0],tx_3[0],tx_4[0],
                            ttsp[0],ttsp1[0],thlsp[0],thlsp1[0],thvsp[0], thvsp1[0],
                            tcplsp[0],tclsp1[0],tcpvsp[0],tcvsp1[0],trlsp[0],trlsp1[0],trvsp[0],trvsp1[0],
                            txvap[0],txrsu[0],txcpsu[0],txcpsutg[0],thi7su[0],tvalp[0],tprgr[0],
                            tpv[0],thv[0],ttspv[0],ttspvv[0],t2tsdpvv[0],
                            thlsv[0],thlsvv[0],thvsv[0],thvsvv[0],
                            ttgcelcius[0],ttgpv[0],ttghv[0],tcpv[0],tcpvpv[0],tcpvhv[0],
                            trv[0],trvpv[0],trvhv[0],ttgk[0],thvmhs[0],
                            tpv1[0],tpv3[0],thv1[0],thv3[0],
                            ttg1[0],trv1[0],trv3[0],
                            trg[0],trg1[0],trg3[0],
                            tfdpv[0],tfdhv[0],tgdpv[0],tgdhv[0],
                            tpvx_1[0],thvx_1[0],ttgx_1[0],trvx_1[0],trgx_1[0],thx_1[0],trx_1[0],
                            tpvx_2[0],thvx_2[0],ttgx_2[0],trvx_2[0],trgx_2[0],thx_2[0],trx_2[0],
                            tpvx_3[0],thvx_3[0],ttgx_3[0],trvx_3[0],trgx_3[0],thx_3[0],trx_3[0],
                            tpvx_4[0],thvx_4[0],ttgx_4[0],trvx_4[0],trgx_4[0],thx_4[0],trx_4[0]) ;
       }

    // -- -- fhmix
    if (comp_mix == 1)
       { if (thg.size()      != sz) thg.resize(sz) ;
         if (ttsp.size()     != sz) ttsp.resize(sz);
         if (ttsp1.size()    != sz) ttsp1.resize(sz);
         if (thlsp.size()    != sz) thlsp.resize(sz);
         if (thlsp1.size()   != sz) thlsp1.resize(sz);
         if (thvsp.size()    != sz) thvsp.resize(sz);
         if (thvsp1.size()   != sz) thvsp1.resize(sz);
         if (tcplsp.size()   != sz) tcplsp.resize(sz);
         if (tclsp1.size()   != sz) tclsp1.resize(sz);
         if (tcpvsp.size()   != sz) tcpvsp.resize(sz);
         if (tcvsp1.size()   != sz) tcvsp1.resize(sz);
         if (trlsp.size()    != sz) trlsp.resize(sz);
         if (trlsp1.size()   != sz) trlsp1.resize(sz);
         if (trvsp.size()    != sz) trvsp.resize(sz);
         if (trvsp1.size()   != sz) trvsp1.resize(sz);
         //
         if (txvap.size()    != sz) txvap.resize(sz);  
         if (txrsu.size()    != sz) txrsu.resize(sz);
         if (txcpsu.size()   != sz) txcpsu.resize(sz);
         if (txcpsutg.size() != sz) txcpsutg.resize(sz);
         if (thi7su.size()   != sz) thi7su.resize(sz);
         if (tvalp.size()    != sz) tvalp.resize(sz);
         if (tprgr.size()    != sz) tprgr.resize(sz);  
         //
         if (tpv.size()      != sz) tpv.resize(sz);
         if (thv.size()      != sz) thv.resize(sz);
         if (ttspv.size()    != sz) ttspv.resize(sz);
         if (ttspvv.size()   != sz) ttspvv.resize(sz);
         if (t2tsdpvv.size() != sz) t2tsdpvv.resize(sz);
         if (thlsv.size()    != sz) thlsv.resize(sz);
         if (thlsvv.size()   != sz) thlsvv.resize(sz);
         if (thvsv.size()    != sz) thvsv.resize(sz);
         if (thvsvv.size()   != sz) thvsvv.resize(sz);
         //
         if (ttg.size()      != sz) ttg.resize(sz);
         if (ttgpv.size()    != sz) ttgpv.resize(sz);
         if (ttghv.size()    != sz) ttghv.resize(sz);
         if (tcpv.size()     != sz) tcpv.resize(sz);
         if (tcpvpv.size()   != sz) tcpvpv.resize(sz);
         if (tcpvhv.size()   != sz) tcpvhv.resize(sz);
         if (trv.size()      != sz) trv.resize(sz);
         if (trvpv.size()    != sz) trvpv.resize(sz);
         if (trvhv.size()    != sz) trvhv.resize(sz);
         if (ttgk.size()     != sz) ttgk.resize(sz);
         if (thvmhs.size()   != sz) thvmhs.resize(sz);
         //
         if (tpv1.size()     != sz) tpv1.resize(sz);
         if (tpv3.size()     != sz) tpv3.resize(sz);
         if (thv1.size()     != sz) thv1.resize(sz);
         if (thv3.size()     != sz) thv3.resize(sz);
         if (ttg1.size()     != sz) ttg1.resize(sz);
         if (ttg3.size()     != sz) ttg3.resize(sz);
         if (trv1.size()     != sz) trv1.resize(sz);
         if (trv3.size()     != sz) trv3.resize(sz);
         if (trg.size()      != sz) trg.resize(sz);
         if (trg1.size()     != sz) trg1.resize(sz);
         if (trg3.size()     != sz) trg3.resize(sz);
         if (tfdpv.size()    != sz) tfdpv.resize(sz);
         if (tfdhv.size()    != sz) tfdhv.resize(sz);
         if (tgdpv.size()    != sz) tgdpv.resize(sz);
         if (tgdhv.size()    != sz) tgdhv.resize(sz);
         //
         if (tpvx_1.size()   != sz) tpvx_1.resize(sz);
         if (thvx_1.size()   != sz) thvx_1.resize(sz);
         if (ttgx_1.size()   != sz) ttgx_1.resize(sz);
         if (trvx_1.size()   != sz) trvx_1.resize(sz);
         if (trgx_1.size()   != sz) trgx_1.resize(sz);
         if (thx_1.size()    != sz) thx_1.resize(sz);
         if (trx_1.size()    != sz) trx_1.resize(sz);
         if (tpvx_2.size()   != sz) tpvx_2.resize(sz);
         if (thvx_2.size()   != sz) thvx_2.resize(sz);
         if (ttgx_2.size()   != sz) ttgx_2.resize(sz);
         if (trvx_2.size()   != sz) trvx_2.resize(sz);
         if (trgx_2.size()   != sz) trgx_2.resize(sz);
         if (thx_2.size()    != sz) thx_2.resize(sz);
         if (trx_2.size()    != sz) trx_2.resize(sz);
         if (tpvx_3.size()   != sz) tpvx_3.resize(sz);
         if (thvx_3.size()   != sz) thvx_3.resize(sz);
         if (ttgx_3.size()   != sz) ttgx_3.resize(sz);
         if (trvx_3.size()   != sz) trvx_3.resize(sz);
         if (trgx_3.size()   != sz) trgx_3.resize(sz);
         if (thx_3.size()    != sz) thx_3.resize(sz);
         if (trx_3.size()    != sz) trx_3.resize(sz);
         if (tpvx_4.size()   != sz) tpvx_4.resize(sz);
         if (thvx_4.size()   != sz) thvx_4.resize(sz);
         if (ttgx_4.size()   != sz) ttgx_4.resize(sz);
         if (trvx_4.size()   != sz) trvx_4.resize(sz);
         if (trgx_4.size()   != sz) trgx_4.resize(sz);
         if (thx_4.size()    != sz) thx_4.resize(sz);
         if (trx_4.size()    != sz) trx_4.resize(sz);

         F77DECLARE(ca_fhmix)(sz,nincon,nincx,
                        tprxr[0],tprxcp[0],
                        tp[0],thg[0],tx_1[0],tx_2[0],tx_3[0],tx_4[0],
                        ttsp[0],ttsp1[0],thlsp[0],thlsp1[0],thvsp[0],thvsp1[0],
                        tcplsp[0],tclsp1[0],tcpvsp[0],tcvsp1[0],trlsp[0],trlsp1[0],trvsp[0],trvsp1[0],
                        txvap[0],txrsu[0],txcpsu[0],txcpsutg[0],thi7su[0],tvalp[0],tprgr[0],
                        tpv[0],thv[0],ttspv[0],ttspvv[0],t2tsdpvv[0],
                        thlsv[0],thlsvv[0],thvsv[0],thvsvv[0],
                        ttg[0],ttgpv[0],ttghv[0],tcpv[0],tcpvpv[0],tcpvhv[0],
                        trv[0],trvpv[0],trvhv[0],ttgk[0],thvmhs[0],
                        tpv1[0],tpv3[0],thv1[0],thv3[0],
                        ttg1[0],ttg3[0],trv1[0],trv3[0],
                        trg[0],trg1[0],trg3[0],
                        tfdpv[0],tfdhv[0],tgdpv[0],tgdhv[0],
                        tpvx_1[0],thvx_1[0],ttgx_1[0],trvx_1[0],trgx_1[0],thx_1[0],trx_1[0],
                        tpvx_2[0],thvx_2[0],ttgx_2[0],trvx_2[0],trgx_2[0],thx_2[0],trx_2[0],
                        tpvx_3[0],thvx_3[0],ttgx_3[0],trvx_3[0],trgx_3[0],thx_3[0],trx_3[0],
                        tpvx_4[0],thvx_4[0],ttgx_4[0],trvx_4[0],trgx_4[0],thx_4[0],trx_4[0]) ;
       }

    // -- -- fhvapate
    if (comp_fhvapa == 1)
       { if (ttlav.size()  != sz) ttlav.resize(sz) ;
         if (tlavpv.size() != sz) tlavpv.resize(sz);
         if (tlavtg.size() != sz) tlavtg.resize(sz);
         if (ttmuv.size()  != sz) ttmuv.resize(sz) ;
         if (tmuvpv.size() != sz) tmuvpv.resize(sz); 
         if (tmuvtg.size() != sz) tmuvtg.resize(sz);
         if (tsi.size()    != sz) tsi.resize(sz)   ;
         if (tsipv.size()  != sz) tsipv.resize(sz) ;

         F77DECLARE(ca_fhvapate)(sz,
                           tp[0],ttg[0],
                           ttsp[0],ttsp1[0],
                           ttlav[0],tlavpv[0],tlavtg[0],
                           ttmuv[0],tmuvpv[0],tmuvtg[0],
                           tsi[0],tsipv[0]) ;
       }

    // -- -- fhgasa
    if (comp_fhgasa == 1)
       { if (txvap.size()  != sz) txvap.resize(sz) ;
         if (tcpv.size()   != sz) tcpv.resize(sz)  ;
         if (ttlav.size()  != sz) ttlav.resize(sz) ;
         if (ttmuv.size()  != sz) ttmuv.resize(sz) ;
         if (tlavtg.size() != sz) tlavtg.resize(sz); 
         if (tmuvtg.size() != sz) tmuvtg.resize(sz);
         if (tcpg.size()   != sz) tcpg.resize(sz)  ;
         if (ttlag.size()  != sz) ttlag.resize(sz) ;
         if (ttmug.size()  != sz) ttmug.resize(sz) ;
         if (tlagtg.size() != sz) tlagtg.resize(sz);
         if (tmugtg.size() != sz) tmugtg.resize(sz);

         F77DECLARE(ca_fhgasa)(sz,txvap[0],
                         tcpv[0],ttlav[0],ttmuv[0],
                         tlavtg[0],tmuvtg[0],
                         tcpg[0],ttlag[0],ttmug[0],
                         tlagtg[0],tmugtg[0]) ;
       }

    // -- -- fhgaxa
    if (comp_fhgaxa == 1)
       { int i ;
         if (nincon >= 1)
            { i = 0 ;
              F77DECLARE(ca_fhgaxa)(sz,
                              tx_1[0],ttgk[0],
                              tprxr[i],tprxcp[i],
                              tprxl0[i],tprxl1[i],tprxl2[i],
                              tprxm0[i],tprxm1[i],tprxm2[i],
                              tcpg[0],ttlag[0],ttmug[0],
                              tlagtg[0],tmugtg[0]) ;
            }
         if (nincon >= 2)
            { i = 1 ;
              F77DECLARE(ca_fhgaxa)(sz,
                              tx_2[0],ttgk[0],
                              tprxr[i],tprxcp[i],
                              tprxl0[i],tprxl1[i],tprxl2[i],
                              tprxm0[i],tprxm1[i],tprxm2[i],
                              tcpg[0],ttlag[0],ttmug[0],
                              tlagtg[0],tmugtg[0]) ;       
            }
         if (nincon >= 3)
            { i = 2 ;
              F77DECLARE(ca_fhgaxa)(sz,
                              tx_3[0],ttgk[0],
                              tprxr[i],tprxcp[i],
                              tprxl0[i],tprxl1[i],tprxl2[i],
                              tprxm0[i],tprxm1[i],tprxm2[i],
                              tcpg[0],ttlag[0],ttmug[0],
                              tlagtg[0],tmugtg[0]) ;       
            }
         if (nincon >= 4)
            { i = 3 ;
              F77DECLARE(ca_fhgaxa)(sz,
                              tx_4[0],ttgk[0],
                              tprxr[i],tprxcp[i],
                              tprxl0[i],tprxl1[i],tprxl2[i],
                              tprxm0[i],tprxm1[i],tprxm2[i],
                              tcpg[0],ttlag[0],ttmug[0],
                              tlagtg[0],tmugtg[0]) ;       
            }
       }

    // -- -- fhdiva
    if (comp_fhdiva == 1)
       { if (tcpg1.size()  != sz) tcpg1.resize(sz) ;
         if (tcpg3.size()  != sz) tcpg3.resize(sz) ;
         if (ttlag1.size() != sz) ttlag1.resize(sz);
         if (ttlag3.size() != sz) ttlag3.resize(sz);
         if (ttmug1.size() != sz) ttmug1.resize(sz);
         if (ttmug3.size() != sz) ttmug3.resize(sz);
         if (tsi1.size()   != sz) tsi1.resize(sz) ;
         if (tsi3.size()   != sz) tsi3.resize(sz) ;

         F77DECLARE(ca_fhdiva)(sz,
                         tpv1[0],tpv3[0],thv1[0],thv3[0],
                         ttg1[0],ttg3[0],txvap[0],tprgr[0],
                         tcpvpv[0],tcpvhv[0],tsipv[0],
                         tlavpv[0],tlagtg[0],tmuvpv[0],tmugtg[0],
                         tcpg1[0],tcpg3[0],
                         ttlag[0],ttlag1[0],ttlag3[0],
                         ttmug[0],ttmug1[0],ttmug3[0],
                         tsi1[0],tsi3[0]) ;
       }

    // -- -- fhdixa
    if (comp_fhdixa == 1)
       { int i ;
         if (nincon >= 1)
            { i = 0 ;
              if (tcpgx_1.size()  != sz) tcpgx_1.resize(sz) ;
              if (ttlagx_1.size() != sz) ttlagx_1.resize(sz);
              if (ttmugx_1.size() != sz) ttmugx_1.resize(sz);
              if (tsix_1.size()   != sz) tsix_1.resize(sz)  ;

              F77DECLARE(ca_fhdixa)(sz,
                              ttgk[0],tpvx_1[0],thvx_1[0],
                              ttgx_1[0],
                              tcpv[0],tcpvpv[0],tcpvhv[0],
                              ttlav[0],tlavpv[0],tlagtg[0],
                              ttmuv[0],tmuvpv[0],tmugtg[0],
                              tsipv[0],txvap[0],tprgr[0],
                              tprxr[i],tprxcp[i],
                              tprxl0[i],tprxl1[i],tprxl2[i],
                              tprxm0[i],tprxm1[i],tprxm2[i],
                              tcpgx_1[0],
                              ttlag[0],ttlagx_1[0],
                              ttmug[0],ttmugx_1[0],tsix_1[0]) ;
            }
         if (nincon >= 2)
            { i = 1 ;
              if (tcpgx_2.size()  != sz) tcpgx_2.resize(sz) ;
              if (ttlagx_2.size() != sz) ttlagx_2.resize(sz);
              if (ttmugx_2.size() != sz) ttmugx_2.resize(sz);
              if (tsix_2.size()   != sz) tsix_2.resize(sz)  ;

              F77DECLARE(ca_fhdixa)(sz,
                              ttgk[0],tpvx_2[0],thvx_2[0],
                              ttgx_2[0],
                              tcpv[0],tcpvpv[0],tcpvhv[0],
                              ttlav[0],tlavpv[0],tlagtg[0],
                              ttmuv[0],tmuvpv[0],tmugtg[0],
                              tsipv[0],txvap[0],tprgr[0],
                              tprxr[i],tprxcp[i],
                              tprxl0[i],tprxl1[i],tprxl2[i],
                              tprxm0[i],tprxm1[i],tprxm2[i],
                              tcpgx_2[0],
                              ttlag[0],ttlagx_2[0],
                              ttmug[0],ttmugx_2[0],tsix_2[0]) ;
            }
         if (nincon >= 3)
            { i = 2 ;
              if (tcpgx_3.size()  != sz) tcpgx_3.resize(sz) ;
              if (ttlagx_3.size() != sz) ttlagx_3.resize(sz);
              if (ttmugx_3.size() != sz) ttmugx_3.resize(sz);
              if (tsix_3.size()   != sz) tsix_3.resize(sz)  ;

              F77DECLARE(ca_fhdixa)(sz,
                              ttgk[0],tpvx_3[0],thvx_3[0],
                              ttgx_3[0],
                              tcpv[0],tcpvpv[0],tcpvhv[0],
                              ttlav[0],tlavpv[0],tlagtg[0],
                              ttmuv[0],tmuvpv[0],tmugtg[0],
                              tsipv[0],txvap[0],tprgr[0],
                              tprxr[i],tprxcp[i],
                              tprxl0[i],tprxl1[i],tprxl2[i],
                              tprxm0[i],tprxm1[i],tprxm2[i],
                              tcpgx_3[0],
                              ttlag[0],ttlagx_3[0],
                              ttmug[0],ttmugx_3[0],tsix_3[0]) ;
            }
         if (nincon >= 4)
            { i = 3 ;
              if (tcpgx_4.size()  != sz) tcpgx_4.resize(sz) ;
              if (ttlagx_4.size() != sz) ttlagx_4.resize(sz);
              if (ttmugx_4.size() != sz) ttmugx_4.resize(sz);
              if (tsix_4.size()   != sz) tsix_4.resize(sz)  ;

              F77DECLARE(ca_fhdixa)(sz,
                              ttgk[0],tpvx_4[0],thvx_4[0],
                              ttgx_4[0],
                              tcpv[0],tcpvpv[0],tcpvhv[0],
                              ttlav[0],tlavpv[0],tlagtg[0],
                              ttmuv[0],tmuvpv[0],tmugtg[0],
                              tsipv[0],txvap[0],tprgr[0],
                              tprxr[i],tprxcp[i],
                              tprxl0[i],tprxl1[i],tprxl2[i],
                              tprxm0[i],tprxm1[i],tprxm2[i],
                              tcpgx_4[0],
                              ttlag[0],ttlagx_4[0],
                              ttmug[0],ttmugx_4[0],tsix_4[0]) ;
            }
       }
    // -- -- others
    // -- -- (all not defined)
    //for (int i=0;i<ind_not_def+1;i++)
    //the_fluid->EOS_Fluid::compute(input,r[not_def[i]],err);

    // -- change unit if needed
    if(chg_T_sat == 1) ttsp += tabsk ;
    if(chg_T == 1)     ttg  += tabsk ;

    // -- end
    return err.find_worst_error().generic_error() ;
  }


  // calca_all_saturation

  inline EOS_Error CATHARE_Water::calca_all_saturation (const NEPTUNE::EOS_Field& P,
            EOS_Fields& r, EOS_Error_Field& err) const
  { // -- -- fpsatte
    ArrOfDouble tp(1)     ;
    ArrOfDouble ttsp(1)   ;
    ArrOfDouble ttsp1(1)  ;
    ArrOfDouble thlsp(1)  ;
    ArrOfDouble thlsp1(1) ;
    ArrOfDouble thvsp(1)  ;
    ArrOfDouble thvsp1(1) ;
    ArrOfDouble tcplsp(1) ;
    ArrOfDouble tclsp1(1) ;
    ArrOfDouble tcpvsp(1) ;
    ArrOfDouble tcvsp1(1) ;
    ArrOfDouble trlsp(1)  ;
    ArrOfDouble trlsp1(1) ;
    ArrOfDouble trvsp(1)  ;
    ArrOfDouble trvsp1(1) ;
    
    // -- ArrOfInt for error
    int sz = P.size() ;
    ArrOfInt terrc(sz) ;
    ArrOfInt internal_err_tmp(sz) ;

    // --
    EOS_Error_Field err_tmp(internal_err_tmp) ;
    EOS_Property prop ;
    EOS_Property propsat ;
    int comp_sat  = 0 ;
    int chg_T_sat = 0 ;

    // -- init for error 
    terrc   = 0 ;
    err     = EOS_Internal_Error::OK ;
    err_tmp = EOS_Internal_Error::OK ;

    // --  input
    prop = P.get_property_number() ;
    switch(prop)
       { case NEPTUNE::p :
            tp.set_ptr(sz, P.get_data().get_ptr()) ;
            break ;
         case NEPTUNE::p_sat :
            tp.set_ptr(sz, P.get_data().get_ptr()) ;
            break ;
         default :
            err = EOS_Internal_Error::NOT_IMPLEMENTED ;
            return EOS_Error::error ;
            break ;
       }

    // -- output
    int nb_fields = r.size() ;
    ArrOfInt not_def(nb_fields) ;
    int ind_not_def = -1 ;

    for (int i=0; i<nb_fields; i++)
       { propsat = r[i].get_property_number() ;
         switch(propsat)
            { case NEPTUNE::T_sat :
                 ttsp.set_ptr(sz, r[i].get_data().get_ptr())  ;
                 comp_sat = 1 ;
                 chg_T_sat = 1 ;
                 break ;
              case NEPTUNE::h_l_sat :
                 thlsp.set_ptr(sz, r[i].get_data().get_ptr()) ;
                 comp_sat = 1 ;
                 break ;
              case NEPTUNE::h_v_sat :
                 thvsp.set_ptr(sz, r[i].get_data().get_ptr()) ;
                 comp_sat = 1 ;
                 break ;
              case NEPTUNE::rho_l_sat :
                 trlsp.set_ptr(sz, r[i].get_data().get_ptr()) ;
                 comp_sat = 1 ;
                 break ;
              case NEPTUNE::rho_v_sat :
                 trvsp.set_ptr(sz, r[i].get_data().get_ptr()) ;
                 comp_sat = 1 ;
                 break ;
              // Derivatives
              case NEPTUNE::d_T_sat_d_p :
                 ttsp1.set_ptr(sz, r[i].get_data().get_ptr()) ;
                 comp_sat = 1 ;
                 break;
              case NEPTUNE::d_h_l_sat_d_p :
                 thlsp1.set_ptr(sz, r[i].get_data().get_ptr()) ;
                 comp_sat = 1 ;
                 break;
              case NEPTUNE::d_h_v_sat_d_p :
                 thvsp1.set_ptr(sz, r[i].get_data().get_ptr()) ;
                 comp_sat = 1 ;
                 break;
              case NEPTUNE::d_rho_l_sat_d_p :
                 trlsp1.set_ptr(sz, r[i].get_data().get_ptr()) ;
                 comp_sat = 1 ;
                 break;
              case NEPTUNE::d_rho_v_sat_d_p :
                 trvsp1.set_ptr(sz, r[i].get_data().get_ptr()) ;
                 comp_sat = 1 ;
                 break;
              case NEPTUNE::NotASatProperty :
                 err = EOS_Internal_Error::NOT_IMPLEMENTED ;
                 return EOS_Error::error ;
                 break;
              default :
                 ind_not_def += 1;
                 not_def(ind_not_def) = i ;
                 break;
           }
       }

    // -- compute using fortran if needed
    // -- -- fpsatte
    if (comp_sat == 1)
       { if (ttsp.size()   != sz) ttsp.resize(sz)   ;
         if (ttsp1.size()  != sz) ttsp1.resize(sz)  ;
         if (thlsp.size()  != sz) thlsp.resize(sz)  ;
         if (thlsp1.size() != sz) thlsp1.resize(sz) ;
         if (thvsp.size()  != sz) thvsp.resize(sz)  ;
         if (thvsp1.size() != sz) thvsp1.resize(sz) ;
         if (tcplsp.size() != sz) tcplsp.resize(sz) ;
         if (tclsp1.size() != sz) tclsp1.resize(sz) ;
         if (tcpvsp.size() != sz) tcpvsp.resize(sz) ;
         if (tcvsp1.size() != sz) tcvsp1.resize(sz) ;
         if (trlsp.size()  != sz) trlsp.resize(sz)  ;
         if (trlsp1.size() != sz) trlsp1.resize(sz) ;
         if (trvsp.size()  != sz) trvsp.resize(sz)  ;
         if (trvsp1.size() != sz) trvsp1.resize(sz) ;

         F77DECLARE(ca_fpsatte)(sz,tp[0],
                          ttsp[0],ttsp1[0],
                          thlsp[0],thlsp1[0],
                          thvsp[0],thvsp1[0],
                          tcplsp[0],tclsp1[0],
                          tcpvsp[0],tcvsp1[0],
                          trlsp[0],trlsp1[0],
                          trvsp[0],trvsp1[0]) ;
       }

    // -- -- others
    // -- -- (all not defined)
    for (int i=0; i<ind_not_def+1; i++)
       { the_fluid->EOS_Fluid::compute(P, r[not_def[i]], err) ;
         err.set_worst_error(err_tmp) ;   
       }
    // -- change unit if needed
    if (chg_T_sat == 1) ttsp += tabsk ;

    // -- end
    return err.find_worst_error().generic_error() ;
  }


  // calca_all_liquid

  inline EOS_Error CATHARE_Water::calca_all_liquid (const EOS_Fields& input,
            EOS_Fields& r, EOS_Error_Field& err) const
  { // -- -- fpsatte
    ArrOfDouble tp(1)     ;
    ArrOfDouble ttsp(1)   ;
    ArrOfDouble ttsp1(1)  ;
    ArrOfDouble thlsp(1)  ;
    ArrOfDouble thlsp1(1) ;
    ArrOfDouble thvsp(1)  ;
    ArrOfDouble thvsp1(1) ;
    ArrOfDouble tcplsp(1) ;
    ArrOfDouble tclsp1(1) ;
    ArrOfDouble tcpvsp(1) ;
    ArrOfDouble tcvsp1(1) ;
    ArrOfDouble trlsp(1)  ;
    ArrOfDouble trlsp1(1) ;
    ArrOfDouble trvsp(1)  ;
    ArrOfDouble trvsp1(1) ;
    
    // -- -- fhliqr6te
    ArrOfDouble thl(1)     ;
    ArrOfDouble ttl(1)     ;
    ArrOfDouble ttl1(1)    ;
    ArrOfDouble ttl2(1)    ;
    ArrOfDouble tcpl(1)    ;
    ArrOfDouble tcpl1(1)   ;
    ArrOfDouble tcpl2(1)   ;
    ArrOfDouble trl(1)     ;
    ArrOfDouble trl1(1)    ;
    ArrOfDouble trl2(1)    ;
    ArrOfDouble tbetal(1)  ;
    ArrOfDouble tbetal1(1) ;
    ArrOfDouble tbetal2(1) ;

    // -- -- fhliqa
    ArrOfDouble ttlal(1)  ;
    ArrOfDouble ttlal1(1) ;
    ArrOfDouble ttlal2(1) ;
    ArrOfDouble ttmul(1)  ;
    ArrOfDouble ttmul1(1) ;
    ArrOfDouble ttmul2(1) ;

    // -- -- others
    ArrOfDouble tc(1) ;

    // -- ArrOfInt for error
    int sz = input[0].size();
    ArrOfInt terrc(sz) ;
    ArrOfInt internal_err_tmp(sz) ;

    // --  
    EOS_Error_Field err_tmp(internal_err_tmp) ;
    EOS_Property prop ;
    int comp_h      = 0 ;
    int comp_c      = 0 ;
    int comp_sat    = 0 ;
    int chg_T_sat   = 0 ;
    int comp_fhliq  = 0 ;
    int chg_T       = 0 ;
    int comp_fhliqa = 0 ;

     // -- init for error 
    terrc   = 0 ;
    err     = EOS_Internal_Error::OK ;
    err_tmp = EOS_Internal_Error::OK ;

   // -- -- input
    ArrOfDouble tintl ;
    int v1 = 0 ;
    int v2 = 0 ;
    int nb_fields_in = input.size() ;
    for (int i=0; i<nb_fields_in; i++)
       { prop = input[i].get_property_number() ;
         switch(prop)
            { case NEPTUNE::p :
                 tp.set_ptr(sz, input[i].get_data().get_ptr()) ;
                 v1 = 1 ;           
                 break ;
              case NEPTUNE::h :
                 thl.set_ptr(sz, input[i].get_data().get_ptr()) ;
                 v2 = 1 ;
                 break ;
              case NEPTUNE::T :
                 tintl.set_ptr(sz, input[i].get_data().get_ptr()) ;
                 v2 = 1 ;           
                 comp_h = 1 ;
                 break ;
              default :
                 err = EOS_Internal_Error::NOT_IMPLEMENTED ;
                 return EOS_Error::error ;
                 break;
            }
       }
    // -- -- check input
    
    if (nb_fields_in != 2 || v1*v2 != 1)
       { err = EOS_Internal_Error::NOT_IMPLEMENTED ;
         return EOS_Error::error ;
       }

    // -- -- output
    int nb_fields=r.size() ;
    ArrOfInt not_def(nb_fields) ;
    int ind_not_def = -1 ;

    for (int i=0; i<nb_fields; i++)
       { prop = r[i].get_property_number() ;
         switch(prop)
            { case NEPTUNE::h :
                 thl.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_h = 1 ;
                 break ;
              case NEPTUNE::T :
                 ttl.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_fhliq = 1 ;
                 chg_T      = 1 ;
                 break ;
              case NEPTUNE::rho :
                 trl.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_fhliq = 1 ;
                 break ;
              case NEPTUNE::cp :
                 tcpl.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_fhliq = 1 ;
                 break ;
              case NEPTUNE::lambda :
                 ttlal.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_fhliqa = 1 ;
                 break ;
              case NEPTUNE::mu :
                 ttmul.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_fhliqa = 1 ;
                 break ;
              case NEPTUNE::w :
                 tc.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_c = 1 ;
                 break ;
              // Derivatives
              case NEPTUNE::d_T_d_p_h :
                 ttl1.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_fhliq = 1 ;
                 break ;
              case NEPTUNE::d_T_d_h_p :
                 ttl2.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_fhliq = 1 ;
                 break ;
              case NEPTUNE::d_rho_d_p_h :
                 trl1.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_fhliq = 1 ;
                 break ;
              case NEPTUNE::d_rho_d_h_p :
                 trl2.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_fhliq = 1 ;
                 break ;
              case NEPTUNE::d_cp_d_p_h :
                 tcpl1.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_fhliq = 1 ;
                 break ;
              case NEPTUNE::d_cp_d_h_p :
                 tcpl2.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_fhliq = 1 ;
                 break ;
              case NEPTUNE::d_lambda_d_p_h :
                 ttlal1.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_fhliqa = 1 ;
                 break ;
              case NEPTUNE::d_lambda_d_h_p :
                 ttlal2.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_fhliqa = 1 ;
                 break ;
              case NEPTUNE::d_mu_d_p_h :
                 ttmul1.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_fhliqa = 1 ;
                 break ;
              case NEPTUNE::d_mu_d_h_p :
                 ttmul2.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_fhliqa = 1 ;
                 break ;
                 
              // Thermodynamic Properties at Saturation
              case NEPTUNE::T_sat :
                 ttsp.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_sat  = 1 ;
                 chg_T_sat = 1 ;
                 break ;
              case NEPTUNE::h_l_sat :
                 thlsp.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_sat = 1 ;
                 break ;
              case NEPTUNE::rho_l_sat :
                 trlsp.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_sat = 1 ;
                 break ;
              case NEPTUNE::h_v_sat :
                 thvsp.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_sat = 1 ;
                 break ;
              case NEPTUNE::rho_v_sat :
                 trvsp.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_sat = 1 ;
                 break ;
              // Derivatives
              case NEPTUNE::d_T_sat_d_p :
                 ttsp1.set_ptr(sz, r[i].get_data().get_ptr()) ;
                 comp_sat = 1 ;
                 break ;
              case NEPTUNE::d_h_l_sat_d_p :
                 thlsp1.set_ptr(sz, r[i].get_data().get_ptr()) ;
                 comp_sat = 1 ;
                 break ;
              case NEPTUNE::d_rho_l_sat_d_p :
                 trlsp1.set_ptr(sz, r[i].get_data().get_ptr()) ;
                 comp_sat = 1 ;
                 break ;
              case NEPTUNE::d_h_v_sat_d_p :
                 thvsp1.set_ptr(sz, r[i].get_data().get_ptr()) ;
                 comp_sat = 1 ;
                 break ;
              case NEPTUNE::d_rho_v_sat_d_p :
                 trvsp1.set_ptr(sz, r[i].get_data().get_ptr()) ;
                 comp_sat = 1 ;
                 break ;
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

    // -- compute using fortran if needed
    if (comp_fhliqa == 1) comp_fhliq = 1 ;
    if (comp_c == 1)      comp_fhliq = 1 ;
    if (comp_fhliq == 1)  comp_sat   = 1 ;

    // -- --hlfptl
    if (comp_h == 1)
       { if (thl.size() != sz) thl.resize(sz) ;
         ArrOfDouble ttlcelcius(sz) ;
         for (int i=0; i<sz; i++)
            ttlcelcius[i] = tintl[i]-tabsk ;
         F77DECLARE(ca_hlfptl)(sz,tp[0],ttlcelcius[0],thl[0]) ;
       }

    // -- -- fpsatte
    if (comp_sat == 1)
       { if( ttsp.size()   != sz) ttsp.resize(sz)   ;
         if( ttsp1.size()  != sz) ttsp1.resize(sz)  ;
         if( thlsp.size()  != sz) thlsp.resize(sz)  ;
         if( thlsp1.size() != sz) thlsp1.resize(sz) ;
         if( thvsp.size()  != sz) thvsp.resize(sz)  ;
         if( thvsp1.size() != sz) thvsp1.resize(sz) ;
         if( tcplsp.size() != sz) tcplsp.resize(sz) ;
         if( tclsp1.size() != sz) tclsp1.resize(sz) ;
         if( tcpvsp.size() != sz) tcpvsp.resize(sz) ;
         if( tcvsp1.size() != sz) tcvsp1.resize(sz) ;
         if( trlsp.size()  != sz) trlsp.resize(sz)  ;
         if( trlsp1.size() != sz) trlsp1.resize(sz) ;
         if( trvsp.size()  != sz) trvsp.resize(sz)  ;
         if( trvsp1.size() != sz) trvsp1.resize(sz) ;

         F77DECLARE(ca_fpsatte)(sz,tp[0],
                          ttsp[0],ttsp1[0],
                          thlsp[0],thlsp1[0],
                          thvsp[0],thvsp1[0],
                          tcplsp[0],tclsp1[0],
                          tcpvsp[0],tcvsp1[0],
                          trlsp[0],trlsp1[0],
                          trvsp[0],trvsp1[0]) ;
      }

    // -- -- fhliqr6te
    if (comp_fhliq == 1)
       { if (ttl.size()     != sz) ttl.resize(sz)     ;
         if (ttl1.size()    != sz) ttl1.resize(sz)    ;
         if (ttl2.size()    != sz) ttl2.resize(sz)    ;
         if (tcpl.size()    != sz) tcpl.resize(sz)    ;
         if (tcpl1.size()   != sz) tcpl1.resize(sz)   ;
         if (tcpl2.size()   != sz) tcpl2.resize(sz)   ;
         if (trl.size()     != sz) trl.resize(sz)     ;
         if (trl1.size()    != sz) trl1.resize(sz)    ;
         if (trl2.size()    != sz) trl2.resize(sz)    ;
         if (tbetal.size()  != sz) tbetal.resize(sz)  ;
         if (tbetal1.size() != sz) tbetal1.resize(sz) ;
         if (tbetal2.size() != sz) tbetal2.resize(sz) ;
         
         F77DECLARE(ca_fhliqr6te)(sz,tp[0],thl[0],
                            ttsp[0],ttsp1[0],
                            thlsp[0],thlsp1[0],
                            tcplsp[0],tclsp1[0],
                            trlsp[0],trlsp1[0],
                            ttl[0],ttl1[0],ttl2[0],
                            tcpl[0],tcpl1[0],tcpl2[0],
                            trl[0],trl1[0],trl2[0],
                            tbetal[0],tbetal1[0],tbetal2[0]) ;
       }

    // -- -- fhliqa
    if (comp_fhliqa == 1)
       { if (ttlal.size()  != sz) ttlal.resize(sz)  ;
         if (ttlal1.size() != sz) ttlal1.resize(sz) ;
         if (ttlal2.size() != sz) ttlal2.resize(sz) ;
         if (ttmul.size()  != sz) ttmul.resize(sz)  ;
         if (ttmul1.size() != sz) ttmul1.resize(sz) ;
         if (ttmul2.size() != sz) ttmul2.resize(sz) ;

         F77DECLARE(ca_fhliqa)(sz,thl[0],
                         ttl[0],ttl1[0],ttl2[0],
                         ttlal[0],ttlal1[0],ttlal2[0],
                         ttmul[0],ttmul1[0],ttmul2[0]) ;
       }

    // -- -- others
    // -- -- (c)
    if (comp_c == 1)
       { double cc ;
         double m05 = -0.5 ;
         for (int i=0; i<sz; i++)
            { cc = trl1[i]+trl2[i]/trl[i] ;
              tc[i] = pow(cc,m05) ;
            }
       }
    
    // -- -- (all not defined)
    for (int i=0; i<ind_not_def+1; i++)
       { the_fluid->EOS_Fluid::compute(input[0] ,input[1], r[not_def[i]], err) ;
         err.set_worst_error(err_tmp) ;   
       }

    // -- change unit if needed
    if (chg_T_sat == 1) ttsp += tabsk ;
    if (chg_T == 1)     ttl  += tabsk ;


    // -- end
    return err.find_worst_error().generic_error() ;
  }


  // calca_all_steam

  inline EOS_Error CATHARE_Water::calca_all_steam (const EOS_Fields& input, 
            EOS_Fields& r, EOS_Error_Field& err) const
  {
    // -- -- fpsatte
    ArrOfDouble tp(1)     ;
    ArrOfDouble ttsp(1)   ;
    ArrOfDouble ttsp1(1)  ;
    ArrOfDouble thlsp(1)  ;
    ArrOfDouble thlsp1(1) ;
    ArrOfDouble thvsp(1)  ;
    ArrOfDouble thvsp1(1) ;
    ArrOfDouble tcplsp(1) ;
    ArrOfDouble tclsp1(1) ;
    ArrOfDouble tcpvsp(1) ;
    ArrOfDouble tcvsp1(1) ;
    ArrOfDouble trlsp(1)  ;
    ArrOfDouble trlsp1(1) ;
    ArrOfDouble trvsp(1)  ;
    ArrOfDouble trvsp1(1) ;
    
    // -- -- fhvappte
    ArrOfDouble thg(1)    ;
    ArrOfDouble ttg(1)    ;
    ArrOfDouble ttgpv(1)  ;
    ArrOfDouble ttghv(1)  ;
    ArrOfDouble tcpv(1)   ;
    ArrOfDouble tcpvpv(1) ;
    ArrOfDouble tcpvhv(1) ;
    ArrOfDouble trv(1)    ;
    ArrOfDouble trvpv(1)  ;
    ArrOfDouble trvhv(1)  ;
    ArrOfDouble ttgk(1)   ;
    ArrOfDouble thvmhs(1) ;

    // -- -- fhvapate
    ArrOfDouble ttlav(1)  ;
    ArrOfDouble tlavpv(1) ;
    ArrOfDouble tlavtg(1) ;
    ArrOfDouble ttmuv(1)  ;
    ArrOfDouble tmuvpv(1) ; 
    ArrOfDouble tmuvtg(1) ;
    ArrOfDouble tsi(1)    ;
    ArrOfDouble tsipv(1)  ;

    // -- -- fhbasa
    ArrOfDouble ttlag1(1) ;
    ArrOfDouble ttlag3(1) ;
    ArrOfDouble ttmug1(1) ;
    ArrOfDouble ttmug3(1) ;

    // -- -- others
    ArrOfDouble tc(1) ;

    // -- ArrOfInt for error
    int sz = input[0].size() ;
    ArrOfInt terrc(sz) ;
    ArrOfInt internal_err_tmp(sz) ;

    // -- 
    EOS_Error_Field err_tmp(internal_err_tmp) ;
    EOS_Property prop ;
    int comp_h      = 0 ;
    int comp_c      = 0 ;
    int comp_sat    = 0 ;
    int chg_T_sat   = 0 ;
    int comp_fhvap  = 0 ;
    int chg_T       = 0 ;
    int comp_fhvapa = 0 ;
    int comp_fhbasa = 0 ;

    // -- init for error 
    terrc = 0 ;
    err     = EOS_Internal_Error::OK ;
    err_tmp = EOS_Internal_Error::OK ;


    // -- -- input
    ArrOfDouble tintg;
    int v1 = 0 ;
    int v2 = 0 ;
    int nb_fields_in=input.size() ;
    for (int i=0; i<nb_fields_in; i++)
       { prop = input[i].get_property_number() ;
         switch(prop)
            { case NEPTUNE::p :
                 tp.set_ptr(sz, input[i].get_data().get_ptr()) ;
                 v1 = 1 ;           
                 break ;
              case NEPTUNE::h :
                 thg.set_ptr(sz, input[i].get_data().get_ptr()) ;
                 v2 = 1 ;
                 break ;
              case NEPTUNE::T :
                 tintg.set_ptr(sz, input[i].get_data().get_ptr()) ;
                 v2 = 1 ;           
                 comp_h = 1 ;
                 break ;
              default :
                 err = EOS_Internal_Error::NOT_IMPLEMENTED ;
                 return EOS_Error::error ;
                 break ;
            }
       }
    // -- -- check input
    
    if (nb_fields_in != 2 || v1*v2 != 1)
       { err = EOS_Internal_Error::NOT_IMPLEMENTED ;
         return EOS_Error::error ;
       }

    // -- -- output
    int nb_fields = r.size() ;
    ArrOfInt not_def(nb_fields) ;
    int ind_not_def = -1 ;

    for (int i=0; i<nb_fields; i++)
       { prop=r[i].get_property_number() ;
         switch(prop)
            { case NEPTUNE::h :
                 thg.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_h = 1 ;
                 break ;
              case NEPTUNE::T :
                 ttg.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_fhvap = 1 ;
                 chg_T = 1 ;
                break ;
              case NEPTUNE::rho :
                 trv.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_fhvap = 1 ;
                 break ;
              case NEPTUNE::cp :
                 tcpv.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_fhvap = 1 ;
                 break ;
              case NEPTUNE::lambda :
                 ttlav.set_ptr(sz, r[i].get_data().get_ptr());
                 comp_fhvapa = 1 ;
                 break ;
              case NEPTUNE::mu :
                 ttmuv.set_ptr(sz, r[i].get_data().get_ptr()) ;
                 comp_fhvapa = 1 ;
                 break ;
              case NEPTUNE::w :
                 tc.set_ptr(sz, r[i].get_data().get_ptr()) ;
                 comp_c = 1 ;
                 break ;
              case NEPTUNE::sigma :
                 tsi.set_ptr(sz, r[i].get_data().get_ptr()) ;
                 comp_fhvapa = 1 ;
                 break ;
              // Derivatives
              case NEPTUNE::d_T_d_p_h :
                 ttgpv.set_ptr(sz, r[i].get_data().get_ptr()) ;
                 comp_fhvap = 1 ;
                 break;
              case NEPTUNE::d_T_d_h_p :
                 ttghv.set_ptr(sz, r[i].get_data().get_ptr()) ;
                 comp_fhvap = 1 ;
                 break;
              case NEPTUNE::d_rho_d_p_h :
                 trvpv.set_ptr(sz, r[i].get_data().get_ptr()) ;
                 comp_fhvap = 1 ;
                 break;
              case NEPTUNE::d_rho_d_h_p :
                 trvhv.set_ptr(sz, r[i].get_data().get_ptr()) ;
                 comp_fhvap = 1 ;
                 break;
              case NEPTUNE::d_cp_d_p_h :
                 tcpvpv.set_ptr(sz, r[i].get_data().get_ptr()) ;
                 comp_fhvap = 1 ;
                 break;
              case NEPTUNE::d_cp_d_h_p :
                 tcpvhv.set_ptr(sz, r[i].get_data().get_ptr()) ;
                 comp_fhvap = 1 ;
                 break;
              case NEPTUNE::d_lambda_d_p_h :
                 ttlag1.set_ptr(sz, r[i].get_data().get_ptr()) ;
                 comp_fhbasa = 1 ;
                 break;
              case NEPTUNE::d_lambda_d_h_p :
                 ttlag3.set_ptr(sz, r[i].get_data().get_ptr()) ;
                 comp_fhbasa = 1 ;
                 break;
              case NEPTUNE::d_mu_d_p_h :
                 ttmug1.set_ptr(sz, r[i].get_data().get_ptr()) ;
                 comp_fhbasa = 1 ;
                 break;
              case NEPTUNE::d_mu_d_h_p :
                 ttmug3.set_ptr(sz, r[i].get_data().get_ptr()) ;
                 comp_fhbasa = 1 ;
                 break;
              case NEPTUNE::d_sigma_d_p_h :
                 tsipv.set_ptr(sz, r[i].get_data().get_ptr()) ;
                 comp_fhvapa = 1 ;
                 break;
              // Thermodynamic Properties at Saturation
              case NEPTUNE::T_sat :
                 ttsp.set_ptr(sz, r[i].get_data().get_ptr()) ;
                 comp_sat = 1 ;
                 chg_T_sat = 1 ;
                 break ;
              case NEPTUNE::h_v_sat :
                 thvsp.set_ptr(sz, r[i].get_data().get_ptr()) ;
                 comp_sat = 1 ;
                 break ;
              case NEPTUNE::rho_v_sat :
                 trvsp.set_ptr(sz, r[i].get_data().get_ptr()) ;
                 comp_sat = 1 ;
                 break ;
              case NEPTUNE::h_l_sat :
                 thlsp.set_ptr(sz, r[i].get_data().get_ptr()) ;
                 comp_sat = 1 ;
                 break ;
              case NEPTUNE::rho_l_sat :
                 trlsp.set_ptr(sz, r[i].get_data().get_ptr()) ;
                 comp_sat = 1 ;
                 break ;
              // Derivatives
              case NEPTUNE::d_T_sat_d_p :
                 ttsp1.set_ptr(sz, r[i].get_data().get_ptr())  ;
                 comp_sat = 1 ;
                 break ;
              case NEPTUNE::d_h_v_sat_d_p :
                 thvsp1.set_ptr(sz, r[i].get_data().get_ptr()) ;
                 comp_sat = 1 ;
                 break ;
              case NEPTUNE::d_rho_v_sat_d_p :
                 trvsp1.set_ptr(sz, r[i].get_data().get_ptr()) ;
                 comp_sat = 1 ;
                 break ;
              case NEPTUNE::d_h_l_sat_d_p :
                 thlsp1.set_ptr(sz, r[i].get_data().get_ptr()) ;
                 comp_sat = 1 ;
                 break ;
              case NEPTUNE::d_rho_l_sat_d_p :
                 trlsp1.set_ptr(sz, r[i].get_data().get_ptr()) ;
                 comp_sat = 1 ;
                 break ;
              case NEPTUNE::NotASatProperty :
                 err = EOS_Internal_Error::NOT_IMPLEMENTED;
                 return EOS_Error::error ;
                 break ;
                 
              default :
                 ind_not_def += 1 ;
                 not_def(ind_not_def) = i ;
                 break ;
            }
       }      

    // -- compute using fortran if needed
    if (comp_fhbasa == 1)  comp_fhvapa = 1 ;
    if (comp_fhvapa == 1)  comp_fhvap  = 1 ;
    if (comp_c      == 1)  comp_fhvap  = 1 ;
    if (comp_fhvap  == 1)  comp_sat    = 1 ;

    // -- --hgfptg
    if (comp_h == 1)
       { if (thg.size() != sz)  thg.resize(sz) ;
         ArrOfDouble ttgcelcius(sz);
         for (int i=0; i<sz; i++)
            ttgcelcius[i] = tintg[i]-tabsk ;
         F77DECLARE(ca_hgfptg)(sz,tp[0],ttgcelcius[0],thg[0],terrc[0]) ;
         convert_eos_error(terrc,err_tmp) ;
         err.set_worst_error(err_tmp) ;
       }

    // -- -- fpsatte
    if (comp_sat == 1)
       { if (ttsp.size()   != sz) ttsp.resize(sz)   ;
         if (ttsp1.size()  != sz) ttsp1.resize(sz)  ;
         if (thlsp.size()  != sz) thlsp.resize(sz)  ;
         if (thlsp1.size() != sz) thlsp1.resize(sz) ;
         if (thvsp.size()  != sz) thvsp.resize(sz)  ;
         if (thvsp1.size() != sz) thvsp1.resize(sz) ;
         if (tcplsp.size() != sz) tcplsp.resize(sz) ;
         if (tclsp1.size() != sz) tclsp1.resize(sz) ;
         if (tcpvsp.size() != sz) tcpvsp.resize(sz) ;
         if (tcvsp1.size() != sz) tcvsp1.resize(sz) ;
         if (trlsp.size()  != sz) trlsp.resize(sz)  ;
         if (trlsp1.size() != sz) trlsp1.resize(sz) ;
         if (trvsp.size()  != sz) trvsp.resize(sz)  ;
         if (trvsp1.size() != sz) trvsp1.resize(sz) ;

         F77DECLARE(ca_fpsatte)(sz,tp[0],
                          ttsp[0],ttsp1[0],
                          thlsp[0],thlsp1[0],
                          thvsp[0],thvsp1[0],
                          tcplsp[0],tclsp1[0],
                          tcpvsp[0],tcvsp1[0],
                          trlsp[0],trlsp1[0],
                          trvsp[0],trvsp1[0]) ;
       }
    
    // -- -- fhvappte
    if (comp_fhvap == 1)
       { if (ttg.size()    != sz) ttg.resize(sz)    ;
         if (ttgpv.size()  != sz) ttgpv.resize(sz)  ;
         if (ttghv.size()  != sz) ttghv.resize(sz)  ;
         if (tcpv.size()   != sz) tcpv.resize(sz)   ;
         if (tcpvpv.size() != sz) tcpvpv.resize(sz) ;
         if (tcpvhv.size() != sz) tcpvhv.resize(sz) ;
         if (trv.size()    != sz) trv.resize(sz)    ;
         if (trvpv.size()  != sz) trvpv.resize(sz)  ;
         if (trvhv.size()  != sz) trvhv.resize(sz)  ;
         if (ttgk.size()   != sz) ttgk.resize(sz)   ;
         if (thvmhs.size() != sz) thvmhs.resize(sz) ;
         
         F77DECLARE(ca_fhvappte)(sz,tp[0],thg[0],
                           ttsp[0],ttsp1[0],
                           thvsp[0],thvsp1[0],
                           tcpvsp[0],tcvsp1[0],
                           trvsp[0],trvsp1[0],
                           ttg[0],ttgpv[0],ttghv[0],
                           tcpv[0],tcpvpv[0],tcpvhv[0],
                           trv[0],trvpv[0],trvhv[0],
                           ttgk[0],thvmhs[0]) ;
       }

    // -- -- fhvapate
    if (comp_fhvapa == 1)
       { if (ttlav.size()  != sz) ttlav.resize(sz)  ;
         if (tlavpv.size() != sz) tlavpv.resize(sz) ;
         if (tlavtg.size() != sz) tlavtg.resize(sz) ;
         if (ttmuv.size()  != sz) ttmuv.resize(sz)  ;
         if (tmuvpv.size() != sz) tmuvpv.resize(sz) ; 
         if (tmuvtg.size() != sz) tmuvtg.resize(sz) ;
         if (tsi.size()    != sz) tsi.resize(sz)    ;
         if (tsipv.size()  != sz) tsipv.resize(sz)  ;
         
         F77DECLARE(ca_fhvapate)(sz,
                           tp[0],ttg[0],
                           ttsp[0],ttsp1[0],
                           ttlav[0],tlavpv[0],tlavtg[0],
                           ttmuv[0],tmuvpv[0],tmuvtg[0],
                           tsi[0],tsipv[0]) ;
       }

    // -- -- fhbasa
    if (comp_fhbasa == 1)
       { if (ttlag1.size() != sz) ttlag1.resize(sz) ;
         if (ttlag3.size() != sz) ttlag3.resize(sz) ;
         if (ttmug1.size() != sz) ttmug1.resize(sz) ;
         if (ttmug3.size() != sz) ttmug3.resize(sz) ;
         
         F77DECLARE(ca_fhbasa)(sz,
                         ttgpv[0],ttghv[0],
                         tlavpv[0],tlavtg[0],tmuvpv[0],tmuvtg[0],
                         ttlag1[0],ttlag3[0],ttmug1[0],ttmug3[0]) ;
       }

    // -- -- others
    // -- -- (c)
    if (comp_c == 1)
       { double cc ;
         double m05 = -0.5 ;
         for (int i=0; i<sz; i++)
            { cc    = trvpv[i]+trvhv[i]/trv[i] ;
              tc[i] = pow(cc,m05) ;
            }
       }
    // -- -- (all not defined)
    for (int i=0;i<ind_not_def+1;i++)
      { the_fluid->EOS_Fluid::compute(input[0], input[1], r[not_def[i]],err) ;      
        err.set_worst_error(err_tmp) ;   
      }

    // -- change unit if needed
    if (chg_T_sat == 1) ttsp += tabsk ;
    if (chg_T     == 1) ttg  += tabsk ; 

    // -- end
    return err.find_worst_error().generic_error();
  }

  // verify for EOS_Field
  inline EOS_Error CATHARE_Water::verify(const EOS_Field& input, EOS_Error_Field& err, int dom) const
  {
    EOS_Fields inputs(1) ;
    inputs[0] = input ;
    return verify(inputs, err, dom) ;
  }

  // verify for EOS_Fields
  inline EOS_Error CATHARE_Water::verify(const EOS_Fields& input, EOS_Error_Field& err, int dom) const
  { // -- init
    err=EOS_Internal_Error::OK ;
    double p_max, p_min, T_max, T_min, h_max, h_min ;
    switch(dom)
       { case 0 :
            p_max = xppte  ;
            p_min = xpmte  ;
            h_max = xhlpte ;
            h_min = xhlmte ;
            T_max = xtlp+tabsk ;
            T_min = xtlm+tabsk ;
            break ;
            
         case 1 :
            p_max = xppte  ;
            p_min = xpmte  ;
            h_max = xhlpte ;
            h_min = xhlmte ;
            T_max = xtlp+tabsk ;
            T_min = xtlm+tabsk ;
            break ;
            
         case 2 :
            p_max = xppte  ;
            p_min = xpmte  ;
            h_max = xhvpte ;
            h_min = xhvmte ;
            T_max = xtgp+tabsk ;
            T_min = xtgm+tabsk ;     
            break ;
            
         default :
            err = EOS_Internal_Error::NOT_IMPLEMENTED ;
            return EOS_Error::error ;
            break ;
       }

    int nb_fields = input.size() ;
    int sz ;
    EOS_Property prop ;
    for (int i=0; i<nb_fields; i++)
       { const NEPTUNE::EOS_Field& field_i=input[i] ;
         prop = field_i.get_property_number() ;
         const ArrOfDouble& values_i = field_i.get_data() ;
         switch(prop)
            { case NEPTUNE::p :
                 sz = values_i.size() ;
                 for (int j=0; j<sz; j++)
                    { if (values_i[j] < p_min)  err.set(j, EOS_Internal_Error(930, bad)) ;
                      if (values_i[j] > p_max)  err.set(j, EOS_Internal_Error(931, bad)) ;
                    }
                 break ;
                 
              case NEPTUNE::h :
                 sz = values_i.size() ;
                 for (int j=0; j<sz; j++)
                    { if (values_i[j] < h_min)  err.set(j, EOS_Internal_Error(920, bad)) ;
                      if (values_i[j] > h_max)  err.set(j, EOS_Internal_Error(921, bad)) ;
                    }
                 break ;
                 
              case NEPTUNE::T :
                 sz = values_i.size() ;
                 for (int j=0; j<sz; j++)
                    { if (values_i[j] < T_min)  err.set(j, EOS_Internal_Error(910, bad)) ;
                      if (values_i[j] > T_max)  err.set(j, EOS_Internal_Error(911, bad)) ;
                    }
                 break ;
                 
              default :
                 err = EOS_Internal_Error::NOT_IMPLEMENTED ;
                 break ;
            }
       }
    return err.find_worst_error().generic_error() ;
  };

  inline EOS_Internal_Error  CATHARE_Water::convert_eos_error(const int partial_code) const 
  { switch(partial_code)
       { case    0 : return EOS_Internal_Error::OK ;
         case 3800 : return EOS_Internal_Error(3800, bad) ;
         default   : return EOS_Internal_Error(partial_code, bad) ;
       }
  };

  inline void  CATHARE_Water::convert_eos_error(const ArrOfInt& terrc, EOS_Error_Field& err) const
  { int sz = terrc.size() ;
    EOS_Internal_Error internal_error = EOS_Internal_Error::OK ;
    for (int i=0; i<sz; i++)
       { internal_error = convert_eos_error(terrc[i]) ;
         err.set(i,internal_error.get_code()) ;
       }
  };

};
#endif
