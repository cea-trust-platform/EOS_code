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
 * EOS_Mesh.hxx
 *
 *  Created on: 17 mai 2010
 */

#ifndef EOS_MESH_HXX_
#define EOS_MESH_HXX_
#include "EOS/API/EOS.hxx"
#include "EOS_Med.hxx"
#include "EOS/API/EOS_Field.hxx"
#include "EOS/API/EOS_Fields.hxx"
#include <vector>
using std::vector;

#define DBL_EPSILON 1e-9

using namespace NEPTUNE;

namespace NEPTUNE_EOS_IGEN
{
   class EOS_Mesh
   { public:
        EOS_Mesh();
        EOS_Mesh(int nb_p, int nb_h, double pmin, double pmax, double hmin, double hmax, int l_max);
        EOS_Mesh(int nb_p, double pmin, double pmax, int l_max);
        //EOS_Mesh(const EOS_Mesh& mesh);
        ~EOS_Mesh();

        void create_mesh(int nb_p, int nb_h, double pmin, double pmax, double hmin, double hmax);
        void create_mesh(int nb_p, double pmin, double pmax);
        
        void add_global_nodes();
        void add_local_nodes(int level, bool cont);
        void add_continuity_nodes(int level);
        
        void set_nodes(double pmin);
        void set_nodes(double pmin, double hmin);
        
        void set_domain_ph(EOS_Field& p, EOS_Field& h);
        void set_domain_continuity_ph(EOS_Field& p, EOS_Field& h);
        void set_domain_p(EOS_Field& p);
        
        void init_mesh_to_node();
        
        void set_error(ArrOfInt& ierr);
        
        bool get_exist() const ;
        int get_level_max() const;
        const EOS_Fields& get_domain() const;
        const EOS_Fields& get_domain_continuity() const;
        
        const double& get_delta_h() const;
        const double& get_delta_p() const;
        
        const int& get_nb_p() const;
        const int& get_nb_h() const;
        const int& get_nb_mesh() const;
        const int& get_nb_node() const;
        const int& get_nb_continuity() const;
        
        ArrOfInt& change_test_qualities_nodes();
        const ArrOfInt& get_test_qualities_nodes() const;
        const ArrOfInt& get_type_of_node() const;
        const ArrOfInt& get_mesh_to_node(int indice) const;
        const vector<ArrOfInt>& get_med_to_node() const;
        const vector<ArrOfInt>& get_continuity_to_node() const;
                        
        const EOS_Mesh & operator= (const EOS_Mesh& right);
        
        int size() const;
           
   private:
      bool exist;
      int nb_p;
      int nb_h;
      int level_max;     // max level of refinement applied (no refinement : 

      double delta_p;      // p step in first mesh
      double delta_h;      // h step in first mesh
      
      int nb_mesh;
      int nb_node;
      int nb_continuity;
      
      ArrOfDouble node_p;
      ArrOfDouble node_h;
      ArrOfDouble node_p_continuity;
      ArrOfDouble node_h_continuity;
      
      EOS_Field p;
      EOS_Field h;
      EOS_Field p_continuity;
      EOS_Field h_continuity;
      
      EOS_Fields domain; 
      EOS_Fields domain_continuity;
                      
      ArrOfInt test_qualities_nodes;
      ArrOfInt node_glb;
      ArrOfInt continuity_node;
      ArrOfInt type_of_node;
      
//    error inutile pour l'instant
      vector<EOS_Error_Field> error;
      vector<ArrOfInt> mesh_to_node;
      vector<ArrOfInt> med_to_node;
      vector<ArrOfInt> continuity_to_node;
      
      vector<ArrOfInt> med_correction;
      vector<ArrOfInt> new_correction;
   };
}


#include "EOS_Mesh_i.hxx"
#endif /* EOS_MESH_HXX_ */
