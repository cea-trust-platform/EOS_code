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
 * EOS_Refprop10_vapor.hxx
 *
 *  Created on: 25 mars 2010
 *  
 */

#ifndef EOS_REFPROP10_VAPOR_HXX_
#define EOS_REFPROP10_VAPOR_HXX_

#include "EOS/Src/EOS_Refprop10/EOS_Refprop10.hxx"

using namespace NEPTUNE;

namespace NEPTUNE_EOS
{
  class EOS_Refprop10_vapor : public EOS_Refprop10
  {
    static const AString phasename;
  public :
    virtual const AString& phase_name() const;
    virtual int init(const Strings&);
    // T_sat(P)
    virtual EOS_Internal_Error compute_T_sat_p(double p, double& T_sat) const;
    //
    virtual EOS_Internal_Error compute_d_T_sat_d_p_p(double p, double& dT_sat_dp) const;

    // P_sat(T)
    virtual EOS_Internal_Error compute_p_sat_T(double T, double& p_sat) const;
    // h_lsat(T), h_vsat(T)
    virtual EOS_Internal_Error compute_h_l_sat_T(double T, double& h_lsat) const;
    virtual EOS_Internal_Error compute_h_v_sat_T(double T, double& h_vsat) const;

    //! h(p,T)
    virtual EOS_Internal_Error compute_h_pT(double p, double T, double& h) const;
    //! h(p,s)
    //virtual EOS_Internal_Error compute_h_ps(double p, double s, double& h) const;
    //virtual EOS_Internal_Error compute_d_h_d_T_p_pT(double p, double T, double& h) const;
    //! rho(p,T)
    //virtual EOS_Internal_Error compute_rho_pT(double p, double T, double&) const;
    //! u(p,T)
    virtual EOS_Internal_Error compute_u_pT(double p, double T, double&) const;
    //! u(p,h)
    virtual EOS_Internal_Error compute_u_ph(double p, double h, double&) const;
    //! s(p,T)
    virtual EOS_Internal_Error compute_s_pT(double p, double T, double&) const;
    //! s(p,h)
    virtual EOS_Internal_Error compute_s_ph(double p, double h, double&) const;
    //! mu(p,T)
    virtual EOS_Internal_Error compute_mu_pT(double p, double T, double&) const;
    //! mu(p,h)
    virtual EOS_Internal_Error compute_mu_ph(double p, double h, double&) const;
    //! lambda(p,T)
    virtual EOS_Internal_Error compute_lambda_pT(double p, double T, double&) const;
    //! lambda(p,h)
    virtual EOS_Internal_Error compute_lambda_ph(double p, double h, double&) const;
    //! cp(p,T)
    virtual EOS_Internal_Error compute_cp_pT(double p, double T, double&) const;
    //! cp(p,h)
    virtual EOS_Internal_Error compute_cp_ph(double p, double h, double&) const;    
    //! cv(p,h)
    virtual EOS_Internal_Error compute_cv_ph(double p, double h, double&) const;   
    //! beta(p,h)
    virtual EOS_Internal_Error compute_beta_ph(double p, double h, double&) const;
    //! w(p,T)
    virtual EOS_Internal_Error compute_w_pT(double p, double T, double&) const;
    //! w(p,h)
    virtual EOS_Internal_Error compute_w_ph(double p, double h, double&) const;
    virtual EOS_Internal_Error compute_d_h_d_T_p_pT(double p, double T, double& h) const;
    virtual EOS_Internal_Error is_metastable(double rho, double p, double T, bool&) const;

  protected:
    //! Compute REFPROP phase flag (1=liquid ; 2=vapor)
    virtual EOS_Internal_Error calrp_kph(double p, double h, int& kph) const;
    //! Compute REFPROP phase flag (1=liquid ; 2=vapor) in (p,T)
    virtual EOS_Internal_Error calrp_kph_pT(double p, double T, int& kph) const;

  private:
    static int type_Id;
    EOS_Internal_Error call_tprho(double p, double T, double& rho) const;
    EOS_Internal_Error call_therm(const char* const property_name, double T, double rho, double& value) const;
  };
}
#include "EOS_Refprop10_vapor_i.hxx"
#endif /* EOS_REFPROP10_VAPOR_HXX_ */
