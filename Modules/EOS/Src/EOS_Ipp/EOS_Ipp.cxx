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

/*
 * EOS_Ipp.cxx
 *
 *  Created on: 16 juin 2010
 */

#include "EOS_Ipp.hxx"
#include "EOS/API/EOS.hxx"
#include "EOS/API/EOS_Field.hxx"
#include "EOS/API/EOS_Fields.hxx"
#include "EOS/API/EOS_Config.hxx"
#include <fstream>
#include <iostream> // pour std::cerr
#include <set>
#include <string>
#include <vector>
#define DBL_EPSILON 1e-9

namespace NEPTUNE_EOS
{
  const AString EOS_Ipp::tablename("Interpolator");
  // Definition of internal errors in EOS_Ipp:
  // The following errors leads to EOS_Error=bad
  static const int EOS_Ipp_Error_base = 1000;
  const EOS_Internal_Error EOS_Ipp::OUT_OF_BOUNDS = EOS_Internal_Error(EOS_Ipp_Error_base + 0, EOS_Error::bad);
  const EOS_Internal_Error EOS_Ipp::INVERT_h_pT = EOS_Internal_Error(EOS_Ipp_Error_base + 1, EOS_Error::error);
  const EOS_Internal_Error EOS_Ipp::PROP_NOT_IN_DB = EOS_Internal_Error(EOS_Ipp_Error_base + 2, EOS_Error::error);
  const EOS_Internal_Error EOS_Ipp::MODEL_NOT_INIT = EOS_Internal_Error(EOS_Ipp_Error_base + 3, EOS_Error::error);
  void EOS_Ipp::describe_error(const EOS_Internal_Error ierr, AString &description) const
  {
    if (ierr == EOS_Internal_Error::OK)
      description = "ok";
    else if (ierr == OUT_OF_BOUNDS)
      description = "Value out of interpolation domain";
    else if (ierr == INVERT_h_pT)
      description = "Invert h(p,T) return no h value";
    else if (ierr == PROP_NOT_IN_DB)
      description = "Property values not in db file";
    else
    {
      description = "Error occured to database production with ";
      description += base_method;
      description += " method";
    }
  }

  const AString &EOS_Ipp::table_name() const
  {
    return tablename;
  }

  EOS_Ipp::EOS_Ipp() : r1_val(20, std::vector<double>(30, 0.0)),
                       r2_val(20, std::vector<double>(30, 0.0)),
                       nodes(3),
                       med_file("none"),
                       save_bound(0),
                       nodes_ph(2),
                       nodes_sat(1),
                       nodes_lim(1),
                       val_prop_ph(0),
                       val_prop_sat(0),
                       val_prop_lim(0),
                       connect_ph(0),
                       index_conn_ph(0),
                       connect_sat(0),
                       connect_lim(0),
                       n_p_ph(0),
                       n_h_ph(0),
                       n_p_satlim(0)

  {
  }

  EOS_Ipp::~EOS_Ipp()
  {
    if (obj_fluid != nullptr)
    {
      delete obj_fluid;
    }
  }

  static RegisteredClass &EOS_Ipp_create()
  {
    return *(new EOS_Ipp());
  }

  int EOS_Ipp::type_Id = (RegisterType("EOS_Ipp", "EOS_Fluid",
                                       "EOS Interpolator",
                                       sizeof(EOS_Ipp),
                                       EOS_Ipp_create));

  ostream &EOS_Ipp::print_On(ostream &stream) const
  {
    return stream;
  }

  istream &EOS_Ipp::read_On(istream &stream)
  {
    return stream;
  }

  const Type_Info &EOS_Ipp::get_Type_Info() const
  {
    return (Types_Info::instance())[type_Id];
  }

  int EOS_Ipp::init(const Strings &strings)
  {
    AString desc_err;
    FluidStr = AString("unknown");
    EOS_Error errM;
    int sz = strings.size();
    if (sz != 1)
    {
      cerr << " EOS_Ipp::init : Reference with size " << sz
           << " is not implemented for EOS_Ipp ! " << endl;
      assert(0);
      exit(-1);
      return EOS_Error::error;
    }

    // directory {DATA}/EOS_Ipp : med_file
    AString &file_name = strings[0];
    if (iret_eos_data_dir)
      return EOS_Error::error;
    med_file = eos_data_dir.c_str();
    med_file += "/EOS_Ipp/";
    med_file += file_name;

    // get method and reference used to generate med file: file name == "EOS_Method"."Liquid".med
    char *save_pt;
    method = strtok_r(file_name.aschar(), ".", &save_pt);
    reference = strtok_r(NULL, ".", &save_pt);

    if (method == "eos_igen_qi")
    {
      method = "";
      reference = "";
    }

    // load med file
    EOS_Med med(med_file);
    errM = med.read_File();
    if (errM != EOS_Error::good)
    {
      cerr << "Error : Open med file" << endl;
      cerr << "Error : EOS_Med::read_File" << endl;
      cerr << "Error : EOS_Ipp::init" << endl;
      return errM;
    }

    AString header;
    errM = med.read_header(header);
    if (errM != EOS_Error::good)
    {
      cerr << "Error : EOS_Med::read_header" << endl;
      cerr << "Error : EOS_Ipp::init" << endl;
      return errM;
    }
    AString hh = strtok(header.aschar(), ":");
    base_method = strtok(NULL, "/");
    base_reference = strtok(NULL, "/");

    errM = load_med_nodes(med);
    if (errM != EOS_Error::good)
    {
      cerr << "Error : EOS_Ipp::init" << endl;
      return errM;
    }

    errM = load_med_champ(med);
    if (errM != EOS_Error::good)
    {
      cerr << "Error : EOS_Ipp::load_med_champ" << endl;
      cerr << "Error : EOS_Ipp::init" << endl;
      return errM;
    }

    errM = load_med_scalar(med);
    errM = med.close_File();
    if (errM != EOS_Error::good)
    {
      cerr << "Error : Close med file" << endl;
      cerr << "Error : EOS_Med::close_File" << endl;
      cerr << "Error : EOS_Ipp::init" << endl;
      return errM;
    }

    // pretraitements (2D)
    if (index_conn_ph.size() != 0)
      f_mesh2r_mesh();
    
    if( connect_sat.size() != 0)
      f_mesh1r_mesh();
    return EOS_Error::ok;
  }

  //"values" contains properties to load
  int EOS_Ipp::init(const Strings &strings, const Strings &values)
  {
    EOS_Error errM;
    AString desc_err;
    FluidStr = AString("unknown");
    int sz = strings.size();
    if (sz != 1)
    {
      cerr << " EOS_Ipp::init : Reference with size " << sz << " is not implemented for EOS_Ipp ! " << endl;
      assert(0);
      exit(-1);
      return EOS_Error::error;
    }

    char *pshell;
    if ((pshell = getenv("NEPTUNE_EOS_DATA")))
      med_file = pshell;
    else if ((pshell = getenv("USER_EOS_DATA")))
      med_file = pshell;
    else
#ifdef EOS_DATA_DIR
      med_file = EOS_DATA_DIR;
#else
    {
      cerr << "Directory DATA is not set " << endl;
      exit(-1);
    }
#endif
    // directory {DATA}/EOS_Ipp : med_file
    med_file += "/EOS_Ipp/";
    // file : med_file
    AString &file_name = strings[0];
    med_file += file_name;

    // get method and reference used to generate med file: file name == "EOS_Method"."Liquid".med
    method = strtok(file_name.aschar(), ".");
    reference = strtok(NULL, ".");

    // load med file
    EOS_Med med(med_file);
    errM = med.read_File();
    if (errM != EOS_Error::good)
    {
      cerr << "Error : EOS_Med::read_File" << endl;
      cerr << "Error : EOS_Ipp::init" << endl;
      return errM;
    }

    AString header;
    errM = med.read_header(header);
    if (errM != EOS_Error::good)
    {
      cerr << "Error : EOS_Ipp::read_header" << endl;
      cerr << "Error : EOS_Ipp::init" << endl;
      return errM;
    }
    AString hh = strtok(header.aschar(), ":");
    base_method = strtok(NULL, "/");
    base_reference = strtok(NULL, "/");

    load_med_nodes(med);
    errM = load_med_champ(med, values);
    if (errM != EOS_Error::good)
    {
      cerr << "Error : EOS_Ipp::load_med_champ" << endl;
      cerr << "Error : EOS_Ipp::init" << endl;
      return errM;
    }
    load_med_scalar(med);
    med.close_File();

    // pretraitements (2D)
    if (index_conn_ph.size() != 0)
      f_mesh2r_mesh();

    if( connect_sat.size() != 0)
      f_mesh1r_mesh();
    return EOS_Error::ok;
  }

  EOS_Error EOS_Ipp::init_model(const std::string &model_name, const std::string &fluid_name, bool switch_comp_sat, bool swch_calc_deriv_fld)
  {
    obj_fluid = new EOS(model_name.c_str(), fluid_name.c_str());
    switch_model = true;
    switch_comp_sat_ = switch_comp_sat;
    swch_calc_deriv_fld_ = swch_calc_deriv_fld;
    // If we use the compute_ function (for debug) : allows to retrieve the bounds of one simulation
    save_bound = 1;

    // Change the bounds of the ipp for the one of the fluid
    obj_fluid->get_h_min(hmin);
    obj_fluid->get_h_max(hmax);
    obj_fluid->get_T_min(tmin);
    obj_fluid->get_T_max(tmax);
    obj_fluid->get_p_min(pmin);
    obj_fluid->get_p_max(pmax);
    return EOS_Error::good;
  }

  EOS_Error EOS_Ipp::compute_(const EOS_Field &pp,
                              const EOS_Field &hh,
                              EOS_Fields &r,
                              EOS_Error_Field &errfield) const
  {
    if (save_bound == 1) // a ne faire qu'une seule fois
    {
      hmin_cpt = 100000.;
      hmax_cpt = 0.;
      tmin_cpt = 100000.;
      tmax_cpt = 0.;
      pmin_cpt = 1000000000.;
      pmax_cpt = 0.;
      save_bound = 0;
    }
    // Displaying the bounds for each calculation
    /* std::cout << "The min and max of the calculations are:" << endl;
    std::cout << "pmin: " << pmin_cpt << endl;
    std::cout << "pmax: " << pmax_cpt << endl;
    std::cout << "hmin: " << hmin_cpt << endl;
    std::cout << "hmax: " << hmax_cpt << endl;
    std::cout << "tmin: " << tmin_cpt << endl;
    std::cout << "tmax: " << tmax_cpt << endl;*/
    // Updating the bounds
    for (int pts = 0; pts < pp.size(); pts++)
    {
      pmin_cpt = min(pmin_cpt, pp.get_data()[pts]);
      pmax_cpt = max(pmax_cpt, pp.get_data()[pts]);
      if (hh.get_property_name() == "T")
      {
        tmin_cpt = min(tmin_cpt, hh.get_data()[pts]); // Be careful, sometimes H = T.
        tmax_cpt = max(tmax_cpt, hh.get_data()[pts]);
      }
      if (hh.get_property_name() == "h")
      {
        hmin_cpt = min(hmin_cpt, hh.get_data()[pts]); // Be careful, sometimes H = T.
        hmax_cpt = max(hmax_cpt, hh.get_data()[pts]);
      }
    }
    // std::vector<std::vector<double>> r1_val; // defined with size 20*30
    // std::vector<std::vector<double>> r2_val; // defined with size 20*30

    // EOS_Error err = EOS_Fluid::compute(pp, hh, r, errfield); // debug : la calcul tourne t'il tjr ?
    EOS_Error err2 = obj_fluid->compute(pp, hh, r, errfield);
    // Remplissage de r1_val et r2_val
    for (int pts = 0; pts < pp.size(); pts++)
      for (int prop = 0; prop < r.size(); prop++)
        r1_val[prop][pts] = r[prop].get_data()[pts]; // debug : la calcul tourne t'il tjr ?
    // EOS_Error err2 = obj_fluid->compute(pp, hh, r, errfield);
    EOS_Error err = EOS_Fluid::compute(pp, hh, r, errfield);
    for (int pts = 0; pts < pp.size(); pts++)
      for (int prop = 0; prop < r.size(); prop++)
        r2_val[prop][pts] = r[prop].get_data()[pts];
    // calcul de l'erreur pour chaque prop et pts et renvoie du max
    double err_ipp_rp = 0;
    int propmax;
    if (err == EOS_Error::good && err2 == EOS_Error::good)
    {
      for (int prop = 0; prop < r.size(); prop++)
      {
        if (r[prop].get_property_name() == "T") // Updating the bound of T
        {
          for (int pts = 0; pts < pp.size(); pts++)
          {
            tmin_cpt = min(tmin_cpt, r[prop].get_data()[pts]);
            tmax_cpt = max(tmax_cpt, r[prop].get_data()[pts]);
          }
        }
        if (r[prop].get_property_name() == "d_sigma_d_p_h") // Updating the bound of Td_sigma_d_p_h
        {
          std::cout << "computation of d_sigma_d_p_h err ipp " << err << " err model" << err2 << endl;
          std::cout << "value of ipp " << r1_val[prop][0] << "and fluid : " << r2_val[prop][0];
        }
        double err_ipp_rp_prop = 0;
        for (int pts = 0; pts < pp.size(); pts++)
          err_ipp_rp_prop = max(err_ipp_rp_prop, abs(2 * (r1_val[prop][pts] - r2_val[prop][pts]) / (r1_val[prop][pts]))); // relative error
        if (err_ipp_rp_prop > err_ipp_rp)
        {
          err_ipp_rp = err_ipp_rp_prop;
          propmax = prop;
        }
      }
      std::cout << " Number of prop :  " << r.size() << " Number of calcul : " << pp.size() << endl;
      std::cout << "The error is " << err_ipp_rp << " for " << r[propmax].get_property_name() << endl;
    }
    else
    {
      std::cout << "Error in computation. err ipp " << err << " err model" << err2 << endl;
      return obj_fluid->compute(pp, hh, r, errfield);
    }
    return err2;
  }

  EOS_Error EOS_Ipp::compute_(const EOS_Field &p,
                              EOS_Fields &r,
                              EOS_Error_Field &errfield) const
  {

    // Affichage des bornes à chaque calcul
    std::cout << "The min and max of the calculations are:" << endl;
    std::cout << "pmin: " << pmin_cpt << endl;
    std::cout << "pmax: " << pmax_cpt << endl;
    // Updating the bounds
    for (int pts = 0; pts < p.size(); pts++)
    {
      pmin_cpt = min(pmin_cpt, p.get_data()[pts]);
      pmax_cpt = max(pmax_cpt, p.get_data()[pts]);
    }
    EOS_Error err = EOS_Fluid::compute(p, r, errfield);
    // Remplissage de r1_val et r2_val
    for (int pts = 0; pts < p.size(); pts++)
      for (int prop = 0; prop < r.size(); prop++)
        r1_val[prop][pts] = r[prop].get_data()[pts];
    EOS_Error err2 = obj_fluid->compute(p, r, errfield);
    for (int pts = 0; pts < p.size(); pts++)
      for (int prop = 0; prop < r.size(); prop++)
        r2_val[prop][pts] = r[prop].get_data()[pts];
    // calcul de l'erreur pour chaque prop et pts et renvoie du max
    double err_ipp_rp = 0;
    int propmax;
    for (int prop = 0; prop < r.size(); prop++)
    {
      double err_ipp_rp_prop = 0;
      for (int pts = 0; pts < p.size(); pts++)
        err_ipp_rp_prop = max(err_ipp_rp_prop, abs((r1_val[prop][pts] - r2_val[prop][pts]) / (1 * (r2_val[prop][pts] == 0) + r2_val[prop][pts])));
      if (err_ipp_rp_prop > err_ipp_rp)
      {
        err_ipp_rp = err_ipp_rp_prop;
        propmax = prop;
      }
    }
    std::cout << "The error is " << err_ipp_rp << " for " << r[propmax].get_property_name() << endl;
    std::cout << "The errors are :  IPP:" << err << "and FLUID : " << err2 << endl;
    return err2;
  }

  EOS_Error EOS_Ipp::compute(const EOS_Field &pp,
                             const EOS_Field &hh,
                             EOS_Fields &r,
                             EOS_Error_Field &errfield) const
  {
    EOS_Error err = EOS_Fluid::compute(pp, hh, r, errfield);
    if ((err != EOS_Error::good)) // if the calculation by ipp did not pass
    {
        std::cout << "Error detected : " << err << " ;";
        std::cout << "for the pair (" << pp.get_data()[0] << "," << hh.get_data()[0] << ") " << std::endl;
        //r.print_On(std::cout);
      if (obj_fluid == nullptr)
      {
        std::cerr << "Error: The interpolator fluid is not initialized. To continue the calculation, call the function init_model(). " << std::endl;
        return err;
      }
      err = obj_fluid->compute(pp, hh, r, errfield);
    }
    // errfield = EOS_Internal_Error::OK; 
    return err;
  }

  EOS_Error EOS_Ipp::compute(const EOS_Field &p,
                             EOS_Fields &r,
                             EOS_Error_Field &errfield) const
  {
    EOS_Error err = EOS_Fluid::compute(p, r, errfield);
    if ((err != EOS_Error::good)) // if the calculation by ipp did not pass
    {
      std::cout <<" Erreur EOS : " << err; 
      std::cout << "The point p= (" << p.get_data() << "), Did not produce a valid calculation. " << std::endl;
      r.print_On(std::cout);
      if (obj_fluid == nullptr)
      {
        std::cerr << "Error: The interpolator fluid is not initialized. To continue the calculation, call the function init_model().  " << std::endl;
        return err;
      }
      err = obj_fluid->compute(p, r, errfield);
    }
    return err;
  }

  // load de tous les champs
  EOS_Error EOS_Ipp::load_med_nodes(EOS_Med &med)
  {
    EOS_Error errM;
    Strings names(0);
    ArrOfInt dim(0);
    int nb_nodes = 0;

    errM = med.get_Maillages_Infos(names, dim);
    if (errM != EOS_Error::good)
    {
      cerr << "Error : EOS_Med::get_Maillages_Infos" << endl;
      return errM;
    }

    // pour chaque maillage get des infos
    int nb_ns = names.size();
    for (int i = 0; i < nb_ns; i++)
    {
      errM = med.get_entity_size(names[i], nb_nodes, MED_NODE, MED_NONE,
                                 MED_COORDINATE, MED_NO_CMODE);
      if (dim[i] == 1) // sat or spinodal
      {
        n_p_satlim.resize(nb_nodes);
        EOS_Field pf("P", "p", n_p_satlim);

        if (names[i] == "sat_domain") // saturation
        {
          nodes_sat[0] = pf;
          errM = med.get_nodes(names[i], dim[i], nodes_sat);
          if (errM != EOS_Error::good)
          {
            cerr << "Error : EOS_Med::get_nodes: Error in reading nodes of sat domain" << endl;
            return EOS_Error::error;
          }
          errM = med.get_Connectivity_1D(names[i], connect_sat);
          if (errM != EOS_Error::good)
          {
            cerr << "Error : EOS_Med::get_nodes: Error in reading connectivity nodes of sat domain" << endl;
            return EOS_Error::error;
          }
        }

        else if (names[i] == "lim_domain")
        {
          nodes_lim[0] = pf;
          errM = med.get_nodes(names[i], dim[i], nodes_lim);
          if (errM != EOS_Error::good)
          {
            cerr << "Error : EOS_Med::get_nodes: Error in reading nodes of lim domain" << endl;
            return EOS_Error::error;
          }
          errM = med.get_Connectivity_1D(names[i], connect_lim);
          if (errM != EOS_Error::good)
          {
            cerr << "Error : EOS_Med::get_nodes: Error in reading connectivity nodes of lim domain" << endl;
            return EOS_Error::error;
          }
        }

        else
        {
          cerr << "Error in loading saturation/spinodale values" << endl;
          return EOS_Error::error;
        }
      }

      else
      {
        n_p_ph.resize(nb_nodes);
        n_h_ph.resize(nb_nodes);

        EOS_Field pf("P", "p", n_p_ph);
        EOS_Field hf("h", "h", n_h_ph);

        nodes_ph[0] = pf;
        nodes_ph[1] = hf;

        errM = med.get_nodes(names[i], dim[i], nodes_ph);
        if (errM != EOS_Error::good)
        {
          cerr << "Error : EOS_Med::get_nodes" << endl;
          return EOS_Error::error;
        }

        errM = med.get_Connectivity_2D(names[i], index_conn_ph, connect_ph);
        if (errM != EOS_Error::good)
        {
          cerr << "Error : EOS_Med::get_Connectivity_2D" << endl;
          return EOS_Error::error;
        }
      }
    }

    return EOS_Error::good;
  }

  // load de tous les champs (== toutes les proprietes presentes)
  EOS_Error EOS_Ipp::load_med_champ(EOS_Med &med)
  {
    EOS_Error errM;
    int nb_champ = 0;
    int nprop_ph = 0;
    int nprop_sat = 0;
    int nprop_lim = 0;
    int nprop_all = 0;
    errM = med.get_number_champ(nb_champ);
    if (nb_champ < 1)
    {
      cerr << "Error : Bad number of field" << endl;
      return EOS_Error::error;
    }
    else if (errM != EOS_Error::good)
    {
      cerr << "Error : EOS_Med::get_number_champ" << endl;
      return EOS_Error::error;
    }

    // correction jp mars 2012 : pourquoi nb_champ/2 ??
    // int vectorsz = nb_champ/2;
    int vectorsz = nb_champ / 2 + 1;
    all_prop_val.resize(vectorsz);
    all_err_val.resize(vectorsz);

    for (int i = 0; i < nb_champ; i++)
    {
      AString name;
      AString m_ass;
      int nbcomp = 0;
      int type = 0;
      errM = med.get_Champ_Noeud_Infos(i, name, type, nbcomp, m_ass);
      if (errM != EOS_Error::good)
      {
        cerr << "Error : EOS_Med::get_Champ_Noeud_Infos" << endl;
        return EOS_Error::error;
      }

      if (type == 1) // float -> properties values
      {
        ArrOfDouble xval(nbcomp);
        all_prop_val[nprop_all] = xval;
        EOS_Field res(name.aschar(), name.aschar(), all_prop_val[nprop_all]);
        errM = med.get_Champ_Noeud(name, res);
        if (errM != EOS_Error::good)
        {
          cerr << "Error : EOS_Med::get_Champ_Noeud" << endl;
          return EOS_Error::error;
        }

        if (m_ass == "ph_domain")
        {
          nprop_ph++;
          val_prop_ph.resize(nprop_ph);
          val_prop_ph[nprop_ph - 1] = res;
        }
        else if (m_ass == "sat_domain")
        {
          nprop_sat++;
          val_prop_sat.resize(nprop_sat);
          val_prop_sat[nprop_sat - 1] = res;
        }
        else if (m_ass == "lim_domain")
        {
          nprop_lim++;
          val_prop_lim.resize(nprop_lim);
          val_prop_lim[nprop_lim - 1] = res;
        }
        nprop_all++;
      }

      else
      {
        ArrOfInt err(nbcomp);
        EOS_Error_Field errf(err);
        errM = med.get_ErrChamp_Noeud(name, errf);
        if (errM != EOS_Error::good)
        {
          cerr << "EOS_Med::get_ErrChamp_Noeud" << endl;
          return EOS_Error::error;
        }
        if (name[0] == 'I' && name[1] == 'E')
        { // "IE propname" --> "propname"
          AString name_field = name;
          name_field.remove(0);
          name_field.remove(0);
          name_field.remove(0);
          errf.set_name(name_field.aschar());
        }
        else
          errf.set_name(name.aschar());

        if (m_ass == "ph_domain")
          node_err2mesh_err(errf);
        else if (m_ass == "sat_domain")
          node_err2segm_err(errf, 0);
        else if (m_ass == "lim_domain")
          node_err2segm_err(errf, 1);
      }
    }

    // Initialisation des dictionnaires :

    // domaine ph
    AString name_prop;
    for (int n_prop = 0; n_prop < val_prop_ph.size(); n_prop++)
    {
      name_prop = val_prop_ph[n_prop].get_property_name();
      Ipp_Prop_ph[name_prop] = n_prop;
    }
    // domaine sat
    for (int n_prop = 0; n_prop < val_prop_sat.size(); n_prop++)
    {
      name_prop = val_prop_sat[n_prop].get_property_name();
      Ipp_Prop_sat[name_prop] = n_prop;
    }

    // domaine lim
    for (int n_prop = 0; n_prop < val_prop_lim.size(); n_prop++)
    {
      name_prop = val_prop_lim[n_prop].get_property_name();
      Ipp_Prop_lim[name_prop] = n_prop;
    }

    /*for (auto toto : Ipp_Prop_sat)
      std::cout << toto.first << " et  " << toto.second << std::endl; */
    // Display interpolated quantities at initialization

    return EOS_Error::good;
  }

  // load selectif : uniquement les champs demandes par l'user
  EOS_Error EOS_Ipp::load_med_champ(EOS_Med &med, const Strings &properties)
  {
    EOS_Error errM;
    int nb_champ = 0;
    int nprop_ph = 0;
    int nprop_sat = 0;
    int nprop_lim = 0;

    errM = med.get_number_champ(nb_champ);
    if (nb_champ < 0)
    {
      cerr << "Error : Bad number of champs" << endl;
      return EOS_Error::error;
    }

    int nb_ps = properties.size();

    for (int i = 0; i < nb_champ; i++)
    {
      AString name;
      AString m_ass;
      int nbcomp = 0;
      int type = 0;
      errM = med.get_Champ_Noeud_Infos(i, name, type, nbcomp, m_ass);

      char namecov[PROPNAME_MSIZE];
      eostp_strcov(name.aschar(), namecov); // namecov : base alphanumérical property

      if (errM != EOS_Error::good)
      {
        cerr << "Error : EOS_Med::get_Champ_Noeud_Infos" << endl;
        return EOS_Error::error;
      }

      if (type == 1) // float -> properties values
      {              // get property values ?
        int j = 0;
        int found = 0;
        while (j < nb_ps && !found)
        {
          if (eostp_strcmp(properties[j].aschar(), namecov) == 0)
            found = 1;
          j++;
        }

        if (found)
        {
          ArrOfDouble xval(nbcomp);
          all_prop_val.push_back(xval);
          EOS_Field res(namecov, namecov, all_prop_val[all_prop_val.size() - 1]);

          med.get_Champ_Noeud(name, res);
          if (m_ass == "ph_domain")
          {
            nprop_ph++;
            val_prop_ph.resize(nprop_ph);
            val_prop_ph[nprop_ph - 1] = res;
          }
          else if (m_ass == "sat_domain")
          {
            nprop_sat++;
            val_prop_sat.resize(nprop_sat);
            val_prop_sat[nprop_sat - 1] = res;
          }
          else if (m_ass == "lim_domain")
          {
            nprop_lim++;
            val_prop_lim.resize(nprop_lim);
            val_prop_lim[nprop_lim - 1] = res;
          }
        }
      }

      else
      { // get property values ?
        int j = 0;
        int found = 0;
        while (j < nb_ps && !found)
        {
          if (eostp_strcmp(properties[j].aschar(), namecov) == 0)
            found = 1;
          j++;
        }

        if (found)
        {
          ArrOfInt err(nbcomp);
          EOS_Error_Field errf(err);

          med.get_ErrChamp_Noeud(name, errf);
          errf.set_name(namecov);

          if (m_ass == "ph_domain")
            node_err2mesh_err(errf);
          else if (m_ass == "sat_domain")
            node_err2segm_err(errf, 0);
          else if (m_ass == "lim_domain")
            node_err2segm_err(errf, 1);
        }
      }
    }

    // Warning if property in "properties" list not implemented
    char *pcha;
    int nb_vpp = val_prop_ph.size();
    int nb_vps = val_prop_sat.size();
    int nb_vpl = val_prop_lim.size();
    for (int i = 0; i < nb_ps; i++)
    {
      int found = 0;
      int j = 0;
      pcha = properties[i].aschar();
      while (j < nb_vpp && !found)
      {
        if (eostp_strcmp(pcha, val_prop_ph[j].get_propname_int().aschar()) == 0)
          found = 1;
        j++;
      }

      if (!found)
      {
        j = 0;
        while (j < nb_vps && !found)
        {
          if (eostp_strcmp(pcha, val_prop_sat[j].get_propname_int().aschar()) == 0)
            found = 1;
          j++;
        }

        if (!found)
        {
          j = 0;
          while (j < nb_vpl && !found)
          {
            if (eostp_strcmp(pcha, val_prop_lim[j].get_propname_int().aschar()) == 0)
              found = 1;
            j++;
          }
          if (!found)
            cerr << "WARNING ! Property " << properties[i].aschar()
                 << "not implemented for this method" << endl;
        }
      }
    }
    return EOS_Error::good;
  }

  EOS_Error EOS_Ipp::load_med_scalar(EOS_Med &med)
  {
    EOS_Error err;

    AString str_pmin("pmin");
    err = med.get_Scalar_Float(str_pmin, pmin);
    if (err != EOS_Error::good)
    {
      cerr << "EOS_Ipp::load_med_scalar : impossible to read  pmin scalar  in MED file" << endl;
      return err;
    }

    AString str_pmax("pmax");
    err = med.get_Scalar_Float(str_pmax, pmax);
    if (err != EOS_Error::good)
    {
      cerr << "EOS_Ipp::load_med_scalar : impossible to read  pmax scalar  in MED file" << endl;
      return err;
    }

    AString str_hmin("hmin");
    err = med.get_Scalar_Float(str_hmin, hmin);
    if (err != EOS_Error::good)
    {
      cerr << "EOS_Ipp::load_med_scalar : impossible to read  hmin scalar  in MED file" << endl;
      return err;
    }

    AString str_hmax("hmax");
    err = med.get_Scalar_Float(str_hmax, hmax);
    if (err != EOS_Error::good)
    {
      cerr << "EOS_Ipp::load_med_scalar : impossible to read  hmax scalar  in MED file" << endl;
      return err;
    }

    AString str_tmin("tmin");
    err = med.get_Scalar_Float(str_tmin, tmin);
    if (err != EOS_Error::good)
    {
      cerr << "EOS_Ipp::load_med_scalar : impossible to read  tmin scalar  in MED file" << endl;
      return err;
    }

    AString str_tmax("tmax");
    err = med.get_Scalar_Float(str_tmax, tmax);
    if (err != EOS_Error::good)
    {
      cerr << "EOS_Ipp::load_med_scalar : impossible to read  tmax scalar  in MED file" << endl;
      return err;
    }

    AString str_dp("delta_p");
    err = med.get_Scalar_Float(str_dp, delta_p_f);
    if (err != EOS_Error::good)
    {
      cerr << "EOS_Ipp::load_med_scalar : impossible to read  delta_p scalar  in MED file" << endl;
      return err;
    }

    nb_p_virtual = round((pmax - pmin) / delta_p_f);

    AString str_dh("delta_h");
    err = med.get_Scalar_Float(str_dh, delta_h_f);
    if (err != EOS_Error::good)
    {
      cerr << "EOS_Ipp::load_med_scalar : impossible to read  delta_h scalar  in MED file" << endl;
      return err;
    }

    nb_h_virtual = round((hmax - hmin) / delta_h_f);

    AString str_tcrit("tcrit");
    err = med.get_Scalar_Float(str_tcrit, tcrit);
    if (err != EOS_Error::good)
    {
      cerr << "EOS_Ipp::load_med_scalar : impossible to read  tcrit scalar  in MED file" << endl;
      return err;
    }

    AString str_pcrit("pcrit");
    err = med.get_Scalar_Float(str_pcrit, pcrit);
    if (err != EOS_Error::good)
    {
      cerr << "EOS_Ipp::load_med_scalar : impossible to read  pcrit scalar  in MED file" << endl;
      return err;
    }

    AString str_hcrit("hcrit");
    err = med.get_Scalar_Float(str_hcrit, hcrit);
    if (err != EOS_Error::good)
    {
      cerr << "EOS_Ipp::load_med_scalar : impossible to read  hcrit scalar  in MED file" << endl;
      return err;
    }

    // save of the bounds
    hmin_ipp = hmin;
    hmax_ipp = hmax;
    tmin_ipp = tmin;
    tmax_ipp = tmax;
    pmin_ipp = pmin;
    pmax_ipp = pmax;
    return err;
  }

  void EOS_Ipp::linear_interpolator(double p, double &res) const
  {
    // nodes
    //  fields[0] = 2 valeurs en p
    //  fields[1] = 2 valeurs de la propriete
    //  p = valeur p du point à interpoler
    //  res = resultat de l'interpolation pour la propriete (prop_name se trouve dans fields[1]

    // Formule interpolation
    // f(p*) = C1f1 + C2f2
    // avec :   C1,C2 : valeur à calculer
    //                          C1 = 1-p*
    //                          C2 = p*
    //                  f1,f2 : valeur de la propriete en C1 et C2
    //                  p* = (p-p1)/(p2-p1)

    double C1, C2;
    double pcal;

    // EOS_Fields nodes2 = *nodes;
    EOS_Fields nodes2(3);

    pcal = (p - nodes2[0].get_data().get_value_at(0)) /
           (nodes2[0].get_data().get_value_at(1) - nodes2[0].get_data().get_value_at(0));
    C1 = 1.e0 - pcal;
    C2 = pcal;

    res = C1 * nodes[1][0] + C2 * nodes[1][1];
  }

  double EOS_Ipp::linear_interpolator(double p, EOS_Fields &segmval) const
  {
    // nodes
    //  fields[0] = 2 valeurs en p
    //  fields[1] = 2 valeurs de la propriete
    //  p = valeur p du point à interpoler
    //  res = resultat de l'interpolation pour la propriete (prop_name se trouve dans fields[1]

    // Formule interpolation
    // f(p*) = C1f1 + C2f2
    // avec :   C1,C2 : valeur à calculer
    //                          C1 = 1-p*
    //                          C2 = p*
    //                  f1,f2 : valeur de la propriete en C1 et C2
    //                  p* = (p-p1)/(p2-p1)

    double res;
    double C1, C2;
    double pcal;

    pcal = (p - segmval[0][0]) / (segmval[0][1] - segmval[0][0]);
    C1 = 1.e0 - pcal;
    C2 = pcal;

    res = C1 * segmval[1][0] + C2 * segmval[1][1];

    return res;
  }

 /* void EOS_Ipp::bilinear_interpolator(double p, double h, double &res) const
  {

    // nodes
    //  fields[0] = 4 valeurs en p
    //  fields[1] = 4 valeurs en h
    //  fields[2] = 4 valeurs de la propriete (pour chaque point)
    //  p = valeur p du point à interpoler
    //  h = valeur h du point à interpoler
    //  res = resultat de l'interpolation pour la propriete (prop_name se trouve dans fields[2]

    // Formule interpolation
    //  f(h*,p*) = somme(Cifi)
    //  avec :    i = 1,...,4 == noeud
    //                   fi = valeur de la propriete au noeud i
    //                   Ci = à calculer
    //                           C4 = h*p*
    //                           C3 = p* - C4
    //                           C2 = h* - C4
    //                           C1 = 1-p*-C2
    //                   p* = (p-p1)/(p3-p1)      => pcal
    //                   h* = (h-h1)/(h2-h1)      => hcal

    double C1, C2, C3, C4;
    double pcal, hcal;

    pcal = (p - nodes[0][0]) / (nodes[0][2] - nodes[0][0]);
    hcal = (h - nodes[1][0]) / (nodes[1][1] - nodes[1][0]);

    C4 = pcal * hcal;
    C3 = pcal - C4;
    C2 = hcal - C4;
    C1 = 1.e0 - pcal - C2;

    res = (C1 * nodes[2][0]) + (C2 * nodes[2][1]) + (C3 * nodes[2][2]) + (C4 * nodes[2][3]);
  }
*/
  double EOS_Ipp::bilinear_interpolator(double p, double h, EOS_Fields &cellval) const
  {
    // nodes
    //  fields[0] = 4 valeurs en p
    //  fields[1] = 4 valeurs en h
    //  fields[2] = 4 valeurs de la propriete (pour chaque point)
    //  p = valeur p du point à interpoler
    //  h = valeur h du point à interpoler
    //  res = resultat de l'interpolation pour la propriete (prop_name se trouve dans fields[2]

    // Formule interpolation
    //  f(h*,p*) = somme(Cifi)
    //  avec :    i = 1,...,4 == noeud
    //                   fi = valeur de la propriete au noeud i
    //                   Ci = à calculer
    //                           C4 = h*p*
    //                           C3 = p* - C4
    //                           C2 = h* - C4
    //                           C1 = 1-p*-C2
    //                   p* = (p-p1)/(p3-p1)      => pcal
    //                   h* = (h-h1)/(h2-h1)      => hcal

    double res;
    double C1, C2, C3, C4;
    double pcal, hcal;

    pcal = (p - cellval[0][0]) / (cellval[0][3] - cellval[0][0]);
    hcal = (h - cellval[1][0]) / (cellval[1][1] - cellval[1][0]);

    C3 = pcal * hcal;
    C4 = pcal - C3;
    C2 = hcal - C3;
    C1 = 1.e0 - pcal - C2;

    res = (C1 * cellval[2][0]) + (C2 * cellval[2][1]) + (C3 * cellval[2][2]) + (C4 * cellval[2][3]);

    return res;
  }
  void EOS_Ipp::f_mesh1r_mesh()
  {
    unsigned int nb_p_nodes = round((pmax_ipp - pmin_ipp) /  delta_p_f);
    fnodes2pnodes.resize(nb_p_nodes);

    int nb_segm = connect_sat.size() / 2;

    for (int jseg = 0; jseg < nb_segm;jseg++)
    {
        double p_min_cell = nodes_sat[0][jseg];
        double p_max_cell = nodes_sat[0][jseg + 1];
        unsigned int i_p_min = round((p_min_cell - pmin_ipp)/ delta_p_f);
        unsigned int i_p_max = round((p_max_cell - pmin_ipp)/ delta_p_f);
      for (unsigned int i_p = i_p_min; i_p <i_p_max; i_p++)
      {
        fnodes2pnodes[i_p] = jseg;
      }
    }
  }
  // correspondance noeud fictif avec 1 noeud du polygone
  // On stocke la valeur de l'index pour pouvoir retrouver polygone dans connect_ph
  //(besoin des 4 noeuds pour les interpolations)
  // facon de faire à revoir peu robuste (Cf. condition d'enregistrement de fnodes2phnodes[l])
  void EOS_Ipp::f_mesh2r_mesh()
  {
    unsigned int nb_cell = index_conn_ph.size() - 1;
    corners.resize(4 * nb_cell);

    unsigned int nb_h_nodes = round((hmax_ipp - hmin_ipp) / delta_h_f);
    unsigned int nb_p_nodes = round((pmax_ipp - pmin_ipp) / delta_p_f);

    fnodes2phnodes.resize(nb_h_nodes * nb_p_nodes);

    for (unsigned int i_med_cell = 0; i_med_cell < nb_cell; i_med_cell++)
    {
      unsigned int nb_node_in_cell = index_conn_ph[i_med_cell + 1] - index_conn_ph[i_med_cell];
      unsigned int num_first_node = index_conn_ph[i_med_cell];
      unsigned int node_1, node_2, node_3;
      unsigned int node_0 = connect_ph[num_first_node];

      // Si plus de 4 sommets dans la maille, on parcourt les sommets de long des arêtes dans le
      // sens trigo et on reconnait le premier coin quand P devient constant, puis le deuxième quand
      // h devient constant, et le troisième quand p redevient constant
      if (nb_node_in_cell > 4)
      {
        unsigned int num_node_in_cell = num_first_node + 1;
        while (nodes_ph[0][num_node_in_cell] - nodes_ph[0][num_node_in_cell - 1] < DBL_EPSILON)
          num_node_in_cell++;
        node_1 = connect_ph[num_node_in_cell];

        while (nodes_ph[1][num_node_in_cell] - nodes_ph[1][num_node_in_cell - 1] < DBL_EPSILON)
          num_node_in_cell++;
        node_2 = connect_ph[num_node_in_cell];

        while (nodes_ph[0][num_node_in_cell] - nodes_ph[0][num_node_in_cell - 1] < DBL_EPSILON)
          num_node_in_cell++;
        node_3 = connect_ph[num_node_in_cell];
      }
      // Si la maille n'a que 4 sommets, alors ce sont les 4 angles de la maille
      else
      {
        node_1 = connect_ph[num_first_node + 1];
        node_2 = connect_ph[num_first_node + 2];
        node_3 = connect_ph[num_first_node + 3];
      }

      double p_min_cell = nodes_ph[0][node_0];
      double p_max_cell = nodes_ph[0][node_2];
      double h_min_cell = nodes_ph[1][node_0];
      double h_max_cell = nodes_ph[1][node_2];

      unsigned int i_p_min = round((p_min_cell - pmin_ipp) / delta_p_f);
      unsigned int i_p_max = round((p_max_cell - pmin_ipp) / delta_p_f);
      unsigned int i_h_min = round((h_min_cell - hmin_ipp) / delta_h_f);
      unsigned int i_h_max = round((h_max_cell - hmin_ipp) / delta_h_f);

      for (unsigned int i_p = i_p_min; i_p < i_p_max; i_p++)
      {
        for (unsigned int i_h = i_h_min; i_h < i_h_max; i_h++)
        {
          fnodes2phnodes[i_h + nb_h_nodes * i_p] = i_med_cell;
        }
      }

      corners[0 + 4 * i_med_cell] = node_0;
      corners[1 + 4 * i_med_cell] = node_1;
      corners[2 + 4 * i_med_cell] = node_2;
      corners[3 + 4 * i_med_cell] = node_3;
    }
  }

  void EOS_Ipp::node_err2mesh_err(EOS_Error_Field &err_nodes_prop_ph)
  {
    int nb_cell = index_conn_ph.size() - 1;

    ArrOfInt err(nb_cell);
    all_err_val.push_back(err);
    EOS_Error_Field errf(all_err_val[all_err_val.size() - 1]);

    for (int j = 0; j < nb_cell; j++)
    { // Nodes number of the cell
      int nb_np = index_conn_ph[j + 1] - index_conn_ph[j];

      int nb;
      EOS_Internal_Error ierr1, ierr2, ierr3;
      int idx = index_conn_ph[j];

      ierr3 = err_nodes_prop_ph[connect_ph[idx]];
      nb = 1;

      while (nb < nb_np)
      {
        ierr1 = ierr3;
        ierr2 = err_nodes_prop_ph[connect_ph[idx + nb]];
        ierr3 = worst_internal_error(ierr1, ierr2);
        nb++;
      }
      errf.set(j, ierr3);
      errf.set_name(err_nodes_prop_ph.get_name().aschar());
    }
    err_cell_ph.push_back(errf);
  }

  void EOS_Ipp::node_err2segm_err(EOS_Error_Field &err_nodes_prop_p, int satlim)
  {
    int idx, idx2;

    if (satlim == 0)
    { // saturation
      int nb_segm = connect_sat.size() / 2;

      ArrOfInt nerr(nb_segm);
      all_err_val.push_back(nerr);
      EOS_Error_Field errf(all_err_val[all_err_val.size() - 1]);

      int k = 0;
      EOS_Internal_Error ierr1, ierr2;
      for (int j = 0; j < nb_segm; j++)
      {
        idx = connect_sat[k];
        idx2 = connect_sat[k + 1];
        ierr1 = err_nodes_prop_p[idx];
        ierr2 = err_nodes_prop_p[idx2];
        errf.set(j, worst_internal_error(ierr1, ierr2));
        k = k + 2;
      }
      err_segm_sat.push_back(errf);
    }

    else
    { // spinodale
      int nb_segm = connect_lim.size() / 2;

      ArrOfInt nerr(nb_segm);
      all_err_val.push_back(nerr);
      EOS_Error_Field errf(all_err_val[all_err_val.size() - 1]);
      int k = 0;
      EOS_Internal_Error ierr1, ierr2;
      for (int j = 0; j < nb_segm; j++)
      {
        idx = connect_lim[k];
        idx2 = connect_lim[k];
        ierr1 = err_nodes_prop_p[idx];
        ierr2 = err_nodes_prop_p[idx2];
        errf.set(j, worst_internal_error(ierr1, ierr2));
        k = k + 2;
      }
      err_segm_lim.push_back(errf);
    }
  }

  // renvoie le numéro de la cellule réelle contenant (p, h)
  int EOS_Ipp::get_cellidx(double &p, double &h) const
  {
    unsigned int ih, ip;
    ih = (unsigned int)((h - hmin_ipp) / delta_h_f);
    ip = (unsigned int)((p - pmin_ipp) / delta_p_f);

    // if h or p respectively equal to hmax_ipp or pmax_ipp
    if (ip == nb_p_virtual)
      ip--;
    if (ih == nb_h_virtual)
      ih--;

    int i = nb_h_virtual * ip + ih;

    return fnodes2phnodes[i];
  }

  /*
   * EOS_Ipp::get_segmidx :
   *      return indice of first node of segment (indice in nodes_sat/lim)
   *            (note = nodes in nodes_sat/lim are stored in ascending order)
   *
   * double& p  : intput p value
   * int     sat_lim  : sat or lim curve
   *
   * return : int (indice)
   */
  int EOS_Ipp::get_segmidx(double &p) const
  {
    unsigned int ip;
    ip = (unsigned int)((p - pmin_ipp) / delta_p_f);
    // if p respectively equal to pmax_ipp
    if (ip == nb_p_virtual)
      ip--;
    return fnodes2phnodes[ip];
  }


  // recupere les valeurs p, h et "property" pour les 4 points (=coin) de la maille réelle
  //  idx = indice dans le maillage med = fnodes2phnodes[indice_h + Nb_pts_h * indice_p]
  EOS_Internal_Error EOS_Ipp::get_cell_values(int idx, std::map<AString, int>::const_iterator n_prop, EOS_Fields &cell_val) const
  {
    //AString property = n_prop->first; // name of the property
    //char propcov[PROPNAME_MSIZE];
    //eostp_strcov(property.aschar(), propcov); // propcov : base alphanumérical property
    //EOS_thermprop enum_property = nam2num_thermprop(propcov);

    unsigned int nb_properties = val_prop_ph.size();
    unsigned int i_property = n_prop->second;
    if (i_property == nb_properties)
      return PROP_NOT_IN_DB;

    for (unsigned short i_node = 0; i_node < 4; i_node++)
    {
      cell_val[0][i_node] = nodes_ph[0][corners[i_node + 4 * idx]];
      cell_val[1][i_node] = nodes_ph[1][corners[i_node + 4 * idx]];
      cell_val[2][i_node] = val_prop_ph[i_property][corners[i_node + 4 * idx]];
    }

    return err_cell_ph[i_property][idx].get_code();
  }

  EOS_Internal_Error EOS_Ipp::get_segm_values(int idx, std::map<AString, int>::const_iterator n_prop, int sat_lim, EOS_Fields &segm_val) const
  {
    unsigned int i_prop = n_prop->second;
    AString name_prop = n_prop->first;
    unsigned int nb_properties;
    if (sat_lim == 0)
    {
      nb_properties = val_prop_sat.size();
    }
    else
    {
      nb_properties = val_prop_lim.size();
    }

    if (i_prop == nb_properties)
      return PROP_NOT_IN_DB;
    
    if (sat_lim == 0)
    {
      //int nb_vps = val_prop_sat.size();
      //int nb_ess = err_segm_sat.size();

      segm_val[0][0] = nodes_sat[0][idx];
      segm_val[0][1] = nodes_sat[0][idx + 1];

      // property values
      segm_val[1][0] = val_prop_sat[i_prop][idx];
      segm_val[1][1] = val_prop_sat[i_prop][idx + 1];

      /*int i=0
      while(i < nb_ess)
         { EOS_Error_Field errf = err_segm_sat[i] ; 
       //  if (eostp_strcmp(errf.get_name().aschar(), propcov) == 0)  return errf[idx].get_code();
           i++ ;
         }*/
      // à adapter à la nouvelle structure std::map
    }
    else
    {
      //int nb_vpl = val_prop_lim.size();
      //int nb_esl = err_segm_lim.size();

      // p
      segm_val[0][0] = nodes_lim[0][idx];
      segm_val[0][1] = nodes_lim[0][idx + 1];

      // property values
      segm_val[1][0] = val_prop_lim[i_prop][idx];
      segm_val[1][1] = val_prop_lim[i_prop][idx + 1];

      /*int i = 0 ;
      while(i < nb_esl)
         { EOS_Error_Field errf = err_segm_lim[i] ;
           if (eostp_strcmp(errf.get_name().aschar(), propcov) == 0)   return errf[idx].get_code() ;
           i++ ;
         }*/
    }
    return err_segm_sat[i_prop][idx].get_code();
  }

  EOS_Internal_Error EOS_Ipp::compute_Ipp_error(double &error_tot, double *&error_cells, AString prop)
  {

    if (switch_model == false)
    {

      return MODEL_NOT_INIT;
    }
    std::map<AString, int>::const_iterator n_prop;
    if (find_in_Ipp_Prop_ph(n_prop, prop) == EOS_Error::bad)
    {
      std::cout << "The property wasn't in the database";
      return PROP_NOT_IN_DB;
    }
    char *propchar = prop.aschar();
    unsigned int nb_cell = index_conn_ph.size() - 1;
    error_cells = new double[nb_cell];
    EOS_Fields values(3);
    std::vector<int> error_eos(1, 0);
    NEPTUNE::EOS_Error_Field eos_error_field(1, &error_eos[0]);
    erreurtot = 0;
    double erreur_loc;
    //double vol_loc;
    long unsigned int nb_cell_pb = 0;
    ArrOfDouble ap(4);
    ArrOfDouble ah(4);
    ArrOfDouble ar(4);
    ArrOfDouble ar_Ipp_bary(1);
    ArrOfDouble ar_fluid_bary(1);
    EOS_Field pf("P", "p", ap);
    EOS_Field hf("h", "h", ah);
    ArrOfDouble ap_bary(1);
    ArrOfDouble ah_bary(1);
    EOS_Field p_bary("P", "p", ap_bary);
    EOS_Field h_bary("h", "h", ah_bary);
    EOS_Field rf_fluid_bary(propchar, propchar, ar_fluid_bary);

    EOS_Field rf(propchar, propchar, ar);
    values[0] = pf;
    values[1] = hf;
    values[2] = rf;
    // Récupération des valeurs des cellules

    for (unsigned int i_cell = 0; i_cell < nb_cell; i_cell++)
    {
      error_cells[i_cell] = 0;
      get_cell_values(i_cell, n_prop, values);
      // Calcule des barycentres et des volumes
      ar_Ipp_bary[0] = 0; //
      ar_fluid_bary[0] = 0;
      //vol_loc = abs((ap[0] - ap[2]) * (ah[0] - ah[2]));
      ap_bary = (ap[0] + ap[1] + ap[2] + ap[3]) / 4;
      ah_bary = (ah[0] + ah[1] + ah[2] + ah[3]) / 4;
      // Calcule avec le fluide
      NEPTUNE::EOS_Error worst_liq = obj_fluid->compute(p_bary, h_bary, rf_fluid_bary, eos_error_field);
      NEPTUNE::EOS_Internal_Error worst_ipp = compute_prop_ph(n_prop, ap_bary[0], ah_bary[0], ar_Ipp_bary[0]);
      if ((worst_liq == 0) && (worst_ipp.get_code() == 0))
      {
        // Calcule des erreurs avec les valeurs des barycentres
        /*if (abs(ar_Ipp_bary[0]-ar_fluid_bary[0])>10)
        {
          std::cout<<"probleme de calcul pour (p,h):" << ap_bary[0] <<","<<ah_bary[0] <<endl ;
          std::cout<<"La diff est " << abs(ar_Ipp_bary[0]-ar_fluid_bary[0])<<endl;
          std::cout << "Les erreurs sont : Ipp : " << worst_ipp.get_partial_code() << ", fluide : " << worst_liq<<endl;
          nb_cell_pb++;
        }
        else
        {*/
        // erreur_loc = (abs(ar_Ipp_bary[0] - ar_fluid_bary[0])) * vol_loc; // Erreur en norme L^1
        erreur_loc = (abs(ar_Ipp_bary[0] - ar_fluid_bary[0])); // Erreur local sans prendre en compte l'aire de la cellule
        error_cells[i_cell] = erreur_loc;
        erreurtot += erreur_loc;
        //}
      }
      else
      {
        // std::cout<< "erreur ipp" << worst_ipp.get_code() << " et " << "erreur liquide : " << worst_liq << endl ;
        /*std::cout<<"probleme de calcul pour (p,h):" << ap_bary[0] <<","<<ah_bary[0] <<endl ; */
        nb_cell_pb++;
      }
    }
    // erreurtot = erreurtot / ((hmax_ipp - hmin_ipp) * (pmax_ipp - pmin_ipp)); // normalisation de l'erreur
    erreurtot = erreurtot / (nb_cell - nb_cell_pb); // moyenne de l'erreur
    error_tot = erreurtot;
    std::cout << "There were " << nb_cell_pb << " problematic cells out of " << nb_cell << ".";
    return EOS_Internal_Error::OK;
  }

  EOS_Internal_Error EOS_Ipp::compute_Ipp_sat_error(double &error_tot, double *&error_cells, AString prop)
  {

    if (switch_model == false)
    {
      std::cout << " Call the init_model() function.";
      return MODEL_NOT_INIT;
    }

    std::map<AString, int>::const_iterator n_prop;
    if (find_in_Ipp_Prop_sat(n_prop, prop) == EOS_Error::bad)
    {
      std::cout << "The property is not in the database.";
      return PROP_NOT_IN_DB; // "Erreur: Nom de propriété non-trouvé dans le plan ph"
    }
    char *propchar = prop.aschar();
    int nb_seg = nodes_sat[0].size();

    error_cells = new double[nb_seg];

    EOS_Fields values(2);

    std::vector<int> error_eos(1, 0);
    NEPTUNE::EOS_Error_Field eos_error_field(1, &error_eos[0]);
    erreurtot = 0;
    double erreur_loc;
    double vol_loc;
    long unsigned int nb_seg_pb = 0;
    ArrOfDouble ap(2);
    ArrOfDouble ah(2);
    ArrOfDouble ar(2);
    ArrOfDouble ar_Ipp_bary(1);
    ArrOfDouble ar_fluid_bary(1);
    EOS_Field pf("P", "p", ap);
    ArrOfDouble ap_bary(1);
    EOS_Field p_bary("P", "p", ap_bary);
    EOS_Field rf_fluid_bary(propchar, propchar, ar_fluid_bary);
    EOS_Field rf(propchar, propchar, ar);
    values[0] = pf;
    values[1] = rf;
    std::cout << "Le nombre de noeuds est " << nodes_sat[0].size() << endl;
    std::cout << "Le nombre de segm est " << nb_seg << endl;
    std::cout << "Pmin= " << pmin_ipp << endl;
    std::cout << "Pmax= " << pmax_ipp << endl;
    // Récupération des valeurs des cellules
    for (int i_seg = 0; i_seg < nb_seg - 1; i_seg++)
    {
      error_cells[i_seg] = 0;
      get_segm_values(i_seg, n_prop, 0, values);
      // Calcule des barycentres et des volumes
      ar_Ipp_bary[0] = 0; //
      ar_fluid_bary[0] = 0;
      vol_loc = abs((ap[1] - ap[0]));
      ap_bary = (ap[0] + ap[1]) / 2;
      // Calcule avec le fluide
      std::cout << "calcul pour (p,h):" << ap_bary[0] << "," << endl;
      NEPTUNE::EOS_Error worst_liq = obj_fluid->compute(p_bary, rf_fluid_bary, eos_error_field);
      NEPTUNE::EOS_Internal_Error worst_ipp = compute_prop_p(n_prop, ap_bary[0], 0, ar_Ipp_bary[0]);
      if ((worst_liq == 0) && (worst_ipp.get_code() == 0))
      {
        // Calcule des erreurs avec les valeurs des barycentres
        /*if (abs(ar_Ipp_bary[0]-ar_fluid_bary[0])>10)
        {
          std::cout<<"probleme de calcul pour (p,h):" << ap_bary[0] <<","<<ah_bary[0] <<endl ;
          std::cout<<"La diff est " << abs(ar_Ipp_bary[0]-ar_fluid_bary[0])<<endl;
          std::cout << "Les erreurs sont : Ipp : " << worst_ipp.get_partial_code() << ", fluide : " << worst_liq<<endl;
          nb_cell_pb++;
        }
        else
        {*/

        std::cout << "La diff est " << abs(ar_Ipp_bary[0] - ar_fluid_bary[0]) << endl;
        erreur_loc = (abs(ar_Ipp_bary[0] - ar_fluid_bary[0])) * vol_loc;
        error_cells[i_seg] = erreur_loc;
        erreurtot += erreur_loc;
        //}
      }
      else
      {
        std::cout << "erreur ipp" << worst_ipp.get_code() << " et " << "erreur liquide : " << worst_liq << endl;
        /*std::cout<<"probleme de calcul pour (p,h):" << ap_bary[0] <<","<<ah_bary[0] <<endl ; */
        nb_seg_pb++;
      }
    }
    erreurtot = erreurtot / ((pmax_ipp - pmin_ipp)); // normalisation de l'erreur
    error_tot = erreurtot;
    std::cout << "There were " << nb_seg_pb << " problematic cells out of " << nb_seg << ".";
    return EOS_Internal_Error::OK;
  }

  EOS_Internal_Error EOS_Ipp::compute_h_pT(double p, double T, double &h) const
  {
    h = NAN;
    double h_l_sat, h_v_sat, T_sat;
    EOS_Internal_Error ierr;
    EOS_Internal_Error ierrT, ierr1, ierr2, ierr3;

    ierr = check_p_bounds_ph(p);
    if (ierr == OUT_OF_BOUNDS)
      return ierr;

    ierrT = compute_T_sat_p(p, T_sat);
    if (ierrT != EOS_Internal_Error::OK)
      return ierrT;

    if (T <= T_sat)
    {
      ierr1 = compute_h_l_pT(p, T, h);
      ierr2 = compute_h_l_pT(p, T_sat, h_l_sat);
      ierr3 = worst_internal_error(ierr1, ierr2);
      if (std::isnan(h))
        return EOS_Internal_Error::EOS_BAD_COMPUTE; // If the computation of h failed
      if (h <= h_l_sat)
        return ierr3;
    }
    else
    {
      ierr1 = compute_h_v_pT(p, T, h);
      ierr2 = compute_h_v_pT(p, T_sat, h_v_sat);
      ierr3 = worst_internal_error(ierr1, ierr2);
      if (std::isnan(h))
        return ierr3; // If the computation of h failed
      if (h >= h_v_sat)
        return ierr3;
    }
    return INVERT_h_pT;
  }

  EOS_Internal_Error EOS_Ipp::compute_h_l_pT(double p, double T, double &res) const
  {
    //    cout << "--- compute_h_l_pT p="<<p<<" T="<<T<<endl;
    EOS_Internal_Error ierr;
    double pcal, hcal, h;
    double a, b, c, d;

    pcal = hcal = h = 0.e0;

    EOS_Fields values(3);
    AString prop = "T";
    std::map<AString, int>::const_iterator n_prop;
    find(n_prop, prop, Ipp_Prop_ph);

    ArrOfDouble ap(4);
    ArrOfDouble ah(4);
    ArrOfDouble ar(4);
    EOS_Field pf("P", "p", ap);
    EOS_Field hf("h", "h", ah);
    EOS_Field rf(prop.aschar(), prop.aschar(), ar);
    values[0] = pf;
    values[1] = hf;
    values[2] = rf;

    // Get all real cells containing p   : TODO: Optimize these lines
    std::set<unsigned int> cells_containing_p;
    for (double h = hmin_ipp + delta_h_f / 2; h < hmax_ipp; h += delta_h_f)
    {
      unsigned int med_id_cell = get_cellidx(p, h);
      cells_containing_p.insert(med_id_cell);
    }

    // read all cells containing p
    // for each cell compute h if 0<=h*<=1
    // return first h computed
    for (auto med_cell : cells_containing_p)
    {
      ierr = get_cell_values(med_cell, n_prop, values);
      pcal = (p - values[0][0]) / (values[0][2] - values[0][0]);

      /* a = values[2][1] - values[2][0];   // This calcul doesn't correspond to the one on the report
      b = values[2][2] - values[2][0];
      c = values[2][3] - values[2][2] - a;
      d = values[2][0]; */ 
      

      a = values[2][1] - values[2][0];
      b = values[2][3] - values[2][0];
      c = values[2][2] - values[2][3] - a;
      d = values[2][0]; 


      hcal = (T - (b * pcal + d)) / (a + c * pcal);

      if (((hcal > 0.0) || (fabs(hcal) < DBL_EPSILON)) && ((hcal < 1.0) || (fabs(hcal - 1.) < DBL_EPSILON)))
      {
        // hcal = (h-h1)/(h2-h1)   =>   h = hcal*(h2-h1)+h1;
        res = hcal * (values[1][1] - values[1][0]) + values[1][0];
        return EOS_Internal_Error::OK;
      }
    }
    // We didn't find such an h*
    return EOS_Ipp::INVERT_h_pT;
  }

  EOS_Internal_Error EOS_Ipp::compute_h_v_pT(double p, double T, double &res) const
  {
    EOS_Internal_Error ierr;
    double pcal, hcal, h;
    double a, b, c, d;

    pcal = hcal = h = 0.0;

    EOS_Fields values(3);
    AString prop = "T";
    std::map<AString, int>::const_iterator n_prop;
    find(n_prop, prop, Ipp_Prop_ph);

    ArrOfDouble ap(4);
    ArrOfDouble ah(4);
    ArrOfDouble ar(4);
    EOS_Field pf("P", "p", ap);
    EOS_Field hf("h", "h", ah);
    EOS_Field rf(prop.aschar(), prop.aschar(), ar);
    values[0] = pf;
    values[1] = hf;
    values[2] = rf;

    // Get all real cells containing p
    std::set<unsigned int> cells_containing_p;
    for (double h = hmin_ipp + delta_h_f / 2; h < hmax_ipp; h += delta_h_f)
    {
      unsigned int med_id_cell = get_cellidx(p, h);
      cells_containing_p.insert(med_id_cell);
    }

    // read all cells containing p
    // for each cell compute h if 0<=h*<=1
    // return first h computed
    for (auto med_cell : cells_containing_p)
    {
      ierr = get_cell_values(med_cell, n_prop, values);
      pcal = (p - values[0][0]) / (values[0][2] - values[0][0]);
      /* a = values[2][1] - values[2][0];
      b = values[2][2] - values[2][0];
      c = values[2][3] - values[2][2] - a;
      d = values[2][0];*/ 
      a = values[2][1] - values[2][0];
      b = values[2][3] - values[2][0];
      c = values[2][2] - values[2][3] - a;
      d = values[2][0]; 


      hcal = (T - (b * pcal + d)) / (a + c * pcal);
      if (((hcal > 0.0) || (fabs(hcal) < DBL_EPSILON)) && ((hcal < 1.0) || (fabs(hcal - 1.) < DBL_EPSILON)))
      {
        // hcal = (h-h1)/(h2-h1)   =>   h = hcal*(h2-h1)+h1;
        res = hcal * (values[1][1] - values[1][0]) + values[1][0];
        return EOS_Internal_Error::OK;
      }
    }
    // We didn't find such an h*
    return EOS_Ipp::INVERT_h_pT;
  }

  EOS_Internal_Error EOS_Ipp::compute_prop_ph(const std::map<AString, int>::const_iterator
                                                  n_prop,
                                              double p, double h, double &res) const
  {
    // changer l'acces au prop
    EOS_Internal_Error ierr;
    EOS_Fields values(3);

    //int i_prop = n_prop->second;
    AString name_prop = n_prop->first;

    ArrOfDouble ap(4);
    ArrOfDouble ah(4);
    ArrOfDouble ar(4);
    EOS_Field pf("P", "p", ap);
    EOS_Field hf("h", "h", ah);
    EOS_Field rf(name_prop.aschar(), name_prop.aschar(), ar); // transformer prop.
    // EOS_Field rf(prop,prop,ar)
    values[0] = pf;
    values[1] = hf;
    values[2] = rf;

    ierr = check_ph_bounds(p, h);
    if (ierr == OUT_OF_BOUNDS)
      return ierr;

    int index = get_cellidx(p, h);
    ierr = get_cell_values(index, n_prop, values);
    if (ierr != EOS_Internal_Error::OK)
      return ierr;

    res = bilinear_interpolator(p, h, values);

    return EOS_Internal_Error::OK;
  }

  // tag = 0 pour sat et tag = 1 pour lim
  EOS_Internal_Error EOS_Ipp::compute_prop_p(std::map<AString, int>::const_iterator
                                                 n_prop,
                                             double p, int sat_lim, double &res) const
  {
    EOS_Internal_Error ierr;
    EOS_Fields values(2);

    ArrOfDouble ap(2);
    ArrOfDouble ar(2);
    EOS_Field pf("P", "p", ap);
    AString name_prop = n_prop->first;
    EOS_Field rf(name_prop.aschar(), name_prop.aschar(), ar); // transformer prop.

    values[0] = pf;
    values[1] = rf;

    ierr = check_p_bounds_satlim(p);
    if (ierr == OUT_OF_BOUNDS)
      return ierr;

    int index = get_segmidx(p);
    ierr = get_segm_values(index, n_prop, sat_lim, values);

    res = linear_interpolator(p, values);

    return EOS_Internal_Error::OK;
  }

  EOS_Error EOS_Ipp::find_in_Ipp_Prop_ph(std::map<AString, int>::const_iterator &it, const AString &prop) const
  {
    char propconv[PROPNAME_MSIZE];
    eostp_strcov(prop.aschar(), propconv);
    it = Ipp_Prop_ph.find(propconv);
    if (it == Ipp_Prop_ph.end())
    {
      return EOS_Error::bad;
    }
    return EOS_Error::good;
  }

  EOS_Error EOS_Ipp::find_in_Ipp_Prop_sat(std::map<AString, int>::const_iterator &it, const AString &prop) const
  {
    char propconv[PROPNAME_MSIZE];
    eostp_strcov(prop.aschar(), propconv);
    it = Ipp_Prop_sat.find(propconv);
    if (it == Ipp_Prop_sat.end())
    {
      return EOS_Error::bad;
    }
    return EOS_Error::good;
  }

  EOS_Error EOS_Ipp::find(std::map<AString, int>::const_iterator &it, const AString &prop, const std::map<AString, int> &map) const
  {
    char propconv[PROPNAME_MSIZE];
    eostp_strcov(prop.aschar(), propconv);
    it = map.find(propconv);
    if (it == map.end())
    {
      return EOS_Error::bad;
    }
    return EOS_Error::good;
  }

  EOS_Internal_Error EOS_Ipp::check_ph_bounds(double p, double h) const
  {
    EOS_Internal_Error ierr;
    if (std::isnan(h))
      return OUT_OF_BOUNDS;

    if ((fabs(h - hmin_ipp)) > DBL_EPSILON)
    //if ((fabs(h - hmin)) > DBL_EPSILON)
    {
      if (h < hmin_ipp)
      //if (h < hmin)
        return OUT_OF_BOUNDS;
    }

    if ((fabs(h - hmax_ipp) > DBL_EPSILON))
    //if ((fabs(h - hmax) > DBL_EPSILON))
    {
      // if ((h > hmax_ipp))
      if ((h > hmax_ipp))
        return OUT_OF_BOUNDS;
    }

    ierr = check_p_bounds_ph(p);
    return ierr;
  }

  EOS_Internal_Error EOS_Ipp::check_p_bounds_ph(double p) const
  {
    if ((fabs(p - pmin_ipp) > DBL_EPSILON))
    //if ((fabs(p - pmin) > DBL_EPSILON))
    {
      if ((p < pmin_ipp))
      //if ((p < pmin))
        return OUT_OF_BOUNDS;
    }

    if ((fabs(p - pmax_ipp) > DBL_EPSILON))
    //if ((fabs(p - pmax) > DBL_EPSILON))
    {
      if ((p > pmax_ipp))
      //if ((p > pmax))
        return OUT_OF_BOUNDS;
    }

    return EOS_Internal_Error::OK;
  }

  EOS_Internal_Error EOS_Ipp::check_p_bounds_satlim(double p) const
  {
    double max;

    if ((fabs(p - pmin_ipp) > DBL_EPSILON))
    //if ((fabs(p - pmin) > DBL_EPSILON))
    {
      if ((p < pmin_ipp))
      //if ((p < pmin))
        return OUT_OF_BOUNDS;
    }

    if (pcrit < pmax_ipp)
    //if (pcrit < pmax)
      max = pcrit;
    else
      max = pmax_ipp;
      //max = pmax;
    if ((fabs(p - max) > DBL_EPSILON))
    {
      if ((p > max))
        return OUT_OF_BOUNDS;
    }

    return EOS_Internal_Error::OK;
  }
}
