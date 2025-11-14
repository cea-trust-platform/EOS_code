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
 * EOS_Mesh.cxx
 *
 *  Created on: 17 mai 2010
 */

#include "EOS_IGen/Src/EOS_Mesh.hxx"
#include "EOS/Src/EOS_Ipp/EOS_Ipp.hxx"

#include <math.h>
#include <fstream>
#include <string.h>

using namespace NEPTUNE_EOS ;
using namespace NEPTUNE ;

namespace NEPTUNE_EOS_IGEN
{

  //constructor for ph domain
  EOS_Mesh::EOS_Mesh() :
  exist(false)
  {
  }
  
  EOS_Mesh::EOS_Mesh(int n_p, int n_h, 
                     double pmin, double pmax, 
                     double hmin, double hmax, int l_max) :
  exist(true),
  nb_p(n_p),
  nb_h(n_h),
  level_max(l_max),
  delta_p((pmax-pmin)/double(n_p-1)),
  delta_h((hmax-hmin)/double(n_h-1)),
  nb_mesh((n_p-1)*(n_h-1)),
  nb_node(n_p*n_h),
  nb_continuity(0)
  { int nn  = nb_p*nb_h ;
    int nn1 = (nb_p-1)*(nb_h-1) ;

    node_p.resize(nn) ;                 node_p               = 0.e0 ;
    node_h.resize(nn) ;                 node_h               = 0.e0 ;
    node_p_continuity.resize(nn) ;      node_p_continuity    = 0.e0 ;
    node_h_continuity.resize(nn) ;      node_h_continuity    = 0.e0 ;
    type_of_node.resize(nn) ;           type_of_node         = 0 ;
    test_qualities_nodes.resize(nn1) ;  test_qualities_nodes = 1 ;
    node_glb.resize(nn) ;               node_glb             = 1 ;
    continuity_node.resize(nn) ;        continuity_node      = 0 ;

    set_nodes(pmin, hmin) ;
    
    EOS_Field p("Pressure", "p", NEPTUNE::p, node_p) ;
    EOS_Field h("Enthalpy", "h", NEPTUNE::h, node_h) ;
    EOS_Field p_continuity("Pressure", "p", NEPTUNE::p, node_p_continuity) ;
    EOS_Field h_continuity("Enthalpy", "h", NEPTUNE::h, node_h_continuity) ;

    domain.resize(2) ;
    set_domain_ph(p, h) ;

    domain_continuity.resize(2) ;
    set_domain_continuity_ph(p_continuity, h_continuity) ;

    init_mesh_to_node() ;
  }
  
   
  //constructor for p domain
  EOS_Mesh::EOS_Mesh(int n_p, double pmin, double pmax, int l_max) :
  exist(true),
  nb_p(n_p),
  level_max(l_max),
  delta_p((pmax-pmin)/double(n_p-1)),
  nb_mesh(n_p-1),
  nb_node(n_p)
  { node_p.resize(nb_p) ;                  node_p               = 0.e0 ;
    test_qualities_nodes.resize(nb_p-1) ;  test_qualities_nodes = 1 ;
    node_glb.resize(nb_p) ;                node_glb             = 1 ;

    set_nodes(pmin) ;

    EOS_Field p("Pressure", "p", NEPTUNE::p, node_p) ;

    domain.resize(1) ;
    set_domain_p(p)  ;

    init_mesh_to_node() ;
  }
  
  EOS_Mesh::~EOS_Mesh()
  {
  }
  
  void EOS_Mesh::init_mesh_to_node()
  { int k = 0 ;
    int ndomain = domain.size() ;
    for (int i=0; i<nb_mesh; i++)
       { if (ndomain == 2)
            { ArrOfInt node(4) ;
              node[0] = i+k        ;
              node[1] = i+k+1      ;
              node[2] = i+k+nb_h   ;
              node[3] = i+k+nb_h+1 ;
              if ((i+1)%(nb_h-1) == 0)  k++ ;
              mesh_to_node.push_back(node) ;
              med_to_node.push_back(node)  ;
            }
         else
            { ArrOfInt node(2) ;
              node[0] = i+k   ;
              node[1] = i+k+1 ;
              mesh_to_node.push_back(node) ;
              med_to_node.push_back(node)  ;
            }
         
       }
  }
  
  void EOS_Mesh::create_mesh(int n_p, int n_h,
                             double pmin, double pmax,
                             double hmin, double hmax)
  { nb_p = n_p ;
    nb_h = n_h ;

    delta_p = (pmax-pmin) / (double(nb_p-1)) ;
    delta_h = (hmax-hmin) / (double(nb_h-1)) ;
    
    node_p.resize(nb_p) ;
    node_h.resize(nb_h) ;

    exist = true ;
    set_nodes(pmin, hmin) ;
    
    EOS_Field p("Pressure", "p", NEPTUNE::p, node_p) ;
    EOS_Field h("Enthalpy", "h", NEPTUNE::h, node_h) ;
    
    domain.resize(2) ;
    set_domain_ph(p, h) ;
  }
  
    
  
  void EOS_Mesh::create_mesh(int n_p, double pmin, double pmax)
  { nb_p = n_p ;

    delta_p = (pmax-pmin) / (double(nb_p-1)) ;
    node_p.resize(nb_p) ;
    exist = true ;
    
    set_nodes(pmin) ;
    
    EOS_Field p("Pressure", "p", NEPTUNE::p, node_p) ;

    domain.resize(1) ;
    set_domain_p(p)  ;
  }
       
  
  void EOS_Mesh::add_global_nodes()
  { int ndomain = domain.size() ;
  
    if (ndomain == 2)
       { nb_p = nb_p*2 - 1 ;
         nb_h = nb_h*2 - 1 ;
               
         double pmin = domain[1][0] ;
         double hmin = domain[0][0] ;
         double pmax = domain[1][domain[1].size()-1] ;
         double hmax = domain[0][domain[0].size()-1] ;
         
         delta_p = (pmax-pmin)/double(nb_p-1) ;
         delta_h = (hmax-hmin)/double(nb_h-1) ;
         
         node_p.resize(nb_p*nb_h) ;
         node_h.resize(nb_p*nb_h) ;
         node_p_continuity.resize(nb_p*nb_h) ;
         node_h_continuity.resize(nb_p*nb_h) ;
         
         set_nodes(pmin, hmin) ;
         
         EOS_Field p("Pressure", "p", NEPTUNE::p, node_p) ;
         EOS_Field h("Enthalpy", "h", NEPTUNE::h, node_h) ;
         
         EOS_Field p_continuity("Pressure", "p", NEPTUNE::p, node_p_continuity) ;
         EOS_Field h_continuity("Enthalpy", "h", NEPTUNE::h, node_h_continuity) ;
         
         set_domain_ph(p, h) ;
         set_domain_continuity_ph(p, h) ;
         
         test_qualities_nodes.resize((nb_p-1)*(nb_h-1)) ;
         test_qualities_nodes = 1 ;
         nb_node = nb_p*nb_h ;
         
         int inc_nb_m = ((nb_p-1)*(nb_h-1)) - nb_mesh ;
         for (int i=0; i<inc_nb_m; i++)
            { ArrOfInt node(4);
              mesh_to_node.push_back(node) ;
            }
         
         nb_mesh = (nb_p-1)*(nb_h-1) ;
         int k = 0 ;
         for (int i=0; i<nb_mesh; i++)
            {
              mesh_to_node[i][0] = i+k        ;
              mesh_to_node[i][1] = i+k+1      ;
              mesh_to_node[i][2] = i+k+nb_h   ;
              mesh_to_node[i][3] = i+k+nb_h+1 ;
              if ((i+1)%(nb_h-1) == 0)  k++ ; 
            }
         type_of_node.resize(nb_p*nb_h) ;
       }
         
    else if (ndomain == 1)
       {
         double pmin = domain[0][0] ;
         double pmax = domain[0][nb_p-1] ;
         
         nb_p = nb_p*2 - 1 ;
         
         delta_p = (pmax-pmin) / double(nb_p-1) ;
         
         node_p.resize(nb_p) ;
         set_nodes(pmin) ;
         
         EOS_Field p("Pressure", "p", NEPTUNE::p, node_p) ;
         
         set_domain_p(p) ;
         test_qualities_nodes.resize(nb_p-1) ;
         test_qualities_nodes = 1 ;
         nb_node = nb_p      ;
         nb_mesh = nb_p - 1 ;
       }
  }
  
  void EOS_Mesh::add_local_nodes(int level, bool cont)
  { int ndomain = domain.size() ;

    if (ndomain == 2)
       { //  création d'un maillage global pour la qualité 
         //  => détermination des mailles accolées plus facil
         int sz_glb_h = pow(2,level)*nb_h - (pow(2,level)-1) ;
         int sz_glb_p = pow(2,level)*nb_p - (pow(2,level)-1) ;
            
         //  affectation des maille global au maille reel
         ArrOfInt glb_to_mesh((sz_glb_h-1)*(sz_glb_p-1)) ;
         int l  = 0 ;
         int k  = 1 ;
         int m  = 0 ;
         int mm = 0 ;
         glb_to_mesh = -8 ;
         while ((m < nb_mesh)  &&  (l < (sz_glb_h*sz_glb_p)))
            { while(node_glb[l+k*sz_glb_h] == 0)
                 k++ ;
              while(node_glb[l+k] == 0)
                 k++ ;
              while(node_glb[l+k*sz_glb_h+k] == 0)
                 k++ ;
              
              for (int i=0; i<k; i++)
                 { for (int j=0; j<k; j++)
                      glb_to_mesh[mm+i*(sz_glb_h-1)+j] = m ;
                 }
              l  += k ;
              mm += k ;
              m++ ;
              
              if ( ((l+1)%sz_glb_h == 0) || (node_glb[l] == 8) )
                 { int d = 0 ;
                   int c = 0 ;
                   if ((l+1)%sz_glb_h == 0)  d++ ;
                   
                   if ((l+1)%sz_glb_h == 0)
                      { l++   ;
                        c = 1 ;
                      }
                   else if (node_glb[l]==8 && node_glb[l+1]==0)
                      { int n = 1 ;
                        bool test = false ;
                        while (node_glb[l+n] == 0)
                           { n++ ;
                             if ((l+n)%sz_glb_h == 0)
                                { test = true ;
                                  break ;
                                }
                           }
                        
                        if (n>=int(pow(2,level)) || test || node_glb[l+n]==8)
                           { l++   ;
                             c = 1 ;
                           }
                      }
                   while(node_glb[l] == 0)
                      { l++ ;
                        c++ ;
                        if (l%sz_glb_h == 0)  d++ ;
                      }
                   mm += c - d ;
                 }
              k = 1 ;
            }
         
         ArrOfInt test_qualities_glb((sz_glb_h-1)*(sz_glb_p-1)) ;
         int nb_tqg = test_qualities_glb.size() ;

         for (int i=0; i<nb_tqg; i++)
            { int m = glb_to_mesh[i] ;
              test_qualities_glb[i] = test_qualities_nodes[m] ;
            }
         
         
//       determination du nb de noeuds du maillage raffiné 
         int inc_sz = 0 ; 
         if (!test_qualities_glb[0])  inc_sz = inc_sz + 5 ;
         for (int m=1; m<nb_tqg; m++)
            { if (!test_qualities_glb[m])
                 { if (m < sz_glb_h-1)
                      { if (!test_qualities_glb[m-1])
                           inc_sz = inc_sz + 4 ;
                        else
                           inc_sz = inc_sz + 5 ;
                      }
                   else if  (m%(sz_glb_h-1) == 0)
                      { if (!test_qualities_glb[m-(sz_glb_h-1)])
                           inc_sz = inc_sz + 4 ;
                        else
                           inc_sz = inc_sz + 5 ;
                      }
                   else
                      { if (!test_qualities_glb[m-1] && !test_qualities_glb[m-(sz_glb_h-1)])
                           inc_sz = inc_sz + 3 ;
                        else if (!test_qualities_glb[m-1] || !test_qualities_glb[m-(sz_glb_h-1)])
                           inc_sz = inc_sz + 4 ;
                        else
                           inc_sz = inc_sz + 5 ;
                      }
                 }
            }

//       determination de la taille du maillage raffiné 
         int inc_m  = 0 ;
         int nb_tqn = test_qualities_nodes.size() ;
         for (int i=0; i<nb_tqn; i++)
            { if (!test_qualities_nodes[i])  inc_m += 3 ;
            }
         
         
//          détermination des noeuds à interpolés
//          =1 : noeuds initiaux ; =2 : entre 2 noeuds initiaux à p cst ;
//          =3 : entre 2 noeuds initiaux à h cst ; =4 : au centre de la maille
         int sz_next_h = pow(2,level+1)*nb_h-(pow(2,level+1)-1) ;
         int sz_next_p = pow(2,level+1)*nb_p-(pow(2,level+1)-1) ;
         
         delta_p = (domain[1][domain[1].size()-1]-domain[1][0])/double(sz_next_p-1) ;
         delta_h = (domain[0][domain[0].size()-1]-domain[0][0])/double(sz_next_h-1) ;   

         ArrOfInt node_glb_tmp(sz_next_h*sz_next_p) ;
         node_glb_tmp = 0 ;

         int nb_ng = node_glb.size() ;
         
         k = 0 ;
         node_glb_tmp[0] = 1 ;
         for (int i=1; i<nb_ng; i++)
            { if (node_glb[i] != 0)       node_glb_tmp[2*i+k*(sz_next_h-1)] = node_glb[i] ;
              if ((i+1)%(sz_glb_h) == 0)  k++ ;
            }
         
         k = 0 ;
         for (int m=0; m<nb_tqg; m++)
            { if (!test_qualities_glb[m])
                 { node_glb_tmp[m+k+1]             = 2 ;
                   node_glb_tmp[m+k+sz_next_h]     = 3 ;
                   node_glb_tmp[m+k+sz_next_h+1]   = 4 ;
                   node_glb_tmp[m+k+sz_next_h+2]   = 3 ;
                   node_glb_tmp[m+k+2*sz_next_h+1] = 2 ;
                 }
              k++ ;
              if ((m+1)%(sz_glb_h-1) == 0) k += sz_next_h+1 ;
            }
         
//          reafectation de noeuds reel dans le maillage global
         node_glb.resize(sz_next_h*sz_next_p) ;
         node_glb = 0 ;
         nb_ng = node_glb.size() ;
         for (int i=0; i<nb_ng; i++)
            { if (node_glb_tmp[i] > 0)
                 { if      (node_glb_tmp[i] == 3 || node_glb_tmp[i] == 8)
                     node_glb[i] = 8 ;
                   else if (node_glb_tmp[i] == 2 || node_glb_tmp[i] == 9)
                     node_glb[i] = 9 ;
                   else
                     node_glb[i] = 1 ;
                 }
            }

           
//          interpolation des noeuds création d'un maillage intermédiaire
//          de taille d'un maillage global pour garder stable l'écart du nb de noeuds
//          pour l'interpolation à h cst
         ArrOfDouble nn_h(sz_next_h*sz_next_p) ;
         ArrOfDouble nn_p(sz_next_h*sz_next_p) ;

         int nb_ngt = node_glb_tmp.size() ;
               
         k = 0 ;
         for (int i=0; i<nb_ngt; i++)
            { if (node_glb_tmp[i] == 1 || node_glb_tmp[i] == 8 || node_glb_tmp[i] == 9)
                 { nn_h[i] = node_h[k] ;
                   nn_p[i] = node_p[k] ;
                   k++ ;
                 }
            }
         nb_node = nb_node + inc_sz ;
         nb_mesh = nb_mesh + inc_m  ;
         
         node_h.resize(nb_node) ;
         node_p.resize(nb_node) ;
         k = 0 ;
         for (int i=0; i<nb_ngt; i++)
            { if (node_glb_tmp[i] > 0)
                 { if (node_glb_tmp[i] == 1 || node_glb_tmp[i] == 8 || node_glb_tmp[i] == 9)
                      { node_h[k] = nn_h[i] ;
                        node_p[k] = nn_p[i] ;
                      }
                   else if (node_glb_tmp[i] == 2)
                      { node_h[k] = 0.5e0 * (nn_h[i-1]+nn_h[i+1]) ;
                        nn_h[i]   = node_h[k]   ;
                        node_p[k] = nn_p[i-1] ;
                      }
                   else if (node_glb_tmp[i] == 3)
                      { node_h[k] = nn_h[i-sz_next_h] ;
                        node_p[k] = 0.5e0 * (nn_p[i-sz_next_h]+nn_p[i+sz_next_h]) ;
                      }
                   else if (node_glb_tmp[i] == 4)
                      { node_h[k] = nn_h[i-sz_next_h] ;
                        node_p[k] = node_p[k-1]       ;
                      }
                   k++ ;
                 }
            }

//          affectation de mesh_to_node
         for (int i=0; i<inc_m; i++)
            { ArrOfInt nn(4) ;
              mesh_to_node.push_back(nn) ;
            }
        
//          affectation des noeuds du maillage fictif au noeuds du maillage reel
         ArrOfInt glb_to_node(sz_next_h*sz_next_p) ;
         k = 0 ;
         for (int i=0; i<sz_next_h*sz_next_p; i++)
            { if (node_glb_tmp[i] != 0)
                 { glb_to_node[i] = k ;
                   k++ ;
                 }
            }
         
         k = 1 ;
         l = 0 ;
         m = 0 ;
//          affectation des maille au 4 noeuds qui l'entoure
         while (m<nb_mesh && l<(sz_next_h*sz_next_p))
            { mesh_to_node[m][0]=l; //glb_to_node[l];
              while(node_glb_tmp[l+k*sz_next_h] == 0)
                 k++ ;
              while(node_glb_tmp[l+k] == 0)
                 k++ ;
              while(node_glb_tmp[l+k*sz_next_h+k] == 0)
                 k++ ;
              
              mesh_to_node[m][2] = l+k*sz_next_h ;   //glb_to_node[l+k*sz_next_h];
              mesh_to_node[m][1] = l+k ;             //glb_to_node[l+k];
              mesh_to_node[m][3] = l+k*sz_next_h+k ; //glb_to_node[l+k*sz_next_h+k];
              
              l += k ;
              m++ ;
              
              if (((l+1)%sz_next_h == 0) || (node_glb_tmp[l] == 3) || (node_glb_tmp[l] == 8))
                 { if ((l+1)%sz_next_h == 0)
                      l++;
                   else if ((node_glb_tmp[l] == 3 || node_glb_tmp[l] == 8) 
                             && node_glb_tmp[l+1] == 0)
                      { int n = 1 ;
                        bool test = false ;
                        while (node_glb_tmp[l+n] == 0)
                           { n++ ;
                             if ((l+n)%sz_next_h == 0)
                                { test = true ;
                                  break ;
                                }
                           }
                        if (   (n >= int(pow(2,level+1))) || test 
                            || (node_glb_tmp[l+n] == 3)   || (node_glb_tmp[l+n] == 8) )
                           l++ ;
                      }
                   while(node_glb_tmp[l] == 0)
                      l++ ;
                 }
              k = 1 ;
            }
   
         int nb_mtn = med_to_node.size() ;
         
         int inc_glb_m = ((sz_next_h-1)*(sz_next_p-1)) - nb_mtn ;
//          affectation de med_to_node
         for (int i=0 ; i<inc_glb_m ; i++)
            { ArrOfInt nn(4) ;
              med_to_node.push_back(nn) ;
            }
         
         l  = 0 ;
         k  = 1 ;
         m  = 0 ;
         mm = 0 ;
         ArrOfInt next_to_mesh((sz_next_h-1)*(sz_next_p-1)) ;
               
         while ( (m < nb_mesh) && (l < (sz_next_h*sz_next_p)) )
            { while(node_glb_tmp[l+k*sz_next_h] == 0)
                 k++ ;
              while(node_glb_tmp[l+k] == 0)
                 k++ ;
              while(node_glb_tmp[l+k*sz_next_h+k] == 0)
                 k++ ;
              
              for (int i=0; i<k; i++)
                 { for (int j=0; j<k; j++)
                      { next_to_mesh[mm+i*(sz_next_h-1)+j] = m ;
                      }
                 }
              l  += k ;
              mm += k ;
              m++ ;
              
              if (   ((l+1)%sz_next_h == 0) || (node_glb_tmp[l] == 3)
                  || (node_glb_tmp[l] == 8) )
                 { int d = 0 ;
                   int c = 0 ;
                   if ((l+1)%sz_next_h == 0) d++ ;
                   
                   if ((l+1)%sz_next_h == 0)
                      { l++ ;
                        c = 1 ;
                      }
                   else if (   (node_glb_tmp[l] == 3 || node_glb_tmp[l] == 8) 
                            && (node_glb_tmp[l+1] == 0) )
                      { int n = 1 ;
                         bool test = false ;
                         while (node_glb_tmp[l+n] == 0)
                            { n++;
                              if ((l+n)%sz_next_h == 0)
                                 { test = true ;
                                   break ;
                                 }
                            }
                         if (   (n >= int(pow(2,level+1))) || test 
                             || (node_glb_tmp[l+n] == 3)   || (node_glb_tmp[l+n] == 8) )
                            { l++ ;
                              c = 1 ;
                            }
                      }
                   while(node_glb_tmp[l] == 0)
                      { l++ ;
                        c++ ;
                        if (l%sz_next_h == 0) d++ ;
                      }
                   mm += c-d ;
                 }
              k = 1 ;
            }
               
         int nb_ntm = next_to_mesh.size() ;
         for (int i=0; i<nb_ntm; i++)
            med_to_node[i] = mesh_to_node[next_to_mesh[i]] ;
         
         int nb_metn = mesh_to_node.size() ;
         for (int i=0; i<nb_metn; i++)
            { for (int j=0; j<4; j++)
                 mesh_to_node[i][j] = glb_to_node[mesh_to_node[i][j]] ;
            }
         
         
         EOS_Field p("Pressure", "p", NEPTUNE::p, node_p) ;
         EOS_Field h("Enthalpy", "h", NEPTUNE::h, node_h) ;
         set_domain_ph(p, h) ;
         
         
//         sans continuite 
         if (! cont)
            { nb_mtn = med_to_node.size() ;
              for (int i=0; i<nb_mtn; i++)
                 { for (int j=0; j<4; j++)
                      med_to_node[i][j] = glb_to_node[med_to_node[i][j]] ;
                 }
              set_domain_continuity_ph(p, h) ;
              type_of_node.resize(nb_node) ;
            }

         test_qualities_nodes.resize(nb_mesh) ;
         test_qualities_nodes = 1 ;
       }

    else if (ndomain == 1)
       { ArrOfDouble node_p_tmp = node_p ;
         int k = 1 ;
         for (int i=0; i<nb_mesh; i++)
            { if (!test_qualities_nodes[i])
                 { nb_node++ ;
                   node_p.resize(nb_node) ;
                   node_p[i+k] = 0.5e0 * (node_p_tmp[i]+node_p_tmp[i+1]) ;
                   k++ ;
                 }
              node_p[i+k] = node_p_tmp[i+1];
            }
         nb_mesh += k-1 ;
         EOS_Field p("Pressure", "p", NEPTUNE::p, node_p) ;
         set_domain_p(p) ;
         test_qualities_nodes.resize(nb_mesh) ;
         test_qualities_nodes = 1 ;
       }
    
  }
  
  void EOS_Mesh::add_continuity_nodes(int level)
  {
    int sz_prec_h = pow(2,level)*nb_h - (pow(2,level)-1) ;
    int sz_prec_p = pow(2,level)*nb_p - (pow(2,level)-1) ;
    int sz_prec   = sz_prec_h * sz_prec_p ;
    
    int sz_glb_h = pow(2,level+1)*nb_h - (pow(2,level+1)-1) ;
    int sz_glb_p = pow(2,level+1)*nb_p - (pow(2,level+1)-1) ;
    int sz_glb_m = (sz_glb_p-1) * (sz_glb_h-1) ;
    
    ArrOfInt prec_to_glb(sz_prec) ;
    int k = 0 ;
    for (int i=0; i<sz_prec; i++)
       { prec_to_glb[i] = 2*i+k*(sz_glb_h-1) ;
         if ((i+1)%(sz_prec_h) == 0)  k++ ;
       }
    
    int nb_ng = node_glb.size() ;
        
//     vecteur de correspondance des noeuds reel au maillage global
    ArrOfInt glb_to_node(nb_ng,-1) ;
    k = 0 ;
    for (int i=0; i<nb_ng; i++)
       { if (node_glb[i] != 0)
            { glb_to_node[i] = k ;
              k++;
            }
       }
    
//     placement des noeuds existant (continuite et reel) dans le nouveau maillage
    int nb_mc = med_correction.size() ;

    for (int i=0; i<nb_mc; i++)
       { for (int j=0 ; j<4; j++)
            { med_correction[i][j] = prec_to_glb[med_correction[i][j]] ;
              new_correction[i][j] = prec_to_glb[new_correction[i][j]] ;
            }
       }
    k = 0 ;
    vector<ArrOfInt> mesh_to_glb ;
    for (int i=0; i<sz_glb_m; i++)
       { ArrOfInt nn(4) ;
         nn[0] = i+k            ;
         nn[1] = i+k+1          ;
         nn[2] = i+k+sz_glb_h   ;
         nn[3] = i+k+sz_glb_h+1 ;
         mesh_to_glb.push_back(nn) ;
         
         if ((i+k+2)%sz_glb_h==0)  k++ ;
       }
    for (int i=0; i<sz_glb_m ; i++)
       { for (int j=0; j<nb_mc; j++)
            { if (   (med_to_node[i][0] == med_correction[j][0]) && (med_to_node[i][3] == med_correction[j][3])
                  && (mesh_to_glb[i][0] >= new_correction[j][0]) && (mesh_to_glb[i][3] <= new_correction[j][3])
                  && ((mesh_to_glb[i][2] >= new_correction[j][2] &&  mesh_to_glb[i][1] >  new_correction[j][1]) 
                  || (mesh_to_glb[i][2] <  new_correction[j][2]  &&  mesh_to_glb[i][1] <= new_correction[j][1])) )
                 { med_to_node[i] = new_correction[j] ;
                   break ;
                 }
            }
       } 
//     determination de la continuite
    ArrOfDouble continuity_p(nb_ng) ;
    ArrOfDouble continuity_h(nb_ng) ;
    
    k = 0 ;
    for (int i=0; i<sz_prec; i++)
       { if (node_glb[prec_to_glb[i]] > 0)
            k++ ;
         else if (continuity_node[i] > 4)
            { continuity_h[prec_to_glb[i]] = domain_continuity[0][k] ;
              continuity_p[prec_to_glb[i]] = domain_continuity[1][k] ;
              k++ ;
            }
       }


    ArrOfInt continuity_node_tmp(nb_ng);
    k = 0 ;
    int nb_cn = continuity_node.size() ;
    for (int i=0; i<nb_cn; i++)
       { if (continuity_node[i] != 0)  continuity_node_tmp[2*i+k*(sz_glb_h-1)] = continuity_node[i] ;
         if ((i+1)%(sz_prec_h)  == 0)  k++ ;
       }
    continuity_node.resize(nb_ng) ;
    continuity_node = continuity_node_tmp ;
//      Ajout au maillage global des nouveau noeuds de continuité 
//      et affectation des valeurs h et p
    for (int i=sz_glb_h; i<nb_ng; i++)
       { if ( (node_glb[i] == 8) && (i%sz_glb_h != 0) && ((i+1)%sz_glb_h != 0) )
            { if (node_glb[i+1] == 0)
                 { int l = 2 ;
                   bool test = true ;
                   while (   (node_glb[i+l-sz_glb_h] != 1) 
                          && (node_glb[i+l+sz_glb_h] != 1)
                          && (continuity_node[i+l-sz_glb_h] < 4) 
                          && (continuity_node[i+l+sz_glb_h] < 4) )
                      { l++ ;
                        if ( (float(l) > (pow(2,level))) || (continuity_node[i+l] > 3) )
                           { test = false ;
                             break ;
                           }
                      }
                   
                   if (test)
                      { continuity_p[i+l] = domain[1][glb_to_node[i]] ;
                        continuity_h[i+l] = domain[0][glb_to_node[i+l-sz_glb_h]] ;
                        continuity_node[i+l] = 1 ;
                        if (continuity_node[i+(l-1)] <= 0)
                           { if (continuity_node[i+(l-1)] == -2)
                                continuity_node[i+(l-1)] = 3 ;
                             else
                                continuity_node[i+(l-1)] = -1 ;
                             
                             continuity_p[i+(l-1)] = domain[1][glb_to_node[i]] ;
                           }
                      }
                 }

              else if (node_glb[i-1] == 0)
                 { int l = 2 ;
                   bool test = true ;
                   while (   (node_glb[i-l-sz_glb_h] != 1)
                          && (node_glb[i-l+sz_glb_h] != 1)
                          && (continuity_node[i-l-sz_glb_h] < 4)
                          && (continuity_node[i-l+sz_glb_h] < 4) )
                      { l++ ;
                        if ((float(l) > (pow(2,level))) || (continuity_node[i-l] > 3) )
                           { test = false ;
                             break ;
                           }
                      }
                   
                   if (test)
                      { continuity_p[i-l] = domain[1][glb_to_node[i]] ;
                        continuity_h[i-l] = domain[0][glb_to_node[i-l-sz_glb_h]] ;
                        continuity_node[i-l] = 1 ;

                        if (continuity_node[i-(l-1)] <= 0)
                           { if (continuity_node[i-(l-1)] == -2)
                                continuity_node[i-(l-1)] =  3 ;
                             else
                                continuity_node[i-(l-1)] = -1 ;
                             
                             continuity_p[i-(l-1)] = domain[1][glb_to_node[i]] ;
                           }
                      }
                 }
            }

         if ( (node_glb[i] == 9) && (i < (nb_ng-sz_glb_h)) )
            { if (node_glb[i+sz_glb_h] == 0)
                 { int l = 2 ;
                   bool test = true ;
                   while (   (node_glb[i+l*sz_glb_h-1] != 1)       && (node_glb[i+l*sz_glb_h+1] != 1)
                          && (continuity_node[i+l*sz_glb_h-1] <4 ) && (continuity_node[i+l*sz_glb_h+1] < 4) )
                      { l++ ;
                        if ( (float(l) > (pow(2,level))) || (continuity_node[i+l*sz_glb_h] > 3) )
                           { test = false ;
                             break ;
                           }
                      }
                   
                   if (test)
                      { continuity_p[i+l*sz_glb_h] = domain[1][glb_to_node[i+l*sz_glb_h-1]] ;
                        continuity_h[i+l*sz_glb_h] = domain[0][glb_to_node[i]] ;
                        continuity_node[i+l*sz_glb_h] = 2 ;

                        if (continuity_node[i+(l-1)*sz_glb_h] <= 0)
                           { if (continuity_node[i+(l-1)*sz_glb_h] == -1)
                                continuity_node[i+(l-1)*sz_glb_h] =  3 ;
                             else
                                continuity_node[i+(l-1)*sz_glb_h] = -2 ;
                             
                             continuity_h[i+(l-1)*sz_glb_h] = domain[0][glb_to_node[i]];
                           }
                      }
                 }

              else if (node_glb[i-sz_glb_h] == 0)
                 { int l = 2 ;
                   bool test = true ;
                   while (   (node_glb[i-l*sz_glb_h-1] != 1)       && (node_glb[i-l*sz_glb_h+1] != 1)
                          && (continuity_node[i-l*sz_glb_h-1] < 4) && (continuity_node[i-l*sz_glb_h+1] < 4) )
                      { l++ ;
                        if ( (float(l) > (pow(2,level))) || (continuity_node[i-l*sz_glb_h] > 3) )
                           { test = false ;
                             break ;
                           }
                      }
                   if (test)
                      { continuity_p[i-l*sz_glb_h] = domain[1][glb_to_node[i-l*sz_glb_h-1]] ;
                        continuity_h[i-l*sz_glb_h] = domain[0][glb_to_node[i]] ;
                        continuity_node[i-l*sz_glb_h] = 2 ;

                        if (continuity_node[i-(l-1)*sz_glb_h] <= 0)
                           { if (continuity_node[i-(l-1)*sz_glb_h] == -1)
                                continuity_node[i-(l-1)*sz_glb_h] =  3 ;
                             else
                                continuity_node[i-(l-1)*sz_glb_h] = -2 ;
                             
                             continuity_h[i-(l-1)*sz_glb_h]=domain[0][glb_to_node[i]];
                           }
                      }
                 }
            }
       }

    
    node_p_continuity.resize(nb_node+nb_continuity) ;
    node_h_continuity.resize(nb_node+nb_continuity) ;
    k = node_p_continuity.size() ;
    int l = 0 ;
    int inc_ct = 0 ;
    for (int i=0; i<nb_ng; i++)
       { if ( (continuity_node[i] > 0) && (glb_to_node[i] < 0) )
            { if (continuity_node[i] < 4)
                 { k++ ;
                   nb_continuity++ ;
                   node_h_continuity.resize(k) ;
                   node_p_continuity.resize(k) ;
                 }
              
              node_h_continuity[l] = continuity_h[i] ;
              node_p_continuity[l] = continuity_p[i] ;
              l++ ;
              if (continuity_node[i] < 4)  inc_ct++ ;
            }

         else if(glb_to_node[i] >= 0)
            { node_h_continuity[l] = domain[0][glb_to_node[i]] ;
              node_p_continuity[l] = domain[1][glb_to_node[i]] ;
              l++ ;
            }
       }
    k = 0 ;
    type_of_node.resize(nb_node + nb_continuity) ;
    for (int i=0; i<nb_ng; i++)
       { if ( (continuity_node[i]>0) || (node_glb[i]>0) )
            { glb_to_node[i] = k ;
              if (node_glb[i] != 0)                                               // noeuds reels
                 type_of_node[k] = 0 ;
              else if ( (continuity_node[i] == 1) || (continuity_node[i] == 7) )  // noeuds de continuite a p cst
                 type_of_node[k] = 1 ;
              else if ( (continuity_node[i] == 2) || (continuity_node[i] == 8) )  // noeuds de continuite a h cst
                 type_of_node[k] = 2 ;
              else if ( (continuity_node[i] == 3) || (continuity_node[i] == 9) )  // noeuds de continuite au centre des mailles
                 type_of_node[k] = 3 ;
              k++ ;
            }
       }
//     validation des noeuds de continuite
    vector<ArrOfInt> new_to_node ; 
    for (int i=0; i<sz_glb_m; i++)
       new_to_node.push_back(med_to_node[i]) ;
    
    for (int i=0; i<sz_glb_m; i++)
       { ArrOfInt n ;
         int m = 0 ;
         for (int j=0; j<4; j++)
            { if (mesh_to_glb[i][j] != new_to_node[i][j])
                 { if (   (continuity_node[mesh_to_glb[i][j]] > 0 && continuity_node[mesh_to_glb[i][j]] < 4) 
                       || (node_glb[mesh_to_glb[i][j]] == 8) || (node_glb[mesh_to_glb[i][j]] == 9) )
                      { new_to_node[i][j] = mesh_to_glb[i][j] ;
                        m++ ;
                        n.resize(m) ;
                        n[m-1] = j ;
                      }
                 }
            }
         if (n.size()>0 && n.size()<3)
            { for (int j=0; j<n.size(); j++)
                 { int r = med_to_node[i][n[j]]-new_to_node[i][n[j]] ;
                   int t = 0 ;
                   if      ((n[j] == 0 && abs(r) >= sz_glb_h) || (n[j] == 3 && abs(r) <  sz_glb_h))
                      t = 1 ;
                   else if ((n[j] == 0 && abs(r) <  sz_glb_h) || (n[j] == 3 && abs(r) >= sz_glb_h))
                      t = 2 ;
                   else if ((n[j] == 1 && abs(r) >= sz_glb_h) || (n[j] == 2 && abs(r) <  sz_glb_h))
                      t = 0 ;
                   else if ((n[j] == 1 && abs(r) <  sz_glb_h) || (n[j] == 2 && abs(r) >= sz_glb_h))
                      t = 3 ;
                   
                   if (   (continuity_node[new_to_node[i][n[j]]] > 0 && continuity_node[new_to_node[i][n[j]]] < 4)
                       && (node_glb[med_to_node[i][t]-r] == 8 || node_glb[med_to_node[i][t]-r] == 9) )
                      new_to_node[i][t] = med_to_node[i][t]-r ;
                   else if ( (continuity_node[med_to_node[i][t]-r] > 0 && continuity_node[med_to_node[i][t]-r] < 4)
                        && (node_glb[new_to_node[i][n[j]]] == 8 || node_glb[new_to_node[i][n[j]]] == 9))
                      new_to_node[i][t] = med_to_node[i][t] - r ;
                   else
                      new_to_node[i][n[j]] = med_to_node[i][n[j]] ;
                 }
            }
       }
    for (int i=0; i<sz_glb_m; i++)
       { if ( (med_to_node[i][0] != new_to_node[i][0]) || (med_to_node[i][3] != new_to_node[i][3]) )
            { med_correction.push_back(med_to_node[i]) ;
              new_correction.push_back(new_to_node[i]) ;
            }
         med_to_node[i] = new_to_node[i] ;
       }
    
    int nb_mtn = med_to_node.size() ;
    for (int i=0; i<nb_mtn; i++)
       { for (int j=0; j<4; j++)
           med_to_node[i][j] = glb_to_node[med_to_node[i][j]] ;
       }
//     affectation des noeuds de continuite aux noeuds permettant le calcul des proprietes
    for (int i=0; i<inc_ct; i++)
       { ArrOfInt nn(2) ;
         continuity_to_node.push_back(nn) ;
       }
    k = 0 ;
//     m pour ne pas faire un tableau de "continuity_to_node" de la taille du maillage
    int m = continuity_to_node.size() - 1 ;
    nb_cn = continuity_node.size() ;
    for (int i=0; i<nb_cn; i++)
       { if ( (continuity_node[i]>0) && (node_glb[i] == 0) )
            { if     ( (continuity_node[i] == 1) || (continuity_node[i] == 7) )
                 { int l = 1 ;
//                 si nouveau noeud de continuite il peut s'appuyer sur un ancien noeud => 2eme condition
                   while( (node_glb[i+l*sz_glb_h] == 0)  && !(continuity_node[i+l*sz_glb_h]>0 && continuity_node[i] == 1) )
                     l++ ;
                   continuity_to_node[k][0] = glb_to_node[i+l*sz_glb_h] ;
                   continuity_to_node[k][1] = glb_to_node[i-l*sz_glb_h] ;
                   k++ ;
                 }
              else if ( (continuity_node[i] == 2) || (continuity_node[i] == 8) )
                 { int l = 1 ;
//                 si nouveau noeud de continuite il peut s'appuyer sur un ancien noeud => 2eme condition
                   while( (node_glb[i+l] == 0)  && !(continuity_node[i+l]>0 && continuity_node[i] == 2) )
                      l++ ;
                   continuity_to_node[k][0] = glb_to_node[i+l] ;
                   continuity_to_node[k][1] = glb_to_node[i-l] ;
                   k++ ;
                 }
              else if ( (continuity_node[i] == 3) || (continuity_node[i] == 9) )
                 { int l = 1 ;
//                 1 des 2 noeuds est continuite
                   while( (node_glb[i+l] == 0)  && (continuity_node[i+l] <= 0) )
                      l++ ;
                   continuity_to_node[m][0] = glb_to_node[i+l] ;
                   continuity_to_node[m][1] = glb_to_node[i-l] ;
                   m-- ;
                 }
            }
       }

    nb_cn = continuity_node.size() ;
    for (int i=0; i<nb_cn; i++)
       { if      (continuity_node[i] == 1)
            continuity_node[i] = 7 ;
         else if (continuity_node[i] == 2)
            continuity_node[i] = 8 ;
         else if (continuity_node[i] == 3)
            continuity_node[i] = 9 ;
         else
            continuity_node[i] = 0 ;
       }
    EOS_Field p("Pressure", "p", NEPTUNE::p, node_p_continuity) ;
    EOS_Field h("Enthalpy", "h", NEPTUNE::h, node_h_continuity) ;
    set_domain_continuity_ph(p, h) ;

  }
  
  const EOS_Mesh& EOS_Mesh::operator=(const EOS_Mesh& right)
  { exist     = right.exist     ;
    delta_h   = right.delta_h   ;
    delta_p   = right.delta_p   ;
    level_max = right.level_max ;
    
    domain = right.domain ;
    node_p = right.node_p ;
    node_h = right.node_h ;
    error  = right.error  ;
    test_qualities_nodes = right.test_qualities_nodes ;
            
    return *this ;
  }
  
}
