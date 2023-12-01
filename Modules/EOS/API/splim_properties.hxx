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



// splim_properties.hxx
#ifndef SPLIM_PROPERTIES_H
#define SPLIM_PROPERTIES_H

namespace NEPTUNE
{
#if  __cplusplus >= 201103L
  static vector<typrop> splimprop =
#else
  static typrop t_splimprop[] =
#endif
    { { "P_lim"   ,  "Pressure in spinodale curves"                 },
      { "h_llim"  ,  "liquid  specific enthalpy in spinodale curve" },
      { "h_vlim"  ,  "vapor   specific enthalpy in spinodale curve" }
    };
#if  __cplusplus <  201103L
 static vector<typrop> splimprop(t_splimprop, 
                                 t_splimprop + sizeof(t_splimprop) / sizeof(typrop) ) ;
#endif


  enum EOS_splimprop
    { NotASplimProperty = d2_T_sat_d_p_d_p+1,

      p_lim   ,
      h_l_lim ,
      h_v_lim
    };
  enum EOS_limsplimprop
    { EOS_TLIMPROP = p_lim 
    };

  EOS_splimprop nam2num_splimprop(const char* const name) ;

}
#include "EOS/API/splim_properties_i.hxx"
#endif
