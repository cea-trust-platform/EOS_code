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

// Offline CLI tool: slices [pmin,pmax]x[hmin,hmax] into a regular grid of
// independent EOS_Ipp .med tiles (generated through EOS_IGen from a real
// source EOS_Fluid model) plus the ".eosmm" manifest that EOS_Ipp's tiled
// ("streaming") mode needs to find and route between them. Core generation
// logic lives in EOS_Ipp_Tiler.hxx/.cxx, shared with the permanent
// regression test in Modules/EOS/Tests/C++/main_ipp_tiled.cxx.

#include "EOS_IGen/Src/EOS_Ipp_Tiler.hxx"
#include "EOS/Src/EOS_Ipp/EOS_Ipp_TileIndex.hxx"
#include "Language/API/Language.hxx"
#include <iostream>
#include <string>
using std::string;
#include "EOS/API/EOS_eosdatadir.hxx"
#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>

using namespace NEPTUNE_EOS_IGEN;
using namespace NEPTUNE;

namespace
{
  bool has_flag(int argc, char **argv, const char *key)
  {
    const std::string k = std::string("--") + key;
    for (int i = 1; i < argc; ++i)
      if (k == argv[i])
        return true;
    return false;
  }

  std::string arg_s(int argc, char **argv, const char *key, const std::string &def)
  {
    const std::string k = std::string("--") + key + "=";
    for (int i = 1; i < argc; ++i)
    {
      const std::string a = argv[i];
      if (a.compare(0, k.size(), k) == 0)
        return a.substr(k.size());
    }
    return def;
  }

  double arg_d(int argc, char **argv, const char *key, double def)
  {
    const std::string v = arg_s(argc, argv, key, "");
    return v.empty() ? def : std::atof(v.c_str());
  }

  int arg_i(int argc, char **argv, const char *key, int def)
  {
    const std::string v = arg_s(argc, argv, key, "");
    return v.empty() ? def : std::atoi(v.c_str());
  }

  void print_usage()
  {
    std::cout <<
      "eos_ipp_tiler: offline generator of a tiled EOS_Ipp database\n\n"
      "usage:\n"
      "  eos_ipp_tiler --method=<M> --reference=<R> --pmin=<p> --pmax=<p> --hmin=<h> --hmax=<h>\n"
      "                [options]\n"
      "  eos_ipp_tiler --info=<manifest>.eosmm\n\n"
      "tiling:\n"
      "  --nb_p_tiles=N --nb_h_tiles=N   grid of tiles over the (p,h) domain (default 1x1)\n"
      "  --halo_fraction=F               per-tile overlap, as a fraction of a cell (default 0.15)\n"
      "  --tile_basename=NAME            tiles are NAME_<ip>_<ih>.med (default \"tile\")\n"
      "  --manifest=NAME.eosmm           manifest file name (default \"tiled.eosmm\")\n\n"
      "mesh of each tile:\n"
      "  --nb_node_p=N --nb_node_h=N     initial mesh resolution (default 4x4)\n"
      "  --level_max=N                   max adaptive refinement level (default -1, EOS_IGen's own)\n"
      "  --quality_property=NAME         property the refinement is driven by (default \"rho\")\n"
      "  --quality_type=TYPE             quality criterion (default \"centre\")\n"
      "  --quality_is_abs=0|1            absolute rather than relative criterion (default 1)\n"
      "  --quality_limit=X               refine until the criterion is below X. EOS_IGen's\n"
      "                                  default is a sentinel under which the test always\n"
      "                                  passes, so without this no cell is ever refined and\n"
      "                                  --level_max has no effect\n"
      "  --refine_continuity=0|1         insert continuity nodes on the hanging edges the\n"
      "                                  refinement creates (default 1)\n\n"
      "running:\n"
      "  --jobs=N                        generate N tiles at a time (default 1)\n"
      "  --allow_domain_shrink           accept a domain smaller than the one requested. Corners\n"
      "                                  of the (p,h) box outside the model's validity contribute\n"
      "                                  nothing to the T range the database is built over, so the\n"
      "                                  result can be a fraction of what was asked for; refused\n"
      "                                  by default\n"
      "  --skip_existing                 leave already generated tiles alone, to resume a run\n"
      "  --allow_partial                 write a manifest even if some tiles failed; the grid\n"
      "                                  then has holes, recorded as MISSING lines\n"
      "  --dry_run                       list what would be generated, write nothing\n"
      "  --quiet                         hide the source model's own diagnostics\n"
      "  --silent                        no per-tile progress\n\n"
      "Tiles and the manifest are written under $USER_EOS_DATA/EOS_Ipp/ (or the compiled-in\n"
      "eos_data_dir), exactly like a regular EOS_IGen-generated database. Load the result from\n"
      "a host code with: EOS_Ipp ipp; ipp.init(Strings{\"<manifest>.eosmm\"});\n"
      "A resident cache budget and a property subset can be requested at load time, e.g.\n"
      "  ipp.init(Strings{\"water_tiled.eosmm:bicubic:cache=512MB\"});\n\n"
      "example:\n"
      "  eos_ipp_tiler --method=EOS_StiffenedGas --reference=WaterLiquid \\\n"
      "                --pmin=1e5 --pmax=5e6 --hmin=1e5 --hmax=1e6 \\\n"
      "                --nb_p_tiles=4 --nb_h_tiles=4 --jobs=4 --quiet \\\n"
      "                --tile_basename=water_tile --manifest=water_tiled.eosmm\n";
  }

  // --info: describe an existing tiled database without opening its tiles.
  int print_info(const std::string &manifest)
  {
    std::string path = manifest;
    if (manifest.find('/') == std::string::npos)
    {
      if (iret_eos_data_dir)
      {
        std::cerr << "eos_ipp_tiler: eos_data_dir is not set, give --info a path" << std::endl;
        return 1;
      }
      path = eos_data_dir + "/EOS_Ipp/" + manifest;
    }

    NEPTUNE_EOS::EOS_Ipp_TileIndex index;
    if (!index.load(path))
      return 1; // load() already said what is wrong, and where

    std::cout << path << "\n";
    std::cout << "  format version   : " << index.format_version() << "\n";
    if (!index.source_method().empty())
      std::cout << "  generated from   : " << index.source_method() << " / "
                << index.source_reference() << "\n";
    else
      std::cout << "  generated from   : (not recorded -- manifest predates SOURCE)\n";
    if (index.nb_node_p() > 0)
      std::cout << "  per-tile mesh    : " << index.nb_node_p() << "x" << index.nb_node_h()
                << ", level_max " << index.level_max()
                << ", halo " << index.halo_fraction() << "\n";
    if (!index.quality_property().empty())
      std::cout << "  refined on       : " << index.quality_property() << " ("
                << index.quality_type() << (index.quality_is_abs() ? ", absolute" : ", relative")
                << ")\n";
    std::cout << "  tiling           : " << index.nb_p_tiles() << " x " << index.nb_h_tiles()
              << " grid, " << index.nb_tiles() << " tile(s) present\n";
    std::cout << "  p range          : [" << index.pmin() << ", " << index.pmax() << "]\n";
    std::cout << "  h range          : [" << index.hmin() << ", " << index.hmax() << "]\n";
    std::cout << "  T range          : [" << index.tmin() << ", " << index.tmax() << "]\n";
    std::cout << "  critical point   : p=" << index.pcrit() << " h=" << index.hcrit()
              << " T=" << index.tcrit() << "\n";

    // What the database costs on disk, and whether it is all there.
    long long total = 0;
    int missing = 0;
    for (int i = 0; i < index.nb_tiles(); i++)
    {
      struct stat st;
      if (stat(index.tile(i).med_file.c_str(), &st) == 0)
        total += (long long)st.st_size;
      else
        missing++;
    }
    std::cout << "  on disk          : " << (total / (1024 * 1024)) << " MB";
    if (missing > 0)
      std::cout << "   (" << missing << " tile file(s) MISSING)";
    std::cout << std::endl;
    return (missing > 0) ? 1 : 0;
  }
}

int main(int argc, char **argv)
{
  if (argc == 1 || has_flag(argc, argv, "help"))
  {
    print_usage();
    return has_flag(argc, argv, "help") ? 0 : 1;
  }

  Language_init();

  const std::string info = arg_s(argc, argv, "info", "");
  if (!info.empty())
    return print_info(info);

  EOS_Ipp_Tiler_Params prm;
  prm.method    = arg_s(argc, argv, "method", "");
  prm.reference = arg_s(argc, argv, "reference", "");
  prm.pmin = arg_d(argc, argv, "pmin", 0.);
  prm.pmax = arg_d(argc, argv, "pmax", 0.);
  prm.hmin = arg_d(argc, argv, "hmin", 0.);
  prm.hmax = arg_d(argc, argv, "hmax", 0.);
  prm.nb_p_tiles = arg_i(argc, argv, "nb_p_tiles", 1);
  prm.nb_h_tiles = arg_i(argc, argv, "nb_h_tiles", 1);
  prm.nb_node_p  = arg_i(argc, argv, "nb_node_p", 4);
  prm.nb_node_h  = arg_i(argc, argv, "nb_node_h", 4);
  prm.level_max  = arg_i(argc, argv, "level_max", -1);
  prm.halo_fraction = arg_d(argc, argv, "halo_fraction", 0.15);
  prm.tile_basename = arg_s(argc, argv, "tile_basename", "tile");
  prm.quality_property = arg_s(argc, argv, "quality_property", "rho");
  prm.quality_type     = arg_s(argc, argv, "quality_type", "centre");
  prm.quality_is_abs   = arg_i(argc, argv, "quality_is_abs", 1);
  prm.quality_limit    = arg_d(argc, argv, "quality_limit", -9999.9);
  prm.refine_continuity = arg_i(argc, argv, "refine_continuity", 1) != 0;
  prm.allow_partial     = has_flag(argc, argv, "allow_partial");
  prm.allow_domain_shrink = has_flag(argc, argv, "allow_domain_shrink");
  prm.nb_jobs       = arg_i(argc, argv, "jobs", 1);
  prm.skip_existing = has_flag(argc, argv, "skip_existing");
  prm.dry_run       = has_flag(argc, argv, "dry_run");
  prm.quiet_source  = has_flag(argc, argv, "quiet");
  prm.verbose       = !has_flag(argc, argv, "silent");
  const std::string manifest = arg_s(argc, argv, "manifest", "tiled.eosmm");

  if (prm.method.empty() || prm.reference.empty())
  {
    std::cerr << "eos_ipp_tiler: --method and --reference are required (run with --help)" << std::endl;
    return 1;
  }
  if (!(prm.pmax > prm.pmin) || !(prm.hmax > prm.hmin))
  {
    std::cerr << "eos_ipp_tiler: need pmin < pmax and hmin < hmax (got p in ["
              << prm.pmin << "," << prm.pmax << "], h in [" << prm.hmin << "," << prm.hmax << "])"
              << std::endl;
    return 1;
  }
  if (prm.nb_p_tiles <= 0 || prm.nb_h_tiles <= 0)
  {
    std::cerr << "eos_ipp_tiler: --nb_p_tiles and --nb_h_tiles must be strictly positive" << std::endl;
    return 1;
  }
  if (manifest.size() < 6 || manifest.compare(manifest.size() - 6, 6, ".eosmm") != 0)
  {
    std::cerr << "eos_ipp_tiler: --manifest must end in \".eosmm\" -- that extension is what makes"
              << " EOS_Ipp::init() take the tiled path (got \"" << manifest << "\")" << std::endl;
    return 1;
  }

  if (!prm.dry_run)
    std::cout << "eos_ipp_tiler: generating " << prm.nb_p_tiles << "x" << prm.nb_h_tiles
              << " tile(s) from " << prm.method << "/" << prm.reference
              << " over p in [" << prm.pmin << "," << prm.pmax << "], h in ["
              << prm.hmin << "," << prm.hmax << "]"
              << (prm.nb_jobs > 1 ? " (" + std::to_string(prm.nb_jobs) + " jobs)" : "")
              << " ..." << std::endl;

  const EOS_Error err = generate_tiled_database(prm, manifest);
  if (err != EOS_Error::good)
  {
    std::cerr << "eos_ipp_tiler: generation FAILED" << std::endl;
    return 1;
  }

  if (!prm.dry_run)
    std::cout << "eos_ipp_tiler: wrote " << (prm.nb_p_tiles * prm.nb_h_tiles)
              << " tile(s) and manifest \"" << manifest << "\" under {EOS_DATA}/EOS_Ipp/" << std::endl;
  return 0;
}
