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



#include "EOS/Src/EOS_Thetis/thetis.hxx"
#include "Language/API/Language.hxx"
#include "EOS/Src/EOS_Thetis/EOS_ThetisUtil.hxx"
#include "EOS/Src/EOS_Thetis/EOS_ThetisWater.hxx"

namespace NEPTUNE_EOS
{
  const AString EOS_ThetisWater::fluidname("Water") ;
  bool EOS_ThetisWater::loaded = false ;
  // Fluid number
  int EOS_ThetisWater::fluidNum = -1 ;
  // Table name (file.mdl)
  const char* EOS_ThetisWater::tableName = "eau.mdl" ;
  // Critical title (default value : 0.5)
  double EOS_ThetisWater::criticalQuality = 0.5 ;

  EOS_ThetisWater::EOS_ThetisWater(){}

  EOS_ThetisWater::~EOS_ThetisWater(){}

  int EOS_ThetisWater::init(const Strings& strings, const int& nbFluidMax, int& fluidCounter)
  { int iret ;
    iret = EOS_ThetisFluid::init(loaded, criticalQuality, fluidNum,
                                 fluidCounter, tableName, strings,
                                 nbFluidMax) ;
    return iret ;
  }

}
