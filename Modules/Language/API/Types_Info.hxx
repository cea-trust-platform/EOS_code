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



#ifndef Types_Info_hh
#define Types_Info_hh 1

#include "Language/API/Language.hxx"
#include "Language/API/UnboundedSetByValue.hxx"
#include <stdexcept>

namespace TYPESHANDLING
{
  class Type_Info;
  class RegisteredClass;
}
using LANGUAGE_KERNEL::UnboundedSetByValue;

namespace TYPESHANDLING
{
  class RegisteredClass;
  class Types_Info 
  {

  public:
    Types_Info(const Types_Info &right);

    virtual ~Types_Info();

    Types_Info & operator=(const Types_Info &right);

    int operator==(const Types_Info &right) const;

    int operator!=(const Types_Info &right) const;

    friend std::ostream & 
    operator<<(std::ostream &stream,const Types_Info &right);

    friend std::istream & 
    operator>>(std::istream &stream,Types_Info &object);


    static Types_Info& instance ();
    void describe () const;

    int register_type (const NEPTUNE::AString& type, const NEPTUNE::AString& mother, 
		       const NEPTUNE::AString& coment, unsigned int sz, 
		       TYPESHANDLING::RegisteredClass& (*create) (void));

    int register_type (const NEPTUNE::AString& type, const NEPTUNE::AString& coment, 
		       unsigned int sz, TYPESHANDLING::RegisteredClass& (*create) (void));

    RegisteredClass& create_Object (NEPTUNE::AString type) const;

    int size_of_Object (const RegisteredClass& ob) const;

    const Type_Info& operator [] (int i) const;

    const Type_Info& operator [] (const NEPTUNE::AString& type) const;

    void  print_types() const;

    void ctri(int n, NEPTUNE::Strings &chac, int *indx) const;

    int nb_types () const;

    void destroy_all ();

    const UnboundedSetByValue<Type_Info> get_the_Type_Info () const;
    void set_the_Type_Info (UnboundedSetByValue<Type_Info> value);

  protected:

  private:
    Types_Info ();
    UnboundedSetByValue<Type_Info> the_Type_Info;

  private: 
    static Types_Info *the_instance;

  };
}

#include "Language/API/Types_Info_i.hxx"
#endif
