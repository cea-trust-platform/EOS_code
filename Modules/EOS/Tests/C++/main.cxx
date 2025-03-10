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



#include "EOS/API/EOS.hxx"
#include "EOS/API/EOS_Fields.hxx"
#include "EOS/API/EOS_Error_Field.hxx"
#include "EOS/API/EOS_Std_Error_Handler.hxx"
#include <stdio.h>

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
       {
        int i1=0;
        for (int i=0; i<stz; i++)
            { ptr_satp = saturprop_r[i].c_str() ; 
              EOS_Field field1(*ptr1, *ptr1, n_list_prop1[i1], array1); 
              EOS_Field field3(ptr_satp, ptr_satp, n_saturprop_r[i], result); 

              array1[0] = 1e5 ;
              // Test field function
              eos.compute(field1, field3, err_field);
              features_status(eos, err_field[0], *ptr1, "", ptr_satp);
            }
         ptr1++ ;
         i1++;
       }
  }
  // Test with two parameters:
  cout << "Testing field methods with 2 parameters" << endl;
  {
    const char *ptr_thep ;
    int stz = thermprop_r.size() ;
    const char ** ptr1 = list_prop1 ;
    while (**ptr1 != 0) 
       {
        int i1=0; 
        const char ** ptr2 = list_prop2 ;
         while (**ptr2 != 0) 
            { 
              int i2=0;
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
         ptr1++ ;
         i1++;
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
  
  cout<<endl<<endl;
  cout<<"--------------------------------------- "<<endl;
  cout<<"---------------- Begin ---------------- "<<endl;
  cout<<"--------------------------------------- "<<endl;

  Language_init();


  cout<<endl<<"--------------------------------------- "<<endl;
  cout<<      "------ Class available ...  ----------- "<<endl<<endl;

  hierrarchy();


  // Test 1 : compute by field or fields
#ifdef WITH_PLUGIN_CATHARE
  {
    cout<<endl<<endl;
    cout<< "------------------------------------------" <<endl;
    cout<< "------ Test 1 : compute by field(s) ------" <<endl<<endl;


    Strings args;
    //EOS fluid("Cathare_LiquidWater", args);
    EOS fluid("EOS_Cathare","WaterLiquid");
    //EOS fluid("Cathare_Steam", args);
    //EOS fluid("EOS_Cathare","WaterVapor");
    //EOS fluid("Ovap_LiquidWater", args);
    //EOS fluid("EOS_ThetisWaterLiquid", args);
    //EOS fluid("EOS_Thetis","WaterLiquid");

    cout << "**   eos" << endl;
    cout << "   * fluid   : " <<fluid.fluid_name()<<endl;
    cout << "   * table   : " <<fluid.table_name()<<endl;
    cout << "   * version : " <<fluid.version_name()<<endl<<endl;
    cout << fluid             <<endl<<endl;

    test_features(fluid);

    double tref;
    tref=50.+273.15; // for liquid
    //tref=150.+273.15;  // for steam

    int n=3;
    int ncout=n; // (ncout<=n)

    // -- saturation properties
    cout<<"----- sat property ----- "<<endl;

    ArrOfDouble xp(n);
    ArrOfInt ierr(n);
    EOS_Field P("Pressure","p",NEPTUNE::p,xp);
    EOS_Error_Field err(ierr);

    ArrOfDouble xtsat(n);
    ArrOfDouble xhlsat(n);
    ArrOfDouble xhvsat(n);
    ArrOfDouble xrholsat(n);
    ArrOfDouble xrhovsat(n);
    ArrOfDouble xdtsatdp(n);
    ArrOfDouble xdhlsatdp(n);
    ArrOfDouble xdhvsatdp(n);
    ArrOfDouble xdrholsatdp(n);
    ArrOfDouble xdrhovsatdp(n);
    
    int nbfields=10;
    EOS_Fields outsat(nbfields);
    outsat[0]=EOS_Field("tsat", "T_sat", NEPTUNE::T_sat, xtsat);
    outsat[1]=EOS_Field("hlsat", "h_l_sat", NEPTUNE::h_l_sat, xhlsat);
    outsat[2]=EOS_Field("hvat", "h_v_sat", NEPTUNE::h_v_sat, xhvsat);
    outsat[3]=EOS_Field("rholsat", "rho_l_sat", NEPTUNE::rho_l_sat, xrholsat);
    outsat[4]=EOS_Field("rhovat", "rho_v_sat", NEPTUNE::rho_v_sat, xrhovsat);
    outsat[5]=EOS_Field("dtsatdp", "d_T_sat_d_p", NEPTUNE::d_T_sat_d_p, xdtsatdp);
    outsat[6]=EOS_Field("dhlsatdp", "d_h_l_sat_d_p", NEPTUNE::d_h_l_sat_d_p, xdhlsatdp);
    outsat[7]=EOS_Field("dhvsatdp", "d_h_v_sat_d_p", NEPTUNE::d_h_v_sat_d_p, xdhvsatdp);
    outsat[8]=EOS_Field("drholsatdp", "d_rho_l_sat_d_p", NEPTUNE::d_rho_l_sat_d_p, xdrholsatdp);
    outsat[9]=EOS_Field("drhovsatdp", "d_rho_v_sat_d_p", NEPTUNE::d_rho_v_sat_d_p, xdrhovsatdp);
    
    for(int i=0; i<n; i++)
      xp[i]=1.e5;
    xp[0]=1.e5;
    xp[1]=10.e5;
    xp[2]=70.e5;

    EOS_Error cr=fluid.compute(P,outsat,err);
    cout << endl << "field fields sat [cr=" << cr <<"]"<< endl;
    if (cr != ok)
      {
        AString s;
        cout<<" err " <<cr << endl;
        for(int i=0; i<ncout; i++)
          {
            fluid.fluid().describe_error(err[i], s);
            cout<<err[i].generic_error()<<" "<<err[i].get_code()<<" "<<err[i].get_partial_code()<<" "<<s<<endl;
          }
      }
    for(int i=0; i<ncout; i++)
      {
        cout <<"in "<<P.get_property_title()<<" "<<P[i]<<endl;
        for(int j=0; j<nbfields; j++)
          cout<<" out "<< outsat[j].get_property_title()<<" "<<outsat[j][i] << endl ;
      }
    cout<<endl<<"--- compare compute by field versus compute by fields -- "<<endl;              
    for(int j=0; j<nbfields; j++)
      {
        ArrOfDouble xref(n);
        for(int i=0; i<n; i++)
          {
            xref[i]=outsat[j][i];
            outsat[j][i]=-123.456;
          }
        cr=fluid.compute(P,outsat[j],err);
        for(int i=0; i<ncout; i++)
          {
            cout << outsat[j].get_property_title() << " " << outsat[j][i] <<" [cr="<<cr<<"] (diff="<<outsat[j][i]-xref[i]<<")"<<endl;
            if (fabs(outsat[j][i]-xref[i])>=fmax(1.e-8,1.e-6*fabs(xref[i])) || outsat[j][i]*xref[i] < 0.e0)
              cout << "--> error "<<outsat[j].get_property_title()<<" values are different fields="<<xref[i]<<" field="<<outsat[j][i]<< endl;
          }
      } 
    
    // -- properties=f(pT)
    cout<<endl<<"--- property (pT) -- "<<endl;
    
    ArrOfDouble xt(n);
    ArrOfDouble xhout(n);
    ArrOfDouble xrho_pt(n);
    ArrOfDouble xcp_pt(n);
    ArrOfDouble xlambda_pt(n);
    ArrOfDouble xmu_pt(n);
    ArrOfDouble xw_pt(n);
    ArrOfDouble xdTdp_pt(n);
    ArrOfDouble xdTdh_pt(n);
    ArrOfDouble xdrhodp_pt(n);
    ArrOfDouble xdrhodh_pt(n);
    ArrOfDouble xdcpdp_pt(n);
    ArrOfDouble xdcpdh_pt(n);
    ArrOfDouble xdlambdadp_pt(n);
    ArrOfDouble xdlambdadh_pt(n);
    ArrOfDouble xdmudp_pt(n);
    ArrOfDouble xdmudh_pt(n);
    
    EOS_Field T("Temperature","T",NEPTUNE::T,xt);
    for(int i=0; i<n; i++)
      xt[i]=tref;
    xt[0]=tref;
    xt[1]=tref+20.;
    xt[2]=tref+40.;

    int nbfields_pT=16+nbfields;
    EOS_Fields outpt(nbfields_pT);
    outpt[0]=EOS_Field("enthalpie", "h", NEPTUNE::h, xhout);
    outpt[1]=EOS_Field("rho", "rho", NEPTUNE::rho, xrho_pt);
    outpt[2]=EOS_Field("cp", "cp", NEPTUNE::cp, xcp_pt);
    outpt[3]=EOS_Field("lambda", "lambda", NEPTUNE::lambda, xlambda_pt);
    outpt[4]=EOS_Field("mu", "mu", NEPTUNE::mu, xmu_pt);
    outpt[5]=EOS_Field("w", "w", NEPTUNE::w, xw_pt);
    outpt[6]=EOS_Field("dTdp", "d_T_d_p_h", NEPTUNE::d_T_d_p_h, xdTdp_pt);
    outpt[7]=EOS_Field("dTdh", "d_T_d_h_p", NEPTUNE::d_T_d_h_p, xdTdh_pt);
    outpt[8]=EOS_Field("drhodp", "d_rho_d_p_h", NEPTUNE::d_rho_d_p_h, xdrhodp_pt);
    outpt[9]=EOS_Field("drhodh", "d_rho_d_h_p", NEPTUNE::d_rho_d_h_p, xdrhodh_pt);
    outpt[10]=EOS_Field("dcpdp", "d_cp_d_p_h", NEPTUNE::d_cp_d_p_h, xdcpdp_pt);
    outpt[11]=EOS_Field("dcpdh", "d_cp_d_h_p", NEPTUNE::d_cp_d_h_p, xdcpdh_pt);
    outpt[12]=EOS_Field("dlambdadp", "d_lambda_d_p_h", NEPTUNE::d_lambda_d_p_h, xdlambdadp_pt);
    outpt[13]=EOS_Field("dlambdadh", "d_lambda_d_h_p", NEPTUNE::d_lambda_d_h_p, xdlambdadh_pt);
    outpt[14]=EOS_Field("dmudp", "d_mu_d_p_h", NEPTUNE::d_mu_d_p_h, xdmudp_pt);
    outpt[15]=EOS_Field("dmudh", "d_mu_d_h_p", NEPTUNE::d_mu_d_h_p, xdmudh_pt);
    for(int j=0; j<nbfields; j++)
      outpt[16+j]=outsat[j];
    
    cr=fluid.compute(P,T,outpt,err);
    cout << endl<< "field field fields [cr=" << cr <<"]"<< endl;
    for(int i=0; i<ncout; i++)
      {
        cout <<"in "<<P.get_property_title()<<" "<<P[i]<<endl;
        cout <<"in "<<T.get_property_title()<<" "<<T[i]<<endl;
        for(int j=0; j<nbfields_pT; j++)
          cout<<" out "<< outpt[j].get_property_title()<<" "<<outpt[j][i] << endl ;
      }
    
    cout<<endl<<"--- compare compute by field versus compute by fields -- "<<endl;              
    for(int j=0; j<nbfields_pT; j++)
      {
        ArrOfDouble xref(n);
        for(int i=0; i<n; i++)
          {
            xref[i]=outpt[j][i];
            outpt[j][i]=-123.456;
          }
        cr=fluid.compute(P,T,outpt[j],err);
        for(int i=0; i<ncout; i++)
          {
            cout << outpt[j].get_property_title() << " " << outpt[j][i] <<" [cr="<<cr<<"] (diff="<<outpt[j][i]-xref[i]<<")"<<endl;
            if (fabs(outpt[j][i]-xref[i])>=fmax(1.e-8,1.e-6*fabs(xref[i])) || outpt[j][i]*xref[i] < 0.e0)
              cout << "--> error "<<outpt[j].get_property_title()<<" values are different fields="<<xref[i]<<" field="<<outpt[j][i]<< endl;
          }
      } 
    
    
    //  properties=f(ph)
    cout<<endl<<"--- property (ph) -- "<<endl;
    ArrOfDouble xhin(n);
    for(int i=0; i<n; i++)
      xhin[i]=outpt[0][i];
    
    EOS_Field hin("enthalpie","h",NEPTUNE::h,xhin);
    ArrOfDouble xtout(n);
    ArrOfDouble xrho_ph(n);
    ArrOfDouble xcp_ph(n);
    ArrOfDouble xlambda_ph(n);
    ArrOfDouble xmu_ph(n);
    ArrOfDouble xw_ph(n);
    ArrOfDouble xdTdp_ph(n);
    ArrOfDouble xdTdh_ph(n);
    ArrOfDouble xdrhodp_ph(n);
    ArrOfDouble xdrhodh_ph(n);
    ArrOfDouble xdcpdp_ph(n);
    ArrOfDouble xdcpdh_ph(n);
    ArrOfDouble xdlambdadp_ph(n);
    ArrOfDouble xdlambdadh_ph(n);
    ArrOfDouble xdmudp_ph(n);
    ArrOfDouble xdmudh_ph(n);
    
    int nbfields_ph=16+nbfields;
    EOS_Fields outph(nbfields_ph);
    outph[0]=EOS_Field("temperature", "T", NEPTUNE::T, xtout);
    outph[1]=EOS_Field("rho", "rho", NEPTUNE::rho, xrho_ph);
    outph[2]=EOS_Field("cp", "cp", NEPTUNE::cp, xcp_ph);
    outph[3]=EOS_Field("lambda", "lambda", NEPTUNE::lambda, xlambda_ph);
    outph[4]=EOS_Field("mu", "mu", NEPTUNE::mu, xmu_ph);
    outph[5]=EOS_Field("w", "w", NEPTUNE::w, xw_ph);
    outph[6]=EOS_Field("dTdp", "d_T_d_p_h", NEPTUNE::d_T_d_p_h, xdTdp_ph);
    outph[7]=EOS_Field("dTdh", "d_T_d_h_p", NEPTUNE::d_T_d_h_p, xdTdh_ph);
    outph[8]=EOS_Field("drhodp", "d_rho_d_p_h", NEPTUNE::d_rho_d_p_h, xdrhodp_ph);
    outph[9]=EOS_Field("drhodh", "d_rho_d_h_p", NEPTUNE::d_rho_d_h_p, xdrhodh_ph);
    outph[10]=EOS_Field("dcpdp", "d_cp_d_p_h", NEPTUNE::d_cp_d_p_h, xdcpdp_ph);
    outph[11]=EOS_Field("dcpdh", "d_cp_d_h_p", NEPTUNE::d_cp_d_h_p, xdcpdh_ph);
    outph[12]=EOS_Field("dlambdadp", "d_lambda_d_p_h", NEPTUNE::d_lambda_d_p_h, xdlambdadp_ph);
    outph[13]=EOS_Field("dlambdadh", "d_lambda_d_h_p", NEPTUNE::d_lambda_d_h_p, xdlambdadh_ph);
    outph[14]=EOS_Field("dmudp", "d_mu_d_p_h", NEPTUNE::d_mu_d_p_h, xdmudp_ph);
    outph[15]=EOS_Field("dmudh", "d_mu_d_h_p", NEPTUNE::d_mu_d_h_p, xdmudh_ph);
    for(int j=0; j<nbfields; j++)
      outph[16+j]=outsat[j];
    
    cr=fluid.compute(P,hin,outph,err);
    cout << endl<< "field field fields [cr=" << cr <<"]"<< endl;
    for(int i=0; i<ncout; i++)
      {
        cout <<"in "<<P.get_property_title()<<" "<<P[i]<<endl;
        cout <<"in "<<hin.get_property_title()<<" "<<hin[i]<<endl;
        for(int j=0; j<nbfields_ph; j++)
          cout<<" out "<< outph[j].get_property_title()<<" "<<outph[j][i] << endl ;
      }
    
    cout<<endl<<"--- compare compute by field versus compute by fields -- "<<endl;              
    for(int j=0; j<nbfields_ph; j++)
      {
        ArrOfDouble xref(n);
        for(int i=0; i<n; i++)
          {
            xref[i]=outph[j][i];
            outph[j][i]=-123.456;
          }
        cr=fluid.compute(P,hin,outph[j],err);
        for(int i=0; i<ncout; i++)
          {
            cout << outph[j].get_property_title() << " " << outph[j][i] <<" [cr="<<cr<<"] (diff="<<outph[j][i]-xref[i]<<")"<<endl;
            if (fabs(outph[j][i]-xref[i])>=fmax(1.e-8,1.e-6*fabs(xref[i])) || outph[j][i]*xref[i] < 0.e0)
              cout << "-->error "<<outph[j].get_property_title()<<" values are different fields="<<xref[i]<<" field="<<outph[j][i]<< endl;
          }
      } 
    
    cout<<endl<<"--- compare (ph) versus (pT) -- "<<endl;               
    if (nbfields_ph == nbfields_pT)
      {
        for(int i=0; i<ncout; i++)
          {
            cout <<T.get_property_title()<< " Tin="<<T[i]<<" T(p,h)="<<outph[0][i]<<" (diff="<<T[i]-outph[0][i]<<")"<<endl;                 
            if (fabs(outph[0][i]-T[i])>=fmax(1.e-8,1.e-6*fabs(T[i])) || outph[0][i]*T[i] < 0.e0)
              cout << "--> error "<<T.get_property_title()<<" values are different Tin="<<T[i]<<" T(p,h)="<<outph[0][i]<< endl ;
          }
        for(int j=1; j<nbfields_ph; j++)
          {
            for(int i=0; i<ncout; i++)
              {
                cout <<outph[j].get_property_title()<< " ph="<<outph[j][i]<<" pT="<<outpt[j][i]<<" (diff="<<outph[j][i]-outpt[j][i]<<")"<<endl;
                if (fabs(outph[j][i]-outpt[j][i])>=fmax(1.e-8,1.e-6*fabs(outph[j][i])) || outph[j][i]*outpt[j][i] < 0.e0)
                  cout << "--> error "<<outph[j].get_property_title()<<" values are different ph="<<outph[j][i]<<" pT="<<outpt[j][i]<< endl;
              }
          }
      }
    
    //  other (cathare) properties=f(ph)
    cout<<endl<<"--- other property (ph) -- "<<endl;
    
    int nbfields_c=3;
    EOS_Fields outcph(nbfields_c);
    ArrOfDouble xlambda(n);
    ArrOfDouble xw(n);
    ArrOfDouble xdcdh(n);
    outcph[0]=EOS_Field("lambda","lambda",NEPTUNE::lambda,xlambda);
    outcph[1]=EOS_Field("vitesse","w",NEPTUNE::w,xw);
    //outcph[2]=EOS_Field("dcdh","d_w_d_h_p",xdcdh);
    outcph[2]=EOS_Field("vitesse","w",NEPTUNE::w,xdcdh);
    cr=fluid.compute(P,hin,outcph,err);
    cout << endl<< "field field fields [cr=" << cr <<"]"<< endl;
    for(int i=0; i<ncout; i++)
      {
        cout <<"in "<<P.get_property_title()<<" "<<P[i]<<endl;
        cout <<"in "<<hin.get_property_title()<<" "<<hin[i]<<endl;
        for(int j=0; j<nbfields_c; j++)
          cout<<" out "<< outcph[j].get_property_title()<<" "<<outcph[j][i] << endl ;
      }

  }
#endif

  // Test 2 : PerfectGas
  {
    cout<<endl<<endl;
    cout<<"--------------------------------------- "<<endl;
    cout<<"------ Test 2 ------------------------- "<<endl<<endl;

    Strings args2(4);
    args2[0]="Cp";
    args2[1]="0.029";
    args2[2]="Cv";
    args2[3]="0.02";
    //EOS gas("EOS_PerfectGas", args2);

    Strings argpg(1);
    //argpg[0]="Nitrogen";
    //argpg[0]="Oxygen";
    //argpg[0]="Hydrogen";
    argpg[0]="Argon";
    //argpg[0]="Helium";
    //EOS gas("EOS_PerfectGas", argpg);
    //EOS gas("EOS_PerfectGas", "Nitrogen");
    EOS gas("EOS_PerfectGas", "Air");

    cout << endl << "**   eos " << endl;
    cout << "   * fluid   : " <<gas.fluid_name()<<endl;
    cout << "   * table   : " <<gas.table_name()<<endl;
    cout << "   * version : " <<gas.version_name()<<endl<<endl;
    cout << gas               <<endl<<endl;

    test_features(gas);

    EOS_Error cr;
    double rho,drhodp,drhodh,mu,dmudh,lambda,dlambdadp,cp;
    cr=gas.compute_rho_ph(1.e5, 1.e6, rho);
    cout<<"[cr="<<cr<<"] rho : " << rho << endl;
    cr=gas.compute("rho", 1.e5, 1.e6, rho);
    cout<<"[cr="<<cr<<"] rho : " << rho << endl;
    cr=gas.compute("d_rho_d_p_h", 1.e5, 1.e6, drhodp);
    cout<<"[cr="<<cr<<"] d_rho_d_p_h : " << drhodp << endl;
    cr=gas.compute_d_rho_d_h_p_ph(1.e5, 1.e6, drhodh);
    cout<<"[cr="<<cr<<"] d_rho_d_h_p : " << drhodh << endl;
    cr=gas.compute_mu_ph(1.e5,1.e6,mu);
    cout<<"[cr="<<cr<<"] mu : " << mu << endl;
    cr=gas.compute_d_mu_d_h_p_ph(1.e5,1.e6,dmudh);
    cout<<"[cr="<<cr<<"] dmudh : " << dmudh << endl;
    cr=gas.compute_lambda_ph(1.e5,1.e6,lambda);
    cout<<"[cr="<<cr<<"] lambda : " << lambda << endl;
    cr=gas.compute_d_lambda_d_p_h_ph(1.e5,1.e6,dlambdadp);
    cout<<"[cr="<<cr<<"] dlambdadp : " << dlambdadp << endl;
    cr=gas.compute_cp_ph(1.e5,1.e6,cp);
    cout<<"[cr="<<cr<<"] cp : " << cp << endl;

    cout<<endl<<endl;
    cout<<"--------------------------------------- "<<endl;
    cout<<"------ Test 2.1 ----------------------- "<<endl<<endl;
    EOS air("PerfectGas", "Air") ;
    
    Strings args(2) ;
    args[0] = "SETREF" ;
    args[1] = "OTH"    ;
    double h0 = 0.e0 ;
    double s0 = 0.e0 ;
    double t0 = 298.15e0 ;
    double p0 = 1.015e5 ;
    EOS air_ref2("PerfectGas", "Air", args, h0, s0, t0, p0) ;
    
    EOS_Error cr1, cr2 ;
    double res1, res2 ;
    double press = 1.015e5 ;
    double tempf = 298.15e0 ;
    
    // h(P,T)
    cr1 = air.compute_h_pT(press, tempf, res1) ;
    cr2 = air_ref2.compute_h_pT(press, tempf, res2)      ;
    cout << endl ;
    cout << " in  P = " << press << endl ;
    cout << " in  T = " << tempf << endl ;
    cout << " [cr=" << cr1 << "] (default) h(P,T)         = " << res1 << endl ;
    cout << " [cr=" << cr2 << "] (ref2)    h(P,T)         = " << res2 << endl ;
    cout << "        delta_h = " << res2-res1 << endl ;

  }

#ifdef WITH_PLUGIN_THETIS
  // Test 3 : EOS_THETIS
  {
    cout<<endl<<endl;
    cout<<"--------------------------------------- "<<endl;
    cout<<"------ Test 3 ------------------------- "<<endl<<endl;


    // Testing PH
    Language_init();
    Strings args(2);

    args[0]= AString("");
    args[1]= AString("");

    //EOS eauL("EOS_ThetisWaterLiquid", args);
    EOS eauL("EOS_Thetis","WaterLiquid");
    EOS eauV("EOS_Thetis","WaterVapor");
    //EOS freonL("EOS_ThetisFreonR113Liquid", args);
    //EOS freonV("EOS_ThetisFreonR113Vapor", args);
    //EOS eauBMPL("EOS_ThetisWaterBMPLiquid", args);
    //EOS eauBMPV("EOS_ThetisWaterBMPVapor", args);

    EOS freonL("EOS_Thetis","R12Liquid");
    EOS freonV("EOS_Thetis","R12Vapor");
    //  EOS eauBMPL("EOS_Thetis","WaterLiquidBMP");
    // EOS eauBMPV("EOS_Thetis","WaterVaporBMP");

    cout << endl << "**   eos " << endl;
    cout << "   * fluid   : " <<eauL.fluid_name()<<endl;
    cout << "   * table   : " <<eauL.table_name()<<endl;
    cout << "   * version : " <<eauL.version_name()<<endl<<endl;
    cout << eauL<<endl;

    // BUG HERE: Thetis has errors in some properties
    test_features(eauL);

    cout << "**   eos " << endl;
    cout << "   * fluid   : " <<eauV.fluid_name()<<endl;
    cout << "   * table   : " <<eauV.table_name()<<endl;
    cout << "   * version : " <<eauV.version_name()<<endl<<endl;
    cout << eauV<<endl;

    // BUG HERE: Thetis has errors in some properties
    test_features(eauV);

    cout << "**   eos " << endl;
    cout << "   * fluid   : " <<freonL.fluid_name()<<endl;
    cout << "   * table   : " <<freonL.table_name()<<endl;
    cout << "   * version : " <<freonL.version_name()<<endl<<endl;
    cout << freonL<<endl;

    // BUG HERE: Thetis has errors in some properties
    test_features(freonL);

    cout << "**   eos " << endl;
    cout << "   * fluid   : " <<freonV.fluid_name()<<endl;
    cout << "   * table   : " <<freonV.table_name()<<endl;
    cout << "   * version : " <<freonV.version_name()<<endl<<endl;
    cout << freonV<<endl;

    // BUG HERE: Thetis has errors in some properties
    test_features(freonV);

    int im = 2;
    int jm = 2;

    double  kelvin = 273.15;
    double    p=70.e5;
    double    hh;

    ArrOfDouble xp(im*jm);
    ArrOfDouble xh(im*jm);
    ArrOfDouble xt(im*jm);
    ArrOfDouble xr(im*jm);
    ArrOfDouble xcp(im*jm);
    ArrOfDouble xl(im*jm);
    ArrOfDouble xmu(im*jm);
    ArrOfDouble xw(im*jm);
    ArrOfDouble xs(im*jm);

    ArrOfDouble xdTdp(im*jm);
    ArrOfDouble xdrhodp(im*jm);
    ArrOfDouble xdrhodh(im*jm);
    ArrOfDouble xdmudp(im*jm);
    ArrOfDouble xdmudh(im*jm);

    ArrOfInt xer(im*jm);

    EOS_Field P("Pressure", "p", NEPTUNE::p, xp);
    EOS_Field h("Enthalpy", "h", NEPTUNE::h, xh);
    EOS_Field T("Temperature", "T", NEPTUNE::T, xt);
    EOS_Field rho("Rho", "rho", NEPTUNE::rho, xr);
    EOS_Field cp("cp", "cp", NEPTUNE::cp, xcp);
    EOS_Field lambda("lambda", "lambda", NEPTUNE::lambda, xl);
    EOS_Field mu("mu", "mu", NEPTUNE::mu, xmu);
    EOS_Field w("w", "w", NEPTUNE::w, xw);
    EOS_Field sigma("sigma", "sigma", NEPTUNE::sigma, xs);

    EOS_Field dTdp("dTdp", "d_T_d_p_h", NEPTUNE::d_T_d_p_h, xdTdp);
    EOS_Field drhodp("drhodp", "d_rho_d_p_h", NEPTUNE::d_rho_d_p_h, xdrhodp);
    EOS_Field drhodh("drhodh", "d_rho_d_h_p", NEPTUNE::d_rho_d_h_p, xdrhodh);
    EOS_Field dmudp("dmudp", "d_mu_d_p_h", NEPTUNE::d_mu_d_p_h, xdmudp);
    EOS_Field dmudh("dmudh", "d_mu_d_h_p", NEPTUNE::d_mu_d_h_p, xdmudh);


    EOS_Error_Field err(xer);
    EOS_Error cr;

    // Compare with Thetis validation cases
    for (int i=0; i<im; i++)
      {
        p=p + 10.e5;
        hh = 9e5;
        for (int j=0; j<jm; j++) {
          hh = hh + 1.5e4;
          xp[i+im*j] =  p;
          xh[i+im*j] = hh;
        }
      }

    cr=eauL.compute(P, h, T,err);
    cout << "Temperature (Celcius) [cr=" << cr<<"]"<<endl;
    for (int i =0; i< im; i++)
      {
        xt[i] = xt[i] - kelvin;
        cout << xt[i] <<" ";
      }
    cout << endl;

    EOS_Fields output(7);
    output[0] = T;
    output[1] = rho;
    output[2] = cp;
    output[3] = lambda;
    output[4] = mu;
    output[5] = w;
    output[6] = sigma;

    EOS_Fields input(2);
    input[0] = P;
    input[1] = h;

    cr=eauL.compute(input, output,err);
    cout << "Properties [cr="<<cr<<"]" << endl;
    for (int i=0; i<im; i++)
      {
        for (int j=0; j<jm; j++)
          {
            printf("%14.5E %14.5E %14.5E %14.5E %14.5E %14.5E %14.5E %14.5E %14.5E \n",
                   xp[i+im*j],xh[i+im*j],xt[i+im*j],xr[i+im*j],xcp[i+im*j],xl[i+im*j],xmu[i+im*j],
                   xw[i+im*j],xs[i+im*j]);
          }
      }

    EOS_Fields output2(5);
    output2[0] = dTdp;
    output2[1] = drhodp;
    output2[2] = drhodh;
    output2[3] = dmudp;
    output2[4] = dmudh;

    cr=eauL.compute(input, output2,err);
    cout << "Der Properties [cr="<<cr<<"]" << endl;
    for (int i=0; i<im; i++)
      {
        for (int j=0; j<jm; j++)
          {
            printf("%14.5E %14.5E %14.5E %14.5E %14.5E %14.5E %14.5E %14.5E %14.5E \n",
                   xp[i+im*j],xh[i+im*j],xt[i+im*j],xr[i+im*j],xcp[i+im*j],xl[i+im*j],xmu[i+im*j],
                   xw[i+im*j],xs[i+im*j]);
          }
      }

    // Saturation

    ArrOfDouble xRhoLSat(im*jm);
    ArrOfDouble xRhoVSat(im*jm);
    ArrOfDouble xHLSat(im*jm);
    ArrOfDouble xHVSat(im*jm);
    ArrOfDouble xTSat(im*jm);

    EOS_Field RhoLSat("RhoLSat", "rho_l_sat", NEPTUNE::rho_l_sat, xRhoLSat);
    EOS_Field RhoVSat("RhoVSat", "rho_v_sat", NEPTUNE::rho_v_sat, xRhoVSat);
    EOS_Field HLSat("HLSat", "h_l_sat", NEPTUNE::h_l_sat, xHLSat);
    EOS_Field HVSat("HVSat", "h_v_sat", NEPTUNE::h_v_sat, xHVSat);
    EOS_Field TSat("TSat", "T_sat", NEPTUNE::T_sat, xTSat);

    EOS_Fields output3(5);

    output3[0] = RhoLSat;
    output3[1] = RhoVSat;
    output3[2] = HLSat;
    output3[3] = HVSat;
    output3[4] = TSat;

    cr=eauL.compute(P, output3,err);
    cout << "Sat Properties [cr="<<cr<<"]" << endl;
    for (int i=0; i<im; i++)
      {
        for (int j=0; j<jm; j++)
          {
            printf("%14.5E %14.5E %14.5E %14.5E %14.5E %14.5E\n",
                   xp[i+im*j], xRhoLSat[i+im*j],xRhoVSat[i+im*j],xHLSat[i+im*j],xHVSat[i+im*j],
                   TSat[i+im*j]);
          }
      }
    Language_finalize();
  }

  {
    // Testing PT
    Language_init();

    Strings args(2);
    args[0]= AString("");
    args[1]= AString("");

    EOS eauL("EOS_ThetisWaterLiquid", args);
    EOS eauV("EOS_ThetisWaterVapor", args);

    // BUG HERE: Thetis has errors in some properties
    test_features(eauL);
     test_features(eauV);

    int im = 2;
    int jm = 2;

    double  kelvin = 273.15;
    double    p=70.e5;
    double    t=200.+kelvin;

    ArrOfDouble xp(im*jm);
    ArrOfDouble xr(im*jm);
    ArrOfDouble xt(im*jm);
    ArrOfInt xer(im*jm);

    EOS_Field P("Pressure", "p", NEPTUNE::p, xp);
    EOS_Field h("Enthalpy", "h", NEPTUNE::h, xr);
    EOS_Field T("Temperature", "T", NEPTUNE::T, xt);
    EOS_Error_Field err(xer);
    EOS_Error cr;

    // Compare with Thetis validation cases
    for (int i=0; i<im; i++)
      {
        p=p + 10.e5;
        t=200.+kelvin;
        for (int j=0; j<jm; j++)
          {
            t=t + 10.;
            xp[i+im*j] =  p;
            xt[i+im*j] = t;
          }
      }

    cr=eauL.compute(P, T, h,err);
    cout << "Enthalpie PT [cr="<<cr<<"]" << endl;
    for (int i=0; i<im; i++)
      {
        for (int j=0; j<jm; j++)
          {
            printf("%14.5E %14.5E %14.5E\n",xp[i+im*j],xt[i+im*j],xr[i+im*j]);
          }
      }
    cout << endl << endl;
    Language_finalize();
  }

#endif

#ifdef WITH_PLUGIN_FLICA4

  // Test 4 : EOS_FLICA4
  {
    cout<<endl<<endl;
    cout<<"--------------------------------------- "<<endl;
    cout<<"------ Test 4 ------------------------- "<<endl<<endl;

    /*
      Strings args(2);
      AString filename("eau.140-210bar:800kJ.xdr");
      AString phasename("Liquid");
      args[0] = filename;
      args[1] = phasename;
      EOS liquid("EOS_FLICA4");
      liquid.init(args);

      phasename="Vapor";
      args[1] = phasename;
      EOS steam("EOS_FLICA4", args);

      Strings argsf2(1);
      argsf2[0] = filename;
      EOS eos("EOS_FLICA4", argsf2);
    */
    EOS liquid("EOS_Flica4","WaterLiquid.140-210");

    EOS steam("EOS_Flica4","WaterVapor.140-210");

    EOS eos("EOS_Flica4","Water.140-210");

    cout << endl << "**   eos " << endl;
    cout << "   * fluid   : " <<liquid.fluid_name()<<endl;
    cout << "   * table   : " <<liquid.table_name()<<endl;
    cout << "   * version : " <<liquid.version_name()<<endl<<endl;
    cout << liquid<<endl<<endl;

    test_features(liquid);

    cout << "**   eos " << endl;
    cout << "   * fluid   : " <<steam.fluid_name()<<endl;
    cout << "   * table   : " <<steam.table_name()<<endl;
    cout << "   * version : " <<steam.version_name()<<endl<<endl;
    cout << steam<<endl<<endl;

    test_features(steam);

    cout << "**   eos " << endl;
    cout << "   * fluid   : " <<eos.fluid_name()<<endl;
    cout << "   * table   : " <<eos.table_name()<<endl;
    cout << "   * version : " <<eos.version_name()<<endl<<endl;
    cout << eos<<endl<<endl;

    test_features(eos);


    EOS_Error cr;
    double rho, T, h, drhodp, drhodh, dTsat, val;
    double rho_l_sat ;

    cr=liquid.compute_rho_ph(1.35e7,700000,rho);
    cout << "[cr="<<cr<<"] rho        = " << rho << endl;
 
    cr=liquid.compute_T_ph(1.35e7,700000,T);
    cout << "[cr="<<cr<<"] T          = " << T << endl;

    cr=liquid.compute_h_pT(1.35e7,T,h);
    cout << "[cr="<<cr<<"] h          = " << h << endl;

    cr=liquid.compute_h_pT(1.35e7,T + 5.e-4,h);
    cout << "[cr="<<cr<<"] h          = " << h << endl;

    cr=liquid.compute_rho_ph(1.35e7,800000,rho);
    cout << "[cr="<<cr<<"] rho        = " << rho << endl;

    cr=liquid.compute_rho_ph(15.e6,1.2e6,rho);
    cout << "[cr="<<cr<<"] rho        = " << rho << endl;

    cr=liquid.compute_T_ph(15.e6,1.2e6,T);
    cout << "[cr="<<cr<<"] T          = " << T << endl;

    cr=liquid.compute_rho_l_sat_p(15.e6, rho_l_sat);
    cout << "[cr="<<cr<<"] rho_l_sat = " << rho_l_sat << endl;

    cr=steam.compute_rho_ph(15.e6,3.0e6,rho);
    cout << "[cr="<<cr<<"] rho        = " << rho << endl;

    cr=steam.compute("rho",15.e6,3.3e6,rho);
    cout << "[cr="<<cr<<"] rho        = " << rho << endl;

    cr=eos.compute_rho_ph(1.5e7,1.0e6,rho);
    cout << "[cr="<<cr<<"] rho        = " << rho << endl;

    cr=eos.compute_T_ph(1.5e7,1.0e6,T);
    cout << "[cr="<<cr<<"] T          = " << T << endl;

    cr=eos.compute_d_rho_d_p_h_ph(1.5e7,1.0e6,drhodp);
    cout << "[cr="<<cr<<"] d_rho_d_p  = " << drhodp << endl;

    cr=eos.compute_d_rho_d_h_p_ph(1.5e7,1.0e6,drhodh);
    cout << "[cr="<<cr<<"] d_rho_d_h  = " << drhodh << endl;

    cr=eos.compute_s_ph(1.5e7,1.0e6,val);
    cout << "[cr="<<cr<<"] s          = " << val << endl;

    cr=eos.compute_mu_ph(1.5e7,1.0e6,val);
    cout << "[cr="<<cr<<"] mu         = " <<  val << endl;

    cr=eos.compute_lambda_ph(1.5e7,1.0e6,val);
    cout << "[cr="<<cr<<"] lambda     = " <<val  << endl;

    cr=eos.compute_cp_ph(1.5e7,1.0e6,val);
    cout << "[cr="<<cr<<"] cp         = " << val << endl;

    cr=eos.compute_h_pT(1.5e7, T ,h);
    cout << "[cr="<<cr<<"] h= " << h << " verif pT" <<endl;

    cr=eos.compute_rho_pT(1.35e7,T,rho);
    cout << "[cr="<<cr<<"] rho        = " << rho << endl;

    cr=eos.compute_s_pT(1.5e7,T,val);
    cout << "[cr="<<cr<<"] s          = " << val << endl;

    cr=eos.compute_mu_pT(1.5e7,T,val);
    cout << "[cr="<<cr<<"] mu         = " <<  val << endl;

    cr=eos.compute_lambda_pT(1.5e7,T,val);
    cout << "[cr="<<cr<<"] lambda     = " << val << endl;

    val =0.0;
    cr=eos.compute_cp_pT(1.5e7,T,val);
    cout << "[cr="<<cr<<"] cp         = " << val << endl;

    cr=eos.compute("rho",15.e6,3.3e6,rho);
    cout << "[cr="<<cr<<"] rho        = " << rho << endl;

    cr=eos.compute_d_rho_d_p_h_ph(1.5e7,3.3e6,drhodp);
    cout << "[cr="<<cr<<"] d_rho_d_p  = " << drhodp << endl;

    cr=eos.compute_rho_ph(1.35e7,700000,rho);
    cout << "[cr="<<cr<<"] rho        = " << rho << endl;

    cr=eos.compute_T_ph(15.e6,3.0e6,T);
    cout << "[cr="<<cr<<"] T          = " << T << endl;

    cr=eos.compute_T_sat_p(15.e6,T);
    cout << "[cr="<<cr<<"] Tsat(p)    = " << T << endl;

    cr=eos.compute_d_T_sat_d_p_p(15.e6,dTsat);
    cout << "[cr="<<cr<<"] dTsat_dp   = " << dTsat << endl;

    cr=eos.compute_T_ph(1.35e7,7.0e5,T);
    cout << "[cr="<<cr<<"] T          = " << T << endl;

    cr=eos.compute_h_pT(1.35e7, T ,h);
    cout << "[cr="<<cr<<"] h= " << h <<  "(expected result 700000)" <<endl;

    rho =0.0;
    cr=eos.compute_rho_pT(1.35e7,T,rho);
    cout << "[cr="<<cr<<"] rho        = " << rho << endl;

    cr=eos.compute_h_v_sat_p(15.e6, h);
    cout << "[cr="<<cr<<"] hvsat      = " << h << endl;

    cr=eos.compute_rho_v_sat_p(15.e6, val);
    cout << "[cr="<<cr<<"] rhovsat    = " << val << endl;

    cr=eos.compute_rho_l_sat_p(15.e6, val);
    cout << "[cr="<<cr<<"] rholsat    = " << val << endl;

    cr=eos.compute_sigma_ph(15.e6, h, val);
    cout << "[cr="<<cr<<"] sigma      = " << val << endl;

    cr=eos.compute_T_ph(15.e6,h,T);
    cout << "[cr="<<cr<<"] T(p,hvsat) = " << T << endl;

    cr=eos.compute_h_l_sat_p(15.e6, h);
    cout << "[cr="<<cr<<"] hlsat      = " << h << endl;

    cr=eos.compute_T_ph(15.e6,h,T);
    cout << "[cr="<<cr<<"] T(p,hlsat) = " << T << endl;

  }

#endif // WITH_PLUGIN_FILCA4

  // Test 5 : EOS_StiffenedGas
  {
    cout<<endl<<endl;
    cout<<"--------------------------------------- "<<endl;
    cout<<"------ Test 5 ------------------------- "<<endl<<endl;

    Strings ref(1);
    ref[0]= AString("WaterLiquid");
    EOS liquid("EOS_StiffenedGas","WaterLiquid");
    ref[0]= AString("WaterVapor");
    EOS steam("EOS_StiffenedGas","WaterVapor");

    cout << endl << "**   eos " << endl;
    cout << "   * fluid   : " <<liquid.fluid_name()<<endl;
    cout << "   * table   : " <<liquid.table_name()<<endl;
    cout << "   * version : " <<liquid.version_name()<<endl<<endl;
    cout << liquid<<endl;
    test_features(liquid);

    cout << "**   eos " << endl;
    cout << "   * fluid   : " <<steam.fluid_name()<<endl;
    cout << "   * table   : " <<steam.table_name()<<endl;
    cout << "   * version : " <<steam.version_name()<<endl<<endl;
    cout << steam<<endl;
    test_features(steam);

    double p=7.152e5;
    double t=439.;
    double h=-1;
    double rho=-1.;
    double s=-1;
    double w=-1;
    double cp=-1;
    EOS_Error cr;

    // ref : 0
    cout << "Ref values 0"<< endl;
    t=298.;
    p=0.03166e5;
    cr=liquid.compute_h_pT(p,t,h);
    cout<<"[cr="<<cr<<"] in p "<<p<<" in T "<<t<<" out hl " <<h<<endl;
    cr=steam.compute_h_pT(p,t,h);
    cout<<"[cr="<<cr<<"] in p "<<p<<" in T "<<t<<" out hv " <<h<<endl;
    cr=steam.compute_rho_pT(p,t,rho);
    cout<<"[cr="<<cr<<"] in p "<<p<<" in T "<<t<<" out 1/rhov " <<1./rho<<endl<<endl;
    // ref : 1
    cout << "Ref values 1"<< endl;
    t=473.;
    p=15.551e5;
    cr=liquid.compute_h_pT(p,t,h);
    cout<<"[cr="<<cr<<"] in p "<<p<<" in T "<<t<<" out hl " <<h<<endl;
    cr=steam.compute_h_pT(p,t,h);
    cout<<"[cr="<<cr<<"] in p "<<p<<" in T "<<t<<" out hv " <<h<<endl;
    cr=steam.compute_rho_pT(p,t,rho);
    cout<<"[cr="<<cr<<"] in p "<<p<<" in T "<<t<<" out 1/rhov " <<1./rho<<endl<<endl;
    // ref : 2
    cout << "Ref values 2"<< endl;
    t=439.;
    p=7.152e5;
    cr=liquid.compute_rho_pT(p,t,rho);
    cout<<"[cr="<<cr<<"] in p "<<p<<" in T "<<t<<" out 1/rhol " <<1./rho<<endl<<endl;
    // ref : 3
    cout << "Ref values 3"<< endl;
    t=588.;
    p=105.3e5;
    cr=liquid.compute_rho_pT(p,t,rho);
    cout<<"[cr="<<cr<<"] in p "<<p<<" in T "<<t<<" out 1/rhol " <<1./rho<<endl<<endl;
    // ref : Tsat
    cout << "Ref values Tsat" << endl;
    p=3166.;
    cr=liquid.compute_T_sat_p(p, t);
    cout<<"[cr="<<cr<<"] in p "<<p<<"             out T_sat " <<t<<endl;
    p=1555100.;
    cr=liquid.compute_T_sat_p(p, t);
    cout<<"[cr="<<cr<<"] in p "<<p<<"             out T_sat " <<t<<endl;
    p=715200.;
    cr=liquid.compute_T_sat_p(p, t);
    cout<<"[cr="<<cr<<"] in p "<<p<<"             out T_sat " <<t<<endl;
    p=10530000.;
    cr=liquid.compute_T_sat_p(p, t);
    cout<<"[cr="<<cr<<"] in p "<<p<<"             out T_sat " <<t<<endl;

    // ref : Psat
    cout << "Ref values Psat" << endl;
    t=298.;
    t=283.712;
    cr=liquid.compute_p_sat_T(t, p);
    cout<<"[cr="<<cr<<"] in T "<<t<<"             out p_sat " <<p<<endl;
    t=473.;
    t=438.39;
    cr=liquid.compute_p_sat_T(t, p);
    cout<<"[cr="<<cr<<"] in T "<<t<<"             out p_sat " <<p<<endl;
    t=439.;
    t=407.865;
    cr=liquid.compute_p_sat_T(t, p);
    cout<<"[cr="<<cr<<"] in T "<<t<<"             out p_sat " <<p<<endl;
    t=588.;
    t=547.08;
    cr=liquid.compute_p_sat_T(t, p);
    cout<<"[cr="<<cr<<"] in T "<<t<<"             out p_sat " <<p<<endl<<endl;

    // other tests
    p=1.e5;
    t=50.+273.15;
    h=-1;
    rho=-1.;

    cr=liquid.compute_h_pT(p, t, h);
    cout<<"[cr="<<cr<<"] in p "<<p<<" in T "<<t<<" out h " <<h<<endl;
    t=-1.;
    cr=liquid.compute_T_sat_p(p, t);
    cout<<"[cr="<<cr<<"] in p "<<p<<"             out T_sat " <<t<<endl;
    t=-1;
    cr=liquid.compute_T_sat_p(2.e5, t);
    cout<<"[cr="<<cr<<"] in p "<<p<<"             out T_sat " <<t<<endl;
    cr=liquid.compute_T_ph(p, h, t);
    cout<<"[cr="<<cr<<"] in p "<<p<<" in h "<<h<<" out T " <<t<<endl;
    t=-1;
    cr=liquid.compute("d_T_sat_d_p",p, t);
    cout<<"[cr="<<cr<<"] in p "<<p<<"             out d_T_sat_d_p " <<t<<endl;
    cr=liquid.compute_rho_ph(p, h, rho);
    cout<<"[cr="<<cr<<"] in p "<<p<<" in h "<<h<<" out rho " <<rho<<endl;
    rho=-3.;
    cr=liquid.compute("d_rho_d_h_p", p, h, rho);
    cout<<"[cr="<<cr<<"] in p "<<p<<" in h "<<h<<" out d_rho_d_h " <<rho<<endl;
    cr=liquid.compute_s_ph(p, h, s);
    cout<<"[cr="<<cr<<"] in p "<<p<<" in h "<<h<<" out s " <<s<<endl;
    s=-1.;
    cr=liquid.compute_s_pT(p, t, s);
    cout<<"[cr="<<cr<<"] in p "<<p<<" in T "<<t<<" out s " <<s<<endl;
    s=-1.;
    cr=liquid.compute_cp_ph(p, h, cp);
    cout<<"[cr="<<cr<<"] in p "<<p<<" in h "<<h<<" out cp " <<cp<<endl;
    cp=-1.;
    cr=liquid.compute_w_ph(p, h, w);
    cout<<"[cr="<<cr<<"] in p "<<p<<" in h "<<h<<" out w " <<w<<endl;
    w=-1.;
  }


  // Test 6 : EOS_Mixing
#ifdef WITH_PLUGIN_CATHARE
  {

    cout<<endl<<endl;
    cout<<"--------------------------------------- "<<endl;
    cout<<"------ Test 6 : EOS_Mixing ------------ "<<endl<<endl;

    //EOS steam("Cathare_Steam");
    EOS steam("EOS_Cathare","WaterVapor");

    Strings argpg1(1);
    argpg1[0]="Nitrogen";
    // EOS C1("EOS_PerfectGas",argpg1);
    EOS C1("EOS_PerfectGas","Argon");

    Strings argpg2(1);
    argpg2[0]="Argon";
    //    EOS C2("EOS_PerfectGas",argpg2);
    EOS C2("EOS_PerfectGas","Nitrogen");

    EOS mixing("EOS_Mixing");
    int nbcomp=3;
    std::vector<EOS*> components(nbcomp);
    components[0]=&steam;
    components[1]=&C1;
    components[2]=&C2;
    mixing.set_components(components.data(), nbcomp);

    cout << "**   eos " << endl;
    cout << "   * fluid   : " <<mixing.fluid_name()<<endl;
    cout << "   * table   : " <<mixing.table_name()<<endl;
    cout << "   * version : " <<mixing.version_name()<<endl<<endl;
    cout << mixing<<endl;

    int n=3;
    int ncout=1; // (ncout<=n)

    EOS_Error cr;
    ArrOfInt ierr(n);
    EOS_Error_Field err(ierr);

    int ninput=2+nbcomp;
    ArrOfDouble xp(n);
    ArrOfDouble xh(n);
    ArrOfDouble xc0(n);
    ArrOfDouble xc(n);
    ArrOfDouble xc1(n);
    ArrOfDouble xc2(n);
    ArrOfDouble xT(n);

    EOS_Field pres("p", "p", NEPTUNE::p, xp);
    EOS_Field enthal("h", "h", NEPTUNE::h, xh);
    EOS_Field c0("c0", "c_0", NEPTUNE::c_0, xc0);
    EOS_Field c1("c1", "c_1", NEPTUNE::c_1, xc1);
    EOS_Field c2("c2", "c_2", NEPTUNE::c_2, xc2);
    EOS_Field T("T", "T", NEPTUNE::T, xT);

    EOS_Fields inph(ninput);
    inph[0]=pres;
    inph[1]=T;
    inph[2]=c0;
    inph[3]=c1;
    inph[4]=c2;

    for(int i=0; i<n; i++)
      {
        xp[i]=1.e5;
        xT[i]=400.;
        xc1[i]=0.1;
        xc2[i]=0.2;
        xc0[i]=1.-xc1[i]-xc2[i];
      }

    int noutph=2;
    ArrOfDouble xrhomix(n);
    EOS_Fields outph(noutph);
    outph[0]=enthal;
    outph[1]=EOS_Field("xrhomix","rho",NEPTUNE::rho,xrhomix);

    cr=mixing.compute(inph,outph,err);
    cout << endl<< "mixing fields fields [cr=" << cr <<"]"<< endl;

    for(int i=0; i<ncout; i++)
      {
        for(int j=0; j<noutph; j++)
          cout << outph[j].get_property_title() << " " << outph[j][i] <<endl;
        cout <<endl;
      }


    EOS_Fields input(ninput);
    input[0]=pres;
    input[1]=enthal;
    input[2]=c0;
    input[3]=c1;
    input[4]=c2;

    ArrOfDouble xTmix(n);
    ArrOfDouble xrho(n);
    ArrOfDouble xcp(n);
    ArrOfDouble xlambda(n);
    ArrOfDouble xmu(n);
                ArrOfDouble xsigma(n);
    ArrOfDouble xdTdp(n);
    ArrOfDouble xdTdh(n);
    ArrOfDouble xdrhodp(n);
    ArrOfDouble xdrhodh(n);
    ArrOfDouble xdcpdp(n);
    ArrOfDouble xdcpdh(n);
    ArrOfDouble xdlambdadp(n);
    ArrOfDouble xdlambdadh(n);
    ArrOfDouble xdmudp(n);
    ArrOfDouble xdmudh(n);
    ArrOfDouble xtsat(n);
    ArrOfDouble xhvsat(n);
                ArrOfDouble xrhovsat(n);
                ArrOfDouble xhlsat(n);
                ArrOfDouble xrholsat(n);
                ArrOfDouble xdtsatdp(n);
    ArrOfDouble xdhvsatdp(n);
                ArrOfDouble xdrhovsatdp(n);
                ArrOfDouble xdhlsatdp(n);
                ArrOfDouble xdrholsatdp(n);

    ArrOfDouble xdTdC1(n);
                ArrOfDouble xdTdC2(n);
                ArrOfDouble xdrhodC1(n);
                ArrOfDouble xdrhodC2(n);
                ArrOfDouble xdcpdC1(n);
                ArrOfDouble xdcpdC2(n);
                ArrOfDouble xdmudC1(n);
                ArrOfDouble xdmudC2(n);
                ArrOfDouble xdlambdadC1(n);
                ArrOfDouble xdlambdadC2(n);
                ArrOfDouble xdsigmadC1(n);
                ArrOfDouble xdsigmadC2(n);

                int noutput=38;
    EOS_Fields output(noutput);
    EOS_Field Tmix("Tmix", "T", NEPTUNE::T, xTmix);
    output[0] = Tmix;
    output[1] = EOS_Field("rho", "rho", NEPTUNE::rho, xrho);
    output[2] = EOS_Field("cp", "cp", NEPTUNE::cp, xcp);
    output[3] = EOS_Field("lambda", "lambda", NEPTUNE::lambda, xlambda);
    output[4] = EOS_Field("mu", "mu", NEPTUNE::mu, xmu);
    output[5] = EOS_Field("sigma", "sigma", NEPTUNE::sigma, xsigma);

    output[6] = EOS_Field("dTdp", "d_T_d_p_h", NEPTUNE::d_T_d_p_h, xdTdp);
    output[7] = EOS_Field("dTdh", "d_T_d_h_p", NEPTUNE::d_T_d_h_p, xdTdh);
    output[8] = EOS_Field("drhodp", "d_rho_d_p_h", NEPTUNE::d_rho_d_p_h, xdrhodp);
    output[9] = EOS_Field("drhodh", "d_rho_d_h_p", NEPTUNE::d_rho_d_h_p, xdrhodh);
    output[10] = EOS_Field("dcpdp", "d_cp_d_p_h", NEPTUNE::d_cp_d_p_h, xdcpdp);
    output[11] = EOS_Field("dcpdh", "d_cp_d_h_p", NEPTUNE::d_cp_d_h_p, xdcpdh);
    output[12] = EOS_Field("dlambdadp", "d_lambda_d_p_h", NEPTUNE::d_lambda_d_p_h, xdlambdadp);
    output[13] = EOS_Field("dlambdadh", "d_lambda_d_h_p", NEPTUNE::d_lambda_d_h_p, xdlambdadh);
    output[14] = EOS_Field("dmudp", "d_mu_d_p_h", NEPTUNE::d_mu_d_p_h, xdmudp);
    output[15] = EOS_Field("dmudh", "d_mu_d_h_p", NEPTUNE::d_mu_d_h_p, xdmudh);

    output[16] = EOS_Field("tsat", "T_sat", NEPTUNE::T_sat, xtsat);
    output[17] = EOS_Field("hvsat", "h_v_sat", NEPTUNE::h_v_sat, xhvsat);
    output[18] = EOS_Field("rhovsat", "rho_v_sat", NEPTUNE::rho_v_sat, xrhovsat);
    output[19] = EOS_Field("hlsat", "h_l_sat", NEPTUNE::h_l_sat, xhlsat);
    output[20] = EOS_Field("rholsat", "rho_l_sat", NEPTUNE::rho_l_sat, xrholsat);

    output[21] = EOS_Field("dtsatdp", "d_T_sat_d_p", NEPTUNE::d_T_sat_d_p, xdtsatdp);
    output[22] = EOS_Field("dhvsatdp", "d_h_v_sat_d_p", NEPTUNE::d_h_v_sat_d_p, xdhvsatdp);
    output[23] = EOS_Field("drhovsatdp", "d_rho_v_sat_d_p", NEPTUNE::d_rho_v_sat_d_p, xdrhovsatdp);
    output[24] = EOS_Field("dhlsatdp", "d_h_l_sat_d_p", NEPTUNE::d_h_l_sat_d_p, xdhlsatdp);
    output[25] = EOS_Field("drholsatdp", "d_rho_l_sat_d_p", NEPTUNE::d_rho_l_sat_d_p, xdrholsatdp);

    output[26] = EOS_Field("dTdC1", "d_T_d_c_1_ph", NEPTUNE::d_T_d_c_1_ph, xdTdC1);
    output[27] = EOS_Field("dTdC2", "d_T_d_c_2_ph", NEPTUNE::d_T_d_c_2_ph, xdTdC2);
    output[28] = EOS_Field("drhodC1", "d_rho_d_c_1_ph", NEPTUNE::d_rho_d_c_1_ph, xdrhodC1);
    output[29] = EOS_Field("drhodC2", "d_rho_d_c_2_ph", NEPTUNE::d_rho_d_c_2_ph, xdrhodC2);
    output[30] = EOS_Field("dcpdC1", "d_cp_d_c_1_ph", NEPTUNE::d_cp_d_c_1_ph, xdcpdC1);
    output[31] = EOS_Field("dcpdC2", "d_cp_d_c_2_ph", NEPTUNE::d_cp_d_c_2_ph, xdcpdC2);
    output[32] = EOS_Field("dmudC1", "d_mu_d_c_1_ph", NEPTUNE::d_mu_d_c_1_ph, xdmudC1);
    output[33] = EOS_Field("dmudC2", "d_mu_d_c_2_ph", NEPTUNE::d_mu_d_c_2_ph, xdmudC2);
    output[34] = EOS_Field("dlambdadC1", "d_lambda_d_c_1_ph", NEPTUNE::d_lambda_d_c_1_ph, xdlambdadC1);
    output[35] = EOS_Field("dlambdadC2", "d_lambda_d_c_2_ph", NEPTUNE::d_lambda_d_c_2_ph, xdlambdadC2);
    output[36] = EOS_Field("dsigmadC1", "d_sigma_d_c_1_ph", NEPTUNE::d_sigma_d_c_1_ph, xdsigmadC1);
    output[37] = EOS_Field("dsigmadC2", "d_sigma_d_c_2_ph", NEPTUNE::d_sigma_d_c_2_ph, xdsigmadC2);


    cr=mixing.compute(input,output,err);
    cout << endl<< "mixing fields fields [cr=" << cr <<"]"<< endl;

    for(int i=0; i<ncout; i++)
      {
        for(int j=0; j<noutput; j++)
          cout << output[j].get_property_title() << " " << output[j][i] <<endl;
        cout <<endl;
      }

                cout<<endl<<"--- compare compute by field versus compute by fields -- "<<endl;          
                for(int j=0; j<noutput; j++)
                  {
                    ArrOfDouble xref(n);
                    for(int i=0; i<n; i++)
                      {
                        xref[i]=output[j][i];
                        output[j][i]=-123.456;
                      }
                    cr=mixing.compute(input,output[j],err);
                    for(int i=0; i<ncout; i++)
                      {
                        cout << output[j].get_property_title() << " " << output[j][i] <<" [cr="<<cr<<"] (diff="<<output[j][i]-xref[i]<<")"<<endl;
                        if (fabs(output[j][i]-xref[i])>=fmax(1.e-8,1.e-6*fabs(xref[i])) || output[j][i]*xref[i] < 0.e0)
                          cout << "-->error "<<output[j].get_property_title()<<" values are different fields="<<xref[i]<<" field="<<output[j][i]<< endl;
        }
                  }

  }
#endif //WITH_PLUGIN_CATHARE
// le test precedent ne test pas specifiquement cathare il faudrait utiliser un autre modele
// dans le cas ou Cathare n est pas installe.
#if defined(WITH_PLUGIN_REFPROP_10)
#if defined(WITH_PLUGIN_CATHARE2)
  {
    cout<<endl<<endl;
    cout<<"--------------------------------------- "<<endl;
    cout<<"------ Test 6.1 : EOS_Mixing and setref refprop10 "<<endl<<endl;
    EOS_Error er ;
    const int NBMAXPR = 50;
    const int NBMAXPT = 50;
    double p_0, t_0, yv_0, h_0 ;
    double t_1, r_1, h_1, cp_1;
    double t_2, r_2, h_2, cp_2;
    double t_3, r_3, h_3, cp_3;
    double t_4, r_4, h_4, cp_4;
    double t_5, r_5, h_5, cp_5;
    p_0 = 1e5 ;
    t_0 = 393.15 ;
    h_0 = 2.72e6 ;
    yv_0 = 0.5 ;

    cout << "T_0 (K)  = " << std::scientific << std::setprecision(8) << t_0 << endl ;
    cout << "H_0 (J/kg)  = " << std::scientific << std::setprecision(8) << h_0 << endl ;
    cout << "P_0 (Pa) = " << std::scientific << std::setprecision(8) << p_0 << endl ;
    cout << "Yv_0     = " << std::scientific << std::setprecision(8) << yv_0 << endl ;

    int nbpt = 1 ;
    double **pt_in = NULL ;
    double **ph_in = NULL ;
    double **pt_out = NULL ;
    double **ph_out = NULL ;
    ph_in = new double*[NBMAXPR];
    pt_in = new double*[NBMAXPR];
    for (int i = 0; i < NBMAXPR; i++)
    {
       ph_in[i] = new double[NBMAXPT];
       pt_in[i] = new double[NBMAXPT];
       for (int j = 0; j < NBMAXPT; j++)
       {
          ph_in[i][j] = 0.0e0 ;
          pt_in[i][j] = 0.0e0 ;
       }
    }

    ph_out = new double*[NBMAXPR];
    pt_out = new double*[NBMAXPR];
    for (int i = 0; i < NBMAXPR; i++)
    {
       ph_out[i] = new double[NBMAXPT];
       pt_out[i] = new double[NBMAXPT];
       for (int j = 0; j < NBMAXPT; j++)
       {
          ph_out[i][j] = 0.0e0 ;
          pt_out[i][j] = 0.0e0 ;
       }
    }

    // Mixing Cathare2/PerfectGas
    EOS wvapor ("EOS_Cathare2","WaterVapor");
    EOS air ("EOS_perfectgas", "Air");

    // Mixing Cathare2/Cathare2
    EOS wvapor2 ("EOS_Cathare2","WaterVapor");
    EOS air2 ("EOS_Cathare2", "AirIncondensable");

    // Mixing Refprop/PerfectGas
    EOS wvapor3("EOS_Refprop10","WaterVapor");
    EOS air3 ("EOS_perfectgas", "Air");

    Strings args(2) ;
    args[0] = "SETREF" ;
    args[1] = "OTH" ;
    // Mixing Refprop/PerfectGas + SETREF isoC2
    double h0 = 2766.43e0 ;
    double s0 = 0.e0 ;
    double t0 = 438.08e0 ;
    double p0 = 7.e5 ;
    EOS wvapor4("EOS_Refprop10","WaterVapor", args, h0, s0, t0, p0);
    EOS air4 ("EOS_perfectgas", "Air");

    // Mixing Refprop/PerfectGas + SETREF 0
    h0 = 0.e0 ;
    s0 = 0.e0 ;
    t0 = 298.15e0 ;
    p0 = 1.015e5 ;
    EOS wvapor5("EOS_Refprop10","WaterVapor", args, h0, s0, t0, p0);
    EOS air5 ("EOS_perfectgas", "Air", args, h0, s0, t0, p0);

    EOS* components[2];
    components[0] = &wvapor;
    components[1] = &air;

    EOS mixing = EOS("EOS_Mixing");
    mixing.set_components(components, 2);
    cout << "**   eos" << endl;
    cout <<mixing<<endl;

    EOS* components3[2];
    components3[0] = &wvapor3;
    components3[1] = &air3;

    EOS mixing3 = EOS("EOS_Mixing");
    mixing3.set_components(components3, 2);
    cout << "**   eos" << endl;
    cout <<mixing3<<endl;

    EOS* components4[2];
    components4[0] = &wvapor4;
    components4[1] = &air4;

    EOS mixing4 = EOS("EOS_Mixing");
    mixing4.set_components(components4, 2);
    cout << "**   eos" << endl;
    cout <<mixing4<<endl;

    EOS* components5[2];
    components5[0] = &wvapor5;
    components5[1] = &air5;

    EOS mixing5 = EOS("EOS_Mixing");
    mixing5.set_components(components5, 2);
    cout << "**   eos" << endl;
    cout <<mixing5<<endl;

    EOS* components2[2];
    components2[0] = &wvapor2;
    components2[1] = &air2;

    EOS mixing2 = EOS("EOS_Mixing");
    mixing2.set_components(components2, 2);


    for (int i = 0; i < NBMAXPR; i++)
    {
       delete[] ph_in[i];
       delete[] pt_in[i];
       delete[] ph_out[i];
       delete[] pt_out[i];
    }
    double pressure[]  = { p_0 }  ;
    double enthalpy_in[] =  { h_0 } ;
    double temperature_in[] =  { t_0 } ;
    double mfrac_wvapor[]   = { yv_0 }  ;
    double mfrac_air[] =  { (1.0 - yv_0) } ;
    std::vector<double> temperature_out(nbpt) ;
    std::vector<double> enthalpy_out(nbpt) ;
    std::vector<double> density(nbpt) ;
    std::vector<double> cp(nbpt)  ;
    std::vector<int>    ierr(nbpt) ;

    pt_in[0]  = pressure ;
    pt_in[1]  = temperature_in ;
    pt_in[2]  = mfrac_wvapor ;
    pt_in[3]  = mfrac_air ;
    pt_out[0] = enthalpy_out.data() ;
    pt_out[1] = density.data() ;
    pt_out[2] = cp.data() ;

    ph_in[0]  = pressure ;
    ph_in[1]  = enthalpy_in ;
    ph_in[2]  = mfrac_wvapor ;
    ph_in[3]  = mfrac_air ;
    ph_out[0] = temperature_out.data() ;
    ph_out[1] = density.data() ;
    ph_out[2] = cp.data() ;

    EOS_Field v1_in1 ("in1",  "p", NEPTUNE::p,nbpt, pt_in[0]);
    EOS_Field v1_in2 ("in2",  "T", NEPTUNE::T,nbpt, pt_in[1]);
    EOS_Field v1_in3 ("in3",  "c_0", NEPTUNE::c_0,nbpt, pt_in[2]);
    EOS_Field v1_in4 ("in3",  "c_1", NEPTUNE::c_1,nbpt, pt_in[3]);
    EOS_Field v1_out1("out1", "h", NEPTUNE::h,nbpt, pt_out[0]);
    EOS_Field v1_out2("out2", "rho", NEPTUNE::rho,nbpt, pt_out[1]);
    EOS_Field v1_out3("out3", "cp", NEPTUNE::cp,nbpt, pt_out[2]);
    EOS_Error_Field v1_err(nbpt, ierr.data());

    EOS_Field v2_in1 ("in1",  "p", NEPTUNE::p,nbpt, ph_in[0]);
    EOS_Field v2_in2 ("in2",  "h", NEPTUNE::h,nbpt, ph_in[1]);
    EOS_Field v2_in3 ("in3",  "c_0", NEPTUNE::c_0,nbpt, ph_in[2]);
    EOS_Field v2_in4 ("in3",  "c_1", NEPTUNE::c_1,nbpt, ph_in[3]);
    EOS_Field v2_out1("out1", "T", NEPTUNE::T,nbpt, ph_out[0]);
    EOS_Field v2_out2("out2", "rho", NEPTUNE::rho,nbpt, ph_out[1]);
    EOS_Field v2_out3("out3", "cp", NEPTUNE::cp,nbpt, ph_out[2]);
    EOS_Error_Field v2_err(nbpt, ierr.data());

    EOS_Fields wp1_in(2) ;
    wp1_in[0]  = v1_in1  ;
    wp1_in[1]  = v1_in2  ;
    EOS_Fields w1_in(4) ;
    w1_in[0]  = v1_in1  ;
    w1_in[1]  = v1_in2  ;
    w1_in[2]  = v1_in3  ;
    w1_in[3]  = v1_in4  ;
    EOS_Fields w1_out(3);
    w1_out[0] = v1_out1 ;
    w1_out[1] = v1_out2 ;
    w1_out[2] = v1_out3 ;

    EOS_Fields wp2_in(2) ;
    wp2_in[0]  = v2_in1  ;
    wp2_in[1]  = v2_in2  ;
    EOS_Fields w2_in(4) ;
    w2_in[0]  = v2_in1  ;
    w2_in[1]  = v2_in2  ;
    w2_in[2]  = v2_in3  ;
    w2_in[3]  = v2_in4  ;
    EOS_Fields w2_out(3);
    w2_out[0] = v2_out1 ;
    w2_out[1] = v2_out2 ;
    w2_out[2] = v2_out3 ;

    cout << endl ;
    cout << "====== CASE 1 : Sans incondensable  =============" << endl ;

    // calcul h=f(p,T)
    pt_out[0][0] = 0.e0 ;
    pt_out[1][0] = 0.e0 ;
    pt_out[2][0] = 0.e0 ;
    er = wvapor.compute(wp1_in,w1_out,v1_err);

    h_1  = pt_out[0][0] ;
    r_1  = pt_out[1][0] ;
    cp_1 = pt_out[2][0] ;

    cout << "H= " << std::scientific << std::setprecision(8) << h_1 << endl ;
    cout << "rho= " << std::scientific << std::setprecision(8) << r_1 << endl ;
    cout << "cp= " << std::scientific << std::setprecision(8) << cp_1 << endl ;
    cout << endl;

    pt_out[0][0] = 0.e0 ;
    pt_out[1][0] = 0.e0 ;
    pt_out[2][0] = 0.e0 ;
    er = wvapor3.compute(wp1_in,w1_out,v1_err);

    h_3  = pt_out[0][0] ;
    r_3  = pt_out[1][0] ;
    cp_3 = pt_out[2][0] ;

    cout << "H3= " << std::scientific << std::setprecision(8) << h_3 << endl ;
    cout << "rho3= " << std::scientific << std::setprecision(8) << r_3 << endl ;
    cout << "cp3= " << std::scientific << std::setprecision(8) << cp_3 << endl ;
    cout << endl;

    pt_out[0][0] = 0.e0 ;
    pt_out[1][0] = 0.e0 ;
    pt_out[2][0] = 0.e0 ;
    er = wvapor4.compute(wp1_in,w1_out,v1_err);

    h_4  = pt_out[0][0] ;
    r_4  = pt_out[1][0] ;
    cp_4 = pt_out[2][0] ;

    cout << "H4= " << std::scientific << std::setprecision(8) << h_4 << endl ;
    cout << "rho4= " << std::scientific << std::setprecision(8) << r_4 << endl ;
    cout << "cp4= " << std::scientific << std::setprecision(8) << cp_4 << endl ;
    cout << endl;

    pt_out[0][0] = 0.e0 ;
    pt_out[1][0] = 0.e0 ;
    pt_out[2][0] = 0.e0 ;
    er = wvapor5.compute(wp1_in,w1_out,v1_err);

    h_5  = pt_out[0][0] ;
    r_5  = pt_out[1][0] ;
    cp_5 = pt_out[2][0] ;

    cout << "H5= " << std::scientific << std::setprecision(8) << h_5 << endl ;
    cout << "rho5= " << std::scientific << std::setprecision(8) << r_5 << endl ;
    cout << "cp5= " << std::scientific << std::setprecision(8) << cp_5 << endl ;
    cout << endl;

    // Verification fonctionnement SETREF : Attention ! avec methodes compute() ca ne marche pas bien car pas de callSetup systematique
    er = wvapor3.compute_h_pT(p_0, t_0, h_3) ;
    er = wvapor4.compute_h_pT(p_0, t_0, h_4) ;
    er = wvapor5.compute_h_pT(p_0, t_0, h_5) ;
    cout << "Verif SETREF : H3, H4, H5= " << std::scientific << std::setprecision(8) << h_3 << " " << h_4 << " " << h_5 << endl ;
    cout << endl ;

    // calcul T=f(p,H)
    ph_in[1][0] = h_1 ;
    ph_out[0][0] = 0.e0 ;
    ph_out[1][0] = 0.e0 ;
    ph_out[2][0] = 0.e0 ;
    er = wvapor.compute(wp2_in,w2_out,v2_err);

    t_1  = ph_out[0][0] ;
    r_1  = ph_out[1][0] ;
    cp_1 = ph_out[2][0] ;

    cout << "T= " << std::scientific << std::setprecision(8) << t_1 << endl ;
    cout << "rho= " << std::scientific << std::setprecision(8) << r_1 << endl ;
    cout << "cp= " << std::scientific << std::setprecision(8) << cp_1 << endl ;
    cout << endl;

    ph_in[1][0] = h_3 ;
    ph_out[0][0] = 0.e0 ;
    ph_out[1][0] = 0.e0 ;
    ph_out[2][0] = 0.e0 ;
    er = wvapor3.compute(wp2_in,w2_out,v2_err);

    t_3  = ph_out[0][0] ;
    r_3  = ph_out[1][0] ;
    cp_3 = ph_out[2][0] ;

    cout << "T3= " << std::scientific << std::setprecision(8) << t_3 << endl ;
    cout << "rho3= " << std::scientific << std::setprecision(8) << r_3 << endl ;
    cout << "cp3= " << std::scientific << std::setprecision(8) << cp_3 << endl ;
    cout << endl;

    ph_in[1][0] = h_4 ;
    ph_out[0][0] = 0.e0 ;
    ph_out[1][0] = 0.e0 ;
    ph_out[2][0] = 0.e0 ;
    er = wvapor4.compute(wp2_in,w2_out,v2_err);

    t_4  = ph_out[0][0] ;
    r_4  = ph_out[1][0] ;
    cp_4 = ph_out[2][0] ;

    cout << "T4= " << std::scientific << std::setprecision(8) << t_4 << endl ;
    cout << "rho4= " << std::scientific << std::setprecision(8) << r_4 << endl ;
    cout << "cp4= " << std::scientific << std::setprecision(8) << cp_4 << endl ;
    cout << endl;

    ph_in[1][0] = h_5 ;
    ph_out[0][0] = 0.e0 ;
    ph_out[1][0] = 0.e0 ;
    ph_out[2][0] = 0.e0 ;
    er = wvapor5.compute(wp2_in,w2_out,v2_err);

    t_5  = ph_out[0][0] ;
    r_5  = ph_out[1][0] ;
    cp_5 = ph_out[2][0] ;

    cout << "T5= " << std::scientific << std::setprecision(8) << t_5 << endl ;
    cout << "rho5= " << std::scientific << std::setprecision(8) << r_5 << endl ;
    cout << "cp5= " << std::scientific << std::setprecision(8) << cp_5 << endl ;
    cout << endl;

    // case 2 : Test de la consistance Air
    cout << endl ;
    cout << "====== CASE 2 : Avec incondensable =============" << endl ;

    // calcul H=f(p,T)
    pt_out[0][0] = 0.e0 ;
    pt_out[1][0] = 0.e0 ;
    pt_out[2][0] = 0.e0 ;
    er = mixing.compute(w1_in,w1_out,v2_err);

    h_1 = pt_out[0][0] ;
    r_1 = pt_out[1][0] ;
    cp_1 = pt_out[2][0] ;

    cout << "H= " << std::scientific << std::setprecision(8) << h_1 << endl ;
    cout << "rho= " << std::scientific << std::setprecision(8) << r_1 << endl ;
    cout << "cp= " << std::scientific << std::setprecision(8) << cp_1 << endl ;
    cout << endl;

    pt_out[0][0] = 0.e0 ;
    pt_out[1][0] = 0.e0 ;
    pt_out[2][0] = 0.e0 ;
    er = mixing2.compute(w1_in,w1_out,v2_err);

    h_2 = pt_out[0][0] ;
    r_2 = pt_out[1][0] ;
    cp_2 = pt_out[2][0] ;

    cout << "H2= " << std::scientific << std::setprecision(8) << h_2 << endl ;
    cout << "rho2= " << std::scientific << std::setprecision(8) << r_2 << endl ;
    cout << "cp2= " << std::scientific << std::setprecision(8) << cp_2 << endl ;
    cout << endl;

    pt_out[0][0] = 0.e0 ;
    pt_out[1][0] = 0.e0 ;
    pt_out[2][0] = 0.e0 ;
    er = mixing3.compute(w1_in,w1_out,v2_err);

    h_3 = pt_out[0][0] ;
    r_3 = pt_out[1][0] ;
    cp_3 = pt_out[2][0] ;

    cout << "H3= " << std::scientific << std::setprecision(8) << h_3 << endl ;
    cout << "rho3= " << std::scientific << std::setprecision(8) << r_3 << endl ;
    cout << "cp3= " << std::scientific << std::setprecision(8) << cp_3 << endl ;
    cout << endl;

    pt_out[0][0] = 0.e0 ;
    pt_out[1][0] = 0.e0 ;
    pt_out[2][0] = 0.e0 ;
    er = mixing4.compute(w1_in,w1_out,v2_err);

    h_4 = pt_out[0][0] ;
    r_4 = pt_out[1][0] ;
    cp_4 = pt_out[2][0] ;

    cout << "H4= " << std::scientific << std::setprecision(8) << h_4 << endl ;
    cout << "rho4= " << std::scientific << std::setprecision(8) << r_4 << endl ;
    cout << "cp4= " << std::scientific << std::setprecision(8) << cp_4 << endl ;
    cout << endl;

    pt_out[0][0] = 0.e0 ;
    pt_out[1][0] = 0.e0 ;
    pt_out[2][0] = 0.e0 ;
    er = mixing5.compute(w1_in,w1_out,v2_err);

    h_5 = pt_out[0][0] ;
    r_5 = pt_out[1][0] ;
    cp_5 = pt_out[2][0] ;

    cout << "H5= " << std::scientific << std::setprecision(8) << h_5 << endl ;
    cout << "rho5= " << std::scientific << std::setprecision(8) << r_5 << endl ;
    cout << "cp5= " << std::scientific << std::setprecision(8) << cp_5 << endl ;
    cout << endl;

    // calcul T=f(p,H)
    ph_in[1][0] = h_1 ;
    ph_out[0][0] = 0.e0 ;
    ph_out[1][0] = 0.e0 ;
    ph_out[2][0] = 0.e0 ;
    er = mixing.compute(w2_in,w2_out,v2_err);

    t_1 = ph_out[0][0] ;
    r_1 = ph_out[1][0] ;
    cp_1 = ph_out[2][0] ;

    cout << "T= " << std::scientific << std::setprecision(8) << t_1 << endl ;
    cout << "rho= " << std::scientific << std::setprecision(8) << r_1 << endl ;
    cout << "cp= " << std::scientific << std::setprecision(8) << cp_1 << endl ;
    cout << endl;

    ph_in[1][0] = h_2 ;
    ph_out[0][0] = 0.e0 ;
    ph_out[1][0] = 0.e0 ;
    ph_out[2][0] = 0.e0 ;
    er = mixing2.compute(w2_in,w2_out,v2_err);

    t_2 = ph_out[0][0] ;
    r_2 = ph_out[1][0] ;
    cp_2 = ph_out[2][0] ;

    cout << "T2= " << std::scientific << std::setprecision(8) << t_2 << endl ;
    cout << "rho2= " << std::scientific << std::setprecision(8) << r_2 << endl ;
    cout << "cp2= " << std::scientific << std::setprecision(8) << cp_2 << endl ;
    cout << endl;

    ph_in[1][0] = h_3 ;
    ph_out[0][0] = 0.e0 ;
    ph_out[1][0] = 0.e0 ;
    ph_out[2][0] = 0.e0 ;
    er = mixing3.compute(w2_in,w2_out,v2_err);

    t_3 = ph_out[0][0] ;
    r_3 = ph_out[1][0] ;
    cp_3 = ph_out[2][0] ;

    cout << "T3= " << std::scientific << std::setprecision(8) << t_3 << endl ;
    cout << "rho3= " << std::scientific << std::setprecision(8) << r_3 << endl ;
    cout << "cp3= " << std::scientific << std::setprecision(8) << cp_3 << endl ;
    cout << endl;

    ph_in[1][0] = h_4 ;
    ph_out[0][0] = 0.e0 ;
    ph_out[1][0] = 0.e0 ;
    ph_out[2][0] = 0.e0 ;
    er = mixing4.compute(w2_in,w2_out,v2_err);

    t_4 = ph_out[0][0] ;
    r_4 = ph_out[1][0] ;
    cp_4 = ph_out[2][0] ;

    cout << "T4= " << std::scientific << std::setprecision(8) << t_4 << endl ;
    cout << "rho4= " << std::scientific << std::setprecision(8) << r_4 << endl ;
    cout << "cp4= " << std::scientific << std::setprecision(8) << cp_4 << endl ;
    cout << endl;

    ph_in[1][0] = h_5 ;
    ph_out[0][0] = 0.e0 ;
    ph_out[1][0] = 0.e0 ;
    ph_out[2][0] = 0.e0 ;
    er = mixing5.compute(w2_in,w2_out,v2_err);

    t_5 = ph_out[0][0] ;
    r_5 = ph_out[1][0] ;
    cp_5 = ph_out[2][0] ;

    cout << "T5= " << std::scientific << std::setprecision(8) << t_5 << endl ;
    cout << "rho5= " << std::scientific << std::setprecision(8) << r_5 << endl ;
    cout << "cp5= " << std::scientific << std::setprecision(8) << cp_5 << endl ;
    cout << endl;


    delete[] ph_in ;
    delete[] pt_in ;
    delete[] ph_out ;
    delete[] pt_out ;
    (void) er;
  }
#endif
#endif
#ifdef WITH_PLUGIN_CATHARE
  // Test 7 : Freon R12 Cathare-like
  {
    cout<<endl<<endl;
    cout<<"--------------------------------------- "<<endl;
    cout<<"------ Test 7 : EOS_Cathare / FreonR12  "<<endl<<endl;


    //EOS fluid("EOS_Cathare","R12Liquid");
    EOS fluid("EOS_Cathare","R12Vapor");

    cout << "**   eos" << endl;
    cout << "   * fluid   : " <<fluid.fluid_name()<<endl;
    cout << "   * equa    : " <<fluid.equation_name()<<endl;
    cout << "   * table   : " <<fluid.table_name()<<endl;
    cout << "   * version : " <<fluid.version_name()<<endl<<endl;
    cout << fluid<<endl;

    test_features(fluid);

    double tref;
    if (fluid.equation_name() == "FreonR12Vapor")
      tref=20.+273.15;  // for vapor
    else
      tref=0.+273.15; // for liquid

    int n=3;
    int ncout=1; // (ncout<=n)


    // -- saturation properties
    cout<<"----- sat property ----- "<<endl;

    ArrOfDouble xp(n);
    ArrOfDouble xh(n);
    ArrOfInt ierr(n);
    EOS_Field P("Pressure","p", NEPTUNE::p,xp);
    EOS_Field h("Enthalpy","h", NEPTUNE::h,xh);
    EOS_Error_Field err(ierr);

    ArrOfDouble xtsat(n);
    ArrOfDouble xdtsatdp(n);
    ArrOfDouble xhlsat(n);
    ArrOfDouble xhvsat(n);
    ArrOfDouble xdhlsatdp(n);
    ArrOfDouble xdhvsatdp(n);
    ArrOfDouble xrholsat(n);
    ArrOfDouble xrhovsat(n);
    ArrOfDouble xdrholsatdp(n);
    ArrOfDouble xdrhovsatdp(n);

    int nbfields=10;
    EOS_Fields outsat(nbfields);
    outsat[0] = EOS_Field("tsat", "T_sat", NEPTUNE::T_sat, xtsat);
    outsat[1] = EOS_Field("dtsatdp", "d_T_sat_d_p", NEPTUNE::d_T_sat_d_p, xdtsatdp);
    outsat[2] = EOS_Field("hlsat", "h_l_sat", NEPTUNE::h_l_sat, xhlsat);
    outsat[3] = EOS_Field("hvat", "h_v_sat", NEPTUNE::h_v_sat, xhvsat);
    outsat[4] = EOS_Field("dhlsatdp", "d_h_l_sat_d_p", NEPTUNE::d_h_l_sat_d_p, xdhlsatdp);
    outsat[5] = EOS_Field("dhvsatdp", "d_h_v_sat_d_p", NEPTUNE::d_h_v_sat_d_p, xdhvsatdp);
    outsat[6] = EOS_Field("rholsat", "rho_l_sat", NEPTUNE::rho_l_sat, xrholsat);
    outsat[7] = EOS_Field("rhovsat", "rho_v_sat", NEPTUNE::rho_v_sat, xrhovsat);
    outsat[8] = EOS_Field("drholsatdp", "d_rho_l_sat_d_p", NEPTUNE::d_rho_l_sat_d_p, xdrholsatdp);
    outsat[9] = EOS_Field("drhovsatdp", "d_rho_v_sat_d_p", NEPTUNE::d_rho_v_sat_d_p, xdrhovsatdp);


    for(int i=0; i<n; i++)
      xp[i]=1.e5;
    xp[0]=1.e5;
    xp[1]=10.e5;
    xp[2]=20.e5;

    EOS_Error cr=fluid.compute(P,outsat,err);
    cout << endl<< "field fields [cr=" << cr <<"]"<< endl;

    for(int i=0; i<ncout; i++)
      {
        AString s;
        cout <<"in "<<P.get_property_title()<<" "<<P[i]<<endl;
        for(int j=0; j<nbfields; j++)
          {
            fluid.fluid().describe_error(err[i], s);
            cout<<" out "<< outsat[j].get_property_title()<<" "<<outsat[j][i] << endl ;
            cout<<" err "<<err[i].generic_error()<<" "<< err[i].get_partial_code()<<" "<<s<<endl ;
          }
      }

    ArrOfDouble xtsat2(n);
    EOS_Field Ts("tsat2","T_sat",NEPTUNE::T_sat,xtsat2);
    cr=fluid.compute(P,Ts,err);
    for(int i=0; i<ncout; i++)
      {
        AString s;
        cout <<"in "<<P.get_property_title()<<" "<<P[i]<<endl;
        fluid.fluid().describe_error(err[i], s);
        cout<<" out "<< Ts.get_property_title()<<" "<<Ts[i] << endl ;
        cout<<" err "<<err[i].generic_error()<<" "<< err[i].get_partial_code()<<" "<<s<<endl ;
      }



    // -- properties=f(pT)
    cout<<endl<<"--- property (pT) -- "<<endl;

    ArrOfDouble xt(n);
    ArrOfDouble xhout(n);
    ArrOfDouble xrho2(n);
    ArrOfDouble xdrhodp2(n);
    ArrOfDouble xdrhodh2(n);

    EOS_Field T("Temperature","T",NEPTUNE::T,xt);
    for(int i=0; i<n; i++)
      xt[i]=tref;
    xt[0]=tref;
    xt[1]=tref+50.;
    xt[2]=tref+51.;

    /*int nbfields_pT=4;
      EOS_Fields outpt(nbfields_pT);
      outpt[0]=EOS_Field("enthalpie","h",xhout);
      outpt[1]=EOS_Field("rho","rho",xrho2);
      outpt[2]=EOS_Field("drhodp","d_rho_d_p_h",xdrhodp2);
      outpt[3]=EOS_Field("drhodh","d_rho_d_h_p",xdrhodh2);*/

    int nbfields_pT=1;
    EOS_Fields outpt(nbfields_pT);
    outpt[0]=EOS_Field("enthalpie","h",NEPTUNE::h,xhout);

    cr=fluid.compute(P,T,outpt,err);
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
    ArrOfDouble xdtdh(n);
    ArrOfDouble xcp(n);
    ArrOfDouble xdcpdp(n);
    ArrOfDouble xdcpdh(n);
    ArrOfDouble xrho(n);
    ArrOfDouble xdrhodp(n);
    ArrOfDouble xdrhodh(n);
    ArrOfDouble xmu(n);
    ArrOfDouble xdmudp(n);
    ArrOfDouble xdmudh(n);
    ArrOfDouble xsigma(n);
    ArrOfDouble xw(n);
    ArrOfDouble xlambda(n);
    ArrOfDouble xdlambdadp(n);
    ArrOfDouble xdlambdadh(n);
    ArrOfDouble xnull(n);

    int nbfields_ph=17;
    EOS_Fields outph(nbfields_ph);
    outph[0] = EOS_Field("temperature", "T", NEPTUNE::T, xtout);
    outph[1] = EOS_Field("dtdp", "d_T_d_p_h", NEPTUNE::d_T_d_p_h, xdtdp);
    outph[2] = EOS_Field("dtdh", "d_T_d_h_p", NEPTUNE::d_T_d_h_p, xdtdh);
    outph[3] = EOS_Field("cp", "cp", NEPTUNE::cp, xcp);
    outph[4] = EOS_Field("dcpdp", "d_cp_d_p_h", NEPTUNE::d_cp_d_p_h, xdcpdp);
    outph[5] = EOS_Field("dcpdh", "d_cp_d_h_p", NEPTUNE::d_cp_d_h_p, xdcpdh);
    outph[6] = EOS_Field("rho", "rho", NEPTUNE::rho, xrho);
    outph[7] = EOS_Field("drhodp", "d_rho_d_p_h", NEPTUNE::d_rho_d_p_h, xdrhodp);
    outph[8] = EOS_Field("drhodh", "d_rho_d_h_p", NEPTUNE::d_rho_d_h_p, xdrhodh);
    outph[9] = EOS_Field("mu", "mu", NEPTUNE::mu, xmu);
    outph[10] = EOS_Field("dmudp", "d_mu_d_p_h", NEPTUNE::d_mu_d_p_h, xdmudp);
    outph[11] = EOS_Field("dmudh", "d_mu_d_h_p", NEPTUNE::d_mu_d_h_p, xdmudh);
    outph[12] = EOS_Field("w", "w", NEPTUNE::w, xw);
    outph[13] = EOS_Field("lambda", "lambda", NEPTUNE::lambda, xlambda);
    outph[14] = EOS_Field("dlambdadp", "d_lambda_d_p_h", NEPTUNE::d_lambda_d_p_h, xdlambdadp);

    if (fluid.equation_name() == "FreonR12Vapor") {
        outph[15] = EOS_Field("dlambdadh", "d_lambda_d_h_p", NEPTUNE::d_lambda_d_h_p, xdlambdadh);
        outph[16] = EOS_Field("sigma", "sigma", NEPTUNE::sigma, xsigma);
    } else {
        outph[15] = EOS_Field("null", "d_lambda_d_h", xnull); // This should cause an error, intentionally
        outph[16] = EOS_Field("dlambdadh", "d_lambda_d_h_p", NEPTUNE::d_lambda_d_h_p, xdlambdadh);
    }

    cr=fluid.compute(P,hin,outph,err);
    cout << endl<< "field field fields [cr=" << cr <<"]"<< endl;
    for(int i=0; i<ncout; i++)
      {
        cout <<"in "<<P.get_property_title()<<" "<<P[i]<<endl;
        cout <<"in "<<hin.get_property_title()<<" "<<hin[i]<<endl;
        for(int j=0; j<nbfields_ph; j++)
          cout<<" out "<< outph[j].get_property_title()<<" "<<outph[j][i] << endl ;
      }

  }
#endif //WITH_PLUGIN_CATHARE

  // Test 8 : Creation error

  {
    cout<<endl<<endl;
    cout<<"--------------------------------------- "<<endl;
    cout<<"------ Test 8 : Creation error ------- "<<endl<<endl;

    try {
      EOS dummy("foo","bar");
    }
    catch (EOS_Internal_Error& error) {
      cout << "Error when creating object: " << error.generic_error() << endl;
    }
    catch (std::bad_alloc &) {
      cout << "Error when creating object: Class not found." << endl;
    }
    catch (...) {
      cout << "Error when creating object: Class not found." << endl;
    } 
    cout<<endl << " This test checks the error message when creating object EOS with bad arguments." << endl;
    cout<<" The expected message is : Error when creating object: Class not found." << endl;
  }

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
      outsat[0] = EOS_Field("rholsat", "rho_l_sat", NEPTUNE::rho_l_sat, xrholsat);
      outsat[1] = EOS_Field("rhovsat", "rho_v_sat", NEPTUNE::rho_v_sat, xrhovsat);
      outsat[2] = EOS_Field("hlsat", "h_l_sat", NEPTUNE::h_l_sat, xhlsat);
      outsat[3] = EOS_Field("hvsat", "h_v_sat", NEPTUNE::h_v_sat, xhvsat);
      outsat[4] = EOS_Field("tsat", "T_sat", NEPTUNE::T_sat, xtsat);
      outsat[5] = EOS_Field("dtsatdp", "d_T_sat_d_p", NEPTUNE::d_T_sat_d_p, xdtsatdp);

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
      outph[0] = EOS_Field("temperature", "T", NEPTUNE::T, xtout);
      outph[1] = EOS_Field("rho", "rho", NEPTUNE::rho, xrho);
      outph[2] = EOS_Field("w", "w", NEPTUNE::w, xw);
      outph[3] = EOS_Field("u", "u", NEPTUNE::u, xu);
      outph[4] = EOS_Field("cp", "cp", NEPTUNE::cp, xcp);
      outph[5] = EOS_Field("mu", "mu", NEPTUNE::mu, xmu);
      outph[6] = EOS_Field("lambda", "lambda", NEPTUNE::lambda, xlambda);
      outph[7] = EOS_Field("sigma", "sigma", NEPTUNE::sigma, xsigma);
      outph[8] = EOS_Field("drhodp", "d_rho_d_p_h", NEPTUNE::d_rho_d_p_h, xdrhodp);
      outph[9] = EOS_Field("drhodh", "d_rho_d_h_p", NEPTUNE::d_rho_d_h_p, xdrhodh);
      outph[10] = EOS_Field("dtdh", "d_T_d_h_p", NEPTUNE::d_T_d_h_p, xdtdh);
      outph[11] = EOS_Field("g", "g", NEPTUNE::g, xg);
      outph[12] = EOS_Field("f", "f", NEPTUNE::f, xf);
      outph[13] = EOS_Field("pr", "pr", NEPTUNE::pr, xpr);
      outph[14] = EOS_Field("cv", "cv", NEPTUNE::cv, xcv);
      outph[15] = EOS_Field("beta", "beta", NEPTUNE::beta, xbeta);

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
      outsat[0] = EOS_Field("rholsat", "rho_l_sat", NEPTUNE::rho_l_sat, xrholsat);
      outsat[1] = EOS_Field("rhovsat", "rho_v_sat", NEPTUNE::rho_v_sat, xrhovsat);
      outsat[2] = EOS_Field("hlsat", "h_l_sat", NEPTUNE::h_l_sat, xhlsat);
      outsat[3] = EOS_Field("hvsat", "h_v_sat", NEPTUNE::h_v_sat, xhvsat);
      outsat[4] = EOS_Field("tsat", "T_sat", NEPTUNE::T_sat, xtsat);
      outsat[5] = EOS_Field("dtsatdp", "d_T_sat_d_p", NEPTUNE::d_T_sat_d_p, xdtsatdp);


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
      // Remplissage des champs avec l'énumération NEPTUNE
      outph[0] = EOS_Field("temperature", "T", NEPTUNE::T, xtout);
      outph[1] = EOS_Field("rho", "rho", NEPTUNE::rho, xrho);
      outph[2] = EOS_Field("w", "w", NEPTUNE::w, xw);
      outph[3] = EOS_Field("u", "u", NEPTUNE::u, xu);
      outph[4] = EOS_Field("cp", "cp", NEPTUNE::cp, xcp);
      outph[5] = EOS_Field("mu", "mu", NEPTUNE::mu, xmu);
      outph[6] = EOS_Field("lambda", "lambda", NEPTUNE::lambda, xlambda);
      outph[7] = EOS_Field("sigma", "sigma", NEPTUNE::sigma, xsigma);
      outph[8] = EOS_Field("drhodp", "d_rho_d_p_h", NEPTUNE::d_rho_d_p_h, xdrhodp);
      outph[9] = EOS_Field("drhodh", "d_rho_d_h_p", NEPTUNE::d_rho_d_h_p, xdrhodh);
      outph[10] = EOS_Field("dtdh", "d_T_d_h_p", NEPTUNE::d_T_d_h_p, xdtdh);
      outph[11] = EOS_Field("g", "g", NEPTUNE::g, xg);
      outph[12] = EOS_Field("f", "f", NEPTUNE::f, xf);
      outph[13] = EOS_Field("pr", "pr", NEPTUNE::pr, xpr);
      outph[14] = EOS_Field("cv", "cv", NEPTUNE::cv, xcv);
      outph[15] = EOS_Field("beta", "beta", NEPTUNE::beta, xbeta);

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
      outsat[0] = EOS_Field("rholsat", "rho_l_sat", NEPTUNE::rho_l_sat, xrholsat);
      outsat[1] = EOS_Field("rhovsat", "rho_v_sat", NEPTUNE::rho_v_sat, xrhovsat);
      outsat[2] = EOS_Field("hlsat", "h_l_sat", NEPTUNE::h_l_sat, xhlsat);
      outsat[3] = EOS_Field("hvsat", "h_v_sat", NEPTUNE::h_v_sat, xhvsat);
      outsat[4] = EOS_Field("tsat", "T_sat", NEPTUNE::T_sat, xtsat);
      outsat[5] = EOS_Field("dtsatdp", "d_T_sat_d_p", NEPTUNE::d_T_sat_d_p, xdtsatdp);


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
      outph[0] = EOS_Field("temperature", "T", NEPTUNE::T, xtout);
      outph[1] = EOS_Field("rho", "rho", NEPTUNE::rho, xrho);
      outph[2] = EOS_Field("w", "w", NEPTUNE::w, xw);
      outph[3] = EOS_Field("u", "u", NEPTUNE::u, xu);
      outph[4] = EOS_Field("cp", "cp", NEPTUNE::cp, xcp);
      outph[5] = EOS_Field("mu", "mu", NEPTUNE::mu, xmu);
      outph[6] = EOS_Field("lambda", "lambda", NEPTUNE::lambda, xlambda);
      outph[7] = EOS_Field("sigma", "sigma", NEPTUNE::sigma, xsigma);
      outph[8] = EOS_Field("drhodp", "d_rho_d_p_h", NEPTUNE::d_rho_d_p_h, xdrhodp);
      outph[9] = EOS_Field("drhodh", "d_rho_d_h_p", NEPTUNE::d_rho_d_h_p, xdrhodh);
      outph[10] = EOS_Field("dtdh", "d_T_d_h_p", NEPTUNE::d_T_d_h_p, xdtdh);
      outph[11] = EOS_Field("g", "g", NEPTUNE::g, xg);
      outph[12] = EOS_Field("f", "f", NEPTUNE::f, xf);
      outph[13] = EOS_Field("pr", "pr", NEPTUNE::pr, xpr);
      outph[14] = EOS_Field("cv", "cv", NEPTUNE::cv, xcv);
      outph[15] = EOS_Field("beta", "beta", NEPTUNE::beta, xbeta);

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
      outsat[0] = EOS_Field("tsat", "T_sat", NEPTUNE::T_sat, xtsat);
      outsat[1] = EOS_Field("dtsatdp", "d_T_sat_d_p", NEPTUNE::d_T_sat_d_p, xdtsatdp);
      outsat[2] = EOS_Field("hlsat", "h_l_sat", NEPTUNE::h_l_sat, xhlsat);
      outsat[3] = EOS_Field("hvsat", "h_v_sat", NEPTUNE::h_v_sat, xhvsat);
      outsat[4] = EOS_Field("dhlsatdp", "d_h_l_sat_d_p", NEPTUNE::d_h_l_sat_d_p, xdhlsatdp);
      outsat[5] = EOS_Field("dhvsatdp", "d_h_v_sat_d_p", NEPTUNE::d_h_v_sat_d_p, xdhvsatdp);

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
      outph[0] = EOS_Field("temperature", "T", NEPTUNE::T, xtout);
      outph[1] = EOS_Field("dtdp", "d_T_d_p_h", NEPTUNE::d_T_d_p_h, xdtdp);
      outph[2] = EOS_Field("cp", "cp", NEPTUNE::cp, xcp);
      outph[3] = EOS_Field("rho", "rho", NEPTUNE::rho, xrho);
      outph[4] = EOS_Field("drhodp", "d_rho_d_p_h", NEPTUNE::d_rho_d_p_h, xdrhodp);
      outph[5] = EOS_Field("drhodh", "d_rho_d_h_p", NEPTUNE::d_rho_d_h_p, xdrhodh);
      outph[6] = EOS_Field("mu", "mu", NEPTUNE::mu, xmu);


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
          outph[0] = EOS_Field("Tgaz", "T", NEPTUNE::T,n, OUT0);
          outph[1] = EOS_Field("DTgaz/DPv", "d_T_d_p_h", NEPTUNE::d_T_d_p_h,n, OUT1);
          outph[2] = EOS_Field("DTgaz/DHvap", "d_T_d_h_p", NEPTUNE::d_T_d_h_p,n, OUT2);
          outph[3] = EOS_Field("Cpvap", "cp", NEPTUNE::cp,n, OUT3);
          outph[4] = EOS_Field("DCpvap/DPv", "d_cp_d_p_h", NEPTUNE::d_cp_d_p_h,n, OUT4);
          outph[5] = EOS_Field("DCpvap/DHv", "d_cp_d_h_p", NEPTUNE::d_cp_d_h_p,n, OUT5);
          outph[6] = EOS_Field("Rhovap", "rho", NEPTUNE::rho,n, OUT6);
          outph[7] = EOS_Field("DRhovap/dPv", "d_rho_d_p_h", NEPTUNE::d_rho_d_p_h,n, OUT7);
          outph[8] = EOS_Field("DRhovap/DHv", "d_rho_d_h_p", NEPTUNE::d_rho_d_h_p,n, OUT8);

          outph[9] = EOS_Field("Svap", "s", NEPTUNE::s,n, OUT9);
          outph[10] = EOS_Field("DSvap/dPv", "d_s_d_p_h", NEPTUNE::d_s_d_p_h,n, OUT10);
          outph[11] = EOS_Field("DSvap/DHv", "d_s_d_h_p", NEPTUNE::d_s_d_h_p,n, OUT11);
          outph[12] = EOS_Field("Wvap", "w", NEPTUNE::w,n, OUT12);
          outph[13] = EOS_Field("DWvap/dPv", "d_w_d_p_h", NEPTUNE::d_w_d_p_h,n, OUT13);
          outph[14] = EOS_Field("DWvap/DHv", "d_w_d_h_p", NEPTUNE::d_w_d_h_p,n, OUT14);
          outph[15] = EOS_Field("Cvvap", "cv", NEPTUNE::cv,n, OUT15);
          outph[16] = EOS_Field("DCvvap/DPv", "d_cv_d_p_h", NEPTUNE::d_cv_d_p_h,n, OUT16);
          outph[17] = EOS_Field("DCvvap/DHv", "d_cv_d_h_p", NEPTUNE::d_cv_d_h_p,n, OUT17);

      
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
        outtran[0]=EOS_Field("TLAG","lambda", NEPTUNE::lambda, n,OUT0);
        outtran[1]=EOS_Field("TLAG1","d_lambda_d_p_h", NEPTUNE::d_lambda_d_p_h, n,OUT1);
        outtran[2]=EOS_Field("TMUG","mu", NEPTUNE::mu, n,OUT2);
        outtran[3]=EOS_Field("TMUG1","d_mu_d_p_h", NEPTUNE::d_mu_d_p_h, n,OUT3);
        outtran[4]=EOS_Field("SI1","d_sigma_d_p_h", NEPTUNE::d_sigma_d_p_h, n,OUT4);
        outtran[5]=EOS_Field("SI3","d_sigma_d_h_p", NEPTUNE::d_sigma_d_h_p, n,OUT5);
        outtran[6]=EOS_Field(" TLAG3","d_lambda_d_h_p", NEPTUNE::d_lambda_d_h_p, n, OUT6);
        outtran[7]=EOS_Field("TMUG3","d_mu_d_h_p", NEPTUNE::d_mu_d_h_p, n,OUT7);
        outtran[8]=EOS_Field("SI","sigma", NEPTUNE::sigma, n,OUT8);
      
        cr=refprop9v.compute(P2,H2,outtran,err_f);
      }

      { int nbfields_ph=27;
        EOS_Fields outph(nbfields_ph);
           
        //Propriétés gas
        for(int i=0; i<n; i++) OUT0[i]=545.0;
        outph[0]=EOS_Field("Tgaz","T", NEPTUNE::T,n,OUT0);
        outph[1]=EOS_Field("DTgaz/DPv", "d_T_d_p_h", NEPTUNE::d_T_d_p_h, n,OUT1);
        outph[2]=EOS_Field( "DTgaz/DHvap","d_T_d_h_p", NEPTUNE::d_T_d_h_p, n,OUT2);
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
        outph[24]=EOS_Field(" TLAG3","d_lambda_d_h_p", NEPTUNE::d_lambda_d_h_p, n, OUT24);
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
      outph[3]=EOS_Field("HLSP","h_l_sat", NEPTUNE::h_l_sat, n,OUT3);
      outph[4]=EOS_Field("DHLSPPP","d_h_l_sat_d_p", NEPTUNE::d_h_l_sat_d_p, n,OUT4);
      outph[5]=EOS_Field("HVSP","h_v_sat", NEPTUNE::h_v_sat, n,OUT5);
      outph[6]=EOS_Field("DHVSPP","d_h_v_sat_d_p", NEPTUNE::d_h_v_sat_d_p, n,OUT6);
      outph[7]=EOS_Field("CPLSP","cp_l_sat", NEPTUNE::cp_l_sat, n,OUT7);
      outph[8]=EOS_Field("DCLSPP","d_cp_l_sat_d_p", NEPTUNE::d_cp_l_sat_d_p, n,OUT8);
      outph[9]=EOS_Field("CPVSP","cp_v_sat", NEPTUNE::cp_v_sat, n,OUT9);
      outph[10]=EOS_Field("DCVSPP","d_cp_v_sat_d_p", NEPTUNE::d_cp_v_sat_d_p ,n,OUT10);
      outph[11]=EOS_Field("RLSP","rho_l_sat", NEPTUNE::rho_l_sat, n,OUT11);
      outph[12]=EOS_Field("DRLSPP","d_rho_l_sat_d_p", NEPTUNE::d_rho_l_sat_d_p, n,OUT12);
      outph[13]=EOS_Field("RVSP","rho_v_sat", NEPTUNE::rho_v_sat, n,OUT13);
      outph[14]=EOS_Field("DRVSPP","d_rho_v_sat_d_p", NEPTUNE::d_rho_v_sat_d_p, n,OUT14);
            
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

#ifdef WITH_PLUGIN_REFPROP_10
  // Test 10: Refprop10
  {
    cout<<endl<<"--------------------------------------- "<<endl;
    cout<<      "------ Test 10 : Refprop10          ------ "<<endl<<endl;
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
      EOS_Field P("Pressure","p", NEPTUNE::p,xp);
      EOS_Field h_f("Enthalpy","h", NEPTUNE::h,xh);
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
      cout<<      "------ Test 10-1: Refprop10-water    ------ "<<endl<<endl;

      EOS refprop10("EOS_Refprop10","Water");
      cout << "**   eos " << endl;
      cout << "   * fluid   : " <<refprop10.fluid_name()<<endl;
      cout << "   * table   : " <<refprop10.table_name()<<endl;
      cout << "   * version : " <<refprop10.version_name()<<endl<<endl;
      cout << refprop10<<endl;

      test_features(refprop10);

      // Valeurs critiques (CRITP)
      cr = refprop10.get_p_crit(pcrit);
      cr = refprop10.get_T_crit(tcrit);
      cr = refprop10.get_h_crit(hcrit);
      cr = refprop10.get_rho_crit(rhocrit);

      cout << endl << endl << "----- critp-------" << endl;
      cout << "Pc    : " << pcrit << endl;
      cout << "Tc    : " << tcrit << endl;
      cout << "hc    : " << hcrit << endl;
      cout << "rhoc   : " << rhocrit << endl << endl << endl;
      
      // Valeurs limites
//       cr = refprop10.get_p_min(pmin);
      cr = refprop10.get_T_min(tmin);
//       cr = refprop10.get_h_min(hmin);
//       cr = refprop10.get_rho_min(rhomin);
      
      cr = refprop10.get_p_max(pmax);
      cr = refprop10.get_T_max(tmax);
      cr = refprop10.get_h_max(hmax);
      cr = refprop10.get_rho_max(rhomax);
      cout << endl << endl << "----- limites-------" << endl;
      cout << "Pmax        : " <<pmax   << endl;
      cout << "Tmin - Tmax : " << tmin   <<" - "<<tmax   << endl;
      cout << "hmax        : " <<hmax   << endl;
      cout << "rhomax      : " <<rhomax << endl << endl << endl;
      //saturation
      cout << endl << "Saturation" << endl;
      //compute by point
      cr=refprop10.compute_rho_l_sat_p(p,res);
      cout << "[cr="<<cr<<"] rholsat_p = " << res << endl;
      cr=refprop10.compute_rho_v_sat_p(p,res);
      cout << "[cr="<<cr<<"] rhovsat_p = " << res << endl;
      cr=refprop10.compute_h_l_sat_p(p,res);
      cout << "[cr="<<cr<<"] hlsat_p   = " << res<< endl;
      cr=refprop10.compute_h_v_sat_p(p,res);
      cout << "[cr="<<cr<<"] hvsat_p   = " << res << endl;
      cr=refprop10.compute_T_sat_p(p,res);
      cout << "[cr="<<cr<<"] tsat_p    = " << res << endl;
      cr=refprop10.compute_d_T_sat_d_p_p(p,res);
      cout << "[cr="<<cr<<"] dtsatdp   = " << res << endl;
      cr=refprop10.compute_p_sat_T(t,res);
      cout << "[cr="<<cr<<"] psat_T    = " << res << endl<<endl;

      //compute by field
      int nbfields=6;
      EOS_Fields outsat(nbfields);
      outsat[0]=EOS_Field("rholsat", "rho_l_sat", NEPTUNE::rho_l_sat,xrholsat);
      outsat[1]=EOS_Field("rhovsat", "rho_v_sat", NEPTUNE::rho_v_sat,xrhovsat);
      outsat[2]=EOS_Field("hlsat","h_l_sat", NEPTUNE::h_l_sat,xhlsat);
      outsat[3]=EOS_Field("hvsat","h_v_sat", NEPTUNE::h_v_sat,xhvsat);
      outsat[4]=EOS_Field("tsat","T_sat", NEPTUNE::T_sat,xtsat);
      outsat[5]=EOS_Field("dtsatdp","d_T_sat_d_p", NEPTUNE::d_T_sat_d_p,xdtsatdp);

      for(int i=0; i<n; i++)
        xp[i]=p;
      xp[0]=1.e5;
      xp[1]=10.e5;
      xp[2]=70.e5;
      for(int i=0; i<n; i++)
        xh[i]=h;

      cr=refprop10.compute(P,outsat,err_f);
      cout << endl << "field fields [cr=" << cr <<"]"<< endl;
      if (cr != good)
        {
          AString s;
          cout<<" err " <<cr << endl;
          for(int i=0; i<ncout; i++)
            {
              refprop10.fluid().describe_error(err_f[i], s);
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
      cr=refprop10.compute_rho_pT(p,t,res);
      cout << "[cr="<<cr<<"] rho_pT    = " << res << endl;
      cr=refprop10.compute_h_pT(p,t,res);
      cout << "[cr="<<cr<<"] h_pT      = " << res << endl;
      cr=refprop10.compute_d_h_d_T_p_pT(p,t,res);
      cout << "[cr="<<cr<<"] dhdT_pT   = " << res << endl;
      cr=refprop10.compute_u_pT(p,t,res);
      cout << "[cr="<<cr<<"] u_pT      = " << res << endl;
      cr=refprop10.compute_s_pT(p,t,res);
      cout << "[cr="<<cr<<"] s_pT      = " << res << endl;
      cr=refprop10.compute_w_pT(p,t,res);
      cout << "[cr="<<cr<<"] w_pT      = " << res << endl;
      cr=refprop10.compute_cp_pT(p,t,res);
      cout << "[cr="<<cr<<"] cp_pT     = " << res << endl;
      cr=refprop10.compute_cv_pT(p,t,res);
      cout << "[cr="<<cr<<"] cv_pT     = " << res << endl;
      cr=refprop10.compute_mu_pT(p,t,res);
      cout << "[cr="<<cr<<"] mu_pT     = " << res << endl;
      cr=refprop10.compute_lambda_pT(p,t,res);
      cout << "[cr="<<cr<<"] lambda_pT = " << res << endl;
      cr=refprop10.compute_sigma_pT(p,t,res);
      cout << "[cr="<<cr<<"] sigma_pT  = " << res << endl;
      cr=refprop10.compute_beta_pT(p,t,res);
      cout << "[cr="<<cr<<"] beta_pT   = " << res << endl;
      cr=refprop10.compute_g_pT(p,t,res);
      cout << "[cr="<<cr<<"] g_pT      = " << res << endl;
      cr=refprop10.compute_f_pT(p,t,res);
      cout << "[cr="<<cr<<"] f_pT      = " << res << endl;
      cr=refprop10.compute_pr_pT(p,t,res);
      cout << "[cr="<<cr<<"] pr_pT     = " << res << endl << endl;


      // -- properties=f(pT)
      int nbfields_pT=1;
      EOS_Fields outpt(nbfields_pT);
      outpt[0]=EOS_Field("enthalpie","h", NEPTUNE::h,xh);

      EOS_Field T("Temperature","T", NEPTUNE::T,xt);
      for(int i=0; i<n; i++)
        xt[i]=t;
      xt[0]=t;
      xt[1]=t+20.;
      xt[2]=t+40.;

      cr=refprop10.compute(P,T,outpt,err_f);
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

      cr=refprop10.compute_T_ph(p,h,res);
      cout << "[cr="<<cr<<"] T_ph      = " << res << endl;
      cr=refprop10.compute_rho_ph(p,h,res);
      cout << "[cr="<<cr<<"] rho_ph    = " << res << endl;
      cr=refprop10.compute_w_ph(p,h,res);
      cout << "[cr="<<cr<<"] w_ph      = " << res << endl;
      cr=refprop10.compute_u_ph(p,h,res);
      cout << "[cr="<<cr<<"] u_ph      = " << res << endl;
      cr=refprop10.compute_cp_ph(p,h,res);
      cout << "[cr="<<cr<<"] cp_ph     = " << res << endl;
      cr=refprop10.compute_cv_ph(p,h,res);
      cout << "[cr="<<cr<<"] cv_ph     = " << res << endl;
      cr=refprop10.compute_mu_ph(p,h,res);
      cout << "[cr="<<cr<<"] mu_ph     = " << res << endl;
      cr=refprop10.compute_lambda_ph(p,h,res);
      cout << "[cr="<<cr<<"] lambda_ph = " << res << endl;
      cr=refprop10.compute_sigma_ph(p,h,res);
      cout << "[cr="<<cr<<"] sigma_ph  = " << res << endl;
      cr=refprop10.compute_beta_ph(p,t,res);
      cout << "[cr="<<cr<<"] beta_ph   = " << res << endl;
      cr=refprop10.compute_d_rho_d_p_h_ph(p,h,res);
      cout << "[cr="<<cr<<"] drhodp_ph = " << res << endl;
      cr=refprop10.compute_d_rho_d_h_p_ph(p,h,res);
      cout << "[cr="<<cr<<"] drhodh_ph = " << res << endl;
      cr=refprop10.compute_d_T_d_h_p_ph(p,h,res);
      cout << "[cr="<<cr<<"] dTdh_ph   = " << res << endl;
      cr=refprop10.compute_g_ph(p,h,res);
      cout << "[cr="<<cr<<"] g_ph      = " << res << endl;
      cr=refprop10.compute_f_ph(p,h,res);
      cout << "[cr="<<cr<<"] f_ph      = " << res << endl;
      cr=refprop10.compute_pr_ph(p,h,res);
      cout << "[cr="<<cr<<"] pr_ph     = " << res << endl << endl;

      //  properties=f(ph)
      ArrOfDouble xhin(n);
      ArrOfDouble xtout(n);

      for(int i=0; i<n; i++)
        xhin[i]=h;
      xhin[0]=h;
      xhin[1]=h+100000.;
      xhin[2]=h+200000.;

      EOS_Field hin("enthalpie","h", NEPTUNE::h,xhin);

      int nbfields_ph=16;
      EOS_Fields outph(nbfields_ph);
      outph[0]=EOS_Field("temperature","T", NEPTUNE::T,xtout);
      outph[1]=EOS_Field("rho","rho", NEPTUNE::rho,xrho);
      outph[2]=EOS_Field("w","w", NEPTUNE::w,xw);
      outph[3]=EOS_Field("u","u", NEPTUNE::u,xu);
      outph[4]=EOS_Field("cp","cp", NEPTUNE::cp,xcp);
      outph[5]=EOS_Field("mu","mu", NEPTUNE::mu,xmu);
      outph[6]=EOS_Field("lambda","lambda", NEPTUNE::lambda,xlambda);
      outph[7]=EOS_Field("sigma","sigma", NEPTUNE::sigma,xsigma);
      outph[8]=EOS_Field("drhodp","d_rho_d_p_h", NEPTUNE::d_rho_d_p_h,xdrhodp);
      outph[9]=EOS_Field("drhodh","d_rho_d_h_p", NEPTUNE::d_rho_d_h_p,xdrhodh);
      outph[10]=EOS_Field("dtdh","d_T_d_h_p", NEPTUNE::d_T_d_h_p,xdtdh);
      outph[11]=EOS_Field("g","g", NEPTUNE::g,xg);
      outph[12]=EOS_Field("f","f", NEPTUNE::f,xf);
      outph[13]=EOS_Field("pr","pr", NEPTUNE::pr,xpr);
      outph[14]=EOS_Field("cv","cv", NEPTUNE::cv,xcv);
      outph[15]=EOS_Field("beta","beta", NEPTUNE::beta,xbeta);

      cr=refprop10.compute(P,hin,outph,err_f);
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
      cr=refprop10.compute_h_l_lim_p(p,res);
      cout << "[cr="<<cr<<"] h_l_lim   = " << res << endl;
      cr=refprop10.compute_h_v_lim_p(p,res);
      cout << "[cr="<<cr<<"] h_v_lim   = " << res << endl<<endl;
          
      //plan ps
      cr=refprop10.compute_h_ps(p,s,res);
      cout << "[cr="<<cr<<"] h_ps      = " << res << endl;
          
      //  properties=f(ps)
      ArrOfDouble xsin(n);
      ArrOfDouble xhout(n);
          
      xsin[0]=s;
      xsin[1]=s+100.;
      xsin[2]=s+200.;

      EOS_Field sin("entropie","s", NEPTUNE::s,xsin);

      int nbfields_ps=1;
      EOS_Fields outps(nbfields_ps);
      outps[0]=EOS_Field("enthalpie","h", NEPTUNE::h,xhout);
          
      cr=refprop10.compute(P,sin,outps,err_f);
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

      cout << "ERRORS Tests" << endl << "Some errors generated by REFPROP10" << endl;

      err = refprop10.fluid().compute_T_sat_p(p,res);
      if (err.generic_error() == good)
        cout << "compute_T_sat_p No Error - generic error code : " << err.generic_error() << endl;

      cout << "new p : " << p2 << endl;
      err = refprop10.fluid().compute_T_sat_p(p2,res);
      if (err.generic_error() != good)
        {
          refprop10.fluid().describe_error(err, description);
          cout << "compute_T_sat_p generic error code : " << err.generic_error() << endl;
          cout << description << endl << endl;
        }
      cout << "new t : " << t2 << endl;
      err = refprop10.fluid().compute_p_sat_T(t2,res);
      if (err.generic_error() != good)
        {
          refprop10.fluid().describe_error(err, description);
          cout << "compute_p_sat_T generic error code : " << err.generic_error() << endl;
          cout << description << endl << endl;
        }
      t2 = 0.;
      cout << "new t : " << t2 << endl;
      err = refprop10.fluid().compute_h_pT(p2,t2,res);
      if (err.generic_error() != good)
        {
          refprop10.fluid().describe_error(err, description);
          cout << "compute_h_pT generic error code : " << err.generic_error() << endl;
          cout << description << endl << endl;
        }

      p2 = -100.;
      cout << "new p : " << p2 << endl;
      err = refprop10.fluid().compute_T_ph(p2,h,res);
      if (err.generic_error() != good)
        {
          refprop10.fluid().describe_error(err, description);
          cout << "compute_T_ph generic error code : " << err.generic_error() << endl;
          cout << description << endl << endl;
        }

      //Constructor test : change enthalpy/entropy reference state
      cout << "Change enthalpy/entropy reference state : NBP"<< endl;

      Strings args(2);
      args[0]="SETREF";
      args[1]="NBP";
      EOS refprop10_newh("EOS_Refprop10","Water", args);
      cout << "**   eos " << endl;
      cout << "   * fluid   : " <<refprop10_newh.fluid_name()<<endl;
      cout << "   * table   : " <<refprop10_newh.table_name()<<endl;
      cout << "   * version : " <<refprop10_newh.version_name()<<endl<<endl;

  
      cout<<refprop10_newh;

      args[1]="OTH";
      double h0 = 0.e0 ;
      double s0 = 0.e0 ;
      double t0 = 298.15e0 ;
      double p0 = 1.015e5 ;
      EOS refprop10_sref("EOS_Refprop10","Water", args, h0, s0, t0, p0);
      cout << "**   eos " << endl;
      cout << "   * fluid   : " <<refprop10_sref.fluid_name()<<endl;
      cout << "   * table   : " <<refprop10_sref.table_name()<<endl;
      cout << "   * version : " <<refprop10_sref.version_name()<<endl<<endl;

  
      cout<<refprop10_sref;
      //Test enthalpy
      t=373.1243;
      cr=refprop10_newh.compute_h_l_sat_T(t,res);
      cout << "[cr="<<cr<<"] hsat_T (NBP)   = " << res<< endl;
      cr=refprop10.compute_h_l_sat_T(t,res2);
      cout << "[cr="<<cr<<"] hsat_T (DEF)   = " << res2 << endl;
      cr=refprop10_sref.compute_h_l_sat_T(t,res3);
      cout << "[cr="<<cr<<"] hsat_T (OTH)   = " << res3 << endl;
      cout << "h difference  : " <<res2-res << " at t = " << t << endl;
      cout << "h2 difference  : " <<res3-res << " at t = " << t << endl;

      t=600.e0;
      refprop10_newh.compute_h_l_sat_T(t,res);
      cout << "hsat_T (NBP)   = " << res<< endl;
      refprop10.compute_h_l_sat_T(t,res2);
      cout << "hsat_T (DEF)   = " << res2 << endl;
      refprop10_sref.compute_h_l_sat_T(t,res3);
      cout << "hsat_T (OTH)   = " << res3 << endl;
      cout << "h difference  : " << res2-res << " at t = " << t << endl;
      cout << "h2 difference  : " << res3-res << " at t = " << t << endl;

      cr=refprop10_newh.compute_h_pT(p,t,res);
      cout << "[cr="<<cr<<"] h_pT (NBP)     = " << res << endl;
      cr=refprop10.compute_h_pT(p,t,res2);
      cout << "[cr="<<cr<<"] h_pT (DEF)     = " << res2 << endl;
      cr=refprop10_sref.compute_h_pT(p,t,res3);
      cout << "[cr="<<cr<<"] h_pT (OTH)     = " << res3 << endl;
      cout << "h difference  : " << res2-res << " at t = " << t << endl;
      cout << "h2 difference  : " << res3-res << " at t = " << t << endl;

      //Test entropy
      t=373.1243;
      cr=refprop10_newh.compute_s_pT(p,t,res);
      cout << "[cr="<<cr<<"] s_pT (NBP)     = " << res << endl;
      cr=refprop10.compute_s_pT(p,t,res2);
      cout << "[cr="<<cr<<"] s_pT (DEF)     = " << res2 << endl;
      cr=refprop10_sref.compute_s_pT(p,t,res3);
      cout << "[cr="<<cr<<"] s_pT (OTH)     = " << res3 << endl;
      cout << "s difference  : " << res2-res << " at t = " << t << endl;
      cout << "s2 difference  : " << res3-res << " at t = " << t << endl;

      t=600.e0;
      cr=refprop10_newh.compute_s_pT(p,t,res);
      cout << "[cr="<<cr<<"] s_pT (NBP)     = " << res << endl;
      cr=refprop10.compute_s_pT(p,t,res2);
      cout << "[cr="<<cr<<"] s_pT (DEF)     = " << res2 << endl;
      cr=refprop10_sref.compute_s_pT(p,t,res3);
      cout << "[cr="<<cr<<"] s_pT (OTH)     = " << res3 << endl;
      cout << "s difference  : " << res2-res <<" at t = " << t << endl;
      cout << "s2 difference  : " << res3-res <<" at t = " << t << endl<<endl;

      //Test internal energy
      t=373.1243;
      cr=refprop10_newh.compute_u_pT(p,t,res);
      cout << "[cr="<<cr<<"] u_pT (NBP)     = " << res << endl;
      cr=refprop10.compute_u_pT(p,t,res2);
      cout << "[cr="<<cr<<"] u_pT (DEF)     = " << res2 << endl;
      cr=refprop10_sref.compute_u_pT(p,t,res3);
      cout << "[cr="<<cr<<"] u_pT (OTH)     = " << res3 << endl;
      cout << "u difference  : " << res2-res << " at t = " << t << endl;
      cout << "u2 difference  : " << res3-res << " at t = " << t << endl;

      t=600.e0;
      cr=refprop10_newh.compute_u_pT(p,t,res);
      cout << "[cr="<<cr<<"] u_pT (NBP)     = " << res << endl;
      cr=refprop10.compute_u_pT(p,t,res2);
      cout << "[cr="<<cr<<"] u_pT (DEF)     = " << res2 << endl;
      cr=refprop10_sref.compute_u_pT(p,t,res3);
      cout << "[cr="<<cr<<"] u_pT (OTH)     = " << res3 << endl;
      cout << "u difference  : " << res2-res <<" at t = " << t << endl;
      cout << "u2 difference  : " << res3-res <<" at t = " << t << endl<<endl;

      //test meta
      double t1,p1;
      t1 = -2. + 273.15;
      p1 = 100000.;
      err = refprop10.fluid().compute_rho_pT(p1,t1,res);
      if (err.generic_error() != good)
        {
          refprop10.fluid().describe_error(err, description);
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
      EOS refprop10_setmod("EOS_Refprop10","Water", args2);
      cout << "**   eos " << endl;
      cout << refprop10_setmod<<endl;
    }


    // Test 10-2: Refprop10
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
      EOS_Field P("Pressure","p", NEPTUNE::p,xp);
      EOS_Field h_f("Enthalpy","h", NEPTUNE::h,xh);
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
      cout<<      "------ Test 10-2 : Refprop10 Liquid   ------ "<<endl<<endl;

      EOS refprop10l("EOS_Refprop10","WaterLiquid");

      cout << "**   eos " << endl;
      cout << "   * fluid   : " <<refprop10l.fluid_name()<<endl;
      cout << "   * table   : " <<refprop10l.table_name()<<endl;
      cout << "   * version : " <<refprop10l.version_name()<<endl<<endl;
      cout << refprop10l<<endl;

      test_features(refprop10l);

      // Valeurs critiques (CRITP)
      cr = refprop10l.get_p_crit(pcrit);
      cr = refprop10l.get_T_crit(tcrit);
      cr = refprop10l.get_h_crit(hcrit);

      cout << "----- critp-------" << endl;
      cout << "Pc    : " << pcrit << endl;
      cout << "Tc    : " << tcrit << endl;
      cout << "hc    : " << hcrit << endl << endl << endl;

      //saturation
      cr=refprop10l.compute_rho_l_sat_p(p,res);
      cout << "[cr="<<cr<<"] rholsat_p = " << res << endl;
      cr=refprop10l.compute_rho_v_sat_p(p,res);
      cout << "[cr="<<cr<<"] rhovsat_p = " << res << endl;
      cr=refprop10l.compute_h_l_sat_p(p,res);
      cout << "[cr="<<cr<<"] hlsat_p   = " << res<< endl;
      cr=refprop10l.compute_h_v_sat_p(p,res);
      cout << "[cr="<<cr<<"] hvsat_p   = " << res << endl;
      cr=refprop10l.compute_T_sat_p(p,res);
      cout << "[cr="<<cr<<"] tsat_p    = " << res << endl;
      cr=refprop10l.compute_d_T_sat_d_p_p(p,res);
      cout << "[cr="<<cr<<"] dtsatdp   = " << res << endl;
      cr=refprop10l.compute_h_l_sat_T(t,res);
      cout << "[cr="<<cr<<"] hsat_T    = " << res << endl;
      cr=refprop10l.compute_p_sat_T(t,res);
      cout << "[cr="<<cr<<"] psat_T    = " << res << endl<<endl;

      //compute by field
      int nbfields=6;
      EOS_Fields outsat(nbfields);
      outsat[0]=EOS_Field("rholsat", "rho_l_sat", NEPTUNE::rho_l_sat,xrholsat);
      outsat[1]=EOS_Field("rhovsat", "rho_v_sat", NEPTUNE::rho_v_sat,xrhovsat);
      outsat[2]=EOS_Field("hlsat","h_l_sat", NEPTUNE::h_l_sat,xhlsat);
      outsat[3]=EOS_Field("hvsat","h_v_sat", NEPTUNE::h_v_sat,xhvsat);
      outsat[4]=EOS_Field("tsat","T_sat", NEPTUNE::T_sat,xtsat);
      outsat[5]=EOS_Field("dtsatdp","d_T_sat_d_p", NEPTUNE::d_T_sat_d_p,xdtsatdp);

      for(int i=0; i<n; i++)
        xp[i]=p;
      xp[0]=1.e5;
      xp[1]=10.e5;
      xp[2]=70.e5;
      for(int i=0; i<n; i++)
        xh[i]=h;

      cr=refprop10l.compute(P,outsat,err_f);
      cout << endl << "field fields [cr=" << cr <<"]"<< endl;
      if (cr != good)
        {
          AString s;
          cout<<" err " <<cr << endl;
          for(int i=0; i<ncout; i++)
            {
              refprop10l.fluid().describe_error(err_f[i], s);
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
      cr=refprop10l.compute_rho_pT(p,t,res);
      cout << "[cr="<<cr<<"] rho_pT    = " << res << endl;
      cr=refprop10l.compute_h_pT(p,t,res);
      cout << "[cr="<<cr<<"] h_pT      = " << res << endl;
      cr=refprop10l.compute_d_h_d_T_p_pT(p,t,res);
      cout << "[cr="<<cr<<"] dhdT_pT   = " << res << endl;
      cr=refprop10l.compute_u_pT(p,t,res);
      cout << "[cr="<<cr<<"] u_pT      = " << res << endl;
      cr=refprop10l.compute_s_pT(p,t,res);
      cout << "[cr="<<cr<<"] s_pT      = " << res << endl;
      cr=refprop10l.compute_w_pT(p,t,res);
      cout << "[cr="<<cr<<"] w_pT      = " << res << endl;
      cr=refprop10l.compute_cp_pT(p,t,res);
      cout << "[cr="<<cr<<"] cp_pT     = " << res << endl;
      cr=refprop10l.compute_cv_pT(p,t,res);
      cout << "[cr="<<cr<<"] cv_pT     = " << res << endl;
      cr=refprop10l.compute_mu_pT(p,t,res);
      cout << "[cr="<<cr<<"] mu_pT     = " << res << endl;
      cr=refprop10l.compute_lambda_pT(p,t,res);
      cout << "[cr="<<cr<<"] lambda_pT = " << res << endl;
      cr=refprop10l.compute_sigma_pT(p,t,res);
      cout << "[cr="<<cr<<"] sigma_pT  = " << res << endl;
      cr=refprop10l.compute_beta_pT(p,t,res);
      cout << "[cr="<<cr<<"] beta_pT   = " << res << endl;
      cr=refprop10l.compute_g_pT(p,t,res);
      cout << "[cr="<<cr<<"] g_pT      = " << res << endl;
      cr=refprop10l.compute_f_pT(p,t,res);
      cout << "[cr="<<cr<<"] f_pT      = " << res << endl;
      cr=refprop10l.compute_pr_pT(p,t,res);
      cout << "[cr="<<cr<<"] pr_pT      = " << res << endl<<endl;

      // -- properties=f(pT)
      int nbfields_pT=1;
      EOS_Fields outpt(nbfields_pT);
      outpt[0]=EOS_Field("enthalpie","h", NEPTUNE::h,xh);

      EOS_Field T("Temperature","T", NEPTUNE::T,xt);
      for(int i=0; i<n; i++)
        xt[i]=t;
      xt[0]=t;
      xt[1]=t+20.;
      xt[2]=t+40.;

      cr=refprop10l.compute(P,T,outpt,err_f);
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
      cr=refprop10l.compute_T_ph(p,h,res);
      cout << "[cr="<<cr<<"] T_ph      = " << res << endl;
      cr=refprop10l.compute_rho_ph(p,h,res);
      cout << "[cr="<<cr<<"] rho_ph    = " << res << endl;
      cr=refprop10l.compute_w_ph(p,h,res);
      cout << "[cr="<<cr<<"] w_ph      = " << res << endl;
      cr=refprop10l.compute_u_ph(p,h,res);
      cout << "[cr="<<cr<<"] u_ph      = " << res << endl;
      cr=refprop10l.compute_cp_ph(p,h,res);
      cout << "[cr="<<cr<<"] cp_ph     = " << res << endl;
      cr=refprop10l.compute_cv_ph(p,h,res);
      cout << "[cr="<<cr<<"] cv_ph     = " << res << endl;
      cr=refprop10l.compute_mu_ph(p,h,res);
      cout << "[cr="<<cr<<"] mu_ph     = " << res << endl;
      cr=refprop10l.compute_lambda_ph(p,h,res);
      cout << "[cr="<<cr<<"] lambda_ph = " << res << endl;
      cr=refprop10l.compute_sigma_ph(p,h,res);
      cout << "[cr="<<cr<<"] sigma_ph  = " << res << endl;
      cr=refprop10l.compute_beta_ph(p,t,res);
      cout << "[cr="<<cr<<"] beta_ph   = " << res << endl;
      cr=refprop10l.compute_d_rho_d_p_h_ph(p,h,res);
      cout << "[cr="<<cr<<"] drhodp_ph = " << res << endl;
      cr=refprop10l.compute_d_rho_d_h_p_ph(p,h,res);
      cout << "[cr="<<cr<<"] drhodh_ph = " << res << endl;
      cr=refprop10l.compute_d_T_d_h_p_ph(p,h,res);
      cout << "[cr="<<cr<<"] dTdh_ph   = " << res << endl;
      cr=refprop10l.compute_g_ph(p,h,res);
      cout << "[cr="<<cr<<"] g_ph      = " << res << endl;
      cr=refprop10l.compute_f_ph(p,h,res);
      cout << "[cr="<<cr<<"] f_ph      = " << res << endl;
      cr=refprop10l.compute_pr_ph(p,h,res);
      cout << "[cr="<<cr<<"] pr_ph      = " << res << endl<<endl;

      //  properties=f(ph)
      ArrOfDouble xhin(n);
      ArrOfDouble xtout(n);

      for(int i=0; i<n; i++)
        xhin[i]=h;
      xhin[0]=h;
      xhin[1]=h+100000.;
      xhin[2]=h+200000.;

      EOS_Field hin("enthalpie","h", NEPTUNE::h,xhin);

      int nbfields_ph=16;
      EOS_Fields outph(nbfields_ph);
      outph[0]=EOS_Field("temperature","T", NEPTUNE::T,xtout);
      outph[1]=EOS_Field("rho","rho", NEPTUNE::rho,xrho);
      outph[2]=EOS_Field("w","w", NEPTUNE::w,xw);
      outph[3]=EOS_Field("u","u", NEPTUNE::u,xu);
      outph[4]=EOS_Field("cp","cp", NEPTUNE::cp,xcp);
      outph[5]=EOS_Field("mu","mu", NEPTUNE::mu,xmu);
      outph[6]=EOS_Field("lambda","lambda", NEPTUNE::lambda,xlambda);
      outph[7]=EOS_Field("sigma","sigma", NEPTUNE::sigma,xsigma);
      outph[8]=EOS_Field("drhodp","d_rho_d_p_h", NEPTUNE::d_rho_d_p_h,xdrhodp);
      outph[9]=EOS_Field("drhodh","d_rho_d_h_p", NEPTUNE::d_rho_d_h_p,xdrhodh);
      outph[10]=EOS_Field("dtdh","d_T_d_h_p", NEPTUNE::d_T_d_h_p,xdtdh);
      outph[11]=EOS_Field("g","g", NEPTUNE::g,xg);
      outph[12]=EOS_Field("f","f", NEPTUNE::f,xf);
      outph[13]=EOS_Field("pr","pr", NEPTUNE::pr,xpr);
      outph[14]=EOS_Field("cv","cv", NEPTUNE::cv,xcv);
      outph[15]=EOS_Field("beta","beta", NEPTUNE::beta,xbeta);

      cr=refprop10l.compute(P,hin,outph,err_f);
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
      cr=refprop10l.compute_h_l_lim_p(p,res);
      cout << "[cr="<<cr<<"] h_l_lim   = " << res << endl;
      cr=refprop10l.compute_h_v_lim_p(p,res);
      cout << "[cr="<<cr<<"] h_v_lim   = " << res << endl<<endl;
          
      //plan ps
      cr=refprop10l.compute_h_ps(p,s,res);
      cout << "[cr="<<cr<<"] h_ps      = " << res << endl;
          
      //  properties=f(ps)
      ArrOfDouble xsin(n);
      ArrOfDouble xhout(n);
          
      xsin[0]=s;
      xsin[1]=s+100.;
      xsin[2]=s+200.;

      EOS_Field sin("entropie","s", NEPTUNE::s,xsin);

      int nbfields_ps=1;
      EOS_Fields outps(nbfields_ps);
      outps[0]=EOS_Field("enthalpie","h", NEPTUNE::h,xhout);
          
      cr=refprop10l.compute(P,sin,outps,err_f);
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

    // Test 10-3: Refprop10-WaterLiquid
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
      EOS_Field P("Pressure","p", NEPTUNE::p,xp);
      EOS_Field h_f("Enthalpy","h", NEPTUNE::h,xh);
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
      cout<<      "------ Test 10-3: Refprop10 Vapor   ------ "<<endl<<endl;

      EOS refprop10v("EOS_Refprop10","WaterVapor");
      EOS_Error cr;

      cout << "**   eos " << endl;
      cout << "   * fluid   : " <<refprop10v.fluid_name()<<endl;
      cout << "   * table   : " <<refprop10v.table_name()<<endl;
      cout << "   * version : " <<refprop10v.version_name()<<endl<<endl;
      cout << refprop10v<<endl;

      //test_features(refprop10Eos);

      // Valeurs critiques (CRITP)
      cr = refprop10v.get_p_crit(pcrit);
      cr = refprop10v.get_T_crit(tcrit);
      cr = refprop10v.get_h_crit(hcrit);

      cout << "----- critp-------" << endl;
      cout << "Pc    : " << pcrit << endl;
      cout << "Tc    : " << tcrit << endl;
      cout << "hc    : " << hcrit << endl << endl << endl;

      //saturation
      cr=refprop10v.compute_rho_l_sat_p(p,res);
      cout << "[cr="<<cr<<"] rholsat_p = " << res << endl;
      cr=refprop10v.compute_rho_v_sat_p(p,res);
      cout << "[cr="<<cr<<"] rhovsat_p = " << res << endl;
      cr=refprop10v.compute_h_l_sat_p(p,res);
      cout << "[cr="<<cr<<"] hlsat_p   = " << res<< endl;
      cr=refprop10v.compute_h_v_sat_p(p,res);
      cout << "[cr="<<cr<<"] hvsat_p   = " << res << endl;
      cr=refprop10v.compute_T_sat_p(p,res);
      cout << "[cr="<<cr<<"] tsat_p    = " << res << endl;
      cr=refprop10v.compute_d_T_sat_d_p_p(p,res);
      cout << "[cr="<<cr<<"] dtsatdp   = " << res << endl;
      cr=refprop10v.compute_h_v_sat_T(t,res);
      cout << "[cr="<<cr<<"] hsat_T    = " << res << endl;
      cr=refprop10v.compute_p_sat_T(t,res);
      cout << "[cr="<<cr<<"] psat_T    = " << res << endl<<endl;

      //compute by field
      int nbfields=6;
      EOS_Fields outsat(nbfields);
      outsat[0]=EOS_Field("rholsat", "rho_l_sat", NEPTUNE::rho_l_sat,xrholsat);
      outsat[1]=EOS_Field("rhovsat", "rho_v_sat", NEPTUNE::rho_v_sat,xrhovsat);
      outsat[2]=EOS_Field("hlsat","h_l_sat", NEPTUNE::h_l_sat,xhlsat);
      outsat[3]=EOS_Field("hvsat","h_v_sat", NEPTUNE::h_v_sat,xhvsat);
      outsat[4]=EOS_Field("tsat","T_sat", NEPTUNE::T_sat,xtsat);
      outsat[5]=EOS_Field("dtsatdp","d_T_sat_d_p", NEPTUNE::d_T_sat_d_p,xdtsatdp);

      for(int i=0; i<n; i++)
        xp[i]=p;
      xp[0]=p;
      xp[1]=10.e5;
      xp[2]=70.e5;

      cr=refprop10v.compute(P,outsat,err_f);
      cout << endl << "field fields [cr=" << cr <<"]"<< endl;
      if (cr != good)
        {
          AString s;
          cout<<" err " <<cr << endl;
          for(int i=0; i<ncout; i++)
            {
              refprop10v.fluid().describe_error(err_f[i], s);
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
      cr=refprop10v.compute_rho_pT(p,t,res);
      cout << "[cr="<<cr<<"] rho_pT    = " << res << endl;
      cr=refprop10v.compute_h_pT(p,t,res);
      cout << "[cr="<<cr<<"] h_pT      = " << res << endl;
      cr=refprop10v.compute_d_h_d_T_p_pT(p,t,res);
      cout << "[cr="<<cr<<"] dhdT_pT   = " << res << endl;
      cr=refprop10v.compute_u_pT(p,t,res);
      cout << "[cr="<<cr<<"] u_pT      = " << res << endl;
      cr=refprop10v.compute_s_pT(p,t,res);
      cout << "[cr="<<cr<<"] s_pT      = " << res << endl;
      cr=refprop10v.compute_w_pT(p,t,res);
      cout << "[cr="<<cr<<"] w_pT      = " << res << endl;
      cr=refprop10v.compute_cp_pT(p,t,res);
      cout << "[cr="<<cr<<"] cp_pT     = " << res << endl;
      cr=refprop10v.compute_cv_pT(p,t,res);
      cout << "[cr="<<cr<<"] cv_pT     = " << res << endl;
      cr=refprop10v.compute_mu_pT(p,t,res);
      cout << "[cr="<<cr<<"] mu_pT     = " << res << endl;
      cr=refprop10v.compute_lambda_pT(p,t,res);
      cout << "[cr="<<cr<<"] lambda_pT = " << res << endl;
      cr=refprop10v.compute_sigma_pT(p,t,res);
      cout << "[cr="<<cr<<"] sigma_pT  = " << res << endl;
      cr=refprop10v.compute_beta_pT(p,t,res);
      cout << "[cr="<<cr<<"] beta_pT   = " << res << endl;
      cr=refprop10v.compute_g_pT(p,t,res);
      cout << "[cr="<<cr<<"] g_pT      = " << res << endl;
      cr=refprop10v.compute_f_pT(p,t,res);
      cout << "[cr="<<cr<<"] f_pT      = " << res << endl;
      cr=refprop10v.compute_pr_pT(p,t,res);
      cout << "[cr="<<cr<<"] pr_pT      = " << res << endl<<endl;


      // -- properties=f(pT)
      int nbfields_pT=1;
      EOS_Fields outpt(nbfields_pT);
      outpt[0]=EOS_Field("enthalpie","h", NEPTUNE::h,xh);

      EOS_Field T("Temperature","T", NEPTUNE::T,xt);
      xt=xtsat;
      xt+=10.;

      cr=refprop10v.compute(P,T,outpt,err_f);
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
      cr=refprop10v.compute_T_ph(p,h,res);
      cout << "[cr="<<cr<<"] T_ph      = " << res << endl;
      cr=refprop10v.compute_rho_ph(p,h,res);
      cout << "[cr="<<cr<<"] rho_ph    = " << res << endl;
      cr=refprop10v.compute_w_ph(p,h,res);
      cout << "[cr="<<cr<<"] w_ph      = " << res << endl;
      cr=refprop10v.compute_u_ph(p,h,res);
      cout << "[cr="<<cr<<"] u_ph      = " << res << endl;
      cr=refprop10v.compute_cp_ph(p,h,res);
      cout << "[cr="<<cr<<"] cp_ph     = " << res << endl;
      cr=refprop10v.compute_cv_ph(p,h,res);
      cout << "[cr="<<cr<<"] cv_ph     = " << res << endl;
      cr=refprop10v.compute_mu_ph(p,h,res);
      cout << "[cr="<<cr<<"] mu_ph     = " << res << endl;
      cr=refprop10v.compute_lambda_ph(p,h,res);
      cout << "[cr="<<cr<<"] lambda_ph = " << res << endl;
      cr=refprop10v.compute_sigma_ph(p,h,res);
      cout << "[cr="<<cr<<"] sigma_ph  = " << res << endl;
      cr=refprop10v.compute_beta_ph(p,t,res);
      cout << "[cr="<<cr<<"] beta_ph   = " << res << endl;
      cr=refprop10v.compute_d_rho_d_p_h_ph(p,h,res);
      cout << "[cr="<<cr<<"] drhodp_ph = " << res << endl;
      cr=refprop10v.compute_d_rho_d_h_p_ph(p,h,res);
      cout << "[cr="<<cr<<"] drhodh_ph = " << res << endl;
      cr=refprop10v.compute_d_T_d_h_p_ph(p,h,res);
      cout << "[cr="<<cr<<"] dTdh_ph   = " << res << endl;
      cr=refprop10v.compute_g_ph(p,h,res);
      cout << "[cr="<<cr<<"] g_ph      = " << res << endl;
      cr=refprop10v.compute_f_ph(p,h,res);
      cout << "[cr="<<cr<<"] f_ph      = " << res << endl;
      cr=refprop10v.compute_pr_ph(p,h,res);
      cout << "[cr="<<cr<<"] pr_ph     = " << res << endl<<endl;

      //  properties=f(ph)
      ArrOfDouble xhin(n);
      ArrOfDouble xtout(n);

      for(int i=0; i<n; i++)
        xhin[i]=h;
      xhin[0]=h;
      xhin[1]=h+100000.;
      xhin[2]=h+200000.;

      EOS_Field hin("enthalpie","h", NEPTUNE::h,xhin);

      int nbfields_ph=16;
      EOS_Fields outph(nbfields_ph);
      outph[0]=EOS_Field("temperature","T", NEPTUNE::T,xtout);
      outph[1]=EOS_Field("rho","rho", NEPTUNE::rho,xrho);
      outph[2]=EOS_Field("w","w", NEPTUNE::w,xw);
      outph[3]=EOS_Field("u","u", NEPTUNE::u,xu);
      outph[4]=EOS_Field("cp","cp", NEPTUNE::cp,xcp);
      outph[5]=EOS_Field("mu","mu", NEPTUNE::mu,xmu);
      outph[6]=EOS_Field("lambda","lambda", NEPTUNE::lambda,xlambda);
      outph[7]=EOS_Field("sigma","sigma", NEPTUNE::sigma,xsigma);
      outph[8]=EOS_Field("drhodp","d_rho_d_p_h", NEPTUNE::d_rho_d_p_h,xdrhodp);
      outph[9]=EOS_Field("drhodh","d_rho_d_h_p", NEPTUNE::d_rho_d_h_p,xdrhodh);
      outph[10]=EOS_Field("dtdh","d_T_d_h_p", NEPTUNE::d_T_d_h_p,xdtdh);
      outph[11]=EOS_Field("g","g", NEPTUNE::g,xg);
      outph[12]=EOS_Field("f","f", NEPTUNE::f,xf);
      outph[13]=EOS_Field("pr","pr", NEPTUNE::pr,xpr);
      outph[14]=EOS_Field("cv","cv", NEPTUNE::cv,xcv);
      outph[15]=EOS_Field("beta","beta", NEPTUNE::beta,xbeta);

      cr=refprop10v.compute(P,hin,outph,err_f);
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
      cr=refprop10v.compute_h_l_lim_p(p,res);
      cout << "[cr="<<cr<<"] h_l_lim   = " << res << endl;
      cr=refprop10v.compute_h_v_lim_p(p,res);
      cout << "[cr="<<cr<<"] h_v_lim   = " << res << endl;
          
      //plan ps
      cr=refprop10v.compute_h_ps(p,s,res);
      cout << "[cr="<<cr<<"] h_ps      = " << res << endl;
          
      //  properties=f(ps)
      ArrOfDouble xsin(n);
      ArrOfDouble xhout(n);
          
      xsin[0]=s;
      xsin[1]=s+100.;
      xsin[2]=s+200.;

      EOS_Field sin("entropie","s", NEPTUNE::s,xsin);

      int nbfields_ps=1;
      EOS_Fields outps(nbfields_ps);
      outps[0]=EOS_Field("enthalpie","h", NEPTUNE::h,xhout);
          
      cr=refprop10v.compute(P,sin,outps,err_f);
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

    //Test 10-4 : Refprop10
    //Compute by Field : comparaison refprop10 with Cathare test (Test 3 : EOS fluid("EOS_Cathare","WaterLiquid"))
    {
      EOS refprop10("EOS_Refprop10","Water");
      double tref;
      tref=50.+273.15;

      int n=3;
      int ncout=1; // (ncout<=n)

      cout<<endl<<"--------------------------------------- "<<endl;
      cout<<      "------ Test 10-4 : Same tests as Cathare Water (Test 3)   ------ "<<endl<<endl;
      cout<<endl<<"--------------------------------------- "<<endl;

      cout << "**   eos " << endl;
      cout << refprop10<<endl;
      
      // -- saturation properties
      cout<<"----- sat property ----- "<<endl;

      ArrOfDouble xp(n);
      ArrOfDouble xh(n);
      ArrOfInt ierr(n);
      EOS_Field P("Pressure","p", NEPTUNE::p,xp);
      EOS_Field h_f("Enthalpy","h", NEPTUNE::h,xh);
      EOS_Error_Field err_f(ierr);

      ArrOfDouble xtsat(n);
      ArrOfDouble xdtsatdp(n);
      ArrOfDouble xhlsat(n);
      ArrOfDouble xhvsat(n);
      ArrOfDouble xdhlsatdp(n);
      ArrOfDouble xdhvsatdp(n);

      int nbfields=6;
      EOS_Fields outsat(nbfields);
      outsat[0]=EOS_Field("tsat","T_sat", NEPTUNE::T_sat,xtsat);
      outsat[1]=EOS_Field("dtsatdp","d_T_sat_d_p", NEPTUNE::d_T_sat_d_p,xdtsatdp);
      outsat[2]=EOS_Field("hlsat","h_l_sat", NEPTUNE::h_l_sat,xhlsat);
      outsat[3]=EOS_Field("hvsat","h_v_sat", NEPTUNE::h_v_sat,xhvsat);
      outsat[4]=EOS_Field("dhlsatdp","d_h_l_sat_d_p", NEPTUNE::d_h_l_sat_d_p,xdhlsatdp);
      outsat[5]=EOS_Field("dhvsatdp","d_h_v_sat_d_p", NEPTUNE::d_h_v_sat_d_p,xdhvsatdp);

      for(int i=0; i<n; i++)
        xp[i]=1.e5;
      xp[0]=1.e5;
      xp[1]=10.e5;
      xp[2]=70.e5;
      for(int i=0; i<n; i++)
        xh[i]=200000.;

      EOS_Error cr=refprop10.compute(P,outsat,err_f);
      cout << endl << "field fields [cr=" << cr <<"]"<< endl;
      if (cr != good)
        {
          AString s;
          cout<<" err " <<cr << endl;
          for(int i=0; i<ncout; i++)
            {
              refprop10.fluid().describe_error(err_f[i], s);
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

      EOS_Field T("Temperature","T",xt);
      for(int i=0; i<n; i++)
        xt[i]=tref;
      xt[0]=tref;
      xt[1]=tref+20.;
      xt[2]=tref+40.;

      int nbfields_pT=1;
      EOS_Fields outpt(nbfields_pT);
      outpt[0]=EOS_Field("enthalpie","h", NEPTUNE::h,xhout);

      cr=refprop10.compute(P,T,outpt,err_f);
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
      EOS_Field hin("enthalpie","h", NEPTUNE::h,xhin);

      ArrOfDouble xdtdp(n);
      ArrOfDouble xcp(n);
      ArrOfDouble xrho(n);
      ArrOfDouble xdrhodp(n);
      ArrOfDouble xdrhodh(n);
      ArrOfDouble xmu(n);

      int nbfields_ph=7;
      EOS_Fields outph(nbfields_ph);
      outph[0]=EOS_Field("temperature","T", NEPTUNE::T,xtout);
      outph[1]=EOS_Field("dtdp","d_T_d_p_h", NEPTUNE::d_T_d_p_h,xdtdp);
      outph[2]=EOS_Field("cp","cp", NEPTUNE::cp,xcp);
      outph[3]=EOS_Field("rho","rho", NEPTUNE::rho,xrho);
      outph[4]=EOS_Field("drhodp","d_rho_d_p_h", NEPTUNE::d_rho_d_p_h,xdrhodp);
      outph[5]=EOS_Field("drhodh","d_rho_d_h_p", NEPTUNE::d_rho_d_h_p,xdrhodh);
      outph[6]=EOS_Field("mu","mu", NEPTUNE::mu,xmu);

      cr=refprop10.compute(P,hin,outph,err_f);
      cout << endl<< "field field fields  [cr=" << cr <<"]"<< endl;
      for(int i=0; i<ncout; i++)
        {
          cout <<"in "<<P.get_property_title()<<" "<<P[i]<<endl;
          cout <<"in "<<hin.get_property_title()<<" "<<hin[i]<<endl;
          for(int j=0; j<nbfields_ph; j++)
            cout<<" out "<< outph[j].get_property_title()<<" "<<outph[j][i] << endl ;
        }
    }

    //Test 10-5 : Refprop10
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
      cout<<      "------ Test 10-5 : Refprop10 Metastable Nitrogen   ------ "<<endl<<endl;

      EOS refprop10N("EOS_Refprop10","Nitrogen");
      EOS refprop10NL("EOS_Refprop10","NitrogenLiquid");
      EOS refprop10NV("EOS_Refprop10","NitrogenVapor");
      EOS_Error cr;

      cout << "**   eos " << endl;
      cout<<refprop10N<<endl;

      // Valeurs critiques (CRITP)
      cr = refprop10N.get_p_crit(pcrit);
      cr = refprop10N.get_T_crit(tcrit);
      cr = refprop10N.get_h_crit(hcrit);

      cout << "----- critp-------" << endl;
      cout << "Pc    : " << pcrit << endl;
      cout << "Tc    : " << tcrit << endl;
      cout << "hc    : " << hcrit << endl << endl << endl;

      cr=refprop10N.compute_p_sat_T(t,res);
      cout << "saturation pressure 100K : " << res << endl;

      cr=refprop10N.compute_rho_pT(p,t,res);
      cout << "[cr="<<cr<<"] rho_pT (generic) : " << res << endl;
      cr=refprop10NL.compute_rho_pT(p,t,res);
      cout << "[cr="<<cr<<"] rho_pT (liquid) : " << res << endl;
      cr=refprop10NV.compute_rho_pT(p,t,res);
      cout << "[cr="<<cr<<"] rho_pT (vapor) : " << res << endl;

    }


    //Test 10-6 : Refprop10
    { cout<<endl<<"--------------------------------------- "<<endl;
      cout<<      "------ Test 10-6: Refprop10 Vapor   ------ "<<endl<<endl;

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
      EOS_Field P("Pressure","p", NEPTUNE::p,xp);
      EOS_Field h_f("Enthalpy","h", NEPTUNE::h,xh);
      EOS_Error_Field err_f(ierr);
      EOS_Error_Field err_f1(ierr1);

      EOS_Field P2("Pvap","p", NEPTUNE::p,n,P_);
      EOS_Field H2("Hvap","h", NEPTUNE::h,n,H_);


      EOS refprop10v("EOS_Refprop10","WaterVapor"); 
      EOS_Error cr;

      cout << "**   eos " << endl;
      cout << refprop10v<<endl;


      { int nbfields_ph=18;
        EOS_Fields outph(nbfields_ph);
        
        //Propriétés gas
        for(int i=0; i<n; i++) OUT0[i]=545.0;
        outph[0]=EOS_Field("Tgaz","T", NEPTUNE::T,n,OUT0);
        outph[1]=EOS_Field("DTgaz/DPv", "d_T_d_p_h", NEPTUNE::d_T_d_p_h, n,OUT1);
        outph[2]=EOS_Field( "DTgaz/DHvap","d_T_d_h_p", NEPTUNE::d_T_d_h_p, n,OUT2);
        outph[3]=EOS_Field("Cpvap","cp", NEPTUNE::cp, n,OUT3);
        outph[4]=EOS_Field("DCpvap/DPv","d_cp_d_p_h", NEPTUNE::d_cp_d_p_h, n,OUT4);
        outph[5]=EOS_Field("DCpvap/DHv","d_cp_d_h_p", NEPTUNE::d_cp_d_h_p, n,OUT5);
        outph[6]=EOS_Field("Rhovap","rho", NEPTUNE::rho, n,OUT6);
        outph[7]=EOS_Field("DRhovap/dPv","d_rho_d_p_h", NEPTUNE::d_rho_d_p_h, n,OUT7);
        outph[8]=EOS_Field("DRhovap/DHv","d_rho_d_h_p", NEPTUNE::d_rho_d_h_p, n,OUT8);
      
        outph[9]=EOS_Field("Svap","s", NEPTUNE::s, n,OUT9);
        outph[10]=  EOS_Field("DSvap/dPv","d_s_d_p_h", NEPTUNE::d_s_d_p_h, n,OUT10);
        outph[11]=EOS_Field("DSvap/DHv","d_s_d_h_p", NEPTUNE::d_s_d_h_p, n,OUT11);
        outph[12]=EOS_Field("Wvap","w", NEPTUNE::w, n,OUT12);
        outph[13]=EOS_Field("DWvap/dPv","d_w_d_p_h", NEPTUNE::d_w_d_p_h, n,OUT13);
        outph[14]=EOS_Field("DWvap/DHv","d_w_d_h_p", NEPTUNE::d_w_d_h_p, n,OUT14);
        outph[15]=EOS_Field("Cvvap","cv", NEPTUNE::cv, n,OUT15);
        outph[16]=EOS_Field("DCvvap/DPv","d_cv_d_p_h", NEPTUNE::d_cv_d_p_h, n,OUT16);
        outph[17]=EOS_Field("DCvvap/DHv","d_cv_d_h_p", NEPTUNE::d_cv_d_h_p, n,OUT17);
      
        cr=refprop10v.compute(P2,H2,outph,err_f);
      
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
        outtran[0]=EOS_Field("TLAG","lambda", NEPTUNE::lambda, n,OUT0);
        outtran[1]=EOS_Field("TLAG1","d_lambda_d_p_h", NEPTUNE::d_lambda_d_p_h, n,OUT1);
        outtran[2]=EOS_Field("TMUG","mu", NEPTUNE::mu, n,OUT2);
        outtran[3]=EOS_Field("TMUG1","d_mu_d_p_h", NEPTUNE::d_mu_d_p_h, n,OUT3);
        outtran[4]=EOS_Field("SI1","d_sigma_d_p_h", NEPTUNE::d_sigma_d_p_h, n,OUT4);
        outtran[5]=EOS_Field("SI3","d_sigma_d_h_p", NEPTUNE::d_sigma_d_h_p, n,OUT5);
        outtran[6]=EOS_Field(" TLAG3","d_lambda_d_h_p", NEPTUNE::d_lambda_d_h_p, n, OUT6);
        outtran[7]=EOS_Field("TMUG3","d_mu_d_h_p", NEPTUNE::d_mu_d_h_p, n,OUT7);
        outtran[8]=EOS_Field("SI","sigma", NEPTUNE::sigma, n,OUT8);
      
        cr=refprop10v.compute(P2,H2,outtran,err_f);
      }

      { int nbfields_ph=27;
        EOS_Fields outph(nbfields_ph);
           
        //Propriétés gas
        for(int i=0; i<n; i++) OUT0[i]=545.0;
        outph[0]=EOS_Field("Tgaz","T", NEPTUNE::T,n,OUT0);
        outph[1]=EOS_Field("DTgaz/DPv", "d_T_d_p_h", NEPTUNE::d_T_d_p_h, n,OUT1);
        outph[2]=EOS_Field( "DTgaz/DHvap","d_T_d_h_p", NEPTUNE::d_T_d_h_p, n,OUT2);
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
        outph[24]=EOS_Field(" TLAG3","d_lambda_d_h_p", NEPTUNE::d_lambda_d_h_p, n, OUT24);
        outph[25]=EOS_Field("TMUG3","d_mu_d_h_p", NEPTUNE::d_mu_d_h_p, n,OUT25);
        outph[26]=EOS_Field("SI","sigma", NEPTUNE::sigma, n,OUT26);

        cr=refprop10v.compute(P2,H2,outph,err_f);
           
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
      outph[3]=EOS_Field("HLSP","h_l_sat", NEPTUNE::h_l_sat, n,OUT3);
      outph[4]=EOS_Field("DHLSPPP","d_h_l_sat_d_p", NEPTUNE::d_h_l_sat_d_p, n,OUT4);
      outph[5]=EOS_Field("HVSP","h_v_sat", NEPTUNE::h_v_sat, n,OUT5);
      outph[6]=EOS_Field("DHVSPP","d_h_v_sat_d_p",NEPTUNE::d_h_v_sat_d_p, n,OUT6);
      outph[7]=EOS_Field("CPLSP","cp_l_sat", NEPTUNE::cp_l_sat, n,OUT7);
      outph[8]=EOS_Field("DCLSPP","d_cp_l_sat_d_p", NEPTUNE::d_cp_l_sat_d_p, n,OUT8);
      outph[9]=EOS_Field("CPVSP","cp_v_sat", NEPTUNE::cp_v_sat, n,OUT9);
      outph[10]=EOS_Field("DCVSPP","d_cp_v_sat_d_p", NEPTUNE::d_cp_v_sat_d_p, n,OUT10);
      outph[11]=EOS_Field("RLSP","rho_l_sat", NEPTUNE::rho_l_sat, n,OUT11);
      outph[12]=EOS_Field("DRLSPP","d_rho_l_sat_d_p", NEPTUNE::d_rho_l_sat_d_p, n,OUT12);
      outph[13]=EOS_Field("RVSP","rho_v_sat", NEPTUNE::rho_v_sat, n,OUT13);
      outph[14]=EOS_Field("DRVSPP","d_rho_v_sat_d_p", NEPTUNE::d_rho_v_sat_d_p, n,OUT14);
            
      cr=refprop10v.compute(P2,outph,err_f);
        
      for(int i=0; i<ncout; i++)
         { cout <<"in "<<P2.get_property_title()<<" "<<P_[i]<<endl;
           for(int j=0; j<nbfields_ph; j++)
              cout<<" out "<< outph[j].get_property_title()<<" "<<outph[j][i] << endl ;
         }
    }


    //Test 10-7 : Refprop10-Air
    {

      double p=777.;
      double t=100.;
      double pcrit,tcrit,hcrit;
      double res;
      EOS_Internal_Error err;
      AString description;

      cout<<endl<<endl;
      cout<< "----------------------------------------" <<endl;
      cout<< "------ Test 10-7 : Refprop10 Air ----------" <<endl<<endl;

      EOS refprop10A("EOS_Refprop10","Air");
      EOS refprop10AL("EOS_Refprop10","AirLiquid");
      EOS refprop10AV("EOS_Refprop10","AirVapor");
      EOS_Error cr;

      cout << "**   eos " << endl;
      cout << "   * fluid   : " <<refprop10A.fluid_name()<<endl;

      // Valeurs critiques (CRITP)
      refprop10A.get_p_crit(pcrit);
      refprop10A.get_T_crit(tcrit);
      refprop10A.get_h_crit(hcrit);

      cout << "----- critp-------" << endl;
      cout << "Pc    : " << pcrit << endl;
      cout << "Tc    : " << tcrit << endl;
      cout << "hc    : " << hcrit << endl << endl << endl;

      //futur cr=refprop10A.compute_p_sat_T(t,res);
      //futur cout << "saturation pressure 100K : " << res << endl;

      //futur cr=refprop10A.compute_rho_pT(p,t,res);
      //futur cout << "[cr="<<cr<<"] rho_pT (generic) : " << res << endl;
      cr=refprop10AL.compute_rho_pT(p,t,res);
      cout << "[cr="<<cr<<"] rho_pT (liquid) : " << res << endl;
      cr=refprop10AV.compute_rho_pT(p,t,res);
      cout << "[cr="<<cr<<"] rho_pT (vapor) : " << res << endl;
    }




  }
#endif
#ifdef WITH_PLUGIN_CATHARE2
  {
    cout<<endl<<endl;
    cout<<"--------------------------------------- "<<endl;
    cout<<"------ Test 11 : EOS_Cathare2 --------- "<<endl<<endl;

     
      int with_sodium=1;
      try
        {
          EOS testsodium("EOS_Cathare2","SodiumVapor");
        }
      catch (...)
        {
          with_sodium=0;
        }
      int with_HXNC=1;
      try
        {
          EOS testHXNC("EOS_Cathare2","HXNCVapor");
        }
      catch (...)
        {
          with_HXNC=0;
        }
      int with_iapws=1;
      try
        {
          EOS testiapws("EOS_Cathare2","IAPWSVapor");
        }
      catch (...)
        {
          with_iapws=0;
        }
      int with_DHO=1;
      try
        {
          EOS testDHO("EOS_Cathare2","DHOVapor");
        }
      catch (...)
        {
          with_DHO=0;
        }
      int with_PB=1;
      try
        {
          EOS testPB("EOS_Cathare2","PBVapor");
        }
      catch (...)
        {
          with_PB=0;
        }
      int with_PBBI=1;
      try
        {
          EOS testPBBI("EOS_Cathare2","PBBIVapor");
        }
      catch (...)
        {
          with_PBBI=0;
        }

      int nb_eosc2 = 7 ;

      std::vector<EOS*> liquid(nb_eosc2) ;
      std::vector<EOS*> vapor(nb_eosc2)  ;
      /*liquid[0] = new EOS("EOS_Cathare2","WaterLiquid");
        liquid[1] = new EOS("EOS_Cathare2","HXNCLiquid");
        liquid[2] = new EOS("EOS_Cathare2","IAPWSLiquid");
        liquid[3] = new EOS("EOS_Cathare2","SodiumLiquid");
        liquid[4] = new EOS("EOS_Cathare2","DHOLiquid");
        liquid[5] = new EOS("EOS_Cathare2","PBLiquid");
        liquid[6] = new EOS("EOS_Cathare2","PBBILiquid");
        vapor[0]  = new EOS("EOS_Cathare2","WaterVapor");
        vapor[1]  = new EOS("EOS_Cathare2","HXNCVapor");
        vapor[2]  = new EOS("EOS_Cathare2","IAPWSVapor");
        vapor[3]  = new EOS("EOS_Cathare2","SodiumVapor");
        vapor[4]  = new EOS("EOS_Cathare2","DHOVapor");
        vapor[5]  = new EOS("EOS_Cathare2","PBVapor");
        vapor[6]  = new EOS("EOS_Cathare2","PBBIVapor");*/
      liquid[0] = new EOS("EOS_Cathare2","WaterLiquid");
      vapor[0]  = new EOS("CATHARE2","WATERVAPOR");
      if (with_HXNC == 1)
        {
          liquid[1] = new EOS("EOS_Cathare2","HXNCLiquid");
          vapor[1]  = new EOS("CATHARE2","HXNCVAPOR");
        }
      else
        {
          liquid[1] = new EOS("EOS_Cathare2","WaterLiquid");
          vapor[1]  = new EOS("CATHARE2","WATERVAPOR");
        }
      if (with_iapws == 1)
        {
          liquid[2] = new EOS("EOS_Cathare2","IAPWSLiquid");
          vapor[2]  = new EOS("CATHARE2","IAPWSVAPOR");
        }
      else
        {
          liquid[2] = new EOS("EOS_Cathare2","WaterLiquid");
          vapor[2]  = new EOS("CATHARE2","WATERVAPOR");
        }       
      if (with_sodium == 1)
        {
          liquid[3] = new EOS("EOS_Cathare2","SodiumLiquid");
          vapor[3]  = new EOS("CATHARE2","SODIUMVAPOR");
        }
      else
        {
          liquid[3] = new EOS("EOS_Cathare2","WaterLiquid");
          vapor[3]  = new EOS("CATHARE2","WATERVAPOR");
        }
      if (with_DHO == 1)
        {
          liquid[4] = new EOS("EOS_Cathare2","DHOLiquid");
          vapor[4]  = new EOS("CATHARE2","DHOVAPOR");
        }
      else
        {
          liquid[4] = new EOS("EOS_Cathare2","WaterLiquid");
          vapor[4]  = new EOS("CATHARE2","WATERVAPOR");
        }
      if (with_PB == 1)
        {
          liquid[5] = new EOS("EOS_Cathare2","PBLiquid");
          vapor[5]  = new EOS("CATHARE2","PBVAPOR");
        }
      else
        {
          liquid[5] = new EOS("EOS_Cathare2","WaterLiquid");
          vapor[5]  = new EOS("CATHARE2","WATERVAPOR");
        }
      if (with_PBBI == 1)
        {
          liquid[6] = new EOS("EOS_Cathare2","PBBILiquid");
          vapor[6]  = new EOS("CATHARE2","PBBIVAPOR");
        }
      else
        {
          liquid[6] = new EOS("EOS_Cathare2","WaterLiquid");
          vapor[6]  = new EOS("CATHARE2","WATERVAPOR");
        }
 
      //Strings ienc(2);
      //ienc[0] = AString("IENC"); ienc[1] = AString("1");
      //liquid[0]->init(ienc);
      //vapor[0]->init(ienc);

      int nb_val = 50 ;

      ArrOfInt err_(nb_val);
      EOS_Error_Field err(err_);
      ArrOfDouble p_(nb_val);
      for (int i=0; i<nb_val; i++)
        p_[i] = (i+1)* 500000.;
      EOS_Field p("P","p",p_);

      for (int i_eosc2=0; i_eosc2<nb_eosc2; i_eosc2++) {
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
        liquid[i_eosc2]->save_error_handler();
        liquid[i_eosc2]->set_error_handler(handler);
        vapor[i_eosc2]->save_error_handler();
        vapor[i_eosc2]->set_error_handler(handler);
        {
          cout<<endl<<endl;
          cout<<"--------------------------------------- "<<endl;
          cout<<"------ Test 11-" << (i_eosc2+1) << "-l : EOS_Cathare2 ----- "<<endl<<endl;
          cout << "**   eos" << endl;
          cout << "   * fluid   : " <<liquid[i_eosc2]->fluid_name()<<endl;
          cout << "   * table   : " <<liquid[i_eosc2]->table_name()<<endl;
          cout << "   * version : " <<liquid[i_eosc2]->version_name()<<endl<<endl;
          cout <<*liquid[i_eosc2]<<endl;

          EOS_Error cr,crc;
          double molarmass=-1.;
          crc=liquid[i_eosc2]->get_mm(molarmass);
          cout << "molar mass="<<molarmass<<" [cr="<<crc<<"]"<<endl;
          
          double pc=-1.,hc=-1.,Tc=-1.;
          crc=liquid[i_eosc2]->get_p_crit(pc);
          cout << "pcrit="<<pc<<" [cr="<<crc<<"]"<<endl;
          crc=liquid[i_eosc2]->get_h_crit(hc);
          cout << "hcrit="<<hc<<" [cr="<<crc<<"]"<<endl;
          crc=liquid[i_eosc2]->get_T_crit(Tc);
          cout << "Tcrit="<<Tc<<" [cr="<<crc<<"]"<<endl;

          double pmin,pmax,hmin,hmax,Tmin,Tmax;
          crc=liquid[i_eosc2]->get_p_min(pmin);
          cout << "pmin="<<pmin<<" [cr="<<crc<<"]"<<endl;
          crc=liquid[i_eosc2]->get_p_max(pmax);
          cout << "pmax="<<pmax<<" [cr="<<crc<<"]"<<endl;
          crc=liquid[i_eosc2]->get_h_min(hmin);
          cout << "hmin="<<hmin<<" [cr="<<crc<<"]"<<endl;
          crc=liquid[i_eosc2]->get_h_max(hmax);
          cout << "hmax="<<hmax<<" [cr="<<crc<<"]"<<endl;
          crc=liquid[i_eosc2]->get_T_min(Tmin);
          cout << "Tmin="<<Tmin<<" [cr="<<crc<<"]"<<endl;
          crc=liquid[i_eosc2]->get_T_max(Tmax);
          cout << "Tmax="<<Tmax<<" [cr="<<crc<<"]"<<endl;
          
          double pref=-1.,href=-1.,Tref=-1.;
          crc=liquid[i_eosc2]->get_p_ref(pref);
          cout << "pref="<<pref<<" [cr="<<crc<<"]"<<endl;
          crc=liquid[i_eosc2]->get_h_ref(href);
          cout << "href="<<href<<" [cr="<<crc<<"]"<<endl;
          crc=liquid[i_eosc2]->get_T_ref(Tref);
          cout << "Tref="<<Tref<<" [cr="<<crc<<"]"<<endl;

          ArrOfDouble tsp_(50), hlsp_(50), hvsp_(50), cplsp_(50), cpvsp_(50), rlsp_(50), rvsp_(50);
          ArrOfDouble tsp1_(50), hlsp1_(50), hvsp1_(50), cplsp1_(50), cpvsp1_(50), rlsp1_(50), rvsp1_(50);
          ArrOfDouble tsp2_(50), hllp_(50), hvlp_(50);
          EOS_Fields out(17);
          EOS_Field tsp("T_sat","T_sat", NEPTUNE::T_sat,tsp_); out[0] = tsp;
          EOS_Field hlsp("h_l_sat","h_l_sat", NEPTUNE::h_l_sat,hlsp_); out[1] = hlsp;
          EOS_Field hvsp("h_v_sat","h_v_sat", NEPTUNE::h_v_sat,hvsp_); out[2] = hvsp;
          EOS_Field hllp("h_l_lim","h_l_lim", NEPTUNE::h_l_lim,hllp_); out[3] = hllp;
          EOS_Field hvlp("h_v_lim","h_v_lim", NEPTUNE::h_v_lim,hvlp_); out[4] = hvlp;
          EOS_Field cplsp("cp_l_sat","cp_l_sat", NEPTUNE::cp_l_sat,cplsp_); out[5] = cplsp;
          EOS_Field cpvsp("cp_v_sat","cp_v_sat", NEPTUNE::cp_v_sat,cpvsp_); out[6] = cpvsp;
          EOS_Field rlsp("rho_l_sat","rho_l_sat", NEPTUNE::rho_l_sat,rlsp_); out[7] = rlsp;
          EOS_Field rvsp("rho_v_sat","rho_v_sat", NEPTUNE::rho_v_sat,rvsp_); out[8] = rvsp;
          EOS_Field tsp1("d_T_sat_d_p","d_T_sat_d_p", NEPTUNE::d_T_sat_d_p,tsp1_); out[9] = tsp1;
          EOS_Field hlsp1("d_h_l_sat_d_p","d_h_l_sat_d_p", NEPTUNE::d_h_l_sat_d_p,hlsp1_); out[10] = hlsp1;
          EOS_Field hvsp1("d_h_v_sat_d_p","d_h_v_sat_d_p", NEPTUNE::d_h_v_sat_d_p,hvsp1_); out[11] = hvsp1;
          EOS_Field cplsp1("d_cp_l_sat_d_p","d_cp_l_sat_d_p", NEPTUNE::d_cp_l_sat_d_p,cplsp1_); out[12] = cplsp1;
          EOS_Field cpvsp1("d_cp_v_sat_d_p","d_cp_v_sat_d_p", NEPTUNE::d_cp_v_sat_d_p,cpvsp1_); out[13] = cpvsp1;
          EOS_Field rlsp1("d_rho_l_sat_d_p","d_rho_l_sat_d_p", NEPTUNE::d_rho_l_sat_d_p,rlsp1_); out[14] = rlsp1;
          EOS_Field rvsp1("d_rho_v_sat_d_p","d_rho_v_sat_d_p", NEPTUNE::d_rho_v_sat_d_p,rvsp1_); out[15] = rvsp1;
          EOS_Field tsp2("d2_T_sat_d_p_d_p","d2_T_sat_d_p_d_p", NEPTUNE::d2_T_sat_d_p_d_p,tsp2_); out[16] = tsp2;
          cout << "TEST 1" << endl << "Input :" << p;
          cr=liquid[i_eosc2]->compute(p, out, err);
          cout << endl<< "TEST 1 [cr=" << cr <<"]"<< endl;
          cout << "Outputs : " << endl << out;
          cout << "Errors :" << endl << err;

          ArrOfDouble h_(50); h_ = hlsp_; h_ *= 0.8; EOS_Field h("h", "h", NEPTUNE::h, h_);
          EOS_Fields out2(18);
          ArrOfDouble t_(50); EOS_Field t("T", "T", NEPTUNE::T, t_); out2[0] = t;
          ArrOfDouble t1_(50); EOS_Field t1("d_T_d_p", "d_T_d_p_h", NEPTUNE::d_T_d_p_h, t1_); out2[1] = t1;
          ArrOfDouble t2_(50); EOS_Field t2("d_T_d_h", "d_T_d_h_p", NEPTUNE::d_T_d_h_p, t2_); out2[2] = t2;
          ArrOfDouble cp_(50); EOS_Field cp("cp", "cp", NEPTUNE::cp, cp_); out2[3] = cp;
          ArrOfDouble cp1_(50); EOS_Field cp1("d_cp_d_p", "d_cp_d_p_h", NEPTUNE::d_cp_d_p_h, cp1_); out2[4] = cp1;
          ArrOfDouble cp2_(50); EOS_Field cp2("d_cp_d_h", "d_cp_d_h_p", NEPTUNE::d_cp_d_h_p, cp2_); out2[5] = cp2;
          ArrOfDouble rho_(50); EOS_Field rho("rho", "rho", NEPTUNE::rho, rho_); out2[6] = rho;
          ArrOfDouble rho1_(50); EOS_Field rho1("d_rho_d_p", "d_rho_d_p_h", NEPTUNE::d_rho_d_p_h, rho1_); out2[7] = rho1;
          ArrOfDouble rho2_(50); EOS_Field rho2("d_rho_d_h", "d_rho_d_h_p", NEPTUNE::d_rho_d_h_p, rho2_); out2[8] = rho2;
          ArrOfDouble beta_(50); EOS_Field beta("beta", "beta", NEPTUNE::beta, beta_); out2[9] = beta;
          ArrOfDouble beta1_(50); EOS_Field beta1("d_beta_d_p", "d_beta_d_p_h", NEPTUNE::d_beta_d_p_h, beta1_); out2[10] = beta1;
          ArrOfDouble beta2_(50); EOS_Field beta2("d_beta_d_h", "d_beta_d_h_p", NEPTUNE::d_beta_d_h_p, beta2_); out2[11] = beta2;
          ArrOfDouble mu_(50); EOS_Field mu("mu", "mu", NEPTUNE::mu, mu_); out2[12] = mu;
          ArrOfDouble mu1_(50); EOS_Field mu1("d_mu_d_p", "d_mu_d_p_h", NEPTUNE::d_mu_d_p_h, mu1_); out2[13] = mu1;
          ArrOfDouble mu2_(50); EOS_Field mu2("d_mu_d_h", "d_mu_d_h_p", NEPTUNE::d_mu_d_h_p, mu2_); out2[14] = mu2;
          ArrOfDouble lambda_(50); EOS_Field lambda("lambda", "lambda", NEPTUNE::lambda, lambda_); out2[15] = lambda;
          ArrOfDouble lambda1_(50); EOS_Field lambda1("d_lambda_d_p", "d_lambda_d_p_h", NEPTUNE::d_lambda_d_p_h, lambda1_); out2[16] = lambda1;
          ArrOfDouble lambda2_(50); EOS_Field lambda2("d_lambda_d_h", "d_lambda_d_h_p", NEPTUNE::d_lambda_d_h_p, lambda2_); out2[17] = lambda2;
          cout << "TEST 2" << endl << "Input :" << p << endl << h << endl;
          cr=liquid[i_eosc2]->compute(p, h, out2, err);
          cout << endl<< "TEST 2 [cr=" << cr <<"]"<< endl;
          cout << "Outputs : " << endl << out2;
          cout << "Errors :" << endl << err;

          ArrOfDouble xt_2(50); xt_2=t_; EOS_Field T_2("T", "T", NEPTUNE::T, xt_2);
          EOS_Fields out3(18);
          ArrOfDouble xh_2(50); EOS_Field h_2("h", "h", NEPTUNE::h, xh_2); out3[0] = h_2;
          ArrOfDouble xt1_2(50); EOS_Field t1_2("d_T_d_p", "d_T_d_p_h", NEPTUNE::d_T_d_p_h, xt1_2); out3[1] = t1_2;
          ArrOfDouble xt2_2(50); EOS_Field t2_2("d_T_d_h", "d_T_d_h_p", NEPTUNE::d_T_d_h_p, xt2_2); out3[2] = t2_2;
          ArrOfDouble xcp_2(50); EOS_Field cp_2("cp", "cp", NEPTUNE::cp, xcp_2); out3[3] = cp_2;
          ArrOfDouble xcp1_2(50); EOS_Field cp1_2("d_cp_d_p", "d_cp_d_p_h", NEPTUNE::d_cp_d_p_h, xcp1_2); out3[4] = cp1_2;
          ArrOfDouble xcp2_2(50); EOS_Field cp2_2("d_cp_d_h", "d_cp_d_h_p", NEPTUNE::d_cp_d_h_p, xcp2_2); out3[5] = cp2_2;
          ArrOfDouble xrho_2(50); EOS_Field rho_2("rho", "rho", NEPTUNE::rho, xrho_2); out3[6] = rho_2;
          ArrOfDouble xrho1_2(50); EOS_Field rho1_2("d_rho_d_p", "d_rho_d_p_h", NEPTUNE::d_rho_d_p_h, xrho1_2); out3[7] = rho1_2;
          ArrOfDouble xrho2_2(50); EOS_Field rho2_2("d_rho_d_h", "d_rho_d_h_p", NEPTUNE::d_rho_d_h_p, xrho2_2); out3[8] = rho2_2;
          ArrOfDouble xbeta_2(50); EOS_Field beta_2("beta", "beta", NEPTUNE::beta, xbeta_2); out3[9] = beta_2;
          ArrOfDouble xbeta1_2(50); EOS_Field beta1_2("d_beta_d_p", "d_beta_d_p_h", NEPTUNE::d_beta_d_p_h, xbeta1_2); out3[10] = beta1_2;
          ArrOfDouble xbeta2_2(50); EOS_Field beta2_2("d_beta_d_h", "d_beta_d_h_p", NEPTUNE::d_beta_d_h_p, xbeta2_2); out3[11] = beta2_2;
          ArrOfDouble xmu_2(50); EOS_Field mu_2("mu", "mu", NEPTUNE::mu, xmu_2); out3[12] = mu_2;
          ArrOfDouble xmu1_2(50); EOS_Field mu1_2("d_mu_d_p", "d_mu_d_p_h", NEPTUNE::d_mu_d_p_h, xmu1_2); out3[13] = mu1_2;
          ArrOfDouble xmu2_2(50); EOS_Field mu2_2("d_mu_d_h", "d_mu_d_h_p", NEPTUNE::d_mu_d_h_p, xmu2_2); out3[14] = mu2_2;
          ArrOfDouble xlambda_2(50); EOS_Field lambda_2("lambda", "lambda", NEPTUNE::lambda, xlambda_2); out3[15] = lambda_2;
          ArrOfDouble xlambda1_2(50); EOS_Field lambda1_2("d_lambda_d_p", "d_lambda_d_p_h", NEPTUNE::d_lambda_d_p_h, xlambda1_2); out3[16] = lambda1_2;
          ArrOfDouble xlambda2_2(50); EOS_Field lambda2_2("d_lambda_d_h", "d_lambda_d_h_p", NEPTUNE::d_lambda_d_h_p, xlambda2_2); out3[17] = lambda2_2;
          cout << "TEST 3" << endl << "Input :" << p << endl << T_2 << endl;
          cr=liquid[i_eosc2]->compute(p, T_2, out3, err);
          cout << endl<< "TEST 3 [cr=" << cr <<"]"<< endl;
          cout << "Outputs : " << endl << out3;
          cout << "Errors :" << endl << err;

          cout<<endl<<"--- compare compute (p,T) versus compute (p,h) -- "<<endl;               
          for(int k=0; k<50; k++)
            {
              cout << out3[0].get_property_title() << " " << out3[0][k] <<" [cr="<<cr<<"] (diff="<<out3[0][k]-h[k]<<")"<<endl;
              if (fabs(out3[0][k]-h[k])>=fmax(1.e-8,1.e-6*fabs(h[k])) || out3[0][k]*h[k] < 0.e0)
                cout << "--> error "<<out3[0].get_property_title()<<" values are different h(p,T)="<<out3[0][k]<<" h="<<h[k]<< endl;
            }
          for(int j=1; j<18; j++)
            {
              for(int k=0; k<50; k++)
                {
                  cout << out3[j].get_property_title() << " " << out3[j][k] <<" [cr="<<cr<<"] (diff="<<out3[j][k]-out2[j][k]<<")"<<endl;
                  if (fabs(out3[j][k]-out2[j][k])>=fmax(1.e-8,1.e-6*fabs(out2[j][k])) || out2[j][k]*out3[j][k] < 0.e0)
                    cout << "--> error "<<out3[j].get_property_title()<<" values are different pT="<<out3[j][k]<<" ph="<<out2[j][k]<< endl;
                }
            }   
        }
        {
          cout<<endl<<endl;
          cout<<"--------------------------------------- "<<endl;
          cout<<"------ Test 11-" << (i_eosc2+1) << "-v : EOS_Cathare2 ------ "<<endl<<endl;
          cout << "**   eos" << endl;
          cout << "   * fluid   : " <<vapor[i_eosc2]->fluid_name()<<endl;
          cout << "   * table   : " <<vapor[i_eosc2]->table_name()<<endl;
          cout << "   * version : " <<vapor[i_eosc2]->version_name()<<endl<<endl;
          cout <<*vapor[i_eosc2]<<endl;

          EOS_Error crc;
          double molarmass=-1.;
          crc=liquid[i_eosc2]->get_mm(molarmass);
          cout << "molar mass="<<molarmass<<" [cr="<<crc<<"]"<<endl;

          double pc=-1.,hc=-1.,Tc=-1.;
          crc=vapor[i_eosc2]->get_p_crit(pc);
          cout << "pcrit="<<pc<<" [cr="<<crc<<"]"<<endl;
          crc=vapor[i_eosc2]->get_h_crit(hc);
          cout << "hcrit="<<hc<<" [cr="<<crc<<"]"<<endl;
          crc=vapor[i_eosc2]->get_T_crit(Tc);
          cout << "Tcrit="<<Tc<<" [cr="<<crc<<"]"<<endl;

          double pmin,pmax,hmin,hmax,Tmin,Tmax;
          crc=vapor[i_eosc2]->get_p_min(pmin);
          cout << "pmin="<<pmin<<" [cr="<<crc<<"]"<<endl;
          crc=vapor[i_eosc2]->get_p_max(pmax);
          cout << "pmax="<<pmax<<" [cr="<<crc<<"]"<<endl;
          crc=vapor[i_eosc2]->get_h_min(hmin);
          cout << "hmin="<<hmin<<" [cr="<<crc<<"]"<<endl;
          crc=vapor[i_eosc2]->get_h_max(hmax);
          cout << "hmax="<<hmax<<" [cr="<<crc<<"]"<<endl;
          crc=vapor[i_eosc2]->get_T_min(Tmin);
          cout << "Tmin="<<Tmin<<" [cr="<<crc<<"]"<<endl;
          crc=vapor[i_eosc2]->get_T_max(Tmax);
          cout << "Tmax="<<Tmax<<" [cr="<<crc<<"]"<<endl;

          double pref=-1.,href=-1.,Tref=-1.;
          crc=vapor[i_eosc2]->get_p_ref(pref);
          cout << "pref="<<pref<<" [cr="<<crc<<"]"<<endl;
          crc=vapor[i_eosc2]->get_h_ref(href);
          cout << "href="<<href<<" [cr="<<crc<<"]"<<endl;
          crc=vapor[i_eosc2]->get_T_ref(Tref);
          cout << "Tref="<<Tref<<" [cr="<<crc<<"]"<<endl;

          EOS_Error cr;
          ArrOfDouble tsp_(50), hlsp_(50), hvsp_(50), cplsp_(50), cpvsp_(50), rlsp_(50), rvsp_(50);
          ArrOfDouble tsp1_(50), hlsp1_(50), hvsp1_(50), cplsp1_(50), cpvsp1_(50), rlsp1_(50), rvsp1_(50);
          ArrOfDouble tsp2_(50), hllp_(50), hvlp_(50);
          EOS_Fields out(17);
          EOS_Field tsp("T_sat","T_sat", NEPTUNE::T_sat,tsp_); out[0] = tsp;
          EOS_Field hlsp("h_l_sat","h_l_sat", NEPTUNE::h_l_sat,hlsp_); out[1] = hlsp;
          EOS_Field hvsp("h_v_sat","h_v_sat", NEPTUNE::h_v_sat,hvsp_); out[2] = hvsp;
          EOS_Field hllp("h_l_lim","h_l_lim", NEPTUNE::h_l_lim,hllp_); out[3] = hllp;
          EOS_Field hvlp("h_v_lim","h_v_lim", NEPTUNE::h_v_lim,hvlp_); out[4] = hvlp;
          EOS_Field cplsp("cp_l_sat","cp_l_sat", NEPTUNE::cp_l_sat,cplsp_); out[5] = cplsp;
          EOS_Field cpvsp("cp_v_sat","cp_v_sat", NEPTUNE::cp_v_sat,cpvsp_); out[6] = cpvsp;
          EOS_Field rlsp("rho_l_sat","rho_l_sat", NEPTUNE::rho_l_sat,rlsp_); out[7] = rlsp;
          EOS_Field rvsp("rho_v_sat","rho_v_sat", NEPTUNE::rho_v_sat,rvsp_); out[8] = rvsp;
          EOS_Field tsp1("d_T_sat_d_p","d_T_sat_d_p", NEPTUNE::d_T_sat_d_p,tsp1_); out[9] = tsp1;
          EOS_Field hlsp1("d_h_l_sat_d_p","d_h_l_sat_d_p", NEPTUNE::d_h_l_sat_d_p,hlsp1_); out[10] = hlsp1;
          EOS_Field hvsp1("d_h_v_sat_d_p","d_h_v_sat_d_p", NEPTUNE::d_h_v_sat_d_p,hvsp1_); out[11] = hvsp1;
          EOS_Field cplsp1("d_cp_l_sat_d_p","d_cp_l_sat_d_p", NEPTUNE::d_cp_l_sat_d_p,cplsp1_); out[12] = cplsp1;
          EOS_Field cpvsp1("d_cp_v_sat_d_p","d_cp_v_sat_d_p", NEPTUNE::d_cp_v_sat_d_p,cpvsp1_); out[13] = cpvsp1;
          EOS_Field rlsp1("d_rho_l_sat_d_p","d_rho_l_sat_d_p", NEPTUNE::d_rho_l_sat_d_p,rlsp1_); out[14] = rlsp1;
          EOS_Field rvsp1("d_rho_v_sat_d_p","d_rho_v_sat_d_p", NEPTUNE::d_rho_v_sat_d_p,rvsp1_); out[15] = rvsp1;
          EOS_Field tsp2("d2_T_sat_d_p_d_p","d2_T_sat_d_p_d_p", NEPTUNE::d2_T_sat_d_p_d_p,tsp2_); out[16] = tsp2;
          cout << "TEST 1" << endl << "Input :" << p;
          cr=vapor[i_eosc2]->compute(p, out, err);
          cout << endl<< "TEST 1 [cr=" << cr <<"]"<< endl;
          cout << "Outputs : " << endl << out;
          cout << "Errors :" << endl << err;

          ArrOfDouble h_(50); h_ = hvsp_; EOS_Field h("h", "h", NEPTUNE::h, h_);
          EOS_Fields out2(18);
          ArrOfDouble t_(50); EOS_Field t("T", "T", NEPTUNE::T, t_); out2[0] = t;
          ArrOfDouble t1_(50); EOS_Field t1("d_T_d_p", "d_T_d_p_h", NEPTUNE::d_T_d_p_h, t1_); out2[1] = t1;
          ArrOfDouble t2_(50); EOS_Field t2("d_T_d_h", "d_T_d_h_p", NEPTUNE::d_T_d_h_p, t2_); out2[2] = t2;
          ArrOfDouble cp_(50); EOS_Field cp("cp", "cp", NEPTUNE::cp, cp_); out2[3] = cp;
          ArrOfDouble cp1_(50); EOS_Field cp1("d_cp_d_p", "d_cp_d_p_h", NEPTUNE::d_cp_d_p_h, cp1_); out2[4] = cp1;
          ArrOfDouble cp2_(50); EOS_Field cp2("d_cp_d_h", "d_cp_d_h_p", NEPTUNE::d_cp_d_h_p, cp2_); out2[5] = cp2;
          ArrOfDouble rho_(50); EOS_Field rho("rho", "rho", NEPTUNE::rho, rho_); out2[6] = rho;
          ArrOfDouble rho1_(50); EOS_Field rho1("d_rho_d_p", "d_rho_d_p_h", NEPTUNE::d_rho_d_p_h, rho1_); out2[7] = rho1;
          ArrOfDouble rho2_(50); EOS_Field rho2("d_rho_d_h", "d_rho_d_h_p", NEPTUNE::d_rho_d_h_p, rho2_); out2[8] = rho2;
          ArrOfDouble sigma_(50); EOS_Field sigma("sigma", "sigma", NEPTUNE::sigma, sigma_); out2[9] = sigma;
          ArrOfDouble sigma1_(50); EOS_Field sigma1("d_sigma_d_p", "d_sigma_d_p_h", NEPTUNE::d_sigma_d_p_h, sigma1_); out2[10] = sigma1;
          ArrOfDouble sigma2_(50); EOS_Field sigma2("d_sigma_d_h", "d_sigma_d_h_p", NEPTUNE::d_sigma_d_h_p, sigma2_); out2[11] = sigma2;
          ArrOfDouble mu_(50); EOS_Field mu("mu", "mu", NEPTUNE::mu, mu_); out2[12] = mu;
          ArrOfDouble mu1_(50); EOS_Field mu1("d_mu_d_p", "d_mu_d_p_h", NEPTUNE::d_mu_d_p_h, mu1_); out2[13] = mu1;
          ArrOfDouble mu2_(50); EOS_Field mu2("d_mu_d_h", "d_mu_d_h_p", NEPTUNE::d_mu_d_h_p, mu2_); out2[14] = mu2;
          ArrOfDouble lambda_(50); EOS_Field lambda("lambda", "lambda", NEPTUNE::lambda, lambda_); out2[15] = lambda;
          ArrOfDouble lambda1_(50); EOS_Field lambda1("d_lambda_d_p", "d_lambda_d_p_h", NEPTUNE::d_lambda_d_p_h, lambda1_); out2[16] = lambda1;
          ArrOfDouble lambda2_(50); EOS_Field lambda2("d_lambda_d_h", "d_lambda_d_h_p", NEPTUNE::d_lambda_d_h_p, lambda2_); out2[17] = lambda2;
          cout << "TEST 2" << endl << "Input :" << p << endl << h << endl;
          cr=vapor[i_eosc2]->compute(p, h, out2, err);
          cout << endl<< "TEST 2 [cr=" << cr <<"]"<< endl;
          cout << "Outputs : " << endl << out2;
          cout << "Errors :" << endl << err;

          ArrOfDouble xt_2(50); xt_2=t_; EOS_Field T_2("T", "T", NEPTUNE::T, xt_2);
          EOS_Fields out3(18);
          ArrOfDouble xh_2(50); EOS_Field h_2("h", "h", NEPTUNE::h, xh_2); out3[0] = h_2;
          ArrOfDouble xt1_2(50); EOS_Field t1_2("d_T_d_p", "d_T_d_p_h", NEPTUNE::d_T_d_p_h, xt1_2); out3[1] = t1_2;
          ArrOfDouble xt2_2(50); EOS_Field t2_2("d_T_d_h", "d_T_d_h_p", NEPTUNE::d_T_d_h_p, xt2_2); out3[2] = t2_2;
          ArrOfDouble xcp_2(50); EOS_Field cp_2("cp", "cp", NEPTUNE::cp, xcp_2); out3[3] = cp_2;
          ArrOfDouble xcp1_2(50); EOS_Field cp1_2("d_cp_d_p", "d_cp_d_p_h", NEPTUNE::d_cp_d_p_h, xcp1_2); out3[4] = cp1_2;
          ArrOfDouble xcp2_2(50); EOS_Field cp2_2("d_cp_d_h", "d_cp_d_h_p", NEPTUNE::d_cp_d_h_p, xcp2_2); out3[5] = cp2_2;
          ArrOfDouble xrho_2(50); EOS_Field rho_2("rho", "rho", NEPTUNE::rho, xrho_2); out3[6] = rho_2;
          ArrOfDouble xrho1_2(50); EOS_Field rho1_2("d_rho_d_p", "d_rho_d_p_h", NEPTUNE::d_rho_d_p_h, xrho1_2); out3[7] = rho1_2;
          ArrOfDouble xrho2_2(50); EOS_Field rho2_2("d_rho_d_h", "d_rho_d_h_p", NEPTUNE::d_rho_d_h_p, xrho2_2); out3[8] = rho2_2;
          ArrOfDouble xsigma_2(50); EOS_Field sigma_2("sigma", "sigma", NEPTUNE::sigma, xsigma_2); out3[9] = sigma_2;
          ArrOfDouble xsigma1_2(50); EOS_Field sigma1_2("d_sigma_d_p", "d_sigma_d_p_h", NEPTUNE::d_sigma_d_p_h, xsigma1_2); out3[10] = sigma1_2;
          ArrOfDouble xsigma2_2(50); EOS_Field sigma2_2("d_sigma_d_h", "d_sigma_d_h_p", NEPTUNE::d_sigma_d_h_p, xsigma2_2); out3[11] = sigma2_2;
          ArrOfDouble xmu_2(50); EOS_Field mu_2("mu", "mu", NEPTUNE::mu, xmu_2); out3[12] = mu_2;
          ArrOfDouble xmu1_2(50); EOS_Field mu1_2("d_mu_d_p", "d_mu_d_p_h", NEPTUNE::d_mu_d_p_h, xmu1_2); out3[13] = mu1_2;
          ArrOfDouble xmu2_2(50); EOS_Field mu2_2("d_mu_d_h", "d_mu_d_h_p", NEPTUNE::d_mu_d_h_p, xmu2_2); out3[14] = mu2_2;
          ArrOfDouble xlambda_2(50); EOS_Field lambda_2("lambda", "lambda", NEPTUNE::lambda, xlambda_2); out3[15] = lambda_2;
          ArrOfDouble xlambda1_2(50); EOS_Field lambda1_2("d_lambda_d_p", "d_lambda_d_p_h", NEPTUNE::d_lambda_d_p_h, xlambda1_2); out3[16] = lambda1_2;
          ArrOfDouble xlambda2_2(50); EOS_Field lambda2_2("d_lambda_d_h", "d_lambda_d_h_p", NEPTUNE::d_lambda_d_h_p, xlambda2_2); out3[17] = lambda2_2;
          cout << "TEST 3" << endl << "Input :" << p << endl << T_2 << endl;
          cr=vapor[i_eosc2]->compute(p, T_2, out3, err);
          cout << endl<< "TEST 3 [cr=" << cr <<"]"<< endl;
          cout << "Outputs : " << endl << out3;
          cout << "Errors :" << endl << err;

          cout<<endl<<"--- compare compute (p,T) versus compute (p,h) -- "<<endl;               
          for(int k=0; k<50; k++)
            {
              cout << out3[0].get_property_title() << " " << out3[0][k] <<" [cr="<<cr<<"] (diff="<<out3[0][k]-h[k]<<")"<<endl;
              if (fabs(out3[0][k]-h[k])>=fmax(1.e-8,1.e-6*fabs(h[k])) || out3[0][k]*h[k] < 0.e0)
                cout << "--> error "<<out3[0].get_property_title()<<" values are different h(p,T)="<<out3[0][k]<<" h="<<h[k]<< endl;
            }
          for(int j=1; j<18; j++)
            {
              for(int k=0; k<50; k++)
                {
                  cout << out3[j].get_property_title() << " " << out3[j][k] <<" [cr="<<cr<<"] (diff="<<out3[j][k]-out2[j][k]<<")"<<endl;
                  if (fabs(out3[j][k]-out2[j][k])>=fmax(1.e-8,1.e-6*fabs(out2[j][k])) || out2[j][k]*out3[j][k] < 0.e0)
                    cout << "--> error "<<out3[j].get_property_title()<<" values are different pT="<<out3[j][k]<<" ph="<<out2[j][k]<< endl;
                }
            }   

        }
        liquid[i_eosc2]->restore_error_handler();
        vapor[i_eosc2]->restore_error_handler();
        delete liquid[i_eosc2];
        delete vapor[i_eosc2];
      }
     
  }

  {
    cout<<endl<<endl;
    cout<<"--------------------------------------- "<<endl;
    cout<<"------ Test 12 : EOS_Cathare2 w/ Mixing "<<endl<<endl;
    {
      int with_sodium=1;
      try
        {
          EOS testsodium("EOS_Cathare2","SodiumVapor");
        }
      catch (...)
        {
          with_sodium=0;
        }
      int with_HXNC=1;
      try
        {
          EOS testHXNC("EOS_Cathare2","HXNCVapor");
        }
      catch (...)
        {
          with_HXNC=0;
        }
      int with_iapws=1;
      try
        {
          EOS testiapws("EOS_Cathare2","IAPWSVapor");
        }
      catch (...)
        {
          with_iapws=0;
        }
              
      EOS* vapor[4];
      vapor[0] = new EOS("EOS_Cathare2", "WaterVapor");
      if (with_HXNC == 1)
        {
          vapor[1]  = new EOS("CATHARE2","HXNCVAPOR");
        }
      else
        {
          vapor[1]  = new EOS("CATHARE2","WATERVAPOR");
        }
      if (with_iapws == 1)
        {
          vapor[2]  = new EOS("CATHARE2","IAPWSVAPOR");
        }
      else
        {
          vapor[2]  = new EOS("CATHARE2","WATERVAPOR");
        }
      if (with_sodium == 1)
        {
          vapor[3] = new EOS("EOS_Cathare2", "SodiumVapor");
        }
      else
        {
          vapor[3] = new EOS("EOS_Cathare2", "WaterVapor");
        }
              
      for (int i=0; i<4; i++) {
        EOS argon    = EOS("EOS_Cathare2", "ArgonIncondensable"   );
        EOS nitrogen = EOS("EOS_Cathare2", "NitrogenIncondensable");
        EOS user     = EOS("EOS_Cathare2", "UserIncondensable");
        Strings userValues(16);
        userValues[0]  = AString("42.0");
        userValues[1]  = AString("42.0");
        userValues[2]  = AString("42.0");
        userValues[3]  = AString("42.0");
        userValues[4]  = AString("42.0");
        userValues[5]  = AString("42.0");
        userValues[6]  = AString("42.0");
        userValues[7]  = AString("42.0");
        userValues[8]  = AString("42.0");
        userValues[9]  = AString("42.0");
        userValues[10] = AString("42.0");
        userValues[11] = AString("42.0");
        userValues[12] = AString("42.0");
        userValues[13] = AString("42.0");
        userValues[14] = AString("42.0");
        userValues[15] = AString("42.0");
        user.init(userValues);

        EOS* components[3];
        components[0] = vapor[i];
        components[1] = &argon;
        components[2] = &nitrogen;
        //components[3] = &user;

        EOS mixing = EOS("EOS_Mixing");
        cout << "**   eos" << endl;
        cout <<mixing<<endl;
        //mixing.set_components(components, 4);
        mixing.set_components(components, 3);
        

        EOS_Error cr;
        int n=3;
        ArrOfDouble xp(n),xc0(n),xc1(n),xc2(n),xc3(n);
        for(int j=0; j<n; j++)
          {
            xp[j] = 100000.;
            xc0[j] = 0.40;
            xc1[j] = 0.35;
            xc2[j] = 0.25;
            xc3[j] = 0.10;
          }
        xp[1] = 1000000.;
        xp[2] = 10000000.;
        EOS_Field p("P","p", NEPTUNE::p,xp);
        EOS_Field c0("c_0","c_0", NEPTUNE::c_0,xc0);
        EOS_Field c1("c_1","c_1", NEPTUNE::c_1,xc1);
        EOS_Field c2("c_2","c_2", NEPTUNE::c_2,xc2);
        EOS_Field c3("c_3","c_3", NEPTUNE::c_3,xc3);

        //EOS_Fields in(5);
        EOS_Fields in(4);
        in[0] = p;
        in[1] = c0;
        in[2] = c1;
        in[3] = c2;
        //in[4] = c3;

        ArrOfInt xerr(n);
        EOS_Error_Field err(xerr);

        //  properties=f(p,xi)
        cout<<endl<<"--- compute (p,xi) -- "<<endl;
        EOS_Fields out(2);
        ArrOfDouble xtsat(n),xh(n);
        EOS_Field tsat("T_sat","T_sat", NEPTUNE::T_sat,xtsat);
        out[0] = EOS_Field("T_sat","T_sat", NEPTUNE::T_sat,xtsat);
        out[1] = EOS_Field("h_v_sat","h_v_sat", NEPTUNE::h_v_sat,xh);
        cr = mixing.compute(in, out, err);
        cout << endl<< "fields fields [cr=" << cr <<"]"<< endl;
        for(int k=0; k<n; k++)
          {
            for (int j=0; j<in.size(); j++)
              cout <<"in "<<in[j].get_property_title()<<" "<<in[j][k]<<endl;
            for(int j=0; j<out.size(); j++)
              cout<<"out "<<out[j].get_property_title()<<" "<<out[j][k] << endl ;
          }
 
        //  properties=f(p,h=hvsat,xi)
        cout << "---compute f(p,h=hvsat,xi) "<<endl ;
        //EOS_Fields in2(6);
        EOS_Fields in2(5);
        in2[0] = p;
        in2[1] = EOS_Field("h","h", NEPTUNE::h, xh);
        in2[2] = c0;
        in2[3] = c1;
        in2[4] = c2;
        //in2[5] = c3;
        EOS_Fields out2(2);
        ArrOfDouble xt(n),xrho(n);
        out2[0] = EOS_Field("T","T", NEPTUNE::T,xt);
        out2[1] = EOS_Field("rho","rho", NEPTUNE::rho,xrho);
        cr = mixing.compute(in2, out2, err);
        cout << endl<< "fields fields [cr=" << cr <<"]"<< endl;
        for(int k=0; k<n; k++)
          {
            for (int j=0; j<in2.size(); j++)
              cout <<"in "<<in2[j].get_property_title()<<" "<<in2[j][k]<<endl;
            for(int j=0; j<out2.size(); j++)
              cout<<"out "<<out2[j].get_property_title()<<" "<<out2[j][k] << endl ;
          }

        //  properties=f(p,T,xi)
        cout << "---compute f(p,T,xi) "<<endl ;
        ArrOfDouble xh2(n),xrho2(n);
        in2[1] = out2[0];
        out2[0] = EOS_Field("h","h", NEPTUNE::h, xh2);
        out2[1] = EOS_Field("rho","rho", NEPTUNE::rho,xrho2);
        cr = mixing.compute(in2, out2, err);
        cout << endl<< "fields fields [cr=" << cr <<"]"<< endl;
        for(int k=0; k<n; k++)
          {
            for (int j=0; j<in2.size(); j++)
              cout <<"in "<<in2[j].get_property_title()<<" "<<in2[j][k]<<endl;
            for(int j=0; j<out2.size(); j++)
              cout<<"out "<<out2[j].get_property_title()<<" "<<out2[j][k] << endl ;
          }
        if (cr != good)
          {
            AString s;
            for(int j=0; j<3; j++)
              {
                cout<<err[j].generic_error()<<" "<<err[j].get_code()<<" "<<err[j].get_partial_code()<<endl;
              }
          }     

        //  properties=f(p,T,xi)
        cout << "----compute rho(p,T,Xi) "<< endl;
        ArrOfDouble xrho3(n),xdrhodp3(n);
        out2[0] = EOS_Field("rho","rho", NEPTUNE::rho,xrho3);
        out2[1] = EOS_Field("drhodp","d_rho_d_p_h", NEPTUNE::d_rho_d_p_h,xdrhodp3);
        cr = mixing.compute(in2, out2, err);
        cout << endl<< "fields fields [cr=" << cr <<"]"<< endl;
        for(int k=0; k<n; k++)
          {
            for (int j=0; j<in2.size(); j++)
              cout <<"in "<<in2[j].get_property_title()<<" "<<in2[j][k]<<endl;
            for(int j=0; j<out2.size(); j++)
              cout<<"out "<<out2[j].get_property_title()<<" "<<out2[j][k] << endl ;
          }
        if (cr != good)
          {
            AString s;
            for(int j=0; j<3; j++)
              {
                cout<<err[j].generic_error()<<" "<<err[j].get_code()<<" "<<err[j].get_partial_code()<<endl;
              }
          }     

        cout << endl<<endl;
        // compute_by_point
        double tp=in2[1][0]+40.;
        double hp=-1.;
        cr=vapor[i]->compute_h_pT(p[0], tp, hp);
        cout<<"by point vapor (without mixing) [cr="<<cr<<"] in p "<<p[0]<<" in T "<<tp<<" out h " <<hp<<endl;
        
      }
      for (int i=0; i<4; i++)
        delete vapor[i];
    }
  }
  {
    cout<<endl<<endl;
    cout<<"--------------------------------------- "<<endl;
    cout<<"------ Test 13 : EOS_Cathare2           "<<endl<<endl;
      
    EOS steam("EOS_Cathare2","WaterVapor");
    cout << "**   eos" << endl;
    cout <<steam<<endl;
    double pp=7727380.;
    double tt=952.69; 
    double hh;
    cout << "in     p="<<pp<<" T="<<tt<<endl;
    EOS_Error cr=steam.compute_h_pT(pp,tt,hh);
    cout << "in/out p="<<pp<<" T="<<tt<<" h(p,T)="<<hh<<" [cr="<<cr<<"]"<<endl;
    ArrOfDouble p_(1),h_(1),t_(1);
    ArrOfInt err_(1);
    EOS_Field p("p", "p", NEPTUNE::p, p_);
    EOS_Field h("h", "h", NEPTUNE::h, h_);
    EOS_Field T("T", "T", NEPTUNE::T, t_);
    EOS_Error_Field err(err_);
    p[0]=pp;
    T[0]=tt;
    cout << "in     p[0]="<<p_[0]<<" t[0]="<<t_[0]<<endl;
    cr=steam.compute(p, T, h, err);
    cout << "in/out p[0]="<<p_[0]<<" t[0]="<<t_[0]<<" h(p,T)[0]="<<h_[0]<<" [cr="<<cr<<"]"<<endl;
  }


  // Test 14 : EOS_Cathare2 IAPWS
  {
    cout<<endl<<endl;
    cout<<"--------------------------------------- "<<endl;
    cout<<"------ Test 14 : EOS_Cathare2 IAPWS     "<<endl<<endl;
 
    {
      cout<<endl<<endl;
      cout<<"--------------------------------------- "<<endl;
      cout<<"------ Test 14-1 : EOS_Cathare2 IAPWSLiquid   "<<endl<<endl;
 
      try
         { EOS testiapws("EOS_Cathare2","IAPWSLiquid") ;
         }
      catch (...)
         { cout << endl << endl ;
           cout << " *** No (EOS_Cathare2,IAPWSLiquid)" << endl << endl ;
           goto endpar_iapwsliq ;
         }

      EOS *obj_eos ;
      obj_eos = new EOS("EOS_Cathare2", "IAPWSLiquid") ;
 
      EOS_Error er ;
 
      cout << endl << endl ;
      cout << " . obj_eos = "        << *obj_eos        ;
 
      // rho_lsat = f(T_sat) : scalar compute in one point
      { double temperat[1], resultat[1] ;
        temperat[0] = 584.149 ;
        resultat[0] = 0e0 ;
  
        er = obj_eos->compute_Tsat("rho_lsat", temperat[0], resultat[0]) ;
 
        cout << endl << endl ;
        cout << " rho_lsat = f(T_sat) : scalar compute scalar in one point" << endl ;
        cout << " .  er              = " << er              << endl ;
        if (er == 0)
           { cout << " -> Temperature     = " << temperat[0]       << endl ;
             cout << " <- density         = " << resultat[0]       << endl ;
           }
        else
           cout << "   Error rho_lsat = f(T_sat)" << endl ;
      }
 
      // rho_lsat = f(P_sat) : scalar compute in one point
      { double pression[1], resultat[1] ;
        pression[0] = 9.99993e+06 ;
        resultat[0] = 0e0 ;
  
        er = obj_eos->compute_Psat("rho_lsat", pression[0], resultat[0]) ;
 
        cout << endl << endl ;
        cout << " rho_lsat = f(P_sat) : scalar compute scalar in one point" << endl ;
        cout << " .  er              = " << er              << endl ;
        if (er == 0)
           { cout << " -> Pressure        = " << pression[0]       << endl ;
             cout << " <- density         = " << resultat[0]       << endl ;
           }
        else
           cout << "   Error  rho_lsat = f(P_sat)" << endl ;
      }
 
      // epstl, ... = f(P,h) : compute Fields in one point
      { double pression[1] , enthalpi[1] ;
        double resulta1[1] ;
        double resulta2[1] ;
        double resulta3[1] ;
        double resulta4[1] ;
        double resulta5[1] ;
        double resulta6[1] ;
        double resulta7[1] ;
        int terr[1] ;
        int nbpoint = 1 ;
        pression[0] = 219.9e5 ;
        enthalpi[0] = 1.7e6 ;
        resulta1[0] = 0e0 ;
        resulta2[0] = 0e0 ;
        resulta3[0] = 0e0 ;
        resulta4[0] = 0e0 ;
        resulta5[0] = 0e0 ;
        resulta6[0] = 0e0 ;
        resulta7[0] = 0e0 ;
 
        EOS_Field fpression("P","P", NEPTUNE::p,nbpoint,pression);
        EOS_Field fenthalpi("h","h", NEPTUNE::h,nbpoint,enthalpi);
        EOS_Fields vfresultat(7);
        EOS_Field fresulta1("T",               "T", NEPTUNE::T,               nbpoint,resulta1); vfresultat[0] = fresulta1 ;
        EOS_Field fresulta2("epstl",           "epstl", NEPTUNE::epstl,           nbpoint,resulta2); vfresultat[1] = fresulta2 ;
        EOS_Field fresulta3("[d(epstl)/dP]h",  "[d(epstl)/dP]h", NEPTUNE::d_epstl_dp_h,  nbpoint,resulta3); vfresultat[2] = fresulta3 ;
        EOS_Field fresulta4("[d(epstl)/dh]P",  "[d(epstl)/dh]P", NEPTUNE::d_epstl_dh_p,  nbpoint,resulta4); vfresultat[3] = fresulta4 ;
        EOS_Field fresulta5("hlspsc",          "hlspsc", NEPTUNE::hlspsc,          nbpoint,resulta5); vfresultat[4] = fresulta5 ;
        EOS_Field fresulta6("[d(hlspsc)/dP]h", "[d(hlspsc)/dP]h", NEPTUNE::d_hlspsc_dp_h, nbpoint,resulta6); vfresultat[5] = fresulta6 ;
        EOS_Field fresulta7("[d(hlspsc)/dh]P", "[d(hlspsc)/dh]P", NEPTUNE::d_hlspsc_dh_p, nbpoint,resulta7); vfresultat[6] = fresulta7 ;
        EOS_Error_Field ferr(nbpoint, terr) ;

        er = obj_eos->compute(fpression, fenthalpi, vfresultat, ferr) ;

        cout << endl << endl ;
        cout << " epstl, ... = f(P,h) : Fields in one point" << endl ;
        cout << " .  er                 = " << er              << endl ;
        if (er == 0)
           { cout << "   (i) Pressure        = " << fpression[0]       << endl ;
             cout << "   (i) Enthalpy        = " << fenthalpi[0]       << endl ;
             cout << "   ---" << endl ;
             cout << "   (o) T               = " << vfresultat[0][0]   << endl ;
             cout << "   (o) epstl           = " << vfresultat[1][0]   << endl ;
             cout << "   (o) [d(epstl)/dP]h  = " << vfresultat[2][0]   << endl ;
             cout << "   (o) [d(epstl)/dh]P  = " << vfresultat[3][0]   << endl ;
             cout << "   (o) hlspsc          = " << vfresultat[4][0]   << endl ;
             cout << "   (o) [d(hlspsc)/dP]h = " << vfresultat[5][0]   << endl ;
             cout << "   (o) [d(hlspsc)/dh]P = " << vfresultat[6][0]   << endl ;
           }
        else
           cout << "   Erreur epstl, ... = f(P,h)" << endl ;
      }
 
      delete obj_eos ;
      endpar_iapwsliq: ;

      (void) er;
    }
 
 
    {
      cout<<endl<<endl;
      cout<<"--------------------------------------- "<<endl;
      cout<<"------ Test 14-2 : EOS_Cathare2 IAPWSVapor    "<<endl<<endl;
 
      try
         { EOS testiapws("EOS_Cathare2","IAPWSVapor") ;
         }
      catch (...)
         { cout << endl << endl ;
           cout << " *** No (EOS_Cathare2,IAPWSVapor)" << endl << endl ;
           goto endpar_iapwsvap ;
         }
 
      EOS *obj_eos ;
      obj_eos = new EOS("EOS_Cathare2", "IAPWSVapor") ;
 
      EOS_Error er ;
 
      cout << endl << endl ;
      cout << " . obj_eos = "        << *obj_eos        ;
 
      // rho_lsat = f(T_sat) : scalar compute in one point
      { double temperat[1], resultat[1] ;
        temperat[0] = 584.149 ;
        resultat[0] = 0e0 ;
  
        er = obj_eos->compute_Tsat("rho_lsat", temperat[0], resultat[0]) ;
 
        cout << endl << endl ;
        cout << " rho_lsat = f(T_sat) : scalar compute in one point" << endl ;
        cout << " .  er              = " << er              << endl ;
        if (er == 0)
           { cout << " -> Temperature     = " << temperat[0]     << " K"      << endl ;
             cout << " <- density         = " << resultat[0]     << " kg/m3"  << endl ;
           }
        else
           cout << "   Error rho_lsat = f(T_sat)" << endl ;
      }
 
      // rho_lsat = f(P_sat) : scalar compute in one point
      { double pression[1], resultat[1] ;
        pression[0] = 9.99993e+06 ;
        resultat[0] = 0e0 ;
  
        er = obj_eos->compute_Psat("rho_lsat", pression[0], resultat[0]) ;
 
        cout << endl << endl ;
        cout << " rho_lsat = f(P_sat) : scalar compute in one point" << endl ;
        cout << " .  er              = " << er              << endl ;
        if (er == 0)
           { cout << " -> Pressure        = " << pression[0]     << " Pa"     << endl ;
             cout << " <- density         = " << resultat[0]     << " kg/m3"  << endl ;
           }
        else
           cout << "   Error  rho_lsat = f(P_sat)" << endl ;
      }
 
      // Without incondensable gas, epstl, ... = f(P,h) : compute Fields in one point
      { double pression[1] , enthalpi[1] ;
        double resulta01[1] ;
        double resulta02[1] ;
        double resulta03[1] ;
        double resulta04[1] ;
        double resulta05[1] ;
        double resulta06[1] ;
        double resulta07[1] ;
        double resulta08[1] ;
        double resulta09[1] ;
        double resulta10[1] ;
        double resulta11[1] ;
        double resulta12[1] ;
        double resulta13[1] ;
        int terr[1] ;
        int nbpoint = 1 ;
        pression[0] = 219.9e5 ;
        enthalpi[0] = 1.7e6 ;
        resulta01[0] = 0e0 ;
        resulta02[0] = 0e0 ;
        resulta03[0] = 0e0 ;
        resulta04[0] = 0e0 ;
        resulta05[0] = 0e0 ;
        resulta06[0] = 0e0 ;
        resulta07[0] = 0e0 ;
        resulta08[0] = 0e0 ;
        resulta09[0] = 0e0 ;
        resulta10[0] = 0e0 ;
        resulta11[0] = 0e0 ;
        resulta12[0] = 0e0 ;
        resulta13[0] = 0e0 ;
 
        EOS_Field fpression("P","P", NEPTUNE::p,nbpoint,pression);
        EOS_Field fenthalpi("h","h", NEPTUNE::h,nbpoint,enthalpi);
        EOS_Fields vfresultat(13); 
        EOS_Field fresulta01("T",                     "T", NEPTUNE::T,                nbpoint,resulta01); vfresultat[0]  = fresulta01 ;
        EOS_Field fresulta02("epstl",                 "epstl", NEPTUNE::epstl,            nbpoint,resulta02); vfresultat[1]  = fresulta02 ;
        EOS_Field fresulta03("[d(epstl)/dP]h",        "[d(epstl)/dP]h", NEPTUNE::d_epstl_dp_h,   nbpoint,resulta03); vfresultat[2]  = fresulta03 ;
        EOS_Field fresulta04("[d(epstl)/dh]P",        "[d(epstl)/dh]P", NEPTUNE::d_epstl_dh_p,   nbpoint,resulta04); vfresultat[3]  = fresulta04 ;
        EOS_Field fresulta05("hlspsc",                "hlspsc", NEPTUNE::hlspsc,           nbpoint,resulta05); vfresultat[4]  = fresulta05 ;
        EOS_Field fresulta06("[d(hlspsc)/dP]h",       "[d(hlspsc)/dP]h", NEPTUNE::d_hlspsc_dp_h,  nbpoint,resulta06); vfresultat[5]  = fresulta06 ;
        EOS_Field fresulta07("[d(hlspsc)/dh]P",       "[d(hlspsc)/dh]P", NEPTUNE::d_hlspsc_dh_p,  nbpoint,resulta07); vfresultat[6]  = fresulta07 ;
        EOS_Field fresulta08("epstg",                 "epstg", NEPTUNE::epstg,            nbpoint,resulta08); vfresultat[7]  = fresulta08 ;
        EOS_Field fresulta09("[d(epstg)/dP]h",        "[d(epstg)/dP]h", NEPTUNE::d_epstg_dp_h,   nbpoint,resulta09); vfresultat[8]  = fresulta09 ;
        EOS_Field fresulta10("[d(epstg)/dh]P",        "[d(epstg)/dh]P", NEPTUNE::d_epstg_dh_p,   nbpoint,resulta10); vfresultat[9]  = fresulta10 ;
        EOS_Field fresulta11("hvspsc",                "hvspsc", NEPTUNE::hvspsc,           nbpoint,resulta11); vfresultat[10] = fresulta11 ;
        EOS_Field fresulta12("[d(hvspsc)/dP]h",       "[d(hvspsc)/dP]h", NEPTUNE::d_hvspsc_dp_h,  nbpoint,resulta12); vfresultat[11] = fresulta12 ;
        EOS_Field fresulta13("[d(hvspsc)/dh]P",       "[d(hvspsc)/dh]P", NEPTUNE::d_hvspsc_dh_p,  nbpoint,resulta13); vfresultat[12] = fresulta13 ;

        EOS_Error_Field ferr(nbpoint, terr) ;

        er = obj_eos->compute(fpression, fenthalpi, vfresultat, ferr) ;

        cout << endl << endl ;
        cout << " Without incondensable gas, epstl, ... = f(P,h) : compute Fields in one point" << endl ;
        cout << " .  er                 = " << er              << endl ;
        if (er == 0)
           { cout << "   (i) Pressure              = " << fpression[0]        << endl ;
             cout << "   (i) enthalpy              = " << fenthalpi[0]        << endl ;
             cout << "   ---" << endl ;
             cout << "   (o) T                     = " << vfresultat[0][0]    << endl ;
             cout << "   (o) epstl                 = " << vfresultat[1][0]    << endl ;
             cout << "   (o) [d(epstl)/dP]h        = " << vfresultat[2][0]    << endl ;
             cout << "   (o) [d(epstl)/dh]P        = " << vfresultat[3][0]    << endl ;
             cout << "   (o) hlspsc                = " << vfresultat[4][0]    << endl ;
             cout << "   (o) [d(hlspsc)/dP]h       = " << vfresultat[5][0]    << endl ;
             cout << "   (o) [d(hlspsc)/dh]P       = " << vfresultat[6][0]    << endl ;
             cout << "   (o) epstg                 = " << vfresultat[7][0]    << endl ;
             cout << "   (o) [d(epstg)/dP]h        = " << vfresultat[8][0]    << endl ;
             cout << "   (o) [d(epstg)/dh]P        = " << vfresultat[9][0]    << endl ;
             cout << "   (o) hvspsc                = " << vfresultat[10][0]   << endl ;
             cout << "   (o) [d(hvspsc)/dP]h       = " << vfresultat[11][0]   << endl ;
             cout << "   (o) [d(hvspsc)/dh]P       = " << vfresultat[12][0]   << endl ;
           }
        else
           cout << "   Error Without incondensable gas, epstl, ... = f(P,h)" << endl ;
      }
 
      // With incondensable gas, hlsvsc, ... = f(P,h) : compute Fields in one point
      { double pression[1] , enthalpi[1] ;
        double mf__veau[1], mf_argon[1], mf_azote[1] ;
        double resulta01[1] ;
        double resulta02[1] ;
        double resulta03[1] ;
        double resulta04[1] ;
        double resulta05[1] ;
        double resulta06[1] ;
        double resulta07[1] ;
        double resulta08[1] ;
        double resulta09[1] ;
        double resulta10[1] ;
        double resulta11[1] ;
        double resulta12[1] ;
        double resulta13[1] ;
        double resulta14[1] ;
        double resulta15[1] ;
        double resulta16[1] ;
        double resulta17[1] ;
        double resulta18[1] ;
        double resulta19[1] ;
        double resulta20[1] ;
        double resulta21[1] ;
        double resulta22[1] ;
        double resulta23[1] ;
        double resulta24[1] ;
        double resulta25[1] ;
        double resulta26[1] ;
        double resulta27[1] ;
        double resulta28[1] ;
        double resulta29[1] ;
        double resulta30[1] ;
        int terr[1] ;
        int nbpoint = 1 ;
        pression[0] = 219.9e5 ;
        enthalpi[0] = 1.7e6 ;
        mf__veau[0] = 0.40e0 ;
        mf_argon[0] = 0.35e0 ;
        mf_azote[0] = 0.25e0 ;
        resulta01[0] = 0e0 ;
        resulta02[0] = 0e0 ;
        resulta03[0] = 0e0 ;
        resulta04[0] = 0e0 ;
        resulta05[0] = 0e0 ;
        resulta06[0] = 0e0 ;
        resulta07[0] = 0e0 ;
        resulta08[0] = 0e0 ;
        resulta09[0] = 0e0 ;
        resulta10[0] = 0e0 ;
        resulta11[0] = 0e0 ;
        resulta12[0] = 0e0 ;
        resulta13[0] = 0e0 ;
        resulta14[0] = 0e0 ;
        resulta15[0] = 0e0 ;
        resulta16[0] = 0e0 ;
        resulta17[0] = 0e0 ;
        resulta18[0] = 0e0 ;
        resulta19[0] = 0e0 ;
        resulta20[0] = 0e0 ;
        resulta21[0] = 0e0 ;
        resulta22[0] = 0e0 ;
        resulta23[0] = 0e0 ;
        resulta24[0] = 0e0 ;
        resulta25[0] = 0e0 ;
        resulta26[0] = 0e0 ;
        resulta27[0] = 0e0 ;
        resulta28[0] = 0e0 ;
        resulta29[0] = 0e0 ;
        resulta30[0] = 0e0 ;
  
        EOS eos_argon ("EOS_Cathare2", "ArgonIncondensable"   );
        EOS eos_azote ("EOS_Cathare2", "NitrogenIncondensable");
        EOS eos_melan ("EOS_Mixing");
        int nbcomp = 3 ;
        std::vector<EOS*> components(nbcomp);
        components[0] = obj_eos;
        components[1] = &eos_argon;
        components[2] = &eos_azote;
        eos_melan.set_components(components.data(), nbcomp);
 
        EOS_Fields vfentree(5) ;
        EOS_Field fpression("P",     "P", NEPTUNE::p,    nbpoint, pression); vfentree[0] = fpression ; 
        EOS_Field fenthalpi("h",     "h", NEPTUNE::h,    nbpoint, enthalpi); vfentree[1] = fenthalpi ;
        EOS_Field fmasfrac1 ("c_0",  "c_0", NEPTUNE::c_0,  nbpoint, mf__veau); vfentree[2] = fmasfrac1 ;
        EOS_Field fmasfrac2 ("c_1",  "c_1", NEPTUNE::c_1,  nbpoint, mf_argon); vfentree[3] = fmasfrac2 ;
        EOS_Field fmasfrac3 ("c_2",  "c_2", NEPTUNE::c_2,  nbpoint, mf_azote); vfentree[4] = fmasfrac3 ;
        EOS_Fields vfresultat(30); 
        EOS_Field fresulta01("T",                     "T", NEPTUNE::T,                     nbpoint,resulta01); vfresultat[0]  = fresulta01 ;
        EOS_Field fresulta02("hlsvsc",                "hlsvsc", NEPTUNE::hlsvsc,                nbpoint,resulta02); vfresultat[1]  = fresulta02 ;
        EOS_Field fresulta03("[d(hlsvsc)/dP]h",       "[d(hlsvsc)/dP]h", NEPTUNE::d_hlsvsc_dp_h ,      nbpoint,resulta03); vfresultat[2]  = fresulta03 ;
        EOS_Field fresulta04("[d(hlsvsc)/dh]P0",      "[d(hlsvsc)/dh]P0", NEPTUNE::d_hlsvsc_dh_p0,      nbpoint,resulta04); vfresultat[3]  = fresulta04 ;
        EOS_Field fresulta05("[d(hlsvsc)/dh]P",       "[d(hlsvsc)/dh]P", NEPTUNE::d_hlsvsc_dh_p,       nbpoint,resulta05); vfresultat[4]  = fresulta05 ;
        EOS_Field fresulta06("[d(hlsvsc)/d(c_1)]Ph",  "[d(hlsvsc)/d(c_1)]Ph", NEPTUNE::d_hlsvsc_d_c_1_ph,  nbpoint,resulta06); vfresultat[5]  = fresulta06 ;
        EOS_Field fresulta07("[d(hlsvsc)/d(c_2)]Ph",  "[d(hlsvsc)/d(c_2)]Ph", NEPTUNE::d_hlsvsc_d_c_2_ph,  nbpoint,resulta07); vfresultat[6]  = fresulta07 ;
        EOS_Field fresulta08("[d(hlsvsc)/d(c_3)]Ph",  "[d(hlsvsc)/d(c_3)]Ph", NEPTUNE::d_hlsvsc_d_c_3_ph,  nbpoint,resulta08); vfresultat[7]  = fresulta08 ;
        EOS_Field fresulta09("[d(hlsvsc)/d(c_4)]Ph",  "[d(hlsvsc)/d(c_4)]Ph", NEPTUNE::d_hlsvsc_d_c_4_ph,  nbpoint,resulta09); vfresultat[8]  = fresulta09 ;
        EOS_Field fresulta10("epstg",                 "epstg", NEPTUNE::epstg,                 nbpoint,resulta10); vfresultat[9]  = fresulta10 ;
        EOS_Field fresulta11("[d(epstg)/dP]h",        "[d(epstg)/dP]h", NEPTUNE::d_epstg_dp_h ,      nbpoint,resulta11); vfresultat[10] = fresulta11 ;
        EOS_Field fresulta12("[d(epstg)/dh]P",        "[d(epstg)/dh]P", NEPTUNE::d_epstg_dh_p ,       nbpoint,resulta12); vfresultat[11] = fresulta12 ;
        EOS_Field fresulta13("[d(epstg)/d(c_1)]Ph",   "[d(epstg)/d(c_1)]Ph", NEPTUNE::d_epstg_d_c_1_ph,   nbpoint,resulta13); vfresultat[12] = fresulta13 ;
        EOS_Field fresulta14("[d(epstg)/d(c_2)]Ph",   "[d(epstg)/d(c_2)]Ph", NEPTUNE::d_epstg_d_c_2_ph,   nbpoint,resulta14); vfresultat[13] = fresulta14 ;
        EOS_Field fresulta15("[d(epstg)/d(c_3)]Ph",   "[d(epstg)/d(c_3)]Ph", NEPTUNE::d_epstg_d_c_3_ph,   nbpoint,resulta15); vfresultat[14] = fresulta15 ;
        EOS_Field fresulta16("[d(epstg)/d(c_4)]Ph",   "[d(epstg)/d(c_4)]Ph", NEPTUNE::d_epstg_d_c_4_ph,   nbpoint,resulta16); vfresultat[15] = fresulta16 ;
        EOS_Field fresulta17("hvspsc",                "hvspsc", NEPTUNE::hvspsc ,               nbpoint,resulta17); vfresultat[16] = fresulta17 ;
        EOS_Field fresulta18("[d(hvspsc)/dP]h",       "[d(hvspsc)/dP]h", NEPTUNE::d_hvspsc_dp_h,      nbpoint,resulta18); vfresultat[17] = fresulta18 ;
        EOS_Field fresulta19("[d(hvspsc)/dh]P",       "[d(hvspsc)/dh]P", NEPTUNE::d_hvsvsc_dh_p ,     nbpoint,resulta19); vfresultat[18] = fresulta19 ;
        EOS_Field fresulta20("[d(hvspsc)/d(c_1)]Ph",  "[d(hvspsc)/d(c_1)]Ph", NEPTUNE::d_hvspsc_d_c_1_ph,  nbpoint,resulta20); vfresultat[19] = fresulta20 ;
        EOS_Field fresulta21("[d(hvspsc)/d(c_2)]Ph",  "[d(hvspsc)/d(c_2)]Ph", NEPTUNE::d_hvspsc_d_c_2_ph, nbpoint,resulta21); vfresultat[20] = fresulta21 ;
        EOS_Field fresulta22("[d(hvspsc)/d(c_3)]Ph",  "[d(hvspsc)/d(c_3)]Ph", NEPTUNE::d_hvspsc_d_c_3_ph , nbpoint,resulta22); vfresultat[21] = fresulta22 ;
        EOS_Field fresulta23("[d(hvspsc)/d(c_4)]Ph",  "[d(hvspsc)/d(c_4)]Ph", NEPTUNE::d_hvspsc_d_c_4_ph,  nbpoint,resulta23); vfresultat[22] = fresulta23 ;
        EOS_Field fresulta24("hvsvsc",                "hvsvsc", NEPTUNE::hvsvsc,                nbpoint,resulta24); vfresultat[23] = fresulta24 ;
        EOS_Field fresulta25("[d(hvsvsc)/dP]h",       "[d(hvsvsc)/dP]h", NEPTUNE::d_hvsvsc_dp_h ,      nbpoint,resulta25); vfresultat[24] = fresulta25 ;
        EOS_Field fresulta26("[d(hvsvsc)/dh]P",       "[d(hvsvsc)/dh]P", NEPTUNE::d_hvsvsc_dh_p,     nbpoint,resulta26); vfresultat[25] = fresulta26 ;
        EOS_Field fresulta27("[d(hvsvsc)/d(c_1)]Ph",  "[d(hvsvsc)/d(c_1)]Ph", NEPTUNE::d_hvsvsc_d_c_1_ph,  nbpoint,resulta27); vfresultat[26] = fresulta27 ;
        EOS_Field fresulta28("[d(hvsvsc)/d(c_2)]Ph",  "[d(hvsvsc)/d(c_2)]Ph", NEPTUNE::d_hvsvsc_d_c_2_ph,  nbpoint,resulta28); vfresultat[27] = fresulta28 ;
        EOS_Field fresulta29("[d(hvsvsc)/d(c_3)]Ph",  "[d(hvsvsc)/d(c_3)]Ph", NEPTUNE::d_hvsvsc_d_c_3_ph,  nbpoint,resulta29); vfresultat[28] = fresulta29 ;
        EOS_Field fresulta30("[d(hvsvsc)/d(c_4)]Ph",  "[d(hvsvsc)/d(c_4)]Ph", NEPTUNE::d_hvsvsc_d_c_4_ph,  nbpoint,resulta30); vfresultat[29] = fresulta30 ;
 
        EOS_Error_Field ferr(nbpoint, terr) ;
 
        er = eos_melan.compute(vfentree, vfresultat, ferr);
 
        cout << endl << endl ;
        cout << " With incondensable gas, hlsvsc, ... = f(P,h) : compute Fields in one point" << endl ;
        cout << " .  er                 = " << er              << endl ;
        if (er == 0)
           { cout << "   (i) Pressure              = " << vfentree[0][0]    <<  endl ;
             cout << "   (i) enthalpy              = " << vfentree[1][0]    <<  endl ;
             cout << "   (i) Frac.vap.H2O          = " << vfentree[2][0]    <<  endl ;
             cout << "   (i) Frac.Ag               = " << vfentree[3][0]    <<  endl ;
             cout << "   (i) Frac.N2               = " << vfentree[4][0]    <<  endl ;
             cout << "   ---" << endl ;
             cout << "   (o) Temperature           = " << vfresultat[0][0]  <<  endl ;
             cout << "   (o) hlsvsc                = " << vfresultat[1][0]  <<  endl ;
             cout << "   (o) [d(hlsvsc)/dP]h       = " << vfresultat[2][0]  <<  endl ;
             cout << "   (o) [d(hlsvsc)/dh]P0      = " << vfresultat[3][0]  <<  endl ;
             cout << "   (o) [d(hlsvsc)/dh]P       = " << vfresultat[4][0]  <<  endl ;
             cout << "   (o) [d(hlsvsc)/d(c_1)]Ph  = " << vfresultat[5][0]  <<  endl ;
             cout << "   (o) [d(hlsvsc)/d(c_2)]Ph  = " << vfresultat[6][0]  <<  endl ;
             cout << "   (o) [d(hlsvsc)/d(c_3)]Ph  = " << vfresultat[7][0]  <<  endl ;
             cout << "   (o) [d(hlsvsc)/d(c_4)]Ph  = " << vfresultat[8][0]  <<  endl ;
             cout << "   (o) epstg                 = " << vfresultat[9][0]  <<  endl ;
             cout << "   (o) [d(epstg)/dP]h        = " << vfresultat[10][0] <<  endl ;
             cout << "   (o) [d(epstg)/dh]P        = " << vfresultat[11][0] <<  endl ;
             cout << "   (o) [d(epstg)/d(c_1)]Ph   = " << vfresultat[12][0] <<  endl ;
             cout << "   (o) [d(epstg)/d(c_2)]Ph   = " << vfresultat[13][0] <<  endl ;
             cout << "   (o) [d(epstg)/d(c_3)]Ph   = " << vfresultat[14][0] <<  endl ;
             cout << "   (o) [d(epstg)/d(c_4)]Ph   = " << vfresultat[15][0] <<  endl ;
             cout << "   (o) hvspsc                = " << vfresultat[16][0] <<  endl ;
             cout << "   (o) [d(hvspsc)/dP]h       = " << vfresultat[17][0] <<  endl ;
             cout << "   (o) [d(hvspsc)/dh]P       = " << vfresultat[18][0] <<  endl ;
             cout << "   (o) [d(hvspsc)/d(c_1)]Ph  = " << vfresultat[19][0] <<  endl ;
             cout << "   (o) [d(hvspsc)/d(c_2)]Ph  = " << vfresultat[20][0] <<  endl ;
             cout << "   (o) [d(hvspsc)/d(c_3)]Ph  = " << vfresultat[21][0] <<  endl ;
             cout << "   (o) [d(hvspsc)/d(c_4)]Ph  = " << vfresultat[22][0] <<  endl ;
             cout << "   (o) hvsvsc                = " << vfresultat[23][0] <<  endl ;
             cout << "   (o) [d(hvsvsc)/dP]h       = " << vfresultat[24][0] <<  endl ;
             cout << "   (o) [d(hvsvsc)/dh]P       = " << vfresultat[25][0] <<  endl ;
             cout << "   (o) [d(hvsvsc)/d(c_1)]Ph  = " << vfresultat[26][0] <<  endl ;
             cout << "   (o) [d(hvsvsc)/d(c_2)]Ph  = " << vfresultat[27][0] <<  endl ;
             cout << "   (o) [d(hvsvsc)/d(c_3)]Ph  = " << vfresultat[28][0] <<  endl ;
             cout << "   (o) [d(hvsvsc)/d(c_4)]Ph  = " << vfresultat[29][0] <<  endl ;
           }
        else
           cout << "   Error With incondensable gas, hlsvsc, ... = f(P,h)" << endl ;
      }
 
      delete obj_eos ;
      endpar_iapwsvap: ;
      (void) er;
    }

  }

#endif

#ifdef WITH_PLUGIN_HITEC
  {
    cout<<endl<<endl;
    cout<<"--------------------------------------- "<<endl;
    cout<<"------ Test 15 HITEC------------------- "<<endl<<endl;

    Strings ref(1);
    ref[0]= AString("Liquid");
    EOS liquid("EOS_Hitec","HitecLiquid");
    ref[0]= AString("Vapor");
    EOS vapor("EOS_Hitec","HitecVapor");

    cout << endl << "**   eos " << endl;
    cout << "   * fluid   : " <<liquid.fluid_name()<<endl;
    cout << "   * table   : " <<liquid.table_name()<<endl;
    cout << "   * version : " <<liquid.version_name()<<endl<<endl;
    cout << liquid<<endl;
    test_features(liquid);

    cout << "**   eos " << endl;
    cout << "   * fluid   : " <<vapor.fluid_name()<<endl;
    cout << "   * table   : " <<vapor.table_name()<<endl;
    cout << "   * version : " <<vapor.version_name()<<endl<<endl;
    cout << vapor<<endl;
    test_features(vapor);

    double pcrit,tcrit;
    EOS_Error cr;

    // Valeurs critiques (CRITP)
    cout << endl << endl << "----- critp-------" << endl;
    cr = liquid.get_p_crit(pcrit);
    cout << "[cr="<<cr<<"]  Pc    : " << pcrit << endl;
    cr = liquid.get_T_crit(tcrit);
    cout << "[cr="<<cr<<"]  Tc    : " << tcrit << endl;

    // (compute by field or fields)
    int n=3;
    int ncout=1; // (ncout<=n)

    ArrOfDouble xp(n);
    for(int i=0; i<n; i++)
      xp[i]=(i+1)*1.e5;
    EOS_Field P("Pressure","p", NEPTUNE::p,xp);

    ArrOfInt ierr(n);
    EOS_Error_Field err(ierr);

    // (field - field)
    std::vector<double> ptsat(n);;
    EOS_Field Tsat("tsat","T_sat", NEPTUNE::T_sat,n,ptsat.data());

    cr=liquid.compute(P,Tsat,err);
    cout << "* field field [cr=" << cr <<"]"<< endl;
    for(int i=0; i<ncout; i++)
      {
        cout << "in "<<P.get_property_title()<<" "<<P[i]<<endl;
        cout <<" out "<<Tsat.get_property_title()<<" "<<Tsat[i]<<endl;
      }

    // (field - fields)
    EOS_Fields output(2);
    ArrOfDouble xdtsat(n);
    output[0]=Tsat;
    output[1]=EOS_Field("dtsat","d_T_sat_d_p", NEPTUNE::d_T_sat_d_p,xdtsat);

    cr=liquid.compute(P,output,err);
    cout << endl << "* field fields [cr=" << cr <<"]"<< endl;
    for(int i=0; i<ncout; i++)
      {
        cout <<"in "<<P.get_property_title()<<" "<<P[i]<<endl;
        cout <<" out "<<output[0].get_property_title()<<" "<<output[0][i]<<endl;
        cout <<" out "<<output[1].get_property_title()<<" "<<output[1][i]<<endl;
      }

    // (field - field - field)
    ArrOfDouble xh(n);
    ArrOfDouble xt(n);

    for(int i=0; i<n; i++)
      xh[i]=(i+1)*1.e5;

    EOS_Field h("Enthalpy","h", NEPTUNE::h,xh);
    EOS_Field temp("Temperature","T", NEPTUNE::T,xt);

    cr=liquid.compute(P,h,temp,err);
    cout << endl<< "* field field field [cr=" << cr <<"]"<< endl;
    for(int i=0; i<ncout; i++)
      {
        cout <<"in "<<P.get_property_title()<<" "<<P[i]<<endl;
        cout <<"in "<<h.get_property_title()<<" "<<h[i]<<endl;
        cout <<" out "<<temp.get_property_title()<<" "<<temp[i]<<endl;
      }

    for(int i=0; i<n; i++)
      temp[i]=0.;

    // (field - field - fields)
    ArrOfDouble xrho(n);
    int nout=2;
    EOS_Fields output2(nout);
    output2[0]=temp;
    output2[1]=EOS_Field("Rho","rho", NEPTUNE::rho,xrho);

    cr=liquid.compute(P,h,output2,err);
    cout << endl<< "* field field fields [cr=" << cr <<"]"<< endl;
    for(int i=0; i<ncout; i++)
      {
        cout <<"in "<<P.get_property_title()<<" "<<P[i]<<endl;
        cout <<"in "<<h.get_property_title()<<" "<<h[i]<<endl;
        for(int j=0; j<nout; j++)
          {
            cout<<" out "<< output2[j].get_property_title()<<" "<<output2[j][i] << endl ;
            output2[j][i]=-1.;
          }
      }

    // (fields - fields)

    EOS_Fields input(2);
    input[0]=P;
    input[1]=h;

    cr=liquid.compute(input,output2,err);
    cout << endl<< "* fields fields [cr=" << cr <<"]"<< endl;
    for(int i=0; i<ncout; i++)
      {
        cout <<"in "<<input[0].get_property_title()<<" "<<input[0][i]<<endl;
        cout <<"in "<<input[1].get_property_title()<<" "<<input[1][i]<<endl;
        for(int j=0; j<nout; j++)
          {
            cout<<" out "<< output2[j].get_property_title()<<" "<<output2[j][i] << endl ;
            output2[j][i]=-1.;
          }
      }

    // (fields - field)

    cr=liquid.compute(input,temp,err);
    cout << endl<< "* fields field [cr=" << cr <<"]"<< endl;
    for(int i=0; i<ncout; i++)
      {
        cout <<"in "<<input[0].get_property_title()<<" "<<input[0][i]<<endl;
        cout <<"in "<<input[1].get_property_title()<<" "<<input[1][i]<<endl;
        for(int i=0; i<ncout; i++)
          cout<<" out "<< temp.get_property_title()<<" "<<temp[i] << endl ;
      }

   // (compute by point)

    double pp=1.e5;
    double tt=200.+273.15;
    double hh=-1;
    double val=-1.;

    cr=liquid.compute_h_pT(pp, tt, hh);
    cout<<"[cr="<<cr<<"] in p "<<pp<<" in T "<<tt<<" out h " <<hh<<endl;
    tt=-1.;
    cr=liquid.compute_T_ph(pp, hh, tt);
    cout<<"[cr="<<cr<<"] in p "<<pp<<" in h "<<hh<<" out T " <<tt<<endl;
    tt=-1.;
    cr=liquid.compute("T",pp, hh, tt);
    cout<<"[cr="<<cr<<"] in p "<<p<<" in hh "<<h<<" out T " <<tt<<endl;
    tt=-1.;
    cr=liquid.compute_T_sat_p(pp, val);
    cout<<"[cr="<<cr<<"] in p "<<pp<<"            out T_sat " <<val<<endl;
    val=-1;
    cr=liquid.compute("d_T_sat_d_p",pp, val);
    cout<<"[cr="<<cr<<"] in p "<<pp<<"             out d_T_sat_d_p " <<val<<endl;
    val=-1;
    cr=liquid.compute_rho_ph(pp, hh, val);
    cout<<"[cr="<<cr<<"] in p "<<pp<<" in h "<<hh<<" out rho " <<val<<endl;
    val=-1;
    cr=liquid.compute("d_rho_d_h_p", pp, hh, val);
    cout<<"[cr="<<cr<<"] in p "<<pp<<" in h "<<hh<<" out d_rho_d_h_p " <<val<<endl;


    tt=1000.+273.15;
    cr=vapor.compute_h_pT(pp, tt, hh);
    cout<<"[cr="<<cr<<"] in p "<<pp<<" in T "<<tt<<" out h " <<hh<<endl;
    tt=-1.;
    cr=vapor.compute_T_ph(pp, hh, tt);
    cout<<"[cr="<<cr<<"] in p "<<pp<<" in h "<<hh<<" out T " <<tt<<endl;
    tt=-1.;
    cr=vapor.compute("T",pp, hh, tt);
    cout<<"[cr="<<cr<<"] in p "<<p<<" in hh "<<h<<" out T " <<tt<<endl;
    tt=-1.;
    cr=vapor.compute_T_sat_p(pp, val);
    cout<<"[cr="<<cr<<"] in p "<<pp<<"            out T_sat " <<val<<endl;
    val=-1;
    cr=vapor.compute("d_T_sat_d_p",pp, val);
    cout<<"[cr="<<cr<<"] in p "<<pp<<"             out d_T_sat_d_p " <<val<<endl;
    val=-1;
    cr=vapor.compute_rho_ph(pp, hh, val);
    cout<<"[cr="<<cr<<"] in p "<<pp<<" in h "<<hh<<" out rho " <<val<<endl;
    val=-1;
    cr=vapor.compute("d_rho_d_h_p", pp, hh, val);
    cout<<"[cr="<<cr<<"] in p "<<pp<<" in h "<<hh<<" out d_rho_d_h_p " <<val<<endl;

  }
#endif //WITH_PLUGIN_HITEC

#ifdef WITH_PLUGIN_NAK
  {
    cout<<endl<<endl;
    cout<<"--------------------------------------- "<<endl;
    cout<<"------ Test 16 NAK------------------- "<<endl<<endl;

    Strings ref(1);
    ref[0]= AString("Liquid");
    EOS liquid("EOS_Nak","NakLiquid");
    ref[0]= AString("Vapor");
    EOS vapor("EOS_Nak","NakVapor");

    cout << endl << "**   eos " << endl;
    cout << "   * fluid   : " <<liquid.fluid_name()<<endl;
    cout << "   * table   : " <<liquid.table_name()<<endl;
    cout << "   * version : " <<liquid.version_name()<<endl<<endl;
    cout << liquid<<endl;
    test_features(liquid);

    cout << "**   eos " << endl;
    cout << "   * fluid   : " <<vapor.fluid_name()<<endl;
    cout << "   * table   : " <<vapor.table_name()<<endl;
    cout << "   * version : " <<vapor.version_name()<<endl<<endl;
    cout << vapor<<endl;
    test_features(vapor);

    double pcrit,tcrit;
    EOS_Error cr;

    // Valeurs critiques (CRITP)
    cout << endl << endl << "----- critp-------" << endl;
    cr = liquid.get_p_crit(pcrit);
    cout << "[cr="<<cr<<"]  Pc    : " << pcrit << endl;
    cr = liquid.get_T_crit(tcrit);
    cout << "[cr="<<cr<<"]  Tc    : " << tcrit << endl;

    // (compute by field or fields)
    int n=3;
    int ncout=1; // (ncout<=n)

    ArrOfDouble xp(n);
    for(int i=0; i<n; i++)
      xp[i]=(i+1)*1.e5;
    EOS_Field P("Pressure","p", NEPTUNE::p,xp);

    ArrOfInt ierr(n);
    EOS_Error_Field err(ierr);

    // (field - field)
    std::vector<double> ptsat(n);
    EOS_Field Tsat("tsat","T_sat", NEPTUNE::T_sat,n,ptsat.data());

    cr=liquid.compute(P,Tsat,err);
    cout << "* field field [cr=" << cr <<"]"<< endl;
    for(int i=0; i<ncout; i++)
      {
        cout << "in "<<P.get_property_title()<<" "<<P[i]<<endl;
        cout <<" out "<<Tsat.get_property_title()<<" "<<Tsat[i]<<endl;
      }

    // (field - fields)
    EOS_Fields output(2);
    ArrOfDouble xdtsat(n);
    output[0]=Tsat;
    output[1]=EOS_Field("dtsat","d_T_sat_d_p", NEPTUNE::d_T_sat_d_p,xdtsat);

    cr=liquid.compute(P,output,err);
    cout << endl << "* field fields [cr=" << cr <<"]"<< endl;
    for(int i=0; i<ncout; i++)
      {
        cout <<"in "<<P.get_property_title()<<" "<<P[i]<<endl;
        cout <<" out "<<output[0].get_property_title()<<" "<<output[0][i]<<endl;
        cout <<" out "<<output[1].get_property_title()<<" "<<output[1][i]<<endl;
      }

    // (field - field - field)
    ArrOfDouble xh(n);
    ArrOfDouble xt(n);

    for(int i=0; i<n; i++)
      xh[i]=(i+1)*1.e5;

    EOS_Field h("Enthalpy","h", NEPTUNE::h,xh);
    EOS_Field temp("Temperature","T", NEPTUNE::T,xt);

    cr=liquid.compute(P,h,temp,err);
    cout << endl<< "* field field field [cr=" << cr <<"]"<< endl;
    for(int i=0; i<ncout; i++)
      {
        cout <<"in "<<P.get_property_title()<<" "<<P[i]<<endl;
        cout <<"in "<<h.get_property_title()<<" "<<h[i]<<endl;
        cout <<" out "<<temp.get_property_title()<<" "<<temp[i]<<endl;
      }

    for(int i=0; i<n; i++)
      temp[i]=0.;

    // (field - field - fields)
    ArrOfDouble xrho(n);
    int nout=2;
    EOS_Fields output2(nout);
    output2[0]=temp;
    output2[1]=EOS_Field("Rho","rho", NEPTUNE::rho,xrho);

    cr=liquid.compute(P,h,output2,err);
    cout << endl<< "* field field fields [cr=" << cr <<"]"<< endl;
    for(int i=0; i<ncout; i++)
      {
        cout <<"in "<<P.get_property_title()<<" "<<P[i]<<endl;
        cout <<"in "<<h.get_property_title()<<" "<<h[i]<<endl;
        for(int j=0; j<nout; j++)
          {
            cout<<" out "<< output2[j].get_property_title()<<" "<<output2[j][i] << endl ;
            output2[j][i]=-1.;
          }
      }

    // (fields - fields)

    EOS_Fields input(2);
    input[0]=P;
    input[1]=h;

    cr=liquid.compute(input,output2,err);
    cout << endl<< "* fields fields [cr=" << cr <<"]"<< endl;
    for(int i=0; i<ncout; i++)
      {
        cout <<"in "<<input[0].get_property_title()<<" "<<input[0][i]<<endl;
        cout <<"in "<<input[1].get_property_title()<<" "<<input[1][i]<<endl;
        for(int j=0; j<nout; j++)
          {
            cout<<" out "<< output2[j].get_property_title()<<" "<<output2[j][i] << endl ;
            output2[j][i]=-1.;
          }
      }

    // (fields - field)

    cr=liquid.compute(input,temp,err);
    cout << endl<< "* fields field [cr=" << cr <<"]"<< endl;
    for(int i=0; i<ncout; i++)
      {
        cout <<"in "<<input[0].get_property_title()<<" "<<input[0][i]<<endl;
        cout <<"in "<<input[1].get_property_title()<<" "<<input[1][i]<<endl;
        for(int i=0; i<ncout; i++)
          cout<<" out "<< temp.get_property_title()<<" "<<temp[i] << endl ;
      }

    // (compute by point)

    double pp=1.e5;
    double tt=200.+273.15;
    double hh=-1;
    double val=-1.;

    cr=liquid.compute_h_pT(pp, tt, hh);
    cout<<"[cr="<<cr<<"] in p "<<pp<<" in T "<<tt<<" out h " <<hh<<endl;
    tt=-1.;
    cr=liquid.compute_T_ph(pp, hh, tt);
    cout<<"[cr="<<cr<<"] in p "<<pp<<" in h "<<hh<<" out T " <<tt<<endl;
    tt=-1.;
    cr=liquid.compute("T",pp, hh, tt);
    cout<<"[cr="<<cr<<"] in p "<<p<<" in hh "<<h<<" out T " <<tt<<endl;
    tt=-1.;
    cr=liquid.compute_T_sat_p(pp, val);
    cout<<"[cr="<<cr<<"] in p "<<pp<<"            out T_sat " <<val<<endl;
    val=-1;
    cr=liquid.compute("d_T_sat_d_p",pp, val);
    cout<<"[cr="<<cr<<"] in p "<<pp<<"             out d_T_sat_d_p " <<val<<endl;
    val=-1;
    cr=liquid.compute_rho_ph(pp, hh, val);
    cout<<"[cr="<<cr<<"] in p "<<pp<<" in h "<<hh<<" out rho " <<val<<endl;
    val=-1;
    cr=liquid.compute("d_rho_d_h_p", pp, hh, val);
    cout<<"[cr="<<cr<<"] in p "<<pp<<" in h "<<hh<<" out d_rho_d_h_p " <<val<<endl;


    tt=1000.+273.15;
    cr=vapor.compute_h_pT(pp, tt, hh);
    cout<<"[cr="<<cr<<"] in p "<<pp<<" in T "<<tt<<" out h " <<hh<<endl;
    tt=-1.;
    cr=vapor.compute_T_ph(pp, hh, tt);
    cout<<"[cr="<<cr<<"] in p "<<pp<<" in h "<<hh<<" out T " <<tt<<endl;
    tt=-1.;
    cr=vapor.compute("T",pp, hh, tt);
    cout<<"[cr="<<cr<<"] in p "<<p<<" in hh "<<h<<" out T " <<tt<<endl;
    tt=-1.;
    cr=vapor.compute_T_sat_p(pp, val);
    cout<<"[cr="<<cr<<"] in p "<<pp<<"            out T_sat " <<val<<endl;
    val=-1;
    cr=vapor.compute("d_T_sat_d_p",pp, val);
    cout<<"[cr="<<cr<<"] in p "<<pp<<"             out d_T_sat_d_p " <<val<<endl;
    val=-1;
    cr=vapor.compute_rho_ph(pp, hh, val);
    cout<<"[cr="<<cr<<"] in p "<<pp<<" in h "<<hh<<" out rho " <<val<<endl;
    val=-1;
    cr=vapor.compute("d_rho_d_h_p", pp, hh, val);
    cout<<"[cr="<<cr<<"] in p "<<pp<<" in h "<<hh<<" out d_rho_d_h_p " <<val<<endl;

  }
#endif //WITH_PLUGIN_NAK
   
  cout<<endl<<endl;
  cout<<"--------------------------------------- "<<endl;
  cout<<"---------------- End ------------------ "<<endl;
  cout<<"--------------------------------------- "<<endl;
  cout <<endl<<endl;

  Language_finalize();
  return 0;
}
