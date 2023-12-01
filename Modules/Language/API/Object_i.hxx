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



#ifndef Object_i_h
#define Object_i_h 1
#include "Language/API/Types_Info.hxx"

using OBJECTSHANDLING::Objects;

inline static const Types_Info& get_Types()
{
  return Types_Info::instance();
}
inline static Types_Info& set_Types()
{
  return Types_Info::instance();
}
inline static const Objects& get_Objects()
{
  return Objects::instance();
}
inline static Objects& set_Objects()
{
  return Objects::instance();
}

inline static UObject& set_object(const NEPTUNE::Object_ID& Id)
{
  if(!(set_Objects().is_object(Id)))
    {
      cerr << Id << " is not a valid Object ID" << endl;
      exit(-1);
    }
  return (UObject&) (set_Objects().set_object(Id));
}
inline static const UObject& get_object(const NEPTUNE::Object_ID& Id)
{
  if(!(set_Objects().is_object(Id)))
    {
      cerr << Id << " is not a valid Object ID" << endl;
      exit(-1);
    }
  return (const UObject&)(set_Objects().get_object(Id));
}
#endif
