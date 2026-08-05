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

#include "EOS_Ipp_Tiler.hxx"
#include "EOS_IGen/API/EOS_IGen.hxx"
#include "EOS/API/EOS.hxx"
#include "EOS/API/EOS_Std_Error_Handler.hxx"
#include "EOS/API/EOS_eosdatadir.hxx"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <cmath>

namespace NEPTUNE_EOS_IGEN
{
  namespace
  {
    // T-range spanning the 4 corners of [p0,p1]x[h0,h1] under 'source',
    // widened by a small safety margin (EOS_IGen's own inversions/
    // interpolations near a box's edge are less accurate than at its
    // center). Returns false if none of the 4 corners could be evaluated.
    bool corner_T_range(EOS &source, double p0, double p1, double h0, double h1,
                         double &Tmin, double &Tmax)
    {
      const double ps[2] = {p0, p1};
      const double hs[2] = {h0, h1};
      Tmin = 1e300;
      Tmax = -1e300;
      bool any_ok = false;
      for (double p : ps)
        for (double h : hs)
        {
          double T;
          if (source.compute_T_ph(p, h, T) == EOS_Error::good)
          {
            Tmin = std::min(Tmin, T);
            Tmax = std::max(Tmax, T);
            any_ok = true;
          }
        }
      if (!any_ok)
        return false;
      const double span = Tmax - Tmin;
      const double margin = 0.02 * (span > 0. ? span : std::abs(Tmax));
      Tmin -= margin;
      Tmax += margin;
      return true;
    }

    void relax(EOS &eos)
    {
      EOS_Std_Error_Handler h;
      h.set_exit_on_error(EOS_Std_Error_Handler::disable_feature);
      h.set_throw_on_error(EOS_Std_Error_Handler::disable_feature);
      eos.set_error_handler(h);
    }

    struct WrittenTile
    {
      int ip, ih;
      double p0, p1, h0, h1; // declared (core) cell -- not the halo-widened generation box
      std::string file_name; // "{tile_basename}_{ip}_{ih}.med"
    };
  }

  EOS_Error generate_tiled_database(const EOS_Ipp_Tiler_Params &prm,
                                     const std::string &manifest_file_name)
  {
    if (prm.nb_p_tiles <= 0 || prm.nb_h_tiles <= 0 || !(prm.pmax > prm.pmin) || !(prm.hmax > prm.hmin))
    {
      cerr << "EOS_Ipp_Tiler: invalid parameters (grid size or domain)" << endl;
      return EOS_Error::error;
    }

    EOS source(prm.method.c_str(), prm.reference.c_str());
    relax(source);

    double Tmin_global, Tmax_global;
    if (!corner_T_range(source, prm.pmin, prm.pmax, prm.hmin, prm.hmax, Tmin_global, Tmax_global))
    {
      cerr << "EOS_Ipp_Tiler: could not evaluate T(p,h) at the global domain corners" << endl;
      return EOS_Error::error;
    }
    double pcrit = 0., hcrit = 0., tcrit = 0.;
    source.get_p_crit(pcrit);
    source.get_h_crit(hcrit);
    source.get_T_crit(tcrit);

    const double dp = (prm.pmax - prm.pmin) / prm.nb_p_tiles;
    const double dh = (prm.hmax - prm.hmin) / prm.nb_h_tiles;

    std::vector<WrittenTile> written;
    written.reserve((std::size_t)prm.nb_p_tiles * prm.nb_h_tiles);

    for (int ip = 0; ip < prm.nb_p_tiles; ++ip)
      for (int ih = 0; ih < prm.nb_h_tiles; ++ih)
      {
        const double p0 = prm.pmin + ip * dp;
        const double p1 = prm.pmin + (ip + 1) * dp;
        const double h0 = prm.hmin + ih * dh;
        const double h1 = prm.hmin + (ih + 1) * dh;

        const double p0h = std::max(prm.pmin, p0 - prm.halo_fraction * dp);
        const double p1h = std::min(prm.pmax, p1 + prm.halo_fraction * dp);
        const double h0h = std::max(prm.hmin, h0 - prm.halo_fraction * dh);
        const double h1h = std::min(prm.hmax, h1 + prm.halo_fraction * dh);

        double Tmin, Tmax;
        if (!corner_T_range(source, p0h, p1h, h0h, h1h, Tmin, Tmax))
        {
          cerr << "EOS_Ipp_Tiler: could not evaluate T(p,h) at tile (" << ip << "," << ih << ") corners" << endl;
          return EOS_Error::error;
        }
        EOS_IGen igen(prm.method.c_str(), prm.reference.c_str());
        igen.set_extremum(p0h, p1h, Tmin, Tmax);

        EOS_Error err = igen.make_mesh(prm.nb_node_p, prm.nb_node_h, prm.level_max);
        if (err != EOS_Error::good)
        {
          cerr << "EOS_Ipp_Tiler: make_mesh failed for tile (" << ip << "," << ih << ")" << endl;
          return err;
        }

        igen.set_quality(prm.quality_property.c_str(), prm.quality_type.c_str(), prm.quality_is_abs);
        err = igen.compute_qualities();
        if (err != EOS_Error::good)
        {
          cerr << "EOS_Ipp_Tiler: compute_qualities failed for tile (" << ip << "," << ih << ")" << endl;
          return err;
        }

        std::ostringstream name;
        name << prm.tile_basename << "_" << ip << "_" << ih;
        AString file_name(name.str().c_str());
        igen.set_file_med_name(file_name);
        err = igen.write_med();
        if (err != EOS_Error::good)
        {
          cerr << "EOS_Ipp_Tiler: write_med failed for tile (" << ip << "," << ih << ")" << endl;
          return err;
        }

        WrittenTile w;
        w.ip = ip; w.ih = ih; w.p0 = p0; w.p1 = p1; w.h0 = h0; w.h1 = h1;
        w.file_name = name.str() + ".med";
        written.push_back(w);
      }

    // Manifest, written next to the tiles (not through EOS_Ipp_TileIndex,
    // cf. header comment on the EOS_Ipp -> EOS_IGen dependency direction).
    if (iret_eos_data_dir)
    {
      cerr << "EOS_Ipp_Tiler: eos_data_dir is not set" << endl;
      return EOS_Error::error;
    }
    const std::string manifest_path = eos_data_dir + "/EOS_Ipp/" + manifest_file_name;
    std::ofstream out(manifest_path.c_str());
    if (!out.good())
    {
      cerr << "EOS_Ipp_Tiler: cannot create manifest file " << manifest_path << endl;
      return EOS_Error::error;
    }

    // Every double below is a domain bound the loading side turns straight
    // into EOS_Ipp's pmin/pmax/hmin/hmax (cf. EOS_Ipp::init_tiled), so it must
    // survive the round-trip exactly: the ostream default of 6 significant
    // digits silently moved a tile edge of 916666.667 to 916667, i.e. by ~1e-6
    // relative, which is enough to make a point right on a boundary route to
    // -- or be rejected by -- the wrong side.
    out << std::setprecision(17);

    out << "# Generated by EOS_Ipp_Tiler (" << prm.method << "/" << prm.reference << ")\n";
    out << "GLOBAL " << prm.pmin << " " << prm.pmax << " " << prm.hmin << " " << prm.hmax << " "
        << Tmin_global << " " << Tmax_global << " " << pcrit << " " << hcrit << " " << tcrit << "\n";
    out << "GRID " << prm.nb_p_tiles << " " << prm.nb_h_tiles << "\n";
    for (const WrittenTile &w : written)
      out << "TILE " << w.ip << " " << w.ih << " "
          << w.p0 << " " << w.p1 << " " << w.h0 << " " << w.h1 << " " << w.file_name << "\n";

    if (!out.good())
    {
      cerr << "EOS_Ipp_Tiler: error writing manifest file " << manifest_path << endl;
      return EOS_Error::error;
    }
    return EOS_Error::good;
  }
}
