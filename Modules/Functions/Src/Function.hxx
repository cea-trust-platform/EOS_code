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



#ifndef FUNCTION_HH
#define FUNCTION_HH 1

#include "Language/API/Language.hxx"

using namespace NEPTUNE ;

enum operation
  { ERR       = 'e' ,
    FINAL     = 'f' ,
    VAR       = 'v' ,
    ADD       = '+' ,
    SUBSTRACT = '-' ,
    DIVIDE    = '/' ,
    MULTIPLY  = '*' ,
    POWER     = '^' ,
    LT        = '<' ,
    GT        = '>' ,
    LE        = '[' ,
    GE        = ']' ,
    MOD       = '%' ,
    COMP      = 'o' ,
    USERS     = 'u' ,
    ARG       = 'a'
  };

namespace FUNCTION
{
  class Function 
  {
  public :
    Function();
    virtual ~Function();
    Function(const AString& str, const AString& var);
    Function(const AString& str, const Strings& var);
    Function(const AString& name, const AString& str, const AString& var);
    Function(const AString& name, const AString& str, const Strings& var);
    Function(const AString& str, Function* mo);
    Function(Function* mo);
    const AString& get_name() const;
    AString& set_name(const AString& str) const;
    double get_value() const;
    double get_value(double x) const;
    double get_value(double x, double y) const;
    double get_value(double x, double y, double z) const;
    double get_value(double x, double y, double z, double t) const;
    int Variable_id(const char* const str) const;
    static std::ostream& 
    print_All_Known_Functions(std::ostream& stream=std::cout);
    virtual std::ostream& print_On (std::ostream& stream=std::cout) const;
    virtual std::istream& read_On (std::istream& stream=std::cin);
    friend std::ostream & 
    operator<<(std::ostream &stream,const Function &right);
    friend std::istream & 
    operator>>(std::istream &stream,Function &object);
    double get_var_values(int) const;
    virtual AString asString() const;
    virtual AString derivative(int) const;
    const Strings& get_main_variables() const;
  protected :
    int is_root() const;
    Function(int n, Function* mo);
    void parse(const AString& str);
    virtual double compute_value() const;
    operation operation_id;
    Strings main_variables;
    mutable ArrOfDouble var_values;
    Function *left  ;
    Function *right ;
    Function *mother;
    mutable double value;
    mutable int status;
    int var_id ;
    int cut(char* str, char* ptr);
    int cut_group(char* str) ;
    int parse_var(char* str) ;
    int simplify() ;
    void rec_simplify() ;
    int Imleft() const ;
    int Imright() const;
    static Function** all_functions;
    static Strings all_functions_names;
    static int nb_Functions;
    Function* get(const AString&);
  private :
    static void add(Function*, AString);
    static void suppr(Function*);
    Function(const Function &);
    int test() const;
    void put_left();
    int exported;
  };
}
#include "Function_i.hxx"
#endif
