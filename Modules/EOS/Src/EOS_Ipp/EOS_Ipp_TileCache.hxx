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

  //! Per-EOS_Ipp-instance tile cache: decides which (p,h) tile a query
  //! belongs to, keeps the ones this instance is using resident within a
  //! budget, and evicts the least recently used beyond it.
  //!
  //! What it does *not* own is the tiles themselves. Those live in the
  //! process-wide EOS_Ipp_TileStore and are shared by every cache: a cache
  //! holds a reference for each tile it has resident and drops it on
  //! eviction. So "resident in this cache" is a per-instance decision, while
  //! "loaded in memory" is a process-wide fact, and N OpenMP domains working
  //! on the same region read each tile once and hold one copy of it.
  //!
  //! Threading model: this class is deliberately *not* internally
  //! synchronized for its compute_* hot path. In this codebase OpenMP is
  //! used to split the computation domain and give each split domain its
  //! own EOS instance (hence its own EOS_Ipp, hence its own
  //! EOS_Ipp_TileCache) -- so a given cache is only ever touched by the one
  //! thread that owns it, and no lock is needed to call compute_prop_ph /
  //! compute_prop_p / compute_h_pT concurrently with another domain's cache.
  //! Several caches may be reading the *same* shared tile at that moment,
  //! which is safe because a loaded tile is immutable (cf. EOS_Ipp_TileStore).
  //! Only acquiring and releasing a tile takes the store's mutex, which also
  //! serializes the .med/HDF5 reads, MED not being known to be thread-safe.
  class EOS_Ipp_TileCache
  {
  public:
    //! Default resident budget, used when the caller asks for neither a byte
    //! nor a tile budget. Deliberately expressed in bytes: how much memory a
    //! given number of tiles costs depends entirely on how finely each was
    //! meshed, so a tile count is not something a host code can size against
    //! the memory it actually has.
    static const std::size_t DEFAULT_BUDGET_BYTES = 512u * 1024u * 1024u;

    //! interpolation_suffix: "bicubic"/"bilinear"/"" (cf. EOS_Ipp's own
    //! ":bicubic"/":bilinear" file-name suffix), applied to every tile file.
    //! budget_bytes / max_resident_tiles: resident budget; whichever is
    //! non-zero applies, and if both are, a tile must satisfy both. Passing 0
    //! for both selects DEFAULT_BUDGET_BYTES.
    EOS_Ipp_TileCache(const std::string &manifest_path,
                       const std::string &interpolation_suffix,
                       std::size_t budget_bytes = 0,
                       std::size_t max_resident_tiles = 0);
    ~EOS_Ipp_TileCache();

    EOS_Ipp_TileCache(const EOS_Ipp_TileCache &) = delete;
    EOS_Ipp_TileCache &operator=(const EOS_Ipp_TileCache &) = delete;

    bool is_valid() const { return valid_; }
    const EOS_Ipp_TileIndex &index() const { return index_; }

    NEPTUNE::EOS_Internal_Error compute_prop_ph(NEPTUNE::EOS_Property prop, double p, double h, double &res);
    NEPTUNE::EOS_Internal_Error compute_prop_p(NEPTUNE::EOS_Property prop, double p, int sat_lim, double &res);
    NEPTUNE::EOS_Internal_Error compute_h_pT(double p, double T, double &res);

    // Telemetry, useful to size the budget / the (p,h) tiling grid. Loads far
    // above the number of distinct tiles a run touches means thrashing.
    std::size_t nb_loads() const { return nb_loads_; }
    std::size_t nb_evictions() const { return nb_evictions_; }
    std::size_t nb_resident() const { return loaded_ids_.size(); }
    std::size_t resident_bytes() const { return resident_bytes_; }
    std::size_t budget_bytes() const { return budget_bytes_; }

    //! Roughly how many tiles the budget can hold at once, from the average
    //! size of what is resident. Used to decide whether a batch of points is
    //! worth regrouping by tile before computing it (cf. EOS_Ipp::compute):
    //! a batch that fits does not need it, one that does not would otherwise
    //! reload tiles all the way through. Returns 0 while nothing is loaded
    //! yet, i.e. "no opinion".
    std::size_t estimated_capacity_tiles() const;

  private:
    // Resolves tile_id to a loaded EOS_Ipp_Tile*, stamping it as the most
    // recently used; loads it on first access and evicts if that pushes
    // residency over max_resident_tiles_. Returns nullptr if tile_id < 0
    // (out of the tiled domain) or the load failed.
    EOS_Ipp_Tile *acquire(int tile_id);
    // Out-of-line slow path of acquire(): everything but the resident hit.
    EOS_Ipp_Tile *acquire_miss(int tile_id);
    bool over_budget() const;
    void enforce_budget();

    EOS_Ipp_TileIndex index_;
    bool valid_ = false;
    std::string interpolation_suffix_;
    std::size_t budget_bytes_ = 0;        // 0 = no byte limit
    std::size_t max_resident_tiles_ = 0;  // 0 = no tile-count limit
    std::size_t resident_bytes_ = 0;

    // Residency is a flat array indexed by tile id -- one load and a null
    // test to route a point -- rather than a hash lookup plus a list splice
    // per call. Recency is a counter, in a parallel array because the tile
    // objects themselves are shared with the other caches of the process
    // (cf. EOS_Ipp_TileStore) and each cache has its own idea of what it used
    // last. The victim is found by scanning the (few) resident tiles when a
    // load actually needs room; evictions are rare next to hits, so paying
    // there instead of on every access is the cheaper trade.
    std::vector<EOS_Ipp_Tile *> resident_; // size nb_tiles, null where not resident here
    std::vector<std::size_t> last_use_;    // size nb_tiles, parallel to resident_
    std::vector<int> loaded_ids_;          // ids of the non-null entries, unordered
    std::size_t tick_ = 0;

    // Reused across compute_h_pT calls so the column scan allocates nothing
    // per point. Safe as plain state: a cache belongs to one thread
    // (cf. the class comment above).
    std::vector<int> column_scratch_;

    std::size_t nb_loads_ = 0;
    std::size_t nb_evictions_ = 0;

    
  };
}
#endif /* EOS_IPP_TILECACHE_HXX_ */
