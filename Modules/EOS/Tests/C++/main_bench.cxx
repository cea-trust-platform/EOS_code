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

//
// EOSBench : reproducible performance benchmark for the EOS field API.
//
// Measures the cost of the calling patterns a system code such as CATHARE
// actually uses : repeated compute() on fields of a fixed size (steady
// regime), and on fields whose size changes between calls (mesh regime).
//
// Every scenario also prints a checksum of the computed values, so a
// performance run doubles as a numerical non-regression check : two runs
// that disagree on the checksum are not comparable on timings either.
//
// Usage :  EOSBench [--reps N] [--calls N] [--csv]
//

#include "EOS/API/EOS.hxx"
#include "EOS/API/EOS_Fields.hxx"
#include "EOS/API/EOS_Error_Field.hxx"
#include "EOS/API/EOS_Config.hxx"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

using namespace NEPTUNE;

namespace
{
  // ---------------------------------------------------------------- timing

  typedef std::chrono::high_resolution_clock clock_type;

  double now_ms()
  { return std::chrono::duration<double, std::milli>
             (clock_type::now().time_since_epoch()).count();
  }

  //! Result of one scenario : the per-repetition wall times, plus a checksum
  //! of the numbers the scenario produced.
  struct Result
  { std::string        name;
    std::string        detail;
    int                calls;
    int                points;
    std::vector<double> times_ms;
    double             checksum;
  };

  double median(std::vector<double> v)
  { std::sort(v.begin(), v.end());
    const size_t n = v.size();
    if (n == 0) return 0.0;
    if (n % 2)  return v[n/2];
    return 0.5 * (v[n/2 - 1] + v[n/2]);
  }

  double stddev(const std::vector<double> &v)
  { const size_t n = v.size();
    if (n < 2) return 0.0;
    double mean = 0.0;
    for (size_t i=0; i<n; i++) mean += v[i];
    mean /= double(n);
    double acc = 0.0;
    for (size_t i=0; i<n; i++) acc += (v[i]-mean)*(v[i]-mean);
    return std::sqrt(acc / double(n-1));
  }

  //! Order-sensitive checksum : catches a value that moved as well as a
  //! value that landed in the wrong cell.
  void accumulate(double &sum, const ArrOfDouble &a)
  { const int n = a.size();
    for (int i=0; i<n; i++)
      { const double x = a[i];
        if (x == x)                       // skip NaN, keep the sum usable
          sum += x * (1.0 + 1e-6*double(i));
      }
  }

  // ------------------------------------------------------- output fields

  //! The (p,h) output set a two-phase system code asks for in one call.
  struct PhOutputs
  { std::vector<ArrOfDouble> storage;
    EOS_Fields               fields;

    PhOutputs(int n) : storage(10), fields(10)
    { static const char *nm[10] =
        { "T", "rho", "cp", "lambda", "mu",
          "d_T_d_p_h", "d_T_d_h_p", "d_rho_d_p_h", "d_rho_d_h_p", "d_cp_d_p_h" };
      static const EOS_Property pr[10] =
        { NEPTUNE::T, NEPTUNE::rho, NEPTUNE::cp, NEPTUNE::lambda, NEPTUNE::mu,
          NEPTUNE::d_T_d_p_h, NEPTUNE::d_T_d_h_p,
          NEPTUNE::d_rho_d_p_h, NEPTUNE::d_rho_d_h_p, NEPTUNE::d_cp_d_p_h };
      for (int j=0; j<10; j++)
        { storage[j].resize(n);
          fields[j] = EOS_Field(nm[j], nm[j], pr[j], storage[j]);
        }
    }

    void checksum(double &sum) const
    { for (size_t j=0; j<storage.size(); j++) accumulate(sum, storage[j]); }
  };

  //! The saturation output set, driven by pressure alone.
  struct SatOutputs
  { std::vector<ArrOfDouble> storage;
    EOS_Fields               fields;

    SatOutputs(int n) : storage(6), fields(6)
    { static const char *nm[6] =
        { "T_sat", "h_l_sat", "h_v_sat", "rho_l_sat", "rho_v_sat", "d_T_sat_d_p" };
      static const EOS_Property pr[6] =
        { NEPTUNE::T_sat, NEPTUNE::h_l_sat, NEPTUNE::h_v_sat,
          NEPTUNE::rho_l_sat, NEPTUNE::rho_v_sat, NEPTUNE::d_T_sat_d_p };
      for (int j=0; j<6; j++)
        { storage[j].resize(n);
          fields[j] = EOS_Field(nm[j], nm[j], pr[j], storage[j]);
        }
    }

    void checksum(double &sum) const
    { for (size_t j=0; j<storage.size(); j++) accumulate(sum, storage[j]); }
  };

  //! Fill (p,h) with a spread of states inside the liquid domain of water.
  void fill_liquid_ph(ArrOfDouble &p, ArrOfDouble &h, int n)
  { for (int i=0; i<n; i++)
       { const double t = double(i) / double(n > 1 ? n-1 : 1);
         p[i] = 5.0e5  + t * (2.0e7 - 5.0e5)  ;
         h[i] = 5.2e5  + t * (1.3e6 - 5.2e5)  ;
       }
  }

  // ------------------------------------------------------------ scenarios

  //! Steady regime : field size never changes between calls. This is the
  //! regime a running transient spends nearly all of its time in, and the
  //! one where per-call bookkeeping that could have been skipped shows up.
  Result bench_ph_steady(EOS &fluid, int n, int calls, int reps)
  { Result r;
    r.name   = "ph_steady";
    r.detail = "compute(p,h,10 fields), constant size";
    r.calls  = calls;
    r.points = n;
    r.checksum = 0.0;

    ArrOfDouble xp(n), xh(n);
    ArrOfInt    ierr(n);
    fill_liquid_ph(xp, xh, n);

    EOS_Field       P("p", "p", NEPTUNE::p, xp);
    EOS_Field       H("h", "h", NEPTUNE::h, xh);
    EOS_Error_Field err(ierr);
    PhOutputs       out(n);

    for (int w=0; w<2; w++)                       // warm up caches and lazy init
      fluid.compute(P, H, out.fields, err);

    for (int rep=0; rep<reps; rep++)
      { const double t0 = now_ms();
        for (int c=0; c<calls; c++)
          fluid.compute(P, H, out.fields, err);
        r.times_ms.push_back(now_ms() - t0);
      }
    out.checksum(r.checksum);
    return r;
  }

  //! Mesh regime : the field size alternates between calls, which forces the
  //! resize path through every working array of the plugin.
  Result bench_ph_resize(EOS &fluid, int n, int calls, int reps)
  { Result r;
    r.name   = "ph_resize";
    r.detail = "compute(p,h,10 fields), size alternating n / n-1";
    r.calls  = calls;
    r.points = n;
    r.checksum = 0.0;

    const int sizes[2] = { n, n > 1 ? n-1 : 1 };

    ArrOfDouble xp(n), xh(n);
    ArrOfInt    ierr(n);
    fill_liquid_ph(xp, xh, n);

    EOS_Field P("p", "p", NEPTUNE::p, xp);
    EOS_Field H("h", "h", NEPTUNE::h, xh);
    PhOutputs out(n);

    double *pp = (double*) xp.get_ptr();
    double *ph = (double*) xh.get_ptr();
    int    *pe = (int*)    ierr.get_ptr();

    for (int w=0; w<2; w++)
      { EOS_Error_Field err(ierr);
        fluid.compute(P, H, out.fields, err);
      }

    for (int rep=0; rep<reps; rep++)
      { const double t0 = now_ms();
        for (int c=0; c<calls; c++)
          { const int m = sizes[c & 1];
            P.reset_data_ptr(m, pp);
            H.reset_data_ptr(m, ph);
            for (int j=0; j<out.fields.size(); j++)
              out.fields[j].reset_data_ptr(m, (double*) out.storage[j].get_ptr());
            EOS_Error_Field err(m, pe);
            fluid.compute(P, H, out.fields, err);
          }
        r.times_ms.push_back(now_ms() - t0);
      }
    out.checksum(r.checksum);
    return r;
  }

  //! Saturation properties from pressure alone : a shorter path that leans
  //! harder on the per-point dispatch than on the working arrays.
  Result bench_sat_steady(EOS &fluid, int n, int calls, int reps)
  { Result r;
    r.name   = "sat_steady";
    r.detail = "compute(p,6 sat fields), constant size";
    r.calls  = calls;
    r.points = n;
    r.checksum = 0.0;

    ArrOfDouble xp(n);
    ArrOfInt    ierr(n);
    for (int i=0; i<n; i++)
      xp[i] = 5.0e5 + (double(i)/double(n > 1 ? n-1 : 1)) * (2.0e7 - 5.0e5);

    EOS_Field       P("p", "p", NEPTUNE::p, xp);
    EOS_Error_Field err(ierr);
    SatOutputs      out(n);

    for (int w=0; w<2; w++)
      fluid.compute(P, out.fields, err);

    for (int rep=0; rep<reps; rep++)
      { const double t0 = now_ms();
        for (int c=0; c<calls; c++)
          fluid.compute(P, out.fields, err);
        r.times_ms.push_back(now_ms() - t0);
      }
    out.checksum(r.checksum);
    return r;
  }

  // -------------------------------------------------------------- report

  void report(const std::vector<Result> &all, const std::string &tag, bool csv)
  { if (csv)
       { for (size_t i=0; i<all.size(); i++)
            { const Result &r = all[i];
              const double med = median(r.times_ms);
              printf("%s,%s,%d,%d,%.4f,%.4f,%.6f,%.17g\n",
                     tag.c_str(), r.name.c_str(), r.points, r.calls,
                     med, stddev(r.times_ms),
                     1e3 * med / double(r.calls), r.checksum);
            }
         return;
       }

    printf("\n  %-12s %-8s %-8s %12s %10s %14s\n",
           "scenario", "points", "calls", "median (ms)", "sd (ms)", "per call (us)");
    printf("  %s\n", std::string(68, '-').c_str());
    for (size_t i=0; i<all.size(); i++)
       { const Result &r = all[i];
         const double med = median(r.times_ms);
         printf("  %-12s %-8d %-8d %12.3f %10.3f %14.3f\n",
                r.name.c_str(), r.points, r.calls, med, stddev(r.times_ms),
                1e3 * med / double(r.calls));
       }
    printf("\n  checksums (must be identical across versions)\n");
    for (size_t i=0; i<all.size(); i++)
       printf("    %-12s %-8d  %.17g\n",
              all[i].name.c_str(), all[i].points, all[i].checksum);
  }
}

int main(int argc, char *argv[])
{ int  reps  = 7;
  int  calls = 2000;
  bool csv   = false;

  for (int i=1; i<argc; i++)
     { if      (!strcmp(argv[i], "--reps")  && i+1 < argc) reps  = atoi(argv[++i]);
       else if (!strcmp(argv[i], "--calls") && i+1 < argc) calls = atoi(argv[++i]);
       else if (!strcmp(argv[i], "--csv"))                 csv   = true;
       else
          { fprintf(stderr, "usage: %s [--reps N] [--calls N] [--csv]\n", argv[0]);
            return 1;
          }
     }

  Language_init();

  const int sizes[4] = { 10, 100, 1000, 10000 };
  std::vector<Result> all;

#ifdef WITH_PLUGIN_CATHARE2
  { EOS liq("EOS_Cathare2", "WaterLiquid");
    for (int s=0; s<4; s++)
       { const int n = sizes[s];
         const int c = std::max(1, (calls * 100) / n);   // keep the work per size comparable
         all.push_back(bench_ph_steady (liq, n, c, reps));
         all.push_back(bench_ph_resize (liq, n, c, reps));
         all.push_back(bench_sat_steady(liq, n, c, reps));
       }
  }
#else
  fprintf(stderr, "EOSBench: built without the CATHARE2 plugin, nothing to measure.\n");
  return 77;
#endif

  const char *tag = getenv("EOS_BENCH_TAG");
  report(all, tag ? tag : "run", csv);

  Language_finalize();
  return 0;
}
