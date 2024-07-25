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



/*
 * EOS_IGen_QI.hxx
 *
 *  Created on: 12 mai 2010
 */

#ifndef EOS_IGEN_QI_HXX_
#define EOS_IGEN_QI_HXX_

#include "Language/API/Language.hxx"
#include "EOS/API/EOS.hxx"
#include "EOS/Src/EOS_Ipp/EOS_Ipp.hxx"
#include "EOS_IGen/Src/EOS_Mesh.hxx"
#include "Common/func.hxx"
#include "EOS/API/EOS_properties.hxx"
#include <vector>
using std::vector ;

using namespace NEPTUNE ;

namespace NEPTUNE_EOS_IGEN
{

  
/**
 * Class EOS_IGen_QI
 * for quality indicator :
 *              quality name (AString)
 *              value (double)
 *              is_abs (int)  : absolute (1) or relative (0) compute for quality
 *              property (AString) : property on which the quality indicator is applied
 *
 */
  class EOS_IGen_QI
  { public:
       EOS_IGen_QI(const char* const prop,const char* const tp, double limit, int abs) ;
       EOS_IGen_QI(const EOS_IGen_QI&) ;
       ~EOS_IGen_QI() ;
       
       void set_quality_mesh(const EOS_Mesh* mesh, EOS_Fields& nodes) ;
       
       const AString& get_property() const ;
       EOS_Property get_property_number() const ;
       AString get_propint()         const ;
       const AString& get_type()     const ;
       int get_is_abs() const ;
       bool get_test_quality() const ;
       double get_limit_qi() const ;
       
       const ArrOfDouble& get_quality_nodes() const ;
    
       void make_centre_nodes(const EOS_Mesh* mesh, EOS_Fields& nodes) ;
       void make_nodes(const EOS_Fields& domain, EOS_Fields& nodes) ;
       
       void make_quality(EOS_Field& res_ipp, EOS_Field& res_eos, ArrOfInt& test_quality_nodes) ;

       const EOS_IGen_QI & operator= (const EOS_IGen_QI right) ;
      
    private:
       AString property ;
       EOS_Property property_number;
       AString type ;
       double limit_qi ;
       int is_abs ;
       
       double average ;
       bool test_quality ;
       ArrOfDouble quality_nodes ;
       
#if  __cplusplus >= 201103L
       constexpr    static const double INIT_DLB=-9999.9 ;
#else
       static const double              INIT_DLB=-9999.9 ;
#endif
  };
}

#include "EOS_IGen_QI_i.hxx"
#endif /* EOS_IGEN_QI_HXX_ */
