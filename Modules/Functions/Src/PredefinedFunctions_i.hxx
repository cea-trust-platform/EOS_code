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



#ifndef PREDEFFUNCTION_I_HH
#define PREDEFFUNCTION_I_HH 1

#include <random>

#define implemente(_TYPE_, STR_TYPE_)                                   \
  inline _TYPE_::_TYPE_(const NEPTUNE::AString& str, Function* mo)      \
    : Function(mo), old_var(1)                                          \
  {                                                                     \
    operation_id=COMP;                                                  \
    left= new Function(str, this);                                      \
    compute_value(0);                                                   \
    assert(right==0);                                                   \
  }                                                                     \
  inline _TYPE_::_TYPE_()                                               \
    : Function(), old_var(1)                                            \
  {                                                                     \
    operation_id=COMP;                                                  \
    compute_value(0);                                                   \
  }                                                                     \
  inline std::ostream& _TYPE_::print_On(std::ostream &stream) const {   \
    stream << STR_TYPE_;                                                \
    assert(right==0);                                                   \
    if(left) return stream << "(" << (*left) << ")";                    \
    else return stream<<"(x)";                                          \
  }                                                                     \
  inline std::istream& _TYPE_::read_On(std::istream &stream)  {         \
    return stream ;                                                     \
  }


static inline double f(const FUNCTION::Function* root, 
                       const FUNCTION::Function* ptr, 
                       int id)
{
  if(ptr) return ptr->get_value();
  return root->get_var_values(id);
}
#define XXX f(this, left, var_id)

namespace FUNCTION
{
  implemente(SIN, "SIN");

  inline double SIN::compute_value() const
  { double x = XXX ;
    if (x == old_var)  return value ;
    old_var = x ;
    return sin(x) ;
  }

  inline double SIN::compute_value(double x) const
  { old_var = x ;
    return value = sin(x) ;
  }

  inline NEPTUNE::AString SIN::derivative(int i) const
  { NEPTUNE::AString fp = "(" ;
    fp += left->derivative(i) ;
    fp += ")*COS" ;
    fp += left->asString() ;
    return fp ;
  }                            

  implemente(COS, "COS") ;;

  inline double COS::compute_value() const
  { double x = XXX ;
    if (x == old_var)  return value ;
    old_var = x ;
    return cos(x) ;
  }

  inline double COS::compute_value(double x) const
  { old_var = x ;
    return value = cos(x) ;
  }

  inline NEPTUNE::AString COS::derivative(int i) const
  { NEPTUNE::AString fp = "-1*(" ;
    fp += left->derivative(i) ;
    fp += ")*SIN" ;
    fp += left->asString() ;
    return fp ;
  }                          

  implemente(TAN, "TAN") ;;

  inline double TAN::compute_value() const
  { double x = XXX ;
    if (x == old_var)  return value ;
    old_var = x ;
    return tan(x) ;
  }

  inline double TAN::compute_value(double x) const
  { old_var = x ;
    return value = tan(x) ;
  }

  inline NEPTUNE::AString TAN::derivative(int i) const
  { NEPTUNE::AString fp = "(" ;
    fp += left->derivative(i) ;
    fp += ")/(COS" ;
    fp += left->asString() ;
    fp += ")^2" ;
    return fp ;
  }     

  implemente(LN, "LN") ;;

  inline double LN::compute_value() const
  { double x=XXX;
    if (x == old_var)  return value;
    old_var=x;
    return log(x);
  }

  inline double LN::compute_value(double x) const
  { old_var = x ;
    return value = log(x) ;
  }

  inline NEPTUNE::AString LN::derivative(int i) const
  { NEPTUNE::AString fp = "(" ;
    fp += left->derivative(i) ;
    fp += ")/(" ;
    fp += left->asString() ;
    fp += ")" ;
    return fp ;
  }     

  implemente(EXP,"EXP");;

  inline double EXP::compute_value() const
  { double x = XXX ;
    if (x==old_var)  return value ;
    old_var = x ;
    return exp(x) ;
  }

  inline double EXP::compute_value(double x) const
  { old_var = x ;
    return value = exp(x) ;
  }

  inline NEPTUNE::AString EXP::derivative(int i) const
  { NEPTUNE::AString fp = "(" ;
    fp += left->derivative(i) ;
    fp += ")*EXP(" ;
    fp += left->asString() ;
    fp += ")" ;
    return fp ;
  }     

  implemente(SQRT, "SQRT");;

  inline double SQRT::compute_value() const
  { double x = XXX ;
    if (x == old_var)  return value ;
    old_var = x ;
    return sqrt(x) ;
  }
  
  inline double SQRT::compute_value(double x) const
  { old_var = x ;
    return value = sqrt(x) ;
  }

  inline NEPTUNE::AString SQRT::derivative(int i) const
  { NEPTUNE::AString fp = "1/2*(" ;
    fp += left->derivative(i) ;
    fp += ")/(" ;
    fp += left->asString() ;
    fp += ")" ;
    return fp ;
  }     

  implemente(INT, "INT") ;;

  inline double INT::
  compute_value() const
  { double x = XXX ;
    if (x == old_var)  return value ;
    old_var = x ;
    return int(x)  ;
  }

  inline double INT::compute_value(double x) const
  { old_var = x ;
    return value = int(x) ;
  }

  inline NEPTUNE::AString INT::derivative(int i) const
  { (void)i ;
    NEPTUNE::AString fp = "0" ;
    return fp ;
  }     

  implemente(ERF, "ERF");;

  inline double ERF::compute_value() const
  { double x = XXX ;
    if (x == old_var)  return value ;
    old_var = x ;
    return erf(x) ;
  }

  inline double ERF::compute_value(double x) const
  { old_var = x ;
    return value = erf(x) ;
  }

  inline NEPTUNE::AString ERF::derivative(int i) const
  { (void)i ;
    NEPTUNE::AString fp = "0" ;
    assert(0) ;
    return fp ;
  }   

  implemente(RND, "RND");;

  inline double RND::compute_value() const
  {
    std::uniform_real_distribution<double> unif(0.,1.);
    std::default_random_engine re;
    return XXX*unif(re);
  }
  inline double RND::compute_value(double x) const
  {
    old_var = x;
    std::uniform_real_distribution<double> unif(0.,1.);
    std::default_random_engine re;
    return value = x*unif(re) ;
  }
  inline NEPTUNE::AString RND::derivative(int i) const
  { (void)i ;
    NEPTUNE::AString fp = "0" ;
    assert(0) ;
    return fp ;
  }   

  implemente(COSH, "COSH") ;;

  inline double COSH::compute_value() const
  { double x = XXX ;
    if (x == old_var)  return value ;
    old_var = x ;
    return cosh(x) ;
  }

  inline double COSH::compute_value(double x) const
  { old_var = x ;
    return value = cosh(x) ;
  }

  inline NEPTUNE::AString COSH::derivative(int i) const
  { (void)i ;
    NEPTUNE::AString fp = "0" ;
    assert(0) ;
    return fp ;
  }  

  implemente(SINH, "SINH") ;;

  inline double SINH::compute_value() const
  { double x = XXX;
    if (x == old_var)  return value ;
    old_var = x ;
    return sinh(x) ;
  }

  inline double SINH::compute_value(double x) const
  { old_var = x ;
    return value=sinh(x) ;
  }

  inline NEPTUNE::AString SINH::derivative(int i) const
  { (void)i ;
    NEPTUNE::AString fp = "0" ;
    assert(0) ;
    return fp ;
  }  

  implemente(TANH, "TANH");;

  inline double TANH::compute_value() const
  { double x = XXX ;
    if (x == old_var)  return value ;
    old_var = x ;
    return tanh(x) ;
  }

  inline double TANH::compute_value(double x) const
  { old_var=x;
    return value=tanh(x);
  }

  inline NEPTUNE::AString TANH::derivative(int i) const
  { (void)i ;
    NEPTUNE::AString fp = "0" ;
    assert(0) ;
    return fp ;
  } 
}
#endif
