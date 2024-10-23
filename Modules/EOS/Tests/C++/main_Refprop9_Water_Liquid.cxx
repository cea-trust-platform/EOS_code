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
              EOS_Field field1(*ptr1, *ptr1, array1);
              EOS_Field field3(ptr_satp, ptr_satp, result);

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
       { const char ** ptr2 = list_prop2 ;
         while (**ptr2 != 0) 
            { for (int i=0; i<stz; i++)
                { ptr_thep = thermprop_r[i].c_str() ; 
                  EOS_Field field1(*ptr1, *ptr1, array1) ;
                  EOS_Field field2(*ptr2, *ptr2, array2) ;
                  EOS_Field field3(ptr_thep, ptr_thep, result) ;

                  array1[0] = 1.5e5 ;
                  array2[0] = 350.  ;
                  eos.compute(field1, field2, field3, err_field);
                  features_status(eos, err_field[0], *ptr1, *ptr2, ptr_thep);
                }
              ptr2++ ;
            }
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
{  
  auto start = std::chrono::high_resolution_clock::now();
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
        EOS_Error cr;
      double p=1.e5;
      double t=350;
      double h=200.e3;
      double pcrit,tcrit,hcrit/*,rhocrit*/;
      //double pmax,tmax,hmax,rhomax;
      //double tmin;
      double res/*, res2, res3*/;
            
      int n=3;
      int ncout=1; // (ncout<=n)
            
      ArrOfDouble xp(n);
      ArrOfDouble xh(n);
      ArrOfInt ierr(n);
      EOS_Field P("Pressure","p",xp);
      EOS_Field h_f("Enthalpy","h",xh);
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
      outsat[0]=EOS_Field("rholsat", "rho_l_sat",xrholsat);
      outsat[1]=EOS_Field("rhovsat", "rho_v_sat",xrhovsat);
      outsat[2]=EOS_Field("hlsat","h_l_sat",xhlsat);
      outsat[3]=EOS_Field("hvsat","h_v_sat",xhvsat);
      outsat[4]=EOS_Field("tsat","T_sat",xtsat);
      outsat[5]=EOS_Field("dtsatdp","d_T_sat_d_p",xdtsatdp);

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
      outpt[0]=EOS_Field("enthalpie","h",xh);

      EOS_Field T("Temperature","T",xt);
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

      EOS_Field hin("enthalpie","h",xhin);

      int nbfields_ph=16;
      EOS_Fields outph(nbfields_ph);
      outph[0]=EOS_Field("temperature","T",xtout);
      outph[1]=EOS_Field("rho","rho",xrho);
      outph[2]=EOS_Field("w","w",xw);
      outph[3]=EOS_Field("u","u",xu);
      outph[4]=EOS_Field("cp","cp",xcp);
      outph[5]=EOS_Field("mu","mu",xmu);
      outph[6]=EOS_Field("lambda","lambda",xlambda);
      outph[7]=EOS_Field("sigma","sigma",xsigma);
      outph[8]=EOS_Field("drhodp","d_rho_d_p_h",xdrhodp);
      outph[9]=EOS_Field("drhodh","d_rho_d_h_p",xdrhodh);
      outph[10]=EOS_Field("dtdh","d_T_d_h_p",xdtdh);
      outph[11]=EOS_Field("g","g",xg);
      outph[12]=EOS_Field("f","f",xf);
      outph[13]=EOS_Field("pr","pr",xpr);
      outph[14]=EOS_Field("cv","cv",xcv);
      outph[15]=EOS_Field("beta","beta",xbeta);

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

      EOS_Field sin("entropie","s",xsin);

      int nbfields_ps=1;
      EOS_Fields outps(nbfields_ps);
      outps[0]=EOS_Field("enthalpie","h",xhout);
          
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
      //double p=1.e5;
      //double t=350;
      //double h=200.e3;
      //double pcrit,tcrit,hcrit;
      //double res;

      int n=3;
      //int ncout=1; // (ncout<=n)

      ArrOfDouble xp(n);
      ArrOfDouble xh(n);
      ArrOfInt ierr(n);
      EOS_Field P("Pressure","p",xp);
      EOS_Field h_f("Enthalpy","h",xh);
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
