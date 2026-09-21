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


using namespace NEPTUNE;

namespace NEPTUNE_EOS_IGEN
{
  EOS_IGen_QI::EOS_IGen_QI(const char* const prop, const char* const tp,
                           double limit, int abs, int sub) :
  property(prop),
  type(tp),
  limit_qi(limit),
  is_abs(abs),
  nb_sub(sub < 1 ? 1 : sub),
  quality_nodes(0)
  {
    property_number = gen_property_number(prop);

    //  A quality limit means something only if it is strictly positive; that,
    //  and nothing else, is what tells a threshold from "no threshold". The
    //  default INIT_DLB is negative and so reads as "no threshold" here.
    //
    //  This used to reset any *relative* limit below 1e-9 to INIT_DLB, i.e. to
    //  no threshold at all -- and 1e-9 came from a DBL_EPSILON this file
    //  #defined for itself, seven orders of magnitude above the one in
    //  <cfloat>. Asking for a tighter tolerance than 1e-9 therefore produced a
    //  mesh that was never refined, silently: --quality_limit=2e-9 gave two
    //  levels of refinement and 5e-10 gave none.
//  An even nb_sub puts every sample at a quarter point and none at the cell
//  centre, which is where a Hermite patch departs furthest from the data.
//  Measured on a 5x5 WaterLiquid tile asking for 1e-6 on T, largest relative
//  error delivered over an 80x80 grid: 1.31e-6 at nb_sub=1, 1.69e-6 at 2,
//  8.13e-7 at 3, 1.32e-6 at 4, and 8.13e-7 again at 5 and 7 -- the odd ones
//  meet the limit and agree on the same mesh from 3 up, the even ones miss it,
//  and nb_sub=4 costs more than 3 to do worse. 3 is the useful setting.
    if (nb_sub > 1 && nb_sub % 2 == 0)
       cerr << "EOS_IGen_QI: quality subsampling " << nb_sub << " for property " << prop
            << " is even, so no sample falls at a cell centre; an odd value measures"
            << " the interpolation error better for less work" << endl ;

    has_limit = (limit_qi > 0.e0) ;
    if (!has_limit && limit_qi > INIT_DLB)
       cerr << "EOS_IGen_QI: quality limit " << limit_qi << " for property "
            << prop << " is not strictly positive; no quality threshold applies"
            << endl ;
  }

  EOS_IGen_QI::EOS_IGen_QI(const EOS_IGen_QI& right):
  property(right.property),
  property_number(right.property_number),
  type(right.type),
  limit_qi(right.limit_qi),
  has_limit(right.has_limit),
  is_abs(right.is_abs),
  nb_sub(right.nb_sub),
  test_quality(right.test_quality),
  quality_nodes(right.quality_nodes)
  {
  }

  EOS_IGen_QI::~EOS_IGen_QI()
  {
  }
  
//  Sample points inside each cell: nb_sub x nb_sub of them, at the centres of
//  the sub-cells of a nb_sub x nb_sub division, so nb_sub = 1 is exactly the
//  single cell centre this always used and nothing moves for a caller that
//  does not ask for more.
//
//  One point per cell is what the criterion had, and it is not what the
//  criterion promises. A cell passes on the strength of its centre while the
//  interpolant is free to be wrong anywhere else in it: a Hermite patch is
//  exact at the four corners and worst somewhere between them, and the centre
//  is one arbitrary place to look. Measured on the raffinement_local fixture,
//  asking for 1e-7 delivered 3.6e-07 -- met at every point the criterion
//  looked at, missed by 3.6x at one it did not.
//
//  The points come out cell by cell, all of a cell's samples together, which
//  is what make_quality relies on to fold them back.
  void EOS_IGen_QI::make_centre_nodes(const EOS_Mesh* mesh, EOS_Fields& nodes)
  { const int k = nb_sub ;
    if (mesh->get_domain().size() == 2)
       { for (int i=0; i<mesh->get_nb_mesh(); i++)
            { const ArrOfInt& c = mesh->get_mesh_to_node(i) ;
              const double h0 = mesh->get_domain()[0][c[0]] ;
              const double h1 = mesh->get_domain()[0][c[1]] ;
              const double p0 = mesh->get_domain()[1][c[0]] ;
              const double p1 = mesh->get_domain()[1][c[2]] ;
              for (int a=0; a<k; a++)
                 for (int b=0; b<k; b++)
                    { const int j = (i*k + a)*k + b ;
                      nodes[0][j] = h0 + ((b + 0.5e0) / double(k)) * (h1 - h0) ;
                      nodes[1][j] = p0 + ((a + 0.5e0) / double(k)) * (p1 - p0) ;
                    }
            }
       }
    else if (mesh->get_domain().size() == 1)
       { for (int i=1; i<mesh->get_domain()[0].size(); i++)
            { const double x0 = mesh->get_domain()[0][i-1] ;
              const double x1 = mesh->get_domain()[0][i] ;
              for (int a=0; a<k; a++)
                 nodes[0][(i-1)*k + a] = x0 + ((a + 0.5e0) / double(k)) * (x1 - x0) ;
            }
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
//  Several samples may belong to one cell, and the cell fails if any of them
//  does. With samples_per_cell() == 1 this is the identity it always was.
    const int spc = samples_per_cell() ;
    for (int i=0; i<res_ipp.size(); i++)
       { if (is_abs)
           quality_nodes[i] = fabs(res_ipp[i]-res_eos[i]) ;
         else
           quality_nodes[i] = fabs((res_ipp[i]-res_eos[i])/res_eos[i]) ;
         if (has_limit && quality_nodes[i] > limit_qi)
         { test_quality = false ;
           test_quality_nodes[i/spc] = false ;
         }
       }
  }
  
  
  const EOS_IGen_QI& EOS_IGen_QI::operator=(const EOS_IGen_QI right)
  { property        = right.property ;
    property_number = right.property_number ;
    type            = right.type ;
    is_abs          = right.is_abs ;
    nb_sub          = right.nb_sub ;
    limit_qi        = right.limit_qi ;
    has_limit       = right.has_limit ;
    test_quality    = right.test_quality ;
    quality_nodes   = right.quality_nodes ;

    return *this ;
  }

}

