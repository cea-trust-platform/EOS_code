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



#include "EOS/Src/EOS_Thetis/EOS_ThetisWaterMPLiquid.hxx"
#include "Language/API/Language.hxx"
#include "EOS/API/EOS_Field.hxx"
#include "Language/API/RegisterType.hxx"
#include "Language/API/Types_Info.hxx"
using namespace TYPESHANDLING ;

namespace NEPTUNE_EOS
{
  const AString EOS_ThetisWaterMPLiquid::equationname("WaterMPLiquid") ;

  EOS_ThetisWaterMPLiquid::EOS_ThetisWaterMPLiquid(){};

  EOS_ThetisWaterMPLiquid::~EOS_ThetisWaterMPLiquid(){};

  static RegisteredClass& EOS_ThetisWaterMPLiquid_create()
  { return *(new EOS_ThetisWaterMPLiquid()) ;
  }

  int EOS_ThetisWaterMPLiquid::type_Id=(RegisterType("EOS_ThetisWaterMPLiquid", "EOS_Fluid",
                                        "Equation Of States for Thetis WaterMP Liquid",
                                        sizeof(EOS_ThetisWaterMPLiquid),
                                        EOS_ThetisWaterMPLiquid_create)) ;
  
  const AString& EOS_ThetisWaterMPLiquid::fluid_name() const
  { return fluidname ;
  }

  const AString& EOS_ThetisWaterMPLiquid::equation_name() const
  { return equationname ;
  } 

  ostream& EOS_ThetisWaterMPLiquid::print_On (ostream& stream) const
  { return stream ;
  }

  istream& EOS_ThetisWaterMPLiquid::read_On (istream& stream)
  { return stream ;
  }

  const Type_Info& EOS_ThetisWaterMPLiquid::get_Type_Info () const
  { return (Types_Info::instance())[type_Id] ;
  }

  //! Thetis initialization + fluid loading
  int EOS_ThetisWaterMPLiquid::init(const Strings& strings)
  { int iret ;
    // Initialization
    iret = EOS_THETIS::init(strings) ;
    // Loading
    if (iret == (int)good)
       iret = EOS_ThetisWaterMP::init(strings, nbFluidMax, fluidCounter) ;
    return iret ;
  }

  //! get fluid number
  int EOS_ThetisWaterMPLiquid::getFluidNum() const
  { return fluidNum ;
  }

}
