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
 * EOS_Med.cxx
 *
 *  Created on: 22 juin 2010
 */


#include <string.h>
#include "EOS_Med.hxx"
#define DBL_EPSILON 1e-9

namespace NEPTUNE
{
 
 
 EOS_Med::EOS_Med()
 {
 }
 
 EOS_Med::EOS_Med(AString& name):
 file(name)
 {
 }
 
 EOS_Med::~EOS_Med()
 {
 }
 
 /*
  * EOS_Med::add_Maillage :
  *              create new mesh (type MED_UNSTRUCTURED_MESH) and add default family ("FAMILLE 0")
  *
  * AString& m_name      : domain name (sat_domain or lim_domain)
  * AString& description : file name
  * int dim              : mesh dimension
  *
  * return : EOS_Error
  */
 EOS_Error EOS_Med::add_Maillage(AString& m_name, AString& description, int dim)
 { /* Creation d'un maillage de type MED_UNSTRUCTURED_MESH
          et de dimension dim
    */
 
   char maillage_name[MED_NAME_SIZE+1] ;
   strcpy(maillage_name, m_name.aschar()) ;
 
   // maillage_dim : cette dimension est inférieure ou égale
   // à celle de la dimension de l'espace de calcul (spacedim)
   // (on peut avoir un maillage 2D dans un espace de calcul 3D) ;
   med_int spacedim = dim ;
   med_int maillage_dim = dim ;
 
   // A voir si on met un med_axis_type = MED_CARTESIAN 
   // avec p et h comme nom au axes respectifs (3 derniere elements de la signature).
   if (MEDmeshCr(fid,
                   maillage_name,
                   spacedim,
                   maillage_dim,
                   MED_UNSTRUCTURED_MESH,
                   description.aschar(),
                   MED_SNAME_BLANK, /*pas de dtunit*/
                   MED_SORT_ITDT, /*choix obligatoire même si inutile*/
                   MED_UNDEF_AXIS_TYPE,
                   MED_SNAME_BLANK,
                   MED_SNAME_BLANK )< 0)
   { cerr << "Erreur a la creation du maillage " << maillage_name << endl ;
     return EOS_Error::error ;
   }
 
   //Creation de la famille 0 (par defaut)
   //nom_maillage, nom_fam, num_fam
   AString fam_name("FAMILLE_0") ;
   add_Famille(m_name, fam_name, 0) ;
 
   return EOS_Error::good ;
 }
 
 /*
  * EOS_Med::add_Nodes :
  *              add nodes to mesh
  *
  * AString& m_name      : mesh name
  * int dim              : mesh dimension
  * EOS_Fields& nodes    : nodes to add
  *
  * return : EOS_Error
  */
 EOS_Error EOS_Med::add_Nodes(AString& m_name, int dim, const EOS_Fields& nodes)
 {
   char maillage_name[MED_NAME_SIZE+1] ;
   strcpy(maillage_name, m_name.aschar()) ;
 
   /* Ecriture des noeuds du maillage */
   med_int npt = nodes[0].size() ;   // le nombre de points (noeuds)
   long int ptxy_size = npt*dim ;    // Attention : int max : 32767 -> Pour avoir une meilleur capacité de stockage, il faudrait changer les int en long unsigned int
 
   /* table des coordonnees  profil : (dimension * nombre de noeuds) */
   /* table des noms et unites des coordonnees */
   med_float *ptxy = new med_float[ptxy_size] ;
           
   int k = 0 ;
   for (int i=0; i<dim; i++)
      { for (int j=0;j<npt;j++)
           { ptxy[k] = nodes[i][j] ;
             k++ ;
           }
      }
 
 //voir peut etre MEDmeshNodeCoordinatewithprofileWr
   if (MEDmeshNodeCoordinateWr(fid,
              maillage_name,
              MED_NO_DT,
              MED_NO_IT,
              0.0,
              MED_NO_INTERLACE,
              npt,
              ptxy) < 0)
      {
        delete[] ptxy ;
        cerr << "Erreur a l'ecriture des coordonnees des noeuds" << endl ;
        return EOS_Error::error ;
      }

   delete [] ptxy;
   return EOS_Error::good ;
 }
 
 /*
  * EOS_Med::add_Nodes2Fam :
  *              associate nodes to family
  *
  * AString& m_name      : mesh domain
  * ArrOfOInt& nodes_f   : family number for each node
  *
  * return : EOS_Error
  * inutile inutile inutile inutile
  */
 EOS_Error EOS_Med::add_Nodes2Fam(AString& m_name, const ArrOfInt& nodes_f)
 {
   /* Ecriture des numeros de famille des noeuds */
   int nb_nodes = nodes_f.size() ;
   med_int *nufano = new med_int[nb_nodes] ;
 
   for (int i=0; i<nb_nodes; i++)
      nufano[i] = nodes_f[i] ;
 
   if (MEDmeshEntityFamilyNumberWr(fid,
              m_name.aschar(),
              MED_NO_DT,
              MED_NO_IT,
              MED_NODE,
              MED_POINT1,
              nb_nodes,
              nufano) < 0)
      {
        delete[] nufano ;
        cerr << "Erreur a l'ecriture des numeros de familles des noeuds" << endl ;
        return EOS_Error::error ;
      }
 
   delete[] nufano ;
   return EOS_Error::good ;
 }
 
 /* EOS_Med::add_Famille :
  *              Create MED Famille without attribute and without groupes
  *
  * AString& m_name : mesh domain
  * AString& f_name : family name
  * int fam_num     : family number
  *
  * return : EOS_Error
  */ 
 EOS_Error EOS_Med::add_Famille(AString& m_name, AString& f_name, int fam_num)
 {
   char maillage_name[MED_NAME_SIZE+1] ;
   char fam_name[MED_NAME_SIZE+1]  = "" ;
   char gro[MED_LNAME_SIZE+1]      = "" ;
   
   strcpy(maillage_name, m_name.aschar()) ;
   strcpy(fam_name,f_name.aschar()) ;
 
   if (MEDfamilyCr(fid,
                maillage_name,
                fam_name,
                fam_num,
                0,
                gro) < 0)
      { cerr << "Erreur a la creation de la famille" << fam_name << endl ;
        return EOS_Error::error ;
      }
 
   return EOS_Error::good ;
 }
 
 /* EOS_Med::add_Champ_Noeud :
  *              Add float MED "champ"
  *
  * AString& m_name   : mesh domain
  * EOS_Fields& champ : floating values
  *
  * return : EOS_Error
  */
 EOS_Error EOS_Med::add_Champ_Noeud(AString& m_name, EOS_Field& champ)
 {
   med_int champ_comp_nb = 1 ;
 
   char champ_name[MED_NAME_SIZE+1] ;
   char mesh_name[MED_NAME_SIZE+1]  ;
   char champ_comp_nom[MED_SNAME_SIZE+1] = MED_SNAME_BLANK ;

   strcpy(champ_name, champ.get_propname_int().aschar()) ;
   strcpy(mesh_name, m_name.aschar()) ;
   strncpy(champ_comp_nom, champ_name, (MED_SNAME_SIZE)) ; //pour avoir la bonne taille du char
   med_int nval = champ.size() ;
   
   if ( MEDfieldCr(fid,
                   champ_name,
                   MED_FLOAT64,
                   champ_comp_nb,
                   champ_comp_nom,
                   MED_SNAME_BLANK,
                   MED_SNAME_BLANK, /*pas de dtunit*/
                   mesh_name) < 0)
      { cerr << "EOS_Med::add_Champ_Noeud: Erreur a la creation du champ : " << champ_name << endl ;
        return EOS_Error::error ;
      }
 
   med_float *val_prop = new med_float[nval] ; // long + mettre un new 
   for (int i=0 ;i<nval; i++)
     val_prop[i] = champ[i] ;
 
   if ( MEDfieldValueWr(fid,
                champ_name,
                MED_NO_DT,
                MED_NO_IT,
                0.0,
                MED_NODE,    //type entite
                MED_NONE,     //type geometrique
                MED_NO_INTERLACE, //switch mode
                champ_comp_nb, //component select
                nval,
                (unsigned char*)val_prop
                 ) < 0)
      {
        delete[] val_prop ;
        cerr << "Erreur à l'ecriture du champ : " << champ_name << endl ;
        return EOS_Error::error ;
      }
   delete [] val_prop;
   delete[] val_prop ;
   return EOS_Error::good ;
 }
 
 /* EOS_Med::add_ErrChamp_Noeud :
  *              Add int MED "champ"
  *
  * AString& m_name   : mesh domain
  * EOS_Fields& champ : int values (error values)
  *
  * return : EOS_Error
  */
 EOS_Error EOS_Med::add_ErrChamp_Noeud(AString& m_name, AString& e_name , EOS_Error_Field& err)
 {
   med_int champ_comp_nb = 1 ;
 
   char champ_name[MED_NAME_SIZE+1] ;
   char mesh_name[MED_NAME_SIZE+1]  ;
   char champ_comp_nom[MED_SNAME_SIZE+1] = MED_SNAME_BLANK ;

   char propcov[MED_NAME_SIZE+1] ;  // property name modified for Med
   eostp_strcov(e_name.aschar(), propcov) ;

   char adpropcov[MED_NAME_SIZE+1] ;
   strcpy(adpropcov, "IE ") ;
   strcat(adpropcov, propcov) ;

   strcpy(champ_name, adpropcov) ;
   strcpy(mesh_name, m_name.aschar())  ;
   strncpy(champ_comp_nom, adpropcov, (MED_SNAME_SIZE) ) ; //pour avoir la bonne taille du 
   med_int nval = err.size() ;
 
   if ( MEDfieldCr(fid,
                   champ_name,
                   MED_INT32,
                   champ_comp_nb,
                   champ_comp_nom,
                   MED_SNAME_BLANK,
                   MED_SNAME_BLANK, /*pas de dtunit*/
                   mesh_name) < 0)
      { cerr << "EOS_Med::add_ErrChamp_Noeud: Erreur a la creation du champ : " << champ_name << endl ;
        return EOS_Error::error ;
      }
 
   med_int *val_prop = new med_int[nval] ;
   for (int i=0;i<nval;i++)
      val_prop[i] = err[i].get_code() ;
 
   if ( MEDfieldValueWr(fid,
                   champ_name,
                   MED_NO_DT,
                   MED_NO_IT,
                   0.0,
                   MED_NODE,    //type entite
                   MED_NONE,     //type geometrique
                   MED_NO_INTERLACE, //switch mode
                   champ_comp_nb, //component select
                   nval,
                   (unsigned char*) val_prop
                    ) < 0)
   
      {
        delete[] val_prop ;
        cerr << "Erreur à l'ecriture du champ : " << champ_name << endl ;
        return EOS_Error::error ;
      }
   
   delete[] val_prop ;
   return EOS_Error::good ;
 }
 
 /* EOS_Med::add_Scalar_Float :
  *              Add float MED "scalar" to stored some values
  *
  * AString& sf_name : scalar name
  * double&  value   : value
  *
  * return : EOS_Error
  */
 EOS_Error EOS_Med::add_Scalar_Float(AString& sf_name, double& value)
 { char scalar_name[MED_NAME_SIZE+1]  ;
   char desc[MED_COMMENT_SIZE+1] = "" ;
   med_float val = value ;
 
   strcpy(scalar_name, sf_name.aschar()) ;
 
   if (MEDparameterCr(fid,
                   scalar_name,
                   MED_FLOAT64,
                   desc,
                   MED_SNAME_BLANK /*pas de dtunit*/
                   ) < 0)
      { cerr << "Error creating scalar float variable" << endl ;
        return EOS_Error::error ;
      }
 
   if (MEDparameterValueWr(fid,
                   scalar_name,
                   MED_NO_DT,
                   MED_NO_IT,
                   0.0,
                   (unsigned char*) &val
                   ) < 0)
      { cerr << "Error writing scalar float variable" << endl ;
        return EOS_Error::error ;
      }
 
   return EOS_Error::good ;
 }
 
 /* EOS_Med::add_Connectivity_1D :
  *              Add connectivity for segments
  *              EOS_Fields node must be sort
  *
  * AString m_name          : mesh name
  * const EOS_Field& nodes  : nodes
  *
  * return : EOS_Error
  */
 EOS_Error EOS_Med::add_Connectivity_1D(AString& m_name,const EOS_Field& nodes)
 {
   int nb_ns = nodes.size() ;
   int nb_seg = nb_ns-1 ;
 
   med_int *conn = new med_int[nb_seg*2] ;
 
   int j = 0 ;
   for (int i=1; i<nb_ns; i++)
      { conn[j]   = i ;
        conn[++j] = i+1 ;
        j++ ;
      }
 
   if (MEDmeshElementConnectivityWr(fid,
                   m_name.aschar(),
                   MED_NO_DT,
                   MED_NO_IT,
                   0.0,
                   MED_CELL, //MED_DESCENDING_FACE,
                   MED_SEG2,
                   MED_NODAL, //MED_DESCENDING,
                   MED_NO_INTERLACE,
                   nb_seg,
                   conn
                   ) < 0)
      {
        delete[] conn ;
        cerr << "Erreur a l'ecriture de la connectivite des ségments MED_SEG2 (1D)" << endl ;
        return EOS_Error::error ;
      }
        delete [] conn;
 
   delete[] conn ;
   return good ;
 }
 
 /* EOS_Med::add_Connectivity_Refine_2D :
  *              Add connectivity for cell (no refinement. a cell is defined by 4 nodes)
  *
  * AString m_name : mesh name
  * int mesh_x     : number of mesh in line
  *
  * return : EOS_Error
  */
 EOS_Error EOS_Med::add_Connectivity_Refine_2D(AString& m_name, const vector<ArrOfInt>& med_to_node)
 {
   int nb_mtn = med_to_node.size() ;
   int sz = (nb_mtn+1)*4 ;
 
   med_int *conn = new med_int[sz] ;
 //med_to_node.size()+1 : ajout d'une maille fictive à la fin sinon derniere maille pas pris en compte
   med_int index_nb = nb_mtn+1 ;
   med_int *index = new med_int[index_nb] ;
   int i = 0 ;
   int j = 0 ; //index pour index[]
   while (j < nb_mtn)
      { index[j]  = i+1 ;
        conn[i]   = med_to_node[j][0] + 1 ;
        conn[i+1] = med_to_node[j][1] + 1 ;
        conn[i+2] = med_to_node[j][3] + 1 ;
        conn[i+3] = med_to_node[j][2] + 1 ;
        j++ ;
        i += 4 ;
      }
 //affectation de la maille fictive sinon plantage pc
   index[nb_mtn] = i+1 ;
   conn[i]   = med_to_node[nb_mtn-1][0] + 3 ;
   conn[i+1] = med_to_node[nb_mtn-1][1] + 4 ;
   conn[i+2] = med_to_node[nb_mtn-1][3] + 6 ;
   conn[i+3] = med_to_node[nb_mtn-1][2] + 5 ;
 
   if (MEDmeshPolygonWr(fid,
              m_name.aschar(),
              MED_NO_DT,
              MED_NO_IT,
              0.0,
              MED_CELL,
              MED_NODAL,
              index_nb,
              index,
              conn
              ) < 0)
      {
        delete[] conn ;
        delete[] index ;
        cerr << "Erreur a l'ecriture de la connectivite des mailles MED_POLYGON " << m_name.aschar() << endl ;
        return EOS_Error::error ;
      }
 
   delete[] conn ;
   delete[] index ;
   return EOS_Error::good ;
 }
 
 
 /* EOS_Med::add_Connectivity_NoRef_2D :
  *              Add connectivity for cell (no refinement. a cell is defined by 4 nodes)
  *
  * AString m_name : mesh name
  * int nb_cell    : number of cells
  * int mesh_x     : number of mesh in line
  *
  * return : EOS_Error
  */
 EOS_Error EOS_Med::add_Connectivity_NoRef_2D(AString& m_name, int nb_cell, int mesh_x)
 {
   int sz = nb_cell*4 ;
 
   med_int *conn = new med_int[sz] ;
   med_int index_nb = nb_cell+1 ;
   med_int *index = new med_int[index_nb] ;
   int nb_nodes_x = mesh_x + 1 ;
   int i = 0 ;
   int j = 0 ; //index pour index[]
   int k = 1 ; //count pour le numéro des points
   while (j < nb_cell)
      { index[j]  = i+1 ;
        conn[i]   = k ;
        conn[i+1] = ++k ;
        conn[i+2] = conn[i+1] + nb_nodes_x ;
        conn[i+3] = conn[i]   + nb_nodes_x ;
        j++ ;
        i += 4 ;
        if ((k%nb_nodes_x) == 0)  k++ ;
      }
   index[j] = i+1 ;
 
   if (MEDmeshPolygonWr(fid,
              m_name.aschar(),
              MED_NO_DT,
              MED_NO_IT,
              0.0,
              MED_CELL,
              MED_NODAL,
              index_nb,
              index,
              conn
              ) < 0)
      {
        delete[] conn ;
        delete[] index ;
        cerr << "Erreur a l'ecriture de la connectivite des mailles MED_POLYGON " << m_name.aschar() << endl ;
        return EOS_Error::error ;
      }
 
   delete[] conn ;
   delete[] index ;
   return EOS_Error::good ;
 }
 
 
 /* EOS_Med::get_nodes :
  *              get nodes of a mesh
  *
  * AString m_name    : mesh name
  * int dim           : mesh dimension
  * EOS_Fields& nodes : values of the nodes to return
  *
  * return : EOS_Error
  */
 EOS_Error EOS_Med::get_nodes(AString& m_name, int dim, EOS_Fields& nodes)
 {
   med_float *coo ; 
 //   char nomcoo[2*MED_SNAME_SIZE+1];
 //   char unicoo[2*MED_SNAME_SIZE+1];
   
   med_int nb_nodes = nodes[0].size() ;
 //   coo = (med_float*) malloc(nb_nodes*dim*sizeof(med_float));
   coo = new med_float[nb_nodes*dim] ;
   char mesh_name[MED_NAME_SIZE+1] ;
   strcpy(mesh_name, m_name.aschar()) ;
   if (MEDmeshNodeCoordinateRd(fid,
              m_name.aschar(),
              MED_NO_DT,
              MED_NO_IT,
              MED_NO_INTERLACE,
              coo
              ) < 0)
      {
        delete[] coo ;
        cerr << "Erreur a la lecture des coordonnees des noeuds du maillage " 
             << m_name.aschar() << endl ;
        return EOS_Error::error ;
      }
   
   if (dim == 1)
      { for (int i=0; i<nb_nodes; i++)
           nodes[0][i] = coo[i] ;
      }
   else if (dim == 2)
      { if (nodes[0].get_propname_int() == "p")
           { for (int i=0; i<nb_nodes;i++)
                { nodes[0][i] = coo[i+nb_nodes] ;
                  nodes[1][i] = coo[i] ;
                }
           }
        else
           { for (int i=0; i<nb_nodes;i++)
                { nodes[0][i] = coo[i] ;
                  nodes[1][i] = coo[i+nb_nodes] ;
                }
           }
        
      }
   else
     {
       delete[] coo ;
       return EOS_Error::error ;
     }
   
   delete [] coo ;
   
   return EOS_Error::good ;
 }
 
 /* EOS_Med::get_Connectivity_1D :
  *              return connectivity array
  *
  * AString m_name  : mesh name
  * ArrOfInt& res   : array with nodes index
  *
  * return : EOS_Error
  */
 EOS_Error EOS_Med::get_Connectivity_1D(AString& m_name, ArrOfInt& res)
 {
   int nb_seg ;
   med_int *segm ;
 
   EOS_Error err  = get_entity_size(m_name,nb_seg, MED_CELL,MED_SEG2,MED_CONNECTIVITY,MED_NODAL);
   if (err != EOS_Error::good)  return err ;
 
   segm = (med_int*) malloc(sizeof(med_int)*2*nb_seg) ;
 
   /* Lecture de la connectivite des segments */
   if (MEDmeshElementConnectivityRd(fid,
              m_name.aschar(),
              MED_NO_DT,
              MED_NO_IT,
              MED_CELL, //MED_DESCENDING_FACE,
              MED_SEG2,
              MED_NODAL, //MED_DESCENDING,
              MED_NO_INTERLACE,
              segm
              ) < 0)
      { cerr << "EOS_Med::get_Connectivity_1D : Erreur a la lecture de la connectivite des segments" << endl ;
        return EOS_Error::error ;
      }
   res.resize(nb_seg*2) ;
   for (int i=0; i<nb_seg*2; i++)
      res[i] = segm[i]-1 ;
    
   free(segm) ;

   return EOS_Error::good ;
 }
 
 /* EOS_Med::get_Connectivity_2D :
  *              return connectivity information (2 arrays)
  *              Warning : in med first index is 1. In returned values first index is 0
  *
  * AString m_name    : mesh name
  * ArrOfInt& ires    : connectivity index (index of the first node of cell in cres)
  * ArrOfInt& cres    : connectivity (all nodes of each cell)
  *
  * return : EOS_Error
  */
 EOS_Error EOS_Med::get_Connectivity_2D(AString& m_name, ArrOfInt& ires, ArrOfInt& cres)
 {
   int n_poly, taille ;
   med_int *con, *index ;
 
   EOS_Error err  = get_entity_size(m_name,n_poly, MED_CELL,MED_POLYGON,MED_INDEX_NODE,MED_NODAL) ;
   if (err != EOS_Error::good)
      { cerr << "EOS_Med::get_Connectivity_2D: Erreur a la lecture de la taille du tableau de connectivité : "
             << m_name << endl;
        return err ;
      }
   err  = get_entity_size(m_name,taille, MED_CELL,MED_POLYGON,MED_CONNECTIVITY,MED_NODAL) ;
   if (err != EOS_Error::good)
      { cerr << "EOS_Med::get_Connectivity_2D: Erreur a la lecture de la taille du tableau des noeuds du maillage" << endl ;
        return err ;
      }
 
   index = (med_int *) malloc(sizeof(med_int)*(n_poly)) ;
   con   = (med_int *) malloc(sizeof(med_int)*(taille)) ;
 
   /* Lecture de la connectivite des mailles polygones */
   if (MEDmeshPolygonRd(fid,
              m_name.aschar(),
              MED_NO_DT,
              MED_NO_IT,
              MED_CELL,
              MED_NODAL,
              index,
              con
              ) < 0)
      { cerr << "Erreur a la lecture de la connectivite du maillage " << m_name.aschar() << endl ;
        return EOS_Error::error ;
      }
 
   ires.resize(n_poly) ;
   cres.resize(taille) ;
   
   for (int i=0; i<n_poly; i++)
      ires[i] = index[i]-1 ;
   
   
   for (int i=0; i<taille; i++)
      cres[i] = con[i]-1 ;
   
   free(index) ;
   free(con)   ;

   return EOS_Error::good ;
 }
 
 /* EOS_Med::get_Champ_Noeud_Infos :
  *              return med champ name, type, associated mesh and nb of values
  *
  * int index       : mesh name
  * AString& c_name : med "champ" name
  * int& type       : FLOAT == 1 ; INT == 2
  * int& nb_val     : number of values
  * AString& m_ass  : associated mesh
  *
  * return : EOS_Error
  */
 EOS_Error EOS_Med::get_Champ_Noeud_Infos(int index, AString& c_name, int& type, int& nb_val, AString& m_ass)
 { med_int ncomp = 1 ;
   med_int nval ;
   med_field_type typcha ;
   med_bool local ;
   char comp[MED_SNAME_SIZE+1]   ;
   char unit[MED_SNAME_SIZE+1]   ;
   char dtunit[MED_SNAME_SIZE+1] ;
   char m_name[MED_NAME_SIZE+1]  ;
   char nomcha[MED_NAME_SIZE+1]  ;
 
   //get champ name + unit
   if ( MEDfieldInfo(fid,
                index+1,
                nomcha,
                m_name,
                &local,
                &typcha,
                comp,
                unit,
                dtunit,
                &ncomp
                ) < 0 )
      { cerr << "Erreur a la demande d'information sur le champ : " ;
        cerr << index+1 << " " << nomcha << " " << typcha << " "
             << comp << " " << unit << " " << ncomp << endl ;
        return EOS_Error::error ;
      }
 
   c_name = AString(nomcha) ;
 
   if (typcha == MED_FLOAT64)
       type = 1 ;
   else
       type = 2 ;
 
   m_ass = AString(m_name) ;
   //get number of values
   nval = MEDfieldnValue(fid, nomcha, MED_NO_DT, MED_NO_IT, MED_NODE, MED_NONE) ;
 
   nb_val = nval ;

   return EOS_Error::good ;
 }
 
 /* EOS_Med::get_Champ_Noeud :
  *              get float nodes values of a med CHAMP
  *
  * AString& c_name : med "champ" name
  * EOS_Field& res  : float values
  * AString& m_ass  : associated mesh [not used]
  *
  * return : EOS_Error
  */
 EOS_Error EOS_Med::get_Champ_Noeud(AString& c_name, EOS_Field& res)
 { int nb_rs = res.size() ;
   med_float *val = new med_float[nb_rs] ;
 
   if ( MEDfieldValueRd(fid,
                c_name.aschar(),
                MED_NO_DT,
                MED_NO_IT,
                MED_NODE,
                MED_NONE,
                MED_NO_INTERLACE,
                MED_ALL_CONSTITUENT,
                (unsigned char*)val
                ) < 0)
      {
        delete[] val;
        cerr << "Error ! Can't read \"champ\" : " << c_name.aschar() << endl ;
        return EOS_Error::error ;
      }
 
   ArrOfDouble x ;
   x.set_ptr(nb_rs, res.get_data().get_ptr()) ;
 
   for (int i=0; i<nb_rs; i++)
      x[i] = val[i] ;
 
   delete [] val ;

   return EOS_Error::good ;
 }
 
 /* EOS_Med::get_Champ_Noeud :
  *              get integer nodes values of a med CHAMP
  *
  * AString& c_name       : med "champ" name
  * EOS_Error_Field& res  : integer values  == EOS_Internal_Error
  * AString& m_ass        : associated mesh [not used]
  *
  * return : EOS_Error
  */
 EOS_Error EOS_Med::get_ErrChamp_Noeud(AString& c_name, EOS_Error_Field& res)
 { int nb_rs = res.size() ;
   med_int *val = new med_int[nb_rs] ;
 
   if ( MEDfieldValueRd(fid,
                c_name.aschar(),
                MED_NO_DT,
                MED_NO_IT,
                MED_NODE,
                MED_NONE,
                MED_NO_INTERLACE,
                MED_ALL_CONSTITUENT,
                (unsigned char*)val
                ) < 0)
      {
        delete[] val;
        cerr << "Error ! Can't read \"champ\" : " << c_name.aschar() << endl ;
        return EOS_Error::error ;
      }
 
   for (int i=0; i<nb_rs; i++)
      res.set(i,val[i]) ;
 
   delete [] val ;
 
   return EOS_Error::good ;
 }
 
 /* EOS_Med::get_Scalar_Float :
  *              get scalar values
  *
  * AString& sf_name : scalar name
  * double& res      : value
  *
  * return : EOS_Error
  */
 EOS_Error EOS_Med::get_Scalar_Float(AString& sf_name, double& res)
 { double valr[1] ;
 
   if (MEDparameterValueRd(fid,
                   sf_name.aschar(),
                   MED_NO_DT,
                   MED_NO_IT,
                   (unsigned char*)valr
                   ) < 0)
      { cerr << "Erreur reading float value " << sf_name.aschar() << endl ;
        return EOS_Error::error ;
      }
   res = valr[0] ;
   return EOS_Error::good ;
 }


}
