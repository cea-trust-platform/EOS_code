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



#ifndef UOBJECT_HXX
#define UOBJECT_HXX

#include <string>
#include <math.h>
#include <stdlib.h>
#include <iostream>

using std::ostream ;
using std::istream ;
using std::cin     ;
using std::cerr    ;
using std::cout    ;
using std::endl    ;

#include "Language/API/RegisteredClass.hxx"
#include "Language/API/NumberedObject.hxx"

namespace TYPESHANDLING
{ class Types_Info ;
}
namespace OBJECTSHANDLING
{ class Objects_ptr ;
}
using OBJECTSHANDLING::NumberedObject ;
using OBJECTSHANDLING::Objects_ptr    ;
using TYPESHANDLING::RegisteredClass  ;
using TYPESHANDLING::Type_Info        ;
using TYPESHANDLING::Types_Info       ;

namespace LANGUAGE_KERNEL
{
  class UObject : public NumberedObject, public RegisteredClass  
  { public:

      UObject(const UObject &right) ;
      virtual ~UObject() ;

      UObject & operator=(const UObject &right) ;
      int operator==(const UObject &right) const ;
      int operator!=(const UObject &right) const ;
      friend ostream & operator<<(ostream &stream,const UObject &right) ;
      friend istream & operator>>(istream &stream,UObject &object)      ;
      const NEPTUNE::Object_ID& get_key () const ;
      virtual const NEPTUNE::AString& get_type () const;

    protected:
      UObject() ;

    private:

  };
}
#include "Language/API/UObject_i.hxx"
#endif
