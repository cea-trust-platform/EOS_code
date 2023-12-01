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
#include "EOS/API/EOS_Std_Error_Handler.hxx"
#include "EOS/API/EOS_Config.hxx"
#include "EOS_IGen/API/EOS_IGen.hxx"

using namespace NEPTUNE;
using namespace NEPTUNE_EOS_IGEN;


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
  int nb_methodes=0;
  Strings methodes(0);

#ifdef WITH_PLUGIN_THETIS
  nb_methodes=nb_methodes+1;
  methodes.resize(nb_methodes);
  methodes[nb_methodes-1]="EOS_Thetis";
#endif
#ifdef WITH_PLUGIN_REFPROP_9
  nb_methodes=nb_methodes+1;
  methodes.resize(nb_methodes);
  methodes[nb_methodes-1]="EOS_Refprop9";
#endif
#ifdef WITH_PLUGIN_CATHARE2
  nb_methodes=nb_methodes+1;
  methodes.resize(nb_methodes);
  methodes[nb_methodes-1]="EOS_Cathare2";
#endif
  
  for (int m=0; m<nb_methodes ; m++)
  {

//     test continuite
//     cout<<endl<<"Test continuite : "<<methodes[m] <<endl<<endl;
    {
      double n=10000;
      double p=1.829e7;
      double h=1.5e5;
      if (methodes[m]=="EOS_Cathare2") 
	h=8.0e5;
      // double hmin=1.22e5;
      // double hmax=9.80e5;
      // double del_h=(hmax-hmin)/float(n-1);
      
      double pmin=1.3e7;
      double pmax=2.0e7;
      double del_p=(pmax-pmin)/float(n-1);
      
      ArrOfDouble h_i(n,h);
      ArrOfDouble p_i(n,p);
      ArrOfDouble cp_ipp(n);
      ArrOfDouble cp_ipp_r(n);
      ArrOfDouble cp_ipp_n(n);
      ArrOfDouble cp_eos(n);
      
//       h_i[0]=hmin;
//       for (int i=1; i<n ; i++)
// 	h_i[i]=h_i[i-1]+del_h;
      
      p_i[0]=pmin;
      for (int i=1; i<n ; i++)
	  p_i[i]=p_i[i-1]+del_p;

      EOS_Field h_f("h","h",h_i);
      EOS_Field p_f("p","p",p_i);
      EOS_Field cp_f_ipp("cp","cp",cp_ipp);
      EOS_Field cp_f_ipp_r("cp","cp",cp_ipp_r);
      EOS_Field cp_f_ipp_n("cp","cp",cp_ipp_n);
      EOS_Field cp_f_eos("cp","cp",cp_eos);
      
      AString med_file = "sans_raffinement_";
      med_file+=methodes[m];
      EOS obj_ipp_r("EOS_Ipp",med_file.aschar());
      for (int i=0 ; i<n ; i++)
	  obj_ipp_r.compute_cp_ph(p_f[i], h_f[i], cp_f_ipp_r[i]);
      
      med_file = "raffinement_local_";
      med_file+=methodes[m];
      EOS obj_ipp("EOS_Ipp",med_file.aschar());
      for (int i=0 ; i<n ; i++)
	  obj_ipp.compute_cp_ph(p_f[i], h_f[i], cp_f_ipp[i]);
      
      med_file = "raffinement_local_non_continu_";
      med_file+=methodes[m];
      EOS obj_ipp_n("EOS_Ipp",med_file.aschar());
      for (int i=0 ; i<n ; i++)
	  obj_ipp_n.compute_cp_ph(p_f[i], h_f[i], cp_f_ipp_n[i]);

      
      EOS obj_eos(methodes[m].aschar(),"WaterLiquid");
      for (int i=0 ; i<n ; i++)
	  obj_eos.compute_cp_ph(p_f[i], h_f[i], cp_f_eos[i]);

      cout<<"p ; h ; cp ipp continu ; cp "<<methodes[m]<<" ; cp non continu ; cp sans raffinement"<<endl;
      for (int i=0 ; i<n ; i++)
	cout<<p_f[i]<<" ; "<<h_f[i]<<" ; "<<cp_f_ipp[i]<<" ; "<< cp_f_eos[i]<<"  ; "<<cp_f_ipp_n[i]<<
	" ; "<<cp_f_ipp_r[i] <<endl;
      cout<<endl;
    }
    
    cout<<endl<<"Test retour d'erreur"<<endl<<endl;
    {
      int np=5;
      int nh=6;
      int n=np*nh;
      
      AString med_file = "sans_raffinement_";
      med_file+=methodes[m];
      
      EOS obj_ipp("EOS_Ipp",med_file.aschar());
      EOS obj_eos(methodes[m].aschar(),"WaterLiquid");
      cout<<"**   eos"<<endl;
      cout<<obj_ipp<<endl;
      ArrOfDouble p_i(n);
      ArrOfDouble h_i(n);
      ArrOfDouble T_ipp(n);
      ArrOfDouble T_eos(n);
      
      double pmin, pmax, hmin, hmax;

      obj_ipp.get_p_min(pmin);
      obj_ipp.get_p_max(pmax);
      obj_ipp.get_h_min(hmin);
      obj_ipp.get_h_max(hmax);
      
      double del_p=(pmax-pmin)/(np-1);
      double del_h=(hmax-hmin)/(nh-1);
      
//       p_i[0]=pmin+(del_p/2.0);
//       h_i[0]=hmin+(del_h/2.0);
      int k=0;
      for (int i=0 ; i<(np) ; i++)
      {
	p_i[k]=(pmin+(del_p/2.0))+i*del_p;
	h_i[k]=hmin+(del_h/2.0);
	k++;
	for (int j=1 ; j<(nh); j++)
	{
	  p_i[k]=p_i[k-1];
	  h_i[k]=h_i[k-1]+del_h;
	  k++;
	}
      }
      
      EOS_Field p_f("p","p",p_i);
      EOS_Field h_f("h","h",h_i);
      
     
      EOS_Fields input(2);
      input[0]=p_f;
      input[1]=h_f;
      
      
      EOS_Field output_ipp("T", "T", T_ipp);
      EOS_Field output_eos("T", "T", T_eos);
      
      ArrOfInt ierr_ipp(n);
      ArrOfInt ierr_eos(n);
      EOS_Error_Field err_ipp(ierr_ipp);
      EOS_Error_Field err_eos(ierr_eos);
      
      
      obj_ipp.compute(input,output_ipp,err_ipp);
      obj_eos.compute(input,output_eos,err_eos);
      
      
      for (int i=0 ; i<n ; i++)
         { AString des_err_ipp ;
           AString des_err_eos ;
           
           obj_ipp.fluid().describe_error(err_ipp[i], des_err_ipp) ;
           obj_eos.fluid().describe_error(err_eos[i], des_err_eos) ;
           
           cout<< "pmin " << pmin << "; pmax " << pmax << "; hmin " << hmin
               << "; hmax " << hmax <<endl ;
           
           cout<< "p " << input[0][i]<< " ; h " << input[1][i]<< " ; T ipp " << output_ipp[i]
               << " ; T eos " << output_eos[i] <<endl;
           cout<< "ipp error [cr = " << err_ipp[i].generic_error() << "] description: " << des_err_ipp <<endl;
           cout<< "eos error [cr = " << err_eos[i].generic_error() << "] description: " << des_err_eos <<endl<<endl;
         }
        
      EOS_Std_Error_Handler handler;
      handler.set_exit_on_error(EOS_Std_Error_Handler::disable_feature);
      handler.set_throw_on_error(EOS_Std_Error_Handler::disable_feature);
      handler.set_dump_on_error(EOS_Std_Error_Handler::disable_feature);
      handler.set_dump_stream(cerr);
      handler.set_throw_on_error(ok);
      
      obj_ipp.save_error_handler();
      obj_ipp.set_error_handler(handler);
      
      obj_eos.save_error_handler();
      obj_eos.set_error_handler(handler);
      
      
      obj_eos.set_error_handler(handler);
      
      const char *ptr_thep ;
      int stz = thermprop_r.size() ;
      for (int i=0; i<stz; i++)
         { ptr_thep = thermprop_r[i].c_str() ;
           double out=0.0;
           double p=125000.0;
           double h=2.86822e+06;
           AString des_err_ipp, des_err_eos;
           EOS_Internal_Error ierr_eos = EOS_Internal_Error::OK;
           EOS_Internal_Error ierr_ipp = EOS_Internal_Error::OK;
           
           try
              { obj_ipp.compute(ptr_thep,p,h,out) ;
              }
           catch (EOS_Std_Exception ex)
              { ierr_ipp = ex.err_code ;
              }
           
           try
              { obj_eos.compute(ptr_thep,p,h,out) ;
              }
           catch (EOS_Std_Exception ex)
              { ierr_eos = ex.err_code ;
              }
          
//            if (ierr_ipp.generic_error()!=good ||ierr_eos.generic_error()!=good)
//         {
// //    	Sometime there is "not in db file" error and eos method is 
// //    	valid because bdds are produced by field,
// //    	here is calculation is by point
             obj_ipp.fluid().describe_error(ierr_ipp, des_err_ipp) ;
             obj_eos.fluid().describe_error(ierr_eos, des_err_eos) ;
             cout << ptr_thep << " ipp error [cr = " << ierr_ipp.generic_error() 
                              << "] description: "   << des_err_ipp<<endl ;
             cout << ptr_thep << " eos error [cr = " << ierr_eos.generic_error() 
                              << "] description: "   << des_err_eos<<endl<<endl ;
//         }
           
         }
      
    }
    
  }

  cout<<endl<<endl ;
  cout<< "--------------------------------------- " <<endl ;
  cout<< "---------------- End ------------------ " <<endl ;
  cout<< "--------------------------------------- " <<endl ;
  cout <<endl<<endl ;

  Language_finalize() ;
  return 0;
}
