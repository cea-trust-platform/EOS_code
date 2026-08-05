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

#ifndef EOS_IPP_TILEINDEX_HXX_
#define EOS_IPP_TILEINDEX_HXX_

#include "EOS_Ipp_PH_BBox.hxx"
#include <string>
#include <vector>

namespace NEPTUNE_EOS
{
  //! One entry of a tiled EOS_Ipp database: a regular (p,h) tile backed by
  //! its own, independently loadable, EOS_Ipp .med file (same file format
  //! and same EOS_Ipp::init() code path as the historical, single-file
  //! database -- tiling only changes how many such files exist and which
  //! subset of the (p,h) domain each one covers).
  struct EOS_Ipp_TileDescriptor
  {
    int id = -1; // index into EOS_Ipp_TileIndex::tiles_ / EOS_Ipp_TileCache lookup key
    int ip = -1;
    int ih = -1;
    EOS_Ipp_PH_BBox bbox;

    // Temperature range the tile's mesh was actually generated over (the
    // (p,T) box handed to EOS_IGen::set_extremum, i.e. the halo-widened
    // cell's corner T-range plus its safety margin). A tile cannot invert
    // T(p,h) = T for a T outside this range, so EOS_Ipp_TileCache uses it to
    // skip -- without loading them -- the tiles of a p-column that provably
    // cannot hold the root. Left wide open (-inf, +inf) by manifests written
    // before the range was recorded, which simply disables the skipping.
    double tmin = -1e300;
    double tmax = 1e300;
    bool contains_T(double T) const { return T >= tmin && T <= tmax; }

    std::string med_file; // path of the tile's standalone .med file
  };

  //! Read-only description of a tiled EOS_Ipp database: the global (p,h,T)
  //! domain, the regular tiling grid over (p,h), and, for each existing
  //! tile, the path to its standalone .med file.
  //!
  //! A manifest is a small text file (a few KB, one line per tile), not the
  //! multi-hundred-MB payload of the tiles themselves -- reading it costs
  //! nothing compared to the historical single-shot full database load it
  //! replaces. It is parsed once by EOS_Ipp::init() and never mutated
  //! afterwards, so it is safe to read concurrently from several EOS_Ipp
  //! instances (e.g. one per OpenMP domain, cf. EOS_Ipp_TileCache) without
  //! any locking.
  //!
  //! Manifest text format (writer: EOS_IGen/Src/EOS_Ipp_Tiler.cxx):
  //!   GLOBAL pmin pmax hmin hmax tmin tmax pcrit hcrit tcrit
  //!   GRID   nb_p nb_h
  //!   TILE   ip ih pmin pmax hmin hmax tmin tmax med_file
  //!   ...
  //! Unknown keywords are ignored, and a TILE line may also carry the older
  //! 7-field form without its (tmin,tmax), so a manifest written by an older
  //! tiler still loads.
  class EOS_Ipp_TileIndex
  {
  public:
    //! True if file_name should be interpreted as a tiled-database manifest
    //! (".eosmm" extension) rather than a plain, single .med file -- the
    //! detection EOS_Ipp::init() uses to pick between the historical
    //! (whole-database, eager) and tiled (lazy, per-tile) loading paths.
    static bool is_manifest(const std::string &file_name);

    //! Parses a manifest file. Returns false on I/O or format error.
    bool load(const std::string &manifest_path);

    double pmin() const { return domain_.pmin; }
    double pmax() const { return domain_.pmax; }
    double hmin() const { return domain_.hmin; }
    double hmax() const { return domain_.hmax; }
    double tmin() const { return tmin_; }
    double tmax() const { return tmax_; }
    double pcrit() const { return pcrit_; }
    double hcrit() const { return hcrit_; }
    double tcrit() const { return tcrit_; }

    int nb_tiles() const { return (int)tiles_.size(); }
    const EOS_Ipp_TileDescriptor &tile(int tile_id) const { return tiles_[tile_id]; }

    //! O(1) lookup (same direct-indexing principle as EOS_Ipp's own
    //! fnodes2phnodes virtual grid, one level up): returns the id of the
    //! tile covering (p,h), or -1 if (p,h) falls outside the tiled domain,
    //! or lies in a grid cell for which no tile was generated.
    int locate(double p, double h) const;

    //! Saturation/limit curves are 1D in p, not part of the 2D (p,h) tiling:
    //! each tile is generated (cf. EOS_IGen::make_mesh_ph) with its own copy
    //! of the sat/lim curve restricted to its p-column, so any resident tile
    //! in that column carries equivalent data. Returns the id of the first
    //! tile found in the p-column containing p (scanning ih from 0), or -1
    //! if p is outside the domain or the column has no tile at all.
    //! NB: this duplicates sat/lim data across every tile of a column; cf.
    //! the mesh-manager design notes for the follow-up (a single
    //! always-resident sat/lim tile, generated once per database rather
    //! than once per column).
    int locate_column(double p) const;

    //! All tile ids covering the p-column containing p, in ascending ih
    //! order (skipping empty grid cells). Used by EOS_Ipp_TileCache's
    //! h(p,T) inversion, which -- exactly like EOS_Ipp::compute_h_pT on a
    //! single, untiled database -- must be able to scan the *entire*
    //! h-extent of the column, not just the one tile a given (p,h) pair
    //! would resolve to via locate(). Cleared and refilled, so the caller
    //! can keep one vector across calls. Empty if p is outside the domain.
    void tiles_in_column(double p, std::vector<int> &result) const;

    //! Same, restricted to the tiles whose recorded generation T-range can
    //! contain T (cf. EOS_Ipp_TileDescriptor::tmin/tmax). This is what makes
    //! the h(p,T) inversion affordable on a tiled database: without it a
    //! query whose root is high in the column -- or which has no root at all
    //! -- loads every tile of that column before giving up. Manifests with no
    //! recorded T-range keep every tile, i.e. the historical full scan.
    void tiles_in_column_for_T(double p, double T, std::vector<int> &result) const;

  private:
    EOS_Ipp_PH_BBox domain_;
    double tmin_ = 0., tmax_ = 0.;
    double pcrit_ = 0., hcrit_ = 0., tcrit_ = 0.;
    int nb_p_ = 0, nb_h_ = 0;
    double delta_p_ = 0., delta_h_ = 0.;
    std::vector<EOS_Ipp_TileDescriptor> tiles_;
    std::vector<int> grid_; // size nb_p_*nb_h_: grid_[ip*nb_h_+ih] -> index into tiles_, or -1
  };
}
#endif /* EOS_IPP_TILEINDEX_HXX_ */
