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



#ifndef Cathare_Steam_i_hxx
#define Cathare_Steam_i_hxx

namespace NEPTUNE_EOS
{
  inline EOS_Error EOS_Cathare_Steam::verify_ph() const
  { // warning : must be changed
    return EOS_Error::error ;
  }

  inline EOS_Internal_Error EOS_Cathare_Steam::get_p_min(double& P_min) const
  { P_min = CATHARE_water.gv_Pmin() ;
    return EOS_Internal_Error::OK ;
  }

  inline EOS_Internal_Error EOS_Cathare_Steam::get_h_min(double& h_min) const
  { h_min = CATHARE_water.gv_hmin() ;
    return EOS_Internal_Error::OK ;
  }

  inline EOS_Internal_Error EOS_Cathare_Steam::get_T_min(double& T_min) const
  { T_min = CATHARE_water.gv_Tmin() ;
    return EOS_Internal_Error::OK ;
  }

  inline EOS_Internal_Error EOS_Cathare_Steam::get_p_max(double& P_max) const
  { P_max = CATHARE_water.gv_Pmax() ;
    return EOS_Internal_Error::OK ;
  }

  inline EOS_Internal_Error EOS_Cathare_Steam::get_h_max(double& h_max) const
  { h_max = CATHARE_water.gv_hmax() ;
    return EOS_Internal_Error::OK ;
  }

  inline EOS_Internal_Error EOS_Cathare_Steam::get_T_max(double& T_max) const
  { T_max = CATHARE_water.gv_Tmax() ;
    return EOS_Internal_Error::OK ;
  }

  inline EOS_Error EOS_Cathare_Steam::compute(const EOS_Field& p, 
                                              const EOS_Field& h, 
                                              EOS_Fields& r, 
                                              EOS_Error_Field& errfield) const
  { EOS_Fields input(2) ;
    input[0] = p ;
    input[1] = h ;
    int dom = 2 ;
    int sz = p.size() ;
    ArrOfInt err2_data(sz) ;
    EOS_Error_Field err2(err2_data) ;

    EOS_Error cr  = CATHARE_water.verify(input,errfield,dom) ;
    EOS_Error cr2 = CATHARE_water.calca_all_steam(input,r,err2) ;
    errfield.set_worst_error(err2) ;
    return worst_generic_error(cr,cr2) ;
  }

  inline EOS_Error EOS_Cathare_Steam::compute(const EOS_Fields& input, 
                                              EOS_Fields& r, 
                                              EOS_Error_Field& errfield) const
  { int dom = 2 ;
    int sz = input[0].size()     ;
    int nb_infields=input.size() ;       
    ArrOfInt err2_data(sz)       ;
    EOS_Error_Field err2(err2_data) ;

    EOS_Error cr  = CATHARE_water.verify(input,errfield,dom);
    EOS_Error cr2 = EOS_Error::error ;
    if (nb_infields == 1)
      { const EOS_Field* p=&(input[0]) ;
        cr2 = CATHARE_water.calca_all_saturation(*p,r,errfield) ;
      }
    else if (nb_infields == 2)
      cr2 = CATHARE_water.calca_all_steam(input,r,errfield) ;
    errfield.set_worst_error(err2) ;
    return worst_generic_error(cr,cr2) ;
  }
}
#endif
