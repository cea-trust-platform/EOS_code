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



#ifndef USERSFUNCTIONI_HH
#define USERSFUNCTIONI_HH 1

// #include "UsersFunction.hxx"
#include <string.h>
static NEPTUNE::AString arg1;
static NEPTUNE::AString arg2;
static const char* coma="(,)";
static const char* coma2=",";
static inline void cutarg(const NEPTUNE::AString& arg)
{
  NEPTUNE::AString cpy(arg);
  char* str=(cpy.aschar());
  char* ptr=strpbrk(str,coma2);
  if(ptr==0)
    {
      arg1=arg;
      arg2="_";
      return;
    }
  ptr=str;
  assert(*ptr=='(');
  int cpt=0;
  do{
    if( *(ptr=strpbrk(ptr+1,coma))=='(')
      ++cpt;
    else if ( *ptr == ')')
      --cpt;
    else
      assert(*ptr == ',');
  }
  while(cpt>0);
  if( (*ptr==')') && *(ptr+1)=='\0')
    {
      arg1=arg;
      arg2="_";
      return;
    }
  else
    {
      if (*ptr==')') ptr++;
      if((*ptr != ','))
	{
	  std::cerr << "At position " << (ptr-str) << " in " << str << " : "
		    << *ptr << " instead of ," << std::endl;
	  assert(0);
	}
      int m=ptr-str;
      int n=strlen(arg.aschar())-m;
      char * newstr=new char[m+2];
      strncpy(newstr, str, m);
      newstr[m]=')';
      newstr[m+1]='\0';
      arg1=newstr;
      delete[] newstr;
      newstr=new char[n+1];
      newstr[0]='(';  
      strncpy(newstr+1, ptr+1, n-1);
      newstr[n]='\0';
      arg2=newstr;
      delete[] newstr;
      std::cerr << arg1 << "," << arg2 << std::endl;
    }
}
namespace FUNCTION
{
  inline  Arg::
  Arg(const NEPTUNE::AString& arg, Function* mo)
    : Function(mo)						
  {
    operation_id=ARG;
    cutarg(arg);
    nb=1;
    NEPTUNE::AString local_arg2=arg2;
    left=new Function(arg1, this);
    if(local_arg2!="_")
      {
	right=new Arg(local_arg2, this);
	nb+=static_cast<Arg*>(right)->nb;
      }
    else
      right=0;
  }
  inline  Arg::
  ~Arg()
  {
  }
  inline double Arg::arg(int i) const
  {
    if(i==0)
      return left->get_value();
    else
      return static_cast<Arg*>(right)->arg(--i);
  }
  inline Arg::
  Arg()
  {
    assert(0);
  }

  inline std::ostream& Arg::
  print_On (std::ostream& stream) const
  {
    if(nb!=1)
      {
	stream << (*left) << "," ;
	assert(right!=0);
	stream << (*right) << std::flush;
      }
    else
      {
	stream << (*left)<< std::flush;
	assert(right==0);
      }
    return stream;
  }   
  inline UsersFunction::
  UsersFunction(const NEPTUNE::AString& Aname, const NEPTUNE::AString& arg, Function* mo)
    : Function(mo), name(Aname)						
  {	
    std::cerr << name << "("<<arg<<")"<<std::endl;
    f=get(name);
    left=new Arg(arg, this);
    assert(static_cast<Arg&> (*left).nb
	   <=
	   f->get_main_variables().size());
    operation_id=COMP;
  }

  inline UsersFunction::
  ~UsersFunction()
  {
  }
  inline double UsersFunction::
  compute_value() const
  {
    double x=static_cast<Arg&>(*left).arg(0);
    return f->get_value(x);
  }
  inline std::ostream& UsersFunction::
  print_On (std::ostream& stream) const
  {
    return stream << name << "(" << (*left) << ")" 
		  <<std::endl;
  }    
  inline NEPTUNE::AString UsersFunction::
  derivative(int i) const
  {
    if(f->get_main_variables()==get_main_variables())
      {
	deriv_str="(";
	Function ff(left->asString(), get_main_variables());
	deriv_str+=(ff.derivative(i));
	deriv_str+=")*(";
	deriv_str+=f->derivative(i);
	deriv_str+=")";
      }
    if(f->get_main_variables().size()!=1)
      {
	std::cerr <<  "SORRY NOT YET IMPLEMENTED";
	return "0";
      }
    //   if(!deriv_str.aschar())
    {
      deriv_str="(";
      Function ff(left->asString(), get_main_variables());
      deriv_str+=(ff.derivative(i));
      deriv_str+=")*(";
      NEPTUNE::AString oldvarname=f->get_main_variables()[0];
      NEPTUNE::AString str_left=left->asString();
      int n=strlen(str_left.aschar());
      str_left.remove(n-1);
      str_left.remove(0);
      f->get_main_variables()[0]=str_left;
      std::cerr << "str_left = " << str_left << std::endl;
      deriv_str+=f->derivative(0);
      deriv_str+=")";
      f->get_main_variables()[0]=oldvarname;
    }
    return deriv_str;
  }    
}     
#endif
