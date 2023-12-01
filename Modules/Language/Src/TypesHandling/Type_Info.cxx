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
#include "Type_Info.hxx"
#include "Language/API/Types_Info.hxx"

namespace TYPESHANDLING
{
  Type_Info::Type_Info (const NEPTUNE::AString &name, const NEPTUNE::AString &com, 
                        RegisteredClass& (*f) (void), unsigned int size, 
                        const NEPTUNE::AString& mother)
    : type_id(-1), sz(size)
  { set_name(name)      ;
    set_comment(com)    ;
    set_mother(mother)  ;
    create_instance = f ;
    type_id = Types_Info::instance().nb_types() ;
  }

  Type_Info::Type_Info (const NEPTUNE::AString &name, const NEPTUNE::AString &com, 
                        RegisteredClass& (*f) (void), unsigned int size)
    : type_id(-1), sz(size)
  { set_name(name)      ;
    set_comment(com)    ;
    set_mother("")      ;
    create_instance = f ;
    type_id = Types_Info::instance().nb_types() ;
  }

  Type_Info::Type_Info (const Type_Info&)
    : NumberedObject(), type_id(-1), sz(0)
  { assert(0) ;
  }

  Type_Info::~Type_Info ()
  {
  }


  int Type_Info::operator==(const Type_Info &right) const
  { return (name == right.name) ;
  }

  int Type_Info::operator!=(const Type_Info &right) const
  { return (name != right.name) ;
  }

  const NEPTUNE::AString& Type_Info::get_type () const
  { static NEPTUNE::AString *str = 0 ;
    if (!str)  str = new NEPTUNE::AString("Type_Info") ;
    return *str ;
  }

  Type_Info& Type_Info::operator = (const Type_Info &t)
  { (void) t ;
    cerr << "Type_Info::operator = should not be called!!" << endl ;
    return *this ;
  }

  RegisteredClass& Type_Info::instance () const
  { return (*create_instance)() ;
  }

  int Type_Info::get_type_id () const
  { assert(0) ;
    return 0  ;
  }

  int Type_Info::get_sz () const
  { return sz ;
  }

  const NEPTUNE::AString& Type_Info::set_comment (const NEPTUNE::AString &str)
  { comment = str ;
    return str    ;
  }

  const NEPTUNE::AString& Type_Info::set_name (const NEPTUNE::AString &str)
  { name = str ;
    return str ;
  }

  const NEPTUNE::AString& Type_Info::set_mother (const NEPTUNE::AString &str)
  { mother = str ;
    return str   ;
  }

  ostream& Type_Info::describe (ostream &stream) const
  { return stream << *this ;
  }

  NEPTUNE::Boolean Type_Info::has_base (const Type_Info &t) const
  { (void) t;
    assert(0) ;
    return False ;
  }

  ostream& Type_Info::print_On (ostream &stream) const
  { return stream << name << "\t: " << comment << "\t: " << mother
                  << "\t, type_id : " << type_id <<" : " 
                  << "\tsize : " << sz ;
  }

  istream& Type_Info::read_On (istream &stream)
  { return stream ;
  }

}
