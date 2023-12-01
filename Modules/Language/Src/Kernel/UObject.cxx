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



#include "Language/API/Language.hxx"
#include "Language/Src/TypesHandling/Type_Info.hxx"

namespace LANGUAGE_KERNEL
{
  UObject::UObject()
  {
  }

  UObject::UObject(const UObject &right) 
  : NumberedObject(), RegisteredClass()
  { (void)right ;
  }

  UObject::~UObject()
  {
  }

  UObject & UObject::operator=(const UObject &right)
  { (void)right ;
    return *this ;
  }

  int UObject::operator==(const UObject &right) const
  { return get_key() == right.get_key() ;
  }

  int UObject::operator!=(const UObject &right) const
  { return get_key() != right.get_key() ;
  }

  ostream & operator<<(ostream &stream,const UObject &right)
  { return right.print_On(stream) ;
  }

  istream & operator>>(istream &stream,UObject &object)
  { return object.read_On(stream); 
  }

  const NEPTUNE::AString& UObject::get_type () const
  { return (get_Type_Info().get_name()) ;
  }

}
