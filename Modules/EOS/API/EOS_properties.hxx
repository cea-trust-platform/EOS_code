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



#ifndef EOS_PROPERTIES_H
#define EOS_PROPERTIES_H

#define PROPNAME_MSIZE 40
#include "Common/func.hxx"
#include <string>
using std::string;
#include <vector>
using std::vector;

class typrop { public:
                 string name ;
                 string definition ; 
             };

#include "therm_properties.hxx"
#include "satur_properties.hxx"
#include "splim_properties.hxx"
#include "camix_properties.hxx"
#include "c2iap_properties.hxx"

namespace NEPTUNE
{

  typedef int EOS_Property;

  inline EOS_Property gen_property_number(const char* namep) 
  {
    EOS_thermprop tprop = nam2num_thermprop(namep) ;
    if (tprop != NotATProperty) return static_cast<EOS_Property>(tprop) ;
  
    EOS_saturprop satprop = nam2num_saturprop(namep) ;
    if (satprop != NotASatProperty) return static_cast<EOS_Property>(satprop) ;
  
    EOS_splimprop splimprop = nam2num_splimprop(namep) ;
    if (splimprop != NotASplimProperty) return static_cast<EOS_Property>(splimprop) ;
  
    EOS_camixprop camixprop = nam2num_camixprop(namep) ;
    if (camixprop != NotACamixProperty) return static_cast<EOS_Property>(camixprop) ;
  
    EOS_c2iapprop c2iapprop = nam2num_c2iapprop(namep) ;
    return static_cast<EOS_Property>(c2iapprop) ;
  }
}


#endif
