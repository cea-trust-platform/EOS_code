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



#ifndef UNBOUNDEDSETBYVALUE_HXX
#define UNBOUNDEDSETBYVALUE_HXX

// Objects_ptr
#include "Language/API/Objects_ptr.hxx"

using  OBJECTSHANDLING::Objects_ptr;

namespace LANGUAGE_KERNEL
{
  template <class T>
  class UnboundedSetByValue : public Objects_ptr  
  {

  public:
    UnboundedSetByValue();
    UnboundedSetByValue(int sz);

    virtual ~UnboundedSetByValue();

    UnboundedSetByValue< T > & operator=(const UnboundedSetByValue< T > &right);

    int operator==(const UnboundedSetByValue< T > &right) const;

    int operator!=(const UnboundedSetByValue< T > &right) const;

    T& operator[](const int index) const;

    template <class X>
    friend ostream & 
    operator<<(ostream &stream,const UnboundedSetByValue< X > &right);

    template <class X>
    friend istream & 
    operator>>(istream &stream,UnboundedSetByValue< X > &object);

  protected:

  private:

  };
}

#include "Language/API/UnboundedSetByValue_i.hxx"
#endif
