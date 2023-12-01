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



#ifndef ARROFINT_HXX
#define ARROFINT_HXX

#include "Language/API/UObject.hxx"
#include "Language/API/ArrOfT.hxx"


using LANGUAGE_KERNEL::UObject;


namespace NEPTUNE
{
  class ArrOfInt : public UObject  , public LANGUAGE_KERNEL::ArrOf<int>
  {

  public:

    ArrOfInt (const ArrOfInt &right);
    ArrOfInt (int nsz=0, const int& x=0);
    ArrOfInt (int nsz, const int* ptr);

    virtual ~ArrOfInt();

    ArrOfInt & operator=(const ArrOfInt &right);
    ArrOfInt & operator=(const int& );
    ArrOfInt & operator+=(const int& );
    ArrOfInt & operator+=(const ArrOfInt& );
    ArrOfInt & operator-=(const int& );
    ArrOfInt & operator-=(const ArrOfInt& );
    ArrOfInt & operator/=(const int& );
    ArrOfInt & operator*=(const int& );

    const int& operator[](int index) const;
    int& operator[](int index);
    const int& operator()(int index) const;
    int& operator()(int index);
    int get_value_at(int index) const;
    void set_value_at(int index, const int& x) ;

    friend ostream & 
    operator<<(ostream &stream,const ArrOfInt &right);

    friend istream & 
    operator>>(istream &stream,ArrOfInt &object);

    virtual const Type_Info& get_Type_Info () const;

    virtual ostream& print_On (ostream& stream=cout) const;

    virtual istream& read_On (istream& stream=cin);

  private: 
    static int type_Id;
  };
} // namespace

#include "Language/API/ArrOfInt_i.hxx"
#endif
