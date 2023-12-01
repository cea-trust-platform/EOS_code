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



#ifndef DynamicArray_h
#define DynamicArray_h 1
#include <iostream>

#include "Language/API/Boolean.hxx"

namespace LANGUAGE_KERNEL
{
  template <class T>
  class DynamicArray 
  {

  public:
    DynamicArray();
    DynamicArray(int sz);

    DynamicArray(const DynamicArray< T > &right);

    virtual ~DynamicArray();

    DynamicArray< T > & operator=(const DynamicArray< T > &right);

    int operator==(const DynamicArray< T > &right) const;

    int operator!=(const DynamicArray< T > &right) const;

    T& operator[](const int index) const;

    template <class X>
    friend ostream & 
    operator<<(ostream &stream,const DynamicArray< X > &right);
    template <class X>
    friend istream & 
    operator>>(istream &stream,DynamicArray< X > &object);

    int size () const;

    int add_value (const T& to_add);

    NEPTUNE::Boolean delete_value (const T& to_suppr);


  protected:

  private:
    const T* get_objs () const;
    void set_objs (T* value);

  private: 
    T* objs;
    int nb_objs;
    int nb_objs_max;
    static int step;
  };
}
#include "Language/API/DynamicArray_i.hxx"
#endif
