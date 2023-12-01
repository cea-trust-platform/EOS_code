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
// Types_Info
#include "Language/API/Types_Info.hxx"
// Type_Info
#include "TypesHandling/Type_Info.hxx"
// RegisteredClass
#include "Language/API/RegisteredClass.hxx"
namespace NEPTUNE
{
  int RegisterType(const char* type, const char* coment, unsigned int sz, 
                   RegisteredClass& (*create) (void))
  { Types_Info& les_types       = Types_Info::instance()   ;
    NEPTUNE::AString type_str   = NEPTUNE::AString(type)   ;
    NEPTUNE::AString coment_str = NEPTUNE::AString(coment) ;
    int return_value            = les_types.register_type(type_str, coment_str, sz, create) ;
    return return_value ;
  }

  int RegisterType(const char* type, const char* mother, const char* coment, 
                   unsigned int sz, RegisteredClass& (*create) (void))
  { Types_Info& les_types       = Types_Info::instance()   ;
    NEPTUNE::AString type_str   = NEPTUNE::AString(type)   ;
    NEPTUNE::AString mother_str = NEPTUNE::AString(mother) ;
    NEPTUNE::AString coment_str = NEPTUNE::AString(coment) ;
    int return_value            = les_types.register_type(type_str, mother_str, coment_str, sz, create) ;
    return return_value ;
  }
}
// Class Types_Info 

namespace TYPESHANDLING
{
  Types_Info *Types_Info::the_instance = 0 ;

  Types_Info::Types_Info(const Types_Info &right)
  { (void)right ;
  }

  Types_Info::Types_Info ()
  {
    //cerr << endl;
    //cerr << endl;
    //cerr << endl;
    //cerr << "************************************** " << endl;
    //cerr <<   "Constructing types information ... " << endl;
    //cerr << "************************************** " << endl;
  }


  Types_Info::~Types_Info()
  {
  }


  Types_Info & Types_Info::operator=(const Types_Info &right)
  { (void)right ;
    assert(0) ;
    cerr << "Types_Info::operator= should not be used ! " << endl ;
    exit(-1) ;
    return *this ;
  }


  int Types_Info::operator==(const Types_Info &right) const
  { (void)right ;
    assert(0) ;
    cerr << "Types_Info::operator== should not be used ! " << endl ;
    exit(-1) ;
    return 1 ;
  }


  int Types_Info::operator!=(const Types_Info &right) const
  { (void)right ;
    assert(0) ;
    cerr << "Types_Info::operator!= should not be used ! " << endl ;
    exit(-1) ;
    return 0 ;
  }


  ostream & operator<<(ostream &stream,const Types_Info &right)
  { (void)right ;
    stream << endl ;
    stream << " All Classes recognized by the application currently : " 
           << endl << endl ;

    // print EOS types
    Types_Info::instance().print_types() ;

    return stream << endl << endl ;
  }


  istream & operator>>(istream &stream,Types_Info &object)
  { (void) object ;
    cerr << "operator>>(istream &stream,Types_Info &object) should not be used!" << endl ;
    exit(-1) ;
    return stream ;
  }


  Types_Info& Types_Info::instance ()
  { if (the_instance == 0)  the_instance = new Types_Info() ;
    return *the_instance ;
  }


  void Types_Info::describe () const
  { cerr << *this<< endl ;
  }


  int Types_Info::register_type (const NEPTUNE::AString& type, 
                                 const NEPTUNE::AString& mother, 
                                 const NEPTUNE::AString& coment, 
                                 unsigned int sz, 
                                 RegisteredClass& (*create) (void))
  { //cerr << "Registering type " << type << endl;
    Type_Info* new_type = new Type_Info(type, coment, create, sz, mother) ;
    return the_Type_Info.add_object(*new_type) ;
  }


  int Types_Info::register_type (const NEPTUNE::AString& type, 
                                 const NEPTUNE::AString& coment, 
                                 unsigned int sz, 
                                 RegisteredClass& (*create) (void))
  { //cerr << "Registering type " << type << endl;
    Type_Info* new_type = new Type_Info(type, coment, create, sz) ;
    int return_value    = the_Type_Info.add_object(*new_type) ;
    return return_value ;
  }


  RegisteredClass& Types_Info::create_Object (NEPTUNE::AString type) const
  { return (*this)[type].instance() ;
  }


  int Types_Info::size_of_Object (const RegisteredClass& ob) const
  { return ob.get_Type_Info().get_sz() ;
  }


  const Type_Info& Types_Info::operator [] (const NEPTUNE::AString& type) const
  { // number of types
    int nbt = Types_Info::instance().nb_types() ;

    for (int i=0; i<nbt; i++)
       { const Type_Info& ti = (*this)[i] ;
         const NEPTUNE::AString& strti = ti.get_name() ;
         if (strti == type)  return ti ;
       }

    cerr << type << " is not a registered type "<< endl ;
    cerr << "The known types are : " << endl<<endl ;
    print_types() ; // print types
    throw std::runtime_error("this type is not a registered type") ;
  }


  // Print EOS types
  void  Types_Info::print_types() const
  { // number of types
    int nbt = Types_Info::instance().nb_types() ;
    //  type names array
    NEPTUNE::Strings chac(nbt) ;
    int *indx = new int[nbt] ;
    for(int i=0; i<nbt;i++)
      { const Type_Info& ti=(*this)[i] ;
        chac[i] = ti.get_name() ;
      }

    //  sort array by name type
    //  before sort : chac[i]       i = 1,nbt
    //  after  sort : chac[indx[i]] 
    ctri(nbt, chac, indx) ; 

    //  print
    cerr << std::left  << std::setw(6)  << " NUM. " << "  "
         << std::left  << std::setw(30) << "         CLASS TYPE          "
         << std::left  << std::setw(60) << "                        DEFINITION                         "
         << std::left  << std::setw(15) << " MOTHER CLASS "
         << std::right << std::setw(6)  << " DIM. " <<endl<<endl ;
    for(int i=0; i<nbt;i++)
      { const Type_Info &ti = (*this)[indx[i]];
        const NEPTUNE::AString& type_mf = ti.get_name()    ;
        const NEPTUNE::AString& defi_mf = ti.get_comment() ;
        const NEPTUNE::AString& heri_mf = ti.get_mother()  ;
        const int               sz_mf   = ti.get_sz()      ;
        cerr << std::right << std::setw(6)  << i+1 << "  "
             << std::left  << std::setw(30) << type_mf
             << std::left  << std::setw(60) << defi_mf
             << std::left  << std::setw(15) << heri_mf 
             << std::right << std::setw(6)  << sz_mf    << endl;
      }
    cerr << endl ;
    delete[] indx ;
  }


  // Sort
  void  Types_Info::ctri(const int nbt, NEPTUNE::Strings &chac, int *indx) const
  { int iflag, k ;

    int nindic = 1 ;
    indx[nindic-1] = 0 ;

    for (int i=1; i<nbt; i++)
       { iflag = 0 ;
         for (int j=0; j<nindic; j++)
            { k = indx[j] ;
              if (chac[i] < chac[k])
                 { for (int l=nindic; l>j; l--)
                      indx[l] = indx[l-1] ;
                   indx[j] = i ;
                   iflag   = 1 ;
                   break ;
                 }
            }
         if (iflag == 0) indx[nindic] = i ;
         nindic++ ;
       }
  }


  void Types_Info::destroy_all ()
  { the_Type_Info = UnboundedSetByValue<Type_Info>() ;
  }
}
