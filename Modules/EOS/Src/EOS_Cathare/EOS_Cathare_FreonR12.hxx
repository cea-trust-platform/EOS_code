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



#ifndef EOS_Cathare_FreonR12_hxx
#define EOS_Cathare_FreonR12_hxx

#include "EOS/API/EOS_Fluid.hxx"
#include "CATHARE_FreonR12.hxx"

using namespace NEPTUNE ;
using namespace CATHARE_FREONR12 ;

namespace NEPTUNE_EOS
{
  class EOS_Cathare_FreonR12: public EOS_Fluid
  { static const AString fluidname   ;
    static const AString tablename   ;
    static const AString versionname ;

  public:
    virtual const AString& fluid_name()   const ;
    virtual const AString& table_name()   const ;
    virtual const AString& version_name() const ;
    
    EOS_Cathare_FreonR12() ;
    virtual ostream& print_On (ostream& stream=cout) const ;
    virtual istream& read_On (istream& stream=cin) ;
    virtual const Type_Info& get_Type_Info () const ;
    virtual int init(const Strings&) ;
    virtual EOS_Error compute(const EOS_Field& p, 
                              const EOS_Field& h, 
                              EOS_Fields& r, 
                              EOS_Error_Field& errfield) const ;
    virtual EOS_Error compute(const EOS_Field& p, 
                              const EOS_Field& h, 
                              EOS_Field& r, 
                              EOS_Error_Field& errfield) const ;
    virtual EOS_Error compute(const EOS_Field& p, 
                              EOS_Fields& output, 
                              EOS_Error_Field& errfield) const ;
    virtual EOS_Error compute(const EOS_Field& p, 
                              EOS_Field& output, 
                              EOS_Error_Field& errfield) const ;
    void describe_error(const EOS_Internal_Error ierr, AString & description) const ;
    
  protected:
    CATHARE_FreonR12 CATHARE_freonR12 ;
  private:
    static int type_Id ;
  };
}
#include "EOS_Cathare_FreonR12_i.hxx"

#endif

