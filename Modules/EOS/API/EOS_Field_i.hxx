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




#ifndef EOS_FIELD_I_H
#define EOS_FIELD_I_H

namespace NEPTUNE
{
  inline  EOS_Field::EOS_Field() :
  property_name("")
  {
  }

  inline  EOS_Field::EOS_Field(const char* const namet, 
                               const char* const namep,
                               int nsz,    double* ptr) :
    data(nsz, ptr),
    property_title(namet), 
    property_name(namep)
  { property_number = gen_property_number(namep) ;
  }

  inline int EOS_Field::gen_property_number(const char* namep) 
  { int iret ;

    iret = (int) nam2num_thermprop(namep) ;
    if (iret != NEPTUNE::NotATProperty) return iret ;

    iret = (int) nam2num_saturprop(namep) ;
    if (iret != NEPTUNE::NotASatProperty) return iret ;

    iret = (int) nam2num_splimprop(namep) ;
    if (iret != NEPTUNE::NotASplimProperty) return iret ;

    iret = (int) nam2num_camixprop(namep) ;
    if (iret != NEPTUNE::NotACamixProperty) return iret ;

    iret = (int) nam2num_c2iapprop(namep) ;
    return iret ;
  }

  inline const double& EOS_Field::operator [] (int i) const
  { return data[i] ;
  }

  inline double& EOS_Field::operator [] (int i)
  { return data[i] ;
  }

  inline void EOS_Field::set_property_title(const char* const name)
  { property_title = name ;
  }

  inline void EOS_Field::set_property_name(const char* const name)
  { property_name = name ;
    property_number = gen_property_number(name);
  }

  inline  int EOS_Field::size() const
  { return data.size() ;
  }
}
#endif
