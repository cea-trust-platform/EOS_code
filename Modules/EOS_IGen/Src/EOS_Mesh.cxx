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
  //! neither, which includes every cell corner. `cross` marks a slot where a
  //! horizontal split of a cell meets a vertical one.
  //!
  //! A slot can be strictly inside an edge of two cells at once -- the two that
  //! share it. When their sides differ the coarser cell is the one that has to
  //! be made continuous, the finer one already carrying a real node there, so
  //! the longer edge wins.
  struct SlotMap
  {
    std::vector<int>  edge_a, edge_b, edge_kind, edge_side, edge_cell ;
    std::vector<char> cross ;
  };

  //! The columns and rows a cell is split along: the interior slots of its
  //! edges that carry a real node, which a finer neighbour put there.
  void cell_splits(const NEPTUNE::ArrOfInt &node_glb, int sz_h, const MeshCell &c,
                   std::vector<int> &vs, std::vector<int> &hs)
  {
    vs.clear() ; hs.clear() ;
    const int r0=c.row, c0=c.col, sd=c.side ;
    for (int t=1; t<sd; t++)
       { if (   node_glb[ r0    *sz_h + c0+t] != 0
             || node_glb[(r0+sd)*sz_h + c0+t] != 0 )  vs.push_back(c0+t) ;
         if (   node_glb[(r0+t)*sz_h + c0]    != 0
             || node_glb[(r0+t)*sz_h + c0+sd] != 0 )  hs.push_back(r0+t) ;
       }
  }

  void map_slots(const NEPTUNE::ArrOfInt &node_glb, int nb_slots, int sz_h,
                 const std::vector<MeshCell> &cells, SlotMap &out)
  {
    out.edge_a.assign(nb_slots, -1) ;
    out.edge_b.assign(nb_slots, -1) ;
    out.edge_kind.assign(nb_slots, 0) ;
    out.edge_side.assign(nb_slots, 0) ;
    out.edge_cell.assign(nb_slots, -1) ;
    out.cross.assign(nb_slots, 0) ;
    std::vector<int> vs, hs ;

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
         cell_splits(node_glb, sz_h, c, vs, hs) ;
         for (int b=0; b<(int)hs.size(); b++)
            for (int a=0; a<(int)vs.size(); a++)
               out.cross[hs[b]*sz_h + vs[a]] = 1 ;
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
  //!    a vertical (resp. horizontal) cell edge, and a type 3 lies where a
  //!    horizontal split crosses a vertical one. Nothing else is a position
  //!    the mesh has room for. A crossing is the cell's centre only when the
  //!    cell has exactly one split each way, which is why looking for the
  //!    centre was too strict: a cell whose neighbour is two levels finer has
  //!    three vertical splits and its crossings sit at the quarter points.
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
         if (kind == 3)  ok = (map.cross[i] != 0) ;
         else            ok = (map.edge_kind[i] == kind) ;
         if (!ok)
            { bad_sound++ ;
              if (verbose && shown++ < 12)
                 std::cerr << "  [check] unsound: continuity node type " << kind
                      << " at row " << i/sz_h << " column " << i%sz_h
                      << " sits on edge_kind " << (int)map.edge_kind[i]
                      << " cross " << (int)map.cross[i] << std::endl ;
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
            
         int k = 1 ;
         int m = 0 ;

         std::vector<MeshCell> cells_glb ;
         collect_cells(node_glb, sz_glb_h, nb_p-1, nb_h-1, int(pow(2,level)), cells_glb) ;
         if ((int)cells_glb.size() != nb_mesh)
            { cerr << "EOS_Mesh::add_local_nodes: the node grid at level " << level
                   << " describes " << cells_glb.size() << " cells where the mesh counts "
                   << nb_mesh << "; refinement stopped" << endl ;
              refine_ok_ = false ; return ; }
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
         
//          Refining a cell fills the whole sub-grid it spans in the next grid.
//          This used to stamp five nodes -- the four edge midpoints and the
//          centre -- around every *global unit cell* the refined cell covers,
//          which leaves out every slot at an even row and an even column. A
//          cell of side 1 has none of those inside it, so nothing showed; a
//          cell of side k has (k-1)^2 in its interior and 2(k-1) more inside
//          its edges, and they were never created. Those slots are corners of
//          the cells the refinement claims to have made, so the mesh came out
//          with cells whose corners hold no node at all: 330 of them on the
//          17x17 WaterLiquid tile at level 2, none at level 1, which is why
//          this stayed hidden until refinement went deep enough to refine a
//          cell coarser than one unit.
         for (int mc = 0; mc < nb_mesh; mc++)
            { if (test_qualities_nodes[mc])  continue ;
              const MeshCell &c = cells_glb[mc] ;
              const int r0 = 2*c.row, c0 = 2*c.col, sd = 2*c.side ;
              for (int a=0; a<=sd; a++)
                 for (int b=0; b<=sd; b++)
                    { const int q = (r0+a)*sz_next_h + c0+b ;
                      if (node_glb_tmp[q] != 0)  continue ;
//                    The tag records the role the node is created in, which is
//                    what the continuity pass reads to find hanging nodes: 3 on
//                    a vertical edge, 2 on a horizontal one, 1 at a corner.
                      if      (b == 0 || b == sd)        node_glb_tmp[q] = 3 ;
                      else if (a == 0 || a == sd)        node_glb_tmp[q] = 2 ;
                      else if ((a & 1) && !(b & 1))      node_glb_tmp[q] = 3 ;
                      else if (!(a & 1) && (b & 1))      node_glb_tmp[q] = 2 ;
                      else                               node_glb_tmp[q] = 1 ;
                    }
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

//          The refined mesh has exactly the nodes the grid now holds. This was
//          a running total kept by a rule that added 3, 4 or 5 per refined
//          global unit cell depending on whether its left and lower neighbours
//          were refined too -- a count of the stamp above that shared no code
//          with it, and that the stamp has now outgrown.
         nb_node = 0 ;
         for (int i=0; i<(int)node_glb_tmp.size(); i++)
            if (node_glb_tmp[i] != 0)  nb_node++ ;
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
  
  //! Places the continuity nodes a locally refined mesh needs, and rewires the
  //! MED connectivity so that every cell written out is a rectangle whose four
  //! corners are nodes.
  //!
  //! A cell is *split* by every real node lying strictly inside one of its
  //! edges -- a hanging node its finer neighbour put there. The split runs
  //! across the cell perpendicular to that edge. A cell with splits therefore
  //! becomes a grid of rectangles, and each hanging node is a corner of the
  //! two that meet at it. That fixes exactly which nodes have to exist:
  //!
  //!   - the far end of every split, on the opposite edge: type 1 where it
  //!     lands on a vertical edge, type 2 on a horizontal one;
  //!   - every crossing of a horizontal split with a vertical one: type 3.
  //!
  //! Where the mesh has no node there already, a continuity node is created.
  //! Types 1 and 2 carry the two ends of the edge they split as supports,
  //! higher coordinate first, which is what forces their value -- a half-sum
  //! for the bilinear reading, the cubic trace for the bicubic one
  //! (cf. EOS_Ipp::retrace_hanging_nodes). Type 3 lies inside the cell, not on
  //! an edge, so it has no junction to match and keeps the model's own value.
  //!
  //! This used to be a scan: from a node tagged as a mid-edge one at creation,
  //! step outwards until something that looked like the far side turned up,
  //! and place a node there. The scan could only ever place one node per edge
  //! and one crossing per cell, both at a fixed offset from the node it
  //! started at, so a cell with hanging nodes on two edges, or with a
  //! neighbour two levels finer, came out with continuity nodes sitting on no
  //! edge at all and splits closing on nothing -- 11 and 72 respectively on
  //! the 17x17 WaterLiquid tile at level 2. It also read those creation tags
  //! to find its starting points, and a node's role changes as the mesh
  //! refines around it while its tag does not.
  //!
  //! Nothing is carried over from the previous level. Which nodes a split
  //! demands is a function of the decomposition as it now stands, so it is
  //! derived again each time. The old code carried its placements forward one
  //! level as types 7, 8 and 9 and zeroed them at the end of the next, which
  //! dropped any that were still needed two levels on.
  void EOS_Mesh::add_continuity_nodes(int level)
  {
    const int sz_glb_h = pow(2,level+1)*nb_h - (pow(2,level+1)-1) ;
    const int sz_glb_p = pow(2,level+1)*nb_p - (pow(2,level+1)-1) ;
    const int sz_glb_m = (sz_glb_p-1) * (sz_glb_h-1) ;
    const int nb_ng    = node_glb.size() ;

//     The decomposition the mesh actually has, and where each slot sits in it.
    std::vector<MeshCell> cells ;
    SlotMap slots ;
    { const int base_side = (sz_glb_h - 1) / (nb_h - 1) ;
      collect_cells(node_glb, sz_glb_h, nb_p-1, nb_h-1, base_side, cells) ;
      map_slots(node_glb, nb_ng, sz_glb_h, cells, slots) ;
    }

//     Where each cell is split, and the nodes those splits demand.
    continuity_node.resize(nb_ng) ;
    continuity_node = 0 ;
    std::vector<int> vs, hs ;
    for (int m=0; m<(int)cells.size(); m++)
       { const MeshCell &c = cells[m] ;
         const int r0=c.row, c0=c.col, sd=c.side ;
         cell_splits(node_glb, sz_glb_h, c, vs, hs) ;

         for (int a=0; a<(int)vs.size(); a++)
            { const int lo = r0*sz_glb_h + vs[a], hi = (r0+sd)*sz_glb_h + vs[a] ;
              if (node_glb[lo] == 0)  continuity_node[lo] = 2 ;
              if (node_glb[hi] == 0)  continuity_node[hi] = 2 ;
            }
         for (int b=0; b<(int)hs.size(); b++)
            { const int lf = hs[b]*sz_glb_h + c0, rt = hs[b]*sz_glb_h + c0+sd ;
              if (node_glb[lf] == 0)  continuity_node[lf] = 1 ;
              if (node_glb[rt] == 0)  continuity_node[rt] = 1 ;
            }
         for (int b=0; b<(int)hs.size(); b++)
            for (int a=0; a<(int)vs.size(); a++)
               { const int q = hs[b]*sz_glb_h + vs[a] ;
                 if (node_glb[q] == 0)  continuity_node[q] = 3 ;
               }
       }

    if (check_continuity_nodes(node_glb, continuity_node, sz_glb_h, slots, cells, level) != 0)
       { cerr << "EOS_Mesh::add_continuity_nodes: the continuity nodes placed at level "
              << level << " do not match the mesh they are meant to close; refinement stopped"
              << endl ;
         refine_ok_ = false ; return ;
       }

//     Node numbering over the mesh and its continuity nodes together, in grid
//     slot order, which is the order every table below and the MED file use.
    ArrOfInt glb_to_node(nb_ng, -1) ;
    int nb_all = 0 ;
    for (int i=0; i<nb_ng; i++)
       { if (node_glb[i] != 0 || continuity_node[i] > 0)
            { glb_to_node[i] = nb_all ;  nb_all++ ; }
       }
    nb_continuity = nb_all - nb_node ;

//     Coordinates. A node's position is fixed by the grid slot it occupies, so
//     the real ones are read back from the domain the refinement built and the
//     continuity ones straight off the grid lines.
    node_h_continuity.resize(nb_all) ;
    node_p_continuity.resize(nb_all) ;
    type_of_node.resize(nb_all) ;
    { int k = 0 ;
      for (int i=0; i<nb_ng; i++)
         { if (node_glb[i] != 0)
              { node_h_continuity[k] = grid_h[i % sz_glb_h] ;
                node_p_continuity[k] = grid_p[i / sz_glb_h] ;
                type_of_node[k] = 0 ;
                k++ ;
              }
           else if (continuity_node[i] > 0)
              { node_h_continuity[k] = grid_h[i % sz_glb_h] ;
                node_p_continuity[k] = grid_p[i / sz_glb_h] ;
                type_of_node[k] = continuity_node[i] ;
                k++ ;
              }
         }
    }

//     One entry per hanging node, in node order, holding the two ends of the
//     edge it splits with the higher coordinate first. EOS_IGen walks the
//     type-1 and type-2 nodes in that order and reads this alongside.
    continuity_to_node.clear() ;
    for (int i=0; i<nb_ng; i++)
       { if (node_glb[i] != 0)  continue ;
         const int t = continuity_node[i] ;
         if (t != 1 && t != 2)  continue ;
         ArrOfInt nn(2) ;
         nn[0] = glb_to_node[slots.edge_b[i]] ;
         nn[1] = glb_to_node[slots.edge_a[i]] ;
         continuity_to_node.push_back(nn) ;
       }

//     MED connectivity. Each cell of the fine global grid is written with the
//     corners of the rectangle of its own cell's split grid that contains it,
//     so a split cell is written as the several rectangles it was cut into and
//     an unsplit one repeats its four corners. This used to be a rewrite of
//     the coarse cell's corners, done per fine cell and able to move at most
//     two of the four, kept across levels in med_correction and replayed on
//     the next grid; the split grid gives it directly.
    med_to_node.clear() ;
    for (int i=0; i<sz_glb_m; i++)
       { ArrOfInt nn(4) ; med_to_node.push_back(nn) ; }

    std::vector<int> xs, ys ;
    for (int m=0; m<(int)cells.size(); m++)
       { const MeshCell &c = cells[m] ;
         const int r0=c.row, c0=c.col, sd=c.side ;
         cell_splits(node_glb, sz_glb_h, c, xs, ys) ;
         xs.insert(xs.begin(), c0) ; xs.push_back(c0+sd) ;
         ys.insert(ys.begin(), r0) ; ys.push_back(r0+sd) ;

         int jb = 0 ;
         for (int c=c0; c<c0+sd; c++)
            { while (xs[jb+1] <= c)  jb++ ;
              int ib = 0 ;
              for (int r=r0; r<r0+sd; r++)
                 { while (ys[ib+1] <= r)  ib++ ;
                   ArrOfInt &nn = med_to_node[r*(sz_glb_h-1) + c] ;
                   nn[0] = glb_to_node[ ys[ib]  *sz_glb_h + xs[jb]  ] ;
                   nn[1] = glb_to_node[ ys[ib]  *sz_glb_h + xs[jb+1]] ;
                   nn[2] = glb_to_node[ ys[ib+1]*sz_glb_h + xs[jb]  ] ;
                   nn[3] = glb_to_node[ ys[ib+1]*sz_glb_h + xs[jb+1]] ;
                 }
            }
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
