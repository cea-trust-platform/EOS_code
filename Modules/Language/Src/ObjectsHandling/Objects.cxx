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
#include <stdlib.h>
#include <iostream>

// AString
#include "Language/API/AString.hxx"

// NumberedObject
#include "Language/API/NumberedObject.hxx"
// Objects
#include "Language/API/Objects.hxx"
// Boolean
#include "Language/API/Boolean.hxx"

#ifdef _OPENMP
#include "omp.h"
#endif

// Class Objects 
namespace OBJECTSHANDLING
{

  Objects *Objects::singleton = 0 ;

  Objects::Objects()
    : the_objects(0)   ,
      nb_obj(0)        ,
      nb_obj_max(1024) ,
      step(1024)       ,
      next(0)          ,
      free(0)
  {
    //std::cerr << endl;
    //std::cerr << endl;
    //std::cerr << endl;
    //std::cerr << "Begining of the Simulation : " <<std::endl
    //   << "   Preparing memory ... " << std::flush;
    the_objects = new NumberedObject* [nb_obj_max] ;
    next = new int[nb_obj_max] ;
    for(int i=0; i<nb_obj_max; i++)
       { the_objects[i] = 0 ;
         next[i] = i+1 ;
       }
    //std::cerr << "OK" << endl;
    //std::cerr << endl;
    //std::cerr << endl;
  }

  Objects::Objects(const Objects &right)
    : the_objects(0)   ,
      nb_obj(0)        ,
      nb_obj_max(1024) ,
      step(1024)       ,
      next(0)          ,
      free(0)
  { (void)right ;
    std::cerr << "This should not append!!" << endl ;
    assert(0) ;
    exit(-1) ;
  }


  Objects::~Objects()
  { std::cerr << "End of the simulation : " << std::endl
              << "Cleaning up memory ... "  << std::flush ;
    for(int i=0; i<nb_obj_max; i++)
       if (the_objects[i] != 0)  delete the_objects[i] ;
    delete[] the_objects ;
    delete[] next ;
    std::cerr << "OK " << std::endl ;
  }


  Objects & Objects::operator=(const Objects &right)
  { (void)right ;
    std::cerr << "This should not append!!" << endl ;
    assert(0) ;
    exit(-1) ;
    return *this ;
  }


  NEPTUNE::Boolean Objects::is_object(const NEPTUNE::Object_ID& key)
  { if ((key<0) || (key>=nb_obj_max))  std::cerr << key << "Not allowed" << endl ;
    if (the_objects[key]) return 1 ;
    else return 0 ;
  }

  int Objects::operator==(const Objects &right) const
  { (void)right ;
    return 1 ;
  }

  int Objects::operator!=(const Objects &right) const
  { (void)right ;
    return 0;
  }


  ostream & operator<<(ostream &stream,const Objects &right)
  { stream << " List of all the " << right.nb_obj
           << " Objects involved in this simulation : " << endl ;
    for(int i=0; i<right.nb_obj_max; i++)
       { NumberedObject* objptr=right.the_objects[i] ;
         if(objptr!=0)
           stream << endl <<"-------------  Object  ------------------" << endl
                 << "\ttype : " << objptr->get_type()  << endl
                 << "\tvalue = "<< (*objptr ) << endl
                 << "\tId : " << i  << endl
                 << "\tadr : " << std::hex << objptr << std::dec  << endl
                 << endl <<"........................................." << endl;
       }
    return stream << " ------------------------------ " << endl ;
  }

  istream & operator>>(istream &stream,Objects &object)
  { (void)object ;
    std::cerr << "operator>>(istream &stream,const Objects &right)"
              << " Not Implemented Yet " << endl;
    return stream ;
  }

  int  Objects::add_object (NumberedObject *obj)
  {
#ifdef _OPENMP
  if (omp_in_parallel()) {
    mutex_objects.lock();
  }
#endif
    int return_value(free) ;
    the_objects[free] = obj ;
    free = next[free] ;
    nb_obj++ ;
    if (nb_obj == nb_obj_max)
      { int newnb_obj_max = nb_obj_max+step ;
        int *newnext = new int[newnb_obj_max] ;
        NumberedObject **newthe_objects = new NumberedObject* [newnb_obj_max] ;
        for(int i=0; i<nb_obj_max; i++)
          { newthe_objects[i] = the_objects[i] ;
            newnext[i] = next[i] ;
          }
        for(int i=nb_obj_max; i<newnb_obj_max; i++)
          { newthe_objects[i] = 0 ;
            newnext[i] = i+1;
          }
        delete[] the_objects;
        the_objects=newthe_objects;
        delete[] next ;
        next=newnext ;
        nb_obj_max = newnb_obj_max ;
      }
#ifdef _OPENMP
  if (omp_in_parallel()) {
    mutex_objects.unlock();
  }
#endif
    return return_value ;
  }

  NEPTUNE::Boolean Objects::delete_object (const NEPTUNE::Object_ID& obj)
  { 
#ifdef _OPENMP
  if (omp_in_parallel()) {
    mutex_objects.lock();
  }
#endif
    int num = obj        ;
    int tmp = next[free] ;
    next[free] = num ;
    next[num]  = tmp ;
    the_objects[num] = 0 ;
    nb_obj-- ;
#ifdef _OPENMP
  if (omp_in_parallel()) {
    mutex_objects.unlock();
  }
#endif
    return True ;
  }

  const NumberedObject& Objects::get_object (int key)
  { assert(the_objects[key]->get_key() == key) ;
    return *(the_objects[key]) ;
  }

  NumberedObject& Objects::set_object (int key) const
  { assert(the_objects[key]->get_key() == key) ;
    return *(the_objects[key]) ;
  }

  Objects& Objects::instance ()
  { if (singleton == 0)  singleton = new Objects ;
    return *singleton ;
  }

  void Objects::destroy_all ()
  { //   for(int i=0; i<nb_obj_max; i++)
    //     {
    //       NumberedObject* obj=the_objects[i];
    //       if(obj)
    //  delete_object(*(obj));
    //     }
    delete[] the_objects ;
    the_objects = 0    ;
    nb_obj_max  = 1024 ;
    step        = 1024 ;
    next        = 0    ;
    free        = 0    ;
    nb_obj      = 0    ;
  }
}
