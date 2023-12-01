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



#include "EOS_Cathare_FreonR12Liquid.hxx"
#include "Language/API/Types_Info.hxx"

namespace NEPTUNE_EOS
{
  const AString EOS_Cathare_FreonR12Liquid::equationname("FreonR12Liquide");

  static RegisteredClass& EOS_Cathare_FreonR12Liquid_create()
  { return *(new EOS_Cathare_FreonR12Liquid());
  }

  int EOS_Cathare_FreonR12Liquid::type_Id=(RegisterType("EOS_Cathare_FreonR12Liquid", "EOS_Fluid",
                                                        "CATHARE FreonR12Liquid Functions",
                                                        sizeof(EOS_Cathare_FreonR12Liquid),
                                                        EOS_Cathare_FreonR12Liquid_create)) ;

  const AString& EOS_Cathare_FreonR12Liquid::equation_name()const
  { return equationname ;
  }
  
  ostream& EOS_Cathare_FreonR12Liquid::print_On (ostream& stream) const
  { return stream ;
  }

  istream& EOS_Cathare_FreonR12Liquid::read_On (istream& stream)
  { return stream ;
  }

  const Type_Info& EOS_Cathare_FreonR12Liquid::get_Type_Info () const
  { return (Types_Info::instance())[type_Id] ;
  }

  int EOS_Cathare_FreonR12Liquid::init(const Strings&)
  { return good ;
  }

}