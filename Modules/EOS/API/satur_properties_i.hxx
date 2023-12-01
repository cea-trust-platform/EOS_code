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




#ifndef SATUR_PROPERTIES_I_H
#define SATUR_PROPERTIES_I_H

namespace NEPTUNE
{
  inline EOS_saturprop nam2num_saturprop(const char* const name)
  { if (eostp_strcmp(name,"psat")       == 0)  return p_sat            ;
    if (eostp_strcmp(name,"tsat")       == 0)  return T_sat            ;
    if (eostp_strcmp(name,"rholsat")    == 0)  return rho_l_sat        ;
    if (eostp_strcmp(name,"rhovsat")    == 0)  return rho_v_sat        ;
    if (eostp_strcmp(name,"hlsat")      == 0)  return h_l_sat          ;
    if (eostp_strcmp(name,"hvsat")      == 0)  return h_v_sat          ;
    if (eostp_strcmp(name,"cplsat")     == 0)  return cp_l_sat         ;
    if (eostp_strcmp(name,"cpvsat")     == 0)  return cp_v_sat         ;
    return nam2num_dsaturprop(name) ;
  }

  inline EOS_saturprop nam2num_dsaturprop(const char* const name)
  { if (eostp_strcmp(name,"drholsatdp") == 0)  return d_rho_l_sat_d_p  ;
    if (eostp_strcmp(name,"drhovsatdp") == 0)  return d_rho_v_sat_d_p  ;
    if (eostp_strcmp(name,"dhlsatdp")   == 0)  return d_h_l_sat_d_p    ;
    if (eostp_strcmp(name,"dhvsatdp")   == 0)  return d_h_v_sat_d_p    ;
    if (eostp_strcmp(name,"dcplsatdp")  == 0)  return d_cp_l_sat_d_p   ;
    if (eostp_strcmp(name,"dcpvsatdp")  == 0)  return d_cp_v_sat_d_p   ;
    if (eostp_strcmp(name,"dtsatdp")    == 0)  return d_T_sat_d_p      ;
    return nam2num_d2saturprop(name) ;
  }

  inline EOS_saturprop nam2num_d2saturprop(const char* const name)
  { if (eostp_strcmp(name,"d2tsatdpdp") == 0)  return d2_T_sat_d_p_d_p ;
    return NotASatProperty ;
  }
}
#endif
