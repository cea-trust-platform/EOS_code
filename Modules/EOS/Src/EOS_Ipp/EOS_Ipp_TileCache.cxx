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

#include "EOS_Ipp_TileCache.hxx"
#include "EOS_Ipp_Tile.hxx"
#include "EOS_Ipp_TileStore.hxx"
#include "EOS_Ipp.hxx"

namespace NEPTUNE_EOS
{
  EOS_Ipp_TileCache::EOS_Ipp_TileCache(const std::string &manifest_path,
                                        const std::string &interpolation_suffix,
                                        std::size_t budget_bytes,
                                        std::size_t max_resident_tiles)
      : interpolation_suffix_(interpolation_suffix),
        budget_bytes_(budget_bytes),
        max_resident_tiles_(max_resident_tiles)
  {
    if (budget_bytes_ == 0 && max_resident_tiles_ == 0)
      budget_bytes_ = DEFAULT_BUDGET_BYTES;

    valid_ = index_.load(manifest_path);
    if (valid_)
    {
      resident_.assign((std::size_t)index_.nb_tiles(), nullptr);
      last_use_.assign((std::size_t)index_.nb_tiles(), 0);
    }
  }

  EOS_Ipp_TileCache::~EOS_Ipp_TileCache()
  {
    // Drop this cache's references; the store destroys a tile once no cache
    // holds it any more.
    for (int tile_id : loaded_ids_)
      EOS_Ipp_TileStore::release(resident_[(std::size_t)tile_id]);
  }

  // Hot path: one bounds test, one array load, one null test, one store.
  EOS_Ipp_Tile *EOS_Ipp_TileCache::acquire(int tile_id)
  {
    if (tile_id < 0 || (std::size_t)tile_id >= resident_.size())
      return nullptr;

    EOS_Ipp_Tile *tile = resident_[(std::size_t)tile_id];
    if (tile != nullptr)
    {
      last_use_[(std::size_t)tile_id] = ++tick_;
      return tile;
    }
    return acquire_miss(tile_id);
  }

  EOS_Ipp_Tile *EOS_Ipp_TileCache::acquire_miss(int tile_id)
  {
    // Shared with every other cache in the process: this reads the .med only
    // if no one else has the tile already (cf. EOS_Ipp_TileStore).
    EOS_Ipp_Tile *tile = EOS_Ipp_TileStore::acquire(index_.tile(tile_id), interpolation_suffix_);
    if (tile == nullptr)
      return nullptr;

    ++nb_loads_;
    last_use_[(std::size_t)tile_id] = ++tick_;
    resident_[(std::size_t)tile_id] = tile;
    loaded_ids_.push_back(tile_id);
    resident_bytes_ += tile->footprint_bytes();
    enforce_budget();
    return tile;
  }

  std::size_t EOS_Ipp_TileCache::estimated_capacity_tiles() const
  {
    if (max_resident_tiles_ > 0 && budget_bytes_ == 0)
      return max_resident_tiles_;

    if (loaded_ids_.empty() || resident_bytes_ == 0)
      return 0; // nothing loaded yet: no basis for an average tile size

    const std::size_t average = resident_bytes_ / loaded_ids_.size();
    std::size_t capacity = (average > 0) ? budget_bytes_ / average : 0;
    if (max_resident_tiles_ > 0 && max_resident_tiles_ < capacity)
      capacity = max_resident_tiles_;
    return capacity;
  }

  bool EOS_Ipp_TileCache::over_budget() const
  {
    if (max_resident_tiles_ > 0 && loaded_ids_.size() > max_resident_tiles_)
      return true;
    if (budget_bytes_ > 0 && resident_bytes_ > budget_bytes_)
      return true;
    return false;
  }

  void EOS_Ipp_TileCache::enforce_budget()
  {
    // Called only when a tile was just acquired; the scan for the least
    // recently used victim is paid here rather than by keeping an MRU order
    // up to date on every access.
    while (over_budget() && loaded_ids_.size() > 1)
    {
      std::size_t victim_k = 0;
      for (std::size_t k = 1; k < loaded_ids_.size(); ++k)
      {
        if (last_use_[(std::size_t)loaded_ids_[k]] < last_use_[(std::size_t)loaded_ids_[victim_k]])
          victim_k = k;
      }

      const int victim_id = loaded_ids_[victim_k];
      if (last_use_[(std::size_t)victim_id] == tick_)
        break; // the tile we just acquired is the only candidate: keep it

      EOS_Ipp_Tile *victim = resident_[(std::size_t)victim_id];
      resident_bytes_ -= victim->footprint_bytes();
      resident_[(std::size_t)victim_id] = nullptr;
      loaded_ids_[victim_k] = loaded_ids_.back();
      loaded_ids_.pop_back();
      EOS_Ipp_TileStore::release(victim);
      ++nb_evictions_;
    }
    // A single tile larger than the whole budget stays resident: refusing to
    // keep it would mean reloading it on every point.
  }

  NEPTUNE::EOS_Internal_Error EOS_Ipp_TileCache::compute_prop_ph(NEPTUNE::EOS_Property prop,
                                                                  double p, double h, double &res)
  {
    if (!valid_)
      return EOS_Ipp::MODEL_NOT_INIT;

    EOS_Ipp_Tile *tile = acquire(index_.locate(p, h));
    if (tile == nullptr)
      return EOS_Ipp::OUT_OF_BOUNDS;

    return tile->ipp()->compute_prop_ph(prop, p, h, res);
  }

  NEPTUNE::EOS_Internal_Error EOS_Ipp_TileCache::compute_prop_p(NEPTUNE::EOS_Property prop,
                                                                 double p, int sat_lim, double &res)
  {
    if (!valid_)
      return EOS_Ipp::MODEL_NOT_INIT;

    // The saturation/limit curves are 1D in p and every tile of a p-column is
    // generated over that column's own p-range, so they all carry a bit-for-bit
    // identical copy of them (checked between tiles of a column: zero relative
    // difference on T_sat and h_l_sat). Any resident tile of the column can
    // therefore answer, and answering from one avoids pulling a whole 2D tile
    // off disk just to read a 1D curve -- which is what always taking the
    // column's first tile used to do, evicting something else in the process.
    // Since the copies are identical, which tile answers cannot change the
    // result.
    index_.tiles_in_column(p, column_scratch_);
    if (column_scratch_.empty())
      return EOS_Ipp::OUT_OF_BOUNDS;

    EOS_Ipp_Tile *tile = nullptr;
    for (int tile_id : column_scratch_)
    {
      if (tile_id >= 0 && (std::size_t)tile_id < resident_.size() && resident_[(std::size_t)tile_id] != nullptr)
      {
        tile = acquire(tile_id);
        break;
      }
    }
    if (tile == nullptr)
      tile = acquire(column_scratch_.front());
    if (tile == nullptr)
      return EOS_Ipp::OUT_OF_BOUNDS;

    return tile->ipp()->compute_prop_p(prop, p, sat_lim, res);
  }

  NEPTUNE::EOS_Internal_Error EOS_Ipp_TileCache::compute_h_pT(double p, double T, double &res)
  {
    if (!valid_)
      return EOS_Ipp::MODEL_NOT_INIT;

    // Only the tiles whose generation T-range can hold the root: the rest
    // cannot answer, and visiting them would mean loading them from disk to
    // be told so.
    index_.tiles_in_column_for_T(p, T, column_scratch_);
    const std::vector<int> &column = column_scratch_;
    if (column.empty())
      return EOS_Ipp::OUT_OF_BOUNDS;

    NEPTUNE::EOS_Internal_Error last_err = EOS_Ipp::OUT_OF_BOUNDS;
    for (int tile_id : column)
    {
      EOS_Ipp_Tile *tile = acquire(tile_id);
      if (tile == nullptr)
        continue;
      last_err = tile->ipp()->compute_h_pT(p, T, res);
      if (last_err == NEPTUNE::EOS_Internal_Error::OK)
        return last_err;
    }
    return last_err;
  }
}
