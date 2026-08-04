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
#include "Language/API/Language.hxx"
#include <iostream>
#include <string>
#include <cstdlib>

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
      "                [--nb_p_tiles=N] [--nb_h_tiles=N] [--nb_node_p=N] [--nb_node_h=N]\n"
      "                [--halo_fraction=F] [--tile_basename=NAME] [--manifest=NAME.eosmm]\n\n"
      "Tiles and the manifest are written under $USER_EOS_DATA/EOS_Ipp/ (or the compiled-in\n"
      "eos_data_dir), exactly like a regular EOS_IGen-generated database. Load the result from\n"
      "a host code with: EOS_Ipp ipp; ipp.init(Strings{\"<manifest>.eosmm\"});\n\n"
      "example:\n"
      "  eos_ipp_tiler --method=EOS_StiffenedGas --reference=WaterLiquid \\\n"
      "                --pmin=1e5 --pmax=5e6 --hmin=1e5 --hmax=1e6 \\\n"
      "                --nb_p_tiles=4 --nb_h_tiles=4 \\\n"
      "                --tile_basename=water_tile --manifest=water_tiled.eosmm\n";
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
  prm.halo_fraction = arg_d(argc, argv, "halo_fraction", 0.15);
  prm.tile_basename = arg_s(argc, argv, "tile_basename", "tile");
  const std::string manifest = arg_s(argc, argv, "manifest", "tiled.eosmm");

  if (prm.method.empty() || prm.reference.empty())
  {
    std::cerr << "eos_ipp_tiler: --method and --reference are required (run with --help)" << std::endl;
    return 1;
  }

  std::cout << "eos_ipp_tiler: generating " << prm.nb_p_tiles << "x" << prm.nb_h_tiles
            << " tile(s) from " << prm.method << "/" << prm.reference
            << " over p in [" << prm.pmin << "," << prm.pmax << "], h in ["
            << prm.hmin << "," << prm.hmax << "] ..." << std::endl;

  const EOS_Error err = generate_tiled_database(prm, manifest);
  if (err != EOS_Error::good)
  {
    std::cerr << "eos_ipp_tiler: generation FAILED" << std::endl;
    return 1;
  }

  std::cout << "eos_ipp_tiler: wrote " << (prm.nb_p_tiles * prm.nb_h_tiles)
            << " tile(s) and manifest \"" << manifest << "\" under {EOS_DATA}/EOS_Ipp/" << std::endl;
  return 0;
}
