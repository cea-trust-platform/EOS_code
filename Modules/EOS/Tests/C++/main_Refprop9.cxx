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


// Tests seulement la base Refprop9 


#include "EOS/API/EOS.hxx"
#include "EOS/API/EOS_Fields.hxx"
#include "EOS/API/EOS_Error_Field.hxx"
#include "EOS/API/EOS_Std_Error_Handler.hxx"
#include <stdio.h>
#include <chrono>
using namespace NEPTUNE; 


const char *list_prop1[] = {
  "p",
  "" };
const char *list_prop2[] = {
  "h",
  "T",
  "" };


// In diagram 2D, Vector thermprop_r
#if  __cplusplus >= 201103L
  static vector<string> thermprop_r =
#else
  static string t_thermprop_r[] =
#endif
    { "T",
      "rho",
      "u",
      "s",
      "mu",
      "lambda",
      "cp",
      "sigma",
      "w",
      "g",
      "f",
      "pr",
      "beta",
      "gamma",
      "d_T_d_p_h",
      "d_T_d_h_p",
      "d_rho_d_p_h",
      "d_rho_d_h_p",
      "d_u_d_p_h",
      "d_u_d_h_p",
      "d_s_d_p_h",
      "d_s_d_h_p",
      "d_mu_d_p_h",
      "d_mu_d_h_p",
      "d_lambda_d_p_h",
      "d_lambda_d_h_p",
      "d_cp_d_p_h",
      "d_cp_d_h_p",
      "d_sigma_d_p_h",
      "d_sigma_d_h_p",
      "d_w_d_p_h",
      "d_w_d_h_p",
      "d_g_d_p_h",
      "d_g_d_h_p",
      "d_f_d_p_h",
      "d_f_d_h_p",
      "d_pr_d_p_h",
      "d_pr_d_h_p",
      "d_beta_d_p_h",
      "d_beta_d_h_p",
      "d_gamma_d_p_h",
      "d_gamma_d_h_p",
      "d_rho_d_T_p",
      "d_u_d_T_p",
      "d_s_d_T_p",
      "d_mu_d_T_p",
      "d_lambda_d_T_p",
      "d_cp_d_T_p",
      "d_sigma_d_T_p",
      "d_w_d_T_p",
      "d_g_d_T_p",
      "d_f_d_T_p",
      "d_pr_d_T_p",
      "d_beta_d_T_p",
      "d_gamma_d_T_p",
      "d_h_d_T_p",
      "d_h_d_p_T"
    };
#if  __cplusplus <  201103L
 static std::vector<string> thermprop_r(t_thermprop_r, 
                                      t_thermprop_r + sizeof(t_thermprop_r) / sizeof(string) ) ;
#endif


// At saturation, Vector saturprop_r
#if  __cplusplus >= 201103L
  static vector<string> saturprop_r =
#else
  static string t_saturprop_r[] =
#endif
    { "rho_l_sat",
      "rho_v_sat",
      "h_l_sat",
      "h_v_sat",
      "cp_l_sat",
      "cp_v_sat",
      "T_sat",
      "d_rho_l_sat_d_p",
      "d_rho_v_sat_d_p",
      "d_h_l_sat_d_p",
      "d_h_v_sat_d_p",
      "d_cp_l_sat_d_p",
      "d_cp_v_sat_d_p",
      "d_T_sat_d_p",
      "d2_T_sat_d_p_d_p"
    };
#if  __cplusplus <  201103L
 static std::vector<string> saturprop_r(t_saturprop_r, 
                                      t_saturprop_r + sizeof(t_saturprop_r) / sizeof(string) ) ;
#endif

static EOS_Property n_list_prop1[] = {
    NEPTUNE::p};
static EOS_Property n_list_prop2[] = {
    NEPTUNE::h,
    NEPTUNE::T};

static vector<EOS_Property> n_thermprop_r =
    {NEPTUNE::T,
     NEPTUNE::rho,
     NEPTUNE::u,
     NEPTUNE::s,
     NEPTUNE::mu,
     NEPTUNE::lambda,
     NEPTUNE::cp,
     NEPTUNE::sigma,
     NEPTUNE::w,
     NEPTUNE::g,
     NEPTUNE::f,
     NEPTUNE::pr,
     NEPTUNE::beta,
     NEPTUNE::gamma,
     NEPTUNE::d_T_d_p_h,
     NEPTUNE::d_T_d_h_p,
     NEPTUNE::d_rho_d_p_h,
     NEPTUNE::d_rho_d_h_p,
     NEPTUNE::d_u_d_p_h,
     NEPTUNE::d_u_d_h_p,
     NEPTUNE::d_s_d_p_h,
     NEPTUNE::d_s_d_h_p,
     NEPTUNE::d_mu_d_p_h,
     NEPTUNE::d_mu_d_h_p,
     NEPTUNE::d_lambda_d_p_h,
     NEPTUNE::d_lambda_d_h_p,
     NEPTUNE::d_cp_d_p_h,
     NEPTUNE::d_cp_d_h_p,
     NEPTUNE::d_sigma_d_p_h,
     NEPTUNE::d_sigma_d_h_p,
     NEPTUNE::d_w_d_p_h,
     NEPTUNE::d_w_d_h_p,
     NEPTUNE::d_g_d_p_h,
     NEPTUNE::d_g_d_h_p,
     NEPTUNE::d_f_d_p_h,
     NEPTUNE::d_f_d_h_p,
     NEPTUNE::d_pr_d_p_h,
     NEPTUNE::d_pr_d_h_p,
     NEPTUNE::d_beta_d_p_h,
     NEPTUNE::d_beta_d_h_p,
     NEPTUNE::d_gamma_d_p_h,
     NEPTUNE::d_gamma_d_h_p,
     NEPTUNE::d_rho_d_T_p,
     NEPTUNE::d_u_d_T_p,
     NEPTUNE::d_s_d_T_p,
     NEPTUNE::d_mu_d_T_p,
     NEPTUNE::d_lambda_d_T_p,
     NEPTUNE::d_cp_d_T_p,
     NEPTUNE::d_sigma_d_T_p,
     NEPTUNE::d_w_d_T_p,
     NEPTUNE::d_g_d_T_p,
     NEPTUNE::d_f_d_T_p,
     NEPTUNE::d_pr_d_T_p,
     NEPTUNE::d_beta_d_T_p,
     NEPTUNE::d_gamma_d_T_p,
     NEPTUNE::d_h_d_T_p,
     NEPTUNE::d_h_d_p_T};

static vector<EOS_Property> n_saturprop_r =
    {NEPTUNE::rho_l_sat,
     NEPTUNE::rho_v_sat,
     NEPTUNE::h_l_sat,
     NEPTUNE::h_v_sat,
     NEPTUNE::cp_l_sat,
     NEPTUNE::cp_v_sat,
     NEPTUNE::T_sat,
     NEPTUNE::d_rho_l_sat_d_p,
     NEPTUNE::d_rho_v_sat_d_p,
     NEPTUNE::d_h_l_sat_d_p,
     NEPTUNE::d_h_v_sat_d_p,
     NEPTUNE::d_cp_l_sat_d_p,
     NEPTUNE::d_cp_v_sat_d_p,
     NEPTUNE::d_T_sat_d_p,
     NEPTUNE::d2_T_sat_d_p_d_p};



// Call to compute(field1, result) and compute(field1, field2, result)
// with many combinations of physical properties.
// Write "Success" for combinations that do not return "NOT_IMPLEMENTED"
void features_status(const EOS & eos,
                     const EOS_Internal_Error err,
                     const char *p1, const char *p2, const char *p3)
{
  char c[1000];
  sprintf(c, "%15s = f(%3s,%3s) ", p3, p1, p2);

  cout << c << "[cr="<<err.generic_error()<<"] ";
  if (err.generic_error() != good) {
    AString s;
    eos.fluid().describe_error(err, s);
    cout << "error " << err.get_partial_code()
         << " " << s;
  }
  cout << endl;
}

void test_features(EOS & eos) 
{ 
  // Configure eos to return to client on any error.

  EOS_Std_Error_Handler handler;
  // EOS should never exit()
  handler.set_exit_on_error(EOS_Std_Error_Handler::disable_feature);
  // EOS should never throw() an exception
  handler.set_throw_on_error(EOS_Std_Error_Handler::disable_feature);
  // EOS should dump informations to cerr on error,bad and ok
  handler.set_dump_on_error(ok);
  handler.set_dump_stream(cerr);

  // eos provides a stack of saved error handlers which
  // is used if we want to locally change the error handler.
  // save_error_handler() must match a restore_error_handler() somewhere...
  eos.save_error_handler();
  eos.set_error_handler(handler);

  // Call compute functions with many combinations of parameters:
  ArrOfDouble array1(1);
  ArrOfDouble array2(1);
  ArrOfDouble result(1);
  ArrOfInt    errors(1);
  EOS_Error_Field err_field(errors);

  cout << "Testing field methods with 1 parameter" << endl;
  // Test with one parameter:
  {
    const char *ptr_satp ;
    int stz = saturprop_r.size() ;
    const char ** ptr1 = list_prop1;
    while (**ptr1 != 0) 
       { for (int i=0; i<stz; i++)
            { ptr_satp = saturprop_r[i].c_str() ; 
              EOS_Field field1(*ptr1, *ptr1, n_list_prop1[0], array1);
              EOS_Field field3(ptr_satp, ptr_satp, n_saturprop_r[i], result);

              array1[0] = 1e5 ;
              // Test field function
              eos.compute(field1, field3, err_field);
              features_status(eos, err_field[0], *ptr1, "", ptr_satp);
            }
         ptr1++ ;
       }
  }
  // Test with two parameters:
  cout << "Testing field methods with 2 parameters" << endl;
  {
    const char *ptr_thep ;
    int stz = thermprop_r.size() ;
    const char ** ptr1 = list_prop1 ;
    while (**ptr1 != 0) 
       { int i1=0; 
        const char ** ptr2 = list_prop2 ;
         while (**ptr2 != 0) 
            { int i2=0;
              for (int i=0; i<stz; i++)
                { ptr_thep = thermprop_r[i].c_str() ; 
                  EOS_Field field1(*ptr1, *ptr1, n_list_prop1[i1], array1) ;
                  EOS_Field field2(*ptr2, *ptr2, n_list_prop2[i2], array2) ; 
                  EOS_Field field3(ptr_thep, ptr_thep, n_thermprop_r[i], result) ;

                  array1[0] = 1.5e5 ;
                  array2[0] = 350.  ;
                  eos.compute(field1, field2, field3, err_field);
                  features_status(eos, err_field[0], *ptr1, *ptr2, ptr_thep);
                }
              ptr2++ ;
              i2++;
            }
         i1++;
         ptr1++ ;
       }
  }
  // Test with two parameters (point method):
  // Point method does not provide the internal error code.
  // To get it, we can use the exception handling feature of the
  // error handling method:
  cout << "Testing point methods with 1 parameter" << endl;
  {
    handler.set_throw_on_error(ok);
    eos.set_error_handler(handler);

    const char *ptr_satp ;
    int stz = saturprop_r.size() ;
    for (int i=0; i<stz; i++)
       { ptr_satp = saturprop_r[i].c_str() ; 
         double p = 1.5e5 ;
         double r = 0.e0  ;
         EOS_Internal_Error ierr = EOS_Internal_Error::OK;
         try 
            { // EOS will throw an EOS_Std_Exception if an error occurs:
              eos.compute(ptr_satp, p, r) ;
            }
         catch (EOS_Std_Exception ex) 
            { // Get the internal error code:
              ierr = ex.err_code ;
            }
         features_status(eos, ierr, "p", "", ptr_satp);
       }
  }
  cout << "Testing point methods with 2 parameters" << endl;
  {
    handler.set_throw_on_error(ok);
    eos.set_error_handler(handler);

    const char *ptr_thep ;
    int stz = thermprop_r.size() ;
    for (int i=0; i<stz; i++)
       { ptr_thep = thermprop_r[i].c_str() ; 
         double p = 1.5e5 ;
         double h = 300   ;
         double r = 0.e0  ;
         EOS_Internal_Error ierr = EOS_Internal_Error::OK;
         try
            { // EOS will throw an EOS_Std_Exception if an error occurs:
              eos.compute(ptr_thep, p, h, r) ;
            }
         catch (EOS_Std_Exception ex)
            { // Get the internal error code:
              ierr = ex.err_code ;
            }
         features_status(eos, ierr, "p", "h", ptr_thep) ;
       }
  }
  // Reset to previous handler
  eos.restore_error_handler() ;
}

int main()
{  auto start = std::chrono::high_resolution_clock::now();

  
  cout<<endl<<endl;
  cout<<"--------------------------------------- "<<endl;
  cout<<"---------------- Begin ---------------- "<<endl;
  cout<<"--------------------------------------- "<<endl;

  Language_init();


  cout<<endl<<"--------------------------------------- "<<endl;
  cout<<      "------ Class available ...  ----------- "<<endl<<endl;

  hierrarchy();






#ifdef WITH_PLUGIN_REFPROP_9
  // Test 9: Refprop9
  {
    cout<<endl<<"--------------------------------------- "<<endl;
    cout<<      "------ Test 9 : Refprop9          ------ "<<endl<<endl;
    {
      EOS_Error cr;
      double p=1.e5;
      double t=350;
      double h=200.e3;
      double pcrit,tcrit,hcrit,rhocrit;
      double pmax,tmax,hmax,rhomax;
      double tmin;
      double res, res2, res3;
            
      int n=3;
      int ncout=1; // (ncout<=n)
            
      ArrOfDouble xp(n);
      ArrOfDouble xh(n);
      ArrOfInt ierr(n);
      EOS_Field P("Pressure","p",NEPTUNE::p,xp);
      EOS_Field h_f("Enthalpy","h",NEPTUNE::h,xh);
      EOS_Error_Field err_f(ierr);
            
      ArrOfDouble xrholsat(n);
      ArrOfDouble xrhovsat(n);
      ArrOfDouble xhlsat(n);
      ArrOfDouble xhvsat(n);
      ArrOfDouble xtsat(n);
      ArrOfDouble xdtsatdp(n);
            
      ArrOfDouble xt(n);
      ArrOfDouble xrho(n);
      ArrOfDouble xu(n);
      ArrOfDouble xdtdh(n);
      ArrOfDouble xw(n);
      ArrOfDouble xs(n);
      ArrOfDouble xcp(n);
      ArrOfDouble xcv(n);
      ArrOfDouble xvp(n);
      ArrOfDouble xdrhodp(n);
      ArrOfDouble xdrhodh(n);
      ArrOfDouble xmu(n);
      ArrOfDouble xlambda(n);
      ArrOfDouble xsigma(n);
      ArrOfDouble xg(n);
      ArrOfDouble xf(n);
      ArrOfDouble xpr(n);
      ArrOfDouble xbeta(n);

      cout<<endl<<"--------------------------------------- "<<endl;
      cout<<      "------ Test9-1: Refprop9-water    ------ "<<endl<<endl;

      EOS refprop9("EOS_Refprop9","Water");
      cout << "**   eos " << endl;
      cout << "   * fluid   : " <<refprop9.fluid_name()<<endl;
      cout << "   * table   : " <<refprop9.table_name()<<endl;
      cout << "   * version : " <<refprop9.version_name()<<endl<<endl;
      cout << refprop9<<endl;

      test_features(refprop9);

      // Valeurs critiques (CRITP)
      cr = refprop9.get_p_crit(pcrit);
      cr = refprop9.get_T_crit(tcrit);
      cr = refprop9.get_h_crit(hcrit);
      cr = refprop9.get_rho_crit(rhocrit);

      cout << endl << endl << "----- critp-------" << endl;
      cout << "Pc    : " << pcrit << endl;
      cout << "Tc    : " << tcrit << endl;
      cout << "hc    : " << hcrit << endl;
      cout << "rhoc   : " << rhocrit << endl << endl << endl;
      
      // Valeurs limites
//       cr = refprop9.get_p_min(pmin);
      cr = refprop9.get_T_min(tmin);
//       cr = refprop9.get_h_min(hmin);
//       cr = refprop9.get_rho_min(rhomin);
      
      cr = refprop9.get_p_max(pmax);
      cr = refprop9.get_T_max(tmax);
      cr = refprop9.get_h_max(hmax);
      cr = refprop9.get_rho_max(rhomax);
      cout << endl << endl << "----- limites-------" << endl;
      cout << "Pmax        : " <<pmax   << endl;
      cout << "Tmin - Tmax : " << tmin   <<" - "<<tmax   << endl;
      cout << "hmax        : " <<hmax   << endl;
      cout << "rhomax      : " <<rhomax << endl << endl << endl;
      //saturation
      cout << endl << "Saturation" << endl;
      //compute by point
      cr=refprop9.compute_rho_l_sat_p(p,res);
      cout << "[cr="<<cr<<"] rholsat_p = " << res << endl;
      cr=refprop9.compute_rho_v_sat_p(p,res);
      cout << "[cr="<<cr<<"] rhovsat_p = " << res << endl;
      cr=refprop9.compute_h_l_sat_p(p,res);
      cout << "[cr="<<cr<<"] hlsat_p   = " << res<< endl;
      cr=refprop9.compute_h_v_sat_p(p,res);
      cout << "[cr="<<cr<<"] hvsat_p   = " << res << endl;
      cr=refprop9.compute_T_sat_p(p,res);
      cout << "[cr="<<cr<<"] tsat_p    = " << res << endl;
      cr=refprop9.compute_d_T_sat_d_p_p(p,res);
      cout << "[cr="<<cr<<"] dtsatdp   = " << res << endl;
      cr=refprop9.compute_p_sat_T(t,res);
      cout << "[cr="<<cr<<"] psat_T    = " << res << endl<<endl;

      //compute by field
      int nbfields=6;
      EOS_Fields outsat(nbfields);
      outsat[0]=EOS_Field("rholsat", "rho_l_sat",NEPTUNE::rho_l_sat,xrholsat);
      outsat[1]=EOS_Field("rhovsat", "rho_v_sat",NEPTUNE::rho_v_sat,xrhovsat);
      outsat[2]=EOS_Field("hlsat","h_l_sat",NEPTUNE::h_l_sat,xhlsat);
      outsat[3]=EOS_Field("hvsat","h_v_sat",NEPTUNE::h_v_sat,xhvsat);
      outsat[4]=EOS_Field("tsat","T_sat",NEPTUNE::T_sat,xtsat);
      outsat[5]=EOS_Field("dtsatdp","d_T_sat_d_p",NEPTUNE::d_T_sat_d_p,xdtsatdp);

      for(int i=0; i<n; i++)
        xp[i]=p;
      xp[0]=1.e5;
      xp[1]=10.e5;
      xp[2]=70.e5;
      for(int i=0; i<n; i++)
        xh[i]=h;

      cr=refprop9.compute(P,outsat,err_f);
      cout << endl << "field fields [cr=" << cr <<"]"<< endl;
      if (cr != good)
        {
          AString s;
          cout<<" err " <<cr << endl;
          for(int i=0; i<ncout; i++)
            {
              refprop9.fluid().describe_error(err_f[i], s);
              cout<<err_f[i].generic_error()<<" "<<err_f[i].get_code()<<" "<<err_f[i].get_partial_code()<<" "<<s<<endl;
            }
        }

      for(int i=0; i<ncout; i++)
        {
          cout <<"in "<<P.get_property_title()<<" "<<P[i]<<endl;
          for(int j=0; j<nbfields; j++)
            cout<<" out "<< outsat[j].get_property_title()<<" "<<outsat[j][i] << endl ;
        }

      //plan pT
      cout << endl << endl << "pT" << endl;
      cr=refprop9.compute_rho_pT(p,t,res);
      cout << "[cr="<<cr<<"] rho_pT    = " << res << endl;
      cr=refprop9.compute_h_pT(p,t,res);
      cout << "[cr="<<cr<<"] h_pT      = " << res << endl;
      cr=refprop9.compute_d_h_d_T_p_pT(p,t,res);
      cout << "[cr="<<cr<<"] dhdT_pT   = " << res << endl;
      cr=refprop9.compute_u_pT(p,t,res);
      cout << "[cr="<<cr<<"] u_pT      = " << res << endl;
      cr=refprop9.compute_s_pT(p,t,res);
      cout << "[cr="<<cr<<"] s_pT      = " << res << endl;
      cr=refprop9.compute_w_pT(p,t,res);
      cout << "[cr="<<cr<<"] w_pT      = " << res << endl;
      cr=refprop9.compute_cp_pT(p,t,res);
      cout << "[cr="<<cr<<"] cp_pT     = " << res << endl;
      cr=refprop9.compute_cv_pT(p,t,res);
      cout << "[cr="<<cr<<"] cv_pT     = " << res << endl;
      cr=refprop9.compute_mu_pT(p,t,res);
      cout << "[cr="<<cr<<"] mu_pT     = " << res << endl;
      cr=refprop9.compute_lambda_pT(p,t,res);
      cout << "[cr="<<cr<<"] lambda_pT = " << res << endl;
      cr=refprop9.compute_sigma_pT(p,t,res);
      cout << "[cr="<<cr<<"] sigma_pT  = " << res << endl;
      cr=refprop9.compute_beta_pT(p,t,res);
      cout << "[cr="<<cr<<"] beta_pT   = " << res << endl;
      cr=refprop9.compute_g_pT(p,t,res);
      cout << "[cr="<<cr<<"] g_pT      = " << res << endl;
      cr=refprop9.compute_f_pT(p,t,res);
      cout << "[cr="<<cr<<"] f_pT      = " << res << endl;
      cr=refprop9.compute_pr_pT(p,t,res);
      cout << "[cr="<<cr<<"] pr_pT     = " << res << endl << endl;


      // -- properties=f(pT)
      int nbfields_pT=1;
      EOS_Fields outpt(nbfields_pT);
      outpt[0]=EOS_Field("enthalpie","h",NEPTUNE::h,xh);

      EOS_Field T("Temperature","T",NEPTUNE::T,xt);
      for(int i=0; i<n; i++)
        xt[i]=t;
      xt[0]=t;
      xt[1]=t+20.;
      xt[2]=t+40.;

      cr=refprop9.compute(P,T,outpt,err_f);
      cout << endl<< "field field fields [cr=" << cr <<"]"<< endl;
      for(int i=0; i<ncout; i++)
        {
          cout <<"in "<<P.get_property_title()<<" "<<P[i]<<endl;
          cout <<"in "<<T.get_property_title()<<" "<<T[i]<<endl;
          for(int j=0; j<nbfields_pT; j++)
            cout<<" out "<< outpt[j].get_property_title()<<" "<<outpt[j][i] << endl ;
        }

      //plan ph
      cout << endl << endl << "ph" << endl;

      cr=refprop9.compute_T_ph(p,h,res);
      cout << "[cr="<<cr<<"] T_ph      = " << res << endl;
      cr=refprop9.compute_rho_ph(p,h,res);
      cout << "[cr="<<cr<<"] rho_ph    = " << res << endl;
      cr=refprop9.compute_w_ph(p,h,res);
      cout << "[cr="<<cr<<"] w_ph      = " << res << endl;
      cr=refprop9.compute_u_ph(p,h,res);
      cout << "[cr="<<cr<<"] u_ph      = " << res << endl;
      cr=refprop9.compute_cp_ph(p,h,res);
      cout << "[cr="<<cr<<"] cp_ph     = " << res << endl;
      cr=refprop9.compute_cv_ph(p,h,res);
      cout << "[cr="<<cr<<"] cv_ph     = " << res << endl;
      cr=refprop9.compute_mu_ph(p,h,res);
      cout << "[cr="<<cr<<"] mu_ph     = " << res << endl;
      cr=refprop9.compute_lambda_ph(p,h,res);
      cout << "[cr="<<cr<<"] lambda_ph = " << res << endl;
      cr=refprop9.compute_sigma_ph(p,h,res);
      cout << "[cr="<<cr<<"] sigma_ph  = " << res << endl;
      cr=refprop9.compute_beta_ph(p,t,res);
      cout << "[cr="<<cr<<"] beta_ph   = " << res << endl;
      cr=refprop9.compute_d_rho_d_p_h_ph(p,h,res);
      cout << "[cr="<<cr<<"] drhodp_ph = " << res << endl;
      cr=refprop9.compute_d_rho_d_h_p_ph(p,h,res);
      cout << "[cr="<<cr<<"] drhodh_ph = " << res << endl;
      cr=refprop9.compute_d_T_d_h_p_ph(p,h,res);
      cout << "[cr="<<cr<<"] dTdh_ph   = " << res << endl;
      cr=refprop9.compute_g_ph(p,h,res);
      cout << "[cr="<<cr<<"] g_ph      = " << res << endl;
      cr=refprop9.compute_f_ph(p,h,res);
      cout << "[cr="<<cr<<"] f_ph      = " << res << endl;
      cr=refprop9.compute_pr_ph(p,h,res);
      cout << "[cr="<<cr<<"] pr_ph     = " << res << endl << endl;

      //  properties=f(ph)
      ArrOfDouble xhin(n);
      ArrOfDouble xtout(n);

      for(int i=0; i<n; i++)
        xhin[i]=h;
      xhin[0]=h;
      xhin[1]=h+100000.;
      xhin[2]=h+200000.;

      EOS_Field hin("enthalpie","h",NEPTUNE::h,xhin);

      int nbfields_ph=16;
      EOS_Fields outph(nbfields_ph);
      outph[0]=EOS_Field("temperature","T",NEPTUNE::T,xtout);
      outph[1]=EOS_Field("rho","rho",NEPTUNE::rho,xrho);
      outph[2]=EOS_Field("w","w",NEPTUNE::w,xw);
      outph[3]=EOS_Field("u","u",NEPTUNE::u,xu);
      outph[4]=EOS_Field("cp","cp",NEPTUNE::cp,xcp);
      outph[5]=EOS_Field("mu","mu",NEPTUNE::mu,xmu);
      outph[6]=EOS_Field("lambda","lambda",NEPTUNE::lambda,xlambda);
      outph[7]=EOS_Field("sigma","sigma",NEPTUNE::sigma,xsigma);
      outph[8]=EOS_Field("drhodp","d_rho_d_p_h",NEPTUNE::d_rho_d_p_h,xdrhodp);
      outph[9]=EOS_Field("drhodh","d_rho_d_h_p",NEPTUNE::d_rho_d_h_p,xdrhodh);
      outph[10]=EOS_Field("dtdh","d_T_d_h_p",NEPTUNE::d_T_d_h_p,xdtdh);
      outph[11]=EOS_Field("g","g",NEPTUNE::g,xg);
      outph[12]=EOS_Field("f","f",NEPTUNE::f,xf);
      outph[13]=EOS_Field("pr","pr",NEPTUNE::pr,xpr);
      outph[14]=EOS_Field("cv","cv",NEPTUNE::cv,xcv);
      outph[15]=EOS_Field("beta","beta",NEPTUNE::beta,xbeta);

      cr=refprop9.compute(P,hin,outph,err_f);
      cout << endl<< "field field fields [cr=" << cr <<"]"<< endl;
      for(int i=0; i<ncout; i++)
        {
          cout <<"in "<<P.get_property_title()<<" "<<P[i]<<endl;
          cout <<"in "<<hin.get_property_title()<<" "<<hin[i]<<endl;
          for(int j=0; j<nbfields_ph; j++)
            cout<<" out "<< outph[j].get_property_title()<<" "<<outph[j][i] << endl ;
        }
      cout << endl << endl;

      //valeurs limites
      cr=refprop9.compute_h_l_lim_p(p,res);
      cout << "[cr="<<cr<<"] h_l_lim   = " << res << endl;
      cr=refprop9.compute_h_v_lim_p(p,res);
      cout << "[cr="<<cr<<"] h_v_lim   = " << res << endl<<endl;
          
      //plan ps
      cr=refprop9.compute_h_ps(p,s,res);
      cout << "[cr="<<cr<<"] h_ps      = " << res << endl;
          
      //  properties=f(ps)
      ArrOfDouble xsin(n);
      ArrOfDouble xhout(n);
          
      xsin[0]=s;
      xsin[1]=s+100.;
      xsin[2]=s+200.;

      EOS_Field sin("entropie","s",NEPTUNE::s,xsin);

      int nbfields_ps=1;
      EOS_Fields outps(nbfields_ps);
      outps[0]=EOS_Field("enthalpie","h",NEPTUNE::h,xhout);
          
      cr=refprop9.compute(P,sin,outps,err_f);
      cout << endl<< "field field fields [cr=" << cr <<"]"<< endl;
      for(int i=0; i<ncout; i++)
        {
          cout <<"in "<<P.get_property_title()<<" "<<P[i]<<endl;
          cout <<"in "<<sin.get_property_title()<<" "<<sin[i]<<endl;
          for(int j=0; j<nbfields_ps; j++)
            cout<<" out "<< outps[j].get_property_title()<<" "<<outps[j][i] << endl ;
        }

      cout << endl << endl;

      //Error tests
      EOS_Internal_Error err;
      AString description;
      double p2 = pcrit + 1.e6;
      double t2 = tcrit + 100.;

      cout << "ERRORS Tests" << endl << "Some errors generated by REFPROP9" << endl;

      err = refprop9.fluid().compute_T_sat_p(p,res);
      if (err.generic_error() == good)
        cout << "compute_T_sat_p No Error - generic error code : " << err.generic_error() << endl;

      cout << "new p : " << p2 << endl;
      err = refprop9.fluid().compute_T_sat_p(p2,res);
      if (err.generic_error() != good)
        {
          refprop9.fluid().describe_error(err, description);
          cout << "compute_T_sat_p generic error code : " << err.generic_error() << endl;
          cout << description << endl << endl;
        }
      cout << "new t : " << t2 << endl;
      err = refprop9.fluid().compute_p_sat_T(t2,res);
      if (err.generic_error() != good)
        {
          refprop9.fluid().describe_error(err, description);
          cout << "compute_p_sat_T generic error code : " << err.generic_error() << endl;
          cout << description << endl << endl;
        }
      t2 = 0.;
      cout << "new t : " << t2 << endl;
      err = refprop9.fluid().compute_h_pT(p2,t2,res);
      if (err.generic_error() != good)
        {
          refprop9.fluid().describe_error(err, description);
          cout << "compute_h_pT generic error code : " << err.generic_error() << endl;
          cout << description << endl << endl;
        }

      p2 = -100.;
      cout << "new p : " << p2 << endl;
      err = refprop9.fluid().compute_T_ph(p2,h,res);
      if (err.generic_error() != good)
        {
          refprop9.fluid().describe_error(err, description);
          cout << "compute_T_ph generic error code : " << err.generic_error() << endl;
          cout << description << endl << endl;
        }

      //Constructor test : change enthalpy/entropy reference state
      cout << "Change enthalpy/entropy reference state : NBP"<< endl;

      Strings args(2);
      args[0]="SETREF";
      args[1]="NBP";
      EOS refprop9_newh("EOS_Refprop9","Water", args);
      cout << "**   eos " << endl;
      cout << "   * fluid   : " <<refprop9_newh.fluid_name()<<endl;
      cout << "   * table   : " <<refprop9_newh.table_name()<<endl;
      cout << "   * version : " <<refprop9_newh.version_name()<<endl<<endl;

  
      cout<<refprop9_newh;

      args[1]="OTH";
      double h0 = 0.e0 ;
      double s0 = 0.e0 ;
      double t0 = 298.15e0 ;
      double p0 = 1.015e5 ;
      EOS refprop9_sref("EOS_Refprop9","Water", args, h0, s0, t0, p0);
      cout << "**   eos " << endl;
      cout << "   * fluid   : " <<refprop9_sref.fluid_name()<<endl;
      cout << "   * table   : " <<refprop9_sref.table_name()<<endl;
      cout << "   * version : " <<refprop9_sref.version_name()<<endl<<endl;

      cout<<refprop9_sref;

      //Test enthalpy
      t=373.1243;
      cr=refprop9_newh.compute_h_l_sat_T(t,res);
      cout << "[cr="<<cr<<"] hsat_T (NBP)   = " << res<< endl;
      cr=refprop9.compute_h_l_sat_T(t,res2);
      cout << "[cr="<<cr<<"] hsat_T (DEF)   = " << res2 << endl;
      cr=refprop9_sref.compute_h_l_sat_T(t,res3);
      cout << "[cr="<<cr<<"] hsat_T (OTH)   = " << res3 << endl;
      cout << "h difference  : " <<res2-res << " at t = " << t << endl;
      cout << "h2 difference  : " <<res3-res << " at t = " << t << endl;

      t=600.e0;
      refprop9_newh.compute_h_l_sat_T(t,res);
      cout << "hsat_T (NBP)   = " << res<< endl;
      refprop9.compute_h_l_sat_T(t,res2);
      cout << "hsat_T (DEF)   = " << res2 << endl;
      refprop9_sref.compute_h_l_sat_T(t,res3);
      cout << "hsat_T (OTH)   = " << res3 << endl;
      cout << "h difference  : " << res2-res << " at t = " << t << endl;
      cout << "h2 difference  : " << res3-res << " at t = " << t << endl;

      cr=refprop9_newh.compute_h_pT(p,t,res);
      cout << "[cr="<<cr<<"] h_pT (NBP)     = " << res << endl;
      cr=refprop9.compute_h_pT(p,t,res2);
      cout << "[cr="<<cr<<"] h_pT (DEF)     = " << res2 << endl;
      cr=refprop9_sref.compute_h_pT(p,t,res3);
      cout << "[cr="<<cr<<"] h_pT (OTH)     = " << res3 << endl;
      cout << "h difference  : " << res2-res << " at t = " << t << endl;
      cout << "h2 difference  : " << res3-res << " at t = " << t << endl;

      //Test entropy
      t=373.1243;
      cr=refprop9_newh.compute_s_pT(p,t,res);
      cout << "[cr="<<cr<<"] s_pT (NBP)     = " << res << endl;
      cr=refprop9.compute_s_pT(p,t,res2);
      cout << "[cr="<<cr<<"] s_pT (DEF)     = " << res2 << endl;
      cr=refprop9_sref.compute_s_pT(p,t,res3);
      cout << "[cr="<<cr<<"] s_pT (OTH)     = " << res3 << endl;
      cout << "s difference  : " << res2-res << " at t = " << t << endl;
      cout << "s2 difference  : " << res3-res << " at t = " << t << endl;

      t=600.e0;
      cr=refprop9_newh.compute_s_pT(p,t,res);
      cout << "[cr="<<cr<<"] s_pT (NBP)     = " << res << endl;
      cr=refprop9.compute_s_pT(p,t,res2);
      cout << "[cr="<<cr<<"] s_pT (DEF)     = " << res2 << endl;
      cr=refprop9_sref.compute_s_pT(p,t,res3);
      cout << "[cr="<<cr<<"] s_pT (OTH)     = " << res3 << endl;
      cout << "s difference  : " << res2-res <<" at t = " << t << endl;
      cout << "s2 difference  : " << res3-res <<" at t = " << t << endl<<endl;

      //Test internal energy
      t=373.1243;
      cr=refprop9_newh.compute_u_pT(p,t,res);
      cout << "[cr="<<cr<<"] u_pT (NBP)     = " << res << endl;
      cr=refprop9.compute_u_pT(p,t,res2);
      cout << "[cr="<<cr<<"] u_pT (DEF)     = " << res2 << endl;
      cr=refprop9_sref.compute_u_pT(p,t,res3);
      cout << "[cr="<<cr<<"] u_pT (OTH)     = " << res3 << endl;
      cout << "u difference  : " << res2-res << " at t = " << t << endl;
      cout << "u2 difference  : " << res3-res << " at t = " << t << endl;

      t=600.e0;
      cr=refprop9_newh.compute_u_pT(p,t,res);
      cout << "[cr="<<cr<<"] u_pT (NBP)     = " << res << endl;
      cr=refprop9.compute_u_pT(p,t,res2);
      cout << "[cr="<<cr<<"] u_pT (DEF)     = " << res2 << endl;
      cr=refprop9_sref.compute_u_pT(p,t,res3);
      cout << "[cr="<<cr<<"] u_pT (OTH)     = " << res3 << endl;
      cout << "u difference  : " << res2-res <<" at t = " << t << endl;
      cout << "u2 difference  : " << res3-res <<" at t = " << t << endl<<endl;

      //test meta
      double t1,p1;
      t1 = -2. + 273.15;
      p1 = 100000.;
      err = refprop9.fluid().compute_rho_pT(p1,t1,res);
      if (err.generic_error() != good)
        {
          refprop9.fluid().describe_error(err, description);
          cout << "compute_rho_pT generic error code : " << err.generic_error() << endl;
          cout << description << endl << endl;
        }
      else
        cout << "RHO " << res << endl;

      cout << "rho_pT    = " << res << endl;

      //Constructor test : change model (setmod)S
      cout << "Change model : SETMOD EOS/FEQ"<< endl;

      Strings args2(4);
      args2[0]="SETMOD";
      args2[1]="EOS";
      args2[2]="NBS";
      args2[3]="NBS";
      EOS refprop9_setmod("EOS_Refprop9","Water", args2);
      cout << "**   eos " << endl;
      cout << refprop9_setmod<<endl;
    }


    // Test 9-2: Refprop9
    {
      EOS_Error cr;
      double p=1.e5;
      double t=350;
      double h=200.e3;
      double pcrit,tcrit,hcrit;
      double res;

      int n=3;
      int ncout=1; // (ncout<=n)

      ArrOfDouble xp(n);
      ArrOfDouble xh(n);
      ArrOfInt ierr(n);
      EOS_Field P("Pressure","p",NEPTUNE::p,xp);
      EOS_Field h_f("Enthalpy","h",NEPTUNE::h,xh);
      EOS_Error_Field err_f(ierr);

      ArrOfDouble xrholsat(n);
      ArrOfDouble xrhovsat(n);
      ArrOfDouble xhlsat(n);
      ArrOfDouble xhvsat(n);
      ArrOfDouble xtsat(n);
      ArrOfDouble xdtsatdp(n);

      ArrOfDouble xt(n);
      ArrOfDouble xrho(n);
      ArrOfDouble xu(n);
      ArrOfDouble xdtdh(n);
      ArrOfDouble xw(n);
      ArrOfDouble xs(n);
      ArrOfDouble xcp(n);
      ArrOfDouble xcv(n);
      ArrOfDouble xdrhodp(n);
      ArrOfDouble xdrhodh(n);
      ArrOfDouble xmu(n);
      ArrOfDouble xlambda(n);
      ArrOfDouble xsigma(n);
      ArrOfDouble xg(n);
      ArrOfDouble xf(n);
      ArrOfDouble xpr(n);
      ArrOfDouble xbeta(n);


      cout<<endl<<"--------------------------------------- "<<endl;
      cout<<      "------ Test 9-2 : Refprop9 Liquid   ------ "<<endl<<endl;

      EOS refprop9l("EOS_Refprop9","WaterLiquid");

      cout << "**   eos " << endl;
      cout << "   * fluid   : " <<refprop9l.fluid_name()<<endl;
      cout << "   * table   : " <<refprop9l.table_name()<<endl;
      cout << "   * version : " <<refprop9l.version_name()<<endl<<endl;
      cout << refprop9l<<endl;

      test_features(refprop9l);

      // Valeurs critiques (CRITP)
      cr = refprop9l.get_p_crit(pcrit);
      cr = refprop9l.get_T_crit(tcrit);
      cr = refprop9l.get_h_crit(hcrit);

      cout << "----- critp-------" << endl;
      cout << "Pc    : " << pcrit << endl;
      cout << "Tc    : " << tcrit << endl;
      cout << "hc    : " << hcrit << endl << endl << endl;

      //saturation
      cr=refprop9l.compute_rho_l_sat_p(p,res);
      cout << "[cr="<<cr<<"] rholsat_p = " << res << endl;
      cr=refprop9l.compute_rho_v_sat_p(p,res);
      cout << "[cr="<<cr<<"] rhovsat_p = " << res << endl;
      cr=refprop9l.compute_h_l_sat_p(p,res);
      cout << "[cr="<<cr<<"] hlsat_p   = " << res<< endl;
      cr=refprop9l.compute_h_v_sat_p(p,res);
      cout << "[cr="<<cr<<"] hvsat_p   = " << res << endl;
      cr=refprop9l.compute_T_sat_p(p,res);
      cout << "[cr="<<cr<<"] tsat_p    = " << res << endl;
      cr=refprop9l.compute_d_T_sat_d_p_p(p,res);
      cout << "[cr="<<cr<<"] dtsatdp   = " << res << endl;
      cr=refprop9l.compute_h_l_sat_T(t,res);
      cout << "[cr="<<cr<<"] hsat_T    = " << res << endl;
      cr=refprop9l.compute_p_sat_T(t,res);
      cout << "[cr="<<cr<<"] psat_T    = " << res << endl<<endl;

      //compute by field
      int nbfields=6;
      EOS_Fields outsat(nbfields);
      outsat[0]=EOS_Field("rholsat", "rho_l_sat",NEPTUNE::rho_l_sat,xrholsat);
      outsat[1]=EOS_Field("rhovsat", "rho_v_sat",NEPTUNE::rho_v_sat,xrhovsat);
      outsat[2]=EOS_Field("hlsat","h_l_sat",NEPTUNE::h_l_sat,xhlsat);
      outsat[3]=EOS_Field("hvsat","h_v_sat",NEPTUNE::h_v_sat,xhvsat);
      outsat[4]=EOS_Field("tsat","T_sat",NEPTUNE::T_sat,xtsat);
      outsat[5]=EOS_Field("dtsatdp","d_T_sat_d_p",NEPTUNE::d_T_sat_d_p,xdtsatdp);

      for(int i=0; i<n; i++)
        xp[i]=p;
      xp[0]=1.e5;
      xp[1]=10.e5;
      xp[2]=70.e5;
      for(int i=0; i<n; i++)
        xh[i]=h;

      cr=refprop9l.compute(P,outsat,err_f);
      cout << endl << "field fields [cr=" << cr <<"]"<< endl;
      if (cr != good)
        {
          AString s;
          cout<<" err " <<cr << endl;
          for(int i=0; i<ncout; i++)
            {
              refprop9l.fluid().describe_error(err_f[i], s);
              cout<<err_f[i].generic_error()<<" "<<err_f[i].get_code()<<" "<<err_f[i].get_partial_code()<<" "<<s<<endl;
            }
        }

      for(int i=0; i<ncout; i++)
        {
          cout <<"in "<<P.get_property_title()<<" "<<P[i]<<endl;
          for(int j=0; j<nbfields; j++)
            cout<<" out "<< outsat[j].get_property_title()<<" "<<outsat[j][i] << endl ;
        }

      cout << endl << endl;

      //plan pT
      cr=refprop9l.compute_rho_pT(p,t,res);
      cout << "[cr="<<cr<<"] rho_pT    = " << res << endl;
      cr=refprop9l.compute_h_pT(p,t,res);
      cout << "[cr="<<cr<<"] h_pT      = " << res << endl;
      cr=refprop9l.compute_d_h_d_T_p_pT(p,t,res);
      cout << "[cr="<<cr<<"] dhdT_pT   = " << res << endl;
      cr=refprop9l.compute_u_pT(p,t,res);
      cout << "[cr="<<cr<<"] u_pT      = " << res << endl;
      cr=refprop9l.compute_s_pT(p,t,res);
      cout << "[cr="<<cr<<"] s_pT      = " << res << endl;
      cr=refprop9l.compute_w_pT(p,t,res);
      cout << "[cr="<<cr<<"] w_pT      = " << res << endl;
      cr=refprop9l.compute_cp_pT(p,t,res);
      cout << "[cr="<<cr<<"] cp_pT     = " << res << endl;
      cr=refprop9l.compute_cv_pT(p,t,res);
      cout << "[cr="<<cr<<"] cv_pT     = " << res << endl;
      cr=refprop9l.compute_mu_pT(p,t,res);
      cout << "[cr="<<cr<<"] mu_pT     = " << res << endl;
      cr=refprop9l.compute_lambda_pT(p,t,res);
      cout << "[cr="<<cr<<"] lambda_pT = " << res << endl;
      cr=refprop9l.compute_sigma_pT(p,t,res);
      cout << "[cr="<<cr<<"] sigma_pT  = " << res << endl;
      cr=refprop9l.compute_beta_pT(p,t,res);
      cout << "[cr="<<cr<<"] beta_pT   = " << res << endl;
      cr=refprop9l.compute_g_pT(p,t,res);
      cout << "[cr="<<cr<<"] g_pT      = " << res << endl;
      cr=refprop9l.compute_f_pT(p,t,res);
      cout << "[cr="<<cr<<"] f_pT      = " << res << endl;
      cr=refprop9l.compute_pr_pT(p,t,res);
      cout << "[cr="<<cr<<"] pr_pT      = " << res << endl<<endl;

      // -- properties=f(pT)
      int nbfields_pT=1;
      EOS_Fields outpt(nbfields_pT);
      outpt[0]=EOS_Field("enthalpie","h",NEPTUNE::h,xh);

      EOS_Field T("Temperature","T",NEPTUNE::T,xt);
      for(int i=0; i<n; i++)
        xt[i]=t;
      xt[0]=t;
      xt[1]=t+20.;
      xt[2]=t+40.;

      cr=refprop9l.compute(P,T,outpt,err_f);
      cout << endl<< "field field fields [cr=" << cr <<"]"<< endl;
      for(int i=0; i<ncout; i++)
        {
          cout <<"in "<<P.get_property_title()<<" "<<P[i]<<endl;
          cout <<"in "<<T.get_property_title()<<" "<<T[i]<<endl;
          for(int j=0; j<nbfields_pT; j++)
            cout<<" out "<< outpt[j].get_property_title()<<" "<<outpt[j][i] << endl ;

        }

      cout << endl << endl;

      //plan ph
      cr=refprop9l.compute_T_ph(p,h,res);
      cout << "[cr="<<cr<<"] T_ph      = " << res << endl;
      cr=refprop9l.compute_rho_ph(p,h,res);
      cout << "[cr="<<cr<<"] rho_ph    = " << res << endl;
      cr=refprop9l.compute_w_ph(p,h,res);
      cout << "[cr="<<cr<<"] w_ph      = " << res << endl;
      cr=refprop9l.compute_u_ph(p,h,res);
      cout << "[cr="<<cr<<"] u_ph      = " << res << endl;
      cr=refprop9l.compute_cp_ph(p,h,res);
      cout << "[cr="<<cr<<"] cp_ph     = " << res << endl;
      cr=refprop9l.compute_cv_ph(p,h,res);
      cout << "[cr="<<cr<<"] cv_ph     = " << res << endl;
      cr=refprop9l.compute_mu_ph(p,h,res);
      cout << "[cr="<<cr<<"] mu_ph     = " << res << endl;
      cr=refprop9l.compute_lambda_ph(p,h,res);
      cout << "[cr="<<cr<<"] lambda_ph = " << res << endl;
      cr=refprop9l.compute_sigma_ph(p,h,res);
      cout << "[cr="<<cr<<"] sigma_ph  = " << res << endl;
      cr=refprop9l.compute_beta_ph(p,t,res);
      cout << "[cr="<<cr<<"] beta_ph   = " << res << endl;
      cr=refprop9l.compute_d_rho_d_p_h_ph(p,h,res);
      cout << "[cr="<<cr<<"] drhodp_ph = " << res << endl;
      cr=refprop9l.compute_d_rho_d_h_p_ph(p,h,res);
      cout << "[cr="<<cr<<"] drhodh_ph = " << res << endl;
      cr=refprop9l.compute_d_T_d_h_p_ph(p,h,res);
      cout << "[cr="<<cr<<"] dTdh_ph   = " << res << endl;
      cr=refprop9l.compute_g_ph(p,h,res);
      cout << "[cr="<<cr<<"] g_ph      = " << res << endl;
      cr=refprop9l.compute_f_ph(p,h,res);
      cout << "[cr="<<cr<<"] f_ph      = " << res << endl;
      cr=refprop9l.compute_pr_ph(p,h,res);
      cout << "[cr="<<cr<<"] pr_ph      = " << res << endl<<endl;

      //  properties=f(ph)
      ArrOfDouble xhin(n);
      ArrOfDouble xtout(n);

      for(int i=0; i<n; i++)
        xhin[i]=h;
      xhin[0]=h;
      xhin[1]=h+100000.;
      xhin[2]=h+200000.;

      EOS_Field hin("enthalpie","h",NEPTUNE::h,xhin);

      int nbfields_ph=16;
      EOS_Fields outph(nbfields_ph);
      outph[0]=EOS_Field("temperature","T",NEPTUNE::T,xtout);
      outph[1]=EOS_Field("rho","rho",NEPTUNE::rho,xrho);
      outph[2]=EOS_Field("w","w",NEPTUNE::w,xw);
      outph[3]=EOS_Field("u","u",NEPTUNE::u,xu);
      outph[4]=EOS_Field("cp","cp",NEPTUNE::cp,xcp);
      outph[5]=EOS_Field("mu","mu",NEPTUNE::mu,xmu);
      outph[6]=EOS_Field("lambda","lambda",NEPTUNE::lambda,xlambda);
      outph[7]=EOS_Field("sigma","sigma",NEPTUNE::sigma,xsigma);
      outph[8]=EOS_Field("drhodp","d_rho_d_p_h",NEPTUNE::d_rho_d_p_h,xdrhodp);
      outph[9]=EOS_Field("drhodh","d_rho_d_h_p",NEPTUNE::d_rho_d_p_h,xdrhodh);
      outph[10]=EOS_Field("dtdh","d_T_d_h_p",NEPTUNE::d_T_d_h_p,xdtdh);
      outph[11]=EOS_Field("g","g",NEPTUNE::g,xg);
      outph[12]=EOS_Field("f","f",NEPTUNE::f,xf);
      outph[13]=EOS_Field("pr","pr",NEPTUNE::pr,xpr);
      outph[14]=EOS_Field("cv","cv",NEPTUNE::cv,xcv);
      outph[15]=EOS_Field("beta","beta",NEPTUNE::beta,xbeta);

      cr=refprop9l.compute(P,hin,outph,err_f);
      cout << endl<< "field field fields [cr=" << cr <<"]"<< endl;
      for(int i=0; i<ncout; i++)
        {
          cout <<"in "<<P.get_property_title()<<" "<<P[i]<<endl;
          cout <<"in "<<hin.get_property_title()<<" "<<hin[i]<<endl;
          for(int j=0; j<nbfields_ph; j++)
            cout<<" out "<< outph[j].get_property_title()<<" "<<outph[j][i] << endl ;
        }

      cout << endl << endl;
          
      //valeurs limites
      cr=refprop9l.compute_h_l_lim_p(p,res);
      cout << "[cr="<<cr<<"] h_l_lim   = " << res << endl;
      cr=refprop9l.compute_h_v_lim_p(p,res);
      cout << "[cr="<<cr<<"] h_v_lim   = " << res << endl<<endl;
          
      //plan ps
      cr=refprop9l.compute_h_ps(p,s,res);
      cout << "[cr="<<cr<<"] h_ps      = " << res << endl;
          
      //  properties=f(ps)
      ArrOfDouble xsin(n);
      ArrOfDouble xhout(n);
          
      xsin[0]=s;
      xsin[1]=s+100.;
      xsin[2]=s+200.;

      EOS_Field sin("entropie","s",NEPTUNE::s,xsin);

      int nbfields_ps=1;
      EOS_Fields outps(nbfields_ps);
      outps[0]=EOS_Field("enthalpie","h",NEPTUNE::h,xhout);
          
      cr=refprop9l.compute(P,sin,outps,err_f);
      cout << endl<< "field field fields [cr=" << cr <<"]"<< endl;
      for(int i=0; i<ncout; i++)
        {
          cout <<"in "<<P.get_property_title()<<" "<<P[i]<<endl;
          cout <<"in "<<sin.get_property_title()<<" "<<sin[i]<<endl;
          for(int j=0; j<nbfields_ps; j++)
            cout<<" out "<< outps[j].get_property_title()<<" "<<outps[j][i] << endl ;
        }

      cout << endl << endl;

    }

    // Test 9-3: Refprop9-WaterLiquid
    {
      double p=1.e5;
      double t=350;
      double h=200.e3;
      double pcrit,tcrit,hcrit;
      double res;

      int n=3;
      int ncout=1; // (ncout<=n)

      ArrOfDouble xp(n);
      ArrOfDouble xh(n);
      ArrOfInt ierr(n);
      EOS_Field P("Pressure","p",NEPTUNE::p,xp);
      EOS_Field h_f("Enthalpy","h",NEPTUNE::h,xh);
      EOS_Error_Field err_f(ierr);

      ArrOfDouble xrholsat(n);
      ArrOfDouble xrhovsat(n);
      ArrOfDouble xhlsat(n);
      ArrOfDouble xhvsat(n);
      ArrOfDouble xtsat(n);
      ArrOfDouble xdtsatdp(n);

      ArrOfDouble xt(n);
      ArrOfDouble xrho(n);
      ArrOfDouble xu(n);
      ArrOfDouble xdtdh(n);
      ArrOfDouble xw(n);
      ArrOfDouble xs(n);
      ArrOfDouble xcp(n);
      ArrOfDouble xcv(n);
      ArrOfDouble xdrhodp(n);
      ArrOfDouble xdrhodh(n);
      ArrOfDouble xmu(n);
      ArrOfDouble xlambda(n);
      ArrOfDouble xsigma(n);
      ArrOfDouble xg(n);
      ArrOfDouble xf(n);
      ArrOfDouble xpr(n);
      ArrOfDouble xbeta(n);


      cout<<endl<<"--------------------------------------- "<<endl;
      cout<<      "------ Test 9-3: Refprop9 Vapor   ------ "<<endl<<endl;

      EOS refprop9v("EOS_Refprop9","WaterVapor");
      EOS_Error cr;

      cout << "**   eos " << endl;
      cout << "   * fluid   : " <<refprop9v.fluid_name()<<endl;
      cout << "   * table   : " <<refprop9v.table_name()<<endl;
      cout << "   * version : " <<refprop9v.version_name()<<endl<<endl;
      cout << refprop9v<<endl;

      //test_features(refprop9Eos);

      // Valeurs critiques (CRITP)
      cr = refprop9v.get_p_crit(pcrit);
      cr = refprop9v.get_T_crit(tcrit);
      cr = refprop9v.get_h_crit(hcrit);

      cout << "----- critp-------" << endl;
      cout << "Pc    : " << pcrit << endl;
      cout << "Tc    : " << tcrit << endl;
      cout << "hc    : " << hcrit << endl << endl << endl;

      //saturation
      cr=refprop9v.compute_rho_l_sat_p(p,res);
      cout << "[cr="<<cr<<"] rholsat_p = " << res << endl;
      cr=refprop9v.compute_rho_v_sat_p(p,res);
      cout << "[cr="<<cr<<"] rhovsat_p = " << res << endl;
      cr=refprop9v.compute_h_l_sat_p(p,res);
      cout << "[cr="<<cr<<"] hlsat_p   = " << res<< endl;
      cr=refprop9v.compute_h_v_sat_p(p,res);
      cout << "[cr="<<cr<<"] hvsat_p   = " << res << endl;
      cr=refprop9v.compute_T_sat_p(p,res);
      cout << "[cr="<<cr<<"] tsat_p    = " << res << endl;
      cr=refprop9v.compute_d_T_sat_d_p_p(p,res);
      cout << "[cr="<<cr<<"] dtsatdp   = " << res << endl;
      //futur  cr=refprop9v.compute_h_v_sat_T(t,res);
      //futur  cout << "[cr="<<cr<<"] hsat_T    = " << res << endl;
      cr=refprop9v.compute_p_sat_T(t,res);
      cout << "[cr="<<cr<<"] psat_T    = " << res << endl<<endl;

      //compute by field
      int nbfields=6;
      EOS_Fields outsat(nbfields);
      outsat[0]=EOS_Field("rholsat", "rho_l_sat",NEPTUNE::rho_l_sat,xrholsat);
      outsat[1]=EOS_Field("rhovsat", "rho_v_sat",NEPTUNE::rho_v_sat,xrhovsat);
      outsat[2]=EOS_Field("hlsat","h_l_sat",NEPTUNE::h_l_sat,xhlsat);
      outsat[3]=EOS_Field("hvsat","h_v_sat",NEPTUNE::h_v_sat,xhvsat);
      outsat[4]=EOS_Field("tsat","T_sat",NEPTUNE::T_sat,xtsat);
      outsat[5]=EOS_Field("dtsatdp","d_T_sat_d_p",NEPTUNE::d_T_sat_d_p,xdtsatdp);

      for(int i=0; i<n; i++)
        xp[i]=p;
      xp[0]=p;
      xp[1]=10.e5;
      xp[2]=70.e5;

      cr=refprop9v.compute(P,outsat,err_f);
      cout << endl << "field fields [cr=" << cr <<"]"<< endl;
      if (cr != good)
        {
          AString s;
          cout<<" err " <<cr << endl;
          for(int i=0; i<ncout; i++)
            {
              refprop9v.fluid().describe_error(err_f[i], s);
              cout<<err_f[i].generic_error()<<" "<<err_f[i].get_code()<<" "<<err_f[i].get_partial_code()<<" "<<s<<endl;
            }
        }
        
      for(int i=0; i<ncout; i++)
        {
          cout <<"in "<<P.get_property_title()<<" "<<P[i]<<endl;
          for(int j=0; j<nbfields; j++)
            cout<<" out "<< outsat[j].get_property_title()<<" "<<outsat[j][i] << endl ;
        }

      cout << endl << endl;

      //plan pT
      t=xtsat[0]+10.;
      cr=refprop9v.compute_rho_pT(p,t,res);
      cout << "[cr="<<cr<<"] rho_pT    = " << res << endl;
      cr=refprop9v.compute_h_pT(p,t,res);
      cout << "[cr="<<cr<<"] h_pT      = " << res << endl;
      cr=refprop9v.compute_d_h_d_T_p_pT(p,t,res);
      cout << "[cr="<<cr<<"] dhdT_pT   = " << res << endl;
      cr=refprop9v.compute_u_pT(p,t,res);
      cout << "[cr="<<cr<<"] u_pT      = " << res << endl;
      cr=refprop9v.compute_s_pT(p,t,res);
      cout << "[cr="<<cr<<"] s_pT      = " << res << endl;
      cr=refprop9v.compute_w_pT(p,t,res);
      cout << "[cr="<<cr<<"] w_pT      = " << res << endl;
      cr=refprop9v.compute_cp_pT(p,t,res);
      cout << "[cr="<<cr<<"] cp_pT     = " << res << endl;
      cr=refprop9v.compute_cv_pT(p,t,res);
      cout << "[cr="<<cr<<"] cv_pT     = " << res << endl;
      cr=refprop9v.compute_mu_pT(p,t,res);
      cout << "[cr="<<cr<<"] mu_pT     = " << res << endl;
      cr=refprop9v.compute_lambda_pT(p,t,res);
      cout << "[cr="<<cr<<"] lambda_pT = " << res << endl;
      cr=refprop9v.compute_sigma_pT(p,t,res);
      cout << "[cr="<<cr<<"] sigma_pT  = " << res << endl;
      cr=refprop9v.compute_beta_pT(p,t,res);
      cout << "[cr="<<cr<<"] beta_pT   = " << res << endl;
      cr=refprop9v.compute_g_pT(p,t,res);
      cout << "[cr="<<cr<<"] g_pT      = " << res << endl;
      cr=refprop9v.compute_f_pT(p,t,res);
      cout << "[cr="<<cr<<"] f_pT      = " << res << endl;
      cr=refprop9v.compute_pr_pT(p,t,res);
      cout << "[cr="<<cr<<"] pr_pT      = " << res << endl<<endl;


      // -- properties=f(pT)
      int nbfields_pT=1;
      EOS_Fields outpt(nbfields_pT);
      outpt[0]=EOS_Field("enthalpie","h",NEPTUNE::h,xh);

      EOS_Field T("Temperature","T",NEPTUNE::T,xt);
      xt=xtsat;
      xt+=10.;

      cr=refprop9v.compute(P,T,outpt,err_f);
      cout << endl<< "field field fields [cr=" << cr <<"]"<< endl;
      for(int i=0; i<ncout; i++)
        {
          cout <<"in "<<P.get_property_title()<<" "<<P[i]<<endl;
          cout <<"in "<<T.get_property_title()<<" "<<T[i]<<endl;
          for(int j=0; j<nbfields_pT; j++)
            cout<<" out "<< outpt[j].get_property_title()<<" "<<outpt[j][i] << endl ;

        }

      cout << endl << endl;

      h=xh[0];
      //plan ph
      cr=refprop9v.compute_T_ph(p,h,res);
      cout << "[cr="<<cr<<"] T_ph      = " << res << endl;
      cr=refprop9v.compute_rho_ph(p,h,res);
      cout << "[cr="<<cr<<"] rho_ph    = " << res << endl;
      cr=refprop9v.compute_w_ph(p,h,res);
      cout << "[cr="<<cr<<"] w_ph      = " << res << endl;
      cr=refprop9v.compute_u_ph(p,h,res);
      cout << "[cr="<<cr<<"] u_ph      = " << res << endl;
      cr=refprop9v.compute_cp_ph(p,h,res);
      cout << "[cr="<<cr<<"] cp_ph     = " << res << endl;
      cr=refprop9v.compute_cv_ph(p,h,res);
      cout << "[cr="<<cr<<"] cv_ph     = " << res << endl;
      cr=refprop9v.compute_mu_ph(p,h,res);
      cout << "[cr="<<cr<<"] mu_ph     = " << res << endl;
      cr=refprop9v.compute_lambda_ph(p,h,res);
      cout << "[cr="<<cr<<"] lambda_ph = " << res << endl;
      cr=refprop9v.compute_sigma_ph(p,h,res);
      cout << "[cr="<<cr<<"] sigma_ph  = " << res << endl;
      cr=refprop9v.compute_beta_ph(p,t,res);
      cout << "[cr="<<cr<<"] beta_ph   = " << res << endl;
      cr=refprop9v.compute_d_rho_d_p_h_ph(p,h,res);
      cout << "[cr="<<cr<<"] drhodp_ph = " << res << endl;
      cr=refprop9v.compute_d_rho_d_h_p_ph(p,h,res);
      cout << "[cr="<<cr<<"] drhodh_ph = " << res << endl;
      cr=refprop9v.compute_d_T_d_h_p_ph(p,h,res);
      cout << "[cr="<<cr<<"] dTdh_ph   = " << res << endl;
      cr=refprop9v.compute_g_ph(p,h,res);
      cout << "[cr="<<cr<<"] g_ph      = " << res << endl;
      cr=refprop9v.compute_f_ph(p,h,res);
      cout << "[cr="<<cr<<"] f_ph      = " << res << endl;
      cr=refprop9v.compute_pr_ph(p,h,res);
      cout << "[cr="<<cr<<"] pr_ph     = " << res << endl<<endl;

      //  properties=f(ph)
      ArrOfDouble xhin(n);
      ArrOfDouble xtout(n);

      for(int i=0; i<n; i++)
        xhin[i]=h;
      xhin[0]=h;
      xhin[1]=h+100000.;
      xhin[2]=h+200000.;

      EOS_Field hin("enthalpie","h",NEPTUNE::h,xhin);

      int nbfields_ph=16;
      EOS_Fields outph(nbfields_ph);
      outph[0]=EOS_Field("temperature","T",NEPTUNE::T,xtout);
      outph[1]=EOS_Field("rho","rho",NEPTUNE::rho,xrho);
      outph[2]=EOS_Field("w","w",NEPTUNE::w,xw);
      outph[3]=EOS_Field("u","u",NEPTUNE::u,xu);
      outph[4]=EOS_Field("cp","cp",NEPTUNE::cp,xcp);
      outph[5]=EOS_Field("mu","mu",NEPTUNE::mu,xmu);
      outph[6]=EOS_Field("lambda","lambda",NEPTUNE::lambda,xlambda);
      outph[7]=EOS_Field("sigma","sigma",NEPTUNE::sigma,xsigma);
      outph[8]=EOS_Field("drhodp","d_rho_d_p_h",NEPTUNE::d_rho_d_p_h,xdrhodp);
      outph[9]=EOS_Field("drhodh","d_rho_d_h_p",NEPTUNE::d_rho_d_h_p,xdrhodh);
      outph[10]=EOS_Field("dtdh","d_T_d_h_p",NEPTUNE::d_T_d_h_p,xdtdh);
      outph[11]=EOS_Field("g","g",NEPTUNE::g,xg);
      outph[12]=EOS_Field("f","f",NEPTUNE::f,xf);
      outph[13]=EOS_Field("pr","pr",NEPTUNE::pr,xpr);
      outph[14]=EOS_Field("cv","cv",NEPTUNE::cv,xcv);
      outph[15]=EOS_Field("beta","beta",NEPTUNE::beta,xbeta);

      cr=refprop9v.compute(P,hin,outph,err_f);
      cout << endl<< "field field fields [cr=" << cr <<"]" << endl;
      for(int i=0; i<ncout; i++)
        {
          cout <<"in "<<P.get_property_title()<<" "<<P[i]<<endl;
          cout <<"in "<<hin.get_property_title()<<" "<<hin[i]<<endl;
          for(int j=0; j<nbfields_ph; j++)
            cout<<" out "<< outph[j].get_property_title()<<" "<<outph[j][i] << endl ;
        }

      cout << endl << endl;

      //valeurs limites
      cr=refprop9v.compute_h_l_lim_p(p,res);
      cout << "[cr="<<cr<<"] h_l_lim   = " << res << endl;
      cr=refprop9v.compute_h_v_lim_p(p,res);
      cout << "[cr="<<cr<<"] h_v_lim   = " << res << endl;
          
      //plan ps
      cr=refprop9v.compute_h_ps(p,s,res);
      cout << "[cr="<<cr<<"] h_ps      = " << res << endl;
          
      //  properties=f(ps)
      ArrOfDouble xsin(n);
      ArrOfDouble xhout(n);
          
      xsin[0]=s;
      xsin[1]=s+100.;
      xsin[2]=s+200.;

      EOS_Field sin("entropie","s",NEPTUNE::s,xsin);

      int nbfields_ps=1;
      EOS_Fields outps(nbfields_ps);
      outps[0]=EOS_Field("enthalpie","h",NEPTUNE::h,xhout);
          
      cr=refprop9v.compute(P,sin,outps,err_f);
      cout << endl<< "field field fields [cr=" << cr <<"]"<< endl;
      for(int i=0; i<ncout; i++)
        {
          cout <<"in "<<P.get_property_title()<<" "<<P[i]<<endl;
          cout <<"in "<<sin.get_property_title()<<" "<<sin[i]<<endl;
          for(int j=0; j<nbfields_ps; j++)
            cout<<" out "<< outps[j].get_property_title()<<" "<<outps[j][i] << endl ;
        }

      cout << endl << endl;       
    }

    //Test 9-4 : Refprop9
    //Compute by Field : comparaison refprop9 with Cathare test (Test 3 : EOS fluid("EOS_Cathare","WaterLiquid"))
    {
      EOS refprop9("EOS_Refprop9","Water");
      double tref;
      tref=50.+273.15;

      int n=3;
      int ncout=1; // (ncout<=n)

      cout<<endl<<"--------------------------------------- "<<endl;
      cout<<      "------ Test 9-4 : Same tests as Cathare Water (Test 3)   ------ "<<endl<<endl;
      cout<<endl<<"--------------------------------------- "<<endl;

      cout << "**   eos " << endl;
      cout << refprop9<<endl;
      
      // -- saturation properties
      cout<<"----- sat property ----- "<<endl;

      ArrOfDouble xp(n);
      ArrOfDouble xh(n);
      ArrOfInt ierr(n);
      EOS_Field P("Pressure","p",NEPTUNE::p,xp);
      EOS_Field h_f("Enthalpy","h",NEPTUNE::h,xh);
      EOS_Error_Field err_f(ierr);

      ArrOfDouble xtsat(n);
      ArrOfDouble xdtsatdp(n);
      ArrOfDouble xhlsat(n);
      ArrOfDouble xhvsat(n);
      ArrOfDouble xdhlsatdp(n);
      ArrOfDouble xdhvsatdp(n);

      int nbfields=6;
      EOS_Fields outsat(nbfields);
      outsat[0]=EOS_Field("tsat","T_sat",NEPTUNE::T_sat,xtsat);
      outsat[1]=EOS_Field("dtsatdp","d_T_sat_d_p",NEPTUNE::d_T_sat_d_p,xdtsatdp);
      outsat[2]=EOS_Field("hlsat","h_l_sat",NEPTUNE::h_l_sat,xhlsat);
      outsat[3]=EOS_Field("hvsat","h_v_sat",NEPTUNE::h_v_sat,xhvsat);
      outsat[4]=EOS_Field("dhlsatdp","d_h_l_sat_d_p",NEPTUNE::d_h_l_sat_d_p,xdhlsatdp);
      outsat[5]=EOS_Field("dhvsatdp","d_h_v_sat_d_p",NEPTUNE::d_h_v_sat_d_p,xdhvsatdp);
      for(int i=0; i<n; i++)
        xp[i]=1.e5;
      xp[0]=1.e5;
      xp[1]=10.e5;
      xp[2]=70.e5;
      for(int i=0; i<n; i++)
        xh[i]=200000.;

      EOS_Error cr=refprop9.compute(P,outsat,err_f);
      cout << endl << "field fields [cr=" << cr <<"]"<< endl;
      if (cr != good)
        {
          AString s;
          cout<<" err " <<cr << endl;
          for(int i=0; i<ncout; i++)
            {
              refprop9.fluid().describe_error(err_f[i], s);
              cout<<err_f[i].generic_error()<<" "<<err_f[i].get_code()<<" "<<err_f[i].get_partial_code()<<" "<<s<<endl;
            }
        }

      for(int i=0; i<ncout; i++)
        {
          cout <<"in "<<P.get_property_title()<<" "<<P[i]<<endl;
          for(int j=0; j<nbfields; j++)
            cout<<" out "<< outsat[j].get_property_title()<<" "<<outsat[j][i] << endl ;
        }

      // -- properties=f(pT)
      cout<<endl<<"--- property (pT) -- "<<endl;

      ArrOfDouble xt(n);
      ArrOfDouble xhout(n);

      EOS_Field T("Temperature","T",NEPTUNE::T,xt);
      for(int i=0; i<n; i++)
        xt[i]=tref;
      xt[0]=tref;
      xt[1]=tref+20.;
      xt[2]=tref+40.;

      int nbfields_pT=1;
      EOS_Fields outpt(nbfields_pT);
      outpt[0]=EOS_Field("enthalpie","h",NEPTUNE::h,xhout);

      cr=refprop9.compute(P,T,outpt,err_f);
      cout << endl<< "field field fields [cr=" << cr <<"]"<< endl;
      for(int i=0; i<ncout; i++)
        {
          cout <<"in "<<P.get_property_title()<<" "<<P[i]<<endl;
          cout <<"in "<<T.get_property_title()<<" "<<T[i]<<endl;
          for(int j=0; j<nbfields_pT; j++)
            cout<<" out "<< outpt[j].get_property_title()<<" "<<outpt[j][i] << endl ;
        }

      //  properties=f(ph)
      cout<<endl<<"--- property (ph) -- "<<endl;

      ArrOfDouble xhin(n);
      ArrOfDouble xtout(n);
      xhin[0]=outpt[0][0];
      xhin[1]=outpt[0][1];
      xhin[2]=outpt[0][2];
      EOS_Field hin("enthalpie","h",NEPTUNE::h,xhin);

      ArrOfDouble xdtdp(n);
      ArrOfDouble xcp(n);
      ArrOfDouble xrho(n);
      ArrOfDouble xdrhodp(n);
      ArrOfDouble xdrhodh(n);
      ArrOfDouble xmu(n);

      int nbfields_ph=7;
      EOS_Fields outph(nbfields_ph);
      outph[0]=EOS_Field("temperature","T",NEPTUNE::T,xtout);
      outph[1]=EOS_Field("dtdp","d_T_d_p_h",NEPTUNE::d_T_d_p_h,xdtdp);
      outph[2]=EOS_Field("cp","cp",NEPTUNE::cp,xcp);
      outph[3]=EOS_Field("rho","rho",NEPTUNE::rho,xrho);
      outph[4]=EOS_Field("drhodp","d_rho_d_p_h",NEPTUNE::d_rho_d_p_h,xdrhodp);
      outph[5]=EOS_Field("drhodh","d_rho_d_h_p",NEPTUNE::d_rho_d_h_p,xdrhodh);
      outph[6]=EOS_Field("mu","mu",NEPTUNE::mu,xmu);

      cr=refprop9.compute(P,hin,outph,err_f);
      cout << endl<< "field field fields  [cr=" << cr <<"]"<< endl;
      for(int i=0; i<ncout; i++)
        {
          cout <<"in "<<P.get_property_title()<<" "<<P[i]<<endl;
          cout <<"in "<<hin.get_property_title()<<" "<<hin[i]<<endl;
          for(int j=0; j<nbfields_ph; j++)
            cout<<" out "<< outph[j].get_property_title()<<" "<<outph[j][i] << endl ;
        }
    }

    //Test 9-5 : Refprop9
    //Metastable state Nitrogen
    /*          For example, the saturation pressure for nitrogen at 100 K is 0.778 MPa.
                Inputs of 100 K and 0.777 MPa will return a vapor state, but inputs of '100' for temperature
                and '0.777>' for pressure will return a metastable liquid state.
    */
    {

      double p=777.;
      double t=100.;
      double pcrit,tcrit,hcrit;
      double res;
      EOS_Internal_Error err;
      AString description;

      cout<<endl<<"--------------------------------------- "<<endl;
      cout<<      "------ Test 9-5 : Refprop9 Metastable Nitrogen   ------ "<<endl<<endl;

      EOS refprop9N("EOS_Refprop9","Nitrogen");
      EOS refprop9NL("EOS_Refprop9","NitrogenLiquid");
      EOS refprop9NV("EOS_Refprop9","NitrogenVapor");
      EOS_Error cr;

      cout << "**   eos " << endl;
      cout<<refprop9N<<endl;

      // Valeurs critiques (CRITP)
      cr = refprop9N.get_p_crit(pcrit);
      cr = refprop9N.get_T_crit(tcrit);
      cr = refprop9N.get_h_crit(hcrit);

      cout << "----- critp-------" << endl;
      cout << "Pc    : " << pcrit << endl;
      cout << "Tc    : " << tcrit << endl;
      cout << "hc    : " << hcrit << endl << endl << endl;

      cr=refprop9N.compute_p_sat_T(t,res);
      cout << "saturation pressure 100K : " << res << endl;

      cr=refprop9N.compute_rho_pT(p,t,res);
      cout << "[cr="<<cr<<"] rho_pT (generic) : " << res << endl;
      cr=refprop9NL.compute_rho_pT(p,t,res);
      cout << "[cr="<<cr<<"] rho_pT (liquid) : " << res << endl;
      cr=refprop9NV.compute_rho_pT(p,t,res);
      cout << "[cr="<<cr<<"] rho_pT (vapor) : " << res << endl;

    }


    //Test 9-6 : Refprop9
    { cout<<endl<<"--------------------------------------- "<<endl;
      cout<<      "------ Test 9-6: Refprop9 Vapor   ------ "<<endl<<endl;

      const int n=62;
      int ncout=1; // (ncout<=n)
        
      double P_[n] = {
      5.696591e+06, 5.695415e+06, 5.694258e+06, 5.693141e+06, 
      5.692124e+06, 5.691260e+06, 5.690457e+06, 5.689646e+06, 
      5.688806e+06, 5.688007e+06, 5.687236e+06, 5.686351e+06, 
      5.685305e+06, 5.684128e+06, 5.682830e+06, 5.681442e+06, 
      5.680136e+06, 5.678919e+06, 5.677702e+06, 5.676533e+06, 
      5.675365e+06, 5.674148e+06, 5.672931e+06, 5.671624e+06, 
      5.670318e+06, 5.669170e+06, 5.668164e+06, 5.667231e+06, 
      5.666310e+06, 5.665416e+06, 5.664586e+06, 5.663868e+06, 
      5.663270e+06, 5.662771e+06, 5.662355e+06, 5.662009e+06, 
      5.661720e+06, 5.661480e+06, 5.661279e+06, 5.661112e+06, 
      5.660973e+06, 5.660857e+06, 5.660752e+06, 5.660648e+06, 
      5.660547e+06, 5.660457e+06, 5.660382e+06, 5.660313e+06, 
      5.660255e+06, 5.660208e+06, 5.660168e+06, 5.660134e+06, 
      5.660107e+06, 5.660084e+06, 5.660064e+06, 5.660048e+06, 
      5.660035e+06, 5.660023e+06, 5.660013e+06, 5.660004e+06, 
      5.696591e+06, 5.660000e+06};

      double H_[n] = {
      2.787777e+06, 2.787789e+06, 2.787801e+06, 2.787812e+06, 
      2.787823e+06, 2.787831e+06, 2.787839e+06, 2.787848e+06, 
      2.787856e+06, 2.787864e+06, 2.787872e+06, 2.787881e+06, 
      2.787892e+06, 2.787903e+06, 2.787917e+06, 2.787931e+06, 
      2.787944e+06, 2.787956e+06, 2.787968e+06, 2.787980e+06, 
      2.787992e+06, 2.788004e+06, 2.788017e+06, 2.788030e+06, 
      2.788043e+06, 2.788054e+06, 2.788065e+06, 2.788074e+06, 
      2.788083e+06, 2.788092e+06, 2.788101e+06, 2.788108e+06, 
      2.788114e+06, 2.788119e+06, 2.788123e+06, 2.788126e+06, 
      2.788129e+06, 2.788132e+06, 2.788134e+06, 2.788135e+06, 
      2.788137e+06, 2.788138e+06, 2.788139e+06, 2.788140e+06, 
      2.788141e+06, 2.788142e+06, 2.788143e+06, 2.788144e+06, 
      2.788144e+06, 2.788145e+06, 2.788145e+06, 2.788145e+06, 
      2.788146e+06, 2.788146e+06, 2.788146e+06, 2.788146e+06, 
      2.788146e+06, 2.788146e+06, 2.788146e+06, 2.788147e+06, 
      2.787777e+06, 2.788147e+06};

      double OUT0[n],OUT1[n],OUT2[n],OUT3[n],OUT4[n],OUT5[n],OUT6[n],
             OUT7[n],OUT8[n],OUT9[n],OUT10[n],OUT11[n],OUT12[n],OUT13[n],
             OUT14[n],OUT15[n],OUT16[n],OUT17[n];
      double OUT18[n],OUT19[n],OUT20[n],OUT21[n],OUT22[n],OUT23[n],
             OUT24[n],OUT25[n],OUT26[n];

      ArrOfDouble xp(1);
      ArrOfDouble xh(1);
      ArrOfDouble xt(1);
      ArrOfDouble drp(1);
      ArrOfDouble drt(1);
      ArrOfInt ierr(n);
      ArrOfInt ierr1(1);
      xp = 5.696591e+06;
      EOS_Field P("Pressure","p",NEPTUNE::p,xp);
      EOS_Field h_f("Enthalpy","h",NEPTUNE::h,xh);
      EOS_Error_Field err_f(ierr);
      EOS_Error_Field err_f1(ierr1);

      EOS_Field P2("Pvap","p",NEPTUNE::p,n,P_);
      EOS_Field H2("Hvap","h",NEPTUNE::h,n,H_);

      EOS refprop9v("EOS_Refprop9","WaterVapor"); 
      EOS_Error cr;

      cout << "**   eos " << endl;
      cout << refprop9v<<endl;


      { int nbfields_ph=18;
        EOS_Fields outph(nbfields_ph);
        
        //Propriétés gas
        for(int i=0; i<n; i++) OUT0[i]=545.0;
          outph[0]=EOS_Field("Tgaz","T",NEPTUNE::T,n,OUT0);
          outph[1]=EOS_Field("DTgaz/DPv", "d_T_d_p_h",NEPTUNE::d_T_d_p_h,n,OUT1);
          outph[2]=EOS_Field("DTgaz/DHvap","d_T_d_h_p",NEPTUNE::d_T_d_h_p,n,OUT2);
          outph[3]=EOS_Field("Cpvap","cp",NEPTUNE::cp,n,OUT3);
          outph[4]=EOS_Field("DCpvap/DPv","d_cp_d_p_h",NEPTUNE::d_cp_d_p_h,n,OUT4);
          outph[5]=EOS_Field("DCpvap/DHv","d_cp_d_h_p",NEPTUNE::d_cp_d_h_p,n,OUT5);
          outph[6]=EOS_Field("Rhovap","rho",NEPTUNE::rho,n,OUT6);
          outph[7]=EOS_Field("DRhovap/dPv","d_rho_d_p_h",NEPTUNE::d_rho_d_p_h,n,OUT7);
          outph[8]=EOS_Field("DRhovap/DHv","d_rho_d_h_p",NEPTUNE::d_rho_d_h_p,n,OUT8);

          outph[9]=EOS_Field("Svap","s",NEPTUNE::s,n,OUT9);
          outph[10]=EOS_Field("DSvap/dPv","d_s_d_p_h",NEPTUNE::d_s_d_p_h,n,OUT10);
          outph[11]=EOS_Field("DSvap/DHv","d_s_d_h_p",NEPTUNE::d_s_d_h_p,n,OUT11);
          outph[12]=EOS_Field("Wvap","w",NEPTUNE::w,n,OUT12);
          outph[13]=EOS_Field("DWvap/dPv","d_w_d_p_h",NEPTUNE::d_w_d_p_h,n,OUT13);
          outph[14]=EOS_Field("DWvap/DHv","d_w_d_h_p",NEPTUNE::d_w_d_h_p,n,OUT14);
          outph[15]=EOS_Field("Cvvap","cv",NEPTUNE::cv,n,OUT15);
          outph[16]=EOS_Field("DCvvap/DPv","d_cv_d_p_h",NEPTUNE::d_cv_d_p_h,n,OUT16);
          outph[17]=EOS_Field("DCvvap/DHv","d_cv_d_h_p",NEPTUNE::d_cv_d_h_p,n,OUT17);
      
        cr=refprop9v.compute(P2,H2,outph,err_f);
      
        cout << endl<< "field field fields [cr=" << cr <<"]" << endl;
        for(int i=0; i<ncout; i++)
        {
          cout <<"in "<<P2.get_property_title()<<" "<<P_[i]<<endl;
          cout <<"in "<<H2.get_property_title()<<" "<<H_[i]<<endl;
          for(int j=0; j<nbfields_ph; j++)
            cout<<" out "<< outph[j].get_property_title()<<" "<<outph[j][i] << endl ;
        }
      
        cout << endl << endl;
      
        nbfields_ph=9;
        EOS_Fields outtran(nbfields_ph);
        //Transport gas
        outtran[0]=EOS_Field("TLAG","lambda",NEPTUNE::lambda,n,OUT0);
        outtran[1]=EOS_Field("TLAG1","d_lambda_d_p_h",NEPTUNE::d_lambda_d_p_h,n,OUT1);
        outtran[2]=EOS_Field("TMUG","mu",NEPTUNE::mu,n,OUT2);
        outtran[3]=EOS_Field("TMUG1","d_mu_d_p_h",NEPTUNE::d_mu_d_p_h,n,OUT3);
        outtran[4]=EOS_Field("SI1","d_sigma_d_p_h",NEPTUNE::d_sigma_d_p_h,n,OUT4);
        outtran[5]=EOS_Field("SI3","d_sigma_d_h_p",NEPTUNE::d_sigma_d_h_p,n,OUT5);
        outtran[6]=EOS_Field("TLAG3","d_lambda_d_h_p",NEPTUNE::d_lambda_d_h_p,n,OUT6);
        outtran[7]=EOS_Field("TMUG3","d_mu_d_h_p",NEPTUNE::d_mu_d_h_p,n,OUT7);
        outtran[8]=EOS_Field("SI","sigma",NEPTUNE::sigma,n,OUT8);
      
        cr=refprop9v.compute(P2,H2,outtran,err_f);
      }

      { int nbfields_ph=27;
        EOS_Fields outph(nbfields_ph);
           
        //Propriétés gas
        for(int i=0; i<n; i++) OUT0[i]=545.0;
          outph[0]=EOS_Field("Tgaz","T",NEPTUNE::T,n,OUT0);
          outph[1]=EOS_Field("DTgaz/DPv", "d_T_d_p_h", NEPTUNE::d_T_d_p_h, n,OUT1);
          outph[2]=EOS_Field("DTgaz/DHvap","d_T_d_h_p", NEPTUNE::d_T_d_h_p, n,OUT2);
          outph[3]=EOS_Field("Cpvap","cp", NEPTUNE::cp, n,OUT3);
          outph[4]=EOS_Field("DCpvap/DPv","d_cp_d_p_h", NEPTUNE::d_cp_d_p_h, n,OUT4);
          outph[5]=EOS_Field("DCpvap/DHv","d_cp_d_h_p", NEPTUNE::d_cp_d_h_p, n,OUT5);
          outph[6]=EOS_Field("Rhovap","rho", NEPTUNE::rho, n,OUT6);
          outph[7]=EOS_Field("DRhovap/dPv","d_rho_d_p_h", NEPTUNE::d_rho_d_p_h, n,OUT7);
          outph[8]=EOS_Field("DRhovap/DHv","d_rho_d_h_p", NEPTUNE::d_rho_d_h_p, n,OUT8);
          outph[9]=EOS_Field("Svap","s", NEPTUNE::s, n,OUT9);
          outph[10]=EOS_Field("DSvap/dPv","d_s_d_p_h", NEPTUNE::d_s_d_p_h, n,OUT10);
          outph[11]=EOS_Field("DSvap/DHv","d_s_d_h_p", NEPTUNE::d_s_d_h_p, n,OUT11);
          outph[12]=EOS_Field("Wvap","w", NEPTUNE::w, n,OUT12);
          outph[13]=EOS_Field("DWvap/dPv","d_w_d_p_h", NEPTUNE::d_w_d_p_h, n,OUT13);
          outph[14]=EOS_Field("DWvap/DHv","d_w_d_h_p", NEPTUNE::d_w_d_h_p, n,OUT14);
          outph[15]=EOS_Field("Cvvap","cv", NEPTUNE::cv, n,OUT15);
          outph[16]=EOS_Field("DCvvap/DPv","d_cv_d_p_h", NEPTUNE::d_cv_d_p_h, n,OUT16);
          outph[17]=EOS_Field("DCvvap/DHv","d_cv_d_h_p", NEPTUNE::d_cv_d_h_p, n,OUT17);
          outph[18]=EOS_Field("TLAG","lambda", NEPTUNE::lambda, n,OUT18);
          outph[19]=EOS_Field("TLAG1","d_lambda_d_p_h", NEPTUNE::d_lambda_d_p_h, n,OUT19);
          outph[20]=EOS_Field("TMUG","mu", NEPTUNE::mu, n,OUT20);
          outph[21]=EOS_Field("TMUG1","d_mu_d_p_h", NEPTUNE::d_mu_d_p_h, n,OUT21);
          outph[22]=EOS_Field("SI1","d_sigma_d_p_h", NEPTUNE::d_sigma_d_p_h, n,OUT22);
          outph[23]=EOS_Field("SI3","d_sigma_d_h_p", NEPTUNE::d_sigma_d_h_p, n,OUT23);
          outph[24]=EOS_Field("TLAG3","d_lambda_d_h_p", NEPTUNE::d_lambda_d_h_p, n, OUT24);
          outph[25]=EOS_Field("TMUG3","d_mu_d_h_p", NEPTUNE::d_mu_d_h_p, n,OUT25);
          outph[26]=EOS_Field("SI","sigma", NEPTUNE::sigma, n,OUT26);

        cr=refprop9v.compute(P2,H2,outph,err_f);
           
        cout << endl<< "field field fields [cr=" << cr <<"]" << endl;
        for(int i=0; i<ncout; i++)
           { cout <<"in "<<P2.get_property_title()<<" "<<P_[i]<<endl;
             cout <<"in "<<H2.get_property_title()<<" "<<H_[i]<<endl;
             for(int j=0; j<nbfields_ph; j++)
                cout<<" out "<< outph[j].get_property_title()<<" "<<outph[j][i] << endl ;
           }
      
        cout << endl << endl;   
      }
        
      int nbfields_ph=15;
      EOS_Fields outph(nbfields_ph);
      outph[0]=EOS_Field("TSP", "T_sat", NEPTUNE::T_sat, n,OUT0);
      outph[1]=EOS_Field("DTSPP", "d_T_sat_d_p", NEPTUNE::d_T_sat_d_p, n,OUT1);
      outph[2]=EOS_Field("D2TSP","d2_T_sat_d_p_d_p", NEPTUNE::d2_T_sat_d_p_d_p, n,OUT2);
      outph[3]=EOS_Field("HLSP", "h_l_sat", NEPTUNE::h_l_sat, n,OUT3);
      outph[4]=EOS_Field("DHLSPPP", "d_h_l_sat_d_p", NEPTUNE::d_h_l_sat_d_p, n,OUT4);
      outph[5]=EOS_Field("HVSP", "h_v_sat", NEPTUNE::h_v_sat, n,OUT5);
      outph[6]=EOS_Field("DHVSPP", "d_h_v_sat_d_p", NEPTUNE::d_h_v_sat_d_p, n,OUT6);
      outph[7]=EOS_Field("CPLSP", "cp_l_sat", NEPTUNE::cp_l_sat, n,OUT7);
      outph[8]=EOS_Field("DCLSPP", "d_cp_l_sat_d_p", NEPTUNE::d_cp_l_sat_d_p, n,OUT8);
      outph[9]=EOS_Field("CPVSP", "cp_v_sat", NEPTUNE::cp_v_sat, n,OUT9);
      outph[10]=EOS_Field("DCVSPP", "d_cp_v_sat_d_p", NEPTUNE::d_cp_v_sat_d_p, n,OUT10);
      outph[11]=EOS_Field("RLSP", "rho_l_sat", NEPTUNE::rho_l_sat, n,OUT11);
      outph[12]=EOS_Field("DRLSPP", "d_rho_l_sat_d_p", NEPTUNE::d_rho_l_sat_d_p, n,OUT12);
      outph[13]=EOS_Field("RVSP", "rho_v_sat", NEPTUNE::rho_v_sat, n,OUT13);
      outph[14]=EOS_Field("DRVSPP", "d_rho_v_sat_d_p", NEPTUNE::d_rho_v_sat_d_p, n,OUT14);
            
      cr=refprop9v.compute(P2,outph,err_f);
        
      for(int i=0; i<ncout; i++)
         { cout <<"in "<<P2.get_property_title()<<" "<<P_[i]<<endl;
           for(int j=0; j<nbfields_ph; j++)
              cout<<" out "<< outph[j].get_property_title()<<" "<<outph[j][i] << endl ;
         }
    }


    //Test 9-7 : Refprop9-Air
    {

      // futur double p=777.;
      // futur double t=100.;
      double pcrit,tcrit,hcrit;
      //futur double res;
      EOS_Internal_Error err;
      AString description;

      cout<<endl<<endl;
      cout<< "----------------------------------------" <<endl;
      cout<< "------ Test 9-7 : Refprop9 Air ----------" <<endl<<endl;

      EOS refprop9A("EOS_Refprop9","Air");
      EOS refprop9AL("EOS_Refprop9","AirLiquid");
      EOS refprop9AV("EOS_Refprop9","AirVapor");
      // futur EOS_Error cr;

      cout << "**   eos " << endl;
      cout << "   * fluid   : " <<refprop9A.fluid_name()<<endl;

      // Valeurs critiques (CRITP)
      refprop9A.get_p_crit(pcrit);
      refprop9A.get_T_crit(tcrit);
      refprop9A.get_h_crit(hcrit);

      cout << "----- critp-------" << endl;
      cout << "Pc    : " << pcrit << endl;
      cout << "Tc    : " << tcrit << endl;
      cout << "hc    : " << hcrit << endl << endl << endl;

      //futur cr=refprop9A.compute_p_sat_T(t,res);
      //futur cout << "saturation pressure 100K : " << res << endl;

      //futur cr=refprop9A.compute_rho_pT(p,t,res);
      //futur cout << "[cr="<<cr<<"] rho_pT (generic) : " << res << endl;
      //futur cr=refprop9AL.compute_rho_pT(p,t,res);
      //futur cout << "[cr="<<cr<<"] rho_pT (liquid) : " << res << endl;
      //futur cr=refprop9AV.compute_rho_pT(p,t,res);
      //futur cout << "[cr="<<cr<<"] rho_pT (vapor) : " << res << endl;
    }




  }
#endif

   
  cout<<endl<<endl;
  cout<<"--------------------------------------- "<<endl;
  cout<<"---------------- End ------------------ "<<endl;
  cout<<"--------------------------------------- "<<endl;
  cout <<endl<<endl;
  // Fin de la mesure du temps
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = end - start;
  std::cout << "Temps écoulé : " << elapsed.count() << " secondes" << std::endl;
  Language_finalize();
  return 0;
}
