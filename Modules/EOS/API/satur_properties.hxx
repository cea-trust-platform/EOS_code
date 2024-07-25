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



// satur_properties.hxx

#ifndef SATUR_PROPERTIES_H
#define SATUR_PROPERTIES_H

namespace NEPTUNE
{ 
#if  __cplusplus >= 201103L
  static vector<typrop> saturprop =
#else
  static typrop t_saturprop[] =
#endif
    { { "P_sat"          ,   "Pressure at saturation"                               },
      { "T_sat"          ,   "Temperature at saturation"                            },
      { "rho_lsat"       ,   "liquid density at saturation"                         },
      { "rho_vsat"       ,   "vapor density at saturation"                          },
      { "h_lsat"         ,   "liquid specific enthalpy at saturation"               },
      { "h_vsat"         ,   "vapor  specific enthalpy at saturation"               },
      { "cp_lsat"        ,   "liquid specific isobaric heat capacity at saturation" },
      { "cp_vsat"        ,   "vapor  specific isobaric heat capacity at saturation" },

      { "d(T_sat)/dP"    ,   "d(T_sat)/dP"                                          },
      { "d(rho_lsat)/dP" ,   "d(rho_lsat)/dP"                                       },
      { "d(rho_vsat)/dP" ,   "d(rho_vsat)/dP"                                       },
      { "d(h_lsat)/dP"   ,   "d(h_lsat)/dP"                                         },
      { "d(h_vsat)/dP"   ,   "d(h_vsat)/dP"                                         },
      { "d(cp_lsat)/dP"  ,   "d(cp_lsat)/dP"                                        },
      { "d(cp_vsat)/dP"  ,   "d(cp_vsat)/dP"                                        }, 

      { "d2(T_sat)/dPdP" ,   "d2(T_sat)/dPdP"                                       }
    };
#if  __cplusplus <  201103L
 static vector<typrop> saturprop(t_saturprop, 
                                 t_saturprop + sizeof(t_saturprop) / sizeof(typrop) );
#endif


  enum EOS_saturprop
    { NotASatProperty = lastTProperty + 1 ,

      p_sat     ,
      firstSatProperty = p_sat,
      T_sat     ,
      rho_l_sat ,
      rho_v_sat ,
      h_l_sat   ,
      h_v_sat   ,
      cp_l_sat  ,
      cp_v_sat  ,

      d_T_sat_d_p     = cp_v_sat + 2 ,
      d_rho_l_sat_d_p ,
      d_rho_v_sat_d_p ,
      d_h_l_sat_d_p   ,
      d_h_v_sat_d_p   ,
      d_cp_l_sat_d_p  ,
      d_cp_v_sat_d_p  ,

      d2_T_sat_d_p_d_p = d_cp_v_sat_d_p + 2,
      lastSatProperty = d2_T_sat_d_p_d_p
    };
  enum EOS_limsaturprop
    { EOS_TSATPROP     =    firstSatProperty ,
      EOS_TSATPROPDER  =  d_T_sat_d_p ,
      EOS_TSATPROPDER2 = d2_T_sat_d_p_d_p
    };
  
  EOS_saturprop nam2num_saturprop(const char* const name)   ;
  EOS_saturprop nam2num_dsaturprop(const char* const name)  ;
  EOS_saturprop nam2num_d2saturprop(const char* const name) ;

}
#include "EOS/API/satur_properties_i.hxx"
#endif
