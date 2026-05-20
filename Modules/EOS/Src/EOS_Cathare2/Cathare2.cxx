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
#include "EOS/API/EOS_Error.hxx"
#include "EOS/API/satur_properties.hxx"
#include "Language/API/ArrOfDouble.hxx"
#include "tracy/Tracy.hpp"

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
  { ZoneScopedN("CATHARE2::CATHARE2");
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
  { ZoneScopedN("CATHARE2::~CATHARE2");
  }

  EOS_Error CATHARE2::verify(const EOS_Field& in,
                             EOS_Error_Field& errfield, 
                             const domain dom) const
  { ZoneScopedN("CATHARE2::verify");
    double tabsk_fluid = 0.e0 ;
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
    {
    ZoneScopedN("CATHARE2::verify for loop");
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
    }
    return errfield.find_worst_error().generic_error() ;
  }

  int CATHARE2::map_eos_field(const EOS_Field& f, domain mode)
  { ZoneScopedN("CATHARE2::map_eos_field");
    assert(f.size() == nsca) ;

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
            else if (mode == saturated)   ltsp.set_ptr(nsca, f.get_data().get_ptr());
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
  { ZoneScopedN("CATHARE2::map_eos_fields");
    int f_size = f.size() ;
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
    //ArrOfDouble ltmp ;
    ZoneScopedN("CATHARE2::unmap_eos_field");
    switch(f.get_property_number()) 
       {
         case NEPTUNE::p :
            lp.clear();
            break ;
         case NEPTUNE::h :
            if (mode == liquid) 
               { if (typ_ths == TH_space::PT)
                    lhlpt.clear() ;
                 else
                    lhl.clear() ;
               }
            else if (mode == vapor)
               {  if (nincon == 0 && typ_ths == TH_space::PT)
                     lhvpt.clear() ;
                  else
                     { lhl.clear() ;
                       lhg.clear() ;
                     }
               }
            else if (mode == unknown) lh.clear();
            break;
         case NEPTUNE::T:
            if (mode == liquid) ltl.clear();
            else if (mode == vapor) ltg.clear();
            else if (mode == unknown) lt.clear();
            break;
         case NEPTUNE::rho:
            if (mode == liquid)
               { if (typ_ths == TH_space::PT)
                    lrlpt.clear() ;
                 else
                    lrl.clear() ;
               }
            else if (mode == vapor)
               { if (nincon == 0 && typ_ths == TH_space::PT)
                   lrvpt.clear() ;
                 else
                   lrg.clear() ;
               }
            else if (mode == unknown) lr.clear();
            break;
         case NEPTUNE::cp:
            if (mode == liquid)
               { if (typ_ths == TH_space::PT)
                    lcplpt.clear() ;
                 else
                    lcpl.clear() ;
               }
            else if (mode == vapor) 
               { if (nincon == 0 && typ_ths == TH_space::PT)
                    lcpvpt.clear() ;
                 else
                    lcpg.clear() ;
               }
            else if (mode == unknown) lcp.clear();
            break;
         case NEPTUNE::beta:
            if (mode == liquid) lbetal.clear();
            else if (mode == unknown) lbetal.clear();
            break;
         case NEPTUNE::lambda:
            if (mode == liquid) ltlal.clear();
            else if (mode == vapor) ltlag.clear();
            else if (mode == unknown) lla.clear();
            break;
         case NEPTUNE::mu:
            if (mode == liquid) ltmul.clear();
            else if (mode == vapor) ltmug.clear();
            else if (mode == unknown) lmu.clear();
            break;
         case NEPTUNE::sigma:
            if (mode == vapor) lsi.clear();
            else if (mode == unknown) lsi.clear();
            break;
         case NEPTUNE::gamma: lkiseng.clear(); break ;
         case NEPTUNE::pr:
            if (mode == vapor) lprandg.clear();
            if (mode == unknown) lprandg.clear();
            break;
         case NEPTUNE::d_T_d_p_h:
            if (mode == liquid) ltl1.clear();
            else if (mode == vapor) ltg1.clear();
            else if (mode == unknown) lt1.clear();
            break;
         case NEPTUNE::d_T_d_h_p:
            if (mode == liquid) ltl2.clear();
            else if (mode == vapor) ltg3.clear();
            else if (mode == unknown) lt2.clear();
            break;
         case NEPTUNE::d_h_d_T_p:
            if (mode == liquid)
               { if (typ_ths == TH_space::PT)
                    lhl2pt.clear() ;
                 else
                    lhl2.clear() ;
               }
            else if (mode == vapor) 
               { if (nincon == 0 && typ_ths == TH_space::PT)
                    lcpvpt.clear() ;
                 else
                    lcpg.clear() ;
               }
            break;
         case NEPTUNE::d_h_d_p_T:
            if (mode == liquid)
               { if (typ_ths == TH_space::PT)
                    lhl1pt.clear() ;
                 else
                    lhl1.clear() ;
               }
            else if (mode == vapor) 
               { if (nincon == 0 && typ_ths == TH_space::PT)
                    lhv1pt.clear() ;
                 else
                    lhg1.clear() ;
               }
            break;
         case NEPTUNE::d_rho_d_p_h:
            if (mode == liquid) lrl1.clear();
            else if (mode == vapor)
               { if (nincon == 0) 
                    lrvpv.clear();
                 else
                    lrg1.clear();
               }
            else if (mode == unknown) lr1.clear();
            break;
         case NEPTUNE::d_rho_d_h_p:
            if (mode == liquid) lrl2.clear();
            else if (mode == vapor)
               { if (nincon == 0) 
                    lrvhv.clear();
                 else
                    lrg3.clear();
               }
            else if (mode == unknown) lr2.clear();
            break;
         case NEPTUNE::d_rho_d_p_T:
            if      (mode == liquid) lrl1pt.clear();
            else if (mode == vapor)  lrv1pt.clear();
            break;
         case NEPTUNE::d_rho_d_T_p:
            if      (mode == liquid) lrl2pt.clear();
            else if (mode == vapor)  lrv3pt.clear();
            break;
         case NEPTUNE::d_cp_d_p_h:
            if (mode == liquid) lcpl1.clear();
            else if (mode == vapor)
               { if (nincon == 0) 
                    lcpvpv.clear();
                 else
                    lcpg1.clear();
               }
            else if (mode == unknown) lcp1.clear();
            break;
         case NEPTUNE::d_cp_d_h_p:
            if (mode == liquid) lcpl2.clear();
            else if (mode == vapor)
               { if (nincon == 0) 
                    lcpvhv.clear();
                 else
                    lcpg3.clear();
               }
            else if (mode == unknown) lcp2.clear();
            break;
         case NEPTUNE::d_beta_d_p_h:
            if (mode == liquid) lbetal1.clear();
            else if (mode == unknown) lbetal1.clear();
            break;
         case NEPTUNE::d_beta_d_h_p:
            if (mode == liquid) lbetal2.clear();
            else if (mode == unknown) lbetal2.clear();
            break;
         case NEPTUNE::d_lambda_d_p_h:
            if (mode == liquid) ltlal1.clear();
            else if (mode == vapor) ltlag1.clear();
            else if (mode == unknown) lla1.clear();
            break;
         case NEPTUNE::d_lambda_d_h_p:
            if (mode == liquid) ltlal2.clear();
            else if (mode == vapor) ltlag3.clear();
            else if (mode == unknown) lla2.clear();
            break;
         case NEPTUNE::d_lambda_d_T_p:
            if (mode == vapor) llagtg.clear();
            else if (mode == unknown) llagtg.clear();
            break;
         case NEPTUNE::d_mu_d_p_h:
            if (mode == liquid) ltmul1.clear();
            else if (mode == vapor) ltmug1.clear();
            else if (mode == unknown) lmu1.clear();
            break;
         case NEPTUNE::d_mu_d_h_p:
            if (mode == liquid) ltmul2.clear();
            else if (mode == vapor) ltmug3.clear();
            else if (mode == unknown) lmu2.clear();
            break;
         case NEPTUNE::d_mu_d_T_p:
            if (mode == vapor) lmugtg.clear();
            break;
         case NEPTUNE::d_sigma_d_p_h:
            if (mode == vapor) lsi1.clear();
            else if (mode == unknown) lsi1.clear();
            break;
         case NEPTUNE::d_sigma_d_h_p:
            if (mode == vapor) lsi3.clear();
            else if (mode == unknown) lsi3.clear();
            break;
         case NEPTUNE::d_cp_d_p_T:
            if      (mode == liquid)  lcpl1pt.clear();
            else if (mode == vapor)   lcpvpvpt.clear();
            else if (mode == unknown) lcpl1pt.clear();
            break;
         case NEPTUNE::d_cp_d_T_p:
            if      (mode == liquid)  lcpl2pt.clear();
            else if (mode == vapor)   lcpgtgpt.clear();
            else if (mode == unknown) lcpgtg.clear();
            break;
         case NEPTUNE::p_sat            : lp.clear() ; break ;
         case NEPTUNE::T_sat            : ltsp.clear() ; break ;
         case NEPTUNE::h_l_sat          : lhlsp.clear() ; break ;
         case NEPTUNE::h_v_sat          : lhvsp.clear() ; break ;
         case NEPTUNE::cp_l_sat         : lcplsp.clear() ; break ;
         case NEPTUNE::cp_v_sat         : lcpvsp.clear() ; break ;
         case NEPTUNE::rho_l_sat        : lrlsp.clear() ; break ;
         case NEPTUNE::rho_v_sat        : lrvsp.clear() ; break ;
         case NEPTUNE::d_T_sat_d_p      : ltsp1.clear() ; break ;
         case NEPTUNE::d_h_l_sat_d_p    : lhlsp1.clear() ; break ;
         case NEPTUNE::d_h_v_sat_d_p    : lhvsp1.clear() ; break ;
         case NEPTUNE::d_cp_l_sat_d_p   : lclsp1.clear() ; break ;
         case NEPTUNE::d_cp_v_sat_d_p   : lcvsp1.clear() ; break ;
         case NEPTUNE::d_rho_l_sat_d_p  : lrlsp1.clear() ; break ;
         case NEPTUNE::d_rho_v_sat_d_p  : lrvsp1.clear() ; break ;
         case NEPTUNE::d2_T_sat_d_p_d_p : l2tsp1.clear() ; break ;

         case NEPTUNE::p_lim   : lp.clear() ; break ;
         case NEPTUNE::h_l_lim : hllim.clear() ; break ;
         case NEPTUNE::h_v_lim : hvlim.clear() ; break ;

         case NEPTUNE::c_0:        if (nincon > 0 && mode == vapor)  lxvap.clear(); break;
         case NEPTUNE::c_1:        if (nincon > 0 && mode == vapor)  lx[0].clear(); break;
         case NEPTUNE::c_2:        if (nincon > 0 && mode == vapor)  lx[1].clear(); break;
         case NEPTUNE::c_3:        if (nincon > 0 && mode == vapor)  lx[2].clear(); break;
         case NEPTUNE::c_4:        if (nincon > 0 && mode == vapor)  lx[3].clear(); break;
         case NEPTUNE::p_0:        if (nincon > 0 && mode == vapor)  lpv.clear(); break;
         case NEPTUNE::p_1:        if (nincon > 0 && mode == vapor)  lpx[0].clear(); break;
         case NEPTUNE::p_2:        if (nincon > 0 && mode == vapor)  lpx[1].clear(); break;
         case NEPTUNE::p_3:        if (nincon > 0 && mode == vapor)  lpx[2].clear(); break;
         case NEPTUNE::p_4:        if (nincon > 0 && mode == vapor)  lpx[3].clear(); break;
         case NEPTUNE::h_0:        if (nincon > 0 && mode == vapor)  lhv.clear(); break;
         case NEPTUNE::h_1:        if (nincon > 0 && mode == vapor)  lhx[0].clear(); break;
         case NEPTUNE::h_2:        if (nincon > 0 && mode == vapor)  lhx[1].clear(); break;
         case NEPTUNE::h_3:        if (nincon > 0 && mode == vapor)  lhx[2].clear(); break;
         case NEPTUNE::h_4:        if (nincon > 0 && mode == vapor)  lhx[3].clear(); break;
         case NEPTUNE::cp_0:       if (nincon > 0 && mode == vapor)  lcpv.clear(); break;
         case NEPTUNE::rho_0:      if (nincon > 0 && mode == vapor)  lrv.clear(); break;
         case NEPTUNE::lambda_0:   if (nincon > 0 && mode == vapor)  ltlav.clear(); break;
         case NEPTUNE::mu_0:       if (nincon > 0 && mode == vapor)  ltmuv.clear(); break;
         case NEPTUNE::T_sat_0:    if (nincon > 0 && mode == vapor)  ltspv.clear(); break;
         case NEPTUNE::h_l_sat_0:  if (nincon > 0 && mode == vapor)  lhlsv.clear(); break;
         case NEPTUNE::h_v_sat_0:  if (nincon > 0 && mode == vapor)  lhvsv.clear(); break;
         case NEPTUNE::cp_l_sat_0: if (nincon > 0 && mode == vapor)  lcplsv.clear(); break;
         case NEPTUNE::cp_v_sat_0: if (nincon > 0 && mode == vapor)  lcpvsv.clear(); break;
         case NEPTUNE::rho_l_sat_0:if (nincon > 0 && mode == vapor)  lrlsv.clear(); break;
         case NEPTUNE::rho_v_sat_0:if (nincon > 0 && mode == vapor)  lrvsv.clear(); break;
         case NEPTUNE::dncv:       if (nincon > 0 && mode == vapor)  ldncv.clear(); break;
         case NEPTUNE::rnc:        if (nincon > 0 && mode == vapor)  lrnc.clear(); break;
         case NEPTUNE::mnc:        if (nincon > 0 && mode == vapor)  lmnc.clear(); break;
         case NEPTUNE::prgr:       if (nincon > 0 && mode == vapor)  lprgr.clear(); break;
         case NEPTUNE::xnc:        if (nincon > 0 && mode == vapor)  lxnc.clear(); break;
         // First Derivatives
         case NEPTUNE::d_p_0_d_p_h:           if (nincon > 0 && mode == vapor)  lpv1.clear(); break;
         case NEPTUNE::d_p_0_d_h_p:           if (nincon > 0 && mode == vapor)  lpv3.clear(); break;
         case NEPTUNE::d_p_0_d_c_1_ph:        if (nincon > 0 && mode == vapor)  lpvx[0].clear(); break;
         case NEPTUNE::d_p_0_d_c_2_ph:        if (nincon > 0 && mode == vapor)  lpvx[1].clear(); break;
         case NEPTUNE::d_p_0_d_c_3_ph:        if (nincon > 0 && mode == vapor)  lpvx[2].clear(); break;
         case NEPTUNE::d_p_0_d_c_4_ph:        if (nincon > 0 && mode == vapor)  lpvx[3].clear(); break;
         case NEPTUNE::d_h_0_d_p_h:           if (nincon > 0 && mode == vapor)  lhv1.clear(); break;
         case NEPTUNE::d_h_0_d_h_p:           if (nincon > 0 && mode == vapor)  lhv3.clear(); break;
         case NEPTUNE::d_h_0_d_c_1_ph:        if (nincon > 0 && mode == vapor)  lhvx[0].clear(); break;
         case NEPTUNE::d_h_0_d_c_2_ph:        if (nincon > 0 && mode == vapor)  lhvx[1].clear(); break;
         case NEPTUNE::d_h_0_d_c_3_ph:        if (nincon > 0 && mode == vapor)  lhvx[2].clear(); break;
         case NEPTUNE::d_h_0_d_c_4_ph:        if (nincon > 0 && mode == vapor)  lhvx[3].clear(); break;
         case NEPTUNE::d_h_d_c_1_pT:          if (nincon > 0 && mode == vapor)  lhgx[0].clear(); break;
         case NEPTUNE::d_h_d_c_2_pT:          if (nincon > 0 && mode == vapor)  lhgx[1].clear(); break;
         case NEPTUNE::d_h_d_c_3_pT:          if (nincon > 0 && mode == vapor)  lhgx[2].clear(); break;
         case NEPTUNE::d_h_d_c_4_pT:          if (nincon > 0 && mode == vapor)  lhgx[3].clear(); break;
         case NEPTUNE::d_T_d_h_0_p:           if (nincon > 0 && mode == vapor)  ltghv.clear(); break;
         case NEPTUNE::d_T_d_p_0_h:           if (nincon > 0 && mode == vapor)  ltgpv.clear(); break;
         case NEPTUNE::d_T_d_c_1_ph:          if (nincon > 0 && mode == vapor)  ltgx[0].clear(); break;
         case NEPTUNE::d_T_d_c_2_ph:          if (nincon > 0 && mode == vapor)  ltgx[1].clear(); break;
         case NEPTUNE::d_T_d_c_3_ph:          if (nincon > 0 && mode == vapor)  ltgx[2].clear(); break;
         case NEPTUNE::d_T_d_c_4_ph:          if (nincon > 0 && mode == vapor)  ltgx[3].clear(); break;
         case NEPTUNE::d_cp_d_c_1_ph:         if (nincon > 0 && mode == vapor)  lcpgx[0].clear(); break;
         case NEPTUNE::d_cp_d_c_2_ph:         if (nincon > 0 && mode == vapor)  lcpgx[1].clear(); break;
         case NEPTUNE::d_cp_d_c_3_ph:         if (nincon > 0 && mode == vapor)  lcpgx[2].clear(); break;
         case NEPTUNE::d_cp_d_c_4_ph:         if (nincon > 0 && mode == vapor)  lcpgx[3].clear(); break;
         case NEPTUNE::d_cp_0_d_p_0_h:        if (nincon > 0 && mode == vapor)  lcpvpv.clear(); break;
         case NEPTUNE::d_cp_0_d_h_0_p:        if (nincon > 0 && mode == vapor)  lcpvhv.clear(); break;
         case NEPTUNE::d_rho_d_c_1_ph:        if (nincon > 0 && mode == vapor)  lrgx[0].clear(); break;
         case NEPTUNE::d_rho_d_c_2_ph:        if (nincon > 0 && mode == vapor)  lrgx[1].clear(); break;
         case NEPTUNE::d_rho_d_c_3_ph:        if (nincon > 0 && mode == vapor)  lrgx[2].clear(); break;
         case NEPTUNE::d_rho_d_c_4_ph:        if (nincon > 0 && mode == vapor)  lrgx[3].clear(); break;
         case NEPTUNE::d_mu_d_c_1_ph:         if (nincon > 0 && mode == vapor)  ltmugx[0].clear(); break;
         case NEPTUNE::d_mu_d_c_2_ph:         if (nincon > 0 && mode == vapor)  ltmugx[1].clear(); break;
         case NEPTUNE::d_mu_d_c_3_ph:         if (nincon > 0 && mode == vapor)  ltmugx[2].clear(); break;
         case NEPTUNE::d_mu_d_c_4_ph:         if (nincon > 0 && mode == vapor)  ltmugx[3].clear(); break;
         case NEPTUNE::d_sigma_d_c_1_ph:      if (nincon > 0 && mode == vapor)  lsix[0].clear(); break;
         case NEPTUNE::d_sigma_d_c_2_ph:      if (nincon > 0 && mode == vapor)  lsix[1].clear(); break;
         case NEPTUNE::d_sigma_d_c_3_ph:      if (nincon > 0 && mode == vapor)  lsix[2].clear(); break;
         case NEPTUNE::d_sigma_d_c_4_ph:      if (nincon > 0 && mode == vapor)  lsix[3].clear(); break;
         case NEPTUNE::d_lambda_d_c_1_ph:     if (nincon > 0 && mode == vapor)  ltlagx[0].clear(); break;
         case NEPTUNE::d_lambda_d_c_2_ph:     if (nincon > 0 && mode == vapor)  ltlagx[1].clear(); break;
         case NEPTUNE::d_lambda_d_c_3_ph:     if (nincon > 0 && mode == vapor)  ltlagx[2].clear(); break;
         case NEPTUNE::d_lambda_d_c_4_ph:     if (nincon > 0 && mode == vapor)  ltlagx[3].clear(); break;
         case NEPTUNE::d_sigma_d_p_0_h:       if (nincon > 0 && mode == vapor)  lsipv.clear(); break;
         case NEPTUNE::d_T_sat_0_d_p_0_h:     if (nincon > 0 && mode == vapor)  ltspvv.clear(); break;
         case NEPTUNE::d_h_l_sat_0_d_p_0_h:   if (nincon > 0 && mode == vapor)  lhlsvv.clear(); break;
         case NEPTUNE::d_h_v_sat_0_d_p_0_h:   if (nincon > 0 && mode == vapor)  lhvsvv.clear(); break;
         case NEPTUNE::d_cp_l_sat_0_d_p_0_h:  if (nincon > 0 && mode == vapor)  lclsvv.clear(); break;
         case NEPTUNE::d_cp_v_sat_0_d_p_0_h:  if (nincon > 0 && mode == vapor)  lcvsvv.clear(); break;
         case NEPTUNE::d_rho_l_sat_0_d_p_0_h: if (nincon > 0 && mode == vapor)  lrlsvv.clear(); break;
         case NEPTUNE::d_rho_v_sat_0_d_p_0_h: if (nincon > 0 && mode == vapor)  lrvsvv.clear(); break;
         case NEPTUNE::d_lambda_0_d_p_0_h:    if (nincon > 0 && mode == vapor)  llavpv.clear(); break;
         case NEPTUNE::d_lambda_0_d_T_p:      if (nincon > 0 && mode == vapor)  llavtg.clear(); break;
         case NEPTUNE::d_rho_0_d_p_h:         if (nincon > 0 && mode == vapor)  lrv1.clear(); break;
         case NEPTUNE::d_rho_0_d_h_p:         if (nincon > 0 && mode == vapor)  lrv3.clear(); break;
         case NEPTUNE::d_rho_0_d_p_0_h:       if (nincon > 0 && mode == vapor)  lrvpv.clear(); break;
         case NEPTUNE::d_rho_0_d_h_0_p:       if (nincon > 0 && mode == vapor)  lrvhv.clear(); break;
         case NEPTUNE::d_rho_0_d_c_1_ph:      if (nincon > 0 && mode == vapor)  lrvx[0].clear(); break;
         case NEPTUNE::d_rho_0_d_c_2_ph:      if (nincon > 0 && mode == vapor)  lrvx[1].clear(); break;
         case NEPTUNE::d_rho_0_d_c_3_ph:      if (nincon > 0 && mode == vapor)  lrvx[2].clear(); break;
         case NEPTUNE::d_rho_0_d_c_4_ph:      if (nincon > 0 && mode == vapor)  lrvx[3].clear(); break;
         case NEPTUNE::d_rnc_d_c_1_ph:        if (nincon > 0 && mode == vapor)  lrncx[0].clear(); break;
         case NEPTUNE::d_rnc_d_c_2_ph:        if (nincon > 0 && mode == vapor)  lrncx[1].clear(); break;
         case NEPTUNE::d_rnc_d_c_3_ph:        if (nincon > 0 && mode == vapor)  lrncx[2].clear(); break;
         case NEPTUNE::d_rnc_d_c_4_ph:        if (nincon > 0 && mode == vapor)  lrncx[3].clear(); break;
         case NEPTUNE::d_mnc_d_c_1_ph:        if (nincon > 0 && mode == vapor)  lmncx[0].clear(); break;
         case NEPTUNE::d_mnc_d_c_2_ph:        if (nincon > 0 && mode == vapor)  lmncx[1].clear(); break;
         case NEPTUNE::d_mnc_d_c_3_ph:        if (nincon > 0 && mode == vapor)  lmncx[2].clear(); break;
         case NEPTUNE::d_mnc_d_c_4_ph:        if (nincon > 0 && mode == vapor)  lmncx[3].clear(); break;
         case NEPTUNE::d_dncv_d_c_1_ph:       if (nincon > 0 && mode == vapor)  ldncvx[0].clear(); break;
         case NEPTUNE::d_dncv_d_c_2_ph:       if (nincon > 0 && mode == vapor)  ldncvx[1].clear(); break;
         case NEPTUNE::d_dncv_d_c_3_ph:       if (nincon > 0 && mode == vapor)  ldncvx[2].clear(); break;
         case NEPTUNE::d_dncv_d_c_4_ph:       if (nincon > 0 && mode == vapor)  ldncvx[3].clear(); break;
         case NEPTUNE::d_dncv_d_p_h:          if (nincon > 0 && mode == vapor)  ldncv1.clear(); break;
         case NEPTUNE::d_dncv_d_h_p:          if (nincon > 0 && mode == vapor)  ldncv3.clear(); break;
         case NEPTUNE::d_mu_0_d_T_p:          if (nincon > 0 && mode == vapor)  lmuvtg.clear(); break;
         case NEPTUNE::d_mu_0_d_p_0_h:        if (nincon > 0 && mode == vapor)  lmuvpv.clear(); break;
         // Second Derivatives
         case NEPTUNE::d2_T_sat_0_d_p_0_d_p_0: if (nincon > 0 && mode == vapor) l2tsdpvv.clear(); break;

         // Cathare2 IAPWS Thermodynamic Properties
         case NEPTUNE::epstl:              if (mode == liquid)  lepstliq.clear();  break;
         case NEPTUNE::hlspsc:             if (mode == liquid)  lhlspsc.clear();  break;
         case NEPTUNE::hlsvsc:             if (mode == liquid)  lhlsvsc.clear();  break;
         case NEPTUNE::epstg:              if (mode == liquid)  lepstgas.clear();  break;
         case NEPTUNE::hvspsc:             if (mode == liquid)  lhvspsc.clear();  break;
         case NEPTUNE::hvsvsc:             if (mode == liquid)  lhvsvsc.clear();  break;
         case NEPTUNE::d_epstl_dp_h:       if (mode == liquid)  lepstliq1.clear();  break;
         case NEPTUNE::d_epstl_dh_p:       if (mode == liquid)  lepstliq2.clear();  break;
         case NEPTUNE::d_hlspsc_dp_h:      if (mode == liquid)  lhlspsc1.clear();  break;
         case NEPTUNE::d_hlspsc_dh_p:      if (mode == liquid)  lhlspsc2.clear();  break;
         case NEPTUNE::d_hlsvsc_dp_h:      if (mode == liquid)  lhlsvsc1.clear();  break;
         case NEPTUNE::d_hlsvsc_dh_p0:     if (mode == liquid)  lhlsvsc2.clear();  break;
         case NEPTUNE::d_hlsvsc_dh_p:      if (mode == liquid)  lhlsvsc3.clear();  break;
         case NEPTUNE::d_hlsvsc_d_c_1_ph:  if (mode == liquid)  lhlsvscx[0].clear();  break;
         case NEPTUNE::d_hlsvsc_d_c_2_ph:  if (mode == liquid)  lhlsvscx[1].clear();  break;
         case NEPTUNE::d_hlsvsc_d_c_3_ph:  if (mode == liquid)  lhlsvscx[2].clear();  break;
         case NEPTUNE::d_hlsvsc_d_c_4_ph:  if (mode == liquid)  lhlsvscx[3].clear();  break;
         case NEPTUNE::d_epstg_dp_h:       if (mode == vapor)   lepstgas1.clear();  break;
         case NEPTUNE::d_epstg_dh_p:       if (mode == vapor)   lepstgas3.clear();  break;
         case NEPTUNE::d_epstg_d_c_1_ph:   if (mode == vapor)   lepstgasx[0].clear();  break;
         case NEPTUNE::d_epstg_d_c_2_ph:   if (mode == vapor)   lepstgasx[1].clear();  break;
         case NEPTUNE::d_epstg_d_c_3_ph:   if (mode == vapor)   lepstgasx[2].clear();  break;
         case NEPTUNE::d_epstg_d_c_4_ph:   if (mode == vapor)   lepstgasx[3].clear();  break;
         case NEPTUNE::d_hvspsc_dp_h:      if (mode == vapor)   lhvspsc1.clear();  break;
         case NEPTUNE::d_hvspsc_dh_p:      if (mode == vapor)   lhvspsc3.clear();  break;
         case NEPTUNE::d_hvspsc_d_c_1_ph:  if (mode == vapor)   lhvspscx[0].clear();  break;
         case NEPTUNE::d_hvspsc_d_c_2_ph:  if (mode == vapor)   lhvspscx[1].clear();  break;
         case NEPTUNE::d_hvspsc_d_c_3_ph:  if (mode == vapor)   lhvspscx[2].clear();  break;
         case NEPTUNE::d_hvspsc_d_c_4_ph:  if (mode == vapor)   lhvspscx[3].clear();  break;
         case NEPTUNE::d_hvsvsc_dp_h:      if (mode == vapor)   lhvsvsc1.clear();  break;
         case NEPTUNE::d_hvsvsc_dh_p:      if (mode == vapor)   lhvsvsc3.clear();  break;
         case NEPTUNE::d_hvsvsc_d_c_1_ph:  if (mode == vapor)   lhvsvscx[0].clear();  break;
         case NEPTUNE::d_hvsvsc_d_c_2_ph:  if (mode == vapor)   lhvsvscx[1].clear();  break;
         case NEPTUNE::d_hvsvsc_d_c_3_ph:  if (mode == vapor)   lhvsvscx[2].clear();  break;
         case NEPTUNE::d_hvsvsc_d_c_4_ph:  if (mode == vapor)   lhvsvscx[3].clear();  break;
         default: 
            return 0 ;
       }
    
    return 1 ;
  }


  int CATHARE2::unmap_eos_fields(const EOS_Fields& f, domain mode) 
  { ZoneScopedN("CATHARE2::unmap_eos_fields");
    assert (f.size() > 0) ;
    int nbunmap = 0 ;
    for (int i=0; i<f.size(); i++)
       { nbunmap += unmap_eos_field(f[i], mode) ;
       }
    return nbunmap ;
  }


  void CATHARE2::rezise_eos_fields(int sz, domain mode) 
  { ZoneScopedN("CATHARE2::resize_eos_fields");
   
    std::vector<ArrOfDouble*> vec; 
    if (ltsp.size() != sz) vec.push_back(&ltsp);
    if (ltsp1.size() != sz) vec.push_back(&ltsp1);
    if (l2tsp1.size() != sz) vec.push_back(&l2tsp1);
    if (lhlsp.size() != sz) vec.push_back(&lhlsp);
    if (lhlsp1.size() != sz) vec.push_back(&lhlsp1);
    if (lhvsp.size() != sz) vec.push_back(&lhvsp);
    if (lhvsp1.size() != sz) vec.push_back(&lhvsp1);
    if (lcplsp.size() != sz) vec.push_back(&lcplsp);
    if (lclsp1.size() != sz) vec.push_back(&lclsp1);
    if (lcpvsp.size() != sz) vec.push_back(&lcpvsp);
    if (lcvsp1.size() != sz) vec.push_back(&lcvsp1);
    if (lrlsp.size() != sz) vec.push_back(&lrlsp);
    if (lrlsp1.size() != sz) vec.push_back(&lrlsp1);
    if (lrvsp.size() != sz) vec.push_back(&lrvsp);
    if (lrvsp1.size() != sz) vec.push_back(&lrvsp1);
    if (lp.size() != sz) vec.push_back(&lp);
    if (lhf.size() != sz) vec.push_back(&lhf);
    if (lhf1.size() != sz) vec.push_back(&lhf1);
    if (ld2tl.size() != sz) vec.push_back(&ld2tl);
    if (ld3tl.size() != sz) vec.push_back(&ld3tl);
    if (ld2tg.size() != sz) vec.push_back(&ld2tg);
    if (ld3tg.size() != sz) vec.push_back(&ld3tg);
    if (ltl21.size() != sz) vec.push_back(&ltl21);
    if (ltg31.size() != sz) vec.push_back(&ltl21);
    if (lptemp.size() != sz) vec.push_back(&lptemp);
    if (lro7.size() != sz) vec.push_back(&lro7);
    if (lro73.size() != sz) vec.push_back(&lro73);
    if (lro72.size() != sz) vec.push_back(&lro72);
    if (lro71.size() != sz) vec.push_back(&lro71);
    if (lro721.size() != sz) vec.push_back(&lro721);
    if (lro731.size() != sz) vec.push_back(&lro731);
    if (ltp10.size() != sz) vec.push_back(&ltp10);
    if (ltp101.size() != sz) vec.push_back(&ltp101);
    if (ltp102.size() != sz) vec.push_back(&ltp102);
    if (ltp1021.size() != sz) vec.push_back(&ltp1021);
    if (ltp1031.size() != sz) vec.push_back(&ltp1031);
    if (ltgh2wrk.size() != sz) vec.push_back(&ltgh2wrk);
    if (ldencvx.size() != sz) vec.push_back(&ldencvx);
    if (lepstliq.size() != sz) vec.push_back(&lepstliq);
    if (lepstliq1.size() != sz) vec.push_back(&lepstliq1);
    if (lepstliq2.size() != sz) vec.push_back(&lepstliq2);
    if (lepstgas.size() != sz) vec.push_back(&lepstgas);
    if (lepstgas1.size() != sz) vec.push_back(&lepstgas1);
    if (lepstgas3.size() != sz) vec.push_back(&lepstgas3);
    if (lhlspsc.size() != sz) vec.push_back(&lhlspsc);
    if (lhlspscv.size() != sz) vec.push_back(&lhlspscv);
    if (lhlspsc1.size() != sz) vec.push_back(&lhlspsc1);
    if (lhlspsc2.size() != sz) vec.push_back(&lhlspsc2);
    if (lhlspsc3.size() != sz) vec.push_back(&lhlspsc3);
    if (lhvspsc.size() != sz) vec.push_back(&lhvspsc);
    if (lhvspscpv.size() != sz) vec.push_back(&lhvspscpv);
    if (lhvspsctg.size() != sz) vec.push_back(&lhvspsctg);
    if (lhvspsc1.size() != sz) vec.push_back(&lhvspsc1);
    if (lhvspsc2.size() != sz) vec.push_back(&lhvspsc2);
    if (lhvspsc3.size() != sz) vec.push_back(&lhvspsc3);
    if (lhlsvscv.size() != sz) vec.push_back(&lhlsvscv);
    if (lhvsvscpv.size() != sz) vec.push_back(&lhvsvscpv);
    if (lhvsvsctg.size() != sz) vec.push_back(&lhvsvsctg);
    if (hllim.size() != sz) vec.push_back(&hllim);
    if (hvlim.size() != sz) vec.push_back(&hvlim);
    if (lbetal.size() != sz) vec.push_back(&lbetal);
    if (lbetal1.size() != sz) vec.push_back(&lbetal1);
    if (lbetal2.size() != sz) vec.push_back(&lbetal2);
    if (ltl.size() != sz) vec.push_back(&ltl);      // epstl l/v
    if (ltl1.size() != sz) vec.push_back(&ltl1);    // epstl l/v
    if (ltl2.size() != sz) vec.push_back(&ltl2);    // epstl l/v
    if (lrl.size() != sz) vec.push_back(&lrl);      // epstl l/v
    if (lrl1.size() != sz) vec.push_back(&lrl1);    // epstl l/v
    if (lrl2.size() != sz) vec.push_back(&lrl2);    // epstl l/v
    if (lcpl.size() != sz) vec.push_back(&lcpl);    // epstl l/v
    if (lcpl1.size() != sz) vec.push_back(&lcpl1);  // epstl l/v
    if (lcpl2.size() != sz) vec.push_back(&lcpl2);  // epstl l/v

    if (mode == unknown) 
       { ZoneScopedN("resize_eos_fields zone mode unknown");
         if (lt.size() != sz) vec.push_back(&lt);
         if (lh.size() != sz) vec.push_back(&lh);
         if (lr.size() != sz) vec.push_back(&lr);
         if (lcp.size() != sz) vec.push_back(&lcp);
         if (lmu.size() != sz) vec.push_back(&lmu);
         if (lla.size() != sz) vec.push_back(&lla);
         if (lt1.size() != sz) vec.push_back(&lt1);
         if (lh1.size() != sz) vec.push_back(&lh1);
         if (lr1.size() != sz) vec.push_back(&lr1);
         if (lcp1.size() != sz) vec.push_back(&lcp1);
         if (lmu1.size() != sz) vec.push_back(&lmu1);
         if (lla1.size() != sz) vec.push_back(&lla1);
         if (lt2.size() != sz) vec.push_back(&lt2);
         if (lh2.size() != sz) vec.push_back(&lh2);
         if (lr2.size() != sz) vec.push_back(&lr2);
         if (lcp2.size() != sz) vec.push_back(&lcp2);
         if (lmu2.size() != sz) vec.push_back(&lmu2);
         if (lla2.size() != sz) vec.push_back(&lla2);
       }

    if ( (mode == liquid) || (mode == unknown) ) 
       { ZoneScopedN("resize_eos_fields zone mode liquid or unknown");
         if (lhl.size() != sz) vec.push_back(&lhl);
         if (lhl1.size() != sz) vec.push_back(&lhl1);
         if (lhl2.size() != sz) vec.push_back(&lhl2);
         if (ltl.size() != sz) vec.push_back(&ltl);
         if (ltl1.size() != sz) vec.push_back(&ltl1);
         if (ltl2.size() != sz) vec.push_back(&ltl2);
         if (lrl.size() != sz) vec.push_back(&lrl);
         if (lrl1.size() != sz) vec.push_back(&lrl1);
         if (lrl2.size() != sz) vec.push_back(&lrl2);
         if (lrl1pt.size() != sz) vec.push_back(&lrl1pt);
         if (lrl2pt.size() != sz) vec.push_back(&lrl2pt);
         if (lcpl.size() != sz) vec.push_back(&lcpl);
         if (lcpl1.size() != sz) vec.push_back(&lcpl1);
         if (lcpl2.size() != sz) vec.push_back(&lcpl2);
         if (lcpl1pt.size() != sz) vec.push_back(&lcpl1pt);
         if (lcpl2pt.size() != sz) vec.push_back(&lcpl2pt);
         if (ltlal.size() != sz) vec.push_back(&ltlal);
         if (ltlal1.size() != sz) vec.push_back(&ltlal1);
         if (ltlal2.size() != sz) vec.push_back(&ltlal2);
         if (ltmul.size() != sz) vec.push_back(&ltmul);
         if (ltmul1.size() != sz) vec.push_back(&ltmul1);
         if (ltmul2.size() != sz) vec.push_back(&ltmul2);
         if (lhlpt.size() != sz) vec.push_back(&lhlpt);    // M.F.
         if (lhl1pt.size() != sz) vec.push_back(&lhl1pt);  // M.F.
         if (lhl2pt.size() != sz) vec.push_back(&lhl2pt);  // M.F.
         if (lcplpt.size() != sz) vec.push_back(&lcplpt);  // M.F.
         if (lrlpt.size() != sz) vec.push_back(&lrlpt);    // M.F.
       }

    if ( (mode == vapor) || (mode == unknown) ) 
       { ZoneScopedN("resize_eos_fields zone mode vapor or unknown");
         // Le pointeur l2tsdpvv est utilise pour calc2_h_pT_mixing que l'on ait
         // ou non des incondensables (notamment pour H20, pour Na, pas ce pb)
         //En attente de correction dans cathare : on devrait avoir l2tsp1 dans CATHARe2_Water.cxx

      if (lhg.size() != sz) vec.push_back(&lhg);
      if (lprgr.size() != sz) vec.push_back(&lprgr);
      if (ltg.size() != sz) vec.push_back(&ltg);
      if (ltg1.size() != sz) vec.push_back(&ltg1);
      if (ltg3.size() != sz) vec.push_back(&ltg3);
      if (ltg31.size() != sz) vec.push_back(&ltg31);
      if (ltgk.size() != sz) vec.push_back(&ltgk);
      if (ltgini.size() != sz) vec.push_back(&ltgini);
      if (lhvmhs.size() != sz) vec.push_back(&lhvmhs);
      if (lcpg.size() != sz) vec.push_back(&lcpg);
      if (lcpg1.size() != sz) vec.push_back(&lcpg1);
      if (lcpg3.size() != sz) vec.push_back(&lcpg3);
      if (lrg.size() != sz) vec.push_back(&lrg);
      if (lrg1.size() != sz) vec.push_back(&lrg1);
      if (lhg1.size() != sz) vec.push_back(&lhg1);
      if (lrg3.size() != sz) vec.push_back(&lrg3);
      if (lrvpv.size() != sz) vec.push_back(& lrvpv);        // M.F.
      if (lrvhv.size() != sz) vec.push_back(& lrvhv);        // M.F.
      if (lcpvpv.size() != sz) vec.push_back(&lcpvpv);       // M.F.
      if (lcpvhv.size() != sz) vec.push_back(&lcpvhv);       // M.F.
      if (lrv1pt.size() != sz) vec.push_back(&lrv1pt);       // M.F.
      if (lrv3pt.size() != sz) vec.push_back(&lrv3pt);       // M.F.
      if (lcpvpvpt.size() != sz) vec.push_back(& lcpvpvpt);  // M.F.
      if (lcpgtgpt.size() != sz) vec.push_back(& lcpgtgpt);  // M.F.
      if (lhvpt.size() != sz) vec.push_back(& lhvpt);        // M.F.
      if (lrvpt.size() != sz) vec.push_back(& lrvpt);        // M.F.
      if (lcpvpt.size() != sz) vec.push_back(&lcpvpt);       // M.F.
      if (lhv1pt.size() != sz) vec.push_back(&lhv1pt);       // M.F.
      if (ltlag.size() != sz) vec.push_back(&ltlag);
      if (ltlag1.size() != sz) vec.push_back(&ltlag1);
      if (ltlag3.size() != sz) vec.push_back(&ltlag3);
      if (llagpv.size() != sz) vec.push_back(&llagpv);
      if (llagtg.size() != sz) vec.push_back(&llagtg);
      if (ltmug.size() != sz) vec.push_back(&ltmug);
      if (ltmug1.size() != sz) vec.push_back(&ltmug1);
      if (ltmug3.size() != sz) vec.push_back(&ltmug3);
      if (lmugpv.size() != sz) vec.push_back(&lmugpv);
      if (lmugtg.size() != sz) vec.push_back(&lmugtg);
      if (lsi.size() != sz) vec.push_back(&lsi);
      if (lsi1.size() != sz) vec.push_back(&lsi1);
      if (lsi3.size() != sz) vec.push_back(&lsi3);
      if (ldncv.size() != sz) vec.push_back(&ldncv);
      if (ldncv1.size() != sz) vec.push_back(&ldncv1);
      if (ldncv3.size() != sz) vec.push_back(&ldncv3);
      if (lcpgtg.size() != sz) vec.push_back(&lcpgtg);
      if (ltlaga.size() != sz) vec.push_back(&ltlaga);
      if (llagapv.size() != sz) vec.push_back(&llagapv);
      if (llagatg.size() != sz) vec.push_back(&llagatg);
      if (lustlagb.size() != sz) vec.push_back(&lustlagb);
      if (luslagbpv.size() != sz) vec.push_back(&luslagbpv);
      if (luslagbtg.size() != sz) vec.push_back(&luslagbtg);
      if (lcoefqv.size() != sz) vec.push_back(&lcoefqv);
      if (lkiseng.size() != sz) vec.push_back(&lkiseng);
      if (lprandg.size() != sz) vec.push_back(&lprandg);
      if (lhv1.size() != sz) vec.push_back(&lhv1);
      if (lhv3.size() != sz) vec.push_back(&lhv3);
      if (lxvap.size() != sz) vec.push_back(&lxvap);
      if (lrnc.size() != sz) vec.push_back(&lrnc);
      if (lmnc.size() != sz) vec.push_back(&lmnc);
      if (lxnc.size() != sz) vec.push_back(&lxnc);
      if (nincon == 0) 
      {
         if (lhvsvsc3.size() != sz) vec.push_back(&lhvsvsc3);
      }
      else
         { ZoneScopedN("nincond > 0");
           if (ltspv.size() != sz) vec.push_back(&ltspv);
           if (ltspvv.size() != sz) vec.push_back(&ltspvv);
           if (l2tsdpvv.size() != sz) vec.push_back(&l2tsdpvv);
           if (lhlsv.size() != sz) vec.push_back(&lhlsv);
           if (lhlsvv.size() != sz) vec.push_back(&lhlsvv);
           if (lhvsv.size() != sz) vec.push_back(&lhvsv);
           if (lhvsvv.size() != sz) vec.push_back(&lhvsvv);
           if (lcplsv.size() != sz) vec.push_back(&lcplsv);
           if (lclsvv.size() != sz) vec.push_back(&lclsvv);
           if (lcpvsv.size() != sz) vec.push_back(&lcpvsv);
           if (lcvsvv.size() != sz) vec.push_back(&lcvsvv);
           if (lrlsv.size() != sz) vec.push_back(&lrlsv);
           if (lrlsvv.size() != sz) vec.push_back(&lrlsvv);
           if (lrvsv.size() != sz) vec.push_back(&lrvsv);
           if (lrvsvv.size() != sz) vec.push_back(&lrvsvv);
           if (lpv.size() != sz) vec.push_back(&lpv);
           if (lhv.size() != sz) vec.push_back(&lhv);
           if (ltgpv.size() != sz) vec.push_back(&ltgpv);
           if (ltghv.size() != sz) vec.push_back(&ltghv);
           if (lcpv.size() != sz) vec.push_back(&lcpv);
           // if (lcpvpv.size() != sz) vec.push_back(&lcpvpv);
           // if (lcpvhv.size() != sz) vec.push_back(&lcpvhv);
           if (lrv.size() != sz) vec.push_back(&lrv);
           // if (lrvpv.size() != sz) vec.push_back(&lrvpv);
           // if (lrvhv.size() != sz) vec.push_back(&lrvhv);
           if (ltlav.size() != sz) vec.push_back(&ltlav);
           if (llavpv.size() != sz) vec.push_back(&llavpv);
           if (llavtg.size() != sz) vec.push_back(&llavtg);
           if (ltmuv.size() != sz) vec.push_back(&ltmuv);
           if (lmuvpv.size() != sz) vec.push_back(&lmuvpv);
           if (lmuvtg.size() != sz) vec.push_back(&lmuvtg);
           if (lsipv.size() != sz) vec.push_back(&lsipv);
           if (lhfv.size() != sz) vec.push_back(&lhfv);
           if (lhfvv.size() != sz) vec.push_back(&lhfvv);
           if (lhlsvsc.size() != sz) vec.push_back(&lhlsvsc);
           if (lhlsvsc1.size() != sz) vec.push_back(&lhlsvsc1);
           if (lhlsvsc2.size() != sz) vec.push_back(&lhlsvsc2);
           if (lhlsvsc3.size() != sz) vec.push_back(&lhlsvsc3);
           if (lhvsvsc.size() != sz) vec.push_back(&lhvsvsc);
           if (lhvsvsc1.size() != sz) vec.push_back(&lhvsvsc1);
           if (lhvsvsc2.size() != sz) vec.push_back(&lhvsvsc2);
           if (lhvsvsc3.size() != sz) vec.push_back(&lhvsvsc3);
           if (lxrsu.size() != sz) vec.push_back(&lxrsu);
           if (lxcpsu.size() != sz) vec.push_back(&lxcpsu);
           if (lxcpsutg.size() != sz) vec.push_back(&lxcpsutg);
           if (lhi7su.size() != sz) vec.push_back(&lhi7su);
           if (lvalp.size() != sz) vec.push_back(&lvalp);
           if (lpv1.size() != sz) vec.push_back(&lpv1);
           if (lpv3.size() != sz) vec.push_back(&lpv3);
           if (lrv1.size() != sz) vec.push_back(&lrv1);
           if (lrv3.size() != sz) vec.push_back(&lrv3);
           if (lhv1.size() != sz) vec.push_back(&lhv1);
           if (lhv3.size() != sz) vec.push_back(&lhv3);
           if (lfdpv.size() != sz) vec.push_back(&lfdpv);
           if (lfdhv.size() != sz) vec.push_back(&lfdhv);
           if (lgdpv.size() != sz) vec.push_back(&lgdpv);
           if (lgdhv.size() != sz) vec.push_back(&lgdhv);
           if (lphivmusu.size() != sz) vec.push_back(&lphivmusu);
           if (lphivmusupv.size() != sz) vec.push_back(&lphivmusupv);
           if (lphivmusutg.size() != sz) vec.push_back(&lphivmusutg);
           {ZoneScopedN("incondensable gas for loop");
           for (int i=0; i<nincon; i++) 
              { if (lx[i].size() != sz) vec.push_back(&lx[i]);
                if (lpx[i].size() != sz) vec.push_back(&lpx[i]);
                if (lpvx[i].size() != sz) vec.push_back(&lpvx[i]);
                if (lhvx[i].size() != sz) vec.push_back(&lhvx[i]);
                if (lhgx[i].size() != sz) vec.push_back(&lhgx[i]);
                if (ltgx[i].size() != sz) vec.push_back(&ltgx[i]);
                if (lrvx[i].size() != sz) vec.push_back(&lrvx[i]);
                if (lrgx[i].size() != sz) vec.push_back(&lrgx[i]);
                if (lhx[i].size() != sz) vec.push_back(&lhx[i]);
                if (lrncx[i].size() != sz) vec.push_back(&lrncx[i]);
                if (lmncx[i].size() != sz) vec.push_back(&lmncx[i]);
                if (lprxcp[i].size() != sz) vec.push_back(&lprxcp[i]);
                if (lprxcptg[i].size() != sz) vec.push_back(&lprxcptg[i]);
                if (ltlax[i].size() != sz) vec.push_back(&ltlax[i]);
                if (llaxtg[i].size() != sz) vec.push_back(&llaxtg[i]);
                if (ltmux[i].size() != sz) vec.push_back(&ltmux[i]);
                if (lmuxtg[i].size() != sz) vec.push_back(&lmuxtg[i]);
                if (lphixmusu[i].size() != sz) vec.push_back(&lphixmusu[i]);
                if (lphixmusupv[i].size() != sz) vec.push_back(&lphixmusupv[i]);
                if (lphixmusutg[i].size() != sz) vec.push_back(&lphixmusutg[i]);
                if (ldncvj[i].size() != sz) vec.push_back(&ldncvj[i]);
                if (lcoefqvx[i].size() != sz) vec.push_back(&lcoefqvx[i]);
                if (lphivmusux[i].size() != sz) vec.push_back(&lphivmusux[i]);
                for (int j=0; j<nincon; j++)
                  if (lphixmusux[i][j].size() != sz) vec.push_back(&lphixmusux[i][j]);
                if (lcpgx[i].size() != sz) vec.push_back(&lcpgx[i]);
                if (ltlagx[i].size() != sz) vec.push_back(&ltlagx[i]);
                if (ltmugx[i].size() != sz) vec.push_back(&ltmugx[i]);
                if (lsix[i].size() != sz) vec.push_back(&lsix[i]);
                if (ldncvx[i].size() != sz) vec.push_back(&ldncvx[i]);
                if (lhlspscx[i].size() != sz) vec.push_back(&lhlspscx[i]);
                if (lhlsvscx[i].size() != sz) vec.push_back(&lhlsvscx[i]);
                if (lhvspscx[i].size() != sz) vec.push_back(&lhvspscx[i]);
                if (lhvsvscx[i].size() != sz) vec.push_back(&lhvsvscx[i]);
                if (lepstgasx[i].size() != sz) vec.push_back(&lepstgasx[i]);
              }
            }
         }
      }

      ArrOfDouble::resize_all(vec, sz);
      vec.clear(); 
      std::vector<int> vecSize; 
      std::vector<const double*> vecPtr; 

      if ((mode == saturated) || (mode == liquid) || (mode == unknown) || (mode == vapor))  
      { ZoneScopedN("resize_eos_fields zone mode saturated");
        // Le pointeur l2tsdpvv est utilise pour d2_T_sat_d_p_d_p que l'on ait
        // ou non des incondensables (notamment pour H20, pour Na, pas ce pb)
        //En attente de correction dans cathare : on devrait avoir l2tsp1 dans CATHARe2_Water.cxx
        vec.push_back(&l2tsdpvv);
        vecSize.push_back(sz);
        vecPtr.push_back(l2tsp1.get_ptr());
      }

      if ( (mode == vapor) || (mode == unknown) ) 
      {
         if(nincon == 0){
           vec.push_back(&ltspv); vecSize.push_back(sz); vecPtr.push_back(ltsp.get_ptr()); 
           vec.push_back(&ltspvv); vecSize.push_back(sz); vecPtr.push_back(ltsp1.get_ptr());
           vec.push_back(&lhlsv); vecSize.push_back(sz); vecPtr.push_back(lhlsp.get_ptr());
           vec.push_back(&lhlsvv); vecSize.push_back(sz); vecPtr.push_back(lhlsp1.get_ptr());
           vec.push_back(&lcplsv); vecSize.push_back(sz); vecPtr.push_back(lcplsp.get_ptr());
           vec.push_back(&lclsvv); vecSize.push_back(sz); vecPtr.push_back(lclsp1.get_ptr());
           vec.push_back(&lrlsv); vecSize.push_back(sz); vecPtr.push_back(lrlsp.get_ptr());
           vec.push_back(&lrlsvv); vecSize.push_back(sz); vecPtr.push_back(lrlsp1.get_ptr());
           vec.push_back(&lhlsvsc); vecSize.push_back(sz); vecPtr.push_back(lhlspsc.get_ptr());
           vec.push_back(&lhlsvsc1); vecSize.push_back(sz); vecPtr.push_back(lhlspsc1.get_ptr());
           vec.push_back(&lhlsvsc2); vecSize.push_back(sz); vecPtr.push_back(lhlspsc2.get_ptr());
           vec.push_back(&lhlsvsc3); vecSize.push_back(sz); vecPtr.push_back(lhlspsc3.get_ptr());
           vec.push_back(&lhvsv); vecSize.push_back(sz); vecPtr.push_back(lhvsp.get_ptr());
           vec.push_back(&lhvsvv); vecSize.push_back(sz); vecPtr.push_back(lhvsp1.get_ptr());
           vec.push_back(&lcpvsv); vecSize.push_back(sz); vecPtr.push_back(lcpvsp.get_ptr());
           vec.push_back(&lcvsvv); vecSize.push_back(sz); vecPtr.push_back(lcvsp1.get_ptr());
           vec.push_back(&lrvsv); vecSize.push_back(sz); vecPtr.push_back(lrvsp.get_ptr());
           vec.push_back(&lrvsvv); vecSize.push_back(sz); vecPtr.push_back(lrvsp1.get_ptr());
           vec.push_back(&lhvsvsc); vecSize.push_back(sz); vecPtr.push_back(lhvspsc.get_ptr());
           vec.push_back(&lhvsvsc1); vecSize.push_back(sz); vecPtr.push_back(lhvspsc1.get_ptr());
           vec.push_back(&lhvsvsc2); vecSize.push_back(sz); vecPtr.push_back(lhvspsc2.get_ptr());
           vec.push_back(&lsipv); vecSize.push_back(sz); vecPtr.push_back(lsi1.get_ptr());
           vec.push_back(&lpv); vecSize.push_back(sz); vecPtr.push_back(lp.get_ptr());
           vec.push_back(&lhv); vecSize.push_back(sz); vecPtr.push_back(lhg.get_ptr());
           vec.push_back(&lrv); vecSize.push_back(sz); vecPtr.push_back(lrg.get_ptr());
           vec.push_back(&lrv1); vecSize.push_back(sz); vecPtr.push_back(lrg1.get_ptr());
           vec.push_back(&lrv3); vecSize.push_back(sz); vecPtr.push_back(lrg3.get_ptr());
           vec.push_back(&ltgpv); vecSize.push_back(sz); vecPtr.push_back(ltg1.get_ptr());
           vec.push_back(&ltghv); vecSize.push_back(sz); vecPtr.push_back(ltg3.get_ptr());
           //vec.push_back(&lrvpv); vecSize.push_back(sz); vecPtr.push_back(lrg1.get_ptr());
           //vec.push_back(&lrvhv); vecSize.push_back(sz); vecPtr.push_back(lrg3.get_ptr());
           vec.push_back(&lcpv); vecSize.push_back(sz); vecPtr.push_back(lcpg.get_ptr());
           //vec.push_back(&lcpvpv); vecSize.push_back(sz); vecPtr.push_back(lcpg1.get_ptr());
           //vec.push_back(&lcpvhv); vecSize.push_back(sz); vecPtr.push_back(lcpg3.get_ptr());
           vec.push_back(&lhfv); vecSize.push_back(sz); vecPtr.push_back(lhf.get_ptr());
           vec.push_back(&lhfvv); vecSize.push_back(sz); vecPtr.push_back(lhf1.get_ptr());
           vec.push_back(&lhvpv); vecSize.push_back(sz); vecPtr.push_back(lhg1.get_ptr());
           vec.push_back(&ltlav); vecSize.push_back(sz); vecPtr.push_back(ltlag.get_ptr());
           vec.push_back(&ltmuv); vecSize.push_back(sz); vecPtr.push_back(ltmug.get_ptr());
           vec.push_back(&llavpv); vecSize.push_back(sz); vecPtr.push_back(llagpv.get_ptr());
           vec.push_back(&llavtg); vecSize.push_back(sz); vecPtr.push_back(llagtg.get_ptr());
           vec.push_back(&lmuvpv); vecSize.push_back(sz); vecPtr.push_back(lmugpv.get_ptr());
           vec.push_back(&lmuvtg); vecSize.push_back(sz); vecPtr.push_back(lmugtg.get_ptr());
         }
      }
      ArrOfDouble::set_ptr_all(vec, vecPtr, vecSize);
  }

  void CATHARE2::preconvert_eos_fields() 
  { ZoneScopedN("CATHARE2::preconvert_eos_fields");
    if (lfluid != 100003) 
       { if (ltl.size() == nsca) ltl -= tabsk;
         if (ltg.size() == nsca) ltg -= tabsk;
         if (ltsp.size() == nsca) ltsp -= tabsk;
       }
  }

  void CATHARE2::postconvert_eos_fields() 
  { ZoneScopedN("CATHARE2::postconvert_eos_fields");
    if (lfluid != 100003) 
       { if (ltl.size()  == nsca) ltl  += tabsk ;
         if (ltg.size()  == nsca) ltg  += tabsk ;
         if (ltsp.size() == nsca) ltsp += tabsk ;
       }
  }

  EOS_Error CATHARE2::calc2_p(const EOS_Field &p, EOS_Fields &out, EOS_Error_Field &ferr)
  { ZoneScopedN("CATHARE2::calc2_p");
    typ_ths = TH_space::Psat ;
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
  { ZoneScopedN("CATHARE2::calc2_t");
    typ_ths = TH_space::Tsat ;
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
  { 
    ZoneScopedN("CATHARE2::calc2_ph");
    typ_ths = TH_space::Ph ;
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
  { ZoneScopedN("CATHARE2::calc2_pt");
    typ_ths = TH_space::PT ;
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
    ZoneScopedN("CATHARE2::calc2_h_pT_mixing");
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
  { ZoneScopedN("CATHARE2::calc2_lim");
    ArrOfDouble diff(nsca) ;
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
  { ZoneScopedN("CATHARE2::calc2_critical");
    if (pc > 0) 
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
  { 
    if (fldm > 0)
      { mm = fldm*1e-3 ;
        return EOS_Internal_Error::OK ;
      }
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error CATHARE2::get_p_crit(double& p_crit )
  { 
    if (critical == EOS_Error::bad)  calc2_critical() ;
    if (pc > 0.e0)
       { p_crit = pc ;
         return EOS_Internal_Error::OK ;
       }
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error CATHARE2::get_h_crit(double& h_crit )
  { 
    if (critical == EOS_Error::bad)  calc2_critical() ;
    if (pc > 0.e0)
       { h_crit = hc ;
         return EOS_Internal_Error::OK ;
       }
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error CATHARE2::get_T_crit(double& T_crit )
  {
    if (critical == EOS_Error::bad)  calc2_critical() ;
    if (pc > 0.e0)
      { T_crit = tc ;
        return EOS_Internal_Error::OK ;
      }
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error CATHARE2::get_p_min(double& p_min )
  { 
    p_min = xpm ;
    return EOS_Internal_Error::OK ;
  }

  EOS_Internal_Error CATHARE2::get_p_max(double& p_max )
  {
    p_max = xpp ;
    return EOS_Internal_Error::OK ;
  }

  EOS_Internal_Error CATHARE2::get_h_min(double& h_min )
  { 
    if (phase == liquid or phase == saturated)  h_min = xhlm ;
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
  { ZoneScopedN("CATHARE2::compute_h_l_lim_p");
    ArrOfInt err_arr(1) ;
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
  { ZoneScopedN("CATHARE2::compute_h_v_lim_p");
    ArrOfInt err_arr(1) ;
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
  { ZoneScopedN("CATHARE2::describe_error");
    switch(error.get_partial_code()) 
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
  { //ZoneScopedN("CATHARE2::convert_eos_error");
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
