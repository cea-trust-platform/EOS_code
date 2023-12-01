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



#ifndef ArrOfInt_inlines
#define ArrOfInt_inlines 1

// Class ArrOfInt 

namespace NEPTUNE
{
  inline ArrOfInt::ArrOfInt (int nsz, const int& x)
    : LANGUAGE_KERNEL::ArrOf<int>(nsz, x)
  {
  }

  inline ArrOfInt::ArrOfInt (int nsz, const int* ptr)
    : LANGUAGE_KERNEL::ArrOf<int>(nsz, ptr)
  {
  }

  inline ArrOfInt& ArrOfInt::operator=(const ArrOfInt& x)
  { LANGUAGE_KERNEL::ArrOf<int>::operator=(x) ;
    return *this ;
  }

  inline ArrOfInt& ArrOfInt::operator=(const int& x)
  { LANGUAGE_KERNEL::ArrOf<int>::operator=(x) ;
    return *this ;
  }

  inline int& ArrOfInt::operator[](int i)
  { return LANGUAGE_KERNEL::ArrOf<int>::operator[](i) ;
  }

  inline const int& ArrOfInt::operator[](int i) const
  { return LANGUAGE_KERNEL::ArrOf<int>::operator[](i) ;
  }

  inline int& ArrOfInt::operator()(int i)
  { return LANGUAGE_KERNEL::ArrOf<int>::operator()(i) ;
  }

  inline const int& ArrOfInt::operator()(int i) const
  { return LANGUAGE_KERNEL::ArrOf<int>::operator()(i) ;
  }

  inline ArrOfInt& ArrOfInt::operator+=(const ArrOfInt& x)
  { LANGUAGE_KERNEL::ArrOf<int>::operator+=(x) ;
    return *this ;
  }

  inline ArrOfInt& ArrOfInt::operator+=(const int& x)
  { LANGUAGE_KERNEL::ArrOf<int>::operator+=(x) ;
    return *this ;
  }
  
  inline ArrOfInt& ArrOfInt::operator-=(const ArrOfInt& x)
  { LANGUAGE_KERNEL::ArrOf<int>::operator-=(x) ;
    return *this ;
  }

  inline ArrOfInt& ArrOfInt::operator-=(const int& x)
  { LANGUAGE_KERNEL::ArrOf<int>::operator-=(x) ;
    return *this ;
  }

  inline ArrOfInt& ArrOfInt::operator *= (const int& x)
  { LANGUAGE_KERNEL::ArrOf<int>::operator*=(x) ;
    return *this ;
  }

  inline ArrOfInt& ArrOfInt::operator /= (const int& x)
  { LANGUAGE_KERNEL::ArrOf<int>::operator/=(x) ;
    return *this ;
  }

  inline  int ArrOfInt::get_value_at(int index) const
  { return (*this)[index] ;
  }

  inline void ArrOfInt::set_value_at(int index, const int& x)
  { (*this)[index] = x ;
  }
}

#endif
