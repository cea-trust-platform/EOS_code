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



#ifndef FUNCTION_I_HH
#define FUNCTION_I_HH 1
#include <string.h>

extern int understand(NEPTUNE::AString& string);

namespace FUNCTION
{
  inline Function::Function()
    : operation_id(ERR), left(0), right(0), mother(0),
      status(0), exported(0)
    {
    }

  inline Function::Function(Function* mo)
    : operation_id(ERR), left(0), right(0), mother(mo),
      status(0), exported(0)
    {
    }

  inline Function::Function(const AString& str, Function* mo)
    : operation_id(ERR), left(0), right(0), mother(mo), status(0), exported(0)
    { parse(str) ;
    }

  inline Function::Function(const Function& )
  { assert(0) ;
  }

  inline Function::~Function()
  { if (left)     delete left  ;
    if (right)    delete right ;
    if (exported) suppr(this)  ;
  }

  inline int Function::cut(char* str, char* ptr)
  { NEPTUNE::AString fin(ptr+1) ;
    int n = (ptr-str);
    char*newptr = new char[n+1] ;
    strncpy(newptr, str, n) ;
    newptr[n] = '\0';
    NEPTUNE::AString debut(newptr) ;
    delete[] newptr ;
    //std::cerr << "cutting in " << debut << " and " << fin << std::endl;
    left  = new Function(debut, this) ;
    right = new Function(fin, this)   ;
    return 1 ;
  }

  inline int Function::cut_group(char* str)
  { int nmax = strlen(str) ;
    int n = 1 ;
    char* ptr = str+1 ;
    int i = 1 ;
    while( (n>0) && (i<nmax))
       { if (*ptr == ')') n-- ;
         if (*ptr == '(') n++ ;
         i++ ;
         ptr++ ;
       }
    ptr-- ;
    assert(*ptr ==  ')') ;
    if (*(ptr+1)=='\0')
       {
         int m = ptr-str;
         char * newstr=new char[m] ;
         strncpy(newstr, str+1, m-1) ;
         newstr[m-1] = '\0';
         //std::cerr << "OK " << newstr  << std::endl;
         ptr++ ;
         parse(newstr) ;
         delete[] newstr ;
       }
    else
       { ptr++ ;
         switch (*ptr)
            { case '(' :
              case ')' :
                 std::cerr << str << std::endl ;
                 std::cerr << ptr << std::endl ;
                 assert(0) ;
                 break ;
              case '+' :
                 { operation_id = ADD ;
                   //            std::cerr << "ADD" << std::endl;
                   cut(str, ptr) ;
                   break ;
                 }
              case '-' :
                 { operation_id = SUBSTRACT ;
                   //            std::cerr << "SUBSTRACT" << std::endl;
                   cut(str, ptr) ;
                   break ;
                 }
              case '*' :
                 { operation_id = MULTIPLY ;
                   //            std::cerr << "MULTIPLY" << std::endl;
                   cut(str, ptr) ;
                   break ;
                 }
              case '/' :
                 { operation_id = DIVIDE ;
                   //            std::cerr << "DIVIDE" << std::endl;
                   cut(str, ptr) ;
                   break ;
                 }
              case '^' :
                 { operation_id = POWER ;
                   //            std::cerr << "POWER" << std::endl;
                   cut(str, ptr) ;
                   break ;
                 }
              case '<' :
                 { operation_id = LT ;
                   //            std::cerr << "LT" << std::endl;
                   cut(str, ptr) ;
                   break ;
                 }
              case '>' :
                 { operation_id = GT ;
                   //            std::cerr << "GT" << std::endl;
                   cut(str, ptr) ;
                   break ;
                 }
            }
       }
    return 1 ;
  }

  inline Function::Function(int n, Function* mo)
    : operation_id(FINAL), main_variables(n), left(0), right(0), 
      mother(mo), status(0)
    {
    }

  inline double Function::get_value() const
  { status = 0 ;
    if (status) 
       { return value ;
         std::cerr << "already computed " << std::endl ;
       }
    value = compute_value() ;
    status = 1 ;
    return value ;
  }

  inline double Function::get_value(double x) const
  { if (is_root())
       { if (x != var_values[0])
            { status = 0 ;
              var_values[0] = x ;
            }
         else
            return value ;
       }
    value = compute_value();
    return value ;
  }

  inline double Function::get_value(double x, double y ) const
  { if (is_root())
       {
         if ( (x != var_values[0]) || (y != var_values[1]) )
            {
              status = 0 ;
              var_values[0] = x ;
              var_values[1] = y ;
            }
         else
            return value ;
       }
    value = compute_value() ;
    return value ;
  }

  inline double Function::get_value(double x, double y, double z) const
  { if (is_root())
       { if ( (x != var_values[0]) || (y != var_values[1]) || (z != var_values[2]))
            { status = 0 ;
              var_values[0] = x ;
              var_values[1] = y ;
              var_values[2] = z ;
            }
         else
            return value ;
       }
    value = compute_value() ;
    return value ;
  }

  inline double Function::get_value(double x, double y, double z, double t) const
  { if (is_root())
       {
         if (    (x != var_values[0]) || (y != var_values[1]) 
              || (z != var_values[2]) || (t != var_values[3]))
            { status = 0 ;
              var_values[0] = x ;
              var_values[1] = y ;
              var_values[2] = z ;
              var_values[3] = t ;
            }
         else
            return value ;
       }
    value = compute_value() ;
    return value ;
  }

  inline int Function::is_root() const
  { return mother == 0 ;
  }

  inline double Function::get_var_values(int i) const
  { if (is_root())
       return var_values[i] ;
    else
       return mother->get_var_values(i) ;
  }

  inline  int Function::Imleft() const
  { assert(!(is_root()));
    return (this == (mother->left)) ;
  }

  inline  int Function::Imright() const
  { assert(!(is_root()));
    return (this == (mother->right)) ;
  }
}
#endif
