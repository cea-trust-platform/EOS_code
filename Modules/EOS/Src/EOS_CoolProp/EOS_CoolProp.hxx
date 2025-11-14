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



#ifndef __EOS_CoolProp_HH__
#define __EOS_CoolProp_HH__

#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>
using std::istringstream ;
using std::ostringstream ;

#include "EOS/API/EOS.hxx"
#include "EOS/API/EOS_Fluid.hxx"
#include "EOS/API/EOS_Field.hxx"
#include "EOS/API/EOS_Fields.hxx"
#include "EOS/API/EOS_Error.hxx"
#include "EOS/API/EOS_Error_Field.hxx"
#include "EOS/API/EOS_Config.hxx"
#include "EOS/API/EOS_eosdatadir.hxx"
#include "Language/API/RegisterType.hxx"
#include "Language/API/Types_Info.hxx"
#include "system/arch.h"

using namespace NEPTUNE ;

namespace NEPTUNE_EOS
{

  //! EOS_CoolProp class
  class EOS_CoolProp : public EOS_Fluid
  {
    static const AString TMStr    ;
    static const AString MVStr    ;

  public:
    //! Constructor method
    EOS_CoolProp();

//    //! Copy constructor, which should not be used
//    EOS_CoolProp(const EOS_CoolProp&);

    //! Destructor method
    ~EOS_CoolProp();

    //! Getter methods
    const AString& table_name()   const ;
    const AString& version_name() const ;
    const AString& phase_name()   const ;

    //! for initialisation :
    //
    const AString& fluid_name() const;
    const AString& equation_name() const;

    //! initializers
    int init(const Strings&);
    int init(const Strings&, const Strings&);
    int init(const Strings&, const Strings&, const double, const double, const double, const double) ;

    //! Compute calculations from fields
    /*!
     * \param[in] in input field. Currently supported are : P
     * \param[in,out] out output field array. All fields must have the correct size.
     * \param[in,out] err error field. Must have the correct size.
     * \return worst error found in error field
     */
//    EOS_Error compute(const EOS_Field& in, EOS_Fields& out, EOS_Error_Field& err) const;

    //! Compute calculations from fields
    /*!
     * \param[in] in input field. Currently supported are : P, h
     * \param[in,out] out output field . Must have the correct size.
     * \param[in,out] err error field. Must have the correct size.
     * \return worst error found in error field
     */
//    EOS_Error compute(const EOS_Field& in, EOS_Field& out, EOS_Error_Field& err) const;

    //! Compute calculations from fields
    /*!
     * \param[in] in1 input field. Currently supported are : P, h, T
     * \param[in] in2 input field. Currently supported are : P, h, T
     * \param[in,out] out output field array. All fields must have the correct size.
     * \param[in,out] err error field. Must have the correct size.
     * \return worst error found in error field
     */
//    EOS_Error compute(const EOS_Field& in1, const EOS_Field& in2, EOS_Fields& out, EOS_Error_Field& err) const;

    //! Compute calculations from fields
    /*!
     * \param[in] in1 input field. Currently supported are : P, h, T
     * \param[in] in2 input field. Currently supported are : P, h, T
     * \param[in,out] out output field . Must have the correct size.
     * \param[in,out] err error field. Must have the correct size.
     * \return worst error found in error field
     */
//    EOS_Error compute(const EOS_Field& in1, const EOS_Field& in2, EOS_Field& out, EOS_Error_Field& err) const;

    //! Global methods
    EOS_Error compute_Ph(const char* const property_name, double in1, double in2, double& out) const ;
    EOS_Error compute_PT(const char* const property_name, double in1, double in2, double& out) const ;
    EOS_Error compute_Ph(const char* const property_name, const int property_number, double in1, double in2, double& out) const ;
    EOS_Error compute_PT(const char* const property_name, const int property_number, double in1, double in2, double& out) const ;
    //Saturation methods
    // T_sat(P)
    EOS_Internal_Error compute_T_sat_p(double p, double& T_sat) const ;

    EOS_Internal_Error compute_p_sat_T(double T, double& r) const;

    // rho_lsat(P), rho_vsat(P)
    EOS_Internal_Error compute_rho_l_sat_p(double p, double& rho_l_sat) const ;
    EOS_Internal_Error compute_rho_v_sat_p(double p, double& rho_v_sat) const ;

    EOS_Internal_Error compute_rho_l_sat_T(double T, double& r) const;
    EOS_Internal_Error compute_rho_v_sat_T(double T, double& r) const;

    // h_lsat(P), h_vsat(P)
    EOS_Internal_Error compute_h_l_sat_p(double p, double& h_l_sat) const ;
    EOS_Internal_Error compute_h_v_sat_p(double p, double& h_v_sat) const ;
    // cp_lsat(P), cp_vsat(P)
    EOS_Internal_Error compute_cp_l_sat_p(double p, double& cp_sat) const ;
    EOS_Internal_Error compute_cp_v_sat_p(double p, double& cp_sat) const ;

    EOS_Internal_Error compute_cp_l_sat_T(double T, double& r) const;
    EOS_Internal_Error compute_cp_v_sat_T(double T, double& r) const;

    // h_lsat(T), h_vsat(T)
    EOS_Internal_Error compute_h_l_sat_T(double T, double& h_lsat) const ;
    EOS_Internal_Error compute_h_v_sat_T(double T, double& h_vsat) const ;

    //ph functions
    //! Pr(p,h)
    EOS_Internal_Error compute_pr_ph(double p, double h, double&) const;

    //! h(p,T)
    EOS_Internal_Error compute_h_pT(double p, double T, double&) const;
    EOS_Internal_Error compute_T_ph(double p, double h, double&) const;

    //! rho(p,T)
    EOS_Internal_Error compute_rho_pT(double p, double T, double&) const;
    EOS_Internal_Error compute_rho_ph(double p, double h, double&) const;

    //! u(p,T)
    EOS_Internal_Error compute_u_pT(double p, double T, double&) const;
    EOS_Internal_Error compute_u_ph(double p, double h, double&) const;

    //! s(p,T)
    EOS_Internal_Error compute_s_pT(double p, double T, double&) const;
    EOS_Internal_Error compute_s_ph(double p, double h, double&) const;

    //! mu(p,T)
    EOS_Internal_Error compute_mu_pT(double p, double T, double&) const;
    EOS_Internal_Error compute_mu_ph(double p, double h, double&) const;

    //! lambda(p,T)
    EOS_Internal_Error compute_lambda_pT(double p, double T, double&) const;
    EOS_Internal_Error compute_lambda_ph(double p, double h, double&) const;

    //! cp(p,T)
    EOS_Internal_Error compute_cp_pT(double p, double T, double&) const;
    EOS_Internal_Error compute_cp_ph(double p, double h, double&) const;

    //! cv(p,T)
    EOS_Internal_Error compute_cv_pT(double p, double T, double&) const;
    EOS_Internal_Error compute_cv_ph(double p, double h, double&) const;

    //! sigma(p,T)
    EOS_Internal_Error compute_sigma_pT(double p, double T, double&) const;
    EOS_Internal_Error compute_sigma_ph(double p, double h, double&) const;

    //! w(p,T)
    EOS_Internal_Error compute_w_pT(double p, double T, double&) const;
    EOS_Internal_Error compute_w_ph(double p, double h, double&) const;

    //! g(p,T)
    EOS_Internal_Error compute_g_pT(double p, double T, double& g) const;
    EOS_Internal_Error compute_g_ph(double p, double h, double& g) const;

    //! f(p,T)
    EOS_Internal_Error compute_f_pT(double p, double T, double& f) const;
    EOS_Internal_Error compute_f_ph(double p, double h, double& f) const;

    EOS_Internal_Error compute_beta_pT(double p, double T, double& r) const;
    EOS_Internal_Error compute_beta_ph(double p, double h, double& r) const;

    //! Gamma(p,h) and Gamma(p,T)
    EOS_Internal_Error compute_gamma_ph(double p, double h, double& r) const;

    EOS_Internal_Error compute_gamma_pT(double p, double T, double& r) const;

    //! Compute Prandtl(p,T)
    EOS_Internal_Error compute_pr_pT(double p, double T, double& pr) const;

    //General fluid properties
    virtual EOS_Internal_Error get_T_min(double&) const;
    virtual EOS_Internal_Error get_T_max(double&) const;
    virtual EOS_Internal_Error get_p_max(double&) const;
    virtual EOS_Internal_Error get_p_min(double&) const;
    virtual EOS_Internal_Error get_h_min(double&) const;
    virtual EOS_Internal_Error get_h_max(double&) const;

    virtual EOS_Internal_Error get_T_crit(double&) const;
    virtual EOS_Internal_Error get_p_crit(double&) const;
    virtual EOS_Internal_Error get_h_crit(double&) const;

    /* Get molar mass in kg/mol */
    virtual EOS_Internal_Error get_mm(double&) const;

    /* Partial derivatives (P,H) */
    virtual EOS_Internal_Error compute_d_T_d_p_h_ph(double p, double h, double& r) const;
    virtual EOS_Internal_Error compute_d_T_d_h_p_ph(double p, double h, double& r) const;

    /*
     * Not used since too "touchy"
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
    */

    /* Partial derivatives (P,T) */
    virtual EOS_Internal_Error compute_d_h_d_p_T_pT(double p, double T, double& r) const;
    virtual EOS_Internal_Error compute_d_h_d_T_p_pT(double p, double T, double& r) const;

    /*
     * Not used since too "touchy"
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
    */

    //
    //  Other methods
    //
    //! see Language
    ostream& print_On (ostream& stream=cout) const;
    //! see Language
    istream& read_On (istream& stream=cin);
    //! see Language
    const Type_Info& get_Type_Info () const;

    //Error metod
    void describe_error(const EOS_Internal_Error error, AString & description) const;


  protected:
    //! Abstract handle for low-level Coolprop API
    long abstract_state_handle_;

    double sat_quality_;
    //! Handle phase id : phase_unkown, phase_gas or phase_liquid
    AString handle_phase_;
    //! Backend
    AString backend_ ;

    AString fluid_name_; //!< Name of fluid
    AString fluid_name_phase_ ; //!< Name of fluid with phase

  private:
    double molar_mass_; // Molar mass (kg/mol)
    static int type_Id;

  };
}
#endif
