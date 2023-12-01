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



// c2iap_properties.hxx

#ifndef C2IAP_PROPERTIES_H
#define C2IAP_PROPERTIES_H

#include "Language/API/AString.hxx"
#include "Common/func.hxx"
#include <string>
using std::string;
#include <vector>
using std::vector;

namespace NEPTUNE
{ 
#if  __cplusplus >= 201103L
  static vector<string> c2iapprop =
#else
  static string t_c2iapprop[] =
#endif
    { "epstl"                     ,   //  epstl               
      "hlspsc"                    ,   //  hlspsc              
      "hlsvsc"                    ,   //  hlsvsc              
      "epstg"                     ,   //  epstg               
      "hvspsc"                    ,   //  hvspsc              
      "hvsvsc"                    ,   //  hvsvsc              

      "[d(epstl)/dP]h"            ,   //  [d(epstl)/dP]h      
      "[d(epstl)/dh]P"            ,   //  [d(epstl)/dh]P      
      "[d(hlspsc)/dP]h"           ,   //  [d(hlspsc)/dP]h       
      "[d(hlspsc)/dh]P"           ,   //  [d(hlspsc)/dh]P       
      "[d(hlsvsc)/dP]h"           ,   //  [d(hlsvsc)/dP]h     
      "[d(hlsvsc)/dh]P0"          ,   //  [d(hlsvsc)/dh]P0    
      "[d(hlsvsc)/dh]P"           ,   //  [d(hlsvsc)/dh]P     
      "[d(hlsvsc)/d(c_1)]Ph"      ,   //  [d(hlsvsc)/d(c_1)]Ph
      "[d(hlsvsc)/d(c_2)]Ph"      ,   //  [d(hlsvsc)/d(c_2)]Ph
      "[d(hlsvsc)/d(c_3)]Ph"      ,   //  [d(hlsvsc)/d(c_3)]Ph
      "[d(hlsvsc)/d(c_4)]Ph"      ,   //  [d(hlsvsc)/d(c_4)]Ph
      "[d(epstg)/dP]h"            ,   //  [d(epstg)/dP]h      
      "[d(epstg)/dh]P"            ,   //  [d(epstg)/dh]P      
      "[d(epstg)/d(c_1)]Ph"       ,   //  [d(epstg)/d(c_1)]Ph 
      "[d(epstg)/d(c_2)]Ph"       ,   //  [d(epstg)/d(c_2)]Ph 
      "[d(epstg)/d(c_3)]Ph"       ,   //  [d(epstg)/d(c_3)]Ph
      "[d(epstg)/d(c_4)]Ph"       ,   //  [d(epstg)/d(c_4)]Ph
      "[d(hvspsc)/dP]h"           ,   //  [d(hvspsc)/dP]h
      "[d(hvspsc)/dh]P"           ,   //  [d(hvspsc)/dh]P
      "[d(hvspsc)/df(c_1)]Ph"     ,   //  [d(hvspsc)/df(c_1)]Ph
      "[d(hvspsc)/df(c_2)]Ph"     ,   //  [d(hvspsc)/df(c_2)]Ph
      "[d(hvspsc)/df(c_3)]Ph"     ,   //  [d(hvspsc)/df(c_3)]Ph
      "[d(hvspsc)/df(c_4)]Ph"     ,   //  [d(hvspsc)/df(c_4)]Ph
      "[d(hvsvsc)/dP]h"           ,   //  [d(hvsvsc)/dP]h            
      "[d(hvsvsc)/dh]P"           ,   //  [d(hvsvsc)/dh]P            
      "[d(hvsvsc)/d(c_1)]Ph"      ,   //  [d(hvsvsc)/d(c_1)]Ph
      "[d(hvsvsc)/d(c_2)]Ph"      ,   //  [d(hvsvsc)/d(c_2)]Ph
      "[d(hvsvsc)/d(c_3)]Ph"      ,   //  [d(hvsvsc)/d(c_3)]Ph
      "[d(hvsvsc)/d(c_4)]Ph"          //  [d(hvsvsc)/d(c_4)]Ph
    };
#if  __cplusplus <  201103L
 static std::vector<string> c2iapprop(t_c2iapprop, 
                                      t_c2iapprop + sizeof(t_c2iapprop) / sizeof(string) );
#endif


  enum EOS_c2iapprop
    { NotAC2iapProperty  = d2_T_sat_0_d_p_0_d_p_0 + 1 ,
      epstl              ,      
      hlspsc             ,     
      hlsvsc             ,     
      epstg              ,
      hvspsc             ,
      hvsvsc             ,

      d_epstl_dp_h       = hvsvsc + 2,   
      d_epstl_dh_p       ,   
      d_hlspsc_dp_h      ,    
      d_hlspsc_dh_p      ,    
      d_hlsvsc_dp_h      ,
      d_hlsvsc_dh_p0     ,
      d_hlsvsc_dh_p      ,
      d_hlsvsc_d_c_1_ph  ,
      d_hlsvsc_d_c_2_ph  ,
      d_hlsvsc_d_c_3_ph  ,
      d_hlsvsc_d_c_4_ph  ,
      d_epstg_dp_h       ,
      d_epstg_dh_p       ,
      d_epstg_d_c_1_ph   ,
      d_epstg_d_c_2_ph   ,
      d_epstg_d_c_3_ph   ,
      d_epstg_d_c_4_ph   ,
      d_hvspsc_dp_h      ,
      d_hvspsc_dh_p      ,
      d_hvspsc_d_c_1_ph  ,
      d_hvspsc_d_c_2_ph  ,
      d_hvspsc_d_c_3_ph  ,
      d_hvspsc_d_c_4_ph  ,
      d_hvsvsc_dp_h      ,     
      d_hvsvsc_dh_p      ,     
      d_hvsvsc_d_c_1_ph  ,
      d_hvsvsc_d_c_2_ph  ,
      d_hvsvsc_d_c_3_ph  ,
      d_hvsvsc_d_c_4_ph
    };
  enum EOS_limc2iapprop
    { EOS_TCIAPROP     =  epstl ,
      EOS_TCIAPROPDER  = d_epstl_dp_h
    };
    
   
  EOS_c2iapprop nam2num_c2iapprop(const char* const name)   ;
  EOS_c2iapprop nam2num_dc2iapprop(const char* const name)  ;

}
#include "EOS/API/c2iap_properties_i.hxx"
#endif 
