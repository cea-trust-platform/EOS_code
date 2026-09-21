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

#ifndef EOS_IPP_TILER_HXX_
#define EOS_IPP_TILER_HXX_

#include "EOS/API/EOS_Error.hxx"
#include <string>

namespace NEPTUNE_EOS_IGEN
{
  //! Parameters of an offline tiled-database generation run. The global
  //! (p,h) domain [pmin,pmax]x[hmin,hmax] is sliced into a regular
  //! nb_p_tiles x nb_h_tiles grid; each cell becomes one standalone EOS_Ipp
  //! .med file, generated from 'method'/'reference' (any registered
  //! EOS_Fluid, e.g. "EOS_StiffenedGas"/"WaterLiquid" or
  //! "EOS_Refprop9"/"WaterLiquid").
  struct EOS_Ipp_Tiler_Params
  {
    std::string method;
    std::string reference; // must contain "Liquid" or "Vapor" (cf. EOS_IGen::write_index)

    double pmin = 0., pmax = 0., hmin = 0., hmax = 0.;
    int nb_p_tiles = 1;
    int nb_h_tiles = 1;

    int nb_node_p = 4; // per-tile initial mesh resolution, cf. EOS_IGen::make_mesh
    int nb_node_h = 4;
    int level_max = -1;

    // Each tile is generated over a (p,h) box widened by this fraction of
    // the cell's own extent on every side (clamped to the global domain):
    // the halo neighboring tiles need to interpolate consistently right up
    // to a shared boundary (cf. the mesh-manager design notes on bicubic
    // continuity across tile edges). 0 disables the halo.
    double halo_fraction = 0.15;

    std::string tile_basename = "tile"; // tile files: {tile_basename}_{ip}_{ih}.med

    std::string quality_property = "rho";
    std::string quality_type = "centre";
    int quality_is_abs = 1;
    //! How many points per cell the "centre" criterion probes: nb x nb. 1 is
    //! the cell centre alone, which is all it ever looked at.
    int quality_subsampling = 1;
    //! Threshold the refinement is driven against. EOS_IGen's own default
    //! (-9999.9) is a sentinel meaning "no threshold", under which the quality
    //! test always passes and no cell is ever refined -- which made level_max
    //! decorative: every database this tool produced was the unrefined base
    //! mesh. Left at the sentinel for compatibility; set it to ask for actual
    //! refinement.
    double quality_limit = -9999.9;

    //! Whether to accept a domain smaller than the one requested. The database
    //! is built over the T box spanned by the 4 corners of the requested (p,h)
    //! box; a corner outside the model's validity contributes nothing, so the
    //! result can be a small fraction of what was asked for. That is refused by
    //! default, since it is not visible in the output otherwise.
    bool allow_domain_shrink = false;
    //! Whether the refinement inserts continuity nodes on the hanging edges it
    //! creates (EOS_IGen::make_local_refine's argument). True matches the
    //! "raffinement_local" reference databases, false the
    //! "raffinement_local_non_continu" ones.
    bool refine_continuity = true;
    //! Write a manifest even when some tiles could not be generated. The grid
    //! then has holes, which the reader supports (EOS_Ipp_TileIndex leaves the
    //! slot empty and locate() answers -1), and the manifest records which
    //! cells are missing and why. Off by default: an incomplete database
    //! should have to be asked for.
    bool allow_partial = false;

    // Generating a tile is independent of every other tile, so a grid of them
    // is embarrassingly parallel; nb_jobs > 1 forks that many worker processes.
    // Processes rather than threads because EOS_IGen and the MED writer are not
    // known to be thread-safe, and a tile is written to its own file anyway.
    int nb_jobs = 1;

    // Skip tiles whose .med already exists, so an interrupted generation can be
    // resumed instead of restarting from nothing -- on a real model a hundred
    // tiles is a long run to lose to a failure on the ninety-ninth.
    bool skip_existing = false;

    // Report what would be generated (tiles, boxes, estimated size) and stop.
    bool dry_run = false;

    // Silence the source model's own diagnostics during generation. Probing a
    // model at tile corners legitimately hits properties it does not implement,
    // and the resulting stream of "Not_implemented" lines buries the tiler's
    // own messages.
    bool quiet_source = false;

    // Progress line per tile.
    bool verbose = true;
  };

  //! Generates nb_p_tiles x nb_h_tiles independent EOS_Ipp .med files
  //! (written, like any EOS_IGen database, under {eos_data_dir}/EOS_Ipp/),
  //! plus the ".eosmm" manifest EOS_Ipp's tiled ("streaming") mode uses to
  //! find and route between them (cf. EOS_Ipp_TileIndex in
  //! Modules/EOS/Src/EOS_Ipp -- not reused directly here: EOS_IGen must not
  //! depend on EOS_Ipp, cf. the existing EOS_Ipp -> EOS_IGen dependency
  //! direction, so the manifest's small text format is just written out
  //! directly).
  //!
  //! EOS_IGen's mesh generation is parameterized by (p,T), not (p,h)
  //! (cf. EOS_IGen::set_extremum), so each tile's (p,T) generation box is
  //! derived from its declared (p,h) cell by querying 'method'/'reference'
  //! itself for T at the (halo-widened) cell's four corners.
  //!
  //! manifest_file_name is written under the same {eos_data_dir}/EOS_Ipp/
  //! directory as the tiles, so a plain, un-prefixed name (e.g.
  //! "mydb.eosmm") is what EOS_Ipp::init() should later be given.
  //! Returns EOS_Error::good on success.
  NEPTUNE::EOS_Error generate_tiled_database(const EOS_Ipp_Tiler_Params &params,
                                              const std::string &manifest_file_name);
}
#endif /* EOS_IPP_TILER_HXX_ */
