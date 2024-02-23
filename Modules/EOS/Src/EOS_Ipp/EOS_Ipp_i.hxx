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
#ifndef EOS_IPP_I_HXX_
#define EOS_IPP_I_HXX_

namespace NEPTUNE_EOS
{
   //! tcrit
   inline EOS_Internal_Error EOS_Ipp::get_T_crit(double& T_crit) const
   { T_crit = tcrit ;
     return EOS_Internal_Error::OK ;
   }

   //! pcrit
   inline EOS_Internal_Error EOS_Ipp::get_p_crit(double& p_crit) const
   { p_crit = pcrit ;
     return EOS_Internal_Error::OK ;
   }

   //! hcrit
   inline EOS_Internal_Error EOS_Ipp::get_h_crit(double& h_crit) const
   { h_crit = hcrit ;
     return EOS_Internal_Error::OK ;
   }
   
   //limits values
   inline EOS_Internal_Error EOS_Ipp::get_T_min(double& T_min) const
   { T_min = tmin ;
     return EOS_Internal_Error::OK ;
   }

   inline EOS_Internal_Error EOS_Ipp::get_T_max(double& T_max) const
   { T_max = tmax ;    
     return EOS_Internal_Error::OK ;
   }

   inline EOS_Internal_Error EOS_Ipp::get_p_min(double& p_min) const
   { p_min = pmin ;
     return EOS_Internal_Error::OK ;
   }

   inline EOS_Internal_Error EOS_Ipp::get_p_max(double& p_max) const
   { p_max = pmax ;
     return EOS_Internal_Error::OK ;
   }

   inline EOS_Internal_Error EOS_Ipp::get_h_min(double& h_min) const
   { h_min = hmin ;
     return EOS_Internal_Error::OK ;
   }

   inline EOS_Internal_Error EOS_Ipp::get_h_max(double& h_max) const
   { h_max = hmax ;
     return EOS_Internal_Error::OK ;
   }
   
   inline EOS_Internal_Error EOS_Ipp::compute_T_ph(double p, double h, double& res) const
   { AString prop("T") ;
     return compute_prop_ph(prop,p,h,res) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_d_T_d_p_h_ph(double p, double h, double& res) const
   { AString prop("d_T_d_p_h") ;
     return compute_prop_ph(prop,p,h,res) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_d_T_d_h_p_ph(double p, double h, double& res) const
   { AString prop("d_T_d_h_p") ;
     return compute_prop_ph(prop,p,h,res) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_rho_ph(double p, double h, double& res) const
   { AString prop("rho") ;
     return compute_prop_ph(prop,p,h,res) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_rho_pT(double p, double T, double& res) const
   { EOS_Internal_Error err, err2 ;
     double h ;
     err  = compute_h_pT(p,T,h) ;
     if (err == EOS_Internal_Error::OK)  err2 = compute_rho_ph(p,h,res) ;
     return worst_internal_error(err,err2) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_d_rho_d_p_h_ph(double p, double h, double& res) const
   { AString prop("d_rho_d_p_h") ;
     return compute_prop_ph(prop,p,h,res) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_d_rho_d_h_p_ph(double p, double h, double& res) const
   { AString prop("d_rho_d_h_p") ;
     return compute_prop_ph(prop,p,h,res) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_u_ph(double p, double h, double& res) const
   { AString prop("u");
     return compute_prop_ph(prop,p,h,res);
   }

   inline EOS_Internal_Error EOS_Ipp::compute_u_pT(double p, double T, double& res) const
   { EOS_Internal_Error err, err2 ;
     double h ;
     err  = compute_h_pT(p,T,h) ;
     if (err == EOS_Internal_Error::OK)  err2 = compute_u_ph(p,h,res) ;
     return worst_internal_error(err,err2) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_d_u_d_p_h_ph(double p, double h, double& res) const
   { AString prop("d_u_d_p_h") ;
     return compute_prop_ph(prop,p,h,res) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_d_u_d_h_p_ph(double p, double h, double& res) const
   { AString prop("d_u_d_h_p") ;
     return compute_prop_ph(prop,p,h,res) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_s_ph(double p, double h, double& res) const
   { AString prop("s") ;
     return compute_prop_ph(prop,p,h,res) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_s_pT(double p, double T, double& res) const
   { EOS_Internal_Error err, err2 ;
     double h ;
     err  = compute_h_pT(p,T,h) ;
     if (err == EOS_Internal_Error::OK)  err2 = compute_s_ph(p,h,res) ;
     return worst_internal_error(err,err2) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_d_s_d_p_h_ph(double p, double h, double& res) const
   { AString prop("d_s_d_p_h") ;
     return compute_prop_ph(prop,p,h,res) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_d_s_d_h_p_ph(double p, double h, double& res) const
   { AString prop("d_s_d_h_p") ;
     return compute_prop_ph(prop,p,h,res) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_mu_ph(double p, double h, double& res) const
   { AString prop("mu") ;
     return compute_prop_ph(prop,p,h,res) ;
   }
   inline EOS_Internal_Error EOS_Ipp::compute_mu_pT(double p, double T, double& res) const
   { EOS_Internal_Error err, err2 ;
     double h ;
     err  = compute_h_pT(p,T,h) ;
     if (err == EOS_Internal_Error::OK)  err2 = compute_mu_ph(p,h,res) ;
     return worst_internal_error(err,err2) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_d_mu_d_p_h_ph(double p, double h, double& res) const
   { AString prop("d_mu_d_p_h") ;
     return compute_prop_ph(prop,p,h,res) ;
   }
   inline EOS_Internal_Error EOS_Ipp::compute_d_mu_d_h_p_ph(double p, double h, double& res) const
   { AString prop("d_mu_d_h_p") ;
     return compute_prop_ph(prop,p,h,res) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_lambda_ph(double p, double h, double& res) const
   { AString prop("lambda") ;
     return compute_prop_ph(prop,p,h,res) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_lambda_pT(double p, double T, double& res) const
   { EOS_Internal_Error err, err2 ;
     double h ;
     err  = compute_h_pT(p,T,h) ;
     if (err == EOS_Internal_Error::OK)  err2 = compute_lambda_ph(p,h,res) ;
     return worst_internal_error(err,err2) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_d_lambda_d_p_h_ph(double p, double h, double& res) const
   { AString prop("d_lambda_d_p_h") ;
     return compute_prop_ph(prop,p,h,res) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_d_lambda_d_h_p_ph(double p, double h, double& res) const
   { AString prop("d_lambda_d_h_p") ;
     return compute_prop_ph(prop,p,h,res) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_cp_ph(double p, double h, double& res) const
   { AString prop("cp") ;
     return compute_prop_ph(prop,p,h,res) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_cp_pT(double p, double T, double& res) const
   { EOS_Internal_Error err, err2 ;
     double h ;
     err  = compute_h_pT(p,T,h) ;
     if (err == EOS_Internal_Error::OK)  err2 = compute_cp_ph(p,h,res) ;
     return worst_internal_error(err,err2) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_d_cp_d_p_h_ph(double p, double h, double& res) const
   { AString prop("d_cp_d_p_h");
     return compute_prop_ph(prop,p,h,res);
   }

   inline EOS_Internal_Error EOS_Ipp::compute_d_cp_d_h_p_ph(double p, double h, double& res) const
   { AString prop("d_cp_d_h_p") ;
     return compute_prop_ph(prop,p,h,res) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_sigma_ph(double p, double h, double& res) const
   { AString prop("sigma") ;
     return compute_prop_ph(prop,p,h,res) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_sigma_pT(double p, double T, double& res) const
   { EOS_Internal_Error err, err2 ;
     double h ;
     err  = compute_h_pT(p,T,h) ;
     if (err == EOS_Internal_Error::OK)  err2 = compute_sigma_ph(p,h,res) ;
     return worst_internal_error(err,err2) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_d_sigma_d_p_h_ph(double p, double h, double& res) const
   { AString prop("d_sigma_d_p_h") ;
     return compute_prop_ph(prop,p,h,res) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_d_sigma_d_h_p_ph(double p, double h, double& res) const
   { AString prop("d_sigma_d_h_p") ;
     return compute_prop_ph(prop,p,h,res) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_w_ph(double p, double h, double& res) const
   { AString prop("w") ;
     return compute_prop_ph(prop,p,h,res) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_w_pT(double p, double T, double& res) const
   { EOS_Internal_Error err, err2 ;
     double h ;
     err  = compute_h_pT(p,T,h) ;
     if (err == EOS_Internal_Error::OK)  err2 = compute_w_ph(p,h,res) ;
     return worst_internal_error(err,err2) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_d_w_d_p_h_ph(double p, double h, double& res) const
   { AString prop("d_w_d_p_h") ;
     return compute_prop_ph(prop,p,h,res) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_d_w_d_h_p_ph(double p, double h, double& res) const
   { AString prop("d_w_d_h_p") ;
     return compute_prop_ph(prop,p,h,res) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_g_ph(double p, double h, double& res) const
   { AString prop("g") ;
     return compute_prop_ph(prop,p,h,res) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_g_pT(double p, double T, double& res) const
   { EOS_Internal_Error err, err2 ;
     double h ;
     err  = compute_h_pT(p,T,h) ;
     if (err == EOS_Internal_Error::OK)  err2 = compute_g_ph(p,h,res) ;
     return worst_internal_error(err,err2) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_d_g_d_p_h_ph(double p, double h, double& res) const
   { AString prop("d_g_d_p_h") ;
     return compute_prop_ph(prop,p,h,res) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_d_g_d_h_p_ph(double p, double h, double& res) const
   { AString prop("d_g_d_h_p") ;
     return compute_prop_ph(prop,p,h,res) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_f_ph(double p, double h, double& res) const
   { AString prop("f") ;
     return compute_prop_ph(prop,p,h,res) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_f_pT(double p, double T, double& res) const
   { EOS_Internal_Error err, err2 ;
     double h ;
     err  = compute_h_pT(p,T,h) ;
     if (err == EOS_Internal_Error::OK)  err2 = compute_f_ph(p,h,res) ;
     return worst_internal_error(err,err2) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_d_f_d_p_h_ph(double p, double h, double& res) const
   { AString prop("d_f_d_p_h") ;
     return compute_prop_ph(prop,p,h,res) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_d_f_d_h_p_ph(double p, double h, double& res) const
   { AString prop("d_f_d_h_p") ;
     return compute_prop_ph(prop,p,h,res) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_pr_ph(double p, double h, double& res) const
   { AString prop("pr") ;
     return compute_prop_ph(prop,p,h,res) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_pr_pT(double p, double T, double& res) const
   { EOS_Internal_Error err, err2 ;
     double h ;
     err  = compute_h_pT(p,T,h) ;
     if (err == EOS_Internal_Error::OK)  err2 = compute_pr_ph(p,h,res) ;
     return worst_internal_error(err,err2) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_d_pr_d_p_h_ph(double p, double h, double& res) const
   { AString prop("d_pr_d_p_h") ;
     return compute_prop_ph(prop,p,h,res) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_d_pr_d_h_p_ph(double p, double h, double& res) const
   { AString prop("d_pr_d_h_p") ;
     return compute_prop_ph(prop,p,h,res) ;
   }
   inline EOS_Internal_Error EOS_Ipp::compute_beta_ph(double p, double h, double& res) const
   { AString prop("beta");
     return compute_prop_ph(prop,p,h,res) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_beta_pT(double p, double T, double& res) const
   { EOS_Internal_Error err, err2 ;
     double h ;
     err  = compute_h_pT(p,T,h) ;
     if (err == EOS_Internal_Error::OK)  err2 = compute_beta_ph(p,h,res) ;
     return worst_internal_error(err,err2) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_d_beta_d_p_h_ph(double p, double h, double& res) const
   { AString prop("d_beta_d_p_h") ;
     return compute_prop_ph(prop,p,h,res) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_d_beta_d_h_p_ph(double p, double h, double& res) const
   { AString prop("d_beta_d_h_p") ;
     return compute_prop_ph(prop,p,h,res) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_gamma_ph(double p, double h, double& res) const
   { AString prop("gamma") ;
     return compute_prop_ph(prop,p,h,res) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_gamma_pT(double p, double T, double& res) const
   { EOS_Internal_Error err, err2 ;
     double h ;
     err  = compute_h_pT(p,T,h) ;
     if (err == EOS_Internal_Error::OK)  err2 = compute_gamma_ph(p,h,res) ;
     return worst_internal_error(err,err2) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_d_gamma_d_p_h_ph(double p, double h, double& res) const
   { AString prop("d_gamma_d_p_h") ;
     return compute_prop_ph(prop,p,h,res) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_d_gamma_d_h_p_ph(double p, double h, double& res) const
   { AString prop("d_gamma_d_h_p") ;
     return compute_prop_ph(prop,p,h,res) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_T_sat_p(double p, double& res) const
   { AString prop("T_sat") ;
     return compute_prop_p(prop,p,0,res) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_rho_l_sat_p(double p, double& res) const
   { double hlsat ;
     EOS_Internal_Error err1 = compute_h_l_sat_p(p,hlsat) ;
     EOS_Internal_Error err2 = compute_rho_ph(p,hlsat,res) ;
     return worst_internal_error(err1,err2) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_rho_v_sat_p(double p, double& res) const
   { double hvsat;
     EOS_Internal_Error err1 = compute_h_v_sat_p(p,hvsat);
     EOS_Internal_Error err2 = compute_rho_ph(p,hvsat,res);
     return worst_internal_error(err1,err2);
   }

   inline EOS_Internal_Error EOS_Ipp::compute_cp_l_sat_p(double p, double& res) const
   { double hlsat;
     EOS_Internal_Error err1 = compute_h_l_sat_p(p,hlsat);
     EOS_Internal_Error err2 = compute_cp_ph(p,hlsat,res);
     return worst_internal_error(err1,err2);
   }

   inline EOS_Internal_Error EOS_Ipp::compute_cp_v_sat_p(double p, double& res) const
   { double hvsat;
     EOS_Internal_Error err1 = compute_h_v_sat_p(p,hvsat);
     EOS_Internal_Error err2 = compute_cp_ph(p,hvsat,res);
     return worst_internal_error(err1,err2);
   }

   inline EOS_Internal_Error EOS_Ipp::compute_h_l_sat_p(double p, double& res) const
   { double tsat ;
     EOS_Internal_Error err = check_p_bounds_satlim(p) ;
     if (err==OUT_OF_BOUNDS)  return err ;
     EOS_Internal_Error err1 = compute_T_sat_p(p, tsat) ;
     EOS_Internal_Error err2 = compute_h_l_pT(p,tsat,res) ;
     return worst_internal_error(err1, err2) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_h_v_sat_p(double p, double& res) const
   { double tsat ;
     EOS_Internal_Error err = check_p_bounds_satlim(p) ;
     if (err==OUT_OF_BOUNDS)  return err ;
     EOS_Internal_Error err1 = compute_T_sat_p(p, tsat) ;
     EOS_Internal_Error err2 = compute_h_v_pT(p,tsat,res) ;
     return worst_internal_error(err1, err2) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_d_rho_l_sat_d_p_p(double p, double& res) const
   { AString prop("d_rho_l_sat_d_p") ;
     return compute_prop_p(prop,p,0,res) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_d_rho_v_sat_d_p_p(double p, double& res) const
   { AString prop("d_rho_v_sat_d_p") ;
     return compute_prop_p(prop,p,0,res) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_d_h_l_sat_d_p_p(double p, double& res) const
   { AString prop("d_h_l_sat_d_p") ;
     return compute_prop_p(prop,p,0,res) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_d_h_v_sat_d_p_p(double p, double& res) const
   { AString prop("d_h_v_sat_d_p") ;
     return compute_prop_p(prop,p,0,res) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_d_cp_l_sat_d_p_p(double p, double& res) const
   { AString prop("d_cp_l_sat_d_p") ;
     return compute_prop_p(prop,p,0,res) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_d_cp_v_sat_d_p_p(double p, double& res) const
   {
     AString prop("d_cp_v_sat_d_p") ;
     return compute_prop_p(prop,p,0,res) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_d_T_sat_d_p_p(double p, double& res) const
   { AString prop("d_T_sat_d_p") ;
     return compute_prop_p(prop,p,0,res) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_h_l_lim_p(double p, double& res) const
   { AString prop("h_l_lim") ;
     return compute_prop_p(prop,p,1,res) ;
   }

   inline EOS_Internal_Error EOS_Ipp::compute_h_v_lim_p(double p, double& res) const
   { AString prop("h_v_lim") ;
     return compute_prop_p(prop,p,1,res) ;
   }

}

#endif /* EOS_IPP_I_HXX_ */

