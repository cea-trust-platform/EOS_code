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

#include "EOS_Ipp_TileIndex.hxx"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <cstdlib>

namespace NEPTUNE_EOS
{
  bool EOS_Ipp_TileIndex::is_manifest(const std::string &file_name)
  {
    const std::string ext = ".eosmm";
    if (file_name.size() < ext.size())
      return false;
    return file_name.compare(file_name.size() - ext.size(), ext.size(), ext) == 0;
  }

  bool EOS_Ipp_TileIndex::load(const std::string &manifest_path)
  {
    std::ifstream in(manifest_path.c_str());
    if (!in.good())
      return false;

    // Tile med_file paths in the manifest are resolved relative to the
    // directory holding the manifest itself, so a tiled database (manifest +
    // per-tile .med files) can be moved/copied as a single directory.
    std::string base_dir;
    const std::size_t slash = manifest_path.find_last_of('/');
    if (slash != std::string::npos)
      base_dir = manifest_path.substr(0, slash + 1);

    bool has_global = false, has_grid = false;
    std::string line;
    while (std::getline(in, line))
    {
      if (line.empty() || line[0] == '#')
        continue;

      std::istringstream ls(line);
      std::string keyword;
      ls >> keyword;

      if (keyword == "GLOBAL")
      {
        ls >> domain_.pmin >> domain_.pmax >> domain_.hmin >> domain_.hmax
            >> tmin_ >> tmax_ >> pcrit_ >> hcrit_ >> tcrit_;
        has_global = true;
      }
      else if (keyword == "GRID")
      {
        ls >> nb_p_ >> nb_h_;
        if (nb_p_ <= 0 || nb_h_ <= 0)
          return false;
        has_grid = true;
      }
      else if (keyword == "TILE")
      {
        if (!has_global || !has_grid)
          return false; // GLOBAL/GRID must appear before any TILE line

        EOS_Ipp_TileDescriptor desc;
        ls >> desc.ip >> desc.ih >> desc.bbox.pmin >> desc.bbox.pmax
            >> desc.bbox.hmin >> desc.bbox.hmax;
        if (!ls)
          return false;

        // Two accepted tails: "<tmin> <tmax> <file>" (current) and just
        // "<file>" (manifests written before the per-tile T-range existed).
        // Collecting the remaining tokens keeps both readable without
        // having to version the line itself.
        std::vector<std::string> tail;
        std::string token;
        while (ls >> token)
          tail.push_back(token);

        std::string rel_path;
        if (tail.size() == 1)
        {
          rel_path = tail[0]; // no T-range recorded: leave it wide open
        }
        else if (tail.size() == 3)
        {
          desc.tmin = atof(tail[0].c_str());
          desc.tmax = atof(tail[1].c_str());
          rel_path = tail[2];
          if (!(desc.tmax >= desc.tmin))
            return false;
        }
        else
        {
          return false;
        }
        if (rel_path.empty())
          return false;

        desc.med_file = (!rel_path.empty() && rel_path[0] == '/') ? rel_path : base_dir + rel_path;
        tiles_.push_back(desc);
      }
      // unknown keywords are ignored, for forward-compatibility
    }

    if (!has_global || !has_grid || tiles_.empty())
      return false;

    delta_p_ = (domain_.pmax - domain_.pmin) / nb_p_;
    delta_h_ = (domain_.hmax - domain_.hmin) / nb_h_;
    if (!(delta_p_ > 0.) || !(delta_h_ > 0.))
      return false;

    grid_.assign((std::size_t)nb_p_ * (std::size_t)nb_h_, -1);
    for (std::size_t k = 0; k < tiles_.size(); ++k)
    {
      const EOS_Ipp_TileDescriptor &d = tiles_[k];
      if (d.ip < 0 || d.ip >= nb_p_ || d.ih < 0 || d.ih >= nb_h_)
        return false;
      grid_[(std::size_t)d.ip * nb_h_ + d.ih] = (int)k;
      tiles_[k].id = (int)k;
    }
    return true;
  }

  int EOS_Ipp_TileIndex::locate(double p, double h) const
  {
    if (!domain_.contains(p, h))
      return -1;

    int ip = (int)std::floor((p - domain_.pmin) / delta_p_);
    int ih = (int)std::floor((h - domain_.hmin) / delta_h_);
    ip = std::min(std::max(ip, 0), nb_p_ - 1); // p==pmax (or fp round-off) falls back on the last column/row
    ih = std::min(std::max(ih, 0), nb_h_ - 1);

    return grid_[(std::size_t)ip * nb_h_ + ih];
  }

  int EOS_Ipp_TileIndex::locate_column(double p) const
  {
    if (p < domain_.pmin || p > domain_.pmax)
      return -1;

    int ip = (int)std::floor((p - domain_.pmin) / delta_p_);
    ip = std::min(std::max(ip, 0), nb_p_ - 1);

    for (int ih = 0; ih < nb_h_; ++ih)
    {
      const int tile_id = grid_[(std::size_t)ip * nb_h_ + ih];
      if (tile_id != -1)
        return tile_id;
    }
    return -1;
  }

  void EOS_Ipp_TileIndex::tiles_in_column(double p, std::vector<int> &result) const
  {
    result.clear();
    if (p < domain_.pmin || p > domain_.pmax)
      return;

    int ip = (int)std::floor((p - domain_.pmin) / delta_p_);
    ip = std::min(std::max(ip, 0), nb_p_ - 1);

    for (int ih = 0; ih < nb_h_; ++ih)
    {
      const int tile_id = grid_[(std::size_t)ip * nb_h_ + ih];
      if (tile_id != -1)
        result.push_back(tile_id);
    }
  }

  void EOS_Ipp_TileIndex::tiles_in_column_for_T(double p, double T, std::vector<int> &result) const
  {
    result.clear();
    if (p < domain_.pmin || p > domain_.pmax)
      return;

    int ip = (int)std::floor((p - domain_.pmin) / delta_p_);
    ip = std::min(std::max(ip, 0), nb_p_ - 1);

    // A tile's mesh was generated over a known (p,T) box, so it cannot hold a
    // root of T(p,h) = T for a T outside that box: filtering here saves the
    // caller from loading such tiles only to be told there is no root inside.
    for (int ih = 0; ih < nb_h_; ++ih)
    {
      const int tile_id = grid_[(std::size_t)ip * nb_h_ + ih];
      if (tile_id != -1 && tiles_[tile_id].contains_T(T))
        result.push_back(tile_id);
    }
  }
}
