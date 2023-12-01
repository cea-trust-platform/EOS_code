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



#ifndef Cathare_Water_i_hxx
#define Cathare_Water_i_hxx
#include "EOS/API/EOS_Error_Field.hxx"

namespace NEPTUNE_EOS
{
  inline EOS_Error EOS_Cathare_Water::verify_ph() const
  { // warning : must be changed
    return EOS_Error::error ;
  }

  inline EOS_Error EOS_Cathare_Water::compute(const EOS_Field& p, 
                                      const EOS_Field& h, 
                                      EOS_Fields& r, 
                                      EOS_Error_Field& errfield) const
  { (void) p ; (void) h ; (void) r ; (void) errfield ;
    // warning : must be changed
    return EOS_Error::error ;
  }

  inline EOS_Error EOS_Cathare_Water::compute(const EOS_Field& p, 
          const EOS_Field& h, 
          EOS_Field& r, 
          EOS_Error_Field& errfield) const
  { EOS_Fields fields(1) ;
    fields[0] = r ;
    return compute(p, h, fields, errfield) ;
  }

  inline EOS_Error EOS_Cathare_Water::compute(const EOS_Field& p, 
          EOS_Fields& output, 
          EOS_Error_Field& errfield) const
  { int dom = 0 ;
    int sz = p.size();
    EOS_Error cr=CATHARE_water.verify(p,errfield,dom);

    ArrOfInt err2_data(sz) ;
    EOS_Error_Field err2(err2_data);
    EOS_Error cr2 = CATHARE_water.calca_all_saturation(p, output, err2) ;

    errfield.set_worst_error(err2) ;
    return worst_generic_error(cr,cr2) ;
  }

  inline EOS_Error EOS_Cathare_Water::compute(const EOS_Field& p, 
                                      EOS_Field& output, 
                                      EOS_Error_Field& errfield) const
  { EOS_Fields fields(1) ;
    fields[0] = output ;
    return compute(p, fields, errfield) ;
  }
}
#endif
