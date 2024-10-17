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

#include "EOS_Refprop10.hxx"
#include "EOS/API/EOS_Utils.hxx"

#include <string.h>
#ifdef _OPENMP
#include <omp.h>
#endif

namespace NEPTUNE_EOS
{

  // Number of the first internal error for EOS_Fluid object:
  static const int EOS_Fluid_Error_base = 1;
  // Definition of internal errors in EOS_Fluid:
  // The following errors leads to EOS_Error=bad
  static const EOS_Internal_Error Error_p_sat_T_compute = EOS_Internal_Error(EOS_Fluid_Error_base + 0, EOS_Error::bad);
  static const EOS_Internal_Error Error_p_sat_T_newton = EOS_Internal_Error(EOS_Fluid_Error_base + 1, EOS_Error::bad);
  static const EOS_Internal_Error Error_lambda_value = EOS_Internal_Error(EOS_Fluid_Error_base + 2, EOS_Error::bad);

  const AString EOS_Refprop10::TMStr("Refprop10");
  const AString EOS_Refprop10::MVStr("10.0");
  const AString EOS_Refprop10::PhaseStr("");

  EOS_Refprop10::EOS_Refprop10() : nbcomp(1), xphase(1),
                                   hrf("DEF"), setmod(0), setref(0), fluid(1),
                                   iline(0), errcode(100), wmm(0), arr_hname(1), arr_hn80(1), arr_hcas(1)
  {
    varr_molfrac.assign(nc_max_10, 0.e0);
    arr_molfrac = &varr_molfrac[0];
  }

  static RegisteredClass &EOS_Refprop10_create()
  {
    return *(new EOS_Refprop10());
  }

  int EOS_Refprop10::type_Id = (RegisterType("EOS_Refprop10", "EOS_Fluid",
                                             "Equation Of States for Refprop10",
                                             sizeof(EOS_Refprop10),
                                             EOS_Refprop10_create));

  const AString &EOS_Refprop10::table_name() const
  {
    return TMStr;
  }

  const AString &EOS_Refprop10::version_name() const
  {
    return MVStr;
  }

  const AString &EOS_Refprop10::fluid_name() const
  {
    return FluidStr;
  }

  const AString &EOS_Refprop10::equation_name() const
  {
    return EosStr;
  }

  const AString &EOS_Refprop10::phase_name() const
  {
    return PhaseStr;
  }

  ostream &EOS_Refprop10::print_On(ostream &stream) const
  {
    return stream;
  }

  istream &EOS_Refprop10::read_On(istream &stream)
  {
    return stream;
  }

  const Type_Info &EOS_Refprop10::get_Type_Info() const
  {
    return (Types_Info::instance())[type_Id];
  }

  EOS_Refprop10::
      ~EOS_Refprop10()
  {
  }

  int EOS_Refprop10::init(const Strings &strings)
  {
    EOS_Internal_Error err;
    AString desc_err;
    /*
        // test refprop_key.txt for NIST-Refprop
        AString nfic = EOS_INSTALL_DIR;
        nfic += "/lib/refprop_key.txt";
        std::ifstream testf(nfic.aschar(),std::ios::in);
        if (!testf)
           { cerr << " EOS_Refprop10::init : NIST-Refprop-10 is not available, call EOS Administrator" << endl;
             exit(-1);
             return EOS_Error::error;
           }
    */
    // test strings
    int sz_strings = strings.size();
    if (sz_strings != 2)
    {
      cerr << " EOS_Refprop10::init(strings) : Reference with size " << sz_strings
           << " is not implemented for EOS_REFPROP10 ! " << endl;
      assert(0);
      exit(-1);
      return EOS_Error::error;
    }

    // fluide name : FluidStr
    char *pname1 = strchr(strings[0].aschar(), '/');
    char *pname2 = strrchr(strings[0].aschar(), '.');
    int lv_chfld = int(pname2 - pname1 - 1);

    int lm_chfld = strlen(strings[0].aschar());
    char *chfld = new char[lm_chfld + 1];
    strncpy(chfld, pname1 + 1, lv_chfld);
    chfld[lv_chfld] = '\0';

    FluidStr = AString(chfld);

    delete[] chfld;

    // equation of state name : EosStr
    EosStr = FluidStr;
    EosStr += phase_name();

    // fluide type
    if (strings[1] == "mixture")
      fluid = 2;
    else if (strings[1] == "ppf")
      fluid = 3;

    set_allpath(strings);

    err = callSetupInitial();
    if (err.generic_error() != EOS_Error::good)
    {
      describe_error(err, desc_err);
      cerr << desc_err << endl;
      // exit(-1);
      throw std::runtime_error("callSetup failure 1");
      return err.generic_error();
    }

    err = init_allparams();
    if (err.generic_error() != EOS_Error::good)
    {
      describe_error(err, desc_err);
      cerr << desc_err << endl;
      // exit(-1);
      throw std::runtime_error("init_allparams failure 1");
      return err.generic_error();
    }

    return EOS_Error::good;
  }

  int EOS_Refprop10::init(const Strings &strings, const Strings &values)
  {
    EOS_Internal_Error err;
    AString desc_err;
    /*
        // test refprop_key.txt for NIST-Refprop
        AString nfic = EOS_INSTALL_DIR;
        nfic += "/lib/refprop_key.txt";
        std::ifstream testf(nfic.aschar(),std::ios::in);
        if (!testf)
        {
           cerr << " EOS_Refprop10::init : NIST-Refprop10 is not available, call EOS Administrator" << endl;
           exit(-1);
           return EOS_Error::error;
       }
    */
    // test strings
    int sz_strings = strings.size();
    if (sz_strings != 2)
    {
      cerr << " EOS_Refprop10::init(strings,values) : Reference with size " << sz_strings
           << " is not implemented for EOS_REFPROP10 ! " << endl;
      assert(0);
      exit(-1);
      return EOS_Error::error;
    }

    // fluide name : FluidStr
    char *pname1 = strchr(strings[0].aschar(), '/');
    char *pname2 = strrchr(strings[0].aschar(), '.');
    int lv_chfld = int(pname2 - pname1 - 1);

    int lm_chfld = strlen(strings[0].aschar());
    char *chfld = new char[lm_chfld + 1];
    strncpy(chfld, pname1 + 1, lv_chfld);
    chfld[lv_chfld] = '\0';

    FluidStr = AString(chfld);

    delete[] chfld;

    // equation of state name : EosStr
    EosStr = FluidStr;
    EosStr += phase_name();

    // fluide type
    if (strings[1] == "mixture")
      fluid = 2;
    else if (strings[1] == "ppf")
      fluid = 3;

    // test values
    int sz_values = values.size();
    if (values[0] == "SETMOD")
    {
      if (sz_values < 4)
      {
        cerr << "Fluid '" << FluidStr << "'  type  '" << strings[1] << "'   "
             << "at least 4 parameters : \"SETMOD\", htype, hmix, hcomp[i] i=1,nbcomp" << endl;
        assert(0);
        exit(-1);
        return EOS_Error::error;
      }
      else
      {
        setmod = 1;
        htype = values[1];
        hmix = values[2];
        int nbc = sz_values - 3;
        hcomp.resize(nbc);
        for (int i = 0; i < nbc; i++)
          hcomp[i] = values[i + 3];
      }
    }
    else if (values[0] == "SETREF")
    {
      if (sz_values != 2)
      {
        cerr << "Fluid '" << FluidStr << "'  type  '" << strings[1] << "'   "
             << "2 parameters : \"SETREF\", hrf" << endl;
        assert(0);
        exit(-1);
        return EOS_Error::error;
      }
      else
        hrf = values[1];
    }
    else
    {
      cerr << "Fluid '" << FluidStr << "'  type  '" << strings[1] << "'   "
           << " EOS_Refprop10::init : value name " << values[0]
           << " is not implemented for EOS_REFPROP10 ! " << endl;
      assert(0);
      exit(-1);
      return EOS_Error::error;
    }

    set_allpath(strings);

    err = callSetupInitial();
    if (err.generic_error() != EOS_Error::good)
    {
      describe_error(err, desc_err);
      cerr << desc_err << endl;
      // exit(-1);
      throw std::runtime_error("callSetup failure 2");
      return err.generic_error();
    }

    err = init_allparams();
    if (err.generic_error() != EOS_Error::good)
    {
      describe_error(err, desc_err);
      cerr << desc_err << endl;
      // exit(-1);
      throw std::runtime_error("init_allparams failure 2");
      return err.generic_error();
    }

    return EOS_Error::good;
  }

  int EOS_Refprop10::init(const Strings &strings, const Strings &values,
                          const double href0, const double sref0, const double tref0, const double pref0)
  {
    EOS_Internal_Error err;
    AString desc_err;
    /*
        // test refprop_key.txt for NIST-Refprop
        AString nfic = EOS_INSTALL_DIR;
        nfic += "/lib/refprop_key.txt";
        std::ifstream testf(nfic.aschar(),std::ios::in);
        if (!testf)
        {
           cerr << " EOS_Refprop10::init : NIST-Refprop10 is not available, call EOS Administrator" << endl;
           exit(-1);
           return EOS_Error::error;
        }
    */
    // test strings
    int sz_strings = strings.size();
    if (sz_strings != 2)
    {
      cerr << " EOS_Refprop10::init(strings,values) : Reference with size " << sz_strings
           << " is not implemented for EOS_REFPROP10 ! " << endl;
      assert(0);
      exit(-1);
      return EOS_Error::error;
    }

    // fluide name : FluidStr
    char *pname1 = strchr(strings[0].aschar(), '/');
    char *pname2 = strrchr(strings[0].aschar(), '.');
    int lv_chfld = int(pname2 - pname1 - 1);

    int lm_chfld = strlen(strings[0].aschar());
    char *chfld = new char[lm_chfld + 1];
    strncpy(chfld, pname1 + 1, lv_chfld);
    chfld[lv_chfld] = '\0';

    FluidStr = AString(chfld);
    delete[] chfld;

    // equation of state name : EosStr
    EosStr = FluidStr;
    EosStr += phase_name();

    // fluide type
    if (strings[1] == "mixture")
      fluid = 2;
    else if (strings[1] == "ppf")
      fluid = 3;

    // test values
    int sz_values = values.size();
    if (values[0] == "SETMOD")
    {
      if (sz_values < 4)
      {
        cerr << "Fluid '" << FluidStr << "'  type  '" << strings[1] << "'   "
             << "at least 4 parameters : \"SETMOD\", htype, hmix, hcomp[i] i=1,nbcomp" << endl;
        assert(0);
        exit(-1);
        return EOS_Error::error;
      }
      else
      {
        setmod = 1;
        htype = values[1];
        hmix = values[2];
        int nbc = sz_values - 3;
        hcomp.resize(nbc);
        for (int i = 0; i < nbc; i++)
          hcomp[i] = values[i + 3];
      }
    }
    else if (values[0] == "SETREF")
    {
      if (sz_values != 2)
      {
        cerr << "Fluid '" << FluidStr << "'  type  '" << strings[1] << "'   "
             << "2 parameters : \"SETREF\", hrf" << endl;
        assert(0);
        exit(-1);
        return EOS_Error::error;
      }
      else
      {
        hrf = values[1];
        if (hrf != "OTH" && hrf != "OT0")
        {
          cerr << "Fluid '" << FluidStr << "'  type  '" << strings[1] << "'   "
               << "hrf must be \"OTH\" or \"OT0\"" << endl;
          assert(0);
          exit(-1);
          return EOS_Error::error;
        }
        else
        {
          h0 = href0;
          s0 = sref0;
          t0 = tref0;
          p0 = pref0;
          setref = 1;
        }
      }
    }
    else
    {
      cerr << "Fluid '" << FluidStr << "'  type  '" << strings[1] << "'   "
           << " EOS_Refprop10::init : value name " << values[0]
           << " is not implemented for EOS_REFPROP10 ! " << endl;
      assert(0);
      exit(-1);
      return EOS_Error::error;
    }

    set_allpath(strings);

    err = callSetupInitial();

    if (err.generic_error() != EOS_Error::good)
    {
      describe_error(err, desc_err);
      cerr << desc_err << endl;
      // exit(-1);
      throw std::runtime_error("callSetup failure 2");
      return err.generic_error();
    }

    err = init_allparams();
    if (err.generic_error() != EOS_Error::good)
    {
      describe_error(err, desc_err);
      cerr << desc_err << endl;
      // exit(-1);
      throw std::runtime_error("init_allparams failure 2");
      return err.generic_error();
    }

    return EOS_Error::good;
  }

  void EOS_Refprop10::set_allpath(const Strings &strings)
  { // Verify $NEPTUNE_EOS_REFPROP system variable.
    // And get all paths

    AString ref_path;
    if (iret_eos_data_dir)
      exit(1);
    ref_path = eos_data_dir.c_str();
    ref_path += "/EOS_Refprop10/";

    std::ifstream ref_path_as(ref_path.aschar());
    if (!ref_path_as)
    {
      cout << "refprop_path not found " << ref_path << endl;
      exit(-1);
    }

    // fluid file_name
    data_file_name = ref_path;
    AString &file_name = strings[0];
    data_file_name += file_name;

    // mixing coefficients file name
    hfmix = ref_path;
    hfmix += file_mix_10;

    // set path
    char rpath_s[HC255];
    memset(rpath_s, '\0', sizeof(rpath_s)); // M.F.A
    strncpy(rpath_s, ref_path.aschar(), strlen(ref_path.aschar()));
    F77NAME(setpath_rp10)
    (rpath_s, sizeof(rpath_s));
  }

  EOS_Internal_Error EOS_Refprop10::callSetup() const
  {
    int ierr;
    char herr[HC255];
    memset(herr, '\0', sizeof(herr));

    if (fluid == 1) // pure fluid
    {
      if (setmod)
      {
        char htype_s[HC3];
        memset(htype_s, '\0', sizeof(htype_s)); // M.F.A.
        strncpy(htype_s, htype.aschar(), strlen(htype.aschar()));

        char hmix_s[HC3];
        memset(hmix_s, '\0', sizeof(hmix_s)); // M.F.A.
        strncpy(hmix_s, hmix.aschar(), strlen(hmix.aschar()));

        long nT;
        char *hcomp_s = packStrings(hcomp, HC3 - 1, nT);
        F77NAME(xsetmod_rp10)
        (nbcomp, htype_s, hmix_s, hcomp_s, ierr, herr,
         sizeof(htype_s) - 1, sizeof(hmix_s) - 1, nT, sizeof(herr) - 1);
        delete[] hcomp_s;
        if (ierr != 0)
          return generate_error(ierr, herr);
      }

      char hfiles_s[NBCOMPMAX * HC255];
      memset(hfiles_s, '\0', sizeof(hfiles_s)); // M.F.A.
      strncpy(hfiles_s, data_file_name.aschar(), strlen(data_file_name.aschar()));

      char hfmix_s[HC255];
      memset(hfmix_s, '\0', sizeof(hfmix_s)); // M.F.A.
      strncpy(hfmix_s, hfmix.aschar(), strlen(hfmix.aschar()));

      char hrf_s[HC3];
      memset(hrf_s, '\0', sizeof(hrf_s)); // M.F.A.
      strncpy(hrf_s, hrf.aschar(), strlen(hrf.aschar()));

      F77NAME(xsetup_rp10)
      (&nbcomp, hfiles_s, hfmix_s, hrf_s, &ierr, herr,
       sizeof(hfiles_s) - 1, sizeof(hfmix_s) - 1, sizeof(hrf_s) - 1, sizeof(herr) - 1);

      if (setref)
      {
        int ixflag = 1;
        strncpy(hrf_s, hrf.aschar(), strlen(hrf.aschar()));
        if (wmm == 0)
          varr_molfrac[0] = 1.e0;
        double wmm0 = F77NAME(wmol_rp10)(arr_molfrac);
        double h0_rp = h0 / ((1e3 * 1.e0) / wmm0);
        double s0_rp = s0 / ((1e3 * 1.e0) / wmm0);
        double t0_rp = t0;
        double p0_rp = pa2kpa(p0);
        F77NAME(setref_rp10)
        (hrf_s, ixflag, arr_molfrac, h0_rp, s0_rp, t0_rp, p0_rp, ierr, herr,
         sizeof(hrf_s) - 1, sizeof(herr) - 1);
        if (ierr != 0)
          return generate_error(ierr, herr);
      }

      if (ierr != 0)
        return generate_error(ierr, herr);

      if (wmm == 0)
        varr_molfrac[0] = 1.e0;
    }

    else if (fluid == 2) // mixing fluids
    {
      char hmx_s[HC255];
      memset(hmx_s, '\0', sizeof(hmx_s)); // M.F.A.
      strncpy(hmx_s, data_file_name.aschar(), strlen(data_file_name.aschar()));

      char hfmix_s[HC255];
      memset(hfmix_s, '\0', sizeof(hfmix_s)); // M.F.A.
      strncpy(hfmix_s, hfmix.aschar(), strlen(hfmix.aschar()));

      char hrf_s[HC3];
      memset(hrf_s, '\0', sizeof(hrf_s)); // M.F.A.
      strncpy(hrf_s, hrf.aschar(), strlen(hrf.aschar()));

      char hfiles_s[NBCOMPMAX * HC255];
      memset(hfiles_s, '\0', sizeof(hfiles_s)); // M.F.A.

      F77NAME(xsetmix_rp10)
      (hmx_s, hfmix_s, hrf_s, nbcomp, hfiles_s, arr_molfrac, ierr, herr,
       sizeof(hmx_s) - 1, sizeof(hfmix_s) - 1, sizeof(hrf_s) - 1, sizeof(hfiles_s) - 1, sizeof(herr) - 1);

      // ierr = -117 : No mixture data are available for one or more binary pairs in
      //               the specified mixture. The mixing parameters have been estimated
      if (ierr != 0 && ierr != -117)
        return generate_error(ierr, herr);

      if (setmod)
      {
        char htype_s[HC3];
        memset(htype_s, '\0', sizeof(htype_s)); // M.F.A.
        strncpy(htype_s, htype.aschar(), strlen(htype.aschar()));
        char hmix_s[HC3];
        memset(hmix_s, '\0', sizeof(hmix_s)); // M.F.A.
        strncpy(hmix_s, hmix.aschar(), strlen(hmix.aschar()));

        long nT;
        char *hcomp_s = packStrings(hcomp, HC3 - 1, nT);
        F77NAME(xsetmod_rp10)
        (nbcomp, htype_s, hmix_s, hcomp_s, ierr, herr,
         sizeof(htype_s) - 1, sizeof(hmix_s) - 1, nT, sizeof(herr) - 1);
        delete[] hcomp_s;
        if (ierr != 0)
          return generate_error(ierr, herr);
      }
    }

    return EOS_Internal_Error::OK;
  }

  EOS_Internal_Error EOS_Refprop10::callSetupInitial() const
  {

    EOS_Internal_Error err;
    err = callSetup();

#ifdef _OPENMP

    // Retrieve fortran commons location and copy
    // values in the main thread's commons to the other
    // threads commons

    int numCommons;
    F77NAME(xnumcommons_rp10)
    (&numCommons);
    int iC, iTh, nTh = omp_get_max_threads();
    long *Q0 = new long[numCommons * nTh];
    long *L = new long[numCommons];

    F77NAME(xdefcommons_rp10)
    (Q0, L, &numCommons, &nTh);

    for (iC = 0; iC < numCommons; iC++)
    {
      void *p0 = reinterpret_cast<void *>(Q0[iC * nTh]);
      for (iTh = 1; iTh < nTh; iTh++)
      {
        void *q0 = reinterpret_cast<void *>(Q0[iTh + iC * nTh]);
        memcpy(q0, p0, L[iC]);
      }
    }
    delete[] L;
    delete[] Q0;
#endif

    return err;
  }

  EOS_Internal_Error EOS_Refprop10::init_allparams()
  {
    EOS_Internal_Error err;

    // arr_hname, arr_hn80, arr_hcas
    if (nbcomp > 1)
    {
      arr_hname.resize(nbcomp);
      arr_hn80.resize(nbcomp);
      arr_hcas.resize(nbcomp);
    }
    char hname[HC12];
    char hn80[HC80];
    char hcas[HC12];
    memset(hname, '\0', sizeof(hname));
    memset(hn80, '\0', sizeof(hn80));
    memset(hcas, '\0', sizeof(hcas));
    int i1;
    for (int i = 0; i < nbcomp; i++)
    {
      i1 = i + 1;
      F77NAME(name_rp10)
      (i1, hname, hn80, hcas, HC12 - 1, HC80 - 1, HC12 - 1);
      arr_hname[i] = AString(hname);
      arr_hn80[i] = AString(hn80);
      arr_hcas[i] = AString(hcas);
    }

    wmm = F77NAME(wmol_rp10)(arr_molfrac);

    init_limits();

    err = critical_point(tcrit, pcrit, rhocrit, hcrit, scrit, ucrit,
                         pcrit_r, rhocrit_r, hcrit_r, scrit_r, ucrit_r);
    return err;
  }

  void EOS_Refprop10::init_limits()
  {
    double dmax, p_tmp, h_tmp;
    char htyp[] = "EOS";
    F77NAME(limits_rp10)
    (htyp, arr_molfrac, tmin, tmax, dmax, p_tmp);
    rhomax = refprop_rho_2_eos(dmax);
    pmax = kpa2pa(p_tmp);
    // compute hmax
    F77NAME(enthal_rp10)
    (tmax, dmax, arr_molfrac, h_tmp);
    hmax = refprop_nrj_2_eos(h_tmp);
    pmin = 1000; // patch to get pmin and hmin -> not given by REFPROP
    hmin = -76319.7;
  }

  EOS_Internal_Error EOS_Refprop10::call_tpflsh(EOS_thermprop prop,
                                                double p, double T, double &value) const
  {
    int ierr;
    char herr[HC255];
    memset(herr, '\0', sizeof(herr));

    EOS_Internal_Error err;
    double rhol, rhov, q, e, h, s, cv, cp, w;
    double *x = new double[nbcomp];
    double *y = new double[nbcomp];

    double p_refprop = pa2kpa(p);
    double rho = 0.e0;
    ierr = 0;
    value = 0.e0;

    //... reset fluid with setup
    err = callSetup();
    if (err.generic_error() != EOS_Error::good)
      return err;

    F77NAME(tpflsh_rp10)
    (T, p_refprop, arr_molfrac, rho, rhol, rhov,
     x, y, q, e, h, s, cv, cp, w, ierr, herr, sizeof(herr) - 1);

    delete[] x;
    delete[] y;
    if (ierr != 0)
      return generate_error(ierr, herr);

    switch (prop)
    {
    case NEPTUNE::h:
      value = refprop_nrj_2_eos(h);
      break;
    case NEPTUNE::rho:
      value = refprop_rho_2_eos(rho);
      break;
    case NEPTUNE::u:
      value = refprop_nrj_2_eos(e);
      break;
    case NEPTUNE::s:
      value = refprop_nrj_2_eos(s);
      break;
    case NEPTUNE::cp:
      value = refprop_nrj_2_eos(cp);
      break;
    case NEPTUNE::w:
      value = w;
      break;
    default:
      return EOS_Internal_Error::NOT_IMPLEMENTED;
      break;
    }

    return EOS_Internal_Error::OK;
  }

  EOS_Internal_Error EOS_Refprop10::call_psflsh(EOS_thermprop prop,
                                                double p, double s, double &value) const
  {
    EOS_Internal_Error err;
    double rho, rhol, rhov, q, e, h, T, cv, cp, w;
    double *x = new double[nbcomp];
    double *y = new double[nbcomp];

    double p_refprop = pa2kpa(p);
    double s_refprop = eos_nrj_2_refprop(s);
    int ierr;
    char herr[HC255];
    memset(herr, '\0', sizeof(herr));

    ierr = 0;
    value = 0.e0;

    //... reset fluid with setup
    err = callSetup();
    if (err.generic_error() != EOS_Error::good)
      return err;

    F77NAME(psflsh_rp10)
    (p_refprop, s_refprop, arr_molfrac, T, rho, rhol, rhov, x, y, q, e, h, cv, cp, w,
     ierr, herr, sizeof(herr) - 1);

    delete[] x;
    delete[] y;
    if (ierr != 0)
      return generate_error(ierr, herr);

    switch (prop)
    {
    case NEPTUNE::h:
      value = refprop_nrj_2_eos(h);
      break;
    default:
      return EOS_Internal_Error::NOT_IMPLEMENTED;
      break;
    }

    return EOS_Internal_Error::OK;
  }

  EOS_Internal_Error EOS_Refprop10::call_phflsh(EOS_thermprop prop,
                                                double p, double h, double &value) const
  {
    EOS_Internal_Error err;
    double t, rho, rhol, rhov, q, e, s, cv, cp, w;
    double *xl = new double[nbcomp];
    double *xv = new double[nbcomp];

    double p_refprop = pa2kpa(p);
    double h_refprop = eos_nrj_2_refprop(h);
    int ierr;
    char herr[HC255];
    memset(herr, '\0', sizeof(herr));

    ierr = 0;
    value = 0.e0;

    //... reset fluid with setup
    err = callSetup();
    if (err.generic_error() != EOS_Error::good)
      return err;

    F77NAME(phflsh_rp10)
    (p_refprop, h_refprop, arr_molfrac, t, rho, rhol, rhov, xl, xv, q, e, s, cv, cp, w,
     ierr, herr, sizeof(herr) - 1);

    delete[] xl;
    delete[] xv;
    if (ierr != 0)
      return generate_error(ierr, herr);

    switch (prop)
    {
    case NEPTUNE::T:
      value = t;
      break;
    case NEPTUNE::rho:
      value = refprop_rho_2_eos(rho);
      break;
    case NEPTUNE::u:
      value = refprop_nrj_2_eos(e);
      break;
    case NEPTUNE::s:
      value = refprop_nrj_2_eos(s);
      break;
    case NEPTUNE::cp:
      value = refprop_nrj_2_eos(cp);
      break;
    case NEPTUNE::cv:
      value = refprop_nrj_2_eos(cv);
      break;
    case NEPTUNE::w:
      value = w;
      break;
    default:
      return EOS_Internal_Error::NOT_IMPLEMENTED;
      break;
    }

    return EOS_Internal_Error::OK;
  }

  EOS_Internal_Error EOS_Refprop10::call_dhd1(EOS_Property prop,
                                              double p, double h, double &value) const
  {
    EOS_Internal_Error err;
    double t, rho, dhdt_d, dhdt_p, dhdd_t, dhdd_p, dhdp_t, dhdp_d;

    //... reset fluid with setup
    err = callSetup();
    if (err.generic_error() != EOS_Error::good)
      return err;

    err = compute_rho_ph(p, h, rho);
    if (err.generic_error() == EOS_Error::bad)
      return err;

    err = compute_T_ph(p, h, t);
    if (err.generic_error() == EOS_Error::bad)
      return err;

    rho = eos_rho_2_refprop(rho);

    F77NAME(dhd1_rp10)
    (t, rho, arr_molfrac, dhdt_d, dhdt_p, dhdd_t, dhdd_p, dhdp_t, dhdp_d);

    switch (prop)
    {
    case NEPTUNE::d_T_d_h_p:
      value = 1 / refprop_nrj_2_eos(dhdt_p);
      break;
    case NEPTUNE::d_T_d_p_h:
      // dtdp_h * dpdh_t * dhdt_p = -1 ==> dtdp_h = -1/(dpdh_t * dhdt_p) avec dpdh_t = 1/dhdp_t
      value = -1 / (refprop_nrj_2_eos(dhdt_p) * (1 / (refprop_nrj_2_eos(dhdp_t) / kpa2pa(1))));
      break;
    default:
      return EOS_Internal_Error::NOT_IMPLEMENTED;
      break;
    }

    return EOS_Internal_Error::OK;
  }

  EOS_Internal_Error EOS_Refprop10::call_ag(EOS_thermprop prop,
                                            double t, double rho, double &value) const
  {
    EOS_Internal_Error err;
    double f, g;
    rho = eos_rho_2_refprop(rho);

    //... reset fluid with setup
    err = callSetup();
    if (err.generic_error() != EOS_Error::good)
      return err;

    F77NAME(ag_rp10)
    (t, rho, arr_molfrac, f, g);

    switch (prop)
    {
    case NEPTUNE::g:
      value = refprop_nrj_2_eos(g);
      break;
    case NEPTUNE::f:
      value = refprop_nrj_2_eos(f);
      break;
    default:
      return EOS_Internal_Error::NOT_IMPLEMENTED;
      break;
    }

    return EOS_Internal_Error::OK;
  }

  EOS_Internal_Error EOS_Refprop10::critical_point(double &tcrit, double &pcrit, double &rhocrit,
                                                   double &hcrit, double &scrit, double &ucrit,
                                                   double &pcrit_r, double &rhocrit_r, double &hcrit_r,
                                                   double &scrit_r, double &ucrit_r) const
  {
    int ierr;
    char herr[HC255];
    memset(herr, '\0', sizeof(herr));

    F77NAME(critp_rp10)
    (arr_molfrac, tcrit, pcrit_r, rhocrit_r, ierr, herr, sizeof(herr) - 1);

    if (ierr != 0 && ierr != -319)
      return generate_error(ierr, herr);

    F77NAME(enthal_rp10)
    (tcrit, rhocrit_r, arr_molfrac, hcrit_r);
    F77NAME(entro_rp10)
    (tcrit, rhocrit_r, arr_molfrac, scrit_r);
    F77NAME(energy_rp10)
    (tcrit, rhocrit_r, arr_molfrac, ucrit_r);

    pcrit = kpa2pa(pcrit_r);
    rhocrit = refprop_rho_2_eos(rhocrit_r);
    hcrit = refprop_nrj_2_eos(hcrit_r);
    scrit = refprop_nrj_2_eos(scrit_r);
    ucrit = refprop_nrj_2_eos(ucrit_r);

    return EOS_Internal_Error::OK;
  }

  EOS_Internal_Error EOS_Refprop10::generate_error(int ierr, const char *err) const
  {
    int EOS_REFPROP_CODE = 0;

#ifdef _OPENMP
#pragma omp critical
    {
#endif
      string serr = err;
      std::size_t found = serr.find_last_not_of(" ");
      if (found != string::npos)
        serr.erase(found + 1);
      else
        serr.clear();

      // Check  err  is or isn't in  errorMsgs
      bool find_err = 0;
      int lsize = errorMsgs.size();
      string strmsg;
      int subcod = 0;
      string submsg;
      string word = "";
      string word1 = "";
      string word2 = "";
      int numm;

      for (int i = 0; i < lsize; i++)
      {
        strmsg = errorMsgs[i];

        // sub code, sub message : subcod, submsg
        istringstream desc_lign(strmsg);
        numm = 0;
        while (desc_lign.good())
        {
          numm++;
          desc_lign >> word;
          if (numm == 1)
            word1 = word;
          else if (numm == 2)
          {
            word2 = word;
            break;
          }
        }

        if (numm == 1 && word1 == "")
        {
          subcod = -1000;
          submsg = "No message at all";
        }
        else if (word2 == "")
        {
          subcod = atoi(word1.c_str());
          submsg = "No message";
        }
        else
        {
          subcod = atoi(word1.c_str());
          submsg = strmsg;
          submsg.replace(strmsg.find_first_of(word1), word1.length(), "");
          submsg = submsg.substr(submsg.find_first_of(word2), submsg.length());
        }

        // matching string
        if (serr == submsg)
        {
          find_err = 1;
          EOS_REFPROP_CODE = subcod;
          break;
        }
      }

      // new error  to add to errorMsgs
      string current_err;
      if (!find_err)
      {
        EOS_REFPROP_CODE = errcode;

        // current_err = static_cast<ostringstream*>( &(ostringstream() << EOS_REFPROP_CODE) )->str();
        ostringstream pstream;
        pstream << EOS_REFPROP_CODE;
        current_err = pstream.str();
        current_err += " ";
        current_err += serr;

        iline++;
        if (iline == err_max_10)
          iline = 0;
        if (lsize < err_max_10)
          errorMsgs.push_back(current_err);
        else
          errorMsgs[iline] = current_err;

        errcode++;
        if (errcode == errcode_max_10)
          errcode = 100;
      }
#ifdef _OPENMP
    }
#endif
    // return EOS_Internal_Error
    if (ierr < 0)
      return EOS_Internal_Error(EOS_REFPROP_CODE, EOS_Error::ok, ierr);
    else
      return EOS_Internal_Error(EOS_REFPROP_CODE, EOS_Error::bad, ierr);
  }

  void EOS_Refprop10::describe_error(const EOS_Internal_Error error, AString &description) const
  {
    //
    // Search error message implemented in errorMsgs
    int EOS_REFPROP_CODE = 0;
    bool find_err = 0;
    int lsize = errorMsgs.size();
    for (int i = 0; i < lsize; i++)
    {
      string strmsg = errorMsgs[i];

      // sub code : EOS_REFPROP_CODE
      char *pt = (char *)strmsg.c_str();
      char *save_pt;
      EOS_REFPROP_CODE = atoi(strtok_r(pt, " ", &save_pt));

      // test  get_partial_code
      if (EOS_REFPROP_CODE == error.get_partial_code())
      {
        find_err = 1;
        description = errorMsgs[i].c_str();
        break;
      }
    }

    //
    // Error message not implemented in errorMsgs : call EOS_Fluid
    if (!find_err)
      EOS_Fluid::describe_error(error, description);
  }

  EOS_Internal_Error EOS_Refprop10::newton_hlim(EOS_splimprop prop, double p, double &h_lim) const
  {
    double p_ref, tsat_ref;
    double rholsat_ref, rhovsat_ref;
    double rho_sat;
    double tinit_ref, t_ref, tp_ref, tm_ref;
    double rhol_ref, rhov_ref, rho_ref, rhop_ref, rhom_ref;
    double h_ref;
    double p_new, pp_new, pm_new;
    double functT = 1.;
    double dfunctT, dT, dTu = 0.0;

    double inewt;
    double nnewt = 100;
    double dTumax = 10.e0;
    double epsn = 1.e-3;
    double epsilon = 1.e-4;

    int ierr;
    char herr[HC255];
    memset(herr, '\0', sizeof(herr));

    EOS_Internal_Error err;

    p_ref = pa2kpa(p); // eos to refprop unit

    // Initialisation :
    //     saturation values + newton
    err = compute_T_sat_p(p, tsat_ref);
    if (err.generic_error() != EOS_Error::good)
      return err;

    err = compute_rho_l_sat_p(p, rho_sat);
    if (err.generic_error() != EOS_Error::good)
      return err;

    rholsat_ref = eos_rho_2_refprop(rho_sat);

    err = compute_rho_v_sat_p(p, rho_sat);
    if (err.generic_error() != EOS_Error::good)
      return err;

    rhovsat_ref = eos_rho_2_refprop(rho_sat);
    rhol_ref = rholsat_ref;
    rhov_ref = rhovsat_ref;

    // h_lim : liquid or vapor
    switch (prop)
    {
    case NEPTUNE::h_l_lim:
      tinit_ref = 0.5 * (tsat_ref + tcrit);
      break;
    case NEPTUNE::h_v_lim:
      tinit_ref = tsat_ref;
      break;
    default:
      return EOS_Internal_Error::NOT_IMPLEMENTED;
      break;
    }

    t_ref = tinit_ref;

    for (inewt = 0; (inewt < nnewt) && (fabs(functT) >= epsn); inewt++)
    {
      F77NAME(liqspndl_rp10)
      (t_ref, arr_molfrac, rhol_ref, ierr, herr, sizeof(herr) - 1);
      if (ierr != 0)
        return generate_error(ierr, herr);
      F77NAME(vapspndl_rp10)
      (t_ref, arr_molfrac, rhov_ref, ierr, herr, sizeof(herr) - 1);
      if (ierr != 0)
        return generate_error(ierr, herr);
      switch (prop)
      {
      case NEPTUNE::h_l_lim:
        rho_ref = rhol_ref;
        break;
      case NEPTUNE::h_v_lim:
        rho_ref = rhov_ref;
        break;
      default:
        break;
      }

      F77NAME(press_rp10)
      (t_ref, rho_ref, arr_molfrac, p_new);
      functT = p_new - p_ref;
      tp_ref = t_ref * (1 + epsilon);
      tm_ref = t_ref * (1 - epsilon);
      rhop_ref = rho_ref;
      rhom_ref = rho_ref;

      switch (prop)
      {
      case NEPTUNE::h_l_lim:
        F77NAME(liqspndl_rp10)
        (tp_ref, arr_molfrac, rhop_ref, ierr, herr, sizeof(herr) - 1);
        if (ierr != 0)
          return generate_error(ierr, herr);
        F77NAME(liqspndl_rp10)
        (tm_ref, arr_molfrac, rhom_ref, ierr, herr, sizeof(herr) - 1);
        if (ierr != 0)
          return generate_error(ierr, herr);
        break;
      case NEPTUNE::h_v_lim:
        F77NAME(vapspndl_rp10)
        (tp_ref, arr_molfrac, rhop_ref, ierr, herr, sizeof(herr) - 1);
        if (ierr != 0)
          return generate_error(ierr, herr);
        F77NAME(vapspndl_rp10)
        (tm_ref, arr_molfrac, rhom_ref, ierr, herr, sizeof(herr) - 1);
        if (ierr != 0)
          return generate_error(ierr, herr);
        break;
      default:
        break;
      }

      F77NAME(press_rp10)
      (tp_ref, rhop_ref, arr_molfrac, pp_new);
      F77NAME(press_rp10)
      (tm_ref, rhom_ref, arr_molfrac, pm_new);
      dfunctT = (pp_new - pm_new) / (tp_ref - tm_ref);

      // time limitation (dTumax)
      dT = -functT / dfunctT;
      dTu = dT / fabs(dT) * (std::min(dTumax, fabs(dT)));
      t_ref = t_ref + dTu;
    }

    if (fabs(functT) < epsn)
    {
      double tmp = t_ref - dTu;
      F77NAME(enthal_rp10)
      (tmp, rho_ref, arr_molfrac, h_ref);
      h_lim = refprop_nrj_2_eos(h_ref);
      return EOS_Internal_Error::OK;
    }
    else
    {
      char msg[] = "EOS_Refprop10::compute_h_lim : Newton algorithm did not converge";
      return generate_error(100, msg);
    }
  }

  //  NOT USED : this function is a part of an old_version of h_l_lim() and h_v_lim()
  //  EOS_Internal_Error EOS_Refprop10::
  //  call_spndl(const char* const property_name, double p, double t_lim, double& rho) const
  //  {
  //      double rho_l, rho_v;
  //
  //      compute_rho_l_sat_p(p, rho_l);
  //      compute_rho_v_sat_p(p, rho_v);
  //
  //      double rho_l_ref = eos_rho_2_refprop(rho_l);
  //      double rho_v_ref = eos_rho_2_refprop(rho_v);
  //      F77NAME(spndl_rp10)(t_lim, arr_molfrac,rho_l_ref, rho_v_ref, ierr, herr, sizeof(herr)-1);
  //
  //      if (!ierr)
  //      {
  //              // add refprop error
  //      }
  //
  //      EOS_splimprop prop = nam2num_splimprop(property_name);
  //      switch(prop)
  //      {
  //      case NEPTUNE::h_l_lim:
  //              rho = refprop_rho_2_eos(rho_l);
  //              break;
  //      case NEPTUNE::h_v_lim:
  //              rho = refprop_rho_2_eos(rho_v);
  //              break;
  //      default:
  //              return EOS_Internal_Error::NOT_IMPLEMENTED;
  //      }
  //
  //      return EOS_Internal_Error::OK;
  //  }

  //  NOT USED : this function is a part of an old version of h_l_lim() and h_v_lim
  //  Evaluate t_lim (with newton) for spinodale function
  //
  //  EOS_Internal_Error EOS_Refprop10::
  //  evaluate_t_lim(const char* const property_name, double p, double& t_lim) const
  //  {
  //      EOS_Internal_Error err1(EOS_Internal_Error::OK);
  //      EOS_Internal_Error err2(EOS_Internal_Error::OK);
  //      EOS_Internal_Error err3(EOS_Internal_Error::OK);
  //      EOS_Internal_Error err4(EOS_Internal_Error::OK);
  //      EOS_Internal_Error err5(EOS_Internal_Error::OK);
  //      double Ti, pi, dpi,rho, drhodt, deltap;
  //
  //      double func=1.;
  //      int nb_iter_max=50;
  //      int iter_Newton;
  //      double epsilon=1.e-6;
  //
  //      //variables for THERM2
  //      double e, h, s, cv, cp, w;
  //      double Z, hjt,A, G,xkappa, beta,dPdrho,d2PdD2,dPT,drhodT,drhodP,spare1,spare2,spare3, spare4;
  //
  //      compute_T_sat_p(p, Ti); // Starting from T_sat
  //
  //      for(iter_Newton = 0;
  //                      (iter_Newton < nb_iter_max) && (fabs(func) > epsilon);
  //                      iter_Newton++ )
  //      {
  //              err1 = call_spndl(property_name, p, Ti, rho); //get rho for Ti
  //              rho = eos_rho_2_refprop(rho);
  //
  //              //Call to therm2 can be replaced by
  //              // call_therm -> give p
  //              // call DPDD  -> give dPT
  //              // call DPDT  -> give dPdrho
  //              F77NAME(therm2_rp10)(Ti,rho,arr_molfrac,pi,e,h,s,cv,cp,w, Z, hjt,A, G, xkappa, beta,dPdrho,d2PdD2,dPT,drhodT,drhodP,spare1,spare2,spare3, spare4);
  //
  //              //compute drhodt with spndl subroutine
  //              double Tm=Ti*(1-epsilon);
  //              double Tp=Ti*(1+epsilon);
  //              double rhom, rhop;
  //              err2 = call_spndl(property_name, p, Tm, rhom);
  //              err3 = call_spndl(property_name, p, Tp, rhop);
  //
  //              drhodt=(eos_rho_2_refprop(rhop) - eos_rho_2_refprop(rhom))/(2*epsilon*Ti);
  //
  //              dpi = dPT + dPdrho*drhodt;
  //              func   = kpa2pa(p) - pi;
  //              deltap = func / dpi;
  //              Ti     = Ti + deltap;
  //
  //              // If bad value encountered during Newton iterations, error
  //              switch (worst_internal_error(err1, err2).generic_error()) {
  //              case error: return worst_internal_error(err2, err3);
  //              case bad: return 1;
  //              default:   ;
  //              }
  //      }
  //      // If not converged, error.
  //      if (iter_Newton >= nb_iter_max && (fabs(func) > epsilon))
  //              return 1;
  //
  //      t_lim = Ti;
  //
  //      return EOS_Internal_Error::OK;
  //  }

  void EOS_Refprop10::calrp_indic_ph(EOS_Fields &r, int *indic) const
  // indic[0]   phfl1
  // indic[1]   therm2
  // indic[2]   trnprp
  // indic[3]   stn
  // indic[4]   ag
  // indic[5]   dhd1
  // indic[6]   therm2 en differences finies
  // indic[7]   trnprp en differences finies
  // indic[8]   stn en differences finies
  // indic[9]   ag en differences finies
  {
    for (int i = 0; i <= 9; i++)
      indic[i] = 0;
    const int nb_fields = r.size();
    for (int k = 0; k < nb_fields; k++)
    {
      EOS_Field rk = r[k];
      EOS_Property prop = rk.get_property_number();
      if (prop >= NEPTUNE::T)
        indic[0] = 1;
      switch (prop)
      {
      case NEPTUNE::T:
      case NEPTUNE::rho:
        break;
      case NEPTUNE::u:
      case NEPTUNE::s:
      case NEPTUNE::cp:
      case NEPTUNE::cv:
      case NEPTUNE::w:
      case NEPTUNE::beta:
        indic[1] = 1;
        break;
      case NEPTUNE::mu:
      case NEPTUNE::lambda:
        indic[2] = 1;
        break;
      case NEPTUNE::pr:
        indic[1] = 1;
        indic[2] = 1;
        break;
      case NEPTUNE::sigma:
        indic[3] = 1;
        break;
      case NEPTUNE::g:
      case NEPTUNE::f:
        indic[4] = 1;
        break;
      default:
        // first derivative
        if (prop >= NEPTUNE::EOS_TPROPDER)
          indic[5] = 1;
        if (prop >= NEPTUNE::d_rho_d_p_h)
          indic[0] = 1;
        switch (prop)
        {
        case NEPTUNE::d_T_d_p_h:
        case NEPTUNE::d_T_d_h_p:
        case NEPTUNE::d_rho_d_p_h:
        case NEPTUNE::d_rho_d_h_p:
          break;
        case NEPTUNE::d_u_d_p_h:
        case NEPTUNE::d_u_d_h_p:
        case NEPTUNE::d_s_d_p_h:
        case NEPTUNE::d_s_d_h_p:
        case NEPTUNE::d_cp_d_p_h:
        case NEPTUNE::d_cp_d_h_p:
        case NEPTUNE::d_cv_d_p_h:
        case NEPTUNE::d_cv_d_h_p:
        case NEPTUNE::d_w_d_p_h:
        case NEPTUNE::d_w_d_h_p:
        case NEPTUNE::d_beta_d_p_h:
        case NEPTUNE::d_beta_d_h_p:
          indic[1] = 1;
          indic[6] = 1;
          break;
        case NEPTUNE::d_mu_d_p_h:
        case NEPTUNE::d_mu_d_h_p:
        case NEPTUNE::d_lambda_d_p_h:
        case NEPTUNE::d_lambda_d_h_p:
          indic[2] = 1;
          indic[7] = 1;
          break;
        case NEPTUNE::d_pr_d_p_h:
        case NEPTUNE::d_pr_d_h_p:
          indic[1] = 1;
          indic[2] = 1;
          indic[6] = 1;
          indic[7] = 1;
          break;
        case NEPTUNE::d_sigma_d_p_h:
        case NEPTUNE::d_sigma_d_h_p:
          indic[3] = 1;
          indic[8] = 1;
          break;
        case NEPTUNE::d_g_d_p_h:
        case NEPTUNE::d_g_d_h_p:
        case NEPTUNE::d_f_d_p_h:
        case NEPTUNE::d_f_d_h_p:
          indic[4] = 1;
          indic[9] = 1;
          break;
        default:
          break;
        }
      }
    }
  }

  inline EOS_Internal_Error EOS_Refprop10::calrp_optim_ph(double p, double h,
                                                          double *tab_prop,
                                                          double *tab_propder,
                                                          int *indic) const
  {
    int kph, ierr;
    char herr[HC255];
    memset(herr, '\0', sizeof(herr));

    double T, s, cp, cv, w, mu, lambda, beta, sigma, f, g;
    double e, rhol, rhov, hjt, pi, Z, h0;
    double A, G, xkappa, dPdrho, d2PdD2, dPT, drhodT, drhodP, spare1, spare2, spare3, spare4;
    EOS_Internal_Error err, err2;

    double p_refprop = pa2kpa(p);
    double h_refprop = eos_nrj_2_refprop(h);
    double rho_refprop = 0.e0;

    err = calrp_kph(p_refprop, h_refprop, kph);
    if (err.generic_error() == EOS_Error::bad)
      return err;

    T = tab_prop[NEPTUNE::T];

    if (indic[0] == 1)
    {
      F77NAME(phfl1_rp10)
      (p_refprop, h_refprop, arr_molfrac, kph, T, rho_refprop,
       ierr, herr, sizeof(herr) - 1);
      if (ierr != 0)
      {
        // Une température initiale éloignée de la solution peut faire échouer phfl1
        // alors que la solution est obtenue avec une température nulle (ensuite convertie en température critique)
        T = 0.;
        F77NAME(phfl1_rp10)
        (p_refprop, h_refprop, arr_molfrac, kph, T, rho_refprop,
         ierr, herr, sizeof(herr) - 1);
        if (ierr)
          err = worst_internal_error(err, generate_error(ierr, herr));
      }
      if (ierr != 0)
      {
        // Quand phfl1 est en echec, on peut obtenir une valeur raisonable de la densité
        // avec un appel de tprho en forçant la phase
        int j = 0;
        int mph = -kph;
        F77NAME(tprho_rp10)
        (T, p_refprop, arr_molfrac, mph, j, rho_refprop,
         ierr, herr, sizeof(herr) - 1);
      }
      tab_prop[NEPTUNE::T] = T;
      tab_prop[NEPTUNE::rho] = refprop_rho_2_eos(rho_refprop);
    }

    F77NAME(therm2_rp10)
    (T, rho_refprop, arr_molfrac, pi, e, h0, s, cv, cp, w, Z, hjt, A, G,
     xkappa, beta, dPdrho, d2PdD2, dPT, drhodT, drhodP, spare1, spare2,
     spare3, spare4);
    if (indic[1] == 1)
    {
      tab_prop[NEPTUNE::u] = refprop_nrj_2_eos(e);
      tab_prop[NEPTUNE::s] = refprop_nrj_2_eos(s);
      tab_prop[NEPTUNE::cp] = refprop_nrj_2_eos(cp);
      tab_prop[NEPTUNE::cv] = refprop_nrj_2_eos(cv);
      tab_prop[NEPTUNE::w] = w;
      tab_prop[NEPTUNE::beta] = beta;
    }

    if (indic[2] == 1)
    {
      F77NAME(trnprp_rp10)
      (T, rho_refprop, arr_molfrac, mu, lambda,
       ierr, herr, sizeof(herr) - 1);
      tab_prop[NEPTUNE::mu] = micropa2pa(mu);
      tab_prop[NEPTUNE::lambda] = lambda;
      if (ierr != 0)
        err = worst_internal_error(err, generate_error(ierr, herr));
    }

    if (indic[1] == 1 && indic[2] == 1)
    {
      tab_prop[NEPTUNE::pr] = tab_prop[NEPTUNE::mu] * tab_prop[NEPTUNE::cp] / tab_prop[NEPTUNE::lambda];
    }

    if (indic[3] == 1)
    {
      F77NAME(stn_rp10)
      (T, rhol = 0.0, rhov = 0.0, arr_molfrac, arr_molfrac, sigma,
       ierr, herr, sizeof(herr) - 1);
      tab_prop[NEPTUNE::sigma] = sigma;
      if (ierr != 0)
      {
        // if the computation of sigma is not successful, don't try to compute its derivatives
        indic[8] = 0;
        tab_propder[NEPTUNE::d_sigma_d_p_h] = 0.0;
        tab_propder[NEPTUNE::d_sigma_d_h_p] = 0.0;
      }
      //       if (ierr != 0) err = worst_internal_error(err, generate_error(ierr, herr));
    }

    if (indic[4] == 1)
    {
      F77NAME(ag_rp10)
      (T, rho_refprop, arr_molfrac, f, g);
      tab_prop[NEPTUNE::g] = refprop_nrj_2_eos(g);
      tab_prop[NEPTUNE::f] = refprop_nrj_2_eos(f);
    }

    double d_T_d_p = 0.e0;
    double d_rho_d_p = 0.e0;
    double d_T_d_h = 0.e0;
    double d_rho_d_h = 0.e0;
    if (indic[5] == 1)
    {
      double dhdt_d, dhdt_p, dhdd_t, dhdd_p, dhdp_t, dhdp_d;
      F77NAME(dhd1_rp10)
      (T, rho_refprop, arr_molfrac, dhdt_d, dhdt_p, dhdd_t, dhdd_p, dhdp_t, dhdp_d);
      d_T_d_p = -1 / refprop_nrj_2_eos(dhdt_p) * refprop_nrj_2_eos(dhdp_t) / kpa2pa(1);
      d_rho_d_p = refprop_rho_2_eos(drhodP) / kpa2pa(1) + refprop_rho_2_eos(drhodT) * d_T_d_p;
      d_T_d_h = 1 / refprop_nrj_2_eos(dhdt_p);
      d_rho_d_h = refprop_rho_2_eos(drhodT) * d_T_d_h;
      tab_propder[NEPTUNE::d_T_d_p_h] = d_T_d_p;
      tab_propder[NEPTUNE::d_T_d_h_p] = d_T_d_h;
      tab_propder[NEPTUNE::d_rho_d_p_h] = d_rho_d_p;
      tab_propder[NEPTUNE::d_rho_d_h_p] = d_rho_d_h;
    }

    // M.F. double Tp, rhop, ep, sp, cvp, cpp, wp, betap, lambdap, mup, sigmap, fp, gp;
    double Tp, rhop;
    double e_tp, s_tp, cv_tp, cp_tp, w_tp, beta_tp, lambda_tp, mu_tp, sigma_tp, f_tp, g_tp;
    double e_rp, s_rp, cv_rp, cp_rp, w_rp, beta_rp, lambda_rp, mu_rp, f_rp, g_rp;
    double delta_T = 0.e0;
    double delta_rho = 0.e0;
    double epsilon = 0.e0;
    if (indic[6] + indic[7] + indic[8] + indic[9] >= 1)
    {
      epsilon = 1e-6;
      Tp = T * (1 + epsilon);
      delta_T = Tp - T;
      rhop = rho_refprop * (1 + epsilon);
      delta_rho = refprop_rho_2_eos(rhop - rho_refprop);
    }

    if (indic[6] == 1)
    {
      F77NAME(therm2_rp10)
      (Tp, rho_refprop, arr_molfrac, pi, e_tp, h0,
       s_tp, cv_tp, cp_tp, w_tp, Z, hjt, A, G,
       xkappa, beta_tp, dPdrho, d2PdD2, dPT, drhodT, drhodP,
       spare1, spare2, spare3, spare4);
      double d_cv_d_T = (refprop_nrj_2_eos(cv_tp) - refprop_nrj_2_eos(cv)) / delta_T;
      double d_cp_d_T = (refprop_nrj_2_eos(cp_tp) - refprop_nrj_2_eos(cp)) / delta_T;
      double d_s_d_T = (refprop_nrj_2_eos(s_tp) - refprop_nrj_2_eos(s)) / delta_T;
      double d_w_d_T = (w_tp - w) / delta_T;
      double d_beta_d_T = (beta_tp - beta) / delta_T;

      F77NAME(therm2_rp10)
      (T, rhop, arr_molfrac, pi, e_rp, h0,
       s_rp, cv_rp, cp_rp, w_rp, Z, hjt, A, G,
       xkappa, beta_rp, dPdrho, d2PdD2, dPT, drhodT, drhodP,
       spare1, spare2, spare3, spare4);
      double d_cv_d_rho = (refprop_nrj_2_eos(cv_rp) - refprop_nrj_2_eos(cv)) / delta_rho;
      double d_cp_d_rho = (refprop_nrj_2_eos(cp_rp) - refprop_nrj_2_eos(cp)) / delta_rho;
      double d_s_d_rho = (refprop_nrj_2_eos(s_rp) - refprop_nrj_2_eos(s)) / delta_rho;
      double d_w_d_rho = (w_rp - w) / delta_rho;
      double d_beta_d_rho = (beta_rp - beta) / delta_rho;

      tab_propder[NEPTUNE::d_cp_d_p_h] = d_cp_d_T * d_T_d_p + d_cp_d_rho * d_rho_d_p;
      tab_propder[NEPTUNE::d_cv_d_p_h] = d_cv_d_T * d_T_d_p + d_cv_d_rho * d_rho_d_p;
      tab_propder[NEPTUNE::d_s_d_p_h] = d_s_d_T * d_T_d_p + d_s_d_rho * d_rho_d_p;
      tab_propder[NEPTUNE::d_w_d_p_h] = d_w_d_T * d_T_d_p + d_w_d_rho * d_rho_d_p;
      tab_propder[NEPTUNE::d_beta_d_p_h] = d_beta_d_T * d_T_d_p + d_beta_d_rho * d_rho_d_p;
      tab_propder[NEPTUNE::d_cp_d_h_p] = d_cp_d_T * d_T_d_h + d_cp_d_rho * d_rho_d_h;
      tab_propder[NEPTUNE::d_cv_d_h_p] = d_cv_d_T * d_T_d_h + d_cv_d_rho * d_rho_d_h;
      tab_propder[NEPTUNE::d_s_d_h_p] = d_s_d_T * d_T_d_h + d_s_d_rho * d_rho_d_h;
      tab_propder[NEPTUNE::d_w_d_h_p] = d_w_d_T * d_T_d_h + d_w_d_rho * d_rho_d_h;
      tab_propder[NEPTUNE::d_beta_d_h_p] = d_beta_d_T * d_T_d_h + d_beta_d_rho * d_rho_d_h;
    }

    if (indic[7] == 1)
    {
      F77NAME(trnprp_rp10)
      (Tp, rho_refprop, arr_molfrac, mu_tp, lambda_tp,
       ierr, herr, sizeof(herr) - 1);
      if (ierr != 0)
        err = worst_internal_error(err, generate_error(ierr, herr));
      double d_lambda_d_T = (lambda_tp - lambda) / delta_T;
      double d_mu_d_T = (micropa2pa(mu_tp) - micropa2pa(mu)) / delta_T;
      F77NAME(trnprp_rp10)
      (T, rhop, arr_molfrac, mu_rp, lambda_rp, ierr, herr, sizeof(herr) - 1);
      if (ierr != 0)
        err = worst_internal_error(err, generate_error(ierr, herr));
      double d_lambda_d_rho = (lambda_rp - lambda) / delta_rho;
      double d_mu_d_rho = (micropa2pa(mu_rp) - micropa2pa(mu)) / delta_rho;
      tab_propder[NEPTUNE::d_lambda_d_p_h] = d_lambda_d_T * d_T_d_p + d_lambda_d_rho * d_rho_d_p;
      tab_propder[NEPTUNE::d_mu_d_p_h] = d_mu_d_T * d_T_d_p + d_mu_d_rho * d_rho_d_p;
      tab_propder[NEPTUNE::d_lambda_d_h_p] = d_lambda_d_T * d_T_d_h + d_lambda_d_rho * d_rho_d_h;
      tab_propder[NEPTUNE::d_mu_d_h_p] = d_mu_d_T * d_T_d_h + d_mu_d_rho * d_rho_d_h;
    }

    if (indic[2] == 1 && indic[3] == 1 && indic[7] == 1 && indic[8] == 1)
    {
      double dmu = tab_propder[NEPTUNE::d_mu_d_p_h];
      double dlambda = tab_propder[NEPTUNE::d_lambda_d_p_h];
      double dcp = tab_propder[NEPTUNE::d_cp_d_p_h];
      lambda = tab_prop[NEPTUNE::lambda];
      if (lambda <= 0.)
        err = Error_lambda_value;
      mu = tab_prop[NEPTUNE::mu];
      cp = tab_prop[NEPTUNE::cp];
      tab_propder[NEPTUNE::d_pr_d_p_h] = ((dmu * cp + mu * dcp) * lambda - mu * cp * dlambda) / (lambda * lambda);
      dmu = tab_propder[NEPTUNE::d_mu_d_h_p];
      dlambda = tab_propder[NEPTUNE::d_lambda_d_h_p];
      dcp = tab_propder[NEPTUNE::d_cp_d_h_p];
      tab_propder[NEPTUNE::d_pr_d_h_p] = ((dmu * cp + mu * dcp) * lambda - mu * cp * dlambda) / (lambda * lambda);
    }

    if (indic[8] == 1)
    {
      F77NAME(stn_rp10)
      (Tp, rhol = 0.0, rhov = 0.0, arr_molfrac, arr_molfrac, sigma_tp,
       ierr, herr, sizeof(herr) - 1);
      //       if (ierr != 0) err = worst_internal_error(err, generate_error(ierr, herr));
      double d_sigma_d_T = (sigma_tp - sigma) / delta_T;
      tab_propder[NEPTUNE::d_sigma_d_p_h] = d_sigma_d_T * d_T_d_p;
      tab_propder[NEPTUNE::d_sigma_d_h_p] = d_sigma_d_T * d_T_d_h;
    }

    if (indic[9] == 1)
    {
      F77NAME(ag_rp10)
      (Tp, rho_refprop, arr_molfrac, f_tp, g_tp);
      double d_f_d_T = (refprop_nrj_2_eos(f_tp) - refprop_nrj_2_eos(f)) / delta_T;
      double d_g_d_T = (refprop_nrj_2_eos(g_tp) - refprop_nrj_2_eos(g)) / delta_T;
      F77NAME(ag_rp10)
      (T, rhop, arr_molfrac, f_rp, g_rp);
      double d_f_d_rho = (refprop_nrj_2_eos(f_rp) - refprop_nrj_2_eos(f)) / delta_rho;
      double d_g_d_rho = (refprop_nrj_2_eos(g_rp) - refprop_nrj_2_eos(g)) / delta_rho;
      tab_propder[NEPTUNE::d_f_d_p_h] = d_f_d_T * d_T_d_p + d_f_d_rho * d_rho_d_p;
      tab_propder[NEPTUNE::d_g_d_p_h] = d_g_d_T * d_T_d_p + d_g_d_rho * d_rho_d_p;
      tab_propder[NEPTUNE::d_f_d_h_p] = d_f_d_T * d_T_d_h + d_f_d_rho * d_rho_d_h;
      tab_propder[NEPTUNE::d_g_d_h_p] = d_g_d_T * d_T_d_h + d_g_d_rho * d_rho_d_h;
    }
    return err;
  }

  inline EOS_Internal_Error EOS_Refprop10::calrp_optim_ps(double p, double s,
                                                          double *tab_prop,
                                                          double *tab_propder) const
  {
    double h;
    int kph;
    int ierr;
    char herr[HC255];
    memset(herr, '\0', sizeof(herr));

    ierr = 0;
    EOS_Internal_Error err = EOS_Internal_Error::OK;

    double p_refprop = pa2kpa(p);
    double s_refprop = eos_nrj_2_refprop(s);
    h = 0.0;
    err = calrp_kph(p_refprop, h, kph);
    if (err.generic_error() == EOS_Error::bad)
      return err;

    /*    double  rho_refprop = 0.0;
          t = 0;
          F77NAME(psfl1_rp10) (p_refprop,s_refprop,arr_molfrac,kph,t,rho_refprop,ierr,herr, sizeof(herr)-1);
          F77NAME(enthal_rp10)(t, rho_refprop, arr_molfrac, h);*/

    double rho, rhol, rhov, q, e, T, cv, cp, w;
    double *x = new double[nbcomp];
    double *y = new double[nbcomp];

    ierr = 0;
    F77NAME(psflsh_rp10)
    (p_refprop, s_refprop, arr_molfrac, T, rho, rhol, rhov, x, y, q, e, h, cv, cp, w, ierr, herr, sizeof(herr) - 1);
    if (ierr != 0)
      err = worst_internal_error(err, generate_error(ierr, herr));
    tab_prop[NEPTUNE::h] = refprop_nrj_2_eos(h);
    tab_propder[NEPTUNE::d_h_d_s_p] = T;

    double epsilon = 1e-6;
    double p_refpropp = p_refprop * (1 + epsilon);
    double delta = kpa2pa(p_refpropp - p_refprop);
    double hp;
    F77NAME(psflsh_rp10)
    (p_refpropp, s_refprop, arr_molfrac, T, rho, rhol, rhov, x, y, q, e, hp, cv, cp, w, ierr, herr, sizeof(herr) - 1);
    if (ierr != 0)
      err = worst_internal_error(err, generate_error(ierr, herr));
    tab_propder[NEPTUNE::d_h_d_p_s] = refprop_nrj_2_eos(hp - h) / delta;

    delete[] x;
    delete[] y;

    return err;
  }

  EOS_Error EOS_Refprop10::compute(const EOS_Field &pp,
                                   const EOS_Field &hh,
                                   EOS_Fields &r,
                                   EOS_Error_Field &errfield) const
  {
    const int nb_fields = r.size();
    const int sz = pp.size();
    assert(hh.size() == sz);
    assert(errfield.size() == sz);
    errfield = EOS_Internal_Error::OK;
    EOS_Internal_Error err;
    vector<double> vec_prop(NUMAXPROP, 0.e0);
    double *tab_prop = &vec_prop[0];
    vector<double> vec_propder(NUMAXPROP, 0.e0);
    double *tab_propder = &vec_propder[0];
    int indic[11];
    EOS_Field rk;
    EOS_Property prop;
    EOS_Property tab_pos[NBMAXFIELD], tab_posder[NBMAXFIELD];

    // reset fluid with setup
    err = callSetup();
    if (err.generic_error() != EOS_Error::good)
      return errfield.find_worst_error().generic_error();
    // reset fluid with setup
    err = callSetup();
    if (err.generic_error() != EOS_Error::good)
      return errfield.find_worst_error().generic_error();

    // tab_pos sert a etablir une correspondance entre la liste de champs "r" et les proprietes
    for (int k = 0; k < nb_fields; k++)
    {
      rk = r[k];
      prop = rk.get_property_number();
      if (prop != NEPTUNE::NotATProperty)
      {
        if (prop < EOS_TPROPDER)
        {
          tab_pos[k] = prop;
          tab_posder[k] = NEPTUNE::NotATProperty;
        }
        else
        {
          tab_pos[k] = NEPTUNE::NotATProperty;
          tab_posder[k] = prop;
        }
      }
      else
      {
        tab_pos[k] = NEPTUNE::NotATProperty;
        tab_posder[k] = NEPTUNE::NotATProperty;
      }
    }

    if ((pp.get_property_number() == NEPTUNE::p) && (hh.get_property_number() == NEPTUNE::T))
    { // return EOS_Fluid::compute(pp, hh, r, errfield);  //Calcul non optimise
      calrp_indic_pt(r, indic);
      for (int i = 0; i < sz; i++)
      {
        err = calrp_optim_pt(pp[i], hh[i], tab_prop, tab_propder, indic);
        for (int k = 0; k < nb_fields; k++)
        {
          rk = r[k];
          if (tab_pos[k] != NEPTUNE::NotATProperty)
            rk[i] = tab_prop[tab_pos[k]];
          else if (tab_posder[k] != NEPTUNE::NotATProperty)
            rk[i] = tab_propder[tab_posder[k]];
        }
        errfield.set(i, err);
      }
    }
    else if ((pp.get_property_number() == NEPTUNE::p) && (hh.get_property_number() == NEPTUNE::h))
    {
      calrp_indic_ph(r, indic);
      int index_temp = -1;
      for (int k = 0; k < nb_fields; k++)
      {
        if (tab_pos[k] == NEPTUNE::T)
          index_temp = k;
      }
      for (int i = 0; i < sz; i++)
      {
        if (index_temp >= 0)
          tab_prop[NEPTUNE::T] = r[index_temp][i];
        err = calrp_optim_ph(pp[i], hh[i], tab_prop, tab_propder, indic);

        for (int k = 0; k < nb_fields; k++)
        {
          rk = r[k];
          if (tab_pos[k] != NEPTUNE::NotATProperty)
            rk[i] = tab_prop[tab_pos[k]];
          else if (tab_posder[k] != NEPTUNE::NotATProperty)
            rk[i] = tab_propder[tab_posder[k]];
        }

        errfield.set(i, err);
      }
    }
    else if ((pp.get_property_number() == NEPTUNE::p) && (hh.get_property_number() == NEPTUNE::s))
    { //          return EOS_Fluid::compute(pp, hh, r, errfield);
      for (int i = 0; i < sz; i++)
      {
        err = calrp_optim_ps(pp[i], hh[i], tab_prop, tab_propder);
        for (int k = 0; k < nb_fields; k++)
        {
          rk = r[k];
          if (tab_pos[k] != NEPTUNE::NotATProperty)
            rk[i] = tab_prop[tab_pos[k]];
          else if (tab_posder[k] != NEPTUNE::NotATProperty)
            rk[i] = tab_propder[tab_posder[k]];
        }
      }
    }
    else
      return EOS_Fluid::compute(pp, hh, r, errfield);

    return errfield.find_worst_error().generic_error();
  }

  void EOS_Refprop10::calrp_indic_sat_p(EOS_Fields &r, int *indic) const
  // indic[0]   satp
  // indic[1]   enthal liquide
  // indic[2]   enthal vapeur
  // indic[3]   cvcp liquide
  // indic[4]   cvcp vapeur
  // indic[5]   satp en differences finies
  // indic[6]   enthal liquide en differences finies
  // indic[7]   enthal vapeur en differences finies
  // indic[8]   cvcp liquide
  // indic[9]   cvcp vapeur
  // indic[10]  satp en differences finies pour derivee seconde
  {
    for (int i = 0; i <= 10; i++)
      indic[i] = 0;
    const int nb_fields = r.size();
    for (int k = 0; k < nb_fields; k++)
    {
      EOS_Field rk = r[k];
      EOS_Property prop = rk.get_property_number();
      if (prop >= NEPTUNE::firstSatProperty && prop <= NEPTUNE::lastSatProperty)

        indic[0] = 1;
      switch (prop)
      {
      case NEPTUNE::T_sat:
        break;
      case NEPTUNE::rho_l_sat:
        break;
      case NEPTUNE::rho_v_sat:
        break;
      case NEPTUNE::h_l_sat:
        indic[1] = 1;
        break;
      case NEPTUNE::h_v_sat:
        indic[2] = 1;
        break;
      case NEPTUNE::cp_l_sat:
        indic[3] = 1;
        break;
      case NEPTUNE::cp_v_sat:
        indic[4] = 1;
        break;

        // first derivative
      case d_T_sat_d_p:
        indic[5] = 1;
        break;

      case NEPTUNE::d_h_l_sat_d_p:
        indic[6] = 1;
        break;
      case NEPTUNE::d_h_v_sat_d_p:
        indic[7] = 1;
        break;
      case NEPTUNE::d_cp_l_sat_d_p:
        indic[8] = 1;
        break;
      case NEPTUNE::d_cp_v_sat_d_p:
        indic[9] = 1;
        break;
        // seconde derivative
      case NEPTUNE::d2_T_sat_d_p_d_p:
        indic[5] = 1;
        indic[10] = 1;
        break;
      }
    }
  }

  inline EOS_Internal_Error EOS_Refprop10::calrp_optim_sat_p(double p, double *tab_prop,
                                                             double *tab_propder,
                                                             double *tab_propder2,
                                                             int *indic) const
  {
    double t = 0.0,
           rhol = 0.0, rhov = 0.0,
           xliq = 0.0, xvapint = 0.0,
           hl_refprop = 0.0, hv_refprop = 0.0,
           cv = 0.0, cpl = 0.0, cpv = 0.0;
    int kph;
    EOS_Internal_Error err = EOS_Internal_Error::OK;
    double p_refprop = pa2kpa(p);

    err = calrp_kph(p_refprop, h, kph);
    if (err.generic_error() == EOS_Error::bad)
      return err;

    int ierr;
    char herr[HC255];
    memset(herr, '\0', sizeof(herr));

    if (indic[0] == 1)
    {
      F77NAME(satp_rp10)
      (p_refprop, arr_molfrac, kph, t, rhol, rhov, xliq, xvapint,
       ierr, herr, sizeof(herr) - 1);
      tab_prop[NEPTUNE::rho_l_sat] = refprop_rho_2_eos(rhol);
      tab_prop[NEPTUNE::rho_v_sat] = refprop_rho_2_eos(rhov);
      if (ierr != 0)
        err = worst_internal_error(err, generate_error(ierr, herr));
      tab_prop[NEPTUNE::T_sat] = t;
    }
    if (indic[1] == 1)
    {
      F77NAME(enthal_rp10)
      (t, rhol, arr_molfrac, hl_refprop);
      tab_prop[NEPTUNE::h_l_sat] = refprop_nrj_2_eos(hl_refprop);
    }
    if (indic[2] == 1)
    {
      F77NAME(enthal_rp10)
      (t, rhov, arr_molfrac, hv_refprop);
      tab_prop[NEPTUNE::h_v_sat] = refprop_nrj_2_eos(hv_refprop);
    }
    if (indic[3] == 1)
    {
      F77NAME(cvcp_rp10)
      (t, rhol, arr_molfrac, cv, cpl);
      tab_prop[NEPTUNE::cp_l_sat] = refprop_nrj_2_eos(cpl);
    }
    if (indic[4] == 1)
    {
      F77NAME(cvcp_rp10)
      (t, rhov, arr_molfrac, cv, cpv);
      tab_prop[NEPTUNE::cp_v_sat] = refprop_nrj_2_eos(cpv);
    }

    double epsilon = 1e-6;
    double p_refpropp = p_refprop * (1 + epsilon);
    double delta = kpa2pa(p_refpropp - p_refprop);
    double tp = 0.0, rholp = 0.0, rhovp = 0.0, hl_refpropp = 0.0, hv_refpropp = 0.0, cplp = 0.0, cpvp = 0.0;

    if (indic[5] == 1)
    {
      F77NAME(satp_rp10)
      (p_refpropp, arr_molfrac, kph, tp, rholp, rhovp, xliq, xvapint, ierr, herr, sizeof(herr) - 1);
      tab_propder[NEPTUNE::d_T_sat_d_p] = (tp - t) / delta;
      tab_propder[NEPTUNE::d_rho_l_sat_d_p] = (refprop_rho_2_eos(rholp) - refprop_rho_2_eos(rhol)) / delta;
      tab_propder[NEPTUNE::d_rho_v_sat_d_p] = (refprop_rho_2_eos(rhovp) - refprop_rho_2_eos(rhov)) / delta;
      if (ierr != 0)
        err = worst_internal_error(err, generate_error(ierr, herr));
    }
    if (indic[6] == 1)
    {
      F77NAME(enthal_rp10)
      (tp, rholp, arr_molfrac, hl_refpropp);
      tab_propder[NEPTUNE::d_h_l_sat_d_p] = (refprop_nrj_2_eos(hl_refpropp) - refprop_nrj_2_eos(hl_refprop)) / delta;
    }
    if (indic[7] == 1)
    {
      F77NAME(enthal_rp10)
      (tp, rhovp, arr_molfrac, hv_refpropp);
      tab_propder[NEPTUNE::d_h_v_sat_d_p] = (refprop_nrj_2_eos(hv_refpropp) - refprop_nrj_2_eos(hv_refprop)) / delta;
    }
    if (indic[8] == 1)
    {
      F77NAME(cvcp_rp10)
      (tp, rholp, arr_molfrac, cv, cplp);
      tab_propder[NEPTUNE::d_cp_l_sat_d_p] = (refprop_nrj_2_eos(cplp) - refprop_nrj_2_eos(cpl)) / delta;
    }
    if (indic[9] == 1)
    {
      F77NAME(cvcp_rp10)
      (tp, rhovp, arr_molfrac, cv, cpvp);
      tab_propder[NEPTUNE::d_cp_v_sat_d_p] = (refprop_nrj_2_eos(cpvp) - refprop_nrj_2_eos(cpv)) / delta;
    }
    if (indic[10] == 1)
    {
      double p_refpropm = p_refprop * (1 - epsilon);
      double tm = 0.0, rholm = 0.0, rhovm = 0.0;
      F77NAME(satp_rp10)
      (p_refpropm, arr_molfrac, kph, tm, rholm, rhovm, xliq, xvapint,
       ierr, herr, sizeof(herr) - 1);
      tab_propder2[NEPTUNE::d2_T_sat_d_p_d_p] = (tp - 2.0 * t + tm) / (delta * delta);
      if (ierr != 0)
        err = worst_internal_error(err, generate_error(ierr, herr));
    }

    return err;
  }

  EOS_Error EOS_Refprop10::compute(const EOS_Field &p,
                                   EOS_Fields &r,
                                   EOS_Error_Field &errfield) const
  {
    if (p.get_property_number() != NEPTUNE::p)
    { // Calcul non optimise
      return EOS_Fluid::compute(p, r, errfield);
    }
    else
    {
      const int nb_fields = r.size();
      const int sz = errfield.size();
      errfield = EOS_Internal_Error::OK;
      EOS_Internal_Error err;
      ArrOfInt err_data(sz);
      EOS_Error_Field err_field2(err_data);
      vector<double> vec_prop(NUMAXPROP, 0.e0);
      double *tab_prop = &vec_prop[0];
      vector<double> vec_propder(NUMAXPROP, 0.e0);
      double *tab_propder = &vec_propder[0];
      vector<double> vec_propder2(NUMAXPROP, 0.e0);
      double *tab_propder2 = &vec_propder[0];
      int indic[11];

      // reset fluid with setup
      err = callSetup();
      if (err.generic_error() != EOS_Error::good)
        return errfield.find_worst_error().generic_error();

      calrp_indic_sat_p(r, indic);
      EOS_Field rk;
      EOS_Property prop;
      EOS_Property tab_pos[NBMAXFIELD], tab_posder[NBMAXFIELD], tab_der2_pos[NBMAXFIELD];

      // tab_pos sert a etablir une correspondance entre la liste de champs "r" et les proprietes
      for (int k = 0; k < nb_fields; k++)
      {
        rk = r[k];
        prop = rk.get_property_number();
        if (prop >= NEPTUNE::firstSatProperty && prop <= NEPTUNE::lastSatProperty)
        {
          if (prop < EOS_TSATPROPDER)

          {
            tab_pos[k] = prop;
            tab_posder[k] = NEPTUNE::NotASatProperty;
            tab_der2_pos[k] = NEPTUNE::NotASatProperty;
          }
          else if (prop < EOS_TSATPROPDER2)
          {
            tab_pos[k] = NEPTUNE::NotASatProperty;
            tab_posder[k] = prop;
            tab_der2_pos[k] = NEPTUNE::NotASatProperty;
          }
          else
          {
            tab_pos[k] = NEPTUNE::NotASatProperty;
            tab_posder[k] = NEPTUNE::NotASatProperty;
            tab_der2_pos[k] = prop;
          }
        }
        else
        {
          tab_pos[k] = NEPTUNE::NotASatProperty;
          tab_posder[k] = NEPTUNE::NotASatProperty;
          tab_der2_pos[k] = NEPTUNE::NotASatProperty;
        }
      }

      for (int i = 0; i < sz; i++)
      {
        err = calrp_optim_sat_p(p[i], tab_prop, tab_propder, tab_propder2, indic);
        errfield.set(i, err);
        for (int k = 0; k < nb_fields; k++)
        {
          rk = r[k];
          if (tab_pos[k] != NEPTUNE::NotASatProperty)
            rk[i] = tab_prop[tab_pos[k]];
          else if (tab_posder[k] != NEPTUNE::NotASatProperty)
            rk[i] = tab_propder[tab_posder[k]];
          else if (tab_der2_pos[k] != NEPTUNE::NotASatProperty)
            rk[i] = tab_propder2[tab_der2_pos[k]];
        }
      }
      return errfield.find_worst_error().generic_error();
    }
  }

  void EOS_Refprop10::calrp_indic_pt(EOS_Fields &r, int *indic) const
  // indic[0]   tprho
  // indic[1]   therm2
  // indic[2]   enthal
  // indic[3]   therm2 en differences finies
  // indic[4]   dhd1
  // indic[5]   trnprp
  // indic[6]   stn
  // indic[7]   trnprp en differences finies
  {
    for (int i = 0; i <= 9; i++)
      indic[i] = 0;
    const int nb_fields = r.size();
    for (int k = 0; k < nb_fields; k++)
    {
      EOS_Field rk = r[k];
      EOS_Property prop = rk.get_property_number();
      indic[0] = 1;
      switch (prop)
      {
      case NEPTUNE::u:
      case NEPTUNE::s:
      case NEPTUNE::cp:
      case NEPTUNE::cv:
      case NEPTUNE::w:
      case NEPTUNE::beta:
        indic[1] = 1;
        break;
      case NEPTUNE::h:
        indic[2] = 1;
        break;
      case NEPTUNE::mu:
      case NEPTUNE::lambda:
        indic[5] = 1;
        break;
      case NEPTUNE::sigma:
        indic[6] = 1;
        break;
      case NEPTUNE::d_rho_d_T_p:
      case NEPTUNE::d_rho_d_p_T:
      case NEPTUNE::d_s_d_p_T:
      case NEPTUNE::d_cp_d_p_T:
      case NEPTUNE::d_cv_d_p_T:
      case NEPTUNE::d_w_d_p_T:
      case NEPTUNE::d_s_d_T_p:
      case NEPTUNE::d_cp_d_T_p:
      case NEPTUNE::d_cv_d_T_p:
      case NEPTUNE::d_w_d_T_p:
        indic[1] = 1;
        indic[3] = 1;
        break;
      case NEPTUNE::d_h_d_T_p:
      case NEPTUNE::d_h_d_p_T:
        indic[4] = 1;
        break;
      case NEPTUNE::d_mu_d_T_p:
      case NEPTUNE::d_mu_d_p_T:
      case NEPTUNE::d_lambda_d_T_p:
      case NEPTUNE::d_lambda_d_p_T:
        indic[5] = 1;
        indic[7] = 1;
        break;
      default:
        break;
      }
    }
  }

  inline EOS_Internal_Error EOS_Refprop10::calrp_optim_pt(double p, double T,
                                                          double *tab_prop,
                                                          double *tab_propder,
                                                          int *indic) const
  {
    int kph;
    double s, cp, cv, w, mu, lambda, beta, sigma;
    double e, rhol, rhov, hjt, pi, Z, h0;
    double A, G, xkappa, dPdrho, d2PdD2, dPT, drhodT, drhodP, spare1, spare2, spare3, spare4;
    double d_rho_d_p = 0.e0;
    double d_rho_d_T = 0.e0;
    double epsilon = 1e-6;
    EOS_Internal_Error err = EOS_Internal_Error::OK;
    EOS_Internal_Error err2;

    double p_refprop = pa2kpa(p);
    double h_refprop = 0.e0;
    double rho_refprop = 0.e0;
    err = calrp_kph(p, h, kph);
    if (err.generic_error() == EOS_Error::bad)
      return err;

    int ierr;
    char herr[HC255];
    memset(herr, '\0', sizeof(herr));

    if (indic[0] == 1)
    {
      int j = 0;
      int mph = -kph;
      F77NAME(tprho_rp10)
      (T, p_refprop, arr_molfrac, mph, j, rho_refprop,
       ierr, herr, sizeof(herr) - 1);
      tab_prop[NEPTUNE::rho] = refprop_rho_2_eos(rho_refprop);
      if (ierr != 0)
        err = worst_internal_error(err, generate_error(ierr, herr));
    }

    if (indic[1] == 1)
    {
      F77NAME(therm2_rp10)
      (T, rho_refprop, arr_molfrac, pi, e, h0, s, cv, cp,
       w, Z, hjt, A, G, xkappa, beta, dPdrho, d2PdD2, dPT, drhodT, drhodP,
       spare1, spare2, spare3, spare4);
      tab_prop[NEPTUNE::u] = refprop_nrj_2_eos(e);
      tab_prop[NEPTUNE::s] = refprop_nrj_2_eos(s);
      tab_prop[NEPTUNE::cp] = refprop_nrj_2_eos(cp);
      tab_prop[NEPTUNE::cv] = refprop_nrj_2_eos(cv);
      tab_prop[NEPTUNE::w] = w;
      tab_prop[NEPTUNE::beta] = beta;
      d_rho_d_p = refprop_rho_2_eos(drhodP) / kpa2pa(1);
      d_rho_d_T = drhodT;
      tab_propder[NEPTUNE::d_rho_d_p_T] = d_rho_d_p;
      tab_propder[NEPTUNE::d_rho_d_T_p] = refprop_rho_2_eos(d_rho_d_T);
    }

    if (indic[2] == 1)
    {
      F77NAME(enthal_rp10)
      (T, rho_refprop, arr_molfrac, h_refprop);
      tab_prop[NEPTUNE::h] = refprop_nrj_2_eos(h_refprop);
    }

    double Tp = T * (1 + epsilon);
    double delta_T = Tp - T;
    double rhop = rho_refprop * (1 + epsilon);
    double delta_rho = refprop_rho_2_eos(rhop - rho_refprop);
    if (indic[3] == 1)
    {
      double ep, sp, cvp, cpp, wp, betap;
      F77NAME(therm2_rp10)
      (Tp, rho_refprop, arr_molfrac, pi, ep, h0, sp, cvp, cpp, wp, Z, hjt, A, G,
       xkappa, betap, dPdrho, d2PdD2, dPT, drhodT, drhodP,
       spare1, spare2, spare3, spare4);
      double d_cp_d_T = (refprop_nrj_2_eos(cpp) - refprop_nrj_2_eos(cp)) / delta_T;
      double d_cv_d_T = (refprop_nrj_2_eos(cvp) - refprop_nrj_2_eos(cv)) / delta_T;
      double d_s_d_T = (refprop_nrj_2_eos(sp) - refprop_nrj_2_eos(s)) / delta_T;
      double d_w_d_T = (wp - w) / delta_T;
      // double d_beta_d_T = (betap-beta)/delta_T;

      F77NAME(therm2_rp10)
      (T, rhop, arr_molfrac, pi, ep, h0, sp, cvp, cpp, wp, Z, hjt, A, G,
       xkappa, beta, dPdrho, d2PdD2, dPT, drhodT, drhodP,
       spare1, spare2, spare3, spare4);
      double d_cp_d_rho = (refprop_nrj_2_eos(cpp) - refprop_nrj_2_eos(cp)) / delta_rho;
      double d_cv_d_rho = (refprop_nrj_2_eos(cvp) - refprop_nrj_2_eos(cv)) / delta_rho;
      double d_s_d_rho = (refprop_nrj_2_eos(sp) - refprop_nrj_2_eos(s)) / delta_rho;
      double d_w_d_rho = (wp - w) / delta_rho;

      tab_propder[NEPTUNE::d_cp_d_p_T] = d_cp_d_rho * d_rho_d_p;
      tab_propder[NEPTUNE::d_cv_d_p_T] = d_cv_d_rho * d_rho_d_p;
      tab_propder[NEPTUNE::d_s_d_p_T] = d_s_d_rho * d_rho_d_p;
      tab_propder[NEPTUNE::d_w_d_p_T] = d_w_d_rho * d_rho_d_p;
      tab_propder[NEPTUNE::d_cp_d_T_p] = d_cp_d_T + d_cp_d_rho * refprop_rho_2_eos(d_rho_d_T);
      tab_propder[NEPTUNE::d_cv_d_T_p] = d_cv_d_T + d_cv_d_rho * refprop_rho_2_eos(d_rho_d_T);
      tab_propder[NEPTUNE::d_s_d_T_p] = d_s_d_T + d_s_d_rho * refprop_rho_2_eos(d_rho_d_T);
      tab_propder[NEPTUNE::d_w_d_T_p] = d_w_d_T + d_w_d_rho * refprop_rho_2_eos(d_rho_d_T);
    }

    if (indic[4] == 1)
    {
      double dhdt_d, dhdt_p, dhdd_t, dhdd_p, dhdp_t, dhdp_d;
      F77NAME(dhd1_rp10)
      (T, rho_refprop, arr_molfrac, dhdt_d, dhdt_p, dhdd_t, dhdd_p, dhdp_t, dhdp_d);
      tab_propder[NEPTUNE::d_h_d_p_T] = refprop_nrj_2_eos(dhdp_t) / kpa2pa(1);
      tab_propder[NEPTUNE::d_h_d_T_p] = refprop_nrj_2_eos(dhdt_p);
    }

    if (indic[5] == 1)
    {
      F77NAME(trnprp_rp10)
      (T, rho_refprop, arr_molfrac, mu, lambda, ierr, herr, sizeof(herr) - 1);
      tab_prop[NEPTUNE::mu] = micropa2pa(mu);
      tab_prop[NEPTUNE::lambda] = lambda;
      if (ierr != 0)
        err = worst_internal_error(err, generate_error(ierr, herr));
    }

    if (indic[6] == 1)
    {
      F77NAME(stn_rp10)
      (T, rhol = 0.0, rhov = 0.0, arr_molfrac, arr_molfrac, sigma,
       ierr, herr, sizeof(herr) - 1);
      //        if (ierr != 0) err = worst_internal_error(err, generate_error(ierr, herr));
      tab_prop[NEPTUNE::sigma] = sigma;
    }

    if (indic[7] == 1)
    {
      double lambdap, mup;
      F77NAME(trnprp_rp10)
      (Tp, rho_refprop, arr_molfrac, mup, lambdap,
       ierr, herr, sizeof(herr) - 1);
      if (ierr != 0)
        err = worst_internal_error(err, generate_error(ierr, herr));
      double d_mu_d_T = (micropa2pa(mup) - micropa2pa(mu)) / delta_T;
      double d_lambda_d_T = (lambdap - lambda) / delta_T;

      F77NAME(trnprp_rp10)
      (T, rhop, arr_molfrac, mup, lambdap,
       ierr, herr, sizeof(herr) - 1);
      if (ierr != 0)
        err = worst_internal_error(err, generate_error(ierr, herr));
      double d_mu_d_rho = (micropa2pa(mup) - micropa2pa(mu)) / delta_rho;
      double d_lambda_d_rho = (lambdap - lambda) / delta_rho;

      tab_propder[NEPTUNE::d_mu_d_T_p] = d_mu_d_T + d_mu_d_rho * refprop_rho_2_eos(d_rho_d_T);
      tab_propder[NEPTUNE::d_mu_d_p_T] = d_mu_d_rho * refprop_rho_2_eos(d_rho_d_p);
      tab_propder[NEPTUNE::d_lambda_d_T_p] = d_lambda_d_T + d_lambda_d_rho * refprop_rho_2_eos(d_rho_d_T);
      tab_propder[NEPTUNE::d_lambda_d_p_T] = d_lambda_d_rho * refprop_rho_2_eos(d_rho_d_p);
    }

    return err;
  }

}
