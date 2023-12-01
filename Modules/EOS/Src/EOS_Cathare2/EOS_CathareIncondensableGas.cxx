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



#include <sstream>
#include "EOS_CathareIncondensableGas.hxx"

using namespace NEPTUNE;

namespace NEPTUNE_EOS
{
  static AString UnknownStr("Unknown") ;

  const AString EOS_CathareIncondensableGas::tablename("CathareIncondensableGas");

  const AString& EOS_CathareIncondensableGas::table_name()const
  { return tablename ;
  } 
//  const AString& EOS_Cathare2::version_name()const
//  {
//        return "v25_2"
//  }
  
  ostream& EOS_CathareIncondensableGas::print_On (ostream& stream) const
  { return stream ;
  }

  istream& EOS_CathareIncondensableGas::read_On (istream& stream)
  { return stream ;
  }

  const AString& EOS_CathareIncondensableGas::fluid_name() const
  { return fluidname ;
  }

  static RegisteredClass& CathareIncondensableGas_create()
  { return *(new EOS_CathareIncondensableGas()) ;
  }

  EOS_CathareIncondensableGas::EOS_CathareIncondensableGas() :
    fluidname(UnknownStr), cathare_name("        ")
  {
  }

  EOS_CathareIncondensableGas::EOS_CathareIncondensableGas(const EOS_CathareIncondensableGas&)
  { assert(0);
  }

  EOS_CathareIncondensableGas::~EOS_CathareIncondensableGas()
  {
  }

  int EOS_CathareIncondensableGas::type_Id = (RegisterType ("EOS_CathareIncondensableGas",
                                                            "EOS_Fluid",
                                                            "Cathare2 Incondensable Gas",
                                                            sizeof(EOS_CathareIncondensableGas),
                                                            CathareIncondensableGas_create)) ;

  const Type_Info& EOS_CathareIncondensableGas::get_Type_Info () const
  { return (Types_Info::instance())[type_Id] ;
  }

  int EOS_CathareIncondensableGas::init(const Strings& args) 
  { int i = 0 ;
    if (cathare_name == AString("        ")) 
       { if (args.size() < 1) return EOS_Error::error ;
         if      ( args[i] == AString("Nitrogen") ) cathare_name = "NITROGEN";
         else if ( args[i] == AString("Hydrogen") ) cathare_name = "HYDROGEN";
         else if ( args[i] == AString("Oxygen"  ) ) cathare_name = "OXYGEN  ";
         else if ( args[i] == AString("Air"     ) ) cathare_name = "AIR     ";
         else if ( args[i] == AString("Argon"   ) ) cathare_name = "ARGON   ";
         else if ( args[i] == AString("Helium"  ) ) cathare_name = "HELIUM  ";
         else if ( args[i] == AString("User"    ) ) cathare_name = "OTHERGAS";
         else return EOS_Error::error ;
         fluidname = args[0];
         // Warning : the optional keyword POLYNCPT is not allowed. By default POLYNCPT=0.  
         F77NAME(c2_ninccopy)(cathare_name.aschar());
         if (cathare_name != AString("OTHERGAS"))
           F77NAME(c2_rproga)( 1, 1, prxr, prxcp0, prxcp1, prxcp2, prxcp3, prxcp4, prxcp5, prxcp6,
                              prxm, prxl0, prxl1, prxl2, prxm0, prxm1, prxm2, prxdv);
         nb_args = 0;
         i++;
       }
    if (cathare_name == "OTHERGAS")
      { for (;i<args.size();i++) 
           { double val;
             std::istringstream iss;
             iss.str(args[i].aschar());
             if ( ! (iss >> val).fail() ) 
                { switch (nb_args) 
                     { case  0: prxr   = val; nb_args++; break;
                       case  1: prxcp0 = val; nb_args++; break;
                       case  2: prxcp1 = val; nb_args++; break;
                       case  3: prxcp2 = val; nb_args++; break;
                       case  4: prxcp3 = val; nb_args++; break;
                       case  5: prxcp4 = val; nb_args++; break;
                       case  6: prxcp5 = val; nb_args++; break;
                       case  7: prxcp6 = val; nb_args++; break;
                       case  8: prxm   = val; nb_args++; break;
                       case  9: prxl0  = val; nb_args++; break;
                       case 10: prxl1  = val; nb_args++; break;
                       case 11: prxl2  = val; nb_args++; break;
                       case 12: prxm0  = val; nb_args++; break;
                       case 13: prxm1  = val; nb_args++; break;
                       case 14: prxm2  = val; nb_args++; break;
                       case 15: prxdv  = val; nb_args++; break;
                       default: return EOS_Error::error ;
                     }
                }
             else 
                return EOS_Error::error ;
           }
      }
    return EOS_Error::good ;
  }

  int EOS_CathareIncondensableGas::init(const Strings& args, const Strings& other_args) 
  { int iret ;
    iret = init(args) ;
    if (iret == (int)EOS_Error::good)  iret = init(other_args) ;
    return iret ;
  }

}
