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

#ifndef EOS_IPP_TILESTORE_HXX_
#define EOS_IPP_TILESTORE_HXX_

#include "EOS_Ipp_TileIndex.hxx"
#include <cstddef>
#include <map>
#include <mutex>
#include <string>
#include <vector>

namespace NEPTUNE_EOS
{
  class EOS_Ipp_Tile;

  //! Process-wide store of loaded tiles, shared by every EOS_Ipp_TileCache.
  //!
  //! The parallelization model this code is built around gives each
  //! OpenMP-split domain its own EOS instance, hence its own EOS_Ipp_TileCache
  //! (cf. that class). Without a store, N domains working on neighbouring
  //! parts of the (p,h) plane each read and hold their own copy of the same
  //! tiles: N times the disk reads at start-up, and N times the resident
  //! memory, for data that is identical.
  //!
  //! A tile's EOS_Ipp is immutable once loaded -- the cache only ever calls
  //! compute_prop_ph / compute_prop_p / compute_h_pT on it, which read the
  //! mesh and write nothing (the mutable r1_val/r2_val/save_bound members
  //! belong to init_model()/compute_(), a debug path a tile never goes
  //! through). So one instance can serve every domain at once, and the caches
  //! keep their lock-free compute path.
  //!
  //! Lifetime is by reference count: a cache holds a reference for each tile
  //! it has resident and drops it on eviction, so a tile survives exactly as
  //! long as some cache still wants it. Only acquire/release take the store's
  //! mutex; nothing on the compute path does.
  class EOS_Ipp_TileStore
  {
  public:
    //! Returns the shared, loaded tile for desc, loading it on first use.
    //! Increments its reference count. Null if the load failed.
    //! interpolation_suffix takes part in the identity of a tile: the same
    //! .med read as bicubic and as bilinear are two different objects.
    static EOS_Ipp_Tile *acquire(const EOS_Ipp_TileDescriptor &desc,
                                 const std::string &interpolation_suffix,
                                 const std::vector<std::string> &properties);

    //! Drops one reference; destroys the tile when the last one goes.
    static void release(EOS_Ipp_Tile *tile);

    // Telemetry: what the whole process is holding, as opposed to what one
    // cache thinks it has resident.
    static std::size_t nb_shared_tiles();
    static std::size_t shared_bytes();
    //! Number of tile loads actually served from disk, i.e. first uses. The
    //! difference with the sum of the caches' nb_loads() is what sharing saved.
    static std::size_t nb_disk_loads();

  private:
    static std::mutex &mutex();
    static std::map<std::string, EOS_Ipp_Tile *> &tiles();
    static std::size_t s_disk_loads;
  };
}
#endif /* EOS_IPP_TILESTORE_HXX_ */
