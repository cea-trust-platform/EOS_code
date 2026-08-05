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
#include "EOS_Ipp.hxx"

namespace NEPTUNE_EOS
{
  std::mutex EOS_Ipp_TileCache::s_load_mutex;

  EOS_Ipp_TileCache::EOS_Ipp_TileCache(const std::string &manifest_path,
                                        const std::string &interpolation_suffix,
                                        std::size_t max_resident_tiles)
      : interpolation_suffix_(interpolation_suffix),
        max_resident_tiles_(max_resident_tiles > 0 ? max_resident_tiles : 1)
  {
    valid_ = index_.load(manifest_path);
  }

  EOS_Ipp_TileCache::~EOS_Ipp_TileCache()
  {
    for (EOS_Ipp_Tile *tile : mru_)
      delete tile;
  }

  EOS_Ipp_Tile *EOS_Ipp_TileCache::acquire(int tile_id)
  {
    if (tile_id < 0)
      return nullptr;

    auto found = lookup_.find(tile_id);
    if (found != lookup_.end())
    {
      EOS_Ipp_Tile *tile = *(found->second);
      if (found->second != mru_.begin())
      {
        mru_.splice(mru_.begin(), mru_, found->second);
        found->second = mru_.begin();
      }
      tile->touch();
      return tile;
    }

    EOS_Ipp_Tile *tile = new EOS_Ipp_Tile(index_.tile(tile_id));
    bool ok;
    {
      // Only the .med/HDF5 I/O is serialized: two domains needing the same
      // tile at the same time must not open it concurrently, but once
      // loaded every subsequent compute_* call is lock-free.
      std::lock_guard<std::mutex> lock(s_load_mutex);
      ok = tile->ensure_loaded(interpolation_suffix_);
    }
    if (!ok)
    {
      delete tile;
      return nullptr;
    }

    ++nb_loads_;
    tile->touch();
    mru_.push_front(tile);
    lookup_[tile_id] = mru_.begin();
    enforce_budget();
    return tile;
  }

  void EOS_Ipp_TileCache::enforce_budget()
  {
    while (mru_.size() > max_resident_tiles_)
    {
      auto victim_it = mru_.end();
      for (auto rit = mru_.rbegin(); rit != mru_.rend(); ++rit)
      {
        if (!(*rit)->is_pinned())
        {
          victim_it = std::next(rit).base();
          break;
        }
      }
      if (victim_it == mru_.end())
        break; // every resident tile is pinned: over budget, but nothing evictable right now

      EOS_Ipp_Tile *victim = *victim_it;
      lookup_.erase(victim->descriptor().id);
      mru_.erase(victim_it);
      delete victim;
      ++nb_evictions_;
    }
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

    EOS_Ipp_Tile *tile = acquire(index_.locate_column(p));
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
