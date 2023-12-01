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



#ifndef ARROFDOUBLE_I_HXX
#define ARROFDOUBLE_I_HXX

namespace NEPTUNE
{
  inline ArrOfDouble::ArrOfDouble (int nsz, const double &x)
    : LANGUAGE_KERNEL::ArrOf<double>(nsz, x)
  {
  }

  inline ArrOfDouble::ArrOfDouble (int nsz, const double *ptr)
    : LANGUAGE_KERNEL::ArrOf<double>(nsz, ptr)
  {
  }

  inline ArrOfDouble& ArrOfDouble::operator=(const ArrOfDouble& x)
  { LANGUAGE_KERNEL::ArrOf<double>::operator=(x) ;
    return *this ;
  }

  inline ArrOfDouble& ArrOfDouble::operator=(const double& x)
  { LANGUAGE_KERNEL::ArrOf<double>::operator=(x) ;
    return *this ;
  }

  inline double& ArrOfDouble::operator[](int i)
  { return LANGUAGE_KERNEL::ArrOf<double>::operator[](i) ;
  }

  inline const double& ArrOfDouble::operator[](int i) const
  { return LANGUAGE_KERNEL::ArrOf<double>::operator[](i) ;
  }

  inline double& ArrOfDouble::operator()(int i)
  { return LANGUAGE_KERNEL::ArrOf<double>::operator()(i) ;
  }

  inline const double& ArrOfDouble::operator()(int i) const
  { return LANGUAGE_KERNEL::ArrOf<double>::operator()(i) ;
  }

  inline ArrOfDouble& ArrOfDouble::operator+=(const ArrOfDouble &x)
  { LANGUAGE_KERNEL::ArrOf<double>::operator+=(x) ;
    return *this ;
  }

  inline ArrOfDouble& ArrOfDouble::operator+=(const double &x)
  { LANGUAGE_KERNEL::ArrOf<double>::operator+=(x) ;
    return *this ;
  }

  inline ArrOfDouble& ArrOfDouble::operator-=(const ArrOfDouble &x)
  { LANGUAGE_KERNEL::ArrOf<double>::operator-=(x) ;
    return *this ;
  }

  inline ArrOfDouble& ArrOfDouble::operator-=(const double &x)
  { LANGUAGE_KERNEL::ArrOf<double>::operator-=(x) ;
    return *this ;
  }

  inline ArrOfDouble& ArrOfDouble::operator *= (const double &x)
  { LANGUAGE_KERNEL::ArrOf<double>::operator*=(x) ;
    return *this ;
  }

  inline ArrOfDouble& ArrOfDouble::operator /= (const double& x)
  { LANGUAGE_KERNEL::ArrOf<double>::operator/=(x) ;
    return *this ;
  }

  inline  double ArrOfDouble::get_value_at(int index) const
  { return (*this)[index] ;
  }

  inline void ArrOfDouble::set_value_at(int index, const double &x)
  { (*this)[index] = x ;
  }

}
#endif
