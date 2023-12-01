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



#ifndef PREDEFFUNCTION_HH
#define PREDEFFUNCTION_HH 1

#include "Function.hxx"

#define definepredef(_TYPE_)				\
  class _TYPE_ : public Function			\
  {							\
  public :						\
    _TYPE_();						\
    _TYPE_(const NEPTUNE::AString&, Function* mo=0);	\
    virtual double compute_value() const;		\
    double compute_value(double x) const;		\
    virtual std::ostream&				\
      print_On (std::ostream& stream=std::cout) const;	\
    virtual std::istream&				\
      read_On (std::istream& stream=std::cin);		\
    virtual NEPTUNE::AString derivative(int) const;	\
  private :						\
    mutable double old_var;				\
  };

namespace FUNCTION
{
  definepredef(SIN)
    definepredef(COS)
    definepredef(TAN)
    definepredef(LN)
    definepredef(EXP)
    definepredef(SQRT)
    definepredef(INT)
    definepredef(ERF)
    definepredef(RND)
    definepredef(COSH)
    definepredef(SINH)
    definepredef(TANH)

    enum Function_ID
  {
    ISIN=0,
    ICOS=1,
    ITAN=2,
    ILN=3,
    IEXP=4,
    ISQRT=5,
    IINT=6,
    IERF=7,
    IRND=8,
    ICOSH=9,
    ISINH=10,
    ITANH=11,
    IERR=-1
  };
}
#include "PredefinedFunctions_i.hxx"
#endif
