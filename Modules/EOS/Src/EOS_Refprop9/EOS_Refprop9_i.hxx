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



#ifndef EOS_Refprop9_i_H
#define EOS_Refprop9_i_H

namespace NEPTUNE_EOS
{

  // Inline functions

  /*
   * Units conversion functions
   * EOS parameters unit (SI)  <--->  REFPROP9 subroutine units
   *
   * e.g. :
   * pression :     Pa         <--->  KPa
   * density  :     kg/m3      <--->  mol/l
   * viscosity:     Pa.s       <--->  microPa.s
   *
   */
  inline double EOS_Refprop9::kpa2pa(double p) const
  { return 1.e3*p ;
  }

  inline double EOS_Refprop9::pa2kpa(double p) const
  { return 1.e-3*p ;
  }

  inline double EOS_Refprop9::mol2kg(double mol) const
  { return (1e-3*mol*wmm) ;  //wmm in refprop : g/mol
  }

  inline double EOS_Refprop9::kg2mol(double kg) const
  { return ((1e3*kg)/wmm) ; //wmm in refprop : g/mol

  }
  inline double EOS_Refprop9::micropa2pa(double p) const
  { return 1e-6*p ;
  }

  inline double EOS_Refprop9::pa2micropa(double p) const
  { return 1e6*p ;
  }

  inline double EOS_Refprop9::eos_rho_2_refprop(double rho) const
  { //kg/m3 ---> mol/l
    double rho_mol ;
    rho_mol = kg2mol(rho) ;
    return 1.e-3*rho_mol  ;

  }
  inline double EOS_Refprop9::refprop_rho_2_eos(double rho) const
  { //mol/l ---> kg/m3
    double rho_kg ;
    rho_kg = mol2kg(rho) ;
    return 1.e3*rho_kg   ;
  }

  inline double EOS_Refprop9::eos_nrj_2_refprop(double nrj) const
  { return (nrj/kg2mol(1.e0)) ;  //J/Kg ---> J/mol
  }

  inline double EOS_Refprop9::refprop_nrj_2_eos(double nrj) const
  { return (nrj/mol2kg(1.e0)) ;  //J/mol ---> J/Kg
  }

  //SATURATION FONCTIONS
  inline EOS_Internal_Error EOS_Refprop9::compute_rho_l_sat_p(double p, double& rho_l_sat) const
  { EOS_Internal_Error err ;
    double t, rhov, xliq, xvapint ;
    int i=1 ;
    rho_l_sat = 0.e0 ;

    double p_refprop = pa2kpa(p);
    double rho_refprop = 0.e0 ;

    //reset fluid with setup
    err = callSetup() ;
    if (err.generic_error() != EOS_Error::good)  return err ;

    F77NAME(satp_rp9)(p_refprop, arr_molfrac, i, t, rho_refprop, rhov, xliq, xvapint, ierr, herr, sizeof(herr)) ;
    if (ierr != 0)  return generate_error(ierr, herr) ;

    rho_l_sat = refprop_rho_2_eos(rho_refprop);

    return EOS_Internal_Error::OK ;
  }

  inline EOS_Internal_Error EOS_Refprop9::compute_rho_v_sat_p(double p, double& rho_v_sat) const
  { EOS_Internal_Error err ;
    double t, rhol, xliq, xvapint;
    int i=1;
    rho_v_sat = 0.e0 ;

    double p_refprop = pa2kpa(p);
    double rho_refprop = 0.e0 ;

    //reset fluid with setup
    err = callSetup() ;
    if (err.generic_error() != EOS_Error::good)  return err ;

    F77NAME(satp_rp9)(p_refprop, arr_molfrac, i, t, rhol, rho_refprop, xliq, xvapint, ierr, herr, sizeof(herr)) ;
    if (ierr != 0)  return generate_error(ierr, herr) ;

    rho_v_sat = refprop_rho_2_eos(rho_refprop);

    return EOS_Internal_Error::OK ;
  }

  inline EOS_Internal_Error EOS_Refprop9::compute_T_sat_p(double p, double& t_sat) const
  { EOS_Internal_Error err ;
    double rhol, rhov, xliq, xvapint;
    int i=1;
    t_sat = 0.e0 ;

    double p_refprop = pa2kpa(p);

    //reset fluid with setup
    err = callSetup() ;
    if (err.generic_error() != EOS_Error::good)  return err ;

    F77NAME(satp_rp9)(p_refprop, arr_molfrac, i, t_sat, rhol, rhov, xliq, xvapint, ierr, herr, sizeof(herr));
    if (ierr != 0)  return generate_error(ierr, herr) ;

    return EOS_Internal_Error::OK ;
  }

  inline EOS_Internal_Error EOS_Refprop9::compute_h_l_sat_p(double p, double& h_l_sat) const
  { EOS_Internal_Error err ;
    double t, rhol, rhov, xliq, xvapint;
    double h_refprop = 0.e0 ;
    int i=1;
    h_l_sat = 0.e0 ;

    double p_refprop = pa2kpa(p);

    //reset fluid with setup
    err = callSetup() ;
    if (err.generic_error() != EOS_Error::good)  return err ;

    F77NAME(satp_rp9)(p_refprop, arr_molfrac, i, t, rhol, rhov, xliq, xvapint, ierr, herr, sizeof(herr));
    if (ierr != 0)  return generate_error(ierr, herr) ;

    F77NAME(enthal_rp9)(t, rhol, arr_molfrac, h_refprop) ;
    h_l_sat = refprop_nrj_2_eos(h_refprop) ;

    return EOS_Internal_Error::OK ;
  }

  inline EOS_Internal_Error EOS_Refprop9::compute_h_v_sat_p(double p, double& h_v_sat) const
  { EOS_Internal_Error err ;
    double t, rhol, rhov, xliq, xvapint;
    double h_refprop = 0.e0 ;
    int i=1;
    h_v_sat = 0.e0 ;

    double p_refprop = pa2kpa(p);

    //reset fluid with setup
    err = callSetup() ;
    if (err.generic_error() != EOS_Error::good)  return err ;

    F77NAME(satp_rp9)(p_refprop, arr_molfrac, i, t, rhol, rhov, xliq, xvapint, ierr, herr, sizeof(herr));
    if (ierr != 0)  return generate_error(ierr, herr) ;

    F77NAME(enthal_rp9)(t, rhov, arr_molfrac, h_refprop) ;
    h_v_sat = refprop_nrj_2_eos(h_refprop) ;

    return EOS_Internal_Error::OK ;
  }

  inline EOS_Internal_Error EOS_Refprop9::compute_cp_l_sat_p(double p, double& cp_l_sat) const
  { EOS_Internal_Error err ;
    double t, rhol, rhov, xliq, xvapint, cp, cv;
    int i=1;
    cp_l_sat = 0.e0 ;

    double p_refprop = pa2kpa(p) ;
        
    //reset fluid with setup
    err = callSetup() ;
    if (err.generic_error() != EOS_Error::good)  return err ;

    F77NAME(satp_rp9)(p_refprop, arr_molfrac, i, t, rhol, rhov, xliq, xvapint, ierr, herr, sizeof(herr));
    if (ierr != 0)  return generate_error(ierr, herr) ;

    F77NAME(cvcp_rp9)(t, rhol, arr_molfrac, cv, cp) ;
    cp_l_sat = refprop_nrj_2_eos(cp) ;

    return EOS_Internal_Error::OK ;
  }

  inline EOS_Internal_Error EOS_Refprop9::compute_cp_v_sat_p(double p, double& cp_v_sat) const
  { EOS_Internal_Error err ;
    double t, rhol, rhov, xliq, xvapint, cp, cv;
    int i=2;
    cp_v_sat = 0.e0 ;

    double p_refprop = pa2kpa(p);
        
    //reset fluid with setup
    err = callSetup() ;
    if (err.generic_error() != EOS_Error::good)  return err ;

    F77NAME(satp_rp9)(p_refprop, arr_molfrac, i, t, rhol, rhov, xliq, xvapint, ierr, herr, sizeof(herr));
    if (ierr != 0)  return generate_error(ierr, herr) ;

    F77NAME(cvcp_rp9)(t, rhov, arr_molfrac, cv, cp) ;
    cp_v_sat = refprop_nrj_2_eos(cp);

    return EOS_Internal_Error::OK ;
  }

  inline EOS_Internal_Error EOS_Refprop9::compute_h_l_sat_T(double T, double& h_lsat) const
  { EOS_Internal_Error err ;
    double p_sat, rhol, rhov, xliq, xvapint;
    double h_refprop = 0.e0 ;
    int i = 1;
    h_lsat = 0.e0 ;

    //reset fluid with setup
    err = callSetup() ;
    if (err.generic_error() != EOS_Error::good)  return err ;

    F77NAME(satt_rp9)(T, arr_molfrac, i, p_sat, rhol, rhov, xliq, xvapint, ierr, herr, sizeof(herr));
    if (ierr != 0)  return generate_error(ierr, herr) ;

    F77NAME(enthal_rp9)(T, rhol, arr_molfrac, h_refprop);
    h_lsat = refprop_nrj_2_eos(h_refprop);

    return EOS_Internal_Error::OK ;
  }

  inline EOS_Internal_Error EOS_Refprop9::compute_h_v_sat_T(double T, double& h_vsat) const
  { EOS_Internal_Error err ;
    double p_sat, rhol, rhov, xliq, xvapint;
    double h_refprop = 0.e0 ;
    int i = 1;
    h_vsat = 0.e0 ;

    //reset fluid with setup
    err = callSetup() ;
    if (err.generic_error() != EOS_Error::good)  return err ;

    F77NAME(satt_rp9)(T, arr_molfrac, i, p_sat, rhol, rhov, xliq, xvapint, ierr, herr, sizeof(herr));
    if (ierr != 0)  return generate_error(ierr, herr) ;

    F77NAME(enthal_rp9)(T, rhov, arr_molfrac, h_refprop);
    h_vsat = refprop_nrj_2_eos(h_refprop);

    return EOS_Internal_Error::OK ;
  }


  inline EOS_Internal_Error EOS_Refprop9::compute_p_sat_T(double T, double& p_sat) const
  { EOS_Internal_Error err ;
    double rhol, rhov, xliq, xvapint;
    double p_refprop = 0.e0 ;
    int i = 1;
    p_sat = 0.e0 ;

    //reset fluid with setup
    err = callSetup() ;
    if (err.generic_error() != EOS_Error::good)  return err ;

    F77NAME(satt_rp9)(T, arr_molfrac, i, p_refprop, rhol, rhov, xliq, xvapint, ierr, herr, sizeof(herr));
    if (ierr != 0)  return generate_error(ierr, herr) ;

    p_sat = kpa2pa(p_refprop) ;

    return EOS_Internal_Error::OK ;
  }

  inline EOS_Internal_Error EOS_Refprop9::compute_d_T_sat_d_p_p(double p, double& dt_sat) const
  { EOS_Internal_Error err ;
    dt_sat = 0.e0 ;

    //reset fluid with setup
    err = callSetup() ;
    if (err.generic_error() != EOS_Error::good)  return err ;

    if (nbcomp == 1)
      { double tsat, dp_sat, p_return, rho, csat;
        int i = 1 ;

        err = compute_T_sat_p(p, tsat);
        if (err.generic_error() == EOS_Error::bad)  return err ;

        F77NAME(dptsatk_rp9)(i, tsat, i, p_return, rho, csat, dp_sat, ierr, herr, sizeof(herr));
        if (ierr != 0)  return generate_error(ierr, herr);

        if (dp_sat != 0)
          dt_sat = 1.e0/(kpa2pa(dp_sat)) ;
        else
          { char msg[] = "[DPTSATK output] Error dptsat == 0. Can compute dtpsat (= 1/dptsat)";
            return generate_error(100, msg) ;
          }
      }
    else
      { err = EOS_Fluid::compute_d_T_sat_d_p_p(p, dt_sat);
        return err ;
      }
      
    return EOS_Internal_Error::OK ;
  }


  //! h(p,T)
  inline EOS_Internal_Error EOS_Refprop9::compute_h_pT(double p, double T, double& h) const
  { return call_tpflsh("h", p, T, h) ;
  }

  //! dh/dT  (p constante)
  inline EOS_Internal_Error EOS_Refprop9::compute_d_h_d_T_p_pT(double p, double T, double& h) const
  {
    EOS_Internal_Error err;
    double rho, dhdt_d, dhdd_t, dhdd_p, dhdp_t,dhdp_d;

    //reset fluid with setup
    err = callSetup() ;
    if (err.generic_error() != EOS_Error::good)  return err ;

    err = compute_rho_pT(p,T, rho);
    if (err.generic_error() == EOS_Error::bad)  return err ;

    rho = eos_rho_2_refprop(rho) ;
    F77NAME(dhd1_rp9)(T, rho, arr_molfrac, dhdt_d, h, dhdd_t, dhdd_p, dhdp_t,dhdp_d) ;
    h = refprop_nrj_2_eos(h) ;
    return EOS_Internal_Error::OK ;
  }

  //! h(p,s)
  inline EOS_Internal_Error EOS_Refprop9::compute_h_ps(double p, double s, double& h) const
  { return call_psflsh("h", p, s, h) ;
  }

  //! rho(p,T)
  inline EOS_Internal_Error EOS_Refprop9::compute_rho_pT(double p, double T, double& rho) const
  { EOS_Internal_Error err, err2 ;
    int kph ;

    err = calrp_kph_pT(p,T,kph);
    if (err.generic_error() == EOS_Error::bad)  return err ;

    double rho_refprop = 0.e0 ;
    err2 = call_tprho_kph(p,T,kph,rho_refprop) ;
    rho = refprop_rho_2_eos(rho_refprop) ;

    return worst_internal_error(err,err2) ;
  }

  //! u(p,T)
  inline EOS_Internal_Error EOS_Refprop9::compute_u_pT(double p, double T, double& u) const
  { return call_tpflsh("u", p, T, u) ;
  }

  //! s(p,T)
  inline EOS_Internal_Error EOS_Refprop9::compute_s_pT(double p, double T, double& s) const
  { return call_tpflsh("s", p, T, s) ;
  }

  //! mu(p,T)
  inline EOS_Internal_Error EOS_Refprop9::compute_mu_pT(double p, double T, double& mu) const
  { EOS_Internal_Error err ;
    double rho, tcx ;
    mu = 0.e0 ;

    err = call_tpflsh("rho", p, T, rho) ;
    if (err.generic_error() == EOS_Error::bad)  return err ;

    rho = eos_rho_2_refprop(rho) ;
    F77NAME(trnprp_rp9)(T,rho,arr_molfrac,mu,tcx,ierr,herr, sizeof(herr));
    if (ierr != 0) return generate_error(ierr, herr);

    mu  = micropa2pa(mu) ;

    return EOS_Internal_Error::OK ;
  }

  //! lambda(p,T)
  inline EOS_Internal_Error EOS_Refprop9::compute_lambda_pT(double p, double T, double& lambda) const
  { EOS_Internal_Error err ;
    double rho, eta;
    lambda = 0.e0 ;

    err = call_tpflsh("rho", p, T, rho) ;
    if (err.generic_error() == EOS_Error::bad)  return err ;

    rho = eos_rho_2_refprop(rho);
    F77NAME(trnprp_rp9)(T,rho,arr_molfrac,eta,lambda,ierr,herr, sizeof(herr));
    if (ierr != 0) return generate_error(ierr, herr);

    return EOS_Internal_Error::OK ;
  }

  //! cp(p,T)
  inline EOS_Internal_Error EOS_Refprop9::compute_cp_pT(double p, double T, double& cp) const
  { return call_tpflsh("cp", p, T, cp);
  }

  //! sigma(p,T)
  inline EOS_Internal_Error EOS_Refprop9::compute_sigma_pT(double p, double T, double& sigma) const
  { EOS_Internal_Error err ;
    double rho, rhol, rhov, q, e, h, s, cv, cp, w;
    std::vector<double> xl(nbcomp), xv(nbcomp);

    double p_refprop = pa2kpa(p) ;

    //reset fluid with setup
    err = callSetup() ;
    if (err.generic_error() != EOS_Error::good)  return err ;

    F77NAME(tpflsh_rp9)(T,p_refprop,arr_molfrac, rho, rhol, rhov, xl.data(), xv.data(), q, e, h, s, cv, cp, w, ierr, herr, sizeof(herr));
    if (ierr!=0)  return generate_error(ierr, herr) ;

    F77NAME(surten_rp9)(T,rhol,rhov,xl.data(),xv.data(),sigma,ierr,herr, sizeof(herr));
    if (ierr!=0)  return generate_error(ierr, herr) ;

    return EOS_Internal_Error::OK ;
  }

  //! w(p,T)
  inline EOS_Internal_Error EOS_Refprop9::compute_w_pT(double p, double T, double& w) const
  { return call_tpflsh("w", p, T, w);
  }

  //! T(p,h)
  inline EOS_Internal_Error EOS_Refprop9::compute_T_ph(double p, double h, double& T) const
  { EOS_Internal_Error err ;
    int kph ;

    err = calrp_kph(p,h,kph);
    if (err.generic_error() == EOS_Error::bad)  return err ;

    err =  call_phfl1(p,h,kph,T) ;
    return err ;
  }

  //! rho(p,h)
  inline EOS_Internal_Error EOS_Refprop9::compute_rho_ph(double p, double h, double& rho) const
  { EOS_Internal_Error err, err2, err3 ;
    int kph ;

    err = calrp_kph(p,h,kph);
    if (err.generic_error() == EOS_Error::bad)  return err ;

    double T ;
    err2 = compute_T_ph(p,h,T) ;
    err3 = calrp_rho_pT_kph(p,T,kph,rho) ;

    return worst_internal_error(err,worst_internal_error(err2,err3));
  }

  //! u(p,h)
  inline EOS_Internal_Error EOS_Refprop9::compute_u_ph(double p, double h, double& u) const
  { return call_phflsh("u", p, h, u) ;
  }

  //! cp(p,h)
  inline EOS_Internal_Error EOS_Refprop9::compute_cp_ph(double p, double h, double& cp) const
  { return call_phflsh("cp", p, h, cp) ;
  }

  //! s(p,h)
  inline EOS_Internal_Error EOS_Refprop9::compute_s_ph(double p, double h, double& s) const
  { return call_phflsh("s", p, h, s) ;
  }

  //! w(p,h)
  inline EOS_Internal_Error EOS_Refprop9::compute_w_ph(double p, double h, double& w) const
  { return call_phflsh("w", p, h, w) ;
  }

  //! cv(p,h)
  inline EOS_Internal_Error EOS_Refprop9::compute_cv_ph(double p, double h, double& cv) const
  { return call_phflsh("cv", p, h, cv) ;
  }

  //! lambda(p,h)
  inline EOS_Internal_Error EOS_Refprop9::compute_lambda_ph(double p, double h, double& lambda) const
  { EOS_Internal_Error err;
    double rho, t, eta;

    lambda = 0.e0 ;

    err = call_phflsh("rho", p, h, rho) ;
    if (err.generic_error() == EOS_Error::bad)  return err ;

    err = call_phflsh("T", p, h, t) ;
    if (err.generic_error() == EOS_Error::bad)  return err ;

    rho = eos_rho_2_refprop(rho);
    F77NAME(trnprp_rp9)(t,rho,arr_molfrac,eta,lambda,ierr,herr, sizeof(herr));
    if (ierr!=0) return generate_error(ierr, herr) ;

    return EOS_Internal_Error::OK ;
  }

  //! beta(p,h)
  inline EOS_Internal_Error EOS_Refprop9::compute_beta_ph(double p, double h, double& beta) const
  { double rho, t, pi, e, s, cv, cp, w, Z ;
        double hjt,A, G, xkappa,dPdrho,d2PdD2,dPT,drhodT,drhodP,spare1,spare2,spare3, spare4;
    EOS_Internal_Error err ;

    err = call_phflsh("rho", p, h, rho) ;
    if (err.generic_error() == EOS_Error::bad)  return err;

    err = call_phflsh("T", p, h, t);
    if (err.generic_error() == EOS_Error::bad)  return err;

    rho = eos_rho_2_refprop(rho) ;
    F77NAME(therm2_rp9)(t,rho,arr_molfrac,pi,e,h,s,cv,cp,w, Z, hjt,A, G, xkappa, beta,
                    dPdrho,d2PdD2,dPT,drhodT,drhodP,spare1,spare2,spare3, spare4);
//    F77NAME(trnprp_rp9)(t,rho,arr_molfrac,eta,lambda,ierr,herr, sizeof(herr));

    return EOS_Internal_Error::OK ;
  }

  //! mu(p,h)
  inline EOS_Internal_Error EOS_Refprop9::compute_mu_ph(double p, double h, double& mu) const
  { double rho, t, tcx;
    EOS_Internal_Error err ;
    mu = 0.e0 ;

    err = call_phflsh("rho", p, h, rho) ;
    if (err.generic_error() == EOS_Error::bad)  return err ;

    err = call_phflsh("T", p, h, t) ;
    if (err.generic_error() == EOS_Error::bad)  return err ;

    rho = eos_rho_2_refprop(rho) ;
    F77NAME(trnprp_rp9)(t,rho,arr_molfrac,mu,tcx,ierr,herr, sizeof(herr)) ;
    if (ierr!=0) return generate_error(ierr, herr) ;
    
    mu  = micropa2pa(mu) ;
    return EOS_Internal_Error::OK ;
  }
  //! sigma(p,h)
  inline EOS_Internal_Error
  EOS_Refprop9::compute_sigma_ph(double p, double h, double& sigma) const
  { EOS_Internal_Error err ;
    double t, rho, rhol, rhov, q, e, s, cv, cp, w;
    std::vector<double> xl(nbcomp), xv(nbcomp) ;

    double p_refprop = pa2kpa(p) ;
    double h_refprop = eos_nrj_2_refprop(h) ;

    //reset fluid with setup
    err = callSetup() ;
    if (err.generic_error() != EOS_Error::good)  return err ;

    F77NAME(phflsh_rp9)(p_refprop,h_refprop, arr_molfrac, t, rho, rhol, rhov, xl.data(), xv.data(), q, e, s, cv, cp, w, ierr, herr, sizeof(herr)) ;
    if (ierr!=0) return generate_error(ierr, herr) ;

    F77NAME(surten_rp9)(t,rhol,rhov,xl.data(),xv.data(),sigma,ierr,herr, sizeof(herr)) ;
    if (ierr!=0) return generate_error(ierr, herr) ;

    return EOS_Internal_Error::OK ;
  }

  //! d(rho)/dp    at constant specific enthalpy
  inline EOS_Internal_Error EOS_Refprop9::compute_d_rho_d_p_h_ph(double p, double h, double& drhodp_h) const
  { double t, rho, drhodp_t, drhodt_p, dtdp_h;
    EOS_Internal_Error err ;

    err = compute_rho_ph(p,h, rho) ;
    if (err.generic_error() == EOS_Error::bad)  return err ;

    err = compute_T_ph(p,h,t) ;
    if (err.generic_error() == EOS_Error::bad)  return err ;

    rho = eos_rho_2_refprop(rho) ;

    F77NAME(dddp_rp9)(t, rho, arr_molfrac, drhodp_t) ;
    F77NAME(dddt_rp9)(t, rho, arr_molfrac, drhodt_p) ;

    err = compute_d_T_d_p_h_ph(p,h,dtdp_h) ;
    if (err.generic_error() == EOS_Error::bad)  return err ;

    drhodp_h = (refprop_rho_2_eos(drhodp_t)/kpa2pa(1)) + (refprop_rho_2_eos(drhodt_p)*dtdp_h);

    return EOS_Internal_Error::OK ;
  }

  //! d(rho)/dh      at constant pressure
  inline EOS_Internal_Error EOS_Refprop9::compute_d_rho_d_h_p_ph(double p, double h, double& drhodh_p) const
  { double t, rho, drhodt_p, dtdh_p;
    EOS_Internal_Error err ;

    err = compute_rho_ph(p,h, rho) ;
    if (err.generic_error() == EOS_Error::bad)  return err ;

    err = compute_T_ph(p,h,t) ;
    if (err.generic_error() == EOS_Error::bad)  return err ;

    rho = eos_rho_2_refprop(rho) ;

    F77NAME(dddt_rp9)(t, rho, arr_molfrac, drhodt_p) ;
    
    err = compute_d_T_d_h_p_ph(p,h,dtdh_p) ;
    if (err.generic_error() == EOS_Error::bad)  return err;

    drhodh_p = refprop_rho_2_eos(drhodt_p)*dtdh_p ;

    return EOS_Internal_Error::OK ;
  }

  //! d(T)/dp      at constant specific enthalpy
  inline EOS_Internal_Error EOS_Refprop9::compute_d_T_d_p_h_ph(double p, double h, double& dtdp) const
  { return call_dhd1("d_T_d_p_h", p, h, dtdp) ;
  }

  //! d(T)/dh      at constant pressure
  inline EOS_Internal_Error EOS_Refprop9::compute_d_T_d_h_p_ph(double p, double h, double& dtdh) const
  { return call_dhd1("d_T_d_h_p", p, h, dtdh) ;
  }

  //! g(p,T)
  inline EOS_Internal_Error EOS_Refprop9::compute_g_pT(double p, double T, double& g) const
  { double rho ;
    EOS_Internal_Error err ;

    err = compute_rho_pT(p,T,rho) ;
    if (err.generic_error() == EOS_Error::bad)  return err;

    return call_ag("g",T,rho,g) ;
  }

  //! g(p,h)
  inline EOS_Internal_Error EOS_Refprop9::compute_g_ph(double p, double h, double& g) const
  { double rho,T ;
    EOS_Internal_Error err ;

    err = compute_rho_ph(p,h,rho) ;
    if (err.generic_error() == EOS_Error::bad)  return err;

    err = compute_T_ph(p,h,T) ;
    if (err.generic_error() == EOS_Error::bad)  return err;

    return call_ag("g",T,rho,g) ;
  }

  //! f(p,T)
  inline EOS_Internal_Error EOS_Refprop9::compute_f_pT(double p, double T, double& f) const
  { double rho ;
    EOS_Internal_Error err ;

    err = compute_rho_pT(p,T,rho) ;
    if (err.generic_error() == EOS_Error::bad)  return err ;

    return call_ag("f",T,rho,f) ;
  }

  //! f(p,h)
  inline EOS_Internal_Error EOS_Refprop9::compute_f_ph(double p, double h, double& f) const
  { double rho,T ;
    EOS_Internal_Error err ;

    err = compute_rho_ph(p,h,rho) ;
    if (err.generic_error() == EOS_Error::bad)  return err ;

    err = compute_T_ph(p,h,T) ;
    if (err.generic_error() == EOS_Error::bad)  return err ;

    return call_ag("f",T,rho,f) ;
  }

  //! tcrit
  inline EOS_Internal_Error EOS_Refprop9::get_T_crit(double& T_crit) const
  { T_crit = tcrit ;
    return EOS_Internal_Error::OK ;
  }

  //! pcrit
  inline EOS_Internal_Error EOS_Refprop9::get_p_crit(double& p_crit) const
  { p_crit = pcrit ;
    return EOS_Internal_Error::OK ;
  }

  //! rhocrit
  inline EOS_Internal_Error EOS_Refprop9::get_rho_crit(double& rho_crit) const
  { rho_crit = rhocrit;
    return EOS_Internal_Error::OK ;
  }

  //! hcrit
  inline EOS_Internal_Error EOS_Refprop9::get_h_crit(double& h_crit) const
  { h_crit = hcrit ;
    return EOS_Internal_Error::OK ;
  }

  //! limits values
  inline EOS_Internal_Error EOS_Refprop9::get_T_min(double& T_min) const
  { T_min = tmin ;
    return EOS_Internal_Error::OK ;
  }

  inline EOS_Internal_Error EOS_Refprop9::get_T_max(double& T_max) const
  { T_max = tmax ;
    return EOS_Internal_Error::OK ;
  }
//   inline EOS_Internal_Error
//   EOS_Refprop9::get_p_min(double& p_min) const
//   {
//     p_min = pmin;
//     return EOS_Internal_Error::OK ;
//   }

  inline EOS_Internal_Error EOS_Refprop9::get_p_max(double& p_max) const
  { p_max = pmax ;
    return EOS_Internal_Error::OK ;
  }

  inline EOS_Internal_Error EOS_Refprop9::get_rho_max(double& rho_max) const
  { rho_max = rhomax ;
    return EOS_Internal_Error::OK ;
  }
//   inline EOS_Internal_Error
//   EOS_Refprop9::get_h_min(double& h_min) const
//   {
//     h_min = hmin;
//     return EOS_Internal_Error::OK ;
//   }

  inline EOS_Internal_Error EOS_Refprop9::get_h_max(double& h_max) const
  { h_max = hmax ;
    return EOS_Internal_Error::OK ;
  }
  //! Molar mass (kg/mol)
  inline EOS_Internal_Error EOS_Refprop9::get_mm(double& mm) const
  { mm = 1.e-3*wmm ;
    return EOS_Internal_Error::OK ;
  }

  inline EOS_Internal_Error EOS_Refprop9::compute_h_l_lim_p(double p, double& h_l) const
  { double temp_sat, rhol_sat ;
    double bid0, bid1, bid2 ;
    int iph = 1 ;

    EOS_Internal_Error err ;

    double p_refprop = pa2kpa(p) ;
    double h_refprop = 0.e0 ;

    F77NAME(satp_rp9)(p_refprop, arr_molfrac, iph, temp_sat, rhol_sat, bid0, bid1, bid2, ierr, herr, sizeof(herr)) ;
    if (ierr == 0)
       F77NAME(liqspndl_rp9)(temp_sat, arr_molfrac, rhol_sat, ierr, herr, sizeof(herr)) ;
    else
      { err = generate_error(ierr, herr) ;
        return err ;
      }
    F77NAME(enthal_rp9)(temp_sat, rhol_sat, arr_molfrac, h_refprop) ;

    h_l = refprop_nrj_2_eos(h_refprop) ;

    return EOS_Internal_Error::OK ;
  }

  inline EOS_Internal_Error EOS_Refprop9::compute_h_v_lim_p(double p, double& h_v) const
  { double temp_sat, rhov_sat ;
    double bid0, bid1, bid2 ;
    int iph = 1 ;
    EOS_Internal_Error err ;

    double p_refprop = pa2kpa(p) ;
    double h_refprop = 0.e0 ;

    F77NAME(satp_rp9)(p_refprop, arr_molfrac, iph, temp_sat, bid0, rhov_sat, bid1, bid2, ierr, herr, sizeof(herr)) ;
    if (ierr == 0)
       F77NAME(vapspndl_rp9)(temp_sat, arr_molfrac, rhov_sat, ierr, herr, sizeof(herr)) ;
    else
      { err = generate_error(ierr, herr) ;
        return err ;
      } 
    F77NAME(enthal_rp9)(temp_sat, rhov_sat, arr_molfrac, h_refprop) ;
   
    h_v = refprop_nrj_2_eos(h_refprop) ;

    return EOS_Internal_Error::OK ;
  }

  //! Compute REFPROP phase flag (1=liquid ; 2=vapor)
  inline EOS_Internal_Error EOS_Refprop9::calrp_kph(double p, double h, int& kph) const
  {
    EOS_Internal_Error err ;
    if (p >= pcrit)
      { kph=1;
        return EOS_Internal_Error::OK ;
      }
    else
      { double hllim,hvlim,hlvlim;
        EOS_Internal_Error errl = compute_h_l_lim_p(p,hllim) ;
        EOS_Internal_Error errv = compute_h_v_lim_p(p,hvlim) ;
        err  = worst_internal_error(errl, errv) ;
        if (err.generic_error() != EOS_Error::good)  return err ;

        hlvlim = 0.5*(hllim+hvlim) ;  
        if (h < hlvlim)
          kph=1 ;
        else
          kph=2 ;

        return err ;
      }
  }

  //! Compute REFPROP phase flag (1=liquid ; 2=vapor) in (p,T)
  inline EOS_Internal_Error EOS_Refprop9::calrp_kph_pT(double p, double T, int& kph) const
  { EOS_Internal_Error err ;

    if (p >= pcrit)
      { kph=1 ;
        return EOS_Internal_Error::OK ;
      }
    else
      { double tsat;
        err = compute_T_sat_p(p,tsat);
        if (T <= tsat)
          kph=1 ;
        else
          kph=2 ;
        return err ;
      }
  }

  //! rho(p,T,kph) with REFPROP phase flag (1=liquid ; 2=vapor)
  inline EOS_Internal_Error EOS_Refprop9::calrp_rho_pT_kph(double p, double T, int kph, double& rho) const
  { double rho_refprop = 0.e0 ;
    EOS_Internal_Error err = call_tprho_kph(p,T,kph,rho_refprop);
    rho = refprop_rho_2_eos(rho_refprop);
    return err ;
  }

  //! call_phfl1  : T(p,h) with the phase flag kph (1=liquid ; 2=vapor)
  inline EOS_Internal_Error EOS_Refprop9::call_phfl1(double p, double h, int kph, double& T) const
  { EOS_Internal_Error err ;
    double p_refprop = pa2kpa(p);
    double h_refprop = eos_nrj_2_refprop(h);
    double rho_refprop = 0.e0 ;
    T = 0.e0 ;
        
    //reset fluid with setup
    err = callSetup() ;
    if (err.generic_error() != EOS_Error::good)  return err ;
  
    F77NAME(phfl1_rp9)(p_refprop, h_refprop, arr_molfrac, kph , T, rho_refprop, ierr, herr, sizeof(herr));
    if (ierr != 0)  return generate_error(ierr, herr) ;
  
    return EOS_Internal_Error::OK ;
  }

  //! call_tprho : rho(p,T) with the phase flag kph (1=liquid ; 2=vapor) - rho return with refprop unit
  inline EOS_Internal_Error EOS_Refprop9::call_tprho_kph(double p, double T, int kph, double& rho_refprop) const
  { EOS_Internal_Error err ;
    //reset fluid with setup
    err = callSetup() ;
    if (err.generic_error() != EOS_Error::good)  return err ;

    int j = 0 ;
    int mkph = -kph ;
    double p_refprop = pa2kpa(p) ;

    /*
     * REFPROP9 TPRHO WARNING:
     * Invalid densities will be returned for T & P outside range of validity,
     * i.e., pressure > melting pressure, pressure less than saturation
     * pressure for kph=1, etc.
     */
    F77NAME(tprho_rp9)(T, p_refprop, arr_molfrac, mkph, j, rho_refprop, ierr, herr, sizeof(herr)) ;
    if (ierr != 0) return generate_error(ierr, herr) ;

    return EOS_Internal_Error::OK ;
  }
}
#endif
