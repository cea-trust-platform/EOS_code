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



#include <iostream>
#include "Language/API/Language.hxx"
#include "Language/API/Types_Info.hxx"

static RegisteredClass& ArrOfInt_create()
{ return *(new NEPTUNE::ArrOfInt()) ;
}

namespace NEPTUNE
{
  int ArrOfInt::type_Id = (RegisterType("ArrOfInt", 
                                        "an array of int",
                                        sizeof(ArrOfInt),
                                        ArrOfInt_create)) ;

  ArrOfInt::ArrOfInt(const ArrOfInt &right)
    : UObject(), LANGUAGE_KERNEL::ArrOf<int>(right)
  {
  }

  ArrOfInt::~ArrOfInt()
  {
  }

  ostream & operator<<(ostream &stream,const ArrOfInt &right)
  { stream << right.size() << endl ;
    for(int i=0; i<right.size(); i++)
      { stream << right[i]<< " " ;
        if ((i%4)==3)  stream << endl ;
      }
    return stream ;
  }

  istream & operator>>(istream &stream, ArrOfInt &object)
  { object.detach();
    int nsz ;
    stream >> nsz ;
    object.resize(nsz) ;
    for(int i=0; i<nsz; i++)
      stream >> object[i] ;  
    return stream ;
  }

  const Type_Info& ArrOfInt::get_Type_Info () const
  { return Types_Info::instance()[type_Id] ;
  }

  ostream& ArrOfInt::print_On (ostream& stream) const
  {
    return stream << *this << endl ;
  }

  istream& ArrOfInt::read_On (istream& stream)
  { return stream >> *this ;
  }
}
