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
#ifndef EOS_FLUID_H
#define EOS_FLUID_H

// UObject
#include "Language/API/Language.hxx"
#include "EOS_Error.hxx"
#include "EOS_properties.hxx"
#include <string>
using std::string ;
#include <vector>
using std::vector ;


using LANGUAGE_KERNEL::UObject;

namespace NEPTUNE
{
  class EOS ;
  class EOS_Field ;
  class EOS_Error_Field ;
  class EOS_Fields; 
  //! Abstract root class for all fluids recognised bye EOS
  class EOS_Fluid :  public UObject
  { public:

    virtual const AString& equation_name() const ;
    virtual const AString& phase_name()    const ;
    virtual const AString& fluid_name()    const ;
    virtual const AString& table_name()    const ;
    virtual const AString& version_name()  const ;
    //! to initialize an implementation of EOS_Fluid
    //! \todo Change return type to EOS_Error
    virtual int init(const Strings&) ;
    //! to initialize an implementation of EOS_Fluid with supplementary parameters
    //! \todo Change return type to EOS_Error
    virtual int init(const Strings&, const Strings&) ;
    //! to initialize an implementation of EOS_Fluid with supplementary parameters + set reference state
    //! \todo Change return type to EOS_Error
    virtual int init(const Strings &, const Strings &,
                     const double, const double, const double, const double) ;

    //! Error handling methods
    virtual void      describe_error(const EOS_Internal_Error error, AString & description) const;

    //! For mixing only
    virtual int set_components(EOS** components, int nb);
    //! For perfectgas (but could be extended for other fluids)
    virtual void set_reference_state(double h, double s, double T, double p);
    //!  provisional for EOS_Mixing
    virtual double get_prxr();   
    //!  provisional for EOS_Mixing
    virtual double get_prxcp();
    //!  provisional for EOS_Mixing
    virtual double get_prxm();
    //!  provisional for EOS_Mixing
    virtual double get_prxl0();
    //!  provisional for EOS_Mixing
    virtual double get_prxl1();
    //!  provisional for EOS_Mixing
    virtual double get_prxl2();
    //!  provisional for EOS_Mixing
    virtual double get_prxm0();
    //!  provisional for EOS_Mixing
    virtual double get_prxm1();
    //!  provisional for EOS_Mixing
    virtual double get_prxm2();
    //!  provisional for EOS_Mixing
    virtual double get_prxdv();
    //  provisional for EOS_Mixing
    //! the main "Fields" method!
    //! the EOS_Error returned value is a "global" error
    //! errfield is a "local" error
    //! \todo remove method
    virtual double is_liquid(double p, double h) const ;
    virtual EOS_Error compute(const EOS_Fields& input,
                              EOS_Field& output, 
                              EOS_Error_Field& errfield) const ;
    virtual EOS_Error compute(const EOS_Fields& input,
                              EOS_Fields& output, 
                              EOS_Error_Field& errfield) const ;
    //! the main "Fields" method!
    //! computes r(p,h)
    //! p, h, r[i], errfield are all of the same size
    //! the EOS_Error returned value is a "global" error
    //! errfield is a "local" error
    virtual EOS_Error compute(const EOS_Field& p, 
                              const EOS_Field& h, 
                              EOS_Fields& r, 
                              EOS_Error_Field& errfield) const;
    //! the main "Field" method!
    //! computes r(p,h)
    //! p, h, r, errfield are all of the same size
    //! the EOS_Error returned value is a "global" error
    //! errfield is a "local" error
    virtual EOS_Error compute(const EOS_Field& p, 
                              const EOS_Field& h, 
                              EOS_Field& r, 
                              EOS_Error_Field& errfield) const;
    virtual EOS_Error compute(const EOS_Field& p,
                              EOS_Fields& output, 
                              EOS_Error_Field& errfield) const;
    virtual EOS_Error compute(const EOS_Field& p,
                              EOS_Field& output, 
                              EOS_Error_Field& errfield) const;
    EOS_Internal_Error compute(const char* const property_name,
                               double p, 
                               double h, 
                               double& x) const;
    EOS_Internal_Error compute(const char* const property_name,
                               double p, double& x) const;

    EOS_Internal_Error compute_Ph(const char* const property_name,
                              double in1, double in2, double& out) const;
    EOS_Internal_Error compute_PT(const char* const property_name, 
                              double in1, double in2, double& out) const;
    EOS_Internal_Error compute_Psat(const char* const property_name,
                              double in, double& out) const;
    EOS_Internal_Error compute_Tsat(const char* const property_name,
                              double in, double& out) const;

    virtual EOS_Error init_model(const std::string& model_name, const std::string& fluid_name);
    virtual EOS_Internal_Error compute_Ipp_error(double& error_tot, AString prop);  
    virtual EOS_Internal_Error get_p(double& p) const;
    virtual EOS_Internal_Error get_mm(double& mm) const;
    virtual EOS_Internal_Error get_p_crit(double& p_crit) const;
    virtual EOS_Internal_Error get_h_crit(double& h_crit) const;
    virtual EOS_Internal_Error get_T_crit(double& T_crit) const;
    virtual EOS_Internal_Error get_rho_crit(double& rho_crit) const;
    virtual EOS_Internal_Error get_p_min(double& p_min) const;
    virtual EOS_Internal_Error get_p_max(double& p_max) const;
    virtual EOS_Internal_Error get_h_max(double& h_max) const;
    virtual EOS_Internal_Error get_h_min(double& h_min) const;
    virtual EOS_Internal_Error get_T_max(double& T_max) const;
    virtual EOS_Internal_Error get_T_min(double& T_min) const;
    virtual EOS_Internal_Error get_rho_max(double& rho_max) const;
    virtual EOS_Internal_Error get_rho_min(double& rho_min) const;
    virtual EOS_Internal_Error get_p_ref(double& p_ref) const;
    virtual EOS_Internal_Error get_h_ref(double& h_ref) const;
    virtual EOS_Internal_Error get_T_ref(double& T_ref) const;

    virtual EOS_Internal_Error compute_T_ph(double p, double h, double& r) const;
    virtual EOS_Internal_Error compute_rho_ph(double p, double h, double& r) const;
    virtual EOS_Internal_Error compute_u_ph(double p, double h, double& r) const;
    virtual EOS_Internal_Error compute_s_ph(double p, double h, double& r) const;
    virtual EOS_Internal_Error compute_mu_ph(double p, double h, double& r) const;
    virtual EOS_Internal_Error compute_lambda_ph(double p, double h, double& r) const;
    virtual EOS_Internal_Error compute_cp_ph(double p, double h, double& r) const;
    virtual EOS_Internal_Error compute_cv_ph(double p, double h, double& r) const;
    virtual EOS_Internal_Error compute_sigma_ph(double p, double h, double& r) const;
    virtual EOS_Internal_Error compute_w_ph(double p, double h, double& r) const;
    virtual EOS_Internal_Error compute_g_ph(double p, double h, double& r) const;
    virtual EOS_Internal_Error compute_f_ph(double p, double h, double& r) const;
    virtual EOS_Internal_Error compute_pr_ph(double p, double h, double& r) const;
    virtual EOS_Internal_Error compute_beta_ph(double p, double h, double& r) const;
    virtual EOS_Internal_Error compute_gamma_ph(double p, double h, double& r) const;

    virtual EOS_Internal_Error compute_h_pT(double p, double T, double& r) const;
    virtual EOS_Internal_Error compute_rho_pT(double p, double T, double& r) const;
    virtual EOS_Internal_Error compute_u_pT(double p, double T, double& r) const;
    virtual EOS_Internal_Error compute_s_pT(double p, double T, double& r) const;
    virtual EOS_Internal_Error compute_mu_pT(double p, double T, double& r) const;
    virtual EOS_Internal_Error compute_lambda_pT(double p, double T, double& r) const;
    virtual EOS_Internal_Error compute_cp_pT(double p, double T, double& r) const;
    virtual EOS_Internal_Error compute_cv_pT(double p, double T, double& r) const;
    virtual EOS_Internal_Error compute_sigma_pT(double p, double T, double& r) const;
    virtual EOS_Internal_Error compute_w_pT(double p, double T, double& r) const;
    virtual EOS_Internal_Error compute_g_pT(double p, double T, double& r) const;
    virtual EOS_Internal_Error compute_f_pT(double p, double T, double& r) const;
    virtual EOS_Internal_Error compute_pr_pT(double p, double T, double& r) const;
    virtual EOS_Internal_Error compute_beta_pT(double p, double T, double& r) const;
    virtual EOS_Internal_Error compute_gamma_pT(double p, double T, double& r) const;

    virtual EOS_Internal_Error compute_h_ps(double p, double s, double& r) const;
    virtual EOS_Internal_Error compute_T_ps(double p, double s, double& r) const;
    virtual EOS_Internal_Error compute_rho_ps(double p, double s, double& r) const;
    virtual EOS_Internal_Error compute_u_ps(double p, double s, double& r) const;
    virtual EOS_Internal_Error compute_mu_ps(double p, double s, double& r) const;
    virtual EOS_Internal_Error compute_lambda_ps(double p, double s, double& r) const;
    virtual EOS_Internal_Error compute_cp_ps(double p, double s, double& r) const;
    virtual EOS_Internal_Error compute_cv_ps(double p, double s, double& r) const;
    virtual EOS_Internal_Error compute_sigma_ps(double p, double s, double& r) const;
    virtual EOS_Internal_Error compute_w_ps(double p, double s, double& r) const;
    virtual EOS_Internal_Error compute_g_ps(double p, double s, double& r) const;
    virtual EOS_Internal_Error compute_f_ps(double p, double s, double& r) const;
    virtual EOS_Internal_Error compute_pr_ps(double p, double s, double& r) const;
    virtual EOS_Internal_Error compute_beta_ps(double p, double s, double& r) const;
    virtual EOS_Internal_Error compute_gamma_ps(double p, double s, double& r) const;

    virtual EOS_Internal_Error compute_d_T_d_p_h_ph(double p, double h, double& r) const;
    virtual EOS_Internal_Error compute_d_T_d_h_p_ph(double p, double h, double& r) const;
    virtual EOS_Internal_Error compute_d_rho_d_p_h_ph(double p, double h, double& r) const;
    virtual EOS_Internal_Error compute_d_rho_d_h_p_ph(double p, double h, double& r) const;
    virtual EOS_Internal_Error compute_d_u_d_p_h_ph(double p, double h, double& r) const;
    virtual EOS_Internal_Error compute_d_u_d_h_p_ph(double p, double h, double& r) const;
    virtual EOS_Internal_Error compute_d_s_d_p_h_ph(double p, double h, double& r) const;
    virtual EOS_Internal_Error compute_d_s_d_h_p_ph(double p, double h, double& r) const;
    virtual EOS_Internal_Error compute_d_mu_d_p_h_ph(double p, double h, double& r) const;
    virtual EOS_Internal_Error compute_d_mu_d_h_p_ph(double p, double h, double& r) const;
    virtual EOS_Internal_Error compute_d_lambda_d_p_h_ph(double p, double h, double& r) const;
    virtual EOS_Internal_Error compute_d_lambda_d_h_p_ph(double p, double h, double& r) const;
    virtual EOS_Internal_Error compute_d_cp_d_p_h_ph(double p, double h, double& r) const;
    virtual EOS_Internal_Error compute_d_cp_d_h_p_ph(double p, double h, double& r) const;
    virtual EOS_Internal_Error compute_d_cv_d_p_h_ph(double p, double h, double& r) const;
    virtual EOS_Internal_Error compute_d_cv_d_h_p_ph(double p, double h, double& r) const;
    virtual EOS_Internal_Error compute_d_sigma_d_p_h_ph(double p, double h, double& r) const;
    virtual EOS_Internal_Error compute_d_sigma_d_h_p_ph(double p, double h, double& r) const;
    virtual EOS_Internal_Error compute_d_w_d_p_h_ph(double p, double h, double& r) const;
    virtual EOS_Internal_Error compute_d_w_d_h_p_ph(double p, double h, double& r) const;
    virtual EOS_Internal_Error compute_d_g_d_p_h_ph(double p, double h, double& r) const;
    virtual EOS_Internal_Error compute_d_g_d_h_p_ph(double p, double h, double& r) const;
    virtual EOS_Internal_Error compute_d_f_d_p_h_ph(double p, double h, double& r) const;
    virtual EOS_Internal_Error compute_d_f_d_h_p_ph(double p, double h, double& r) const;
    virtual EOS_Internal_Error compute_d_pr_d_p_h_ph(double p, double h, double& r) const;
    virtual EOS_Internal_Error compute_d_pr_d_h_p_ph(double p, double h, double& r) const;
    virtual EOS_Internal_Error compute_d_beta_d_p_h_ph(double p, double h, double& r) const;
    virtual EOS_Internal_Error compute_d_beta_d_h_p_ph(double p, double h, double& r) const;
    virtual EOS_Internal_Error compute_d_gamma_d_p_h_ph(double p, double h, double& r) const;
    virtual EOS_Internal_Error compute_d_gamma_d_h_p_ph(double p, double h, double& r) const;

    virtual EOS_Internal_Error compute_d_h_d_p_T_pT(double p, double T, double& r) const;
    virtual EOS_Internal_Error compute_d_h_d_T_p_pT(double p, double T, double& r) const;
    virtual EOS_Internal_Error compute_d_rho_d_p_T_pT(double p, double T, double& r) const;
    virtual EOS_Internal_Error compute_d_rho_d_T_p_pT(double p, double T, double& r) const;
    virtual EOS_Internal_Error compute_d_u_d_p_T_pT(double p, double T, double& r) const;
    virtual EOS_Internal_Error compute_d_u_d_T_p_pT(double p, double T, double& r) const;
    virtual EOS_Internal_Error compute_d_s_d_p_T_pT(double p, double T, double& r) const;
    virtual EOS_Internal_Error compute_d_s_d_T_p_pT(double p, double T, double& r) const;
    virtual EOS_Internal_Error compute_d_mu_d_p_T_pT(double p, double T, double& r) const;
    virtual EOS_Internal_Error compute_d_mu_d_T_p_pT(double p, double T, double& r) const;
    virtual EOS_Internal_Error compute_d_lambda_d_p_T_pT(double p, double T, double& r) const;
    virtual EOS_Internal_Error compute_d_lambda_d_T_p_pT(double p, double T, double& r) const;
    virtual EOS_Internal_Error compute_d_cp_d_p_T_pT(double p, double T, double& r) const;
    virtual EOS_Internal_Error compute_d_cp_d_T_p_pT(double p, double T, double& r) const;
    virtual EOS_Internal_Error compute_d_cv_d_p_T_pT(double p, double T, double& r) const;
    virtual EOS_Internal_Error compute_d_cv_d_T_p_pT(double p, double T, double& r) const;
    virtual EOS_Internal_Error compute_d_sigma_d_p_T_pT(double p, double T, double& r) const;
    virtual EOS_Internal_Error compute_d_sigma_d_T_p_pT(double p, double T, double& r) const;
    virtual EOS_Internal_Error compute_d_w_d_p_T_pT(double p, double T, double& r) const;
    virtual EOS_Internal_Error compute_d_w_d_T_p_pT(double p, double T, double& r) const;
    virtual EOS_Internal_Error compute_d_g_d_p_T_pT(double p, double T, double& r) const;
    virtual EOS_Internal_Error compute_d_g_d_T_p_pT(double p, double T, double& r) const;
    virtual EOS_Internal_Error compute_d_f_d_p_T_pT(double p, double T, double& r) const;
    virtual EOS_Internal_Error compute_d_f_d_T_p_pT(double p, double T, double& r) const;
    virtual EOS_Internal_Error compute_d_pr_d_p_T_pT(double p, double T, double& r) const;
    virtual EOS_Internal_Error compute_d_pr_d_T_p_pT(double p, double T, double& r) const;
    virtual EOS_Internal_Error compute_d_beta_d_p_T_pT(double p, double T, double& r) const;
    virtual EOS_Internal_Error compute_d_beta_d_T_p_pT(double p, double T, double& r) const;
    virtual EOS_Internal_Error compute_d_gamma_d_p_T_pT(double p, double T, double& r) const;
    virtual EOS_Internal_Error compute_d_gamma_d_T_p_pT(double p, double T, double& r) const;

    virtual EOS_Internal_Error compute_d_h_d_p_s_ps(double p, double s, double& r) const;
    virtual EOS_Internal_Error compute_d_h_d_s_p_ps(double p, double s, double& r) const;
    virtual EOS_Internal_Error compute_d_T_d_p_s_ps(double p, double s, double& r) const;
    virtual EOS_Internal_Error compute_d_T_d_s_p_ps(double p, double s, double& r) const;
    virtual EOS_Internal_Error compute_d_rho_d_p_s_ps(double p, double s, double& r) const;
    virtual EOS_Internal_Error compute_d_rho_d_s_p_ps(double p, double s, double& r) const;
    virtual EOS_Internal_Error compute_d_u_d_p_s_ps(double p, double s, double& r) const;
    virtual EOS_Internal_Error compute_d_u_d_s_p_ps(double p, double s, double& r) const;
    virtual EOS_Internal_Error compute_d_mu_d_p_s_ps(double p, double s, double& r) const;
    virtual EOS_Internal_Error compute_d_mu_d_s_p_ps(double p, double s, double& r) const;
    virtual EOS_Internal_Error compute_d_lambda_d_p_s_ps(double p, double s, double& r) const;
    virtual EOS_Internal_Error compute_d_lambda_d_s_p_ps(double p, double s, double& r) const;
    virtual EOS_Internal_Error compute_d_cp_d_p_s_ps(double p, double s, double& r) const;
    virtual EOS_Internal_Error compute_d_cp_d_s_p_ps(double p, double s, double& r) const;
    virtual EOS_Internal_Error compute_d_cv_d_p_s_ps(double p, double s, double& r) const;
    virtual EOS_Internal_Error compute_d_cv_d_s_p_ps(double p, double s, double& r) const;
    virtual EOS_Internal_Error compute_d_sigma_d_p_s_ps(double p, double s, double& r) const;
    virtual EOS_Internal_Error compute_d_sigma_d_s_p_ps(double p, double s, double& r) const;
    virtual EOS_Internal_Error compute_d_w_d_p_s_ps(double p, double s, double& r) const;
    virtual EOS_Internal_Error compute_d_w_d_s_p_ps(double p, double s, double& r) const;
    virtual EOS_Internal_Error compute_d_g_d_p_s_ps(double p, double s, double& r) const;
    virtual EOS_Internal_Error compute_d_g_d_s_p_ps(double p, double s, double& r) const;
    virtual EOS_Internal_Error compute_d_f_d_p_s_ps(double p, double s, double& r) const;
    virtual EOS_Internal_Error compute_d_f_d_s_p_ps(double p, double s, double& r) const;
    virtual EOS_Internal_Error compute_d_pr_d_p_s_ps(double p, double s, double& r) const;
    virtual EOS_Internal_Error compute_d_pr_d_s_p_ps(double p, double s, double& r) const;
    virtual EOS_Internal_Error compute_d_beta_d_p_s_ps(double p, double s, double& r) const;
    virtual EOS_Internal_Error compute_d_beta_d_s_p_ps(double p, double s, double& r) const;
    virtual EOS_Internal_Error compute_d_gamma_d_p_s_ps(double p, double s, double& r) const;
    virtual EOS_Internal_Error compute_d_gamma_d_s_p_ps(double p, double s, double& r) const;

    virtual EOS_Internal_Error compute_T_sat_p(double p, double& r) const;
    virtual EOS_Internal_Error compute_rho_l_sat_p(double p, double& r) const;
    virtual EOS_Internal_Error compute_rho_v_sat_p(double p, double& r) const;
    virtual EOS_Internal_Error compute_h_l_sat_p(double p, double& r) const;
    virtual EOS_Internal_Error compute_h_v_sat_p(double p, double& r) const;
    virtual EOS_Internal_Error compute_cp_l_sat_p(double p, double& r) const;
    virtual EOS_Internal_Error compute_cp_v_sat_p(double p, double& r) const;

    virtual EOS_Internal_Error compute_p_sat_T(double T, double& r) const;
    virtual EOS_Internal_Error compute_rho_l_sat_T(double T, double& r) const;
    virtual EOS_Internal_Error compute_rho_v_sat_T(double T, double& r) const;
    virtual EOS_Internal_Error compute_h_l_sat_T(double T, double& r) const;
    virtual EOS_Internal_Error compute_h_v_sat_T(double T, double& r) const;
    virtual EOS_Internal_Error compute_cp_l_sat_T(double T, double& r) const;
    virtual EOS_Internal_Error compute_cp_v_sat_T(double T, double& r) const;

    virtual EOS_Internal_Error compute_d_T_sat_d_p_p(double p, double& r) const;
    virtual EOS_Internal_Error compute_d_rho_l_sat_d_p_p(double p, double& r) const;
    virtual EOS_Internal_Error compute_d_rho_v_sat_d_p_p(double p, double& r) const;
    virtual EOS_Internal_Error compute_d_h_l_sat_d_p_p(double p, double& r) const;
    virtual EOS_Internal_Error compute_d_h_v_sat_d_p_p(double p, double& r) const;
    virtual EOS_Internal_Error compute_d_cp_l_sat_d_p_p(double p, double& r) const;
    virtual EOS_Internal_Error compute_d_cp_v_sat_d_p_p(double p, double& r) const;

    virtual EOS_Internal_Error compute_d_p_sat_d_T_T(double T, double& r) const;
    virtual EOS_Internal_Error compute_d_rho_l_sat_d_T_T(double T, double& r) const;
    virtual EOS_Internal_Error compute_d_rho_v_sat_d_T_T(double T, double& r) const;
    virtual EOS_Internal_Error compute_d_h_l_sat_d_T_T(double T, double& r) const;
    virtual EOS_Internal_Error compute_d_h_v_sat_d_T_T(double T, double& r) const;
    virtual EOS_Internal_Error compute_d_cp_l_sat_d_T_T(double T, double& r) const;
    virtual EOS_Internal_Error compute_d_cp_v_sat_d_T_T(double T, double& r) const;

    virtual EOS_Internal_Error compute_d2_T_sat_d_p_d_p_p(double p, double& r) const;
    virtual EOS_Internal_Error compute_d2_rho_l_sat_d_p_d_p_p(double p, double& r) const;
    virtual EOS_Internal_Error compute_d2_rho_v_sat_d_p_d_p_p(double p, double& r) const;
    virtual EOS_Internal_Error compute_d2_h_l_sat_d_p_d_p_p(double p, double& r) const;
    virtual EOS_Internal_Error compute_d2_h_v_sat_d_p_d_p_p(double p, double& r) const;
    virtual EOS_Internal_Error compute_d2_cp_l_sat_d_p_d_p_p(double p, double& r) const;
    virtual EOS_Internal_Error compute_d2_cp_v_sat_d_p_d_p_p(double p, double& r) const;

    virtual EOS_Internal_Error compute_d2_p_sat_d_T_d_T_T(double T, double& r) const;
    virtual EOS_Internal_Error compute_d2_rho_l_sat_d_T_d_T_T(double T, double& r) const;
    virtual EOS_Internal_Error compute_d2_rho_v_sat_d_T_d_T_T(double T, double& r) const;
    virtual EOS_Internal_Error compute_d2_h_l_sat_d_T_d_T_T(double T, double& r) const;
    virtual EOS_Internal_Error compute_d2_h_v_sat_d_T_d_T_T(double T, double& r) const;
    virtual EOS_Internal_Error compute_d2_cp_l_sat_d_T_d_T_T(double T, double& r) const;
    virtual EOS_Internal_Error compute_d2_cp_v_sat_d_T_d_T_T(double T, double& r) const;

    virtual EOS_Internal_Error compute_h_l_lim_p(double p, double& r) const;
    virtual EOS_Internal_Error compute_h_v_lim_p(double p, double& r) const;
    virtual EOS_Internal_Error compute_d_h_l_lim_d_p_p(double p, double& r) const;
    virtual EOS_Internal_Error compute_d_h_v_lim_d_p_p(double p, double& r) const;
    virtual EOS_Internal_Error compute_d2_h_l_lim_d_p_d_p_p(double p, double& r) const;
    virtual EOS_Internal_Error compute_d2_h_v_lim_d_p_d_p_p(double p, double& r) const;

    virtual EOS_Internal_Error compute_T_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_rho_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_u_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_s_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_mu_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_lambda_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_cp_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_cv_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_sigma_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_w_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_g_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_f_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_pr_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_beta_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_gamma_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;

    virtual EOS_Internal_Error compute_h_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_rho_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_u_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_s_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_mu_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_lambda_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_cp_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_cv_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_sigma_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_w_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_g_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_f_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_pr_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_beta_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_gamma_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;

    virtual EOS_Internal_Error compute_h_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_T_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_rho_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_u_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_mu_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_lambda_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_cp_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_cv_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_sigma_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_w_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_g_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_f_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_pr_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_beta_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_gamma_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;

    virtual EOS_Internal_Error compute_d_T_d_p_h_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_T_d_h_p_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_T_d_c_0_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_T_d_c_1_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_T_d_c_2_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_T_d_c_3_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_T_d_c_4_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_rho_d_p_h_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_rho_d_h_p_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_rho_d_c_0_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_rho_d_c_1_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_rho_d_c_2_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_rho_d_c_3_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_rho_d_c_4_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_u_d_p_h_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_u_d_h_p_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_u_d_c_0_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_u_d_c_1_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_u_d_c_2_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_u_d_c_3_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_u_d_c_4_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_s_d_p_h_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_s_d_h_p_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_s_d_c_0_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_s_d_c_1_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_s_d_c_2_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_s_d_c_3_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_s_d_c_4_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_mu_d_p_h_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_mu_d_h_p_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_mu_d_c_0_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_mu_d_c_1_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_mu_d_c_2_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_mu_d_c_3_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_mu_d_c_4_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_lambda_d_p_h_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_lambda_d_h_p_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_lambda_d_c_0_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_lambda_d_c_1_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_lambda_d_c_2_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_lambda_d_c_3_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_lambda_d_c_4_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_cp_d_p_h_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_cp_d_h_p_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_cp_d_c_0_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_cp_d_c_1_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_cp_d_c_2_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_cp_d_c_3_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_cp_d_c_4_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_cv_d_p_h_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_cv_d_h_p_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_cv_d_c_0_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_cv_d_c_1_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_cv_d_c_2_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_cv_d_c_3_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_cv_d_c_4_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_sigma_d_p_h_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_sigma_d_h_p_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_sigma_d_c_0_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_sigma_d_c_1_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_sigma_d_c_2_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_sigma_d_c_3_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_sigma_d_c_4_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_w_d_p_h_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_w_d_h_p_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_w_d_c_0_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_w_d_c_1_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_w_d_c_2_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_w_d_c_3_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_w_d_c_4_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_g_d_p_h_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_g_d_h_p_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_g_d_c_0_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_g_d_c_1_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_g_d_c_2_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_g_d_c_3_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_g_d_c_4_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_f_d_p_h_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_f_d_h_p_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_f_d_c_0_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_f_d_c_1_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_f_d_c_2_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_f_d_c_3_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_f_d_c_4_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_pr_d_p_h_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_pr_d_h_p_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_pr_d_c_0_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_pr_d_c_1_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_pr_d_c_2_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_pr_d_c_3_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_pr_d_c_4_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_beta_d_p_h_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_beta_d_h_p_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_beta_d_c_0_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_beta_d_c_1_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_beta_d_c_2_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_beta_d_c_3_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_beta_d_c_4_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_gamma_d_p_h_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_gamma_d_h_p_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_gamma_d_c_0_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_gamma_d_c_1_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_gamma_d_c_2_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_gamma_d_c_3_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_gamma_d_c_4_ph_ph(double p, double h, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;

    virtual EOS_Internal_Error compute_d_h_d_p_T_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_h_d_T_p_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_h_d_c_0_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_h_d_c_1_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_h_d_c_2_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_h_d_c_3_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_h_d_c_4_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_rho_d_p_T_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_rho_d_T_p_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_rho_d_c_0_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_rho_d_c_1_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_rho_d_c_2_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_rho_d_c_3_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_rho_d_c_4_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_u_d_p_T_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_u_d_T_p_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_u_d_c_0_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_u_d_c_1_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_u_d_c_2_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_u_d_c_3_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_u_d_c_4_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_s_d_p_T_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_s_d_T_p_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_s_d_c_0_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_s_d_c_1_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_s_d_c_2_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_s_d_c_3_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_s_d_c_4_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_mu_d_p_T_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_mu_d_T_p_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_mu_d_c_0_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_mu_d_c_1_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_mu_d_c_2_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_mu_d_c_3_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_mu_d_c_4_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_lambda_d_p_T_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_lambda_d_T_p_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_lambda_d_c_0_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_lambda_d_c_1_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_lambda_d_c_2_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_lambda_d_c_3_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_lambda_d_c_4_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_cp_d_p_T_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_cp_d_T_p_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_cp_d_c_0_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_cp_d_c_1_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_cp_d_c_2_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_cp_d_c_3_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_cp_d_c_4_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_cv_d_p_T_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_cv_d_T_p_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_cv_d_c_0_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_cv_d_c_1_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_cv_d_c_2_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_cv_d_c_3_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_cv_d_c_4_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_sigma_d_p_T_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_sigma_d_T_p_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_sigma_d_c_0_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_sigma_d_c_1_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_sigma_d_c_2_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_sigma_d_c_3_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_sigma_d_c_4_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_w_d_p_T_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_w_d_T_p_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_w_d_c_0_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_w_d_c_1_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_w_d_c_2_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_w_d_c_3_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_w_d_c_4_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_g_d_p_T_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_g_d_T_p_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_g_d_c_0_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_g_d_c_1_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_g_d_c_2_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_g_d_c_3_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_g_d_c_4_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_f_d_p_T_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_f_d_T_p_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_f_d_c_0_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_f_d_c_1_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_f_d_c_2_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_f_d_c_3_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_f_d_c_4_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_pr_d_p_T_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_pr_d_T_p_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_pr_d_c_0_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_pr_d_c_1_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_pr_d_c_2_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_pr_d_c_3_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_pr_d_c_4_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_beta_d_p_T_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_beta_d_T_p_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_beta_d_c_0_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_beta_d_c_1_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_beta_d_c_2_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_beta_d_c_3_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_beta_d_c_4_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_gamma_d_p_T_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_gamma_d_T_p_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_gamma_d_c_0_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_gamma_d_c_1_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_gamma_d_c_2_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_gamma_d_c_3_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_gamma_d_c_4_pT_pT(double p, double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;

    virtual EOS_Internal_Error compute_d_h_d_p_s_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_h_d_s_p_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_h_d_c_0_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_h_d_c_1_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_h_d_c_2_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_h_d_c_3_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_h_d_c_4_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_T_d_p_s_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_T_d_s_p_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_T_d_c_0_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_T_d_c_1_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_T_d_c_2_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_T_d_c_3_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_T_d_c_4_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_rho_d_p_s_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_rho_d_s_p_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_rho_d_c_0_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_rho_d_c_1_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_rho_d_c_2_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_rho_d_c_3_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_rho_d_c_4_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_u_d_p_s_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_u_d_s_p_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_u_d_c_0_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_u_d_c_1_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_u_d_c_2_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_u_d_c_3_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_u_d_c_4_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_mu_d_p_s_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_mu_d_s_p_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_mu_d_c_0_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_mu_d_c_1_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_mu_d_c_2_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_mu_d_c_3_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_mu_d_c_4_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_lambda_d_p_s_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_lambda_d_s_p_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_lambda_d_c_0_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_lambda_d_c_1_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_lambda_d_c_2_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_lambda_d_c_3_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_lambda_d_c_4_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_cp_d_p_s_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_cp_d_s_p_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_cp_d_c_0_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_cp_d_c_1_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_cp_d_c_2_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_cp_d_c_3_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_cp_d_c_4_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_cv_d_p_s_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_cv_d_s_p_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_cv_d_c_0_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_cv_d_c_1_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_cv_d_c_2_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_cv_d_c_3_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_cv_d_c_4_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_sigma_d_p_s_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_sigma_d_s_p_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_sigma_d_c_0_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_sigma_d_c_1_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_sigma_d_c_2_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_sigma_d_c_3_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_sigma_d_c_4_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_w_d_p_s_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_w_d_s_p_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_w_d_c_0_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_w_d_c_1_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_w_d_c_2_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_w_d_c_3_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_w_d_c_4_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_g_d_p_s_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_g_d_s_p_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_g_d_c_0_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_g_d_c_1_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_g_d_c_2_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_g_d_c_3_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_g_d_c_4_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_f_d_p_s_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_f_d_s_p_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_f_d_c_0_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_f_d_c_1_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_f_d_c_2_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_f_d_c_3_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_f_d_c_4_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_pr_d_p_s_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_pr_d_s_p_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_pr_d_c_0_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_pr_d_c_1_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_pr_d_c_2_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_pr_d_c_3_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_pr_d_c_4_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_beta_d_p_s_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_beta_d_s_p_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_beta_d_c_0_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_beta_d_c_1_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_beta_d_c_2_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_beta_d_c_3_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_beta_d_c_4_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_gamma_d_p_s_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_gamma_d_s_p_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_gamma_d_c_0_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_gamma_d_c_1_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_gamma_d_c_2_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_gamma_d_c_3_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_gamma_d_c_4_ps_ps(double p, double s, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;

    virtual EOS_Internal_Error compute_T_sat_p(double p, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_rho_l_sat_p(double p, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_rho_v_sat_p(double p, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_h_l_sat_p(double p, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_h_v_sat_p(double p, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_cp_l_sat_p(double p, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_cp_v_sat_p(double p, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;

    virtual EOS_Internal_Error compute_p_sat_T(double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_rho_l_sat_T(double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_rho_v_sat_T(double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_h_l_sat_T(double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_h_v_sat_T(double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_cp_l_sat_T(double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_cp_v_sat_T(double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;

    virtual EOS_Internal_Error compute_d_T_sat_d_p_p(double p, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_rho_l_sat_d_p_p(double p, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_rho_v_sat_d_p_p(double p, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_h_l_sat_d_p_p(double p, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_h_v_sat_d_p_p(double p, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_cp_l_sat_d_p_p(double p, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_cp_v_sat_d_p_p(double p, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;

    virtual EOS_Internal_Error compute_d_p_sat_d_T_T(double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_rho_l_sat_d_T_T(double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_rho_v_sat_d_T_T(double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_h_l_sat_d_T_T(double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_h_v_sat_d_T_T(double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_cp_l_sat_d_T_T(double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d_cp_v_sat_d_T_T(double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;

    virtual EOS_Internal_Error compute_d2_T_sat_d_p_d_p_p(double p, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d2_rho_l_sat_d_p_d_p_p(double p, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d2_rho_v_sat_d_p_d_p_p(double p, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d2_h_l_sat_d_p_d_p_p(double p, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d2_h_v_sat_d_p_d_p_p(double p, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d2_cp_l_sat_d_p_d_p_p(double p, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d2_cp_v_sat_d_p_d_p_p(double p, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;

    virtual EOS_Internal_Error compute_d2_p_sat_d_T_d_T_T(double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d2_rho_l_sat_d_T_d_T_T(double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d2_rho_v_sat_d_T_d_T_T(double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d2_h_l_sat_d_T_d_T_T(double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d2_h_v_sat_d_T_d_T_T(double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d2_cp_l_sat_d_T_d_T_T(double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    virtual EOS_Internal_Error compute_d2_cp_v_sat_d_T_d_T_T(double T, double& r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;

    //! list of all properties (for a given fluid) for which compute_* functions are implemented
    vector<string> is_implemented(const char* pprop1, const char* pprop2, 
                                  const vector<typrop> list_prop, double vprop1, double vprop2) ;
    
    //! list of all properties (for a given fluid for sat or lim) for which compute_* functions are implemented
    vector<string> is_implemented(const char* pprop1, const vector<typrop> list_prop, double vprop1) ;

    //! use for surcharged operator<<
    virtual void fluid_description(std::ostream &flux) const ;
 
  protected:
    EOS_Fluid();
  private: 
    static int type_Id;
  };
}
#include "EOS/API/EOS_Fluid_i.hxx"
#endif
