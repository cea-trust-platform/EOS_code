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

#ifndef EOS_IPP_CELLLOCATOR_HXX_
#define EOS_IPP_CELLLOCATOR_HXX_

#include <cstddef>
#include <vector>

namespace NEPTUNE_EOS
{
  //! Which mesh cell contains a point, on the adaptive (p,h) mesh EOS_IGen
  //! produces.
  //!
  //! What this replaces. EOS_Ipp used to answer with a flat lookup table
  //! (fnodes2phnodes) holding one entry per cell of the *finest* virtual grid
  //! -- the grid whose step is the smallest cell in the mesh. Its size is
  //! therefore 4^level_max entries per base cell, no matter how few cells the
  //! mesh actually has, because refining one corner of the domain refines the
  //! index of the whole of it. A 101x101 base at level 5 costs 41 MB, at level
  //! 7 costs 655 MB, and at level 8 costs 2.6 GB -- which is the wall that
  //! made level_max above about 7 unusable, and the reason a database had to
  //! be cut into tiles to be loadable at all. Building it costs the same
  //! order: 10^8 to 10^9 scattered writes, paid again for every tile loaded.
  //!
  //! The mesh is already a quadtree: EOS_IGen starts from a regular
  //! nb_p x nb_h grid and quadrisects cells up to level_max times (cf.
  //! EOS_Mesh::add_local_nodes), so every cell is a square block of the
  //! virtual grid, of side 2^(level_max - level), aligned on a multiple of its
  //! own side. Flattening that into a uniform table throws the structure away
  //! and pays for the finest cell everywhere. This keeps it: memory becomes
  //! O(number of real cells) instead of O(4^level_max), about 5.3 bytes per
  //! cell against 4 bytes per virtual cell.
  //!
  //! The descent is not free, and where the table fits in cache it loses: on
  //! an unrefined 48x48 mesh, whose table is 9 kB and lives in L1, six
  //! dependent loads cost 20-40% more than one subscript. So the table is kept
  //! alongside the tree while it is small (cf. flat_budget_bytes) and used for
  //! the point lookup; the tree takes over above that, where the table's
  //! single load is a guaranteed cache and TLB miss while the upper levels of
  //! a few-MB tree stay resident. The column scan uses the tree either way --
  //! there it wins outright, having replaced a walk over the column's virtual
  //! rows.
  //!
  //! Everything here is in index space -- the (ip, ih) coordinates of the
  //! virtual grid -- so the mapping from (p,h) stays where it already was, in
  //! EOS_Ipp.
  //!
  //! Robustness: the build does not *assume* cells are aligned squares. It
  //! subdivides a block whenever a cell covers only part of it, down to a
  //! single virtual cell if it has to, so a mesh that breaks the quadtree
  //! invariant still gets located correctly -- it just costs more nodes,
  //! degrading towards what the flat table cost. Correctness never depends on
  //! the assumption; only the memory saving does.
  class EOS_Ipp_CellLocator
  {
  public:
    //! Starts a build over a virtual grid of nb_p_virtual x nb_h_virtual
    //! cells. Discards anything previously built.
    void begin(unsigned int nb_p_virtual, unsigned int nb_h_virtual);

    //! Declares that cell occupies [ip0,ip1) x [ih0,ih1) of the virtual grid.
    //! Cells are expected to partition the grid; a box overlapping one already
    //! declared would overwrite it where they meet.
    void add_cell(int cell, long ip0, long ip1, long ih0, long ih1);

    //! Releases the build's spare capacity. Optional.
    void finish();

    bool is_built() const { return depth_ >= 0; }
    void clear();

    //! Cell containing the virtual cell (ip, ih), or -1 where the mesh has no
    //! cell -- outside its bounding box, or in a hole of a mesh that does not
    //! tile it. The caller is expected to have range-checked ip/ih.
    int locate_index(long ip, long ih) const
    {
      // A small flat table beats the tree and is kept when it is small enough
      // to be worth the memory (cf. flat_budget_bytes). Measured on an
      // unrefined 48x48 mesh, whose table is 9 kB and therefore lives in L1,
      // the descent costs 20-40% more on the (p,h) path -- six dependent loads
      // against one. The tree exists for the case the table cannot cover, not
      // to beat it where it fits.
      if (!flat_.empty())
        return flat_[(std::size_t)(ip * (long)nb_h_virtual_ + ih)];

      int node = 0; // the root is internal node 0
      for (int k = depth_ - 1; k >= 0; --k)
      {
        const int v = child_[(std::size_t)(4 * node) + (std::size_t)(((ip >> k) & 1) << 1 | ((ih >> k) & 1))];
        if (v <= 0)
          return -v - 1; // leaf; 0 encodes "no cell"
        node = v - 1;
      }
      return -1; // unreachable: a single-virtual-cell block is always a leaf
    }

    //! True when the flat table was small enough to keep alongside the tree.
    bool has_flat_index() const { return !flat_.empty(); }

    //! Largest flat table worth keeping, in bytes. Above this the table is the
    //! problem the tree solves (4^level_max entries per base cell, hundreds of
    //! MB on a refined database) and only the tree is built. Overridable with
    //! EOS_IPP_FLAT_INDEX_MAX_MB, mostly so the tree can be exercised on a
    //! small database.
    static std::size_t flat_budget_bytes();

    //! Cells of the p-column ip, in ascending h, appended to out (cleared
    //! first). This is what the h(p,T) inversions scan.
    void cells_in_column(long ip, std::vector<unsigned int> &out) const;

    //! Resident size, for the tile cache's memory budget.
    std::size_t footprint_bytes() const
    { return (child_.capacity() + flat_.capacity()) * sizeof(int); }
    std::size_t nb_nodes() const { return child_.size() / 4; }
    int depth() const { return depth_; }

  private:
    //! Slot encoding, shared by the root's children and every other slot:
    //!   0  : leaf, no cell (the value a fresh node is born with)
    //!   <0 : leaf, cell -v-1
    //!   >0 : internal node v-1
    void insert(std::size_t slot, long p0, long h0, long size,
                long cp0, long cp1, long ch0, long ch1, int cell);
    int new_node();

    std::vector<int> child_;
    int depth_ = -1; // the padded square the tree covers has side 1 << depth_

    //! The historical flat table, kept only while it is small (cf.
    //! flat_budget_bytes). Empty otherwise, and locate_index falls back to the
    //! tree.
    std::vector<int> flat_;
    unsigned int nb_p_virtual_ = 0, nb_h_virtual_ = 0;
  };
}
#endif /* EOS_IPP_CELLLOCATOR_HXX_ */
