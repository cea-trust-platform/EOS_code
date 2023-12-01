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



#include "EOS/Src/EOS_Thetis/EOS_THETIS.hxx"
#include "EOS/Src/EOS_Thetis/EOS_ThetisUtil.hxx"
#include "EOS/Src/EOS_Thetis/thetis.hxx"
#include "EOS/Src/EOS_Thetis/Thetis/TH_erreurs.h"
#include "EOS/API/EOS_Field.hxx"
#include "EOS/API/EOS_Fields.hxx"
#include "EOS/API/EOS_Error_Field.hxx"
#include "EOS/API/EOS_properties.hxx"
#include "EOS/API/EOS_Error.hxx"
#include <math.h>

namespace NEPTUNE_EOS
{
  const AString EOS_THETIS::tablename("Thetis") ;

  const AString EOS_THETIS::versionname("2.1.b") ;

  const AString& EOS_THETIS::table_name()const
  { return tablename ;
  }

  const AString& EOS_THETIS::version_name()const
  { return versionname ;
  } 

  //   const AString& EOS_THETIS::version_name(){return "Unknown");
  
  //! to know if Thetis is already initialized
  bool EOS_THETIS::initialized = false ;
  //! maximum fluid number that Thetis can load
  const int EOS_THETIS::nbFluidMax = 10 ;
  int EOS_THETIS::fluidCounter     = 0 ;

  //! Thetis compute mod
  //!     1 : compute at loading (Default mod)
  //!     2 : compute at first compute
  //!     3 : every time
  int EOS_THETIS::computeMod = 1 ;
 
  EOS_THETIS::EOS_THETIS(){} ;
  
  EOS_THETIS::~EOS_THETIS(){} ;

  int EOS_THETIS::init(const Strings& strings)
  { AString sValue ;
    AString optName("Mod") ;
    int computeModDefine = getOptionValue(optName, strings, sValue) ;
    int errorInit = 0 ;

    // Thetis Initialization
    if ( EOS_THETIS::initialized and computeModDefine == 1)
       { cerr << "Warning : Thetis is already initialized, the new \"Mod\" value is not taking into account" << endl ;
       }

    if ( ! EOS_THETIS::initialized)
       { if (computeModDefine == 1)
            { EOS_THETIS::computeMod = atoi(sValue.aschar()) ;
            }

         const int *pnbFluidMax = &EOS_THETIS::nbFluidMax ;
         int *ppnbFluidMax = const_cast<int *>(pnbFluidMax) ;

         errorInit = TVINIT(*ppnbFluidMax,EOS_THETIS::computeMod) ;

         if (errorInit > 0)
            { TVERROR(errorInit) ;
              exit(-1) ;
              return EOS_Error::error ;
            }
         EOS_THETIS::initialized = true ;
       }

    return EOS_Error::good ;

  } /* init */


  // Exception thrown internally in EOS_THETIS.cxx.
  // This exception is catched in calth_all methods
  // and converted to an EOS_Internal_Error return code.
  struct Thetis_Exception {
    Thetis_Exception(EOS_Internal_Error err) :
      err_(err)
    {
    }
    EOS_Internal_Error err_ ;
  };


  //! compute all properties 
  EOS_Error EOS_THETIS::calth_all(const EOS_Fields& input, 
                                  EOS_Fields& r, EOS_Error_Field& err) const
  { ArrOfDouble tp  ;
    ArrOfDouble ti  ;
    ArrOfDouble tr  ;
    ArrOfDouble tiT ;

    ArrOfDouble arrayT     ;
    ArrOfDouble arrayC     ;
    ArrOfDouble arrayMu    ;
    ArrOfDouble arrayRho   ;
    ArrOfDouble arrayDtdp  ;
    ArrOfDouble arrayDrdp  ;
    ArrOfDouble arrayDrdh  ;
    ArrOfDouble arrayDmudp ;
    ArrOfDouble arrayDmudh ;

    AString nameT     ;
    AString nameC     ;
    AString nameMu    ;
    AString nameRho   ;
    AString nameDtdp  ;
    AString nameDrdp  ;
    AString nameDrdh  ;
    AString nameDmudp ;
    AString nameDmudh ;

    bool computeT     = false ;
    bool computeC     = false ;
    bool computeH     = false ;
    bool computeMu    = false ;
    bool computeRho   = false ;
    bool computeDtdp  = false ;
    bool computeDrdp  = false ;
    bool computeDrdh  = false ;
    bool computeDmudp = false ;
    bool computeDmudh = false ;

    // --  
    EOS_thermprop prop ;

    // -- -- input
    int v1 = 0 ;
    int v2 = 0 ;
    int nb_fields_in = input.size() ;
    int sz = input[0].size() ;

    double *ptP  = NULL ;
    double *ptI  = NULL ;
    double *ptR  = NULL ;
    double *ptIT = NULL ;

    double deltaP =  1.e0 ;
    double deltaH = 10.e0 ;
    if (is_liquid())  deltaH = -10.e0 ;

    ArrOfInt terr ;
    int *ptE ;
    assert(err.size() == sz) ;
    terr.set_ptr(sz, err.get_data().get_ptr()) ;
    ptE = const_cast<int *>(terr.get_ptr()) ;

    EOS_Internal_Error ierr(EOS_Internal_Error::OK) ;

    try {

      for(int i=0; i<nb_fields_in; i++)
        { prop = input[i].get_property() ;
          switch(prop)
            { case NEPTUNE::p :
                tp.set_ptr(sz, input[i].get_data().get_ptr()) ;
                ptP = const_cast<double *>(tp.get_ptr()) ;
                v1 = 1 ;
                break ;

              case NEPTUNE::h :
                ti.set_ptr(sz, input[i].get_data().get_ptr()) ;
                ptI = const_cast<double *>(ti.get_ptr()) ;
                v2 = 1 ;
                break ;

              case NEPTUNE::T :
                tiT.set_ptr(sz, input[i].get_data().get_ptr()) ;
                ptIT = const_cast<double *>(tiT.get_ptr()) ;
                v2 = 1 ;       
                computeH = true ;
                break ;

              default :
                throw Thetis_Exception(EOS_Internal_Error::NOT_IMPLEMENTED) ;
                break ;
            }
        }
      // -- -- check
    
      if ( (nb_fields_in != 2) || (v1*v2 != 1) )
         throw Thetis_Exception(EOS_Internal_Error::NOT_IMPLEMENTED) ;

      // -- -- output
      int nb_fields = r.size() ;
      int fluidNum = getFluidNum() ;

      int errorNum = 0 ;

      if (computeH)
         { ArrOfDouble quality(sz) ;
           getQuality(quality) ;
           double *ptQ = const_cast<double *>(quality.get_ptr()) ;
           ti.resize(sz) ;
           ptI = const_cast<double *>(ti.get_ptr()) ;
           errorNum = TVHHPT(fluidNum, sz, ptP, ptIT, ptQ, ptI, ptE) ;
           AString name("temporary_enthalpy_pT") ;
           ierr = worst_internal_error(ierr, errorStudy(errorNum,name)) ;
         }
      

      for (int i=0; i<nb_fields; i++)
         { prop = r[i].get_property() ;
           assert(r[i].size() == sz) ;

           tr.set_ptr(sz, r[i].get_data().get_ptr()) ;
           ptR = const_cast<double *>(tr.get_ptr()) ;

           switch(prop)
              { case NEPTUNE::h :
                   r[i].set_data() = ti ;
                   // r[i].set_data().set_ptr(ti.size(), ti.get_ptr()) ;
                   break ;
                case NEPTUNE::T :
                   computeT = true ;
                   arrayT.set_ptr(sz, r[i].get_data().get_ptr()) ;
                   nameT = r[i].get_property_title() ;
                   break ;
                case NEPTUNE::rho :
                   computeRho = true ;
                   arrayRho.set_ptr(sz, r[i].get_data().get_ptr()) ;
                   nameRho = r[i].get_property_title() ;
                   break ;
                case NEPTUNE::cp :
                   errorNum = TVCPPH(fluidNum, sz, ptP, ptI, ptR, ptE);
                   break ;
                case NEPTUNE::lambda :
                   errorNum = TVALPH(fluidNum, sz, ptP, ptI, ptR, ptE) ;
                   break ;
                case NEPTUNE::mu :
                   computeMu = true ;
                   arrayMu.set_ptr(sz, r[i].get_data().get_ptr()) ;
                   nameMu = r[i].get_property_title() ;
                   break ;
                case NEPTUNE::w :
                   computeC = true ;
                   arrayC.set_ptr(sz, r[i].get_data().get_ptr()) ;
                   nameC = r[i].get_property_title() ;
                   break ;
                case NEPTUNE::sigma :
                   errorNum = TVSIGP(fluidNum, sz, ptP, ptR , ptE) ;
                   break ;
                // Derivatives
                case NEPTUNE::d_T_d_p_h :
                   computeDtdp = true ;
                   arrayDtdp.set_ptr(sz, r[i].get_data().get_ptr()) ;
                   nameDtdp = r[i].get_property_title();
                   break ;
                case NEPTUNE::d_rho_d_p_h :
                   computeDrdp = true ;
                   arrayDrdp.set_ptr(sz, r[i].get_data().get_ptr()) ;
                   nameDrdp = r[i].get_property_title() ;
                   break ;
                case NEPTUNE::d_rho_d_h_p :
                   computeDrdh = true ;
                   arrayDrdh.set_ptr(sz, r[i].get_data().get_ptr());
                   nameDrdh = r[i].get_property_title() ;
                   break ;
                case NEPTUNE::d_mu_d_p_h :
                   computeDmudp = true ;
                   arrayDmudp.set_ptr(sz, r[i].get_data().get_ptr()) ;
                   nameDmudp = r[i].get_property_title() ;
                   break ;
                case NEPTUNE::d_mu_d_h_p :
                   computeDmudh = true ;
                   arrayDmudh.set_ptr(sz, r[i].get_data().get_ptr()) ;
                   nameDmudh = r[i].get_property_title() ;
                   break ;
                default :
                   throw Thetis_Exception(EOS_Internal_Error::NOT_IMPLEMENTED) ;
                   break ;
              }
           ierr = worst_internal_error(ierr, errorStudy(errorNum, r[i].get_property_title())) ;
         }

      if (computeT || computeDtdp || computeDrdh || computeDrdp)
         { if (arrayT.size() == 0)   arrayT.resize(sz) ;

           ptR = const_cast<double *>(arrayT.get_ptr()) ;

           int errorNum = TVTTPH(fluidNum, sz, ptP, ptI, ptR, ptE) ;
           ierr = worst_internal_error(ierr, errorStudy(errorNum, nameT)) ;
         }

      if (computeMu or computeDmudh or computeDmudp)
         { if (arrayMu.size() == 0)  arrayMu.resize(sz) ;

           ptR = const_cast<double *>(arrayMu.get_ptr()) ;

           int errorNum = TVETPH(fluidNum, sz, ptP, ptI, ptR, ptE) ;
           ierr = worst_internal_error(ierr, errorStudy(errorNum, nameMu)) ;
         }

      if (computeDtdp)
         { assert(arrayT.size() != 0)    ;
           assert(arrayDtdp.size() != 0) ;

           ArrOfDouble pDeltaP(tp) ;
           pDeltaP += deltaP ;
           double *ptP1 = const_cast<double *>(pDeltaP.get_ptr())   ;
           ptR  = const_cast<double *>(arrayDtdp.get_ptr()) ;

           int errorNum = TVTTPH(fluidNum, sz, ptP1, ptI, ptR, ptE) ;
           ierr = worst_internal_error(ierr, errorStudy(errorNum, nameDtdp)) ;
         
           arrayDtdp -= arrayT ;
           arrayDtdp /= deltaP ;
        }

      if (computeDmudp)
         { assert(arrayMu.size() != 0)    ;
           assert(arrayDmudp.size() != 0) ;

           ArrOfDouble pDeltaP(tp) ;
           pDeltaP += deltaP ;
           double *ptP1 = const_cast<double *>(pDeltaP.get_ptr())    ;
           ptR  = const_cast<double *>(arrayDmudp.get_ptr()) ;

           int errorNum = TVETPH(fluidNum, sz, ptP1, ptI, ptR, ptE) ;
           ierr = worst_internal_error(ierr, errorStudy(errorNum, nameDmudp)) ;
           arrayDmudp -= arrayMu ;
           arrayDmudp /= deltaP ;
         }

      if (computeDmudh)
         { assert(arrayMu.size() != 0)    ;
           assert(arrayDmudh.size() != 0) ;

           ArrOfDouble hDeltaH(ti) ;
           hDeltaH += deltaH ;
           double *ptH1 = const_cast<double *>(hDeltaH.get_ptr())    ;
           ptR  = const_cast<double *>(arrayDmudh.get_ptr()) ;

           int errorNum = TVETPH(fluidNum, sz, ptP, ptH1, ptR, ptE) ;
           ierr = worst_internal_error(ierr, errorStudy(errorNum, nameDmudh)) ;
           arrayDmudh -= arrayMu ;
           arrayDmudh /= deltaH  ;
         }

      if (computeRho || computeDrdp)
         { if (arrayRho.size() == 0)  arrayRho.resize(sz) ;

           ptR = const_cast<double *>(arrayRho.get_ptr()) ;

           int errorNum = TVROPH(fluidNum, sz, ptP, ptI, ptR, ptE) ;
           ierr = worst_internal_error(ierr, errorStudy(errorNum, nameRho)) ;
         }

      if (computeDrdh || computeDrdp)
         { assert(arrayT.size() != 0) ;
           //(dro/dh)|P = 1/T * (dro/dS)|P

           if (arrayDrdh.size() == 0)  arrayDrdh.resize(sz) ;

           ptR = const_cast<double *>(arrayDrdh.get_ptr()) ;

           int errorNum = TVBEPH(fluidNum, sz, ptP, ptI, ptR, ptE) ;
           ierr = worst_internal_error(ierr, errorStudy(errorNum, nameDrdh)) ;

           for (int i=0; i < sz; i++)
             arrayDrdh[i] = arrayDrdh[i] / arrayT[i] ;
         }

      if (computeC || computeDrdp)
         { if (arrayC.size() == 0)  arrayC.resize(sz) ;

           ptR = const_cast<double *>(arrayC.get_ptr()) ;

           int errorNum = TVA2PH(fluidNum, sz, ptP, ptI, ptR, ptE) ;
           ierr = worst_internal_error(ierr, errorStudy(errorNum, nameC)) ;

           // Voir si on envoie la celerite au carre
           for (int i=0; i < sz; i++)
             arrayC[i] = sqrt(arrayC[i]) ;
         }

      if (computeDrdp)
         { assert(arrayC.size()    != 0) ;
           assert(arrayDrdh.size() != 0) ;
           assert(arrayRho.size()  != 0) ;
           assert(arrayDrdp.size() != 0) ;

           //(dro/dp)|h = (dro/dp)|S - 1/ro * (dro/dh)|P
           // with (dro/dp)|S = 1/C2

           for (int i=0; i < sz; i++)
             { arrayDrdp[i] = 1.e0/(arrayC[i]*arrayC[i]) 
                              - arrayDrdh[i]/arrayRho[i] ;
             }
         }
    }

    catch (Thetis_Exception ex) 
       { ierr = ex.err_;
       }

    // Thetis low level functions return only one global error code
    // for all compute points. We cannot fill a detailed error field.
    // This is why the error field is filled with one unique value:
    err = ierr ;

    return ierr.generic_error() ;
  }

  //! compute all saturation properties
  EOS_Error EOS_THETIS::calth_all_saturation(const EOS_Field& P, 
                        EOS_Fields& r, 
                        EOS_Error_Field& err) const
  { ArrOfDouble tp ;
    ArrOfDouble ti ;
    ArrOfDouble tr ;

    //
    ArrOfDouble arrayT     ;
    ArrOfDouble arrayHL    ;
    ArrOfDouble arrayHV    ;
    ArrOfDouble arrayRhoL  ;
    ArrOfDouble arrayRhoV  ;
    ArrOfDouble arrayDtdp  ;
    ArrOfDouble arrayDrdpL ;
    ArrOfDouble arrayDrdpV ;
    ArrOfDouble arrayDhdpL ;
    ArrOfDouble arrayDhdpV ;

    AString nameT     ;
    AString nameHL    ;
    AString nameHV    ;
    AString nameRhoL  ;
    AString nameRhoV  ;
    AString nameDtdp  ;
    AString nameDrdpL ;
    AString nameDrdpV ;
    AString nameDhdpL ;
    AString nameDhdpV ;

    bool computeT     = false ;
    bool computeHL    = false ;
    bool computeHV    = false ;
    bool computeRhoL  = false ;
    bool computeRhoV  = false ;
    bool computeDtdp  = false ;
    bool computeDrdpL = false ;
    bool computeDrdpV = false ;
    bool computeDhdpL = false ;
    bool computeDhdpV = false ;

    // --  
    EOS_thermprop prop    ;
    EOS_saturprop propsat ;

    // -- -- input
    int sz = P.size() ;

    double *ptP ;
    double *ptR ;

    double deltaP =  1.e0 ;

    ArrOfInt terr ;
    int *ptE ;
    terr.set_ptr(sz, err.get_data().get_ptr()) ;
    ptE = const_cast<int *>(terr.get_ptr()) ;

    EOS_Internal_Error ierr(EOS_Internal_Error::OK) ;

    try
       { prop = P.get_property() ;
         switch(prop)
            { case NEPTUNE::p :
                 tp.set_ptr(sz, P.get_data().get_ptr()) ;
                 ptP = const_cast<double *>(tp.get_ptr()) ;
                 break ;
              default :
                propsat = P.get_sat_property() ;
                switch(propsat)
                   { case NEPTUNE::p_sat :
                        tp.set_ptr(sz, P.get_data().get_ptr()) ;
                        ptP = const_cast<double *>(tp.get_ptr()) ;
                        break ;
                     default :
                        throw Thetis_Exception(EOS_Internal_Error::NOT_IMPLEMENTED) ;
                        break ;
                   }
                break ;
            }

         // -- -- output
         int nb_fields=r.size() ;
         int fluidNum = getFluidNum() ;

         for (int i=0; i<nb_fields; i++)
            { EOS_saturprop propsat = r[i].get_sat_property() ;

              tr.set_ptr(sz, r[i].get_data().get_ptr()) ;
              ptR = const_cast<double *>(tr.get_ptr()) ;

              switch(propsat)
                 { case NEPTUNE::T_sat :
                      computeT = true ;
                      arrayT.set_ptr(sz, r[i].get_data().get_ptr()) ;
                      nameT = r[i].get_property_title() ;
                      break ;
                   case NEPTUNE::rho_l_sat :
                      computeRhoL = true ;
                      arrayRhoL.set_ptr(sz, r[i].get_data().get_ptr()) ;
                      nameRhoL = r[i].get_property_title() ;
                      break ;
                   case NEPTUNE::rho_v_sat :
                      computeRhoV = true ;
                      arrayRhoV.set_ptr(sz, r[i].get_data().get_ptr()) ;
                      nameRhoV = r[i].get_property_title() ;
                      break ;
                   case NEPTUNE::h_l_sat :
                      computeHL = true ;
                      arrayHL.set_ptr(sz, r[i].get_data().get_ptr()) ;
                      nameHL = r[i].get_property_title() ;
                      break ;
                   case NEPTUNE::h_v_sat :
                      computeHV = true ;
                      arrayHV.set_ptr(sz, r[i].get_data().get_ptr()) ;
                      nameHV = r[i].get_property_title() ;
                      break ;
                   // Derivatives
                   case NEPTUNE::d_T_sat_d_p :
                      computeDtdp = true ;
                      arrayDtdp.set_ptr(sz, r[i].get_data().get_ptr()) ;
                      nameDtdp = r[i].get_property_title() ;
                      break ;
                   case NEPTUNE::d_rho_l_sat_d_p :
                      computeDrdpL = true ;
                      arrayDrdpL.set_ptr(sz, r[i].get_data().get_ptr()) ;
                      nameDrdpL = r[i].get_property_title() ;
                      break ;
                   case NEPTUNE::d_rho_v_sat_d_p :
                      computeDrdpV = true ;
                      arrayDrdpV.set_ptr(sz, r[i].get_data().get_ptr()) ;
                      nameDrdpV = r[i].get_property_title() ;
                      break ;
                   case NEPTUNE::d_h_l_sat_d_p :
                      computeDhdpL = true ;
                      arrayDhdpL.set_ptr(sz, r[i].get_data().get_ptr()) ;
                      nameDhdpL = r[i].get_property_title() ;
                      break ;
                   case NEPTUNE::d_h_v_sat_d_p :
                      computeDhdpV = true ;
                      arrayDhdpV.set_ptr(sz, r[i].get_data().get_ptr()) ;
                      nameDhdpV = r[i].get_property_title() ;
                      break;
                   default :
                      throw Thetis_Exception(EOS_Internal_Error::NOT_IMPLEMENTED) ;
                      break ;
                 }
            }

         if (computeRhoL || computeDtdp || computeDrdpL ||
             computeRhoV || computeDrdpV)
            { if (arrayRhoL.size() == 0)  arrayRhoL.resize(sz) ;

              if (arrayRhoV.size() == 0)  arrayRhoV.resize(sz) ;

              double *ptRL = const_cast<double *>(arrayRhoL.get_ptr()) ;
              double *ptRV = const_cast<double *>(arrayRhoV.get_ptr()) ;

              int errorNum = TVROSA(fluidNum, sz, ptP, ptRL, ptRV, ptE) ;
              ierr = worst_internal_error(ierr, errorStudy(errorNum, nameT)) ;
            }

         if (computeHL || computeDhdpL ||
             computeHV || computeDhdpV || computeDtdp)
           { if (arrayHL.size() == 0)  arrayHL.resize(sz) ;

             if (arrayHV.size() == 0)  arrayHV.resize(sz) ;

             double *ptRL = const_cast<double *>(arrayHL.get_ptr()) ;
             double *ptRV = const_cast<double *>(arrayHV.get_ptr()) ;

             int errorNum = TVHHSA(fluidNum, sz, ptP, ptRL, ptRV, ptE) ;
             ierr = worst_internal_error(ierr, errorStudy(errorNum, nameT)) ;

           }

         if (computeT || computeDtdp)
            { if (arrayT.size() == 0)  arrayT.resize(sz) ;

              ptR = const_cast<double *>(arrayT.get_ptr()) ;

              int errorNum = TVTTSA(fluidNum, sz, ptP, ptR, ptE) ;
              ierr = worst_internal_error(ierr, errorStudy(errorNum, nameT)) ;

            }

         if (computeDtdp)
            { //d(Tsat)/Dp by Clapeyron
              assert(arrayT.size()    != 0) ;
              assert(arrayDtdp.size() != 0) ;
              assert(arrayHL.size()   != 0) ;
              assert(arrayHV.size()   != 0) ;
              assert(arrayRhoL.size() != 0) ;
              assert(arrayRhoV.size() != 0) ;

              for (int i=0; i < sz; i++)
                { arrayDtdp[i] = arrayT[i] / (arrayHV[i] - arrayHL[i])
                                 * ( 1./arrayRhoV[i] - 1./arrayRhoL[i]) ;
                }
            }


         if (computeDrdpL || computeDrdpV)
            { if (arrayDrdpL.size() == 0)  arrayDrdpL.resize(sz) ;

              if (arrayDrdpV.size() == 0)  arrayDrdpV.resize(sz) ;

              ArrOfDouble pDeltaP(tp) ;
              pDeltaP += deltaP ;

              double *ptP1 = const_cast<double *>(pDeltaP.get_ptr())    ;
              double *ptR1 = const_cast<double *>(arrayDrdpL.get_ptr()) ;
              double *ptR2 = const_cast<double *>(arrayDrdpV.get_ptr()) ;

              int errorNum = TVROSA(fluidNum, sz, ptP1, ptR1, ptR2, ptE) ;
              ierr = worst_internal_error(ierr, errorStudy(errorNum, nameDtdp)) ;
            
              if (computeDhdpL)
                 { arrayDrdpL -= arrayHL ;
                   arrayDrdpL /= deltaP  ;
                 }

              if (computeDhdpV)
                 { arrayDrdpV -= arrayHV ;
                   arrayDrdpV /= deltaP  ;
                 }
            }

         if (computeDhdpL || computeDhdpV)
            { if (arrayDhdpL.size() == 0)  arrayDhdpL.resize(sz) ;

              if (arrayDhdpV.size() == 0)  arrayDhdpV.resize(sz) ;

              ArrOfDouble pDeltaP(tp) ;
              pDeltaP += deltaP ;

              double *ptP1 = const_cast<double *>(pDeltaP.get_ptr())    ;
              double *ptH1 = const_cast<double *>(arrayDhdpL.get_ptr()) ;
              double *ptH2 = const_cast<double *>(arrayDhdpV.get_ptr()) ;

              int errorNum = TVHHSA(fluidNum, sz, ptP1, ptH1, ptH2, ptE) ;
              ierr = worst_internal_error(ierr, errorStudy(errorNum, nameDtdp)) ;
            
              if (computeDhdpL)
                 { arrayDhdpL -= arrayHL ;
                   arrayDhdpL /= deltaP  ;
                 }

              if (computeDhdpV)
                 { arrayDhdpV -= arrayHV ;
                   arrayDhdpV /= deltaP  ;
                 }
            }
        }

    catch(Thetis_Exception ex) 
       { ierr = ex.err_ ;
       }

    // Thetis low level functions return only one global error code
    // for all compute points. We cannot fill a detailed error field.
    // This is why the error field is filled with one unique value:
    err = ierr ;
    return ierr.generic_error() ;
  }
  
  static AString error_description_data ;

  // Partial code for thetis is 100 + internal error number.
  static const int EOS_THETIS_ERROR_BASE = 100;

  // Internal error numbers, starting from TVOK = 0
  // Errors between 0 and EOS_THETIS_BAD-1 are "EOS_Error = error",
  // Other errors are "EOS_Error = bad";
  static const int EOS_THETIS_BAD = TVERR_PRESSION ;

  static const int AI_ERRNO[CN_NB_MAX_ERR] = {
    TVOK, 
    TVERR, 
    TVERR_MEMALLOC, 
    TVERR_FICHDONNEES, 
    TVERR_FICHINEXISTANT,
    TVERR_FICHINVALIDE, 
    TVERR_IDINVALIDE, 
    TVERR_INITAPPEL, 
    TVERR_LGINVALIDE,
    TVERR_MODECOEFS,
    TVERR_NBMAXFLUIDES,
    TVERR_NOFLUIDE,
    TVERR_NOPLAN,
    TVERR_NOFONCTION,
    TVERR_NOPHASE,
    TVERR_PRESSION,
    TVERR_TEMPERATURE,
    TVERR_ENTROPIE,
    TVERR_ENTHALPIE,
    TVERR_PT,
    TVERR_PS,
    TVERR_PH,
    TVERR_TITRE,
    TVERR_VECTEURS,
    TVERR_CODEPRESSION  };

  // Description of each internal error number
  static const char AC_ERRMSG[CN_NB_MAX_ERR][70] = {
    "Pas d erreur",
    "Erreur indeterminee",
    "Allocation memoire impossible",
    "Format de fichier invalide",
    "Fichier inexistant",
    "Fichier existant ou fichier invalide",
    "Type d enregistrement invalide",
    "TVINIT appels multiples interdit",
    "Longueur d enregistrement invalide",
    "Mode de calcul des coefs invalide",
    /* C      */
    "Nombre maximum de fluides atteint",
    "Numero de fluide invalide ",
    "Numero de plan invalide",
    "Numero de fonction invalide",
    "Numero de phase invalide",
    /* C      */
    "Valeur(s) pression hors limite",
    "Valeurs temperature hors limite",
    "Valeur(s) entropie hors limite",
    "Valeur(s) enthalpie hors limite",
    "Valeurs pression et temperatures hors limite",
    "Valeurs pression et entropie hors limite",
    "Valeurs pression et enthalpie hors limite",
    "Valeur du titre en vapeur incorrecte",
    "Vecteur(s) memorise(s) invalide(s)",
    "Code de pression invalide ou pression critique inexistante"
  };


  EOS_Internal_Error EOS_THETIS::errorStudy(const int errorNum, const AString& name) const
  {
    if (errorNum == 0)  return EOS_Internal_Error::OK ;

    int I = 0 ;
    while( I < CN_NB_MAX_ERR && (errorNum != AI_ERRNO[I]) )
       I++ ;

    EOS_Error eos_error = EOS_Error::bad ;
    error_description_data = name ;
    // critical errors throw thetis_exception.
    // do not perform further calculation, return directely 
    // to client code.
    if (I > 0 && I < EOS_THETIS_BAD) 
       { eos_error = EOS_Error::error ;
         throw Thetis_Exception(EOS_Internal_Error(EOS_THETIS_ERROR_BASE + I, eos_error)) ;
       }

    if ( I>=CN_NB_MAX_ERR ) 
       return EOS_Internal_Error(EOS_THETIS_ERROR_BASE, bad) ;
    else 
       return EOS_Internal_Error(EOS_THETIS_ERROR_BASE + I, eos_error) ;
  }


  void EOS_THETIS::describe_error(const EOS_Internal_Error err_code, AString & description) const
  {
    const int pcode = err_code.get_partial_code();

    if (pcode == EOS_THETIS_ERROR_BASE) 
       { description = "EOS_THETIS: Unknown internal error, field ";
         description += error_description_data;
       } 
    else if (pcode > EOS_THETIS_ERROR_BASE 
             && pcode < EOS_THETIS_ERROR_BASE + CN_NB_MAX_ERR) 
       {
         const int icode = pcode - EOS_THETIS_ERROR_BASE ;
         description = "EOS_THETIS: Internal error " ;
         description += icode ;
         description += " (" ;
         description += AC_ERRMSG[icode] ;
         description += "), field " ;
         description += error_description_data ;
       }
    else
       EOS_Fluid::describe_error(err_code, description) ;
  }


  EOS_Error EOS_THETIS::compute(const EOS_Field& p, const EOS_Field& h, 
                                EOS_Fields& r, EOS_Error_Field& errfield) const
  { EOS_Fields input(2) ;
    input[0] = p ;
    input[1] = h ;
    EOS_Error cr = calth_all(input,r,errfield) ;
    return cr ;
  }


  EOS_Error EOS_THETIS::compute(const EOS_Field& p, const EOS_Field& h, 
                                EOS_Field& r, EOS_Error_Field& errfield) const
  { EOS_Fields fields(1) ;
    fields[0] = r ;
    return compute(p, h, fields, errfield) ;
  }


  EOS_Error EOS_THETIS::compute(const EOS_Field& p, 
                                EOS_Field& output, EOS_Error_Field& errfield) const
  { EOS_Fields fields(1) ;
    fields[0] = output ;
    return compute(p, fields, errfield) ;
  }


  EOS_Error EOS_THETIS::compute(const EOS_Field& p, 
                                EOS_Fields& output, EOS_Error_Field& errfield) const
  { return calth_all_saturation(p, output, errfield) ;
  }

  EOS_Error EOS_THETIS::compute(const EOS_Fields& input,
                                EOS_Fields& output, EOS_Error_Field& errfield) const
  { int nbi=input.size() ;
    EOS_Error err ;
    const EOS_Field& p=input[0] ;

    switch(nbi)
       { case 1 :
            err = compute(p, output, errfield) ;
            break ;
         case 2 :
            err = compute(p, input[1], output, errfield) ;
            break ;
         default :
            errfield = EOS_Internal_Error::NOT_IMPLEMENTED ;
            return EOS_Error::error ;
            break ;
       }
    return err ;
  }

} /* namespace NEPTUNE_EOS */ 
