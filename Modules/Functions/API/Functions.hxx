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



#ifndef FUNCTIONS_HH
#define FUNCTIONS_HH 1

namespace FUNCTION
{
  class Function;
}
#include "Language/API/Language.hxx"

#include <iostream>
namespace NEPTUNE
{
  class AFunction
  {
  public :
    //!AFunction("x*x", "x")
    AFunction(const AString& str, const AString& var);
    AFunction(const char* const str, const AString& var);
    AFunction(const char* const str, const char* const str2);

    //!AFunction("x*x*y", ["x", "y"])
    AFunction(const AString& str, const Strings& var);
    AFunction(const char* const str, const Strings& var);
    //!AFunction("f", "x*x", "x")
    //! f will now be understood so that AFunction("f(x*y)", ["x", "y"])
    //! is now valid
    AFunction(const AString& name, const AString& str, const AString& var);
    AFunction(const char* const name, const AString& str, const AString& var);
    AFunction(const char* const name, const char* const str, const AString& var);
    AFunction(const char* const name, const char* const str, const char* const var);
    //!AFunction("f", "x*x*y", ["x", "y"])
    //! f will now be understood so that AFunction("f(x*x,y*y)", ["x", "y"])
    //! is now valid
    AFunction(const AString& name, const AString& str, const Strings& var);
    AFunction(const char* const name, const AString& str, const Strings& var);
    AFunction(const char* const name, const char* const str, const Strings& var);
    ~AFunction();
    //! returns f(x), f(x,0), or f(x,0,0,0,...)
    double get_value(double x) const;
    //! returns f(x,y), f(x,0), or f(x,0,0,0,...)
    double get_value(double x, double y) const;
    double get_value(double x, double y, double z) const;
    double get_value(double x, double y, double z, double t) const;
    static void print_All_Known_Functions();
    AString asString() const;
    AString derivative(int) const;
    const Strings& get_main_variables() const;
  private :
    static int self_test();
    FUNCTION::Function *func;
  };
}
// M.F. #include "Functions_i.hxx"
#endif
