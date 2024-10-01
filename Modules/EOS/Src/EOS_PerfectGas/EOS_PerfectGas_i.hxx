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



#ifndef EOS_PerfectGas_i_H
#define EOS_PerfectGas_i_H

namespace NEPTUNE_EOS
{
  //  provisional for EOS_Mixing
  inline double EOS_PerfectGas::get_prxr()
  { return prxr ;
  }

  //  provisional for EOS_Mixing
  inline double EOS_PerfectGas::get_prxcp()
  { return prxcp ;
  }

  //  provisional for EOS_Mixing
  inline double EOS_PerfectGas::get_prxm()
  { return prxm ;
  }

  //  provisional for EOS_Mixing
  inline double EOS_PerfectGas::get_prxl0()
  { return prxl0 ;
  }

  //  provisional for EOS_Mixing
  inline double EOS_PerfectGas::get_prxl1()
  { return prxl1 ;
  }

  //  provisional for EOS_Mixing
  inline double EOS_PerfectGas::get_prxl2()
  { return prxl2 ;
  }

  //  provisional for EOS_Mixing
  inline double EOS_PerfectGas::get_prxm0()
  { return prxm0 ;
  }

  //  provisional for EOS_Mixing
  inline double EOS_PerfectGas::get_prxm1()
  { return prxm1 ;
  }

  //  provisional for EOS_Mixing
  inline double EOS_PerfectGas::get_prxm2()
  { return prxm2 ;
  }

  //  provisional for EOS_Mixing
  inline double EOS_PerfectGas::get_prxdv()
  { return prxdv ;
  }

  // molar mass (kg/mol)   (prxm en g/mol)
  inline EOS_Internal_Error EOS_PerfectGas::get_mm(double& mm ) const
  { mm = prxm*1.e-3;
    return EOS_Internal_Error::OK ;
  }
  inline EOS_Internal_Error EOS_PerfectGas::compute_T_ph(double p, double h, double& T) const
  { (void)p ;
    T = (h-href)/cp + Tref ;
    return EOS_Internal_Error::OK ;
  }

  inline EOS_Internal_Error EOS_PerfectGas::compute_h_pT(double p, double T, double& h) const
  { (void)p ;
    h = href + cp*(T - Tref) ;
    return EOS_Internal_Error::OK ;
  }

  inline EOS_Internal_Error EOS_PerfectGas::compute_rho_ph(double p, double h, double& rho) const
  { double T = 1.e0 ;
    EOS_Internal_Error err=compute_T_ph(p, h, T) ;
    rho = p/(r*T) ;
    return err ;
  }

  inline EOS_Internal_Error EOS_PerfectGas::compute_rho_pT(double p, double T, double& rho) const
  { rho = p /(r*T) ;
    return EOS_Internal_Error::OK ;
  }

  inline EOS_Internal_Error EOS_PerfectGas::compute_cp_ph(double p, double h, double& Cp) const
  { (void)p ; (void)h ;
    Cp = cp ;
    return EOS_Internal_Error::OK ;
  }

  inline EOS_Internal_Error EOS_PerfectGas::compute_cp_pT(double p, double T, double& Cp) const
  { (void)p ; (void)T ;
    Cp = cp ;
    return EOS_Internal_Error::OK ;
  }

  inline EOS_Internal_Error EOS_PerfectGas::compute_mu_ph(double p, double h, double& mu) const
  { double T = 1.e0 ;
    EOS_Internal_Error err = compute_T_ph(p, h, T) ;
    EOS_Internal_Error err2 = compute_mu_pT(p,T,mu) ;
    return worst_internal_error(err, err2) ;
  }

  inline EOS_Internal_Error EOS_PerfectGas::compute_mu_pT(double p, double T, double& mu) const
  { (void)p ;
    mu = prxm0+T*(prxm1+T*prxm2) ;
    return EOS_Internal_Error::OK ;
  }

  inline EOS_Internal_Error EOS_PerfectGas::compute_d_mu_d_T_p_pT(double p, double T, double& d_mu_d_T_p) const
  { (void)p ;
    d_mu_d_T_p = prxm1+2.e0*T*prxm2 ;
    return EOS_Internal_Error::OK ;
  }

  inline EOS_Internal_Error EOS_PerfectGas::compute_lambda_ph(double p, double h, double& lambda) const
  { double T = 1.e0 ;
    EOS_Internal_Error err  = compute_T_ph(p, h, T) ;
    EOS_Internal_Error err2 = compute_lambda_pT(p,T,lambda) ;
    return worst_internal_error(err, err2) ;
  }

  inline EOS_Internal_Error EOS_PerfectGas::compute_lambda_pT(double p, double T, double& lambda) const
  { (void)p ;
    lambda = prxl0+T*(prxl1+T*prxl2) ;
    return EOS_Internal_Error::OK ;
  }

  inline EOS_Internal_Error EOS_PerfectGas::compute_d_lambda_d_T_p_pT(double p, double T, double& d_lambda_d_T_p) const
  { (void)p ;
    d_lambda_d_T_p = prxl1+2.e0*T*prxl2 ;
    return EOS_Internal_Error::OK ;
  }
}

#endif
