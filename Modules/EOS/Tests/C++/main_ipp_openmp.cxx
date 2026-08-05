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

// Concurrency regression test for EOS_Ipp, single-file *and* tiled.
//
// The tiled mode is designed around "one EOS instance per OpenMP-split
// domain": that is what lets EOS_Ipp_TileCache keep no lock on its compute
// path (cf. its header). Nothing used to exercise that model -- the other Ipp
// tests set OMP_NUM_THREADS but are single-threaded -- and it was in fact
// broken for both modes: the scalar compute path allocated Language
// NumberedObjects per point, and the global OBJECTSHANDLING::Objects registry
// they register into reallocates its arrays while other threads index them,
// so two threads were enough to abort with "double free or corruption".
//
// This test therefore checks two things at once, and both matter:
//   - it does not crash, hang or corrupt the heap under real concurrency;
//   - every parallel result is bit-identical to the serial one, so no thread
//     ever read another's scratch.
// Both the historical single-file path and the tiled path are covered, since
// the failure was in code shared by the two.

#include "EOS/API/EOS.hxx"
#include "EOS/API/EOS_Std_Error_Handler.hxx"
#include "EOS/API/EOS_Config.hxx"
#include "EOS_IGen/Src/EOS_Ipp_Tiler.hxx"
#include <iostream>
#include <vector>
#include <cmath>
#ifdef _OPENMP
#include <omp.h>
#endif

using namespace NEPTUNE;
using namespace NEPTUNE_EOS_IGEN;

namespace
{
  int g_failures = 0;

  void relax(EOS &eos)
  {
    EOS_Std_Error_Handler h;
    h.set_exit_on_error(EOS_Std_Error_Handler::disable_feature);
    h.set_throw_on_error(EOS_Std_Error_Handler::disable_feature);
    eos.set_error_handler(h);
  }

  const double pmin = 1.0e5, pmax = 5.0e6;
  const double hmin = 1.0e5, hmax = 1.0e6;
  const int NB_POINTS = 20000;

  // Deterministic, reproducible spread over the domain (no <random>, so the
  // point set does not depend on the library implementation).
  void make_points(std::vector<double> &p, std::vector<double> &h)
  {
    p.resize(NB_POINTS);
    h.resize(NB_POINTS);
    unsigned int s = 12345u;
    for (int i = 0; i < NB_POINTS; i++)
    {
      s = s * 1103515245u + 12345u;
      const double up = ((s >> 16) & 0x7fff) / 32767.;
      s = s * 1103515245u + 12345u;
      const double uh = ((s >> 16) & 0x7fff) / 32767.;
      p[i] = pmin + (0.02 + 0.96 * up) * (pmax - pmin);
      h[i] = hmin + (0.02 + 0.96 * uh) * (hmax - hmin);
    }
  }

  // Serial reference, then the same points again with one EOS instance per
  // thread -- the parallelization model the tiled mode documents.
  void check_parallel_matches_serial(const char *label, const std::string &database)
  {
    std::vector<double> p, h;
    make_points(p, h);

    std::vector<double> ref(NB_POINTS, 0.);
    std::vector<int> ref_err(NB_POINTS, 0);
    {
      Strings args(1);
      args[0] = database.c_str();
      EOS serial("EOS_Ipp", args);
      relax(serial);
      for (int i = 0; i < NB_POINTS; i++)
        ref_err[i] = (int)serial.compute_rho_ph(p[i], h[i], ref[i]);
    }

    int nb_threads = 1;
#ifdef _OPENMP
    nb_threads = omp_get_max_threads();
    if (nb_threads < 2)
      nb_threads = 2; // the whole point is to have concurrency
#endif

    std::vector<EOS *> instances(nb_threads, nullptr);
    for (int t = 0; t < nb_threads; t++)
    {
      Strings args(1);
      args[0] = database.c_str();
      instances[t] = new EOS("EOS_Ipp", args);
      relax(*instances[t]);
    }

    std::vector<double> par(NB_POINTS, 0.);
    std::vector<int> par_err(NB_POINTS, 0);

#pragma omp parallel num_threads(nb_threads)
    {
      int id = 0;
#ifdef _OPENMP
      id = omp_get_thread_num();
#endif
      EOS &mine = *instances[id];
      const int lo = (int)((long)NB_POINTS * id / nb_threads);
      const int hi = (int)((long)NB_POINTS * (id + 1) / nb_threads);
      for (int i = lo; i < hi; i++)
        par_err[i] = (int)mine.compute_rho_ph(p[i], h[i], par[i]);
    }

    int nb_diff = 0, nb_good = 0;
    for (int i = 0; i < NB_POINTS; i++)
    {
      if (ref_err[i] != par_err[i] || ref[i] != par[i])
      {
        if (nb_diff < 5)
          std::cerr << "FAILED " << label << ": point " << i << " (p=" << p[i] << ", h=" << h[i]
                    << ") serial=" << ref[i] << " (err " << ref_err[i] << ")"
                    << " parallel=" << par[i] << " (err " << par_err[i] << ")" << std::endl;
        nb_diff++;
      }
      if (ref_err[i] == (int)EOS_Error::good)
        nb_good++;
    }

    if (nb_diff > 0)
    {
      std::cerr << "FAILED " << label << ": " << nb_diff << "/" << NB_POINTS
                << " points differ between the serial and the " << nb_threads << "-thread run" << std::endl;
      g_failures++;
    }
    else if (nb_good == 0)
    {
      std::cerr << "FAILED " << label << ": not a single point computed successfully,"
                << " the test would pass on an empty domain" << std::endl;
      g_failures++;
    }
    else
    {
      std::cout << "OK " << label << ": " << NB_POINTS << " points, " << nb_good
                << " in domain, identical on " << nb_threads << " threads" << std::endl;
    }

    for (EOS *e : instances)
      delete e;
  }
}

int main()
{
  Language_init();

#ifndef _OPENMP
  std::cout << "EOSTestIppOpenMP: built without OpenMP, running the checks serially" << std::endl;
#endif

  const std::string method = "EOS_StiffenedGas";
  const std::string reference = "WaterLiquid";

  // Same fixtures as main_ipp_tiled.cxx: a single-tile database loaded through
  // the historical whole-file path, and a genuinely tiled one.
  EOS_Ipp_Tiler_Params mono;
  mono.method = method;
  mono.reference = reference;
  mono.pmin = pmin; mono.pmax = pmax;
  mono.hmin = hmin; mono.hmax = hmax;
  mono.nb_p_tiles = 1;
  mono.nb_h_tiles = 1;
  mono.nb_node_p = 6;
  mono.nb_node_h = 6;
  mono.halo_fraction = 0.;
  mono.tile_basename = "eos_ipp_omp_test_mono";
  if (generate_tiled_database(mono, "eos_ipp_omp_test_mono.eosmm") != EOS_Error::good)
  {
    std::cerr << "FAILED: could not generate the single-file reference database" << std::endl;
    return 1;
  }

  EOS_Ipp_Tiler_Params tiled = mono;
  tiled.nb_p_tiles = 3;
  tiled.nb_h_tiles = 3;
  tiled.halo_fraction = 0.15;
  tiled.tile_basename = "eos_ipp_omp_test_tile";
  if (generate_tiled_database(tiled, "eos_ipp_omp_test.eosmm") != EOS_Error::good)
  {
    std::cerr << "FAILED: could not generate the tiled database" << std::endl;
    return 1;
  }

  // Single-file path: shares the whole scalar compute path with the tiled one,
  // and was just as broken under threads, so it is covered here too.
  check_parallel_matches_serial("single .med", "eos_ipp_omp_test_mono_0_0.med");

  // Tiled path: additionally exercises concurrent lazy tile loading (several
  // threads racing to load the same tile file, cf. EOS_Ipp_TileCache).
  check_parallel_matches_serial("tiled 3x3", "eos_ipp_omp_test.eosmm");

  if (g_failures > 0)
  {
    std::cerr << "EOSTestIppOpenMP: " << g_failures << " failure(s)" << std::endl;
    return 1;
  }
  std::cout << "EOSTestIppOpenMP: all checks passed" << std::endl;
  return 0;
}
