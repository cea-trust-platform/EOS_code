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



#ifndef EOS_StiffenedGas_i_H
#define EOS_StiffenedGas_i_H

namespace NEPTUNE_EOS
{
  const EOS_Internal_Error STIFF_GAS_NEWTON_FAILED = EOS_Internal_Error(101, bad) ;


  // p ...
  inline EOS_Internal_Error EOS_StiffenedGas::compute_p_sat_T(double T, double& psat) const
  { int nb_iter_max = 50 ;
    int iteration_Newton ;
    double epsn = 1.e-3 ;
    double func = 1.e0 ;
    double dfunc, deltap ;
    double k = exp(SGa+SGb/T+SGc*log(T)) ;
    double p = 1.e+6 ;
    
    for(iteration_Newton = 0;
        (iteration_Newton < nb_iter_max) && (fabs(func) > epsn);
        iteration_Newton++ )
      { func   = p+SGpinfv-k*pow(p+SGpinfl,SGd);
        dfunc  = 1.-SGd*k*pow(p+SGpinfl,SGd-1.);
        deltap = -func/dfunc;
        p      = p+deltap;
      }
    if (iteration_Newton>=nb_iter_max && (fabs(func) > epsn) )  return STIFF_GAS_NEWTON_FAILED ;
    psat = p ;
    return EOS_Internal_Error::OK ;
  }


  // h ...
  inline EOS_Internal_Error EOS_StiffenedGas::compute_h_pT(double p, double T, double& h) const
  { (void)p ;
    h = SGgamma*SGcv*T+SGq ;
    return EOS_Internal_Error::OK ;
  }

  inline EOS_Internal_Error EOS_StiffenedGas::compute_h_v_sat_p(double p, double& hvsat) const
  { double T = 1.e0 ;
    EOS_Internal_Error err=compute_T_sat_p(p,T) ;    
    hvsat = SGgammav*SGcvv*T+SGqv ;
    return err ;
  }

  inline EOS_Internal_Error EOS_StiffenedGas::compute_h_l_sat_p(double p, double& hlsat) const
  { double T = 1.e0 ;
    EOS_Internal_Error err=compute_T_sat_p(p,T) ;   
    hlsat = SGgammal*SGcvl*T+SGql ;
    return err ;
  }


  // T ...
  inline EOS_Internal_Error EOS_StiffenedGas::compute_T_sat_p(double p, double& Tsat) const
  { int nb_iter_max = 50 ;
    int iteration_Newton ;
    double epsn = 1.e-3 ;
    double func = 1.e0 ;
    double dfunc, deltaT ;
    double v = log(p+SGpinfv)-SGd*log(p+SGpinfl)-SGa ;
    double T = 300.e0 ;

    for(iteration_Newton = 0;
        (iteration_Newton < nb_iter_max) && (fabs(func) > epsn); 
        iteration_Newton++ ) 
      { func   = SGb/T+SGc*log(T)-v ;
        dfunc  = (SGc - SGb/T)/T ;
        deltaT = -func/dfunc ;
        T      = T+deltaT ;
      }
    if (iteration_Newton>=nb_iter_max && (fabs(func) > epsn) )  return STIFF_GAS_NEWTON_FAILED ;
    Tsat = T ;
    return EOS_Internal_Error::OK ;
  }

  inline EOS_Internal_Error EOS_StiffenedGas::compute_T_ph(double p, double h, double& T) const
  { (void)p ;
    T = (h-SGq)/(SGgamma*SGcv) ;
    return EOS_Internal_Error::OK ;
  }


  // rho ...
  inline EOS_Internal_Error EOS_StiffenedGas::compute_rho_pT(double p, double T, double& rho) const
  { rho = (p+SGpinf)/((SGgamma-1.e0)*SGcv*T) ;
    return EOS_Internal_Error::OK ;
  }

  inline EOS_Internal_Error EOS_StiffenedGas::compute_rho_ph(double p, double h, double& rho) const
  { double T = 1.e0 ;
    EOS_Internal_Error err = compute_T_ph(p,h,T) ;
    if (err.generic_error() <= EOS_Error::ok)  return compute_rho_pT(p,T,rho) ;
    return err ;
  }

  inline EOS_Internal_Error EOS_StiffenedGas::compute_rho_l_sat_p(double p, double& rholsat) const
  { double T = 1.e0;
    EOS_Internal_Error err = compute_T_sat_p(p,T) ;
    rholsat = (p+SGpinfl)/((SGgammal-1.e0)*SGcvl*T) ;
    return err ;
  }

  inline EOS_Internal_Error EOS_StiffenedGas::compute_rho_v_sat_p(double p, double& rhovsat) const
  { double T = 1.e0 ;
    EOS_Internal_Error err=compute_T_sat_p(p,T) ;
    rhovsat = (p+SGpinfv)/((SGgammav-1.e0)*SGcvv*T) ;
    return err ;
  }


  // cp ..
  inline EOS_Internal_Error EOS_StiffenedGas::compute_cp_ph(double p, double h, double& cp) const
  { (void)p ; (void)h ;
    cp = SGcp ;
    return EOS_Internal_Error::OK ;
  }

  inline EOS_Internal_Error EOS_StiffenedGas::compute_cp_pT(double p, double T, double& cp) const
  { (void)p ; (void)T ;
    cp = SGcp ;
    return EOS_Internal_Error::OK ;
  }

  inline EOS_Internal_Error EOS_StiffenedGas::compute_d_cp_d_p_h_ph(double p, double h, double& dcpdp) const
  { (void)p ; (void)h ;
    dcpdp =0.e0 ;
    return EOS_Internal_Error::OK ; 
  }

  inline EOS_Internal_Error EOS_StiffenedGas::compute_d_cp_d_h_p_ph(double p, double h, double& dcpdh) const
  { (void)p ; (void)h ;
    dcpdh = 0.e0 ;
    return EOS_Internal_Error::OK ; 
  }


  // u ...
  inline EOS_Internal_Error EOS_StiffenedGas::compute_u_ph(double p, double h, double& u) const
  { double T ;
    EOS_Internal_Error err=compute_T_ph(p,h,T) ;
    if (err.generic_error() <= EOS_Error::ok)  return compute_u_pT(p,T,u) ;
    return err ; 
  }

  inline EOS_Internal_Error EOS_StiffenedGas::compute_u_pT(double p, double T, double& u) const
  { double rho ;
    EOS_Internal_Error err = compute_rho_pT(p,T,rho) ;
    u = (p+SGgamma*SGpinf)/(rho*(SGgamma-1.e0))+SGq ;
    return err ;
  }


  // s ...
  inline EOS_Internal_Error EOS_StiffenedGas::compute_s_pT(double p, double T, double& s) const
  { s = pow(T,SGgamma)/pow(p+SGpinf,SGgamma-1.e0) ;
    s = SGcv*log(s)+SGqp ;
    return EOS_Internal_Error::OK ; 
  }


  // w ...
  inline EOS_Internal_Error EOS_StiffenedGas::compute_w_pT(double p, double T, double& w) const
  { (void)p ;
    w = SGgamma*(SGgamma-1.e0)*SGcv*T ;
    w = pow(w,0.5e0) ;
    return EOS_Internal_Error::OK ; 
  }

  inline EOS_Internal_Error EOS_StiffenedGas::compute_w_ph(double p, double h, double& w) const
  { double T = 1.0e0 ;
    EOS_Internal_Error err = compute_T_ph(p,h,T) ;
    if (err.generic_error() <= EOS_Error::ok)  return compute_w_pT(p,T,w) ;
    return err ;
  }



  // s ...
  inline EOS_Internal_Error EOS_StiffenedGas::compute_s_ph(double p, double h, double& s) const
  { double T = 1.e0 ;
    EOS_Internal_Error err=compute_T_ph(p,h,T) ;
    if (err.generic_error() <= EOS_Error::ok)  return compute_s_pT(p,T,s) ;
    return err ;
  }
}
#endif
