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



#include "eosihmSrc1.hxx"
#include <iostream>
#include <fstream>
#include <string>
#include "Language/API/Language.hxx"
#include "EOS/API/EOS.hxx"
#include "EOS/API/EOS_Fields.hxx"
#include "EOS/API/EOS_Error_Field.hxx"
#include "EOS/API/EOS_Std_Error_Handler.hxx"

using namespace NEPTUNE;

//using namespace std ;

// constructeur

EosIhm::EosIhm()
{ value = NULL; nbval = 0;
  info_erreur=NULL;
  the_error_field=NULL;
}

// destructeur

EosIhm::~EosIhm()
{ if (value != NULL)            {delete [] value ;}
  if (info_erreur != NULL)      {delete [] info_erreur ;}
  if (the_error_field != NULL)  {delete [] the_error_field ;}
}

void EosIhm::createValue(int nn){value = new double[nn]; nbval = nn;}

///////////////////////////////
#ifdef WITHMED
extern "C" void eos2med_(double *, double *, double *, int *, int *,
                         char *, char *, char *, char *, char *, char *,
                         char *, size_t, size_t, size_t, size_t, size_t,
                         size_t, size_t) ;
#endif
///////////////////////////////

void EosIhm::putTheArrays(double *thelist, double *thelist2, double *thelist3,
                          int lll, int lll2, int lll3, char *fluid, char *method,
                          char *ref, char *func, char *thex,char *they, char *medfile)
{ // std::cout<<"EosIhm::putTheArrays entering"         <<std::endl;
  // std::cout<<"the fluid                  :"  <<fluid <<std::endl;
  // std::cout<<"the method                 :"  <<method<<std::endl;
  // std::cout<<"the reference              :"  <<ref   <<std::endl;
  // std::cout<<"the thermodynamic quantity :"  <<func  <<std::endl;
  // std::cout<<"the x variable             :"  <<thex  <<std::endl;
  // std::cout<<"the y variable             :"  <<they  <<std::endl;
  // std::cout<<"lll="                          <<lll   <<std::endl;
  // std::cout<<"the X"                                 <<std::endl;
  // for (int i=0;i<lll;i++){
  //   std::cout << thelist[i] << " ";}
  // std::cout << "lll2="<<lll2<<std::endl;
  // std::cout << "the Y"<<std::endl;
  // for (int i=0;i<lll2;i++){
  //   std::cout << thelist2[i] << " ";}
  // dans thelist3 on balaye d'abord en X
  // std::cout << "lll3="<<lll3<<std::endl;
  // std::cout << "the Z"<<std::endl;
  // for (int i=0;i<lll3;i++){
  //   std::cout << thelist3[i] << " ";}
  
  // std::cout << "EosIhm::putTheArrays Fortran calling"<<std::endl;
  
#ifdef WITHMED
  (void)lll3 ;
  eos2med_(thelist, thelist2, thelist3, &lll, &lll2, fluid, method, ref, func,
           thex, they, medfile, (size_t)strlen(fluid), (size_t)strlen(method),
           (size_t)strlen(ref), (size_t)strlen(func), (size_t)strlen(thex), 
           (size_t) strlen(they), (size_t) strlen(medfile)) ;
#else
  std::cout << "EosIhm::putTheArrays : no med" <<std::endl;
  std::cout << " *thelist = " << *thelist << "   *thelist2 = " << *thelist2 << "   *thelist3 = " << *thelist3 << endl
            << " lll      = " << lll      << "   lll2      = " << lll2      << "   lll3      = " << lll3      << endl
            << " fluid    = " << fluid    << "   method    = " << method    << "   ref       = " << ref       << endl
            << " func     = " << func     << "   thex      = " << thex      << "   they      = " << they      << endl
            << " medfile  = " << medfile  << endl ;
#endif
  
  // std::cout << "EosIhm::putTheArrays leaving"<<std::endl;
}

////////////////////////////////

int EosIhm::getNumberOfValue(){return nbval;}

void EosIhm::initValueByStep(double a, double b, int nn)
{ int nmx = nn + 1 ;
  if (value != NULL)
     { if (nmx != nbval)
          { delete [] value ;
            value = new double[nmx] ;
            nbval = nmx ;
          }
     }
  else
     { value = new double[nmx] ;
       nbval = nmx;
     }  
  double dv = b - a ;
  double step = dv / double(nn) ;
  for (int i=0; i<nbval; i++)
     value[i] = a + (double(i)*step) ;
}


void EosIhm::getValueAsList(double * vv, int nn)
{ if (nn != nbval)  std::cout << "EosIhm::getValueAsList : array length error"<< std::endl ;
  for (int i=0; i<nbval; i++)
    vv[i] = value[i] ;
}


void EosIhm::getErrorAsList(int * vv, int nn)
{ if (nn != nbval)  std::cout << "EosIhm::getErrorAsList : array length error"<< std::endl ;
  for (int i=0; i<nbval; i++)
    vv[i] = the_error_field[i] ;
}


void EosIhm::getTheErrorI(int * iii)
{ *iii = indice_erreur ;
}


void EosIhm::getTheErrorC(char *rrr)
{ rrr[0] = '\0' ;
  if (info_erreur != NULL)  strcpy(rrr,info_erreur) ;
}


void EosIhm::errorManaging(EOS_Std_Exception &ex, EOS *myphase)
{ //std::cout<<"---on entre en erreur---"<<std::endl; 
  indice_erreur = 1 ;
  // une fois que tu arrives ici, tu n'as que deux infos disponibles: 
  // 1) Get the internal error code:(code d'erreur interne qui a déclanché l'exception) 
  EOS_Internal_Error ierr2 = ex.err_code ;
  // int ierr3 = ierr2.get_code() ;
  //std::cout<<"---code erreur---"<<ierr3<<std::endl;
  AString s ; 
  myphase->fluid().describe_error(ierr2, s) ;
  //std::cerr <<"description erreur:" << s<<std::endl; 
  int k = strlen(s.aschar());
  //std::cerr <<"longueur de la chaine:" << k<<std::endl; 
  if (info_erreur != NULL)  delete [] info_erreur ;
  info_erreur = new char[k+1] ;
  strcpy(info_erreur,s.aschar()) ;
  
  //std::cerr <<"info_erreur:" <<info_erreur <<std::endl; 
  // 2) info genre "compute_rho_pT()" 
  // const char *source_code_location = ex.code_info ; 
      //std::cout<<"---code location---"<<source_code_location<<std::endl;
}

void EosIhm::computeValue(char *meth, char *ref, char *var, char *qty, char *par2, double val2)
{ // std::cout<<"the method                 :"<<meth<<std::endl;
  // std::cout<<"the reference              :"<<ref<<std::endl;
  // std::cout<<"the variable               :"<<var<<std::endl;
  // std::cout<<"the thermodynamic quantity :"<<qty<<std::endl;
  // std::cout<<"the second parameter       :"<<par2<<std::endl;
  // std::cout<<"the second parameter value :"<<val2<<std::endl;
  if (value == NULL)
     { std::cout << "EosIhm::computeValue : no array"<< std::endl ;
       exit(0) ;
     }
  EOS_Std_Error_Handler handler ;
  handler.set_exit_on_error(EOS_Std_Error_Handler::disable_feature); // EOS will never exit()
  // configure error handler to throw an error if the function produces an EOS_Error equal to error or bad (ok and good will not throw an exception)
  handler.set_throw_on_error(bad) ;
  handler.set_dump_on_error(ok) ; // EOS will dump informations on a stream if error,bad or ok
  std::ofstream error_file ;
  error_file.open("le_fichier_des_erreurs") ;
  //  handler.set_dump_stream(cerr); // EOS will dump informations on cerr
  handler.set_dump_stream(error_file); // EOS will dump informations on error_file

  Language_init() ; 
  EOS *myphase ;
  myphase = new EOS(meth, ref) ;
  // eos provides a stack of saved error handlers which
  // is used if we want to locally change the error handler.
  // save_error_handler() must match a restore_error_handler() somewhere...
  myphase->save_error_handler() ;
  myphase->set_error_handler(handler) ;
 
  int n = nbval ;
  ArrOfDouble xp(n) ;
  for(int i=0; i<n; i++)
    xp[i] = value[i] ;
  // que faire avec la premiere chaine ? ce qu'on veut !
  EOS_Field f1("A_name",var,xp) ;
  
  ArrOfInt ierr(n) ;
  EOS_Error_Field err(ierr) ;
  
  ArrOfDouble yp(n) ;
  EOS_Field f2("A_name",qty,yp) ;    

  if (AString(par2) == AString("saturation") || AString(par2) == AString("limit"))
     { //std::cout<<"---appel compute cas saturation ou limit---"<<std::endl;
       indice_erreur = 0 ;
       try { //std::cout << "===APPEL METHODE COMPUTE DE EOS CAS SAT OU LIM==="<<std::endl;
             //std::cout <<f1<<std::endl;
             //std::cout <<f2<<std::endl;
             myphase->compute(f1, f2, err) ;
           }
       catch (EOS_Std_Exception ex) 
          { if (the_error_field != NULL)  delete [] the_error_field ;
            the_error_field = new int[n+1] ;
            for(int i=0; i<n; i++)
               the_error_field[i]=ierr.get_value_at(i) ;
            errorManaging(ex, myphase) ;
            return ;
          }
     }
  else 
     { ArrOfDouble zp(n) ;
       for(int i=0; i<n; i++)
         zp[i] = val2 ;
       EOS_Field f3("A_name",par2,zp) ;    
       //std::cout<<"<><><>appel compute cas general---"<<std::endl;
       indice_erreur = 0 ;
       try { myphase->compute(f1, f3, f2, err) ;
           }
       catch (EOS_Std_Exception ex) 
          { //cout<<err.print_On()<<std::endl;
            //cout<<ierr.print_On()<<std::endl;
            //for(int i=0; i<n; i++)
            // {std::cout<<i<<":"<<ierr.get_value_at(i)<<std::endl;}
            if (the_error_field != NULL)  delete [] the_error_field ;
            the_error_field = new int[n+1] ;
            //cout<<"===BEGIN LOOP==="<<std::endl;
            for(int i=0; i<n; i++)
              the_error_field[i] = ierr.get_value_at(i) ;
            //cout<<"===END LOOP==="<<std::endl;
            
            errorManaging(ex, myphase) ;
            return ;
          }
     }

  for(int i=0; i<n; i++)
     value[i] = f2[i] ; 
  
}
