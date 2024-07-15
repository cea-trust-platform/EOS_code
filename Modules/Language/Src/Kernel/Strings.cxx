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
#include "Language/API/Types_Info.hxx"

static RegisteredClass& Strings_create()
{ return *(new NEPTUNE::Strings()) ;
}


namespace NEPTUNE
{
  int Strings::type_Id = (RegisterType("Strings", 
                                       "some strings",
                                       sizeof(Strings),
                                       Strings_create)) ;

  Strings::Strings() 
  : data(0), sz(0)
  {
  }

  Strings::Strings(int asz) 
  : data(new AString[asz]), sz(asz)
  {
  }

  Strings::Strings(const Strings &right) 
  : UObject(), data(new AString[right.sz]), sz(right.sz)
  { for(int i=0; i<sz; i++)
       data[i] = right.data[i] ;
  }

  Strings::~Strings()
  { if (data)  delete [] data ;
  }


  Strings & Strings::operator=(const Strings &right)
  { if (data)  delete [] data ;
    sz = right.sz ;
    data = new AString[sz] ;
    for(int i=0; i<sz; i++)
       data[i] = right.data[i] ;
    return *this ;
  }

  int Strings::operator==(const Strings &right) const
  { int ok = 1 ;
    for(int i=0; (ok && i<sz); i++)
      ok *= (data[i]==right.data[i]) ;
    return ok ;
  }

  int Strings::operator!=(const Strings &right) const
  { return !(*this == right) ;
  }


  ostream & operator<<(ostream &stream,const Strings &right)
  {  stream << right.sz << endl ;
    for(int i=0; i<right.sz; i++)
      stream << right.data[i] << " " ;
    return stream<<endl ;
  }

  istream & operator>>(istream &stream,Strings &object)
  { int sz ;
    stream >> sz;
    object.resize(sz);
    for(int i=0; i<object.sz; i++)
       stream >> object.data[i] ;
    return stream ;
  }

  const Type_Info& Strings::get_Type_Info () const
  { return (Types_Info::instance())[type_Id] ;
  }

  ostream& Strings::print_On (ostream& stream) const
  { return stream << *this ;
  }

  istream& Strings::read_On (istream& stream)
  { return stream >> *this ;
  }

  int Strings::resize(int newsz)
  { AString* old=data ;
    data = new AString[newsz] ;
    int i = 0 ;
    if (newsz >= sz)
      { for(i=0; i<sz; i++)
          data[i] = old[i] ;
        for(i=sz; i<newsz; i++)
          data[i] = "empty" ;
      }
    else
      { for(i=0; i<newsz; i++)
          data[i] = old[i] ;
      }
    sz = newsz ;
    if (old)  delete [] old ;
    return 1 ;
  }

  int Strings::search(const char* const str) const
  { int i = 0 ;
    while(i<size())
      { if (data[i] == str)  return i ;
        ++i ;
      }
    return -1 ;
  }
}
