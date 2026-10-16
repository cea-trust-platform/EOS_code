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




#ifndef __EOS_CoolProp_HH__
#define __EOS_CoolProp_HH__

#include <exception>
#include <memory>
#include <string>

#include "EOS/API/EOS_Fluid.hxx"
#include "EOS/API/EOS_Field.hxx"
#include "EOS/API/EOS_Fields.hxx"
#include "EOS/API/EOS_Error.hxx"
#include "EOS/API/EOS_Error_Field.hxx"
#include "Language/API/Types_Info.hxx"

using namespace NEPTUNE ;

// The CoolProp headers are only needed by the implementation
namespace CoolProp
{
  class AbstractState ;
}

namespace NEPTUNE_EOS
{
  //! Equation of state provided by the CoolProp library (HEOS backend)
  /*!
   * One CoolProp::AbstractState is held per object and kept in the phase
   * given at initialisation (phase_liquid or phase_gas). The field methods
   * update that state once per point and read every requested output from
   * it, so a call costs one flash per point whatever the number of outputs.
   * Derivatives are taken analytically whenever CoolProp provides them, by
   * centred differences otherwise. Outputs CoolProp cannot evaluate are
   * delegated to the generic EOS_Fluid implementation.
   */
  class EOS_CoolProp : public EOS_Fluid
  {
  public:
    //! Constructor method
    EOS_CoolProp();

    //! Destructor method
    ~EOS_CoolProp();

    //! Getter methods
    const AString& table_name()    const ;
    const AString& version_name()  const ;
    const AString& phase_name()    const ;
    const AString& fluid_name()    const ;
    const AString& equation_name() const ;

    //! Initialisation from the index.eos reference
    /*!
     * \param[in] strings { CoolProp fluid name, "phase_liquid" or "phase_gas" }
     * \return EOS_Error::good on success, EOS_Error::error otherwise
     */
    int init(const Strings& strings);
    //! Not supported by this model : returns EOS_Error::error
    int init(const Strings&, const Strings&);
    //! Not supported by this model : returns EOS_Error::error
    int init(const Strings&, const Strings&, const double, const double, const double, const double);

    //! Compute saturation properties from one input field
    /*!
     * \param[in] input input field. Currently supported are : p (also p_sat, p_lim), T (also T_sat)
     * \param[in,out] outputs output field array. All fields must have the correct size.
     * \param[in,out] err error field. Must have the correct size.
     * \return worst error found in error field
     */
    EOS_Error compute(const EOS_Field& input, EOS_Fields& outputs, EOS_Error_Field& err) const;

    //! Compute one saturation property from one input field
    /*!
     * \param[in] input input field. Currently supported are : p (also p_sat, p_lim), T (also T_sat)
     * \param[in,out] output output field. Must have the correct size.
     * \param[in,out] err error field. Must have the correct size.
     * \return worst error found in error field
     */
    EOS_Error compute(const EOS_Field& input, EOS_Field& output, EOS_Error_Field& err) const;

    //! Compute properties from two input fields
    /*!
     * \param[in] input1 input field. Currently supported are : p, h, T, s
     * \param[in] input2 input field. Currently supported are : p, h, T, s
     * \param[in,out] outputs output field array. All fields must have the correct size.
     * \param[in,out] err error field. Must have the correct size.
     * \return worst error found in error field
     */
    EOS_Error compute(const EOS_Field& input1, const EOS_Field& input2, EOS_Fields& outputs, EOS_Error_Field& err) const;

    //! Compute one property from two input fields
    /*!
     * \param[in] input1 input field. Currently supported are : p, h, T, s
     * \param[in] input2 input field. Currently supported are : p, h, T, s
     * \param[in,out] output output field. Must have the correct size.
     * \param[in,out] err error field. Must have the correct size.
     * \return worst error found in error field
     */
    EOS_Error compute(const EOS_Field& input1, const EOS_Field& input2, EOS_Field& output, EOS_Error_Field& err) const;

    //! General fluid properties, evaluated once at initialisation
    virtual EOS_Internal_Error get_T_min(double&) const;
    virtual EOS_Internal_Error get_T_max(double&) const;
    virtual EOS_Internal_Error get_p_min(double&) const;
    virtual EOS_Internal_Error get_p_max(double&) const;
    virtual EOS_Internal_Error get_h_min(double&) const;
    virtual EOS_Internal_Error get_h_max(double&) const;
    virtual EOS_Internal_Error get_T_crit(double&) const;
    virtual EOS_Internal_Error get_p_crit(double&) const;
    virtual EOS_Internal_Error get_h_crit(double&) const;
    //! Molar mass (kg/mol)
    virtual EOS_Internal_Error get_mm(double&) const;

    //
    //  Other methods
    //
    //! see Language
    ostream& print_On (ostream& stream=cout) const;
    //! see Language
    istream& read_On (istream& stream=cin);
    //! see Language
    const Type_Info& get_Type_Info () const;
    //! Error method
    void describe_error(const EOS_Internal_Error error, AString & description) const;

  private:
    //! Record a CoolProp failure for point idata and keep its message for describe_error()
    void record_failure(EOS_Error_Field& err, int idata, const std::exception& e) const;

    std::unique_ptr<CoolProp::AbstractState> state_ ;
    bool                liquid_ ;         //!< Phase imposed to the single-phase flashes : liquid or gas
    AString             fluid_name_ ;     //!< CoolProp fluid name
    AString             phase_name_ ;     //!< "phase_liquid" or "phase_gas"
    mutable std::string last_message_ ;   //!< Message of the last CoolProp exception

    double p_min_  ;
    double p_max_  ;
    double T_min_  ;
    double T_max_  ;
    double h_min_  ;
    double h_max_  ;
    double p_crit_ ;
    double T_crit_ ;
    double h_crit_ ;  //!< NaN when CoolProp could not evaluate it
    double mm_     ;
  };
}
#endif
