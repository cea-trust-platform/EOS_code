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





#ifndef Type_Info_h
#define Type_Info_h 1

#include "Language/API/Language.hxx"
#include "Language/API/NumberedObject.hxx"

namespace NEPTUNE
{
  class Boolean;
}

namespace TYPESHANDLING
{
  class RegisteredClass;
  class Type_Info : public NumberedObject  
  {

  public:
    Type_Info (const NEPTUNE::AString& name, const NEPTUNE::AString& com, 
	       RegisteredClass& (*f) (void), unsigned int size, 
	       const NEPTUNE::AString& mother);

    Type_Info (const NEPTUNE::AString& name, const NEPTUNE::AString& com, 
	       RegisteredClass& (*f) (void), 
	       unsigned int size);

    ~Type_Info ();

    int operator==(const Type_Info &right) const;

    int operator!=(const Type_Info &right) const;

    virtual const NEPTUNE::AString& get_type () const;

    Type_Info& operator = (const Type_Info& t);

    RegisteredClass& instance () const;

    int get_type_id () const;

    int get_sz () const;

    const NEPTUNE::AString& get_name () const;

    const NEPTUNE::AString& get_comment () const;

    const NEPTUNE::AString& get_mother () const;

    const NEPTUNE::AString& set_comment (const NEPTUNE::AString& str);

    const NEPTUNE::AString& set_name (const NEPTUNE::AString& str);

    const NEPTUNE::AString& set_mother (const NEPTUNE::AString& str);

    ostream& describe (ostream& stream) const;

    NEPTUNE::Boolean has_base (const Type_Info& t) const;

    virtual ostream& print_On (ostream& stream) const;

    virtual istream& read_On (istream& stream);

  protected:

  private:
    Type_Info (const Type_Info& );

  private: 
    RegisteredClass& (*create_instance) (void);
    int type_id;
    int sz;
    NEPTUNE::AString name;
    NEPTUNE::AString comment;
    NEPTUNE::AString mother;
  };
}
#include "Type_Info_i.hxx"
#endif
