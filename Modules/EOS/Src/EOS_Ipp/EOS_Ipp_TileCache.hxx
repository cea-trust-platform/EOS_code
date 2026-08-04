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

#ifndef EOS_IPP_TILECACHE_HXX_
#define EOS_IPP_TILECACHE_HXX_

#include "EOS_Ipp_TileIndex.hxx"
#include "EOS/API/EOS_Error.hxx"
#include "EOS/API/EOS_properties.hxx"
#include <list>
#include <unordered_map>
#include <mutex>
#include <string>

namespace NEPTUNE_EOS
{
  class EOS_Ipp_Tile;

  //! Per-EOS_Ipp-instance tile cache: lazily loads the (p,h) tiles a tiled
  //! EOS_Ipp database is split into, keeps the most recently used ones
  //! resident (LRU, bounded by max_resident_tiles), and evicts the rest.
  //!
  //! Threading model: this class is deliberately *not* internally
  //! synchronized for its compute_* hot path. In this codebase OpenMP is
  //! used to split the computation domain and give each split domain its
  //! own EOS instance (hence its own EOS_Ipp, hence its own
  //! EOS_Ipp_TileCache) -- so a given cache is only ever touched by the one
  //! thread that owns it, and no lock is needed to call compute_prop_ph /
  //! compute_prop_p / compute_h_pT concurrently with another domain's cache.
  //! The one piece of state that *is* genuinely shared across domains is the
  //! underlying .med/HDF5 files on disk: two domains can legitimately need
  //! the same tile at the same time. To stay safe without knowing whether
  //! the MED/HDF5 libraries linked in are built thread-safe,
  //! EOS_Ipp_Tile::ensure_loaded() calls are serialized process-wide via
  //! s_load_mutex -- held only for the (rare) duration of a tile load, never
  //! during compute calls.
  class EOS_Ipp_TileCache
  {
  public:
    //! interpolation_suffix: "bicubic"/"bilinear"/"" (cf. EOS_Ipp's own
    //! ":bicubic"/":bilinear" file-name suffix), applied to every tile file.
    EOS_Ipp_TileCache(const std::string &manifest_path,
                       const std::string &interpolation_suffix,
                       std::size_t max_resident_tiles = 64);
    ~EOS_Ipp_TileCache();

    EOS_Ipp_TileCache(const EOS_Ipp_TileCache &) = delete;
    EOS_Ipp_TileCache &operator=(const EOS_Ipp_TileCache &) = delete;

    bool is_valid() const { return valid_; }
    const EOS_Ipp_TileIndex &index() const { return index_; }

    NEPTUNE::EOS_Internal_Error compute_prop_ph(NEPTUNE::EOS_Property prop, double p, double h, double &res);
    NEPTUNE::EOS_Internal_Error compute_prop_p(NEPTUNE::EOS_Property prop, double p, int sat_lim, double &res);
    NEPTUNE::EOS_Internal_Error compute_h_pT(double p, double T, double &res);

    // Telemetry, useful to size max_resident_tiles / the (p,h) tiling grid.
    std::size_t nb_loads() const { return nb_loads_; }
    std::size_t nb_evictions() const { return nb_evictions_; }
    std::size_t nb_resident() const { return mru_.size(); }

  private:
    // Resolves tile_id to a loaded, MRU-touched, EOS_Ipp_Tile*; loads it on
    // first access and evicts the least-recently-used non-pinned tile(s) if
    // that pushes residency over max_resident_tiles_. Returns nullptr if
    // tile_id < 0 (out of the tiled domain) or the load failed.
    EOS_Ipp_Tile *acquire(int tile_id);
    void enforce_budget();

    EOS_Ipp_TileIndex index_;
    bool valid_ = false;
    std::string interpolation_suffix_;
    std::size_t max_resident_tiles_;

    std::list<EOS_Ipp_Tile *> mru_; // front = most recently used
    std::unordered_map<int, std::list<EOS_Ipp_Tile *>::iterator> lookup_; // tile_id -> position in mru_

    std::size_t nb_loads_ = 0;
    std::size_t nb_evictions_ = 0;

    static std::mutex s_load_mutex;
  };
}
#endif /* EOS_IPP_TILECACHE_HXX_ */
