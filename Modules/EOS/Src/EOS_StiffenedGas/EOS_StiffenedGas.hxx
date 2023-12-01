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



#ifndef EOS_StiffenedGas_H
#define EOS_StiffenedGas_H
#include "EOS/API/EOS_Fluid.hxx"
using namespace NEPTUNE ;

namespace NEPTUNE_EOS
{
  //! Stiffened Gas
  class EOS_StiffenedGas : public EOS_Fluid
  {
    static const AString fluidname   ;
    static const AString tablename   ;
    static const AString versionname ;

    public :
    //! for initialisation :
    
    virtual const AString& table_name()   const ;
    virtual const AString& version_name() const ;
    
    virtual int init(const Strings&) ;
    virtual const AString& fluid_name() const ;
    //! T_sat(p)           Temperature at saturation
    virtual EOS_Internal_Error compute_T_sat_p(double p, double& Tsat) const;
    //! d(T_sat)/dp (p)    at constant specific enthalpy
    //virtual EOS_Internal_Error compute_d_T_sat_d_p_p(double p, double& dTsatdp) const;
    //! h_v_sat (p)            specific enthalpy at saturation
    virtual EOS_Internal_Error compute_h_v_sat_p(double p, double& hsat) const;
    //! d(h_v_sat)/dp (p)      
    //virtual EOS_Internal_Error compute_d_h_v_sat_d_p_p(double p, double& dhsatdp) const;
    //! h_l_sat (p)            specific enthalpy at saturation
    virtual EOS_Internal_Error compute_h_l_sat_p(double p, double& hsat) const;
    //! d(h_l_sat)/dp (p)      
    //virtual EOS_Internal_Error compute_d_h_l_sat_d_p_p(double p, double& dhsatdp) const;
    //! rho_v_sat(p)         density at saturation
    virtual EOS_Internal_Error compute_rho_v_sat_p(double p, double& rhosat) const;
    //! d(rho_v_sat)/dp (p)  at constant pressure
    //virtual EOS_Internal_Error compute_d_rho_v_sat_d_p_p(double p, double& drhosatdp) const;
    //! rho_l_sat(p)         density at saturation
    virtual EOS_Internal_Error compute_rho_l_sat_p(double p, double& rhosat) const;
    //! d(rho_l_sat)/dp (p)  at constant specific enthalpy
    //virtual EOS_Internal_Error compute_d_rho_l_sat_d_p_p(double p, double& drhosatdp) const;
    //! p_sat(T)   
    virtual EOS_Internal_Error compute_p_sat_T(double T, double& psat) const; 
    //! h_lsat(T), h_vsat(T)
    //virtual EOS_Internal_Error compute_h_l_sat_T(double T, double& hsat) const;
    //virtual EOS_Internal_Error compute_h_v_sat_T(double T, double& hsat) const;
    //! rho(p,h)             density
    virtual EOS_Internal_Error compute_rho_ph(double p, double h, double& rho) const;
    //! d(rho)/dp (p,h)      at constant specific enthalpy
    //virtual EOS_Internal_Error compute_d_rho_d_p_h_ph(double p, double h, double& drhodp) const;
    //! d(rho)/dh (p,h)      at constant pressure
    //virtual EOS_Internal_Error compute_d_rho_d_h_p_ph(double p, double h, double& drhodh) const;
    //! T(p,h)               Temperature
    virtual EOS_Internal_Error compute_T_ph(double p, double h, double& T) const;
    //! dT/dp (p,h)          at constant specific enthalpy
    //virtual EOS_Internal_Error compute_d_T_d_p_h_ph(double p, double h, double& dTdp) const;
    //! dT/dh (p,h)          at constant pressure
    //virtual EOS_Internal_Error compute_d_T_d_h_p_ph(double p, double h, double& dTdh) const;
    //! h_sat (p)            specific enthalpy at saturation
    //virtual EOS_Internal_Error compute_h_sat_p(double p, double& hsat) const;
    //! d(h_sat)/dp (p)      
    //virtual EOS_Internal_Error compute_d_h_sat_d_p_p(double p, double& dhsatdp) const;
    //! u(p,h)               internal energy
    virtual EOS_Internal_Error compute_u_ph(double p, double h, double& u) const ;
    //! du/dh                at constant specific enthalpy
    //virtual EOS_Internal_Error compute_d_u_d_h_p_ph(double p, double h, double& dudh) const;
    //! du/dp                at constant pressure
    //virtual EOS_Internal_Error compute_d_u_d_p_h_ph(double p, double h, double& dudp) const;
    //! s(p,h)               specific entropy
    virtual EOS_Internal_Error compute_s_ph(double p, double h, double& s) const ;
    //! ds/dh                at constant specific enthalpy
    //virtual EOS_Internal_Error compute_d_s_d_h_p_ph(double p, double h, double& dsdh) const;
    //! ds/dp                at constant pressure
    //virtual EOS_Internal_Error compute_d_s_d_p_h_ph(double p, double h, double& dsdp) const;
    //! cp(p,h)
    virtual EOS_Internal_Error compute_cp_ph(double p, double h, double& Cp) const ;
    //! d(cp)/dp (p,h)       at constant specific enthalpy
    virtual EOS_Internal_Error compute_d_cp_d_p_h_ph(double p, double h, double& dCpdp) const ;
    //! d(cp)/dp (p,h)       at constant pressure
    virtual EOS_Internal_Error compute_d_cp_d_h_p_ph(double p, double h, double& dCpdh) const ;
    //! w(p,h)               w
    virtual EOS_Internal_Error compute_w_ph(double p, double h, double& w) const ;    
    virtual EOS_Internal_Error compute_h_pT(double p, double T, double& h) const ;
    virtual EOS_Internal_Error compute_rho_pT(double p, double T, double& rho) const ;
    virtual EOS_Internal_Error compute_u_pT(double p, double T, double& u) const ;
    virtual EOS_Internal_Error compute_s_pT(double p, double T, double& s) const ;
    //! cp(p,T)
    virtual EOS_Internal_Error compute_cp_pT(double p, double T, double& Cp) const ;
    //! w(p,T)               w
    virtual EOS_Internal_Error compute_w_pT(double p, double T, double& w) const ; 
    EOS_Internal_Error verify(double p, double h) const ;

    //  Other methods
    //
    //! see Language
    virtual ostream& print_On (ostream& stream=cout) const ;
    //! see Language
    virtual istream& read_On (istream& stream=cin) ;
    //! see Language
    virtual const Type_Info& get_Type_Info () const ;

    protected:
    //virtual int can_compute(const char* const property_name) const;

    private: 
    static int type_Id ;
    AString FluidStr ;
    double SGpinfl, SGcpl, SGcvl, SGgammal, SGql, SGqpl ;
    double SGpinfv, SGcpv, SGcvv, SGgammav, SGqv, SGqpv ;
    double SGpinf,  SGcp,  SGcv,  SGgamma,  SGq,  SGqp  ;
    double SGa, SGb, SGc, SGd ;

  };
}
#include "EOS_StiffenedGas_i.hxx"
#endif
