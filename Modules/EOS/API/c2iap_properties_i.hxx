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




#ifndef C2IAP_PROPERTIES_I_H
#define C2IAP_PROPERTIES_I_H

namespace NEPTUNE
{
  inline EOS_c2iapprop nam2num_c2iapprop(const char* const name)
  { if (eostp_strcmp(name,"epstl")    == 0)  return epstl  ;
    if (eostp_strcmp(name,"hlspsc")   == 0)  return hlspsc ;
    if (eostp_strcmp(name,"hlsvsc")   == 0)  return hlsvsc ;
    if (eostp_strcmp(name,"epstg")    == 0)  return epstg  ;
    if (eostp_strcmp(name,"hvspsc")   == 0)  return hvspsc ;
    if (eostp_strcmp(name,"hvsvsc")   == 0)  return hvsvsc ;

    return nam2num_dc2iapprop(name) ;
  }

  inline EOS_c2iapprop nam2num_dc2iapprop(const char* const name)
  { if (eostp_strcmp(name,"depstldph")    == 0) return d_epstl_dp_h       ; 
    if (eostp_strcmp(name,"depstldhp")    == 0) return d_epstl_dh_p       ; 
    if (eostp_strcmp(name,"dhlspscdph")   == 0) return d_hlspsc_dp_h      ; 
    if (eostp_strcmp(name,"dhlspscdhp")   == 0) return d_hlspsc_dh_p      ; 
    if (eostp_strcmp(name,"dhlsvscdph")   == 0) return d_hlsvsc_dp_h      ; 
    if (eostp_strcmp(name,"dhlsvscdhp0")  == 0) return d_hlsvsc_dh_p0     ; 
    if (eostp_strcmp(name,"dhlsvscdhp")   == 0) return d_hlsvsc_dh_p      ; 
    if (eostp_strcmp(name,"dhlsvscdc1ph") == 0) return d_hlsvsc_d_c_1_ph  ;
    if (eostp_strcmp(name,"dhlsvscdc2ph") == 0) return d_hlsvsc_d_c_2_ph  ;
    if (eostp_strcmp(name,"dhlsvscdc3ph") == 0) return d_hlsvsc_d_c_3_ph  ;
    if (eostp_strcmp(name,"dhlsvscdc4ph") == 0) return d_hlsvsc_d_c_4_ph  ;
    if (eostp_strcmp(name,"depstgdph")    == 0) return d_epstg_dp_h       ;
    if (eostp_strcmp(name,"depstgdhp")    == 0) return d_epstg_dh_p       ;
    if (eostp_strcmp(name,"depstgdc1ph")  == 0) return d_epstg_d_c_1_ph   ; 
    if (eostp_strcmp(name,"depstgdc2ph")  == 0) return d_epstg_d_c_2_ph   ; 
    if (eostp_strcmp(name,"depstgdc3ph")  == 0) return d_epstg_d_c_3_ph   ; 
    if (eostp_strcmp(name,"depstgdc4ph")  == 0) return d_epstg_d_c_4_ph   ; 
    if (eostp_strcmp(name,"dhvspscdph")   == 0) return d_hvspsc_dp_h      ;    
    if (eostp_strcmp(name,"dhvspscdhp")   == 0) return d_hvspsc_dh_p      ;    
    if (eostp_strcmp(name,"dhvspscdc1ph") == 0) return d_hvspsc_d_c_1_ph  ;
    if (eostp_strcmp(name,"dhvspscdc2ph") == 0) return d_hvspsc_d_c_2_ph  ;
    if (eostp_strcmp(name,"dhvspscdc3ph") == 0) return d_hvspsc_d_c_3_ph  ;
    if (eostp_strcmp(name,"dhvspscdc4ph") == 0) return d_hvspsc_d_c_4_ph  ;
    if (eostp_strcmp(name,"dhvsvscdph")   == 0) return d_hvsvsc_dp_h      ;
    if (eostp_strcmp(name,"dhvsvscdhp")   == 0) return d_hvsvsc_dh_p      ;
    if (eostp_strcmp(name,"dhvsvscdc1ph") == 0) return d_hvsvsc_d_c_1_ph  ;
    if (eostp_strcmp(name,"dhvsvscdc2ph") == 0) return d_hvsvsc_d_c_2_ph  ;
    if (eostp_strcmp(name,"dhvsvscdc3ph") == 0) return d_hvsvsc_d_c_3_ph  ;
    if (eostp_strcmp(name,"dhvsvscdc4ph") == 0) return d_hvsvsc_d_c_4_ph  ;

    return NotAC2iapProperty ;
  }

}
#endif
