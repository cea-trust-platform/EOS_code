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



#include "PredefinedFunctions.hxx"
#include "UsersFunction.hxx"
#include <sstream>
#include <string.h>
#define __BLOCSZ__ 512
static int bloc_sz=__BLOCSZ__;
static NEPTUNE::ArrOfInt sep_index(__BLOCSZ__);
FUNCTION::Function** FUNCTION::Function::all_functions=new Function*[__BLOCSZ__];
NEPTUNE::Strings FUNCTION::Function::all_functions_names(__BLOCSZ__);
static NEPTUNE::ArrOfInt next_free(__BLOCSZ__);
#undef __BLOCSZ__

int FUNCTION::Function::nb_Functions = 0 ;
static int ifree = 0 ;
static int init  = 0 ;

static int nb_sep = 0 ;
static const char* separateur  = "()+-*/^<>%"  ;
static const char* separateur2 = "()+-*/^<>%," ;

using namespace FUNCTION ;
static Function* ptr2del = 0 ;

static inline Function_ID val(const NEPTUNE::AString& str)
{ char c = ((NEPTUNE::AString&)(str)).aschar()[0] ;
  switch(c)
    { case 'C' :
         { if (str == "COS")
              return ICOS;
           else if (str == "COSH")
              return ICOSH;
           break;
         }
      case 'S' :
         { if (str == "SIN")
              return ISIN;
           else if (str == "SINH")
              return ISINH;
           else if (str == "SQRT")
              return ISQRT;
           break;
         }
      case 'T' :
         { if(str == "TAN")
             return ITAN;
           else if (str == "TANH")
             return ITANH;
           break;
         }
      case 'L' :
         { if (str == "LN")  return ILN;
           break;
         }
      case 'E' :
         { if (str == "EXP")
              return IEXP;
           else if (str == "ERF")
             return IERF;
           break;
         }
      case 'I' :
         { if (str == "INT")  return IINT ;
           break;
         }
      case 'R' :
         { if (str == "RND")  return IRND;
           break;
         }
      default :
         { return IERR ;
         }
    }
  return IERR ;
}

static inline void clean_ptr2del()
{ if (ptr2del)  delete ptr2del ;
  ptr2del = 0 ;
}

static int is_sep(char c)
{ int n = strlen(separateur) ;
  for(int i=0; i<n; i++)
    if (separateur[i] == c)  return 1 ;
  return 0;
}

static int get_sep(const NEPTUNE::AString& string)
{ nb_sep = 0 ;
  sep_index = 0 ;
  NEPTUNE::AString string2=string;
  char* str=string2.aschar();
  char *ptr=strpbrk(str,separateur2);
  while (ptr!=0)
    { sep_index[nb_sep++]=(ptr-str);
      if(nb_sep==sep_index.size())
        sep_index.resize(sep_index.size()+bloc_sz);
      ptr=strpbrk(ptr+1, separateur2);
    }
  return 1;
}

static int get_left_expr(const NEPTUNE::AString& string, int i)
{
  if (i == 0)  return 0;
  if (string[sep_index[i]-1] == ')')
     { i--;
       int n=1;
       while( (n>0) && (i>0) )
          { i--;
            if( string[sep_index[i]]=='(' )
              n--;
            if( string[sep_index[i]]==')' )
              n++;
          }
       if (n>0)
          { std::cerr << "Too many ) " << std::endl;
            return -1;
          }
       else
          { if (i == 0)
              return 0;
            else
              return sep_index[i-1]+1;
          }
     }
  else
    return sep_index[i-1]+1;
}

static int get_right_expr(const NEPTUNE::AString& string, int i, int j=0)
{
  if(i == (nb_sep-1))  return strlen(string.aschar());
  char c=string[sep_index[i+1]];
  if ((c == '(')|| (j &&c != ')'))
     { i++ ;
       int n = 1 ;
       if (c == '(') n += j ;
       while( (n>0) && (i<nb_sep) )
          { i++ ;
            if ( string[sep_index[i]] == '(' )  n++ ;
            if ( string[sep_index[i]] == ')' )  n-- ;
          }
       if (n > 0)
          { if (j)
               return strlen(string.aschar()) ;
            else
               std::cerr << "Too many ( " << std::endl;
            return -1 ;
          }
       else
         return sep_index[i] ;
     }
  else
     { //       std::cerr << string[sep_index[i+1]] << "!= '('" << std::endl;
       return sep_index[i+1];
     }
}

static void clear(NEPTUNE::AString& string)
{
  { static int ok=0;
    if (!ok)
       { ok=1;
         NEPTUNE::AString x("(x)");
         clear(x);
         assert(x=="(x)");
       }
  }
  get_sep(string) ;
  int i ;
  char c = '(' ;
  int n=strlen(string.aschar())-1;
  if (    (nb_sep == 2) 
       && ((string[0] == c) && (string[n] == ')')) ) return ;
  for(i=0; i<nb_sep; i++)
     { if (string[sep_index[i]]==c)
          { int ileft  = get_left_expr(string, i) ;
            int iright = get_right_expr(string, i, 1) ;
            if (string[ileft] == c)
               { if ((ileft == 0)||(iright == n))
                    {   
                    }
                 else if (   (string[ileft-1] == c)
                          && (string[iright+1] == ')') )
                    { if ( (ileft==1) || is_sep(string[ileft-2]))
                         { string.remove(iright) ;
                           string.remove(ileft) ;  
                           get_sep(string);
                           n -= 2 ;
                           i-- ;     
                         } 
                    }
               }
          }
     }
}

static int understand_c(NEPTUNE::AString& string, char c)
{ get_sep(string) ;
  int i ;
  for(i=0; i<nb_sep; i++)
     {
       //       std::cerr << "sep_index = " << sep_index << std::endl;
       //       std::cerr << "string = " << string << std::endl;
       if (string[sep_index[i]]==c)
          { int ileft = get_left_expr(string, i);
            int iright ;
            if (c == '(')
               iright = get_right_expr(string, i, 1) ;
            else
               iright = get_right_expr(string, i) ;
            if ( (ileft==0) ||
                 ( ( (string[ileft-1]!='(') && (string[ileft-1]!=',') )|| 
                   (string[iright+1]!=')'))
                 )
              { if (string[ileft]==',')  ileft++;
                string.insert('(',ileft);
                string.insert(')',iright+1);
                get_sep(string); i++;
              }
          }
       //       else std::cerr << str[sep_index[i]] << " ignored" << std::endl;
     }
  return 1 ;
}
int understand(NEPTUNE::AString& string)
{ std:: cerr << "understanding : " << string << std::endl;
  understand_c(string, '-');
  understand_c(string,'(');
  understand_c(string, '^');
  understand_c(string, '/');
  understand_c(string, '*');
  std:: cerr << "understood : " << string << std::endl;
  clear(string);
  std:: cerr << "cleared as : " << string << std::endl;
  return 1;
}

namespace FUNCTION
{
  void Function::add(Function* f, NEPTUNE::AString f_name)
  { if (!init)
       { init=1;
         for(int i=0; i<bloc_sz; i++)
           {
             next_free[i]=i+1;
             all_functions[i]=0;
           }
       }
    std :: cout << "adding " << f_name
                << " = " << f->asString()
                << std::endl;
    all_functions[ifree]=f;
    all_functions_names[ifree]=f_name;
    ifree=next_free[ifree];
    nb_Functions++;
    int sz=all_functions_names.size();
    if(nb_Functions==sz)
      {
        all_functions_names.resize(sz+bloc_sz);
        next_free.resize(sz+bloc_sz);
        int i;
        for(i=sz; i<sz+bloc_sz; i++)    
          next_free[i]=i+1;
        Function** old=all_functions;
        all_functions=new Function*[bloc_sz];
        for(i=0; i<sz; i++)
          all_functions[i]=old[i];
      }
  }

  void Function::suppr(Function* f)
  {
    int sz=all_functions_names.size();
    for(int i=0; i<sz; i++)
      if(f==all_functions[i])
        {
          std::cout << "forgetting " << i << std::endl;
          nb_Functions--;
          all_functions[i]=0;
          all_functions_names[i]="";
          int j=next_free[ifree];
          next_free[ifree]=i;
          next_free[i]=j;
          i=sz;
        }
  }

  std::ostream& Function::print_All_Known_Functions(std::ostream& stream)
  {
    int sz=all_functions_names.size();
    int cpt=0;
    for(int i=0; (i<sz && cpt<nb_Functions); i++)
      if(all_functions[i]!=0)
        {
          stream<<i << " , " << all_functions_names[i]
                << " : " << (all_functions[i]->asString())
                << std::endl;
          cpt++;
        }
    return stream;
  }


  Function* Function::get(const NEPTUNE::AString& name)
  {
    int sz=all_functions_names.size();
    int cpt=0;
    for(int i=0; (i<sz && cpt<nb_Functions); i++)
      if(all_functions[i]!=0)
        if(all_functions_names[i]== name)
          return all_functions[i];
    std::cerr << "name is not a known function!" << std::endl
              << "known functions are : "<< std::endl;
    print_All_Known_Functions(std::cerr);
    assert(0);
    return 0;
  }

  void Function::parse(const NEPTUNE::AString& string)
  {
    //   std::cerr << "Parsing " << string << "...\n";
    NEPTUNE::AString cpy(string);
    char* str=(cpy.aschar());
    char* ptr=strpbrk(str,separateur);
    if (!ptr)
       { switch (str[0])
           { case '\0' :
               std::cerr << "EOS" << std::endl ;
               // fall through
             case '0' :
             case '1' :
             case '2' :
             case '3' :
             case '4' :
             case '5' :
             case '6' :
             case '7' :
             case '8' :
             case '9' :
               { //NOMBRE      std::cerr<<"NOMBRE" << std::endl;
                 value=strtod(str,NULL);
                 status=1;
                 operation_id=FINAL;
                 break;
               }
             default :
               { if( (var_id=Variable_id(str)) !=-1 )
                   { operation_id=VAR;
                   }
                 else
                   { std::cerr << "ERROR" << std::endl;
                     std::cerr << str << "!=" 
                               << get_main_variables()[0]
                               << std::endl;
                     assert(0);
                   }
                 //            std::cerr<<"VAR" << std::endl;
                 break;
               }
           }
       }

    else
       { char sep=*ptr;
         switch(sep)
           { case '(' :
               { if (ptr==str)
                    cut_group(str);
                 else
                   { operation_id=COMP;
                     //            std::cerr << "COMP" << std::endl;
                     NEPTUNE::AString fin(ptr);
                     int n=(ptr-str);
                     char* newptr=new char[n+1];
                     strncpy(newptr, str, n);
                     newptr[n]='\0';
                     NEPTUNE::AString debut(newptr);
                     delete[] newptr;
                     switch(val(debut))
                       { case ICOS :
                           { left=new COS(fin, this);
                             break;
                           }
                         case ISIN :
                           { left=new SIN(fin, this);
                             break;
                           }
                         case IEXP :
                           { left=new EXP(fin, this);
                             break;
                           }
                         case ITAN :
                           { left=new TAN(fin, this);
                             break;
                           }
                         case ILN :
                           { left=new LN(fin, this);
                             break;
                           }
                         case IERF :
                           { left=new ERF(fin, this);
                             break;
                           }
                         case ISQRT :
                           { left=new SQRT(fin, this);
                             break;
                           }
                         case IINT :
                           { left=new INT(fin, this);
                             break;
                           }
                         case ICOSH :
                           { left=new COSH(fin, this);
                             break;
                           }
                         case ISINH :
                           { left=new SINH(fin, this);
                             break;
                           }
                         case ITANH :
                           { left=new TANH(fin, this);
                             break;
                           }
                         default :
                           { left=new UsersFunction(debut, fin, this);
                             break;
                           }
                       }
                   }
                 break;
               }
             case ')' :
               { assert(0);
                 break;
               }
             case '+' :
               { operation_id=ADD;
                 //            std::cerr << "ADD" << std::endl;
                 cut(str, ptr);
                 break;
               }
             case '-' :
               { operation_id=SUBSTRACT;
                 //            std::cerr << "SUBSTRACT" << std::endl;
                 cut(str, ptr);
                 break;
               }
             case '*' :
               { operation_id=MULTIPLY;
                 //            std::cerr << "MULTIPLY" << std::endl;
                 cut(str, ptr);
                 break;
               }
             case '/' :
               { operation_id=DIVIDE;
                 //            std::cerr << "DIVIDE" << std::endl;
                 cut(str, ptr);
                 break;
               }
             case '^' :
               { operation_id=POWER;
                 //            std::cerr << "POW" << std::endl;
                 cut(str, ptr);
                 break;
               }
             case '<' :
               { operation_id=LT;
                 //            std::cerr << "<" << std::endl;
                 cut(str, ptr);
                 break;
               }
             case '>' :
               { operation_id=GT;
                 //            std::cerr << ">" << std::endl;
                 cut(str, ptr);
                 break;
               }
           }
       }
    if (is_root())  assert(test());
  }

  void Function::rec_simplify()
  {
    static int test = 0 ;
    {
      if (!test)
        { test=1;
          Function ft("(((SIN((x)))*(1))+((0)+((COS((x)))*(-1))))", "x");
          assert(ft.asString()=="((SIN((x)))+((COS((x)))*(-1)))");
        }
    }
    if ((operation_id==VAR)||(operation_id==FINAL))
      return;
    int nb=3;
    while(nb!=1)
      { int ok=1;
        int left_ok=1, right_ok=1;
        nb=3;
        while((nb--)&&(ok))
          {
            ok=0;
            if(/*left_ok && */left)
              {
                left_ok=(left->simplify());
                clean_ptr2del();
                ok=left_ok;
              }
            if(/*right_ok && */right)
              {
                right_ok=(right->simplify());
                clean_ptr2del();
                ok+=right_ok;
              }
          }
      }
    if (is_root())
       while(simplify() && ptr2del == 0) ;
    else
       { if (left)
            left->rec_simplify();
         else if (right)
            right->rec_simplify();
       }
  }

  int Function::simplify()
  {
    {
      static int ok = 0 ;
      if (!ok)
         { ok = 1 ;
           Function test("((0)*(2))","x");
           test.simplify();
           clean_ptr2del();
           assert(test.asString()=="(0)");
         }
    }
    //   std::cerr << "Function::simplify" << asString() << std::endl;
    assert(test()) ;
    switch(operation_id)
      { case VAR :
        case FINAL :
          break;
        case COMP :
          left->rec_simplify();
          break;
        case ADD :
          { left->rec_simplify();
            if (right)  right->rec_simplify();
            if (operation_id != ADD)  return 1 ;
            assert(test());
            NEPTUNE::AString f_left  = left->asString()  ;
            NEPTUNE::AString f_right = right->asString() ;
            if ( f_left == f_right )
               { // f+f=2*f
                 operation_id = MULTIPLY ;
                 right->operation_id = FINAL ;
                 right->value=2;
                 if(right->left)   delete right->left  ;
                 if(right->right)  delete right->right ;
                 right->left = right->right = 0 ;
                 return 1;
               }
            if (     (left->operation_id  == FINAL)
                  && (right->operation_id == FINAL) )
               { // a+b=(a+b)
                 operation_id=FINAL;
                 value=left->value+right->value;
                 delete left; left=0;
                 delete right; right=0;
                 assert(test());
                 return 1;
               }
            if (left->operation_id == FINAL)
               { Function* old_left=left;
                 left=right;
                 right=old_left;
               }
            if (right->operation_id == FINAL)
               { if (right->value == 0)
                    { //f+0=f
                      if (is_root())
                         { operation_id=COMP;
                           delete right;
                           right=0;
                           return 1;
                         }
                      delete right ; right = 0 ;
                      put_left() ;
                      return 1 ;
                    }
               }
            else if(left->operation_id == ADD)
              { //(f+g)+a=f+(g+a)
                Function* oldleft=left;
                Function* oldright=right;
                left=left->left;
                left->mother=this;
                right=oldleft;
                right->left=oldleft->right;
                right->left->mother=right;
                right->right=oldright;
                right->right->mother=right;
                return 1 ;
              }
            break;
          }
        case SUBSTRACT :
          { left->rec_simplify() ;
            if (right)  right->rec_simplify() ;
            if (operation_id!= SUBSTRACT) return 1 ;
            NEPTUNE::AString f_left=left->asString() ;
            NEPTUNE::AString f_right=right->asString() ;
            if ( f_left == f_right )
               { // f-f=0
                 operation_id = FINAL ;
                 value = 0 ;
                 delete left  ; left  = 0 ;
                 delete right ; right = 0 ;
                 return 1 ;
               }
            if ( (left->operation_id == FINAL) &&
                 (right->operation_id == FINAL) )
               { // a-b=(a-b)
                 operation_id=FINAL;
                 value=left->value-right->value;
                 delete left;  left  = 0 ;
                 delete right; right = 0 ;
                 return 1;
               }
            if (right->operation_id == FINAL)
               { if (right->value == 0)
                    { //f-0=f
                      if (is_root())
                         { operation_id = COMP;
                           delete right ;
                           right = 0 ;
                           return 1;
                         }
                      delete right ; right = 0 ;
                      put_left() ;
                      return 1 ;
                    }
               }
            break ;
          }
        case DIVIDE :
          { left->rec_simplify();
            if (right)  right->rec_simplify();
            if (operation_id != DIVIDE )  return 1;
            NEPTUNE::AString f_left  = left->asString()  ;
            NEPTUNE::AString f_right = right->asString() ;
            if ( f_left==f_right )
               { // f/f=1
                 operation_id=FINAL;
                 value=1;
                 delete left; left=0;
                 delete right; right=0;
                 return 1;
               }
            if (    (left->operation_id  == FINAL) 
                 && (right->operation_id == FINAL) )
               { operation_id=FINAL;
                 value=left->value/right->value;
                 delete left ; left  = 0 ;
                 delete right; right = 0 ;
                 assert(test());
                 return 1;
               }
            if ( (left->operation_id == FINAL) && (left->value == 0))
               {//0/f=0
                 operation_id=FINAL;
                 value=0;
                 delete left; left   = 0 ;
                 delete right; right = 0 ;
                 assert(test()) ;
                 return 1 ;
               }
            if ( (right->operation_id == FINAL) && (right->value == 1))
               {//f/1=f
                 if (is_root())
                    { operation_id = COMP;
                      delete right ;
                      right = 0;
                      return 1;
                    }
                 delete right; right = 0;
                 return 1;
               }
            break;
          }
        case MULTIPLY :
          { left->rec_simplify() ;
            if (right)  right->rec_simplify() ;
            if (operation_id!=MULTIPLY )  return 1 ;
            NEPTUNE::AString f_left=left->asString();
            NEPTUNE::AString f_right=right->asString();
            if ( f_left == f_right )
               { //f*f=f^2
                 operation_id = POWER ;
                 right->operation_id = FINAL ;
                 right->value=2;
                 if (right->left)   delete right->left  ;
                 if (right->right)  delete right->right ;
                 right->left = right->right = 0;
                 return 1;
               }
            else if (left->operation_id == POWER )
              { //f^p*f=f^(p+1)
                if ( (left->left->asString()) == f_right)
                   { operation_id = POWER ;
                     Function* fp = left  ;
                     left  = right ;
                     right = fp ;
                     right->operation_id = ADD ;
                     Function* f = right->left;
                     right->left = fp->right;
                     delete f;
                     f = 0 ;
                     fp = 0 ;
                     right->right = new Function("1", right) ;
                     left->rec_simplify() ;
                     right->rec_simplify() ;
                   }
                else if(right->operation_id == POWER )
                   { // (f^a)*(f^b)=f^(a+b)
                     if ( (left->left->asString()) == (right->left->asString()))
                        { operation_id=POWER;
                          Function* oldleft  = left  ;
                          Function* oldright = right ;
                          left = oldleft->left;
                          left->mother = this ;
                          right->operation_id = ADD ;
                          delete oldright->left ;
                          oldright->left = 0;
                          right->left = oldleft->right ;
                          right->left->mother = right ;
                          left->rec_simplify()  ;
                          right->rec_simplify() ;
                          return 1;
                        }
                   }
              }
            else if (right->operation_id==POWER )
              { Function* tmp = left ;
                left  = right ;
                right = tmp ;
                //f*f^p=f^(p+1)
                if ( (left->left->asString()) == f_left)
                   { operation_id = POWER ;
                     Function* fp = left ;
                     left  = right ;
                     right = fp ;
                     right->operation_id = ADD;
                     Function* f = right->left;
                     right->left = fp->right;
                     delete f ;
                     f  = 0 ;
                     fp = 0 ;
                     right->right = new Function("1", right) ;
                     left->rec_simplify()  ;
                     right->rec_simplify() ;
                   }
              }
            if ( (left->operation_id == FINAL) &&
                 (right->operation_id == FINAL) )
               { //a*b=(a*b)
                 operation_id = FINAL ;
                 value=left->value*right->value ;
                 delete left  ; left  = 0 ;
                 delete right ; right = 0 ;
                 assert(test()) ;
                 return 1 ;
               }
            if (left->operation_id == FINAL)
               { Function* old_left = left ;
                 left  = right    ;
                 right = old_left ;
               }
            if (right->operation_id == FINAL)
               { if (right->value == 0)
                    { //f*0=0
                      operation_id = FINAL ;
                      value = 0 ;
                      delete left  ; left  = 0 ;
                      delete right ; right = 0 ;
                      assert(test()) ;
                      return 1 ;
                    }
                 else if (right->value == 1)
                    { //f*1=f
                      if (is_root())
                         { operation_id = COMP ;
                           delete right ;
                           right = 0 ;
                           return 1 ;
                         }
                      delete right ; right = 0 ;
                      put_left() ;
                      return 1 ;
                    }
                 else if(left->operation_id == MULTIPLY)
                    { //(f*g)*a=f*(g*a)
                      Function* oldleft  = left  ;
                      Function* oldright = right ;
                      left = left->left ;
                      left->mother = this;
                      right = oldleft;
                      right->left = oldleft->right;
                      right->left->mother = right;
                      right->right = oldright;
                      right->right->mother = right;
                      return 1;
                    }
               }
            break ;
          }
        case POWER :
          { left->rec_simplify() ;
            if (right)  right->rec_simplify() ;
            if (operation_id!=POWER ) return 1;
            if (    (left->operation_id  == FINAL) 
                 && (right->operation_id == FINAL) )
               { //a^b=(a^b)
                 operation_id = FINAL ;
                 value=pow(left->value, right->value);
                 delete left  ; left  = 0 ;
                 delete right ; right = 0 ;
                 assert(test()) ;
                 return 1 ;
               }
            if (right->operation_id == FINAL)
               { if (right->value == 0)
                   { //f^0=1
                     operation_id = FINAL ;
                     value = 1 ;
                     delete left  ; left  = 0 ;
                     delete right ; right = 0 ;
                     assert(test()) ;
                     return 1 ;
                   }
                 else if (right->value == 1)
                   { if (is_root())
                        { operation_id = COMP ;
                          delete right ;
                          right = 0 ;
                          return 1 ;
                        }
                     //f^1=f
                     delete right; right=0;
                     put_left();
                     return 1;
                   }
               }
            if (left->operation_id == POWER)
               { // (f^a)^b = f^(a*b)
                 Function *f = left->left  ;
                 Function *a = left->right ;
                 Function *b = right ;
                 right = left ;
                 left = f ;
                 f->mother = this;
                 right->operation_id = MULTIPLY ;
                 a->mother = right ;
                 b->mother = right ;
                 right->left  = a ;
                 right->right = b ;
                 left->rec_simplify()  ;
                 right->rec_simplify() ;
                 return 1 ;
               }
            break ;
          }
        case LT :
          { left->rec_simplify() ;
            if (right)  right->rec_simplify() ;
            break ;
          }
        case GT :
          { left->rec_simplify() ;
            if (right)  right->rec_simplify() ;
            break ;
          }
        case LE :
          { left->rec_simplify() ;
            if (right)  right->rec_simplify() ;
            break ;
          }
        case GE :
          { left->rec_simplify() ;
            if (right)  right->rec_simplify() ;
            break ;
          }
        case MOD :
          { left->rec_simplify() ;
            if (right)  right->rec_simplify() ;
            break ;
          }
        case ARG :
          break  ;
        case ERR :
        default :
          assert(0) ;
      }
    return 0 ;
  }

  double  Function::compute_value() const
  {
    assert(test());
    switch(operation_id)
      { case VAR :
          { return get_var_values(var_id);
          }
        case FINAL :
          { return value;
          }
        case COMP :
          { return left->get_value();
          }
        case ADD :
          { return left->get_value()+right->get_value();
          }
        case SUBSTRACT :
          { return left->get_value()-right->get_value();
          }
        case DIVIDE :
          { return left->get_value()/right->get_value();
          }
        case MULTIPLY :
          { return left->get_value()*right->get_value();
          }
        case POWER :
          { return pow(left->get_value(),(right->get_value()));
          }
        case LT :
          { return left->get_value()<right->get_value();
          }
        case GT :
          { return left->get_value()>right->get_value();
          }
        case LE :
          { return left->get_value()<=right->get_value();
          }
        case GE :
          { return left->get_value()>=right->get_value();
          }
        case MOD :
          { return int (left->get_value()) % int (right->get_value());
          }
        case ARG :
          break;
        case ERR :
        default :
          assert(0);
      }
    return 0;
  }
  int Function::Variable_id(const char* const str) const
  { if (str[0]=='(')
       { NEPTUNE::AString tmp(str);
         int n=strlen(str);
         tmp.remove(n);
         tmp.remove(0);
         return Variable_id(tmp.aschar());
       }
    return get_main_variables().search(str);
  }


  const NEPTUNE::Strings& Function::get_main_variables() const
  { if (is_root())
       return main_variables;
    else
       return mother->get_main_variables();
  }

  std::ostream & 
  operator<<(std::ostream &stream, const Function &right)
  { return right.print_On(stream);
  }

  std::ostream& Function::print_On (std::ostream& stream) const
  { assert(test());
    stream << "(";
    switch(operation_id)
      { case VAR :
          stream<< get_main_variables()[var_id];
          break;
        case FINAL :
          stream<< value;
          break;
        case COMP :
          stream << (*left);
          break;
        case ADD :
          stream <<  (*left)<<"+"<<(*right);
          break;
        case SUBSTRACT :
          stream <<  (*left)<<"-"<<(*right);
          break;
        case DIVIDE :
          stream <<  (*left)<<"/"<<(*right);
          break;
        case MULTIPLY :
          stream <<  (*left)<<"*"<<(*right);
          break;
        case POWER :
          stream <<  (*left)<<"^"<<(*right);
          break;
        case LT :
          stream <<  (*left)<<"<"<<(*right);
          break;
        case GT :
          stream <<  (*left)<<">"<<(*right);
          break;
        case LE :
          stream <<  (*left)<<"<="<<(*right);
          break;
        case GE :
          stream <<  (*left)<<">="<<(*right);
          break;
        case MOD :
          stream <<  (*left)<<"%"<<(*right);
          break;
        case ARG :
          assert(0);
          break;
        case ERR :
        default :
          assert(0);
      }
    return stream << ")";
  }

  std::istream& Function::read_On (std::istream& stream)
  { return stream;
  }

  NEPTUNE::AString Function::derivative(int i) const
  { assert(test()) ;
    NEPTUNE::AString result("(") ;
    switch(operation_id)
      { case VAR :
          if (var_id!=i)  return ("(0)") ;
          return ("(1)") ;
          break ;
        case FINAL :
          return ("(0)") ;
          break ;
        case COMP :
          return left->derivative(i) ;
          break ;
        case ADD :
          result += left->derivative(i) ;
          result += ")+(" ;
          result += right->derivative(i) ;
          result += ")" ;
          return result ;
          break;
        case SUBSTRACT :
          result += left->derivative(i) ;
          result += ")-(" ;
          result += right->derivative(i) ;
          result += ")" ;
          return result ;
          break;
        case DIVIDE :
          result += "(-" ;
          result += right->derivative(i) ;
          result += ")/(" ;
          result += right->asString() ;
          result += "^2)*" ;
          result += left->asString() ;
          result += ")+(" ;
          result += left->derivative(i) ;
          result += ")/(" ;
          result += right->asString() ;
          result += ")" ;
          return result ;
          break;
        case MULTIPLY :
          result += left->derivative(i) ;
          result += ")*(" ;
          result += right->asString() ;
          result += ")+(" ;
          result += left->asString() ;
          result += ")*(" ;
          result += right->derivative(i) ;
          result += ")" ;
          return result ;
          break ;
        case POWER :
          result += right->asString() ;
          result += ")*" ;
          result += left->derivative(i) ;
          result += "*" ;
          result += left->asString() ;
          result += "^(" ;
          result += (int(right->get_value()-1)) ;
          result += ")" ;
          return result ;
          break ;
        case LT :
          return left->derivative(i) ;
          break ;
        case GT :
          return left->derivative(i) ;
          break ;
        case LE :
          return left->derivative(i) ;
          break ;
        case GE :
          return left->derivative(i) ;
          break ;
        case MOD :
          return left->derivative(i);
          break ;
        case ARG :
          break ;
        case ERR :
        default :
          assert(0) ;
      }
    return "" ;
  }

  NEPTUNE::AString Function::asString() const
  { std::stringstream ostr;
    ostr.precision(12);
    ostr << *this;
    NEPTUNE::AString result;
    ostr >> result;
    NEPTUNE::AString reste;
    while(ostr)
       { ostr >> reste;
         if(ostr)
           result+=reste;
       }
    assert(!ostr);
    std::cerr << "in asString" << std::endl;
    understand(result);
    return result;
  }


  int Function::test() const
  { assert(is_root() || Imleft() || Imright());
    switch(operation_id)
       { case VAR :
         case FINAL :
           assert(right==0);
           assert(left==0);
           break;
         case COMP :
           assert(right==0);
           assert(left!=0);
           left->test();
           break;
         case ADD :
         case SUBSTRACT :
         case DIVIDE :
         case MULTIPLY :
         case POWER :
         case LT :
         case GT :
         case LE :
         case GE :
         case MOD :
           assert(left!=0);
           assert(right!=0);
           left->test();
           right->test();
           break;
         case ARG :
           assert(left!=0);
           left->test();
           if(right)
             right->test();
           break;
         case ERR :
         default :
           assert(0);
       }
    return 1 ;
  }

  void Function::put_left()
  { assert(ptr2del==0);
    Function* old_left=left;
    left=0;
    if (Imleft())
       { ptr2del=mother->left;
         mother->left=old_left;
         mother->left->mother=mother;
       }
    else if(Imright())
      {
        ptr2del=mother->right;
        mother->right=old_left;
        mother->right->mother=mother;
      }
    else
      {
        assert(0);
      }
  }

  Function::Function(const AString& str, const AString& var)
    : operation_id(ERR), main_variables(1), left(0), right(0),
      mother(0), status(0), exported(0)
  {
    main_variables[0]=var;
    NEPTUNE::AString cpy(str);
    understand(cpy);
    parse(cpy);
    std::cerr << "function " << asString() << std::endl;
    rec_simplify();
    clean_ptr2del();
    rec_simplify();
    clean_ptr2del();
    std::cerr << str << " simplified as " << asString() << std::endl;
    var_values.resize(1);
  }
  Function::Function(const AString& name, const AString& str, const AString& var)
    : operation_id(ERR), main_variables(1), left(0), right(0),
      mother(0), status(0), exported(1)
  {
    main_variables[0]=var;
    NEPTUNE::AString cpy(str);
    understand(cpy);
    parse(cpy);
    std::cerr << "function " << asString() << std::endl;
    rec_simplify();
    clean_ptr2del();
    rec_simplify();
    clean_ptr2del();
    std::cerr << str << " simplified as " << asString() << std::endl;
    var_values.resize(1);
    add(this, name);
  }

  Function::Function(const AString& str, const Strings& var)
    : operation_id(ERR), main_variables(var), left(0), right(0),
      mother(0), status(0), exported(0)
  {
    NEPTUNE::AString cpy(str);
    understand(cpy);
    parse(cpy);
    std::cerr << "function " << asString() << std::endl;
    rec_simplify();
    clean_ptr2del();
    rec_simplify();
    clean_ptr2del();
    std::cerr << str << " simplified as " << asString() << std::endl;
    var_values.resize(var.size());
  }

  Function::Function(const AString& name, const AString& str, const Strings& var)
    : operation_id(ERR), main_variables(var), left(0), right(0),
      mother(0), status(0), exported(1)
  {
    NEPTUNE::AString cpy(str);
    understand(cpy);
    parse(cpy);
    std::cerr << "function " << asString() << std::endl;
    rec_simplify();
    clean_ptr2del();
    rec_simplify();
    clean_ptr2del();
    std::cerr << str << " simplified as " << asString() << std::endl;
    var_values.resize(var.size());
    add(this, name);
  }
}
