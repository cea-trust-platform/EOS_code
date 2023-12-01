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



#ifndef CATHARE2_HXX
#define CATHARE2_HXX

#include "system/arch.h"
#include "EOS/API/EOS_Field.hxx"
#include "EOS/API/EOS_Fields.hxx"
#include "EOS/API/EOS_Error_Field.hxx"
#include "EOS/API/EOS_Error.hxx"
#include "EOS/API/EOS_properties.hxx"
#include "EOS/API/EOS_Fluid.hxx"
#include "EOS/Src/EOS_Cathare2/Cathare2_Water.f77.hxx"

using namespace NEPTUNE ;

extern "C"
{ //! Set the CATHARE2::CATHARE2::in_erpile flag
  /*!
   * This global method replaces the Cathare2 subroutine ERPILE.
   * It must be called only from other Cathare2 subroutines.
   */
  void F77DECLARE(c2_erpile)(char*) ;

  //! Fill the fluids properties arrays
  /*!
   * This is the declaration of a FORTRAN subroutine.
   * Code is in the Cathare2-source product.
   */
  void F77DECLARE(c2_inifld)() ;

  //! Fetch properties from fluids properties arrays
  /*!
   * This is the declaration of a FORTRAN subroutine.
   * Code is part of the CATHARE2 plug (GETFLDPROP.f)
   * \todo Document this method
   */
  void F77DECLARE(c2_getfldprop)(int& ifluid, double& fldxpm, double& fldxhlm, double& fldxhvm,
                              double& fldxtlm, double& fldxtgm, double& fldxpp, double& fldxhlp,
                              double& fldxhvp, double& fldxtlp, double& fldxtgp, double& fldxtgpic,
                              double& fldxsip, double& fluidm, double& fluidr, double& fluiddv,
                              double& fldcpcst,
                              double& fldepsl, double& fldepsg, double& fldepsr, double& fldepsp,
                              double& fldepshl, double& fldepshg, double& fldepstl, double& fldepstg,
                              double& fldepsal, double& fldepsmx, double& fldepspx, double& fldepsv,
                              double& fldepstw, double& fldepsmi, double& fldepsms, double& fldepsei,
                              double& fldepses, double& fldutol, double& fldutog, double& href,
                              double& tref, double& trefk, double& pc) ;
}

namespace NEPTUNE_EOS
{
  class EOS_Cathare2       ;
  class EOS_Cathare2Liquid ;
  class EOS_Cathare2Vapor  ;
}

namespace CATHARE2
{
  //! Available domains
  enum domain
  { saturated = 0 , //< use to bypass all calculations but saturated values
    liquid  ,       //< liquid domain
    vapor   ,       //< vapor domain
    unknown         //< unknown domain
  };

  enum TH_space
  {  no = 0 ,       // 
     Psat ,         // Saturation curve with Psat
     Tsat ,         // Saturation curve with Tsat
     Ph   ,         // Diagram Ph
     PT   ,         // Diagram PT
     Ps             // Diagram Ps
  };

  /*!
   * \enum error_code
   * Error codes used by CATHARE2 or the CATHARE2-EOS plug
   */
  enum error_code
  { ok             =    0 ,   //!< no error
    cathare_illp   = 3100 ,
    cathare_illhl  = 3200 ,
    cathare_illhg  = 3300 ,
    cathare_illhv  = 3310 ,
    cathare_illtg  = 3320 ,
    cathare_illtl  = 3330 ,
    cathare_illtw  = 3340 ,
    cathare_illag  = 3400 ,
    cathare_illzc  = 3450 ,
    cathare_illrg  = 3500 ,
    cathare_illrl  = 3510 ,
    cathare_illx   = 3700 ,
    cathare_illxsu = 3710 ,
    cathare_illpv  = 3720 ,
    cathare_illcvr = 3800 ,
    cathare_illpiv = 3900 ,
    P_below_min  ,            //!< pressure is below minimum
    P_above_max  ,            //!< pressure is above maximum
    h_below_min  ,            //!< enthalpy is below minimum
    h_above_max  ,            //!< enthalpy is above maximum
    T_below_min  ,            //!< temperature is below minimum
    T_above_max  ,            //!< temperature is above maximum
    unstable     ,            //!< (p,h) is in unstable domain
    canceled     ,            //!< because of a previous error, the computation has been canceled
    erpile_error ,            //!< CATHARE2 subroutine jumped to ERPILE: something went wrong
    h_pt_mix_newton           //!< Newton for h(p,T) for mixing is not converged
  };

  //! Pilot for Cathare2 Hydcom/EOS subroutines
  class CATHARE2
  {
    //! EOS_Cathare2 can use CATHARE2 private methods
    friend class NEPTUNE_EOS::EOS_Cathare2 ;
    //! EOS_Cathare2Liquid can use CATHARE2 private methods
    friend class NEPTUNE_EOS::EOS_Cathare2Liquid ;
    //! EOS_Cathare2Vapor can use CATHARE2 private methods
    friend class NEPTUNE_EOS::EOS_Cathare2Vapor ;

  public:

    //! Main constructor for CATHARE2
    /*!
     * \param fluid a EOS_Cathare2 pointer (for re-processing of unimplemented computations)
     * \param domain CATHARE2::liquid or CATHARE2::vapor
     * \param c2ref Cathare2 integer code for fluid
     */
    CATHARE2(NEPTUNE::EOS_Fluid* fluid, domain ph, int c2ref) ;

    //! Copy constructor. Should NOT be used.
    CATHARE2(const CATHARE2&) ;

    //! CATHARE2 destructor
    /*!
     * ~CATHARE2() is pure virtual to force CATHARE2 to be abstract
     */
    virtual ~CATHARE2() = 0 ;
    static bool in_erpile ;     //!< erpile error flag

  protected:
    int nincon ;                //!< Number of noncondensible gas
    int nsca   ;                //!< Number of scalars

    static const double tabsk ; //!< Thermodynamic temperature of water triple point
    static TH_space typ_ths   ;
    double un, zero, epspp    ;

    EOS_Fluid* fluid ;      //!< Pointer to the EOS_Cathare2 adapter
    domain phase;           //!< liquid or vapor
    double xpm,xhlm,xhvm,xtlm,xtgm,xpp,xhlp,xhvp,xtlp,xtgp,xtgpic,xsip,fldm,fldr,flddv,
           hvs7,tss7,tss7k,cpcst,epsl,epsg,epsr,epsp,epshl,epshg,epstl,epstg,epsal,epsmx,
           epspx,epsv,epstw,epsmi,epsms,epsei,epses,utol,utog,href,tref,trefk ;
    double pc ,             //!< Pressure on critical point
           tc ,             //!< Temperature on critical point
           hc ;             //!< Enthalpy on critical point
    // temporary
    double xpcri ;          //!< Pressure on critical point (temporary !!)
    ArrOfDouble pcarg ;     //!< Pressure on critical point (temporary !!)
    // temporary
    EOS_Error critical ;    //!< Status of critical computation

    //! Cathare2 flags
    int lfluid,    //< Cathare2 integer code for fluid
        licar,     //< Icare couplage
        licargas,  //< Icare couplage
        lienc,     //< ENCEINTE flag
        lirev,     //< Water table revision
        litermin,  //< Minimum newton iterations
        llatypml,  //< Compute mode for lambda
        lmutypml,  //< Compute mode for mu
        liter,     //< Number of newton iterations
        ligotra;   //< Transient or permanent

    ArrOfDouble lp;
    ArrOfDouble lpv, lpv1, lpv3;
    ArrOfDouble ltsp, ltsp1, l2tsdpvv, l2tsp1;
    ArrOfDouble ltspv, ltspvv;
    ArrOfDouble lhlsp, lhlsp1, lhvsp, lhvsp1;
    ArrOfDouble lhlsv, lhlsvv, lhvsv, lhvsvv;
    ArrOfDouble lcplsp, lclsp1, lcpvsp, lcvsp1;
    ArrOfDouble lcplsv, lclsvv, lcpvsv, lcvsvv;
    ArrOfDouble lrlsp, lrlsp1, lrvsp, lrvsp1;
    ArrOfDouble lrlsv, lrlsvv, lrvsv, lrvsvv;
    ArrOfDouble ltl, ltl1, ltl2;
    ArrOfDouble ltg, ltg1, ltg3, ltgpv, ltghv, ltgini, ltgk;
    ArrOfDouble lrl, lrlpt, lrl1, lrl2, lrl1pt, lrl2pt ;
    ArrOfDouble lrg, lrg1, lrg3, lrv, lrvpt, lrvpv, lrvhv, lrv1, lrv3, lrv1pt, lrv3pt ;
    ArrOfDouble lcpl, lcplpt, lcpl1, lcpl2, lcpl1pt, lcpl2pt ;
    ArrOfDouble lcpg, lcpg1, lcpg3, lcpv, lcpvpt, lcpvpv, lcpvhv, lcpgtg, lcpvpvpt, lcpgtgpt ;
    ArrOfDouble lbetal, lbetal1, lbetal2;
    ArrOfDouble lhl, lhlpt, lhl1, lhl1pt, lhl2, lhl2pt, lhg, lhv, lhvpt, lhvmhs, lhv1, lhv1pt, lhv3;
    ArrOfDouble lhg1,lhvpv ;
    ArrOfDouble lxvap, lxnc, lrnc, lmnc, lprgr;
    ArrOfDouble lfdpv, lfdhv, lgdpv, lgdhv;
    ArrOfDouble lxrsu, lxcpsu, lxcpsutg, lhi7su, lvalp, lphivmusu, lphivmusupv, lphivmusutg;
    ArrOfDouble lphixmusu[4], lphixmusupv[4], lphixmusutg[4];
    ArrOfDouble lphivmusux[4], lphixmusux[4][4];
    ArrOfDouble lprxcp[4], lprxcptg[4];
    ArrOfDouble ltlal, ltlal1, ltlal2;
    ArrOfDouble ltmul, ltmul1, ltmul2;
    ArrOfDouble ltlav, llavpv, llavtg;
    ArrOfDouble ltmuv, lmuvpv, lmuvtg;
    ArrOfDouble ltlag, llagpv, llagtg, ltlag1, ltlag3;
    ArrOfDouble ltmug, lmugpv, lmugtg, ltmug1, ltmug3;
    ArrOfDouble ltlaga, llagapv, llagatg;
    ArrOfDouble lustlagb, luslagbpv, luslagbtg;
    ArrOfDouble lsi, lsi1, lsi3, lsipv;
    ArrOfDouble ldncv, ldncv1, ldncv3, ldncvj[4];
    ArrOfDouble lcoefqv, lcoefqvx[4];
    ArrOfDouble lkiseng;
    ArrOfDouble lprandg;

    // H2
    ArrOfDouble ld2tl, ld3tl, ld2tg, ld3tg, ltg31, ltl21, lptemp, ltgh2wrk;
    ArrOfDouble lro7, lro72, lro71, lro721, lro73, lro731, ltp10, ltp101, ltp102, ltp1021, ltp1031;
    ArrOfDouble lhf, lhf1, lhfv, lhfvv, ldencvx;

    // IAPWS
    ArrOfDouble lepstliq, lepstliq1, lepstliq2;
    ArrOfDouble lepstgas, lepstgas1, lepstgas3, lepstgasx[4];
    ArrOfDouble lhlspsc, lhlspsc1, lhlspsc2, lhlspsc3;
    ArrOfDouble lhlsvsc, lhlsvsc1, lhlsvsc2, lhlsvsc3;
    ArrOfDouble lhvspsc, lhvspsc1, lhvspsc2, lhvspsc3;
    ArrOfDouble lhvsvsc, lhvsvsc1, lhvsvsc2, lhvsvsc3;
    ArrOfDouble lhlspscx[4], lhvspscx[4];
    ArrOfDouble lhlsvscx[4], lhvsvscx[4];
    ArrOfDouble lhlspscv, lhvspscpv, lhvspsctg;
    ArrOfDouble lhlsvscv, lhvsvscpv, lhvsvsctg;

    // Non condensible gas properties
    ArrOfDouble lprxcp0, lprxcp1, lprxcp2, lprxcp3, lprxcp4, lprxcp5, lprxcp6, lprxr, lprxm;
    ArrOfDouble lprxl0, lprxl1, lprxl2, lprxm0, lprxm1, lprxm2, lprxdv, lzero, lbidon;
    ArrOfDouble lx[4], lxi, lpx[4], lpvx[4], lhvx[4], ltgx[4], lrvx[4], lrgx[4], lhx[4], lrncx[4], lmncx[4];
    //aforce c2
    ArrOfDouble lhgx[4];

    ArrOfDouble lcpgx[4], ltlagx[4], ltmugx[4], lsix[4], ldncvx[4], lprxmi,lprxri;
    ArrOfDouble ltlax[4], llaxtg[4], ltmux[4], lmuxtg[4], ltmuxi, lmuxtgi;
    ArrOfDouble lphixmusuxii,lphixmusutgi, lphixmusupvi, lphixmusui,lphixmusuxv, lphivmusuxi;
    int luserq;
    double lcoefq;
//    ArrOfDouble lcoefqv;
//    ArrOfDouble lcoefqvx[4] ;
    Strings lxinnam;

    // Metastable limits
    ArrOfDouble hllim, hvlim;

    // When thermo domain is unknown
    ArrOfDouble lt, lh, lr, lcp, lmu, lla;
    ArrOfDouble lt1, lh1, lr1, lcp1, lmu1, lla1;
    ArrOfDouble lt2, lh2, lr2, lcp2, lmu2, lla2;

    //! Check boundaries for a field
    /*!
     * \param[in] in field to check
     * \param[in,out] errfield error field
     * \param[in] dom CATHARE2::liquid or CATHARE2::vapor or CATHARE2::saturated
     * \return worst error found in errfield
     */
    virtual EOS_Error verify(const EOS_Field& in,
                             EOS_Error_Field& errfield,
                             const domain dom) const;

    //! Generate EOS_Error code from partial error code
    /*!
     * \param[in] partial_code partial code defined in CATHARE2::error_code
     * \return EOS_Internal_Error (includes EOS_Error and partial code)
     */
    EOS_Internal_Error convert_eos_error(const int partial_code) const;

    //! Describe error
    /*!
     * \param[in] error the EOS_Internal_Error code
     * \param[out] description an human readable description of the error
     */
    static void describe_error(const EOS_Internal_Error error, AString & description);

    //! Map field to CATHARE2 variable
    /*!
     * \param[in] f the field to map
     * \param[in] mode the mapping mode
     * \return 1 on success, 0 on error
     */
    int map_eos_field(const EOS_Field& f, domain mode);

    //! Map fields to CATHARE2 variables
    /*!
     * \param[in] f the fields to map
     * \param[in] mode the mapping mode
     * \param[out]  existprop_fields  array of map_eos_field(const EOS_Field& f) return values
     * \return NEPTUNE::error if none of the fluid are mapped,
     *         EOS_Error::bad   if a least one fluid is mapped, but not all
     *         EOS_Error::ok    if all fluids are mapped
     */
    EOS_Error map_eos_fields(const EOS_Fields& f, vector<int>& existprop_fields, domain mode);

    //! Resize all unmapped fields
    /*!
     * \param[in] sz the target size
     * \param[in] mode the mapping mode
     */
    int unmap_eos_field(const EOS_Field& f, domain mode);

    //! map field to work ArrOfDouble (unmap from CATHARE2 variable)
    /*!
     * \param[in] f the field to unmap
     * \param[in] mode the mapping mode
     * \return the number of field unmapped (0/1)
     */

    int unmap_eos_fields(const EOS_Fields& f, domain mode);

    //! map fields to work ArrOfDouble (unmap from CATHARE2 variable)
    /*!
     * \param[in] f the filds to unmap
     * \param[in] mode the mapping mode
     * \return the number of fields unmapped
     */

    void rezise_eos_fields(int sz, domain mode) ;

    //! Convert units to SI
    /*!
     * \param[in] mode the mapping mode
     */
    void preconvert_eos_fields() ;
    //! Convert units from SI
    /*!
     * \param[in] mode the mapping mode
     */
    void postconvert_eos_fields() ;

    //! Compute all saturated variables from pressure
    /*!
     * \param[in] p pressure field
     * \param[out] out output fields
     * \param[out] err error field
     * \return worst error found in error field
     */
    EOS_Error calc2_p( const EOS_Field &p, EOS_Fields &out, EOS_Error_Field &ferr) ;

    //! Compute all saturated variables from temperature
    /*!
     * \param[in] t temperature field
     * \param[out] out output fields
     * \param[out] err error field
     * \return worst error found in error field
     */
    EOS_Error calc2_t( const EOS_Field &t, EOS_Fields &out, EOS_Error_Field &ferr) ;

    //! Compute all variables from pressure and enthalpy
    /*!
     * \param[in] p pressure field
     * \param[in] h enthalpy field
     * \param[in,out] out output fields
     * \param[in,out] err error field
     * \return worst error found in error field
     */
    EOS_Error calc2_ph( const EOS_Field &p, const EOS_Field &h, EOS_Fields &out, EOS_Error_Field &ferr) ;

    //! Compute all variables from pressure and temperature
    /*!
     * \param[in] p pressure field
     * \param[in] t temperature field
     * \param[in,out] out output fields
     * \param[in,out] err error field
     * \return worst error found in error field
     */
    EOS_Error calc2_pt( const EOS_Field &p, const EOS_Field &t, EOS_Fields &out, EOS_Error_Field &ferr) ;

    //! h(p,T,Xi)
    EOS_Internal_Error calc2_h_pT_mixing(double p, double T, double &h) ;

    //! Compute metastable enthalpy limits
    /*!
     * \return error status
     */
    EOS_Error calc2_lim() ;

    //! FHPFLD pilot (generated from Fortran)
    virtual int fhpfld(int& ill, int& ivstat, int& ivalu, domain mode) = 0 ;
    //! FTRAFLD pilot (generated from Fortran)
    virtual int ftrafld(int& ill, int& ivstat, domain mode) = 0 ;
    //! FTSATP pilot (Fortran call)
    virtual void ftsatpfld() = 0 ;
    //! FPSATT pilot (Fortran call)
    virtual void fpsattfld() = 0 ;
    //! FTLIQ pilot (Fortran call)
    virtual void ftliqfld() = 0 ;
    //! FTVAP pilot (Fortran call)
    virtual void ftvapfld() = 0 ;

    //! Compute critical point coordinates
    EOS_Error calc2_critical();

    //! Limit liquid enthalpy
    /*!
     * \param[in] p the pressure
     * \param[out] h_l_lim the limit liquid enthalpy
     * \return an error code
     */
    EOS_Internal_Error compute_h_l_lim_p( double p, double& h_l_lim ) ;

    //! Limit vapor enthalpy
    /*!
     * \param[in] p the pressure
     * \param[out] h_v_lim the limit vapor enthalpy
     * \return an error code
     */
    EOS_Internal_Error compute_h_v_lim_p( double p, double& h_v_lim ) ;

    //!  p at critical point
    EOS_Internal_Error get_p_crit(double& p_crit) ;
    //!  h at critical point
    EOS_Internal_Error get_h_crit(double& h_crit) ;
    //!  T at critical point
    EOS_Internal_Error get_T_crit(double& T_crit) ;
    //!  molar mass
    EOS_Internal_Error get_mm(double& mm) ;
    //!  p limits (min)
    EOS_Internal_Error get_p_min(double& p_min) ;
    //!  h limits (min)
    EOS_Internal_Error get_h_min(double& h_min) ;
    //!  T limits (min)
    EOS_Internal_Error get_T_min(double& T_min) ;
    //!  p limits (max)
    EOS_Internal_Error get_p_max(double& p_max) ;
    //!  h limits (max)
    EOS_Internal_Error get_h_max(double& h_max) ;
    //!  T limits (max)
    EOS_Internal_Error get_T_max(double& T_max) ;
    //!  p at reference state (for reference enthalpy)
    EOS_Internal_Error get_p_ref(double& p_ref) ;
    //!  h at reference state
    EOS_Internal_Error get_h_ref(double& h_ref) ;
    //!  T at reference state (for reference enthalpy)
    EOS_Internal_Error get_T_ref(double& T_ref) ;

    //! set LICAR flag
    EOS_Error set_icar(int icar);
    //! set LICARGAS flag
    EOS_Error set_icargas(int icargas);
    //! set LIENC flag
    EOS_Error set_ienc(int ienc);
    //! set LIREV flag
    EOS_Error set_irev(int irev);
    //! set LITERMIN flag
    EOS_Error set_itermin(int itermin);
    //! set LLATYPML flag
    EOS_Error set_latypml(int latypml);
    //! set LMUTYPML flag
    EOS_Error set_mutypml(int mutypml);
    //! set LIGOTRA flag
    EOS_Error set_igotra(int igotra);
    //! Set mixing properties
    EOS_Error set_mixing_properties(int, ArrOfDouble&, ArrOfDouble&, ArrOfDouble&, ArrOfDouble&, ArrOfDouble&,
             ArrOfDouble&, ArrOfDouble&, ArrOfDouble&, ArrOfDouble&, ArrOfDouble&, ArrOfDouble&, ArrOfDouble&,
             ArrOfDouble&, ArrOfDouble&, ArrOfDouble&, ArrOfDouble&) ;
    //! Set mixing components
    EOS_Error set_mixing_properties(const EOS_Fields&) ;
    //! No mixing
    EOS_Error set_mixing_properties() ;
  };

  inline EOS_Error CATHARE2::set_icar(int icar) 
  { if ((icar < 0) || (icar>1))  return EOS_Error::error ;
    licar = icar ;
    critical = EOS_Error::bad ;
    return EOS_Error::good ;
  }

  inline EOS_Error CATHARE2::set_icargas(int icargas)
  { if ((icargas < 0) || (icargas > 1))  return EOS_Error::error ;
    licargas = icargas ;
    critical = EOS_Error::bad ;
    return EOS_Error::good ;
  }

  inline EOS_Error CATHARE2::set_ienc(int ienc)
  { if ((ienc < 0) || (ienc > 1))  return EOS_Error::error ;
    lienc = ienc ;
    critical = EOS_Error::bad ;
    return EOS_Error::good ;
  }

  inline EOS_Error CATHARE2::set_irev(int irev)
  { if (irev < 0) return EOS_Error::error ;
    lirev = irev ;
    critical = EOS_Error::bad ;
    return EOS_Error::good ;
  }

  inline EOS_Error CATHARE2::set_itermin(int itermin) 
  { if (itermin < 0)  return EOS_Error::error ;
    litermin = itermin ;
    critical = EOS_Error::bad ;
    return EOS_Error::good ;
  }

  inline EOS_Error CATHARE2::set_latypml(int latypml)
  { if ( (latypml < 0) || (latypml > 2) )  return EOS_Error::error ;
    llatypml = latypml ;
    critical = EOS_Error::bad ;
    return EOS_Error::good ;
  }

  inline EOS_Error CATHARE2::set_mutypml(int mutypml) 
  { if ((mutypml < 0) || (mutypml > 2))  return EOS_Error::error ;
    lmutypml = mutypml ;
    critical = EOS_Error::bad ;
    return EOS_Error::good ;
  }

  inline EOS_Error CATHARE2::set_igotra(int igotra) 
  { if ((igotra < 0) || (igotra > 1))  return EOS_Error::error ;
    ligotra  = igotra ;
    critical = EOS_Error::bad ; 
    return EOS_Error::good ;
  }

  inline EOS_Error CATHARE2::set_mixing_properties(int n,
      ArrOfDouble& r,   ArrOfDouble& cp0, ArrOfDouble& cp1, ArrOfDouble& cp2,
      ArrOfDouble& cp3, ArrOfDouble& cp4, ArrOfDouble& cp5, ArrOfDouble& cp6,
      ArrOfDouble& m,   ArrOfDouble& l0,  ArrOfDouble& l1,  ArrOfDouble& l2,
      ArrOfDouble& m0,  ArrOfDouble& m1,  ArrOfDouble& m2,  ArrOfDouble& dv) 
  { nincon = n ;
    assert(r.size() == nincon);
    assert(cp0.size() == nincon);
    assert(cp1.size() == nincon);
    assert(cp2.size() == nincon);
    assert(cp3.size() == nincon);
    assert(cp4.size() == nincon);
    assert(cp5.size() == nincon);
    assert(cp6.size() == nincon);
    assert(m.size() == nincon);
    assert(l0.size() == nincon);
    assert(l1.size() == nincon);
    assert(l2.size() == nincon);
    assert(m0.size() == nincon);
    assert(m1.size() == nincon);
    assert(m2.size() == nincon);
    assert(dv.size() == nincon);
    lprxr  .set_ptr(nincon, r  .get_ptr()) ;
    lprxcp0.set_ptr(nincon, cp0.get_ptr()) ;
    lprxcp1.set_ptr(nincon, cp1.get_ptr()) ;
    lprxcp2.set_ptr(nincon, cp2.get_ptr()) ;
    lprxcp3.set_ptr(nincon, cp3.get_ptr()) ;
    lprxcp4.set_ptr(nincon, cp4.get_ptr()) ;
    lprxcp5.set_ptr(nincon, cp5.get_ptr()) ;
    lprxcp6.set_ptr(nincon, cp6.get_ptr()) ;
    lprxm  .set_ptr(nincon, m  .get_ptr()) ;
    lprxl0 .set_ptr(nincon, l0 .get_ptr()) ;
    lprxl1 .set_ptr(nincon, l1 .get_ptr()) ;
    lprxl2 .set_ptr(nincon, l2 .get_ptr()) ;
    lprxm0 .set_ptr(nincon, m0 .get_ptr()) ;
    lprxm1 .set_ptr(nincon, m1 .get_ptr()) ;
    lprxm2 .set_ptr(nincon, m2 .get_ptr()) ;
    lprxdv .set_ptr(nincon, dv .get_ptr()) ;
    return EOS_Error::good ;
  }

  inline EOS_Error CATHARE2::set_mixing_properties() 
  { nincon = 0 ;
    return EOS_Error::good ;
  }

  inline EOS_Error CATHARE2::set_mixing_properties(const EOS_Fields& C) 
  { assert (C.size() == nincon+1) ;
    nsca = C[0].size() ;
    assert (nsca > 0) ;
    for (int i=0; i<nincon+1; i++)
       { if (map_eos_field(C[i], ::CATHARE2::vapor) == 0)  return EOS_Error::error ;
       }
    return EOS_Error::good ; 
  }

}

#endif
