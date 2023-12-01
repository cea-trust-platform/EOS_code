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




#include "Language/API/Language.hxx"
#include "Language/API/Types_Info.hxx"

#include <string>
#include <math.h>
#include <assert.h>

static inline char intochar(int i)
{
  assert( (-9<=i) && (i<=9) );
  return ('0'+i);
}
static RegisteredClass& AString_create()
{
  return *(new NEPTUNE::AString());
}

namespace NEPTUNE
{
  AString::AString(const char* const a_str)
  { int sz = strlen(a_str) ;
    str = new char[sz+1] ;
    strcpy(str, a_str) ;
    str[sz] = '\0' ;
  }
  // Class AString 


  int AString::type_Id=(RegisterType("AString", 
                        "UObject",
                        "a string",
                        sizeof(AString),
                        AString_create)) ;

  AString::AString() : str(new char[1])
  { str[0] = '\0' ;
  }

  AString::AString(const AString &right)
    : UObject(), str(0)
  { str = new char[strlen(right.str)+1] ;
    strcpy(str, right.str) ;
  }


  AString::~AString()
  { delete[] str ;
  }


  int AString::operator==(const AString &right) const
  { return strcmp(str, right.str)==0 ;
  }

  int AString::operator!=(const AString &right) const
  { return strcmp(str, right.str)!=0 ;
  }


  int AString::operator<(const AString &right) const
  { return strcmp(str, right.str)<0 ;
  }

  int AString::operator>(const AString &right) const
  { return strcmp(str, right.str)>0 ;
  }

  int AString::operator<=(const AString &right) const
  { return strcmp(str, right.str)<=0 ;
  }

  int AString::operator>=(const AString &right) const
  { return strcmp(str, right.str)>=0 ;
  }


  ostream & operator<<(ostream &stream,const AString &right)
  { if (right.str)
       return stream<<right.str ;
    else
      return stream ;
  }

  istream & operator>>(istream &stream,AString &object)
  { if (object.str)  delete[] object.str ;
    unsigned int n = 1048576 ; // 1024*1024
    static char *tmp = new char[n];
    stream >> tmp ;
    if (strlen(tmp) > (n-1))
       { cerr << "String too long " << endl;
         exit(-1);
       }
    object.str = new char[strlen(tmp)+1] ;
    strcpy(object.str, tmp) ;
    return stream ;
  }

  const Type_Info& AString::get_Type_Info () const
  { return (Types_Info::instance())[type_Id];
  }

  AString::AString(int data)
  { int j=1, k, l=10, m=0 ;
    while( (data/l) != 0)
       { j++ ;
         l *= 10 ;
       }
    str = new char[++j] ;
    str[--j] = '\0' ;
    while(j--)
       { l /= 10 ;
         k = data/l ;
         str[m++] = intochar(k) ;
         data -= k*l ;
       }
  }

  AString::AString(double data)
  { AString minus("-") ;
    AString signe ;
    if (data >= 0)
       { signe = minus ;
         data  *= -1 ;
       }
    int exp = (int) log10(data) ;
    AString signe_exp ;
    if (exp < 0)  signe_exp = minus ;
    AString exponent(abs(exp)) ;
    int mantisse ;
    if (exp == 0)
       mantisse = (int) data ;
    else
       { data /= pow(10, exp) ;
         mantisse = (int) data ;
       }
    AString mant(mantisse) ;
    *this =  signe ;
    *this += (mant);
    *this += (signe_exp);
    *this += (exponent) ;
  }

  UObject& AString::copy (const UObject &right)
  { (void)right ;
    assert(0) ;
    return *this ;
  }

  AString& AString::uppercase ()
  { unsigned int i ;
    int decal = 'a'-'A' ;
    for (i=0; i<=strlen(str); i++)
       if ( (str[i]>='a') && (str[i]<='z'))  str[i] -= decal ;
    return *this ;
  }

  char* AString::aschar ()
  { return str ;
  }

  const char* AString::aschar () const
  { return str ;
  }

  AString& AString::operator += (const AString& x)
  { char* newstr = new char[strlen(str)+strlen(x.str)+1] ;
    strcpy(newstr, str)   ;
    strcat(newstr, x.str) ;
    delete[] str;
    str=newstr;
    return *this;
  }

  AString& AString::operator=(const AString &right) 
  { if (str)  delete[] str ;
    str = new char[strlen(right.str)+1] ;
    strcpy(str, right.str) ;
    return *this ;
  }

  ostream& AString::print_On (ostream& stream) const
  { return stream<<*this ;
  }

  istream& AString::read_On (istream& stream)
  { return stream>>*this ;
  }

  void AString::insert(char c, int index)
  { int n = strlen(str) ;
    char *old = str ;
    str=new char[n+2] ;
    int i = 0 ;
    for (; i<index; i++)
       str[i] = old[i];
    str[i++] = c ;
    for(; i<n+1; i++)
       str[i] = old[i-1] ;
    str[i] = '\0' ;
    delete[] old ;
  }

  void AString::remove(int index)
  { int n = strlen(str) ;
    char *old = str ;
    str = new char[n];
    int i = 0 ;
    for (; i<index; i++)
       str[i] = old[i] ;
    for (; i<n-1; i++)
       str[i] = old[i+1] ;
    str[i] = '\0' ;
    delete[] old ;
  }
}
