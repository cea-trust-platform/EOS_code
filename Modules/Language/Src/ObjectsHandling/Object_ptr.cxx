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



#include "Language/API/Objects.hxx"
#include "Language/Src/ObjectsHandling/Object_ptr.hxx"
#include "Language/API/NumberedObject.hxx"
#include "Language/API/Boolean.hxx"
#include "Language/API/UObject.hxx"

// Class Object_ptr 
namespace OBJECTSHANDLING
{
  Object_ptr::Object_ptr()
  {
  }

  Object_ptr::Object_ptr(const Object_ptr &right)
    : key(right.key)
  {
  }

  Object_ptr::~Object_ptr()
  {
  }


  Object_ptr & Object_ptr::operator=(const Object_ptr &right)
  { key = right.key ;
    return *this ;
  }

  int Object_ptr::operator==(const Object_ptr &right) const
  { return (key == right.key) ;
  }

  int Object_ptr::operator!=(const Object_ptr &right) const
  { return (key != right.key) ;
  }

  ostream & operator<<(ostream &stream,const Object_ptr &right)
  { (void)right   ;
    return stream ;
  }

  istream & operator>>(istream &stream,Object_ptr &object)
  { (void)object  ;
    return stream ;
  }



  const NumberedObject& Object_ptr::get_object ()
  { return Objects::instance().get_object(key) ;
  }

  NEPTUNE::Boolean Object_ptr::set_object (const NumberedObject& obj)
  { key = obj.get_key() ;
    return True ;
  }
}
