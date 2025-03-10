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



#include "Cathare2.hxx"

extern "C" 
{ void F77NAME(c2_erpile)(char* format) 
  { 
    format[119]='\0';
    cerr << "in erpile=" << format << endl ;
    CATHARE2::CATHARE2::in_erpile = true ;
  }
}

namespace CATHARE2
{
  bool CATHARE2::in_erpile = false          ;
  const double CATHARE2::tabsk = 273.15e0   ;
  TH_space CATHARE2::typ_ths = TH_space::no ;

  CATHARE2::CATHARE2(NEPTUNE::EOS_Fluid* fld, domain ph, int c2ref) :
      nincon(0), un(1.e0), zero(0.e0), epspp(1.e3),
      fluid(fld), phase(ph), 
      fldm(-1.e0),
      tc(-1.e0), hc(-1.e0), 
      critical(EOS_Error::bad),
      lfluid(c2ref), licar(0), licargas(0), lienc(0), lirev(61),
      litermin(0), llatypml(0), lmutypml(0), liter(1), ligotra(0)
  {
    F77NAME(c2_inifld)() ;
    F77NAME(c2_getfldprop)(lfluid, 
                        xpm, xhlm, xhvm, xtlm, xtgm,
                        xpp, xhlp, xhvp, xtlp, xtgp,
                        xtgpic, xsip,
                        fldm, fldr, flddv,
                        cpcst,
                        epsl, epsg, epsr,
                        epsp, epshl, epshg, epstl,
                        epstg, epsal, epsmx, epspx,
                        epsv, epstw, epsmi, epsms,
                        epsei, epses, utol, utog,
                        href, tref, trefk, pc) ;
    // temporary
    xpcri=pc;
    pcarg.resize(1);
    pcarg[0]=pc;
    // temporary
  }

  CATHARE2::CATHARE2(const CATHARE2& )
  { assert(-1) ;
  }

  CATHARE2::~CATHARE2()
  {
  }

  EOS_Error CATHARE2::verify(const EOS_Field& in,
                             EOS_Error_Field& errfield, 
                             const domain dom) const
  { double tabsk_fluid = 0.e0 ;
    double P_min       = 0.e0 ;
    double P_max       = 0.e0 ;
    double h_min       = 0.e0 ;
    double h_max       = 0.e0 ;
    double T_min       = 0.e0 ;
    double T_max       = 0.e0 ;
    
    if (lfluid != 100003) tabsk_fluid = tabsk ;
    switch (dom) 
       { case unknown:
         case saturated:
           P_min = xpm;
           P_max = xpp;
           h_min = xhlm;
           h_max = xhvp;
           T_min = xtlm + tabsk_fluid;
           T_max = xtgp + tabsk_fluid;
           break;
         case liquid:
           P_min = xpm;
           P_max = xpp;
           h_min = xhlm;
           h_max = xhlp;
           T_min = xtlm + tabsk_fluid;
           T_max = xtlp + tabsk_fluid;
           break;
         case vapor:
           P_min = xpm;
           P_max = xpp;
           h_min = xhvm;
           h_max = xhvp;
           T_min = xtgm + tabsk_fluid;
           T_max = xtgp + tabsk_fluid;
           break;
       }
    int partial_error = ok ;
    EOS_Property prop = in.get_property_number() ;
    for (int i=0; i<in.size(); i++) 
       { switch(prop)
            { case NEPTUNE::p:
                 if (in[i] < P_min)  partial_error = P_below_min ;
                 if (in[i] > P_max)  partial_error = P_above_max ;
                 break ;
              case NEPTUNE::T:
                 if (in[i] < T_min)  partial_error = T_below_min ;
                 if (in[i] > T_max)  partial_error = T_above_max ;
                 break ;
              case NEPTUNE::h:
                 if (in[i] < h_min)  partial_error = h_below_min ;
                 if (in[i] > h_max)  partial_error = h_above_max ;
                 break ;
              default:
                 break ;
            }
         errfield.set(i,convert_eos_error(partial_error)) ;
       }
    return errfield.find_worst_error().generic_error() ;
  }

  int CATHARE2::map_eos_field(const EOS_Field& f, domain mode)
  { assert(f.size() == nsca) ;

    switch(f.get_property_number())
       { // Thermodynamic Properties 
         case NEPTUNE::p:
            lp.set_ptr(nsca, f.get_data().get_ptr());
            break;
         case NEPTUNE::h:
            if (mode == liquid) 
               { if (typ_ths == TH_space::PT)
                    lhlpt.set_ptr(nsca, f.get_data().get_ptr());
                 else
                    lhl.set_ptr(nsca, f.get_data().get_ptr());
               }
            else if (mode == vapor) 
               {  if (nincon == 0 && typ_ths == TH_space::PT)
                     lhvpt.set_ptr(nsca, f.get_data().get_ptr());
                  else
                     { lhl.set_ptr(nsca, f.get_data().get_ptr()); // epstl l/v
                       lhg.set_ptr(nsca, f.get_data().get_ptr());
                     }
               }
            else if (mode == unknown) lh.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;
            break;
         case NEPTUNE::T:
            if (mode == liquid) ltl.set_ptr(nsca, f.get_data().get_ptr());
            else if (mode == vapor) ltg.set_ptr(nsca, f.get_data().get_ptr());
            else if (mode == unknown) lt.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;
            break;
         case NEPTUNE::rho:
            if (mode == liquid) 
               { if (typ_ths == TH_space::PT)
                    lrlpt.set_ptr(nsca, f.get_data().get_ptr());
                 else
                    lrl.set_ptr(nsca, f.get_data().get_ptr());
               }
            else if (mode == vapor)
               { if (nincon == 0 && typ_ths == TH_space::PT)
                   lrvpt.set_ptr(nsca, f.get_data().get_ptr());
                 else
                   lrg.set_ptr(nsca, f.get_data().get_ptr());
               }
            else if (mode == unknown) lr.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;
            break;
         case NEPTUNE::cp:
            if (mode == liquid) 
                { if (typ_ths == TH_space::PT)
                     lcplpt.set_ptr(nsca, f.get_data().get_ptr());
                  else
                     lcpl.set_ptr(nsca, f.get_data().get_ptr());
                }
            else if (mode == vapor) 
               { if (nincon == 0 && typ_ths == TH_space::PT)
                    lcpvpt.set_ptr(nsca, f.get_data().get_ptr()) ;
                 else
                    lcpg.set_ptr(nsca, f.get_data().get_ptr()) ;
               }
            else if (mode == unknown) lcp.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;
            break;
         case NEPTUNE::beta:
            if (mode == liquid) lbetal.set_ptr(nsca, f.get_data().get_ptr());
            else if (mode == unknown) lbetal.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;
            break;
         case NEPTUNE::lambda:
            if (mode == liquid) ltlal.set_ptr(nsca, f.get_data().get_ptr());
            else if (mode == vapor) ltlag.set_ptr(nsca, f.get_data().get_ptr());
            else if (mode == unknown) lla.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;
            break;
         case NEPTUNE::mu:
            if (mode == liquid) ltmul.set_ptr(nsca, f.get_data().get_ptr());
            else if (mode == vapor) ltmug.set_ptr(nsca, f.get_data().get_ptr());
            else if (mode == unknown) lmu.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;
            break;
         case NEPTUNE::sigma:
            if (mode == vapor) lsi.set_ptr(nsca, f.get_data().get_ptr());
            else if (mode == unknown) lsi.set_ptr(nsca, f.get_data().get_ptr());
            break;
         case NEPTUNE::gamma:
            lkiseng.set_ptr(nsca, f.get_data().get_ptr()); break;
         case NEPTUNE::pr:
            if (mode == vapor) lprandg.set_ptr(nsca, f.get_data().get_ptr());
            if (mode == unknown) lprandg.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;
            break;
         // First Derivatives
         case NEPTUNE::d_T_d_p_h:
            if (mode == liquid) ltl1.set_ptr(nsca, f.get_data().get_ptr());
            else if (mode == vapor) ltg1.set_ptr(nsca, f.get_data().get_ptr());
            else if (mode == unknown) lt1.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;
            break;
         case NEPTUNE::d_T_d_h_p:
            if (mode == liquid) ltl2.set_ptr(nsca, f.get_data().get_ptr());
            else if (mode == vapor) ltg3.set_ptr(nsca, f.get_data().get_ptr());
            else if (mode == unknown) lt2.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;
            break;
         case NEPTUNE::d_h_d_T_p:
            if (mode == liquid) 
               { if (typ_ths == TH_space::PT)
                   lhl2pt.set_ptr(nsca, f.get_data().get_ptr());
                 else
                   lhl2.set_ptr(nsca, f.get_data().get_ptr());
                }
            else if (mode == vapor) 
               { if (nincon == 0 && typ_ths == TH_space::PT)
                    lcpvpt.set_ptr(nsca, f.get_data().get_ptr()) ;
                 else
                    lcpg.set_ptr(nsca, f.get_data().get_ptr()) ;
               }
            else return 0;
            break;
         case NEPTUNE::d_h_d_p_T:
            if (mode == liquid) 
                { if (typ_ths == TH_space::PT)
                     lhl1pt.set_ptr(nsca, f.get_data().get_ptr());
                  else
                     lhl1.set_ptr(nsca, f.get_data().get_ptr());
                }
            else if (mode == vapor) 
               { if (nincon == 0 && typ_ths == TH_space::PT)
                    lhv1pt.set_ptr(nsca, f.get_data().get_ptr()); 
                 else
                    lhg1.set_ptr(nsca, f.get_data().get_ptr()); 
               }
            else return 0;
            break;
         case NEPTUNE::d_rho_d_p_h:
            if (mode == liquid) lrl1.set_ptr(nsca, f.get_data().get_ptr());
            else if (mode == vapor)
               { if (nincon == 0) 
                    lrvpv.set_ptr(nsca, f.get_data().get_ptr());
                 else
                    lrg1.set_ptr(nsca, f.get_data().get_ptr());
               }
            else if (mode == unknown) lr1.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;
            break;
         case NEPTUNE::d_rho_d_h_p:
            if (mode == liquid) lrl2.set_ptr(nsca, f.get_data().get_ptr());
            else if (mode == vapor)
                  { if (nincon == 0) 
                       lrvhv.set_ptr(nsca, f.get_data().get_ptr());
                    else
                       lrg3.set_ptr(nsca, f.get_data().get_ptr());
                  }
            else if (mode == unknown) lr2.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;
            break;
         case NEPTUNE::d_rho_d_p_T:
            if      (mode == liquid) lrl1pt.set_ptr(nsca, f.get_data().get_ptr());
            else if (mode == vapor) lrv1pt.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;
            break;
         case NEPTUNE::d_rho_d_T_p:
            if      (mode == liquid) lrl2pt.set_ptr(nsca, f.get_data().get_ptr());
            else if (mode == vapor)  lrv3pt.set_ptr(nsca, f.get_data().get_ptr());
            break;
         case NEPTUNE::d_cp_d_p_h:
            if (mode == liquid) lcpl1.set_ptr(nsca, f.get_data().get_ptr());
            else if (mode == vapor) 
               { if (nincon == 0)
                     lcpvpv.set_ptr(nsca, f.get_data().get_ptr());
                 else
                     lcpg1.set_ptr(nsca, f.get_data().get_ptr());
               }
            else if (mode == unknown) lcp1.set_ptr(nsca, f.get_data().get_ptr());
            break;
         case NEPTUNE::d_cp_d_h_p:
            if (mode == liquid) lcpl2.set_ptr(nsca, f.get_data().get_ptr());
            else if (mode == vapor)
               { if (nincon == 0)
                    lcpvhv.set_ptr(nsca, f.get_data().get_ptr());
                 else
                    lcpg3.set_ptr(nsca, f.get_data().get_ptr());
               }
            else if (mode == unknown) lcp2.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;
            break;
         case NEPTUNE::d_beta_d_p_h:
            if (mode == liquid) lbetal1.set_ptr(nsca, f.get_data().get_ptr());
            else if (mode == unknown) lbetal1.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;
            break;
         case NEPTUNE::d_beta_d_h_p:
            if (mode == liquid) lbetal2.set_ptr(nsca, f.get_data().get_ptr());
            else if (mode == unknown) lbetal2.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;
            break;
         case NEPTUNE::d_lambda_d_p_h:
            if (mode == liquid) ltlal1.set_ptr(nsca, f.get_data().get_ptr());
            else if (mode == vapor) ltlag1.set_ptr(nsca, f.get_data().get_ptr());
            else if (mode == unknown) lla1.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;
            break;
         case NEPTUNE::d_lambda_d_h_p:
            if (mode == liquid) ltlal2.set_ptr(nsca, f.get_data().get_ptr());
            else if (mode == vapor) ltlag3.set_ptr(nsca, f.get_data().get_ptr());
            else if (mode == unknown) lla2.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;
            break;
         case NEPTUNE::d_lambda_d_T_p:
            if (mode == vapor) llagtg.set_ptr(nsca, f.get_data().get_ptr());
            else if (mode == unknown) llagtg.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;
            break;
         case NEPTUNE::d_mu_d_p_h:
            if (mode == liquid) ltmul1.set_ptr(nsca, f.get_data().get_ptr());
            else if (mode == vapor) ltmug1.set_ptr(nsca, f.get_data().get_ptr());
            else if (mode == unknown) lmu1.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;
            break;
         case NEPTUNE::d_mu_d_h_p:
            if (mode == liquid) ltmul2.set_ptr(nsca, f.get_data().get_ptr());
            else if (mode == vapor) ltmug3.set_ptr(nsca, f.get_data().get_ptr());
            else if (mode == unknown) lmu2.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;
            break;
         case NEPTUNE::d_mu_d_T_p:
            if (mode == vapor) lmugtg.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;
            break;
         case NEPTUNE::d_sigma_d_p_h:
            if (mode == vapor) lsi1.set_ptr(nsca, f.get_data().get_ptr());
            else if (mode == unknown) lsi1.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;
            break;
         case NEPTUNE::d_sigma_d_h_p:
            if (mode == vapor) lsi3.set_ptr(nsca, f.get_data().get_ptr());
            else if (mode == unknown) lsi3.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;
            break;
         case NEPTUNE::d_cp_d_p_T:
            if      (mode == liquid)  lcpl1pt.set_ptr(nsca, f.get_data().get_ptr());
            else if (mode == vapor)   lcpvpvpt.set_ptr(nsca, f.get_data().get_ptr());
            else if (mode == unknown) lcpl1pt.set_ptr(nsca,  f.get_data().get_ptr());
            else return 0;
            break;
         case NEPTUNE::d_cp_d_T_p:
            if      (mode == liquid)  lcpl2pt.set_ptr(nsca, f.get_data().get_ptr());
            else if (mode == vapor)   lcpgtgpt.set_ptr(nsca,  f.get_data().get_ptr());
            else if (mode == unknown) lcpgtg.set_ptr(nsca,  f.get_data().get_ptr());
            else return 0;
            break;
            
         // Saturation of Thermodynamic Properties
         case NEPTUNE::p_sat:
            lp.set_ptr(nsca, f.get_data().get_ptr());
            break;
         case NEPTUNE::T_sat:
            ltsp.set_ptr(nsca, f.get_data().get_ptr());
            break;
         case NEPTUNE::h_l_sat:
            lhlsp.set_ptr(nsca, f.get_data().get_ptr());
            break;
         case NEPTUNE::h_v_sat:
            lhvsp.set_ptr(nsca, f.get_data().get_ptr());
            break;
         case NEPTUNE::cp_l_sat:
            lcplsp.set_ptr(nsca, f.get_data().get_ptr());
            break;
         case NEPTUNE::cp_v_sat:
            lcpvsp.set_ptr(nsca, f.get_data().get_ptr());
            break;
         case NEPTUNE::rho_l_sat:
            lrlsp.set_ptr(nsca, f.get_data().get_ptr());
            break;
         case NEPTUNE::rho_v_sat:
            lrvsp.set_ptr(nsca, f.get_data().get_ptr());
            break;
         // First Derivatives
         case NEPTUNE::d_T_sat_d_p:
            ltsp1.set_ptr(nsca, f.get_data().get_ptr());
            break;
         case NEPTUNE::d_h_l_sat_d_p:
            lhlsp1.set_ptr(nsca, f.get_data().get_ptr());
            break;
         case NEPTUNE::d_h_v_sat_d_p:
            lhvsp1.set_ptr(nsca, f.get_data().get_ptr());
            break;
         case NEPTUNE::d_cp_l_sat_d_p:
            lclsp1.set_ptr(nsca, f.get_data().get_ptr());
            break;
         case NEPTUNE::d_cp_v_sat_d_p:
            lcvsp1.set_ptr(nsca, f.get_data().get_ptr());
            break;
         case NEPTUNE::d_rho_l_sat_d_p:
            lrlsp1.set_ptr(nsca, f.get_data().get_ptr());
            break;
         case NEPTUNE::d_rho_v_sat_d_p:
            lrvsp1.set_ptr(nsca, f.get_data().get_ptr());
            break;
         // Second Derivatives
         case NEPTUNE::d2_T_sat_d_p_d_p:
            l2tsp1.set_ptr(nsca, f.get_data().get_ptr());
            break;
            
         // Spinodale limites Thermodynamic Properties
         case NEPTUNE::p_lim:
            lp.set_ptr(nsca, f.get_data().get_ptr());
            break;
         case NEPTUNE::h_l_lim:
            hllim.set_ptr(nsca, f.get_data().get_ptr());
            break;
         case NEPTUNE::h_v_lim:
            hvlim.set_ptr(nsca, f.get_data().get_ptr());
            break;
            
         // Cathare Mixing Thermodynamic Properties
         case NEPTUNE::c_0:
            if (nincon > 0 && mode == vapor)
              lxvap.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;
            break;
         case NEPTUNE::c_1:
            if (nincon > 0 && mode == vapor) lx[0].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::c_2: 
            if (nincon > 0 && mode == vapor) lx[1].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::c_3:
            if (nincon > 0 && mode == vapor) lx[2].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::c_4:
            if (nincon > 0 && mode == vapor) lx[3].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::p_0:
            if (nincon > 0 && mode == vapor) lpv.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break ;
         case NEPTUNE::p_1:
            if (nincon > 0 && mode == vapor) lpx[0].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;
            break;
         case NEPTUNE::p_2:
            if (nincon > 0 && mode == vapor) lpx[1].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;
            break;
         case NEPTUNE::p_3:
            if (nincon > 0 && mode == vapor) lpx[2].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::p_4:
            if (nincon > 0 && mode == vapor) lpx[3].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;
            break;
         case NEPTUNE::h_0:
            if (nincon > 0 && mode == vapor) lhv.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;
            break;
         case NEPTUNE::h_1:
            if (nincon > 0 && mode == vapor) lhx[0].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::h_2:
            if (nincon > 0 && mode == vapor) lhx[1].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::h_3:
            if (nincon > 0 && mode == vapor) lhx[2].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::h_4:
            if (nincon > 0 && mode == vapor) lhx[3].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::rho_0:
            if (nincon > 0 && mode == vapor) lrv.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::mu_0:
            if (nincon > 0 && mode == vapor) ltmuv.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::lambda_0:
            if (nincon > 0 && mode == vapor) ltlav.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::cp_0:
            if (nincon > 0 && mode == vapor) lcpv.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::T_sat_0:
            if (nincon > 0 && mode == vapor) ltspv.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::h_l_sat_0:
            if (nincon > 0 && mode == vapor) lhlsv.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::h_v_sat_0:
            if (nincon > 0 && mode == vapor)  lhvsv.set_ptr(nsca, f.get_data().get_ptr()); 
            else return 0;     
            break;
         case NEPTUNE::cp_l_sat_0:
            if (nincon > 0 && mode == vapor)  lcplsv.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::cp_v_sat_0:
            if (nincon > 0 && mode == vapor)  lcpvsv.set_ptr(nsca, f.get_data().get_ptr()); 
            else return 0;     
            break;
         case NEPTUNE::rho_l_sat_0:
            if (nincon > 0 && mode == vapor)  lrlsv.set_ptr(nsca, f.get_data().get_ptr()); 
            else return 0;     
            break;
         case NEPTUNE::rho_v_sat_0:
            if (nincon > 0 && mode == vapor)  lrvsv.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::dncv:
            if (nincon > 0 && mode == vapor) ldncv.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::rnc:
            if (nincon > 0 && mode == vapor) lrnc.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::mnc:
            if (nincon > 0 && mode == vapor) lmnc.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::prgr:
            if (nincon > 0 && mode == vapor) lprgr.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::xnc:
            if (nincon > 0 && mode == vapor) lxnc.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         // First Derivatives
         case NEPTUNE::d_p_0_d_p_h:
            if (nincon > 0 && mode == vapor) lpv1.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_p_0_d_h_p:
            if (nincon > 0 && mode == vapor) lpv3.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_p_0_d_c_1_ph:
            if (nincon > 0 && mode == vapor) lpvx[0].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_p_0_d_c_2_ph:
            if (nincon > 0 && mode == vapor) lpvx[1].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_p_0_d_c_3_ph:
            if (nincon > 0 && mode == vapor) lpvx[2].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_p_0_d_c_4_ph:
            if (nincon > 0 && mode == vapor) lpvx[3].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_h_0_d_p_h:
            if (nincon > 0 && mode == vapor) lhv1.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_h_0_d_h_p:
            if (nincon > 0 && mode == vapor) lhv3.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_h_0_d_c_1_ph:
            if (nincon > 0 && mode == vapor) lhvx[0].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_h_0_d_c_2_ph:
            if (nincon > 0 && mode == vapor) lhvx[1].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_h_0_d_c_3_ph:
            if (nincon > 0 && mode == vapor) lhvx[2].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_h_0_d_c_4_ph:
            if (nincon > 0 && mode == vapor) lhvx[3].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_h_d_c_1_pT:
            if (nincon > 0 && mode == vapor) lhgx[0].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;
            break;
         case NEPTUNE::d_h_d_c_2_pT:
            if (nincon > 0 && mode == vapor) lhgx[1].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;
            break;
         case NEPTUNE::d_h_d_c_3_pT:
            if (nincon > 0 && mode == vapor) lhgx[2].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;
            break;
         case NEPTUNE::d_h_d_c_4_pT:
            if (nincon > 0 && mode == vapor) lhgx[3].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;
            break;
         case NEPTUNE::d_T_d_h_0_p:
            if (nincon > 0 && mode == vapor) ltghv.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_T_d_p_0_h:
            if (nincon > 0 && mode == vapor) ltgpv.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_T_d_c_1_ph:
            if (nincon > 0 && mode == vapor) ltgx[0].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_T_d_c_2_ph:
            if (nincon > 0 && mode == vapor) ltgx[1].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_T_d_c_3_ph:
            if (nincon > 0 && mode == vapor) ltgx[2].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_T_d_c_4_ph:
            if (nincon > 0 && mode == vapor) ltgx[3].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_cp_d_c_1_ph:
            if (nincon > 0 && mode == vapor) lcpgx[0].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_cp_d_c_2_ph:
            if (nincon > 0 && mode == vapor) lcpgx[1].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_cp_d_c_3_ph:
            if (nincon > 0 && mode == vapor) lcpgx[2].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_cp_d_c_4_ph:
            if (nincon > 0 && mode == vapor) lcpgx[3].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_cp_0_d_p_0_h:
            if (nincon > 0 && mode == vapor) lcpvpv.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_cp_0_d_h_0_p:
            if (nincon > 0 && mode == vapor) lcpvhv.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_rho_d_c_1_ph:
            if (nincon > 0 && mode == vapor) lrgx[0].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_rho_d_c_2_ph:
            if (nincon > 0 && mode == vapor) lrgx[1].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_rho_d_c_3_ph:
            if (nincon > 0 && mode == vapor) lrgx[2].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_rho_d_c_4_ph:
            if (nincon > 0 && mode == vapor) lrgx[3].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_mu_d_c_1_ph:
            if (nincon > 0 && mode == vapor) ltmugx[0].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_mu_d_c_2_ph:
            if (nincon > 0 && mode == vapor) ltmugx[1].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_mu_d_c_3_ph:
            if (nincon > 0 && mode == vapor) ltmugx[2].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_mu_d_c_4_ph:
            if (nincon > 0 && mode == vapor) ltmugx[3].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_sigma_d_c_1_ph:
            if (nincon > 0 && mode == vapor) lsix[0].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_sigma_d_c_2_ph:
            if (nincon > 0 && mode == vapor) lsix[1].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_sigma_d_c_3_ph:
            if (nincon > 0 && mode == vapor) lsix[2].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_sigma_d_c_4_ph:
            if (nincon > 0 && mode == vapor) lsix[3].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_lambda_d_c_1_ph:
            if (nincon > 0 && mode == vapor) ltlagx[0].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_lambda_d_c_2_ph:
            if (nincon > 0 && mode == vapor) ltlagx[1].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_lambda_d_c_3_ph:
            if (nincon > 0 && mode == vapor) ltlagx[2].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_lambda_d_c_4_ph:
            if (nincon > 0 && mode == vapor) ltlagx[3].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_sigma_d_p_0_h:
            if (nincon > 0 && mode == vapor) lsipv.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_T_sat_0_d_p_0_h:
            if (nincon > 0 && mode == vapor) ltspvv.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_h_l_sat_0_d_p_0_h:
            if (nincon > 0 && mode == vapor) lhlsvv.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_h_v_sat_0_d_p_0_h:
            if (nincon > 0 && mode == vapor) lhvsvv.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_cp_l_sat_0_d_p_0_h:
            if (nincon > 0 && mode == vapor)  lclsvv.set_ptr(nsca, f.get_data().get_ptr()); 
            else return 0;     
            break;
         case NEPTUNE::d_cp_v_sat_0_d_p_0_h:
            if (nincon > 0 && mode == vapor)  lcvsvv.set_ptr(nsca, f.get_data().get_ptr()); 
            else return 0;     
            break;
         case NEPTUNE::d_rho_l_sat_0_d_p_0_h:
            if (nincon > 0 && mode == vapor)  lrlsvv.set_ptr(nsca, f.get_data().get_ptr()); 
            else return 0;     
            break;
         case NEPTUNE::d_rho_v_sat_0_d_p_0_h:
            if (nincon > 0 && mode == vapor)  lrvsvv.set_ptr(nsca, f.get_data().get_ptr()); 
            else return 0;     
            break;
         case NEPTUNE::d_lambda_0_d_p_0_h:
            if (nincon > 0 && mode == vapor) llavpv.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_lambda_0_d_T_p:
            if (nincon > 0 && mode == vapor) llavtg.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_rho_0_d_p_h:
            if (nincon > 0 && mode == vapor) lrv1.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_rho_0_d_h_p:
            if (nincon > 0 && mode == vapor) lrv3.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_rho_0_d_p_0_h:
            if (nincon > 0 && mode == vapor) lrvpv.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_rho_0_d_h_0_p:
            if (nincon > 0 && mode == vapor) lrvhv.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_rho_0_d_c_1_ph:
            if (nincon > 0 && mode == vapor) lrvx[0].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_rho_0_d_c_2_ph:
            if (nincon > 0 && mode == vapor) lrvx[1].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_rho_0_d_c_3_ph:
            if (nincon > 0 && mode == vapor) lrvx[2].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_rho_0_d_c_4_ph:
            if (nincon > 0 && mode == vapor) lrvx[3].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_rnc_d_c_1_ph:
            if (nincon > 0 && mode == vapor) lrncx[0].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_rnc_d_c_2_ph:
            if (nincon > 0 && mode == vapor) lrncx[1].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_rnc_d_c_3_ph:
            if (nincon > 0 && mode == vapor) lrncx[2].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_rnc_d_c_4_ph:
            if (nincon > 0 && mode == vapor) lrncx[3].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_mnc_d_c_1_ph:
            if (nincon > 0 && mode == vapor) lmncx[0].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_mnc_d_c_2_ph:
            if (nincon > 0 && mode == vapor) lmncx[1].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_mnc_d_c_3_ph:
            if (nincon > 0 && mode == vapor) lmncx[2].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_mnc_d_c_4_ph:
            if (nincon > 0 && mode == vapor) lmncx[3].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_dncv_d_c_1_ph:
            if (nincon > 0 && mode == vapor) ldncvx[0].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_dncv_d_c_2_ph:
            if (nincon > 0 && mode == vapor) ldncvx[1].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_dncv_d_c_3_ph:
            if (nincon > 0 && mode == vapor) ldncvx[2].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_dncv_d_c_4_ph:
            if (nincon > 0 && mode == vapor) ldncvx[3].set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_dncv_d_p_h:
            if (nincon > 0 && mode == vapor) ldncv1.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_dncv_d_h_p:
            if (nincon > 0 && mode == vapor) ldncv3.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_mu_0_d_T_p:
            if (nincon > 0 && mode == vapor) lmuvtg.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         case NEPTUNE::d_mu_0_d_p_0_h:
            if (nincon > 0 && mode == vapor) lmuvpv.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
         // Second Derivatives
         case NEPTUNE::d2_T_sat_0_d_p_0_d_p_0:
            if (nincon > 0 && mode == vapor) l2tsdpvv.set_ptr(nsca, f.get_data().get_ptr());
            else return 0;     
            break;
            
         // Cathare2 IAPWS Thermodynamic Properties
         case NEPTUNE::epstl:
            if (mode == liquid)        lepstliq.set_ptr(nsca, f.get_data().get_ptr()) ;
            else if (mode == vapor)    lepstliq.set_ptr(nsca, f.get_data().get_ptr()) ;
            else if (mode == unknown)  lepstliq.set_ptr(nsca, f.get_data().get_ptr()) ;
            else return 0 ;
            break;
         case NEPTUNE::hlspsc:
            if (mode == liquid)        lhlspsc.set_ptr(nsca, f.get_data().get_ptr()) ;
            else if (mode == vapor)    lhlspsc.set_ptr(nsca, f.get_data().get_ptr()) ;
            else if (mode == unknown)  lhlspsc.set_ptr(nsca, f.get_data().get_ptr()) ;
            else return 0 ;
            break;
         case NEPTUNE::hlsvsc:
            if (mode == liquid)                       return 0 ;
            else if (nincon > 0 && mode == vapor)     lhlsvsc.set_ptr(nsca, f.get_data().get_ptr()) ;
            else if (nincon > 0 && mode == unknown)   lhlsvsc.set_ptr(nsca, f.get_data().get_ptr()) ;
            else return 0 ;
            break;
         case NEPTUNE::epstg:
            if (mode == liquid)        return 0 ;
            else if (mode == vapor)    lepstgas.set_ptr(nsca, f.get_data().get_ptr()) ;
            else if (mode == unknown)  lepstgas.set_ptr(nsca, f.get_data().get_ptr()) ;
            else return 0 ;
            break;
         case NEPTUNE::hvspsc:
            if (mode == liquid)        return 0 ;
            else if (mode == vapor)    lhvspsc.set_ptr(nsca, f.get_data().get_ptr()) ;
            else if (mode == unknown)  lhvspsc.set_ptr(nsca, f.get_data().get_ptr()) ;
            else return 0 ;
            break;
         case NEPTUNE::hvsvsc:
            if (mode == liquid)                      return 0 ;
            else if (nincon > 0 && mode == vapor)    lhvsvsc.set_ptr(nsca, f.get_data().get_ptr()) ;
            else if (nincon > 0 && mode == unknown)  lhvsvsc.set_ptr(nsca, f.get_data().get_ptr()) ;
            else return 0 ;
            break;
         // First Derivatives
         case NEPTUNE::d_epstl_dp_h:
            if (mode == liquid)        lepstliq1.set_ptr(nsca, f.get_data().get_ptr()) ;
            else if (mode == vapor)    lepstliq1.set_ptr(nsca, f.get_data().get_ptr()) ;
            else if (mode == unknown)  lepstliq1.set_ptr(nsca, f.get_data().get_ptr()) ;
            else return 0 ;
            break;
         case NEPTUNE::d_epstl_dh_p:
            if (mode == liquid)        lepstliq2.set_ptr(nsca, f.get_data().get_ptr()) ;
            else if (mode == vapor)    lepstliq2.set_ptr(nsca, f.get_data().get_ptr()) ;
            else if (mode == unknown)  lepstliq2.set_ptr(nsca, f.get_data().get_ptr()) ;
            else return 0 ;
            break;
         case NEPTUNE::d_hlspsc_dp_h:
            if (mode == liquid)        lhlspsc1.set_ptr(nsca, f.get_data().get_ptr()) ;
            else if (mode == vapor)    lhlspsc1.set_ptr(nsca, f.get_data().get_ptr()) ;
            else if (mode == unknown)  lhlspsc1.set_ptr(nsca, f.get_data().get_ptr()) ;
            else return 0 ;
            break;
         case NEPTUNE::d_hlspsc_dh_p:
            if (mode == liquid)        lhlspsc2.set_ptr(nsca, f.get_data().get_ptr()) ;
            else if (mode == vapor)    lhlspsc2.set_ptr(nsca, f.get_data().get_ptr()) ;
            else if (mode == unknown)  lhlspsc2.set_ptr(nsca, f.get_data().get_ptr()) ;
            else return 0 ;
            break;
         case NEPTUNE::d_hlsvsc_dp_h:
            if (mode == liquid)                      return 0 ;
            else if (nincon > 0 && mode == vapor)    lhlsvsc1.set_ptr(nsca, f.get_data().get_ptr()) ;
            else if (nincon > 0 && mode == unknown)  lhlsvsc1.set_ptr(nsca, f.get_data().get_ptr()) ;
            else return 0 ;
            break;
         case NEPTUNE::d_hlsvsc_dh_p0:
            if (mode == liquid)                      return 0 ;
            else if (nincon > 0 && mode == vapor)    lhlsvsc2.set_ptr(nsca, f.get_data().get_ptr()) ;
            else if (nincon > 0 && mode == unknown)  lhlsvsc2.set_ptr(nsca, f.get_data().get_ptr()) ;
            else return 0 ;
            break;
         case NEPTUNE::d_hlsvsc_dh_p:
            if (mode == liquid)                      return 0 ;
            else if (nincon > 0 && mode == vapor)    lhlsvsc3.set_ptr(nsca, f.get_data().get_ptr()) ;
            else if (nincon > 0 && mode == unknown)  lhlsvsc3.set_ptr(nsca, f.get_data().get_ptr()) ;
            else return 0 ;
            break;
         case NEPTUNE::d_hlsvsc_d_c_1_ph:
            if (mode == liquid)                      return 0 ;
            else if (nincon > 0 && mode == vapor)    lhlsvscx[0].set_ptr(nsca, f.get_data().get_ptr()) ;
            else if (nincon > 0 && mode == unknown)  lhlsvscx[0].set_ptr(nsca, f.get_data().get_ptr()) ;
            else return 0 ;
            break;
         case NEPTUNE::d_hlsvsc_d_c_2_ph:
            if (mode == liquid)                      return 0 ;
            else if (nincon > 0 && mode == vapor)    lhlsvscx[1].set_ptr(nsca, f.get_data().get_ptr()) ;
            else if (nincon > 0 && mode == unknown)  lhlsvscx[1].set_ptr(nsca, f.get_data().get_ptr()) ;
            else return 0 ;
            break;
         case NEPTUNE::d_hlsvsc_d_c_3_ph:
            if (mode == liquid)                      return 0 ;
            else if (nincon > 0 && mode == vapor)    lhlsvscx[2].set_ptr(nsca, f.get_data().get_ptr()) ;
            else if (nincon > 0 && mode == unknown)  lhlsvscx[2].set_ptr(nsca, f.get_data().get_ptr()) ;
            else return 0 ;
            break;
         case NEPTUNE::d_hlsvsc_d_c_4_ph:
            if (mode == liquid)                      return 0 ;
            else if (nincon > 0 && mode == vapor)    lhlsvscx[3].set_ptr(nsca, f.get_data().get_ptr()) ;
            else if (nincon > 0 && mode == unknown)  lhlsvscx[3].set_ptr(nsca, f.get_data().get_ptr()) ;
            else return 0 ;
            break;
         case NEPTUNE::d_epstg_dp_h:
            if (mode == liquid)        return 0 ;
            else if (mode == vapor)    lepstgas1.set_ptr(nsca, f.get_data().get_ptr()) ;
            else if (mode == unknown)  lepstgas1.set_ptr(nsca, f.get_data().get_ptr()) ;
            else return 0 ;
            break;
         case NEPTUNE::d_epstg_dh_p:
            if (mode == liquid)        return 0 ;
            else if (mode == vapor)    lepstgas3.set_ptr(nsca, f.get_data().get_ptr()) ;
            else if (mode == unknown)  lepstgas3.set_ptr(nsca, f.get_data().get_ptr()) ;
            else return 0 ;
            break;
         case NEPTUNE::d_epstg_d_c_1_ph:
            if (mode == liquid)        return 0 ;
            else if (nincon > 0 && mode == vapor )   lepstgasx[0].set_ptr(nsca, f.get_data().get_ptr()) ;
            else if (nincon > 0 && mode == unknown)  lepstgasx[0].set_ptr(nsca, f.get_data().get_ptr()) ;
            else return 0 ;
            break;
         case NEPTUNE::d_epstg_d_c_2_ph:
            if (mode == liquid)        return 0 ;
            else if (nincon > 0 && mode == vapor )   lepstgasx[1].set_ptr(nsca, f.get_data().get_ptr()) ;
            else if (nincon > 0 && mode == unknown)  lepstgasx[1].set_ptr(nsca, f.get_data().get_ptr()) ;
            else return 0 ;
            break;
         case NEPTUNE::d_epstg_d_c_3_ph:
            if (mode == liquid)        return 0 ;
            else if (nincon > 0 && mode == vapor )   lepstgasx[2].set_ptr(nsca, f.get_data().get_ptr()) ;
            else if (nincon > 0 && mode == unknown)  lepstgasx[2].set_ptr(nsca, f.get_data().get_ptr()) ;
            else return 0 ;
            break;
         case NEPTUNE::d_epstg_d_c_4_ph:
            if (mode == liquid)        return 0 ;
            else if (nincon > 0 && mode == vapor )   lepstgasx[3].set_ptr(nsca, f.get_data().get_ptr()) ;
            else if (nincon > 0 && mode == unknown)  lepstgasx[3].set_ptr(nsca, f.get_data().get_ptr()) ;
            else return 0 ;
            break;
         case NEPTUNE::d_hvspsc_dp_h:
            if (mode == liquid)                      return 0 ;
            else if (mode == vapor)                  lhvspsc1.set_ptr(nsca, f.get_data().get_ptr()) ;
            else if (mode == unknown)                lhvspsc1.set_ptr(nsca, f.get_data().get_ptr()) ;
            else return 0 ;
            break;
         case NEPTUNE::d_hvspsc_dh_p:
            if (mode == liquid)                      return 0 ;
            else if (mode == vapor)                  lhvspsc3.set_ptr(nsca, f.get_data().get_ptr()) ;
            else if (mode == unknown)                lhvspsc3.set_ptr(nsca, f.get_data().get_ptr()) ;
            else return 0 ;
            break;
         case NEPTUNE::d_hvspsc_d_c_1_ph:
            if (mode == liquid)                      return 0 ;
            else if (nincon > 0 && mode == vapor )   lhvspscx[0].set_ptr(nsca, f.get_data().get_ptr()) ;
            else if (nincon > 0 && mode == unknown)  lhvspscx[0].set_ptr(nsca, f.get_data().get_ptr()) ;
            else return 0 ;
            break;
         case NEPTUNE::d_hvspsc_d_c_2_ph:
            if (mode == liquid)                      return 0 ;
            else if (nincon > 0 && mode == vapor )   lhvspscx[1].set_ptr(nsca, f.get_data().get_ptr()) ;
            else if (nincon > 0 && mode == unknown)  lhvspscx[1].set_ptr(nsca, f.get_data().get_ptr()) ;
            else return 0 ;
            break;
         case NEPTUNE::d_hvspsc_d_c_3_ph:
            if (mode == liquid)                      return 0 ;
            else if (nincon > 0 && mode == vapor )   lhvspscx[2].set_ptr(nsca, f.get_data().get_ptr()) ;
            else if (nincon > 0 && mode == unknown)  lhvspscx[2].set_ptr(nsca, f.get_data().get_ptr()) ;
            else return 0 ;
            break;
         case NEPTUNE::d_hvspsc_d_c_4_ph:
            if (mode == liquid)                      return 0 ;
            else if (nincon > 0 && mode == vapor )   lhvspscx[3].set_ptr(nsca, f.get_data().get_ptr()) ;
            else if (nincon > 0 && mode == unknown)  lhvspscx[3].set_ptr(nsca, f.get_data().get_ptr()) ;
            else return 0 ;
            break;
         case NEPTUNE::d_hvsvsc_dp_h:
            if (mode == liquid)        return 0 ;
            else if (nincon > 0 && mode == vapor)    lhvsvsc1.set_ptr(nsca, f.get_data().get_ptr()) ;
            else if (nincon > 0 && mode == unknown)  lhvsvsc1.set_ptr(nsca, f.get_data().get_ptr()) ;
            else return 0 ;
            break;
         case NEPTUNE::d_hvsvsc_dh_p:
            if (mode == liquid)        return 0 ;
            else if (nincon > 0 && mode == vapor)    lhvsvsc3.set_ptr(nsca, f.get_data().get_ptr()) ;
            else if (nincon > 0 && mode == unknown)  lhvsvsc3.set_ptr(nsca, f.get_data().get_ptr()) ;
            else return 0 ;
            break;
         case NEPTUNE::d_hvsvsc_d_c_1_ph:
            if (mode == liquid)        return 0 ;
            else if (nincon > 0 && mode == vapor )   lhvsvscx[0].set_ptr(nsca, f.get_data().get_ptr()) ;
            else if (nincon > 0 && mode == unknown)  lhvsvscx[0].set_ptr(nsca, f.get_data().get_ptr()) ;
            else return 0 ;
            break;
         case NEPTUNE::d_hvsvsc_d_c_2_ph:
            if (mode == liquid)        return 0 ;
            else if (nincon > 0 && mode == vapor )   lhvsvscx[1].set_ptr(nsca, f.get_data().get_ptr()) ;
            else if (nincon > 0 && mode == unknown)  lhvsvscx[1].set_ptr(nsca, f.get_data().get_ptr()) ;
            else return 0 ;
            break;
         case NEPTUNE::d_hvsvsc_d_c_3_ph:
            if (mode == liquid)        return 0 ;
            else if (nincon > 0 && mode == vapor )   lhvsvscx[2].set_ptr(nsca, f.get_data().get_ptr()) ;
            else if (nincon > 0 && mode == unknown)  lhvsvscx[2].set_ptr(nsca, f.get_data().get_ptr()) ;
            else return 0 ;
            break;
         case NEPTUNE::d_hvsvsc_d_c_4_ph:
            if (mode == liquid)        return 0 ;
            else if (nincon > 0 && mode == vapor )   lhvsvscx[3].set_ptr(nsca, f.get_data().get_ptr()) ;
            else if (nincon > 0 && mode == unknown)  lhvsvscx[3].set_ptr(nsca, f.get_data().get_ptr()) ;
            else return 0 ;
            break;
         default: 
            return 0 ;
       }

    return 1 ;
  }

  EOS_Error CATHARE2::map_eos_fields(const EOS_Fields& f, vector<int>& existprop_fields, domain mode) 
  { int f_size = f.size() ;
    assert (f_size > 0);
    int can = 0;
    for (int i=0; i<f_size; i++) 
       { existprop_fields[i] = map_eos_field(f[i], mode) ;
         if (existprop_fields[i] != 0) can++ ;
       }
    if (can == f_size) return EOS_Error::good ;
    return EOS_Error::error ;
  }


  int CATHARE2::unmap_eos_field(const EOS_Field& f, domain mode)
  { assert(f.size() == nsca) ;
    ArrOfDouble ltmp ;

    switch(f.get_property_number()) 
       {
         case NEPTUNE::p :
            lp=ltmp ;
            break ;
         case NEPTUNE::h :
            if (mode == liquid) 
               { if (typ_ths == TH_space::PT)
                    lhlpt = ltmp ;
                 else
                    lhl = ltmp ;
               }
            else if (mode == vapor)
               {  if (nincon == 0 && typ_ths == TH_space::PT)
                     lhvpt = ltmp ;
                  else
                     { lhl = ltmp ;
                       lhg = ltmp ;
                     }
               }
            else if (mode == unknown) lh=ltmp;
            break;
         case NEPTUNE::T:
            if (mode == liquid) ltl=ltmp;
            else if (mode == vapor) ltg=ltmp;
            else if (mode == unknown) lt=ltmp;
            break;
         case NEPTUNE::rho:
            if (mode == liquid)
               { if (typ_ths == TH_space::PT)
                    lrlpt = ltmp ;
                 else
                    lrl = ltmp ;
               }
            else if (mode == vapor)
               { if (nincon == 0 && typ_ths == TH_space::PT)
                   lrvpt = ltmp ;
                 else
                   lrg = ltmp ;
               }
            else if (mode == unknown) lr=ltmp;
            break;
         case NEPTUNE::cp:
            if (mode == liquid)
               { if (typ_ths == TH_space::PT)
                    lcplpt = ltmp ;
                 else
                    lcpl = ltmp ;
               }
            else if (mode == vapor) 
               { if (nincon == 0 && typ_ths == TH_space::PT)
                    lcpvpt = ltmp ;
                 else
                    lcpg = ltmp ;
               }
            else if (mode == unknown) lcp=ltmp;
            break;
         case NEPTUNE::beta:
            if (mode == liquid) lbetal=ltmp;
            else if (mode == unknown) lbetal=ltmp;
            break;
         case NEPTUNE::lambda:
            if (mode == liquid) ltlal=ltmp;
            else if (mode == vapor) ltlag=ltmp;
            else if (mode == unknown) lla=ltmp;
            break;
         case NEPTUNE::mu:
            if (mode == liquid) ltmul=ltmp;
            else if (mode == vapor) ltmug=ltmp;
            else if (mode == unknown) lmu=ltmp;
            break;
         case NEPTUNE::sigma:
            if (mode == vapor) lsi=ltmp;
            else if (mode == unknown) lsi=ltmp;
            break;
         case NEPTUNE::gamma: lkiseng=ltmp; break ;
         case NEPTUNE::pr:
            if (mode == vapor) lprandg=ltmp;
            if (mode == unknown) lprandg=ltmp;
            break;
         case NEPTUNE::d_T_d_p_h:
            if (mode == liquid) ltl1=ltmp;
            else if (mode == vapor) ltg1=ltmp;
            else if (mode == unknown) lt1=ltmp;
            break;
         case NEPTUNE::d_T_d_h_p:
            if (mode == liquid) ltl2=ltmp;
            else if (mode == vapor) ltg3=ltmp;
            else if (mode == unknown) lt2=ltmp;
            break;
         case NEPTUNE::d_h_d_T_p:
            if (mode == liquid)
               { if (typ_ths == TH_space::PT)
                    lhl2pt = ltmp ;
                 else
                    lhl2 = ltmp ;
               }
            else if (mode == vapor) 
               { if (nincon == 0 && typ_ths == TH_space::PT)
                    lcpvpt = ltmp ;
                 else
                    lcpg = ltmp ;
               }
            break;
         case NEPTUNE::d_h_d_p_T:
            if (mode == liquid)
               { if (typ_ths == TH_space::PT)
                    lhl1pt = ltmp ;
                 else
                    lhl1 = ltmp ;
               }
            else if (mode == vapor) 
               { if (nincon == 0 && typ_ths == TH_space::PT)
                    lhv1pt = ltmp ;
                 else
                    lhg1 = ltmp ;
               }
            break;
         case NEPTUNE::d_rho_d_p_h:
            if (mode == liquid) lrl1=ltmp;
            else if (mode == vapor)
               { if (nincon == 0) 
                    lrvpv=ltmp;
                 else
                    lrg1=ltmp;
               }
            else if (mode == unknown) lr1=ltmp;
            break;
         case NEPTUNE::d_rho_d_h_p:
            if (mode == liquid) lrl2=ltmp;
            else if (mode == vapor)
               { if (nincon == 0) 
                    lrvhv=ltmp;
                 else
                    lrg3=ltmp;
               }
            else if (mode == unknown) lr2=ltmp;
            break;
         case NEPTUNE::d_rho_d_p_T:
            if      (mode == liquid) lrl1pt=ltmp;
            else if (mode == vapor)  lrv1pt=ltmp;
            break;
         case NEPTUNE::d_rho_d_T_p:
            if      (mode == liquid) lrl2pt=ltmp;
            else if (mode == vapor)  lrv3pt=ltmp;
            break;
         case NEPTUNE::d_cp_d_p_h:
            if (mode == liquid) lcpl1=ltmp;
            else if (mode == vapor)
               { if (nincon == 0) 
                    lcpvpv=ltmp;
                 else
                    lcpg1=ltmp;
               }
            else if (mode == unknown) lcp1=ltmp;
            break;
         case NEPTUNE::d_cp_d_h_p:
            if (mode == liquid) lcpl2=ltmp;
            else if (mode == vapor)
               { if (nincon == 0) 
                    lcpvhv=ltmp;
                 else
                    lcpg3=ltmp;
               }
            else if (mode == unknown) lcp2=ltmp;
            break;
         case NEPTUNE::d_beta_d_p_h:
            if (mode == liquid) lbetal1=ltmp;
            else if (mode == unknown) lbetal1=ltmp;
            break;
         case NEPTUNE::d_beta_d_h_p:
            if (mode == liquid) lbetal2=ltmp;
            else if (mode == unknown) lbetal2=ltmp;
            break;
         case NEPTUNE::d_lambda_d_p_h:
            if (mode == liquid) ltlal1=ltmp;
            else if (mode == vapor) ltlag1=ltmp;
            else if (mode == unknown) lla1=ltmp;
            break;
         case NEPTUNE::d_lambda_d_h_p:
            if (mode == liquid) ltlal2=ltmp;
            else if (mode == vapor) ltlag3=ltmp;
            else if (mode == unknown) lla2=ltmp;
            break;
         case NEPTUNE::d_lambda_d_T_p:
            if (mode == vapor) llagtg=ltmp;
            else if (mode == unknown) llagtg=ltmp;
            break;
         case NEPTUNE::d_mu_d_p_h:
            if (mode == liquid) ltmul1=ltmp;
            else if (mode == vapor) ltmug1=ltmp;
            else if (mode == unknown) lmu1=ltmp;
            break;
         case NEPTUNE::d_mu_d_h_p:
            if (mode == liquid) ltmul2=ltmp;
            else if (mode == vapor) ltmug3=ltmp;
            else if (mode == unknown) lmu2=ltmp;
            break;
         case NEPTUNE::d_mu_d_T_p:
            if (mode == vapor) lmugtg=ltmp;
            break;
         case NEPTUNE::d_sigma_d_p_h:
            if (mode == vapor) lsi1=ltmp;
            else if (mode == unknown) lsi1=ltmp;
            break;
         case NEPTUNE::d_sigma_d_h_p:
            if (mode == vapor) lsi3=ltmp;
            else if (mode == unknown) lsi3=ltmp;
            break;
         case NEPTUNE::d_cp_d_p_T:
            if      (mode == liquid)  lcpl1pt=ltmp;
            else if (mode == vapor)   lcpvpvpt=ltmp;
            else if (mode == unknown) lcpl1pt=ltmp;
            break;
         case NEPTUNE::d_cp_d_T_p:
            if      (mode == liquid)  lcpl2pt=ltmp;
            else if (mode == vapor)   lcpgtgpt=ltmp;
            else if (mode == unknown) lcpgtg=ltmp;
            break;
         case NEPTUNE::p_sat            : lp     = ltmp ; break ;
         case NEPTUNE::T_sat            : ltsp   = ltmp ; break ;
         case NEPTUNE::h_l_sat          : lhlsp  = ltmp ; break ;
         case NEPTUNE::h_v_sat          : lhvsp  = ltmp ; break ;
         case NEPTUNE::cp_l_sat         : lcplsp = ltmp ; break ;
         case NEPTUNE::cp_v_sat         : lcpvsp = ltmp ; break ;
         case NEPTUNE::rho_l_sat        : lrlsp  = ltmp ; break ;
         case NEPTUNE::rho_v_sat        : lrvsp  = ltmp ; break ;
         case NEPTUNE::d_T_sat_d_p      : ltsp1  = ltmp ; break ;
         case NEPTUNE::d_h_l_sat_d_p    : lhlsp1 = ltmp ; break ;
         case NEPTUNE::d_h_v_sat_d_p    : lhvsp1 = ltmp ; break ;
         case NEPTUNE::d_cp_l_sat_d_p   : lclsp1 = ltmp ; break ;
         case NEPTUNE::d_cp_v_sat_d_p   : lcvsp1 = ltmp ; break ;
         case NEPTUNE::d_rho_l_sat_d_p  : lrlsp1 = ltmp ; break ;
         case NEPTUNE::d_rho_v_sat_d_p  : lrvsp1 = ltmp ; break ;
         case NEPTUNE::d2_T_sat_d_p_d_p : l2tsp1 = ltmp ; break ;

         case NEPTUNE::p_lim   : lp    = ltmp ; break ;
         case NEPTUNE::h_l_lim : hllim = ltmp ; break ;
         case NEPTUNE::h_v_lim : hvlim = ltmp ; break ;

         case NEPTUNE::c_0:        if (nincon > 0 && mode == vapor)  lxvap  = ltmp; break;
         case NEPTUNE::c_1:        if (nincon > 0 && mode == vapor)  lx[0]  = ltmp; break;
         case NEPTUNE::c_2:        if (nincon > 0 && mode == vapor)  lx[1]  = ltmp; break;
         case NEPTUNE::c_3:        if (nincon > 0 && mode == vapor)  lx[2]  = ltmp; break;
         case NEPTUNE::c_4:        if (nincon > 0 && mode == vapor)  lx[3]  = ltmp; break;
         case NEPTUNE::p_0:        if (nincon > 0 && mode == vapor)  lpv    = ltmp; break;
         case NEPTUNE::p_1:        if (nincon > 0 && mode == vapor)  lpx[0] = ltmp; break;
         case NEPTUNE::p_2:        if (nincon > 0 && mode == vapor)  lpx[1] = ltmp; break;
         case NEPTUNE::p_3:        if (nincon > 0 && mode == vapor)  lpx[2] = ltmp; break;
         case NEPTUNE::p_4:        if (nincon > 0 && mode == vapor)  lpx[3] = ltmp; break;
         case NEPTUNE::h_0:        if (nincon > 0 && mode == vapor)  lhv    = ltmp; break;
         case NEPTUNE::h_1:        if (nincon > 0 && mode == vapor)  lhx[0] = ltmp; break;
         case NEPTUNE::h_2:        if (nincon > 0 && mode == vapor)  lhx[1] = ltmp; break;
         case NEPTUNE::h_3:        if (nincon > 0 && mode == vapor)  lhx[2] = ltmp; break;
         case NEPTUNE::h_4:        if (nincon > 0 && mode == vapor)  lhx[3] = ltmp; break;
         case NEPTUNE::cp_0:       if (nincon > 0 && mode == vapor)  lcpv   = ltmp; break;
         case NEPTUNE::rho_0:      if (nincon > 0 && mode == vapor)  lrv    = ltmp; break;
         case NEPTUNE::lambda_0:   if (nincon > 0 && mode == vapor)  ltlav  = ltmp; break;
         case NEPTUNE::mu_0:       if (nincon > 0 && mode == vapor)  ltmuv  = ltmp; break;
         case NEPTUNE::T_sat_0:    if (nincon > 0 && mode == vapor)  ltspv  = ltmp; break;
         case NEPTUNE::h_l_sat_0:  if (nincon > 0 && mode == vapor)  lhlsv  = ltmp; break;
         case NEPTUNE::h_v_sat_0:  if (nincon > 0 && mode == vapor)  lhvsv  = ltmp; break;
         case NEPTUNE::cp_l_sat_0: if (nincon > 0 && mode == vapor)  lcplsv = ltmp; break;
         case NEPTUNE::cp_v_sat_0: if (nincon > 0 && mode == vapor)  lcpvsv = ltmp; break;
         case NEPTUNE::rho_l_sat_0:if (nincon > 0 && mode == vapor)  lrlsv  = ltmp; break;
         case NEPTUNE::rho_v_sat_0:if (nincon > 0 && mode == vapor)  lrvsv  = ltmp; break;
         case NEPTUNE::dncv:       if (nincon > 0 && mode == vapor)  ldncv  = ltmp; break;
         case NEPTUNE::rnc:        if (nincon > 0 && mode == vapor)  lrnc   = ltmp; break;
         case NEPTUNE::mnc:        if (nincon > 0 && mode == vapor)  lmnc   = ltmp; break;
         case NEPTUNE::prgr:       if (nincon > 0 && mode == vapor)  lprgr  = ltmp; break;
         case NEPTUNE::xnc:        if (nincon > 0 && mode == vapor)  lxnc   = ltmp; break;
         // First Derivatives
         case NEPTUNE::d_p_0_d_p_h:           if (nincon > 0 && mode == vapor)  lpv1     = ltmp; break;
         case NEPTUNE::d_p_0_d_h_p:           if (nincon > 0 && mode == vapor)  lpv3     = ltmp; break;
         case NEPTUNE::d_p_0_d_c_1_ph:        if (nincon > 0 && mode == vapor)  lpvx[0]  = ltmp; break;
         case NEPTUNE::d_p_0_d_c_2_ph:        if (nincon > 0 && mode == vapor)  lpvx[1]  = ltmp; break;
         case NEPTUNE::d_p_0_d_c_3_ph:        if (nincon > 0 && mode == vapor)  lpvx[2]  = ltmp; break;
         case NEPTUNE::d_p_0_d_c_4_ph:        if (nincon > 0 && mode == vapor)  lpvx[3]  = ltmp; break;
         case NEPTUNE::d_h_0_d_p_h:           if (nincon > 0 && mode == vapor)  lhv1     = ltmp; break;
         case NEPTUNE::d_h_0_d_h_p:           if (nincon > 0 && mode == vapor)  lhv3     = ltmp; break;
         case NEPTUNE::d_h_0_d_c_1_ph:        if (nincon > 0 && mode == vapor)  lhvx[0]  = ltmp; break;
         case NEPTUNE::d_h_0_d_c_2_ph:        if (nincon > 0 && mode == vapor)  lhvx[1]  = ltmp; break;
         case NEPTUNE::d_h_0_d_c_3_ph:        if (nincon > 0 && mode == vapor)  lhvx[2]  = ltmp; break;
         case NEPTUNE::d_h_0_d_c_4_ph:        if (nincon > 0 && mode == vapor)  lhvx[3]  = ltmp; break;
         case NEPTUNE::d_h_d_c_1_pT:          if (nincon > 0 && mode == vapor)  lhgx[0]  = ltmp; break;
         case NEPTUNE::d_h_d_c_2_pT:          if (nincon > 0 && mode == vapor)  lhgx[1]  = ltmp; break;
         case NEPTUNE::d_h_d_c_3_pT:          if (nincon > 0 && mode == vapor)  lhgx[2]  = ltmp; break;
         case NEPTUNE::d_h_d_c_4_pT:          if (nincon > 0 && mode == vapor)  lhgx[3]  = ltmp; break;
         case NEPTUNE::d_T_d_h_0_p:           if (nincon > 0 && mode == vapor)  ltghv    = ltmp; break;
         case NEPTUNE::d_T_d_p_0_h:           if (nincon > 0 && mode == vapor)  ltgpv    = ltmp; break;
         case NEPTUNE::d_T_d_c_1_ph:          if (nincon > 0 && mode == vapor)  ltgx[0]  = ltmp; break;
         case NEPTUNE::d_T_d_c_2_ph:          if (nincon > 0 && mode == vapor)  ltgx[1]  = ltmp; break;
         case NEPTUNE::d_T_d_c_3_ph:          if (nincon > 0 && mode == vapor)  ltgx[2]  = ltmp; break;
         case NEPTUNE::d_T_d_c_4_ph:          if (nincon > 0 && mode == vapor)  ltgx[3]  = ltmp; break;
         case NEPTUNE::d_cp_d_c_1_ph:         if (nincon > 0 && mode == vapor)  lcpgx[0] = ltmp; break;
         case NEPTUNE::d_cp_d_c_2_ph:         if (nincon > 0 && mode == vapor)  lcpgx[1] = ltmp; break;
         case NEPTUNE::d_cp_d_c_3_ph:         if (nincon > 0 && mode == vapor)  lcpgx[2] = ltmp; break;
         case NEPTUNE::d_cp_d_c_4_ph:         if (nincon > 0 && mode == vapor)  lcpgx[3] = ltmp; break;
         case NEPTUNE::d_cp_0_d_p_0_h:        if (nincon > 0 && mode == vapor)  lcpvpv   = ltmp; break;
         case NEPTUNE::d_cp_0_d_h_0_p:        if (nincon > 0 && mode == vapor)  lcpvhv   = ltmp; break;
         case NEPTUNE::d_rho_d_c_1_ph:        if (nincon > 0 && mode == vapor)  lrgx[0]  = ltmp; break;
         case NEPTUNE::d_rho_d_c_2_ph:        if (nincon > 0 && mode == vapor)  lrgx[1]  = ltmp; break;
         case NEPTUNE::d_rho_d_c_3_ph:        if (nincon > 0 && mode == vapor)  lrgx[2]  = ltmp; break;
         case NEPTUNE::d_rho_d_c_4_ph:        if (nincon > 0 && mode == vapor)  lrgx[3]  = ltmp; break;
         case NEPTUNE::d_mu_d_c_1_ph:         if (nincon > 0 && mode == vapor)  ltmugx[0]= ltmp; break;
         case NEPTUNE::d_mu_d_c_2_ph:         if (nincon > 0 && mode == vapor)  ltmugx[1]= ltmp; break;
         case NEPTUNE::d_mu_d_c_3_ph:         if (nincon > 0 && mode == vapor)  ltmugx[2]= ltmp; break;
         case NEPTUNE::d_mu_d_c_4_ph:         if (nincon > 0 && mode == vapor)  ltmugx[3]= ltmp; break;
         case NEPTUNE::d_sigma_d_c_1_ph:      if (nincon > 0 && mode == vapor)  lsix[0]  = ltmp; break;
         case NEPTUNE::d_sigma_d_c_2_ph:      if (nincon > 0 && mode == vapor)  lsix[1]  = ltmp; break;
         case NEPTUNE::d_sigma_d_c_3_ph:      if (nincon > 0 && mode == vapor)  lsix[2]  = ltmp; break;
         case NEPTUNE::d_sigma_d_c_4_ph:      if (nincon > 0 && mode == vapor)  lsix[3]  = ltmp; break;
         case NEPTUNE::d_lambda_d_c_1_ph:     if (nincon > 0 && mode == vapor)  ltlagx[0]= ltmp; break;
         case NEPTUNE::d_lambda_d_c_2_ph:     if (nincon > 0 && mode == vapor)  ltlagx[1]= ltmp; break;
         case NEPTUNE::d_lambda_d_c_3_ph:     if (nincon > 0 && mode == vapor)  ltlagx[2]= ltmp; break;
         case NEPTUNE::d_lambda_d_c_4_ph:     if (nincon > 0 && mode == vapor)  ltlagx[3]= ltmp; break;
         case NEPTUNE::d_sigma_d_p_0_h:       if (nincon > 0 && mode == vapor)  lsipv    = ltmp; break;
         case NEPTUNE::d_T_sat_0_d_p_0_h:     if (nincon > 0 && mode == vapor)  ltspvv   = ltmp; break;
         case NEPTUNE::d_h_l_sat_0_d_p_0_h:   if (nincon > 0 && mode == vapor)  lhlsvv   = ltmp; break;
         case NEPTUNE::d_h_v_sat_0_d_p_0_h:   if (nincon > 0 && mode == vapor)  lhvsvv   = ltmp; break;
         case NEPTUNE::d_cp_l_sat_0_d_p_0_h:  if (nincon > 0 && mode == vapor)  lclsvv   = ltmp; break;
         case NEPTUNE::d_cp_v_sat_0_d_p_0_h:  if (nincon > 0 && mode == vapor)  lcvsvv   = ltmp; break;
         case NEPTUNE::d_rho_l_sat_0_d_p_0_h: if (nincon > 0 && mode == vapor)  lrlsvv   = ltmp; break;
         case NEPTUNE::d_rho_v_sat_0_d_p_0_h: if (nincon > 0 && mode == vapor)  lrvsvv   = ltmp; break;
         case NEPTUNE::d_lambda_0_d_p_0_h:    if (nincon > 0 && mode == vapor)  llavpv   = ltmp; break;
         case NEPTUNE::d_lambda_0_d_T_p:      if (nincon > 0 && mode == vapor)  llavtg   = ltmp; break;
         case NEPTUNE::d_rho_0_d_p_h:         if (nincon > 0 && mode == vapor)  lrv1     = ltmp; break;
         case NEPTUNE::d_rho_0_d_h_p:         if (nincon > 0 && mode == vapor)  lrv3     = ltmp; break;
         case NEPTUNE::d_rho_0_d_p_0_h:       if (nincon > 0 && mode == vapor)  lrvpv    = ltmp; break;
         case NEPTUNE::d_rho_0_d_h_0_p:       if (nincon > 0 && mode == vapor)  lrvhv    = ltmp; break;
         case NEPTUNE::d_rho_0_d_c_1_ph:      if (nincon > 0 && mode == vapor)  lrvx[0]  = ltmp; break;
         case NEPTUNE::d_rho_0_d_c_2_ph:      if (nincon > 0 && mode == vapor)  lrvx[1]  = ltmp; break;
         case NEPTUNE::d_rho_0_d_c_3_ph:      if (nincon > 0 && mode == vapor)  lrvx[2]  = ltmp; break;
         case NEPTUNE::d_rho_0_d_c_4_ph:      if (nincon > 0 && mode == vapor)  lrvx[3]  = ltmp; break;
         case NEPTUNE::d_rnc_d_c_1_ph:        if (nincon > 0 && mode == vapor)  lrncx[0] = ltmp; break;
         case NEPTUNE::d_rnc_d_c_2_ph:        if (nincon > 0 && mode == vapor)  lrncx[1] = ltmp; break;
         case NEPTUNE::d_rnc_d_c_3_ph:        if (nincon > 0 && mode == vapor)  lrncx[2] = ltmp; break;
         case NEPTUNE::d_rnc_d_c_4_ph:        if (nincon > 0 && mode == vapor)  lrncx[3] = ltmp; break;
         case NEPTUNE::d_mnc_d_c_1_ph:        if (nincon > 0 && mode == vapor)  lmncx[0] = ltmp; break;
         case NEPTUNE::d_mnc_d_c_2_ph:        if (nincon > 0 && mode == vapor)  lmncx[1] = ltmp; break;
         case NEPTUNE::d_mnc_d_c_3_ph:        if (nincon > 0 && mode == vapor)  lmncx[2] = ltmp; break;
         case NEPTUNE::d_mnc_d_c_4_ph:        if (nincon > 0 && mode == vapor)  lmncx[3] = ltmp; break;
         case NEPTUNE::d_dncv_d_c_1_ph:       if (nincon > 0 && mode == vapor)  ldncvx[0]= ltmp; break;
         case NEPTUNE::d_dncv_d_c_2_ph:       if (nincon > 0 && mode == vapor)  ldncvx[1]= ltmp; break;
         case NEPTUNE::d_dncv_d_c_3_ph:       if (nincon > 0 && mode == vapor)  ldncvx[2]= ltmp; break;
         case NEPTUNE::d_dncv_d_c_4_ph:       if (nincon > 0 && mode == vapor)  ldncvx[3]= ltmp; break;
         case NEPTUNE::d_dncv_d_p_h:          if (nincon > 0 && mode == vapor)  ldncv1   = ltmp; break;
         case NEPTUNE::d_dncv_d_h_p:          if (nincon > 0 && mode == vapor)  ldncv3   = ltmp; break;
         case NEPTUNE::d_mu_0_d_T_p:          if (nincon > 0 && mode == vapor)  lmuvtg   = ltmp; break;
         case NEPTUNE::d_mu_0_d_p_0_h:        if (nincon > 0 && mode == vapor)  lmuvpv   = ltmp; break;
         // Second Derivatives
         case NEPTUNE::d2_T_sat_0_d_p_0_d_p_0: if (nincon > 0 && mode == vapor) l2tsdpvv = ltmp; break;

         // Cathare2 IAPWS Thermodynamic Properties
         case NEPTUNE::epstl:              if (mode == liquid)  lepstliq     = ltmp;  break;
         case NEPTUNE::hlspsc:             if (mode == liquid)  lhlspsc      = ltmp;  break;
         case NEPTUNE::hlsvsc:             if (mode == liquid)  lhlsvsc      = ltmp;  break;
         case NEPTUNE::epstg:              if (mode == liquid)  lepstgas     = ltmp;  break;
         case NEPTUNE::hvspsc:             if (mode == liquid)  lhvspsc      = ltmp;  break;
         case NEPTUNE::hvsvsc:             if (mode == liquid)  lhvsvsc      = ltmp;  break;
         case NEPTUNE::d_epstl_dp_h:       if (mode == liquid)  lepstliq1    = ltmp;  break;
         case NEPTUNE::d_epstl_dh_p:       if (mode == liquid)  lepstliq2    = ltmp;  break;
         case NEPTUNE::d_hlspsc_dp_h:      if (mode == liquid)  lhlspsc1     = ltmp;  break;
         case NEPTUNE::d_hlspsc_dh_p:      if (mode == liquid)  lhlspsc2     = ltmp;  break;
         case NEPTUNE::d_hlsvsc_dp_h:      if (mode == liquid)  lhlsvsc1     = ltmp;  break;
         case NEPTUNE::d_hlsvsc_dh_p0:     if (mode == liquid)  lhlsvsc2     = ltmp;  break;
         case NEPTUNE::d_hlsvsc_dh_p:      if (mode == liquid)  lhlsvsc3     = ltmp;  break;
         case NEPTUNE::d_hlsvsc_d_c_1_ph:  if (mode == liquid)  lhlsvscx[0]  = ltmp;  break;
         case NEPTUNE::d_hlsvsc_d_c_2_ph:  if (mode == liquid)  lhlsvscx[1]  = ltmp;  break;
         case NEPTUNE::d_hlsvsc_d_c_3_ph:  if (mode == liquid)  lhlsvscx[2]  = ltmp;  break;
         case NEPTUNE::d_hlsvsc_d_c_4_ph:  if (mode == liquid)  lhlsvscx[3]  = ltmp;  break;
         case NEPTUNE::d_epstg_dp_h:       if (mode == vapor)   lepstgas1    = ltmp;  break;
         case NEPTUNE::d_epstg_dh_p:       if (mode == vapor)   lepstgas3    = ltmp;  break;
         case NEPTUNE::d_epstg_d_c_1_ph:   if (mode == vapor)   lepstgasx[0] = ltmp;  break;
         case NEPTUNE::d_epstg_d_c_2_ph:   if (mode == vapor)   lepstgasx[1] = ltmp;  break;
         case NEPTUNE::d_epstg_d_c_3_ph:   if (mode == vapor)   lepstgasx[2] = ltmp;  break;
         case NEPTUNE::d_epstg_d_c_4_ph:   if (mode == vapor)   lepstgasx[3] = ltmp;  break;
         case NEPTUNE::d_hvspsc_dp_h:      if (mode == vapor)   lhvspsc1     = ltmp;  break;
         case NEPTUNE::d_hvspsc_dh_p:      if (mode == vapor)   lhvspsc3     = ltmp;  break;
         case NEPTUNE::d_hvspsc_d_c_1_ph:  if (mode == vapor)   lhvspscx[0]  = ltmp;  break;
         case NEPTUNE::d_hvspsc_d_c_2_ph:  if (mode == vapor)   lhvspscx[1]  = ltmp;  break;
         case NEPTUNE::d_hvspsc_d_c_3_ph:  if (mode == vapor)   lhvspscx[2]  = ltmp;  break;
         case NEPTUNE::d_hvspsc_d_c_4_ph:  if (mode == vapor)   lhvspscx[3]  = ltmp;  break;
         case NEPTUNE::d_hvsvsc_dp_h:      if (mode == vapor)   lhvsvsc1     = ltmp;  break;
         case NEPTUNE::d_hvsvsc_dh_p:      if (mode == vapor)   lhvsvsc3     = ltmp;  break;
         case NEPTUNE::d_hvsvsc_d_c_1_ph:  if (mode == vapor)   lhvsvscx[0]  = ltmp;  break;
         case NEPTUNE::d_hvsvsc_d_c_2_ph:  if (mode == vapor)   lhvsvscx[1]  = ltmp;  break;
         case NEPTUNE::d_hvsvsc_d_c_3_ph:  if (mode == vapor)   lhvsvscx[2]  = ltmp;  break;
         case NEPTUNE::d_hvsvsc_d_c_4_ph:  if (mode == vapor)   lhvsvscx[3]  = ltmp;  break;
         default: 
            return 0 ;
       }
    
    return 1 ;
  }


  int CATHARE2::unmap_eos_fields(const EOS_Fields& f, domain mode) 
  { assert (f.size() > 0) ;
    int nbunmap = 0 ;
    for (int i=0; i<f.size(); i++)
       { nbunmap += unmap_eos_field(f[i], mode) ;
       }
    return nbunmap ;
  }


  void CATHARE2::rezise_eos_fields(int sz, domain mode) 
  { if (ltsp.size() != sz) ltsp.resize(sz);
    if (ltsp1.size() != sz) ltsp1.resize(sz);
    if (l2tsp1.size() != sz) l2tsp1.resize(sz);
    if (lhlsp.size() != sz) lhlsp.resize(sz);
    if (lhlsp1.size() != sz) lhlsp1.resize(sz);
    if (lhvsp.size() != sz) lhvsp.resize(sz);
    if (lhvsp1.size() != sz) lhvsp1.resize(sz);
    if (lcplsp.size() != sz) lcplsp.resize(sz);
    if (lclsp1.size() != sz) lclsp1.resize(sz);
    if (lcpvsp.size() != sz) lcpvsp.resize(sz);
    if (lcvsp1.size() != sz) lcvsp1.resize(sz);
    if (lrlsp.size() != sz) lrlsp.resize(sz);
    if (lrlsp1.size() != sz) lrlsp1.resize(sz);
    if (lrvsp.size() != sz) lrvsp.resize(sz);
    if (lrvsp1.size() != sz) lrvsp1.resize(sz);
    if (lp.size() != sz) lp.resize(sz);
    if (lhf.size() != sz) lhf.resize(sz);
    if (lhf1.size() != sz) lhf1.resize(sz);
    if (ld2tl.size() != sz) ld2tl.resize(sz);
    if (ld3tl.size() != sz) ld3tl.resize(sz);
    if (ld2tg.size() != sz) ld2tg.resize(sz);
    if (ld3tg.size() != sz) ld3tg.resize(sz);
    if (ltl21.size() != sz) ltl21.resize(sz);
    if (ltg31.size() != sz) ltl21.resize(sz);
    if (lptemp.size() != sz) lptemp.resize(sz);
    if (lro7.size() != sz) lro7.resize(sz);
    if (lro73.size() != sz) lro73.resize(sz);
    if (lro72.size() != sz) lro72.resize(sz);
    if (lro71.size() != sz) lro71.resize(sz);
    if (lro721.size() != sz) lro721.resize(sz);
    if (lro731.size() != sz) lro731.resize(sz);
    if (ltp10.size() != sz) ltp10.resize(sz);
    if (ltp101.size() != sz) ltp101.resize(sz);
    if (ltp102.size() != sz) ltp102.resize(sz);
    if (ltp1021.size() != sz) ltp1021.resize(sz);
    if (ltp1031.size() != sz) ltp1031.resize(sz);
    if (ltgh2wrk.size() != sz) ltgh2wrk.resize(sz);
    if (ldencvx.size() != sz) ldencvx.resize(sz);
    if (lepstliq.size() != sz) lepstliq.resize(sz);
    if (lepstliq1.size() != sz) lepstliq1.resize(sz);
    if (lepstliq2.size() != sz) lepstliq2.resize(sz);
    if (lepstgas.size() != sz) lepstgas.resize(sz);
    if (lepstgas1.size() != sz) lepstgas1.resize(sz);
    if (lepstgas3.size() != sz) lepstgas3.resize(sz);
    if (lhlspsc.size() != sz) lhlspsc.resize(sz);
    if (lhlspscv.size() != sz) lhlspscv.resize(sz);
    if (lhlspsc1.size() != sz) lhlspsc1.resize(sz);
    if (lhlspsc2.size() != sz) lhlspsc2.resize(sz);
    if (lhlspsc3.size() != sz) lhlspsc3.resize(sz);
    if (lhvspsc.size() != sz) lhvspsc.resize(sz);
    if (lhvspscpv.size() != sz) lhvspscpv.resize(sz);
    if (lhvspsctg.size() != sz) lhvspsctg.resize(sz);
    if (lhvspsc1.size() != sz) lhvspsc1.resize(sz);
    if (lhvspsc2.size() != sz) lhvspsc2.resize(sz);
    if (lhvspsc3.size() != sz) lhvspsc3.resize(sz);
    if (lhlsvscv.size() != sz) lhlsvscv.resize(sz);
    if (lhvsvscpv.size() != sz) lhvsvscpv.resize(sz);
    if (lhvsvsctg.size() != sz) lhvsvsctg.resize(sz);
    if (hllim.size() != sz) hllim.resize(sz);
    if (hvlim.size() != sz) hvlim.resize(sz);
    if (lbetal.size() != sz) lbetal.resize(sz);
    if (lbetal1.size() != sz) lbetal1.resize(sz);
    if (lbetal2.size() != sz) lbetal2.resize(sz);
    if (ltl.size() != sz) ltl.resize(sz);      // epstl l/v
    if (ltl1.size() != sz) ltl1.resize(sz);    // epstl l/v
    if (ltl2.size() != sz) ltl2.resize(sz);    // epstl l/v
    if (lrl.size() != sz) lrl.resize(sz);      // epstl l/v
    if (lrl1.size() != sz) lrl1.resize(sz);    // epstl l/v
    if (lrl2.size() != sz) lrl2.resize(sz);    // epstl l/v
    if (lcpl.size() != sz) lcpl.resize(sz);    // epstl l/v
    if (lcpl1.size() != sz) lcpl1.resize(sz);  // epstl l/v
    if (lcpl2.size() != sz) lcpl2.resize(sz);  // epstl l/v
    
    if (mode == unknown) 
       { if (lt.size() != sz) lt.resize(sz);
         if (lh.size() != sz) lh.resize(sz);
         if (lr.size() != sz) lr.resize(sz);
         if (lcp.size() != sz) lcp.resize(sz);
         if (lmu.size() != sz) lmu.resize(sz);
         if (lla.size() != sz) lla.resize(sz);
         if (lt1.size() != sz) lt1.resize(sz);
         if (lh1.size() != sz) lh1.resize(sz);
         if (lr1.size() != sz) lr1.resize(sz);
         if (lcp1.size() != sz) lcp1.resize(sz);
         if (lmu1.size() != sz) lmu1.resize(sz);
         if (lla1.size() != sz) lla1.resize(sz);
         if (lt2.size() != sz) lt2.resize(sz);
         if (lh2.size() != sz) lh2.resize(sz);
         if (lr2.size() != sz) lr2.resize(sz);
         if (lcp2.size() != sz) lcp2.resize(sz);
         if (lmu2.size() != sz) lmu2.resize(sz);
         if (lla2.size() != sz) lla2.resize(sz);
       }

    if (mode == saturated)  
       { // Le pointeur l2tsdpvv est utilise pour d2_T_sat_d_p_d_p que l'on ait
         // ou non des incondensables (notamment pour H20, pour Na, pas ce pb)
         //En attente de correction dans cathare : on devrait avoir l2tsp1 dans CATHARe2_Water.cxx
         l2tsdpvv.set_ptr(sz, l2tsp1.get_ptr());
       }

    if ( (mode == liquid) || (mode == unknown) ) 
       { // Le pointeur l2tsdpvv est utilise pour d2_T_sat_d_p_d_p que l'on ait
         // ou non des incondensables (notamment pour H20, pour Na, pas ce pb)
         //En attente de correction dans cathare : on devrait avoir l2tsp1 dans CATHARe2_Water.cxx
         l2tsdpvv.set_ptr(sz, l2tsp1.get_ptr());
         if (lhl.size() != sz) lhl.resize(sz);
         if (lhl1.size() != sz) lhl1.resize(sz);
         if (lhl2.size() != sz) lhl2.resize(sz);
         if (ltl.size() != sz) ltl.resize(sz);
         if (ltl1.size() != sz) ltl1.resize(sz);
         if (ltl2.size() != sz) ltl2.resize(sz);
         if (lrl.size() != sz) lrl.resize(sz);
         if (lrl1.size() != sz) lrl1.resize(sz);
         if (lrl2.size() != sz) lrl2.resize(sz);
         if (lrl1pt.size() != sz) lrl1pt.resize(sz);
         if (lrl2pt.size() != sz) lrl2pt.resize(sz);
         if (lcpl.size() != sz) lcpl.resize(sz);
         if (lcpl1.size() != sz) lcpl1.resize(sz);
         if (lcpl2.size() != sz) lcpl2.resize(sz);
         if (lcpl1pt.size() != sz) lcpl1pt.resize(sz);
         if (lcpl2pt.size() != sz) lcpl2pt.resize(sz);
         if (ltlal.size() != sz) ltlal.resize(sz);
         if (ltlal1.size() != sz) ltlal1.resize(sz);
         if (ltlal2.size() != sz) ltlal2.resize(sz);
         if (ltmul.size() != sz) ltmul.resize(sz);
         if (ltmul1.size() != sz) ltmul1.resize(sz);
         if (ltmul2.size() != sz) ltmul2.resize(sz);
         if (lhlpt.size() != sz) lhlpt.resize(sz);    // M.F.
         if (lhl1pt.size() != sz) lhl1pt.resize(sz);  // M.F.
         if (lhl2pt.size() != sz) lhl2pt.resize(sz);  // M.F.
         if (lcplpt.size() != sz) lcplpt.resize(sz);  // M.F.
         if (lrlpt.size() != sz) lrlpt.resize(sz);    // M.F.
       }

    if ( (mode == vapor) || (mode == unknown) ) 
       { // Le pointeur l2tsdpvv est utilise pour calc2_h_pT_mixing que l'on ait
         // ou non des incondensables (notamment pour H20, pour Na, pas ce pb)
         //En attente de correction dans cathare : on devrait avoir l2tsp1 dans CATHARe2_Water.cxx
      l2tsdpvv.set_ptr(sz, l2tsp1.get_ptr());

      if (lhg.size() != sz) lhg.resize(sz);
      if (lprgr.size() != sz) lprgr.resize(sz);
      if (ltg.size() != sz) ltg.resize(sz);
      if (ltg1.size() != sz) ltg1.resize(sz);
      if (ltg3.size() != sz) ltg3.resize(sz);
      if (ltg31.size() != sz) ltg31.resize(sz);
      if (ltgk.size() != sz) ltgk.resize(sz);
      if (ltgini.size() != sz) ltgini.resize(sz);
      if (lhvmhs.size() != sz) lhvmhs.resize(sz);
      if (lcpg.size() != sz) lcpg.resize(sz);
      if (lcpg1.size() != sz) lcpg1.resize(sz);
      if (lcpg3.size() != sz) lcpg3.resize(sz);
      if (lrg.size() != sz) lrg.resize(sz);
      if (lrg1.size() != sz) lrg1.resize(sz);
      if (lhg1.size() != sz) lhg1.resize(sz);
      if (lrg3.size() != sz) lrg3.resize(sz);
      if (lrvpv.size() != sz)  lrvpv.resize(sz);        // M.F.
      if (lrvhv.size() != sz)  lrvhv.resize(sz);        // M.F.
      if (lcpvpv.size() != sz) lcpvpv.resize(sz);       // M.F.
      if (lcpvhv.size() != sz) lcpvhv.resize(sz);       // M.F.
      if (lrv1pt.size() != sz) lrv1pt.resize(sz);       // M.F.
      if (lrv3pt.size() != sz) lrv3pt.resize(sz);       // M.F.
      if (lcpvpvpt.size() != sz)  lcpvpvpt.resize(sz);  // M.F.
      if (lcpgtgpt.size() != sz)  lcpgtgpt.resize(sz);  // M.F.
      if (lhvpt.size() != sz)  lhvpt.resize(sz);        // M.F.
      if (lrvpt.size() != sz)  lrvpt.resize(sz);        // M.F.
      if (lcpvpt.size() != sz) lcpvpt.resize(sz);       // M.F.
      if (lhv1pt.size() != sz) lhv1pt.resize(sz);       // M.F.
      if (ltlag.size() != sz) ltlag.resize(sz);
      if (ltlag1.size() != sz) ltlag1.resize(sz);
      if (ltlag3.size() != sz) ltlag3.resize(sz);
      if (llagpv.size() != sz) llagpv.resize(sz);
      if (llagtg.size() != sz) llagtg.resize(sz);
      if (ltmug.size() != sz) ltmug.resize(sz);
      if (ltmug1.size() != sz) ltmug1.resize(sz);
      if (ltmug3.size() != sz) ltmug3.resize(sz);
      if (lmugpv.size() != sz) lmugpv.resize(sz);
      if (lmugtg.size() != sz) lmugtg.resize(sz);
      if (lsi.size() != sz) lsi.resize(sz);
      if (lsi1.size() != sz) lsi1.resize(sz);
      if (lsi3.size() != sz) lsi3.resize(sz);
      if (ldncv.size() != sz) ldncv.resize(sz);
      if (ldncv1.size() != sz) ldncv1.resize(sz);
      if (ldncv3.size() != sz) ldncv3.resize(sz);
      if (lcpgtg.size() != sz) lcpgtg.resize(sz);
      if (ltlaga.size() != sz) ltlaga.resize(sz);
      if (llagapv.size() != sz) llagapv.resize(sz);
      if (llagatg.size() != sz) llagatg.resize(sz);
      if (lustlagb.size() != sz) lustlagb.resize(sz);
      if (luslagbpv.size() != sz) luslagbpv.resize(sz);
      if (luslagbtg.size() != sz) luslagbtg.resize(sz);
      if (lcoefqv.size() != sz) lcoefqv.resize(sz);
      if (lkiseng.size() != sz) lkiseng.resize(sz);
      if (lprandg.size() != sz) lprandg.resize(sz);
      if (lhv1.size() != sz) lhv1.resize(sz);
      if (lhv3.size() != sz) lhv3.resize(sz);
      if (lxvap.size() != sz) lxvap.resize(sz);
      if (lrnc.size() != sz) lrnc.resize(sz);
      if (lmnc.size() != sz) lmnc.resize(sz);
      if (lxnc.size() != sz) lxnc.resize(sz);
      if (nincon == 0) 
         { ltspv.set_ptr(sz, ltsp.get_ptr());
           ltspvv.set_ptr(sz, ltsp1.get_ptr());
           lhlsv.set_ptr(sz, lhlsp.get_ptr());
           lhlsvv.set_ptr(sz, lhlsp1.get_ptr());
           lcplsv.set_ptr(sz, lcplsp.get_ptr());
           lclsvv.set_ptr(sz, lclsp1.get_ptr());
           lrlsv.set_ptr(sz, lrlsp.get_ptr());
           lrlsvv.set_ptr(sz, lrlsp1.get_ptr());
           lhlsvsc.set_ptr(sz, lhlspsc.get_ptr());
           lhlsvsc1.set_ptr(sz, lhlspsc1.get_ptr());
           lhlsvsc2.set_ptr(sz, lhlspsc2.get_ptr());
           lhlsvsc3.set_ptr(sz, lhlspsc3.get_ptr());
           lhvsv.set_ptr(sz, lhvsp.get_ptr());
           lhvsvv.set_ptr(sz, lhvsp1.get_ptr());
           lcpvsv.set_ptr(sz, lcpvsp.get_ptr());
           lcvsvv.set_ptr(sz, lcvsp1.get_ptr());
           lrvsv.set_ptr(sz, lrvsp.get_ptr());
           lrvsvv.set_ptr(sz, lrvsp1.get_ptr());
           lhvsvsc.set_ptr(sz, lhvspsc.get_ptr());
           lhvsvsc1.set_ptr(sz, lhvspsc1.get_ptr());
           lhvsvsc2.set_ptr(sz, lhvspsc2.get_ptr());
           if (lhvsvsc3.size() != sz) lhvsvsc3.resize(sz);
           lsipv.set_ptr(sz, lsi1.get_ptr());
           lpv.set_ptr(sz, lp.get_ptr());
           lhv.set_ptr(sz, lhg.get_ptr());
           lrv.set_ptr(sz, lrg.get_ptr());
           lrv1.set_ptr(sz, lrg1.get_ptr());
           lrv3.set_ptr(sz, lrg3.get_ptr());
           ltgpv.set_ptr(sz, ltg1.get_ptr());
           ltghv.set_ptr(sz, ltg3.get_ptr());
           // lrvpv.set_ptr(sz, lrg1.get_ptr());
           // lrvhv.set_ptr(sz, lrg3.get_ptr());
           lcpv.set_ptr(sz, lcpg.get_ptr());
           // lcpvpv.set_ptr(sz, lcpg1.get_ptr());
           // lcpvhv.set_ptr(sz, lcpg3.get_ptr());
           lhfv.set_ptr(sz, lhf.get_ptr());
           lhfvv.set_ptr(sz, lhf1.get_ptr());
           lhvpv.set_ptr(sz, lhg1.get_ptr());
           ltlav.set_ptr(sz, ltlag.get_ptr());
           ltmuv.set_ptr(sz, ltmug.get_ptr());
           llavpv.set_ptr(sz, llagpv.get_ptr());
           llavtg.set_ptr(sz, llagtg.get_ptr());
           lmuvpv.set_ptr(sz, lmugpv.get_ptr());
           lmuvtg.set_ptr(sz, lmugtg.get_ptr());
         }
      else
         { if (ltspv.size() != sz) ltspv.resize(sz);
           if (ltspvv.size() != sz) ltspvv.resize(sz);
           if (l2tsdpvv.size() != sz) l2tsdpvv.resize(sz);
           if (lhlsv.size() != sz) lhlsv.resize(sz);
           if (lhlsvv.size() != sz) lhlsvv.resize(sz);
           if (lhvsv.size() != sz) lhvsv.resize(sz);
           if (lhvsvv.size() != sz) lhvsvv.resize(sz);
           if (lcplsv.size() != sz) lcplsv.resize(sz);
           if (lclsvv.size() != sz) lclsvv.resize(sz);
           if (lcpvsv.size() != sz) lcpvsv.resize(sz);
           if (lcvsvv.size() != sz) lcvsvv.resize(sz);
           if (lrlsv.size() != sz) lrlsv.resize(sz);
           if (lrlsvv.size() != sz) lrlsvv.resize(sz);
           if (lrvsv.size() != sz) lrvsv.resize(sz);
           if (lrvsvv.size() != sz) lrvsvv.resize(sz);
           if (lpv.size() != sz) lpv.resize(sz);
           if (lhv.size() != sz) lhv.resize(sz);
           if (ltgpv.size() != sz) ltgpv.resize(sz);
           if (ltghv.size() != sz) ltghv.resize(sz);
           if (lcpv.size() != sz) lcpv.resize(sz);
           // if (lcpvpv.size() != sz) lcpvpv.resize(sz);
           // if (lcpvhv.size() != sz) lcpvhv.resize(sz);
           if (lrv.size() != sz) lrv.resize(sz);
           // if (lrvpv.size() != sz) lrvpv.resize(sz);
           // if (lrvhv.size() != sz) lrvhv.resize(sz);
           if (ltlav.size() != sz) ltlav.resize(sz);
           if (llavpv.size() != sz) llavpv.resize(sz);
           if (llavtg.size() != sz) llavtg.resize(sz);
           if (ltmuv.size() != sz) ltmuv.resize(sz);
           if (lmuvpv.size() != sz) lmuvpv.resize(sz);
           if (lmuvtg.size() != sz) lmuvtg.resize(sz);
           if (lsipv.size() != sz) lsipv.resize(sz);
           if (lhfv.size() != sz) lhfv.resize(sz);
           if (lhfvv.size() != sz) lhfvv.resize(sz);
           if (lhlsvsc.size() != sz) lhlsvsc.resize(sz);
           if (lhlsvsc1.size() != sz) lhlsvsc1.resize(sz);
           if (lhlsvsc2.size() != sz) lhlsvsc2.resize(sz);
           if (lhlsvsc3.size() != sz) lhlsvsc3.resize(sz);
           if (lhvsvsc.size() != sz) lhvsvsc.resize(sz);
           if (lhvsvsc1.size() != sz) lhvsvsc1.resize(sz);
           if (lhvsvsc2.size() != sz) lhvsvsc2.resize(sz);
           if (lhvsvsc3.size() != sz) lhvsvsc3.resize(sz);
           if (lxrsu.size() != sz) lxrsu.resize(sz);
           if (lxcpsu.size() != sz) lxcpsu.resize(sz);
           if (lxcpsutg.size() != sz) lxcpsutg.resize(sz);
           if (lhi7su.size() != sz) lhi7su.resize(sz);
           if (lvalp.size() != sz) lvalp.resize(sz);
           if (lpv1.size() != sz) lpv1.resize(sz);
           if (lpv3.size() != sz) lpv3.resize(sz);
           if (lrv1.size() != sz) lrv1.resize(sz);
           if (lrv3.size() != sz) lrv3.resize(sz);
           if (lhv1.size() != sz) lhv1.resize(sz);
           if (lhv3.size() != sz) lhv3.resize(sz);
           if (lfdpv.size() != sz) lfdpv.resize(sz);
           if (lfdhv.size() != sz) lfdhv.resize(sz);
           if (lgdpv.size() != sz) lgdpv.resize(sz);
           if (lgdhv.size() != sz) lgdhv.resize(sz);
           if (lphivmusu.size() != sz) lphivmusu.resize(sz);
           if (lphivmusupv.size() != sz) lphivmusupv.resize(sz);
           if (lphivmusutg.size() != sz) lphivmusutg.resize(sz);
           for (int i=0; i<nincon; i++) 
              { if (lx[i].size() != sz) lx[i].resize(sz);
                if (lpx[i].size() != sz) lpx[i].resize(sz);
                if (lpvx[i].size() != sz) lpvx[i].resize(sz);
                if (lhvx[i].size() != sz) lhvx[i].resize(sz);
                if (lhgx[i].size() != sz) lhgx[i].resize(sz);
                if (ltgx[i].size() != sz) ltgx[i].resize(sz);
                if (lrvx[i].size() != sz) lrvx[i].resize(sz);
                if (lrgx[i].size() != sz) lrgx[i].resize(sz);
                if (lhx[i].size() != sz) lhx[i].resize(sz);
                if (lrncx[i].size() != sz) lrncx[i].resize(sz);
                if (lmncx[i].size() != sz) lmncx[i].resize(sz);
                if (lprxcp[i].size() != sz) lprxcp[i].resize(sz);
                if (lprxcptg[i].size() != sz) lprxcptg[i].resize(sz);
                if (ltlax[i].size() != sz) ltlax[i].resize(sz);
                if (llaxtg[i].size() != sz) llaxtg[i].resize(sz);
                if (ltmux[i].size() != sz) ltmux[i].resize(sz);
                if (lmuxtg[i].size() != sz) lmuxtg[i].resize(sz);
                if (lphixmusu[i].size() != sz) lphixmusu[i].resize(sz);
                if (lphixmusupv[i].size() != sz) lphixmusupv[i].resize(sz);
                if (lphixmusutg[i].size() != sz) lphixmusutg[i].resize(sz);
                if (ldncvj[i].size() != sz) ldncvj[i].resize(sz);
                if (lcoefqvx[i].size() != sz) lcoefqvx[i].resize(sz);
                if (lphivmusux[i].size() != sz) lphivmusux[i].resize(sz);
                for (int j=0; j<nincon; j++)
                  if (lphixmusux[i][j].size() != sz) lphixmusux[i][j].resize(sz);
                if (lcpgx[i].size() != sz) lcpgx[i].resize(sz);
                if (ltlagx[i].size() != sz) ltlagx[i].resize(sz);
                if (ltmugx[i].size() != sz) ltmugx[i].resize(sz);
                if (lsix[i].size() != sz) lsix[i].resize(sz);
                if (ldncvx[i].size() != sz) ldncvx[i].resize(sz);
                if (lhlspscx[i].size() != sz) lhlspscx[i].resize(sz);
                if (lhlsvscx[i].size() != sz) lhlsvscx[i].resize(sz);
                if (lhvspscx[i].size() != sz) lhvspscx[i].resize(sz);
                if (lhvsvscx[i].size() != sz) lhvsvscx[i].resize(sz);
                if (lepstgasx[i].size() != sz) lepstgasx[i].resize(sz);
              }
         }
       }
  }

  void CATHARE2::preconvert_eos_fields() 
  { if (lfluid != 100003) 
       { if (ltl.size() == nsca) ltl -= tabsk;
         if (ltg.size() == nsca) ltg -= tabsk;
         if (ltsp.size() == nsca) ltsp -= tabsk;
       }
  }

  void CATHARE2::postconvert_eos_fields() 
  { if (lfluid != 100003) 
       { if (ltl.size()  == nsca) ltl  += tabsk ;
         if (ltg.size()  == nsca) ltg  += tabsk ;
         if (ltsp.size() == nsca) ltsp += tabsk ;
       }
  }

  EOS_Error CATHARE2::calc2_p(const EOS_Field &p, EOS_Fields &out, EOS_Error_Field &ferr)
  { typ_ths = TH_space::Psat ;
    ferr = EOS_Internal_Error::OK ;
    nsca = p.size() ;
    assert (nsca > 0) ;
    ArrOfInt err_array(nsca);
    EOS_Error_Field err_tmp(err_array);
    err_tmp = EOS_Internal_Error::OK;
    if (map_eos_field(p, saturated) == 0)  return EOS_Error::error ;
    int size_out = out.size() ;
    vector<int> existprop_fields(size_out) ;

    // M.F.
    ArrOfDouble ar0(nsca, 0e0) ;
    for (int i=0; i<size_out; i++)
       out[i].set_data() = ar0 ;

    // \todo : all unknown properties should be computed by EOS instead of return EOS_Error::error
    if (map_eos_fields(out, existprop_fields, saturated) == EOS_Error::error)
       { ferr = EOS_Internal_Error::NOT_IMPLEMENTED ;
         return EOS_Error::error ;
       }
    preconvert_eos_fields() ;
    rezise_eos_fields(nsca, saturated) ;

    int ill, ivstat, ivalue = 0 ;
    fhpfld(ill, ivstat = 0, ivalue, saturated);
    if (ivstat != 0) 
       { ferr.set(ill-1, convert_eos_error(ivstat)) ;
         for (; ill<nsca; ill++)
            err_tmp.set(ill, convert_eos_error(::CATHARE2::canceled)) ;
         ferr.set_worst_error(err_tmp) ;
         unmap_eos_fields(out, saturated);
         unmap_eos_field(p, saturated);
         return ferr.find_worst_error().generic_error() ;
       }
    calc2_lim() ;
    ferr.set_worst_error(err_tmp) ;
    postconvert_eos_fields() ;
    // M.F. for (int i=0; i<size_out; i++)
    // M.F.   if ( existprop_fields[i] == 0 ) 
    // M.F.      { cerr << "Cannot compute field: " << out[i] << endl ;
    // M.F.        err_tmp = EOS_Internal_Error::NOT_IMPLEMENTED ;
    // M.F.       //fluid->EOS_Fluid::compute(p, out[i], err_tmp);
    // M.F.        ferr.set_worst_error(err_tmp) ;
    // M.F.      }
    unmap_eos_fields(out, saturated) ;
    unmap_eos_field(p, saturated) ;
    return ferr.find_worst_error().generic_error() ;
  }

  EOS_Error CATHARE2::calc2_t(const EOS_Field &t, EOS_Fields &out, EOS_Error_Field &ferr)
  { typ_ths = TH_space::Tsat ;
    ferr = EOS_Internal_Error::OK ;
    nsca = t.size() ;
    assert (nsca > 0) ;
    ArrOfInt err_array(nsca) ;
    EOS_Error_Field err_tmp(err_array) ;
    err_tmp = EOS_Internal_Error::OK ;
    if (map_eos_field(t, saturated) == 0)  return EOS_Error::error ;
    int size_out = out.size() ;
    vector<int> existprop_fields(size_out) ;

    // M.F.
    ArrOfDouble ar0(nsca, 0e0) ;
    for (int i=0; i<size_out; i++)
       out[i].set_data() = ar0 ;

    // \todo : all unknown properties should be computed by EOS instead of return EOS_Error::error
    if (map_eos_fields(out, existprop_fields, saturated) == EOS_Error::error) 
       { ferr = EOS_Internal_Error::NOT_IMPLEMENTED ;
         return EOS_Error::error ;
       }
    preconvert_eos_fields() ;

    rezise_eos_fields(nsca, saturated) ;

    fpsattfld() ;

    int ill, ivstat, ivalue = 0 ;

    fhpfld(ill, ivstat = 0, ivalue, saturated) ;
    if (ivstat != 0) 
       { ferr.set(ill-1, convert_eos_error(ivstat)) ;
         for (; ill<nsca; ill++)
         err_tmp.set(ill, convert_eos_error(::CATHARE2::canceled)) ;
         ferr.set_worst_error(err_tmp) ;
         unmap_eos_fields(out, saturated);
         unmap_eos_field(t, saturated);
         return ferr.find_worst_error().generic_error() ;
    }
    calc2_lim() ;
    ferr.set_worst_error(err_tmp) ;
    postconvert_eos_fields() ;

    // M.F. for (int i=0; i<size_out; i++)
    // M.F.   if ( existprop_fields[i] == 0 ) 
    // M.F.      { cerr << "Cannot compute field: " << out[i] << endl;
    // M.F.        err_tmp = EOS_Internal_Error::NOT_IMPLEMENTED;
    // M.F.        //fluid->EOS_Fluid::compute(p, out[i], err_tmp);
    // M.F.        ferr.set_worst_error(err_tmp);
    // M.F.      }
    unmap_eos_fields(out, saturated) ;
    unmap_eos_field(t, saturated) ;
    return ferr.find_worst_error().generic_error() ;
  }

  EOS_Error CATHARE2::calc2_ph(const EOS_Field &p, const EOS_Field &h, EOS_Fields &out, EOS_Error_Field &ferr)
  { typ_ths = TH_space::Ph ;
    ferr = EOS_Internal_Error::OK ;
    nsca = p.size() ;
    assert (nsca > 0) ;
    if (map_eos_field(p, phase) == 0)  return EOS_Error::error ;
    if (map_eos_field(h, phase) == 0)  return EOS_Error::error ;
    int size_out = out.size() ;
    vector<int> existprop_fields(size_out) ;

    // M.F.
    ArrOfDouble ar0(nsca, 0e0) ;
    for (int i=0; i<size_out; i++)
       out[i].set_data() = ar0 ;

    // \todo : all unknown properties should be computed by EOS instead of return EOS_Error::error
    if (map_eos_fields(out, existprop_fields, phase) == EOS_Error::error) 
       { ferr = EOS_Internal_Error::NOT_IMPLEMENTED ;
         return EOS_Error::error ;
       }
    preconvert_eos_fields() ;
    rezise_eos_fields(nsca, phase) ;

    if (phase == unknown) lhg = lhl = lh ;

    ArrOfInt err_array(nsca);
    EOS_Error_Field err_tmp(err_array);
    int ill, ivstat, ivalue = 0 ;
    err_tmp = EOS_Internal_Error::OK ;
    fhpfld(ill, ivstat = 0, ivalue, phase) ;
    if (ivstat != 0) 
       { err_tmp.set(ill-1, convert_eos_error(ivstat)) ;
         for (; ill<nsca; ill++)
            err_tmp.set(ill, convert_eos_error(::CATHARE2::canceled)) ;
         ferr.set_worst_error(err_tmp) ;
         unmap_eos_fields(out, phase);
         unmap_eos_field(p, phase);
         unmap_eos_field(h, phase);
         return ferr.find_worst_error().generic_error() ;
       }
    // Calculation [dh/dP]T    ---> lhg1
    //             [dh/dcx]P,T ---> lhgx
    if (phase == vapor) 
       { for (int i=0 ; i<nsca ; i++)
         { lhg1[i] = -ltg1[i]/ltg3[i] ;
           for (int j=0 ; j<nincon ; j++)
              lhgx[j][i] = -ltgx[j][i]/ltg3[i] ;
         }
    }
    // M.F.
    if (nincon == 0)
       { if ((phase == liquid) || (phase == unknown)) ftliqfld() ;
         if ((phase == vapor)  || (phase == unknown)) ftvapfld() ;
       }
    // M.F.
    ferr.set_worst_error(err_tmp) ;
    err_tmp = EOS_Internal_Error::OK ;
    ftrafld(ill, ivstat = 0, phase) ;
    if (ivstat != 0) 
       { err_tmp.set(ill-1, convert_eos_error(ivstat));
         for (; ill<nsca; ill++)
            err_tmp.set(ill, convert_eos_error(::CATHARE2::canceled));
         ferr.set_worst_error(err_tmp) ;
         unmap_eos_fields(out, phase);
         unmap_eos_field(p, phase);
         unmap_eos_field(h, phase);
         return ferr.find_worst_error().generic_error() ;
       }
    ferr.set_worst_error(err_tmp) ;
    calc2_lim() ;
    ferr.set_worst_error(err_tmp) ;
    postconvert_eos_fields() ;

    err_tmp = EOS_Internal_Error::OK ;
    if (phase == unknown) 
       { for (ill=0; ill<nsca; ill++) 
            { if (lh[ill] <= hllim[ill]) 
                 { lt.set_value_at(ill, ltl[ill]);
                   lh.set_value_at(ill, lhl[ill]);
                   lcp.set_value_at(ill, lcpl[ill]);
                   lr.set_value_at(ill, lrl[ill]);
                   lmu.set_value_at(ill, ltmul[ill]);
                   lla.set_value_at(ill, ltlal[ill]);
                   lt1.set_value_at(ill, ltl1[ill]);
                   lh1.set_value_at(ill, lhl1[ill]);
                   lcp1.set_value_at(ill, lcpl1[ill]);
                   lr1.set_value_at(ill, lrl1[ill]);
                   lmu1.set_value_at(ill, ltmul1[ill]);
                   lla1.set_value_at(ill, ltlal1[ill]);
                   lt2.set_value_at(ill, ltl2[ill]);
                   lh2.set_value_at(ill, lhl2[ill]);
                   lcp2.set_value_at(ill, lcpl2[ill]);
                   lr2.set_value_at(ill, lrl2[ill]);
                   lmu2.set_value_at(ill, ltmul2[ill]);
                   lla2.set_value_at(ill, ltlal2[ill]);
                 } 
              else if (lh[ill] >= hvlim[ill]) 
                 { lt.set_value_at(ill, ltg[ill]);
                   lh.set_value_at(ill, lhg[ill]);
                   lcp.set_value_at(ill, lcpg[ill]);
                   lr.set_value_at(ill, lrg[ill]);
                   lmu.set_value_at(ill, ltmug[ill]);
                   lla.set_value_at(ill, ltlag[ill]);
                   lt1.set_value_at(ill, ltg1[ill]);
                   lh1.set_value_at(ill, lhv1[ill]);
                   lcp1.set_value_at(ill, lcpg1[ill]);
                   lr1.set_value_at(ill, lrg1[ill]);
                   lmu1.set_value_at(ill, ltmug1[ill]);
                   lla1.set_value_at(ill, ltlag1[ill]);
                   lt2.set_value_at(ill, ltg3[ill]);
                   lh2.set_value_at(ill, lhv3[ill]);
                   lcp2.set_value_at(ill, lcpg3[ill]);
                   lr2.set_value_at(ill, lrg3[ill]);
                   lmu2.set_value_at(ill, ltmug3[ill]);
                   lla2.set_value_at(ill, ltlag3[ill]);
                 } 
              else 
                 err_tmp.set(ill, convert_eos_error(::CATHARE2::unstable));
            }
         ferr.set_worst_error(err_tmp) ;
       }

    // M.F. for (int i=0; i<size_out; i++)
    // M.F.   if ( existprop_fields[i] == 0 ) 
    // M.F.      { cerr << "Cannot compute field: " << out[i] << endl;
    // M.F.        err_tmp = EOS_Internal_Error::NOT_IMPLEMENTED;
    // M.F.        //fluid->EOS_Fluid::compute(p, out[i], err_tmp);
    // M.F.        ferr.set_worst_error(err_tmp);
    // M.F.      }

    unmap_eos_fields(out, phase) ;
    unmap_eos_field(p, phase)    ;
    unmap_eos_field(h, phase)    ;
//    if  (Temp == 0) {
//        preconvert_eos_fields() ;
//    }
    return ferr.find_worst_error().generic_error() ;
  }

  EOS_Error CATHARE2::calc2_pt(const EOS_Field &p, const EOS_Field &t, EOS_Fields &out, EOS_Error_Field &ferr)
  { typ_ths = TH_space::PT ;
    ferr = EOS_Internal_Error::OK ;
    nsca = p.size() ;
    assert (nsca > 0) ; 
    if (map_eos_field(p, phase) == 0)    return EOS_Error::error ;
    ArrOfDouble xtin = t.get_data() ;                              // Replace
    EOS_Field tin("tin",t.get_property_name().aschar(),t.get_property(),xtin) ;     // if (map_eos_field(t, phase))  return EOS_Error::error ;
    if (map_eos_field(tin, phase) == 0)  return EOS_Error::error ; //
    int size_out = out.size() ;
    vector<int> existprop_fields(size_out) ;

    // M.F.
    ArrOfDouble ar0(nsca, 0e0) ;
    for (int i=0; i<size_out; i++)
       out[i].set_data() = ar0 ;

    // \todo : all unknown properties should be computed by EOS instead of return EOS_Error::error
    if (map_eos_fields(out, existprop_fields, phase) == EOS_Error::error)
       { ferr = EOS_Internal_Error::NOT_IMPLEMENTED ;
         return EOS_Error::error ;
       }
    preconvert_eos_fields() ;
    rezise_eos_fields(nsca, phase) ;
    ftsatpfld() ;
    if (phase == unknown) ltg = ltl = lt ;
    if ((phase == liquid) || (phase == unknown)) 
       { ftliqfld() ;
         lhl.set_ptr(nsca, lhlpt.get_ptr()) ;
       }
    if ((phase == vapor)  || (phase == unknown)) 
       { ftvapfld() ;
         lhg.set_ptr(nsca, lhvpt.get_ptr()) ;
         lhl.set_ptr(nsca, lhvpt.get_ptr()) ; // for IAPWS fhliqiapws
       }
    // temporaire debut
    postconvert_eos_fields()       ;
    unmap_eos_field(tin, phase)    ;
    rezise_eos_fields(nsca, phase) ;
    preconvert_eos_fields()        ;
    // temporaire fin   
    ArrOfInt err_array(nsca) ;
    EOS_Error_Field err_tmp(err_array) ;
    int ill, ivstat, ivalue = 0 ;
    err_tmp = EOS_Internal_Error::OK ;
    fhpfld(ill, ivstat = 0, ivalue, phase) ;
    if (ivstat != 0) 
       { err_tmp.set(ill-1, convert_eos_error(ivstat)) ;
         for (; ill<nsca; ill++)
            err_tmp.set(ill, convert_eos_error(::CATHARE2::canceled)) ;
         ferr.set_worst_error(err_tmp) ;
         unmap_eos_fields(out, phase);
         unmap_eos_field(p, phase);
         unmap_eos_field(t, phase);
         return ferr.find_worst_error().generic_error() ;
       }
//     Calcul de d_h_d_p_T = -d_T_d_p_h / d_T_d_h_p
//     Calcul de d_h_d_c_(i)_p = - d_T_d_c_(i)_ph / d_T_d_h_p
    if (phase == vapor) 
       { for (int i=0 ; i<nsca ; i++)
            { lhg1[i] = -ltg1[i]/ltg3[i] ;
              //aforce c2
              //inutile ici car pour l'instant dans le cas mixing car on se replace dans le plan ph par calc2_h_pT_mixing
//          for (int j=0 ; j<nincon ; j++)
//              lhgx[j][i]=-ltgx[j][i]/ltg3[i];
            }
       }
    ferr.set_worst_error(err_tmp) ;
    err_tmp = EOS_Internal_Error::OK ;
    ftrafld(ill, ivstat = 0, phase) ;
    if (ivstat != 0) 
       { err_tmp.set(ill-1, convert_eos_error(ivstat)) ;
         for (; ill<nsca; ill++)
            err_tmp.set(ill, convert_eos_error(::CATHARE2::canceled)) ;
         ferr.set_worst_error(err_tmp) ;
         unmap_eos_fields(out, phase);
         unmap_eos_field(p, phase);
         unmap_eos_field(t, phase);
         return ferr.find_worst_error().generic_error() ;
       }
    ferr.set_worst_error(err_tmp) ;
    calc2_lim() ;
    ferr.set_worst_error(err_tmp) ;
    postconvert_eos_fields() ;

    err_tmp = EOS_Internal_Error::OK;
    if (phase == unknown) 
       { for (ill=0; ill<nsca; ill++) 
            {
              if (lt[ill] <= ltsp[ill]) 
                 { lt.set_value_at(ill, ltl[ill]);
                   lh.set_value_at(ill, lhl[ill]);
                   lcp.set_value_at(ill, lcpl[ill]);
                   lr.set_value_at(ill, lrl[ill]);
                   lmu.set_value_at(ill, ltmul[ill]);
                   lla.set_value_at(ill, ltlal[ill]);
                   lt1.set_value_at(ill, ltl1[ill]);
                   lh1.set_value_at(ill, lhl1[ill]);
                   lcp1.set_value_at(ill, lcpl1[ill]);
                   lr1.set_value_at(ill, lrl1[ill]);
                   lmu1.set_value_at(ill, ltmul1[ill]);
                   lla1.set_value_at(ill, ltlal1[ill]);
                   lt2.set_value_at(ill, ltl2[ill]);
                   lh2.set_value_at(ill, lhl2[ill]);
                   lcp2.set_value_at(ill, lcpl2[ill]);
                   lr2.set_value_at(ill, lrl2[ill]);
                   lmu2.set_value_at(ill, ltmul2[ill]);
                   lla2.set_value_at(ill, ltlal2[ill]);
                 } 
             else 
                { lt.set_value_at(ill, ltg[ill]);
                  lh.set_value_at(ill, lhg[ill]);
                  lcp.set_value_at(ill, lcpg[ill]);
                  lr.set_value_at(ill, lrg[ill]);
                  lmu.set_value_at(ill, ltmug[ill]);
                  lla.set_value_at(ill, ltlag[ill]);
                  lt1.set_value_at(ill, ltg1[ill]);
                  lh1.set_value_at(ill, lhv1[ill]);
                  lcp1.set_value_at(ill, lcpg1[ill]);
                  lr1.set_value_at(ill, lrg1[ill]);
                  lmu1.set_value_at(ill, ltmug1[ill]);
                  lla1.set_value_at(ill, ltlag1[ill]);
                  lt2.set_value_at(ill, ltg3[ill]);
                  lh2.set_value_at(ill, lhv3[ill]);
                  lcp2.set_value_at(ill, lcpg3[ill]);
                  lr2.set_value_at(ill, lrg3[ill]);
                  lmu2.set_value_at(ill, ltmug3[ill]);
                  lla2.set_value_at(ill, ltlag3[ill]);
                 } 
              ferr.set_worst_error(err_tmp) ;
            }
       }

    // M.F. for (int i=0; i<size_out; i++)
    // M.F.   if (existprop_fields[i] == 0) 
    // M.F.      { cerr << "Cannot compute field: " << out[i] << endl;
    // M.F.        err_tmp = EOS_Internal_Error::NOT_IMPLEMENTED ;
    // M.F.        //fluid->EOS_Fluid::compute(p, out[i], err_tmp);
    // M.F.        ferr.set_worst_error(err_tmp);
    // M.F.      }
    unmap_eos_fields(out, phase) ;
    unmap_eos_field(p, phase)    ;
    unmap_eos_field(t, phase)    ;
    return ferr.find_worst_error().generic_error() ;
  }

  EOS_Internal_Error CATHARE2::calc2_h_pT_mixing(double p, double T, double &h)
  {
    // set_mixing_properties has already been done for (r,cp0,cp1,... and c_i)
    nsca = 1 ;
    EOS_Field fh("h","h",NEPTUNE::h,1,&h) ;
    if (map_eos_field(fh, phase) == 0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    rezise_eos_fields(nsca, phase) ;
    lp[0] = p ;
    preconvert_eos_fields() ;
    if (licargas != 1) 
       { lhi7su[0]   = 0.e0 ;
         lxcpsu[0]   = 0.e0 ;
         lxcpsutg[0] = 0.e0 ;
         ltgini[0]   = T-tabsk ;
         for (int j=1; j<=nincon; j++) 
            { F77NAME(c2_fhxcp)(lfluid, nsca, ltgini[0], lprxcp0[j-1],
                                lprxcp1[j-1], lprxcp2[j-1], lprxcp3[j-1],
                                lprxcp4[j-1], lprxcp5[j-1], lprxcp6[j-1],
                                lprxcp[j-1][0], lprxcptg[j-1][0]) ;
              F77NAME(c2_fhxsum)(lfluid, nsca, 1, lx[j-1][0], 
                                 lprxr[j-1], lprxcp[j-1][0], lprxcptg[j-1][0], lprxm[j-1],
                                 lxnc[0], lmnc[0], lxrsu[0], lxcpsu[0], lxcpsutg[0],
                                 lhi7su[0], lvalp[0]) ;
            }
       }
    // compute hvsat(pv) and tsat(pv) with pv=p*xvap
    lp[0] = p*lxvap[0] ;
    ftsatpfld() ;
    // initialization of hg 
    // HG=(HVSV(1)+(TG0-TSPV(1))*CPCONST(IFLUID))*XVAP+HI7SUM+XCPSUM*TG0
    double Tcelcius = T-tabsk ;
    lhg[0] = (lhvsp[0]+(Tcelcius-ltsp[0])*cpcst)*lxvap[0] + lhi7su[0] + lxcpsu[0]*Tcelcius ;
    // computation of hg
    lp[0] = p ;
    int k ;
    int nb_iter_max = 50 ;
    double epsn = 1.e-2 ;
    double func = 2*epsn ;
    int ill ;
    int ivstat = 0 ;
    int ivalue = 0 ;
    for(k = 0; (ivstat == 0) && (k < nb_iter_max) && (fabs(func) > epsn); k++)
       { fhpfld(ill, ivstat = 0, ivalue, phase) ;
         func = Tcelcius-ltg[0] ;
         lhg[0] = lhg[0]+func/ltg3[0] ;
         lhg[0] = std::min(lhg[0],xhvp) ;
         lhg[0] = std::max(lhg[0],xhvm) ;
       }
    if (unmap_eos_field(fh, phase) == 0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    if (ivstat != 0)  return convert_eos_error(ivstat) ;
    if (k >= nb_iter_max && (fabs(func) > epsn))  return convert_eos_error(h_pt_mix_newton) ;
    return EOS_Internal_Error::OK ; 
  }
  
  EOS_Error CATHARE2::calc2_lim() 
  { ArrOfDouble diff(nsca) ;
    diff   = lhvsp ;
    diff  -= lhlsp ;
    diff  *= 0.4   ;
    hllim  = lhlsp ;
    hllim += diff  ;
    hvlim  = lhvsp ;
    hvlim -= diff  ;
    return EOS_Error::ok ;
  }

  EOS_Error CATHARE2::calc2_critical()
  { if (pc > 0) 
       { ArrOfInt err_arr(1) ;
         EOS_Error_Field ferr(err_arr) ;
         ferr = EOS_Internal_Error::OK ;
         EOS_Field p("P", "p",NEPTUNE::p, 1, &pc) ;
         EOS_Field t("T_sat", "T_sat",NEPTUNE::T_sat, 1, &tc) ;
         EOS_Field h ;
         if (phase == liquid ) 
            { h = EOS_Field("h_l_sat","h_l_sat",NEPTUNE::h_l_sat, 1, &hc); }
         else
            { h = EOS_Field("h_v_sat","h_v_sat",NEPTUNE::h_v_sat, 1, &hc); }
         EOS_Fields out(2) ;
         out[0] = t ;
         out[1] = h ;                 
         calc2_p(p, out, ferr) ;
       }
    return EOS_Error::error ;
  }

  EOS_Internal_Error CATHARE2::get_mm(double& mm )
  { if (fldm > 0)
      { mm = fldm*1e-3 ;
        return EOS_Internal_Error::OK ;
      }
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error CATHARE2::get_p_crit(double& p_crit )
  { if (critical == EOS_Error::bad)  calc2_critical() ;
    if (pc > 0.e0)
       { p_crit = pc ;
         return EOS_Internal_Error::OK ;
       }
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error CATHARE2::get_h_crit(double& h_crit )
  { if (critical == EOS_Error::bad)  calc2_critical() ;
    if (pc > 0.e0)
       { h_crit = hc ;
         return EOS_Internal_Error::OK ;
       }
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error CATHARE2::get_T_crit(double& T_crit )
  { if (critical == EOS_Error::bad)  calc2_critical() ;
    if (pc > 0.e0)
      { T_crit = tc ;
        return EOS_Internal_Error::OK ;
      }
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error CATHARE2::get_p_min(double& p_min )
  { p_min = xpm ;
    return EOS_Internal_Error::OK ;
  }

  EOS_Internal_Error CATHARE2::get_p_max(double& p_max )
  { p_max = xpp ;
    return EOS_Internal_Error::OK ;
  }

  EOS_Internal_Error CATHARE2::get_h_min(double& h_min )
  { if (phase == liquid or phase == saturated)  h_min = xhlm ;
    if (phase == vapor)  h_min = xhvm ;
    return EOS_Internal_Error::OK ;
  }

  EOS_Internal_Error CATHARE2::get_h_max(double& h_max )
  { if (phase == liquid or phase == saturated)  h_max = xhlp ;
    if (phase == vapor)  h_max = xhvp ;
    return EOS_Internal_Error::OK ;
  }

  EOS_Internal_Error CATHARE2::get_T_min(double& T_min )
  { double tabsk_fluid = 0. ;
    if (lfluid != 100003)  tabsk_fluid = tabsk ;
    if (phase == liquid or phase == saturated)  T_min = xtlm+tabsk_fluid ;
    if (phase == vapor)  T_min = xtgm+tabsk_fluid ;
    return EOS_Internal_Error::OK ;
  }

  EOS_Internal_Error CATHARE2::get_T_max(double& T_max )
  { double tabsk_fluid=0.;
    if (lfluid != 100003) tabsk_fluid=tabsk;
    if (phase == liquid or phase == saturated) T_max=xtlp+tabsk_fluid;
    if (phase == vapor) T_max=xtgp+tabsk_fluid;
    return EOS_Internal_Error::OK;
  }

  EOS_Internal_Error CATHARE2::get_h_ref(double& h_ref )
  { h_ref = href ;
    return EOS_Internal_Error::OK ;
  }

  EOS_Internal_Error CATHARE2::get_T_ref(double& T_ref )
  { double tabsk_fluid = 0. ;
    if (lfluid != 100003)  tabsk_fluid = tabsk ;
    T_ref = tref + tabsk_fluid ;
    return EOS_Internal_Error::OK ;
  }

  EOS_Internal_Error CATHARE2::get_p_ref(double& p_ref )
  { p_ref = pc ; // not used at the moment so we put it at pc
    return EOS_Internal_Error::OK;
  }

  EOS_Internal_Error CATHARE2::compute_h_l_lim_p( double p, double& h_l_lim )
  { ArrOfInt err_arr(1) ;
    EOS_Error_Field ferr(err_arr) ;
    ferr = EOS_Internal_Error::OK ;
    EOS_Field pf("P", "p",NEPTUNE::p, 1, &p) ;
    EOS_Field hf("h_l_lim","h_l_lim",NEPTUNE::h_l_lim, 1, &h_l_lim) ;
    EOS_Fields out(1) ;
    out[0] = hf ;
    calc2_p(pf, out, ferr) ;
    return ferr[0] ;
  }

  EOS_Internal_Error CATHARE2::compute_h_v_lim_p( double p, double& h_v_lim )
  { ArrOfInt err_arr(1) ;
    EOS_Error_Field ferr(err_arr) ;
    ferr = EOS_Internal_Error::OK ;
    EOS_Field pf("P", "p",NEPTUNE::p, 1, &p);
    EOS_Field hf("h_v_lim","h_v_lim",NEPTUNE::h_v_lim, 1, &h_v_lim) ;
    EOS_Fields out(1) ;
    out[0] = hf ;
    calc2_p(pf, out, ferr) ;
    return ferr[0] ;
  }

  void CATHARE2::describe_error(const EOS_Internal_Error error, AString & description)
  { switch(error.get_partial_code()) 
       { case ::CATHARE2::erpile_error:
           description = "EOS_Cathare2: erpile error"; break;
         case ::CATHARE2::canceled:
           description = "EOS_Cathare2: computation canceled due to previous error"; break;
         case ::CATHARE2::unstable:
           description = "EOS_Cathare2: (p,h) is in unstable domain"; break;
         case ::CATHARE2::P_below_min:
           description = "EOS_Cathare2: p < pmin"; break;
         case ::CATHARE2::P_above_max:
           description = "EOS_Cathare2: p > pmax"; break;
         case ::CATHARE2::T_below_min:
           description = "EOS_Cathare2: t < tmin"; break;
         case ::CATHARE2::T_above_max:
           description = "EOS_Cathare2: t > tmax"; break;
         case ::CATHARE2::h_below_min:
           description = "EOS_Cathare2: h < hmin"; break;
         case ::CATHARE2::h_above_max:
           description = "EOS_Cathare2: h > hmax"; break;
         case ::CATHARE2::cathare_illp:
           description = "EOS_Cathare2: p is out of boundaries"; break;
         case ::CATHARE2::cathare_illhl:
           description = "EOS_Cathare2: hl is out of boundaries"; break;
         case ::CATHARE2::cathare_illhg:
           description = "EOS_Cathare2: hg is out of boundaries"; break;
         case ::CATHARE2::cathare_illhv:
           description = "EOS_Cathare2: hv is out of boundaries"; break;
         case ::CATHARE2::cathare_illtg:
           description = "EOS_Cathare2: tg is out of boundaries"; break;
         case ::CATHARE2::cathare_illtl:
           description = "EOS_Cathare2: tl is out of boundaries"; break;
         case ::CATHARE2::cathare_illtw:
           description = "EOS_Cathare2: tw is out of boundaries"; break;
         case ::CATHARE2::cathare_illag:
           description = "EOS_Cathare2: ag is out of boundaries"; break;
         case ::CATHARE2::cathare_illzc:
           description = "EOS_Cathare2: zc is out of boundaries"; break;
         case ::CATHARE2::cathare_illrg:
           description = "EOS_Cathare2: rg is out of boundaries"; break;
         case ::CATHARE2::cathare_illrl:
           description = "EOS_Cathare2: rl is out of boundaries"; break;
         case ::CATHARE2::cathare_illx:
           description = "EOS_Cathare2: x is out of boundaries"; break;
         case ::CATHARE2::cathare_illxsu:
           description = "EOS_Cathare2:  xsu is out of boundaries"; break;
         case ::CATHARE2::cathare_illpv:
           description = "EOS_Cathare2:  pv is out of boundaries"; break;
         case ::CATHARE2::cathare_illcvr:
           description = "EOS_Cathare2:  cvr is out of boundaries"; break ;
         case ::CATHARE2::cathare_illpiv:
           description = "EOS_Cathare2:  piv is out of boundaries"; break ;
         default:
           description = "Unknown error"; break ;
       }
  }

  EOS_Internal_Error CATHARE2::convert_eos_error(const int partial_code) const
  {
//    return EOS_Internal_Error::OK;
    switch (partial_code) 
       { case ::CATHARE2::ok:               return EOS_Internal_Error::OK ;
         case ::CATHARE2::canceled:         return EOS_Internal_Error(partial_code, EOS_Error::bad)   ;
         case ::CATHARE2::cathare_illp:     return EOS_Internal_Error(partial_code, EOS_Error::bad)   ;
         case ::CATHARE2::cathare_illhl:    return EOS_Internal_Error(partial_code, EOS_Error::bad)   ;
         case ::CATHARE2::cathare_illhv:    return EOS_Internal_Error(partial_code, EOS_Error::bad)   ;
         case ::CATHARE2::cathare_illhg:    return EOS_Internal_Error(partial_code, EOS_Error::bad)   ;
         case ::CATHARE2::cathare_illtg:    return EOS_Internal_Error(partial_code, EOS_Error::bad)   ;
         case ::CATHARE2::cathare_illtl:    return EOS_Internal_Error(partial_code, EOS_Error::bad)   ;
         case ::CATHARE2::cathare_illtw:    return EOS_Internal_Error(partial_code, EOS_Error::bad)   ;
         case ::CATHARE2::cathare_illag:    return EOS_Internal_Error(partial_code, EOS_Error::bad)   ;
         case ::CATHARE2::cathare_illzc:    return EOS_Internal_Error(partial_code, EOS_Error::bad)   ;
         case ::CATHARE2::cathare_illrg:    return EOS_Internal_Error(partial_code, EOS_Error::bad)   ;
         case ::CATHARE2::cathare_illx:     return EOS_Internal_Error(partial_code, EOS_Error::bad)   ;
         case ::CATHARE2::cathare_illxsu:   return EOS_Internal_Error(partial_code, EOS_Error::bad)   ;
         case ::CATHARE2::cathare_illpv:    return EOS_Internal_Error(partial_code, EOS_Error::bad)   ;
         case ::CATHARE2::cathare_illcvr:   return EOS_Internal_Error(partial_code, EOS_Error::bad)   ;
         case ::CATHARE2::cathare_illpiv:   return EOS_Internal_Error(partial_code, EOS_Error::bad)   ;
         case ::CATHARE2::h_pt_mix_newton:  return EOS_Internal_Error(partial_code, EOS_Error::bad)   ;
         case ::CATHARE2::erpile_error:     return EOS_Internal_Error(partial_code, EOS_Error::error) ;
         default:
           return EOS_Internal_Error(partial_code, EOS_Error::bad) ;
       }
  }

}
