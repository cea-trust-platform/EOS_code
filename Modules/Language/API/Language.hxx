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



#ifndef LanguageAPI_h
#define LanguageAPI_h 1

#include <iostream>
#include <iomanip>
#include "Language/API/RegisterType.hxx"
#include "Language/API/Object_ID.hxx"
#include "Language/API/AString.hxx"
#include "Language/API/Strings.hxx"
#include "Language/API/ArrOfInt.hxx"
#include "Language/API/ArrOfDouble.hxx"

namespace NEPTUNE
{
  class Object
  {
  public :
    Object(const char* const str);
    Object(const UObject* ptr);
    const Object_ID& id() const;
    virtual ~Object();
    Object();
  private :
    const Object_ID& ptr;
  };
  extern void
  hierrarchy ();

  extern int 
  hierrarchy_on_file (const char* const);
  
  extern void
  list_objects ();

  extern int 
  list_objects_on_file (const char* const);

  extern void 
  print_obj (const Object& obj_id);
  
  extern void
  read_obj (const Object& obj_id);

  extern const AString& 
  get_type (const Object& obj_id);

  extern int 
  has_type (const char* const, const Object& obj_id);

  extern int 
  has_sub_type (const char* const, const Object& obj_id);

  extern int 
  resize_obj (const Object& obj, int sz);

  extern void 
  Language_init ();

  extern void 
  Language_finalize ();

  extern int 
  self_test();
}// namespace

    
#endif
