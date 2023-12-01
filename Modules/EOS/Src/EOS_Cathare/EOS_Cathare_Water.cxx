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



#include "EOS_Cathare_Water.hxx"
#include "Language/API/Types_Info.hxx"

namespace NEPTUNE_EOS
{
  const AString EOS_Cathare_Water::fluidname("Water")    ;
  const AString EOS_Cathare_Water::tablename("Cathare")  ;
  const AString EOS_Cathare_Water::versionname("V1.5.b") ;

  static RegisteredClass& EOS_Cathare_Water_create()
  { return *(new EOS_Cathare_Water()) ;
  }

  int EOS_Cathare_Water::type_Id=(RegisterType("EOS_Cathare_Water", "EOS_Fluid",
                                  "CATHARE Liquid Water Functions",
                                  sizeof(EOS_Cathare_Water),
                                  EOS_Cathare_Water_create)) ;

  const AString& EOS_Cathare_Water::fluid_name() const
  { return fluidname ;
  }

  const AString& EOS_Cathare_Water::table_name() const
  { return tablename ;
  }

  const AString& EOS_Cathare_Water::version_name() const
  { return versionname ;
  }
  
  EOS_Cathare_Water::EOS_Cathare_Water() :
  CATHARE_water(this)
  {}

  ostream& EOS_Cathare_Water::print_On (ostream& stream) const
  { return stream ;
  }

  istream& EOS_Cathare_Water::read_On (istream& stream)
  { return stream ;
  }

  const Type_Info& EOS_Cathare_Water::get_Type_Info () const
  { return (Types_Info::instance())[type_Id] ;
  }

  int EOS_Cathare_Water::init(const Strings&)
  { return good ;
  }

  void EOS_Cathare_Water::describe_error(const EOS_Internal_Error ierr, AString & description) const
  { switch(ierr.get_partial_code()) 
       { case    930 :  description = "Cathare_Water: p < p_min" ;            break ;
         case    931 :  description = "Cathare_Water: p > p_max" ;            break ;
         case    920 :  description = "Cathare_Water: h < h_min" ;            break ;
         case    921 :  description = "Cathare_Water: h > h_max" ;            break ;
         case    910 :  description = "Cathare_Water: T < T_min" ;            break ;
         case    911 :  description = "Cathare_Water: T > T_max" ;            break ;
         case   3800 :  description = "Cathare_Water: no convergence h(p,T)"; break ;
         default     :  EOS_Fluid::describe_error(ierr, description);         break ;
       }
  }

}
