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




#ifndef THERM_PROPERTIES_I_H
#define THERM_PROPERTIES_I_H

namespace NEPTUNE
{
  inline EOS_thermprop nam2num_thermprop(const char* const name)
  { if (eostp_strcmp(name,"p")          == 0)  return p              ;
    if (eostp_strcmp(name,"h")          == 0)  return h              ;
    if (eostp_strcmp(name,"t")          == 0)  return T              ;
    if (eostp_strcmp(name,"rho")        == 0)  return rho            ;
    if (eostp_strcmp(name,"u")          == 0)  return u              ;
    if (eostp_strcmp(name,"s")          == 0)  return s              ;
    if (eostp_strcmp(name,"mu")         == 0)  return mu             ;
    if (eostp_strcmp(name,"lambda")     == 0)  return lambda         ;
    if (eostp_strcmp(name,"cp")         == 0)  return cp             ;
    if (eostp_strcmp(name,"cv")         == 0)  return cv             ;
    if (eostp_strcmp(name,"sigma")      == 0)  return sigma          ;
    if (eostp_strcmp(name,"w")          == 0)  return w              ;
    if (eostp_strcmp(name,"g")          == 0)  return g              ;
    if (eostp_strcmp(name,"f")          == 0)  return f              ;
    if (eostp_strcmp(name,"pr")         == 0)  return pr             ;
    if (eostp_strcmp(name,"beta")       == 0)  return beta           ;
    if (eostp_strcmp(name,"gamma")      == 0)  return gamma          ;
    return nam2num_dthermprop(name) ;
  }

  inline EOS_thermprop nam2num_dthermprop(const char* const name)
  { if (eostp_strcmp(name,"dtdph")      == 0)  return d_T_d_p_h      ;
    if (eostp_strcmp(name,"dtdhp")      == 0)  return d_T_d_h_p      ;
    if (eostp_strcmp(name,"drhodph")    == 0)  return d_rho_d_p_h    ;
    if (eostp_strcmp(name,"drhodhp")    == 0)  return d_rho_d_h_p    ;
    if (eostp_strcmp(name,"drhodtp")    == 0)  return d_rho_d_T_p    ;
    if (eostp_strcmp(name,"drhodpt")    == 0)  return d_rho_d_p_T    ;
    if (eostp_strcmp(name,"dudph")      == 0)  return d_u_d_p_h      ;
    if (eostp_strcmp(name,"dudhp")      == 0)  return d_u_d_h_p      ;
    if (eostp_strcmp(name,"dsdph")      == 0)  return d_s_d_p_h      ;
    if (eostp_strcmp(name,"dsdhp")      == 0)  return d_s_d_h_p      ;
    if (eostp_strcmp(name,"dmudph")     == 0)  return d_mu_d_p_h     ;
    if (eostp_strcmp(name,"dmudhp")     == 0)  return d_mu_d_h_p     ;
    if (eostp_strcmp(name,"dlambdadph") == 0)  return d_lambda_d_p_h ;
    if (eostp_strcmp(name,"dlambdadhp") == 0)  return d_lambda_d_h_p ;
    if (eostp_strcmp(name,"dcpdph")     == 0)  return d_cp_d_p_h     ;
    if (eostp_strcmp(name,"dcpdhp")     == 0)  return d_cp_d_h_p     ;
    if (eostp_strcmp(name,"dcvdph")     == 0)  return d_cv_d_p_h     ;
    if (eostp_strcmp(name,"dcvdhp")     == 0)  return d_cv_d_h_p     ;
    if (eostp_strcmp(name,"dsigmadph")  == 0)  return d_sigma_d_p_h  ;
    if (eostp_strcmp(name,"dsigmadhp")  == 0)  return d_sigma_d_h_p  ;
    if (eostp_strcmp(name,"dwdph")      == 0)  return d_w_d_p_h      ;
    if (eostp_strcmp(name,"dwdhp")      == 0)  return d_w_d_h_p      ;
    if (eostp_strcmp(name,"dgdph")      == 0)  return d_g_d_p_h      ;
    if (eostp_strcmp(name,"dgdhp")      == 0)  return d_g_d_h_p      ;
    if (eostp_strcmp(name,"dfdph")      == 0)  return d_f_d_p_h      ;
    if (eostp_strcmp(name,"dfdhp")      == 0)  return d_f_d_h_p      ;
    if (eostp_strcmp(name,"dprdph")     == 0)  return d_pr_d_p_h     ;
    if (eostp_strcmp(name,"dprdhp")     == 0)  return d_pr_d_h_p     ;
    if (eostp_strcmp(name,"dbetadph")   == 0)  return d_beta_d_p_h   ;
    if (eostp_strcmp(name,"dbetadhp")   == 0)  return d_beta_d_h_p   ;
    if (eostp_strcmp(name,"dgammadph")  == 0)  return d_gamma_d_p_h  ;
    if (eostp_strcmp(name,"dgammadhp")  == 0)  return d_gamma_d_h_p  ;
    if (eostp_strcmp(name,"dudtp")      == 0)  return d_u_d_T_p      ;
    if (eostp_strcmp(name,"dsdtp")      == 0)  return d_s_d_T_p      ;
    if (eostp_strcmp(name,"dsdpt")      == 0)  return d_s_d_p_T      ;
    if (eostp_strcmp(name,"dmudtp")     == 0)  return d_mu_d_T_p     ;
    if (eostp_strcmp(name,"dmudpt")     == 0)  return d_mu_d_p_T     ;
    if (eostp_strcmp(name,"dlambdadpt") == 0)  return d_lambda_d_p_T ;
    if (eostp_strcmp(name,"dlambdadtp") == 0)  return d_lambda_d_T_p ;
    if (eostp_strcmp(name,"dcpdtp")     == 0)  return d_cp_d_T_p     ;
    if (eostp_strcmp(name,"dcpdpt")     == 0)  return d_cp_d_p_T     ;
    if (eostp_strcmp(name,"dcvdtp")     == 0)  return d_cv_d_T_p     ;
    if (eostp_strcmp(name,"dcvdpt")     == 0)  return d_cv_d_p_T     ;
    if (eostp_strcmp(name,"dsigmadtp")  == 0)  return d_sigma_d_T_p  ;
    if (eostp_strcmp(name,"dwdtp")      == 0)  return d_w_d_T_p      ;
    if (eostp_strcmp(name,"dwdpt")      == 0)  return d_w_d_p_T      ;
    if (eostp_strcmp(name,"dgdtp")      == 0)  return d_g_d_T_p      ;
    if (eostp_strcmp(name,"dfdtp")      == 0)  return d_f_d_T_p      ;
    if (eostp_strcmp(name,"dprdtp")     == 0)  return d_pr_d_T_p     ;
    if (eostp_strcmp(name,"dbetadtp")   == 0)  return d_beta_d_T_p   ;
    if (eostp_strcmp(name,"dgammadtp")  == 0)  return d_gamma_d_T_p  ;
    if (eostp_strcmp(name,"dhdtp")      == 0)  return d_h_d_T_p      ;
    if (eostp_strcmp(name,"dhdpt")      == 0)  return d_h_d_p_T      ;
    if (eostp_strcmp(name,"dhdsp")      == 0)  return d_h_d_s_p      ;
    if (eostp_strcmp(name,"dhdps")      == 0)  return d_h_d_p_s      ;
    return NotATProperty ;
  }

}
#endif
