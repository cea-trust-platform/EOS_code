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



#ifndef EOS_Refprop10_HH
#define EOS_Refprop10_HH

#define HC255 256 // characters fortran strings
#define HC80   81
#define HC12   13
#define HC3     4
#define NBCOMPMAX 20 // maximum composant number of fluids for Refprop mixing fluids

#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>
using std::istringstream ;
using std::ostringstream ;

#include "REFPROP10_declare.hxx"
#include "EOS/API/EOS.hxx"
#include "EOS/API/EOS_Fluid.hxx"
#include "EOS/API/EOS_Field.hxx"
#include "EOS/API/EOS_Fields.hxx"
#include "EOS/API/EOS_Error_Field.hxx"
#include "EOS/API/EOS_Config.hxx"
#include "EOS/API/EOS_eosdatadir.hxx"
#include "Language/API/RegisterType.hxx"
#include "Language/API/Types_Info.hxx"
#include "system/arch.h"

#define NBMAXFIELD 100
#define NUMAXPROP  10000

using namespace NEPTUNE ;
static AString file_mix_10("fluids/HMX.BNC") ;
static const int nc_max_10  = 20 ;        // number composant max allowed in Refprop mixture
static const int err_max_10 = 10 ;        // number max of error messages
static const int errcode_max_10 = 10000 ; // maximum code number

static int  ierr_10        ;
static char herr_10[HC255] ;




namespace NEPTUNE_EOS
{

  //!  Refprop 
  class EOS_Refprop10 : public EOS_Fluid
  {
    static const AString TMStr    ;
    static const AString MVStr    ;
    static const AString PhaseStr ;

  public :  
    EOS_Refprop10();
    virtual ~EOS_Refprop10();

    virtual const AString& table_name()   const ;
    virtual const AString& version_name() const ;
    virtual const AString& phase_name()   const ;
    
    //! for initialisation :
    //
    const AString& fluid_name() const;
    const AString& equation_name() const;

    virtual int init(const Strings&);
    virtual int init(const Strings&, const Strings&);
    virtual int init(const Strings&, const Strings&, const double, const double, const double, const double) ; 

    // const int get_nbcomp() const;

    // const Strings& get_hname() const;
    // const Strings& get_hn80() const;
    // const Strings& get_hcas() const;

    //set methods
    // int set_hname(const Strings&);
    // int set_hn80(const Strings&);
    // int set_hcas(const Strings&);

    //! the main "Fields" method!
    //! computes r(p,h)
    //! p, h, r[i], errfield are all of the same size
    //! the EOS_Error returned value is a "global" error
    //! errfield is a "local" error
    virtual EOS_Error compute(const EOS_Field& p, const EOS_Field& h, EOS_Fields& r, 
                              EOS_Error_Field& errfield) const;

    //! computes an array of flags in order to minimize the number of calls 
    //! to Refprop routines according to the requested properties 
    void calrp_indic_ph(EOS_Fields& r, int* indic) const;
  
    //! computes properties in (p,h) plane by 
    //! direct calls to refprop methods for performance optimisation
    EOS_Internal_Error calrp_optim_ph(double p, double h, double* tab_prop, 
                                      double* tab_der_prop, int* indic) const;
        
    //! computes properties in (p,s) plane by 
    //! direct calls to refprop methods for performance optimisation
    EOS_Internal_Error calrp_optim_ps(double p, double s, double* tab_prop, 
                                      double* tab_der_prop) const;
        
    //! computes an array of flags in order to minimize the number of calls 
    //! to Refprop routines according to the requested properties 
    void calrp_indic_pt(EOS_Fields& r, int* indic) const;

    //! computes properties in (p,t) plane by 
    //! direct calls to refprop methods for performance optimisation
    EOS_Internal_Error calrp_optim_pt(double p, double t, double* tab_prop, 
                                      double* tab_der_prop, int* indic) const;
        
    //! computes an array of flags in order to minimize the number of calls 
    //! to Refprop routines according to the requested saturation properties 
    void calrp_indic_sat_p(EOS_Fields& r, int* indic) const;
        
    //! computes saturation properties by 
    //! direct calls to refprop methods for performance optimisation
    inline EOS_Internal_Error calrp_optim_sat_p(double p, double* tab_prop,
                                       double* tab_der_prop, double* tab_der2_prop,
                                       int* indic) const;

    //! computes saturation properties
    EOS_Error compute( const EOS_Field& p, EOS_Fields& r, EOS_Error_Field& errfield) const;

    //Saturation methods
    // T_sat(P)
    virtual EOS_Internal_Error compute_T_sat_p(double p, double& T_sat) const ;
    // rho_lsat(P), rho_vsat(P)
    virtual EOS_Internal_Error compute_rho_l_sat_p(double p, double& rho_l_sat) const ;
    virtual EOS_Internal_Error compute_rho_v_sat_p(double p, double& rho_v_sat) const ;
    // h_lsat(P), h_vsat(P)
    virtual EOS_Internal_Error compute_h_l_sat_p(double p, double& h_l_sat) const ;
    virtual EOS_Internal_Error compute_h_v_sat_p(double p, double& h_v_sat) const ;
    // cp_lsat(P), cp_vsat(P)
    virtual EOS_Internal_Error compute_cp_l_sat_p(double p, double& cp_sat) const ;      
    virtual EOS_Internal_Error compute_cp_v_sat_p(double p, double& cp_sat) const ;      
    //
    virtual EOS_Internal_Error compute_d_T_sat_d_p_p(double p, double& dT_sat_dp) const ;

    // p_sat(T)
    virtual EOS_Internal_Error compute_p_sat_T(double T, double& p_sat) const ;
    // h_lsat(T), h_vsat(T)
    virtual EOS_Internal_Error compute_h_l_sat_T(double T, double& h_lsat) const ;
    virtual EOS_Internal_Error compute_h_v_sat_T(double T, double& h_vsat) const ;

    //ph functions
    //! T(p,h)
    virtual EOS_Internal_Error compute_T_ph(double p, double h, double&) const;
    //! rho(p,h)
    virtual EOS_Internal_Error compute_rho_ph(double p, double h, double&) const;
    //! lambda(p,h)
    virtual EOS_Internal_Error compute_lambda_ph(double p, double h, double&) const;
    //! beta(p,h)
    virtual EOS_Internal_Error compute_beta_ph(double p, double h, double&) const;
    //! mu(p,h)
    virtual EOS_Internal_Error compute_mu_ph(double p, double h, double&) const;
    //! cv(p,h)
    virtual EOS_Internal_Error compute_cv_ph(double p, double h, double&) const;
    //! cp(p,h)
    virtual EOS_Internal_Error compute_cp_ph(double p, double h, double&) const;
    //! sigma(p,h)
    virtual EOS_Internal_Error compute_sigma_ph(double p, double h, double&) const;
    //! w(p,h)
    virtual EOS_Internal_Error compute_w_ph(double p, double h, double&) const;
    //! u(p,h)
    virtual EOS_Internal_Error compute_u_ph(double p, double h, double&) const;
    //! s(p,h)
    virtual EOS_Internal_Error compute_s_ph(double p, double h, double&) const;

    //! d(rho)/dp    at constant specific enthalpy
    virtual EOS_Internal_Error compute_d_rho_d_p_h_ph(double p, double h, double&) const;
    //! d(rho)/dh      at constant pressure
    virtual EOS_Internal_Error compute_d_rho_d_h_p_ph(double p, double h, double&) const;
    //! d(T)/dp      at constant specific enthalpy
    virtual EOS_Internal_Error compute_d_T_d_p_h_ph(double p, double h, double&) const;
    //! d(T)/dh      at constant pressure
    virtual EOS_Internal_Error compute_d_T_d_h_p_ph(double p, double h, double&) const;

    //! h(p,T)
    virtual EOS_Internal_Error compute_h_pT(double p, double T, double& h) const;
    virtual EOS_Internal_Error compute_d_h_d_T_p_pT(double p, double T, double& h) const;
    //! h(p,s)
    virtual EOS_Internal_Error compute_h_ps(double p, double s, double& h) const;
    //! rho(p,T)
    virtual EOS_Internal_Error compute_rho_pT(double p, double T, double&) const;
    //! u(p,T)
    virtual EOS_Internal_Error compute_u_pT(double p, double T, double&) const;
    //! s(p,T)
    virtual EOS_Internal_Error compute_s_pT(double p, double T, double&) const;
    //! mu(p,T)
    virtual EOS_Internal_Error compute_mu_pT(double p, double T, double&) const;
    //! lambda(p,T)
    virtual EOS_Internal_Error compute_lambda_pT(double p, double T, double&) const;
    //! pp(p,T)
    virtual EOS_Internal_Error compute_cp_pT(double p, double T, double&) const;
    //! sigma(p,T)
    virtual EOS_Internal_Error compute_sigma_pT(double p, double T, double&) const;
    //! w(p,T)
    virtual EOS_Internal_Error compute_w_pT(double p, double T, double&) const;
    //! g(p,T)
    virtual EOS_Internal_Error compute_g_pT(double p, double T, double& g) const;
    //! g(p,h)
    virtual EOS_Internal_Error compute_g_ph(double p, double h, double& g) const;
    //! f(p,T)
    virtual EOS_Internal_Error compute_f_pT(double p, double T, double& f) const;
    //! f(p,h)
    virtual EOS_Internal_Error compute_f_ph(double p, double h, double& f) const;


    //General fluid properties
    //critical values
    virtual EOS_Internal_Error get_T_crit(double&) const;
    virtual EOS_Internal_Error get_p_crit(double&) const;
    virtual EOS_Internal_Error get_rho_crit(double& ) const;
    virtual EOS_Internal_Error get_h_crit(double&) const;

    //limits values
    virtual EOS_Internal_Error get_T_min(double&) const;
//     h_min, p_min and rho_min not implemented
//     virtual EOS_Internal_Error get_h_min(double&) const;
//     virtual EOS_Internal_Error get_p_min(double&) const;
    virtual EOS_Internal_Error get_T_max(double&) const;
    virtual EOS_Internal_Error get_p_max(double&) const;
    virtual EOS_Internal_Error get_rho_max(double&) const;
    virtual EOS_Internal_Error get_h_max(double&) const;

    // Molar mass (kg/mol)
    virtual EOS_Internal_Error get_mm(double&) const;
    //End General fluid properties

    // spinodal
    virtual EOS_Internal_Error compute_h_v_lim_p(double p, double& h_v) const;
    virtual EOS_Internal_Error compute_h_l_lim_p(double p, double& h_l) const;

    //
    //  Other methods
    //
    //! see Language
    virtual ostream& print_On (ostream& stream=cout) const;
    //! see Language
    virtual istream& read_On (istream& stream=cin);
    //! see Language
    virtual const Type_Info& get_Type_Info () const;

    //Error metod
    void describe_error(const EOS_Internal_Error error, AString & description) const;

  protected:
    mutable double* arr_molfrac          ;   // array of mol fraction used in some Refprop subroutine
    mutable vector <double> varr_molfrac ;
    mutable int nbcomp     ;                 // number of component in mixture; 1 for pure fluid
    mutable int xphase     ;                 // xphase = 1/2 : for mixture fluid, arr_molfrac is liquid or vapor composition
    AString data_file_name ;
    AString hrf   ;
    AString hmix  ;
    AString hfmix ;
    AString htype ;
    Strings hcomp ;
    AString fluid_path ;
    int setmod ;
    int setref ;
    int fluid  ;
    double h0, s0, t0, p0 ;
    
    //Errors
    mutable vector<string> errorMsgs;  // array of AStrings for error messages
    mutable int iline;                 // line number in errMsgs ( 0 to (err_max_10-1))
    mutable int errcode;               // error code to used   ( < errcodemax)
    
    EOS_Internal_Error callSetup() const;
    
    //Generate error AString
    EOS_Internal_Error generate_error(int ierr_10, const char* err) const;
    
    //! Compute REFPROP phase flag (1=liquid ; 2=vapor)
    virtual EOS_Internal_Error calrp_kph(double p, double h, int& kph) const;
 
    //! Compute REFPROP phase flag (1=liquid ; 2=vapor) in (p,T)
    virtual EOS_Internal_Error calrp_kph_pT(double p, double T, int& kph) const;

    //! rho(p,T,kph) with REFPROP phase flag (1=liquid ; 2=vapor)
    virtual EOS_Internal_Error calrp_rho_pT_kph(double p, double T, int kph, double& rho) const;
 
    /*
     * Units conversion functions
     * EOS parameters unit (SI)  <--->  REFPROP subroutine units
     */
    double kpa2pa(double) const;
    double pa2kpa(double) const;
    double mol2kg(double) const;
    double kg2mol(double) const;
    double eos_rho_2_refprop(double) const;
    double refprop_rho_2_eos(double) const;
    double micropa2pa(double) const;
    double pa2micropa(double) const;
    double eos_nrj_2_refprop(double) const;
    double refprop_nrj_2_eos(double) const;
    double refprop_pr_2_eos(double) const;

  private:
    static int type_Id;
    AString FluidStr ;
    AString EosStr; 

    double wmm; // molecular weight (g/mol)
    double tcrit;
    double pcrit,   pcrit_r   ;
    double rhocrit, rhocrit_r ;
    double hcrit,   hcrit_r   ;
    double scrit,   scrit_r   ;
    double ucrit,   ucrit_r   ;


    //limit domains values
    double tmin;
    double tmax;
    double hmax;
    double pmax;
    double rhomax;

    Strings arr_hname; // component name [character*12]
    Strings arr_hn80;  // component name--long form [character*80]
    Strings arr_hcas;  // CAS (Chemical Abstracts Service) number [character*12]


    //Functions used in init()
    void set_allpath(const Strings& strings);
    void init_limits();
    EOS_Internal_Error init_allparams();

    //! call_tpflsh : call TPFLSH REFPROP subroutine
    //! call_tpflsh : units of p and value parameters : EOS unit
    EOS_Internal_Error call_tpflsh(EOS_thermprop prop, double p, double T, double& value) const;

    //! call_phflsh : call PHFLSH REFPROP subroutine
    //! call_phflsh : units of p, h  and value parameters : EOS unit
    EOS_Internal_Error call_phflsh(EOS_thermprop prop, double p, double h, double& value) const;

    //! call_psflsh : call PSFLSH REFPROP subroutine
    //! call_psflsh : units of p, s  and value parameters : EOS unit
    EOS_Internal_Error call_psflsh(EOS_thermprop prop, double p, double s, double& value) const;

    //! call_dhd1   : call DHD1 REFPROP subroutine
    EOS_Internal_Error call_dhd1(EOS_Property prop, double p, double h, double& value) const;

    //! call_ag     : call AG REFPROP subroutine
    //! call_ag     : units of rho and value parameters : EOS Unit
    EOS_Internal_Error call_ag(EOS_thermprop prop, double t, double rho, double& value) const;

    EOS_Internal_Error critical_point(double& tcrit, double& pcrit, double& rhocrit,
                                      double& hcrit, double& scrit, double& ucrit,
                                      double& pcrit_r, double& rhocrit_r, double& hcrit_r,
                                      double& scrit_r, double& ucrit_r) const;

    //! Evaluate h_l_lim or h_v_lim (newton method) of spinodal
    EOS_Internal_Error newton_hlim(const char* const property_name, double p, double& h_lim) const;

    //! call_phfl1  : T(p,h) with the phase flag kph (1=liquid ; 2=vapor)
    EOS_Internal_Error call_phfl1(double p, double h, int kph, double& T) const;

    //! call_tprho : rho(p,T) with the phase flag kph (1=liquid ; 2=vapor) - rho return with refprop unit
    EOS_Internal_Error call_tprho_kph(double p, double h, int kph, double& rho_refprop) const;

    // NOT USED
    //! evaluate_t_lim : evaluate t_lim with Newton method (for spinodal function)
    //EOS_Internal_Error evaluate_t_lim(const char* const property_name, double p, double& t_lim) const;
    //! call_spndl  : call SPNDL REFPROP subroutine
    //! call_spndl  : return values in EOS unit
    //EOS_Internal_Error call_spndl(const char* const property_name, double p, double t_lim, double& rho) const;


  };
}
#include "EOS_Refprop10_i.hxx"
#endif
