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
#include <mutex>
#include <string>
#include <vector>

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
    std::size_t nb_resident() const { return loaded_.size(); }

  private:
    // Resolves tile_id to a loaded EOS_Ipp_Tile*, stamping it as the most
    // recently used; loads it on first access and evicts if that pushes
    // residency over max_resident_tiles_. Returns nullptr if tile_id < 0
    // (out of the tiled domain) or the load failed.
    EOS_Ipp_Tile *acquire(int tile_id);
    // Out-of-line slow path of acquire(): everything but the resident hit.
    EOS_Ipp_Tile *acquire_miss(int tile_id);
    void enforce_budget();

    EOS_Ipp_TileIndex index_;
    bool valid_ = false;
    std::string interpolation_suffix_;
    std::size_t max_resident_tiles_;

    // Residency is a flat array indexed by tile id -- one load and a null
    // test to route a point -- rather than a hash lookup plus a list splice
    // per call. Recency is a counter stamped on the tile, and the victim is
    // found by a scan of the (few) loaded tiles when a load actually needs
    // room; evictions are rare next to hits, so paying there instead of on
    // every single access is the cheaper trade.
    std::vector<EOS_Ipp_Tile *> resident_; // size nb_tiles, null where not loaded
    std::vector<EOS_Ipp_Tile *> loaded_;   // the non-null entries of resident_, unordered
    std::size_t tick_ = 0;

    // Reused across compute_h_pT calls so the column scan allocates nothing
    // per point. Safe as plain state: a cache belongs to one thread
    // (cf. the class comment above).
    std::vector<int> column_scratch_;

    std::size_t nb_loads_ = 0;
    std::size_t nb_evictions_ = 0;

    static std::mutex s_load_mutex;
  };
}
#endif /* EOS_IPP_TILECACHE_HXX_ */
