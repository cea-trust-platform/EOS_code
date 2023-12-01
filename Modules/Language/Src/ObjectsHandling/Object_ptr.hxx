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





#ifndef Object_ptr_h
#define Object_ptr_h 1

#include "Language/API/Boolean.hxx"
#include "Language/API/Object_ID.hxx"

class UObject;
namespace OBJECTSHANDLING
{
  class NumberedObject;

  class Object_ptr 
  {

  public:
    Object_ptr();

    Object_ptr(const Object_ptr &right);

    virtual ~Object_ptr();

    Object_ptr & operator=(const Object_ptr &right);

    int operator==(const Object_ptr &right) const;

    int operator!=(const Object_ptr &right) const;

    NumberedObject* operator->() const;

    NumberedObject& operator*() const;

    friend ostream & 
    operator<<(ostream &stream,const Object_ptr &right);

    friend istream & 
    operator>>(istream &stream,Object_ptr &object);

    const NumberedObject& get_object ();

    NEPTUNE::Boolean set_object (const NumberedObject& obj);

  protected:
    int key;

  private:

  private: 

  };
}

#include "Object_ptr_i.hxx"
#endif
