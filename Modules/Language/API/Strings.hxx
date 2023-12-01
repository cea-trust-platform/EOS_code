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





#ifndef Strings_h
#define Strings_h 1


// UObject
#include "Language/API/UObject.hxx"


using LANGUAGE_KERNEL::UObject;

namespace NEPTUNE
{
  class AString;
  //! An array Of Strings
  class Strings : public UObject  
  {

  public:
    //! Constructs an empty array of String
    Strings();

    //! Constructs an array of sz Strings
    Strings(int sz);

    Strings(const Strings &right);

    virtual ~Strings();

    Strings & operator=(const Strings &right);
  
    int operator==(const Strings &right) const;

    int operator!=(const Strings &right) const;

    //! returns the index_th String
    AString & operator[](const int index) const;

    //! returns the index_th String
    AString & value_at(const int index) const;

    //! operator[](index)=x
    AString & set_value_at(const int index, const AString& x) const;

    int resize(int );

    int size() const;

    //! returns -1 if operator[](i)!=str forall i;
    //! else returns i such that operator[](i)==str
    int search(const char* const str) const;

    friend ostream & 
    operator<<(ostream &stream,
	       const Strings &right);

    friend istream & 
    operator>>(istream &stream,Strings &object);

    virtual const Type_Info& get_Type_Info () const;

    virtual ostream& 
    print_On (ostream& stream=cout) const;

    virtual istream& 
    read_On (istream& stream=cin);


  protected:

  private:
    static int type_Id;
    AString* data;
    int sz;
    int ref_count;
  };
}
#include "Language/API/Strings_i.hxx"
#endif
