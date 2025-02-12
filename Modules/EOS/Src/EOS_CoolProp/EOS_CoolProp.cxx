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


// CoolProp headers
extern "C" {
#include "CoolPropLib.h"
}
#include "DataStructures.h"

#include "EOS_CoolProp.hxx"

#include "Language/API/RegisterType.hxx"
#include "Language/API/Types_Info.hxx"
#include "EOS/API/EOS_Config.hxx"
#include <fstream>

namespace NEPTUNE_EOS
{
  const AString UnknownStr("Unknown");
  //const AString DefaultBackend_("TTSE&HEOS");
  const AString DefaultBackend_("HEOS");

  // ---------------------------------------------------------------------------
  // ---------------------------------------------------------------------------

  static inline void
  print_error_message_(char message_buffer[])
  {
    cerr << "EOS_CoolProp error:" << endl;
    cerr << "  " << message_buffer << endl;
  }

  // ---------------------------------------------------------------------------
  // ---------------------------------------------------------------------------

  static void
  get_coolprop_ids_from_eos_(EOS_Fields &fields,
                             std::vector<long> &coolprop_ids)
  {
    assert(static_cast<size_t>(fields.size()) == coolprop_ids.size());
    for (int i = 0; i < fields.size(); i++)
    {
      /* Regular property */
      switch (fields[i].get_property())
      {
      case NEPTUNE::p:
      {
        coolprop_ids[i] = (long)CoolProp::iP;
        break;
      }
      case NEPTUNE::T:
      {
        coolprop_ids[i] = (long)CoolProp::iT;
        break;
      }
      case NEPTUNE::rho:
      {
        coolprop_ids[i] = (long)CoolProp::iDmass;
        break;
      }
      case NEPTUNE::cp:
      {
        coolprop_ids[i] = (long)CoolProp::iCpmass;
        break;
      }
      case NEPTUNE::cv:
      {
        coolprop_ids[i] = (long)CoolProp::iCvmass;
        break;
      }
      case NEPTUNE::mu:
      {
        coolprop_ids[i] = (long)CoolProp::iviscosity;
        break;
      }
      case NEPTUNE::lambda:
      {
        coolprop_ids[i] = (long)CoolProp::iconductivity;
        break;
      }
      case NEPTUNE::sigma:
      {
        coolprop_ids[i] = (long)CoolProp::isurface_tension;
        break;
      }
      case NEPTUNE::beta:
      {
        coolprop_ids[i] = (long)CoolProp::iisobaric_expansion_coefficient;
        break;
      }
      case NEPTUNE::f:
      case NEPTUNE::g:
      {
        coolprop_ids[i] = (long)CoolProp::iGmass;
        break;
      }
      case NEPTUNE::w:
      {
        coolprop_ids[i] = (long)CoolProp::ispeed_sound;
        break;
      }
      case NEPTUNE::u:
      {
        coolprop_ids[i] = (long)CoolProp::iUmass;
        break;
      }
      case NEPTUNE::s:
      {
        coolprop_ids[i] = (long)CoolProp::iSmass;
        break;
      }
      default:
      {
        coolprop_ids[i] = (long)CoolProp::INVALID_PARAMETER;
        break;
      }
      case NEPTUNE::p_sat:
      {
        coolprop_ids[i] = (long)CoolProp::iP;
        break;
      }
      case NEPTUNE::T_sat:
      {
        coolprop_ids[i] = (long)CoolProp::iT;
        break;
      }
      case NEPTUNE::h_l_sat:
      case NEPTUNE::h_v_sat:
      {
        coolprop_ids[i] = (long)CoolProp::iHmass;
        break;
      }
      case NEPTUNE::rho_v_sat:
      case NEPTUNE::rho_l_sat:
      {
        coolprop_ids[i] = (long)CoolProp::iDmass;
        break;
      }
      case NEPTUNE::cp_l_sat:
      case NEPTUNE::cp_v_sat:
      {
        coolprop_ids[i] = (long)CoolProp::iCpmass;
        break;
      }
      case NEPTUNE::p_lim:
      {
        coolprop_ids[i] = (long)CoolProp::iP;
        break;
      }
      }
    }
  }

  // ---------------------------------------------------------------------------
  // ---------------------------------------------------------------------------

  //! Internal function to compute values from CoolProp
  //TODO: Switch directly to ABstractState instead of using the handle
  static inline double
  _update_and_compute_from_pair(long                   handle_,
                                const double           in1,
                                const double           in2,
                                CoolProp::input_pairs  input_pair_key,
                                CoolProp::parameters   output_key,
                                long&                  errcode)
  {
    double retval = -99999.;
    errcode = 0;
    char message_buffer[512];

    AbstractState_update(handle_,
                         (long)input_pair_key,
                         in1, in2,
                         &errcode, message_buffer, 511);
    if (errcode != 0) {
      print_error_message_(message_buffer);
      return retval;
    }

    retval = AbstractState_keyed_output(handle_,
                                        (long)output_key,
                                        &errcode,
                                        message_buffer,
                                        511);

    if (errcode != 0)
      print_error_message_(message_buffer);

    return retval;
  }

  static inline double
  _compute_param(long                   handle_,
                 CoolProp::parameters   output_key,
                 long&                  errcode)
  {
    double retval = -99999.;
    errcode = 0;
    char message_buffer[512];

    retval = AbstractState_keyed_output(handle_,
                                        (long)output_key,
                                        &errcode,
                                        message_buffer,
                                        511);

    if (errcode != 0)
      print_error_message_(message_buffer);

    return retval;
  }

  // ---------------------------------------------------------------------------
  // ---------------------------------------------------------------------------

  template <bool calc_from_PH>
  static EOS_Error
  compute_list_from_pair_(long             handle_,
                          const EOS_Field& in1,
                          const EOS_Field& in2,
                          EOS_Fields&      out,
                          EOS_Error_Field& err)
  {
    long errcode = 0;
    char message_buffer[512];

    ArrOfInt err_array(in1.size());
    EOS_Error_Field err_tmp(err_array);
    int _n_vals = out.size();

    std::vector<long> tmp_ids(_n_vals, -1);

    get_coolprop_ids_from_eos_(out, tmp_ids);

    long _compute_pair_id = -1;

    EOS_Error retval = EOS_Error::good;

    if (calc_from_PH)
      _compute_pair_id = (long)CoolProp::HmassP_INPUTS;
    else
      _compute_pair_id = (long)CoolProp::PT_INPUTS;

    for (int i = 0; i < in1.size(); i++) {
      AbstractState_update(handle_, _compute_pair_id,
                           in1[i], in2[i],
                           &errcode, message_buffer, 511);

      if (errcode == 0) {
        long _errcode = 0;
        for (int i_val = 0; i_val < _n_vals; i_val++) {
          out[i_val][i] =
            AbstractState_keyed_output(handle_,
                                       tmp_ids[i_val],
                                       &_errcode, message_buffer, 511);
          if (_errcode != 0) {
            err_array[i] = _errcode;
            retval = EOS_Error::error;
          }
        }
      }
      else {
        err_array[i] = errcode;
        retval = EOS_Error::error;
      }

    }

    return retval;
  }

  // ---------------------------------------------------------------------------
  // ---------------------------------------------------------------------------

  template <bool calc_from_P>
  static EOS_Error
  compute_list_at_sat_(long             handle_,
                       const EOS_Field& in,
                       const double     Q_,
                       EOS_Fields&      out,
                       EOS_Error_Field& err)
  {
    long errcode = 0;
    char message_buffer[512];

    ArrOfInt err_array(in.size());
    EOS_Error_Field err_tmp(err_array);
    int _n_vals = out.size();

    std::vector<long> tmp_ids(_n_vals, -1);

    get_coolprop_ids_from_eos_(out, tmp_ids);

    EOS_Error retval = EOS_Error::good;

    for (int i = 0; i < in.size(); i++) {
      if (calc_from_P) {
        AbstractState_update(handle_, CoolProp::PQ_INPUTS,
                             in[i], Q_,
                             &errcode, message_buffer, 511);
      }
      else {
        AbstractState_update(handle_, CoolProp::QT_INPUTS,
                             Q_, in[i],
                             &errcode, message_buffer, 511);
      }
      if (errcode == 0) {
        long _errcode = 0;
        for (int i_val = 0; i_val < _n_vals; i_val++) {
          out[i_val][i] =
            AbstractState_keyed_output(handle_,
                                       tmp_ids[i_val],
                                       &_errcode, message_buffer, 511);
          if (_errcode != 0) {
            err_array[i] = _errcode;
            retval = EOS_Error::error;
          }
        }
      }
      else {
        err_array[i] = errcode;
        retval = EOS_Error::error;
      }
    }

    return retval;
  }

  // ---------------------------------------------------------------------------
  // ---------------------------------------------------------------------------

  // Number of the first internal error for EOS_Fluid object:
  static const int EOS_Fluid_Error_base = 2000;
  // Definition of internal errors in EOS_Fluid:
  // The following errors leads to EOS_Error=bad
  static const EOS_Internal_Error COMPUTE_ERROR_ = EOS_Internal_Error(EOS_Fluid_Error_base + 0, EOS_Error::error);
  static const EOS_Internal_Error NOT_IMPLEMENTED_ = EOS_Internal_Error(EOS_Fluid_Error_base + 1, EOS_Error::bad);
  static const EOS_Internal_Error WRONG_INPUT_TYPE_ = EOS_Internal_Error(EOS_Fluid_Error_base + 2, EOS_Error::bad);

  // ---------------------------------------------------------------------------
  // ---------------------------------------------------------------------------

  EOS_CoolProp::EOS_CoolProp() :
    abstract_state_handle_(-1),
    handle_phase_(UnknownStr),
    backend_(DefaultBackend_),
    fluid_name_(UnknownStr),
    fluid_name_phase_(UnknownStr)
  {
  }

  // ---------------------------------------------------------------------------
  // ---------------------------------------------------------------------------

  static RegisteredClass& EOS_CoolProp_create()
  { return *(new EOS_CoolProp());
  }

  int EOS_CoolProp::type_Id = (
      RegisterType("EOS_CoolProp", "EOS_Fluid",
                   "Equation Of States for CoolProp",
                   sizeof(EOS_CoolProp),
                   EOS_CoolProp_create)
      );

  // ---------------------------------------------------------------------------
  // ---------------------------------------------------------------------------

  const AString EOS_CoolProp::TMStr("Coolprop");
  const AString EOS_CoolProp::MVStr("6.4");

  // ---------------------------------------------------------------------------
  // ---------------------------------------------------------------------------

  const AString& EOS_CoolProp::table_name()const
  { return TMStr ;
  }

  // ---------------------------------------------------------------------------
  // ---------------------------------------------------------------------------

  const AString& EOS_CoolProp::version_name()const
  { return MVStr ;
  }

  // ---------------------------------------------------------------------------
  // ---------------------------------------------------------------------------

  ostream& EOS_CoolProp::print_On (ostream& stream) const
  { return stream;
  }

  // ---------------------------------------------------------------------------
  // ---------------------------------------------------------------------------

  istream& EOS_CoolProp::read_On (istream& stream)
  { return stream;
  }

  // ---------------------------------------------------------------------------
  // ---------------------------------------------------------------------------

  const AString& EOS_CoolProp::fluid_name() const
  { return fluid_name_ ;
  }

  // ---------------------------------------------------------------------------
  // ---------------------------------------------------------------------------

  const AString& EOS_CoolProp::equation_name()const
  { return fluid_name_phase_ ;
  }

//  EOS_CoolProp::EOS_CoolProp(const EOS_CoolProp&)
//  { assert(0);
//  }

  EOS_CoolProp::~EOS_CoolProp()
  {
    if (abstract_state_handle_ != -1) {
      long errcode = 0;
      char message_buffer[512];
      AbstractState_free(abstract_state_handle_,
                         &errcode, message_buffer, 511);
      abstract_state_handle_ = -1;
    }
  }

  const Type_Info& EOS_CoolProp::get_Type_Info () const
  { return (Types_Info::instance())[type_Id];
  }

  const AString& EOS_CoolProp::phase_name()const
  { return handle_phase_;
  }

  void
  EOS_CoolProp::describe_error(const EOS_Internal_Error error,
                               AString& description) const
  {
    if (error == EOS_Internal_Error::OK)
      description = "OK";
    else
      description = "Something went horribly wrong in CoolProp...";
  }

  int EOS_CoolProp::init(const Strings& strings)
  { EOS_Internal_Error err ;
    AString desc_err ;

    // test strings
    int sz_strings = strings.size();
    if (sz_strings != 2)
       { cerr << " EOS_CoolProp::init(strings) : Reference with size " << sz_strings
              << " is not implemented for EOS_CoolProp ! " << endl;
         assert(0) ;
         exit(-1) ;
         return EOS_Error::error ;
       }

    // fluide name : FluidStr
    fluid_name_ = AString(strings[0]) ;

    // Check for phase type
    handle_phase_ = AString(strings[1]);

    if (handle_phase_ == "phase_gas")
      sat_quality_ = 1;
    else
      sat_quality_ = 0;


    // equation of state name : EosStr
    fluid_name_phase_ =  fluid_name_;

    // Get handles

    long errcode = 0;
    char message_buffer[512];
    abstract_state_handle_ = AbstractState_factory(backend_.aschar(),
                                                   fluid_name_.aschar(),
                                                   &errcode,
                                                   message_buffer,
                                                   511);

    if (errcode != 0) {
      print_error_message_(message_buffer);
      return EOS_Error::error;
    }

    errcode = 0;
    AbstractState_specify_phase(abstract_state_handle_,
                                handle_phase_.aschar(),
                                &errcode,
                                message_buffer,
                                511);
    if (errcode != 0) {
      print_error_message_(message_buffer);
      return EOS_Error::error;
    }

    /* Initialize molar_mass */

    molar_mass_ = AbstractState_keyed_output(abstract_state_handle_,
                                             (long)CoolProp::imolar_mass,
                                             &errcode,
                                             message_buffer,
                                             511);

    if (errcode != 0) {
      print_error_message_(message_buffer);
      return EOS_Error::error;
    }

    return EOS_Error::good ;
  }

  // ---------------------------------------------------------------------------

  int EOS_CoolProp::init([[maybe_unused]] const Strings& strings, [[maybe_unused]] const Strings& values)
  {
    // avoid warnings
    throw std::runtime_error("not implemented yet...");
    return EOS_Error::error;
  }

  // ---------------------------------------------------------------------------

  int EOS_CoolProp::init([[maybe_unused]]const Strings& strings, [[maybe_unused]]const Strings& values,
                         [[maybe_unused]]const double href0, [[maybe_unused]]const double sref0, [[maybe_unused]]const double tref0, [[maybe_unused]]const double pref0)
  {
    // avoid warnings
    throw std::runtime_error("not implemented yet...");
    return EOS_Error::error;
  }

  // ---------------------------------------------------------------------------
  //! Compute several outputs based on a single input
  //
  /*
  EOS_Error EOS_CoolProp::compute(const EOS_Field& in,
                                  EOS_Fields&      out,
                                  EOS_Error_Field& err) const
  {
    assert(abstract_state_handle_ != -1);

    if ( in.get_property() == NEPTUNE::p ||
         in.get_sat_property() == NEPTUNE::p_sat ||
         in.get_lim_property() == NEPTUNE::p_lim) {
      return compute_list_at_sat_<true>(abstract_state_handle_,
                                        in, sat_quality_,
                                        out, err);
    }
    else if (in.get_property() == NEPTUNE::T ||
             in.get_sat_property() == NEPTUNE::T_sat) {
      return compute_list_at_sat_<false>(abstract_state_handle_,
                                         in, sat_quality_,
                                         out, err);
    }
    else
      return EOS_Error::error;
  }
  */

  // ---------------------------------------------------------------------------
  //! Compute on output based on a single input
  // ---------------------------------------------------------------------------
  /*
  EOS_Error EOS_CoolProp::compute(const EOS_Field& in,
                                  EOS_Field& out,
                                  EOS_Error_Field& err) const
  {
    int compute_from_p = 0;
    int compute_from_T = 0;
    if ( in.get_property() == NEPTUNE::p ||
         in.get_sat_property() == NEPTUNE::p_sat ||
         in.get_lim_property() == NEPTUNE::p_lim) {
      compute_from_p = 1;
    }
    else if (in.get_property() == NEPTUNE::T ||
             in.get_sat_property() == NEPTUNE::T_sat) {
      compute_from_T = 1;
    }
    else {
      err = EOS_Internal_Error::NOT_IMPLEMENTED;
      return EOS_Error::error;
    }

    EOS_saturprop propsat = out.get_sat_property();

    const int _size = p.size();

    if (compute_from_p) {
      switch (propsat) {
        case NEPTUNE::rho_l_sat:
          {
            for (int i = 0; i < _size; i++)
              err.set(i, compute_rho_l_sat_p(in[i], out[i]));
            break;
          }
        case NEPTUNE::rho_v_sat:
          {
            for (int i = 0; i < _size; i++)
              err.set(i, compute_rho_v_sat_p(in[i], out[i]));
            break;
          }
        case NEPTUNE::h_l_sat:
          {
            for (int i = 0; i < _size; i++)
              err.set(i, compute_h_l_sat_p(in[i], out[i]));
            break;
          }
        case NEPTUNE::h_v_sat:
          {
            for (int i = 0; i < _size; i++)
              err.set(i, compute_h_v_sat_p(in[i], out[i]));
            break;
          }
        case NEPTUNE::cp_l_sat:
          {
            for (int i = 0; i < _size; i++)
              err.set(i, compute_cp_l_sat_p(in[i], out[i]));
            break;
          }
        case NEPTUNE::cp_v_sat:
          {
            for (int i = 0; i < _size; i++)
              err.set(i, compute_cp_v_sat_p(in[i], out[i]));
            break;
          }
        case NEPTUNE::T_sat:
          {
            for (int i = 0; i < _size; i++)
              err.set(i, compute_T_sat_p(in[i], out[i]));
            break;
          }
        case NEPTUNE::d_rho_l_sat_d_p:
          {
            for (int i = 0; i < _size; i++)
              err.set(i, compute_d_rho_l_sat_d_p_p(in[i], out[i]));
            break;
          }
        case NEPTUNE::d_rho_v_sat_d_p:
          {
            for (int i = 0; i < _size; i++)
              err.set(i, compute_d_rho_v_sat_d_p_p(in[i], out[i]));
            break;
          }
        case NEPTUNE::d_h_l_sat_d_p:
          {
            for (int i = 0; i < _size; i++)
              err.set(i, compute_d_h_l_sat_d_p_p(in[i], out[i]));
            break;
          }
        case NEPTUNE::d_h_v_sat_d_p:
          {
            for (int i = 0; i < _size; i++)
              err.set(i, compute_d_h_v_sat_d_p_p(in[i], out[i]));
            break;
          }
        case NEPTUNE::d_cp_l_sat_d_p:
          {
            for (int i = 0; i < _size; i++)
              err.set(i, compute_d_cp_l_sat_d_p_p(in[i], out[i]));
            break;
          }
        case NEPTUNE::d_cp_v_sat_d_p:
          {
            for (int i = 0; i < _size; i++)
              err.set(i, compute_d_cp_v_sat_d_p_p(in[i], out[i]));
            break;
          }
        case NEPTUNE::d_T_sat_d_p:
          {
            for (int i = 0; i < _size; i++)
              err.set(i, compute_d_T_sat_d_p_p(in[i], out[i]));
            break;
          }
        case NEPTUNE::d2_T_sat_d_p_d_p:
          {
            for (int i = 0; i < _size; i++)
              err.set(i, compute_d2_T_sat_d_p_d_p_p(in[i], out[i]));
            break;
          }
      }
    }
    else if (compute_from_T) {
    }

    EOS_Fields _fields(1);
    _fields[0] = out;
    return compute(in, _fields, err);
  }
*/

  // ---------------------------------------------------------------------------
  //! Compute several outputs based on two inputs (P/T or P/H)
  // ---------------------------------------------------------------------------

  /*
  EOS_Error EOS_CoolProp::compute(const EOS_Field& in1,
                                  const EOS_Field& in2,
                                  EOS_Fields&      out,
                                  EOS_Error_Field& err) const
  {
    assert(abstract_state_handle_ != -1);

    // CoolProp inputs are (P,T) and (H,P) !
    if ( (in1.get_property() == NEPTUNE::p && in2.get_property() == NEPTUNE::h)) {
      return compute_list_from_pair_<true>(abstract_state_handle_, in2, in1, out, err);
    }
    else if ((in2.get_property() == NEPTUNE::p && in1.get_property() == NEPTUNE::h)) {
      return compute_list_from_pair_<true>(abstract_state_handle_, in1, in2, out, err);
    }
    else if ((in1.get_property() == NEPTUNE::p && in2.get_property() == NEPTUNE::T)) {
      return compute_list_from_pair_<false>(abstract_state_handle_, in1, in2, out, err);
    }
    else if ((in2.get_property() == NEPTUNE::p && in1.get_property() == NEPTUNE::T)) {
      return compute_list_from_pair_<false>(abstract_state_handle_, in2, in1, out, err);
    }
    else {
      return EOS_Error::error;
    }
  }
  */

  /*
  EOS_Error EOS_CoolProp::compute(const EOS_Field& in1,
                                  const EOS_Field& in2,
                                  EOS_Field&      out,
                                  EOS_Error_Field& err) const
  {
    EOS_Fields _fields(1);
    _fields[0] = out;
    return compute(in1, in2, _fields, err);
  }
  */

  // ---------------------------------------------------------------------------
  //! Dedicated functions for specific properties
  // ---------------------------------------------------------------------------

  EOS_Error
  EOS_CoolProp::compute_Ph(const char* const property_name,
                           double in1, double in2, double& out) const
  {
    int ierr;
    EOS_Error er;

    EOS_Field fP("P", "P", 1, &in1);
    EOS_Field fh("h", "h", 1, &in2);
    EOS_Field fout(property_name,property_name,1,&out);
    EOS_Error_Field ferr(1, &ierr);

    er = compute(fP, fh, fout, ferr);
    return er;
  }


  EOS_Error
  EOS_CoolProp::compute_PT(const char* const property_name,
                           double in1, double in2, double& out) const
  {
    int ierr;
    EOS_Error er;

    EOS_Field fP("P", "P", 1, &in1);
    EOS_Field fT("T", "T", 1, &in2);
    EOS_Field fout(property_name,property_name,1,&out);
    EOS_Error_Field ferr(1, &ierr);

    er = compute(fP, fT, fout, ferr);
    return er;
  }

  // ---------------------------------------------------------------------------
  //! Dedicated functions for specific properties
  // ---------------------------------------------------------------------------
  //

  // ---------------------------------------------------------------------------

  EOS_Internal_Error
  EOS_CoolProp::compute_pr_ph(double p, double h, double& pr) const
  {
    long err_ = 0;

    pr = _update_and_compute_from_pair(abstract_state_handle_,
                                       h, p,
                                       CoolProp::HmassP_INPUTS,
                                       CoolProp::iPrandtl,
                                       err_);

    if (err_ != 0) return COMPUTE_ERROR_;

    return EOS_Internal_Error::OK;
  }

  // ---------------------------------------------------------------------------

  EOS_Internal_Error
  EOS_CoolProp::compute_pr_pT(double p, double T, double& pr) const
  {
    long err_ = 0;

    pr = _update_and_compute_from_pair(abstract_state_handle_,
                                       p, T,
                                       CoolProp::PT_INPUTS,
                                       CoolProp::iPrandtl,
                                       err_);

    if (err_ != 0) return COMPUTE_ERROR_;

    return EOS_Internal_Error::OK;
  }

  // ---------------------------------------------------------------------------

  EOS_Internal_Error
  EOS_CoolProp::compute_h_pT(double p, double T, double& h) const
  {
    long err_ = 0;

    h = _update_and_compute_from_pair(abstract_state_handle_,
                                      p, T,
                                      CoolProp::PT_INPUTS,
                                      CoolProp::iHmass,
                                      err_);

    if (err_ != 0) return COMPUTE_ERROR_;

    return EOS_Internal_Error::OK;
  }

  // ---------------------------------------------------------------------------

  EOS_Internal_Error
  EOS_CoolProp::compute_T_ph(double p, double h, double& T) const
  {
    long err_ = 0;

    T = _update_and_compute_from_pair(abstract_state_handle_,
                                      h, p,
                                      CoolProp::HmassP_INPUTS,
                                      CoolProp::iT,
                                      err_);

    if (err_ != 0) return COMPUTE_ERROR_;


    return EOS_Internal_Error::OK;
  }

  // ---------------------------------------------------------------------------

  EOS_Internal_Error
  EOS_CoolProp::compute_gamma_ph(double p, double h, double& r) const
  {
    long err_ = 0;

    double cv = _update_and_compute_from_pair(abstract_state_handle_,
                                              h, p,
                                              CoolProp::HmassP_INPUTS,
                                              CoolProp::iCvmass, err_);

    if (cv <= 0.e0 || err_ != 0) return COMPUTE_ERROR_;

    /* No need to reset h,p */
    double cp = _compute_param(abstract_state_handle_,
                               CoolProp::iCpmass,
                               err_);

    if (err_ != 0) return COMPUTE_ERROR_;

    r = cp / cv;

    return EOS_Internal_Error::OK;
  }

  // ---------------------------------------------------------------------------

  EOS_Internal_Error
  EOS_CoolProp::compute_gamma_pT(double p, double T, double& r) const
  {
    long err_ = 0;

    double cv = _update_and_compute_from_pair(abstract_state_handle_,
                                              p, T,
                                              CoolProp::PT_INPUTS,
                                              CoolProp::iCvmass, err_);

    if (cv <= 0.e0 || err_ != 0) return COMPUTE_ERROR_;

    double cp = _compute_param(abstract_state_handle_,
                               CoolProp::iCpmass,
                               err_);

    if (err_ != 0) return COMPUTE_ERROR_;

    r = cp / cv;

    return EOS_Internal_Error::OK;
  }

  // ---------------------------------------------------------------------------

  EOS_Internal_Error
  EOS_CoolProp::compute_rho_pT(double p, double T, double& r) const
  {
    long err_ = 0;

    r = _update_and_compute_from_pair(abstract_state_handle_,
                                      p, T,
                                      CoolProp::PT_INPUTS,
                                      CoolProp::iDmass,
                                      err_);

    if (err_ != 0) return COMPUTE_ERROR_;

    return EOS_Internal_Error::OK;
  }

  // ---------------------------------------------------------------------------

  EOS_Internal_Error
  EOS_CoolProp::compute_rho_ph(double p, double h, double& r) const
  {
    long err_ = 0;

    r = _update_and_compute_from_pair(abstract_state_handle_,
                                      h, p,
                                      CoolProp::HmassP_INPUTS,
                                      CoolProp::iDmass,
                                      err_);

    if (err_ != 0) return COMPUTE_ERROR_;

    return EOS_Internal_Error::OK;
  }

  // ---------------------------------------------------------------------------

  EOS_Internal_Error
  EOS_CoolProp::compute_u_pT(double p, double T, double& r) const
  {
    long err_ = 0;

    r = _update_and_compute_from_pair(abstract_state_handle_,
                                      p, T,
                                      CoolProp::PT_INPUTS,
                                      CoolProp::iUmass,
                                      err_);

    if (err_ != 0) return COMPUTE_ERROR_;

    return EOS_Internal_Error::OK;
  }

  // ---------------------------------------------------------------------------

  EOS_Internal_Error
  EOS_CoolProp::compute_u_ph(double p, double h, double& r) const
  {
    long err_ = 0;

    r = _update_and_compute_from_pair(abstract_state_handle_,
                                      h, p,
                                      CoolProp::HmassP_INPUTS,
                                      CoolProp::iUmass,
                                      err_);

    if (err_ != 0) return COMPUTE_ERROR_;

    return EOS_Internal_Error::OK;
  }

  // ---------------------------------------------------------------------------

  EOS_Internal_Error
  EOS_CoolProp::compute_s_pT(double p, double T, double& r) const
  {
    long err_ = 0;

    r = _update_and_compute_from_pair(abstract_state_handle_,
                                      p, T,
                                      CoolProp::PT_INPUTS,
                                      CoolProp::iSmass,
                                      err_);

    if (err_ != 0) return COMPUTE_ERROR_;

    return EOS_Internal_Error::OK;
  }

  // ---------------------------------------------------------------------------

  EOS_Internal_Error
  EOS_CoolProp::compute_s_ph(double p, double h, double& r) const
  {
    long err_ = 0;

    r = _update_and_compute_from_pair(abstract_state_handle_,
                                      h, p,
                                      CoolProp::HmassP_INPUTS,
                                      CoolProp::iSmass,
                                      err_);

    if (err_ != 0) return COMPUTE_ERROR_;

    return EOS_Internal_Error::OK;
  }

  // ---------------------------------------------------------------------------

  EOS_Internal_Error
  EOS_CoolProp::compute_mu_pT(double p, double T, double& r) const
  {
    long err_ = 0;

    r = _update_and_compute_from_pair(abstract_state_handle_,
                                      p, T,
                                      CoolProp::PT_INPUTS,
                                      CoolProp::iviscosity,
                                      err_);

    if (err_ != 0) return COMPUTE_ERROR_;

    return EOS_Internal_Error::OK;
  }

  // ---------------------------------------------------------------------------

  EOS_Internal_Error
  EOS_CoolProp::compute_mu_ph(double p, double h, double& r) const
  {
    long err_ = 0;

    r = _update_and_compute_from_pair(abstract_state_handle_,
                                      h, p,
                                      CoolProp::HmassP_INPUTS,
                                      CoolProp::iviscosity,
                                      err_);

    if (err_ != 0) return COMPUTE_ERROR_;

    return EOS_Internal_Error::OK;
  }

  // ---------------------------------------------------------------------------

  EOS_Internal_Error
  EOS_CoolProp::compute_lambda_pT(double p, double T, double& r) const
  {
    long err_ = 0;

    r = _update_and_compute_from_pair(abstract_state_handle_,
                                      p, T,
                                      CoolProp::PT_INPUTS,
                                      CoolProp::iconductivity,
                                      err_);

    if (err_ != 0) return COMPUTE_ERROR_;

    return EOS_Internal_Error::OK;
  }

  // ---------------------------------------------------------------------------

  EOS_Internal_Error
  EOS_CoolProp::compute_lambda_ph(double p, double h, double& r) const
  {
    long err_ = 0;

    r = _update_and_compute_from_pair(abstract_state_handle_,
                                      h, p,
                                      CoolProp::HmassP_INPUTS,
                                      CoolProp::iconductivity,
                                      err_);

    if (err_ != 0) return COMPUTE_ERROR_;

    return EOS_Internal_Error::OK;
  }

  // ---------------------------------------------------------------------------

  EOS_Internal_Error
  EOS_CoolProp::compute_cp_pT(double p, double T, double& r) const
  {
    long err_ = 0;

    r = _update_and_compute_from_pair(abstract_state_handle_,
                                      p, T,
                                      CoolProp::PT_INPUTS,
                                      CoolProp::iCpmass,
                                      err_);

    if (err_ != 0) return COMPUTE_ERROR_;

    return EOS_Internal_Error::OK;
  }

  // ---------------------------------------------------------------------------

  EOS_Internal_Error
  EOS_CoolProp::compute_cp_ph(double p, double h, double& r) const
  {
    long err_ = 0;

    r = _update_and_compute_from_pair(abstract_state_handle_,
                                      h, p,
                                      CoolProp::HmassP_INPUTS,
                                      CoolProp::iCpmass,
                                      err_);

    if (err_ != 0) return COMPUTE_ERROR_;

    return EOS_Internal_Error::OK;
  }

  // ---------------------------------------------------------------------------

  EOS_Internal_Error
  EOS_CoolProp::compute_cv_pT(double p, double T, double& r) const
  {
    long err_ = 0;

    r = _update_and_compute_from_pair(abstract_state_handle_,
                                      p, T,
                                      CoolProp::PT_INPUTS,
                                      CoolProp::iCvmass,
                                      err_);

    if (err_ != 0) return COMPUTE_ERROR_;

    return EOS_Internal_Error::OK;
  }

  // ---------------------------------------------------------------------------

  EOS_Internal_Error
  EOS_CoolProp::compute_cv_ph(double p, double h, double& r) const
  {
    long err_ = 0;

    r = _update_and_compute_from_pair(abstract_state_handle_,
                                      h, p,
                                      CoolProp::HmassP_INPUTS,
                                      CoolProp::iCvmass,
                                      err_);

    if (err_ != 0) return COMPUTE_ERROR_;

    return EOS_Internal_Error::OK;
  }

  // ---------------------------------------------------------------------------

  EOS_Internal_Error
  EOS_CoolProp::compute_sigma_pT(double p, double T, double& r) const
  {
    long err_ = 0;

    r = _update_and_compute_from_pair(abstract_state_handle_,
                                      p, T,
                                      CoolProp::PT_INPUTS,
                                      CoolProp::isurface_tension,
                                      err_);

    if (err_ != 0) return COMPUTE_ERROR_;

    return EOS_Internal_Error::OK;
  }

  // ---------------------------------------------------------------------------

  EOS_Internal_Error
  EOS_CoolProp::compute_sigma_ph(double p, double h, double& r) const
  {
    long err_ = 0;
    (void)h;//h not used
    r = _update_and_compute_from_pair(abstract_state_handle_,
                                      p, sat_quality_,
                                      CoolProp::PQ_INPUTS,
                                      CoolProp::isurface_tension,
                                      err_);

    if (err_ != 0) return COMPUTE_ERROR_;

    return EOS_Internal_Error::OK;
  }

  // ---------------------------------------------------------------------------

  EOS_Internal_Error
  EOS_CoolProp::compute_w_pT(double p, double T, double& r) const
  {
    long err_ = 0;

    r = _update_and_compute_from_pair(abstract_state_handle_,
                                      p, T,
                                      CoolProp::PT_INPUTS,
                                      CoolProp::ispeed_sound,
                                      err_);

    if (err_ != 0) return COMPUTE_ERROR_;

    return EOS_Internal_Error::OK;
  }

  // ---------------------------------------------------------------------------

  EOS_Internal_Error
  EOS_CoolProp::compute_w_ph(double p, double h, double& r) const
  {
    long err_ = 0;

    r = _update_and_compute_from_pair(abstract_state_handle_,
                                      h, p,
                                      CoolProp::HmassP_INPUTS,
                                      CoolProp::ispeed_sound,
                                      err_);

    if (err_ != 0) return COMPUTE_ERROR_;

    return EOS_Internal_Error::OK;
  }

  // ---------------------------------------------------------------------------

  EOS_Internal_Error
  EOS_CoolProp::compute_g_pT(double p, double T, double& r) const
  {
    long err_ = 0;

    r = _update_and_compute_from_pair(abstract_state_handle_,
                                      p, T,
                                      CoolProp::PT_INPUTS,
                                      CoolProp::iGmass,
                                      err_);

    if (err_ != 0) return COMPUTE_ERROR_;

    return EOS_Internal_Error::OK;
  }

  // ---------------------------------------------------------------------------

  EOS_Internal_Error
  EOS_CoolProp::compute_g_ph(double p, double h, double& r) const
  {
    long err_ = 0;

    r = _update_and_compute_from_pair(abstract_state_handle_,
                                      h, p,
                                      CoolProp::HmassP_INPUTS,
                                      CoolProp::iGmass,
                                      err_);

    if (err_ != 0) return COMPUTE_ERROR_;

    return EOS_Internal_Error::OK;
  }

  // ---------------------------------------------------------------------------

  EOS_Internal_Error
  EOS_CoolProp::compute_f_pT(double p, double T, double& r) const
  {
    long err_ = 0;

    r = _update_and_compute_from_pair(abstract_state_handle_,
                                      p, T,
                                      CoolProp::PT_INPUTS,
                                      CoolProp::iGmass,
                                      err_);

    if (err_ != 0) return COMPUTE_ERROR_;

    return EOS_Internal_Error::OK;
  }

  // ---------------------------------------------------------------------------

  EOS_Internal_Error
  EOS_CoolProp::compute_f_ph(double p, double h, double& r) const
  {
    long err_ = 0;

    r = _update_and_compute_from_pair(abstract_state_handle_,
                                      h, p,
                                      CoolProp::HmassP_INPUTS,
                                      CoolProp::iGmass,
                                      err_);

    if (err_ != 0) return COMPUTE_ERROR_;

    return EOS_Internal_Error::OK;
  }

  // ---------------------------------------------------------------------------

  EOS_Internal_Error
  EOS_CoolProp::compute_beta_pT(double p, double T, double& r) const
  {
    long err_ = 0;

    r = _update_and_compute_from_pair(abstract_state_handle_,
                                      p, T,
                                      CoolProp::PT_INPUTS,
                                      CoolProp::iisobaric_expansion_coefficient,
                                      err_);

    if (err_ != 0) return COMPUTE_ERROR_;

    return EOS_Internal_Error::OK;
  }

  // ---------------------------------------------------------------------------

  EOS_Internal_Error
  EOS_CoolProp::compute_beta_ph(double p, double h, double& r) const
  {
    long err_ = 0;

    r = _update_and_compute_from_pair(abstract_state_handle_,
                                      h, p,
                                      CoolProp::HmassP_INPUTS,
                                      CoolProp::iisobaric_expansion_coefficient,
                                      err_);

    if (err_ != 0) return COMPUTE_ERROR_;

    return EOS_Internal_Error::OK;
  }

  // ---------------------------------------------------------------------------

  EOS_Internal_Error
  EOS_CoolProp::compute_rho_l_sat_p(double p, double& r) const
  {
    long err_ = 0;

    r = _update_and_compute_from_pair(abstract_state_handle_,
                                      p, 0.,
                                      CoolProp::PQ_INPUTS,
                                      CoolProp::iDmass,
                                      err_);

    if (err_ != 0) return COMPUTE_ERROR_;

    return EOS_Internal_Error::OK;
  }

  // ---------------------------------------------------------------------------

  EOS_Internal_Error
  EOS_CoolProp::compute_rho_l_sat_T(double T, double& r) const
  {
    long err_ = 0;

    r = _update_and_compute_from_pair(abstract_state_handle_,
                                      0., T,
                                      CoolProp::QT_INPUTS,
                                      CoolProp::iDmass,
                                      err_);

    if (err_ != 0) return COMPUTE_ERROR_;

    return EOS_Internal_Error::OK;
  }

  // ---------------------------------------------------------------------------

  EOS_Internal_Error
  EOS_CoolProp::compute_rho_v_sat_p(double p, double& r) const
  {
    long err_ = 0;

    r = _update_and_compute_from_pair(abstract_state_handle_,
                                      p, 1.,
                                      CoolProp::PQ_INPUTS,
                                      CoolProp::iDmass,
                                      err_);

    if (err_ != 0) return COMPUTE_ERROR_;

    return EOS_Internal_Error::OK;
  }

  // ---------------------------------------------------------------------------

  EOS_Internal_Error
  EOS_CoolProp::compute_rho_v_sat_T(double T, double& r) const
  {
    long err_ = 0;

    r = _update_and_compute_from_pair(abstract_state_handle_,
                                      1., T,
                                      CoolProp::QT_INPUTS,
                                      CoolProp::iDmass,
                                      err_);

    if (err_ != 0) return COMPUTE_ERROR_;

    return EOS_Internal_Error::OK;
  }

  // ---------------------------------------------------------------------------

  EOS_Internal_Error
  EOS_CoolProp::compute_h_l_sat_p(double p, double& r) const
  {
    long err_ = 0;

    r = _update_and_compute_from_pair(abstract_state_handle_,
                                      p, 0.,
                                      CoolProp::PQ_INPUTS,
                                      CoolProp::iHmass,
                                      err_);

    if (err_ != 0) return COMPUTE_ERROR_;

    return EOS_Internal_Error::OK;
  }

  // ---------------------------------------------------------------------------

  EOS_Internal_Error
  EOS_CoolProp::compute_h_l_sat_T(double T, double& r) const
  {
    long err_ = 0;

    r = _update_and_compute_from_pair(abstract_state_handle_,
                                      0., T,
                                      CoolProp::QT_INPUTS,
                                      CoolProp::iHmass,
                                      err_);

    if (err_ != 0) return COMPUTE_ERROR_;

    return EOS_Internal_Error::OK;
  }

  // ---------------------------------------------------------------------------

  EOS_Internal_Error
  EOS_CoolProp::compute_h_v_sat_p(double p, double& r) const
  {
    long err_ = 0;

    r = _update_and_compute_from_pair(abstract_state_handle_,
                                      p, 1.,
                                      CoolProp::PQ_INPUTS,
                                      CoolProp::iHmass,
                                      err_);

    if (err_ != 0) return COMPUTE_ERROR_;

    return EOS_Internal_Error::OK;
  }

  // ---------------------------------------------------------------------------

  EOS_Internal_Error
  EOS_CoolProp::compute_h_v_sat_T(double T, double& r) const
  {
    long err_ = 0;

    r = _update_and_compute_from_pair(abstract_state_handle_,
                                      1., T,
                                      CoolProp::QT_INPUTS,
                                      CoolProp::iHmass,
                                      err_);

    if (err_ != 0) return COMPUTE_ERROR_;

    return EOS_Internal_Error::OK;
  }

  // ---------------------------------------------------------------------------

  EOS_Internal_Error
  EOS_CoolProp::compute_cp_l_sat_p(double p, double& r) const
  {
    long err_ = 0;

    r = _update_and_compute_from_pair(abstract_state_handle_,
                                      p, 0.,
                                      CoolProp::PQ_INPUTS,
                                      CoolProp::iCpmass,
                                      err_);

    if (err_ != 0) return COMPUTE_ERROR_;

    return EOS_Internal_Error::OK;
  }

  // ---------------------------------------------------------------------------

  EOS_Internal_Error
  EOS_CoolProp::compute_cp_l_sat_T(double T, double& r) const
  {
    long err_ = 0;

    r = _update_and_compute_from_pair(abstract_state_handle_,
                                      0., T,
                                      CoolProp::QT_INPUTS,
                                      CoolProp::iCpmass,
                                      err_);

    if (err_ != 0) return COMPUTE_ERROR_;

    return EOS_Internal_Error::OK;
  }

  // ---------------------------------------------------------------------------

  EOS_Internal_Error
  EOS_CoolProp::compute_cp_v_sat_p(double p, double& r) const
  {
    long err_ = 0;

    r = _update_and_compute_from_pair(abstract_state_handle_,
                                      p, 1.,
                                      CoolProp::PQ_INPUTS,
                                      CoolProp::iCpmass,
                                      err_);

    if (err_ != 0) return COMPUTE_ERROR_;

    return EOS_Internal_Error::OK;
  }

  // ---------------------------------------------------------------------------

  EOS_Internal_Error
  EOS_CoolProp::compute_cp_v_sat_T(double T, double& r) const
  {
    long err_ = 0;

    r = _update_and_compute_from_pair(abstract_state_handle_,
                                      1., T,
                                      CoolProp::QT_INPUTS,
                                      CoolProp::iCpmass,
                                      err_);

    if (err_ != 0) return COMPUTE_ERROR_;

    return EOS_Internal_Error::OK;
  }

  // ---------------------------------------------------------------------------

  EOS_Internal_Error
  EOS_CoolProp::compute_T_sat_p(double p, double& r) const
  {
    long err_ = 0;

    r = _update_and_compute_from_pair(abstract_state_handle_,
                                      p, sat_quality_,
                                      CoolProp::PQ_INPUTS,
                                      CoolProp::iT,
                                      err_);

    if (err_ != 0) return COMPUTE_ERROR_;

    return EOS_Internal_Error::OK;
  }

  // ---------------------------------------------------------------------------

  EOS_Internal_Error
  EOS_CoolProp::compute_p_sat_T(double T, double& r) const
  {
    long err_ = 0;

    r = _update_and_compute_from_pair(abstract_state_handle_,
                                      sat_quality_, T,
                                      CoolProp::QT_INPUTS,
                                      CoolProp::iP,
                                      err_);

    if (err_ != 0) return COMPUTE_ERROR_;

    return EOS_Internal_Error::OK;
  }

  // ---------------------------------------------------------------------------

  EOS_Internal_Error
  EOS_CoolProp::get_p_min(double& p_min) const
  {
    long errcode = 0;
    char message_buffer[512];

    /* Trivial output, doesnt need a state set ! */
    p_min = AbstractState_keyed_output(abstract_state_handle_,
                                       (long)CoolProp::iP_min,
                                       &errcode,
                                       message_buffer,
                                       511);
    if (errcode != 0) {
      print_error_message_(message_buffer);
      return COMPUTE_ERROR_;
    }

    if (p_min < 1000.)
      p_min = 1000;

    return EOS_Internal_Error::OK;
  }

  // ---------------------------------------------------------------------------

  EOS_Internal_Error
  EOS_CoolProp::get_p_max(double& p_max) const
  {
    long errcode = 0;
    char message_buffer[512];

    /* Trivial output, doesnt need a state set ! */
    p_max = AbstractState_keyed_output(abstract_state_handle_,
                                       (long)CoolProp::iP_max,
                                       &errcode,
                                       message_buffer,
                                       511);

    if (errcode != 0) {
      print_error_message_(message_buffer);
      return COMPUTE_ERROR_;
    }

    return EOS_Internal_Error::OK;
  }

  // ---------------------------------------------------------------------------

  EOS_Internal_Error
  EOS_CoolProp::get_T_min(double& T_min) const
  {
    long errcode = 0;
    char message_buffer[512];

    /* Trivial output, doesnt need a state set ! */
    T_min = AbstractState_keyed_output(abstract_state_handle_,
                                       (long)CoolProp::iT_min,
                                       &errcode,
                                       message_buffer,
                                       511);

    if (errcode != 0) {
      print_error_message_(message_buffer);
      return COMPUTE_ERROR_;
    }

    return EOS_Internal_Error::OK;
  }

  // ---------------------------------------------------------------------------

  EOS_Internal_Error
  EOS_CoolProp::get_T_max(double& T_max) const
  {
    long errcode = 0;
    char message_buffer[512];

    /* Trivial output, doesnt need a state set ! */
    T_max = AbstractState_keyed_output(abstract_state_handle_,
                                       (long)CoolProp::iT_max,
                                       &errcode,
                                       message_buffer,
                                       511);

    if (errcode != 0) {
      print_error_message_(message_buffer);
      return COMPUTE_ERROR_;
    }

    return EOS_Internal_Error::OK;
  }

  // ---------------------------------------------------------------------------

  EOS_Internal_Error
  EOS_CoolProp::get_h_min(double& h_min) const
  {
    EOS_Internal_Error err_ = EOS_Internal_Error::OK;

    /* Trivial output, doesnt need a state set ! */
    double T;
    err_ = get_T_min(T);

    if (err_ != EOS_Internal_Error::OK)
      return err_;

    /* Trivial output, doesnt need a state set ! */
    double P;
    err_ = get_p_min(P);

    if (err_ != EOS_Internal_Error::OK)
      return err_;

    err_ = compute_h_pT(P, T, h_min);
    if (err_ != EOS_Internal_Error::OK) {
      h_min = 1.;
      err_ = EOS_Internal_Error::OK;
    }

    return err_;
  }

  // ---------------------------------------------------------------------------

  EOS_Internal_Error
  EOS_CoolProp::get_h_max(double& h_max) const
  {
    EOS_Internal_Error err_ = EOS_Internal_Error::OK;

    /* Trivial output, doesnt need a state set ! */
    double T;
    err_ = get_T_max(T);

    if (err_ != EOS_Internal_Error::OK)
      return err_;

    /* Trivial output, doesnt need a state set ! */
    double P;
    err_ = get_p_max(P);

    if (err_ != EOS_Internal_Error::OK)
      return err_;

    err_ = compute_h_pT(P*0.95, T*0.95, h_max);
    if (err_ != EOS_Internal_Error::OK) {
      h_max = 1.e20;
      err_ = EOS_Internal_Error::OK;
    }

    return err_;
  }

  // ---------------------------------------------------------------------------

  EOS_Internal_Error
  EOS_CoolProp::get_p_crit(double& p_crit) const
  {
    long errcode = 0;
    char message_buffer[512];

    /* Trivial output, doesnt need a state set ! */
    p_crit = AbstractState_keyed_output(abstract_state_handle_,
                                       (long)CoolProp::iP_critical,
                                       &errcode,
                                       message_buffer,
                                       511);

    if (errcode != 0) {
      print_error_message_(message_buffer);
      return COMPUTE_ERROR_;
    }

    return EOS_Internal_Error::OK;
  }

  // ---------------------------------------------------------------------------

  EOS_Internal_Error
  EOS_CoolProp::get_T_crit(double& T_crit) const
  {
    long errcode = 0;
    char message_buffer[512];

    /* Trivial output, doesnt need a state set ! */
    T_crit = AbstractState_keyed_output(abstract_state_handle_,
                                       (long)CoolProp::iT_critical,
                                       &errcode,
                                       message_buffer,
                                       511);

    if (errcode != 0) {
      print_error_message_(message_buffer);
      return COMPUTE_ERROR_;
    }

    return EOS_Internal_Error::OK;
  }

  // ---------------------------------------------------------------------------

  EOS_Internal_Error
  EOS_CoolProp::get_h_crit(double& h_crit) const
  {
    EOS_Internal_Error err_ = EOS_Internal_Error::OK;

    /* Trivial output, doesnt need a state set ! */
    double T;
    err_ = get_T_crit(T);

    if (err_ != EOS_Internal_Error::OK)
      return err_;

    /* Trivial output, doesnt need a state set ! */
    double P;
    err_ = get_p_crit(P);

    if (err_ != EOS_Internal_Error::OK)
      return err_;

    err_ = compute_h_pT(P, T, h_crit);

    return err_;
  }

  // ---------------------------------------------------------------------------

  EOS_Internal_Error
  EOS_CoolProp::get_mm(double& mm) const
  {
    /* Molar moss does not depend on parameters */
    mm = molar_mass_;
    return EOS_Internal_Error::OK;
  }

  /* ------------------------------------------------------------------------ */
  /* Partial derivative functions (P,H)*/
  /* ------------------------------------------------------------------------ */

  EOS_Internal_Error
  EOS_CoolProp::compute_d_T_d_p_h_ph(double p, double h, double& r) const
  {
    long errcode = 0;
    char message_buffer[512];

    AbstractState_update(abstract_state_handle_,
                         (long)CoolProp::HmassP_INPUTS,
                         h, p,
                         &errcode, message_buffer, 511);

    AbstractState_specify_phase(abstract_state_handle_,
                                handle_phase_.aschar(),
                                &errcode,
                                message_buffer,
                                511);

    r = AbstractState_first_partial_deriv(abstract_state_handle_,
                                             (long)CoolProp::iT,
                                             (long)CoolProp::iP,
                                             (long)CoolProp::iHmass,
                                             &errcode,
                                             message_buffer,
                                             511);

    if (errcode != 0) {
      print_error_message_(message_buffer);
      return COMPUTE_ERROR_;
    }

    return EOS_Internal_Error::OK;
  }

  // ---------------------------------------------------------------------------

  EOS_Internal_Error
  EOS_CoolProp::compute_d_T_d_h_p_ph(double p, double h, double& r) const
  {
    long errcode = 0;
    char message_buffer[512];

    AbstractState_update(abstract_state_handle_,
                         (long)CoolProp::HmassP_INPUTS,
                         h, p,
                         &errcode, message_buffer, 511);

    AbstractState_specify_phase(abstract_state_handle_,
                                handle_phase_.aschar(),
                                &errcode,
                                message_buffer,
                                511);

    r = AbstractState_first_partial_deriv(abstract_state_handle_,
                                             (long)CoolProp::iT,
                                             (long)CoolProp::iHmass,
                                             (long)CoolProp::iP,
                                             &errcode,
                                             message_buffer,
                                             511);

    if (errcode != 0) {
      print_error_message_(message_buffer);
      return COMPUTE_ERROR_;
    }

    return EOS_Internal_Error::OK;
  }

  /*
  EOS_Internal_Error
  EOS_CoolProp::compute_d_rho_d_p_h_ph(double p, double h, double& r) const
  {
    long errcode = 0;
    char message_buffer[512];

    AbstractState_update(abstract_state_handle_,
                         (long)CoolProp::HmassP_INPUTS,
                         h, p,
                         &errcode, message_buffer, 511);

    AbstractState_specify_phase(abstract_state_handle_,
                                handle_phase_.aschar(),
                                &errcode,
                                message_buffer,
                                511);

    r = AbstractState_first_partial_deriv(abstract_state_handle_,
                                             (long)CoolProp::iDmass,
                                             (long)CoolProp::iP,
                                             (long)CoolProp::iHmass,
                                             &errcode,
                                             message_buffer,
                                             511);

    if (errcode != 0) {
      print_error_message_(message_buffer);
      return COMPUTE_ERROR_;
    }

    return EOS_Internal_Error::OK;
  }

  EOS_Internal_Error
  EOS_CoolProp::compute_d_rho_d_h_p_ph(double p, double h, double& r) const
  {
    long errcode = 0;
    char message_buffer[512];

    AbstractState_update(abstract_state_handle_,
                         (long)CoolProp::HmassP_INPUTS,
                         h, p,
                         &errcode, message_buffer, 511);

    AbstractState_specify_phase(abstract_state_handle_,
                                handle_phase_.aschar(),
                                &errcode,
                                message_buffer,
                                511);

    r = AbstractState_first_partial_deriv(abstract_state_handle_,
                                             (long)CoolProp::iDmass,
                                             (long)CoolProp::iHmass,
                                             (long)CoolProp::iP,
                                             &errcode,
                                             message_buffer,
                                             511);

    if (errcode != 0) {
      print_error_message_(message_buffer);
      return COMPUTE_ERROR_;
    }

    return EOS_Internal_Error::OK;
  }

  EOS_Internal_Error
  EOS_CoolProp::compute_d_u_d_p_h_ph(double p, double h, double& r) const
  {
    long errcode = 0;
    char message_buffer[512];

    AbstractState_update(abstract_state_handle_,
                         (long)CoolProp::HmassP_INPUTS,
                         h, p,
                         &errcode, message_buffer, 511);

    AbstractState_specify_phase(abstract_state_handle_,
                                handle_phase_.aschar(),
                                &errcode,
                                message_buffer,
                                511);

    r = AbstractState_first_partial_deriv(abstract_state_handle_,
                                             (long)CoolProp::iUmass,
                                             (long)CoolProp::iP,
                                             (long)CoolProp::iHmass,
                                             &errcode,
                                             message_buffer,
                                             511);

    if (errcode != 0) {
      print_error_message_(message_buffer);
      return COMPUTE_ERROR_;
    }

    return EOS_Internal_Error::OK;
  }

  EOS_Internal_Error
  EOS_CoolProp::compute_d_u_d_h_p_ph(double p, double h, double& r) const
  {
    long errcode = 0;
    char message_buffer[512];

    AbstractState_update(abstract_state_handle_,
                         (long)CoolProp::HmassP_INPUTS,
                         h, p,
                         &errcode, message_buffer, 511);

    AbstractState_specify_phase(abstract_state_handle_,
                                handle_phase_.aschar(),
                                &errcode,
                                message_buffer,
                                511);

    r = AbstractState_first_partial_deriv(abstract_state_handle_,
                                             (long)CoolProp::iUmass,
                                             (long)CoolProp::iHmass,
                                             (long)CoolProp::iP,
                                             &errcode,
                                             message_buffer,
                                             511);

    if (errcode != 0) {
      print_error_message_(message_buffer);
      return COMPUTE_ERROR_;
    }

    return EOS_Internal_Error::OK;
  }

  EOS_Internal_Error
  EOS_CoolProp::compute_d_s_d_p_h_ph(double p, double h, double& r) const
  {
    long errcode = 0;
    char message_buffer[512];

    AbstractState_update(abstract_state_handle_,
                         (long)CoolProp::HmassP_INPUTS,
                         h, p,
                         &errcode, message_buffer, 511);

    AbstractState_specify_phase(abstract_state_handle_,
                                handle_phase_.aschar(),
                                &errcode,
                                message_buffer,
                                511);

    r = AbstractState_first_partial_deriv(abstract_state_handle_,
                                             (long)CoolProp::iSmass,
                                             (long)CoolProp::iP,
                                             (long)CoolProp::iHmass,
                                             &errcode,
                                             message_buffer,
                                             511);

    if (errcode != 0) {
      print_error_message_(message_buffer);
      return COMPUTE_ERROR_;
    }

    return EOS_Internal_Error::OK;
  }

  EOS_Internal_Error
  EOS_CoolProp::compute_d_s_d_h_p_ph(double p, double h, double& r) const
  {
    long errcode = 0;
    char message_buffer[512];

    AbstractState_update(abstract_state_handle_,
                         (long)CoolProp::HmassP_INPUTS,
                         h, p,
                         &errcode, message_buffer, 511);

    AbstractState_specify_phase(abstract_state_handle_,
                                handle_phase_.aschar(),
                                &errcode,
                                message_buffer,
                                511);

    r = AbstractState_first_partial_deriv(abstract_state_handle_,
                                             (long)CoolProp::iSmass,
                                             (long)CoolProp::iHmass,
                                             (long)CoolProp::iP,
                                             &errcode,
                                             message_buffer,
                                             511);

    if (errcode != 0) {
      print_error_message_(message_buffer);
      return COMPUTE_ERROR_;
    }

    return EOS_Internal_Error::OK;
  }

  EOS_Internal_Error
  EOS_CoolProp::compute_d_mu_d_p_h_ph(double p, double h, double& r) const
  {
    long errcode = 0;
    char message_buffer[512];

    AbstractState_update(abstract_state_handle_,
                         (long)CoolProp::HmassP_INPUTS,
                         h, p,
                         &errcode, message_buffer, 511);

    AbstractState_specify_phase(abstract_state_handle_,
                                handle_phase_.aschar(),
                                &errcode,
                                message_buffer,
                                511);

    r = AbstractState_first_partial_deriv(abstract_state_handle_,
                                             (long)CoolProp::iviscosity,
                                             (long)CoolProp::iP,
                                             (long)CoolProp::iHmass,
                                             &errcode,
                                             message_buffer,
                                             511);

    if (errcode != 0) {
      print_error_message_(message_buffer);
      return COMPUTE_ERROR_;
    }

    return EOS_Internal_Error::OK;
  }

  EOS_Internal_Error
  EOS_CoolProp::compute_d_mu_d_h_p_ph(double p, double h, double& r) const
  {
    long errcode = 0;
    char message_buffer[512];

    AbstractState_update(abstract_state_handle_,
                         (long)CoolProp::HmassP_INPUTS,
                         h, p,
                         &errcode, message_buffer, 511);

    AbstractState_specify_phase(abstract_state_handle_,
                                handle_phase_.aschar(),
                                &errcode,
                                message_buffer,
                                511);

    r = AbstractState_first_partial_deriv(abstract_state_handle_,
                                             (long)CoolProp::iviscosity,
                                             (long)CoolProp::iHmass,
                                             (long)CoolProp::iP,
                                             &errcode,
                                             message_buffer,
                                             511);

    if (errcode != 0) {
      print_error_message_(message_buffer);
      return COMPUTE_ERROR_;
    }

    return EOS_Internal_Error::OK;
  }

  EOS_Internal_Error
  EOS_CoolProp::compute_d_lambda_d_p_h_ph(double p, double h, double& r) const
  {
    long errcode = 0;
    char message_buffer[512];

    AbstractState_update(abstract_state_handle_,
                         (long)CoolProp::HmassP_INPUTS,
                         h, p,
                         &errcode, message_buffer, 511);

    AbstractState_specify_phase(abstract_state_handle_,
                                handle_phase_.aschar(),
                                &errcode,
                                message_buffer,
                                511);

    r = AbstractState_first_partial_deriv(abstract_state_handle_,
                                             (long)CoolProp::iconductivity,
                                             (long)CoolProp::iP,
                                             (long)CoolProp::iHmass,
                                             &errcode,
                                             message_buffer,
                                             511);

    if (errcode != 0) {
      print_error_message_(message_buffer);
      return COMPUTE_ERROR_;
    }

    return EOS_Internal_Error::OK;
  }

  EOS_Internal_Error
  EOS_CoolProp::compute_d_lambda_d_h_p_ph(double p, double h, double& r) const
  {
    long errcode = 0;
    char message_buffer[512];

    AbstractState_update(abstract_state_handle_,
                         (long)CoolProp::HmassP_INPUTS,
                         h, p,
                         &errcode, message_buffer, 511);

    AbstractState_specify_phase(abstract_state_handle_,
                                handle_phase_.aschar(),
                                &errcode,
                                message_buffer,
                                511);

    r = AbstractState_first_partial_deriv(abstract_state_handle_,
                                             (long)CoolProp::iviscosity,
                                             (long)CoolProp::iHmass,
                                             (long)CoolProp::iP,
                                             &errcode,
                                             message_buffer,
                                             511);

    if (errcode != 0) {
      print_error_message_(message_buffer);
      return COMPUTE_ERROR_;
    }

    return EOS_Internal_Error::OK;
  }

  EOS_Internal_Error
  EOS_CoolProp::compute_d_cp_d_p_h_ph(double p, double h, double& r) const
  {
    long errcode = 0;
    char message_buffer[512];

    AbstractState_update(abstract_state_handle_,
                         (long)CoolProp::HmassP_INPUTS,
                         h, p,
                         &errcode, message_buffer, 511);

    AbstractState_specify_phase(abstract_state_handle_,
                                handle_phase_.aschar(),
                                &errcode,
                                message_buffer,
                                511);

    r = AbstractState_first_partial_deriv(abstract_state_handle_,
                                             (long)CoolProp::iCpmass,
                                             (long)CoolProp::iP,
                                             (long)CoolProp::iHmass,
                                             &errcode,
                                             message_buffer,
                                             511);

    if (errcode != 0) {
      print_error_message_(message_buffer);
      return COMPUTE_ERROR_;
    }

    return EOS_Internal_Error::OK;
  }

  EOS_Internal_Error
  EOS_CoolProp::compute_d_cp_d_h_p_ph(double p, double h, double& r) const
  {
    long errcode = 0;
    char message_buffer[512];

    AbstractState_update(abstract_state_handle_,
                         (long)CoolProp::HmassP_INPUTS,
                         h, p,
                         &errcode, message_buffer, 511);

    AbstractState_specify_phase(abstract_state_handle_,
                                handle_phase_.aschar(),
                                &errcode,
                                message_buffer,
                                511);

    r = AbstractState_first_partial_deriv(abstract_state_handle_,
                                             (long)CoolProp::iCpmass,
                                             (long)CoolProp::iHmass,
                                             (long)CoolProp::iP,
                                             &errcode,
                                             message_buffer,
                                             511);

    if (errcode != 0) {
      print_error_message_(message_buffer);
      return COMPUTE_ERROR_;
    }

    return EOS_Internal_Error::OK;
  }

  EOS_Internal_Error
  EOS_CoolProp::compute_d_cv_d_p_h_ph(double p, double h, double& r) const
  {
    long errcode = 0;
    char message_buffer[512];

    AbstractState_update(abstract_state_handle_,
                         (long)CoolProp::HmassP_INPUTS,
                         h, p,
                         &errcode, message_buffer, 511);

    AbstractState_specify_phase(abstract_state_handle_,
                                handle_phase_.aschar(),
                                &errcode,
                                message_buffer,
                                511);

    r = AbstractState_first_partial_deriv(abstract_state_handle_,
                                             (long)CoolProp::iCvmass,
                                             (long)CoolProp::iP,
                                             (long)CoolProp::iHmass,
                                             &errcode,
                                             message_buffer,
                                             511);

    if (errcode != 0) {
      print_error_message_(message_buffer);
      return COMPUTE_ERROR_;
    }

    return EOS_Internal_Error::OK;
  }

  EOS_Internal_Error
  EOS_CoolProp::compute_d_cv_d_h_p_ph(double p, double h, double& r) const
  {
    long errcode = 0;
    char message_buffer[512];

    AbstractState_update(abstract_state_handle_,
                         (long)CoolProp::HmassP_INPUTS,
                         h, p,
                         &errcode, message_buffer, 511);

    AbstractState_specify_phase(abstract_state_handle_,
                                handle_phase_.aschar(),
                                &errcode,
                                message_buffer,
                                511);

    r = AbstractState_first_partial_deriv(abstract_state_handle_,
                                             (long)CoolProp::iCvmass,
                                             (long)CoolProp::iHmass,
                                             (long)CoolProp::iP,
                                             &errcode,
                                             message_buffer,
                                             511);

    if (errcode != 0) {
      print_error_message_(message_buffer);
      return COMPUTE_ERROR_;
    }

    return EOS_Internal_Error::OK;
  }
  */

  /* ------------------------------------------------------------------------ */
  /* Partial derivative functions (P,T)*/
  /* ------------------------------------------------------------------------ */

  EOS_Internal_Error
  EOS_CoolProp::compute_d_h_d_p_T_pT(double p, double T, double& r) const
  {
    long errcode = 0;
    char message_buffer[512];

    AbstractState_update(abstract_state_handle_,
                         (long)CoolProp::PT_INPUTS,
                         p, T,
                         &errcode, message_buffer, 511);

    AbstractState_specify_phase(abstract_state_handle_,
                                handle_phase_.aschar(),
                                &errcode,
                                message_buffer,
                                511);

    r = AbstractState_first_partial_deriv(abstract_state_handle_,
                                             (long)CoolProp::iHmass,
                                             (long)CoolProp::iP,
                                             (long)CoolProp::iT,
                                             &errcode,
                                             message_buffer,
                                             511);

    if (errcode != 0) {
      print_error_message_(message_buffer);
      return COMPUTE_ERROR_;
    }

    return EOS_Internal_Error::OK;
  }

  // ---------------------------------------------------------------------------

  EOS_Internal_Error
  EOS_CoolProp::compute_d_h_d_T_p_pT(double p, double T, double& r) const
  {
    long errcode = 0;
    char message_buffer[512];

    AbstractState_update(abstract_state_handle_,
                         (long)CoolProp::PT_INPUTS,
                         p, T,
                         &errcode, message_buffer, 511);

    AbstractState_specify_phase(abstract_state_handle_,
                                handle_phase_.aschar(),
                                &errcode,
                                message_buffer,
                                511);

    r = AbstractState_first_partial_deriv(abstract_state_handle_,
                                             (long)CoolProp::iHmass,
                                             (long)CoolProp::iT,
                                             (long)CoolProp::iP,
                                             &errcode,
                                             message_buffer,
                                             511);

    if (errcode != 0) {
      print_error_message_(message_buffer);
      return COMPUTE_ERROR_;
    }

    return EOS_Internal_Error::OK;
  }

  // ---------------------------------------------------------------------------

  /*
  EOS_Internal_Error
  EOS_CoolProp::compute_d_rho_d_p_T_pT(double p, double T, double& r) const
  {
    long errcode = 0;
    char message_buffer[512];

    AbstractState_update(abstract_state_handle_,
                         (long)CoolProp::PT_INPUTS,
                         p, T,
                         &errcode, message_buffer, 511);

    AbstractState_specify_phase(abstract_state_handle_,
                                handle_phase_.aschar(),
                                &errcode,
                                message_buffer,
                                511);

    r = AbstractState_first_partial_deriv(abstract_state_handle_,
                                             (long)CoolProp::iDmass,
                                             (long)CoolProp::iP,
                                             (long)CoolProp::iT,
                                             &errcode,
                                             message_buffer,
                                             511);

    if (errcode != 0) {
      print_error_message_(message_buffer);
      return COMPUTE_ERROR_;
    }

    return EOS_Internal_Error::OK;
  }

  EOS_Internal_Error
  EOS_CoolProp::compute_d_rho_d_T_p_pT(double p, double T, double& r) const
  {
    long errcode = 0;
    char message_buffer[512];

    AbstractState_update(abstract_state_handle_,
                         (long)CoolProp::PT_INPUTS,
                         p, T,
                         &errcode, message_buffer, 511);

    AbstractState_specify_phase(abstract_state_handle_,
                                handle_phase_.aschar(),
                                &errcode,
                                message_buffer,
                                511);

    r = AbstractState_first_partial_deriv(abstract_state_handle_,
                                             (long)CoolProp::iDmass,
                                             (long)CoolProp::iT,
                                             (long)CoolProp::iP,
                                             &errcode,
                                             message_buffer,
                                             511);

    if (errcode != 0) {
      print_error_message_(message_buffer);
      return COMPUTE_ERROR_;
    }

    return EOS_Internal_Error::OK;
  }

  EOS_Internal_Error
  EOS_CoolProp::compute_d_u_d_p_T_pT(double p, double T, double& r) const
  {
    long errcode = 0;
    char message_buffer[512];

    AbstractState_update(abstract_state_handle_,
                         (long)CoolProp::PT_INPUTS,
                         p, T,
                         &errcode, message_buffer, 511);

    AbstractState_specify_phase(abstract_state_handle_,
                                handle_phase_.aschar(),
                                &errcode,
                                message_buffer,
                                511);

    r = AbstractState_first_partial_deriv(abstract_state_handle_,
                                             (long)CoolProp::iUmass,
                                             (long)CoolProp::iP,
                                             (long)CoolProp::iT,
                                             &errcode,
                                             message_buffer,
                                             511);

    if (errcode != 0) {
      print_error_message_(message_buffer);
      return COMPUTE_ERROR_;
    }

    return EOS_Internal_Error::OK;
  }

  EOS_Internal_Error
  EOS_CoolProp::compute_d_u_d_T_p_pT(double p, double T, double& r) const
  {
    long errcode = 0;
    char message_buffer[512];

    AbstractState_update(abstract_state_handle_,
                         (long)CoolProp::PT_INPUTS,
                         p, T,
                         &errcode, message_buffer, 511);

    AbstractState_specify_phase(abstract_state_handle_,
                                handle_phase_.aschar(),
                                &errcode,
                                message_buffer,
                                511);

    r = AbstractState_first_partial_deriv(abstract_state_handle_,
                                             (long)CoolProp::iUmass,
                                             (long)CoolProp::iT,
                                             (long)CoolProp::iP,
                                             &errcode,
                                             message_buffer,
                                             511);

    if (errcode != 0) {
      print_error_message_(message_buffer);
      return COMPUTE_ERROR_;
    }

    return EOS_Internal_Error::OK;
  }

  EOS_Internal_Error
  EOS_CoolProp::compute_d_s_d_p_T_pT(double p, double T, double& r) const
  {
    long errcode = 0;
    char message_buffer[512];

    AbstractState_update(abstract_state_handle_,
                         (long)CoolProp::PT_INPUTS,
                         p, T,
                         &errcode, message_buffer, 511);

    AbstractState_specify_phase(abstract_state_handle_,
                                handle_phase_.aschar(),
                                &errcode,
                                message_buffer,
                                511);

    r = AbstractState_first_partial_deriv(abstract_state_handle_,
                                             (long)CoolProp::iSmass,
                                             (long)CoolProp::iP,
                                             (long)CoolProp::iT,
                                             &errcode,
                                             message_buffer,
                                             511);

    if (errcode != 0) {
      print_error_message_(message_buffer);
      return COMPUTE_ERROR_;
    }

    return EOS_Internal_Error::OK;
  }

  EOS_Internal_Error
  EOS_CoolProp::compute_d_s_d_T_p_pT(double p, double T, double& r) const
  {
    long errcode = 0;
    char message_buffer[512];

    AbstractState_update(abstract_state_handle_,
                         (long)CoolProp::PT_INPUTS,
                         p, T,
                         &errcode, message_buffer, 511);

    AbstractState_specify_phase(abstract_state_handle_,
                                handle_phase_.aschar(),
                                &errcode,
                                message_buffer,
                                511);

    r = AbstractState_first_partial_deriv(abstract_state_handle_,
                                             (long)CoolProp::iSmass,
                                             (long)CoolProp::iT,
                                             (long)CoolProp::iP,
                                             &errcode,
                                             message_buffer,
                                             511);

    if (errcode != 0) {
      print_error_message_(message_buffer);
      return COMPUTE_ERROR_;
    }

    return EOS_Internal_Error::OK;
  }

  EOS_Internal_Error
  EOS_CoolProp::compute_d_mu_d_p_T_pT(double p, double T, double& r) const
  {
    long errcode = 0;
    char message_buffer[512];

    AbstractState_update(abstract_state_handle_,
                         (long)CoolProp::PT_INPUTS,
                         p, T,
                         &errcode, message_buffer, 511);

    AbstractState_specify_phase(abstract_state_handle_,
                                handle_phase_.aschar(),
                                &errcode,
                                message_buffer,
                                511);

    r = AbstractState_first_partial_deriv(abstract_state_handle_,
                                             (long)CoolProp::iviscosity,
                                             (long)CoolProp::iP,
                                             (long)CoolProp::iT,
                                             &errcode,
                                             message_buffer,
                                             511);

    if (errcode != 0) {
      print_error_message_(message_buffer);
      return COMPUTE_ERROR_;
    }

    return EOS_Internal_Error::OK;
  }

  EOS_Internal_Error
  EOS_CoolProp::compute_d_mu_d_T_p_pT(double p, double T, double& r) const
  {
    long errcode = 0;
    char message_buffer[512];

    AbstractState_update(abstract_state_handle_,
                         (long)CoolProp::PT_INPUTS,
                         p, T,
                         &errcode, message_buffer, 511);

    AbstractState_specify_phase(abstract_state_handle_,
                                handle_phase_.aschar(),
                                &errcode,
                                message_buffer,
                                511);

    r = AbstractState_first_partial_deriv(abstract_state_handle_,
                                             (long)CoolProp::iviscosity,
                                             (long)CoolProp::iT,
                                             (long)CoolProp::iP,
                                             &errcode,
                                             message_buffer,
                                             511);

    if (errcode != 0) {
      print_error_message_(message_buffer);
      return COMPUTE_ERROR_;
    }

    return EOS_Internal_Error::OK;
  }

  EOS_Internal_Error
  EOS_CoolProp::compute_d_lambda_d_p_T_pT(double p, double T, double& r) const
  {
    long errcode = 0;
    char message_buffer[512];

    AbstractState_update(abstract_state_handle_,
                         (long)CoolProp::PT_INPUTS,
                         p, T,
                         &errcode, message_buffer, 511);

    AbstractState_specify_phase(abstract_state_handle_,
                                handle_phase_.aschar(),
                                &errcode,
                                message_buffer,
                                511);

    r = AbstractState_first_partial_deriv(abstract_state_handle_,
                                             (long)CoolProp::iconductivity,
                                             (long)CoolProp::iP,
                                             (long)CoolProp::iT,
                                             &errcode,
                                             message_buffer,
                                             511);

    if (errcode != 0) {
      print_error_message_(message_buffer);
      return COMPUTE_ERROR_;
    }

    return EOS_Internal_Error::OK;
  }

  EOS_Internal_Error
  EOS_CoolProp::compute_d_lambda_d_T_p_pT(double p, double T, double& r) const
  {
    long errcode = 0;
    char message_buffer[512];

    AbstractState_update(abstract_state_handle_,
                         (long)CoolProp::PT_INPUTS,
                         p, T,
                         &errcode, message_buffer, 511);

    AbstractState_specify_phase(abstract_state_handle_,
                                handle_phase_.aschar(),
                                &errcode,
                                message_buffer,
                                511);

    r = AbstractState_first_partial_deriv(abstract_state_handle_,
                                             (long)CoolProp::iviscosity,
                                             (long)CoolProp::iT,
                                             (long)CoolProp::iP,
                                             &errcode,
                                             message_buffer,
                                             511);

    if (errcode != 0) {
      print_error_message_(message_buffer);
      return COMPUTE_ERROR_;
    }

    return EOS_Internal_Error::OK;
  }

  EOS_Internal_Error
  EOS_CoolProp::compute_d_cp_d_p_T_pT(double p, double T, double& r) const
  {
    long errcode = 0;
    char message_buffer[512];

    AbstractState_update(abstract_state_handle_,
                         (long)CoolProp::PT_INPUTS,
                         p, T,
                         &errcode, message_buffer, 511);

    AbstractState_specify_phase(abstract_state_handle_,
                                handle_phase_.aschar(),
                                &errcode,
                                message_buffer,
                                511);

    r = AbstractState_first_partial_deriv(abstract_state_handle_,
                                             (long)CoolProp::iCpmass,
                                             (long)CoolProp::iP,
                                             (long)CoolProp::iT,
                                             &errcode,
                                             message_buffer,
                                             511);

    if (errcode != 0) {
      print_error_message_(message_buffer);
      return COMPUTE_ERROR_;
    }

    return EOS_Internal_Error::OK;
  }

  EOS_Internal_Error
  EOS_CoolProp::compute_d_cp_d_T_p_pT(double p, double T, double& r) const
  {
    long errcode = 0;
    char message_buffer[512];

    AbstractState_update(abstract_state_handle_,
                         (long)CoolProp::PT_INPUTS,
                         p, T,
                         &errcode, message_buffer, 511);

    AbstractState_specify_phase(abstract_state_handle_,
                                handle_phase_.aschar(),
                                &errcode,
                                message_buffer,
                                511);

    r = AbstractState_first_partial_deriv(abstract_state_handle_,
                                             (long)CoolProp::iCpmass,
                                             (long)CoolProp::iT,
                                             (long)CoolProp::iP,
                                             &errcode,
                                             message_buffer,
                                             511);

    if (errcode != 0) {
      print_error_message_(message_buffer);
      return COMPUTE_ERROR_;
    }

    return EOS_Internal_Error::OK;
  }

  EOS_Internal_Error
  EOS_CoolProp::compute_d_cv_d_p_T_pT(double p, double T, double& r) const
  {
    long errcode = 0;
    char message_buffer[512];

    AbstractState_update(abstract_state_handle_,
                         (long)CoolProp::PT_INPUTS,
                         p, T,
                         &errcode, message_buffer, 511);

    AbstractState_specify_phase(abstract_state_handle_,
                                handle_phase_.aschar(),
                                &errcode,
                                message_buffer,
                                511);

    r = AbstractState_first_partial_deriv(abstract_state_handle_,
                                             (long)CoolProp::iCvmass,
                                             (long)CoolProp::iP,
                                             (long)CoolProp::iT,
                                             &errcode,
                                             message_buffer,
                                             511);

    if (errcode != 0) {
      print_error_message_(message_buffer);
      return COMPUTE_ERROR_;
    }

    return EOS_Internal_Error::OK;
  }

  EOS_Internal_Error
  EOS_CoolProp::compute_d_cv_d_T_p_pT(double p, double T, double& r) const
  {
    long errcode = 0;
    char message_buffer[512];

    AbstractState_update(abstract_state_handle_,
                         (long)CoolProp::PT_INPUTS,
                         p, T,
                         &errcode, message_buffer, 511);

    AbstractState_specify_phase(abstract_state_handle_,
                                handle_phase_.aschar(),
                                &errcode,
                                message_buffer,
                                511);

    r = AbstractState_first_partial_deriv(abstract_state_handle_,
                                             (long)CoolProp::iCvmass,
                                             (long)CoolProp::iT,
                                             (long)CoolProp::iP,
                                             &errcode,
                                             message_buffer,
                                             511);

    if (errcode != 0) {
      print_error_message_(message_buffer);
      return COMPUTE_ERROR_;
    }

    return EOS_Internal_Error::OK;
  }
  */

}
