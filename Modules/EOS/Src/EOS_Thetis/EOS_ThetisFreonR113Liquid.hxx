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



#ifndef  EOS_THETISFREONR113LIQUID_H
#define  EOS_THETISFREONR113LIQUID_H

#include "EOS/Src/EOS_Thetis/EOS_ThetisFreonR113.hxx"
#include "EOS/Src/EOS_Thetis/EOS_ThetisLiquid.hxx"
#include "EOS/Src/EOS_Thetis/EOS_THETIS.hxx"
#include "EOS/Src/EOS_Thetis/thetis.hxx"

#include "EOS/API/EOS_Fluid.hxx"



using namespace NEPTUNE;

namespace NEPTUNE_EOS
{
  class EOS_ThetisFreonR113Liquid : private EOS_ThetisFreonR113, 
				public EOS_ThetisLiquid
  {
	  static const AString equationname;
  public:

    virtual const AString& fluid_name() const;
    virtual const AString& equation_name() const;
    
    
    EOS_ThetisFreonR113Liquid();
    virtual ~EOS_ThetisFreonR113Liquid();

    //! Thetis initialization + fluid loading
    virtual int init(const Strings&);

    //  Other methods
    //
    //! see Language
    virtual ostream& print_On (ostream& stream=cout) const;
    //! see Language
    virtual istream& read_On (istream& stream=cin);
    //! see Language
    virtual const Type_Info& get_Type_Info () const;

  protected :
    // get fluid number
    int getFluidNum() const;

  private: 
    static int type_Id;    
  };
}

#endif

