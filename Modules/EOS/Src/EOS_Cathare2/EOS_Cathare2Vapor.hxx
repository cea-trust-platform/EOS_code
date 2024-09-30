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



#ifndef EOS_CATHARE2VAPOR_HXX
#define EOS_CATHARE2VAPOR_HXX

#include "EOS_Cathare2.hxx"

using namespace NEPTUNE ;

namespace NEPTUNE_EOS
{
  //! EOS_Cathare2Liquid : adapter to Hydcom/Eos Cathare2 subroutines, vapor phase
  class EOS_Cathare2Vapor : public EOS_Cathare2
  {
    static const AString phasename ;

  public:
    //! Main EOS_Cathare2Vapor constructor
    EOS_Cathare2Vapor();
    //! EOS_Cathare2Vapor destructor
    ~EOS_Cathare2Vapor();
    //! Type information method: see language documentation
    const Type_Info& get_Type_Info () const ;
    //! Return the phase name ("Vapor")
    virtual const AString& phase_name() const ;
    //! Set mixing properties
    EOS_Error set_mixing_properties(int, ArrOfDouble&, ArrOfDouble&, ArrOfDouble&, ArrOfDouble&, ArrOfDouble&,
             ArrOfDouble&, ArrOfDouble&, ArrOfDouble&, ArrOfDouble&, ArrOfDouble&, ArrOfDouble&, ArrOfDouble&,
             ArrOfDouble&, ArrOfDouble&, ArrOfDouble&, ArrOfDouble&) ;
    //! No mixing
    EOS_Error set_mixing_properties();
    EOS_Error calc2_mixing( int n , const EOS_Fields& in, EOS_Fields& out, EOS_Error_Field& err) ;

  private:
    //! Type identifier: see language documentation
    static int type_Id ;

  };

  inline EOS_Error EOS_Cathare2Vapor::set_mixing_properties(int nincon,
                   ArrOfDouble& r,   ArrOfDouble& cp0, ArrOfDouble& cp1, ArrOfDouble& cp2,
                   ArrOfDouble& cp3, ArrOfDouble& cp4, ArrOfDouble& cp5, ArrOfDouble& cp6,
                   ArrOfDouble& m,   ArrOfDouble& l0,  ArrOfDouble& l1,  ArrOfDouble& l2,
                   ArrOfDouble& m0,  ArrOfDouble& m1,  ArrOfDouble& m2,  ArrOfDouble& dv) 
  { 
#ifdef _OPENMP
    assert(pilot) ;
    assert(pilot[omp_get_thread_num()]) ;
    return (pilot[omp_get_thread_num()]->set_mixing_properties(nincon, 
                                 r,   cp0, cp1, cp2, 
                                 cp3, cp4, cp5, cp6,
                                 m,    l0,  l1,  l2,
                                 m0,   m1,  m2,  dv)) ;
#else
    assert(pilot) ;
    return (pilot->set_mixing_properties(nincon, 
                                 r,   cp0, cp1, cp2, 
                                 cp3, cp4, cp5, cp6,
                                 m,    l0,  l1,  l2,
                                 m0,   m1,  m2,  dv)) ;
#endif
  }

  inline EOS_Error EOS_Cathare2Vapor::set_mixing_properties() 
  {
#ifdef _OPENMP
    assert(pilot) ;
    assert(pilot[omp_get_thread_num()]) ;
    return (pilot[omp_get_thread_num()]->set_mixing_properties()) ;
#else
    assert(pilot) ;
    return (pilot->set_mixing_properties()) ;
#endif
  }

}

#endif
