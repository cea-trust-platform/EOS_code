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




#include "Language/API/Types_Info.hxx"
#include "EOS/API/EOS_Fields.hxx"
#include "EOS/API/EOS_Field.hxx"

namespace NEPTUNE
{
  static RegisteredClass& EOS_Fields_create()
  { return *(new EOS_Fields()) ;
  }
  int EOS_Fields::type_Id=(RegisterType("EOS_Fields", 
                           "Some Fields for EOS",
                           sizeof(EOS_Fields),
                           EOS_Fields_create));

  ostream& EOS_Fields::print_On (ostream& stream) const
  { stream << " Number of EOS_Field : " << n << endl ;
    for(int i=0; i<n; i++)
      stream << " " << i+1 << "- " << the_fields[i] << endl ;
    return stream ;
  }

  istream& EOS_Fields::read_On (istream& stream)
  { return stream ;
  }

  const Type_Info& EOS_Fields::get_Type_Info () const
  { return (Types_Info::instance())[type_Id] ;
  }

  EOS_Fields::EOS_Fields(int sz) :
    n(sz)
  { the_fields = new EOS_Field[sz] ;
  }
  
  EOS_Fields::~EOS_Fields()
  { if (the_fields)  delete[] the_fields;
  }

  EOS_Field& EOS_Fields::operator[](const int index) const
  { assert(index>-1);
    assert(index<n);
    return the_fields[index] ;
  }

  int EOS_Fields::size() const
  { return n ;
  }

  int EOS_Fields::resize(int newsz)
  { EOS_Field* old = the_fields;
    the_fields = new EOS_Field[newsz];

    int i=0;
    if (newsz >= n)
       { for (i=0; i<n; i++)  the_fields[i] = old[i] ;
       }
    else
       { for (i=0; i<newsz; i++)  the_fields[i] = old[i] ;
       }
    n = newsz ;

    if (old) delete[] old ;

    return 1 ;
  }

}
