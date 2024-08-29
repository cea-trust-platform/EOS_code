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



#ifndef EOS_CATHARE2_HXX
#define EOS_CATHARE2_HXX

#include "EOS/Src/EOS_Cathare2/Cathare2.hxx"
#include "EOS/Src/EOS_Cathare2/Cathare2_fld.hxx"
#include "EOS/API/EOS_Fluid.hxx"
#include "Language/API/Types_Info.hxx"

using namespace NEPTUNE ;

namespace NEPTUNE_EOS
{

  //! EOS_Cathare2 : adapter to Hydcom/Eos Cathare2 subroutines
  /*!
   * EOS_Cathare2 is abstract: you should use EOS_Cathare2Liquid
   * or EOS_Cathare2Vapor instead.
   */
  class EOS_Cathare2 : public EOS_Fluid
  {
   static const AString tablename   ;
   static const AString versionname ;

  public:

    virtual const AString& table_name()    const ;
    virtual const AString& version_name()  const ;
    virtual const AString& equation_name() const ;

    //! Output stream method: see language documentation
    ostream& print_On (ostream& stream) const;
    //! Input stream method: see language documentation
    istream& read_On (istream& stream);
    //! Type information method: see language documentation
    virtual const Type_Info& get_Type_Info () const;

    //! Main constructor for class EOS_Cathare2
    /*!
       \param[in] phase the fluid phase (CATHARE2::liquid or CATHARE2::vapor).
       \return The EOS_Cathare2 pointer
       \sa CATHARE2::domain
     */
    EOS_Cathare2();
    EOS_Cathare2(CATHARE2::domain phase);

    //! Copy constructor. Should NOT be used.
    EOS_Cathare2(const EOS_Cathare2&);

    //! EOS_Cathare2 destructor
    /*!
     * ~EOS_Cathare2() is pure virtual to force EOS_Cathare2 to be abstract
     */
    virtual ~EOS_Cathare2();

    virtual const AString&  fluid_name() const;

    //! Return the phase name
    /*!
     * \return the phase name
     */
//     virtual const AString&  phase_name() const;

    //! Initialize the EOS_Fluid object
    /*!
     * On first call, args[0] must be a fluid name. Authorized fluids are those defined in
     * the linked Cathare2-source product.
     * Other arguments are pairs of two strings, the second one representing an integer
     * Authorized arguments are : ICAR, ICARGAS, IENC, IREV, ITERMIN, LATYPML, MUTYPML
     * \param[in] args an array of parameter strings
     * \return 2 on success, -1 on error
     * \todo Change return type to EOS_Error
     */
    int init(const Strings& args);

    //! Initialize the EOS_Fluid object
    /*!
     * Usage is the same as init(const Strings& args)
     * \param[in] args1 an array of parameter strings
     * \param[in] args2 another array of parameter strings
     * \return 2 on success, -1 on error
     * \todo Change return type to EOS_Error
     */
    int init(const Strings& args1, const Strings& args2);

    //! Compute calculations from fields
    /*!
     * \param[in] in input field. Currently supported are : P
     * \param[in,out] out output field array. All fields must have the correct size.
     * \param[in,out] err error field. Must have the correct size.
     * \return worst error found in error field
     */
    EOS_Error compute (const EOS_Field& in, EOS_Fields& out, EOS_Error_Field& err) const;

    //! Compute calculations from fields
    /*!
     * \param[in] in1 input field. Currently supported are : P, h
     * \param[in] in2 input field. Currently supported are : P, h
     * \param[in,out] out output field array. All fields must have the correct size.
     * \param[in,out] err error field. Must have the correct size.
     * \return worst error found in error field
     */
    EOS_Error compute (const EOS_Field& in1, const EOS_Field& in2, EOS_Fields& out, EOS_Error_Field& err) const;

    //! Compute calculations from fields
    /*!
     * \param[in] in1 input field. Currently supported are : P, h
     * \param[in] in2 input field. Currently supported are : P, h
     * \param[in,out] out output field . Must have the correct size.
     * \param[in,out] err error field. Must have the correct size.
     * \return worst error found in error field
     */
    EOS_Error compute (const EOS_Field& in1, const EOS_Field& in2, EOS_Field& out, EOS_Error_Field& err) const;

    //! Compute calculations from fields
    /*!
     * \param[in] in input field. Currently supported are : P, h
     * \param[in,out] out output field . Must have the correct size.
     * \param[in,out] err error field. Must have the correct size.
     * \return worst error found in error field
     */
    EOS_Error compute (const EOS_Field& in, EOS_Field& out, EOS_Error_Field& err) const;

    //! Compute calculations from fields
    /*!
     * \param[in] in input field array.
     * \param[in,out] out output field array. All fields must have the correct size.
     * \param[in,out] err error field. Must have the correct size.
     * \return worst error found in error field
     */
    EOS_Error compute (const EOS_Fields& in, EOS_Fields& out, EOS_Error_Field& err) const;

    EOS_Error compute_Ph(const char* const property_name, double in1, double in2, double& out) const ;
    EOS_Error compute_PT(const char* const property_name, double in1, double in2, double& out) const ;
    EOS_Error compute_Psat(const char* const property_name, double in, double& out) const ;
    EOS_Error compute_Tsat(const char* const property_name, double in, double& out) const ;

    //General fluid properties
    //!  p at critical point
    virtual EOS_Internal_Error get_p_crit(double&) const;
    //!  h at critical point
    virtual EOS_Internal_Error get_h_crit(double&) const;
    //!  T at critical point
    virtual EOS_Internal_Error get_T_crit(double&) const;
    //!  molar mass
    virtual EOS_Internal_Error get_mm(double&) const;
    //!  p limits (min)
    virtual EOS_Internal_Error get_p_min(double&) const;
    //!  h limits (min)
    virtual EOS_Internal_Error get_h_min(double&) const;
    //!  T limits (min)
    virtual EOS_Internal_Error get_T_min(double&) const;
    //!  p limits (max)
    virtual EOS_Internal_Error get_p_max(double&) const;
    //!  h limits (max)
    virtual EOS_Internal_Error get_h_max(double&) const;
    //!  T limits (max)
    virtual EOS_Internal_Error get_T_max(double&) const;
    //!  p at reference state (for reference enthalpy)
    virtual EOS_Internal_Error get_p_ref(double&) const;
    //!  h at reference state
    virtual EOS_Internal_Error get_h_ref(double&) const;
    //!  T at reference state (for reference enthalpy)
    virtual EOS_Internal_Error get_T_ref(double&) const;
    //End General fluid properties

    //! Limit liquid enthalpy
    /*!
     * \param[in] p the pressure
     * \param[out] h_l_lim the limit liquid enthalpy
     * \return an error code
     */
    EOS_Internal_Error compute_h_l_lim_p( double p, double& h_l_lim ) const;

    //! Limit vapor enthalpy
    /*!
     * \param[in] p the pressure
     * \param[out] h_v_lim the limit vapor enthalpy
     * \return an error code
     */
    EOS_Internal_Error compute_h_v_lim_p( double p, double& h_v_lim ) const;

    //! Describe error
    /*!
     * \param[in] error the EOS_Internal_Error code
     * \param[out] description an human readable description of the error
     * If error is not a CATHARE2 specific error code, use EOS_Fluid::describe_error instead
     */
    void describe_error(const EOS_Internal_Error error, AString & description) const;

  protected:
    CATHARE2::CATHARE2 *pilot;   //!< pointer to the pilot class
    CATHARE2::domain phase;      //!< liquid or vapor
    AString fluidname;         //!< fluid name
    AString equationname;           //!< fluid name (with phase)

  private:
    //! Type identifier: see language documentation
    static int type_Id;
  };

  inline EOS_Internal_Error EOS_Cathare2::get_mm(double& mm) const
  { assert(pilot);
    return pilot->get_mm( mm);
  }

  inline EOS_Internal_Error EOS_Cathare2::get_p_crit(double& p_crit) const
  { assert(pilot);
    return pilot->get_p_crit( p_crit);
  }

  inline EOS_Internal_Error EOS_Cathare2::get_h_crit(double& h_crit) const
  { assert(pilot);
    return pilot->get_h_crit( h_crit);
  }

  inline EOS_Internal_Error EOS_Cathare2::get_T_crit(double& T_crit) const
  { assert(pilot);
    return pilot->get_T_crit( T_crit);
  }

  inline EOS_Internal_Error EOS_Cathare2::get_p_min(double& p_min) const
  { assert(pilot);
    return pilot->get_p_min( p_min);
  }
  inline EOS_Internal_Error EOS_Cathare2::get_p_max(double& p_max) const
  { assert(pilot);
    return pilot->get_p_max( p_max);
  }
  inline EOS_Internal_Error EOS_Cathare2::get_h_min(double& h_min) const
  { assert(pilot);
    return pilot->get_h_min( h_min);
  }

  inline EOS_Internal_Error EOS_Cathare2::get_h_max(double& h_max) const
  { assert(pilot);
    return pilot->get_h_max( h_max);
  }

  inline EOS_Internal_Error EOS_Cathare2::get_T_min(double& T_min) const
  { assert(pilot);
    return pilot->get_T_min( T_min);
  }

  inline EOS_Internal_Error EOS_Cathare2::get_T_max(double& T_max) const
  { assert(pilot);
    return pilot->get_T_max( T_max);
  }

  inline EOS_Internal_Error EOS_Cathare2::get_p_ref(double& p_ref) const
  { assert(pilot);
    return pilot->get_p_ref( p_ref);
  }

  inline EOS_Internal_Error EOS_Cathare2::get_h_ref(double& h_ref) const
  { assert(pilot);
    return pilot->get_h_ref( h_ref);
  }

  inline EOS_Internal_Error EOS_Cathare2::get_T_ref(double& T_ref) const
  { assert(pilot);
    return pilot->get_T_ref( T_ref);
  }

  inline EOS_Internal_Error EOS_Cathare2::compute_h_l_lim_p( double p, double& h_l_lim ) const
  { assert(pilot);
    return pilot->compute_h_l_lim_p( p, h_l_lim );
  }

  inline EOS_Internal_Error EOS_Cathare2::compute_h_v_lim_p( double p, double& h_v_lim ) const
  { assert(pilot);
    return pilot->compute_h_v_lim_p( p, h_v_lim );
  }

  inline void EOS_Cathare2::describe_error(const EOS_Internal_Error ierr, AString & description) const
  {
    ::CATHARE2::CATHARE2::describe_error(ierr, description);
    if ( description == AString("Unknown error") )  EOS_Fluid::describe_error(ierr,description) ;
  }

}

#endif
