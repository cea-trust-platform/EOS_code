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
 * EOS_IGen_QI_i.hxx
 *
 *  Created on: 12 mai 2010
 */

#ifndef EOS_IGEN_QI_I_HXX_
#define EOS_IGEN_QI_I_HXX_


namespace NEPTUNE_EOS_IGEN
{
  
  inline void EOS_IGen_QI::set_quality_mesh(const EOS_Mesh* mesh, EOS_Fields& nodes)
  { if      (type == "centre")
       make_centre_nodes(mesh,nodes) ;
    else if (type == "node")
       make_nodes(mesh->get_domain(), nodes) ;
    else 
       cerr << "EOS_IGen_QI::set_quality_mesh :  type " << type 
            << " is not implemented" << endl ;
  }
  
  inline const AString& EOS_IGen_QI::get_type() const
  { return type ;
  }
  
  inline int EOS_IGen_QI::get_is_abs() const
  { return is_abs ;
  }
  
  inline bool EOS_IGen_QI::get_test_quality() const
  { return test_quality ;
  }
  
  inline double EOS_IGen_QI::get_limit_qi() const
  { return limit_qi ;
  }
   
  inline const ArrOfDouble& EOS_IGen_QI::get_quality_nodes() const
  { return quality_nodes ; 
  }
  
  inline const AString& EOS_IGen_QI::get_property() const
  { return property ;
  }
  
  inline EOS_Property EOS_IGen_QI::get_property_number() const
  { return property_number ;
  }

  inline AString EOS_IGen_QI::get_propint() const
  { char propconv[PROPNAME_MSIZE] ;
    eostp_strcov(property.aschar(), propconv) ;
    return propconv ;
  }



}
#endif /* EOS_IGEN_QI_I_HXX_ */
