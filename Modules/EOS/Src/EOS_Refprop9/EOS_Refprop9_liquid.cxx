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



/*
 * EOS_Refprop9_liquid.cxx
 *
 *  Created on: 25 mars 2010
 *
 */

#include "EOS_Refprop9_liquid.hxx"

namespace NEPTUNE_EOS
{
  const AString EOS_Refprop9_liquid::phasename("Liquid");
  
  const AString& EOS_Refprop9_liquid::phase_name()const
  { return phasename ;
  }
  
  static RegisteredClass& EOS_Refprop9_liquid_create()
  { return *(new EOS_Refprop9_liquid());
  }

  int EOS_Refprop9_liquid::type_Id = (RegisterType("EOS_Refprop9_liquid", "EOS_Fluid",
                                                  "Equation Of States for Refprop9 liquid",
                                                  sizeof(EOS_Refprop9_liquid),
                                                  EOS_Refprop9_liquid_create)) ;


  int EOS_Refprop9_liquid::init(const Strings& strings)
  { int iret ;
    iret = EOS_Refprop9::init(strings) ;
    return iret ;
  }

  EOS_Internal_Error EOS_Refprop9_liquid::call_therm(const char* const property_name, double T, double rho, double& value) const
  { EOS_Internal_Error err ;
    double e, h, p,s, cv, cp, w, hjt ;

    //reset fluid with setup
    err = callSetup() ;
    if (err.generic_error() != EOS_Error::good)  return err ;

    F77NAME(therm_rp9)(T,rho,arr_molfrac,p,e,h,s,cv,cp,w,hjt) ;

    EOS_thermprop prop = nam2num_thermprop(property_name) ;
    switch(prop)
       { case NEPTUNE::h :
            value = refprop_nrj_2_eos(h);
            break;
         case NEPTUNE::u :
            value = refprop_nrj_2_eos(e);
            break;
         case NEPTUNE::s :
            value = refprop_nrj_2_eos(s);
            break;
         case NEPTUNE::cp :
            value = refprop_nrj_2_eos(cp);
            break;
         case NEPTUNE::cv :
            value = refprop_nrj_2_eos(cv);
            break;
         case NEPTUNE::w :
            value = w;
            break;
         default :
            return EOS_Internal_Error::NOT_IMPLEMENTED ;
       }

    return EOS_Internal_Error::OK ;
  }


}
