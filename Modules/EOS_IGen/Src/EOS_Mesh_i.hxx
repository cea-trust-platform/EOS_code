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
 * EOS_Mesh_i.hxx
 *
 *  Created on: 17 mai 2010
 */

#ifndef EOS_MESH_I_HXX_
#define EOS_MESH_I_HXX_

namespace NEPTUNE_EOS_IGEN
{
  inline void EOS_Mesh::set_domain_ph(EOS_Field& p, EOS_Field& h)
  { domain[1] = p ;
    domain[0] = h ;
  }
  
  inline void EOS_Mesh::set_domain_continuity_ph(EOS_Field& p, EOS_Field& h)
  { domain_continuity[1] = p ;
    domain_continuity[0] = h ;
  }

  inline void EOS_Mesh::set_domain_p(EOS_Field& p)
  { domain[0] = p ;
  }
  
  inline void EOS_Mesh::set_nodes(double pmin)
  { node_p[0] = pmin ;
    for (int i=1; i< nb_p; i++)
       node_p[i] = node_p[i-1] + delta_p ;
  }
  
  inline void EOS_Mesh::set_nodes(double pmin, double hmin)
  { double tmp_p = pmin ;
    double tmp_h = hmin ;
    
    int k = 0 ;
    for (int i=0; i< nb_p; i++)
       { node_p[k] = tmp_p ;
         node_h[k] = tmp_h ;
         node_p_continuity[k] = tmp_p ;
         node_h_continuity[k] = tmp_h ;
         k++ ;
         for (int j=1; j<nb_h; j++)
            { node_p[k] = node_p[k-1] ;
              node_h[k] = node_h[k-1] + delta_h ;
              node_p_continuity[k] = node_p_continuity[k-1] ;
              node_h_continuity[k] = node_h_continuity[k-1] + delta_h ;
              k++ ;
            }
         tmp_p = tmp_p + delta_p ;
       }
  }
  
  inline void EOS_Mesh::set_error(ArrOfInt& ierr)
  { EOS_Error_Field err(ierr) ;
    error.push_back(err) ;
  }


  inline bool EOS_Mesh::get_exist() const
  { return exist ;
  }
  
 
  inline int EOS_Mesh::get_level_max() const
  { return level_max ;
  }

  inline const EOS_Fields& EOS_Mesh::get_domain() const
  { return domain ;
  }
  
  inline const EOS_Fields& EOS_Mesh::get_domain_continuity() const
  { return domain_continuity ;
  }
  

  inline const double& EOS_Mesh::get_delta_h() const
  { return delta_h ;
  }
  
  inline const double& EOS_Mesh::get_delta_p() const
  { return delta_p ;
  }
  
  inline const int& EOS_Mesh::get_nb_p() const
  { return nb_p ;
  }
  
  inline const int& EOS_Mesh::get_nb_mesh() const
  { return nb_mesh ;
  }
  
  inline const int& EOS_Mesh::get_nb_node() const
  { return nb_node ;
  }
  
  
  inline const int& EOS_Mesh::get_nb_continuity() const
  { return nb_continuity ;
  }
  
  inline const int& EOS_Mesh::get_nb_h() const
  { return nb_h ;
  }
  
  inline ArrOfInt& EOS_Mesh::change_test_qualities_nodes() 
  { return test_qualities_nodes ;
  }
  
  const inline ArrOfInt& EOS_Mesh::get_test_qualities_nodes() const
  { return test_qualities_nodes ;
  }
  
  const inline ArrOfInt& EOS_Mesh::get_type_of_node() const
  { return type_of_node ;
  }
  
  const inline ArrOfInt& EOS_Mesh::get_mesh_to_node(int indice) const
  { return mesh_to_node[indice] ;
  }
  
  const inline vector<ArrOfInt>& EOS_Mesh::get_med_to_node() const
  { return med_to_node ;
  }
  
  const inline vector<ArrOfInt>& EOS_Mesh::get_continuity_to_node() const
  { return continuity_to_node ;
  }
  
  inline int EOS_Mesh::size() const
  { return domain.size() ;
  }

}
#endif /* EOS_MESH_I_HXX_ */
