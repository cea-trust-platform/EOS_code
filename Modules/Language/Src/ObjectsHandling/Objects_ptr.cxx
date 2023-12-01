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




#include <assert.h>
#include <iostream>
#include "Language/API/Objects_ptr.hxx"
#include "Language/API/NumberedObject.hxx"
// Class Objects_ptr 


namespace OBJECTSHANDLING
{
  Objects_ptr::Objects_ptr()
  {
  }

  Objects_ptr::Objects_ptr(const Objects_ptr &right)
  {
    the_keys=right.the_keys;
  }


  Objects_ptr::~Objects_ptr()
  {
  }


  Objects_ptr & Objects_ptr::operator=(const Objects_ptr &right)
  {
    the_keys=right.the_keys;
    return *this;
  }


  int Objects_ptr::operator==(const Objects_ptr &right) const
  {
    return the_keys==right.the_keys;
  }

  int Objects_ptr::operator!=(const Objects_ptr &right) const
  {
    return the_keys!=right.the_keys;
  }


  ostream & operator<<(ostream &stream,const Objects_ptr &right)
  {
    for(int i=0; i<right.size(); i++)
      stream<<right.get_object(i) << endl;
    return stream;
  }

  istream & operator>>(istream &stream,Objects_ptr &object)
  {
    for(int i=0; i<object.size(); i++)
      stream>>object.set_object(i);
    return stream;
  }


  const NumberedObject& Objects_ptr::get_object (int key) const
  {
    int id=the_keys[key];
    return Objects::instance().get_object(id);
  }

  NumberedObject& Objects_ptr::set_object (int key) const
  {
    int id=the_keys[key];
    return Objects::instance().set_object(id);
  }

  void Objects_ptr::destroy_all ()
  {
    for(int i=0; i<size();i++)
      delete &(set_object(the_keys[i]));
  }

  int Objects_ptr::add_object (NumberedObject& obj)
  {
    return the_keys.add_value(obj.get_key());
  }

  NEPTUNE::Boolean Objects_ptr::delete_object (int key)
  {
    return the_keys.delete_value(key);
  }
}
