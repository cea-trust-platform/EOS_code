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
 * EOS_Med.hxx
 *
 *  Created on: 22 juin 2010
 */

#ifndef EOS_MED_HXX_
#define EOS_MED_HXX_

/*extern "C"
{
#include <med.hf>
  //#include <med_utils.h>
  }*/
/*#include "med_config.h"
#include "med_utils.h"*/
#include <med.h>
#define MESGERR 1
#include <med_utils.h>


#include "EOS/API/EOS_Error.hxx"
#include "EOS/API/EOS_Field.hxx"
#include "EOS/API/EOS_Fields.hxx"
#include "EOS/API/EOS_Error_Field.hxx"
#include "Common/func.hxx"

#include <vector>
using std::vector;

using namespace std;

namespace NEPTUNE
{

class EOS_Med
{ public:
     EOS_Med();
     EOS_Med(AString& file);
     ~EOS_Med();

     EOS_Error create_File();    //MED_ACC_CREAT
     EOS_Error rw_File();        //MED_ACC_RDWR
     EOS_Error read_File();      //MED_ACC_RDONLY
     EOS_Error close_File();
     EOS_Error write_header(AString str);
     EOS_Error read_header(AString& str);

     EOS_Error add_Maillage(AString& name, AString& description, int dim);
     EOS_Error add_Famille(AString& m_name, AString& fam_name, int fam_num);
     EOS_Error add_Champ_Noeud(AString& m_name, EOS_Field& champ);
     EOS_Error add_ErrChamp_Noeud(AString& m_name, AString& e_name, EOS_Error_Field& err);
     EOS_Error add_Scalar_Float(AString& sf_name, double& value);

     EOS_Error add_Nodes(AString& m_name, int dim, const EOS_Fields& nodes);
     EOS_Error add_Nodes2Fam(AString& name, const ArrOfInt& nodes);

     EOS_Error add_Connectivity_1D(AString& m_name,const EOS_Field& nodes);
     EOS_Error add_Connectivity_NoRef_2D(AString&  m_name, int nb_cell, int mesh_x);
     EOS_Error add_connectivity_WithRef_2D(AString&  m_name, int nb_cell_p, int mesh_x, const EOS_Fields& nodes_val, int nb_nodes_p, int nb_nodes_s);
     EOS_Error add_Connectivity_Refine_2D(AString& m_name, const vector<ArrOfInt>& med_to_node);

     EOS_Error get_Maillages_Infos(Strings& res_name, ArrOfInt& res_dim);
     
     EOS_Error get_entity_size(AString& m_name, int& size, const med_entity_type entite, const med_geometry_type gemetrie, const  med_data_type data, const med_connectivity_mode connectivite);
//   EOS_Error get_number_nodes(AString& m_name, int& res);
//   EOS_Error get_number_segm(AString& m_name, int& res);
//   EOS_Error get_number_poly(AString& m_name, int& res);
     EOS_Error get_number_champ(int& res);

     EOS_Error get_Champ_Noeud_Infos(int index, AString& c_name, int& type, int& nb_val, AString& m_ass);
     EOS_Error get_Champ_Noeud(AString& c_name, EOS_Field& res) ;
     EOS_Error get_ErrChamp_Noeud(AString& c_name, EOS_Error_Field& res) ;
     EOS_Error get_Scalar_Float(AString& sf_name, double& res);


     EOS_Error get_nodes(AString& m_name, int dim, EOS_Fields& nodes);
     EOS_Error get_nodes_fam(AString& m_name, ArrOfInt& res);
     EOS_Error get_Connectivity_1D(AString& m_name, ArrOfInt& res);
     EOS_Error get_Connectivity_2D(AString& m_name, ArrOfInt& index, ArrOfInt& connect);
        
  private:
     AString file;
     med_idt fid;
     EOS_Error check_file_format();
};

}

#include "EOS_Med_i.hxx"
#endif /* EOS_MED_HXX_ */
