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
#ifndef EOS_IPP_HXX_
#define EOS_IPP_HXX_

#include "EOS/API/EOS.hxx"
#include "EOS/API/EOS_Error.hxx"              // ajout M.F.
#include "EOS/API/EOS_Std_Error_Handler.hxx"  // ajout M.F.
#include "Language/API/Language.hxx"
#include "EOS_IGen/Src/EOS_Med.hxx"
#include <vector>
using std::vector;

using namespace NEPTUNE;

namespace NEPTUNE_EOS
{
   class EOS_Ipp :  public EOS_Fluid
   { static const AString tablename ;

     public:
        virtual const AString& table_name() const ;

        EOS_Ipp() ;
        virtual ~EOS_Ipp() ;

        //! to initialize an implementation of EOS_Ipp
        virtual int init(const Strings&) ;
        //! to initialize an implementation of EOS_Ipp with supplementary parameters
        virtual int init(const Strings&, const Strings&) ;

        //! Error handling methods
        void describe_error(const EOS_Internal_Error error, AString & description) const ;

        static const EOS_Internal_Error OUT_OF_BOUNDS  ;
        static const EOS_Internal_Error INVERT_h_pT    ;
        static const EOS_Internal_Error PROP_NOT_IN_DB ;

        //! critical T
        virtual EOS_Internal_Error get_T_crit(double&) const ;
        //! critical p
        virtual EOS_Internal_Error get_p_crit(double&) const ;
        //! critical h
        virtual EOS_Internal_Error get_h_crit(double&) const ;
  
        //! limits values
        virtual EOS_Internal_Error get_h_min(double&) const ;
        virtual EOS_Internal_Error get_h_max(double&) const ;
        virtual EOS_Internal_Error get_T_min(double&) const ;
        virtual EOS_Internal_Error get_T_max(double&) const ;
        virtual EOS_Internal_Error get_p_min(double&) const ;
        virtual EOS_Internal_Error get_p_max(double&) const ;

        //
        //  Other methods
        //
        //! see Language
        virtual ostream& print_On (ostream& stream=cout) const ;
        //! see Language
        virtual istream& read_On (istream& stream=cin) ;
        //! see Language
        virtual const Type_Info& get_Type_Info () const ;
        
        //! h(p,T)
        virtual EOS_Internal_Error compute_h_pT(double p, double T, double& h) const ;
        //virtual EOS_Internal_Error compute_d_h_d_T_pT(double p, double T, double& h) const;
             
        //! T(p,h)
        virtual EOS_Internal_Error compute_T_ph(double p, double h, double&) const ;
        //! d(T)/dp      at constant specific enthalpy
        virtual EOS_Internal_Error compute_d_T_d_p_h_ph(double p, double h, double&) const ;
        //! d(T)/dh      at constant pressure
        virtual EOS_Internal_Error compute_d_T_d_h_p_ph(double p, double h, double&) const ;
        //! rho(p,h)
        virtual EOS_Internal_Error compute_rho_ph(double p, double h, double&) const ;
        //! rho(p,T)
        virtual EOS_Internal_Error compute_rho_pT(double p, double T, double&) const ;
        //! d(rho)/dp      at constant specific enthalpy
        virtual EOS_Internal_Error compute_d_rho_d_p_h_ph(double p, double h, double&) const ;
        //! d(rho)/dh      at constant pressure
        virtual EOS_Internal_Error compute_d_rho_d_h_p_ph(double p, double h, double&) const ;
        //! u(p,h)
        virtual EOS_Internal_Error compute_u_ph(double p, double h, double&) const ;
        //! u(p,T)
        virtual EOS_Internal_Error compute_u_pT(double p, double T, double&) const ;
        //! d(u)/dp      at constant specific enthalpy
        virtual EOS_Internal_Error compute_d_u_d_p_h_ph(double p, double h, double&) const ;
        //! d(u)/dh      at constant pressure
        virtual EOS_Internal_Error compute_d_u_d_h_p_ph(double p, double h, double&) const ;
        //! s(p,h)
        virtual EOS_Internal_Error compute_s_ph(double p, double h, double&) const ;
        //! s(p,T)
        virtual EOS_Internal_Error compute_s_pT(double p, double T, double&) const ;
        //! d(s)/dp      at constant specific enthalpy
        virtual EOS_Internal_Error compute_d_s_d_p_h_ph(double p, double h, double&) const ;
        //! d(s)/dh      at constant pressure
        virtual EOS_Internal_Error compute_d_s_d_h_p_ph(double p, double h, double&) const ;
        //! mu(p,h)
        virtual EOS_Internal_Error compute_mu_ph(double p, double h, double&) const ;
        //! mu(p,T)
        virtual EOS_Internal_Error compute_mu_pT(double p, double T, double&) const ;
        //! d(mu)/dp      at constant specific enthalpy
        virtual EOS_Internal_Error compute_d_mu_d_p_h_ph(double p, double h, double&) const ;
        //! d(mu)/dh      at constant pressure
        virtual EOS_Internal_Error compute_d_mu_d_h_p_ph(double p, double h, double&) const ;
        //! lambda(p,h)
        virtual EOS_Internal_Error compute_lambda_ph(double p, double h, double&) const ;
        //! lambda(p,T)
        virtual EOS_Internal_Error compute_lambda_pT(double p, double T, double&) const ;
        //! d(lambda)/dp      at constant specific enthalpy
        virtual EOS_Internal_Error compute_d_lambda_d_p_h_ph(double p, double h, double&) const ;
        //! d(lambda)/dh      at constant pressure
        virtual EOS_Internal_Error compute_d_lambda_d_h_p_ph(double p, double h, double&) const ;
        //! cp(p,h)
        virtual EOS_Internal_Error compute_cp_ph(double p, double h, double&) const ;
        //! cp(p,T)
        virtual EOS_Internal_Error compute_cp_pT(double p, double T, double&) const ;
        //! d(cp)/dp      at constant specific enthalpy
        virtual EOS_Internal_Error compute_d_cp_d_p_h_ph(double p, double h, double&) const ;
        //! d(cp)/dh      at constant pressure
        virtual EOS_Internal_Error compute_d_cp_d_h_p_ph(double p, double h, double&) const ;
        //! sigma(p,h)
        virtual EOS_Internal_Error compute_sigma_ph(double p, double h, double&) const ;
        //! sigma(p,T)
        virtual EOS_Internal_Error compute_sigma_pT(double p, double T, double&) const ;
        //! d(sigma)/dp      at constant specific enthalpy
        virtual EOS_Internal_Error compute_d_sigma_d_p_h_ph(double p, double h, double&) const ;
        //! d(sigma)/dh      at constant pressure
        virtual EOS_Internal_Error compute_d_sigma_d_h_p_ph(double p, double h, double&) const ;
        //! w(p,h)
        virtual EOS_Internal_Error compute_w_ph(double p, double h, double&) const ;
        //! w(p,T)
        virtual EOS_Internal_Error compute_w_pT(double p, double T, double&) const ;
        //! d(w)/dp      at constant specific enthalpy
        virtual EOS_Internal_Error compute_d_w_d_p_h_ph(double p, double h, double&) const ;
        //! d(w)/dh      at constant pressure
        virtual EOS_Internal_Error compute_d_w_d_h_p_ph(double p, double h, double&) const ;
        //! g(p,h)
        virtual EOS_Internal_Error compute_g_ph(double p, double h, double&) const ;
        //! g(p,T)
        virtual EOS_Internal_Error compute_g_pT(double p, double T, double&) const ;
        //! d(g)/dp      at constant specific enthalpy
        virtual EOS_Internal_Error compute_d_g_d_p_h_ph(double p, double h, double&) const ;
        //! d(g)/dh      at constant pressure
        virtual EOS_Internal_Error compute_d_g_d_h_p_ph(double p, double h, double&) const ;
        //! f(p,h)
        virtual EOS_Internal_Error compute_f_ph(double p, double h, double&) const ;
        //! f(p,T)
        virtual EOS_Internal_Error compute_f_pT(double p, double T, double&) const ;
        //! d(f)/dp      at constant specific enthalpy
        virtual EOS_Internal_Error compute_d_f_d_p_h_ph(double p, double h, double&) const ;
        //! d(f)/dh      at constant pressure
        virtual EOS_Internal_Error compute_d_f_d_h_p_ph(double p, double h, double&) const ;
        //! pr(p,h)
        virtual EOS_Internal_Error compute_pr_ph(double p, double h, double&) const;
        //! pr(p,T)
        virtual EOS_Internal_Error compute_pr_pT(double p, double T, double&) const;
        //! d(pr)/dp      at constant specific enthalpy
        virtual EOS_Internal_Error compute_d_pr_d_p_h_ph(double p, double h, double&) const;
        //! d(pr)/dh      at constant pressure
        virtual EOS_Internal_Error compute_d_pr_d_h_p_ph(double p, double h, double&) const;
        //! beta(p,h)
        virtual EOS_Internal_Error compute_beta_ph(double p, double h, double&) const;
        //! beta(p,T)
        virtual EOS_Internal_Error compute_beta_pT(double p, double T, double&) const;
        //! d(beta)/dp      at constant specific enthalpy
        virtual EOS_Internal_Error compute_d_beta_d_p_h_ph(double p, double h, double&) const;
        //! d(beta)/dh      at constant pressure
        virtual EOS_Internal_Error compute_d_beta_d_h_p_ph(double p, double h, double&) const;
        //! gamma(p,h)
        virtual EOS_Internal_Error compute_gamma_ph(double p, double h, double&) const;
        //! gamma(p,T)
        virtual EOS_Internal_Error compute_gamma_pT(double p, double T, double&) const;
        //! d(gamma)/dp      at constant specific enthalpy
        virtual EOS_Internal_Error compute_d_gamma_d_p_h_ph(double p, double h, double&) const;
        //! d(gamma)/dh      at constant pressure
        virtual EOS_Internal_Error compute_d_gamma_d_h_p_ph(double p, double h, double&) const;
        //! rho_l_sat
        virtual EOS_Internal_Error compute_rho_l_sat_p(double p, double&) const;
        virtual EOS_Internal_Error compute_d_rho_l_sat_d_p_p(double p, double&) const;
        //! rho_v_sat
        virtual EOS_Internal_Error compute_rho_v_sat_p(double p, double&) const;
        virtual EOS_Internal_Error compute_d_rho_v_sat_d_p_p(double p, double&) const;
        //! h_l_sat
        virtual EOS_Internal_Error compute_h_l_sat_p(double p, double&) const;
        virtual EOS_Internal_Error compute_d_h_l_sat_d_p_p(double p, double&) const;
        //! h_v_sat
        virtual EOS_Internal_Error compute_h_v_sat_p(double p, double&) const;
        virtual EOS_Internal_Error compute_d_h_v_sat_d_p_p(double p, double&) const;
        //! cp_l_sat
        virtual EOS_Internal_Error compute_cp_l_sat_p(double p, double&) const;
        virtual EOS_Internal_Error compute_d_cp_l_sat_d_p_p(double p, double&) const;
        //! cp_v_sat
        virtual EOS_Internal_Error compute_cp_v_sat_p(double p, double&) const;
        virtual EOS_Internal_Error compute_d_cp_v_sat_d_p_p(double p, double&) const;
        //! T_sat
        virtual EOS_Internal_Error compute_T_sat_p(double p, double&) const;
        virtual EOS_Internal_Error compute_d_T_sat_d_p_p(double p, double&) const;
        //! h_l_lim
        virtual EOS_Internal_Error compute_h_l_lim_p(double p, double&) const;
        //! h_v_lim
        virtual EOS_Internal_Error compute_h_v_lim_p(double p, double&) const;

     protected:
        EOS_Fields nodes ;

        AString method    ;
        AString reference ;
        
        AString base_method ;
        AString base_reference ;
        
        AString med_file ;

        double pmin      ;
        double pmax      ;
        double hmin      ;
        double hmax      ;
        double tmin      ;
        double tmax      ;
        double tcrit     ;
        double pcrit     ;
        double hcrit     ;
        double delta_p_f ;
        double delta_h_f ;

        //Load db med file
        EOS_Fields nodes_ph     ;
        EOS_Fields nodes_sat    ;
        EOS_Fields nodes_lim    ;
        EOS_Fields val_prop_ph  ;
        EOS_Fields val_prop_sat ;
        EOS_Fields val_prop_lim ;
        ArrOfInt connect_ph    ;
        ArrOfInt index_conn_ph ;
        ArrOfInt connect_sat   ;
        ArrOfInt connect_lim   ;
        
        ArrOfDouble n_p_ph     ;
        ArrOfDouble n_h_ph     ;
        ArrOfDouble n_p_satlim ;
        vector<ArrOfDouble> all_prop_val ;
        vector<ArrOfInt>    all_err_val  ;

        //pre-traitement
        //EOS_Fields fm_ph;
        vector<EOS_Error_Field> err_cell_ph  ;
        vector<EOS_Error_Field> err_segm_sat ;
        vector<EOS_Error_Field> err_segm_lim ;

        //void make_f_mesh();
        void f_mesh2r_mesh() ;
        void node_err2mesh_err(EOS_Error_Field& val_nodes_ph) ;
        void node_err2segm_err(EOS_Error_Field& val_nodes_p, int satlim) ;

        EOS_Internal_Error compute_prop_ph(AString& prop, double p, double h, double& res) const ;
        EOS_Internal_Error compute_prop_p(AString& prop, double p, int tag, double& res) const ;
        EOS_Internal_Error get_cell_values(int idx, AString& property, EOS_Fields& cell_val) const ;
        EOS_Internal_Error get_segm_values(int idx, AString& property, int tag, EOS_Fields& segm_val) const ;

        EOS_Internal_Error compute_h_l_pT(double p, double T, double& res) const ;
        EOS_Internal_Error compute_h_v_pT(double p, double T, double& res) const ;
        EOS_Internal_Error check_p_bounds_ph(double p) const ;
                
     private: 
        static int type_Id ;
        AString FluidStr ;

        ArrOfInt corners;           // liste des 4 noeuds formant les angles de chaque mailles du
                                    // maillage non conforme. Taille : 4 * nb_cells_med_mesh
                                    // sommet i de la maille j -> corners[i + 4*j]
        ArrOfInt fnodes2phnodes ;   // correspondance entre chaque maille du maillage ph et la maille
                                    // du maillage non conforme (med) dans laquelle elle est
          
        void load_domain_values(EOS_Med& med) ;
        EOS_Error load_med_nodes(EOS_Med& med);
        EOS_Error load_med_champ(EOS_Med& med);
        EOS_Error load_med_champ(EOS_Med& med, const Strings& properties) ;
        EOS_Error load_med_scalar(EOS_Med& med) ;

        int get_cellidx(double& p, double& h) const ;
        int get_segmidx(double& p, int tag) const ;
        void linear_interpolator(double p, double& res) const ;
        double linear_interpolator(double p, EOS_Fields& segmval) const ;
        void bilinear_interpolator(double p, double h, double& res) const ;
        double bilinear_interpolator(double p, double h, EOS_Fields& cellval) const ;

        EOS_Internal_Error check_ph_bounds(double p,double h) const ;
        EOS_Internal_Error check_p_bounds_satlim(double p) const ;
  };
}

#include "EOS_Ipp_i.hxx"
#endif /* EOS_IPP_HXX_ */
