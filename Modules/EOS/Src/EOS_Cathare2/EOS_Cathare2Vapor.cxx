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



#include "EOS_Cathare2Vapor.hxx"

using namespace NEPTUNE ;

namespace NEPTUNE_EOS
{
  const AString EOS_Cathare2Vapor::phasename("Vapor") ;

  static RegisteredClass& Cathare2Vapor_create()
  { return *(new EOS_Cathare2Vapor()) ;
  }

  EOS_Cathare2Vapor::EOS_Cathare2Vapor() : EOS_Cathare2(CATHARE2::vapor)
  {}

  EOS_Cathare2Vapor::~EOS_Cathare2Vapor()
  {
    //this->EOS_Cathare2::~EOS_Cathare2();
  }

  int EOS_Cathare2Vapor::type_Id = (RegisterType ("EOS_Cathare2Vapor",
                                                  "EOS_Fluid",
                                                  "Cathare2 Vapor Functions",
                                                  sizeof(EOS_Cathare2Vapor),
                                                  Cathare2Vapor_create)) ;

  const Type_Info& EOS_Cathare2Vapor::get_Type_Info () const
  { return (Types_Info::instance())[type_Id];
  }

  const AString& EOS_Cathare2Vapor::phase_name() const
  { return phasename ;
  }

  EOS_Error EOS_Cathare2Vapor::calc2_mixing(const int n, const EOS_Fields& in, EOS_Fields& out, EOS_Error_Field& ferr)
  { // set_mixing_properties has already been done for (r,cp0,cp1,...)
    assert (pilot) ;
    assert ((n>0) && (n<5)) ;
    EOS_Error err ;
    int np = n+1 ;
    int p = -1, t = -1, h = -1, c[5] ;
    for (int i=0; i<5; i++)  c[i] = -1 ;
    for (int i=0; i< in.size(); i++) 
       { if      (in[i].get_property() == NEPTUNE::p) p = i ;
         else if (in[i].get_property() == NEPTUNE::T) t = i ;
         else if (in[i].get_property() == NEPTUNE::h) h = i ;
         else if (in[i].get_camix_property() == NEPTUNE::c_0) c[0] = i ;
         else if (in[i].get_camix_property() == NEPTUNE::c_1) c[1] = i ;
         else if (in[i].get_camix_property() == NEPTUNE::c_2) c[2] = i ;
         else if (in[i].get_camix_property() == NEPTUNE::c_3) c[3] = i ;
         else if (in[i].get_camix_property() == NEPTUNE::c_4) c[4] = i ;
       }
    EOS_Fields C(np) ;  // proportions of vapor and incondensables gases
    for (int i=0; i<np; i++) 
       { if (c[i] == -1) 
            { ferr = EOS_Internal_Error::NOT_IMPLEMENTED ;
              return EOS_Error::error ;
            }
         else 
            C[i] = in[c[i]] ;
       }
    // compute f(p,xi)
    if ( (p > -1) && (h == -1) && (t == -1) ) 
       { err = pilot->set_mixing_properties(C) ;
         if (err == EOS_Error::error)  return EOS_Error::error ;
         return pilot->calc2_p(in[p], out, ferr) ;
       }
    // compute f(p,h,xi)
    else if ( (p > -1) && (h > -1) ) 
      { err = pilot->set_mixing_properties(C) ;
         if (err == EOS_Error::error)  return EOS_Error::error ;
         return pilot->calc2_ph(in[p], in[h], out, ferr) ;
      }
    // compute f(p,T,xi)
    else if ((p > -1) && (t > -1)) 
       { // check if h is in the EOS_Fields "out"
         int nscai = in[0].size() ;
         ArrOfDouble xhi(nscai) ;
         EOS_Field hi("hi","h",xhi) ;
         int nout=out.size() ;
         EOS_Fields outi(nout-1) ;
         int ih =  0 ;
         int ii = -1 ;
         for (int i=0; i<nout; i++) 
            { if ( out[i].get_property() == NEPTUNE::h) 
                 { ih = 1 ;
                   hi = out[i] ;
                 }
              else
                 { ii++ ;
                   if (ii<nout-1) outi[ii] = out[i] ;
                 }
            }
         // h(p,t,xi) is computed by a Newton, point by point
         EOS_Fields Ci(np) ; // proportions of vapor and incondensables gases with size=1
         double xci[5] = {0.e0, 0.e0, 0.e0, 0.e0, 0.e0} ;
         for (int i=0; i<np; i++)
            Ci[i] = EOS_Field("Ci", C[i].get_property_name().aschar(), 1, &xci[i]) ;
         
         for (int j=0; j<nscai; j++)
            { for (int i=0; i<np; i++)
                 xci[i] = C[i][j] ;
              err = pilot->set_mixing_properties(Ci) ;
              if (err == EOS_Error::error)  return EOS_Error::error ;
              ferr.set(j,pilot->calc2_h_pT_mixing(in[p][j], in[t][j],hi[j])) ;
            }
         ArrOfInt xerrph(nscai) ;
         EOS_Error_Field errph(xerrph) ;

         err = pilot->set_mixing_properties(C) ;
         if (err == EOS_Error::error)  return EOS_Error::error ;
         if (ih == 0) 
              pilot->calc2_ph(in[p], hi, out, errph) ;
         else if ( nout >= 2 ) 
              pilot->calc2_ph(in[p], hi, outi, errph) ;

         ferr.set_worst_error(errph) ;
         return ferr.find_worst_error().generic_error() ;
       }
    else 
       { ferr = EOS_Internal_Error::NOT_IMPLEMENTED ;
         return EOS_Error::error ;
       }
  }
}
