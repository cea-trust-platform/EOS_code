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

// Regression test for EOS::init_status() / EOS::is_initialized().
//
// The constructors of NEPTUNE::EOS do not fail: when the fluid model cannot
// be initialised (interpolator manifest or table not found, unreadable
// file, ...) the object is built all the same and only a message on cerr
// says so. The EOS_Error code returned by EOS_Fluid::init is kept in the
// object and reported by init_status(), and is_initialized() tells whether
// it is good or ok. This checks that:
//   - a model with no table to load (EOS_StiffenedGas) reports success ;
//   - EOS_Ipp on a missing .eosmm manifest, and on a missing .med table,
//     reports failure through every constructor form that initialises ;
//   - EOS_Ipp on a valid tiled database, generated at run time through
//     EOS_Ipp_Tiler as main_ipp_tiled.cxx does, reports success ;
//   - init() on an object built from the class name alone replaces the
//     status with its own result, so a failed load followed by a good one
//     ends up initialised, and init() returns the code it stored.
// Self-contained: no dependency on another test's output.

#include "EOS/API/EOS.hxx"
#include "EOS/API/EOS_eosdatadir.hxx"
#include "EOS_IGen/Src/EOS_Ipp_Tiler.hxx"
#include <iostream>
#include <string>

using namespace NEPTUNE;
using namespace NEPTUNE_EOS_IGEN;

namespace
{
  int g_failures = 0;

  // is_initialized() must agree with init_status(), and both with what the
  // scenario expects.
  void check_status(const char *label, const EOS &eos, bool expected)
  {
    const EOS_Error code = eos.init_status();
    const bool initialized = eos.is_initialized();
    const bool coherent = initialized == (code == EOS_Error::good || code == EOS_Error::ok);
    if (initialized != expected || !coherent)
    {
      std::cerr << "FAILED " << label << ": is_initialized=" << initialized
                << " init_status=" << code << ", expected "
                << (expected ? "initialized" : "not initialized") << std::endl;
      ++g_failures;
    }
    else
      std::cout << "OK " << label << ": is_initialized=" << initialized
                << " init_status=" << code << std::endl;
  }

  void check_returned(const char *label, int iret, const EOS &eos)
  {
    if (iret != eos.init_status())
    {
      std::cerr << "FAILED " << label << ": init() returned " << iret
                << " but init_status() is " << eos.init_status() << std::endl;
      ++g_failures;
    }
  }
}

int main()
{
  Language_init();

  const std::string missing_manifest = "./eos_init_status_missing.eosmm";
  const std::string missing_table = "./eos_init_status_missing.med";

  // ---- 1. a model with no table to load
  {
    EOS water("EOS_StiffenedGas", "WaterLiquid");
    check_status("EOS(EOS_StiffenedGas, WaterLiquid)", water, true);
  }

  // ---- 2. EOS_Ipp on files that do not exist, through each constructor
  //         form that initialises the model
  {
    EOS by_path("EOS_Ipp", missing_manifest.c_str());
    check_status("EOS(EOS_Ipp, missing manifest path)", by_path, false);

    EOS table_by_path("EOS_Ipp", missing_table.c_str());
    check_status("EOS(EOS_Ipp, missing table path)", table_by_path, false);

    EOS liquid_by_path("EOS_Ipp_liquid", missing_manifest.c_str());
    check_status("EOS(EOS_Ipp_liquid, missing manifest path)", liquid_by_path, false);

    Strings args(1);
    args[0] = missing_manifest.c_str();
    EOS by_args("EOS_Ipp", args);
    check_status("EOS(EOS_Ipp, Strings)", by_args, false);

    Strings props(1);
    props[0] = "rho";
    EOS with_props("EOS_Ipp", missing_manifest.c_str(), props);
    check_status("EOS(EOS_Ipp, missing manifest path, properties)", with_props, false);
  }

  // ---- 3. a valid tiled database, generated here from a fast,
  //         license-free analytic model
  EOS_Ipp_Tiler_Params prm;
  prm.method = "EOS_StiffenedGas";
  prm.reference = "WaterLiquid";
  prm.pmin = 1.0e5; prm.pmax = 5.0e6;
  prm.hmin = 1.0e5; prm.hmax = 1.0e6;
  prm.nb_p_tiles = 2;
  prm.nb_h_tiles = 2;
  prm.nb_node_p = 4;
  prm.nb_node_h = 4;
  prm.tile_basename = "eos_init_status_test_tile";
  const std::string manifest_name = "eos_init_status_test.eosmm";
  if (generate_tiled_database(prm, manifest_name) != EOS_Error::good)
  {
    std::cerr << "FAILED: could not generate the tiled database" << std::endl;
    return 1;
  }
  const std::string manifest_path = eos_data_dir + "/EOS_Ipp/" + manifest_name;
  {
    EOS valid("EOS_Ipp", manifest_path.c_str());
    check_status("EOS(EOS_Ipp, valid manifest path)", valid, true);

    EOS valid_by_name("EOS_Ipp", "eos_init_status_test");
    check_status("EOS(EOS_Ipp, valid index.eos name)", valid_by_name, true);
  }

  // ---- 4. re-initialisation: the status is that of the last init()
  {
    EOS later("EOS_Ipp");
    check_status("EOS(EOS_Ipp) before any init", later, true);

    Strings bad(1);
    bad[0] = missing_manifest.c_str();
    const int iret_bad = later.init(bad);
    check_status("init(missing manifest)", later, false);
    check_returned("init(missing manifest)", iret_bad, later);

    Strings good(1);
    good[0] = manifest_path.c_str();
    const int iret_good = later.init(good);
    check_status("init(valid manifest) after a failed init", later, true);
    check_returned("init(valid manifest)", iret_good, later);
  }

  if (g_failures)
  {
    std::cerr << g_failures << " init_status check(s) FAILED" << std::endl;
    return 1;
  }
  std::cout << "All init_status checks passed" << std::endl;
  return 0;
}
