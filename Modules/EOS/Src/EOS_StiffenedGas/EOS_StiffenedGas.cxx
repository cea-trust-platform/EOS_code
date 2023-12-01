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



#include "EOS_StiffenedGas.hxx"
#include "EOS/API/EOS.hxx"
#include "EOS/API/EOS_Field.hxx"
#include "Language/API/RegisterType.hxx"
#include "Language/API/Types_Info.hxx"
#include "EOS/API/EOS_Config.hxx"
#include "EOS/API/EOS_eosdatadir.hxx"
#include <fstream>

namespace NEPTUNE_EOS
{
  const AString EOS_StiffenedGas::tablename("StiffenedGas") ;
  const AString EOS_StiffenedGas::versionname("V1"); 

  static RegisteredClass& EOS_StiffenedGas_create()
  { return *(new EOS_StiffenedGas()) ;
  }
  int EOS_StiffenedGas::type_Id=(RegisterType("EOS_StiffenedGas", "EOS_Fluid",
                        "Equation Of States for Stiffened Gas",
                        sizeof(EOS_StiffenedGas),
                        EOS_StiffenedGas_create)) ;
  
  const AString& EOS_StiffenedGas::table_name()const
  { return tablename ;
  }

  const AString& EOS_StiffenedGas::version_name()const
  { return versionname ;
  }
  
  ostream& EOS_StiffenedGas::print_On (ostream& stream) const
  { return stream ;
  }

  istream& EOS_StiffenedGas::read_On (istream& stream)
  { return stream ;
  }

  const Type_Info& EOS_StiffenedGas::get_Type_Info () const
  { return (Types_Info::instance())[type_Id] ;
  }

  const AString& EOS_StiffenedGas::fluid_name() const
  { return FluidStr ;
  }
  
  int EOS_StiffenedGas::init(const Strings& strings)
  { FluidStr = AString("Unknown") ;
    int sz = strings.size() ;
    if (sz == 2)
       { AString data_file_name ;
         if (iret_eos_data_dir) return EOS_Error::error ;
         data_file_name  = eos_data_dir.c_str() ;
         data_file_name += "/EOS_StiffenedGas/" ;
         AString& file_name = strings[0] ;
         data_file_name += file_name ;
         int iok = 0 ;
         std::ifstream data_file(data_file_name.aschar()) ;
         if (!data_file)
            { cerr << data_file_name << " not found!" << endl ;
              assert(0) ;
              exit(-1)  ;
              return EOS_Error::error ;
            }
         // reading data_file
         else
            { iok = 1 ;
              //cerr << "Reading " << data_file_name << " ... " << std::flush;

              int nc = 1000 ;
              int ic =    0 ;
              AString str("EmptyString") ;
              data_file >> str ;
              while(str != ""  &&  ic < nc)
                 { if (str == "Fluid")
                      { data_file >> FluidStr ;
                        data_file >> str ;
                        ic++ ;
                      }
                   else if (str == "SGpinfl")
                      { data_file >> SGpinfl ;
                        data_file >> str ;
                        ic++ ;
                      }
                   else if (str == "SGcpl")
                      { data_file >> SGcpl ;
                        data_file >> str ;
                        ic++ ;
                      }
                   else if (str == "SGcvl")
                      { data_file >> SGcvl ;
                        data_file >> str ;
                        ic++ ;
                      }
                   else if (str == "SGgammal")
                      { data_file >> SGgammal ;
                        data_file >> str ;
                        ic++ ;
                      }
                   else if (str == "SGql")
                      { data_file >> SGql ;
                        data_file >> str ;
                        ic++ ;
                      }
                   else if (str == "SGqpl")
                      { data_file >> SGqpl ;
                        data_file >> str ;
                        ic++ ;
                      }
                   else if (str == "SGpinfv")
                      { data_file >> SGpinfv ;
                        data_file >> str ;
                        ic++ ;
                      }
                   else if (str == "SGcpv")
                      { data_file >> SGcpv ;
                        data_file >> str ;
                        ic++ ;
                      }
                   else if (str == "SGcvv")
                      { data_file >> SGcvv ;
                        data_file >> str ;
                        ic++ ;
                      }
                   else if (str == "SGgammav")
                      { data_file >> SGgammav ;
                        data_file >> str ;
                        ic++ ;
                      }
                   else if (str == "SGqv")
                      { data_file >> SGqv ;
                        data_file >> str ;
                        ic++ ;
                      }
                   else if (str == "SGqpv")
                      { data_file >> SGqpv ;
                        data_file >> str ;
                        ic++ ;
                      }
                   else
                      { cerr << "Ref. " << str << " is unknown !" << endl ;
                        assert(0) ;
                        exit(-1) ;
                        return EOS_Error::error ;
                     }
                 }
            }

         if (iok == 1)
            { // intermediate values
              double unsur=1.e0/(SGcpv-SGcvv) ;
              SGa = (SGcpl-SGcpv+SGqpv-SGqpl)*unsur ;
              SGb = (SGql-SGqv)*unsur   ;
              SGc = (SGcpv-SGcpl)*unsur ;
              SGd = (SGcpl-SGcvl)*unsur ;
              // if liquid or vapor ...
              AString& state = strings[1] ;
              if (state == "Liquid")
                 { SGpinf  = SGpinfl  ;
                   SGcp    = SGcpl    ;
                   SGcv    = SGcvl    ;
                   SGgamma = SGgammal ;
                   SGq     = SGql     ;
                   SGqp    = SGqpl    ;
                   return EOS_Error::good ;
                 }
              else if (state == "Vapor")
                 { SGpinf  = SGpinfv  ;
                   SGcp    = SGcpv    ;
                   SGcv    = SGcvv    ;
                   SGgamma = SGgammav ;
                   SGq     = SGqv     ;
                   SGqp    = SGqpv    ;
                   return EOS_Error::good ;
                 }
              else
                 { cerr << " The arguments " << strings[0] << " and "
                        << strings[1] 
                        << "are not implemented for EOS_StiffenedGas" 
                        << endl ;
                   assert(0) ;
                   exit(-1)  ;
                   return EOS_Error::error ;
                 }
            }
         else
            return EOS_Error::error ;
       }

    else
       {
         cerr << " Reference with size " << sz << " is not implemented for EOS_StiffenedGas ! " << endl ;
         assert(0) ;
         exit(-1)  ;
         return EOS_Error::error ;
       }
  }
}
