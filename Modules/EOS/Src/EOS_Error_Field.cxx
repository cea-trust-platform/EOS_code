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



#include "system/arch.h"
#include "Language/API/Types_Info.hxx"
#include "EOS/API/EOS_Error_Field.hxx"

namespace NEPTUNE
{
  static RegisteredClass& EOS_Error_Field_create()
  { return *(new EOS_Error_Field()) ;
  }
  
  int EOS_Error_Field::type_Id = (RegisterType("EOS_Error_Field", 
                                  "An Error Field for EOS",
                                  sizeof(EOS_Error_Field),
                                  EOS_Error_Field_create)) ;


  ostream& EOS_Error_Field::print_On (ostream &stream) const
  { stream << "EOS_Error_Field name = " << name 
           << "  data = " 
           << data << endl ;
    return stream ;
  }

  istream& EOS_Error_Field::read_On (istream &stream)
  { return stream ;
  }

  const Type_Info& EOS_Error_Field::get_Type_Info () const
  { return (Types_Info::instance())[type_Id] ;
  }

  EOS_Error_Field::EOS_Error_Field():
    data(), library_codes(), name()
  {
  }

  EOS_Error_Field::EOS_Error_Field(const EOS_Error_Field &f):
     UObject(), data(f.data.size(), &f.data[0]),
     library_codes(f.library_codes.size(), &f.library_codes[0]),
     name(f.name)
  {
  }

  EOS_Error_Field::EOS_Error_Field(ArrOfInt &a):
    data(a.size(), &a[0]),
    library_codes(a.size()),
    name()
  {
  }

  ArrOfInt& EOS_Error_Field::set_data()
  { return data ;
  }

  const ArrOfInt& EOS_Error_Field::get_data() const
  { return data ;
  }

  const AString& EOS_Error_Field::get_name() const
  { return name ;
  }

  const EOS_Error_Field & EOS_Error_Field::operator= (EOS_Internal_Error err)
  { const int n = size() ;
    for (int i=0; i<n; i++)
    {
       data[i] = err.get_code() ;
       library_codes[i] = err.get_library_code() ;
    }
    return *this ;
  }
  
  // Set error field to worst error between this and "field"
  void EOS_Error_Field::set_worst_error(const EOS_Error_Field &field)
  { const int n = size() ;
    for (int i=0; i<n; i++) 
       { EOS_Internal_Error err = worst_internal_error(field[i], EOS_Internal_Error(data[i])) ;
         data[i] = err.get_code() ;
         library_codes[i] = err.get_library_code() ;
       }
  }
   
  // Returns the worst error in the array
  EOS_Internal_Error EOS_Error_Field::find_worst_error() const
  { EOS_Internal_Error err = EOS_Internal_Error::OK ;
    const int n = size() ;
    for (int i=0; i<n; i++)
       err = worst_internal_error(err, EOS_Internal_Error(data[i])) ;
    return err ;
  }
 
}
