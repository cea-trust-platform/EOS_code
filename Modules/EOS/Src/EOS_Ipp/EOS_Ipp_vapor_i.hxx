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



//
#ifndef EOS_IPP_VAPOR_I_HXX_
#define EOS_IPP_VAPOR_I_HXX_

namespace NEPTUNE_EOS
{
  inline EOS_Internal_Error EOS_Ipp_vapor::compute_h_pT(double p, double T, double& res) const
     { EOS_Internal_Error ierr = check_p_bounds_ph(p);
       if (ierr == OUT_OF_BOUNDS)  return ierr;
       return compute_h_v_pT(p,T,res) ;
     }

  inline EOS_Internal_Error EOS_Ipp_vapor::compute_rho_pT(double p, double T, double& res) const
     { EOS_Internal_Error ierr, ierr2;
       double h;
       ierr  = compute_h_pT(p,T,h);
       if (ierr == EOS_Internal_Error::OK)  ierr2 = compute_rho_ph(p,h,res);
       return worst_internal_error(ierr,ierr2) ;
     }

  inline EOS_Internal_Error EOS_Ipp_vapor::compute_u_pT(double p, double T, double& res) const
     { EOS_Internal_Error ierr, ierr2;
       double h;
       ierr  = compute_h_pT(p,T,h);
       if (ierr == EOS_Internal_Error::OK)  ierr2 = compute_u_ph(p,h,res);
       return worst_internal_error(ierr,ierr2) ;
     }

  inline EOS_Internal_Error EOS_Ipp_vapor::compute_s_pT(double p, double T, double& res) const
     { EOS_Internal_Error ierr, ierr2;
       double h;
       ierr  = compute_h_pT(p,T,h);
       if (ierr == EOS_Internal_Error::OK)  ierr2 = compute_s_ph(p,h,res);
       return worst_internal_error(ierr,ierr2) ;
     }

  inline EOS_Internal_Error EOS_Ipp_vapor::compute_mu_pT(double p, double T, double& res) const
     { EOS_Internal_Error ierr, ierr2;
       double h;
       ierr  = compute_h_pT(p,T,h);
       if (ierr == EOS_Internal_Error::OK)  ierr2 = compute_mu_ph(p,h,res);
       return worst_internal_error(ierr,ierr2) ;
     }

  inline EOS_Internal_Error EOS_Ipp_vapor::compute_lambda_pT(double p, double T, double& res) const
     { EOS_Internal_Error ierr, ierr2;
       double h;
       ierr  = compute_h_pT(p,T,h);
       if (ierr == EOS_Internal_Error::OK)  ierr2 = compute_lambda_ph(p,h,res);
       return worst_internal_error(ierr,ierr2) ;
     }

  inline EOS_Internal_Error EOS_Ipp_vapor::compute_cp_pT(double p, double T, double& res) const
     { EOS_Internal_Error ierr, ierr2;
       double h;
       ierr  = compute_h_pT(p,T,h);
       if (ierr == EOS_Internal_Error::OK)  ierr2 = compute_cp_ph(p,h,res);
       return worst_internal_error(ierr,ierr2) ;
     }

  inline EOS_Internal_Error EOS_Ipp_vapor::compute_sigma_pT(double p, double T, double& res) const
     { EOS_Internal_Error ierr, ierr2;
       double h;
       ierr  = compute_h_pT(p,T,h);
       if (ierr == EOS_Internal_Error::OK)  ierr2 = compute_sigma_ph(p,h,res);
       return worst_internal_error(ierr,ierr2) ;
     }

  inline EOS_Internal_Error EOS_Ipp_vapor::compute_w_pT(double p, double T, double& res) const
     { EOS_Internal_Error ierr, ierr2;
       double h;
       ierr  = compute_h_pT(p,T,h);
       if (ierr == EOS_Internal_Error::OK)  ierr2 = compute_w_ph(p,h,res);
       return worst_internal_error(ierr,ierr2) ;
     }

  inline EOS_Internal_Error EOS_Ipp_vapor::compute_g_pT(double p, double T, double& res) const
     { EOS_Internal_Error ierr, ierr2;
       double h;
       ierr  = compute_h_pT(p,T,h);
       if (ierr == EOS_Internal_Error::OK)  ierr2 = compute_g_ph(p,h,res);
       return worst_internal_error(ierr,ierr2) ;
     }

  inline EOS_Internal_Error EOS_Ipp_vapor::compute_f_pT(double p, double T, double& res) const
     { EOS_Internal_Error ierr, ierr2;
       double h;
       ierr  = compute_h_pT(p,T,h);
       if (ierr == EOS_Internal_Error::OK)  ierr2 = compute_f_ph(p,h,res);
       return worst_internal_error(ierr,ierr2) ;
     }

  inline EOS_Internal_Error EOS_Ipp_vapor::compute_pr_pT(double p, double T, double& res) const
     { EOS_Internal_Error ierr, ierr2;
       double h;
       ierr  = compute_h_pT(p,T,h);
       if (ierr == EOS_Internal_Error::OK)  ierr2 = compute_pr_ph(p,h,res);
       return worst_internal_error(ierr,ierr2) ;
     }

  inline EOS_Internal_Error EOS_Ipp_vapor::compute_beta_pT(double p, double T, double& res) const
     { EOS_Internal_Error ierr, ierr2;
       double h;
       ierr  = compute_h_pT(p,T,h);
       if (ierr == EOS_Internal_Error::OK)  ierr2 = compute_beta_ph(p,h,res);
       return worst_internal_error(ierr,ierr2) ;
     }

  inline EOS_Internal_Error EOS_Ipp_vapor::compute_gamma_pT(double p, double T, double& res) const
     { EOS_Internal_Error ierr, ierr2;
       double h;
       ierr  = compute_h_pT(p,T,h);
       if (ierr == EOS_Internal_Error::OK)  ierr2 = compute_gamma_ph(p,h,res);
       return worst_internal_error(ierr,ierr2) ;
     }

}

#endif /* EOS_IPP_VAPOR_I_HXX_ */
