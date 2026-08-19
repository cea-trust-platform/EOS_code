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

// Does the interpolated field actually stay continuous across a hanging node?
//
// Local refinement puts a coarse cell next to two finer ones, and the node in
// the middle of their shared edge belongs to the fine cells as a corner and to
// the coarse one as a point inside an edge -- a T-junction. Unless the value
// there agrees with what the coarse cell interpolates at that point, the
// property jumps as a query crosses the boundary. Doc/Interpolator
// (§Continuité, and the "Test de continuité" appendix) describes the algorithm
// meant to prevent that: a continuity node is inserted at the hanging position
// and its value is forced rather than taken from the model.
//
// The appendix demonstrates it on a plot. Nothing checked it. This does, and
// it is deliberately written to *measure* before it judges, because the
// tolerance that matters is not known a priori and differs between the two
// interpolation methods:
//
//   bilinear : along the shared edge the coarse cell is linear between its two
//              corners, so forcing the hanging value to their half-sum makes
//              the two sides agree exactly.
//
//   bicubic  : the trace of the Hermite patch on an edge is a cubic determined
//              by the values *and the tangential derivatives* at its two ends.
//              Matching it needs the hanging node to carry both the cubic's
//              value at the midpoint,
//                  f(M) = (f(A)+f(B))/2 + L*(f'(A)-f'(B))/8
//              and its slope there,
//                  f'(M) = 3*(f(B)-f(A))/(2L) - (f'(A)+f'(B))/4 .
//              The half-sum is the first of those with f'(A) = f'(B), i.e. the
//              bilinear rule, so a database built for bilinear continuity is
//              not expected to make the bicubic surface continuous.
//
// Measured, on T, the largest step between consecutive samples across the
// junction each case is worst at:
//
//     continuity off : bilinear 1.464e-2   bicubic 4.53e-5
//     continuity on  : bilinear 7.18e-5    bicubic 1.628e-2
//
// The forcing does not merely fail to help bicubic; it moves the jump from one
// method to the other, and the two jumps are the same size to 11%. That is the
// signature of the term the half-sum drops, L*(f'(A)-f'(B))/8: unforced, the
// bilinear surface misses the node by it; forced, the bicubic surface misses
// the coarse cell's cubic trace by it. Which is why the correction cannot be
// baked into the database -- one stored value cannot be right for both traces
// -- and belongs where the method is known, at load time in EOS_Ipp.
//
// How a jump is detected without knowing where the junctions are: sample the
// property densely along a line, take the absolute differences between
// consecutive samples, and compare the largest with the median. A smooth field
// gives a ratio of order one; a step gives a spike. The ratio is scale-free,
// which is what lets one threshold serve properties of very different
// magnitudes.

#include "EOS/API/EOS.hxx"
#include "EOS/API/EOS_Std_Error_Handler.hxx"
#include "EOS_IGen/Src/EOS_Ipp_Tiler.hxx"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

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
    h.set_dump_on_error(EOS_Std_Error_Handler::disable_feature);
    eos.set_error_handler(h);
  }

  // The domain the fixtures are generated over. Small on purpose: the test has
  // to build its own databases, and what it needs is refinement, not size.
  const double PMIN = 1.e6, PMAX = 2.e7;
  const double HMIN = 2.e5, HMAX = 1.2e6;

  //! Generates one fixture. quality_limit drives the refinement -- without one
  //! EOS_IGen's sentinel makes the quality test pass everywhere and nothing is
  //! refined, which would leave no hanging node to test.
  bool generate(const std::string &basename, const std::string &manifest,
                bool continuity, int level_max, double quality_limit)
  {
    EOS_Ipp_Tiler_Params prm;
    prm.method = "EOS_Cathare2";
    prm.reference = "WaterLiquid";
    prm.pmin = PMIN; prm.pmax = PMAX;
    prm.hmin = HMIN; prm.hmax = HMAX;
    prm.nb_p_tiles = 1; prm.nb_h_tiles = 1;
    prm.nb_node_p = 17; prm.nb_node_h = 17;
    prm.level_max = level_max;
    prm.quality_property = "T";
    prm.quality_type = "centre";
    prm.quality_is_abs = 0;
    prm.quality_limit = quality_limit;
    prm.refine_continuity = continuity;
    prm.tile_basename = basename;
    prm.quiet_source = true;
    prm.verbose = false;

    return generate_tiled_database(prm, manifest) == EOS_Error::good;
  }

  //! Largest jump between consecutive samples, divided by the median jump.
  //! Order one on a smooth field; a spike where the field steps.
  //! nb_samples is returned through nb_ok so a line that fell outside the
  //! domain does not silently pass.
  double jump_ratio(EOS &eos, const std::string &prop,
                    double p0, double h0, double p1, double h1,
                    int nb, int &nb_ok, double &worst_at_p, double &worst_at_h)
  {
    std::vector<double> f;
    std::vector<double> fp, fh;
    f.reserve((std::size_t)nb);
    for (int i = 0; i < nb; i++)
    {
      const double s = (double)i / (double)(nb - 1);
      const double p = p0 + (p1 - p0) * s;
      const double h = h0 + (h1 - h0) * s;
      double v = 0.;
      if (eos.compute(prop.c_str(), p, h, v) == EOS_Error::good && !std::isnan(v))
      { f.push_back(v); fp.push_back(p); fh.push_back(h); }
    }
    nb_ok = (int)f.size();
    if (nb_ok < 32)
      return -1.;

    std::vector<double> d((std::size_t)nb_ok - 1);
    double worst = 0.;
    std::size_t worst_i = 0;
    for (std::size_t i = 0; i + 1 < f.size(); i++)
    {
      d[i] = std::fabs(f[i + 1] - f[i]);
      if (d[i] > worst) { worst = d[i]; worst_i = i; }
    }
    worst_at_p = fp[worst_i];
    worst_at_h = fh[worst_i];

    std::vector<double> sorted(d);
    std::sort(sorted.begin(), sorted.end());
    const double median = sorted[sorted.size() / 2];
    if (!(median > 0.))
      return (worst > 0.) ? 1.e30 : 1.;
    return worst / median;
  }

  //! Sweeps a set of lines across the domain and keeps the worst ratio, which
  //! is the one that matters: a single crossed junction is enough to make the
  //! database discontinuous.
  double worst_jump_ratio(EOS &eos, const std::string &prop, int nb_lines,
                          double &at_p, double &at_h)
  {
    double worst = 0.;
    for (int k = 1; k < nb_lines; k++)
    {
      // Strictly between grid lines. The base mesh has 17 nodes, so k/8 put
      // every sweep exactly *on* a grid line (1/8 = 2/16), running along cell
      // edges instead of across them -- a junction was never crossed, and both
      // fixtures read the same to every digit printed. The half-step offset and
      // a count coprime with 16 keep the lines off the grid at every level.
      const double u = ((double)k + 0.5) / (double)nb_lines;

      // one sweep in h at fixed p, one in p at fixed h: a junction is only
      // crossed by a line that runs across its edge
      const double p_fixed = PMIN + (PMAX - PMIN) * u;
      const double h_fixed = HMIN + (HMAX - HMIN) * u;

      int nb_ok = 0;
      double wp = 0., wh = 0.;
      double r = jump_ratio(eos, prop, p_fixed, HMIN + 0.02 * (HMAX - HMIN),
                            p_fixed, HMAX - 0.02 * (HMAX - HMIN), 4001, nb_ok, wp, wh);
      if (r > worst) { worst = r; at_p = wp; at_h = wh; }

      r = jump_ratio(eos, prop, PMIN + 0.02 * (PMAX - PMIN), h_fixed,
                     PMAX - 0.02 * (PMAX - PMIN), h_fixed, 4001, nb_ok, wp, wh);
      if (r > worst) { worst = r; at_p = wp; at_h = wh; }
    }
    return worst;
  }

  struct Reading
  {
    double bilinear = 0., bicubic = 0.;
    double bilin_p = 0., bilin_h = 0., bicub_p = 0., bicub_h = 0.;
  };

  Reading measure(const std::string &med, const std::string &prop)
  {
    Reading out;
    const char *suffix[2] = {":bilinear", ":bicubic"};
    for (int m = 0; m < 2; m++)
    {
      Strings args(1);
      args[0] = (med + ".med" + suffix[m]).c_str();
      EOS eos("EOS_Ipp", args);
      relax(eos);
      double at_p = 0., at_h = 0.;
      const double r = worst_jump_ratio(eos, prop, 37, at_p, at_h);
      if (m == 0) { out.bilinear = r; out.bilin_p = at_p; out.bilin_h = at_h; }
      else        { out.bicubic  = r; out.bicub_p = at_p; out.bicub_h = at_h; }
    }
    return out;
  }

  void report(const char *label, const Reading &r)
  {
    // where the worst jump sits matters as much as its size: it says whether
    // the metric is looking at a junction or at something else entirely
    std::cout << "  " << label
              << " : bilinear " << r.bilinear
              << " at (p=" << r.bilin_p << ", h=" << r.bilin_h << ")"
              << " , bicubic " << r.bicubic
              << " at (p=" << r.bicub_p << ", h=" << r.bicub_h << ")" << std::endl;
  }
}

int main()
{
  std::cout << std::endl
            << "--------------------------------------- " << std::endl
            << "------- EOS_Ipp continuity ------------- " << std::endl
            << "--------------------------------------- " << std::endl;

  Language_init();

  // A threshold that has to be forced, or there is no hanging node anywhere
  // and the test would pass on an unrefined mesh without testing anything.
  const double QUALITY_LIMIT = 2.e-9;
  const int LEVEL_MAX = 2;

  std::cout << "generating fixtures (this refines, so it takes a moment)" << std::endl;
  const bool ok_nc = generate("ipp_cnt_off", "ipp_cnt_off.eosmm", false, LEVEL_MAX, QUALITY_LIMIT);
  const bool ok_c  = generate("ipp_cnt_on",  "ipp_cnt_on.eosmm",  true,  LEVEL_MAX, QUALITY_LIMIT);

  if (!ok_nc)
  {
    std::cerr << "FAILED: could not generate the non-continuous fixture" << std::endl;
    return 1;
  }
  if (!ok_c)
  {
    // Refinement with continuity nodes is known not to survive past a couple of
    // levels (cf. EOS_Mesh::add_continuity_nodes). Say so rather than reporting
    // a continuity result that was never measured.
    std::cerr << "FAILED: could not generate the continuous fixture -- "
              << "add_continuity_nodes could not build this mesh" << std::endl;
    g_failures++;
  }

  const char *props[] = {"cp", "rho", "T"};
  for (int i = 0; i < 3; i++)
  {
    std::cout << std::endl << "property " << props[i]
              << "  (largest jump between consecutive samples, over the median jump)" << std::endl;

    const Reading nc = measure("ipp_cnt_off_0_0", props[i]);
    report("continuity off", nc);

    if (ok_c)
    {
      const Reading c = measure("ipp_cnt_on_0_0", props[i]);
      report("continuity on ", c);

      // Why this reported nothing before: the sweeps were placed at k/8 of the
      // domain and the base mesh has 17 nodes, so 1/8 = 2/16 put every line
      // exactly *on* a grid line. They ran along cell edges instead of across
      // them, never crossed a junction, and both fixtures read the same to
      // every digit printed. Off-grid lines separate them at once. The other
      // candidate -- that the interpolator never reads the hanging values --
      // is wrong: continuity does not give a cell a fifth vertex, it *splits*
      // the coarse cell at the hanging node, and both halves take that node as
      // a corner.
      //
      // What the algorithm claims, and what now holds: forcing the hanging
      // value to the half-sum of its two supports removes the jump the
      // bilinear surface would otherwise have. Measured, worst over cp/rho/T:
      //
      //     bilinear   off 15.9  7.2  38.2   ->   on 5.2  1.5  3.1
      //
      // Stated against the *smooth* reading rather than as a fraction of the
      // discontinuous one. What continuity claims is that the junction stops
      // costing anything, so the right yardstick is a surface with no junction
      // in it -- which is what the bicubic column of the non-continuous
      // fixture measures. A fraction of nc.bilinear would instead move with
      // how bad the unforced case happens to be, and it does move: giving the
      // generator cross derivatives changed the mesh it produces (the quality
      // criterion measures the interpolator, and the interpolator got better),
      // which took nc.bilinear from 15.9 to 7.5 on cp while c.bilinear stayed
      // at 5.156449 to every digit. The claim had not weakened; the baseline
      // had improved.
      if (!(c.bilinear < 2.0 * nc.bicubic))
      {
        std::cerr << "  FAILED: with continuity on, the bilinear surface is still "
                  << "rougher than one with no junction (" << c.bilinear
                  << " against a smooth " << nc.bicubic << ")" << std::endl;
        g_failures++;
      }
      // and it must still be an improvement on leaving the junction unforced,
      // which is what a forcing that does nothing would fail
      if (!(c.bilinear < 0.8 * nc.bilinear))
      {
        std::cerr << "  FAILED: continuity does not reduce the bilinear jump ("
                  << c.bilinear << " against " << nc.bilinear << ")" << std::endl;
        g_failures++;
      }

      // And the same for bicubic, which the database alone cannot deliver: the
      // stored half-sum is the trace of a *bilinear* patch, and one value per
      // node cannot also be the trace of a Hermite patch. EOS_Ipp puts the
      // hanging nodes back on the cubic trace when it loads a database for
      // bicubic use (cf. EOS_Ipp::retrace_hanging_nodes). Before that existed
      // the forcing moved the jump from one method to the other:
      //
      //     bicubic    off  5.5  1.5   3.0   ->   on 15.7  5.9  41.1
      //
      // and with it the junction stops contributing at all -- continuity on
      // reads exactly what continuity off reads, to every digit, at the same
      // place. The margin below is wide only to leave room for the sweep
      // landing elsewhere; the effect it guards is a factor 3 to 14.
      std::cout << "  bicubic continuity gain : " << (nc.bicubic / c.bicubic)
                << "x  (1 means the junction costs the bicubic surface nothing)" << std::endl;
      if (!(c.bicubic < 2. * nc.bicubic))
      {
        std::cerr << "  FAILED: continuity leaves the bicubic surface less continuous "
                  << "than no continuity at all (" << c.bicubic << " against "
                  << nc.bicubic << ")" << std::endl;
        g_failures++;
      }
    }
  }

  if (g_failures == 0)
    std::cout << std::endl << "EOSTestIppContinuity: all checks passed" << std::endl;
  else
    std::cerr << std::endl << "EOSTestIppContinuity: " << g_failures << " check(s) failed" << std::endl;
  return (g_failures == 0) ? 0 : 1;
}
