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

// Does the quality criterion honour the threshold it was given?
//
// This is the rule the whole local refinement rests on: a cell is refined when
// the interpolated value differs from the model's by more than the limit. It
// regressed without a sound, because "no threshold" and "a very small
// threshold" were represented by the same field: any relative limit below 1e-9
// was rewritten to the sentinel meaning "never refine", so asking for a finer
// mesh produced a coarser one (cf. EOS_IGen_QI's constructor).
//
// Exercised directly on EOS_IGen_QI rather than through a generation, so it
// costs milliseconds and can afford to check the boundary cases one by one.

#include "EOS_IGen/API/EOS_IGen_QI.hxx"

#include <iostream>
#include <string>

using namespace NEPTUNE;
using namespace NEPTUNE_EOS_IGEN;

namespace
{
  int g_failures = 0;

  //! Runs the criterion over one node whose interpolated value is off by
  //! `rel_error` in relative terms, and says whether the node was rejected.
  bool rejects(double limit, int is_abs, double rel_error)
  {
    ArrOfDouble a(1), b(1);
    b[0] = 1000.;
    a[0] = b[0] * (1. + rel_error);

    EOS_Field f_ipp("T", "T", NEPTUNE::T, a);
    EOS_Field f_eos("T", "T", NEPTUNE::T, b);

    ArrOfInt ok(1);
    ok = 1;

    EOS_IGen_QI qi("T", "centre", limit, is_abs);
    qi.make_quality(f_ipp, f_eos, ok);
    return ok[0] == 0;
  }

  //! Runs two criteria in turn over the same three cells, as
  //! EOS_IGen::compute_qualities does, and returns which cells came out
  //! rejected. `swap` runs them the other way round.
  //!
  //! Cell 0 fails the first criterion only, cell 1 the second only, cell 2
  //! neither, so the union is the only answer that can be right.
  std::string two_criteria(bool swap)
  {
    ArrOfDouble ipp_a(3), ipp_b(3), eos(3);
    eos[0] = eos[1] = eos[2] = 1000.;
    ipp_a[0] = eos[0] * (1. + 1.e-2);   // way over criterion A
    ipp_a[1] = eos[1] * (1. + 1.e-8);   // under both
    ipp_a[2] = eos[2] * (1. + 1.e-8);
    ipp_b[0] = eos[0] * (1. + 1.e-8);
    ipp_b[1] = eos[1] * (1. + 1.e-2);   // way over criterion B
    ipp_b[2] = eos[2] * (1. + 1.e-8);

    EOS_Field fa("T", "T", NEPTUNE::T, ipp_a);
    EOS_Field fb("rho", "rho", NEPTUNE::rho, ipp_b);
    EOS_Field fe_a("T", "T", NEPTUNE::T, eos);
    EOS_Field fe_b("rho", "rho", NEPTUNE::rho, eos);

    ArrOfInt ok(3);
    ok = 1;
    EOS_IGen_QI qa("T", "centre", 1.e-4, 0);
    EOS_IGen_QI qb("rho", "centre", 1.e-4, 0);
    if (swap)
      { qb.make_quality(fb, fe_b, ok); qa.make_quality(fa, fe_a, ok); }
    else
      { qa.make_quality(fa, fe_a, ok); qb.make_quality(fb, fe_b, ok); }

    std::string r;
    for (int i = 0; i < 3; i++)  r += (ok[i] == 0) ? 'R' : 'k';
    return r;
  }

  void check(const char *what, bool got, bool want)
  {
    if (got == want)
      std::cout << "  ok   : " << what << std::endl;
    else
    {
      std::cerr << "  FAIL : " << what << " (got " << (got ? "refine" : "keep")
                << ", expected " << (want ? "refine" : "keep") << ")" << std::endl;
      g_failures++;
    }
  }
}

int main()
{
  std::cout << std::endl
            << "--------------------------------------- " << std::endl
            << "------- EOS_IGen quality limit --------- " << std::endl
            << "--------------------------------------- " << std::endl;

  Language_init();

  // An ordinary threshold, either side of it
  check("rel 1e-3, error 1e-2 -> refine", rejects(1.e-3, 0, 1.e-2), true);
  check("rel 1e-3, error 1e-4 -> keep",   rejects(1.e-3, 0, 1.e-4), false);

  // The range that used to be thrown away. 1e-9 was the old cut-off, so these
  // are the cases that decide whether the bug is back.
  check("rel 5e-10, error 1e-9  -> refine", rejects(5.e-10, 0, 1.e-9),  true);
  check("rel 1e-12, error 1e-11 -> refine", rejects(1.e-12, 0, 1.e-11), true);
  check("rel 5e-10, error 1e-11 -> keep",   rejects(5.e-10, 0, 1.e-11), false);

  // Absolute criterion: the value is 1000, so a relative 1e-2 is an absolute 10
  check("abs 1., error 1e-2 (=10 abs) -> refine", rejects(1., 1, 1.e-2), true);
  check("abs 100., error 1e-2 (=10 abs) -> keep", rejects(100., 1, 1.e-2), false);

  // No threshold: the default sentinel, and the degenerate values that must
  // read the same way rather than as "refine everything"
  check("no limit given -> keep", rejects(-9999.9, 0, 1.e30), false);
  check("limit 0        -> keep", rejects(0., 0, 1.e30), false);

  // Several quality properties at once. EOS_IGen::set_quality appends to a
  // list and compute_qualities runs every entry over the same per-cell verdict
  // array, so a cell has to end up refined when *any* property fails there and
  // the answer must not depend on the order they were given in. That holds
  // because make_quality only ever writes a rejection and never clears one --
  // which is easy to undo by accident, hence these two.
  check("two properties, in order  -> union", two_criteria(false) == "RRk", true);
  check("two properties, swapped   -> union", two_criteria(true)  == "RRk", true);

  if (g_failures == 0)
    std::cout << std::endl << "EOSIGenQILimitTest: all checks passed" << std::endl;
  else
    std::cerr << std::endl << "EOSIGenQILimitTest: " << g_failures
              << " check(s) failed" << std::endl;
  return (g_failures == 0) ? 0 : 1;
}
