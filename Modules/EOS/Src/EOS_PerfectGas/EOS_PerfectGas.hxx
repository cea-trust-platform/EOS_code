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



#ifndef EOS_PerfectGas_HH
#define EOS_PerfectGas_HH

#include "EOS/API/EOS_Fluid.hxx"

using namespace NEPTUNE;
namespace NEPTUNE_EOS
{
  //! Perfect Gas
  class EOS_PerfectGas : public EOS_Fluid
  {
        static const AString versionname;
        static const AString tablename;

  public :  
    
    virtual const AString& table_name()   const ;
    virtual const AString& version_name() const ;

    
    //! for initialisation :
    virtual int init(const Strings&) ;
    virtual int init(const Strings&, const Strings&, const double, const double, const double, const double) ;
    virtual const AString& fluid_name() const ;
    // molar mass (kg/mol)
    virtual EOS_Internal_Error get_mm(double&) const;
    //! T(p,h)               Temperature
    virtual EOS_Internal_Error compute_T_ph(double p, double h, double& T) const;
    //! rho(p,h) 
    virtual EOS_Internal_Error compute_rho_ph(double p, double h, double& rho) const;
    //! cp(p,h) 
    virtual EOS_Internal_Error compute_cp_ph(double p, double h, double& Cp) const;
    //! mu(p,h)
    virtual EOS_Internal_Error compute_mu_ph(double p, double h, double& mu) const;
    //! lambda(p,h)
    virtual EOS_Internal_Error compute_lambda_ph(double p, double h, double& lambda) const;
    //! h(p,T)               Enthalpy
    virtual EOS_Internal_Error compute_h_pT(double p, double T, double& h) const;
    //! rho(p,T) 
    virtual EOS_Internal_Error compute_rho_pT(double p, double T, double& rho) const;
    //! cp(p,T) 
    virtual EOS_Internal_Error compute_cp_pT(double p, double T, double& Cp) const;
    //! mu(p,T)
    virtual EOS_Internal_Error compute_mu_pT(double p, double T, double& mu) const;
    //! lambda(p,T)
    virtual EOS_Internal_Error compute_lambda_pT(double p, double T, double& lambda) const;
    //
    //  Other methods
    //
    //! see Language
    virtual ostream& print_On (ostream& stream=cout) const;
    //! see Language
    virtual istream& read_On (istream& stream=cin);
    //! see Language
    virtual const Type_Info& get_Type_Info () const;
    //
    void set_reference_state(double h, double s, double t, double p);
    //!  provisional for EOS_Mixing
    double get_prxr();
    //!  provisional for EOS_Mixing
    double get_prxcp();
    //!  provisional for EOS_Mixing
    double get_prxm();
    //!  provisional for EOS_Mixing
    double get_prxl0();
    //!  provisional for EOS_Mixing
    double get_prxl1();
    //!  provisional for EOS_Mixing
    double get_prxl2();
    //!  provisional for EOS_Mixing
    double get_prxm0();
    //!  provisional for EOS_Mixing
    double get_prxm1();
    //!  provisional for EOS_Mixing
    double get_prxm2();
    //!  provisional for EOS_Mixing
    double get_prxdv();
    //  provisional for EOS_Mixing
  protected:
    //virtual int can_compute(const char* const property_name) const;
    // Cathare-like perfect gas
    double prxr  ;
    double prxcp ;
    double prxm  ;
    double prxl0 ;
    double prxl1 ;
    double prxl2 ;
    double prxm0 ;
    double prxm1 ;
    double prxm2 ;
    double prxdv ;
    // Cathare-like perfect gas
  private:
    static int type_Id ;
    AString FluidStr ;
    double r     ;
    double cp    ;
    double h0    ;
    double gamma ;
    double cv    ;
    double href  ;
    double sref  ;
    double Tref  ;
    double Pref  ;
  };
}
#include "EOS_PerfectGas_i.hxx"
#endif
