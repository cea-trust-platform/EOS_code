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



#include "Language/API/Language.hxx"
#include "Language/API/F77Language.hxx"
#include "Language/API/Types_Info.hxx"
#include "Language/API/Object_i.hxx"
#include <fstream>
using std::ofstream ;

#define maxF77Objects 124
static NEPTUNE::ArrOfInt F77Objectids(maxF77Objects) ;
static NEPTUNE::Object* F77Object[maxF77Objects]     ;
static int F77Objectcpt = 0 ;

inline static NEPTUNE::Object *get_obj(int j, int& k)
{ for(int i=0; i<maxF77Objects; i++)
     { if (F77Objectids[i] == j)
          { k = i ;
            return F77Object[i] ;
          }
     }
  return 0 ;
}

inline static void create(NEPTUNE::Object *ob, int& i)
{ i=ob->id() ;
  F77Objectids[F77Objectcpt] =  i ;
  F77Object[F77Objectcpt]    = ob ;
  F77Objectcpt++ ;
}

void F77NAME(create_object)(const char *str, int &i)
{ if (F77Objectcpt == 0)
     { for(int i=0; i<maxF77Objects; i++)
          { F77Objectids[i] = -1 ;
            F77Object[i] = 0 ;
          }
     }
  NEPTUNE::Object *ob = new NEPTUNE::Object(str) ;
  create(ob, i) ;
}
 
void F77NAME(adopt_array_of_double)(double *ptr, const int &sz, int &i)
{ NEPTUNE::ArrOfDouble *x  = new NEPTUNE::ArrOfDouble(sz, ptr) ;
  NEPTUNE::Object      *ob = new NEPTUNE::Object(x) ;
  create(ob, i) ;
}

void F77NAME(adopt_array_of_int)(int *ptr, const int &sz, int &i)
{ NEPTUNE::ArrOfInt *x  = new NEPTUNE::ArrOfInt(sz, ptr) ;
  NEPTUNE::Object   *ob = new NEPTUNE::Object(x) ;
  create(ob, i);
}

void F77NAME(adopt_string)(const char *const str, int &i)
{ NEPTUNE::AString *string = new NEPTUNE::AString(str) ;
  NEPTUNE::Object  *ob     = new NEPTUNE::Object(string) ;
  create(ob, i);
}

void F77NAME(delete_object)(const int& i)
{ int k = -1 ;
  NEPTUNE::Object* ob=get_obj(i, k) ;
  delete ob ;
  F77Objectids[k] = -1 ;
  F77Object[k]    =  0 ;
  
}

NEPTUNE::Object& NEPTUNE_Object(int i)
{ int j = 0 ;
  return *(get_obj(i, j)) ;
}

NEPTUNE::ArrOfDouble& NEPTUNE_ArrOfDouble(int id)
{ const NEPTUNE::Object     &obj    = NEPTUNE_Object(id) ;
  const NEPTUNE::Object_ID  &obj_id = obj.id() ;
  if (has_sub_type("ArrOfDouble", obj))
     { NEPTUNE::ArrOfDouble& arr=(NEPTUNE::ArrOfDouble&) get_object(obj_id) ;
       return arr ;
     }
  else
     { cerr << id << " is not an ArrOfDouble!" << endl ;
       exit(-1) ;
     }
  return NEPTUNE_ArrOfDouble(-1) ;
}

NEPTUNE::ArrOfInt& NEPTUNE_ArrOfInt(int id)
{ const NEPTUNE::Object     &obj    = NEPTUNE_Object(id) ;
  const NEPTUNE::Object_ID  &obj_id = obj.id() ;
  if (has_sub_type("ArrOfInt", obj))
     { NEPTUNE::ArrOfInt& arr = (NEPTUNE::ArrOfInt&) get_object(obj_id) ;
       return arr ;
     }
  else
     { cerr << id << " is not an ArrOfInt!" << endl ;
       exit(-1) ;
     }
  return NEPTUNE_ArrOfInt(-1) ;
}

NEPTUNE::AString& NEPTUNE_AString(int id)
{ const NEPTUNE::Object& obj=NEPTUNE_Object(id);
  const NEPTUNE::Object_ID& obj_id=obj.id();
  if (has_sub_type("AString", obj))
     { NEPTUNE::AString &arr = (NEPTUNE::AString&) get_object(obj_id) ;
       return arr ;
     }
  else
     { cerr << id << " is not an AString!" << endl ;
       exit(-1) ;
     }
  return NEPTUNE_AString(-1);
}

void NEPTUNE::hierrarchy ()
{ cout << get_Types() ;
}

int NEPTUNE::hierrarchy_on_file (const char* const filename)
{ ofstream os(filename) ;
  os << get_Types() ;
  if (os) return True ;
  return False ;
}

void NEPTUNE::list_objects ()
{ cout<<get_Objects() ;
}

int NEPTUNE::list_objects_on_file (const char* const filename)
{ ofstream os(filename);
  os << get_Objects() ;
  if (os) return True ;
  return False ;
}

void NEPTUNE::print_obj (const Object &obj)
{ const Object_ID &obj_id = obj.id() ;
  if (!(set_Objects().is_object(obj_id)))
     { cerr << obj_id << " is not a valid Object ID" << endl ;
     }
  cout<<get_object(obj_id) ;
}

void NEPTUNE::read_obj (const Object &obj)
{ const Object_ID &obj_id = obj.id();
  if (!(set_Objects().is_object(obj_id)))
     { cerr << obj_id << " is not a valid Object ID" << endl ;
     }
  cin>>set_object(obj_id) ;
}

const NEPTUNE::AString &NEPTUNE::get_type (const Object &obj)
{ const Object_ID &obj_id = obj.id() ;
  if (!(set_Objects().is_object(obj_id)))
     { cerr << obj_id << " is not a valid Object ID" << endl ;
       return *(new AString("not an Object")) ;
     }
  return get_object(obj_id).get_type() ;
}

int NEPTUNE::has_type (const char *const type, const Object &obj)
{ const NEPTUNE::Object_ID& obj_id = obj.id() ;
  if (!(set_Objects().is_object(obj_id)))
     { cerr << obj_id << " is not a valid Object ID" << endl ;
       return 0 ;
     }
  return (NEPTUNE::AString(type) == get_type(obj));
}

int NEPTUNE::has_sub_type (const char *const type, 
                           const Object &obj)
{ const NEPTUNE::Object_ID& obj_id=obj.id();
  if (!(set_Objects().is_object(obj_id)))
     {
       cerr << obj_id << " is not a valid Object ID" << endl ;
       return 0;
     }
  if (has_type(type, obj))
     return True ;
  else
     return get_object(obj_id).has_type(type) ;
}

int NEPTUNE::resize_obj (const Object& obj, int sz)
{
  const Object_ID& obj_id=obj.id();
  if (!(set_Objects().is_object(obj_id)))
     { cerr << obj_id << " is not a valid Object ID" << endl ;
       return 0 ;
     }
  if (has_sub_type("ArrOfInt", obj))
     { ArrOfInt &arr = (ArrOfInt&) get_object(obj_id) ;
       arr.resize(sz) ;
       return sz ;
     }
  else if (has_sub_type("ArrOfDouble", obj))
     { ArrOfDouble &arr = (ArrOfDouble&) get_object(obj_id) ;
       arr.resize(sz) ;
       return sz ;
     }
  else if (has_sub_type("Strings", obj))
     { Strings &arr = (Strings&) get_object(obj_id) ;
       arr.resize(sz) ;
       return sz ;
     }
  else
     { cerr << get_type(obj) << "doesn't support resize" << endl ;
       return 0 ;
     }
}

void NEPTUNE::Language_init()
{ cout.sync_with_stdio() ;
  cerr.sync_with_stdio() ;
  cin.sync_with_stdio()  ;
}

void NEPTUNE::Language_finalize()
{ cerr << "finalize"  << endl ;
}

int NEPTUNE::self_test()
{ cerr << "self_test"  << endl ;
  NEPTUNE::ArrOfDouble xd(10) ;
  xd.set_value_at(0, 0) ;
  xd.get_value_at(0)    ;
  NEPTUNE::ArrOfInt xi(10)   ;
  NEPTUNE::Strings xs(10)    ;
  NEPTUNE::AString str("10") ;
  return 1 ;
}

NEPTUNE::Object::Object(const char* const str)
  : ptr(((UObject&)(get_Types().create_Object(str))).get_key())
{
}

NEPTUNE::Object::Object(const UObject *ptr)
  : ptr(ptr->get_key())
{
}

NEPTUNE::Object::Object()
  : ptr(((UObject&)(get_Types().create_Object(""))).get_key())
{}

NEPTUNE::Object::~Object()
{ // cerr << "Destruction d'un Object" << endl;
  delete &(set_object(ptr)) ;
}

const NEPTUNE::Object_ID& NEPTUNE::Object::id() const
{ return ptr ;
}
