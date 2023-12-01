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




#ifndef EOS_Error_Field_i_H
#define EOS_Error_Field_i_H

namespace NEPTUNE
{
  inline  EOS_Error_Field::
  EOS_Error_Field( int nsz, int* ptr) :
    data(nsz, ptr), library_codes(nsz)
  {
  }

  inline const EOS_Internal_Error EOS_Error_Field::
  operator [] (int i) const
  {
    return EOS_Internal_Error(data[i], library_codes[i]);
  }

  inline void EOS_Error_Field::
  set (int i, EOS_Internal_Error err)
  {
    data[i] = err.get_code();
    library_codes[i] = err.get_library_code() ;
  }

  inline const AString& EOS_Error_Field::
  set_name(const char* const a_name)
  {
    AString tmp(a_name);
    return name=tmp;
  }
  inline  int EOS_Error_Field::
  size() const
  {
    assert(data.size() == library_codes.size());
    return data.size();
  }
}
#endif
