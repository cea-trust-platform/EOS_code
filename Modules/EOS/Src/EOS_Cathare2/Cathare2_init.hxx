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



      lp(tab_memsp[NEPTUNE::p].pt)                            ,
      lkiseng(tab_memsp[NEPTUNE::gamma].pt)                   ,

      lhl(tab_memsp[NEPTUNE::h].pt)                           , // 2D, liquid
      lhlpt(tab_memsp[::NCATHARE2::h_pt].pt)                  ,
      // lhlpt(tab_memsp[NEPTUNE::h].pt)                      ,
      ltl(tab_memsp[NEPTUNE::T].pt)                           ,
      ltlpt(tab_memsp[::NCATHARE2::T_pt].pt)                  ,
      lrl(tab_memsp[NEPTUNE::rho].pt)                         ,
      lrlpt(tab_memsp[::NCATHARE2::rho_pt].pt)                ,
      lcpl(tab_memsp[NEPTUNE::cp].pt)                         ,
      lcplpt(tab_memsp[::NCATHARE2::cp_pt].pt)                ,
      lbetal(tab_memsp[NEPTUNE::beta].pt)                     ,
      llal(tab_memsp[NEPTUNE::lambda].pt)                     ,
      lmul(tab_memsp[NEPTUNE::mu].pt)                         ,

      ltl1(tab_memsp[NEPTUNE::d_T_d_p_h].pt)                  ,
      ltl2(tab_memsp[NEPTUNE::d_T_d_h_p].pt)                  ,
      lhl1(tab_memsp[NEPTUNE::d_h_d_p_T].pt)                  ,
      lhl1pt(tab_memsp[::NCATHARE2::dhdp_t_pt].pt)            ,
      lhl2pt(tab_memsp[NEPTUNE::d_h_d_T_p].pt)                ,
      lrl1(tab_memsp[NEPTUNE::d_rho_d_p_h].pt)                ,
      lrl2(tab_memsp[NEPTUNE::d_rho_d_h_p].pt)                ,
      lrl1pt(tab_memsp[NEPTUNE::d_rho_d_p_T].pt)              ,
      lrl2pt(tab_memsp[NEPTUNE::d_rho_d_T_p].pt)              ,
      lcpl1(tab_memsp[NEPTUNE::d_cp_d_p_h].pt)                ,
      lcpl2(tab_memsp[NEPTUNE::d_cp_d_h_p].pt)                ,
      lcpl1pt(tab_memsp[NEPTUNE::d_cp_d_p_T].pt)              ,
      lcpl2pt(tab_memsp[NEPTUNE::d_cp_d_T_p].pt)              ,

      lbetal1(tab_memsp[NEPTUNE::d_beta_d_p_h].pt)            ,
      lbetal2(tab_memsp[NEPTUNE::d_beta_d_h_p].pt)            ,
      llal1(tab_memsp[NEPTUNE::d_lambda_d_p_h].pt)            ,
      llal2(tab_memsp[NEPTUNE::d_lambda_d_h_p].pt)            ,
      lmul1(tab_memsp[NEPTUNE::d_mu_d_p_h].pt)                ,
      lmul2(tab_memsp[NEPTUNE::d_mu_d_h_p].pt)                ,

      lhg(tab_memsp[NEPTUNE::h].pt)                           , // 2D, vapor
      lhvpt(tab_memsp[::NCATHARE2::h_pt].pt)                  ,
      // lhvpt(tab_memsp[NEPTUNE::h].pt)                      ,
      ltg(tab_memsp[NEPTUNE::T].pt)                           ,
      ltgpt(tab_memsp[::NCATHARE2::T_pt].pt)                  ,
      lrg(tab_memsp[NEPTUNE::rho].pt)                         ,
      lrvpt(tab_memsp[::NCATHARE2::rho_pt].pt)                ,
      lcpg(tab_memsp[NEPTUNE::cp].pt)                         ,
      lcpvpt(tab_memsp[::NCATHARE2::cp_pt].pt)                ,
      llag(tab_memsp[NEPTUNE::lambda].pt)                     ,
      lmug(tab_memsp[NEPTUNE::mu].pt)                         ,
      lsi(tab_memsp[NEPTUNE::sigma].pt)                       ,
      lprandg(tab_memsp[NEPTUNE::pr].pt)                      ,

      ltg1(tab_memsp[NEPTUNE::d_T_d_p_h].pt)                  ,
      ltg3(tab_memsp[NEPTUNE::d_T_d_h_p].pt)                  ,
      lhg1(tab_memsp[NEPTUNE::d_h_d_p_T].pt)                  ,
      lhv1pt(tab_memsp[::NCATHARE2::dhdp_t_pt].pt)            ,
      lrvpv(tab_memsp[NEPTUNE::d_rho_d_p_h].pt)               ,
      lrvhv(tab_memsp[NEPTUNE::d_rho_d_h_p].pt)               ,
      lrv1pt(tab_memsp[NEPTUNE::d_rho_d_p_T].pt)              ,
      lrv3pt(tab_memsp[NEPTUNE::d_rho_d_T_p].pt)              ,
      lcpvpv(tab_memsp[NEPTUNE::d_cp_d_p_h].pt)               ,
      lcpvhv(tab_memsp[NEPTUNE::d_cp_d_h_p].pt)               ,
      lcpvpvpt(tab_memsp[NEPTUNE::d_cp_d_p_T].pt)             ,
      lcpgtgpt(tab_memsp[NEPTUNE::d_cp_d_T_p].pt)             ,
      llag1(tab_memsp[NEPTUNE::d_lambda_d_p_h].pt)            ,
      llag3(tab_memsp[NEPTUNE::d_lambda_d_h_p].pt)            ,
      llagtg(tab_memsp[NEPTUNE::d_lambda_d_T_p].pt)           ,
      lmug1(tab_memsp[NEPTUNE::d_mu_d_p_h].pt)                ,
      lmug3(tab_memsp[NEPTUNE::d_mu_d_h_p].pt)                ,
      lmugtg(tab_memsp[NEPTUNE::d_mu_d_T_p].pt)               ,
      lsi1(tab_memsp[NEPTUNE::d_sigma_d_p_h].pt)              ,
      lsi3(tab_memsp[NEPTUNE::d_sigma_d_h_p].pt)              ,

      lh(tab_memsp[NEPTUNE::h].pt)                            , // 2D, phase unkown
      lt(tab_memsp[NEPTUNE::T].pt)                            ,
      lr(tab_memsp[NEPTUNE::rho].pt)                          ,
      lcp(tab_memsp[NEPTUNE::cp].pt)                          ,
      lla(tab_memsp[NEPTUNE::lambda].pt)                      ,
      lmu(tab_memsp[NEPTUNE::mu].pt)                          ,

      lh2pt(tab_memsp[NEPTUNE::d_h_d_T_p].pt)                 ,
      lt1(tab_memsp[NEPTUNE::d_T_d_p_h].pt)                   ,
      lt2(tab_memsp[NEPTUNE::d_T_d_h_p].pt)                   ,
      lr1(tab_memsp[NEPTUNE::d_rho_d_p_h].pt)                 ,
      lr2(tab_memsp[NEPTUNE::d_rho_d_h_p].pt)                 ,
      lcp1(tab_memsp[NEPTUNE::d_cp_d_p_h].pt)                 ,
      lcp2(tab_memsp[NEPTUNE::d_cp_d_h_p].pt)                 ,
      lcpgtg(tab_memsp[NEPTUNE::d_cp_d_T_p].pt)               ,
      lla1(tab_memsp[NEPTUNE::d_lambda_d_p_h].pt)             ,
      lla2(tab_memsp[NEPTUNE::d_lambda_d_h_p].pt)             ,
      lmu1(tab_memsp[NEPTUNE::d_mu_d_p_h].pt)                 ,
      lmu2(tab_memsp[NEPTUNE::d_mu_d_h_p].pt)                 ,
   
      lp_sat(tab_memsp[NEPTUNE::p_sat].pt)                    , // sat.
      ltsp(tab_memsp[NEPTUNE::T_sat].pt)                      ,
      lhlsp(tab_memsp[NEPTUNE::h_l_sat].pt)                   ,
      lhvsp(tab_memsp[NEPTUNE::h_v_sat].pt)                   ,
      lcplsp(tab_memsp[NEPTUNE::cp_l_sat].pt)                 ,
      lcpvsp(tab_memsp[NEPTUNE::cp_v_sat].pt)                 ,

      lrlsp(tab_memsp[NEPTUNE::rho_l_sat].pt)                 ,
      lrvsp(tab_memsp[NEPTUNE::rho_v_sat].pt)                 ,

      ltsp1(tab_memsp[NEPTUNE::d_T_sat_d_p].pt)               ,
      lhlsp1(tab_memsp[NEPTUNE::d_h_l_sat_d_p].pt)            ,
      lhvsp1(tab_memsp[NEPTUNE::d_h_v_sat_d_p].pt)            ,
      lclsp1(tab_memsp[NEPTUNE::d_cp_l_sat_d_p].pt)           ,
      lcvsp1(tab_memsp[NEPTUNE::d_cp_v_sat_d_p].pt)           ,
      lrlsp1(tab_memsp[NEPTUNE::d_rho_l_sat_d_p].pt)          ,
      lrvsp1(tab_memsp[NEPTUNE::d_rho_v_sat_d_p].pt)          ,

      l2tsp1(tab_memsp[NEPTUNE::d2_T_sat_d_p_d_p].pt)         ,

      lp_lim(tab_memsp[NEPTUNE::p_lim].pt)                    , // Spinodale limites
      hllim(tab_memsp[NEPTUNE::h_l_lim].pt)                   ,
      hvlim(tab_memsp[NEPTUNE::h_v_lim].pt)                   ,

      lxvap(tab_memsp[NEPTUNE::c_0].pt)                       , // mixing c2, 2D, vapor
      lpv(tab_memsp[NEPTUNE::p_0].pt)                         ,
      lhv(tab_memsp[NEPTUNE::h_0].pt)                         ,
      lrv(tab_memsp[NEPTUNE::rho_0].pt)                       ,
      lcpv(tab_memsp[NEPTUNE::cp_0].pt)                       ,
      llav(tab_memsp[NEPTUNE::lambda_0].pt)                   ,
      lmuv(tab_memsp[NEPTUNE::mu_0].pt)                       ,

      ltspv(tab_memsp[NEPTUNE::T_sat_0].pt)                   , // mixing c2, saturation
      lhlsv(tab_memsp[NEPTUNE::h_l_sat_0].pt)                 ,
      lhvsv(tab_memsp[NEPTUNE::h_v_sat_0].pt)                 ,
      lrlsv(tab_memsp[NEPTUNE::rho_l_sat_0].pt)               ,
      lrvsv(tab_memsp[NEPTUNE::rho_v_sat_0].pt)               ,
      lcplsv(tab_memsp[NEPTUNE::cp_l_sat_0].pt)               ,
      lcpvsv(tab_memsp[NEPTUNE::cp_v_sat_0].pt)               ,

      ldncv(tab_memsp[NEPTUNE::dncv].pt)                      ,
      lrnc(tab_memsp[NEPTUNE::rnc].pt)                        ,
      lmnc(tab_memsp[NEPTUNE::mnc].pt)                        ,
      lprgr(tab_memsp[NEPTUNE::prgr].pt)                      ,
      lxnc(tab_memsp[NEPTUNE::xnc].pt)                        ,

      lpv1(tab_memsp[NEPTUNE::d_p_0_d_p_h].pt)                , // mixing c2, derivation, 2D, vapor 
      lpv3(tab_memsp[NEPTUNE::d_p_0_d_h_p].pt)                ,
      lhv1(tab_memsp[NEPTUNE::d_h_0_d_p_h].pt)                ,
      lhv3(tab_memsp[NEPTUNE::d_h_0_d_h_p].pt)                ,
      ltgpv(tab_memsp[NEPTUNE::d_T_d_p_0_h].pt)               ,
      ltghv(tab_memsp[NEPTUNE::d_T_d_h_0_p].pt)               ,
      lrv1(tab_memsp[NEPTUNE::d_rho_0_d_p_h].pt)              ,
      lrv3(tab_memsp[NEPTUNE::d_rho_0_d_h_p].pt)              ,
      lrvpv0(tab_memsp[NEPTUNE::d_rho_0_d_p_0_h].pt)          ,
      lrvhv0(tab_memsp[NEPTUNE::d_rho_0_d_h_0_p].pt)          ,
      lcpvpv0(tab_memsp[NEPTUNE::d_cp_0_d_p_0_h].pt)          ,
      lcpvhv0(tab_memsp[NEPTUNE::d_cp_0_d_h_0_p].pt)          ,
      llavpv(tab_memsp[NEPTUNE::d_lambda_0_d_p_0_h].pt)       ,
      llavtg(tab_memsp[NEPTUNE::d_lambda_0_d_T_p].pt)         ,
      lmuvpv(tab_memsp[NEPTUNE::d_mu_0_d_p_0_h].pt)           ,
      lmuvtg(tab_memsp[NEPTUNE::d_mu_0_d_T_p].pt)             ,
      lsipv(tab_memsp[NEPTUNE::d_sigma_d_p_0_h].pt)           ,

      lrg1(tab_memsp[NEPTUNE::d_rho_d_p_h].pt)                ,
      lrg3(tab_memsp[NEPTUNE::d_rho_d_h_p].pt)                ,
      lcpg1(tab_memsp[NEPTUNE::d_cp_d_p_h].pt)                ,
      lcpg3(tab_memsp[NEPTUNE::d_cp_d_h_p].pt)                ,

      ltspvv(tab_memsp[NEPTUNE::d_T_sat_0_d_p_0_h].pt)        , // mixing c2, derivation saturation
      lhlsvv(tab_memsp[NEPTUNE::d_h_l_sat_0_d_p_0_h].pt)      ,
      lhvsvv(tab_memsp[NEPTUNE::d_h_v_sat_0_d_p_0_h].pt)      ,
      lrlsvv(tab_memsp[NEPTUNE::d_rho_l_sat_0_d_p_0_h].pt)    ,
      lrvsvv(tab_memsp[NEPTUNE::d_rho_v_sat_0_d_p_0_h].pt)    ,
      lclsvv(tab_memsp[NEPTUNE::d_cp_l_sat_0_d_p_0_h].pt)     ,
      lcvsvv(tab_memsp[NEPTUNE::d_cp_v_sat_0_d_p_0_h].pt)     ,
      l2tsdpvv(tab_memsp[NEPTUNE::d2_T_sat_0_d_p_0_d_p_0].pt) ,

      ldncv1(tab_memsp[NEPTUNE::d_dncv_d_p_h].pt)             ,
      ldncv3(tab_memsp[NEPTUNE::d_dncv_d_h_p].pt)             ,

      lepstliq(tab_memsp[NEPTUNE::epstl].pt)                  , // F.E. IAPWS, liquid
      lhlspsc(tab_memsp[NEPTUNE::hlspsc].pt)                  ,
      lhlsvsc(tab_memsp[NEPTUNE::hlsvsc].pt)                  ,
      lepstgas(tab_memsp[NEPTUNE::epstg].pt)                  , // F.E. IAPWS, vapor
      lhvspsc(tab_memsp[NEPTUNE::hvspsc].pt)                  ,
      lhvsvsc(tab_memsp[NEPTUNE::hvsvsc].pt)                  ,

      lepstliq1(tab_memsp[NEPTUNE::d_epstl_dp_h].pt)          , // F.E. IAPWS, liquid, derivation
      lepstliq2(tab_memsp[NEPTUNE::d_epstl_dh_p].pt)          ,
      lhlspsc1(tab_memsp[NEPTUNE::d_hlspsc_dp_h].pt)          ,
      lhlspsc2(tab_memsp[NEPTUNE::d_hlspsc_dh_p].pt)          ,
      lhlsvsc1(tab_memsp[NEPTUNE::d_hlsvsc_dp_h].pt)          ,
      lhlsvsc2(tab_memsp[NEPTUNE::d_hlsvsc_dh_p0].pt)         ,
      lhlsvsc3(tab_memsp[NEPTUNE::d_hlsvsc_dh_p].pt)          ,

      lepstgas1(tab_memsp[NEPTUNE::d_epstg_dp_h].pt)          , // F.E. IAPWS, vapor,  derivation
      lepstgas3(tab_memsp[NEPTUNE::d_epstg_dh_p].pt)          ,
      lhvspsc1(tab_memsp[NEPTUNE::d_hvspsc_dp_h].pt)          ,
      lhvspsc3(tab_memsp[NEPTUNE::d_hvspsc_dh_p].pt)          ,
      lhvsvsc1(tab_memsp[NEPTUNE::d_hvsvsc_dp_h].pt)          ,
      lhvsvsc3(tab_memsp[NEPTUNE::d_hvsvsc_dh_p].pt)

