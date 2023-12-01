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





#ifndef Objects_ptr_h
#define Objects_ptr_h 1


#include "Language/API/DynamicArray.hxx"
using LANGUAGE_KERNEL::DynamicArray;

namespace NEPTUNE
{
  class Boolean;
}

namespace OBJECTSHANDLING
{
  class NumberedObject;


  class Objects_ptr 
  {

  public:
    Objects_ptr();
    Objects_ptr(int sz);

    Objects_ptr(const Objects_ptr &right);
    virtual ~Objects_ptr();

    Objects_ptr & operator=(const Objects_ptr &right);

    int operator==(const Objects_ptr &right) const;

    int operator!=(const Objects_ptr &right) const;

    friend ostream & operator<<(ostream &stream,const Objects_ptr &right);

    friend istream & operator>>(istream &stream,Objects_ptr &object);


    const NumberedObject& get_object (int key) const;
    NumberedObject& set_object (int key) const;
    void destroy_all ();
    int add_object (NumberedObject& obj);
    virtual NEPTUNE::Boolean delete_object (int key);
    int size () const;
    const DynamicArray< int  > get_the_keys () const;
    void set_the_keys (DynamicArray< int  > value);
  protected:

  private:
    DynamicArray< int  > the_keys;

  private: 

  };
}
#include "Language/API/Objects_ptr_i.hxx"
#endif
