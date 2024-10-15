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





#ifndef Objects_h
#define Objects_h 1

#include "Language/API/Boolean.hxx"

#ifdef _OPENMP
#include <mutex>
#endif
  
using std::ostream;
using std::istream;
namespace OBJECTSHANDLING
{
  class NumberedObject;
}

namespace NEPTUNE
{
  class Object_ID;
}
namespace OBJECTSHANDLING
{
  class Objects 
  {
    
  public:
    Objects(const Objects &right);
    
    virtual ~Objects();
    
    Objects & operator=(const Objects &right);
    
    int operator==(const Objects &right) const;
    
    int operator!=(const Objects &right) const;

    friend ostream & 
    operator<<(ostream &stream,const Objects &right);
    
    friend istream & 
    operator>>(istream &stream,Objects &object);
    
    NEPTUNE::Boolean is_object(const NEPTUNE::Object_ID& key);
    
    int add_object (NumberedObject* obj);
    
    NEPTUNE::Boolean delete_object (const NEPTUNE::Object_ID& obj);
    
    const NumberedObject& get_object (int key);
    
    NumberedObject& set_object (int key) const;
    
    static Objects& instance ();
    
    void destroy_all ();
    
  protected:
    
  private:
    Objects();
    
  private: 
    static Objects* singleton;
    NumberedObject** the_objects;
    int nb_obj;
    int nb_obj_max;
    int step;
    int* next;
    int free;

#ifdef _OPENMP
    std::mutex mutex_objects;
#endif
  };
}


#endif
