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



#ifndef EOS_H
#define EOS_H

#include "Language/API/Language.hxx"
#include "Language/API/Object_i.hxx"
#include "EOS/API/EOS_Error.hxx"
#include "EOS/API/EOS_properties.hxx"
#include "EOS/API/EOS_Error_Handler_Stack.hxx"
#include "EOS/API/EOS_Config.hxx"
#include "EOS/API/EOS_eosdatadir.hxx"
#include <iostream>
#include <string>
using std::string ;
#include <vector>
using std::vector ;

using NEPTUNE::Strings ;
using NEPTUNE::AString ;


namespace NEPTUNE
{
  class EOS             ;
  class EOS_Field       ;
  class EOS_Error_Field ;
  class EOS_Fields      ;
  class EOS_Fluid       ;
  class Strings         ;
  class AString         ;
  class ArrOfDouble     ;

  //! The EOS API
  class EOS : public Object
  { public :

    static const AString eos_version ;

    //! EOS water("Cathare_Water");
    EOS(const char *const str) ;
    //! EOS eos("method_name", "ref_name");
    EOS(const char *const method_str, const char *const ref_str) ;
    //! EOS eos("method_name", "ref_name", [values]);
    EOS(const char *const method_str, const char *const ref_str, const Strings &init_args) ;
    //! EOS eos("Thermodynamic Model", "Fluid Equation", [values], h0, s0, t0, p0);
    EOS(const char *const method_str, const char *const ref_str, const Strings &init_args, 
        const double h0, const double s0, const double t0, const double p0) ;
    //!
    EOS(const char *const str, const Strings &init_args) ;
    //! AFluid should be of type EOS_Fluid
    EOS(const char *const str, const Object &AFluid) ;
    virtual ~EOS() ;
    //! returns void fraction
    int init(const Strings &init_args) ;
    int init(const Strings &init_args,
             const double h0, const double s0, const double t0, const double p0) ;
    //! For mixing only
    int set_components(EOS **components, int nb) ;
    //! For perfectgas
    void set_reference_state(double h, double s, double T, double p);
    //! provisional for EOS_Mixing
    virtual double get_prxr() ;
    //! provisional for EOS_Mixing
    virtual double get_prxcp();
    //! provisional for EOS_Mixing
    virtual double get_prxm() ;
     //! provisional for EOS_Mixing
    virtual double get_prxl0();
     //! provisional for EOS_Mixing
    virtual double get_prxl1();
     //! provisional for EOS_Mixing
    virtual double get_prxl2();
    //! provisional for EOS_Mixing
    virtual double get_prxm0();
    //! provisional for EOS_Mixing
    virtual double get_prxm1();
    //! provisional for EOS_Mixing
    virtual double get_prxm2();
    //! provisional for EOS_Mixing
    virtual double get_prxdv(); 
    //  provisional for EOS_Mixing
    double is_liquid(double p, double h) const ;
    const AString &equation_name() const ;
    const AString &fluid_name()    const ;
    const AString &phase_name()    const ;
    const AString &table_name()    const ;
    const AString &version_name()  const ;
    virtual EOS_Error init_model(const std::string& model_name, const std::string& fluid_name, bool switch_comp_sat=true, bool swch_calc_deriv_fld=true); // For the interpolator  
    virtual EOS_Internal_Error compute_Ipp_error(double& error_tot, double* &error_cells, AString prop);  
    virtual EOS_Internal_Error compute_Ipp_sat_error(double& error_tot, double* &error_cells, AString prop);
    EOS_Error compute(const EOS_Fields &input, 
                      EOS_Field &output, 
                      EOS_Error_Field &errfield) const ;
    EOS_Error compute(const EOS_Fields &input, 
                      EOS_Fields &output, 
                      EOS_Error_Field &errfield) const ;
    EOS_Error compute(const EOS_Field &p, 
                      const EOS_Field &h, 
                      EOS_Field &r, 
                      EOS_Error_Field &errfield) const ;
    EOS_Error compute(const EOS_Field &p, 
                      const EOS_Field &h,                              
                      EOS_Fields &r, 
                      EOS_Error_Field &errfield) const ;
    EOS_Error compute(const char *const property_name,
                      double p, 
                      double h, 
                      double &x) const ;
    EOS_Error compute(const char *const property_name,
                      double p, 
                      double &x) const ;
    EOS_Error compute(const EOS_Field &p, 
                      EOS_Fields &output, 
                      EOS_Error_Field &errfield) const ;
    EOS_Error compute(const EOS_Field &p, 
                      EOS_Field &output, 
                      EOS_Error_Field &errfield) const ;
    EOS_Error compute_Ph(const char* const property_name, 
                      double in1, double in2, double& out) const;
    EOS_Error compute_PT(const char* const property_name, 
                      double in1, double in2, double& out) const;
    EOS_Error compute_Psat(const char* const property_name,
                      double in, double& out) const;
    EOS_Error compute_Tsat(const char* const property_name, 
                      double in, double& out) const;

    EOS_Error get_p(double &p) const   ;
    EOS_Error get_mm(double &mm) const ;
    EOS_Error get_p_crit(double &p_crit) const     ;
    EOS_Error get_h_crit(double &h_crit) const     ;
    EOS_Error get_T_crit(double &T_crit) const     ;
    EOS_Error get_rho_crit(double &rho_crit) const ;
    EOS_Error get_p_min(double &p_min) const ;
    EOS_Error get_p_max(double &p_max) const ;
    EOS_Error get_h_max(double &h_max) const ;
    double get_h_max() const ;
    EOS_Error get_h_min(double &h_min) const ;
    EOS_Error get_T_max(double &T_max) const ;
    EOS_Error get_T_min(double &T_min) const ;
    EOS_Error get_rho_max(double &rho_max) const ;
    EOS_Error get_rho_min(double &rho_min) const ;
    EOS_Error get_p_ref(double &p_ref) const     ;
    EOS_Error get_h_ref(double &h_ref) const     ;
    EOS_Error get_T_ref(double &T_ref) const     ;

    EOS_Error compute_T_ph(double p, double h, double &r) const;
    EOS_Error compute_rho_ph(double p, double h, double &r) const;
    EOS_Error compute_u_ph(double p, double h, double &r) const;
    EOS_Error compute_s_ph(double p, double h, double &r) const;
    EOS_Error compute_mu_ph(double p, double h, double &r) const;
    EOS_Error compute_lambda_ph(double p, double h, double &r) const;
    EOS_Error compute_cp_ph(double p, double h, double &r) const;
    EOS_Error compute_cv_ph(double p, double h, double &r) const;
    EOS_Error compute_sigma_ph(double p, double h, double &r) const;
    EOS_Error compute_w_ph(double p, double h, double &r) const;
    EOS_Error compute_g_ph(double p, double h, double &r) const;
    EOS_Error compute_f_ph(double p, double h, double &r) const;
    EOS_Error compute_pr_ph(double p, double h, double &r) const;
    EOS_Error compute_beta_ph(double p, double h, double &r) const;
    EOS_Error compute_gamma_ph(double p, double h, double &r) const;

    EOS_Error compute_h_pT(double p, double T, double &r) const;
    EOS_Error compute_rho_pT(double p, double T, double &r) const;
    EOS_Error compute_u_pT(double p, double T, double &r) const;
    EOS_Error compute_s_pT(double p, double T, double &r) const;
    EOS_Error compute_mu_pT(double p, double T, double &r) const;
    EOS_Error compute_lambda_pT(double p, double T, double &r) const;
    EOS_Error compute_cp_pT(double p, double T, double &r) const;
    EOS_Error compute_cv_pT(double p, double T, double &r) const;
    EOS_Error compute_sigma_pT(double p, double T, double &r) const;
    EOS_Error compute_w_pT(double p, double T, double &r) const;
    EOS_Error compute_g_pT(double p, double T, double &r) const;
    EOS_Error compute_f_pT(double p, double T, double &r) const;
    EOS_Error compute_pr_pT(double p, double T, double &r) const;
    EOS_Error compute_beta_pT(double p, double T, double &r) const;
    EOS_Error compute_gamma_pT(double p, double T, double &r) const;

    EOS_Error compute_h_ps(double p, double s, double &r) const;
    EOS_Error compute_T_ps(double p, double s, double &r) const;
    EOS_Error compute_rho_ps(double p, double s, double &r) const;
    EOS_Error compute_u_ps(double p, double s, double &r) const;
    EOS_Error compute_mu_ps(double p, double s, double &r) const;
    EOS_Error compute_lambda_ps(double p, double s, double &r) const;
    EOS_Error compute_cp_ps(double p, double s, double &r) const;
    EOS_Error compute_cv_ps(double p, double s, double &r) const;
    EOS_Error compute_sigma_ps(double p, double s, double &r) const;
    EOS_Error compute_w_ps(double p, double s, double &r) const;
    EOS_Error compute_g_ps(double p, double s, double &r) const;
    EOS_Error compute_f_ps(double p, double s, double &r) const;
    EOS_Error compute_pr_ps(double p, double s, double &r) const;
    EOS_Error compute_beta_ps(double p, double s, double &r) const;
    EOS_Error compute_gamma_ps(double p, double s, double &r) const;

    EOS_Error compute_d_T_d_p_h_ph(double p, double h, double &r) const;
    EOS_Error compute_d_T_d_h_p_ph(double p, double h, double &r) const;
    EOS_Error compute_d_rho_d_p_h_ph(double p, double h, double &r) const;
    EOS_Error compute_d_rho_d_h_p_ph(double p, double h, double &r) const;
    EOS_Error compute_d_u_d_p_h_ph(double p, double h, double &r) const;
    EOS_Error compute_d_u_d_h_p_ph(double p, double h, double &r) const;
    EOS_Error compute_d_s_d_p_h_ph(double p, double h, double &r) const;
    EOS_Error compute_d_s_d_h_p_ph(double p, double h, double &r) const;
    EOS_Error compute_d_mu_d_p_h_ph(double p, double h, double &r) const;
    EOS_Error compute_d_mu_d_h_p_ph(double p, double h, double &r) const;
    EOS_Error compute_d_lambda_d_p_h_ph(double p, double h, double &r) const;
    EOS_Error compute_d_lambda_d_h_p_ph(double p, double h, double &r) const;
    EOS_Error compute_d_cp_d_p_h_ph(double p, double h, double &r) const;
    EOS_Error compute_d_cp_d_h_p_ph(double p, double h, double &r) const;
    EOS_Error compute_d_cv_d_p_h_ph(double p, double h, double &r) const;
    EOS_Error compute_d_cv_d_h_p_ph(double p, double h, double &r) const;
    EOS_Error compute_d_sigma_d_p_h_ph(double p, double h, double &r) const;
    EOS_Error compute_d_sigma_d_h_p_ph(double p, double h, double &r) const;
    EOS_Error compute_d_w_d_p_h_ph(double p, double h, double &r) const;
    EOS_Error compute_d_w_d_h_p_ph(double p, double h, double &r) const;
    EOS_Error compute_d_g_d_p_h_ph(double p, double h, double &r) const;
    EOS_Error compute_d_g_d_h_p_ph(double p, double h, double &r) const;
    EOS_Error compute_d_f_d_p_h_ph(double p, double h, double &r) const;
    EOS_Error compute_d_f_d_h_p_ph(double p, double h, double &r) const;
    EOS_Error compute_d_pr_d_p_h_ph(double p, double h, double &r) const;
    EOS_Error compute_d_pr_d_h_p_ph(double p, double h, double &r) const;
    EOS_Error compute_d_beta_d_p_h_ph(double p, double h, double &r) const;
    EOS_Error compute_d_beta_d_h_p_ph(double p, double h, double &r) const;
    EOS_Error compute_d_gamma_d_p_h_ph(double p, double h, double &r) const;
    EOS_Error compute_d_gamma_d_h_p_ph(double p, double h, double &r) const;

    EOS_Error compute_d_h_d_p_T_pT(double p, double T, double &r) const;
    EOS_Error compute_d_h_d_T_p_pT(double p, double T, double &r) const;
    EOS_Error compute_d_rho_d_p_T_pT(double p, double T, double &r) const;
    EOS_Error compute_d_rho_d_T_p_pT(double p, double T, double &r) const;
    EOS_Error compute_d_u_d_p_T_pT(double p, double T, double &r) const;
    EOS_Error compute_d_u_d_T_p_pT(double p, double T, double &r) const;
    EOS_Error compute_d_s_d_p_T_pT(double p, double T, double &r) const;
    EOS_Error compute_d_s_d_T_p_pT(double p, double T, double &r) const;
    EOS_Error compute_d_mu_d_p_T_pT(double p, double T, double &r) const;
    EOS_Error compute_d_mu_d_T_p_pT(double p, double T, double &r) const;
    EOS_Error compute_d_lambda_d_p_T_pT(double p, double T, double &r) const;
    EOS_Error compute_d_lambda_d_T_p_pT(double p, double T, double &r) const;
    EOS_Error compute_d_cp_d_p_T_pT(double p, double T, double &r) const;
    EOS_Error compute_d_cp_d_T_p_pT(double p, double T, double &r) const;
    EOS_Error compute_d_cv_d_p_T_pT(double p, double T, double &r) const;
    EOS_Error compute_d_cv_d_T_p_pT(double p, double T, double &r) const;
    EOS_Error compute_d_sigma_d_p_T_pT(double p, double T, double &r) const;
    EOS_Error compute_d_sigma_d_T_p_pT(double p, double T, double &r) const;
    EOS_Error compute_d_w_d_p_T_pT(double p, double T, double &r) const;
    EOS_Error compute_d_w_d_T_p_pT(double p, double T, double &r) const;
    EOS_Error compute_d_g_d_p_T_pT(double p, double T, double &r) const;
    EOS_Error compute_d_g_d_T_p_pT(double p, double T, double &r) const;
    EOS_Error compute_d_f_d_p_T_pT(double p, double T, double &r) const;
    EOS_Error compute_d_f_d_T_p_pT(double p, double T, double &r) const;
    EOS_Error compute_d_pr_d_p_T_pT(double p, double T, double &r) const;
    EOS_Error compute_d_pr_d_T_p_pT(double p, double T, double &r) const;
    EOS_Error compute_d_beta_d_p_T_pT(double p, double T, double &r) const;
    EOS_Error compute_d_beta_d_T_p_pT(double p, double T, double &r) const;
    EOS_Error compute_d_gamma_d_p_T_pT(double p, double T, double &r) const;
    EOS_Error compute_d_gamma_d_T_p_pT(double p, double T, double &r) const;

    EOS_Error compute_d_h_d_p_s_ps(double p, double s, double &r) const;
    EOS_Error compute_d_h_d_s_p_ps(double p, double s, double &r) const;
    EOS_Error compute_d_T_d_p_s_ps(double p, double s, double &r) const;
    EOS_Error compute_d_T_d_s_p_ps(double p, double s, double &r) const;
    EOS_Error compute_d_rho_d_p_s_ps(double p, double s, double &r) const;
    EOS_Error compute_d_rho_d_s_p_ps(double p, double s, double &r) const;
    EOS_Error compute_d_u_d_p_s_ps(double p, double s, double &r) const;
    EOS_Error compute_d_u_d_s_p_ps(double p, double s, double &r) const;
    EOS_Error compute_d_mu_d_p_s_ps(double p, double s, double &r) const;
    EOS_Error compute_d_mu_d_s_p_ps(double p, double s, double &r) const;
    EOS_Error compute_d_lambda_d_p_s_ps(double p, double s, double &r) const;
    EOS_Error compute_d_lambda_d_s_p_ps(double p, double s, double &r) const;
    EOS_Error compute_d_cp_d_p_s_ps(double p, double s, double &r) const;
    EOS_Error compute_d_cp_d_s_p_ps(double p, double s, double &r) const;
    EOS_Error compute_d_cv_d_p_s_ps(double p, double s, double &r) const;
    EOS_Error compute_d_cv_d_s_p_ps(double p, double s, double &r) const;
    EOS_Error compute_d_sigma_d_p_s_ps(double p, double s, double &r) const;
    EOS_Error compute_d_sigma_d_s_p_ps(double p, double s, double &r) const;
    EOS_Error compute_d_w_d_p_s_ps(double p, double s, double &r) const;
    EOS_Error compute_d_w_d_s_p_ps(double p, double s, double &r) const;
    EOS_Error compute_d_g_d_p_s_ps(double p, double s, double &r) const;
    EOS_Error compute_d_g_d_s_p_ps(double p, double s, double &r) const;
    EOS_Error compute_d_f_d_p_s_ps(double p, double s, double &r) const;
    EOS_Error compute_d_f_d_s_p_ps(double p, double s, double &r) const;
    EOS_Error compute_d_pr_d_p_s_ps(double p, double s, double &r) const;
    EOS_Error compute_d_pr_d_s_p_ps(double p, double s, double &r) const;
    EOS_Error compute_d_beta_d_p_s_ps(double p, double s, double &r) const;
    EOS_Error compute_d_beta_d_s_p_ps(double p, double s, double &r) const;
    EOS_Error compute_d_gamma_d_p_s_ps(double p, double s, double &r) const;
    EOS_Error compute_d_gamma_d_s_p_ps(double p, double s, double &r) const;

    EOS_Error compute_T_sat_p(double p, double &r) const;
    EOS_Error compute_rho_l_sat_p(double p, double &r) const;
    EOS_Error compute_rho_v_sat_p(double p, double &r) const;
    EOS_Error compute_h_l_sat_p(double p, double &r) const;
    EOS_Error compute_h_v_sat_p(double p, double &r) const;
    EOS_Error compute_cp_l_sat_p(double p, double &r) const;
    EOS_Error compute_cp_v_sat_p(double p, double &r) const;

    EOS_Error compute_p_sat_T(double T, double &r) const;
    EOS_Error compute_rho_l_sat_T(double T, double &r) const;
    EOS_Error compute_rho_v_sat_T(double T, double &r) const;
    EOS_Error compute_h_l_sat_T(double T, double &r) const;
    EOS_Error compute_h_v_sat_T(double T, double &r) const;
    EOS_Error compute_cp_l_sat_T(double T, double &r) const;
    EOS_Error compute_cp_v_sat_T(double T, double &r) const;

    EOS_Error compute_d_T_sat_d_p_p(double p, double &r) const;
    EOS_Error compute_d_rho_l_sat_d_p_p(double p, double &r) const;
    EOS_Error compute_d_rho_v_sat_d_p_p(double p, double &r) const;
    EOS_Error compute_d_h_l_sat_d_p_p(double p, double &r) const;
    EOS_Error compute_d_h_v_sat_d_p_p(double p, double &r) const;
    EOS_Error compute_d_cp_l_sat_d_p_p(double p, double &r) const;
    EOS_Error compute_d_cp_v_sat_d_p_p(double p, double &r) const;

    EOS_Error compute_d_p_sat_d_T_T(double T, double &r) const;
    EOS_Error compute_d_rho_l_sat_d_T_T(double T, double &r) const;
    EOS_Error compute_d_rho_v_sat_d_T_T(double T, double &r) const;
    EOS_Error compute_d_h_l_sat_d_T_T(double T, double &r) const;
    EOS_Error compute_d_h_v_sat_d_T_T(double T, double &r) const;
    EOS_Error compute_d_cp_l_sat_d_T_T(double T, double &r) const;
    EOS_Error compute_d_cp_v_sat_d_T_T(double T, double &r) const;

    EOS_Error compute_d2_T_sat_d_p_d_p_p(double p, double &r) const;
    EOS_Error compute_d2_rho_l_sat_d_p_d_p_p(double p, double &r) const;
    EOS_Error compute_d2_rho_v_sat_d_p_d_p_p(double p, double &r) const;
    EOS_Error compute_d2_h_l_sat_d_p_d_p_p(double p, double &r) const;
    EOS_Error compute_d2_h_v_sat_d_p_d_p_p(double p, double &r) const;
    EOS_Error compute_d2_cp_l_sat_d_p_d_p_p(double p, double &r) const;
    EOS_Error compute_d2_cp_v_sat_d_p_d_p_p(double p, double &r) const;

    EOS_Error compute_d2_p_sat_d_T_d_T_T(double T, double &r) const;
    EOS_Error compute_d2_rho_l_sat_d_T_d_T_T(double T, double &r) const;
    EOS_Error compute_d2_rho_v_sat_d_T_d_T_T(double T, double &r) const;
    EOS_Error compute_d2_h_l_sat_d_T_d_T_T(double T, double &r) const;
    EOS_Error compute_d2_h_v_sat_d_T_d_T_T(double T, double &r) const;
    EOS_Error compute_d2_cp_l_sat_d_T_d_T_T(double T, double &r) const;
    EOS_Error compute_d2_cp_v_sat_d_T_d_T_T(double T, double &r) const;

    EOS_Error compute_h_l_lim_p(double p, double &r) const;
    EOS_Error compute_h_v_lim_p(double p, double &r) const;
    EOS_Error compute_d_h_l_lim_d_p_p(double p, double &r) const;
    EOS_Error compute_d_h_v_lim_d_p_p(double p, double &r) const;
    EOS_Error compute_d2_h_l_lim_d_p_d_p_p(double p, double &r) const;
    EOS_Error compute_d2_h_v_lim_d_p_d_p_p(double p, double &r) const;

    EOS_Error compute_T_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_rho_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_u_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_s_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_mu_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_lambda_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_cp_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_cv_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_sigma_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_w_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_g_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_f_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_pr_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_beta_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_gamma_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;

    EOS_Error compute_h_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_rho_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_u_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_s_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_mu_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_lambda_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_cp_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_cv_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_sigma_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_w_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_g_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_f_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_pr_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_beta_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_gamma_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;

    EOS_Error compute_h_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_T_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_rho_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_u_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_mu_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_lambda_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_cp_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_cv_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_sigma_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_w_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_g_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_f_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_pr_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_beta_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_gamma_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;

    EOS_Error compute_d_T_d_p_h_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_T_d_h_p_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_T_d_c_0_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_T_d_c_1_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_T_d_c_2_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_T_d_c_3_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_T_d_c_4_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_rho_d_p_h_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_rho_d_h_p_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_rho_d_c_0_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_rho_d_c_1_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_rho_d_c_2_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_rho_d_c_3_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_rho_d_c_4_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_u_d_p_h_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_u_d_h_p_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_u_d_c_0_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_u_d_c_1_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_u_d_c_2_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_u_d_c_3_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_u_d_c_4_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_s_d_p_h_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_s_d_h_p_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_s_d_c_0_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_s_d_c_1_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_s_d_c_2_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_s_d_c_3_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_s_d_c_4_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_mu_d_p_h_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_mu_d_h_p_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_mu_d_c_0_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_mu_d_c_1_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_mu_d_c_2_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_mu_d_c_3_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_mu_d_c_4_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_lambda_d_p_h_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_lambda_d_h_p_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_lambda_d_c_0_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_lambda_d_c_1_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_lambda_d_c_2_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_lambda_d_c_3_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_lambda_d_c_4_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_cp_d_p_h_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_cp_d_h_p_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_cp_d_c_0_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_cp_d_c_1_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_cp_d_c_2_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_cp_d_c_3_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_cp_d_c_4_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_cv_d_p_h_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_cv_d_h_p_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_cv_d_c_0_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_cv_d_c_1_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_cv_d_c_2_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_cv_d_c_3_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_cv_d_c_4_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_sigma_d_p_h_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_sigma_d_h_p_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_sigma_d_c_0_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_sigma_d_c_1_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_sigma_d_c_2_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_sigma_d_c_3_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_sigma_d_c_4_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_w_d_p_h_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_w_d_h_p_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_w_d_c_0_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_w_d_c_1_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_w_d_c_2_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_w_d_c_3_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_w_d_c_4_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_g_d_p_h_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_g_d_h_p_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_g_d_c_0_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_g_d_c_1_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_g_d_c_2_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_g_d_c_3_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_g_d_c_4_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_f_d_p_h_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_f_d_h_p_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_f_d_c_0_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_f_d_c_1_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_f_d_c_2_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_f_d_c_3_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_f_d_c_4_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_pr_d_p_h_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_pr_d_h_p_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_pr_d_c_0_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_pr_d_c_1_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_pr_d_c_2_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_pr_d_c_3_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_pr_d_c_4_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_beta_d_p_h_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_beta_d_h_p_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_beta_d_c_0_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_beta_d_c_1_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_beta_d_c_2_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_beta_d_c_3_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_beta_d_c_4_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_gamma_d_p_h_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_gamma_d_h_p_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_gamma_d_c_0_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_gamma_d_c_1_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_gamma_d_c_2_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_gamma_d_c_3_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_gamma_d_c_4_ph_ph(double p, double h, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;

    EOS_Error compute_d_h_d_p_T_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_h_d_T_p_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_h_d_c_0_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_h_d_c_1_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_h_d_c_2_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_h_d_c_3_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_h_d_c_4_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_rho_d_p_T_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_rho_d_T_p_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_rho_d_c_0_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_rho_d_c_1_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_rho_d_c_2_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_rho_d_c_3_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_rho_d_c_4_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_u_d_p_T_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_u_d_T_p_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_u_d_c_0_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_u_d_c_1_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_u_d_c_2_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_u_d_c_3_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_u_d_c_4_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_s_d_p_T_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_s_d_T_p_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_s_d_c_0_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_s_d_c_1_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_s_d_c_2_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_s_d_c_3_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_s_d_c_4_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_mu_d_p_T_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_mu_d_T_p_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_mu_d_c_0_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_mu_d_c_1_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_mu_d_c_2_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_mu_d_c_3_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_mu_d_c_4_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_lambda_d_p_T_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_lambda_d_T_p_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_lambda_d_c_0_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_lambda_d_c_1_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_lambda_d_c_2_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_lambda_d_c_3_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_lambda_d_c_4_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_cp_d_p_T_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_cp_d_T_p_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_cp_d_c_0_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_cp_d_c_1_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_cp_d_c_2_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_cp_d_c_3_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_cp_d_c_4_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_cv_d_p_T_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_cv_d_T_p_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_cv_d_c_0_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_cv_d_c_1_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_cv_d_c_2_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_cv_d_c_3_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_cv_d_c_4_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_sigma_d_p_T_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_sigma_d_T_p_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_sigma_d_c_0_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_sigma_d_c_1_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_sigma_d_c_2_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_sigma_d_c_3_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_sigma_d_c_4_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_w_d_p_T_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_w_d_T_p_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_w_d_c_0_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_w_d_c_1_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_w_d_c_2_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_w_d_c_3_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_w_d_c_4_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_g_d_p_T_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_g_d_T_p_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_g_d_c_0_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_g_d_c_1_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_g_d_c_2_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_g_d_c_3_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_g_d_c_4_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_f_d_p_T_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_f_d_T_p_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_f_d_c_0_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_f_d_c_1_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_f_d_c_2_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_f_d_c_3_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_f_d_c_4_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_pr_d_p_T_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_pr_d_T_p_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_pr_d_c_0_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_pr_d_c_1_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_pr_d_c_2_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_pr_d_c_3_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_pr_d_c_4_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_beta_d_p_T_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_beta_d_T_p_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_beta_d_c_0_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_beta_d_c_1_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_beta_d_c_2_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_beta_d_c_3_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_beta_d_c_4_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_gamma_d_p_T_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_gamma_d_T_p_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_gamma_d_c_0_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_gamma_d_c_1_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_gamma_d_c_2_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_gamma_d_c_3_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_gamma_d_c_4_pT_pT(double p, double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;

    EOS_Error compute_d_h_d_p_s_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_h_d_s_p_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_h_d_c_0_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_h_d_c_1_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_h_d_c_2_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_h_d_c_3_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_h_d_c_4_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_T_d_p_s_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_T_d_s_p_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_T_d_c_0_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_T_d_c_1_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_T_d_c_2_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_T_d_c_3_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_T_d_c_4_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_rho_d_p_s_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_rho_d_s_p_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_rho_d_c_0_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_rho_d_c_1_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_rho_d_c_2_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_rho_d_c_3_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_rho_d_c_4_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_u_d_p_s_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_u_d_s_p_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_u_d_c_0_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_u_d_c_1_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_u_d_c_2_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_u_d_c_3_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_u_d_c_4_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_mu_d_p_s_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_mu_d_s_p_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_mu_d_c_0_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_mu_d_c_1_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_mu_d_c_2_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_mu_d_c_3_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_mu_d_c_4_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_lambda_d_p_s_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_lambda_d_s_p_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_lambda_d_c_0_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_lambda_d_c_1_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_lambda_d_c_2_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_lambda_d_c_3_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_lambda_d_c_4_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_cp_d_p_s_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_cp_d_s_p_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_cp_d_c_0_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_cp_d_c_1_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_cp_d_c_2_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_cp_d_c_3_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_cp_d_c_4_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_cv_d_p_s_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_cv_d_s_p_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_cv_d_c_0_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_cv_d_c_1_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_cv_d_c_2_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_cv_d_c_3_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_cv_d_c_4_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_sigma_d_p_s_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_sigma_d_s_p_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_sigma_d_c_0_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_sigma_d_c_1_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_sigma_d_c_2_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_sigma_d_c_3_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_sigma_d_c_4_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_w_d_p_s_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_w_d_s_p_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_w_d_c_0_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_w_d_c_1_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_w_d_c_2_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_w_d_c_3_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_w_d_c_4_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_g_d_p_s_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_g_d_s_p_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_g_d_c_0_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_g_d_c_1_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_g_d_c_2_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_g_d_c_3_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_g_d_c_4_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_f_d_p_s_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_f_d_s_p_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_f_d_c_0_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_f_d_c_1_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_f_d_c_2_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_f_d_c_3_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_f_d_c_4_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_pr_d_p_s_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_pr_d_s_p_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_pr_d_c_0_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_pr_d_c_1_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_pr_d_c_2_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_pr_d_c_3_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_pr_d_c_4_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const;
    EOS_Error compute_d_beta_d_p_s_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_beta_d_s_p_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_beta_d_c_0_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_beta_d_c_1_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_beta_d_c_2_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_beta_d_c_3_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_beta_d_c_4_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_gamma_d_p_s_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_gamma_d_s_p_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_gamma_d_c_0_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_gamma_d_c_1_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_gamma_d_c_2_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_gamma_d_c_3_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_gamma_d_c_4_ps_ps(double p, double s, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;

    EOS_Error compute_T_sat_p(double p, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_rho_l_sat_p(double p, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_rho_v_sat_p(double p, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_h_l_sat_p(double p, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_h_v_sat_p(double p, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_cp_l_sat_p(double p, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_cp_v_sat_p(double p, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;

    EOS_Error compute_p_sat_T(double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_rho_l_sat_T(double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_rho_v_sat_T(double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_h_l_sat_T(double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_h_v_sat_T(double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_cp_l_sat_T(double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_cp_v_sat_T(double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;

    EOS_Error compute_d_T_sat_d_p_p(double p, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_rho_l_sat_d_p_p(double p, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_rho_v_sat_d_p_p(double p, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_h_l_sat_d_p_p(double p, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_h_v_sat_d_p_p(double p, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_cp_l_sat_d_p_p(double p, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_cp_v_sat_d_p_p(double p, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;

    EOS_Error compute_d_p_sat_d_T_T(double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_rho_l_sat_d_T_T(double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_rho_v_sat_d_T_T(double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_h_l_sat_d_T_T(double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_h_v_sat_d_T_T(double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_cp_l_sat_d_T_T(double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d_cp_v_sat_d_T_T(double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;

    EOS_Error compute_d2_T_sat_d_p_d_p_p(double p, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d2_rho_l_sat_d_p_d_p_p(double p, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d2_rho_v_sat_d_p_d_p_p(double p, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d2_h_l_sat_d_p_d_p_p(double p, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d2_h_v_sat_d_p_d_p_p(double p, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d2_cp_l_sat_d_p_d_p_p(double p, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d2_cp_v_sat_d_p_d_p_p(double p, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;

    EOS_Error compute_d2_p_sat_d_T_d_T_T(double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d2_rho_l_sat_d_T_d_T_T(double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d2_rho_v_sat_d_T_d_T_T(double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d2_h_l_sat_d_T_d_T_T(double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d2_h_v_sat_d_T_d_T_T(double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d2_cp_l_sat_d_T_d_T_T(double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;
    EOS_Error compute_d2_cp_v_sat_d_T_d_T_T(double T, double &r, double c_0,
                   double c_1=0, double c_2=0, double c_3=0, double c_4=0) const ;

    static int test() ;

    //! Specify a new error handler. The error handler must be initialized.
    //! Il will be duplicated and stored in the EOS object so the caller
    //! can destroy it's own instance of the handler.
    void set_error_handler(const EOS_Error_Handler  &handler) ;

    //! Save the current error handler on a stack for future reuse
    //! see also EOS::restore_error_handler()
    void save_error_handler() ;

    //! Restore previously saved error handler. If no previously saved
    //! handler, reset to default error handler.
    void restore_error_handler() ;

    //! fluid() is used in EOS_mixing which is a nested EOS object.
    //! Could not find a syntax to hide thid method
     EOS_Fluid  &fluid() const ;
    
    //! list of all properties (for a given fluid) for which compute_* functions are implemented
    vector<string> is_implemented(const char *pprop1, const char *pprop2, 
                                  const vector<string> list_prop, double vprop1, double vprop2) ;
    
    //! list of all properties (for a given fluid for sat or lim) for which compute_* functions are implemented
    vector<string> is_implemented(const char *pprop1, const vector<string> list_prop, double vprop1) ;

    //
    void get_index_file(AString &index_file_name) ;
    const char* INDEX(const char* const method_str, const char* const ref_str) ;
    int index_data(const char *const tmname_in, const char *const fename_in) ;

    const string info_thermmodel()    const ; // Thermodynamic Model Name
    const string info_fluidequa()     const ; // Fluid Equation Name
    const string info_eosthermmodel() const ; // EOS Thermodynamic Model Name
    const string info_tmdatadir()     const ; // Thermodynamic Model Data Directory
    const string info_tmversion()     const ; // Thermodynamic Model Version
    const string info_mfluid()        const ; // Fluid Name
    const string info_mphase()        const ; // Phase Name

   
  private :
    char fluid_model_class[200] ;
    Strings fluid_model_args ;
    string therm_model     = "" ;  // Thermodynamic Model Name
    string fluid_equa      = "" ;  // Fluid Equation Name
    string eos_therm_model = "" ;  // EOS Thermodynamic Model Name
    string tm_data_dir     = "" ;  // Thermodynamic Model Data Directory
    string tm_version      = "" ;  // Thermodynamic Model version
    string m_fluid         = "" ;  // Fluid Name
    string m_phase         = "" ;  // Phase Name
    
    EOS_Fluid &fluid_model_obj ;
    //! Pointer to the error_handler (The EOS object owns the handler:
    //!  err_handler is destroyed when the EOS object dies).
    //! The pointer must never be null.
    EOS_Error_Handler *err_handler ;
    //! Stack of saved error_handlers
    EOS_Error_Handler_Stack err_handler_stack_;

    
  //friends
    friend std::ostream &operator<<(std::ostream &flux, const EOS &eos) ;
  };
}
#include "EOS_i.hxx"
#endif
