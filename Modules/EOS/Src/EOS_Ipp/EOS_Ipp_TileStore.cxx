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

#include "EOS_Ipp_TileStore.hxx"
#include "EOS_Ipp_Tile.hxx"

namespace NEPTUNE_EOS
{
  std::size_t EOS_Ipp_TileStore::s_disk_loads = 0;

  // Function-local statics: the store is reached from EOS_Ipp::init(), which a
  // host code may well call from a static initializer of its own, so the order
  // of construction must not matter.
  std::mutex &EOS_Ipp_TileStore::mutex()
  {
    static std::mutex m;
    return m;
  }

  std::map<std::string, EOS_Ipp_Tile *> &EOS_Ipp_TileStore::tiles()
  {
    static std::map<std::string, EOS_Ipp_Tile *> t;
    return t;
  }

  EOS_Ipp_Tile *EOS_Ipp_TileStore::acquire(const EOS_Ipp_TileDescriptor &desc,
                                            const std::string &interpolation_suffix,
                                            const std::vector<std::string> &properties)
  {
    // The .med path identifies the data; the interpolation method and the set
    // of properties loaded identify how much of it was read and how. Two tiles
    // differing in either are different objects and must not be shared.
    std::string key = desc.med_file + "|" + interpolation_suffix;
    for (std::size_t k = 0; k < properties.size(); k++)
      key += "|" + properties[k];

    std::lock_guard<std::mutex> lock(mutex());

    std::map<std::string, EOS_Ipp_Tile *>::iterator found = tiles().find(key);
    if (found != tiles().end())
    {
      found->second->add_ref();
      return found->second;
    }

    // First use: read it. The load stays inside the lock, which serializes
    // concurrent loads process-wide exactly as the previous per-cache load
    // mutex did -- and now also means two domains wanting the same tile at the
    // same time produce one read instead of two.
    EOS_Ipp_Tile *tile = new EOS_Ipp_Tile(desc);
    if (!tile->ensure_loaded(interpolation_suffix, properties))
    {
      delete tile;
      return nullptr;
    }

    ++s_disk_loads;
    tile->set_store_key(key);
    tile->add_ref();
    tiles()[key] = tile;
    return tile;
  }

  void EOS_Ipp_TileStore::release(EOS_Ipp_Tile *tile)
  {
    if (tile == nullptr)
      return;

    std::lock_guard<std::mutex> lock(mutex());
    if (tile->drop_ref() > 0)
      return;

    tiles().erase(tile->store_key());
    delete tile;
  }

  std::size_t EOS_Ipp_TileStore::nb_shared_tiles()
  {
    std::lock_guard<std::mutex> lock(mutex());
    return tiles().size();
  }

  std::size_t EOS_Ipp_TileStore::shared_bytes()
  {
    std::lock_guard<std::mutex> lock(mutex());
    std::size_t bytes = 0;
    for (std::map<std::string, EOS_Ipp_Tile *>::const_iterator it = tiles().begin();
         it != tiles().end(); ++it)
      bytes += it->second->footprint_bytes();
    return bytes;
  }

  std::size_t EOS_Ipp_TileStore::nb_disk_loads()
  {
    std::lock_guard<std::mutex> lock(mutex());
    return s_disk_loads;
  }
}
