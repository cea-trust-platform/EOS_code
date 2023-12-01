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




#include "Functions.hxx"
#include "Functions/Src/UsersFunction.hxx"

using FUNCTION::Function;
namespace NEPTUNE
{
  AFunction::
  AFunction(const AString& str, const AString& var)
  {
    func = new Function(str, var);
  }
  AFunction::
  AFunction(const char* const str, const AString& var)
  {
    func = new Function(AString(str), var);
  }
  AFunction::
  AFunction(const char* const str, const char* const var)
  {
    func = new Function(AString(str), AString(var));
  }

  AFunction::
  AFunction(const AString& str, const Strings& var)
  {
    func = new Function(str, var);
  }
  AFunction::
  AFunction(const char* const str, const Strings& var)
  {
    func = new Function(AString(str), var);
  }

  AFunction::
  AFunction(const AString& name, const AString& str, const AString& var)
  {
    func = new Function(name, str, var);
  }
  AFunction::
  AFunction(const char* const name, const AString& str, const AString& var)
  {
    func = new Function(AString(name), str, var);
  }
  AFunction::
  AFunction(const char* const name, const char* const str, const AString& var)
  {
    func = new Function(AString(name), AString(str), var);
  }
  AFunction::
  AFunction(const char* const name, const char* const str, const char* const var)
  {
    func = new Function(AString(name), AString(str), AString(var));
  }

  AFunction::
  AFunction(const AString& name, const AString& str, const Strings& var)
  {
    func = new Function(name, str, var);
  }
  AFunction::
  AFunction(const char* const name, const AString& str, const Strings& var)
  {
    func = new Function(AString(name), str, var);
  }
  AFunction::
  AFunction(const char* const name, const char* const str, const Strings& var)
  {
    func = new Function(AString(name), AString(str), var);
  }

  AFunction::
  ~AFunction()
  {
    delete func;
  }
  double AFunction::
  get_value(double x) const
  {
    return func->get_value(x);
  }
  double AFunction::
  get_value(double x, double y) const
  {
    return func->get_value(x,y);
  }
  double AFunction::
  get_value(double x, double y, double z) const
  {
    return func->get_value(x,y,z);
  }
  double AFunction::
  get_value(double x, double y, double z, double t) const
  {
    return func->get_value(x,y,z,t);
  }
  void AFunction::
  print_All_Known_Functions()
  {
    Function::print_All_Known_Functions(std::cout);
  }
  AString AFunction::
  asString() const
  {
    return func->asString();
  }
  AString AFunction::
  derivative(int i) const
  {
    return func->derivative(i);
  }
  const Strings& AFunction::
  get_main_variables() const
  {
    return func->get_main_variables();
  }
}
