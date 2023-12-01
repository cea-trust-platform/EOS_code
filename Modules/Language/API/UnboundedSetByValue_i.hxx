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



#ifndef UNBOUNDEDSETBYVALUE_I_HXX
#define UNBOUNDEDSETBYVALUE_I_HXX

// #include "Language/API/UnboundedSetByValue.hxx"

namespace LANGUAGE_KERNEL
{
  template <class T>
  inline T& UnboundedSetByValue<T>::operator[](const int index) const
  { return (T&) get_object(index) ;
  }

  template <class T>
  inline ostream & 
  operator<<(ostream &stream,const UnboundedSetByValue<T> &right)
  { for(int i=0; i<right.size(); i++)
       stream<<right.get_object(i) << endl ;
    return stream ;
  }

  template <class T>
  inline istream & 
  operator>>(istream &stream,UnboundedSetByValue<T> &object)
  { for(int i=0; i<object.size(); i++)
       stream>>object.set_object(i) ;
    return stream;
  }

  template <class T>
  inline UnboundedSetByValue<T>::UnboundedSetByValue()
  {
  }

  template <class T>
  inline UnboundedSetByValue<T>::UnboundedSetByValue(int sz)
    :Objects_ptr(sz)
  {
  }

  template <class T>
  inline UnboundedSetByValue<T>::~UnboundedSetByValue()
  {
  }

  template <class T>
  inline UnboundedSetByValue<T> & UnboundedSetByValue<T>::operator=(const UnboundedSetByValue<T> &right)
  { Objects_ptr::operator=(right) ;
    return *this ;
  }

  template <class T>
  inline int UnboundedSetByValue<T>::operator==(const UnboundedSetByValue<T> &right) const
  { assert(0) ;
    return 0 ;
  }

  template <class T>
  inline int UnboundedSetByValue<T>::operator!=(const UnboundedSetByValue<T> &right) const
  { assert(0) ;
    return 1 ;
  }
}

#endif
