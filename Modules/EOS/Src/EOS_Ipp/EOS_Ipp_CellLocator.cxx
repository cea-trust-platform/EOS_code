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

#include "EOS_Ipp_CellLocator.hxx"
#include <cstdlib>

namespace NEPTUNE_EOS
{
  std::size_t EOS_Ipp_CellLocator::flat_budget_bytes()
  {
    static const std::size_t budget = []() {
      const char *env = getenv("EOS_IPP_FLAT_INDEX_MAX_MB");
      const long mb = (env != nullptr) ? atol(env) : 16;
      return (mb >= 0) ? (std::size_t)mb * 1024u * 1024u : (std::size_t)0;
    }();
    return budget;
  }

  void EOS_Ipp_CellLocator::clear()
  {
    child_.clear();
    flat_.clear();
    depth_ = -1;
    nb_p_virtual_ = nb_h_virtual_ = 0;
  }

  void EOS_Ipp_CellLocator::begin(unsigned int nb_p_virtual, unsigned int nb_h_virtual)
  {
    clear();
    if (nb_p_virtual == 0 || nb_h_virtual == 0)
      return;

    // The tree covers a square whose side is a power of two, so that halving
    // stays exact at every level. The virtual grid is generally a rectangle
    // and generally not a power of two, so the square is padded; the padding
    // is never reached, because callers range-check (ip,ih) first and nothing
    // is inserted there.
    nb_p_virtual_ = nb_p_virtual;
    nb_h_virtual_ = nb_h_virtual;

    // Keep the flat table too when it is small: it is one load against a
    // descent, and the memory it costs in that regime is irrelevant. The tree
    // is built either way, because the column scan wants it whatever the size.
    const std::size_t flat_bytes =
        (std::size_t)nb_p_virtual * (std::size_t)nb_h_virtual * sizeof(int);
    if (flat_bytes <= flat_budget_bytes())
      flat_.assign((std::size_t)nb_p_virtual * (std::size_t)nb_h_virtual, -1);

    const unsigned int side = (nb_p_virtual > nb_h_virtual) ? nb_p_virtual : nb_h_virtual;
    depth_ = 0;
    while (((unsigned long)1 << depth_) < (unsigned long)side)
      ++depth_;

    child_.assign(4, 0); // internal node 0 is the root
  }

  int EOS_Ipp_CellLocator::new_node()
  {
    const int node = (int)(child_.size() / 4);
    child_.resize(child_.size() + 4, 0);
    return node;
  }

  void EOS_Ipp_CellLocator::insert(std::size_t slot, long p0, long h0, long size,
                                    long cp0, long cp1, long ch0, long ch1, int cell)
  {
    // The block is entirely inside the cell: it is a leaf, whatever its size.
    // This is the case that keeps the tree small -- an aligned square cell
    // ends the descent at exactly one node.
    if (cp0 <= p0 && p0 + size <= cp1 && ch0 <= h0 && h0 + size <= ch1)
    {
      child_[slot] = -(cell + 1);
      return;
    }

    // A single virtual cell cannot be split further. Reached only by a mesh
    // whose cells are not aligned squares; the cell owns it, since it covers
    // part of it and cells are meant to partition the grid.
    if (size == 1)
    {
      child_[slot] = -(cell + 1);
      return;
    }

    int node;
    if (child_[slot] > 0)
      node = child_[slot] - 1;
    else
    {
      node = new_node(); // may reallocate child_, so re-subscript rather than keep a reference
      child_[slot] = node + 1;
    }

    const long half = size / 2;
    for (int c = 0; c < 4; ++c)
    {
      const long np0 = p0 + (((c >> 1) & 1) ? half : 0);
      const long nh0 = h0 + ((c & 1) ? half : 0);
      // only the children the cell actually reaches
      if (np0 + half <= cp0 || np0 >= cp1 || nh0 + half <= ch0 || nh0 >= ch1)
        continue;
      insert((std::size_t)(4 * node + c), np0, nh0, half, cp0, cp1, ch0, ch1, cell);
    }
  }

  void EOS_Ipp_CellLocator::add_cell(int cell, long ip0, long ip1, long ih0, long ih1)
  {
    if (depth_ < 0 || cell < 0 || ip1 <= ip0 || ih1 <= ih0)
      return;

    if (!flat_.empty())
      for (long ip = ip0; ip < ip1; ++ip)
        for (long ih = ih0; ih < ih1; ++ih)
          flat_[(std::size_t)(ip * (long)nb_h_virtual_ + ih)] = cell;

    const long size = (long)1 << depth_;
    const long half = size / 2;
    if (depth_ == 0)
    {
      // Degenerate mesh of a single virtual cell: the root's children are the
      // grid, so there is nothing to descend.
      for (int c = 0; c < 4; ++c)
        child_[(std::size_t)c] = -(cell + 1);
      return;
    }

    for (int c = 0; c < 4; ++c)
    {
      const long np0 = (((c >> 1) & 1) ? half : 0);
      const long nh0 = ((c & 1) ? half : 0);
      if (np0 + half <= ip0 || np0 >= ip1 || nh0 + half <= ih0 || nh0 >= ih1)
        continue;
      insert((std::size_t)c, np0, nh0, half, ip0, ip1, ih0, ih1, cell);
    }
  }

  void EOS_Ipp_CellLocator::finish()
  {
    std::vector<int>(child_).swap(child_);
  }


  namespace
  {
    // Leaves under this node whose block spans the column ip, in ascending h.
    // Only the two children on the right side of the p split are visited, and
    // for each of them h=0 before h=1, which is what puts the result in
    // ascending h order.
    void collect(const std::vector<int> &child, int node, int k, long ip,
                 std::vector<unsigned int> &out)
    {
      const int pbit = (int)((ip >> k) & 1);
      for (int hbit = 0; hbit < 2; ++hbit)
      {
        const int v = child[(std::size_t)(4 * node + (pbit << 1 | hbit))];
        if (v == 0)
          continue; // no cell here
        if (v < 0)
        {
          const unsigned int cell = (unsigned int)(-v - 1);
          // A cell that is an aligned square appears as exactly one leaf. One
          // that is not can span several, and they are consecutive here, so
          // this keeps the list free of repeats either way.
          if (out.empty() || out.back() != cell)
            out.push_back(cell);
          continue;
        }
        collect(child, v - 1, k - 1, ip, out);
      }
    }
  }

  void EOS_Ipp_CellLocator::cells_in_column(long ip, std::vector<unsigned int> &out) const
  {
    out.clear();
    if (depth_ < 0)
      return;
    if (depth_ == 0)
    {
      for (int c = 0; c < 2; ++c)
      {
        const int v = child_[(std::size_t)((int)(ip & 1) << 1 | c)];
        if (v < 0)
        {
          const unsigned int cell = (unsigned int)(-v - 1);
          if (out.empty() || out.back() != cell)
            out.push_back(cell);
        }
      }
      return;
    }
    collect(child_, 0, depth_ - 1, ip, out);
  }
}
