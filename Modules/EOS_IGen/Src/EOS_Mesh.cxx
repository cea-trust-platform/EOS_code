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
#include <iostream>
#include <algorithm>
#include <vector>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

using namespace NEPTUNE_EOS ;
using namespace NEPTUNE ;

namespace
{
  //! One cell of a refinement grid: the (row, col) of its lower-left corner in
  //! that grid, and its side, both in grid steps.
  struct MeshCell
  {
    int row, col, side;
  };

  //! Cells of the mesh a refinement grid describes, in ascending (row, col) of
  //! their lower-left corner.
  //!
  //! This replaces three copies of a walk that tried to recover the same
  //! information by stepping through the grid: take the current position as a
  //! cell corner, scan for the other three at a common offset, and at the end
  //! of a row step one entry and skip empty ones along it to find the next
  //! cell. That last rule only lands on the next row of cells while every cell
  //! in the row has the same height, which local refinement stops guaranteeing
  //! from level 2 on. The walk then stood on a hole or on a mid-edge node and
  //! scanned off the end of the grid.
  //!
  //! There is no need to walk anything. Refining a cell writes a node at its
  //! centre (the type-4 node, cf. add_local_nodes), and only refining does. So
  //! a block is subdivided exactly when its centre holds a node, which is a
  //! local test, and the decomposition follows by recursion from the cells of
  //! the initial nb_p x nb_h grid. Cells come out in the order the walk
  //! produced wherever the walk worked -- by lower-left corner, bottom row
  //! first, left to right -- because the sort below is exactly what its
  //! left-to-right, bottom-to-top progression amounted to.
  //! True when the block [row,row+side] x [col,col+side] holds a node strictly
  //! inside it. Nodes on its edges do not count: those are the hanging nodes a
  //! finer neighbour put there, and they do not divide this cell.
  bool has_interior_node(const NEPTUNE::ArrOfInt &grid, int sz_h,
                         int row, int col, int side)
  {
    for (int r = row + 1; r < row + side; r++)
      for (int c = col + 1; c < col + side; c++)
        if (grid[r * sz_h + c] != 0)
          return true ;
    return false ;
  }

  void subdivide_cell(const NEPTUNE::ArrOfInt &grid, int sz_h,
                      int row, int col, int side, std::vector<MeshCell> &out)
  {
    const int half = side / 2;
    if (half >= 1 && has_interior_node(grid, sz_h, row, col, side))
      { subdivide_cell(grid, sz_h, row,        col,        half, out) ;
        subdivide_cell(grid, sz_h, row,        col + half, half, out) ;
        subdivide_cell(grid, sz_h, row + half, col,        half, out) ;
        subdivide_cell(grid, sz_h, row + half, col + half, half, out) ;
      }
    else
      out.push_back(MeshCell{row, col, side}) ;
  }

  //! Grid index of the first node reached from `from` by repeated `step`, or
  //! -1 if the search leaves [lo,hi] first. `lean_on_continuity` also accepts a
  //! continuity node, as a node placed at this level may rest on one placed at
  //! the previous one.
  //!
  //! The two supports of a continuity node used to be taken as symmetric, at
  //! +l and -l for the single l that the forward scan stopped at. That holds
  //! only where the node splits its edge in half. From level 3 on, dyadic
  //! refinement also puts nodes at the quarter points of an edge, whose
  //! supports sit at different distances; the backward index was then simply
  //! wrong, and near a boundary it was negative.
  int support_index(const NEPTUNE::ArrOfInt &node_glb,
                    const NEPTUNE::ArrOfInt &continuity_node,
                    int from, int step, int lo, int hi,
                    bool lean_on_continuity)
  {
    for (int idx = from + step; idx >= lo && idx <= hi; idx += step)
      { if (node_glb[idx] != 0) return idx ;
        if (lean_on_continuity && continuity_node[idx] > 0) return idx ;
      }
    return -1 ;
  }

  //! Where a slot of the refined grid sits with respect to the cells, read off
  //! the same quadtree decomposition the refinement itself walks.
  //!
  //! `edge_kind` is 1 when the slot lies strictly inside a *vertical* cell edge
  //! and 2 when it lies strictly inside a *horizontal* one, with `edge_a` and
  //! `edge_b` the two ends of that edge, `edge_side` the side of the cell it
  //! belongs to and `edge_cell` that cell. It is 0 when the slot is inside
  //! neither, which includes every cell corner. `centre` marks a cell centre.
  //!
  //! A slot can be strictly inside an edge of two cells at once -- the two that
  //! share it. When their sides differ the coarser cell is the one that has to
  //! be made continuous, the finer one already carrying a real node there, so
  //! the longer edge wins.
  struct SlotMap
  {
    std::vector<int>  edge_a, edge_b, edge_kind, edge_side, edge_cell ;
    std::vector<char> centre ;
  };

  void map_slots(int nb_slots, int sz_h, const std::vector<MeshCell> &cells,
                 SlotMap &out)
  {
    out.edge_a.assign(nb_slots, -1) ;
    out.edge_b.assign(nb_slots, -1) ;
    out.edge_kind.assign(nb_slots, 0) ;
    out.edge_side.assign(nb_slots, 0) ;
    out.edge_cell.assign(nb_slots, -1) ;
    out.centre.assign(nb_slots, 0) ;

    for (int m=0; m<(int)cells.size(); m++)
       { const MeshCell &c = cells[m] ;
         const int r0=c.row, c0=c.col, sd=c.side ;
         for (int t=1; t<sd; t++)
            { int q ;
              q = (r0+t)*sz_h + c0 ;
              if (out.edge_side[q] < sd)
                 { out.edge_a[q] = r0*sz_h+c0 ;      out.edge_b[q] = (r0+sd)*sz_h+c0 ;
                   out.edge_kind[q] = 1 ; out.edge_side[q] = sd ; out.edge_cell[q] = m ; }
              q = (r0+t)*sz_h + c0+sd ;
              if (out.edge_side[q] < sd)
                 { out.edge_a[q] = r0*sz_h+c0+sd ;   out.edge_b[q] = (r0+sd)*sz_h+c0+sd ;
                   out.edge_kind[q] = 1 ; out.edge_side[q] = sd ; out.edge_cell[q] = m ; }
              q = r0*sz_h + c0+t ;
              if (out.edge_side[q] < sd)
                 { out.edge_a[q] = r0*sz_h+c0 ;      out.edge_b[q] = r0*sz_h+c0+sd ;
                   out.edge_kind[q] = 2 ; out.edge_side[q] = sd ; out.edge_cell[q] = m ; }
              q = (r0+sd)*sz_h + c0+t ;
              if (out.edge_side[q] < sd)
                 { out.edge_a[q] = (r0+sd)*sz_h+c0 ; out.edge_b[q] = (r0+sd)*sz_h+c0+sd ;
                   out.edge_kind[q] = 2 ; out.edge_side[q] = sd ; out.edge_cell[q] = m ; }
            }
         if (sd >= 2)  out.centre[(r0+sd/2)*sz_h + c0+sd/2] = 1 ;
       }
  }

  //! grid is the node grid (0 = no node), sz_h its width, and base_side the
  //! side an unrefined cell of the initial grid has in it.
  void collect_cells(const NEPTUNE::ArrOfInt &grid, int sz_h,
                     int nb_base_p, int nb_base_h, int base_side,
                     std::vector<MeshCell> &out)
  {
    out.clear() ;
    for (int rp = 0; rp < nb_base_p; rp++)
      for (int ch = 0; ch < nb_base_h; ch++)
        subdivide_cell(grid, sz_h, rp * base_side, ch * base_side, base_side, out) ;

    std::sort(out.begin(), out.end(),
              [](const MeshCell &a, const MeshCell &b)
              { return (a.row != b.row) ? (a.row < b.row) : (a.col < b.col) ; }) ;
  }

  //! The invariant the continuity nodes exist to satisfy, checked against the
  //! cell decomposition rather than against the code that placed them.
  //!
  //! A cell bordered by finer ones is *split* through each real node sitting
  //! strictly inside one of its edges, perpendicular to that edge, so that
  //! every cell stays a four-cornered rectangle and the hanging node is a
  //! corner of both halves. That leaves two things to be true, and this checks
  //! both:
  //!
  //!  - soundness: a continuity node of type 1 (resp. 2) lies strictly inside
  //!    a vertical (resp. horizontal) cell edge, and a type 3 lies at a cell
  //!    centre. Nothing else is a position the mesh has room for;
  //!  - completeness: the far end of every split line carries a node, real or
  //!    continuity. A split whose far end is empty cannot close.
  //!
  //! Returns the number of violations; prints them when EOS_MESH_CHECK is set.
  int check_continuity_nodes(const NEPTUNE::ArrOfInt &node_glb,
                             const NEPTUNE::ArrOfInt &continuity_node,
                             int sz_h, const SlotMap &map,
                             const std::vector<MeshCell> &cells,
                             int level)
  {
    const bool verbose = (getenv("EOS_MESH_CHECK") != NULL) ;
    const int  n       = node_glb.size() ;
    int bad_sound = 0, bad_complete = 0, shown = 0 ;
    int n_type[4] = {0,0,0,0} ;

    for (int i=0; i<n; i++)
       { const int t = continuity_node[i] ;
         if (t <= 0 || node_glb[i] != 0)  continue ;
         const int kind = (t == 1 || t == 7) ? 1 : (t == 2 || t == 8) ? 2 :
                          (t == 3 || t == 9) ? 3 : 0 ;
         if (kind == 0)  continue ;
         n_type[kind]++ ;
         bool ok ;
         if (kind == 3)  ok = (map.centre[i] != 0) ;
         else            ok = (map.edge_kind[i] == kind) ;
         if (!ok)
            { bad_sound++ ;
              if (verbose && shown++ < 12)
                 std::cerr << "  [check] unsound: continuity node type " << kind
                      << " at row " << i/sz_h << " column " << i%sz_h
                      << " sits on edge_kind " << (int)map.edge_kind[i]
                      << " centre " << (int)map.centre[i] << std::endl ;
            }
       }

    //  Every corner of every cell carries a node. Nothing downstream can hold
    //  together without this: a cell whose corner slot is empty has no value
    //  to interpolate from there, and both the cell count and the continuity
    //  placement will still look consistent while it is false.
    shown = 0 ;
    int bad_corner = 0 ;
    for (int m=0; m<(int)cells.size(); m++)
       { const MeshCell &c = cells[m] ;
         const int corners[4] = { c.row*sz_h + c.col,
                                  c.row*sz_h + c.col + c.side,
                                  (c.row+c.side)*sz_h + c.col,
                                  (c.row+c.side)*sz_h + c.col + c.side } ;
         for (int e=0; e<4; e++)
            { if (node_glb[corners[e]] != 0)  continue ;
              bad_corner++ ;
              if (verbose && shown++ < 12)
                 std::cerr << "  [check] hole: the cell at row " << c.row << " column "
                           << c.col << " side " << c.side << " has no node at its corner row "
                           << corners[e]/sz_h << " column " << corners[e]%sz_h << std::endl ;
            }
       }

    shown = 0 ;
    for (int m=0; m<(int)cells.size(); m++)
       { const MeshCell &c = cells[m] ;
         const int r0=c.row, c0=c.col, sd=c.side ;
         for (int t=1; t<sd; t++)
            { //  a real node strictly inside one of this cell's edges splits it
              const int probe[4] = { (r0+t)*sz_h + c0, (r0+t)*sz_h + c0+sd,
                                     r0*sz_h + c0+t,   (r0+sd)*sz_h + c0+t } ;
              for (int e=0; e<4; e++)
                 { const int q = probe[e] ;
                   if (node_glb[q] == 0)             continue ;
                   if (map.edge_side[q] != sd)       continue ;  // a finer cell owns it
                   const int far = (e < 2) ? (r0+t)*sz_h + (e == 0 ? c0+sd : c0)
                                           : (e == 2 ? r0+sd : r0)*sz_h + c0+t ;
                   if (node_glb[far] != 0 || continuity_node[far] > 0)  continue ;
                   bad_complete++ ;
                   if (verbose && shown++ < 12)
                      std::cerr << "  [check] incomplete: split through row " << q/sz_h
                           << " column " << q%sz_h << " of the cell at row " << r0
                           << " column " << c0 << " side " << sd
                           << " has nothing at row " << far/sz_h
                           << " column " << far%sz_h << std::endl ;
                 }
            }
       }

    //  EOS_MESH_DUMP="level,row0,row1,col0,col1" prints that window of the grid:
    //  digits are node_glb, letters a/b/c the continuity types 1/2/3, and
    //  upper case a corner of a cell, so the decomposition is readable at once.
    { const char *w = getenv("EOS_MESH_DUMP") ;
      int wl, r0, r1, c0, c1 ;
      if (w != NULL && sscanf(w, "%d,%d,%d,%d,%d", &wl, &r0, &r1, &c0, &c1) == 5 && wl == level)
         { std::vector<char> corner(n, 0) ;
           for (int m=0; m<(int)cells.size(); m++)
              { const MeshCell &c = cells[m] ;
                corner[c.row*sz_h+c.col] = 1 ;
                corner[c.row*sz_h+c.col+c.side] = 1 ;
                corner[(c.row+c.side)*sz_h+c.col] = 1 ;
                corner[(c.row+c.side)*sz_h+c.col+c.side] = 1 ;
              }
           std::cerr << "  [dump] rows " << r0 << ".." << r1 << " cols " << c0 << ".." << c1 << std::endl ;
           for (int r=r1; r>=r0; r--)
              { std::cerr << "  [dump] " ;
                std::cerr.width(4) ; std::cerr << r << " " ;
                for (int c=c0; c<=c1; c++)
                   { const int q = r*sz_h + c ;
                     char ch = '.' ;
                     if      (node_glb[q] != 0)        ch = '0' + node_glb[q] ;
                     else if (continuity_node[q] > 0)  ch = 'a' + ((continuity_node[q]-1) % 3) ;
                     else if (continuity_node[q] < 0)  ch = '-' ;
                     if (corner[q] && ch == '.')       ch = '+' ;
                     std::cerr << ch ;
                   }
                std::cerr << std::endl ;
              }
           std::cerr << "  [dump]      " ;
           for (int c=c0; c<=c1; c++)  std::cerr << (c%10) ;
           std::cerr << std::endl ;
         }
    }

    if (verbose)
       std::cerr << "  [check] level " << level << ": " << cells.size() << " cells, "
            << n_type[1] << " type-1, " << n_type[2] << " type-2, " << n_type[3]
            << " type-3 continuity nodes; " << bad_corner << " corner holes, "
            << bad_sound << " unsound, " << bad_complete << " unclosed splits" << std::endl ;

    return bad_corner + bad_sound + bad_complete ;
  }
}

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
  {
     refine_ok_ = true ; int ndomain = domain.size() ;

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

         std::vector<MeshCell> cells_glb ;
         collect_cells(node_glb, sz_glb_h, nb_p-1, nb_h-1, int(pow(2,level)), cells_glb) ;
         if ((int)cells_glb.size() != nb_mesh)
            { cerr << "EOS_Mesh::add_local_nodes: the node grid at level " << level
                   << " describes " << cells_glb.size() << " cells where the mesh counts "
                   << nb_mesh << "; refinement stopped" << endl ;
              refine_ok_ = false ; return ; }
         for (m = 0; m < nb_mesh; m++)
            { const MeshCell &c = cells_glb[m] ;
              for (int i=0; i<c.side; i++)
                 for (int j=0; j<c.side; j++)
                    glb_to_mesh[(c.row+i)*(sz_glb_h-1) + c.col+j] = m ;
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
//       Refining a cell writes the five nodes that quadrisect *each of its
//       coarse sub-cells* (cf. the marking loop below, which walks
//       test_qualities_glb, one entry per coarse global cell). A cell of
//       coarse side k therefore becomes 4k^2 cells, not 4: it is refined all
//       the way down to the current finest step, not quadrisected once.
//       This counted +3 per refined cell regardless, which is right only when
//       every refined cell already sits at the finest step -- true under
//       global refinement, and true of local refinement only at the first
//       level or two, by luck of which cells the quality criterion picks. From
//       there nb_mesh disagreed with the mesh actually described by the nodes,
//       and everything downstream that walks it went looking for cells that
//       were not where it expected.
         int inc_m  = 0 ;
         int nb_tqn = test_qualities_nodes.size() ;
         for (int i=0; i<nb_tqn; i++)
            { if (!test_qualities_nodes[i])
                 { const int k = cells_glb[i].side ;
                   inc_m += 4*k*k - 1 ;
                 }
            }
         
         
//          détermination des noeuds à interpolés
//          =1 : noeuds initiaux ; =2 : entre 2 noeuds initiaux à p cst ;
//          =3 : entre 2 noeuds initiaux à h cst ; =4 : au centre de la maille
         int sz_next_h = pow(2,level+1)*nb_h-(pow(2,level+1)-1) ;
         int sz_next_p = pow(2,level+1)*nb_p-(pow(2,level+1)-1) ;
         
         delta_p = (domain[1][domain[1].size()-1]-domain[1][0])/double(sz_next_p-1) ;
         delta_h = (domain[0][domain[0].size()-1]-domain[0][0])/double(sz_next_h-1) ;

//          Bisect the grid lines to the new level. Every node coordinate below
//          is read off these two arrays, so a slot that no node occupies yet
//          still has a well defined position -- which is what refining a cell
//          coarser than the current finest step needs, and what averaging the
//          neighbouring slots could not give (they may hold no node at all).
         { ArrOfDouble next_grid_h(sz_next_h) ;
           ArrOfDouble next_grid_p(sz_next_p) ;
           for (int j=0; j<grid_h.size()-1; j++)
              { next_grid_h[2*j]   = grid_h[j] ;
                next_grid_h[2*j+1] = 0.5e0 * (grid_h[j] + grid_h[j+1]) ;
              }
           next_grid_h[sz_next_h-1] = grid_h[grid_h.size()-1] ;
           for (int i=0; i<grid_p.size()-1; i++)
              { next_grid_p[2*i]   = grid_p[i] ;
                next_grid_p[2*i+1] = 0.5e0 * (grid_p[i] + grid_p[i+1]) ;
              }
           next_grid_p[sz_next_p-1] = grid_p[grid_p.size()-1] ;
           grid_h = next_grid_h ;
           grid_p = next_grid_p ;
         }

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

           
         int nb_ngt = node_glb_tmp.size() ;

         nb_node = nb_node + inc_sz ;
         nb_mesh = nb_mesh + inc_m  ;

//          Coordinates of every node of the refined mesh, taken from the grid
//          slot it occupies. This used to average the four grid neighbours of
//          a new node, which only works while the refined cell is one grid step
//          wide: a cell coarser than that has new nodes whose neighbouring
//          slots are still empty, and the averages then read zeros. It cost
//          240 nodes placed at h=0 and 324 at p=0 on a 17x17 level-3 tile,
//          silently, and the geometry is what everything downstream trusts.
         node_h.resize(nb_node) ;
         node_p.resize(nb_node) ;
         k = 0 ;
         for (int i=0; i<nb_ngt; i++)
            { if (node_glb_tmp[i] > 0)
                 { node_h[k] = grid_h[i % sz_next_h] ;
                   node_p[k] = grid_p[i / sz_next_h] ;
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
         
         //  Cells of the refined mesh, and the two tables built from them:
         //  mesh_to_node (its 4 corners) and next_to_mesh (which cell covers
         //  each cell of the fine global grid). Both used to be recovered by
         //  walking the grid; cf. collect_cells for why that could not work.
         std::vector<MeshCell> cells_next ;
         collect_cells(node_glb_tmp, sz_next_h, nb_p-1, nb_h-1, int(pow(2,level+1)), cells_next) ;
         if ((int)cells_next.size() != nb_mesh)
            { cerr << "EOS_Mesh::add_local_nodes: the refined node grid describes "
                   << cells_next.size() << " cells where the count kept by the refinement says "
                   << nb_mesh << "; refinement stopped" << endl ;
              refine_ok_ = false ; return ; }

         ArrOfInt next_to_mesh((sz_next_h-1)*(sz_next_p-1)) ;

         int nb_mtn = med_to_node.size() ;
         int inc_glb_m = ((sz_next_h-1)*(sz_next_p-1)) - nb_mtn ;
//          affectation de med_to_node
         for (int i=0 ; i<inc_glb_m ; i++)
            { ArrOfInt nn(4) ;
              med_to_node.push_back(nn) ;
            }

         for (m = 0; m < nb_mesh; m++)
            { const MeshCell &c = cells_next[m] ;
              const int ll = c.row * sz_next_h + c.col ;
              mesh_to_node[m][0] = ll ;
              mesh_to_node[m][1] = ll + c.side ;
              mesh_to_node[m][2] = ll + c.side * sz_next_h ;
              mesh_to_node[m][3] = ll + c.side * sz_next_h + c.side ;

              for (int i=0; i<c.side; i++)
                 for (int j=0; j<c.side; j++)
                    next_to_mesh[(c.row+i)*(sz_next_h-1) + c.col+j] = m ;
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
                      { continuity_p[i+l] = grid_p[(i+l) / sz_glb_h] ;
                        continuity_h[i+l] = grid_h[(i+l) % sz_glb_h] ;
                        continuity_node[i+l] = 1 ;
                        if (continuity_node[i+(l-1)] <= 0)
                           { if (continuity_node[i+(l-1)] == -2)
                                continuity_node[i+(l-1)] = 3 ;
                             else
                                continuity_node[i+(l-1)] = -1 ;
                             
                             continuity_p[i+(l-1)] = grid_p[(i+(l-1)) / sz_glb_h] ;
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
                      { continuity_p[i-l] = grid_p[(i-l) / sz_glb_h] ;
                        continuity_h[i-l] = grid_h[(i-l) % sz_glb_h] ;
                        continuity_node[i-l] = 1 ;

                        if (continuity_node[i-(l-1)] <= 0)
                           { if (continuity_node[i-(l-1)] == -2)
                                continuity_node[i-(l-1)] =  3 ;
                             else
                                continuity_node[i-(l-1)] = -1 ;
                             
                             continuity_p[i-(l-1)] = grid_p[(i-(l-1)) / sz_glb_h] ;
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
                      { continuity_p[i+l*sz_glb_h] = grid_p[(i+l*sz_glb_h) / sz_glb_h] ;
                        continuity_h[i+l*sz_glb_h] = grid_h[(i+l*sz_glb_h) % sz_glb_h] ;
                        continuity_node[i+l*sz_glb_h] = 2 ;

                        if (continuity_node[i+(l-1)*sz_glb_h] <= 0)
                           { if (continuity_node[i+(l-1)*sz_glb_h] == -1)
                                continuity_node[i+(l-1)*sz_glb_h] =  3 ;
                             else
                                continuity_node[i+(l-1)*sz_glb_h] = -2 ;
                             
                             continuity_h[i+(l-1)*sz_glb_h] = grid_h[(i+(l-1)*sz_glb_h) % sz_glb_h] ;
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
                      { continuity_p[i-l*sz_glb_h] = grid_p[(i-l*sz_glb_h) / sz_glb_h] ;
                        continuity_h[i-l*sz_glb_h] = grid_h[(i-l*sz_glb_h) % sz_glb_h] ;
                        continuity_node[i-l*sz_glb_h] = 2 ;

                        if (continuity_node[i-(l-1)*sz_glb_h] <= 0)
                           { if (continuity_node[i-(l-1)*sz_glb_h] == -1)
                                continuity_node[i-(l-1)*sz_glb_h] =  3 ;
                             else
                                continuity_node[i-(l-1)*sz_glb_h] = -2 ;
                             
                             continuity_h[i-(l-1)*sz_glb_h] = grid_h[(i-(l-1)*sz_glb_h) % sz_glb_h] ;
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
//     Which cell edge each grid slot lies strictly inside, and that edge's two
//     ends. This is the same decomposition the refinement itself walks, so the
//     supports of a hanging node are read off it rather than guessed by
//     scanning outwards -- a scan cannot tell the end of an edge from the next
//     node along, and at this depth the two stop being the same thing.
    SlotMap slots ;
    std::vector<MeshCell> cells ;
    { const int base_side = (sz_glb_h - 1) / (nb_h - 1) ;
      collect_cells(node_glb, sz_glb_h, nb_p-1, nb_h-1, base_side, cells) ;
      map_slots(nb_ng, sz_glb_h, cells, slots) ;
    }
    const std::vector<int> &edge_a = slots.edge_a ;
    const std::vector<int> &edge_b = slots.edge_b ;
    const std::vector<int> &edge_kind = slots.edge_kind ;
    check_continuity_nodes(node_glb, continuity_node, sz_glb_h, slots, cells, level) ;

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
                 { //  on a vertical edge: the supports are its two ends
                   const int up = edge_b[i], down = edge_a[i] ;
                   if (edge_kind[i] != 1 || glb_to_node[up] < 0 || glb_to_node[down] < 0)
                      { cerr << "EOS_Mesh::add_continuity_nodes: a continuity node at row "
                             << i/sz_glb_h << " column " << i%sz_glb_h
                             << " lies on no cell edge; the mesh cannot be made continuous" << endl ;
                        refine_ok_ = false ; return ; }
                   continuity_to_node[k][0] = glb_to_node[up]   ;
                   continuity_to_node[k][1] = glb_to_node[down] ;
                   k++ ;
                 }
              else if ( (continuity_node[i] == 2) || (continuity_node[i] == 8) )
                 { //  on a horizontal edge: the supports are its two ends
                   const int right = edge_b[i], left = edge_a[i] ;
                   if (edge_kind[i] != 2 || glb_to_node[right] < 0 || glb_to_node[left] < 0)
                      { cerr << "EOS_Mesh::add_continuity_nodes: a continuity node at row "
                             << i/sz_glb_h << " column " << i%sz_glb_h
                             << " lies on no cell edge; the mesh cannot be made continuous" << endl ;
                        refine_ok_ = false ; return ; }
                   continuity_to_node[k][0] = glb_to_node[right] ;
                   continuity_to_node[k][1] = glb_to_node[left]  ;
                   k++ ;
                 }
              else if ( (continuity_node[i] == 3) || (continuity_node[i] == 9) )
                 { //  node at a cell centre: interpolated at constant p, so
                   //  between its left and right neighbours
                   const int row_lo = (i/sz_glb_h)*sz_glb_h ;
                   const int row_hi = row_lo + sz_glb_h - 1 ;
                   const int right = support_index(node_glb, continuity_node, i,  1, row_lo, row_hi, true) ;
                   const int left  = support_index(node_glb, continuity_node, i, -1, row_lo, row_hi, true) ;
                   if (m < 0 || right < 0 || left < 0 || glb_to_node[right] < 0 || glb_to_node[left] < 0)
                      { cerr << "EOS_Mesh::add_continuity_nodes: no support found for the"
                             << " cell-centre continuity node at row " << i/sz_glb_h
                             << " column " << i%sz_glb_h << "; refinement stopped" << endl ;
                        refine_ok_ = false ; return ; }
                   continuity_to_node[m][0] = glb_to_node[right] ;
                   continuity_to_node[m][1] = glb_to_node[left]  ;
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
