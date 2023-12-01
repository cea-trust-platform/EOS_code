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



#ifndef ARROFDOUBLE_HXX
#define ARROFDOUBLE_HXX

// UObject
#include "Language/API/UObject.hxx"
#include "Language/API/ArrOfT.hxx"

using LANGUAGE_KERNEL::UObject;

namespace NEPTUNE
{
  class ArrOfDouble : public UObject , public LANGUAGE_KERNEL::ArrOf<double>
  {

  public:

    ArrOfDouble (const ArrOfDouble &right) ;
    ArrOfDouble (int nsz=0, const double& x=0) ;
    ArrOfDouble (int nsz, const double* ptr) ;

    virtual ~ArrOfDouble();

    ArrOfDouble & operator=(const ArrOfDouble &right);
    ArrOfDouble & operator=(const double& );
    ArrOfDouble & operator+=(const double& );
    ArrOfDouble & operator+=(const ArrOfDouble& );
    ArrOfDouble & operator-=(const double& );
    ArrOfDouble & operator-=(const ArrOfDouble& );
    ArrOfDouble & operator/=(const double& );
    ArrOfDouble & operator*=(const double& );

    const double& operator[](int index) const;
    double& operator[](int index);
    const double& operator()(int index) const;
    double& operator()(int index);
    double get_value_at(int index) const;
    void set_value_at(int index, const double&) ;

    friend ostream & 
    operator<<(ostream &stream,const ArrOfDouble &right);

    friend istream & 
    operator>>(istream &stream,ArrOfDouble &object);

    virtual const Type_Info& get_Type_Info () const;

    virtual ostream& print_On (ostream& stream=cout) const;

    virtual istream& read_On (istream& stream=cin);

  private: 
    static int type_Id ;
  };
}// namespace

#include "Language/API/ArrOfDouble_i.hxx"
#endif

