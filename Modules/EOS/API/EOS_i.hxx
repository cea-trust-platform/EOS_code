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
#ifndef EOS_i_HXX
#define EOS_i_HXX

#if !defined(__APPLE__)
  #include <features.h>
#endif

#include "Language/API/Object_i.hxx"
#include "EOS_Fluid.hxx"
#include "EOS_Field.hxx"
#include "EOS_Fields.hxx"
#include "EOS_Error_Field.hxx"
#include "EOS_Error_Handler.hxx"

// These macros call the error handler with appropriate arguments.
//
// If NO_EOS_ERROR is defined, then no error checking is performed (quick and dirty).
// __PRETTY_FUNCTION__ is a magical variable containing the demangled name
// of the function beeing defined (gcc 2.6 and later), for use in the error handler.
// If not available, use __FILE__ or __LINE__. See <assert.h>
#ifndef NO_EOS_ERROR
#define RETURN_ERROR(error_code,arg1,arg2,resu) return err_handler->handle_error_point(fluid_model_obj,error_code,__PRETTY_FUNCTION__,arg1,arg2,resu)
#define RETURN_ERROR_FIELD(error_field,arg1,arg2,resu) return err_handler->handle_error_field(fluid_model_obj,error_field,__PRETTY_FUNCTION__,arg1,arg2,resu)
#else
#define RETURN_ERROR(error_code,arg1,arg2,resu) return EOS_Error::ok
#define RETURN_ERROR_FIELD(error_code,arg1,arg2,resu) return EOS_Error::ok
#endif

namespace NEPTUNE
{
  inline int EOS::init(const Strings& args)
  { return fluid_model_obj.init(args) ;
  }

  inline int EOS::set_components(EOS** components, int nb)
  { return fluid_model_obj.set_components(components, nb) ;
  }
  
  inline void EOS::set_reference_state(double h, double s, double T, double p)
  { return fluid_model_obj.set_reference_state(h, s, T, p) ;
  }

  // provisional for EOS_Mixing
  inline double EOS::get_prxr()
  { return fluid_model_obj.get_prxr() ;
  }

  // provisional for EOS_Mixing
  inline double EOS::get_prxcp()
  { return fluid_model_obj.get_prxcp() ;
  }
  // provisional for EOS_Mixing
  inline double EOS::get_prxm()
  { return fluid_model_obj.get_prxm() ;
  }

  // provisional for EOS_Mixing
  inline double EOS::get_prxl0()
  { return fluid_model_obj.get_prxl0() ;
  }

  // provisional for EOS_Mixing
  inline double EOS::get_prxl1()
  { return fluid_model_obj.get_prxl1() ;
  }

  // provisional for EOS_Mixing
  inline double EOS::get_prxl2()
  { return fluid_model_obj.get_prxl2() ;
  }

  // provisional for EOS_Mixing
  inline double EOS::get_prxm0()
  {
    return fluid_model_obj.get_prxm0() ;
  }

  // provisional for EOS_Mixing
  inline double EOS::get_prxm1()
  { return fluid_model_obj.get_prxm1() ;
  }

  // provisional for EOS_Mixing
  inline double EOS::get_prxm2()
  { return fluid_model_obj.get_prxm2() ;
  }

  // provisional for EOS_Mixing
  inline double EOS::get_prxdv()
  { return fluid_model_obj.get_prxdv() ;
  }

  // provisional for EOS_Mixing
  inline double EOS::is_liquid(double p, double h) const
  { return fluid_model_obj.is_liquid(p,h) ;
  }

  inline const AString& EOS::phase_name() const
  { return fluid_model_obj.phase_name() ;
  }

  inline const AString& EOS::equation_name() const
  { return fluid_model_obj.equation_name() ;
  }

  inline const AString& EOS::fluid_name() const
  { return fluid_model_obj.fluid_name() ;
  }

  inline const AString& EOS::table_name() const
  { return fluid_model_obj.table_name() ;
  }

  inline const AString& EOS::version_name() const
  { return fluid_model_obj.version_name() ;
  }

  inline EOS_Error EOS::compute(const EOS_Field& p, 
                                const EOS_Field& h, 
                                EOS_Field& r, 
                                EOS_Error_Field& errfield) const
  { fluid_model_obj.compute(p, h, r, errfield) ;
    RETURN_ERROR_FIELD(errfield, &p, &h, &r) ;
  }

  inline EOS_Error EOS::compute(const EOS_Fields& input, 
                                EOS_Field& r, 
                                EOS_Error_Field& errfield) const
  { fluid_model_obj.compute(input, r, errfield) ;
    RETURN_ERROR_FIELD(errfield, &input, NULL, &r) ;
  }

  inline EOS_Error EOS::compute(const EOS_Fields& input, 
                                EOS_Fields& r, 
                                EOS_Error_Field& errfield) const
  { fluid_model_obj.compute(input, r, errfield) ;
    RETURN_ERROR_FIELD(errfield, &input, NULL, &r) ;
  }

  inline EOS_Error EOS::compute(const EOS_Field& p, 
                                EOS_Fields& r, 
                                EOS_Error_Field& errfield) const
  { fluid_model_obj.compute(p, r, errfield) ;
    RETURN_ERROR_FIELD(errfield, &p, NULL, &r) ;
  }

  inline EOS_Error EOS::compute(const EOS_Field& p, 
                                const EOS_Field& h, 
                                EOS_Fields& r, 
                                EOS_Error_Field& errfield) const
  { fluid_model_obj.compute(p, h, r, errfield) ;
    RETURN_ERROR_FIELD(errfield, &p, &h, &r) ;
  }

  inline EOS_Error EOS::compute(const EOS_Field& p, 
                                EOS_Field& r, 
                                EOS_Error_Field& errfield) const
  { fluid_model_obj.compute(p, r, errfield);
    RETURN_ERROR_FIELD(errfield, &p, NULL, &r);
  }

  inline EOS_Error EOS::compute(const char* const property_name,
                                double p, double h, double& x) const
  { EOS_Internal_Error err = fluid_model_obj.compute(property_name, p, h, x) ;
    RETURN_ERROR(err, p, h, x) ;
  } 

  inline EOS_Error EOS::compute(const char* const property_name,
                                double p, double& x) const
  { EOS_Internal_Error err = fluid_model_obj.compute(property_name, p, x) ;
    RETURN_ERROR(err, p, 0.e0, x) ;
  }

  inline EOS_Error EOS::compute_Ph(const char* const property_name, 
                                   double in1, double in2, double& out) const
  { EOS_Internal_Error err = fluid_model_obj.compute_Ph(property_name, in1, in2, out) ;
    RETURN_ERROR(err, in1, in2, out) ;
  }

  inline EOS_Error EOS::compute_PT(const char* const property_name, 
                                   double in1, double in2, double& out) const
  { EOS_Internal_Error err = fluid_model_obj.compute_PT(property_name, in1, in2, out) ;
    RETURN_ERROR(err, in1, in2, out) ;
  }

  inline EOS_Error EOS::compute_Psat(const char* const property_name,
                                  double in, double& out) const
  { EOS_Internal_Error err = fluid_model_obj.compute_Psat(property_name, in, out) ;
    RETURN_ERROR(err, in, 0.e0, out) ;
  }

  inline EOS_Error EOS::compute_Tsat(const char* const property_name, 
                                  double in, double& out) const
  { EOS_Internal_Error err = fluid_model_obj.compute_Tsat(property_name, in, out) ;
    RETURN_ERROR(err, in, 0.e0, out) ;
  }

  inline EOS_Error EOS::get_p(double& p) const
  { EOS_Internal_Error err = fluid_model_obj.get_p(p) ;
    RETURN_ERROR(err, 0.e0, 0.e0, p) ;
  }

  inline EOS_Error EOS::get_mm(double& mm) const
  { EOS_Internal_Error err = fluid_model_obj.get_mm(mm) ;
    RETURN_ERROR(err, 0.e0, 0.e0, mm) ;
  }

  inline EOS_Error EOS::get_p_crit(double& p_crit) const
  { EOS_Internal_Error err = fluid_model_obj.get_p_crit(p_crit) ;
    RETURN_ERROR(err, 0.e0, 0.e0, p_crit) ;
  }

  inline EOS_Error EOS::get_h_crit(double& h_crit) const
  { EOS_Internal_Error err = fluid_model_obj.get_h_crit(h_crit) ;
    RETURN_ERROR(err, 0.e0, 0.e0, h_crit) ;
  }

  inline EOS_Error EOS::get_T_crit(double& T_crit) const
  { EOS_Internal_Error err = fluid_model_obj.get_T_crit(T_crit) ;
    RETURN_ERROR(err, 0.e0, 0.e0, T_crit) ;
  }

  inline EOS_Error EOS::get_rho_crit(double& rho_crit) const
  { EOS_Internal_Error err = fluid_model_obj.get_rho_crit(rho_crit) ;
    RETURN_ERROR(err, 0.e0, 0.e0, rho_crit) ;
  }

  inline EOS_Error EOS::get_p_min(double& p_min) const
  { EOS_Internal_Error err = fluid_model_obj.get_p_min(p_min) ;
    RETURN_ERROR(err, 0.e0, 0.e0, p_min) ;
  }

  inline EOS_Error EOS::get_p_max(double& p_max) const
  { EOS_Internal_Error err = fluid_model_obj.get_p_max(p_max) ;
    RETURN_ERROR(err, 0, 0, p_max) ;
  }

  inline EOS_Error EOS::get_h_max(double& h_max) const
  { EOS_Internal_Error err = fluid_model_obj.get_h_max(h_max) ;
    RETURN_ERROR(err, 0.e0, 0.e0, h_max) ;
  }

  inline double EOS::get_h_max() const
  { std::cerr<<"Value of h_max not initialised by fluid_model_obj"; 
    return 3.6; // Correspond to a patch for RP in code Cathare.
  }

  inline EOS_Error EOS::get_h_min(double& h_min) const
  { EOS_Internal_Error err = fluid_model_obj.get_h_min(h_min) ;
    RETURN_ERROR(err, 0.e0, 0.e0, h_min) ;
  }

  inline EOS_Error EOS::get_T_max(double& T_max) const
  { EOS_Internal_Error err = fluid_model_obj.get_T_max(T_max) ;
    RETURN_ERROR(err, 0.e0, 0.e0, T_max) ;
  }

  inline EOS_Error EOS::get_T_min(double& T_min) const
  { EOS_Internal_Error err = fluid_model_obj.get_T_min(T_min) ;
    RETURN_ERROR(err, 0.e0, 0.e0, T_min) ;
  }

  inline EOS_Error EOS::get_rho_max(double& rho_max) const
  { EOS_Internal_Error err = fluid_model_obj.get_rho_max(rho_max) ;
    RETURN_ERROR(err, 0.e0, 0.e0, rho_max) ;
  }

  inline EOS_Error EOS::get_rho_min(double& rho_min) const
  { EOS_Internal_Error err = fluid_model_obj.get_rho_min(rho_min) ;
    RETURN_ERROR(err, 0.e0, 0.e0, rho_min) ;
  }

  inline EOS_Error EOS::get_p_ref(double& p_ref) const
  { EOS_Internal_Error err = fluid_model_obj.get_p_ref(p_ref) ;
    RETURN_ERROR(err, 0.e0, 0.e0, p_ref) ;
  }

  inline EOS_Error EOS::get_h_ref(double& h_ref) const
  { EOS_Internal_Error err = fluid_model_obj.get_h_ref(h_ref) ;
    RETURN_ERROR(err, 0.e0, 0.e0, h_ref) ;
  }

  inline EOS_Error EOS::get_T_ref(double& T_ref) const
  { EOS_Internal_Error err = fluid_model_obj.get_T_ref(T_ref) ;
    RETURN_ERROR(err, 0.e0, 0.e0, T_ref) ;
  }

  inline EOS_Error EOS::compute_T_ph(double p, double h, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_T_ph(p, h, r) ;
    RETURN_ERROR(err, p, h, r) ;
  }

  inline EOS_Error EOS::compute_rho_ph(double p, double h, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_rho_ph(p, h, r) ;
    RETURN_ERROR(err, p, h, r) ;
  }

  inline EOS_Error EOS::compute_u_ph(double p, double h, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_u_ph(p, h, r) ;
    RETURN_ERROR(err, p, h, r) ;
  }

  inline EOS_Error EOS::compute_s_ph(double p, double h, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_s_ph(p, h, r) ;
    RETURN_ERROR(err, p, h, r) ;
  }

  inline EOS_Error EOS::compute_mu_ph(double p, double h, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_mu_ph(p, h, r) ;
    RETURN_ERROR(err, p, h, r) ;
  }

  inline EOS_Error EOS::compute_lambda_ph(double p, double h, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_lambda_ph(p, h, r) ;
    RETURN_ERROR(err, p, h, r) ;
  }

  inline EOS_Error EOS::compute_cp_ph(double p, double h, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_cp_ph(p, h, r) ;
    RETURN_ERROR(err, p, h, r) ;
  }

  inline EOS_Error EOS::compute_cv_ph(double p, double h, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_cv_ph(p, h, r) ;
    RETURN_ERROR(err, p, h, r) ;
  }

  inline EOS_Error EOS::compute_sigma_ph(double p, double h, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_sigma_ph(p, h, r) ;
    RETURN_ERROR(err, p, h, r) ;
  }

  inline EOS_Error EOS::compute_w_ph(double p, double h, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_w_ph(p, h, r) ;
    RETURN_ERROR(err, p, h, r) ;
  }

  inline EOS_Error EOS::compute_g_ph(double p, double h, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_g_ph(p, h, r) ;
    RETURN_ERROR(err, p, h, r) ;
  }

  inline EOS_Error EOS::compute_f_ph(double p, double h, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_f_ph(p, h, r) ;
    RETURN_ERROR(err, p, h, r) ;
  }

  inline EOS_Error EOS::compute_pr_ph(double p, double h, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_pr_ph(p, h, r) ;
    RETURN_ERROR(err, p, h, r) ;
  }

  inline EOS_Error EOS::compute_beta_ph(double p, double h, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_beta_ph(p, h, r) ;
    RETURN_ERROR(err, p, h, r) ;
  }

  inline EOS_Error EOS::compute_gamma_ph(double p, double h, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_gamma_ph(p, h, r) ;
    RETURN_ERROR(err, p, h, r) ;
  }

  inline EOS_Error EOS::compute_h_pT(double p, double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_h_pT(p, T, r) ;
    RETURN_ERROR(err, p, T, r) ;
  }

  inline EOS_Error EOS::compute_rho_pT(double p, double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_rho_pT(p, T, r) ;
    RETURN_ERROR(err, p, T, r) ;
  }

  inline EOS_Error EOS::compute_u_pT(double p, double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_u_pT(p, T, r) ;
    RETURN_ERROR(err, p, T, r) ;
  }

  inline EOS_Error EOS::compute_s_pT(double p, double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_s_pT(p, T, r) ;
    RETURN_ERROR(err, p, T, r) ;
  }

  inline EOS_Error EOS::compute_mu_pT(double p, double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_mu_pT(p, T, r) ;
    RETURN_ERROR(err, p, T, r) ;
  }

  inline EOS_Error EOS::compute_lambda_pT(double p, double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_lambda_pT(p, T, r) ;
    RETURN_ERROR(err, p, T, r) ;
  }

  inline EOS_Error EOS::compute_cp_pT(double p, double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_cp_pT(p, T, r) ;
    RETURN_ERROR(err, p, T, r) ;
  }

  inline EOS_Error EOS::compute_cv_pT(double p, double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_cv_pT(p, T, r) ;
    RETURN_ERROR(err, p, T, r) ;
  }

  inline EOS_Error EOS::compute_sigma_pT(double p, double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_sigma_pT(p, T, r) ;
    RETURN_ERROR(err, p, T, r) ;
  }

  inline EOS_Error EOS::compute_w_pT(double p, double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_w_pT(p, T, r) ;
    RETURN_ERROR(err, p, T, r) ;
  }

  inline EOS_Error EOS::compute_g_pT(double p, double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_g_pT(p, T, r) ;
    RETURN_ERROR(err, p, T, r) ;
  }

  inline EOS_Error EOS::compute_f_pT(double p, double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_f_pT(p, T, r) ;
    RETURN_ERROR(err, p, T, r) ;
  }

  inline EOS_Error EOS::compute_pr_pT(double p, double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_pr_pT(p, T, r) ;
    RETURN_ERROR(err, p, T, r) ;
  }

  inline EOS_Error EOS::compute_beta_pT(double p, double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_beta_pT(p, T, r) ;
    RETURN_ERROR(err, p, T, r) ;
  }

  inline EOS_Error EOS::compute_gamma_pT(double p, double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_gamma_pT(p, T, r) ;
    RETURN_ERROR(err, p, T, r) ;
  }

  inline EOS_Error EOS::compute_h_ps(double p, double s, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_h_ps(p, s, r) ;
    RETURN_ERROR(err, p, s, r) ;
  }

  inline EOS_Error EOS::compute_T_ps(double p, double s, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_T_ps(p, s, r) ;
    RETURN_ERROR(err, p, s, r) ;
  }

  inline EOS_Error EOS::compute_rho_ps(double p, double s, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_rho_ps(p, s, r) ;
    RETURN_ERROR(err, p, s, r) ;
  }

  inline EOS_Error EOS::compute_u_ps(double p, double s, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_u_ps(p, s, r) ;
    RETURN_ERROR(err, p, s, r) ;
  }

  inline EOS_Error EOS::compute_mu_ps(double p, double s, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_mu_ps(p, s, r) ;
    RETURN_ERROR(err, p, s, r) ;
  }

  inline EOS_Error EOS::compute_lambda_ps(double p, double s, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_lambda_ps(p, s, r) ;
    RETURN_ERROR(err, p, s, r) ;
  }

  inline EOS_Error EOS::compute_cp_ps(double p, double s, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_cp_ps(p, s, r) ;
    RETURN_ERROR(err, p, s, r) ;
  }

  inline EOS_Error EOS::compute_cv_ps(double p, double s, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_cv_ps(p, s, r) ;
    RETURN_ERROR(err, p, s, r) ;
  }

  inline EOS_Error EOS::compute_sigma_ps(double p, double s, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_sigma_ps(p, s, r) ;
    RETURN_ERROR(err, p, s, r) ;
  }

  inline EOS_Error EOS::compute_w_ps(double p, double s, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_w_ps(p, s, r) ;
    RETURN_ERROR(err, p, s, r) ;
  }

  inline EOS_Error EOS::compute_g_ps(double p, double s, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_g_ps(p, s, r) ;
    RETURN_ERROR(err, p, s, r) ;
  }

  inline EOS_Error EOS::compute_f_ps(double p, double s, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_f_ps(p, s, r) ;
    RETURN_ERROR(err, p, s, r) ;
  }

  inline EOS_Error EOS::compute_pr_ps(double p, double s, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_pr_ps(p, s, r) ;
    RETURN_ERROR(err, p, s, r) ;
  }

  inline EOS_Error EOS::compute_beta_ps(double p, double s, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_beta_ps(p, s, r) ;
    RETURN_ERROR(err, p, s, r) ;
  }

  inline EOS_Error EOS::compute_gamma_ps(double p, double s, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_gamma_ps(p, s, r) ;
    RETURN_ERROR(err, p, s, r) ;
  }

  inline EOS_Error EOS::compute_d_T_d_p_h_ph(double p, double h, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_T_d_p_h_ph(p, h, r) ;
    RETURN_ERROR(err, p, h, r) ;
  }

  inline EOS_Error EOS::compute_d_T_d_h_p_ph(double p, double h, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_T_d_h_p_ph(p, h, r) ;
    RETURN_ERROR(err, p, h, r) ;
  }

  inline EOS_Error EOS::compute_d_rho_d_p_h_ph(double p, double h, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_rho_d_p_h_ph(p, h, r) ;
    RETURN_ERROR(err, p, h, r) ;
  }

  inline EOS_Error EOS::compute_d_rho_d_h_p_ph(double p, double h, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_rho_d_h_p_ph(p, h, r) ;
    RETURN_ERROR(err, p, h, r) ;
  }

  inline EOS_Error EOS::compute_d_u_d_p_h_ph(double p, double h, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_u_d_p_h_ph(p, h, r) ;
    RETURN_ERROR(err, p, h, r) ;
  }

  inline EOS_Error EOS::compute_d_u_d_h_p_ph(double p, double h, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_u_d_h_p_ph(p, h, r) ;
    RETURN_ERROR(err, p, h, r) ;
  }

  inline EOS_Error EOS::compute_d_s_d_p_h_ph(double p, double h, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_s_d_p_h_ph(p, h, r) ;
    RETURN_ERROR(err, p, h, r) ;
  }

  inline EOS_Error EOS::compute_d_s_d_h_p_ph(double p, double h, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_s_d_h_p_ph(p, h, r) ;
    RETURN_ERROR(err, p, h, r) ;
  }

  inline EOS_Error EOS::compute_d_mu_d_p_h_ph(double p, double h, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_mu_d_p_h_ph(p, h, r) ;
    RETURN_ERROR(err, p, h, r) ;
  }

  inline EOS_Error EOS::compute_d_mu_d_h_p_ph(double p, double h, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_mu_d_h_p_ph(p, h, r) ;
    RETURN_ERROR(err, p, h, r) ;
  }

  inline EOS_Error EOS::compute_d_lambda_d_p_h_ph(double p, double h, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_lambda_d_p_h_ph(p, h, r) ;
    RETURN_ERROR(err, p, h, r) ;
  }

  inline EOS_Error EOS::compute_d_lambda_d_h_p_ph(double p, double h, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_lambda_d_h_p_ph(p, h, r) ;
    RETURN_ERROR(err, p, h, r) ;
  }

  inline EOS_Error EOS::compute_d_cp_d_p_h_ph(double p, double h, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cp_d_p_h_ph(p, h, r) ;
    RETURN_ERROR(err, p, h, r) ;
  }

  inline EOS_Error EOS::compute_d_cp_d_h_p_ph(double p, double h, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cp_d_h_p_ph(p, h, r) ;
    RETURN_ERROR(err, p, h, r) ;
  }

  inline EOS_Error EOS::compute_d_cv_d_p_h_ph(double p, double h, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cv_d_p_h_ph(p, h, r) ;
    RETURN_ERROR(err, p, h, r) ;
  }

  inline EOS_Error EOS::compute_d_cv_d_h_p_ph(double p, double h, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cv_d_h_p_ph(p, h, r) ;
    RETURN_ERROR(err, p, h, r) ;
  }

  inline EOS_Error EOS::compute_d_sigma_d_p_h_ph(double p, double h, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_sigma_d_p_h_ph(p, h, r) ;
    RETURN_ERROR(err, p, h, r) ;
  }

  inline EOS_Error EOS::compute_d_sigma_d_h_p_ph(double p, double h, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_sigma_d_h_p_ph(p, h, r) ;
    RETURN_ERROR(err, p, h, r) ;
  }

  inline EOS_Error EOS::compute_d_w_d_p_h_ph(double p, double h, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_w_d_p_h_ph(p, h, r) ;
    RETURN_ERROR(err, p, h, r) ;
  }

  inline EOS_Error EOS::compute_d_w_d_h_p_ph(double p, double h, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_w_d_h_p_ph(p, h, r) ;
    RETURN_ERROR(err, p, h, r) ;
  }

  inline EOS_Error EOS::compute_d_g_d_p_h_ph(double p, double h, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_g_d_p_h_ph(p, h, r) ;
    RETURN_ERROR(err, p, h, r) ;
  }

  inline EOS_Error EOS::compute_d_g_d_h_p_ph(double p, double h, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_g_d_h_p_ph(p, h, r) ;
    RETURN_ERROR(err, p, h, r) ;
  }

  inline EOS_Error EOS::compute_d_f_d_p_h_ph(double p, double h, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_f_d_p_h_ph(p, h, r) ;
    RETURN_ERROR(err, p, h, r) ;
  }

  inline EOS_Error EOS::compute_d_f_d_h_p_ph(double p, double h, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_f_d_h_p_ph(p, h, r) ;
    RETURN_ERROR(err, p, h, r) ;
  }

  inline EOS_Error EOS::compute_d_pr_d_p_h_ph(double p, double h, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_pr_d_p_h_ph(p, h, r) ;
    RETURN_ERROR(err, p, h, r) ;
  }

  inline EOS_Error EOS::compute_d_pr_d_h_p_ph(double p, double h, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_pr_d_h_p_ph(p, h, r) ;
    RETURN_ERROR(err, p, h, r) ;
  }

  inline EOS_Error EOS::compute_d_beta_d_p_h_ph(double p, double h, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_beta_d_p_h_ph(p, h, r) ;
    RETURN_ERROR(err, p, h, r) ;
  }

  inline EOS_Error EOS::compute_d_beta_d_h_p_ph(double p, double h, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_beta_d_h_p_ph(p, h, r) ;
    RETURN_ERROR(err, p, h, r) ;
  }

  inline EOS_Error EOS::compute_d_gamma_d_p_h_ph(double p, double h, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_gamma_d_p_h_ph(p, h, r) ;
    RETURN_ERROR(err, p, h, r) ;
  }

  inline EOS_Error EOS::compute_d_gamma_d_h_p_ph(double p, double h, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_gamma_d_h_p_ph(p, h, r) ;
    RETURN_ERROR(err, p, h, r) ;
  }

  inline EOS_Error EOS::compute_d_h_d_p_T_pT(double p, double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_h_d_p_T_pT(p, T, r) ;
    RETURN_ERROR(err, p, T, r) ;
  }

  inline EOS_Error EOS::compute_d_h_d_T_p_pT(double p, double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_h_d_T_p_pT(p, T, r) ;
    RETURN_ERROR(err, p, T, r) ;
  }

  inline EOS_Error EOS::compute_d_rho_d_p_T_pT(double p, double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_rho_d_p_T_pT(p, T, r) ;
    RETURN_ERROR(err, p, T, r) ;
  }

  inline EOS_Error EOS::compute_d_rho_d_T_p_pT(double p, double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_rho_d_T_p_pT(p, T, r) ;
    RETURN_ERROR(err, p, T, r) ;
  }

  inline EOS_Error EOS::compute_d_u_d_p_T_pT(double p, double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_u_d_p_T_pT(p, T, r) ;
    RETURN_ERROR(err, p, T, r) ;
  }

  inline EOS_Error EOS::compute_d_u_d_T_p_pT(double p, double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_u_d_T_p_pT(p, T, r) ;
    RETURN_ERROR(err, p, T, r) ;
  }

  inline EOS_Error EOS::compute_d_s_d_p_T_pT(double p, double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_s_d_p_T_pT(p, T, r) ;
    RETURN_ERROR(err, p, T, r) ;
  }

  inline EOS_Error EOS::compute_d_s_d_T_p_pT(double p, double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_s_d_T_p_pT(p, T, r) ;
    RETURN_ERROR(err, p, T, r) ;
  }

  inline EOS_Error EOS::compute_d_mu_d_p_T_pT(double p, double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_mu_d_p_T_pT(p, T, r) ;
    RETURN_ERROR(err, p, T, r) ;
  }

  inline EOS_Error EOS::compute_d_mu_d_T_p_pT(double p, double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_mu_d_T_p_pT(p, T, r) ;
    RETURN_ERROR(err, p, T, r) ;
  }

  inline EOS_Error EOS::compute_d_lambda_d_p_T_pT(double p, double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_lambda_d_p_T_pT(p, T, r) ;
    RETURN_ERROR(err, p, T, r) ;
  }

  inline EOS_Error EOS::compute_d_lambda_d_T_p_pT(double p, double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_lambda_d_T_p_pT(p, T, r) ;
    RETURN_ERROR(err, p, T, r) ;
  }

  inline EOS_Error EOS::compute_d_cp_d_p_T_pT(double p, double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cp_d_p_T_pT(p, T, r) ;
    RETURN_ERROR(err, p, T, r) ;
  }

  inline EOS_Error EOS::compute_d_cp_d_T_p_pT(double p, double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cp_d_T_p_pT(p, T, r) ;
    RETURN_ERROR(err, p, T, r) ;
  }

  inline EOS_Error EOS::compute_d_cv_d_p_T_pT(double p, double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cv_d_p_T_pT(p, T, r) ;
    RETURN_ERROR(err, p, T, r) ;
  }

  inline EOS_Error EOS::compute_d_cv_d_T_p_pT(double p, double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cv_d_T_p_pT(p, T, r) ;
    RETURN_ERROR(err, p, T, r) ;
  }

  inline EOS_Error EOS::compute_d_sigma_d_p_T_pT(double p, double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_sigma_d_p_T_pT(p, T, r) ;
    RETURN_ERROR(err, p, T, r) ;
  }

  inline EOS_Error EOS::compute_d_sigma_d_T_p_pT(double p, double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_sigma_d_T_p_pT(p, T, r) ;
    RETURN_ERROR(err, p, T, r) ;
  }

  inline EOS_Error EOS::compute_d_w_d_p_T_pT(double p, double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_w_d_p_T_pT(p, T, r) ;
    RETURN_ERROR(err, p, T, r) ;
  }

  inline EOS_Error EOS::compute_d_w_d_T_p_pT(double p, double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_w_d_T_p_pT(p, T, r) ;
    RETURN_ERROR(err, p, T, r) ;
  }

  inline EOS_Error EOS::compute_d_g_d_p_T_pT(double p, double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_g_d_p_T_pT(p, T, r) ;
    RETURN_ERROR(err, p, T, r) ;
  }

  inline EOS_Error EOS::compute_d_g_d_T_p_pT(double p, double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_g_d_T_p_pT(p, T, r) ;
    RETURN_ERROR(err, p, T, r) ;
  }

  inline EOS_Error EOS::compute_d_f_d_p_T_pT(double p, double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_f_d_p_T_pT(p, T, r) ;
    RETURN_ERROR(err, p, T, r) ;
  }

  inline EOS_Error EOS::compute_d_f_d_T_p_pT(double p, double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_f_d_T_p_pT(p, T, r) ;
    RETURN_ERROR(err, p, T, r) ;
  }

  inline EOS_Error EOS::compute_d_pr_d_p_T_pT(double p, double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_pr_d_p_T_pT(p, T, r) ;
    RETURN_ERROR(err, p, T, r) ;
  }

  inline EOS_Error EOS::compute_d_pr_d_T_p_pT(double p, double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_pr_d_T_p_pT(p, T, r) ;
    RETURN_ERROR(err, p, T, r) ;
  }

  inline EOS_Error EOS::compute_d_beta_d_p_T_pT(double p, double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_beta_d_p_T_pT(p, T, r) ;
    RETURN_ERROR(err, p, T, r) ;
  }

  inline EOS_Error EOS::compute_d_beta_d_T_p_pT(double p, double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_beta_d_T_p_pT(p, T, r) ;
    RETURN_ERROR(err, p, T, r) ;
  }

  inline EOS_Error EOS::compute_d_gamma_d_p_T_pT(double p, double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_gamma_d_p_T_pT(p, T, r) ;
    RETURN_ERROR(err, p, T, r) ;
  }

  inline EOS_Error EOS::compute_d_gamma_d_T_p_pT(double p, double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_gamma_d_T_p_pT(p, T, r) ;
    RETURN_ERROR(err, p, T, r) ;
  }

  inline EOS_Error EOS::compute_d_h_d_p_s_ps(double p, double s, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_h_d_p_s_ps(p, s, r) ;
    RETURN_ERROR(err, p, s, r) ;
  }

  inline EOS_Error EOS::compute_d_h_d_s_p_ps(double p, double s, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_h_d_s_p_ps(p, s, r) ;
    RETURN_ERROR(err, p, s, r) ;
  }

  inline EOS_Error EOS::compute_d_T_d_p_s_ps(double p, double s, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_T_d_p_s_ps(p, s, r) ;
    RETURN_ERROR(err, p, s, r) ;
  }

  inline EOS_Error EOS::compute_d_T_d_s_p_ps(double p, double s, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_T_d_s_p_ps(p, s, r) ;
    RETURN_ERROR(err, p, s, r) ;
  }

  inline EOS_Error EOS::compute_d_rho_d_p_s_ps(double p, double s, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_rho_d_p_s_ps(p, s, r) ;
    RETURN_ERROR(err, p, s, r) ;
  }

  inline EOS_Error EOS::compute_d_rho_d_s_p_ps(double p, double s, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_rho_d_s_p_ps(p, s, r) ;
    RETURN_ERROR(err, p, s, r) ;
  }

  inline EOS_Error EOS::compute_d_u_d_p_s_ps(double p, double s, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_u_d_p_s_ps(p, s, r) ;
    RETURN_ERROR(err, p, s, r) ;
  }

  inline EOS_Error EOS::compute_d_u_d_s_p_ps(double p, double s, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_u_d_s_p_ps(p, s, r) ;
    RETURN_ERROR(err, p, s, r) ;
  }

  inline EOS_Error EOS::compute_d_mu_d_p_s_ps(double p, double s, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_mu_d_p_s_ps(p, s, r) ;
    RETURN_ERROR(err, p, s, r) ;
  }

  inline EOS_Error EOS::compute_d_mu_d_s_p_ps(double p, double s, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_mu_d_s_p_ps(p, s, r) ;
    RETURN_ERROR(err, p, s, r) ;
  }

  inline EOS_Error EOS::compute_d_lambda_d_p_s_ps(double p, double s, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_lambda_d_p_s_ps(p, s, r) ;
    RETURN_ERROR(err, p, s, r) ;
  }

  inline EOS_Error EOS::compute_d_lambda_d_s_p_ps(double p, double s, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_lambda_d_s_p_ps(p, s, r) ;
    RETURN_ERROR(err, p, s, r) ;
  }

  inline EOS_Error EOS::compute_d_cp_d_p_s_ps(double p, double s, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cp_d_p_s_ps(p, s, r) ;
    RETURN_ERROR(err, p, s, r) ;
  }

  inline EOS_Error EOS::compute_d_cp_d_s_p_ps(double p, double s, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cp_d_s_p_ps(p, s, r) ;
    RETURN_ERROR(err, p, s, r) ;
  }

  inline EOS_Error EOS::compute_d_cv_d_p_s_ps(double p, double s, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cv_d_p_s_ps(p, s, r) ;
    RETURN_ERROR(err, p, s, r) ;
  }

  inline EOS_Error EOS::compute_d_cv_d_s_p_ps(double p, double s, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cv_d_s_p_ps(p, s, r) ;
    RETURN_ERROR(err, p, s, r) ;
  }

  inline EOS_Error EOS::compute_d_sigma_d_p_s_ps(double p, double s, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_sigma_d_p_s_ps(p, s, r) ;
    RETURN_ERROR(err, p, s, r) ;
  }

  inline EOS_Error EOS::compute_d_sigma_d_s_p_ps(double p, double s, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_sigma_d_s_p_ps(p, s, r) ;
    RETURN_ERROR(err, p, s, r) ;
  }

  inline EOS_Error EOS::compute_d_w_d_p_s_ps(double p, double s, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_w_d_p_s_ps(p, s, r) ;
    RETURN_ERROR(err, p, s, r) ;
  }

  inline EOS_Error EOS::compute_d_w_d_s_p_ps(double p, double s, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_w_d_s_p_ps(p, s, r) ;
    RETURN_ERROR(err, p, s, r) ;
  }

  inline EOS_Error EOS::compute_d_g_d_p_s_ps(double p, double s, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_g_d_p_s_ps(p, s, r) ;
    RETURN_ERROR(err, p, s, r) ;
  }

  inline EOS_Error EOS::compute_d_g_d_s_p_ps(double p, double s, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_g_d_s_p_ps(p, s, r) ;
    RETURN_ERROR(err, p, s, r) ;
  }

  inline EOS_Error EOS::compute_d_f_d_p_s_ps(double p, double s, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_f_d_p_s_ps(p, s, r) ;
    RETURN_ERROR(err, p, s, r) ;
  }

  inline EOS_Error EOS::compute_d_f_d_s_p_ps(double p, double s, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_f_d_s_p_ps(p, s, r) ;
    RETURN_ERROR(err, p, s, r) ;
  }

  inline EOS_Error EOS::compute_d_pr_d_p_s_ps(double p, double s, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_pr_d_p_s_ps(p, s, r) ;
    RETURN_ERROR(err, p, s, r) ;
  }

  inline EOS_Error EOS::compute_d_pr_d_s_p_ps(double p, double s, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_pr_d_s_p_ps(p, s, r) ;
    RETURN_ERROR(err, p, s, r) ;
  }

  inline EOS_Error EOS::compute_d_beta_d_p_s_ps(double p, double s, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_beta_d_p_s_ps(p, s, r) ;
    RETURN_ERROR(err, p, s, r) ;
  }

  inline EOS_Error EOS::compute_d_beta_d_s_p_ps(double p, double s, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_beta_d_s_p_ps(p, s, r) ;
    RETURN_ERROR(err, p, s, r) ;
  }

  inline EOS_Error EOS::compute_d_gamma_d_p_s_ps(double p, double s, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_gamma_d_p_s_ps(p, s, r) ;
    RETURN_ERROR(err, p, s, r) ;
  }

  inline EOS_Error EOS::compute_d_gamma_d_s_p_ps(double p, double s, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_gamma_d_s_p_ps(p, s, r) ;
    RETURN_ERROR(err, p, s, r) ;
  }

  inline EOS_Error EOS::compute_T_sat_p(double p, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_T_sat_p(p, r) ;
    RETURN_ERROR(err, p, 0, r) ;
  }

  inline EOS_Error EOS::compute_rho_l_sat_p(double p, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_rho_l_sat_p(p, r) ;
    RETURN_ERROR(err, p, 0, r) ;
  }

  inline EOS_Error EOS::compute_rho_v_sat_p(double p, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_rho_v_sat_p(p, r) ;
    RETURN_ERROR(err, p, 0, r) ;
  }

  inline EOS_Error EOS::compute_h_l_sat_p(double p, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_h_l_sat_p(p, r) ;
    RETURN_ERROR(err, p, 0, r) ;
  }

  inline EOS_Error EOS::compute_h_v_sat_p(double p, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_h_v_sat_p(p, r) ;
    RETURN_ERROR(err, p, 0, r) ;
  }

  inline EOS_Error EOS::compute_cp_l_sat_p(double p, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_cp_l_sat_p(p, r) ;
    RETURN_ERROR(err, p, 0, r) ;
  }

  inline EOS_Error EOS::compute_cp_v_sat_p(double p, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_cp_v_sat_p(p, r) ;
    RETURN_ERROR(err, p, 0, r) ;
  }

  inline EOS_Error EOS::compute_p_sat_T(double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_p_sat_T(T, r) ;
    RETURN_ERROR(err, T, 0, r) ;
  }

  inline EOS_Error EOS::compute_rho_l_sat_T(double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_rho_l_sat_T(T, r) ;
    RETURN_ERROR(err, T, 0, r) ;
  }

  inline EOS_Error EOS::compute_rho_v_sat_T(double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_rho_v_sat_T(T, r) ;
    RETURN_ERROR(err, T, 0, r) ;
  }

  inline EOS_Error EOS::compute_h_l_sat_T(double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_h_l_sat_T(T, r) ;
    RETURN_ERROR(err, T, 0, r) ;
  }

  inline EOS_Error EOS::compute_h_v_sat_T(double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_h_v_sat_T(T, r) ;
    RETURN_ERROR(err, T, 0, r) ;
  }

  inline EOS_Error EOS::compute_cp_l_sat_T(double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_cp_l_sat_T(T, r) ;
    RETURN_ERROR(err, T, 0, r) ;
  }

  inline EOS_Error EOS::compute_cp_v_sat_T(double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_cp_v_sat_T(T, r) ;
    RETURN_ERROR(err, T, 0, r) ;
  }

  inline EOS_Error EOS::compute_d_T_sat_d_p_p(double p, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_T_sat_d_p_p(p, r) ;
    RETURN_ERROR(err, p, 0, r) ;
  }

  inline EOS_Error EOS::compute_d_rho_l_sat_d_p_p(double p, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_rho_l_sat_d_p_p(p, r) ;
    RETURN_ERROR(err, p, 0, r) ;
  }

  inline EOS_Error EOS::compute_d_rho_v_sat_d_p_p(double p, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_rho_v_sat_d_p_p(p, r) ;
    RETURN_ERROR(err, p, 0, r) ;
  }

  inline EOS_Error EOS::compute_d_h_l_sat_d_p_p(double p, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_h_l_sat_d_p_p(p, r) ;
    RETURN_ERROR(err, p, 0, r) ;
  }

  inline EOS_Error EOS::compute_d_h_v_sat_d_p_p(double p, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_h_v_sat_d_p_p(p, r) ;
    RETURN_ERROR(err, p, 0, r) ;
  }

  inline EOS_Error EOS::compute_d_cp_l_sat_d_p_p(double p, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cp_l_sat_d_p_p(p, r) ;
    RETURN_ERROR(err, p, 0, r) ;
  }

  inline EOS_Error EOS::compute_d_cp_v_sat_d_p_p(double p, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cp_v_sat_d_p_p(p, r) ;
    RETURN_ERROR(err, p, 0, r) ;
  }

  inline EOS_Error EOS::compute_d_p_sat_d_T_T(double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_p_sat_d_T_T(T, r) ;
    RETURN_ERROR(err, T, 0, r) ;
  }

  inline EOS_Error EOS::compute_d_rho_l_sat_d_T_T(double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_rho_l_sat_d_T_T(T, r) ;
    RETURN_ERROR(err, T, 0, r) ;
  }

  inline EOS_Error EOS::compute_d_rho_v_sat_d_T_T(double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_rho_v_sat_d_T_T(T, r) ;
    RETURN_ERROR(err, T, 0, r) ;
  }

  inline EOS_Error EOS::compute_d_h_l_sat_d_T_T(double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_h_l_sat_d_T_T(T, r) ;
    RETURN_ERROR(err, T, 0, r) ;
  }

  inline EOS_Error EOS::compute_d_h_v_sat_d_T_T(double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_h_v_sat_d_T_T(T, r) ;
    RETURN_ERROR(err, T, 0, r) ;
  }

  inline EOS_Error EOS::compute_d_cp_l_sat_d_T_T(double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cp_l_sat_d_T_T(T, r) ;
    RETURN_ERROR(err, T, 0, r) ;
  }

  inline EOS_Error EOS::compute_d_cp_v_sat_d_T_T(double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cp_v_sat_d_T_T(T, r) ;
    RETURN_ERROR(err, T, 0, r) ;
  }

  inline EOS_Error EOS::compute_d2_T_sat_d_p_d_p_p(double p, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d2_T_sat_d_p_d_p_p(p, r) ;
    RETURN_ERROR(err, p, 0, r) ;
  }

  inline EOS_Error EOS::compute_d2_rho_l_sat_d_p_d_p_p(double p, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d2_rho_l_sat_d_p_d_p_p(p, r) ;
    RETURN_ERROR(err, p, 0, r) ;
  }

  inline EOS_Error EOS::compute_d2_rho_v_sat_d_p_d_p_p(double p, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d2_rho_v_sat_d_p_d_p_p(p, r) ;
    RETURN_ERROR(err, p, 0, r) ;
  }

  inline EOS_Error EOS::compute_d2_h_l_sat_d_p_d_p_p(double p, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d2_h_l_sat_d_p_d_p_p(p, r) ;
    RETURN_ERROR(err, p, 0, r) ;
  }

  inline EOS_Error EOS::compute_d2_h_v_sat_d_p_d_p_p(double p, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d2_h_v_sat_d_p_d_p_p(p, r) ;
    RETURN_ERROR(err, p, 0, r) ;
  }

  inline EOS_Error EOS::compute_d2_cp_l_sat_d_p_d_p_p(double p, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d2_cp_l_sat_d_p_d_p_p(p, r) ;
    RETURN_ERROR(err, p, 0, r) ;
  }

  inline EOS_Error EOS::compute_d2_cp_v_sat_d_p_d_p_p(double p, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d2_cp_v_sat_d_p_d_p_p(p, r) ;
    RETURN_ERROR(err, p, 0, r) ;
  }

  inline EOS_Error EOS::compute_d2_p_sat_d_T_d_T_T(double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d2_p_sat_d_T_d_T_T(T, r) ;
    RETURN_ERROR(err, T, 0, r) ;
  }

  inline EOS_Error EOS::compute_d2_rho_l_sat_d_T_d_T_T(double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d2_rho_l_sat_d_T_d_T_T(T, r) ;
    RETURN_ERROR(err, T, 0, r) ;
  }

  inline EOS_Error EOS::compute_d2_rho_v_sat_d_T_d_T_T(double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d2_rho_v_sat_d_T_d_T_T(T, r) ;
    RETURN_ERROR(err, T, 0, r) ;
  }

  inline EOS_Error EOS::compute_d2_h_l_sat_d_T_d_T_T(double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d2_h_l_sat_d_T_d_T_T(T, r) ;
    RETURN_ERROR(err, T, 0, r) ;
  }

  inline EOS_Error EOS::compute_d2_h_v_sat_d_T_d_T_T(double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d2_h_v_sat_d_T_d_T_T(T, r) ;
    RETURN_ERROR(err, T, 0, r) ;
  }

  inline EOS_Error EOS::compute_d2_cp_l_sat_d_T_d_T_T(double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d2_cp_l_sat_d_T_d_T_T(T, r) ;
    RETURN_ERROR(err, T, 0, r) ;
  }

  inline EOS_Error EOS::compute_d2_cp_v_sat_d_T_d_T_T(double T, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d2_cp_v_sat_d_T_d_T_T(T, r) ;
    RETURN_ERROR(err, T, 0, r) ;
  }

  inline EOS_Error EOS::compute_h_l_lim_p(double p, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_h_l_lim_p(p, r) ;
    RETURN_ERROR(err, p, 0, r) ;
  }

  inline EOS_Error EOS::compute_h_v_lim_p(double p, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_h_v_lim_p(p, r) ;
    RETURN_ERROR(err, p, 0, r) ;
  }

  inline EOS_Error EOS::compute_d_h_l_lim_d_p_p(double p, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_h_l_lim_d_p_p(p, r) ;
    RETURN_ERROR(err, p, 0, r) ;
  }

  inline EOS_Error EOS::compute_d_h_v_lim_d_p_p(double p, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_h_v_lim_d_p_p(p, r) ;
    RETURN_ERROR(err, p, 0, r) ;
  }

  inline EOS_Error EOS::compute_d2_h_l_lim_d_p_d_p_p(double p, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d2_h_l_lim_d_p_d_p_p(p, r) ;
    RETURN_ERROR(err, p, 0, r) ;
  }

  inline EOS_Error EOS::compute_d2_h_v_lim_d_p_d_p_p(double p, double& r) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d2_h_v_lim_d_p_d_p_p(p, r) ;
    RETURN_ERROR(err, p, 0, r) ;
  }

  inline EOS_Error EOS::compute_T_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_T_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_rho_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_rho_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_u_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_u_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_s_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_s_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_mu_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_mu_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_lambda_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_lambda_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_cp_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_cp_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_cv_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_cv_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_sigma_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_sigma_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_w_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_w_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_g_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_g_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_f_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_f_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_pr_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_pr_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_beta_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_beta_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_gamma_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_gamma_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_h_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_h_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_rho_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_rho_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_u_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_u_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_s_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_s_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_mu_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_mu_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_lambda_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_lambda_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_cp_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_cp_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_cv_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_cv_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_sigma_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_sigma_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_w_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_w_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_g_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_g_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_f_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_f_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_pr_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_pr_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_beta_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_beta_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_gamma_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_gamma_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_h_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_h_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_T_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_T_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_rho_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_rho_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_u_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_u_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_mu_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_mu_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_lambda_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_lambda_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_cp_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_cp_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_cv_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_cv_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_sigma_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_sigma_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_w_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_w_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_g_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_g_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_f_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_f_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_pr_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_pr_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_beta_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_beta_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_gamma_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_gamma_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_T_d_p_h_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_T_d_p_h_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_T_d_h_p_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_T_d_h_p_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_T_d_c_0_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_T_d_c_0_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_T_d_c_1_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_T_d_c_1_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_T_d_c_2_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_T_d_c_2_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_T_d_c_3_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_T_d_c_3_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_T_d_c_4_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_T_d_c_4_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_rho_d_p_h_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_rho_d_p_h_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_rho_d_h_p_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_rho_d_h_p_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_rho_d_c_0_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_rho_d_c_0_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_rho_d_c_1_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_rho_d_c_1_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_rho_d_c_2_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_rho_d_c_2_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_rho_d_c_3_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_rho_d_c_3_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_rho_d_c_4_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_rho_d_c_4_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_u_d_p_h_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_u_d_p_h_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_u_d_h_p_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_u_d_h_p_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_u_d_c_0_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_u_d_c_0_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_u_d_c_1_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_u_d_c_1_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_u_d_c_2_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_u_d_c_2_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_u_d_c_3_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_u_d_c_3_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_u_d_c_4_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_u_d_c_4_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_s_d_p_h_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_s_d_p_h_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_s_d_h_p_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_s_d_h_p_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_s_d_c_0_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_s_d_c_0_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_s_d_c_1_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_s_d_c_1_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_s_d_c_2_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_s_d_c_2_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_s_d_c_3_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_s_d_c_3_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_s_d_c_4_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_s_d_c_4_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_mu_d_p_h_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_mu_d_p_h_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_mu_d_h_p_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_mu_d_h_p_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_mu_d_c_0_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_mu_d_c_0_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_mu_d_c_1_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_mu_d_c_1_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_mu_d_c_2_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_mu_d_c_2_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_mu_d_c_3_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_mu_d_c_3_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_mu_d_c_4_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_mu_d_c_4_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_lambda_d_p_h_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_lambda_d_p_h_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_lambda_d_h_p_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_lambda_d_h_p_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_lambda_d_c_0_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_lambda_d_c_0_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_lambda_d_c_1_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_lambda_d_c_1_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_lambda_d_c_2_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_lambda_d_c_2_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_lambda_d_c_3_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_lambda_d_c_3_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_lambda_d_c_4_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_lambda_d_c_4_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_cp_d_p_h_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cp_d_p_h_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_cp_d_h_p_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cp_d_h_p_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_cp_d_c_0_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cp_d_c_0_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_cp_d_c_1_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cp_d_c_1_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_cp_d_c_2_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cp_d_c_2_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_cp_d_c_3_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cp_d_c_3_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_cp_d_c_4_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cp_d_c_4_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_cv_d_p_h_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cv_d_p_h_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_cv_d_h_p_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cv_d_h_p_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_cv_d_c_0_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cv_d_c_0_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_cv_d_c_1_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cv_d_c_1_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_cv_d_c_2_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cv_d_c_2_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_cv_d_c_3_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cv_d_c_3_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_cv_d_c_4_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cv_d_c_4_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_sigma_d_p_h_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_sigma_d_p_h_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_sigma_d_h_p_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_sigma_d_h_p_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_sigma_d_c_0_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_sigma_d_c_0_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_sigma_d_c_1_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_sigma_d_c_1_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_sigma_d_c_2_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_sigma_d_c_2_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_sigma_d_c_3_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_sigma_d_c_3_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_sigma_d_c_4_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_sigma_d_c_4_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_w_d_p_h_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_w_d_p_h_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_w_d_h_p_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_w_d_h_p_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_w_d_c_0_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_w_d_c_0_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_w_d_c_1_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_w_d_c_1_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_w_d_c_2_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_w_d_c_2_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_w_d_c_3_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_w_d_c_3_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_w_d_c_4_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_w_d_c_4_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_g_d_p_h_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_g_d_p_h_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_g_d_h_p_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_g_d_h_p_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_g_d_c_0_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_g_d_c_0_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_g_d_c_1_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_g_d_c_1_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_g_d_c_2_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_g_d_c_2_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_g_d_c_3_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_g_d_c_3_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_g_d_c_4_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_g_d_c_4_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_f_d_p_h_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_f_d_p_h_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_f_d_h_p_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_f_d_h_p_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_f_d_c_0_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_f_d_c_0_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_f_d_c_1_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_f_d_c_1_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_f_d_c_2_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_f_d_c_2_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_f_d_c_3_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_f_d_c_3_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_f_d_c_4_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_f_d_c_4_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_pr_d_p_h_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_pr_d_p_h_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_pr_d_h_p_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_pr_d_h_p_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_pr_d_c_0_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_pr_d_c_0_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_pr_d_c_1_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_pr_d_c_1_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_pr_d_c_2_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_pr_d_c_2_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_pr_d_c_3_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_pr_d_c_3_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_pr_d_c_4_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_pr_d_c_4_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_beta_d_p_h_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_beta_d_p_h_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_beta_d_h_p_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_beta_d_h_p_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_beta_d_c_0_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_beta_d_c_0_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_beta_d_c_1_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_beta_d_c_1_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_beta_d_c_2_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_beta_d_c_2_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_beta_d_c_3_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_beta_d_c_3_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_beta_d_c_4_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_beta_d_c_4_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_gamma_d_p_h_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_gamma_d_p_h_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_gamma_d_h_p_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_gamma_d_h_p_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_gamma_d_c_0_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_gamma_d_c_0_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_gamma_d_c_1_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_gamma_d_c_1_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_gamma_d_c_2_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_gamma_d_c_2_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_gamma_d_c_3_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_gamma_d_c_3_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_gamma_d_c_4_ph_ph(double p, double h, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_gamma_d_c_4_ph_ph(p, h, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, h, r);
  }

  inline EOS_Error EOS::compute_d_h_d_p_T_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_h_d_p_T_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_h_d_T_p_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_h_d_T_p_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_h_d_c_0_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_h_d_c_0_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_h_d_c_1_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_h_d_c_1_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_h_d_c_2_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_h_d_c_2_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_h_d_c_3_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_h_d_c_3_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_h_d_c_4_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_h_d_c_4_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_rho_d_p_T_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_rho_d_p_T_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_rho_d_T_p_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_rho_d_T_p_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_rho_d_c_0_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_rho_d_c_0_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_rho_d_c_1_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_rho_d_c_1_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_rho_d_c_2_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_rho_d_c_2_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_rho_d_c_3_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_rho_d_c_3_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_rho_d_c_4_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_rho_d_c_4_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_u_d_p_T_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_u_d_p_T_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_u_d_T_p_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_u_d_T_p_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_u_d_c_0_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_u_d_c_0_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_u_d_c_1_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_u_d_c_1_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_u_d_c_2_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_u_d_c_2_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_u_d_c_3_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_u_d_c_3_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_u_d_c_4_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_u_d_c_4_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_s_d_p_T_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_s_d_p_T_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_s_d_T_p_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_s_d_T_p_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_s_d_c_0_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_s_d_c_0_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_s_d_c_1_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_s_d_c_1_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_s_d_c_2_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_s_d_c_2_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_s_d_c_3_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_s_d_c_3_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_s_d_c_4_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_s_d_c_4_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_mu_d_p_T_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_mu_d_p_T_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_mu_d_T_p_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_mu_d_T_p_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_mu_d_c_0_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_mu_d_c_0_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_mu_d_c_1_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_mu_d_c_1_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_mu_d_c_2_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_mu_d_c_2_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_mu_d_c_3_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_mu_d_c_3_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_mu_d_c_4_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_mu_d_c_4_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_lambda_d_p_T_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_lambda_d_p_T_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_lambda_d_T_p_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_lambda_d_T_p_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_lambda_d_c_0_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_lambda_d_c_0_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_lambda_d_c_1_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_lambda_d_c_1_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_lambda_d_c_2_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_lambda_d_c_2_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_lambda_d_c_3_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_lambda_d_c_3_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_lambda_d_c_4_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_lambda_d_c_4_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_cp_d_p_T_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cp_d_p_T_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_cp_d_T_p_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cp_d_T_p_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_cp_d_c_0_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cp_d_c_0_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_cp_d_c_1_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cp_d_c_1_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_cp_d_c_2_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cp_d_c_2_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_cp_d_c_3_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cp_d_c_3_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_cp_d_c_4_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cp_d_c_4_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_cv_d_p_T_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cv_d_p_T_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_cv_d_T_p_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cv_d_T_p_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_cv_d_c_0_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cv_d_c_0_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_cv_d_c_1_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cv_d_c_1_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_cv_d_c_2_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cv_d_c_2_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_cv_d_c_3_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cv_d_c_3_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_cv_d_c_4_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cv_d_c_4_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_sigma_d_p_T_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_sigma_d_p_T_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_sigma_d_T_p_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_sigma_d_T_p_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_sigma_d_c_0_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_sigma_d_c_0_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_sigma_d_c_1_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_sigma_d_c_1_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_sigma_d_c_2_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_sigma_d_c_2_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_sigma_d_c_3_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_sigma_d_c_3_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_sigma_d_c_4_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_sigma_d_c_4_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_w_d_p_T_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_w_d_p_T_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_w_d_T_p_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_w_d_T_p_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_w_d_c_0_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_w_d_c_0_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_w_d_c_1_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_w_d_c_1_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_w_d_c_2_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_w_d_c_2_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_w_d_c_3_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_w_d_c_3_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_w_d_c_4_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_w_d_c_4_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_g_d_p_T_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_g_d_p_T_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_g_d_T_p_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_g_d_T_p_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_g_d_c_0_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_g_d_c_0_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_g_d_c_1_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_g_d_c_1_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_g_d_c_2_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_g_d_c_2_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_g_d_c_3_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_g_d_c_3_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_g_d_c_4_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_g_d_c_4_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_f_d_p_T_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_f_d_p_T_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_f_d_T_p_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_f_d_T_p_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_f_d_c_0_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_f_d_c_0_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_f_d_c_1_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_f_d_c_1_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_f_d_c_2_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_f_d_c_2_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_f_d_c_3_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_f_d_c_3_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_f_d_c_4_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_f_d_c_4_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_pr_d_p_T_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_pr_d_p_T_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_pr_d_T_p_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_pr_d_T_p_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_pr_d_c_0_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_pr_d_c_0_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_pr_d_c_1_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_pr_d_c_1_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_pr_d_c_2_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_pr_d_c_2_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_pr_d_c_3_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_pr_d_c_3_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_pr_d_c_4_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_pr_d_c_4_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_beta_d_p_T_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_beta_d_p_T_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_beta_d_T_p_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_beta_d_T_p_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_beta_d_c_0_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_beta_d_c_0_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_beta_d_c_1_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_beta_d_c_1_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_beta_d_c_2_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_beta_d_c_2_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_beta_d_c_3_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_beta_d_c_3_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_beta_d_c_4_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_beta_d_c_4_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_gamma_d_p_T_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_gamma_d_p_T_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_gamma_d_T_p_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_gamma_d_T_p_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_gamma_d_c_0_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_gamma_d_c_0_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_gamma_d_c_1_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_gamma_d_c_1_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_gamma_d_c_2_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_gamma_d_c_2_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_gamma_d_c_3_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_gamma_d_c_3_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_gamma_d_c_4_pT_pT(double p, double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_gamma_d_c_4_pT_pT(p, T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, T, r);
  }

  inline EOS_Error EOS::compute_d_h_d_p_s_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_h_d_p_s_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_h_d_s_p_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_h_d_s_p_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_h_d_c_0_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_h_d_c_0_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_h_d_c_1_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_h_d_c_1_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_h_d_c_2_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_h_d_c_2_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_h_d_c_3_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_h_d_c_3_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_h_d_c_4_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_h_d_c_4_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_T_d_p_s_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_T_d_p_s_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_T_d_s_p_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_T_d_s_p_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_T_d_c_0_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_T_d_c_0_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_T_d_c_1_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_T_d_c_1_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_T_d_c_2_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_T_d_c_2_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_T_d_c_3_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_T_d_c_3_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_T_d_c_4_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_T_d_c_4_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_rho_d_p_s_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_rho_d_p_s_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_rho_d_s_p_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_rho_d_s_p_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_rho_d_c_0_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_rho_d_c_0_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_rho_d_c_1_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_rho_d_c_1_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_rho_d_c_2_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_rho_d_c_2_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_rho_d_c_3_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_rho_d_c_3_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_rho_d_c_4_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_rho_d_c_4_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_u_d_p_s_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_u_d_p_s_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_u_d_s_p_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_u_d_s_p_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_u_d_c_0_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_u_d_c_0_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_u_d_c_1_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_u_d_c_1_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_u_d_c_2_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_u_d_c_2_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_u_d_c_3_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_u_d_c_3_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_u_d_c_4_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_u_d_c_4_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_mu_d_p_s_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_mu_d_p_s_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_mu_d_s_p_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_mu_d_s_p_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_mu_d_c_0_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_mu_d_c_0_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_mu_d_c_1_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_mu_d_c_1_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_mu_d_c_2_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_mu_d_c_2_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_mu_d_c_3_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_mu_d_c_3_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_mu_d_c_4_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_mu_d_c_4_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_lambda_d_p_s_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_lambda_d_p_s_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_lambda_d_s_p_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_lambda_d_s_p_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_lambda_d_c_0_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_lambda_d_c_0_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_lambda_d_c_1_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_lambda_d_c_1_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_lambda_d_c_2_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_lambda_d_c_2_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_lambda_d_c_3_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_lambda_d_c_3_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_lambda_d_c_4_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_lambda_d_c_4_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_cp_d_p_s_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cp_d_p_s_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_cp_d_s_p_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cp_d_s_p_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_cp_d_c_0_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cp_d_c_0_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_cp_d_c_1_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cp_d_c_1_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_cp_d_c_2_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cp_d_c_2_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_cp_d_c_3_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cp_d_c_3_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_cp_d_c_4_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cp_d_c_4_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_cv_d_p_s_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cv_d_p_s_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_cv_d_s_p_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cv_d_s_p_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_cv_d_c_0_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cv_d_c_0_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_cv_d_c_1_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cv_d_c_1_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_cv_d_c_2_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cv_d_c_2_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_cv_d_c_3_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cv_d_c_3_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_cv_d_c_4_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cv_d_c_4_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_sigma_d_p_s_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_sigma_d_p_s_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_sigma_d_s_p_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_sigma_d_s_p_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_sigma_d_c_0_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_sigma_d_c_0_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_sigma_d_c_1_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_sigma_d_c_1_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_sigma_d_c_2_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_sigma_d_c_2_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_sigma_d_c_3_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_sigma_d_c_3_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_sigma_d_c_4_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_sigma_d_c_4_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_w_d_p_s_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_w_d_p_s_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_w_d_s_p_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_w_d_s_p_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_w_d_c_0_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_w_d_c_0_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_w_d_c_1_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_w_d_c_1_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_w_d_c_2_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_w_d_c_2_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_w_d_c_3_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_w_d_c_3_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_w_d_c_4_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_w_d_c_4_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_g_d_p_s_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_g_d_p_s_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_g_d_s_p_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_g_d_s_p_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_g_d_c_0_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_g_d_c_0_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_g_d_c_1_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_g_d_c_1_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_g_d_c_2_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_g_d_c_2_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_g_d_c_3_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_g_d_c_3_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_g_d_c_4_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_g_d_c_4_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_f_d_p_s_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_f_d_p_s_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_f_d_s_p_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_f_d_s_p_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_f_d_c_0_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_f_d_c_0_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_f_d_c_1_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_f_d_c_1_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_f_d_c_2_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_f_d_c_2_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_f_d_c_3_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_f_d_c_3_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_f_d_c_4_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_f_d_c_4_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_pr_d_p_s_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_pr_d_p_s_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_pr_d_s_p_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_pr_d_s_p_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_pr_d_c_0_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_pr_d_c_0_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_pr_d_c_1_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_pr_d_c_1_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_pr_d_c_2_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_pr_d_c_2_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_pr_d_c_3_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_pr_d_c_3_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_pr_d_c_4_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_pr_d_c_4_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_beta_d_p_s_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_beta_d_p_s_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_beta_d_s_p_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_beta_d_s_p_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_beta_d_c_0_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_beta_d_c_0_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_beta_d_c_1_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_beta_d_c_1_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_beta_d_c_2_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_beta_d_c_2_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_beta_d_c_3_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_beta_d_c_3_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_beta_d_c_4_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_beta_d_c_4_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_gamma_d_p_s_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_gamma_d_p_s_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_gamma_d_s_p_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_gamma_d_s_p_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_gamma_d_c_0_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_gamma_d_c_0_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_gamma_d_c_1_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_gamma_d_c_1_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_gamma_d_c_2_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_gamma_d_c_2_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_gamma_d_c_3_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_gamma_d_c_3_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_d_gamma_d_c_4_ps_ps(double p, double s, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_gamma_d_c_4_ps_ps(p, s, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, s, r);
  }

  inline EOS_Error EOS::compute_T_sat_p(double p, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_T_sat_p(p, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, 0, r);
  }

  inline EOS_Error EOS::compute_rho_l_sat_p(double p, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_rho_l_sat_p(p, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, 0, r);
  }

  inline EOS_Error EOS::compute_rho_v_sat_p(double p, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_rho_v_sat_p(p, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, 0, r);
  }

  inline EOS_Error EOS::compute_h_l_sat_p(double p, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_h_l_sat_p(p, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, 0, r);
  }

  inline EOS_Error EOS::compute_h_v_sat_p(double p, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_h_v_sat_p(p, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, 0, r);
  }

  inline EOS_Error EOS::compute_cp_l_sat_p(double p, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_cp_l_sat_p(p, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, 0, r);
  }

  inline EOS_Error EOS::compute_cp_v_sat_p(double p, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_cp_v_sat_p(p, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, 0, r);
  }

  inline EOS_Error EOS::compute_p_sat_T(double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_p_sat_T(T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, T, 0, r);
  }

  inline EOS_Error EOS::compute_rho_l_sat_T(double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_rho_l_sat_T(T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, T, 0, r);
  }

  inline EOS_Error EOS::compute_rho_v_sat_T(double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_rho_v_sat_T(T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, T, 0, r);
  }

  inline EOS_Error EOS::compute_h_l_sat_T(double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_h_l_sat_T(T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, T, 0, r);
  }

  inline EOS_Error EOS::compute_h_v_sat_T(double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_h_v_sat_T(T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, T, 0, r);
  }

  inline EOS_Error EOS::compute_cp_l_sat_T(double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_cp_l_sat_T(T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, T, 0, r);
  }

  inline EOS_Error EOS::compute_cp_v_sat_T(double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_cp_v_sat_T(T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, T, 0, r);
  }

  inline EOS_Error EOS::compute_d_T_sat_d_p_p(double p, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_T_sat_d_p_p(p, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, 0, r);
  }

  inline EOS_Error EOS::compute_d_rho_l_sat_d_p_p(double p, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_rho_l_sat_d_p_p(p, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, 0, r);
  }

  inline EOS_Error EOS::compute_d_rho_v_sat_d_p_p(double p, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_rho_v_sat_d_p_p(p, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, 0, r);
  }

  inline EOS_Error EOS::compute_d_h_l_sat_d_p_p(double p, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_h_l_sat_d_p_p(p, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, 0, r);
  }

  inline EOS_Error EOS::compute_d_h_v_sat_d_p_p(double p, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_h_v_sat_d_p_p(p, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, 0, r);
  }

  inline EOS_Error EOS::compute_d_cp_l_sat_d_p_p(double p, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cp_l_sat_d_p_p(p, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, 0, r);
  }

  inline EOS_Error EOS::compute_d_cp_v_sat_d_p_p(double p, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cp_v_sat_d_p_p(p, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, 0, r);
  }

  inline EOS_Error EOS::compute_d_p_sat_d_T_T(double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_p_sat_d_T_T(T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, T, 0, r);
  }

  inline EOS_Error EOS::compute_d_rho_l_sat_d_T_T(double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_rho_l_sat_d_T_T(T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, T, 0, r);
  }

  inline EOS_Error EOS::compute_d_rho_v_sat_d_T_T(double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_rho_v_sat_d_T_T(T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, T, 0, r);
  }

  inline EOS_Error EOS::compute_d_h_l_sat_d_T_T(double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_h_l_sat_d_T_T(T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, T, 0, r);
  }

  inline EOS_Error EOS::compute_d_h_v_sat_d_T_T(double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_h_v_sat_d_T_T(T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, T, 0, r);
  }

  inline EOS_Error EOS::compute_d_cp_l_sat_d_T_T(double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cp_l_sat_d_T_T(T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, T, 0, r);
  }

  inline EOS_Error EOS::compute_d_cp_v_sat_d_T_T(double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d_cp_v_sat_d_T_T(T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, T, 0, r);
  }

  inline EOS_Error EOS::compute_d2_T_sat_d_p_d_p_p(double p, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d2_T_sat_d_p_d_p_p(p, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, 0, r);
  }

  inline EOS_Error EOS::compute_d2_rho_l_sat_d_p_d_p_p(double p, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d2_rho_l_sat_d_p_d_p_p(p, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, 0, r);
  }

  inline EOS_Error EOS::compute_d2_rho_v_sat_d_p_d_p_p(double p, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d2_rho_v_sat_d_p_d_p_p(p, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, 0, r);
  }

  inline EOS_Error EOS::compute_d2_h_l_sat_d_p_d_p_p(double p, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d2_h_l_sat_d_p_d_p_p(p, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, 0, r);
  }

  inline EOS_Error EOS::compute_d2_h_v_sat_d_p_d_p_p(double p, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d2_h_v_sat_d_p_d_p_p(p, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, 0, r);
  }

  inline EOS_Error EOS::compute_d2_cp_l_sat_d_p_d_p_p(double p, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d2_cp_l_sat_d_p_d_p_p(p, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, 0, r);
  }

  inline EOS_Error EOS::compute_d2_cp_v_sat_d_p_d_p_p(double p, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d2_cp_v_sat_d_p_d_p_p(p, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, p, 0, r);
  }

  inline EOS_Error EOS::compute_d2_p_sat_d_T_d_T_T(double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d2_p_sat_d_T_d_T_T(T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, T, 0, r);
  }

  inline EOS_Error EOS::compute_d2_rho_l_sat_d_T_d_T_T(double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d2_rho_l_sat_d_T_d_T_T(T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, T, 0, r);
  }

  inline EOS_Error EOS::compute_d2_rho_v_sat_d_T_d_T_T(double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d2_rho_v_sat_d_T_d_T_T(T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, T, 0, r);
  }

  inline EOS_Error EOS::compute_d2_h_l_sat_d_T_d_T_T(double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d2_h_l_sat_d_T_d_T_T(T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, T, 0, r);
  }

  inline EOS_Error EOS::compute_d2_h_v_sat_d_T_d_T_T(double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d2_h_v_sat_d_T_d_T_T(T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, T, 0, r);
  }

  inline EOS_Error EOS::compute_d2_cp_l_sat_d_T_d_T_T(double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d2_cp_l_sat_d_T_d_T_T(T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, T, 0, r);
  }

  inline EOS_Error EOS::compute_d2_cp_v_sat_d_T_d_T_T(double T, double& r,
                                        double c_0, double c_1, double c_2,
                                        double c_3, double c_4) const
  { EOS_Internal_Error err = fluid_model_obj.compute_d2_cp_v_sat_d_T_d_T_T(T, r, c_0, c_1, c_2, c_3, c_4);
    RETURN_ERROR(err, T, 0, r);
  }

  // fluid() is used in mixing which is a nested EOS object
  inline EOS_Fluid & EOS::fluid() const
  {
    return fluid_model_obj ;
  }

  inline std::ostream& operator<<(std::ostream& flux, const EOS& eos)
  {
    eos.fluid_model_obj.fluid_description(flux);
    return flux;
  }
}
#endif
