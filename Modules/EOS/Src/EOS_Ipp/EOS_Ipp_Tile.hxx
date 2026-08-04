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

#ifndef EOS_IPP_TILE_HXX_
#define EOS_IPP_TILE_HXX_

#include "EOS_Ipp_TileIndex.hxx"
#include <chrono>

namespace NEPTUNE_EOS
{
  class EOS_Ipp; // defined in EOS_Ipp.hxx

  //! One cached, lazily-loaded tile of a tiled EOS_Ipp database: an EOS_Ipp
  //! instance built from a single tile's standalone .med file (through the
  //! historical, unmodified EOS_Ipp loading code, cf.
  //! EOS_Ipp::load_from_med_path), plus the bookkeeping EOS_Ipp_TileCache
  //! needs to decide when the tile can be evicted.
  //!
  //! Not thread-safe on its own: ensure_loaded() mutates the object, so
  //! concurrent loads of the *same* EOS_Ipp_Tile must be serialized by the
  //! caller. In this codebase's parallelization model (one EOS instance --
  //! hence one EOS_Ipp_TileCache -- per OpenMP-split domain, cf. the mesh
  //! manager design notes), a given EOS_Ipp_Tile is only ever touched by the
  //! single thread that owns its cache, so no locking is needed there; only
  //! the underlying .med/HDF5 file I/O performed by ensure_loaded() is
  //! serialized process-wide, in case two domains load the same tile file at
  //! the same time (cf. EOS_Ipp_TileCache::s_load_mutex).
  class EOS_Ipp_Tile
  {
  public:
    explicit EOS_Ipp_Tile(const EOS_Ipp_TileDescriptor &desc) : desc_(desc) {}
    ~EOS_Ipp_Tile();

    EOS_Ipp_Tile(const EOS_Ipp_Tile &) = delete;
    EOS_Ipp_Tile &operator=(const EOS_Ipp_Tile &) = delete;

    //! Loads the underlying EOS_Ipp from desc_.med_file if not already
    //! loaded. Returns true on success.
    bool ensure_loaded(const std::string &interpolation_suffix);

    bool is_loaded() const { return ipp_ != nullptr; }
    EOS_Ipp *ipp() const { return ipp_; }
    const EOS_Ipp_TileDescriptor &descriptor() const { return desc_; }

    void touch() { last_access_ = std::chrono::steady_clock::now(); }
    std::chrono::steady_clock::time_point last_access() const { return last_access_; }

    //! Pinning keeps a tile out of the LRU eviction pool while it is in use
    //! by the batch of points currently being dispatched by the cache.
    void pin() { ++pin_count_; }
    void unpin()
    {
      if (pin_count_ > 0)
        --pin_count_;
    }
    bool is_pinned() const { return pin_count_ > 0; }

    //! Rough memory footprint, used by EOS_Ipp_TileCache to enforce a
    //! tile-count (or, in a later iteration, a byte) budget.
    std::size_t footprint_hint() const { return 1; }

  private:
    EOS_Ipp_TileDescriptor desc_;
    EOS_Ipp *ipp_ = nullptr;
    int pin_count_ = 0;
    std::chrono::steady_clock::time_point last_access_ = std::chrono::steady_clock::now();
  };
}
#endif /* EOS_IPP_TILE_HXX_ */
