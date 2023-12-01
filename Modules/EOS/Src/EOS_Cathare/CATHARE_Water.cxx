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



#include "CATHARE_Water.hxx"

namespace CATHARE
{
  CATHARE_Water::CATHARE_Water() :
    // init
    tabsk(273.15),
    // bounds
    xtlm (0.0), xtlp(400.0), xtgm (-150.0), xtgp(2000.0),
    xpmte (0.01e5), xppte (260.e5),
    xhlmte (100.0), xhlpte(2.4e6), xhvmte (1.0), xhvpte(1.e8)
    {}

  CATHARE_Water::CATHARE_Water(NEPTUNE::EOS_Fluid* fluid) :
    // init
    tabsk(273.15),
    // bounds
    xtlm (0.0), xtlp(400.0), xtgm (-150.0), xtgp(2000.0),
    xpmte (0.01e5), xppte (260.e5),
    xhlmte (100.0), xhlpte(2.4e6), xhvmte (1.0), xhvpte(1.e8),
    // fluid
    the_fluid(fluid)
  {}
}
