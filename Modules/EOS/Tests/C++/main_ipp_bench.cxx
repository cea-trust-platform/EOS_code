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

// Performance bench and value-regression harness for EOS_Ipp.
//
// Two jobs in one binary, deliberately: a timing number is only worth
// something next to the proof that the values did not move to get it. Every
// scenario therefore both times a workload and can dump the values it
// produced, so an optimisation is validated by
//     eos_ipp_bench --dump before.txt      (before the change)
//     eos_ipp_bench --check before.txt     (after it)
// which compares bit for bit, not within a tolerance: an optimisation that
// only reorders work has no reason to move the last ulp, and one that does
// move it should have to say so.
//
// Scenarios, each isolating one cost the module is known to pay:
//   load      opening the database (mesh load + f_mesh2r_mesh prétraitement)
//   ph1       (p,h) batch, one property        -- baseline per-point cost
//   phN       (p,h) batch, N properties        -- shows the per-property
//                                                 re-location of the cell
//   pTN       (p,T) batch, N properties        -- shows the per-property
//                                                 re-inversion of h(p,T)
//   phN_oob   (p,h) batch, N properties, a few points outside the domain,
//             with a reference model attached  -- shows what one bad point
//                                                 costs the whole batch
//   scalar    scalar compute_rho_ph loop       -- per-call overhead, no batch
//
// Points are drawn from a fixed LCG so two runs of the same binary on the
// same database see exactly the same workload.

#include "EOS/API/EOS.hxx"
#include "EOS/API/EOS_Std_Error_Handler.hxx"
#include "timer.hxx"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

using namespace NEPTUNE;

namespace
{
  // ---------------------------------------------------------------- options

  struct Options
  {
    std::string reference = "raffinement_local_EOS_Cathare2.med:bicubic";
    int accuracy = 0;
    std::string model     = "EOS_Cathare2";
    std::string fluid     = "WaterLiquid";
    int         nb_points = 200000;
    int         nb_repeat = 3;
    std::string dump_path;
    std::string check_path;
    bool        quiet_scenarios = false;
  };

  void usage(const char *argv0)
  {
    std::cout
      << "usage: " << argv0 << " [options]\n"
      << "  --reference <name>   EOS_Ipp reference, e.g. \"base.med:bicubic\" or \"base.eosmm\"\n"
      << "  --model <name>       reference model for the fallback scenario (default EOS_Cathare2)\n"
      << "  --fluid <name>       reference fluid  for the fallback scenario (default WaterLiquid)\n"
      << "  --points <n>         points per batch (default 200000)\n"
      << "  --repeat <n>         timed repetitions, best kept (default 3)\n"
      << "  --dump <file>        write every computed value, for a later --check\n"
      << "  --check <file>       compare every computed value against <file>, bit for bit\n"
      << "  --accuracy <n>       also measure interpolation error against the reference\n"
      << "                       model on an n x n grid (0, off, by default)\n"
      << "\n"
      << "the database is looked up under $USER_EOS_DATA/EOS_Ipp/ . ctest sets that\n"
      << "variable per test; an interactive shell does not, and without it the load\n"
      << "fails.\n"
      << std::endl;
  }

  // -------------------------------------------------------------- utilities

  // Resident set size, in kB, straight from /proc: the point of the tiled
  // work and of the index structures is memory, so a bench that only reports
  // seconds would miss half of what changes.
  long rss_kb()
  {
    std::ifstream in("/proc/self/status");
    std::string line;
    while (std::getline(in, line))
      if (line.compare(0, 6, "VmRSS:") == 0)
        return atol(line.c_str() + 6);
    return -1;
  }

  // A (p,T) point whose root lies outside the meshed region is a normal part
  // of the workload, not a reason to stop -- and dumping the whole field for
  // each one would time the error handler rather than the interpolator.
  void relax(EOS &eos)
  {
    EOS_Std_Error_Handler h;
    h.set_exit_on_error(EOS_Std_Error_Handler::disable_feature);
    h.set_throw_on_error(EOS_Std_Error_Handler::disable_feature);
    h.set_dump_on_error(EOS_Std_Error_Handler::disable_feature);
    eos.set_error_handler(h);
  }

  // Fixed 64-bit LCG: the workload must be identical between a --dump run and
  // the --check run that validates a change against it.
  class Lcg
  {
  public:
    explicit Lcg(unsigned long long seed) : s_(seed) {}
    double next01()
    {
      s_ = s_ * 6364136223846793005ULL + 1442695040888963407ULL;
      return (double)((s_ >> 11) & 0x1FFFFFFFFFFFFFULL) / (double)0x20000000000000ULL;
    }
  private:
    unsigned long long s_;
  };

  // ------------------------------------------------------------- collecting

  //! One scenario's timing plus the values it produced. The values are what
  //! makes the timing trustworthy, so they travel together.
  struct Result
  {
    std::string        name;
    double             seconds = 0.;
    long               nb_calls = 0;
    std::vector<double> values;   // dumped / checked
    std::vector<int>    codes;    // dumped / checked
    std::string        note;
  };

  // Only a sample of the values is kept: comparing 200000 x 7 doubles adds
  // nothing over comparing a well-spread 20000 of them, and keeps the
  // reference file readable and cheap to diff.
  const int SAMPLE_STRIDE = 37;

  void collect(Result &r, const EOS_Fields &out, const EOS_Error_Field &err)
  {
    for (int f = 0; f < out.size(); f++)
      for (int i = 0; i < out[f].size(); i += SAMPLE_STRIDE)
      {
        r.values.push_back(out[f][i]);
        r.codes.push_back(err[i].get_code());
      }
  }

  // ------------------------------------------------------------- the bench

  class Bench
  {
  public:
    Bench(const Options &opt) : opt_(opt) {}

    bool run();
    const std::vector<Result> &results() const { return results_; }
    long load_rss_kb() const { return load_rss_kb_; }

  private:
    void scenario_ph(EOS &eos, const char *name, const std::vector<std::string> &props);
    void scenario_ph_local(EOS &eos, const char *name, const std::vector<std::string> &props);
    void scenario_pT(EOS &eos, const char *name, const std::vector<std::string> &props,
                     bool coherent = false);
    void scenario_ph_oob(EOS &eos, const char *name, const std::vector<std::string> &props);
    void scenario_scalar(EOS &eos);

    // Fills p/h (or p/T) with points well inside the domain. oob_fraction of
    // them are pushed just outside it instead, which is what a host code
    // actually produces at the edge of a transient.
    void make_points(std::vector<double> &x, std::vector<double> &y,
                     double xmin, double xmax, double ymin, double ymax,
                     double oob_fraction, unsigned long long seed) const;

    Options              opt_;
    std::vector<Result>  results_;
    long                 load_rss_kb_ = -1;
    double               pmin_ = 0., pmax_ = 0., hmin_ = 0., hmax_ = 0., tmin_ = 0., tmax_ = 0.;
  };

  void Bench::make_points(std::vector<double> &x, std::vector<double> &y,
                          double xmin, double xmax, double ymin, double ymax,
                          double oob_fraction, unsigned long long seed) const
  {
    const int n = opt_.nb_points;
    x.resize((std::size_t)n);
    y.resize((std::size_t)n);
    Lcg rng(seed);

    // 2% margin: a point exactly on a domain edge exercises the boundary
    // clamping rather than the interpolation this bench is about.
    const double xlo = xmin + 0.02 * (xmax - xmin), xhi = xmax - 0.02 * (xmax - xmin);
    const double ylo = ymin + 0.02 * (ymax - ymin), yhi = ymax - 0.02 * (ymax - ymin);

    for (int i = 0; i < n; i++)
    {
      const double u = rng.next01(), v = rng.next01(), w = rng.next01();
      if (oob_fraction > 0. && w < oob_fraction)
      {
        x[(std::size_t)i] = xmin - 0.10 * (xmax - xmin) * (1. + u);
        y[(std::size_t)i] = ylo + (yhi - ylo) * v;
      }
      else
      {
        x[(std::size_t)i] = xlo + (xhi - xlo) * u;
        y[(std::size_t)i] = ylo + (yhi - ylo) * v;
      }
    }
  }

  void Bench::scenario_ph(EOS &eos, const char *name, const std::vector<std::string> &props)
  {
    std::vector<double> p, h;
    make_points(p, h, pmin_, pmax_, hmin_, hmax_, 0., 12345ULL);

    const int n  = opt_.nb_points;
    const int nf = (int)props.size();

    ArrOfDouble pd(n, &p[0]), hd(n, &h[0]);
    EOS_Field pf("p", "p", NEPTUNE::p, pd);
    EOS_Field hf("h", "h", NEPTUNE::h, hd);

    std::vector<ArrOfDouble> out_data((std::size_t)nf);
    EOS_Fields out(nf);
    for (int f = 0; f < nf; f++)
    {
      out_data[(std::size_t)f].resize(n);
      out[f] = EOS_Field(props[(std::size_t)f].c_str(), props[(std::size_t)f].c_str(),
                         out_data[(std::size_t)f]);
    }
    ArrOfInt err_data(n);
    EOS_Error_Field err(err_data);

    Result r;
    r.name = name;
    double best = 0.;
    for (int it = 0; it < opt_.nb_repeat; it++)
    {
      Timer t;
      t.start();
      eos.compute(pf, hf, out, err);
      t.stop();
      if (it == 0 || t.elapsed() < best) best = t.elapsed();
    }
    r.seconds  = best;
    r.nb_calls = (long)n * nf;
    collect(r, out, err);
    results_.push_back(r);
  }

  // Same request as scenario_ph, but with the spatial locality a solver
  // actually has. The uniformly random points of the other scenarios are the
  // worst case for anything that remembers the last cell: with 200000 points
  // scattered over a refined mesh, consecutive points are almost never in the
  // same one. A host code does not work that way -- it walks its own mesh, and
  // neighbouring cells of the flow sit close together in (p,h) -- so both
  // patterns have to be measured, and an optimisation that helps one must be
  // checked not to hurt the other.
  //
  // The field here is a smooth sweep (a slow loop across the domain) with a
  // little noise on top, which keeps the points moving without teleporting
  // them.
  void Bench::scenario_ph_local(EOS &eos, const char *name, const std::vector<std::string> &props)
  {
    const int n  = opt_.nb_points;
    const int nf = (int)props.size();

    std::vector<double> p((std::size_t)n), h((std::size_t)n);
    {
      Lcg rng(97531ULL);
      const double p0 = pmin_ + 0.05 * (pmax_ - pmin_), p1 = pmax_ - 0.05 * (pmax_ - pmin_);
      const double h0 = hmin_ + 0.05 * (hmax_ - hmin_), h1 = hmax_ - 0.05 * (hmax_ - hmin_);
      const double period = 5000.;
      for (int i = 0; i < n; i++)
      {
        const double u = 0.5 * (1. + sin(2. * M_PI * (double)i / period));
        const double v = 0.5 * (1. + cos(2. * M_PI * (double)i / (period * 1.7)));
        const double jp = 0.002 * (rng.next01() - 0.5);
        const double jh = 0.002 * (rng.next01() - 0.5);
        p[(std::size_t)i] = p0 + (p1 - p0) * std::min(1., std::max(0., u + jp));
        h[(std::size_t)i] = h0 + (h1 - h0) * std::min(1., std::max(0., v + jh));
      }
    }

    ArrOfDouble pd(n, &p[0]), hd(n, &h[0]);
    EOS_Field pf("p", "p", NEPTUNE::p, pd);
    EOS_Field hf("h", "h", NEPTUNE::h, hd);

    std::vector<ArrOfDouble> out_data((std::size_t)nf);
    EOS_Fields out(nf);
    for (int f = 0; f < nf; f++)
    {
      out_data[(std::size_t)f].resize(n);
      out[f] = EOS_Field(props[(std::size_t)f].c_str(), props[(std::size_t)f].c_str(),
                         out_data[(std::size_t)f]);
    }
    ArrOfInt err_data(n);
    EOS_Error_Field err(err_data);

    Result r;
    r.name = name;
    double best = 0.;
    for (int it = 0; it < opt_.nb_repeat; it++)
    {
      Timer t;
      t.start();
      eos.compute(pf, hf, out, err);
      t.stop();
      if (it == 0 || t.elapsed() < best) best = t.elapsed();
    }
    r.seconds  = best;
    r.nb_calls = (long)n * nf;
    r.note     = "spatially coherent points, as a solver produces";
    collect(r, out, err);
    results_.push_back(r);
  }

  void Bench::scenario_pT(EOS &eos, const char *name, const std::vector<std::string> &props,
                          bool coherent)
  {
    const int n  = opt_.nb_points;
    const int nf = (int)props.size();

    std::vector<double> p, T;
    if (coherent)
    {
      // The (p,T) counterpart of scenario_ph_local: consecutive points stay in
      // the same p-column, as a solver's neighbouring mesh cells do. The
      // uniformly random alternative below is the worst case for anything that
      // remembers a column, so measuring only that would hide both the gain
      // and any cost.
      p.resize((std::size_t)n);
      T.resize((std::size_t)n);
      Lcg rng(86420ULL);
      const double p0 = pmin_ + 0.05 * (pmax_ - pmin_), p1 = pmax_ - 0.05 * (pmax_ - pmin_);
      const double t0 = tmin_ + 0.05 * (tmax_ - tmin_), t1 = tmax_ - 0.05 * (tmax_ - tmin_);
      const double period = 5000.;
      for (int i = 0; i < n; i++)
      {
        const double u = 0.5 * (1. + sin(2. * M_PI * (double)i / period));
        const double v = 0.5 * (1. + cos(2. * M_PI * (double)i / (period * 1.7)));
        p[(std::size_t)i] = p0 + (p1 - p0) * std::min(1., std::max(0., u + 0.002 * (rng.next01() - 0.5)));
        T[(std::size_t)i] = t0 + (t1 - t0) * std::min(1., std::max(0., v + 0.002 * (rng.next01() - 0.5)));
      }
    }
    else
    {
      // T bounds come from the database; staying inside them is what makes the
      // inversion succeed rather than scan a whole column for nothing.
      make_points(p, T, pmin_, pmax_, tmin_, tmax_, 0., 67890ULL);
    }

    ArrOfDouble pd(n, &p[0]), Td(n, &T[0]);
    EOS_Field pf("p", "p", NEPTUNE::p, pd);
    EOS_Field Tf("T", "T", NEPTUNE::T, Td);

    std::vector<ArrOfDouble> out_data((std::size_t)nf);
    EOS_Fields out(nf);
    for (int f = 0; f < nf; f++)
    {
      out_data[(std::size_t)f].resize(n);
      out[f] = EOS_Field(props[(std::size_t)f].c_str(), props[(std::size_t)f].c_str(),
                         out_data[(std::size_t)f]);
    }
    ArrOfInt err_data(n);
    EOS_Error_Field err(err_data);

    Result r;
    r.name = name;
    double best = 0.;
    for (int it = 0; it < opt_.nb_repeat; it++)
    {
      Timer t;
      t.start();
      eos.compute(pf, Tf, out, err);
      t.stop();
      if (it == 0 || t.elapsed() < best) best = t.elapsed();
    }
    r.seconds  = best;
    r.nb_calls = (long)n * nf;
    if (coherent)
      r.note = "spatially coherent points, as a solver produces";
    collect(r, out, err);
    results_.push_back(r);
  }

  void Bench::scenario_ph_oob(EOS &eos, const char *name, const std::vector<std::string> &props)
  {
    // 0.5% of the points outside the domain. A reference model is attached to
    // this EOS beforehand by the caller, so this measures what the module
    // actually does with a batch it cannot fully interpolate.
    std::vector<double> p, h;
    make_points(p, h, pmin_, pmax_, hmin_, hmax_, 0.005, 24680ULL);

    const int n  = opt_.nb_points;
    const int nf = (int)props.size();

    ArrOfDouble pd(n, &p[0]), hd(n, &h[0]);
    EOS_Field pf("p", "p", NEPTUNE::p, pd);
    EOS_Field hf("h", "h", NEPTUNE::h, hd);

    std::vector<ArrOfDouble> out_data((std::size_t)nf);
    EOS_Fields out(nf);
    for (int f = 0; f < nf; f++)
    {
      out_data[(std::size_t)f].resize(n);
      out[f] = EOS_Field(props[(std::size_t)f].c_str(), props[(std::size_t)f].c_str(),
                         out_data[(std::size_t)f]);
    }
    ArrOfInt err_data(n);
    EOS_Error_Field err(err_data);

    Result r;
    r.name = name;
    double best = 0.;
    for (int it = 0; it < opt_.nb_repeat; it++)
    {
      Timer t;
      t.start();
      eos.compute(pf, hf, out, err);
      t.stop();
      if (it == 0 || t.elapsed() < best) best = t.elapsed();
    }
    r.seconds  = best;
    r.nb_calls = (long)n * nf;
    r.note     = "0.5% of the points outside the domain, reference model attached";
    collect(r, out, err);
    results_.push_back(r);
  }

  void Bench::scenario_scalar(EOS &eos)
  {
    std::vector<double> p, h;
    make_points(p, h, pmin_, pmax_, hmin_, hmax_, 0., 13579ULL);

    const int n = opt_.nb_points;
    Result r;
    r.name = "scalar";
    double best = 0.;
    double sink = 0.;
    for (int it = 0; it < opt_.nb_repeat; it++)
    {
      Timer t;
      t.start();
      double acc = 0.;
      for (int i = 0; i < n; i++)
      {
        double rho = 0.;
        eos.compute_rho_ph(p[(std::size_t)i], h[(std::size_t)i], rho);
        acc += rho;
      }
      t.stop();
      sink = acc;
      if (it == 0 || t.elapsed() < best) best = t.elapsed();
    }
    r.seconds  = best;
    r.nb_calls = n;
    // The accumulated sum is a value check in its own right, and it stops the
    // loop being optimised away.
    r.values.push_back(sink);
    r.codes.push_back(0);
    results_.push_back(r);
  }

  bool Bench::run()
  {
    // ---- load ----
    Strings args(1);
    args[0] = opt_.reference.c_str();

    const long rss_before = rss_kb();
    Timer tload;
    tload.start();
    EOS eos("EOS_Ipp", args);
    tload.stop();
    load_rss_kb_ = rss_kb() - rss_before;
    relax(eos);

    Result rload;
    rload.name    = "load";
    rload.seconds = tload.elapsed();
    rload.nb_calls = 1;
    results_.push_back(rload);

    eos.get_p_min(pmin_);
    eos.get_p_max(pmax_);
    eos.get_h_min(hmin_);
    eos.get_h_max(hmax_);
    eos.get_T_min(tmin_);
    eos.get_T_max(tmax_);
    std::cout << "  domain : p=[" << pmin_ << "," << pmax_ << "]"
              << " h=[" << hmin_ << "," << hmax_ << "]"
              << " T=[" << tmin_ << "," << tmax_ << "]" << std::endl;
    if (!(pmax_ > pmin_) || !(hmax_ > hmin_))
    {
      std::cerr << "eos_ipp_bench: the database reports an empty (p,h) domain" << std::endl;
      return false;
    }

    // An ordered domain is not enough to say the load worked. When the .med
    // cannot be opened, EOS_Ipp still constructs, and the bounds it then
    // reports are uninitialised memory -- p=[3.2e-57,5.9e-38] in the case that
    // prompted this, which passes the test above and let the run continue to
    // completion. It timed the fallback path, exited 0, and wrote a --dump of
    // values no database had produced; a later --check against that file
    // compared garbage with garbage and reported "values identical". A
    // regression harness that cannot fail is worse than none.
    //
    // What actually settles it is whether the database answers anywhere inside
    // the domain it just declared. One good value is enough: a legitimate base
    // may well be invalid over part of its box (two-phase, out of the model's
    // validity), but one that answers nowhere has not been loaded.
    {
      int nb_good = 0;
      for (int i = 1; i <= 5 && nb_good == 0; i++)
        for (int j = 1; j <= 5 && nb_good == 0; j++)
        {
          const double p = pmin_ + (pmax_ - pmin_) * (double)i / 6.;
          const double h = hmin_ + (hmax_ - hmin_) * (double)j / 6.;
          double v = 0.;
          if (eos.compute("T", p, h, v) == EOS_Error::good)
            nb_good++;
        }
      if (nb_good == 0)
      {
        std::cerr << "eos_ipp_bench: \"" << opt_.reference
                  << "\" answers nowhere inside its own domain; it was not loaded"
                  << std::endl;
        return false;
      }
    }

    // A representative multi-property request: the value, two of its
    // derivatives, and the transport properties a solver asks for in the same
    // breath.
    std::vector<std::string> one, many;
    one.push_back("rho");
    many.push_back("T");
    many.push_back("rho");
    many.push_back("cp");
    many.push_back("mu");
    many.push_back("lambda");
    many.push_back("d_rho_d_p_h");
    many.push_back("d_rho_d_h_p");

    // (p,T) cannot ask for T back; the rest is the same request.
    std::vector<std::string> many_pT(many.begin() + 1, many.end());

    // One property from (p,T) is one h(p,T) inversion plus one interpolation,
    // so pT1 minus ph1 is the inversion on its own -- which is the only way to
    // attribute a change to the column scan rather than to everything else.
    std::vector<std::string> one_pT;
    one_pT.push_back("rho");

    scenario_ph(eos, "ph1", one);
    scenario_ph(eos, "phN", many);
    scenario_pT(eos, "pT1", one_pT);
    scenario_pT(eos, "pTN", many_pT);
    scenario_pT(eos, "pT1_loc", one_pT, true);
    scenario_scalar(eos);

    // The fallback scenario needs a reference model: without one, EOS_Ipp
    // reports the error and returns, and there is nothing to measure.
    // Derivative short-circuiting is turned off (both flags false) so that
    // what is timed is the interpolator, not the reference model standing in
    // for it on every derivative.
    EOS_Error ierr = eos.init_model(opt_.model, opt_.fluid, false, false);
    if (ierr == EOS_Error::good)
      scenario_ph_oob(eos, "phN_oob", many);
    else
      std::cout << "  (skipping phN_oob: no reference model " << opt_.model
                << "/" << opt_.fluid << ")" << std::endl;

    scenario_ph_local(eos, "phN_loc", many);

    return true;
  }

  // ------------------------------------------------------------- accuracy
  //
  // How close the interpolated surface actually is to the model it was built
  // from. The bench otherwise only compares a database with itself, which
  // catches a change but says nothing about whether the change was an
  // improvement -- so anything meant to make the interpolation *better*
  // (a finer mesh, the stored cross derivative, a better treatment of the
  // validity boundary) had no way of being judged.
  //
  // A second EOS object holds the reference model. The interpolator is loaded
  // separately and *without* init_model, so a point it cannot do returns an
  // error and is skipped rather than being quietly answered by the model --
  // which would report the model's agreement with itself as perfect accuracy.
  void measure_accuracy(const Options &opt)
  {
    Strings args(1);
    args[0] = opt.reference.c_str();
    EOS ipp("EOS_Ipp", args);
    relax(ipp);

    EOS ref(opt.model.c_str(), opt.fluid.c_str());
    relax(ref);

    double pmin = 0., pmax = 0., hmin = 0., hmax = 0.;
    ipp.get_p_min(pmin); ipp.get_p_max(pmax);
    ipp.get_h_min(hmin); ipp.get_h_max(hmax);

    const char *props[] = {"T", "rho", "cp"};
    const int nb_props = 3;
    const int n = opt.accuracy;

    std::cout << std::endl
              << "  accuracy against " << opt.model << "/" << opt.fluid
              << " on a " << n << "x" << n << " grid" << std::endl;
    std::cout << "  property      max rel err        rms rel err     n     skipped"
              << std::endl;

    for (int ip = 0; ip < nb_props; ip++)
    {
      double worst = 0., sum2 = 0., at_p = 0., at_h = 0.;
      long nb = 0, skipped = 0;

      for (int i = 1; i <= n; i++)
        for (int j = 1; j <= n; j++)
        {
          const double p = pmin + (pmax - pmin) * (double)i / (double)(n + 1);
          const double h = hmin + (hmax - hmin) * (double)j / (double)(n + 1);
          double vi = 0., vr = 0.;
          if (ipp.compute(props[ip], p, h, vi) != EOS_Error::good ||
              ref.compute(props[ip], p, h, vr) != EOS_Error::good ||
              !(vr != 0.))
          { skipped++; continue; }

          const double e = std::fabs((vi - vr) / vr);
          if (e > worst) { worst = e; at_p = p; at_h = h; }
          sum2 += e * e;
          nb++;
        }

      const double rms = (nb > 0) ? std::sqrt(sum2 / (double)nb) : 0.;
      std::cout << "  " << std::left << std::setw(10) << props[ip] << std::right
                << std::scientific << std::setprecision(4)
                << std::setw(15) << worst
                << std::setw(19) << rms
                << std::setw(8) << nb
                << std::setw(10) << skipped << std::endl;
      std::cout << std::defaultfloat;
      if (nb > 0)
        std::cout << "              worst at p=" << at_p << " h=" << at_h << std::endl;
      std::cout << "ACCURACY " << props[ip] << " " << std::setprecision(6)
                << worst << " " << rms << std::endl;
    }
  }

  // ------------------------------------------------------- dump / check I/O

  bool write_dump(const std::string &path, const std::vector<Result> &results)
  {
    std::ofstream out(path.c_str());
    if (!out.good())
    {
      std::cerr << "eos_ipp_bench: cannot write " << path << std::endl;
      return false;
    }
    out << std::setprecision(17);
    for (std::size_t s = 0; s < results.size(); s++)
    {
      const Result &r = results[s];
      if (r.values.empty())
        continue;
      out << "SCENARIO " << r.name << " " << r.values.size() << "\n";
      for (std::size_t k = 0; k < r.values.size(); k++)
        out << r.values[k] << " " << r.codes[k] << "\n";
    }
    return out.good();
  }

  // Bit-for-bit, on purpose (cf. the file header). NaN is compared as a
  // value: a point that legitimately returns NaN must keep returning NaN.
  //
  // Scenarios are matched by name, not by position, so that adding one -- which
  // is how a new cost gets isolated -- does not invalidate the reference files
  // recorded before it existed. A scenario the reference does not know is
  // reported and skipped; one it knows and this run did not produce is an error,
  // since that means coverage was lost.
  bool check_dump(const std::string &path, const std::vector<Result> &results, int &nb_diff)
  {
    std::ifstream in(path.c_str());
    if (!in.good())
    {
      std::cerr << "eos_ipp_bench: cannot read " << path << std::endl;
      return false;
    }

    nb_diff = 0;
    bool ok = true;
    std::string keyword, name;
    std::size_t count = 0;

    while (in >> keyword >> name >> count)
    {
      if (keyword != "SCENARIO")
      {
        std::cerr << "eos_ipp_bench: malformed reference file at \"" << keyword << "\"" << std::endl;
        return false;
      }

      const Result *match = nullptr;
      for (std::size_t s = 0; s < results.size(); s++)
        if (results[s].name == name)
          { match = &results[s]; break; }

      if (match == nullptr)
      {
        std::cerr << "FAILED regression: reference has scenario \"" << name
                  << "\" but this run did not produce it" << std::endl;
        return false;
      }
      if (match->values.size() != count)
      {
        std::cerr << "FAILED regression: scenario \"" << name << "\" has "
                  << match->values.size() << " values, reference has " << count << std::endl;
        return false;
      }

      for (std::size_t k = 0; k < count; k++)
      {
        double v; int c;
        in >> v >> c;
        const bool same_value = (v == match->values[k])
                             || (v != v && match->values[k] != match->values[k]); // NaN == NaN here
        if (!same_value || c != match->codes[k])
        {
          if (nb_diff < 10)
            std::cerr << "FAILED regression: " << name << "[" << k << "] "
                      << std::setprecision(17) << match->values[k] << " (code " << match->codes[k]
                      << ") != reference " << v << " (code " << c << ")" << std::endl;
          nb_diff++;
          ok = false;
        }
      }
    }
    return ok;
  }
}

int main(int argc, char **argv)
{
  Options opt;
  for (int i = 1; i < argc; i++)
  {
    const std::string a = argv[i];
    const bool has_next = (i + 1 < argc);
    if      (a == "--accuracy"  && has_next) opt.accuracy   = atoi(argv[++i]);
    else if (a == "--reference" && has_next) opt.reference  = argv[++i];
    else if (a == "--model"     && has_next) opt.model      = argv[++i];
    else if (a == "--fluid"     && has_next) opt.fluid      = argv[++i];
    else if (a == "--points"    && has_next) opt.nb_points  = atoi(argv[++i]);
    else if (a == "--repeat"    && has_next) opt.nb_repeat  = atoi(argv[++i]);
    else if (a == "--dump"      && has_next) opt.dump_path  = argv[++i];
    else if (a == "--check"     && has_next) opt.check_path = argv[++i];
    else if (a == "--help" || a == "-h")   { usage(argv[0]); return 0; }
    else { std::cerr << "unknown option: " << a << std::endl; usage(argv[0]); return 2; }
  }
  if (opt.nb_points < 1 || opt.nb_repeat < 1)
  {
    std::cerr << "eos_ipp_bench: --points and --repeat must be >= 1" << std::endl;
    return 2;
  }

  Language_init();

  std::cout << "=== EOS_Ipp bench ===" << std::endl;
  std::cout << "  reference : " << opt.reference << std::endl;
  std::cout << "  points    : " << opt.nb_points << "   repeat : " << opt.nb_repeat << std::endl;

  Bench bench(opt);
  if (!bench.run())
    return 1;

  std::cout << std::endl;
  std::cout << "  scenario        seconds     ns/value        values" << std::endl;
  for (std::size_t s = 0; s < bench.results().size(); s++)
  {
    const Result &r = bench.results()[s];
    const double ns = (r.nb_calls > 0) ? r.seconds * 1.e9 / (double)r.nb_calls : 0.;
    std::cout << "  " << std::left << std::setw(14) << r.name << std::right
              << std::setw(10) << std::fixed << std::setprecision(4) << r.seconds
              << std::setw(13) << std::setprecision(1) << ns
              << std::setw(14) << r.nb_calls;
    if (!r.note.empty())
      std::cout << "   (" << r.note << ")";
    std::cout << std::endl;
  }
  std::cout << "  resident after load : " << bench.load_rss_kb() << " kB" << std::endl;

  // One machine-readable line per scenario, so a series of runs can be diffed
  // without re-reading the table above.
  std::cout << std::endl;
  for (std::size_t s = 0; s < bench.results().size(); s++)
    std::cout << "BENCH " << bench.results()[s].name << " "
              << std::setprecision(6) << bench.results()[s].seconds << std::endl;
  std::cout << "BENCH rss_kb " << bench.load_rss_kb() << std::endl;

  if (opt.accuracy > 0)
    measure_accuracy(opt);

  int status = 0;
  if (!opt.dump_path.empty())
  {
    if (!write_dump(opt.dump_path, bench.results()))
      status = 1;
    else
      std::cout << "values written to " << opt.dump_path << std::endl;
  }
  if (!opt.check_path.empty())
  {
    int nb_diff = 0;
    if (!check_dump(opt.check_path, bench.results(), nb_diff))
    {
      std::cerr << "REGRESSION: " << nb_diff << " value(s) differ from "
                << opt.check_path << std::endl;
      status = 1;
    }
    else
      std::cout << "values identical to " << opt.check_path << std::endl;
  }
  return status;
}
