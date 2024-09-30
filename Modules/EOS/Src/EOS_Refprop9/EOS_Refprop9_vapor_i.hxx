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



/*
 * EOS_Refprop9_vapor_i.hxx
 *
 *  Created on: 13 avr. 2010
 *  
 */

#ifndef EOS_REFPROP9_VAPOR_I_HXX_
#define EOS_REFPROP9_VAPOR_I_HXX_

namespace NEPTUNE_EOS
{

  inline EOS_Internal_Error EOS_Refprop9_vapor::compute_T_sat_p(double p, double& t_sat) const
  { EOS_Internal_Error err ;
    double rhol, rhov, xliq, xvapint ;
    int i=2;
    t_sat = 0.e0 ;

    double p_refprop = pa2kpa(p);

    //reset fluid with setup
    err = callSetup();
    if (err.generic_error() != EOS_Error::good) return err;
 
    int ierr;
    char herr[HC255];
    memset(herr, '\0', sizeof(herr));

    F77NAME(satp_rp9)(p_refprop, arr_molfrac, i, t_sat, rhol, rhov, xliq, xvapint, ierr, herr, sizeof(herr));
    if (ierr != 0) return generate_error(ierr,herr);

    return EOS_Internal_Error::OK;
  }

  inline EOS_Internal_Error EOS_Refprop9_vapor::compute_h_l_sat_T(double T, double& h_lsat) const
  { EOS_Internal_Error err ;
    double p_sat, rhol, rhov, xliq, xvapint;
    double h_refprop = 0.e0 ;
    int i = 2 ;
    h_lsat = 0.e0 ;

    //reset fluid with setup
    err = callSetup();
    if (err.generic_error() != EOS_Error::good)  return err;
 
    int ierr;
    char herr[HC255];
    memset(herr, '\0', sizeof(herr));

    F77NAME(satt_rp9)(T, arr_molfrac, i, p_sat, rhol, rhov, xliq, xvapint, ierr, herr, sizeof(herr));
    if (ierr != 0)  return generate_error(ierr, herr);

    F77NAME(enthal_rp9)(T, rhol, arr_molfrac, h_refprop);
    h_lsat = refprop_nrj_2_eos(h_refprop);

    return EOS_Internal_Error::OK;
  }

  inline EOS_Internal_Error EOS_Refprop9_vapor::compute_h_v_sat_T(double T, double& h_vsat) const
  { EOS_Internal_Error err ;
    double p_sat, rhol, rhov, xliq, xvapint;
    double h_refprop = 0.e0 ;
    int i = 2 ;
    h_vsat = 0.e0 ;

    //reset fluid with setup
    err = callSetup();
    if (err.generic_error() != EOS_Error::good)  return err;
 
    int ierr;
    char herr[HC255];
    memset(herr, '\0', sizeof(herr));

    F77NAME(satt_rp9)(T, arr_molfrac, i, p_sat, rhol, rhov, xliq, xvapint, ierr, herr, sizeof(herr));
    if (ierr != 0)  return generate_error(ierr, herr);

    F77NAME(enthal_rp9)(T, rhov, arr_molfrac, h_refprop);
    h_vsat = refprop_nrj_2_eos(h_refprop);

    return EOS_Internal_Error::OK;
  }


  inline EOS_Internal_Error EOS_Refprop9_vapor::compute_p_sat_T(double T, double& p_sat) const
  { EOS_Internal_Error err ;
    double rhol, rhov, xliq, xvapint;
    double p_refprop = 0.e0 ;
    int i=2;
    p_sat = 0.e0 ;

    //reset fluid with setup
    err = callSetup();
    if (err.generic_error() != EOS_Error::good) return err;
 
    int ierr;
    char herr[HC255];
    memset(herr, '\0', sizeof(herr));

    F77NAME(satt_rp9)(T, arr_molfrac, i, p_refprop, rhol, rhov, xliq, xvapint, ierr, herr, sizeof(herr));
    if (ierr != 0)  return generate_error(ierr, herr);

    p_sat = kpa2pa(p_refprop) ;

    return EOS_Internal_Error::OK ;
  }


  //! call tprho - rho return with refprop unit
  inline EOS_Internal_Error EOS_Refprop9_vapor::call_tprho(double p, double T, double& rho_refprop) const
  { EOS_Internal_Error err ;
    int i = -2;
    int j = 0;
    rho_refprop = 0.e0 ;

    double p_refprop = pa2kpa(p);

    //reset fluid with setup
    err = callSetup() ;
    if (err.generic_error() != EOS_Error::good) return err;

    /*
     * REFPROP9 TPRHO WARNING:
     * Invalid densities will be returned for T & P outside range of validity,
     * i.e., pressure > melting pressure, pressure less than saturation
     * pressure for kph=1, etc.
     */
 
    int ierr;
    char herr[HC255];
    memset(herr, '\0', sizeof(herr));

    F77NAME(tprho_rp9)(T, p_refprop, arr_molfrac, i , j, rho_refprop, ierr, herr, sizeof(herr));
    if (ierr != 0) return generate_error(ierr, herr);

    return EOS_Internal_Error::OK ;
  }

  //! rho(p,T)
//   inline EOS_Internal_Error
//   EOS_Refprop9_vapor::compute_rho_pT(double p, double T, double& rho) const
//   {
//     EOS_Internal_Error err;
//     double rho_refprop = 0.e0 ;
//     err = call_tprho(p,T,rho_refprop);
//     if (err.generic_error() == EOS_Error::bad)
//       return err;
//     rho = refprop_rho_2_eos(rho_refprop);
//     return EOS_Internal_Error::OK;
//   }

  //! h(p,T)
  inline EOS_Internal_Error EOS_Refprop9_vapor::compute_h_pT(double p, double T, double& h) const
  { EOS_Internal_Error err;

    double rho_refprop = 0.e0 ;
    err = call_tprho(p,T,rho_refprop);
    if (err.generic_error() == EOS_Error::bad) return err;

    return call_therm("h",T,rho_refprop,h);
  }
  //! h(p,s)
/*  inline EOS_Internal_Error
  EOS_Refprop9_vapor::compute_h_ps(double p, double s, double& h) const
  {
    int kph = 2;
    double p_refprop = pa2kpa(p);
    double s_refprop = eos_nrj_2_refprop(s);
    double rho_refprop = 0.e0 ;
        double T = 0.0;

        //reset fluid with setup
    EOS_Internal_Error errSetup = callSetup();
 
    int ierr;
    char herr[HC255];
    memset(herr, '\0', sizeof(herr));
    
    F77NAME(psfl1_rp9)(p_refprop,s_refprop,arr_molfrac,kph,T,rho_refprop,ierr,herr, sizeof(herr));
        
    if (ierr != 0)
      return generate_error(ierr, herr);

    return call_therm("h",T,rho_refprop,h);
  }*/
  //! u(p,T)

  inline EOS_Internal_Error EOS_Refprop9_vapor::compute_u_pT(double p, double T, double& u) const
  { EOS_Internal_Error err;

    double rho_refprop = 0.e0 ;
    err = call_tprho(p,T,rho_refprop);
    if (err.generic_error() == EOS_Error::bad) return err;

    return call_therm("u",T,rho_refprop,u);
  }

  //! u(p,h)
  inline EOS_Internal_Error EOS_Refprop9_vapor::compute_u_ph(double p, double h, double& u) const
  { double T;
    EOS_Internal_Error err;
    
    err = compute_T_ph(p,h,T);
    if (err.generic_error() == EOS_Error::bad) return err;   
          
    double rho_refprop = 0.e0 ;
    err = call_tprho(p,T,rho_refprop);
    if (err.generic_error() == EOS_Error::bad) return err;

    return call_therm("u",T,rho_refprop,u);
  }

  //! s(p,T)
  inline EOS_Internal_Error EOS_Refprop9_vapor::compute_s_pT(double p, double T, double& s) const
  { EOS_Internal_Error err;

    double rho_refprop = 0.e0 ;
    err = call_tprho(p,T,rho_refprop);
    if (err.generic_error() == EOS_Error::bad) return err;

    return call_therm("s",T,rho_refprop,s);
  }

  //! s(p,h)
  inline EOS_Internal_Error EOS_Refprop9_vapor::compute_s_ph(double p, double h, double& s) const
  { double T;
    EOS_Internal_Error err;
    
    err = compute_T_ph(p,h,T);
    if (err.generic_error() == EOS_Error::bad) return err;   

    double rho_refprop = 0.e0 ;
    err = call_tprho(p,T,rho_refprop);
    if (err.generic_error() == EOS_Error::bad) return err;

    return call_therm("s",T,rho_refprop,s);
  }

  //! cp(p,T)
  inline EOS_Internal_Error EOS_Refprop9_vapor::compute_cp_pT(double p, double T, double& cp) const
  { EOS_Internal_Error err;

    double rho_refprop = 0.e0 ;
    err = call_tprho(p,T,rho_refprop);
    if (err.generic_error() == EOS_Error::bad) return err;

    return call_therm("cp",T,rho_refprop,cp);
  } 

   //! cp(p,h)
  inline EOS_Internal_Error EOS_Refprop9_vapor::compute_cp_ph(double p, double h, double& cp) const
  { double T;
    EOS_Internal_Error err;
    
    err = compute_T_ph(p,h,T);
    if (err.generic_error() == EOS_Error::bad) return err;   
      
    double rho_refprop = 0.e0 ;
    err = call_tprho(p,T,rho_refprop);
    if (err.generic_error() == EOS_Error::bad) return err;    

    return call_therm("cp",T,rho_refprop,cp);
  }

   //! cv(p,h)
  inline EOS_Internal_Error EOS_Refprop9_vapor::compute_cv_ph(double p, double h, double& cv) const
  { double T;
    EOS_Internal_Error err;
    
    err = compute_T_ph(p,h,T);
    if (err.generic_error() == EOS_Error::bad) return err;   
      
    double rho_refprop = 0.e0 ;
    err = call_tprho(p,T,rho_refprop);
    if (err.generic_error() == EOS_Error::bad) return err;    

    return call_therm("cv",T,rho_refprop,cv);
  }

   //! beta(p,h)
  inline EOS_Internal_Error EOS_Refprop9_vapor::compute_beta_ph(double p, double h, double& beta) const
  { double T;
    double pi,e,s,cv,cp,w, Z, hjt,A, G, xkappa,dPdrho,d2PdD2,dPT,drhodT,drhodP,spare1,spare2,spare3, spare4;
    EOS_Internal_Error err;
    
    err = compute_T_ph(p,h,T);
    if (err.generic_error() == EOS_Error::bad) return err;   
      
    double rho_refprop = 0.e0 ;
    err = call_tprho(p,T,rho_refprop);
    if (err.generic_error() == EOS_Error::bad) return err;    

    F77NAME(therm2_rp9)(T,rho_refprop,arr_molfrac,pi,e,h,s,cv,cp,w, Z, hjt,A, G, xkappa, beta,dPdrho,d2PdD2,dPT,drhodT,drhodP,spare1,spare2,spare3, spare4);

    return EOS_Internal_Error::OK;
  }    

  //! w(p,T)
  inline EOS_Internal_Error EOS_Refprop9_vapor::compute_w_pT(double p, double T, double& w) const
  { EOS_Internal_Error err ;

    double rho_refprop = 0.e0 ;
    err = call_tprho(p,T,rho_refprop);
    if (err.generic_error() == EOS_Error::bad) return err ;

    return call_therm("w",T,rho_refprop,w);
  }

  //! w(p,h)
  inline EOS_Internal_Error EOS_Refprop9_vapor::compute_w_ph(double p, double h, double& w) const
  { double T;
    EOS_Internal_Error err ;

    err = compute_T_ph(p,h,T) ;
    if (err.generic_error() == EOS_Error::bad) return err ;   
      
    double rho_refprop = 0.e0 ;
    err = call_tprho(p,T,rho_refprop) ;
    if (err.generic_error() == EOS_Error::bad) return err;    

    return call_therm("w",T,rho_refprop,w) ;
  }

  //! mu(p,T)
  inline EOS_Internal_Error EOS_Refprop9_vapor::compute_mu_pT(double p, double T, double& mu) const
  { EOS_Internal_Error err ;
    double tcx;
    mu = 0.e0 ;

    double rho_refprop = 0.e0 ;
    err = call_tprho(p,T,rho_refprop);
    if (err.generic_error() == EOS_Error::bad) return err ;

    int ierr;
    char herr[HC255];
    memset(herr, '\0', sizeof(herr));

    F77NAME(trnprp_rp9)(T,rho_refprop,arr_molfrac,mu,tcx,ierr,herr, sizeof(herr)) ;
    if (ierr != 0) return generate_error(ierr, herr) ;

    mu = micropa2pa(mu) ;

    return EOS_Internal_Error::OK ;
  }

  //! mu(p,h)
  inline EOS_Internal_Error EOS_Refprop9_vapor::compute_mu_ph(double p, double h, double& mu) const
  { EOS_Internal_Error err ;
    double T,tcx ;
    mu = 0.e0 ;
    
    err = compute_T_ph(p,h,T);
    if (err.generic_error() == EOS_Error::bad) return err;   
      
    double rho_refprop = 0.e0 ;
    err = call_tprho(p,T,rho_refprop);
    if (err.generic_error() == EOS_Error::bad) return err;

    int ierr;
    char herr[HC255];
    memset(herr, '\0', sizeof(herr));

    F77NAME(trnprp_rp9)(T,rho_refprop,arr_molfrac,mu,tcx,ierr,herr, sizeof(herr));
    if (ierr != 0) return generate_error(ierr, herr);

    mu = micropa2pa(mu) ;
  
    return EOS_Internal_Error::OK ;
  }

  //! lambda(p,T)
  inline EOS_Internal_Error EOS_Refprop9_vapor::compute_lambda_pT(double p, double T, double& lambda) const
  { EOS_Internal_Error err ;
    double eta;
    lambda = 0.e0 ;

    double rho_refprop = 0.e0 ;
    err = call_tprho(p,T,rho_refprop);
    if (err.generic_error() == EOS_Error::bad) return err;

    int ierr;
    char herr[HC255];
    memset(herr, '\0', sizeof(herr));

    F77NAME(trnprp_rp9)(T,rho_refprop,arr_molfrac,eta,lambda,ierr,herr, sizeof(herr));
    if (ierr != 0) return generate_error(ierr, herr);

    return EOS_Internal_Error::OK ;
  }

  //! lambda(p,h)
  inline EOS_Internal_Error EOS_Refprop9_vapor::compute_lambda_ph(double p, double h, double& lambda) const
  { EOS_Internal_Error err;
    double T,eta;
    lambda = 0.e0 ;
    
    err = compute_T_ph(p,h,T);
    if (err.generic_error() == EOS_Error::bad) return err;   

    double rho_refprop = 0.e0 ;
    err = call_tprho(p,T,rho_refprop);
    if (err.generic_error() == EOS_Error::bad) return err;

    int ierr;
    char herr[HC255];
    memset(herr, '\0', sizeof(herr));

    F77NAME(trnprp_rp9)(T,rho_refprop,arr_molfrac,eta,lambda,ierr,herr, sizeof(herr));
    if (ierr != 0) return generate_error(ierr, herr);

    return EOS_Internal_Error::OK ;
  }

  //! dh/dT  (p constante)
  inline EOS_Internal_Error EOS_Refprop9_vapor::compute_d_h_d_T_p_pT(double p, double T, double& h) const
  { EOS_Internal_Error err ;
    double rho, dhdt_d, dhdd_t, dhdd_p, dhdp_t,dhdp_d ;

    err = compute_rho_pT(p,T, rho);
    if (err.generic_error() == EOS_Error::bad) return err;

    rho = eos_rho_2_refprop(rho) ;
    F77NAME(dhd1_rp9)(T, rho, arr_molfrac, dhdt_d, h, dhdd_t, dhdd_p, dhdp_t,dhdp_d) ;
    h = refprop_nrj_2_eos(h) ;

    return EOS_Internal_Error::OK ;
  }

  inline EOS_Internal_Error EOS_Refprop9_vapor::compute_d_T_sat_d_p_p(double p, double& dt_sat) const
  { EOS_Internal_Error err ;
    dt_sat = 0.e0 ;

    if (nbcomp == 1)
      { double tsat, dp_sat, p_return, rho, csat ;
        int i = 2;
        
        err = compute_T_sat_p(p, tsat);

        if (err.generic_error() == EOS_Error::bad) return err;

        int ierr;
        char herr[HC255];
        memset(herr, '\0', sizeof(herr));

        F77NAME(dptsatk_rp9)(nbcomp, tsat, i, p_return, rho, csat, dp_sat, ierr, herr, sizeof(herr));
        if (ierr != 0)  return generate_error(ierr, herr);

        if (dp_sat != 0)
          dt_sat = 1.e0/kpa2pa(dp_sat);
        else
          { char msg[] = "[DPTSATK output] Error dptsat == 0. Can compute dtpsat (= 1/dptsat)";
            return generate_error(100, msg);
          }
      }
    else
      { err = EOS_Fluid::compute_d_T_sat_d_p_p(p, dt_sat);
        return err;
      }

    return EOS_Internal_Error::OK ;
  }

  inline EOS_Internal_Error EOS_Refprop9_vapor::is_metastable(double rho, double p, double T, bool& meta) const
  { EOS_Internal_Error err ;
    double h, rho2;

    err = compute_h_pT(p,T,h);
    if (err.generic_error() == EOS_Error::bad)  return err;

    err = compute_rho_ph(p,h, rho2);
    if (err.generic_error() == EOS_Error::bad)  return err;

    if (rho == rho2)
      meta = 0 ;
    else
      meta = 1 ;

    return EOS_Internal_Error::OK ;
  }

  //! Compute REFPROP phase flag (1=liquid ; 2=vapor)
  inline EOS_Internal_Error EOS_Refprop9_vapor::calrp_kph(double p, double h, int& kph) const
  { (void)p ; (void)h ;
    kph = 2 ;
    return EOS_Internal_Error::OK ;
  }

  //! Compute REFPROP phase flag (1=liquid ; 2=vapor) in (p,T)
  inline EOS_Internal_Error EOS_Refprop9_vapor::calrp_kph_pT(double p, double T, int& kph) const
  { (void)p ; (void)T ;
    kph = 2 ;
    return EOS_Internal_Error::OK ;
  }
}
#endif /* EOS_REFPROP9_VAPOR_I_HXX_ */
