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



#include <stdlib.h>
#include "EOS/API/EOS.hxx"
#include "EOS/Src/EOS_Thetis/thetis.hxx"
#include "Language/API/Language.hxx"
#include "EOS/Src/EOS_Thetis/EOS_ThetisUtil.hxx"
#include "EOS/Src/EOS_Thetis/EOS_ThetisFluid.hxx"
#include "EOS/API/EOS_Config.hxx"
#include "EOS/API/EOS_eosdatadir.hxx"

namespace NEPTUNE_EOS 
{
  EOS_ThetisFluid::EOS_ThetisFluid(){}

  EOS_ThetisFluid::~EOS_ThetisFluid(){}

  int EOS_ThetisFluid::init(bool& loaded, double& criticalTitle,
                            int& fluidNum, int& fluidCounter,
                            const char* tableName, const Strings& strings,
                            const int& nbFluidMax)
  { AString sValue ;
    int criticalTitleDefine = getOptionValue("criticalTitle", strings, sValue) ;
    int errorLoad = 0 ;

    if (loaded and criticalTitleDefine == 1)
       { cerr << "Warning : " << tableName 
              << " is already loaded,the new \"criticalTitle\" value is not taking into account" << endl ;
         return EOS_Error::error ;
       }

    if (!loaded)
       { if (fluidCounter != nbFluidMax)
            { fluidCounter++ ;
              fluidNum = fluidCounter ;
              if (criticalTitleDefine == 1)  criticalTitle = atof(sValue.aschar()) ;

              AString TableFile ;
              if (iret_eos_data_dir) return EOS_Error::error ;
              TableFile  = eos_data_dir.c_str() ;
              TableFile += "/EOS_Thetis/" ;
              TableFile += AString(tableName) ;
              char * tableNameTemp = const_cast<char *>(TableFile.aschar()) ;
              errorLoad = TVLOAD(fluidNum, &tableNameTemp, criticalTitle) ;
              loaded = true ;

              if (errorLoad > 0)
                 { TVERROR(errorLoad) ;
                   exit(0) ;
                   return EOS_Error::good ;
                 }
            }
         else
            { cerr << "Error : too many Thetis fluids loaded " << endl ;
              exit(0) ;
              return EOS_Error::error ;
            }
       }

    return EOS_Error::good ;
  }

}
