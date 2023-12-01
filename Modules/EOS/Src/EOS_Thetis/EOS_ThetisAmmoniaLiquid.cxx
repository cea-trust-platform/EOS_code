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



#include "EOS/Src/EOS_Thetis/EOS_ThetisAmmoniaLiquid.hxx"
#include "Language/API/Language.hxx"
#include "EOS/API/EOS_Field.hxx"
#include "Language/API/RegisterType.hxx"
#include "Language/API/Types_Info.hxx"
using namespace TYPESHANDLING ;

namespace NEPTUNE_EOS
{
  const AString EOS_ThetisAmmoniaLiquid::equationname("AmmoniaLiquid") ;

  const AString& EOS_ThetisAmmoniaLiquid::fluid_name() const
  { return fluidname ;
  }

  const AString& EOS_ThetisAmmoniaLiquid::equation_name() const
  { return equationname ;
  }
  
  EOS_ThetisAmmoniaLiquid::EOS_ThetisAmmoniaLiquid(){} ;

  EOS_ThetisAmmoniaLiquid::~EOS_ThetisAmmoniaLiquid(){} ;

  static RegisteredClass& EOS_ThetisAmmoniaLiquid_create()
  { return *(new EOS_ThetisAmmoniaLiquid()) ;
  }

  int EOS_ThetisAmmoniaLiquid::type_Id=(RegisterType("EOS_ThetisAmmoniaLiquid", "EOS_Fluid",
                                        "Equation Of States for Thetis Ammonia Liquid",
                                        sizeof(EOS_ThetisAmmoniaLiquid),
                                        EOS_ThetisAmmoniaLiquid_create));

  ostream& EOS_ThetisAmmoniaLiquid::print_On (ostream& stream) const
  { return stream ;
  }

  istream& EOS_ThetisAmmoniaLiquid::read_On (istream& stream)
  { return stream ;
  }

  const Type_Info& EOS_ThetisAmmoniaLiquid::get_Type_Info () const
  { return (Types_Info::instance())[type_Id] ;
  }

  //! Thetis initialization + fluid loading
  int EOS_ThetisAmmoniaLiquid::init(const Strings& strings)
  { int iret ;
    // Initialization
    iret = EOS_THETIS::init(strings) ;
    // Loading
    if (iret == (int)good) 
       iret = EOS_ThetisAmmonia::init(strings, nbFluidMax, fluidCounter) ;
    return iret ;
  }

  //! get fluid number
  int EOS_ThetisAmmoniaLiquid::getFluidNum() const
  { return fluidNum ;
  }

}
