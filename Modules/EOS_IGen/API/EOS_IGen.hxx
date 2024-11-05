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
 * EOS_IGen.hxx
 *
 *  Created on: 11 mai 2010
 *
 */

#ifndef EOS_IGEN_HXX_
#define EOS_IGEN_HXX_

#include "Language/API/Language.hxx"
#include "EOS/API/EOS_Fields.hxx"
#include "EOS/API/EOS_Error.hxx"
#include "EOS_IGen_QI.hxx"
#include "EOS/API/EOS.hxx"
#include "EOS_IGen/Src/EOS_Mesh.hxx"
#include "EOS/API/EOS_Std_Error_Handler.hxx"
#include "Common/func.hxx"
#include <vector>
using std::vector;

using namespace NEPTUNE;

namespace NEPTUNE_EOS_IGEN
{
   class EOS_IGen
   { public:
        EOS_IGen() ;
        EOS_IGen(const EOS_IGen& right) ;
        EOS_IGen(const char* const meth, const char* const ref) ;
        ~EOS_IGen() ;

        void set_method(const char* const str)    ;
        void set_reference(const char* const str) ;
        
        void set_extremum(double p_min, double p_max, double T_min, double T_max) ;
        EOS_Error set_memory_max(int mmax) ;
        
        void set_obj_Ipp() ;
        EOS* get_obj_Ipp() const;

        void set_fluid()   ;
        void set_mesh_ph(int nb_mesh_p, int nb_mesh_h, int level_max) ;
        void set_mesh_p(int nb_mesh_p, int level_max) ;
        
        void set_IGen_handler() ;
        
        void set_tempory_med_name()  ;
        void set_path_environement() ;
        void set_file_med_name(AString& file_name) ;
        void set_file_med_name() ;
        void make_header(AString& header) ; 
        
        void set_quality(const char* const property, const char* const type, 
                         int const is_abs, double const limit_qi=-9999.9) ;
        
        EOS_Error make_mesh(int nb_mesh_p, int nb_mesh_h, int level_max=-1)    ;
        EOS_Error make_mesh_ph(int nb_mesh_p, int nb_mesh_h, int level_max=-1) ;
        EOS_Error make_mesh_p(int nb_mesh_p, int level_max=-1)                 ;
        
        EOS_Error compute_qualities() ;
        
        EOS_Error make_global_refine() ;
        EOS_Error make_local_refine(bool cont=true) ;
        
        int check_properties(const AString& property) const ;
        
        EOS_Error make_properties(EOS_Med& med) ;
        EOS_Error compute_properties(EOS_Field& field, EOS_Error_Field& error) ;
        
        EOS_Error write_med()               ;
        EOS_Error write_tempory_med()       ;
        EOS_Error write_index(bool tempory) ;
        
        const AString& get_method() const    ;
        const AString& get_reference() const ;
        
        const EOS_Mesh& get_mesh_p() const  ;
        const EOS_Mesh& get_mesh_ph() const ;
        
        EOS get_fluid() const;
        
        int get_memory_max() const ;
        
        const vector<EOS_IGen_QI>& get_qualities() const ;
        
        const EOS_IGen & operator= (const EOS_IGen& right) ;

        void set_list_propi(vector<string>& lpi)      ;
        void set_list_propi_sat(vector<string>& lpis) ;
        void set_list_propi_lim(vector<string>& lpil) ;


     private:
        AString method    ;
        AString reference ;

        double pmin ;
        double pmax ;
        double Tmin ;
        double Tmax ;
        double hmin ;
        double hmax ;
                        
        int memory_max ;
        
        EOS_Mesh* mesh_p  ;
        EOS_Mesh* mesh_ph ;
        
        EOS* fluid   ;
        EOS* obj_Ipp ;
        
        EOS_Std_Error_Handler IGen_handler ;
        
        AString path_environement ;
        AString tempory_med_name  ;
        AString file_med_name     ;
        
        vector<EOS_IGen_QI> qualities ;
        bool test_qualities ;
        bool refine ;

        vector<string> list_propi     ;
        vector<string> list_propi_sat ;
        vector<string> list_propi_lim ;
   };
}
#include "EOS_IGen_i.hxx"
#endif /* EOS_IGEN_HXX_ */
