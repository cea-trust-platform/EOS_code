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



#ifndef EOS_CATHARE2LIQUID_HXX
#define EOS_CATHARE2LIQUID_HXX

#include "EOS_Cathare2.hxx"

using namespace NEPTUNE;

namespace NEPTUNE_EOS
{

  //! EOS_Cathare2Liquid : adapter to Hydcom/Eos Cathare2 subroutines, liquid phase
  class EOS_Cathare2Liquid : public EOS_Cathare2
  {
    static const AString phasename;

  public:
    //! Main EOS_Cathare2Liquid constructor
    EOS_Cathare2Liquid();
    //! EOS_Cathare2Liquid destructor
    ~EOS_Cathare2Liquid();
    //! Type information method: see language documentation
    const Type_Info& get_Type_Info () const;
    //! Return the phase name ("Liquid")
    const AString& phase_name() const;

  private:
    //! Type identifier: see language documentation
    static int type_Id;

  };

}

#endif
