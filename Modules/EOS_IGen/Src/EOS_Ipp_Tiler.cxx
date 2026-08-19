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
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <utility>
#include <cstdlib>
#include <algorithm>
#include <cmath>

namespace NEPTUNE_EOS_IGEN
{
  namespace
  {
    //! Splits "T:1e-7,rho,cp:5e-8" into (property, limit) pairs, an entry with
    //! no colon taking `fallback`. Returns false on an empty or malformed
    //! entry rather than dropping it: a criterion that quietly went missing is
    //! a database refined on less than was asked for, and nothing downstream
    //! would ever say so.
    bool parse_quality_list(const std::string &spec, double fallback,
                            std::vector<std::pair<std::string, double> > &out)
    {
      out.clear();
      if (spec.empty())  return false;
      std::size_t i = 0;
      while (true)
      {
        std::size_t j = spec.find(',', i);
        const bool last = (j == std::string::npos);
        if (last)  j = spec.size();
        const std::string item = spec.substr(i, j - i);
        if (item.empty())  return false;
        const std::size_t c = item.find(':');
        if (c == std::string::npos)
          out.push_back(std::make_pair(item, fallback));
        else
        {
          const std::string name = item.substr(0, c), lim = item.substr(c + 1);
          if (name.empty() || lim.empty())  return false;
          out.push_back(std::make_pair(name, std::atof(lim.c_str())));
        }
        if (last)  break;
        i = j + 1;
      }
      return !out.empty();
    }

    // T-range spanning the 4 corners of [p0,p1]x[h0,h1] under 'source',
    // widened by a small safety margin (EOS_IGen's own inversions/
    // interpolations near a box's edge are less accurate than at its
    // center). Returns false if none of the 4 corners could be evaluated,
    // and reports through nb_bad how many of them could not.
    //
    // nb_bad is not a detail. The database is built over a *T* box: this
    // range is what reaches set_extremum, and EOS_IGen then recomputes the
    // enthalpies from it. A corner that fails simply does not contribute, so
    // a box with corners outside the model's validity silently yields the
    // T-range of whatever corners survived -- asking for p=[1e5,2e7],
    // h=[1e5,3e6] gave back h=[84423,115664], T=[293,297]. A four-kelvin band
    // where a domain twenty-five times larger was requested, announced as
    // "wrote 1 tile(s)".
    bool corner_T_range(EOS &source, double p0, double p1, double h0, double h1,
                         double &Tmin, double &Tmax, int &nb_bad)
    {
      const double ps[2] = {p0, p1};
      const double hs[2] = {h0, h1};
      Tmin = 1e300;
      Tmax = -1e300;
      bool any_ok = false;
      nb_bad = 0;
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
          else
            nb_bad++;
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

    // EOS_IGen exposes no error handler of its own, and probing a model at
    // tile corners legitimately asks for properties it does not implement, so
    // the only way to keep the tiler's own messages readable is to send the
    // model's chatter elsewhere for the duration.
    class Muted_stderr
    {
    public:
      explicit Muted_stderr(bool active) : active_(active), saved_(nullptr)
      {
        if (!active_)
          return;
        saved_ = cerr.rdbuf();
        cerr.rdbuf(sink_.rdbuf());
      }
      ~Muted_stderr()
      {
        if (active_ && saved_ != nullptr)
          cerr.rdbuf(saved_);
      }
    private:
      bool active_;
      std::streambuf *saved_;
      std::ostringstream sink_;
    };

    // EOS_IGen::write_med() finishes by rewriting the shared index.eos of the
    // data directory, so however independent generating two tiles is, writing
    // them is not: concurrent workers otherwise corrupt that index and lose
    // each other's entries. An advisory lock on a file next to it serializes
    // just the write, leaving the meshing -- which is the expensive part --
    // fully parallel.
    class Index_lock
    {
    public:
      explicit Index_lock(bool active) : fd_(-1)
      {
        if (!active || iret_eos_data_dir)
          return;
        const std::string path = eos_data_dir + "/EOS_Ipp/.eos_ipp_tiler.lock";
        fd_ = open(path.c_str(), O_CREAT | O_RDWR, 0644);
        if (fd_ >= 0 && flock(fd_, LOCK_EX) != 0)
        {
          close(fd_);
          fd_ = -1;
        }
      }
      ~Index_lock()
      {
        if (fd_ >= 0)
        {
          flock(fd_, LOCK_UN);
          close(fd_);
        }
      }
    private:
      int fd_;
    };

    // Where a worker leaves the tiles it could not generate. Workers are
    // forked processes, so they cannot append to the parent's list; one small
    // file per job, read and removed by the parent, is enough and needs no
    // synchronisation since each job writes only its own.
    std::string failure_file(const std::string &manifest_file_name, int job)
    {
      std::ostringstream path;
      path << eos_data_dir << "/EOS_Ipp/." << manifest_file_name << ".failed." << job;
      return path.str();
    }

    // Does this tile's .med already sit in the output directory?
    bool tile_exists(const std::string &file_name)
    {
      if (iret_eos_data_dir)
        return false;
      const std::string path = eos_data_dir + "/EOS_Ipp/" + file_name;
      struct stat st;
      return stat(path.c_str(), &st) == 0;
    }

    struct WrittenTile
    {
      int ip, ih;
      double p0, p1, h0, h1;     // declared (core) cell -- not the halo-widened generation box
      double bp0, bp1, bh0, bh1; // the halo-widened box handed to EOS_IGen
      double tmin, tmax;         // (p,T) generation box actually handed to EOS_IGen
      std::string file_name;     // "{tile_basename}_{ip}_{ih}.med"
    };

    //! A grid cell with no tile, and why. The manifest records these so a
    //! partial database says what it is missing instead of looking complete.
    struct MissingTile
    {
      int ip, ih;
      std::string reason;
    };
  }

  namespace
  {
    std::string g_manifest_file_name; // for record_failure, set on entry below

    void record_failure(const EOS_Ipp_Tiler_Params &, int job, int ip, int ih,
                        const std::string &reason)
    {
      if (iret_eos_data_dir)
        return;
      std::ofstream out(failure_file(g_manifest_file_name, job).c_str(), std::ios::app);
      if (out.good())
        out << ip << " " << ih << " " << reason << "\n";
    }
  }

  EOS_Error generate_tiled_database(const EOS_Ipp_Tiler_Params &prm,
                                     const std::string &manifest_file_name)
  {
    g_manifest_file_name = manifest_file_name;
    if (prm.nb_p_tiles <= 0 || prm.nb_h_tiles <= 0 || !(prm.pmax > prm.pmin) || !(prm.hmax > prm.hmin))
    {
      cerr << "EOS_Ipp_Tiler: invalid parameters (grid size or domain)" << endl;
      return EOS_Error::error;
    }

    EOS source(prm.method.c_str(), prm.reference.c_str());
    relax(source);

    double Tmin_global, Tmax_global;
    int nb_bad_global = 0;
    if (!corner_T_range(source, prm.pmin, prm.pmax, prm.hmin, prm.hmax,
                        Tmin_global, Tmax_global, nb_bad_global))
    {
      cerr << "EOS_Ipp_Tiler: could not evaluate T(p,h) at the global domain corners" << endl;
      return EOS_Error::error;
    }
    if (nb_bad_global > 0)
    {
      // The surviving corners still give a T range, so generation would go on
      // and produce a database over a domain nobody asked for. Say what was
      // requested and what it would become, and stop -- unless the caller has
      // said that is what they want.
      cerr << "EOS_Ipp_Tiler: " << nb_bad_global << " of the 4 corners of the requested domain"
           << " p=[" << prm.pmin << "," << prm.pmax << "]"
           << " h=[" << prm.hmin << "," << prm.hmax << "]"
           << " lie outside " << prm.method << "/" << prm.reference << endl;
      cerr << "  the database would instead cover T=[" << Tmin_global << "," << Tmax_global
           << "], which is not the domain that was asked for." << endl;
      if (!prm.allow_domain_shrink)
      {
        cerr << "  refusing to write it. Narrow the requested domain, or pass"
             << " allow_domain_shrink (--allow_domain_shrink) to accept this one." << endl;
        return EOS_Error::error;
      }
      cerr << "  allow_domain_shrink is set, generating over the reduced domain." << endl;
    }
    double pcrit = 0., hcrit = 0., tcrit = 0.;
    source.get_p_crit(pcrit);
    source.get_h_crit(hcrit);
    source.get_T_crit(tcrit);

    const double dp = (prm.pmax - prm.pmin) / prm.nb_p_tiles;
    const double dh = (prm.hmax - prm.hmin) / prm.nb_h_tiles;

    const int nb_tiles = prm.nb_p_tiles * prm.nb_h_tiles;

    // Every tile's declared cell, halo-widened box and T-range, worked out up
    // front: the dry run reports them, and the workers need them without
    // having to re-derive the ordering.
    std::vector<WrittenTile> written;
    written.reserve((std::size_t)nb_tiles);
    std::vector<MissingTile> missing;

    for (int k = 0; k < nb_tiles; ++k)
    {
      const int ip = k / prm.nb_h_tiles;
      const int ih = k % prm.nb_h_tiles;

      const double p0 = prm.pmin + ip * dp;
      const double p1 = prm.pmin + (ip + 1) * dp;
      const double h0 = prm.hmin + ih * dh;
      const double h1 = prm.hmin + (ih + 1) * dh;

      const double bp0 = std::max(prm.pmin, p0 - prm.halo_fraction * dp);
      const double bp1 = std::min(prm.pmax, p1 + prm.halo_fraction * dp);
      const double bh0 = std::max(prm.hmin, h0 - prm.halo_fraction * dh);
      const double bh1 = std::min(prm.hmax, h1 + prm.halo_fraction * dh);

      // A tile the source model cannot even be evaluated at is recorded and
      // skipped rather than ending the run. It is one grid cell out of
      // nb_tiles, and the reader already copes with a missing one: the tile
      // index leaves that grid slot empty and locate() answers -1 there
      // (cf. EOS_Ipp_TileIndex). Abandoning the whole database for it threw
      // away every tile that would have generated perfectly.
      double Tmin, Tmax;
      int nb_bad_tile = 0;
      if (!corner_T_range(source, bp0, bp1, bh0, bh1, Tmin, Tmax, nb_bad_tile))
      {
        cerr << "EOS_Ipp_Tiler: tile (" << ip << "," << ih
             << ") skipped: T(p,h) cannot be evaluated at its corners" << endl;
        missing.push_back(MissingTile{ip, ih, "T(p,h) not evaluable at the tile corners"});
        continue;
      }

      WrittenTile w;
      w.ip = ip; w.ih = ih; w.p0 = p0; w.p1 = p1; w.h0 = h0; w.h1 = h1;
      w.bp0 = bp0; w.bp1 = bp1; w.bh0 = bh0; w.bh1 = bh1;
      w.tmin = Tmin; w.tmax = Tmax; // exactly the box handed to set_extremum below
      std::ostringstream name;
      name << prm.tile_basename << "_" << ip << "_" << ih;
      w.file_name = name.str() + ".med";
      written.push_back(w);
    }

    if (prm.dry_run)
    {
      cout << "eos_ipp_tiler: dry run, " << written.size() << " of " << nb_tiles
           << " tile(s) would be generated from "
           << prm.method << "/" << prm.reference << "\n";
      for (std::size_t k = 0; k < written.size(); ++k)
        cout << "  " << written[k].file_name
             << "  p=[" << written[k].p0 << "," << written[k].p1 << "]"
             << "  h=[" << written[k].h0 << "," << written[k].h1 << "]"
             << "  T=[" << written[k].tmin << "," << written[k].tmax << "]"
             << (tile_exists(written[k].file_name) ? "   (already present)" : "") << "\n";
      cout << "  mesh " << prm.nb_node_p << "x" << prm.nb_node_h
           << ", level_max " << prm.level_max << ", halo " << prm.halo_fraction << "\n";
      cout << "  nothing written (--dry_run)" << endl;
      return EOS_Error::good;
    }

    // One tile is independent of every other, so the grid is embarrassingly
    // parallel. Forked processes rather than threads: EOS_IGen and the MED
    // writer are not known to be thread-safe, and each tile writes its own
    // file, so there is nothing to share and nothing to lock.
    const int nb_jobs = (prm.nb_jobs > 1) ? std::min(prm.nb_jobs, (int)written.size()) : 1;

    // Any failure files left by a previous run would be read back as this
    // run's failures, so a resumed generation reported tiles that had since
    // been produced -- and counted them twice.
    if (!iret_eos_data_dir)
      for (int job = 0; job < nb_jobs; ++job)
        remove(failure_file(manifest_file_name, job).c_str());
    std::vector<pid_t> workers;
    for (int job = 0; job < nb_jobs; ++job)
    {
      if (nb_jobs > 1)
      {
        const pid_t pid = fork();
        if (pid < 0)
        {
          cerr << "EOS_Ipp_Tiler: fork failed" << endl;
          return EOS_Error::error;
        }
        if (pid > 0)
        {
          workers.push_back(pid);
          continue; // parent: start the next worker
        }
      }

      // Worker (or the single process when nb_jobs == 1): tiles job, job+n, ...
      EOS_Error worker_err = EOS_Error::good;
      for (int k = job; k < (int)written.size(); k += nb_jobs)
      {
        const WrittenTile &w = written[k];

        if (prm.skip_existing && tile_exists(w.file_name))
        {
          if (prm.verbose)
            cout << "eos_ipp_tiler: [" << (k + 1) << "/" << written.size() << "] " << w.file_name
                 << " already present, skipped" << endl;
          continue;
        }

        Muted_stderr mute(prm.quiet_source);
        EOS_IGen igen(prm.method.c_str(), prm.reference.c_str());
        igen.set_extremum(w.bp0, w.bp1, w.tmin, w.tmax);

        // A tile that fails is recorded and skipped, not the end of the run.
        // Its neighbours are independent of it -- each generates its own mesh
        // from the source model over its own box -- so stopping at the first
        // failure threw away every tile after it as well, on a grid that can
        // take hours. What the failures cost is a manifest: it is only written
        // when the database is complete, or when --allow_partial says a
        // holed one is wanted.
        EOS_Error err = igen.make_mesh(prm.nb_node_p, prm.nb_node_h, prm.level_max);
        if (err != EOS_Error::good)
        {
          cerr << "EOS_Ipp_Tiler: tile (" << w.ip << "," << w.ih << ") failed: make_mesh" << endl;
          record_failure(prm, job, w.ip, w.ih, "make_mesh failed");
          worker_err = err;
          continue;
        }

        { //  Several properties may drive the refinement at once. EOS_IGen keeps
          //  a list of criteria and marks a cell for subdivision when *any* of
          //  them fails there, so the mesh comes out fine enough for all of
          //  them and the order they are given in does not matter
          //  (cf. EOSIGenQILimitTest).
          //
          //  Only one was ever passed on, so a database refined on T was then
          //  read for rho and cp too, and those came out several times less
          //  accurate on that mesh for no reason but not having been asked for.
          std::vector<std::pair<std::string, double> > crit;
          if (!parse_quality_list(prm.quality_property, prm.quality_limit, crit))
          {
            record_failure(prm, job, w.ip, w.ih, "bad --quality_property list");
            worker_err = EOS_Error::error;
            continue;
          }
          for (std::size_t c = 0; c < crit.size(); c++)
            igen.set_quality(crit[c].first.c_str(), prm.quality_type.c_str(),
                             prm.quality_is_abs, crit[c].second);
        }

        // Refinement, when a threshold was asked for. compute_qualities() only
        // evaluates the criterion; refining is make_local_refine()'s job, and
        // this called only the former -- so --level_max had no effect and every
        // database this tool has produced is its unrefined base mesh. Without a
        // threshold EOS_IGen's sentinel makes the quality test pass everywhere,
        // which would refine nothing anyway, so the historical behaviour is
        // kept exactly for callers that do not ask.
        const bool refine = (prm.quality_limit > 0.) && (prm.level_max != 0);
        err = refine ? igen.make_local_refine(prm.refine_continuity)
                     : igen.compute_qualities();
        if (err != EOS_Error::good)
        {
          const char *what = refine ? "make_local_refine" : "compute_qualities";
          cerr << "EOS_Ipp_Tiler: tile (" << w.ip << "," << w.ih << ") failed: " << what << endl;
          record_failure(prm, job, w.ip, w.ih, what);
          worker_err = err;
          continue;
        }

        std::ostringstream name;
        name << prm.tile_basename << "_" << w.ip << "_" << w.ih;
        AString file_name(name.str().c_str());
        igen.set_file_med_name(file_name);
        {
          Index_lock lock(nb_jobs > 1); // write_med() rewrites the shared index.eos
          err = igen.write_med();
        }
        if (err != EOS_Error::good)
        {
          cerr << "EOS_Ipp_Tiler: tile (" << w.ip << "," << w.ih << ") failed: write_med" << endl;
          record_failure(prm, job, w.ip, w.ih, "write_med failed");
          worker_err = err;
          continue;
        }

        if (prm.verbose)
          cout << "eos_ipp_tiler: [" << (k + 1) << "/" << written.size() << "] " << w.file_name << endl;
      }

      if (nb_jobs > 1)
        _exit(worker_err == EOS_Error::good ? 0 : 1); // child: never returns
      // Single process: no early return. The failures are in the same file the
      // forked workers would have written, and the aggregation below is what
      // decides whether a manifest may be written -- returning here skipped it
      // and lost the report along with the tiles that did succeed.
      (void)worker_err;
    }

    // Parent: wait for every worker, then collect what they could not
    // generate. Each worker left its failures in its own file (cf.
    // record_failure); reading them here is what lets the manifest say which
    // grid cells have no tile instead of the run simply dying.
    for (std::size_t i = 0; i < workers.size(); ++i)
    {
      int status = 0;
      waitpid(workers[i], &status, 0);
    }
    for (int job = 0; job < nb_jobs; ++job)
    {
      const std::string path = failure_file(manifest_file_name, job);
      std::ifstream in(path.c_str());
      std::string line;
      while (std::getline(in, line))
      {
        std::istringstream ls(line);
        MissingTile m;
        if (ls >> m.ip >> m.ih)
        {
          std::getline(ls, m.reason);
          while (!m.reason.empty() && m.reason[0] == ' ')
            m.reason.erase(0, 1);
          missing.push_back(m);
        }
      }
      in.close();
      remove(path.c_str());
    }

    // Tiles that failed are holes in the grid, which the reader supports, but
    // a database is not silently allowed to be incomplete: the manifest is
    // written only when every declared tile exists, unless the caller says a
    // partial one is what it wants. Either way the tiles that did generate are
    // on disk, and --skip_existing resumes from there.
    // A tile that failed must not appear in the manifest: its .med does not
    // exist, and a TILE line naming a missing file is worse than no line at
    // all -- the reader would take the grid slot as filled and try to load it.
    {
      std::vector<WrittenTile> ok;
      ok.reserve(written.size());
      for (const WrittenTile &w : written)
      {
        bool failed = false;
        for (const MissingTile &m : missing)
          if (m.ip == w.ip && m.ih == w.ih) { failed = true; break; }
        if (!failed)
          ok.push_back(w);
      }
      written.swap(ok);
    }

    if (!missing.empty())
    {
      cerr << "EOS_Ipp_Tiler: " << missing.size() << " of " << nb_tiles
           << " tile(s) could not be generated:" << endl;
      for (const MissingTile &m : missing)
        cerr << "  (" << m.ip << "," << m.ih << ") " << m.reason << endl;
      if (!prm.allow_partial)
      {
        cerr << "EOS_Ipp_Tiler: no manifest written; the " << written.size()
             << " tile(s) that succeeded are on disk, so --skip_existing resumes,"
             << " and --allow_partial writes a manifest for them as they are" << endl;
        return EOS_Error::error;
      }
      cerr << "EOS_Ipp_Tiler: --allow_partial: writing a manifest for the "
           << written.size() << " tile(s) that succeeded" << endl;
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
    out << "VERSION 2\n";
    // Provenance: what this database was generated from and how. The loading
    // side ignores all of it, but without it a .eosmm and its pile of .med
    // files say nothing about which model, resolution or halo produced them,
    // and there is no way to tell a stale database from a current one.
    out << "SOURCE " << prm.method << " " << prm.reference << "\n";
    out << "MESH " << prm.nb_node_p << " " << prm.nb_node_h << " " << prm.level_max << " "
        << prm.halo_fraction << "\n";
    out << "QUALITY " << prm.quality_property << " " << prm.quality_type << " "
        << prm.quality_is_abs << " " << prm.quality_limit << "\n";
    out << "GLOBAL " << prm.pmin << " " << prm.pmax << " " << prm.hmin << " " << prm.hmax << " "
        << Tmin_global << " " << Tmax_global << " " << pcrit << " " << hcrit << " " << tcrit << "\n";
    out << "GRID " << prm.nb_p_tiles << " " << prm.nb_h_tiles << "\n";
    // The per-tile (tmin,tmax) is the (p,T) box the tile's mesh was generated
    // over, so a reader can tell -- without opening the .med -- that a tile
    // cannot hold the root of T(p,h) = T (cf. EOS_Ipp_TileIndex).
    // A grid cell with no tile. The loader ignores unknown keywords, so this
    // is readable by older readers too; it is here so a partial database
    // carries the record of what it is missing and why.
    for (const MissingTile &m : missing)
      out << "MISSING " << m.ip << " " << m.ih << " " << m.reason << "\n";

    for (const WrittenTile &w : written)
      out << "TILE " << w.ip << " " << w.ih << " "
          << w.p0 << " " << w.p1 << " " << w.h0 << " " << w.h1 << " "
          << w.tmin << " " << w.tmax << " " << w.file_name << "\n";

    if (!out.good())
    {
      cerr << "EOS_Ipp_Tiler: error writing manifest file " << manifest_path << endl;
      return EOS_Error::error;
    }
    return EOS_Error::good;
  }
}
