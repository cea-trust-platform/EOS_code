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
#include "EOS_properties.hxx"

namespace NEPTUNE
{
    std::map<EOS_Property, AString> property_name_map;

    // Initialisation automatique
    static struct EOS_PropInit
    {
        EOS_PropInit()
        {
            // THERM
            register_property(p, "p");
            register_property(h, "h");
            register_property(T, "t");
            register_property(rho, "rho");
            register_property(u, "u");
            register_property(s, "s");
            register_property(mu, "mu");
            register_property(lambda, "lambda");
            register_property(cp, "cp");
            register_property(cv, "cv");
            register_property(sigma, "sigma");
            register_property(w, "w");
            register_property(g, "g");
            register_property(f, "f");
            register_property(pr, "pr");
            register_property(beta, "beta");
            register_property(gamma, "gamma");

            // DERIVEES THERM
            register_property(d_T_d_p_h, "dtdph");
            register_property(d_T_d_h_p, "dtdhp");
            register_property(d_rho_d_p_h, "drhodph");
            register_property(d_rho_d_h_p, "drhodhp");
            register_property(d_rho_d_T_p, "drhodtp");
            register_property(d_rho_d_p_T, "drhodpt");
            register_property(d_u_d_p_h, "dudph");
            register_property(d_u_d_h_p, "dudhp");
            register_property(d_s_d_p_h, "dsdph");
            register_property(d_s_d_h_p, "dsdhp");
            register_property(d_mu_d_p_h, "dmudph");
            register_property(d_mu_d_h_p, "dmudhp");
            register_property(d_lambda_d_p_h, "dlambdadph");
            register_property(d_lambda_d_h_p, "dlambdadhp");
            register_property(d_cp_d_p_h, "dcpdph");
            register_property(d_cp_d_h_p, "dcpdhp");
            register_property(d_cv_d_p_h, "dcvdph");
            register_property(d_cv_d_h_p, "dcvdhp");
            register_property(d_sigma_d_p_h, "dsigmadph");
            register_property(d_sigma_d_h_p, "dsigmadhp");
            register_property(d_w_d_p_h, "dwdph");
            register_property(d_w_d_h_p, "dwdhp");
            register_property(d_g_d_p_h, "dgdph");
            register_property(d_g_d_h_p, "dgdhp");
            register_property(d_f_d_p_h, "dfdph");
            register_property(d_f_d_h_p, "dfdhp");
            register_property(d_pr_d_p_h, "dprdph");
            register_property(d_pr_d_h_p, "dprdhp");
            register_property(d_beta_d_p_h, "dbetadph");
            register_property(d_beta_d_h_p, "dbetadhp");
            register_property(d_gamma_d_p_h, "dgammadph");
            register_property(d_gamma_d_h_p, "dgammadhp");

            // CROSS DERIVEES (d2X/dPdh, used by the bicubic Hermite patch)
            register_property(d2_T_d_p_d_h, "d2tdpdh");
            register_property(d2_rho_d_p_d_h, "d2rhodpdh");
            register_property(d2_u_d_p_d_h, "d2udpdh");
            register_property(d2_s_d_p_d_h, "d2sdpdh");
            register_property(d2_mu_d_p_d_h, "d2mudpdh");
            register_property(d2_lambda_d_p_d_h, "d2lambdadpdh");
            register_property(d2_cp_d_p_d_h, "d2cpdpdh");
            register_property(d2_cv_d_p_d_h, "d2cvdpdh");
            register_property(d2_sigma_d_p_d_h, "d2sigmadpdh");
            register_property(d2_w_d_p_d_h, "d2wdpdh");
            register_property(d2_g_d_p_d_h, "d2gdpdh");
            register_property(d2_f_d_p_d_h, "d2fdpdh");
            register_property(d2_pr_d_p_d_h, "d2prdpdh");
            register_property(d2_beta_d_p_d_h, "d2betadpdh");
            register_property(d2_gamma_d_p_d_h, "d2gammadpdh");

            // … et toutes les autres dérivées

            // SATURATION
            register_property(p_sat, "psat");
            register_property(T_sat, "tsat");
            register_property(rho_l_sat, "rholsat");
            register_property(rho_v_sat, "rhovsat");
            register_property(h_l_sat, "hlsat");
            register_property(h_v_sat, "hvsat");
            register_property(cp_l_sat, "cplsat");
            register_property(cp_v_sat, "cpvsat");

            register_property(d_rho_l_sat_d_p, "drholsatdp");
            register_property(d_rho_v_sat_d_p, "drhovsatdp");
            register_property(d_h_l_sat_d_p, "dhlsatdp");
            register_property(d_h_v_sat_d_p, "dhvsatdp");
            register_property(d_cp_l_sat_d_p, "dcplsatdp");
            register_property(d_cp_v_sat_d_p, "dcpvsatdp");
            register_property(d_T_sat_d_p, "dtsatdp");
            register_property(d2_T_sat_d_p_d_p, "d2tsatdpdp");

            // SPLIM
            register_property(p_lim, "plim");
            register_property(h_l_lim, "hllim");
            register_property(h_v_lim, "hvlim");
        }
    } _prop_init;
}