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



#ifndef EOS_Cathare_FreonR12Liquid_i_hxx
#define EOS_Cathare_FreonR12Liquid_i_hxx

namespace NEPTUNE_EOS
{
  inline EOS_Internal_Error EOS_Cathare_FreonR12Liquid::get_p_min(double& P_min) const
  { P_min = CATHARE_freonR12.gl_Pmin() ;
    return EOS_Internal_Error::OK  ;
  }

  inline EOS_Internal_Error EOS_Cathare_FreonR12Liquid::get_h_min(double& h_min) const
  { h_min = CATHARE_freonR12.gl_hmin() ;
    return EOS_Internal_Error::OK  ;
  }

  inline EOS_Internal_Error EOS_Cathare_FreonR12Liquid::get_T_min(double& T_min) const
  { T_min = CATHARE_freonR12.gl_Tmin() ;
    return EOS_Internal_Error::OK  ;
  }

  inline EOS_Internal_Error EOS_Cathare_FreonR12Liquid::get_p_max(double& P_max) const
  { P_max = CATHARE_freonR12.gl_Pmax() ;
    return EOS_Internal_Error::OK  ;
  }

  inline EOS_Internal_Error EOS_Cathare_FreonR12Liquid::get_h_max(double& h_max) const
  { h_max = CATHARE_freonR12.gl_hmax() ;
    return EOS_Internal_Error::OK  ;
  }

  inline EOS_Internal_Error EOS_Cathare_FreonR12Liquid::get_T_max(double& T_max) const
  { T_max = CATHARE_freonR12.gl_Tmax() ;
    return EOS_Internal_Error::OK  ;
  }

  inline EOS_Error EOS_Cathare_FreonR12Liquid::compute(const EOS_Field& p, 
                                                       const EOS_Field& h, 
                                                       EOS_Fields& r, 
                                                       EOS_Error_Field& errfield) const
  { EOS_Fields input(2) ;
    input[0] = p ;
    input[1] = h ;
    int dom = 1 ;
    int sz=input[0].size() ;
    ArrOfInt err2_data(sz) ;
    EOS_Error_Field err2(err2_data) ;

    EOS_Error cr  = CATHARE_freonR12.verify(input,errfield,dom) ;
    EOS_Error cr2 = CATHARE_freonR12.calca_all_liquid(input,r,err2) ;
    errfield.set_worst_error(err2) ;
    return worst_generic_error(cr,cr2) ;
  }

  inline EOS_Error EOS_Cathare_FreonR12Liquid::compute(const EOS_Fields& input, 
                                                        EOS_Fields& r, 
                                                        EOS_Error_Field& errfield) const
  { int dom = 1 ;
    int sz = input[0].size() ;
    int nb_infields=input.size() ;       
    ArrOfInt err2_data(sz) ;
    EOS_Error_Field err2(err2_data) ;

    EOS_Error cr  = CATHARE_freonR12.verify(input,errfield,dom) ;
    EOS_Error cr2 = EOS_Error::error ;
    if (nb_infields == 1)
      { const EOS_Field* p=&(input[0]) ;
        cr2 = CATHARE_freonR12.calca_all_saturation(*p,r,err2) ;
      }
    else if(nb_infields==2)
      cr2 = CATHARE_freonR12.calca_all_liquid(input,r,err2) ;
    errfield.set_worst_error(err2) ;
    return worst_generic_error(cr,cr2) ;
  }
}
#endif
