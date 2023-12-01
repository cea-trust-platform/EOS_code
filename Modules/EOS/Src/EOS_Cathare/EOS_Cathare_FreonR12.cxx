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



#include "EOS_Cathare_FreonR12.hxx"
#include "Language/API/Types_Info.hxx"

// static AString FreonR12Str("FreonR12");
// static AString CATHAREStr("CATHARE");
// static AString VersionStr("V2");

namespace NEPTUNE_EOS
{
  const AString EOS_Cathare_FreonR12::fluidname("FreonR12")         ;
  const AString EOS_Cathare_FreonR12::tablename("Cathare_FreonR12") ;
  const AString EOS_Cathare_FreonR12::versionname("V2")             ;

  static RegisteredClass& EOS_Cathare_FreonR12_create()
  { return *(new EOS_Cathare_FreonR12()) ;
  }

  int EOS_Cathare_FreonR12::type_Id=(RegisterType("EOS_Cathare_FreonR12", "EOS_Fluid",
                                     "CATHARE FreonR12 Functions",
                                     sizeof(EOS_Cathare_FreonR12),
                                     EOS_Cathare_FreonR12_create)) ;
  
  const AString& EOS_Cathare_FreonR12::fluid_name() const
  { return fluidname ;
  }

  const AString& EOS_Cathare_FreonR12::table_name() const
  { return tablename ;
  }
  
  const AString& EOS_Cathare_FreonR12::version_name() const
  { return versionname ;
  }
  
  EOS_Cathare_FreonR12::EOS_Cathare_FreonR12() :
  CATHARE_freonR12(this)
  {}

  ostream& EOS_Cathare_FreonR12::print_On (ostream& stream) const
  { return stream ;
  }

  istream& EOS_Cathare_FreonR12::read_On (istream& stream)
  { return stream ;
  }

  const Type_Info& EOS_Cathare_FreonR12::get_Type_Info () const
  { return (Types_Info::instance())[type_Id] ;
  }

  int EOS_Cathare_FreonR12::init(const Strings&)
  { return good ;
  }
  
  void EOS_Cathare_FreonR12::describe_error(const EOS_Internal_Error ierr, AString & description) const
  { switch(ierr.get_partial_code())
       { case  930 :  description = "EOS_Cathare_FreonR12: p < p_min" ;               break ;
         case  931 :  description = "EOS_Cathare_FreonR12: p > p_max" ;               break ;
         case  920 :  description = "EOS_Cathare_FreonR12: h < h_min" ;               break ;
         case  921 :  description = "EOS_Cathare_FreonR12: h > h_max" ;               break ;
         case  910 :  description = "EOS_Cathare_FreonR12: T < T_min" ;               break ;
         case  911 :  description = "EOS_Cathare_FreonR12: T > T_max" ;               break ;
         case 3800 :  description = "EOS_Cathare_FreonR12: Newton algorithm failed" ; break ;
         default   :  EOS_Fluid::describe_error(ierr, description) ;                  break ;
       }
  }

}
