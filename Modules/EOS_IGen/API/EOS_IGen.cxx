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
 * EOS_IGen.cxx
 *
 *  Created on: 11 mai 2010
 *
 */
#include "EOS_IGen.hxx"
#include "EOS/API/EOS_Config.hxx"

#include <fstream>
#include <string>
using std::string;

namespace NEPTUNE_EOS_IGEN
{
  EOS_IGen::EOS_IGen():
  method("unknown"),
  reference("unknown"),
  memory_max(-1),
  IGen_handler(),
  test_qualities(false),
  refine(false)
  { mesh_ph = new EOS_Mesh() ;
    mesh_p  = new EOS_Mesh() ;
  }
      
  
  EOS_IGen::EOS_IGen(const EOS_IGen& right):
  method(right.method),
  reference(right.reference),
  pmin(right.pmin),
  pmax(right.pmax),
  Tmin(right.Tmin),
  Tmax(right.Tmax),
  hmin(right.hmin),
  hmax(right.hmax),
  memory_max(right.memory_max),
  mesh_p(right.mesh_p),
  mesh_ph(right.mesh_ph),
  fluid(right.fluid),
  IGen_handler(right.IGen_handler),
  qualities(right.qualities),
  test_qualities(right.test_qualities),
  refine(right.refine)
  {
  }
        
  EOS_IGen::EOS_IGen(const char* const meth, const char* const ref):
  method(meth),
  reference(ref),
  memory_max(-1),
  IGen_handler(),
  test_qualities(false),
  refine(false)
  { mesh_ph = new EOS_Mesh() ;
    mesh_p  = new EOS_Mesh() ;
  }
        
  
  EOS_IGen::~EOS_IGen()
   { /*
   if (mesh_p != nullptr) 
   {
       delete mesh_p;
      mesh_p = nullptr;
   }
   if (mesh_ph != nullptr) 
   {
       delete mesh_ph;
      mesh_ph = nullptr;
   }
    if (fluid != nullptr) 
   {
       delete fluid;
      fluid = nullptr;
   }
   if ((obj_Ipp != nullptr) || ((int)(obj_Ipp->id())) != 0) 
   {
      delete obj_Ipp;
      obj_Ipp = nullptr;
   } */
  }
  
  
  EOS_Error EOS_IGen::make_mesh(int nb_node_p, int nb_node_h, int level_max)
  { ArrOfDouble hh(4) ;
    EOS_Internal_Error ierr ;
    EOS_Error err ;
    
    if ((nb_node_p < 2) || ( nb_node_h < 2))
       { cerr<< "Error EOS_IGen::make_mesh : a mesh object needs "
             << " a number of nodes larger than 2" <<endl ;
         exit(1) ;
       }
    set_fluid() ;
    set_IGen_handler() ;
    
    ierr = fluid->fluid().compute_h_pT(pmin,Tmin,hh[0]) ;
    if (ierr.generic_error() != EOS_Error::good)
       { cerr << "Error EOS_IGen::make_mesh : bad value for pmin " << pmin 
              << "and Tmin " << Tmin <<endl ;
         
         return ierr.generic_error() ;    
       }
    
    ierr = fluid->fluid().compute_h_pT(pmin,Tmax,hh[1]) ;
    if (ierr.generic_error() != EOS_Error::good)
    { cerr << "Error EOS_IGen::make_mesh : bad value for pmin "<< pmin 
              << "and Tmax " << Tmax <<endl ;
         return ierr.generic_error() ; 
       }
    
    ierr = fluid->fluid().compute_h_pT(pmax,Tmin,hh[2]) ;
    if (ierr.generic_error() != EOS_Error::good)
       { cerr << "Error EOS_IGen::make_mesh : bad value for pmax "<< pmax 
              << "and Tmin " << Tmin <<endl ;
         return ierr.generic_error() ;
       }
    ierr = fluid->fluid().compute_h_pT(pmax,Tmax,hh[3]) ;
    if (ierr.generic_error() != EOS_Error::good)
       { cerr << "Error EOS_IGen::make_mesh : bad value for pmax " << pmax 
              << "and Tmax " << Tmax <<endl ;
         return ierr.generic_error() ;
       }
       
    hmin = hmax = hh[0] ;
    for (int i=0; i<4; i++)
       { if (hh[i] < hmin)  hmin = hh[i] ;
         if (hh[i] > hmax)  hmax = hh[i] ;
       }

    set_mesh_ph(nb_node_p, nb_node_h, level_max) ;
    set_mesh_p(nb_node_p, level_max) ;
    err = write_tempory_med() ;
    if (err != EOS_Error::good)
       { cerr<< "Error EOS_IGen::make_mesh : no temporary MED file writed" <<endl ;
         return err ;
       }
    err = write_index(true) ;
    if (err != EOS_Error::good)
       { cerr<< "Error EOS_IGen::make_mesh : no index.eos file writed" <<endl ;
         return err ;
       }
       
    set_obj_Ipp() ;
    
    return good ;
  }
  
  
  EOS_Error EOS_IGen::make_mesh_ph(int nb_node_p, int nb_node_h, int level_max)
  { ArrOfDouble hh(4) ;
    EOS_Internal_Error ierr ;
    EOS_Error err ;
    
    if (nb_node_p<2 || nb_node_h<2)
       { cerr<< "Error EOS_IGen::make_mesh_ph : a mesh object needs "
             << "a number of nodes larger than 2" <<endl ;
         exit(1) ;
       }
    
    set_fluid() ;
    ierr = fluid->fluid().compute_h_pT(pmin,Tmin,hh[0]) ;
    if (ierr.generic_error() != EOS_Error::good)
       { cerr << "Error EOS_IGen::make_mesh_ph : bad value for pmin " << pmin 
              << "and Tmin " << Tmin <<endl ;
         return ierr.generic_error() ;
       }
    
    ierr = fluid->fluid().compute_h_pT(pmin,Tmax,hh[1]) ;
    if (ierr.generic_error() != EOS_Error::good)
       { cerr << "Error EOS_IGen::make_mesh_ph : bad value for pmin " << pmin 
              << "and Tmax " << Tmax <<endl ;
         return ierr.generic_error() ;
       }
    
    ierr = fluid->fluid().compute_h_pT(pmax,Tmin,hh[2]) ;
    if (ierr.generic_error() != EOS_Error::good)
       { cerr << "Error EOS_IGen::make_mesh_ph : bad value for pmax " << pmax 
              << "and Tmin " << Tmin <<endl ;
         return ierr.generic_error() ;
       }

    ierr = fluid->fluid().compute_h_pT(pmax,Tmax,hh[3]) ;
    if (ierr.generic_error() != EOS_Error::good)
       { cerr << "Error EOS_IGen::make_mesh_ph : bad value for pmax " << pmax
              << "and Tmax " << Tmax <<endl ;
         return ierr.generic_error() ;
       }
    
    hmin = hmax = hh[0] ;
    for (int i=0; i<4; i++)
       { if (hh[i] < hmin) hmin = hh[i] ;
         if (hh[i] > hmax) hmax = hh[i] ;
       }

    set_mesh_ph(nb_node_p, nb_node_h, level_max) ;
    err = write_tempory_med() ;
    if (err != EOS_Error::good)
       { cerr<< "Error EOS_IGen::make_mesh_ph : no temporary MED file writed" <<endl ;
         return err ;
       }
    err = write_index(true) ;
    if (err != EOS_Error::good)
       { cerr<< "Error EOS_IGen::make_mesh_ph : no index.eos file writed" <<endl ;
         return err ;
       }
       
    set_obj_Ipp() ;
       
    return EOS_Error::good ;
  }
  
  
  EOS_Error EOS_IGen::make_mesh_p(int nb_node_p, int level_max)
  { EOS_Error err ;
    
    if (nb_node_p < 2)
       { cerr<< "Error EOS_IGen::make_mesh_p : a mesh object needs "
             << "a number of nodes larger than 2" <<endl ;
         exit(1) ;
       }
    set_fluid() ;
        
    set_mesh_p(nb_node_p, level_max) ;
    err = write_tempory_med() ;
    if (err != EOS_Error::good)
       { cerr<< "Error EOS_IGen::make_mesh_p : no temporary MED file writed" <<endl ;
         return err ;
       }
    err = write_index(true) ;
    if (err != EOS_Error::good)
       { cerr<< "Error EOS_IGen::make_mesh_p : no index.eos file writed" <<endl ;
         return err ;
       }

    set_obj_Ipp() ;
    
    return EOS_Error::good ;
  }
 

  void EOS_IGen::set_path_environement()
  { if (iret_eos_data_dir) exit(1) ;
    path_environement = eos_data_dir.c_str() ;
  }

 
  EOS_Error EOS_IGen::write_med()
  { EOS_Error err ;

    if (strlen(file_med_name.aschar())     == 0)  set_file_med_name() ;
    if (strlen(path_environement.aschar()) == 0)  set_path_environement() ;
    
    AString med_file = ""         ;
    med_file += path_environement ;
    med_file += "/EOS_Ipp/"       ;
    med_file += file_med_name     ;
    med_file += ".med"            ;
    EOS_Med med(med_file) ;
    err = med.create_File() ;
    if (err != EOS_Error::good)
       { cerr<< " Error EOS_IGen::write_med : impossible to create MED file" <<endl ;
         return err ;
       }
    
    AString header("eos:") ;
    make_header(header) ;
    err = med.write_header(header) ;
    if (err != EOS_Error::good)
       { cerr<< " Error EOS_IGen::write_med : impossible to write header in MED file" <<endl ;
         return err ;
       }

    err = make_properties(med) ;
    if (err != EOS_Error::good)
       { cerr<< " Error EOS_IGen::write_med : impossible to write properties in MED file" <<endl ;
         return err ;
       }

    err = med.close_File() ;
    if (err != EOS_Error::good)
       { cerr<< " Error EOS_IGen::write_med : impossible to close MED file" <<endl ;
         return err ;
       }

    err = write_index(false) ;
    if (err != EOS_Error::good)
       { cerr<< " Error EOS_IGen::write_med : impossible to write index.eos file" <<endl ;
         return err ;
       }
    
    return EOS_Error::good ;
  }
  
  
  EOS_Error EOS_IGen::write_tempory_med()
  { EOS_Error err ;
  
    set_tempory_med_name() ;
    if (strlen(tempory_med_name.aschar())  == 0) set_tempory_med_name()  ;
    if (strlen(path_environement.aschar()) == 0) set_path_environement() ;

    AString med_file = ""         ;
    med_file += path_environement ;
    med_file += "/EOS_Ipp/"       ;
    med_file += tempory_med_name  ;
    med_file += ".med"            ;
    EOS_Med med(med_file) ;
    err = med.create_File() ;
    if (err != EOS_Error::good)
       { cerr<< " Error EOS_IGen::write_tempory_med : impossible to create temporary MED file" <<endl ;
         return err ;
       }

    AString header("eos:") ;
    make_header(header) ;
    err = med.write_header(header) ;
    if (err != EOS_Error::good)
       { cerr<< " Error EOS_IGen::write_tempory_med : impossible to write header in temporary MED file" <<endl ;
         return err ;
       }

    err = make_properties(med) ;
    if (err != EOS_Error::good)
       { cerr<< " Error EOS_IGen::write_tempory_med : impossible to write properties in temporary MED file" <<endl ;
         return err ;
       }

    err = med.close_File() ;
    if (err != good)
       { cerr<< " Error EOS_IGen::write_tempory_med : impossible to close temporary MED file" <<endl ;
         return err ;
       }

    return EOS_Error::good ;
  }

  
/* make_header en prevision de se servir du header 
 * pour recreer la bdd.
 * pour l'instant header limite a 200c dc erreur
 */
  void EOS_IGen::make_header(AString& header)
  { header += method    ;
    header += "/"       ;
    header += reference ;
    header += "/"       ;
    if (mesh_ph->get_exist())
       { header += "mesh_ph/p_"        ;
         header += mesh_ph->get_nb_p() ;
         header += "_nodes_"           ;
         { char buffer[30] ;
           sprintf (buffer, "%e_%e", pmin, pmax) ;
           AString infos(buffer) ;
           header += infos ;
         }
         header += "/h_"               ;
         header += mesh_ph->get_nb_h() ;
         header += "_nodes_"           ;
         { char buffer[30] ;
           sprintf (buffer, "%e_%e", hmin, hmax) ;
           AString infos(buffer) ;
           header += infos ;
         }
         header += "/" ;
       }
    if (mesh_p->get_exist())
       { header += "mesh_p/p_"        ;
         header += mesh_p->get_nb_p() ;
         header += "_nodes_"           ;
         { char buffer[30] ;
           sprintf (buffer, "%e_%e", pmin, pmax) ;
           AString infos(buffer) ;
           header += infos ;
         }
         header += "/" ;
       }
//     for (int i=0 ; i<qualities.size() ; i++)
//     {
//       header+="quality";
//       header+=i;
//       header+="_";
//       header+=qualities[i].get_property_number();
//       header+="_";
//       header+=qualities[i].get_type();
//       header+="_";
//       if (qualities[i].get_is_abs())
//      header+="absolute_";
//       header+="limite_";
//       char *buffer = new char[100];
//       int n=sprintf (buffer, "%e", qualities[i].get_limit_qi());
//       AString infos(buffer);
//       header+=infos;
//     }
  }
  

  EOS_Error EOS_IGen::make_properties(EOS_Med& med)
  { EOS_Error err ;

    // 2D diagram P-h
    if (mesh_ph->get_exist())
       { // couple of floats
         double tp1 = mesh_ph->get_domain()[1][0] ;
         double tp2 = mesh_ph->get_domain()[0][0] ;

         // properties list effectively used : str_properties
         vector<string>  str_properties ;
         int nb_lps = list_propi.size() ;
         if (nb_lps == 0)
            { // use thermprop
              int sz = thermprop.size() ;
              vector<string>  list_propt ;
              for(int i=0; i<sz; i++)
                 list_propt.push_back(thermprop[i].name) ;
              str_properties  = fluid->is_implemented("P", "h", list_propt, tp1, tp2) ;
            }
         else
            { // use list_propi
              str_properties = fluid->is_implemented("P", "h", list_propi, tp1, tp2) ;
            }

//       ArrOfInt nerr(mesh_ph->get_nb_node());
//       ArrOfInt fnodes(mesh_ph->get_nb_node());
//       AString f_name("real nodes");
//       for (int i=0; i<mesh_ph->get_nb_node(); i++)
//        fnodes[i] = 1;
//       err = med.add_Famille(mesh_name,f_name,1);
//       err = med.add_Nodes2Fam(mesh_name, fnodes);
         
         AString mesh_name("ph_domain") ;
         AString description("ph mesh") ;

         err = med.add_Maillage(mesh_name, description, 2) ;
         err = med.add_Nodes(mesh_name,2,mesh_ph->get_domain_continuity()) ;

         int nb_nc = mesh_ph->get_nb_node() + mesh_ph->get_nb_continuity() ;
         ArrOfDouble x(nb_nc) ;
         ArrOfInt nerr(nb_nc) ;
         EOS_Error_Field err_field(nerr) ;

         int nb_sp = str_properties.size() ;
         if (!refine)
            { err = med.add_Connectivity_NoRef_2D(mesh_name, mesh_ph->get_nb_mesh(), 
                                                  mesh_ph->get_nb_h()-1) ;
              if (err != EOS_Error::good)   return err ;
              for (int i=0; i<nb_sp; i++)
                 { const char *pprop = str_properties[i].c_str() ;
                   EOS_Field field(pprop, pprop, x) ;

                   fluid->compute(mesh_ph->get_domain_continuity()[1], 
                                  mesh_ph->get_domain_continuity()[0], field, err_field) ;
                   err = med.add_Champ_Noeud(mesh_name,field) ;

                   AString err_name = pprop ;
                   err = med.add_ErrChamp_Noeud(mesh_name, err_name, err_field) ;
                   if (err != EOS_Error::good)  return err ;
                 }
            }
         else
            { err = med.add_Connectivity_Refine_2D(mesh_name ,mesh_ph->get_med_to_node()) ;
              if (err != EOS_Error::good)   return err ;
              for (int i=0; i<nb_sp; i++)
                 { const char *pprop = str_properties[i].c_str() ;
                   EOS_Field field(pprop, pprop, x) ;

                   err = compute_properties(field, err_field) ;
                   err = med.add_Champ_Noeud(mesh_name,field) ;

                   AString err_name = pprop ;
                   err = med.add_ErrChamp_Noeud(mesh_name, err_name, err_field) ;
                   if (err != EOS_Error::good)  return err ;
                 }
            }
       }
    
    // 1D curve
    if (mesh_p->get_exist())
       { double tp = mesh_p->get_domain()[0][0] ;  // float
         
         // saturation curve
         { int nb_lpss = list_propi_sat.size() ;

           // properties list effectively used : str_properties_sat
           vector<string>  str_properties_sat ;
           if (nb_lpss == 0)
              { // use saturprop
                int sz = saturprop.size() ;
                vector<string>  list_propt_sat ;
                for(int i=0; i<sz; i++)
                   list_propt_sat.push_back(saturprop[i].name) ;
                str_properties_sat  = fluid->is_implemented("Psat", list_propt_sat, tp) ;
              }
           else
              { // use list_propi_sat
                str_properties_sat = fluid->is_implemented("Psat", list_propi_sat, tp) ;
              }

           AString mesh_name("sat_domain") ;
           AString description("saturation mesh") ;
           
           err = med.add_Maillage(mesh_name, description, 1) ;
           err = med.add_Nodes(mesh_name, 1, mesh_p->get_domain()) ;
           err = med.add_Connectivity_1D(mesh_name, mesh_p->get_domain()[0]) ;
           if (err != EOS_Error::good)  return err ;
                 
           ArrOfDouble x(mesh_p->get_nb_node()) ;
           ArrOfInt nerr(mesh_p->get_nb_node()) ;
           EOS_Error_Field err_field(nerr) ;
           int nb_sps = str_properties_sat.size() ;
           for (int i=0; i<nb_sps; i++)
              { const char *pprop = str_properties_sat[i].c_str() ;
                EOS_Field field(pprop, pprop, x) ;

                fluid->compute(mesh_p->get_domain()[0], field, err_field) ;
                err = med.add_Champ_Noeud(mesh_name,field) ;
                
                AString err_name = pprop ;
                err = med.add_ErrChamp_Noeud(mesh_name, err_name, err_field) ;
                if (err != EOS_Error::good)  return err ;
              }
         }

         // spinodal curves
         { int nb_lpls = list_propi_lim.size() ;

           // properties list effectively used : str_properties_lim
           vector<string>  str_properties_lim ;
           if (nb_lpls == 0)
              { // use splimprop
                int sz = splimprop.size() ;
                vector<string>  list_propt_lim ;
                for(int i=0; i<sz; i++)
                   list_propt_lim.push_back(splimprop[i].name) ;
                str_properties_lim  = fluid->is_implemented("Plim", list_propt_lim, tp) ;
              }
           else
              { // use list_propi_lim
                str_properties_lim = fluid->is_implemented("Plim", list_propi_lim, tp) ;
              }

           AString mesh_name("lim_domain");
           AString description("spinodal mesh");
           
           err = med.add_Maillage(mesh_name, description, 1) ;
           err = med.add_Nodes(mesh_name,1,mesh_p->get_domain()) ;
           err = med.add_Connectivity_1D(mesh_name, mesh_p->get_domain()[0]) ;
           if (err != EOS_Error::good)  return err ;

           ArrOfDouble x(mesh_p->get_nb_node()) ;
           ArrOfInt nerr(mesh_p->get_nb_node()) ;
           EOS_Error_Field err_field(nerr) ;
           int nb_spl = str_properties_lim.size() ;
           for (int i=0; i<nb_spl; i++)
              { const char *pprop = str_properties_lim[i].c_str() ;
                EOS_Field field(pprop, pprop, x) ;

                fluid->compute(mesh_p->get_domain()[0], field, err_field) ;
                err = med.add_Champ_Noeud(mesh_name,field) ;
                
                AString err_name = pprop ;
                err = med.add_ErrChamp_Noeud(mesh_name, err_name, err_field) ;
                if (err != EOS_Error::good)  return err ;
              }
         }
       }

    AString str_pmin("pmin") ;
    err = med.add_Scalar_Float(str_pmin, pmin) ;

    AString str_pmax("pmax") ;
    err = med.add_Scalar_Float(str_pmax, pmax) ;

    AString str_hmin("hmin") ;
    err = med.add_Scalar_Float(str_hmin, hmin) ;

    AString str_hmax("hmax") ;
    err = med.add_Scalar_Float(str_hmax, hmax) ;

    AString str_tmin("tmin") ;
    err = med.add_Scalar_Float(str_tmin, Tmin) ;
    
    AString str_tmax("tmax") ;
    err = med.add_Scalar_Float(str_tmax, Tmax) ;

    AString str_dp("delta_p") ;
    AString str_dh("delta_h") ;
    if (mesh_ph->get_exist())
       { double dp = mesh_ph->get_delta_p()     ;
         err = med.add_Scalar_Float(str_dp, dp) ;
         double dh = mesh_ph->get_delta_h()     ;
         err = med.add_Scalar_Float(str_dh, dh) ;
       }
    else if (mesh_p->get_exist())
       { double dp = mesh_p->get_delta_p()      ;
         err = med.add_Scalar_Float(str_dp, dp) ; 
         double dh = 0.e0 ;
         err = med.add_Scalar_Float(str_dh, dh) ;
       }
    
    double Tc ;
    AString str_tcrit("tcrit") ;
    try
       { fluid->get_T_crit(Tc) ;
       }
    catch (EOS_Std_Exception ex)
       { Tc = -1.e0 ;
       }
    err = med.add_Scalar_Float(str_tcrit, Tc) ;

    double pc ;
    AString str_pcrit("pcrit") ;
    try
       { fluid->get_p_crit(pc) ;
       }
    catch (EOS_Std_Exception ex)
       { pc = -1.e0 ;
       }
    err = med.add_Scalar_Float(str_pcrit, pc) ;

    double hc ;
    AString str_hcrit("hcrit") ;
    try
       { fluid->get_h_crit(hc) ;
       }
    catch (EOS_Std_Exception ex)
       { hc = -1.e0 ;
       }
    err = med.add_Scalar_Float(str_hcrit, hc) ;

    return EOS_Error::good ;
  }
  
  EOS_Error EOS_IGen::compute_properties(EOS_Field& field, EOS_Error_Field& err_field)
  { int sz = mesh_ph->get_nb_node() + mesh_ph->get_nb_continuity() ;
    
//     valable pour les noeuds réels et les noeuds de continuite au centre de mailles
    fluid->compute(mesh_ph->get_domain_continuity()[1], 
                   mesh_ph->get_domain_continuity()[0], field, err_field) ;
    
    int k = 0 ;
    for (int i=0; i<sz; i++)
       { if (mesh_ph->get_type_of_node()[i] == 1 || mesh_ph->get_type_of_node()[i] == 2)
         { field[i] = 0.5e0 * ( field[mesh_ph->get_continuity_to_node()[k][0]] 
                              + field[mesh_ph->get_continuity_to_node()[k][1]] ) ;
           k++ ;
         }
       }
//     Dans le cas des noeuds de continuite au centre, la valeur de la methode choisi (ex : Refprop) est preferee
//     ci-dessous l'interpolation pour ces noeuds est faite a p cst 
//     une etude, pourait etre faite, permettant de valider la meilleur methode à utiliser, pour les noeuds de continuité au centre des mailles,
    
//     k=mesh_ph->get_continuity_to_node().size()-1;
//     for (int i=0 ; i<sz ; i++)
//     {
//       if (mesh_ph->get_type_of_node()[i]==3)
//       {
//      field[i]=(field[mesh_ph->get_continuity_to_node()[k][0]]+field[mesh_ph->get_continuity_to_node()[k][1]])/2.0;
//      k--;
//       }
//     }
    return EOS_Error::good ;
  }
 

  int EOS_IGen::check_properties(const AString& property) const
  { 
    char propcov[PROPNAME_MSIZE] ;
    eostp_strcov(property.aschar(), propcov) ;

    if (mesh_ph->get_exist())
       { double tp1 = mesh_ph->get_domain()[1][0] ;
         double tp2 = mesh_ph->get_domain()[0][0] ;

         // properties list effectively used : str_properties
         vector<string>  str_properties ;
         int nb_lps = list_propi.size() ;
         if (nb_lps == 0)
            { // use thermprop
              int sz = thermprop.size() ;
              vector<string>  list_propt ;
              for(int i=0; i<sz; i++)
                 list_propt.push_back(thermprop[i].name) ;
              str_properties = fluid->is_implemented("P", "h", list_propt,  tp1, tp2) ;
            }
         else
            str_properties = fluid->is_implemented("P", "h", list_propi, tp1, tp2) ; // use list_propi

         int nb_sp = str_properties.size() ;
         for (int i=0; i<nb_sp; i++)
            { if (eostp_strcmp(str_properties[i].c_str(), propcov) == 0)  return 2 ;
            }
       }

    if (mesh_p->get_exist())
       { double tp = mesh_p->get_domain()[0][0] ; 

         // saturation curve
         { int nb_lpss = list_propi_sat.size() ;

           // properties list effectively used : str_properties_sat
           vector<string>  str_properties_sat ;
           if (nb_lpss == 0)
              { // use saturprop
                int sz = saturprop.size() ;
                vector<string>  list_propt_sat ;
                for(int i=0; i<sz; i++)
                   list_propt_sat.push_back(saturprop[i].name) ;
                str_properties_sat = fluid->is_implemented("Psat", list_propt_sat, tp) ;
              }
           else
              str_properties_sat = fluid->is_implemented("Psat", list_propi_sat, tp) ; // use list_propi_sat

           int nb_sps = str_properties_sat.size() ;
           for (int i=0; i<nb_sps; i++)
              { if (eostp_strcmp(str_properties_sat[i].c_str(), propcov) == 0)  return 1 ;
              }
         }

         // spinodal curves
         { int nb_lpls = list_propi_lim.size() ;

           // properties list effectively used : str_properties_lim
           vector<string>  str_properties_lim ;
           if (nb_lpls == 0)
              { // use splimprop
                int sz = splimprop.size() ;
                vector<string>  list_propt_lim ;
                for(int i=0; i<sz; i++)
                   list_propt_lim.push_back(splimprop[i].name) ;
                str_properties_lim = fluid->is_implemented("Plim", list_propt_lim, tp) ;
              }
           else
              str_properties_lim = fluid->is_implemented("Plim", list_propi_lim, tp) ; // use list_propi_lim
    
           int nb_spl = str_properties_lim.size() ;
           for (int i=0; i<nb_spl; i++)
              { if (eostp_strcmp(str_properties_lim[i].c_str(), propcov) == 0)  return 1 ;
              }
         }
       }
       
    return 0 ;
  }
  

 /*
 * EOS_IGen::set_quality :
 *              Instanciate an EOS_IGen_QI object, and tag result as relatif
 * double limit_qi         : threshold
 * const char* const prop  : property name to used for quality method
 * int is_abs              : flag for relatif or absolute quality
 */
  void EOS_IGen::set_quality(const char* const property, const char* const type,
                             int const is_abs, double const limit_qi)
  { EOS_IGen_QI qi(property, type, limit_qi, is_abs) ;
    qualities.push_back(qi) ;
  }
  
  
     
  /*
  *  Write in index.eos file (EOS/aata)
  *  EOS_Ipp ; EOS_Ipp_vapor: EOS_Ipp_liquid
  */
  EOS_Error EOS_IGen::write_index(bool tempory)  
  { 
    const char *ref = reference.aschar() ;

    // sub-class  Ipp : sc_ipp
    // State Equation liquid ===> EOS_Ipp_liquid
    // State Equation vapor  ===> EOS_Ipp_vapor
    AString sc_ipp("EOS_Ipp") ;
    if      (strcasestr(ref,"liquid") != NULL)
       sc_ipp += "_liquid" ;
    else if (strcasestr(ref,"vapor")  != NULL)
       sc_ipp += "_vapor" ;
    else
       { cerr << "Error EOS_IGen::write_index : State Equation name not found" << endl ;
         return EOS_Error::error ;
       }

    // MED file without suffixe
    AString smed_file = "" ;
    if (tempory)
       smed_file += tempory_med_name ;
    else
       smed_file += file_med_name ;

    // MED file 
    AString med_file = smed_file ;
    med_file += ".med" ;

    // line to write in index.eos
    AString line = "Ipp " ;
    line += smed_file     ;
    line += " "           ;
    line += sc_ipp        ;
    line += " "           ;
    line += reference     ;
    line += " Unknown 1 " ;
    line += med_file      ;

    // Write in local and global index.eos files : lidx_file, gidx_file
    AString lidx_file  = path_environement ;
    lidx_file         += "/EOS_Ipp/index.eos" ;

    AString gidx_file  = path_environement ;
    gidx_file         += "/index.eos" ;

    std::ifstream ifile(lidx_file.aschar(), std::ios::in) ;
    if (ifile)
       { // existing line
         int found = 0 ;
         std::string iline ;
         while(getline(ifile, iline) && !found)
            { if (strcmp(iline.c_str(),line.aschar()) == 0)  found = 1 ;
            }
         ifile.close() ;

         // write line
         if (!found)
           { std::ofstream stream(lidx_file.aschar(), std::ios::out | std::ios::app) ;
             if (stream)
                { stream << line.aschar() <<endl ;
                  stream.close();
                }

             std::ofstream stream2(gidx_file.aschar(), std::ios::out | std::ios::app) ;
             if (stream2)
                { stream2 << line.aschar() <<endl ;
                  stream2.close() ;
                }
           }
         return good ;
       }
    else
       { cerr << "Error EOS_IGen::write_index : Can't open index.eos file" << endl ;
         return EOS_Error::error ;
       }
  }
  
  
/*
 * EOS_IGen::compute_qualities :
 */
  EOS_Error EOS_IGen::compute_qualities()
  { test_qualities = true ;

    int nb_q = qualities.size() ;
    for (int i=0; i<nb_q; i++)
       { int prop = check_properties(qualities[i].get_property()) ;
         if (prop == 0)
            { cerr << "Error EOS_IGen::compute_qualities :  quality property : " 
                   << qualities[i].get_property()
                   << " not implemented" <<endl ;
              if (!mesh_ph->get_exist())
                 cerr << "          no mesh is created in ph field" <<endl ;
              if (!mesh_p->get_exist())
                 cerr << "          no mesh is created in p field"  <<endl ;
              return EOS_Error::error ;
            }

         else if (prop == 2)
            { EOS_Fields nodes(2) ;
              int sz ;
              if      (qualities[i].get_type() == "centre")
                 sz = mesh_ph->get_nb_mesh() ;
              else if (qualities[i].get_type() == "node")
                 sz = mesh_ph->get_nb_node() ;
              else
                 sz = 0 ;
              ArrOfDouble xp(sz) ;
              ArrOfDouble xh(sz) ;
              EOS_Field p("p","p",xp) ;
              EOS_Field h("h","h",xh) ;
              nodes[1] = p ;
              nodes[0] = h ;
                    
              qualities[i].set_quality_mesh(mesh_ph,nodes) ;
              
              int nb_n0 = nodes[0].size() ;
              ArrOfDouble x_eos(nb_n0) ;
              ArrOfDouble x_ipp(nb_n0) ;
              
              EOS_Field res_ipp(qualities[i].get_property().aschar(), 
                                qualities[i].get_property().aschar(), x_ipp);
              EOS_Field res_eos(qualities[i].get_property().aschar(), 
                                qualities[i].get_property().aschar(), x_eos);
              
              ArrOfInt ierr(sz) ;
              EOS_Error_Field err_ipp(ierr) ;
              EOS_Error_Field err_eos(ierr) ;

              obj_Ipp->compute(nodes[1], nodes[0], res_ipp, err_ipp) ;
              fluid->compute(nodes[1], nodes[0], res_eos, err_eos) ;
              qualities[i].make_quality(res_ipp, res_eos, 
                                        mesh_ph->change_test_qualities_nodes()) ;

              if (! qualities[i].get_test_quality())  test_qualities = false ;
              cout << "In domain ph : "      <<endl<< mesh_ph->get_domain() ;
              cout << "Result with methode " << method << " and reference " 
                   << reference <<endl << res_eos <<endl;
            }
         
         else if (prop == 1)
            { EOS_Fields nodes(1) ;
              int sz;
              if      (qualities[i].get_type() == "centre")
                 sz = mesh_p->get_nb_mesh() ;
              else if (qualities[i].get_type() == "node")
                 sz = mesh_p->get_nb_node() ;
              else
                 sz = 0 ;
              
              ArrOfDouble xp(sz) ;
              EOS_Field p("p","p",xp) ;
              nodes[0] = p ;
              
              qualities[i].set_quality_mesh(mesh_p, nodes) ;
              
              ArrOfDouble x_eos(sz) ;
              ArrOfDouble x_ipp(sz) ;
              
              EOS_Field res_ipp(qualities[i].get_property().aschar(), 
                                qualities[i].get_property().aschar(), x_ipp) ;
              EOS_Field res_eos(qualities[i].get_property().aschar(), 
                                qualities[i].get_property().aschar(), x_eos) ;
              
              ArrOfInt ierr(sz) ;
              EOS_Error_Field err_ipp(ierr) ;
              EOS_Error_Field err_eos(ierr) ;
              
              obj_Ipp->compute(nodes[0], res_ipp, err_ipp) ;
              fluid->compute(nodes[0], res_eos, err_eos)   ;
                      
              qualities[i].make_quality(res_ipp, res_eos, mesh_p->change_test_qualities_nodes()) ;
              
              if (! qualities[i].get_test_quality())  test_qualities = false ;
              cout<< "In domain p : " <<endl<< mesh_p->get_domain() ;
              cout<< " Result with methode " << method << " and reference " << reference 
                  <<endl<< res_eos <<endl ;
            }

         cout << "property : "     << qualities[i].get_property_number()      << endl ;
         cout << "Have quality : " << qualities[i].get_quality_nodes() << endl<<endl ;
       }
    return EOS_Error::good ;
  }
  
  
/*
 * EOS_IGen::make_global_refine :
 */
  EOS_Error EOS_IGen::make_global_refine()
  { int level = 0 ;
    bool test_level_p  = false ;
    bool test_level_ph = false ;
    EOS_Error err  ;
    
    err = compute_qualities() ;
    if (err != EOS_Error::good)
       { cerr<< "Error EOS_IGen::make_global_refine : compute_qualities not made" <<endl ;
         return err ;
       }

    while (!test_qualities)
       { if (mesh_p->get_exist() && (mesh_p->get_level_max() != level))
            mesh_p->add_global_nodes() ;
         else
            test_level_p = true ;
         
         if ( (mesh_ph->get_exist()) && (mesh_ph->get_level_max() != level) )
            mesh_ph->add_global_nodes() ;
         else
            test_level_ph = true ;
         
         if (test_level_p && test_level_ph)  break ;
         
         err = write_tempory_med() ;
         if (err != EOS_Error::good)
            { cerr << "Error EOS_IGen::make_global_refine : no temporary MED file writed" <<endl ;
              return err ;
            }
         
         delete obj_Ipp ;
         set_obj_Ipp() ;

         err = compute_qualities() ;
         if (err != good)
            { cerr<< "Error EOS_IGen::make_global_refine : compute_qualities not made" <<endl ;
              return err ;
            }
            
         level++;
       }
    return EOS_Error::good ;
  }
  

/*
 * EOS_IGen::make_local_refine :
 */
  EOS_Error EOS_IGen::make_local_refine(bool cont)
  { int level = 0 ;
    bool test_level_p  = false ;
    bool test_level_ph = false ;
    EOS_Error err ;
    
    err = compute_qualities() ;
    if (err != EOS_Error::good)
       { cerr<< "Error EOS_IGen::make_local_refine : compute_qualities not made" <<endl ;
         return err ;
       }
    
    while (!test_qualities)
       { if ( (mesh_p->get_exist()) && (mesh_p->get_level_max() != level) )
            mesh_p->add_local_nodes(level, cont) ;
         else
            test_level_p = true ;
         if ( (mesh_ph->get_exist()) && (mesh_ph->get_level_max() != level) )
            { mesh_ph->add_local_nodes(level, cont) ;
              if (cont)  mesh_ph->add_continuity_nodes(level) ;
            }
         else
            test_level_ph = true ;
         
         if (test_level_p && test_level_ph)  break ;
         refine = true ;
         err = write_tempory_med() ;
         if (err != EOS_Error::good)
            { cerr << "Error EOS_IGen::make_local_refine : no temporary MED file writed" <<endl ;
              return err ;
            }
         
         delete obj_Ipp ;
         set_obj_Ipp() ;

         err = compute_qualities() ;
         if (err != EOS_Error::good)
            { cerr << "Error EOS_IGen::make_local_refine : compute_qualities not made" <<endl ;
              return err ;
            }

         level++ ;
       }
    return EOS_Error::good ;
  }
  

  const EOS_IGen& EOS_IGen::operator=(const EOS_IGen& right)
  { method    = right.method    ;
    reference = right.reference ;

    pmin = right.pmin ;
    pmax = right.pmax ;
    Tmin = right.Tmin ;
    Tmax = right.Tmax ;
    hmin = right.hmin ;
    hmax = right.hmax ;
    
    memory_max = right.memory_max ;
    
    mesh_p  = right.mesh_p  ;
    mesh_ph = right.mesh_ph ;
    
    IGen_handler = right.IGen_handler ;
    
    fluid     = right.fluid     ;
    qualities = right.qualities ;
    
    return *this ;
  }
  
} 
