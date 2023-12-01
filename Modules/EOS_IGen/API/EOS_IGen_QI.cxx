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
 * EOS_IGen_QI.cxx
 *
 *  Created on: 12 mai 2010
 *
 */

#include "EOS_IGen/API/EOS_IGen_QI.hxx"
#include "EOS_IGen/Src/EOS_Med.hxx"
#include <math.h>
#include <time.h>
#include <fstream>

#define DBL_EPSILON 1e-9

using namespace NEPTUNE;

namespace NEPTUNE_EOS_IGEN
{
  EOS_IGen_QI::EOS_IGen_QI(const char* const prop, const char* const tp,
                           double limit, int abs) :
  property(prop),
  type(tp),
  limit_qi(limit),
  is_abs(abs),
  average(0.0),
  quality_nodes(0)
  { if (!is_abs && (limit_qi < DBL_EPSILON))
       limit_qi=INIT_DLB ;
  }

  EOS_IGen_QI::EOS_IGen_QI(const EOS_IGen_QI& right):
  property(right.property),
  type(right.type),
  limit_qi(right.limit_qi),
  is_abs(right.is_abs),
  average(right.average),
  test_quality(right.test_quality),
  quality_nodes(right.quality_nodes)
  {
  }

  EOS_IGen_QI::~EOS_IGen_QI()
  {
  }
  
  void EOS_IGen_QI::make_centre_nodes(const EOS_Mesh* mesh, EOS_Fields& nodes)
  { if (mesh->get_domain().size() == 2)
       { for (int i=0; i<mesh->get_nb_mesh(); i++)
            { nodes[0][i] = 0.5 * (mesh->get_domain()[0][mesh->get_mesh_to_node(i)[0]] 
                                 + mesh->get_domain()[0][mesh->get_mesh_to_node(i)[1]]) ;
              nodes[1][i] = 0.5 * (mesh->get_domain()[1][mesh->get_mesh_to_node(i)[0]] 
                                 + mesh->get_domain()[1][mesh->get_mesh_to_node(i)[2]]) ;
            }
       }
    else if (mesh->get_domain().size() == 1)
       { for (int i=1; i<mesh->get_domain()[0].size(); i++)
            nodes[0][i-1] = 0.5 * (mesh->get_domain()[0][i-1] + mesh->get_domain()[0][i]) ;
       }
  }
   
  void EOS_IGen_QI::make_nodes(const EOS_Fields& domain, EOS_Fields& nodes)
  { if (domain.size() == 2)
       { for (int i=0; i<domain[0].size(); i++)
            { nodes[0][i] = domain[0][i] ;
              nodes[1][i] = domain[1][i] ;
            }
       }
    else if (domain.size() == 1)
       { for (int i=0; i<domain[0].size(); i++)
            nodes[0][i] = domain[0][i] ;
       }
   }
  
  void EOS_IGen_QI::make_quality(EOS_Field& res_ipp, EOS_Field& res_eos, ArrOfInt& test_quality_nodes)
  {
    test_quality = true ;
    quality_nodes.resize(res_ipp.size()) ;
    for (int i=0; i<res_ipp.size(); i++)
       { if (is_abs)
           quality_nodes[i] = fabs(res_ipp[i]-res_eos[i]) ;
         else
           quality_nodes[i] = fabs((res_ipp[i]-res_eos[i])/res_eos[i]) ;
         average = average+quality_nodes[i] ;
         
         if (quality_nodes[i]>limit_qi && limit_qi>(INIT_DLB+DBL_EPSILON) )
         { test_quality = false ;
           test_quality_nodes[i] = false ;
         }
       }
    average = average / res_ipp.size() ;
  }
  
  
  const EOS_IGen_QI& EOS_IGen_QI::operator=(const EOS_IGen_QI right)
  { property      = right.property ;
    type          = right.type ;
    is_abs        = right.is_abs ;
    limit_qi      = right.limit_qi ;
    average       = right.average ;
    test_quality  = right.test_quality ;
    quality_nodes = right.quality_nodes ;

    return *this ;
  }

}

