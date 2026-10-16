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




#include "EOS_CoolProp.hxx"

#include "Language/API/RegisterType.hxx"
#include <AbstractState.h>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <limits>
#include <vector>

namespace NEPTUNE_EOS
{
  // ---------------------------------------------------------------------------
  // Registration
  // ---------------------------------------------------------------------------

  static RegisteredClass& EOS_CoolProp_create()
  { return *(new EOS_CoolProp()) ;
  }

  static const int CoolProp_type_Id =
      RegisterType("EOS_CoolProp", "EOS_Fluid",
                   "Equation Of States for CoolProp",
                   sizeof(EOS_CoolProp),
                   EOS_CoolProp_create) ;

  namespace
  {
    // -------------------------------------------------------------------------
    // Internal errors of EOS_CoolProp, numbered from 2000 like the other models
    // -------------------------------------------------------------------------

    const int EOS_CoolProp_Error_base = 2000 ;
    //! CoolProp raised an exception : describe_error() gives its message
    const EOS_Internal_Error COMPUTE_ERROR_(EOS_CoolProp_Error_base + 0, EOS_Error::error) ;
    //! The input fields are not a pair CoolProp can flash from
    const EOS_Internal_Error WRONG_INPUT_TYPE_(EOS_CoolProp_Error_base + 2, EOS_Error::bad) ;

    // -------------------------------------------------------------------------
    // Correspondence between the EOS properties and CoolProp
    // -------------------------------------------------------------------------

    //! Step of the centred differences : relative, with an absolute floor so
    //! that it never vanishes near a zero of the variable (reference state).
    double difference_step(double x)
    { return std::max(1.0e-3, 1.0e-6 * std::fabs(x)) ;
    }

    //! CoolProp key of an EOS property, INVALID_PARAMETER when CoolProp has none
    CoolProp::parameters coolprop_key(int property)
    { switch (property)
         { case NEPTUNE::p :
           case NEPTUNE::p_sat :
           case NEPTUNE::p_lim :     return CoolProp::iP ;
           case NEPTUNE::h :
           case NEPTUNE::h_l_sat :
           case NEPTUNE::h_v_sat :   return CoolProp::iHmass ;
           case NEPTUNE::T :
           case NEPTUNE::T_sat :     return CoolProp::iT ;
           case NEPTUNE::rho :
           case NEPTUNE::rho_l_sat :
           case NEPTUNE::rho_v_sat : return CoolProp::iDmass ;
           case NEPTUNE::u :         return CoolProp::iUmass ;
           case NEPTUNE::s :         return CoolProp::iSmass ;
           case NEPTUNE::mu :        return CoolProp::iviscosity ;
           case NEPTUNE::lambda :    return CoolProp::iconductivity ;
           case NEPTUNE::cp :
           case NEPTUNE::cp_l_sat :
           case NEPTUNE::cp_v_sat :  return CoolProp::iCpmass ;
           case NEPTUNE::cv :        return CoolProp::iCvmass ;
           case NEPTUNE::sigma :     return CoolProp::isurface_tension ;
           case NEPTUNE::w :         return CoolProp::ispeed_sound ;
           case NEPTUNE::g :         return CoolProp::iGmass ;
           case NEPTUNE::f :         return CoolProp::iHelmholtzmass ;
           case NEPTUNE::pr :        return CoolProp::iPrandtl ;
           case NEPTUNE::beta :      return CoolProp::iisobaric_expansion_coefficient ;
           default :                 return CoolProp::INVALID_PARAMETER ;
         }
    }

    //! Whether CoolProp differentiates this property analytically
    //! (AbstractState::first_partial_deriv handles T, p, rho, h, s, u and g)
    bool has_analytic_derivatives(int property)
    { switch (property)
         { case NEPTUNE::p :
           case NEPTUNE::T :
           case NEPTUNE::rho :
           case NEPTUNE::h :
           case NEPTUNE::s :
           case NEPTUNE::u :
           case NEPTUNE::g :  return true ;
           default :          return false ;
         }
    }

    //! Value of a property on the current state
    double evaluate(CoolProp::AbstractState& state, int property)
    { if (property == NEPTUNE::gamma)  return state.cpmass() / state.cvmass() ;
      return state.keyed_output(coolprop_key(property)) ;
    }

    //! Specific enthalpy at (p, T) on the current state, fallback when CoolProp fails
    double enthalpy_or(CoolProp::AbstractState& state, double p, double T, double fallback)
    { try
         { state.update(CoolProp::PT_INPUTS, p, T) ;
           return state.hmass() ;
         }
      catch (const std::exception&)
         { return fallback ;
         }
    }

    //! CoolProp phase imposed to the single-phase flashes of the object
    CoolProp::phases imposed_phase(bool liquid)
    { return liquid ? CoolProp::iphase_liquid : CoolProp::iphase_gas ;
    }

    //! Vapour quality of the saturated state on the branch of the imposed phase
    double saturation_quality(bool liquid)
    { return liquid ? 0.0 : 1.0 ;
    }

    //! Set the state to (value1, value2) in the imposed phase
    /*!
     * CoolProp drops the imposed phase inside some of its flash routines,
     * so the phase is imposed again before every single-phase update.
     */
    void update_state(CoolProp::AbstractState& state, bool liquid,
                      CoolProp::input_pairs pair, double value1, double value2)
    { state.specify_phase(imposed_phase(liquid)) ;
      state.update(pair, value1, value2) ;
    }

    //! Set the state to (value1, value2) on the saturation curve, the phase left free
    void update_saturation_state(CoolProp::AbstractState& state,
                                 CoolProp::input_pairs pair, double value1, double value2)
    { state.unspecify_phase() ;
      state.update(pair, value1, value2) ;
    }

    //! A first derivative d_X_d_Y_Z : X with respect to Y at constant Z
    struct DerivativeRule
    { int property ;
      int quantity ;
      int wrt ;
      int constant ;
    } ;

    const DerivativeRule derivative_rules[] =
    { { NEPTUNE::d_T_d_p_h,      NEPTUNE::T,      NEPTUNE::p, NEPTUNE::h },
      { NEPTUNE::d_T_d_h_p,      NEPTUNE::T,      NEPTUNE::h, NEPTUNE::p },
      { NEPTUNE::d_rho_d_p_h,    NEPTUNE::rho,    NEPTUNE::p, NEPTUNE::h },
      { NEPTUNE::d_rho_d_h_p,    NEPTUNE::rho,    NEPTUNE::h, NEPTUNE::p },
      { NEPTUNE::d_u_d_p_h,      NEPTUNE::u,      NEPTUNE::p, NEPTUNE::h },
      { NEPTUNE::d_u_d_h_p,      NEPTUNE::u,      NEPTUNE::h, NEPTUNE::p },
      { NEPTUNE::d_s_d_p_h,      NEPTUNE::s,      NEPTUNE::p, NEPTUNE::h },
      { NEPTUNE::d_s_d_h_p,      NEPTUNE::s,      NEPTUNE::h, NEPTUNE::p },
      { NEPTUNE::d_mu_d_p_h,     NEPTUNE::mu,     NEPTUNE::p, NEPTUNE::h },
      { NEPTUNE::d_mu_d_h_p,     NEPTUNE::mu,     NEPTUNE::h, NEPTUNE::p },
      { NEPTUNE::d_lambda_d_p_h, NEPTUNE::lambda, NEPTUNE::p, NEPTUNE::h },
      { NEPTUNE::d_lambda_d_h_p, NEPTUNE::lambda, NEPTUNE::h, NEPTUNE::p },
      { NEPTUNE::d_cp_d_p_h,     NEPTUNE::cp,     NEPTUNE::p, NEPTUNE::h },
      { NEPTUNE::d_cp_d_h_p,     NEPTUNE::cp,     NEPTUNE::h, NEPTUNE::p },
      { NEPTUNE::d_cv_d_p_h,     NEPTUNE::cv,     NEPTUNE::p, NEPTUNE::h },
      { NEPTUNE::d_cv_d_h_p,     NEPTUNE::cv,     NEPTUNE::h, NEPTUNE::p },
      { NEPTUNE::d_sigma_d_p_h,  NEPTUNE::sigma,  NEPTUNE::p, NEPTUNE::h },
      { NEPTUNE::d_sigma_d_h_p,  NEPTUNE::sigma,  NEPTUNE::h, NEPTUNE::p },
      { NEPTUNE::d_w_d_p_h,      NEPTUNE::w,      NEPTUNE::p, NEPTUNE::h },
      { NEPTUNE::d_w_d_h_p,      NEPTUNE::w,      NEPTUNE::h, NEPTUNE::p },
      { NEPTUNE::d_g_d_p_h,      NEPTUNE::g,      NEPTUNE::p, NEPTUNE::h },
      { NEPTUNE::d_g_d_h_p,      NEPTUNE::g,      NEPTUNE::h, NEPTUNE::p },
      { NEPTUNE::d_f_d_p_h,      NEPTUNE::f,      NEPTUNE::p, NEPTUNE::h },
      { NEPTUNE::d_f_d_h_p,      NEPTUNE::f,      NEPTUNE::h, NEPTUNE::p },
      { NEPTUNE::d_pr_d_p_h,     NEPTUNE::pr,     NEPTUNE::p, NEPTUNE::h },
      { NEPTUNE::d_pr_d_h_p,     NEPTUNE::pr,     NEPTUNE::h, NEPTUNE::p },
      { NEPTUNE::d_beta_d_p_h,   NEPTUNE::beta,   NEPTUNE::p, NEPTUNE::h },
      { NEPTUNE::d_beta_d_h_p,   NEPTUNE::beta,   NEPTUNE::h, NEPTUNE::p },
      { NEPTUNE::d_gamma_d_p_h,  NEPTUNE::gamma,  NEPTUNE::p, NEPTUNE::h },
      { NEPTUNE::d_gamma_d_h_p,  NEPTUNE::gamma,  NEPTUNE::h, NEPTUNE::p },
      { NEPTUNE::d_rho_d_p_T,    NEPTUNE::rho,    NEPTUNE::p, NEPTUNE::T },
      { NEPTUNE::d_rho_d_T_p,    NEPTUNE::rho,    NEPTUNE::T, NEPTUNE::p },
      { NEPTUNE::d_u_d_T_p,      NEPTUNE::u,      NEPTUNE::T, NEPTUNE::p },
      { NEPTUNE::d_s_d_T_p,      NEPTUNE::s,      NEPTUNE::T, NEPTUNE::p },
      { NEPTUNE::d_s_d_p_T,      NEPTUNE::s,      NEPTUNE::p, NEPTUNE::T },
      { NEPTUNE::d_mu_d_T_p,     NEPTUNE::mu,     NEPTUNE::T, NEPTUNE::p },
      { NEPTUNE::d_mu_d_p_T,     NEPTUNE::mu,     NEPTUNE::p, NEPTUNE::T },
      { NEPTUNE::d_lambda_d_p_T, NEPTUNE::lambda, NEPTUNE::p, NEPTUNE::T },
      { NEPTUNE::d_lambda_d_T_p, NEPTUNE::lambda, NEPTUNE::T, NEPTUNE::p },
      { NEPTUNE::d_cp_d_T_p,     NEPTUNE::cp,     NEPTUNE::T, NEPTUNE::p },
      { NEPTUNE::d_cp_d_p_T,     NEPTUNE::cp,     NEPTUNE::p, NEPTUNE::T },
      { NEPTUNE::d_cv_d_T_p,     NEPTUNE::cv,     NEPTUNE::T, NEPTUNE::p },
      { NEPTUNE::d_cv_d_p_T,     NEPTUNE::cv,     NEPTUNE::p, NEPTUNE::T },
      { NEPTUNE::d_sigma_d_T_p,  NEPTUNE::sigma,  NEPTUNE::T, NEPTUNE::p },
      { NEPTUNE::d_w_d_T_p,      NEPTUNE::w,      NEPTUNE::T, NEPTUNE::p },
      { NEPTUNE::d_w_d_p_T,      NEPTUNE::w,      NEPTUNE::p, NEPTUNE::T },
      { NEPTUNE::d_g_d_T_p,      NEPTUNE::g,      NEPTUNE::T, NEPTUNE::p },
      { NEPTUNE::d_f_d_T_p,      NEPTUNE::f,      NEPTUNE::T, NEPTUNE::p },
      { NEPTUNE::d_pr_d_T_p,     NEPTUNE::pr,     NEPTUNE::T, NEPTUNE::p },
      { NEPTUNE::d_beta_d_T_p,   NEPTUNE::beta,   NEPTUNE::T, NEPTUNE::p },
      { NEPTUNE::d_gamma_d_T_p,  NEPTUNE::gamma,  NEPTUNE::T, NEPTUNE::p },
      { NEPTUNE::d_h_d_T_p,      NEPTUNE::h,      NEPTUNE::T, NEPTUNE::p },
      { NEPTUNE::d_h_d_p_T,      NEPTUNE::h,      NEPTUNE::p, NEPTUNE::T },
      { NEPTUNE::d_h_d_p_s,      NEPTUNE::h,      NEPTUNE::p, NEPTUNE::s },
      { NEPTUNE::d_h_d_s_p,      NEPTUNE::h,      NEPTUNE::s, NEPTUNE::p } } ;

    //! A cross derivative d2_X_d_p_d_h
    struct CrossRule
    { int property ;
      int quantity ;
    } ;

    const CrossRule cross_rules[] =
    { { NEPTUNE::d2_T_d_p_d_h,      NEPTUNE::T      },
      { NEPTUNE::d2_rho_d_p_d_h,    NEPTUNE::rho    },
      { NEPTUNE::d2_u_d_p_d_h,      NEPTUNE::u      },
      { NEPTUNE::d2_s_d_p_d_h,      NEPTUNE::s      },
      { NEPTUNE::d2_mu_d_p_d_h,     NEPTUNE::mu     },
      { NEPTUNE::d2_lambda_d_p_d_h, NEPTUNE::lambda },
      { NEPTUNE::d2_cp_d_p_d_h,     NEPTUNE::cp     },
      { NEPTUNE::d2_cv_d_p_d_h,     NEPTUNE::cv     },
      { NEPTUNE::d2_sigma_d_p_d_h,  NEPTUNE::sigma  },
      { NEPTUNE::d2_w_d_p_d_h,      NEPTUNE::w      },
      { NEPTUNE::d2_g_d_p_d_h,      NEPTUNE::g      },
      { NEPTUNE::d2_f_d_p_d_h,      NEPTUNE::f      },
      { NEPTUNE::d2_pr_d_p_d_h,     NEPTUNE::pr     },
      { NEPTUNE::d2_beta_d_p_d_h,   NEPTUNE::beta   },
      { NEPTUNE::d2_gamma_d_p_d_h,  NEPTUNE::gamma  } } ;

    //! Saturated branch a saturation property refers to
    enum Branch
    { liquid_branch  = 0,  //!< vapour quality 0
      vapor_branch   = 1,  //!< vapour quality 1
      imposed_branch = 2   //!< the branch of the phase imposed to the object
    } ;

    //! A saturation property : quantity read on a saturated state, and its
    //! derivation order with respect to pressure along the saturation curve
    struct SaturationRule
    { int property ;
      int quantity ;
      int branch ;
      int order ;
    } ;

    const SaturationRule saturation_rules[] =
    { { NEPTUNE::p_sat,            NEPTUNE::p,   imposed_branch, 0 },
      { NEPTUNE::T_sat,            NEPTUNE::T,   imposed_branch, 0 },
      { NEPTUNE::rho_l_sat,        NEPTUNE::rho, liquid_branch,  0 },
      { NEPTUNE::rho_v_sat,        NEPTUNE::rho, vapor_branch,   0 },
      { NEPTUNE::h_l_sat,          NEPTUNE::h,   liquid_branch,  0 },
      { NEPTUNE::h_v_sat,          NEPTUNE::h,   vapor_branch,   0 },
      { NEPTUNE::cp_l_sat,         NEPTUNE::cp,  liquid_branch,  0 },
      { NEPTUNE::cp_v_sat,         NEPTUNE::cp,  vapor_branch,   0 },
      { NEPTUNE::d_T_sat_d_p,      NEPTUNE::T,   imposed_branch, 1 },
      { NEPTUNE::d_rho_l_sat_d_p,  NEPTUNE::rho, liquid_branch,  1 },
      { NEPTUNE::d_rho_v_sat_d_p,  NEPTUNE::rho, vapor_branch,   1 },
      { NEPTUNE::d_h_l_sat_d_p,    NEPTUNE::h,   liquid_branch,  1 },
      { NEPTUNE::d_h_v_sat_d_p,    NEPTUNE::h,   vapor_branch,   1 },
      { NEPTUNE::d_cp_l_sat_d_p,   NEPTUNE::cp,  liquid_branch,  1 },
      { NEPTUNE::d_cp_v_sat_d_p,   NEPTUNE::cp,  vapor_branch,   1 },
      { NEPTUNE::d2_T_sat_d_p_d_p, NEPTUNE::T,   imposed_branch, 2 } } ;

    // -------------------------------------------------------------------------
    // Evaluation plan of a two-input compute
    // -------------------------------------------------------------------------

    //! How one output of a two-input compute is obtained from the state
    struct Output
    { enum Kind
         { value,                  //!< property of the state
           analytic,               //!< first_partial_deriv on the state
           cross,                  //!< second_partial_deriv d2/dp.dh on the state
           difference,             //!< centred difference on one perturbed input
           saturation,             //!< property of the saturated state at the same pressure
           saturation_difference,  //!< centred difference of that property in pressure
           zero                    //!< identically zero
         } ;
      int                  index ;      //!< position in the output fields
      Kind                 kind ;
      int                  quantity ;   //!< EOS property to evaluate
      CoolProp::parameters wrt ;        //!< derivation variable (analytic)
      CoolProp::parameters constant ;   //!< variable held constant (analytic)
      int                  perturbed ;  //!< input perturbed by the difference : 0 or 1
    } ;

    //! Classify an output of a two-input compute
    /*!
     * The surface tension is a property of the saturation curve : it is read
     * on the saturated state at the input pressure, depends on nothing else,
     * and so has a zero derivative with respect to h or T.
     * \param[in] property EOS property of the output
     * \param[in] input    EOS properties of the two inputs
     * \param[in] p_input  index of the pressure input, -1 if none
     * \param[out] out     plan entry, filled when the function returns true
     * \return false when the output must be left to the EOS_Fluid implementation
     */
    bool classify_output(int property, const int input[2], int p_input, Output& out)
    { if ( (property < NEPTUNE::p) || (property > NEPTUNE::lastTProperty) )  return false ;
      out.quantity  = property ;
      out.perturbed = -1 ;
      out.wrt       = CoolProp::INVALID_PARAMETER ;
      out.constant  = CoolProp::INVALID_PARAMETER ;

      if (property == NEPTUNE::sigma)
         { if (p_input < 0)  return false ;
           out.kind = Output::saturation ;
           return true ;
         }
      if ( (property == NEPTUNE::gamma) || (coolprop_key(property) != CoolProp::INVALID_PARAMETER) )
         { out.kind = Output::value ;
           return true ;
         }

      for (const DerivativeRule& rule : derivative_rules)
         { if (rule.property != property)  continue ;
           out.quantity = rule.quantity ;
           if (rule.quantity == NEPTUNE::sigma)
              { if (rule.wrt != NEPTUNE::p)
                   { out.kind = Output::zero ;
                     return true ;
                   }
                if (p_input < 0)  return false ;
                out.kind = Output::saturation_difference ;
                return true ;
              }
           if (has_analytic_derivatives(rule.quantity))
              { out.kind     = Output::analytic ;
                out.wrt      = coolprop_key(rule.wrt) ;
                out.constant = coolprop_key(rule.constant) ;
                return true ;
              }
           // Centred difference : the derivation variable must be one input
           // and the variable held constant the other one
           for (int k = 0; k < 2; k++)
              { if ( (input[k] == rule.wrt) && (input[1-k] == rule.constant) )
                   { out.kind      = Output::difference ;
                     out.perturbed = k ;
                     return true ;
                   }
              }
           return false ;
         }

      for (const CrossRule& rule : cross_rules)
         { if (rule.property != property)  continue ;
           out.quantity = rule.quantity ;
           if (rule.quantity == NEPTUNE::sigma)
              { out.kind = Output::zero ;
                return true ;
              }
           if (!has_analytic_derivatives(rule.quantity))  return false ;
           out.kind = Output::cross ;
           return true ;
         }
      return false ;
    }

    // -------------------------------------------------------------------------
    // Evaluation plan of a saturation compute
    // -------------------------------------------------------------------------

    //! How one output of a saturation compute is obtained from a saturated state
    struct SaturationOutput
    { enum Kind
         { value,       //!< property of the saturated state
           analytic,    //!< first_saturation_deriv on the saturated state
           second,      //!< second_saturation_deriv on the saturated state
           difference   //!< centred difference in pressure along the saturation curve
         } ;
      int  index ;      //!< position in the output fields
      Kind kind ;
      int  quantity ;   //!< EOS property to evaluate
      int  branch ;     //!< liquid_branch or vapor_branch
    } ;

    //! Classify an output of a saturation compute
    /*!
     * \param[in] property  EOS property of the output
     * \param[in] imposed   branch of the phase imposed to the object
     * \param[out] out      plan entry, filled when the function returns true
     * \return false when the output must be left to the EOS_Fluid implementation
     */
    bool classify_saturation_output(int property, int imposed, SaturationOutput& out)
    { for (const SaturationRule& rule : saturation_rules)
         { if (rule.property != property)  continue ;
           out.quantity = rule.quantity ;
           out.branch   = (rule.branch == imposed_branch) ? imposed : rule.branch ;
           switch (rule.order)
              { case 0 :  out.kind = SaturationOutput::value ;  break ;
                case 1 :  out.kind = has_analytic_derivatives(rule.quantity) ? SaturationOutput::analytic
                                                                             : SaturationOutput::difference ;  break ;
                default : out.kind = SaturationOutput::second ; break ;
              }
           return true ;
         }
      return false ;
    }
  }

  // ---------------------------------------------------------------------------
  // Construction and initialisation
  // ---------------------------------------------------------------------------

  EOS_CoolProp::EOS_CoolProp() :
    liquid_(true),
    fluid_name_("Unknown"),
    phase_name_("Unknown"),
    p_min_(0.0), p_max_(0.0), T_min_(0.0), T_max_(0.0),
    h_min_(0.0), h_max_(0.0), p_crit_(0.0), T_crit_(0.0),
    h_crit_(std::numeric_limits<double>::quiet_NaN()),
    mm_(0.0)
  {
  }

  EOS_CoolProp::~EOS_CoolProp() = default ;

  int EOS_CoolProp::init(const Strings& strings)
  { if (strings.size() != 2)
       { cerr << " EOS_CoolProp::init(strings) : Reference with size " << strings.size()
              << " is not implemented for EOS_CoolProp ! " << endl ;
         return EOS_Error::error ;
       }
    fluid_name_ = strings[0] ;
    phase_name_ = strings[1] ;

    if (phase_name_ == "phase_liquid")
       liquid_ = true ;
    else if (phase_name_ == "phase_gas")
       liquid_ = false ;
    else
       { cerr << " EOS_CoolProp::init(strings) : unknown phase " << phase_name_.aschar()
              << " (expected phase_liquid or phase_gas)" << endl ;
         return EOS_Error::error ;
       }

    try
       { state_.reset(CoolProp::AbstractState::factory("HEOS", fluid_name_.aschar())) ;
         p_min_  = state_->p_triple() ;
         p_max_  = state_->pmax() ;
         T_min_  = state_->Tmin() ;
         T_max_  = state_->Tmax() ;
         p_crit_ = state_->p_critical() ;
         T_crit_ = state_->T_critical() ;
         mm_     = state_->molar_mass() ;
       }
    catch (const std::exception& e)
       { cerr << " EOS_CoolProp::init(strings) : " << e.what() << endl ;
         state_.reset() ;
         return EOS_Error::error ;
       }

    // Enthalpy bounds. The flashes at (p_max, T_max) and at the critical point
    // only converge with the phase left free. The one at (p_min, T_min) lies on
    // the saturation curve and only converges with the phase imposed, which
    // also selects the bound relevant to the phase of this object.
    h_max_  = enthalpy_or(*state_, p_max_, T_max_, 1.0e20) ;
    h_crit_ = enthalpy_or(*state_, p_crit_, T_crit_, std::numeric_limits<double>::quiet_NaN()) ;
    state_->specify_phase(imposed_phase(liquid_)) ;
    h_min_  = enthalpy_or(*state_, p_min_, T_min_, 1.0) ;

    return EOS_Error::good ;
  }

  int EOS_CoolProp::init(const Strings&, const Strings&)
  { return EOS_Error::error ;
  }

  int EOS_CoolProp::init(const Strings&, const Strings&,
                         const double, const double, const double, const double)
  { return EOS_Error::error ;
  }

  // ---------------------------------------------------------------------------
  // Description
  // ---------------------------------------------------------------------------

  const AString& EOS_CoolProp::table_name() const
  { static const AString TMStr("Coolprop") ;
    return TMStr ;
  }

  const AString& EOS_CoolProp::version_name() const
  { static const AString MVStr("6.4") ;
    return MVStr ;
  }

  const AString& EOS_CoolProp::phase_name() const
  { return phase_name_ ;
  }

  const AString& EOS_CoolProp::fluid_name() const
  { return fluid_name_ ;
  }

  const AString& EOS_CoolProp::equation_name() const
  { return fluid_name_ ;
  }

  ostream& EOS_CoolProp::print_On(ostream& stream) const
  { return stream ;
  }

  istream& EOS_CoolProp::read_On(istream& stream)
  { return stream ;
  }

  const Type_Info& EOS_CoolProp::get_Type_Info() const
  { return (Types_Info::instance())[CoolProp_type_Id] ;
  }

  void EOS_CoolProp::describe_error(const EOS_Internal_Error error, AString& description) const
  { if (error == COMPUTE_ERROR_)
       description = AString(last_message_.empty() ? "CoolProp : computation failed" : last_message_.c_str()) ;
    else if (error == WRONG_INPUT_TYPE_)
       description = AString("EOS_CoolProp : unsupported input properties") ;
    else
       EOS_Fluid::describe_error(error, description) ;
  }

  // ---------------------------------------------------------------------------
  // General fluid properties
  // ---------------------------------------------------------------------------

  EOS_Internal_Error EOS_CoolProp::get_p_min(double& p_min) const
  { p_min = p_min_ ;
    return EOS_Internal_Error::OK ;
  }

  EOS_Internal_Error EOS_CoolProp::get_p_max(double& p_max) const
  { p_max = p_max_ ;
    return EOS_Internal_Error::OK ;
  }

  EOS_Internal_Error EOS_CoolProp::get_T_min(double& T_min) const
  { T_min = T_min_ ;
    return EOS_Internal_Error::OK ;
  }

  EOS_Internal_Error EOS_CoolProp::get_T_max(double& T_max) const
  { T_max = T_max_ ;
    return EOS_Internal_Error::OK ;
  }

  EOS_Internal_Error EOS_CoolProp::get_h_min(double& h_min) const
  { h_min = h_min_ ;
    return EOS_Internal_Error::OK ;
  }

  EOS_Internal_Error EOS_CoolProp::get_h_max(double& h_max) const
  { h_max = h_max_ ;
    return EOS_Internal_Error::OK ;
  }

  EOS_Internal_Error EOS_CoolProp::get_p_crit(double& p_crit) const
  { p_crit = p_crit_ ;
    return EOS_Internal_Error::OK ;
  }

  EOS_Internal_Error EOS_CoolProp::get_T_crit(double& T_crit) const
  { T_crit = T_crit_ ;
    return EOS_Internal_Error::OK ;
  }

  EOS_Internal_Error EOS_CoolProp::get_h_crit(double& h_crit) const
  { if (std::isnan(h_crit_))  return COMPUTE_ERROR_ ;
    h_crit = h_crit_ ;
    return EOS_Internal_Error::OK ;
  }

  EOS_Internal_Error EOS_CoolProp::get_mm(double& mm) const
  { mm = mm_ ;
    return EOS_Internal_Error::OK ;
  }

  // ---------------------------------------------------------------------------
  // Error reporting
  // ---------------------------------------------------------------------------

  void EOS_CoolProp::record_failure(EOS_Error_Field& err, int idata, const std::exception& e) const
  { err.set(idata, COMPUTE_ERROR_) ;
    last_message_ = std::string("CoolProp : ") + e.what() ;
  }

  // ---------------------------------------------------------------------------
  // Field methods
  // ---------------------------------------------------------------------------

  EOS_Error EOS_CoolProp::compute(const EOS_Field& input, EOS_Fields& outputs, EOS_Error_Field& err) const
  { const int size = input.size() ;
    assert(err.size() == size) ;
    err = EOS_Internal_Error::OK ;

    const CoolProp::parameters input_key = coolprop_key(input.get_property_number()) ;
    if ( !state_ || ((input_key != CoolProp::iP) && (input_key != CoolProp::iT)) )
       { err = WRONG_INPUT_TYPE_ ;
         return err.find_worst_error().generic_error() ;
       }
    // Argument order CoolProp expects for (input, vapour quality)
    int first = 0, second = 1 ;
    const CoolProp::input_pairs pair = CoolProp::generate_update_pair(input_key, 0, CoolProp::iQ, 1, first, second) ;
    const int imposed = liquid_ ? liquid_branch : vapor_branch ;

    // Sort the outputs once for the whole field : which saturated states are
    // needed, and what is read on each of them
    std::vector<SaturationOutput> plan ;
    std::vector<int>              delegated ;
    bool needed[2]     = { false, false } ;
    bool difference[2] = { false, false } ;
    for (int i = 0; i < outputs.size(); i++)
       { SaturationOutput out ;
         out.index = i ;
         if (!classify_saturation_output(outputs[i].get_property_number(), imposed, out))
            { delegated.push_back(i) ;
              continue ;
            }
         plan.push_back(out) ;
         needed[out.branch] = true ;
         if (out.kind == SaturationOutput::difference)  difference[out.branch] = true ;
       }

    for (int idata = 0; idata < size; idata++)
       { double x = input[idata] ;
         // CoolProp has no saturated state outside [p_triple, p_crit]
         if (input_key == CoolProp::iP)  x = std::min(std::max(x, p_min_), p_crit_) ;
         try
            { for (int branch = 0; branch < 2; branch++)
                 { if (!needed[branch])  continue ;
                   const double values[2] = { x, double(branch) } ;
                   update_saturation_state(*state_, pair, values[first], values[second]) ;
                   for (const SaturationOutput& out : plan)
                      { if (out.branch != branch)  continue ;
                        double& result = outputs[out.index][idata] ;
                        switch (out.kind)
                           { case SaturationOutput::value :
                                result = evaluate(*state_, out.quantity) ;
                                break ;
                             case SaturationOutput::analytic :
                                result = state_->first_saturation_deriv(coolprop_key(out.quantity), CoolProp::iP) ;
                                break ;
                             case SaturationOutput::second :
                                result = state_->second_saturation_deriv(coolprop_key(out.quantity), CoolProp::iP, CoolProp::iP) ;
                                break ;
                             default :
                                break ;
                           }
                      }
                   if (!difference[branch])  continue ;
                   // Centred difference along the saturation curve : two more flashes
                   const double p   = state_->p() ;
                   const double eps = difference_step(p) ;
                   update_saturation_state(*state_, CoolProp::PQ_INPUTS, p + eps, double(branch)) ;
                   for (const SaturationOutput& out : plan)
                      { if ( (out.branch == branch) && (out.kind == SaturationOutput::difference) )
                           outputs[out.index][idata] = evaluate(*state_, out.quantity) ;
                      }
                   update_saturation_state(*state_, CoolProp::PQ_INPUTS, p - eps, double(branch)) ;
                   for (const SaturationOutput& out : plan)
                      { if ( (out.branch == branch) && (out.kind == SaturationOutput::difference) )
                           outputs[out.index][idata] = (outputs[out.index][idata] - evaluate(*state_, out.quantity)) / (2.0 * eps) ;
                      }
                 }
            }
         catch (const std::exception& e)
            { record_failure(err, idata, e) ;
            }
       }

    // Outputs CoolProp cannot evaluate : generic EOS_Fluid implementation, field by field
    if (!delegated.empty())
       { ArrOfInt err_data(size) ;
         EOS_Error_Field err_field(err_data) ;
         for (int i : delegated)
            { EOS_Fluid::compute(input, outputs[i], err_field) ;
              err.set_worst_error(err_field) ;
            }
       }
    return err.find_worst_error().generic_error() ;
  }

  EOS_Error EOS_CoolProp::compute(const EOS_Field& input, EOS_Field& output, EOS_Error_Field& err) const
  { EOS_Fields outputs(1) ;
    outputs[0] = output ;
    return compute(input, outputs, err) ;
  }

  EOS_Error EOS_CoolProp::compute(const EOS_Field& input1, const EOS_Field& input2,
                                  EOS_Fields& outputs, EOS_Error_Field& err) const
  { const int size = input1.size() ;
    assert(input2.size() == size) ;
    assert(err.size() == size) ;
    err = EOS_Internal_Error::OK ;

    // Input pair, in the argument order CoolProp expects
    const int input_property[2] = { input1.get_property_number(), input2.get_property_number() } ;
    int first = 0, second = 1 ;
    const CoolProp::input_pairs pair = CoolProp::generate_update_pair(coolprop_key(input_property[0]), 0,
                                                                      coolprop_key(input_property[1]), 1,
                                                                      first, second) ;
    if ( !state_ || (pair == CoolProp::INPUT_PAIR_INVALID) )
       { err = WRONG_INPUT_TYPE_ ;
         return err.find_worst_error().generic_error() ;
       }
    int p_input = -1 ;
    if      (input_property[0] == NEPTUNE::p)  p_input = 0 ;
    else if (input_property[1] == NEPTUNE::p)  p_input = 1 ;

    // Sort the outputs once for the whole field : what is read on the state
    // of the point, which inputs have to be perturbed, whether the saturated
    // state at the same pressure is needed
    std::vector<Output> plan ;
    std::vector<int>    delegated ;
    bool difference[2]         = { false, false } ;
    bool saturation            = false ;
    bool saturation_difference = false ;
    for (int i = 0; i < outputs.size(); i++)
       { Output out ;
         out.index = i ;
         if (!classify_output(outputs[i].get_property_number(), input_property, p_input, out))
            { delegated.push_back(i) ;
              continue ;
            }
         plan.push_back(out) ;
         switch (out.kind)
            { case Output::difference :             difference[out.perturbed] = true ;  break ;
              case Output::saturation :             saturation = true ;                 break ;
              case Output::saturation_difference :  saturation = saturation_difference = true ;  break ;
              default :                             break ;
            }
       }

    for (int idata = 0; idata < size; idata++)
       { const double values[2] = { input1[idata], input2[idata] } ;
         try
            { update_state(*state_, liquid_, pair, values[first], values[second]) ;
              for (const Output& out : plan)
                 { double& result = outputs[out.index][idata] ;
                   switch (out.kind)
                      { case Output::value :
                           result = evaluate(*state_, out.quantity) ;
                           break ;
                        case Output::analytic :
                           result = state_->first_partial_deriv(coolprop_key(out.quantity), out.wrt, out.constant) ;
                           break ;
                        case Output::cross :
                           result = state_->second_partial_deriv(coolprop_key(out.quantity),
                                                                 CoolProp::iP, CoolProp::iHmass,
                                                                 CoolProp::iHmass, CoolProp::iP) ;
                           break ;
                        case Output::zero :
                           result = 0.0 ;
                           break ;
                        default :
                           break ;
                      }
                 }

              // Centred differences : two more flashes per perturbed input,
              // shared by all the derivatives taken with respect to it
              for (int k = 0; k < 2; k++)
                 { if (!difference[k])  continue ;
                   const double eps = difference_step(values[k]) ;
                   double shifted[2] = { values[0], values[1] } ;
                   shifted[k] = values[k] + eps ;
                   update_state(*state_, liquid_, pair, shifted[first], shifted[second]) ;
                   for (const Output& out : plan)
                      { if ( (out.kind == Output::difference) && (out.perturbed == k) )
                           outputs[out.index][idata] = evaluate(*state_, out.quantity) ;
                      }
                   shifted[k] = values[k] - eps ;
                   update_state(*state_, liquid_, pair, shifted[first], shifted[second]) ;
                   for (const Output& out : plan)
                      { if ( (out.kind == Output::difference) && (out.perturbed == k) )
                           outputs[out.index][idata] = (outputs[out.index][idata] - evaluate(*state_, out.quantity)) / (2.0 * eps) ;
                      }
                 }

              // Surface tension : read on the saturated state at the same pressure
              if (saturation)
                 { const double p = values[p_input] ;
                   const double quality = saturation_quality(liquid_) ;
                   update_saturation_state(*state_, CoolProp::PQ_INPUTS, p, quality) ;
                   const double sigma = state_->surface_tension() ;
                   double d_sigma_d_p = 0.0 ;
                   if (saturation_difference)
                      { const double eps = difference_step(p) ;
                        update_saturation_state(*state_, CoolProp::PQ_INPUTS, p + eps, quality) ;
                        d_sigma_d_p = state_->surface_tension() ;
                        update_saturation_state(*state_, CoolProp::PQ_INPUTS, p - eps, quality) ;
                        d_sigma_d_p = (d_sigma_d_p - state_->surface_tension()) / (2.0 * eps) ;
                      }
                   for (const Output& out : plan)
                      { if      (out.kind == Output::saturation)             outputs[out.index][idata] = sigma ;
                        else if (out.kind == Output::saturation_difference)  outputs[out.index][idata] = d_sigma_d_p ;
                      }
                 }
            }
         catch (const std::exception& e)
            { record_failure(err, idata, e) ;
            }
       }

    // Outputs CoolProp cannot evaluate : generic EOS_Fluid implementation, field by field
    if (!delegated.empty())
       { ArrOfInt err_data(size) ;
         EOS_Error_Field err_field(err_data) ;
         for (int i : delegated)
            { EOS_Fluid::compute(input1, input2, outputs[i], err_field) ;
              err.set_worst_error(err_field) ;
            }
       }
    return err.find_worst_error().generic_error() ;
  }

  EOS_Error EOS_CoolProp::compute(const EOS_Field& input1, const EOS_Field& input2,
                                  EOS_Field& output, EOS_Error_Field& err) const
  { EOS_Fields outputs(1) ;
    outputs[0] = output ;
    return compute(input1, input2, outputs, err) ;
  }
}
