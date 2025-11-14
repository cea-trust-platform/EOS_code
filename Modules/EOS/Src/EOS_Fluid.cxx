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
#include "EOS/API/EOS_Fluid.hxx"
#include "EOS/API/EOS_Field.hxx"
#include "EOS/API/EOS_Error_Field.hxx"
#include "EOS/API/EOS_Fields.hxx"

// for fluid_name, table_name, version_name
static NEPTUNE::AString UNKNOWN("Unknown") ;
// for numerical derivation
static double epsilon = 1.e-6 ;
// for Newton
static  int nb_iter_max = 50 ;
static  int iter_Newton;
static  double epsn = 1.e-8 ;

namespace NEPTUNE
{
  // Number of the first internal error for EOS_Fluid object:
  static const int EOS_Fluid_Error_base = 1 ;

  // Definition of internal errors in EOS_Fluid:
  // The following errors leads to EOS_Error=bad
  static const EOS_Internal_Error Error_p_sat_T_compute = EOS_Internal_Error(EOS_Fluid_Error_base + 0, EOS_Error::bad) ;
  static const EOS_Internal_Error Error_p_sat_T_newton  = EOS_Internal_Error(EOS_Fluid_Error_base + 1, EOS_Error::bad) ;
  static const EOS_Internal_Error Error_lambda_value    = EOS_Internal_Error(EOS_Fluid_Error_base + 2, EOS_Error::bad) ;


  // Implementation of describe_error in derived classes must call the parent class
  // to handle unknown errors.
  void EOS_Fluid::describe_error(const EOS_Internal_Error err_code, AString & description) const
  { if      (err_code == EOS_Internal_Error::OK)
       description = "ok" ;
    else if (err_code == EOS_Internal_Error::NOT_IMPLEMENTED)
       description = "EOS_Fluid: not implemented" ;
    else if (err_code == Error_p_sat_T_compute)
       description = "EOS_Fluid::compute_p_sat_T compute returned bad value during Newton" ;
    else if (err_code == Error_p_sat_T_newton)
       description = "EOS_Fluid::compute_p_sat_T Newton algorithm did not converge" ;
    else if (err_code == Error_lambda_value)
       description = "EOS_Fluid::lambda value is not positive" ;
    else
       description = "EOS_Fluid unknown error code" ;
  }

  int EOS_Fluid::type_Id=(RegisterType("EOS_Fluid", "UObject",
                          "Equation Of States",
                          sizeof(EOS_Fluid),
                          0)) ;


  const AString& EOS_Fluid::equation_name() const
  { return UNKNOWN ;
  }


  const AString& EOS_Fluid::phase_name() const
  { return UNKNOWN ;
  }


  const AString& EOS_Fluid::fluid_name() const
  { return UNKNOWN ;
  }


  const AString& EOS_Fluid::table_name() const
  { return UNKNOWN ;
  }


  const AString& EOS_Fluid::version_name() const
  { return UNKNOWN ;
  }


  EOS_Fluid::EOS_Fluid(){}


  // Compute output field function of input fields.
  // Default implementation (can be overloaded in derived classes):
  //  call to compute(P) or compute(P, h).

  EOS_Error EOS_Fluid::init_model(const std::string& /*model_name*/, const std::string& /*fluid_name*/, bool /*switch_comp_sat*/, bool /*swch_calc_deriv_fld*/)
  {
    std::cout<<"Attention ! Mauvaise initialisation de l'IPP ";
    return EOS_Error::ok;
  }

  EOS_Internal_Error EOS_Fluid::compute_Ipp_error(double& /*error_tot*/, double* &/*error_cells*/, AString /*prop*/)  
  {
    std::cout<<"Attention ! Mauvaise utilisation de la routine compute_Ipp_error avec un fluide et pas un interpolateur ";
    return EOS_Internal_Error::OK;
  }
  EOS_Internal_Error EOS_Fluid::compute_Ipp_sat_error(double& /*error_tot*/,  double* &/*error_cells*/, AString /*prop*/)  
  {
    std::cout<<"Attention ! Mauvaise utilisation de la routine compute_Ipp_sat_error avec un fluide et pas un interpolateur ";
    return EOS_Internal_Error::OK;
  }

  EOS_Error EOS_Fluid::compute( const EOS_Fields & input,
                                EOS_Field & output,
                                EOS_Error_Field & errfield) const
  { int nbi = input.size() ;
    errfield = EOS_Internal_Error::OK ;

    if (nbi == 2)
       { const EOS_Field *p = NULL ;
         const EOS_Field *h = NULL ;
         const EOS_Field *T = NULL ;
         const EOS_Field *s = NULL ;
         for (int i=0; i<nbi; i++) 
            { switch(input[i].get_property_number()) 
                 { case NEPTUNE::p :  p = &(input[i]) ; break ;
                   case NEPTUNE::h :  h = &(input[i]) ; break ;
                   case NEPTUNE::T :  T = &(input[i]) ; break ;
                   case NEPTUNE::s :  s = &(input[i]) ; break ;
                   default: break;
                 }
            }

         if (p && h)
            return compute(*p, *h, output, errfield) ;
         else if (p && s)
            return compute(*p, *s, output, errfield) ;
         else if (p && T)
            { const EOS_Field& T = *h ;
              const EOS_Field& P = *p ;
              const int sz = p->size() ;

              ArrOfDouble h_data(sz) ;
              EOS_Field h("h", "h",NEPTUNE::h, h_data) ;
              ArrOfInt err_data(sz) ;
              EOS_Error_Field err(err_data) ;
              for (int i=0; i<sz; i++)
                 err.set(i, compute_h_pT(P[i], T[i], h[i]));
              compute(P, h, output, errfield) ;
              errfield.set_worst_error(err)   ;
              return errfield.find_worst_error().generic_error() ;
            }
       }

    else if (nbi == 1)
       { const EOS_Field *p    = NULL ;
         const EOS_Field *psat = NULL ;
         const EOS_Field *plim = NULL ;
         for (int i=0; i<nbi; i++) 
            { switch(input[i].get_property_number())
                 { case NEPTUNE::p :      p    = &(input[i]) ; break ;
                   case NEPTUNE::p_sat :  psat = &(input[i]) ; break ;
                   case NEPTUNE::p_lim :  plim = &(input[i]) ; break ;
                   default : break;
                 }
            }
         if      (p)
            return compute(*p,    output, errfield) ;
         else if (psat)
            return compute(*psat, output, errfield) ;
         else if (plim)
            return compute(*plim, output, errfield) ;
       }

    errfield = EOS_Internal_Error::NOT_IMPLEMENTED ;
    return EOS_Error::error ;
  }

//
  EOS_Error EOS_Fluid::compute(const EOS_Fields& input,
                               EOS_Fields& output,
                               EOS_Error_Field& errfield) const
  { int nbi = input.size() ;
    errfield = EOS_Internal_Error::OK ;

    if (nbi == 2)
       { const EOS_Field *p = NULL ;
         const EOS_Field *h = NULL ;
         const EOS_Field *T = NULL ;
         const EOS_Field *s = NULL ;
         for (int i=0; i<nbi; i++) 
            { switch(input[i].get_property_number()) 
                 { case NEPTUNE::p :  p = &(input[i]) ; break ;
                   case NEPTUNE::h :  h = &(input[i]) ; break ;
                   case NEPTUNE::T :  T = &(input[i]) ; break ;
                   case NEPTUNE::s :  s = &(input[i]) ; break ;
                   default: break;
                 }
            }
         if      (p && h)
            return compute(*p, *h, output, errfield) ;
         else if (p && T)
            return compute(*p, *T, output, errfield) ;
         else if (p && s)
            return compute(*p, *s, output, errfield) ;
       }
    else if (nbi == 1)
       { const EOS_Field *p    = NULL ;
         const EOS_Field *psat = NULL ;
         const EOS_Field *plim = NULL ;
         for (int i=0; i<nbi; i++) 
            { switch(input[i].get_property_number())
                 { case NEPTUNE::p :  p = &(input[i]) ; break ;
                   case NEPTUNE::p_sat :  psat = &(input[i]) ; break ;
                   case NEPTUNE::p_lim :  plim = &(input[i]) ; break ;
                   default : break;
                 }
            }
         if      (p)
            return compute(*p,    output, errfield) ;
         else if (psat)
            return compute(*psat, output, errfield) ;
         else if (plim)
            return compute(*plim, output, errfield) ;
       }

    errfield = EOS_Internal_Error::NOT_IMPLEMENTED ;
    return EOS_Error::error ;
  }

//
  EOS_Error EOS_Fluid::compute(const EOS_Field& p,
                               const EOS_Field& h,
                               EOS_Fields& r,
                               EOS_Error_Field& errfield) const
  { const int nb_fields = r.size() ;
    const int sz        = errfield.size() ;
    errfield = EOS_Internal_Error::OK ;
    EOS_Error err = EOS_Error::good  ;
    ArrOfInt err_data(sz) ;
    EOS_Error_Field err_field2(err_data) ;
    for (int i=0; i<nb_fields; i++)
       {
         EOS_Error err2 = compute(p, h, r[i], err_field2) ;
         err = worst_generic_error(err, err2) ;
         errfield.set_worst_error(err_field2) ;
       }
    return err ;
  }

//
  EOS_Error EOS_Fluid::compute(const EOS_Field& p,
                               EOS_Fields& r,
                               EOS_Error_Field& errfield) const
  { const int nb_fields = r.size() ;
    const int sz        = errfield.size() ;
    errfield = EOS_Internal_Error::OK ;
    EOS_Error err = EOS_Error::good ;
    ArrOfInt err_data(sz) ;
    EOS_Error_Field err_field2(err_data) ;
    for (int i=0; i<nb_fields; i++)
       { EOS_Error err2 = compute(p, r[i], err_field2) ;
         err = worst_generic_error(err, err2) ;
         errfield.set_worst_error(err_field2) ;
       }
    return err ;
  }

//
  int EOS_Fluid::init(const Strings&) {return EOS_Error::good ;}

//
  int EOS_Fluid::init(const Strings&, const Strings&) {return EOS_Error::good ;}
  //
  int EOS_Fluid::init(const Strings&, const Strings&,
                      const double, const double, const double, const double)
  {return EOS_Error::good ;}



//
  double EOS_Fluid::is_liquid(double p, double h) const
  { cerr << " *** eos_fluid *** Error is_liquid,  P = " << p << "  h = " << h << endl ;
    assert(0)   ;
    exit(-1)    ;
    return (-1) ;
  }

//
  EOS_Error EOS_Fluid::compute(const EOS_Field& p,
                               EOS_Field& r,
                               EOS_Error_Field& errfield) const
  { const int sz = p.size() ;

    assert(r.size() == sz) ;
    assert(errfield.size() == sz) ;

    EOS_Property prop_p  = p.get_property_number();
    EOS_Property propsat = r.get_property_number();

    if ( (prop_p == NEPTUNE::T) && (propsat == NEPTUNE::p_sat) )
       { for (int i=0; i<sz; i++)
             errfield.set(i, compute_p_sat_T(p[i],r[i])) ;
         return errfield.find_worst_error().generic_error() ;
       }

    if (    (prop_p != NEPTUNE::p) 
         && (prop_p != NEPTUNE::p_sat)
         && (prop_p != NEPTUNE::p_lim) )
       { errfield = EOS_Internal_Error::NOT_IMPLEMENTED ;
         return EOS_Error::error ;   
       }

    switch(propsat)
       { // Saturation of Thermodynamic Properties
         case NEPTUNE::rho_l_sat : 
            for (int i=0; i<sz; i++)
               errfield.set(i, compute_rho_l_sat_p(p[i],r[i])) ;
            break ;
         case NEPTUNE::rho_v_sat : 
            for (int i=0; i<sz; i++)
               errfield.set(i, compute_rho_v_sat_p(p[i],r[i])) ;
            break ;
         case NEPTUNE::h_l_sat : 
            for (int i=0; i<sz; i++)
               errfield.set(i, compute_h_l_sat_p(p[i],r[i])) ;
            break ;
         case NEPTUNE::h_v_sat : 
            for (int i=0; i<sz; i++)
               errfield.set(i, compute_h_v_sat_p(p[i],r[i])) ;
            break ;
         case NEPTUNE::cp_l_sat : 
            for (int i=0; i<sz; i++)
               errfield.set(i, compute_cp_l_sat_p(p[i],r[i])) ;
            break ;
         case NEPTUNE::cp_v_sat : 
            for (int i=0; i<sz; i++)
               errfield.set(i, compute_cp_v_sat_p(p[i], r[i])) ;
            break ;
         case NEPTUNE::T_sat : 
            for (int i=0; i<sz; i++)
               errfield.set(i, compute_T_sat_p(p[i], r[i])) ;
            break ;
         // Derivatives     
         case NEPTUNE::d_rho_l_sat_d_p : 
            for (int i=0; i<sz; i++)
               errfield.set(i, compute_d_rho_l_sat_d_p_p(p[i], r[i])) ;
            break ;
         case NEPTUNE::d_rho_v_sat_d_p : 
            for (int i=0; i<sz; i++)
               errfield.set(i, compute_d_rho_v_sat_d_p_p(p[i], r[i])) ;
            break ;
         case NEPTUNE::d_h_l_sat_d_p : 
            for (int i=0; i<sz; i++)
               errfield.set(i, compute_d_h_l_sat_d_p_p(p[i], r[i])) ;
            break;
         case NEPTUNE::d_h_v_sat_d_p : 
            for (int i=0; i<sz; i++)
               errfield.set(i, compute_d_h_v_sat_d_p_p(p[i], r[i])) ;
            break ;
         case NEPTUNE::d_cp_l_sat_d_p : 
            for (int i=0; i<sz; i++)
               errfield.set(i, compute_d_cp_l_sat_d_p_p(p[i], r[i])) ;
            break ;
         case NEPTUNE::d_cp_v_sat_d_p : 
            for (int i=0; i<sz; i++)
               errfield.set(i, compute_d_cp_v_sat_d_p_p(p[i], r[i])) ;
            break ;
         case NEPTUNE::d_T_sat_d_p : 
            for (int i=0; i<sz; i++)
               errfield.set(i, compute_d_T_sat_d_p_p(p[i], r[i])) ;
            break ;

         case NEPTUNE::d2_T_sat_d_p_d_p : 
            for (int i=0; i<sz; i++)
               errfield.set(i, compute_d2_T_sat_d_p_d_p_p(p[i], r[i])) ;
            break;

            // Spinodale limits of Thermodynamic properties
         case NEPTUNE::h_l_lim : 
            for (int i=0; i<sz; i++)
               errfield.set(i, compute_h_l_lim_p(p[i], r[i])) ;
            break ;
         case NEPTUNE::h_v_lim : 
            for (int i=0; i<sz; i++)
               errfield.set(i, compute_h_v_lim_p(p[i], r[i])) ;
            break ;
         default :
            errfield = EOS_Internal_Error::NOT_IMPLEMENTED ;
            return EOS_Error::error ;
            break ;
       }

    return errfield.find_worst_error().generic_error() ;
  }


  EOS_Error EOS_Fluid::compute(const EOS_Field& p,
                               const EOS_Field& h,
                               EOS_Field& r,
                               EOS_Error_Field& errfield) const
  { const int sz = p.size() ;

    assert(h.size() == sz) ;
    assert(r.size() == sz) ;
    assert(errfield.size() == sz) ;

    errfield = EOS_Internal_Error::OK ;
    ArrOfDouble pp_data(sz) ;
    ArrOfDouble hh_data(sz) ;
    ArrOfDouble tt_data(sz) ;
    ArrOfDouble ss_data(sz) ;
    EOS_Field pp("pp","p",NEPTUNE::p,pp_data) ;
    EOS_Field hh("hh","h",NEPTUNE::h,hh_data) ;
    EOS_Field tt("tt","T",NEPTUNE::T,tt_data) ;
    EOS_Field ss("ss","s",NEPTUNE::s,ss_data) ;
    int lp = 0 ;
    int lh = 0 ;
    int lt = 0 ;
    int ls = 0 ;

    switch(p.get_property_number())
       { case NEPTUNE::p:
            pp = p ;
            lp = 1 ;
            break  ;
         case NEPTUNE::h:
            hh = p ;
            lh = 1 ;
            break  ;
         case NEPTUNE::T:
            tt = p ;
            lt = 1 ;
            break  ;
         case NEPTUNE::s:
            ss = p ;
            ls = 1 ;
            break  ;
         default: break;
       }

    switch(h.get_property_number())
       { case NEPTUNE::p:
            pp = h ;
            lp = 1 ;
            break  ;
         case NEPTUNE::h:
            hh = h ;
            lh = 1 ;
            break  ;
         case NEPTUNE::T:
            tt = h ;
            lt=+1 ;
            break ;
         case NEPTUNE::s:
            ss = h ;
            ls = 1 ;
            break  ;
         default: break;
       }

    if ( (lp == 1) && (lt == 1) )
       { for (int i=0; i<sz; i++)
            errfield.set(i, compute_h_pT(pp[i], tt[i], hh[i])) ;
         lh = 1 ;
       }

    if ( (lp == 1) && (ls == 1) ) 
       {
         switch(r.get_property_number()) 
            { case NEPTUNE::h : 
                 for (int i=0; i<sz; i++) 
                    { EOS_Internal_Error err = compute_h_ps(pp[i], ss[i], r[i]) ;
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err) ;
                    }
                 return errfield.find_worst_error().generic_error() ;

              case NEPTUNE::d_h_d_p_s :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_d_h_d_p_s_ps(pp[i], ss[i], r[i]) ;
                      err = worst_internal_error(errfield[i], err) ;
                      errfield.set(i, err) ;
                    }
                 return errfield.find_worst_error().generic_error() ;

              case NEPTUNE::d_h_d_s_p :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_d_h_d_s_p_ps(pp[i], ss[i], r[i]) ;
                      err = worst_internal_error(errfield[i], err) ;
                      errfield.set(i, err) ;
                    }
                 return errfield.find_worst_error().generic_error() ;

              default :
                 errfield = EOS_Internal_Error::NOT_IMPLEMENTED ;
                 return EOS_Error::error ;
            }
       }

    if ( (lp == 1) && (lh == 1) )
       {
         switch(r.get_property_number())
            { case NEPTUNE::h :
                 if (lt == 1)
                    { for (int i=0; i<sz; i++)
                        { EOS_Internal_Error err = compute_h_pT(pp[i],tt[i],r[i]) ;
                          err = worst_internal_error(errfield[i], err) ;
                          errfield.set(i, err) ;
                        }
                      return errfield.find_worst_error().generic_error() ;
                    }
                 else
                    { errfield = EOS_Internal_Error::NOT_IMPLEMENTED ;
                      return EOS_Error::error ;
                    }
              case NEPTUNE::T :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_T_ph(pp[i],hh[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::rho :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_rho_ph(pp[i],hh[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::u :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_u_ph(pp[i],hh[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::s :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_s_ph(pp[i],hh[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::mu :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_mu_ph(pp[i],hh[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::lambda :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_lambda_ph(pp[i],hh[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::cp :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_cp_ph(pp[i],hh[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::cv :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_cv_ph(pp[i],hh[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::sigma :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_sigma_ph(pp[i],hh[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::w :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_w_ph(pp[i],hh[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::g :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_g_ph(pp[i],hh[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::f :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_f_ph(pp[i],hh[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::pr :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_pr_ph(pp[i],hh[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::beta :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_beta_ph(pp[i],hh[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::gamma :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_gamma_ph(pp[i],hh[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();

              // Derivatives
              case NEPTUNE::d_T_d_p_h :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_d_T_d_p_h_ph(pp[i],hh[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::d_T_d_h_p :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_d_T_d_h_p_ph(pp[i],hh[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::d_rho_d_p_h :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_d_rho_d_p_h_ph(pp[i],hh[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::d_rho_d_h_p :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_d_rho_d_h_p_ph(pp[i],hh[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::d_u_d_p_h :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_d_u_d_p_h_ph(pp[i],hh[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::d_u_d_h_p :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_d_u_d_h_p_ph(pp[i],hh[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::d_s_d_p_h :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_d_s_d_p_h_ph(pp[i],hh[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::d_s_d_h_p :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_d_s_d_h_p_ph(pp[i],hh[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::d_mu_d_p_h :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_d_mu_d_p_h_ph(pp[i],hh[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::d_mu_d_h_p :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_d_mu_d_h_p_ph(pp[i],hh[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::d_lambda_d_p_h :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_d_lambda_d_p_h_ph(pp[i],hh[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::d_lambda_d_h_p :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_d_lambda_d_h_p_ph(pp[i],hh[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::d_cp_d_p_h :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_d_cp_d_p_h_ph(pp[i],hh[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::d_cp_d_h_p :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_d_cp_d_h_p_ph(pp[i],hh[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::d_cv_d_p_h :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_d_cv_d_p_h_ph(pp[i],hh[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::d_cv_d_h_p :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_d_cv_d_h_p_ph(pp[i],hh[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::d_sigma_d_p_h :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_d_sigma_d_p_h_ph(pp[i],hh[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::d_sigma_d_h_p :
                    for (int i=0; i<sz; i++)
                       { EOS_Internal_Error err = compute_d_sigma_d_h_p_ph(pp[i],hh[i],r[i]);
                         err = worst_internal_error(errfield[i], err);
                         errfield.set(i, err);
                       }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::d_w_d_p_h :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_d_w_d_p_h_ph(pp[i],hh[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::d_w_d_h_p :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_d_w_d_h_p_ph(pp[i],hh[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::d_g_d_p_h :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_d_g_d_p_h_ph(pp[i],hh[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::d_g_d_h_p :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_d_g_d_h_p_ph(pp[i],hh[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::d_f_d_p_h :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_d_f_d_p_h_ph(pp[i],hh[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::d_f_d_h_p :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_d_f_d_h_p_ph(pp[i],hh[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::d_pr_d_p_h :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_d_pr_d_p_h_ph(pp[i],hh[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::d_pr_d_h_p :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_d_pr_d_h_p_ph(pp[i],hh[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::d_beta_d_p_h :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_d_beta_d_p_h_ph(pp[i],hh[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::d_beta_d_h_p :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_d_beta_d_h_p_ph(pp[i],hh[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::d_gamma_d_p_h :
                    for (int i=0; i<sz; i++)
                       { EOS_Internal_Error err = compute_d_gamma_d_p_h_ph(pp[i],hh[i],r[i]);
                         err = worst_internal_error(errfield[i], err);
                         errfield.set(i, err);
                       }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::d_gamma_d_h_p :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_d_gamma_d_h_p_ph(pp[i],hh[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::d_rho_d_T_p :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_T_ph(pp[i],hh[i],tt[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_d_rho_d_T_p_pT(pp[i],tt[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::d_rho_d_p_T :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_T_ph(pp[i],hh[i],tt[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_d_rho_d_p_T_pT(pp[i],tt[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::d_u_d_T_p :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_T_ph(pp[i],hh[i],tt[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_d_u_d_T_p_pT(pp[i],tt[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::d_s_d_T_p :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_T_ph(pp[i],hh[i],tt[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_d_s_d_T_p_pT(pp[i],tt[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::d_s_d_p_T :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_T_ph(pp[i],hh[i],tt[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_d_s_d_p_T_pT(pp[i],tt[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::d_mu_d_T_p :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_T_ph(pp[i],hh[i],tt[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_d_mu_d_T_p_pT(pp[i],tt[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::d_mu_d_p_T :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_T_ph(pp[i],hh[i],tt[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_d_mu_d_p_T_pT(pp[i],tt[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::d_lambda_d_p_T :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_T_ph(pp[i],hh[i],tt[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_d_lambda_d_p_T_pT(pp[i],tt[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::d_lambda_d_T_p :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_T_ph(pp[i],hh[i],tt[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_d_lambda_d_T_p_pT(pp[i],tt[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::d_cp_d_T_p :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_T_ph(pp[i],hh[i],tt[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_d_cp_d_T_p_pT(pp[i],tt[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::d_cp_d_p_T :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_T_ph(pp[i],hh[i],tt[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_d_cp_d_p_T_pT(pp[i],tt[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::d_cv_d_T_p :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_T_ph(pp[i],hh[i],tt[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_d_cv_d_T_p_pT(pp[i],tt[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::d_cv_d_p_T :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_T_ph(pp[i],hh[i],tt[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_d_cv_d_p_T_pT(pp[i],tt[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::d_sigma_d_T_p :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_T_ph(pp[i],hh[i],tt[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_d_sigma_d_T_p_pT(pp[i],tt[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::d_w_d_T_p :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_T_ph(pp[i],hh[i],tt[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_d_w_d_T_p_pT(pp[i],tt[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::d_w_d_p_T :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_T_ph(pp[i],hh[i],tt[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_d_w_d_p_T_pT(pp[i],tt[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::d_g_d_T_p :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_T_ph(pp[i],hh[i],tt[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_d_g_d_T_p_pT(pp[i],tt[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::d_f_d_T_p :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_T_ph(pp[i],hh[i],tt[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_d_f_d_T_p_pT(pp[i],tt[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::d_pr_d_T_p :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_T_ph(pp[i],hh[i],tt[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_d_pr_d_T_p_pT(pp[i],tt[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::d_beta_d_T_p :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_T_ph(pp[i],hh[i],tt[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_d_beta_d_T_p_pT(pp[i],tt[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::d_gamma_d_T_p :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_T_ph(pp[i],hh[i],tt[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_d_gamma_d_T_p_pT(pp[i],tt[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::d_h_d_T_p :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_T_ph(pp[i],hh[i],tt[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_d_h_d_T_p_pT(pp[i],tt[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              case NEPTUNE::d_h_d_p_T :
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_T_ph(pp[i],hh[i],tt[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 for (int i=0; i<sz; i++)
                    { EOS_Internal_Error err = compute_d_h_d_p_T_pT(pp[i],tt[i],r[i]);
                      err = worst_internal_error(errfield[i], err);
                      errfield.set(i, err);
                    }
                 return errfield.find_worst_error().generic_error();
              default :
                 errfield = EOS_Internal_Error::NOT_IMPLEMENTED ;
                 return EOS_Error::error ;
            }
       }

    else
       { errfield = EOS_Internal_Error::NOT_IMPLEMENTED ;
         return EOS_Error::error ;
       }

    return EOS_Error::error ;
  }

//
  EOS_Internal_Error EOS_Fluid::compute(const char* const property_name,
                                        double p,
                                        double h,
                                        double& x) const
  { EOS_thermprop prop = nam2num_thermprop(property_name) ;

    return EOS_Fluid::compute(property_name,prop,p,h,x);
  }

EOS_Internal_Error EOS_Fluid::compute([[maybe_unused]] const char* const property_name,
                                      const int prop,
                                        double p,
                                        double h,
                                        double& x) const
  { 
    switch(prop)
       { // Thermodynamic Properties
         case NEPTUNE::T              :  return compute_T_ph(p, h, x)            ;
         case NEPTUNE::rho            :  return compute_rho_ph(p, h, x)          ;
         case NEPTUNE::u              :  return compute_u_ph(p, h, x)            ;
         case NEPTUNE::s              :  return compute_s_ph(p, h, x)            ;
         case NEPTUNE::mu             :  return compute_mu_ph(p, h, x)           ;
         case NEPTUNE::lambda         :  return compute_lambda_ph(p, h, x)       ;
         case NEPTUNE::cp             :  return compute_cp_ph(p, h, x)           ;
         case NEPTUNE::sigma          :  return compute_sigma_ph(p, h, x)        ;
         case NEPTUNE::w              :  return compute_w_ph(p, h, x)            ;
         case NEPTUNE::g              :  return compute_g_ph(p, h, x)            ;
         case NEPTUNE::f              :  return compute_f_ph(p, h, x)            ;
         case NEPTUNE::pr             :  return compute_pr_ph(p, h, x)           ;
         case NEPTUNE::beta           :  return compute_beta_ph(p, h, x)         ;
         case NEPTUNE::gamma          :  return compute_gamma_ph(p, h, x)        ;
         // Derivatives
         case NEPTUNE::d_T_d_p_h      :  return compute_d_T_d_p_h_ph(p,h,x)      ;
         case NEPTUNE::d_T_d_h_p      :  return compute_d_T_d_h_p_ph(p,h,x)      ;
         case NEPTUNE::d_rho_d_p_h    :  return compute_d_rho_d_p_h_ph(p,h,x)    ;
         case NEPTUNE::d_rho_d_h_p    :  return compute_d_rho_d_h_p_ph(p,h,x)    ;
         case NEPTUNE::d_u_d_p_h      :  return compute_d_u_d_p_h_ph(p,h,x)      ;
         case NEPTUNE::d_u_d_h_p      :  return compute_d_u_d_h_p_ph(p,h,x)      ;
         case NEPTUNE::d_s_d_p_h      :  return compute_d_s_d_p_h_ph(p,h,x)      ;
         case NEPTUNE::d_s_d_h_p      :  return compute_d_s_d_h_p_ph(p,h,x)      ;
         case NEPTUNE::d_mu_d_p_h     :  return compute_d_mu_d_p_h_ph(p,h,x)     ;
         case NEPTUNE::d_mu_d_h_p     :  return compute_d_mu_d_h_p_ph(p,h,x)     ;
         case NEPTUNE::d_lambda_d_p_h :  return compute_d_lambda_d_p_h_ph(p,h,x) ;
         case NEPTUNE::d_lambda_d_h_p :  return compute_d_lambda_d_h_p_ph(p,h,x) ;
         case NEPTUNE::d_cp_d_p_h     :  return compute_d_cp_d_p_h_ph(p,h,x)     ;
         case NEPTUNE::d_cp_d_h_p     :  return compute_d_cp_d_h_p_ph(p,h,x)     ;
         case NEPTUNE::d_sigma_d_p_h  :  return compute_d_sigma_d_p_h_ph(p,h,x)  ;
         case NEPTUNE::d_sigma_d_h_p  :  return compute_d_sigma_d_h_p_ph(p,h,x)  ;
         case NEPTUNE::d_w_d_p_h      :  return compute_d_w_d_p_h_ph(p,h,x)      ;
         case NEPTUNE::d_w_d_h_p      :  return compute_d_w_d_h_p_ph(p,h,x)      ;
         case NEPTUNE::d_g_d_p_h      :  return compute_d_g_d_p_h_ph(p,h,x)      ;
         case NEPTUNE::d_g_d_h_p      :  return compute_d_g_d_h_p_ph(p,h,x)      ;
         case NEPTUNE::d_f_d_p_h      :  return compute_d_f_d_p_h_ph(p,h,x)      ;
         case NEPTUNE::d_f_d_h_p      :  return compute_d_f_d_h_p_ph(p,h,x)      ;
         case NEPTUNE::d_pr_d_p_h     :  return compute_d_pr_d_p_h_ph(p,h,x)     ;
         case NEPTUNE::d_pr_d_h_p     :  return compute_d_pr_d_h_p_ph(p,h,x)     ;
         case NEPTUNE::d_beta_d_p_h   :  return compute_d_beta_d_p_h_ph(p,h,x)   ;
         case NEPTUNE::d_beta_d_h_p   :  return compute_d_beta_d_h_p_ph(p,h,x)   ;
         case NEPTUNE::d_gamma_d_p_h  :  return compute_d_gamma_d_p_h_ph(p,h,x)  ;
         case NEPTUNE::d_gamma_d_h_p  :  return compute_d_gamma_d_h_p_ph(p,h,x)  ;
         case NEPTUNE::d_rho_d_T_p    :  return compute_d_rho_d_T_p_pT(p,h,x)    ;
         case NEPTUNE::d_rho_d_p_T    :  return compute_d_rho_d_p_T_pT(p,h,x)    ;
         case NEPTUNE::d_u_d_T_p      :  return compute_d_u_d_T_p_pT(p,h,x)      ;
         case NEPTUNE::d_s_d_T_p      :  return compute_d_s_d_T_p_pT(p,h,x)      ;
         case NEPTUNE::d_s_d_p_T      :  return compute_d_s_d_p_T_pT(p,h,x)      ;
         case NEPTUNE::d_mu_d_T_p     :  return compute_d_mu_d_T_p_pT(p,h,x)     ;
         case NEPTUNE::d_mu_d_p_T     :  return compute_d_mu_d_p_T_pT(p,h,x)     ;
         case NEPTUNE::d_lambda_d_T_p :  return compute_d_lambda_d_T_p_pT(p,h,x) ;
         case NEPTUNE::d_cp_d_T_p     :  return compute_d_cp_d_T_p_pT(p,h,x)     ;
         case NEPTUNE::d_cp_d_p_T     :  return compute_d_cp_d_p_T_pT(p,h,x)     ;
         case NEPTUNE::d_cv_d_T_p     :  return compute_d_cv_d_T_p_pT(p,h,x)     ;
         case NEPTUNE::d_cv_d_p_T     :  return compute_d_cv_d_p_T_pT(p,h,x)     ;
         case NEPTUNE::d_sigma_d_T_p  :  return compute_d_sigma_d_T_p_pT(p,h,x)  ;
         case NEPTUNE::d_w_d_T_p      :  return compute_d_w_d_T_p_pT(p,h,x)      ;
         case NEPTUNE::d_w_d_p_T      :  return compute_d_w_d_p_T_pT(p,h,x)      ;
         case NEPTUNE::d_g_d_T_p      :  return compute_d_g_d_T_p_pT(p,h,x)      ;
         case NEPTUNE::d_f_d_T_p      :  return compute_d_f_d_T_p_pT(p,h,x)      ;
         case NEPTUNE::d_pr_d_T_p     :  return compute_d_pr_d_T_p_pT(p,h,x)     ;
         case NEPTUNE::d_beta_d_T_p   :  return compute_d_beta_d_T_p_pT(p,h,x)   ;
         case NEPTUNE::d_gamma_d_T_p  :  return compute_d_gamma_d_T_p_pT(p,h,x)  ;
         case NEPTUNE::d_h_d_T_p      :  return compute_d_h_d_T_p_pT(p,h,x)      ;
         case NEPTUNE::d_h_d_p_T      :  return compute_d_h_d_p_T_pT(p,h,x)      ;
         default :  return EOS_Internal_Error::NOT_IMPLEMENTED ;

       }
  }

//
  EOS_Internal_Error EOS_Fluid::compute(const char* const property_name,
                                        double p,
                                        double& x) const
  { EOS_saturprop propsat = nam2num_saturprop(property_name) ;
    return EOS_Fluid::compute(property_name,propsat,p,x);
  }

    EOS_Internal_Error EOS_Fluid::compute([[maybe_unused]]const char* const property_name,
                                        const int propsat, 
                                        double p,
                                        double& x) const
  { 
    switch(propsat)
      { case NEPTUNE::T_sat            :  return compute_T_sat_p(p, x)           ;
        case NEPTUNE::rho_l_sat        :  return compute_rho_l_sat_p(p, x)       ;
        case NEPTUNE::rho_v_sat        :  return compute_rho_v_sat_p(p, x)       ;
        case NEPTUNE::h_l_sat          :  return compute_h_l_sat_p(p, x)         ;
        case NEPTUNE::h_v_sat          :  return compute_h_v_sat_p(p, x)         ;
        case NEPTUNE::cp_l_sat         :  return compute_cp_l_sat_p(p, x)        ;
        case NEPTUNE::cp_v_sat         :  return compute_cp_v_sat_p(p, x)        ;
        // Derivatives
        case NEPTUNE::d_rho_l_sat_d_p  :  return compute_d_rho_l_sat_d_p_p(p, x) ;
        case NEPTUNE::d_rho_v_sat_d_p  :  return compute_d_rho_v_sat_d_p_p(p, x) ;
        case NEPTUNE::d_h_l_sat_d_p    :  return compute_d_h_l_sat_d_p_p(p, x)   ;
        case NEPTUNE::d_h_v_sat_d_p    :  return compute_d_h_v_sat_d_p_p(p, x)   ;
        case NEPTUNE::d_cp_l_sat_d_p   :  return compute_d_cp_l_sat_d_p_p(p, x)  ;
        case NEPTUNE::d_cp_v_sat_d_p   :  return compute_d_cp_v_sat_d_p_p(p, x)  ;
        case NEPTUNE::d_T_sat_d_p      :  return compute_d_T_sat_d_p_p(p, x)     ;
        // Second Derivatives
        case NEPTUNE::d2_T_sat_d_p_d_p :  return compute_d2_T_sat_d_p_d_p_p(p,x);
        default :  return EOS_Internal_Error::NOT_IMPLEMENTED ;
      }
  }

  //
  EOS_Internal_Error EOS_Fluid::compute_Ph(const char* const property_name,
                                           double in1, double in2, double& out) const 
  { 
    const int property_number = gen_property_number(property_name);
    return compute_Ph(property_name,property_number,in1,in2,out);
  }

    EOS_Internal_Error EOS_Fluid::compute_Ph(const char* const property_name,
                                             const int property_number,
                                           double in1, double in2, double& out) const 
  { int err_data[1]  ;
    EOS_Field fP("P","P",NEPTUNE::p,1,&in1) ;
    EOS_Field fh("h","h",NEPTUNE::h,1,&in2) ;
    EOS_Field fout(property_name,property_name,property_number,1,&out) ; 
    EOS_Error_Field ferr(1,err_data) ;

    compute(fP, fh, fout, ferr) ;
    return ferr[0] ;
  }

  EOS_Internal_Error EOS_Fluid::compute_PT(const char* const property_name,
                                           double in1, double in2, double& out) const
  { 
    const int property_number = gen_property_number(property_name);
    return compute_PT(property_name,property_number,in1,in2,out);
  }

  EOS_Internal_Error EOS_Fluid::compute_PT(const char* const property_name,
                                           const int property_number,
                                           double in1, double in2, double& out) const
  { 
    int err_data[1]  ;
    EOS_Field fP("P","P",NEPTUNE::p,1,&in1) ;
    EOS_Field fT("T","T",NEPTUNE::T,1,&in2) ;
    EOS_Field fout(property_name,property_name,property_number,1,&out) ; 
    EOS_Error_Field ferr(1,err_data) ;

    compute(fP, fT, fout, ferr) ;
    return ferr[0] ;
  }

  EOS_Internal_Error EOS_Fluid::compute_Psat(const char* const property_name,
                                             double in, double& out) const
  {
    const int property_number = gen_property_number(property_name);
    return compute_Psat(property_name,property_number,in,out);
  }

  EOS_Internal_Error EOS_Fluid::compute_Psat(const char* const property_name,
                                             const int property_number,
                                             double in, double& out) const
  { int err_data[1]  ;
    EOS_Field fP("Psat","Psat",NEPTUNE::p_sat,1,&in) ;
    EOS_Field fout(property_name,property_name,property_number,1,&out) ; 
    EOS_Error_Field ferr(1,err_data) ;

    compute(fP, fout, ferr) ;
    return ferr[0] ;
  }

  EOS_Internal_Error EOS_Fluid::compute_Tsat(const char* const property_name,
                                             double in, double& out) const
  { 
    const int property_number = gen_property_number(property_name);
    return compute_Tsat(property_name,property_number,in,out);
  }

    EOS_Internal_Error EOS_Fluid::compute_Tsat(const char* const property_name,
                                             const int property_number,
                                             double in, double& out) const
  { int err_data[1]  ;
    EOS_Field fT("Tsat","Tsat",NEPTUNE::T_sat,1,&in) ;
    EOS_Field fout(property_name,property_name,property_number,1,&out) ;
    EOS_Error_Field ferr(1,err_data) ;

    compute(fT, fout, ferr) ;
    return ferr[0] ;
  }

  //
  EOS_Internal_Error EOS_Fluid::get_p(double& /*p*/) const
  { cerr << " *** eos_fluid *** Not_implemented get_p,  P = "  << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::get_mm(double& /*mm*/) const
  { cerr << " *** eos_fluid *** Not_implemented get_mm,  mm = " << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::get_p_crit(double& /*p_crit*/) const
  { cerr << " *** eos_fluid *** Not_implemented get_p_crit,  p_crit = " << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::get_h_crit(double& /*h_crit*/) const
  { cerr << " *** eos_fluid *** Not_implemented get_h_crit,  h_crit = " << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::get_T_crit(double& /*T_crit*/) const
  { cerr << " *** eos_fluid *** Not_implemented get_T_crit,  T_crit = " << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::get_rho_crit(double& /*rho_crit*/) const
  { cerr << " *** eos_fluid *** Not_implemented get_rho_crit,  rho_crit = "<< endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::get_p_min(double& /*p_min*/) const
  { cerr << " *** eos_fluid *** Not_implemented get_p_min,  p_min = "  << endl ;
    cerr << " *** eos_fluid *** The model does not have a minimum pressure implemented; the user code must define it "  << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::get_p_max(double& /*p_max*/) const
  { cerr << " *** eos_fluid *** Not_implemented get_p_max,  p_max = "  << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::get_h_min(double& /*h_min*/) const
  { cerr << " *** eos_fluid *** Not_implemented get_h_min,  h_min = "  << endl ;
    cerr << " *** eos_fluid *** The model does not have a minimum enthalpy implemented; the user code must define it. "  << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::get_h_max(double& /*h_max*/) const
  { cerr << " *** eos_fluid *** Not_implemented get_h_max,  h_max = "  << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::get_T_min(double& /*T_min*/) const
  { cerr << " *** eos_fluid *** Not_implemented get_T_min,  T_min = "  << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::get_T_max(double& /*T_max*/) const
  { cerr << " *** eos_fluid *** Not_implemented get_T_max,  T_max = "  << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::get_rho_min(double& /*rho_min*/) const
  { cerr << " *** eos_fluid *** Not_implemented get_rho_min,  rho_min = "  << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::get_rho_max(double& /*rho_max*/) const
  { cerr << " *** eos_fluid *** Not_implemented get_rho_max,  rho_max = " << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::get_p_ref(double& /*p_ref*/) const
  { cerr << " *** eos_fluid *** Not_implemented get_p_ref,  p_ref = " << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::get_h_ref(double& /*h_ref*/) const
  { cerr << " *** eos_fluid *** Not_implemented get_h_ref,  h_ref = " << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::get_T_ref(double& /*T_ref*/) const
  { cerr << " *** eos_fluid *** Not_implemented get_T_ref,  T_ref = " << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  //
  EOS_Internal_Error EOS_Fluid::compute_pr_ph(double p, double h, double& pr) const
  { double mu, cp, lambda ;
    EOS_Internal_Error ierr ;
    pr = 0.e0 ;

    ierr = compute_mu_ph(p, h, mu) ;
    if (ierr.generic_error() >= EOS_Error::bad)  return ierr ;

    ierr = compute_cp_ph(p, h, cp) ;
    if (ierr.generic_error() >= EOS_Error::bad)  return ierr ;

    ierr = compute_lambda_ph(p, h, lambda) ;
    if (ierr.generic_error() >= EOS_Error::bad)  return ierr ;

    if (lambda <= 0.e0)  return Error_lambda_value ;
    pr = (mu*cp)/lambda ;

    return ierr ;
  }

  //
  EOS_Internal_Error EOS_Fluid::compute_pr_pT(double p, double T, double& pr) const
  { double mu, cp, lambda ;
    EOS_Internal_Error ierr ;
    pr = 0.e0 ;

    ierr = compute_mu_pT(p, T, mu) ;
    if (ierr.generic_error() >= EOS_Error::bad)  return ierr ;

    ierr = compute_cp_pT(p, T, cp) ;
    if (ierr.generic_error() >= EOS_Error::bad)  return ierr ;

    ierr = compute_lambda_pT(p, T, lambda) ;
    if (ierr.generic_error() >= EOS_Error::bad)  return ierr ;

    if (lambda <= 0.e0)  return Error_lambda_value ;
    pr = (mu*cp)/lambda ;

    return ierr ;
  }

  // Default implementation for p_sat(T)
  // Newton iteration to solve for P such that T_sat(P) = T
  EOS_Internal_Error EOS_Fluid::compute_p_sat_T(double T, double& p) const
  { EOS_Internal_Error err1(EOS_Internal_Error::OK) ;
    EOS_Internal_Error err2(EOS_Internal_Error::OK) ;

    double Ti, dTi, deltap ;
    double func = 1.e0 ;
    double pi   = 1.e5 ; // Starting from 1 bar.
    for (iter_Newton = 0 ;
        (iter_Newton < nb_iter_max) && (fabs(func) > epsn);
        iter_Newton++ )
      { err1   = compute_T_sat_p(pi,Ti) ;
        err2   = compute_d_T_sat_d_p_p(pi, dTi) ;
        func   = T - Ti      ;
        deltap = func / dTi  ;
        pi     = pi + deltap ;
        // If bad value encountered during Newton iterations, error
        switch (worst_internal_error(err1, err2).generic_error())
           { case EOS_Error::error :  return worst_internal_error(err1, err2) ;
             case EOS_Error::bad   :  return Error_p_sat_T_compute            ;
             default               :  break ;
        }
      }
    // If not converged, error.
    if ( (iter_Newton >= nb_iter_max) && (fabs(func) > epsn) )  return Error_p_sat_T_newton ;

    p = pi ;
    // Result is based on the last compute_T_sat_p, so return the same error code:
    return err1 ;
  }


  // - --- Auto --- -

  EOS_Internal_Error EOS_Fluid::compute_T_ph(double p, double h, double& r) const
  { static int loop = 0 ;
  #ifdef _OPENMP
  #pragma omp threadprivate(loop)
  #endif

    if (!loop)
       { loop = 1 ;
         const EOS_Field fin1("p", "p",NEPTUNE::p, 1, &p) ;
         const EOS_Field fin2("h", "h",NEPTUNE::h, 1, &h) ;
         EOS_Field fout("T", "T",NEPTUNE::T, 1, &r) ;
         ArrOfInt tmp(1) ;
         EOS_Error_Field ferr(tmp) ;
         compute(fin1, fin2, fout, ferr) ;
         loop = 0 ;
         return ferr[0] ;
       }
    loop = 0 ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_rho_ph(double p, double h, double& r) const
  { static int loop = 0 ;
  #ifdef _OPENMP
  #pragma omp threadprivate(loop)
  #endif
    if (!loop)
       { loop = 1 ;
         const EOS_Field fin1("p", "p",NEPTUNE::p, 1, &p) ;
         const EOS_Field fin2("h", "h",NEPTUNE::h, 1, &h) ;
         EOS_Field fout("rho", "rho",NEPTUNE::rho, 1, &r) ;
         ArrOfInt tmp(1) ;
         EOS_Error_Field ferr(tmp) ;
         compute(fin1, fin2, fout, ferr) ;
         loop = 0 ;
         return ferr[0] ;
       }
    loop = 0 ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_u_ph(double p, double h, double& r) const
  { static int loop = 0 ;
  #ifdef _OPENMP
  #pragma omp threadprivate(loop)
  #endif
    if (!loop)
       { loop = 1 ;
         const EOS_Field fin1("p", "p",NEPTUNE::p, 1, &p) ;
         const EOS_Field fin2("h", "h",NEPTUNE::h, 1, &h) ;
         EOS_Field fout("u", "u",NEPTUNE::u, 1, &r) ;
         ArrOfInt tmp(1) ;
         EOS_Error_Field ferr(tmp) ;
         compute(fin1, fin2, fout, ferr) ;
         loop = 0 ;
         return ferr[0] ;
       }
    else
       return EOS_Internal_Error::NOT_IMPLEMENTED ;
    loop = 0 ;
  }

  EOS_Internal_Error EOS_Fluid::compute_s_ph(double p, double h, double& r) const
  { static int loop = 0 ;
    if (!loop)
       { loop = 1 ;
         const EOS_Field fin1("p", "p",NEPTUNE::p, 1, &p) ;
         const EOS_Field fin2("h", "h",NEPTUNE::h, 1, &h) ;
         EOS_Field fout("s", "s",NEPTUNE::s, 1, &r) ;
         ArrOfInt tmp(1) ;
         EOS_Error_Field ferr(tmp) ;
         compute(fin1, fin2, fout, ferr) ;
         loop = 0 ;
         return ferr[0] ;
       }
    loop = 0 ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_mu_ph(double p, double h, double& r) const
  { static int loop = 0 ;
  #ifdef _OPENMP
  #pragma omp threadprivate(loop)
  #endif
    if (!loop)
       { loop = 1 ;
         const EOS_Field fin1("p", "p",NEPTUNE::p, 1, &p) ;
         const EOS_Field fin2("h", "h",NEPTUNE::h, 1, &h) ;
         EOS_Field fout("mu", "mu",NEPTUNE::mu, 1, &r) ;
         ArrOfInt tmp(1) ;
         EOS_Error_Field ferr(tmp) ;
         compute(fin1, fin2, fout, ferr) ;
         loop = 0 ;
         return ferr[0] ;
       }
    loop = 0 ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_lambda_ph(double p, double h, double& r) const
  { static int loop = 0 ;
  #ifdef _OPENMP
  #pragma omp threadprivate(loop)
  #endif
    if (!loop)
       { loop = 1 ;
         const EOS_Field fin1("p", "p",NEPTUNE::p, 1, &p) ;
         const EOS_Field fin2("h", "h",NEPTUNE::h, 1, &h) ;
         EOS_Field fout("lambda", "lambda",NEPTUNE::lambda, 1, &r) ;
         ArrOfInt tmp(1) ;
         EOS_Error_Field ferr(tmp) ;
         compute(fin1, fin2, fout, ferr) ;
         loop = 0 ;
         return ferr[0] ;
       }

    loop = 0 ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_cp_ph(double p, double h, double& r) const
  { static int loop = 0 ;
  #ifdef _OPENMP
  #pragma omp threadprivate(loop)
  #endif
    if (!loop)
       { loop = 1 ;
         const EOS_Field fin1("p", "p",NEPTUNE::p, 1, &p) ;
         const EOS_Field fin2("h", "h",NEPTUNE::h, 1, &h) ;
         EOS_Field fout("cp", "cp",NEPTUNE::cp, 1, &r) ;
         ArrOfInt tmp(1) ;
         EOS_Error_Field ferr(tmp) ;
         compute(fin1, fin2, fout, ferr) ;
         loop = 0 ;
         return ferr[0] ;
       }
    loop = 0 ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_cv_ph(double p, double h, double& r) const
  { static int loop = 0 ;
  #ifdef _OPENMP
  #pragma omp threadprivate(loop)
  #endif
    if (!loop)
       { loop = 1 ;
         const EOS_Field fin1("p", "p",NEPTUNE::p, 1, &p) ;
         const EOS_Field fin2("h", "h",NEPTUNE::h, 1, &h) ;
         EOS_Field fout("cv", "cv",NEPTUNE::cv, 1, &r) ;
         ArrOfInt tmp(1) ;
         EOS_Error_Field ferr(tmp) ;
         compute(fin1, fin2, fout, ferr) ;
         loop = 0 ;
         return ferr[0] ;
       }
    loop = 0 ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_sigma_ph(double p, double h, double& r) const
  { static int loop = 0 ;
  #ifdef _OPENMP
  #pragma omp threadprivate(loop)
  #endif
    if (!loop)
       { loop = 1 ;
         const EOS_Field fin1("p", "p",NEPTUNE::p, 1, &p) ;
         const EOS_Field fin2("h", "h",NEPTUNE::h, 1, &h) ;
         EOS_Field fout("sigma", "sigma",NEPTUNE::sigma, 1, &r) ;
         ArrOfInt tmp(1) ;
         EOS_Error_Field ferr(tmp) ;
         compute(fin1, fin2, fout, ferr) ;
         loop = 0 ;
         return ferr[0] ;
       }
    loop = 0 ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_w_ph(double p, double h, double& r) const
  { static int loop = 0 ;
  #ifdef _OPENMP
  #pragma omp threadprivate(loop)
  #endif
    if (!loop)
       { loop = 1 ;
         const EOS_Field fin1("p", "p",NEPTUNE::p, 1, &p) ;
         const EOS_Field fin2("h", "h",NEPTUNE::h, 1, &h) ;
         EOS_Field fout("w", "w",NEPTUNE::w, 1, &r) ;
         ArrOfInt tmp(1) ;
         EOS_Error_Field ferr(tmp) ;
         compute(fin1, fin2, fout, ferr) ;
         loop = 0 ;
         return ferr[0] ;
       }
    loop = 0 ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_g_ph(double p, double h, double& r) const
  { static int loop = 0 ;
  #ifdef _OPENMP
  #pragma omp threadprivate(loop)
  #endif
    if (!loop)
       { loop = 1 ;
         const EOS_Field fin1("p", "p",NEPTUNE::p, 1, &p) ;
         const EOS_Field fin2("h", "h",NEPTUNE::h, 1, &h) ;
         EOS_Field fout("g", "g",NEPTUNE::g, 1, &r) ;
         ArrOfInt tmp(1) ;
         EOS_Error_Field ferr(tmp) ;
         compute(fin1, fin2, fout, ferr) ;
         loop = 0 ;
         return ferr[0] ;
       }
    loop = 0 ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_f_ph(double p, double h, double& r) const
  { static int loop = 0 ;
  #ifdef _OPENMP
  #pragma omp threadprivate(loop)
  #endif
    if (!loop)
       { loop = 1 ;
         const EOS_Field fin1("p", "p",NEPTUNE::p, 1, &p) ;
         const EOS_Field fin2("h", "h",NEPTUNE::h, 1, &h) ;
         EOS_Field fout("f", "f",NEPTUNE::f, 1, &r) ;
         ArrOfInt tmp(1) ;
         EOS_Error_Field ferr(tmp) ;
         compute(fin1, fin2, fout, ferr) ;
         loop = 0 ;
         return ferr[0] ;
       }
    loop = 0 ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }


  EOS_Internal_Error EOS_Fluid::compute_beta_ph(double p, double h, double& r) const
  { static int loop = 0 ;
  #ifdef _OPENMP
  #pragma omp threadprivate(loop)
  #endif
    if (!loop)
       { loop = 1 ;
         const EOS_Field fin1("p", "p",NEPTUNE::p, 1, &p) ;
         const EOS_Field fin2("h", "h",NEPTUNE::h, 1, &h) ;
         EOS_Field fout("beta", "beta",NEPTUNE::beta, 1, &r) ;
         ArrOfInt tmp(1) ;
         EOS_Error_Field ferr(tmp) ;
         compute(fin1, fin2, fout, ferr) ;
         loop = 0 ;
         return ferr[0] ;
       }
    loop = 0 ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_gamma_ph(double p, double h, double& r) const
  { static int loop = 0 ;
  #ifdef _OPENMP
  #pragma omp threadprivate(loop)
  #endif
   if (!loop)
       { loop = 1 ;
         const EOS_Field fin1("p", "p",NEPTUNE::p, 1, &p) ;
         const EOS_Field fin2("h", "h",NEPTUNE::h, 1, &h) ;
         EOS_Field fout("gamma", "gamma",NEPTUNE::gamma, 1, &r) ;
         ArrOfInt tmp(1) ;
         EOS_Error_Field ferr(tmp) ;
         compute(fin1, fin2, fout, ferr) ;
         loop = 0 ;
         return ferr[0] ;
       }
    loop = 0 ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_h_pT(double p, double T, double& r) const
  { static int loop = 0;
  #ifdef _OPENMP
  #pragma omp threadprivate(loop)
  #endif
    EOS_Internal_Error ierr(EOS_Internal_Error::NOT_IMPLEMENTED) ;
    if (!loop)
       { loop = 1 ;
         const EOS_Field fin1("p", "p",NEPTUNE::p, 1, &p) ;
         const EOS_Field fin2("T", "T",NEPTUNE::T, 1, &T) ;
         EOS_Field fout("h", "h",NEPTUNE::h, 1, &r) ;
         ArrOfInt tmp(1) ;
         EOS_Error_Field ferr(tmp) ;
         compute(fin1, fin2, fout, ferr) ;
         loop = 0 ;
         ierr = ferr[0] ;
       }
    return ierr ;
  }

  EOS_Internal_Error EOS_Fluid::compute_rho_pT(double p, double T, double& r) const
  { double r2 ;
    EOS_Internal_Error ierr1, ierr2 ;
    ierr1 = compute_h_pT(p, T, r2) ;
    if (ierr1 == EOS_Internal_Error::OK)
       { ierr2 = compute_rho_ph(p, r2, r) ;
         return ierr2 ;
       }
    else
       return ierr1 ;
  }

  EOS_Internal_Error EOS_Fluid::compute_u_pT(double p, double T, double& r) const
  { double r2 ;
    EOS_Internal_Error ierr1, ierr2 ;
    ierr1 = compute_h_pT(p, T, r2) ;
    if (ierr1 == EOS_Internal_Error::OK)
       { ierr2 = compute_u_ph(p, r2, r) ;
         return ierr2 ;
       }
    else
       return ierr1 ;
  }

  EOS_Internal_Error EOS_Fluid::compute_s_pT(double p, double T, double& r) const
  { double r2 ;
    EOS_Internal_Error ierr1, ierr2 ;
    ierr1 = compute_h_pT(p, T, r2) ;
    if (ierr1 == EOS_Internal_Error::OK)
       { ierr2 = compute_s_ph(p, r2, r) ;
         return ierr2 ;
       }
    else
       return ierr1 ;
  }

  EOS_Internal_Error EOS_Fluid::compute_mu_pT(double p, double T, double& r) const
  { double r2 ;
    EOS_Internal_Error ierr1, ierr2 ;
    ierr1 = compute_h_pT(p, T, r2) ;
    if (ierr1 == EOS_Internal_Error::OK)
       { ierr2 = compute_mu_ph(p, r2, r) ;
         return ierr2 ;
       }
    else
       return ierr1 ;
  }

  EOS_Internal_Error EOS_Fluid::compute_lambda_pT(double p, double T, double& r) const
  { double r2 ;
    EOS_Internal_Error ierr1, ierr2 ;
    ierr1 = compute_h_pT(p, T, r2) ;
    if (ierr1 == EOS_Internal_Error::OK)
       { ierr2 = compute_lambda_ph(p, r2, r) ;
         return ierr2 ;
       }
    else
       return ierr1 ;
  }

  EOS_Internal_Error EOS_Fluid::compute_cp_pT(double p, double T, double& r) const
  { double r2 ;
    EOS_Internal_Error ierr1, ierr2 ;
    ierr1 = compute_h_pT(p, T, r2) ;
    if (ierr1 == EOS_Internal_Error::OK)
       { ierr2 = compute_cp_ph(p, r2, r) ;
         return ierr2 ;
       }
    else
       return ierr1 ;
  }

  EOS_Internal_Error EOS_Fluid::compute_cv_pT(double p, double T, double& r) const
  { double r2 ;
    EOS_Internal_Error ierr1, ierr2 ;
    ierr1 = compute_h_pT(p, T, r2) ;
    if (ierr1 == EOS_Internal_Error::OK)
       { ierr2 = compute_cv_ph(p, r2, r) ;
         return ierr2 ;
       }
    else
       return ierr1 ;
  }

  EOS_Internal_Error EOS_Fluid::compute_sigma_pT(double p, double T, double& r) const
  { double r2 ;
    EOS_Internal_Error ierr1, ierr2 ;
    ierr1 = compute_h_pT(p, T, r2) ;
    if (ierr1 == EOS_Internal_Error::OK)
       { ierr2 = compute_sigma_ph(p, r2, r) ;
         return ierr2 ;
       }
    else
       return ierr1 ;
  }

  EOS_Internal_Error EOS_Fluid::compute_w_pT(double p, double T, double& r) const
  { double r2 ;
    EOS_Internal_Error ierr1, ierr2 ;
    ierr1 = compute_h_pT(p, T, r2) ;
    if (ierr1 == EOS_Internal_Error::OK)
       { ierr2 = compute_w_ph(p, r2, r) ;
         return ierr2 ;
       }
    else
       return ierr1 ;
  }

  EOS_Internal_Error EOS_Fluid::compute_g_pT(double p, double T, double& r) const
  { double r2 ;
    EOS_Internal_Error ierr1, ierr2 ;
    ierr1 = compute_h_pT(p, T, r2) ;
    if (ierr1 == EOS_Internal_Error::OK)
       { ierr2 = compute_g_ph(p, r2, r) ;
         return ierr2 ;
       }
    else
       return ierr1 ;
  }

  EOS_Internal_Error EOS_Fluid::compute_f_pT(double p, double T, double& r) const
  { double r2 ;
    EOS_Internal_Error ierr1, ierr2 ;
    ierr1 = compute_h_pT(p, T, r2) ;
    if (ierr1 == EOS_Internal_Error::OK)
       { ierr2 = compute_f_ph(p, r2, r) ;
         return ierr2 ;
       }
    else
       return ierr1 ;
  }


  EOS_Internal_Error EOS_Fluid::compute_beta_pT(double p, double T, double& r) const
  { double r2 ;
    EOS_Internal_Error ierr1, ierr2 ;
    ierr1 = compute_h_pT(p, T, r2) ;
    if (ierr1 == EOS_Internal_Error::OK)
       { ierr2 = compute_beta_ph(p, r2, r) ;
         return ierr2 ;
       }
    else
       return ierr1 ;
  }

  EOS_Internal_Error EOS_Fluid::compute_gamma_pT(double p, double T, double& r) const
  { double r2 ;
    EOS_Internal_Error ierr1, ierr2 ;
    ierr1 = compute_h_pT(p, T, r2) ;
    if (ierr1 == EOS_Internal_Error::OK)
       { ierr2 = compute_gamma_ph(p, r2, r) ;
         return ierr2 ;
       }
    else
       return ierr1 ;
  }

  EOS_Internal_Error EOS_Fluid::compute_h_ps(double p, double s, double& r) const
  { static int loop = 0;
  #ifdef _OPENMP
  #pragma omp threadprivate(loop)
  #endif
    EOS_Internal_Error ierr(EOS_Internal_Error::NOT_IMPLEMENTED) ;
    if (!loop)
       { loop = 1 ;
         const EOS_Field fin1("p", "p",NEPTUNE::p, 1, &p) ;
         const EOS_Field fin2("s", "s",NEPTUNE::s, 1, &s) ;
         EOS_Field fout("h", "h",NEPTUNE::h, 1, &r) ;
         ArrOfInt tmp(1) ;
         EOS_Error_Field ferr(tmp) ;
         compute(fin1, fin2, fout, ferr) ;
         loop = 0 ;
         ierr = ferr[0] ;
       }
    return ierr ;
  }

  EOS_Internal_Error EOS_Fluid::compute_T_ps(double p, double s, double& r) const
  { double r2 ;
    EOS_Internal_Error ierr1, ierr2 ;
    ierr1 = compute_h_ps(p, s, r2) ;
    if (ierr1 == EOS_Internal_Error::OK)
       { ierr2 = compute_T_ph(p, r2, r) ;
         return ierr2 ;
       }
    else
       return ierr1 ;
  }

  EOS_Internal_Error EOS_Fluid::compute_rho_ps(double p, double s, double& r) const
  { double r2 ;
    EOS_Internal_Error ierr1, ierr2 ;
    ierr1 = compute_h_ps(p, s, r2) ;
    if (ierr1 == EOS_Internal_Error::OK)
       { ierr2 = compute_rho_ph(p, r2, r) ;
         return ierr2 ;
       }
    else
       return ierr1 ;
  }

  EOS_Internal_Error EOS_Fluid::compute_u_ps(double p, double s, double& r) const
  { double r2 ;
    EOS_Internal_Error ierr1, ierr2 ;
    ierr1 = compute_h_ps(p, s, r2) ;
    if (ierr1 == EOS_Internal_Error::OK)
       { ierr2 = compute_u_ph(p, r2, r) ;
         return ierr2 ;
       }
    else
       return ierr1 ;
  }

  EOS_Internal_Error EOS_Fluid::compute_mu_ps(double p, double s, double& r) const
  { double r2 ;
    EOS_Internal_Error ierr1, ierr2 ;
    ierr1 = compute_h_ps(p, s, r2) ;
    if (ierr1 == EOS_Internal_Error::OK)
       { ierr2 = compute_mu_ph(p, r2, r) ;
         return ierr2 ;
       }
    else
       return ierr1 ;
  }

  EOS_Internal_Error EOS_Fluid::compute_lambda_ps(double p, double s, double& r) const
  { double r2 ;
    EOS_Internal_Error ierr1, ierr2 ;
    ierr1 = compute_h_ps(p, s, r2) ;
    if (ierr1 == EOS_Internal_Error::OK)
       { ierr2 = compute_lambda_ph(p, r2, r) ;
         return ierr2 ;
       }
    else
       return ierr1 ;
  }

  EOS_Internal_Error EOS_Fluid::compute_cp_ps(double p, double s, double& r) const
  { double r2 ;
    EOS_Internal_Error ierr1, ierr2 ;
    ierr1 = compute_h_ps(p, s, r2) ;
    if (ierr1 == EOS_Internal_Error::OK)
       { ierr2 = compute_cp_ph(p, r2, r) ;
         return ierr2 ;
       }
    else
       return ierr1 ;
  }

  EOS_Internal_Error EOS_Fluid::compute_cv_ps(double p, double s, double& r) const
  { double r2 ;
    EOS_Internal_Error ierr1, ierr2 ;
    ierr1 = compute_h_ps(p, s, r2) ;
    if (ierr1 == EOS_Internal_Error::OK)
       { ierr2 = compute_cv_ph(p, r2, r) ;
         return ierr2 ;
       }
    else
       return ierr1 ;
  }

  EOS_Internal_Error EOS_Fluid::compute_sigma_ps(double p, double s, double& r) const
  { double r2 ;
    EOS_Internal_Error ierr1, ierr2 ;
    ierr1 = compute_h_ps(p, s, r2) ;
    if (ierr1 == EOS_Internal_Error::OK)
       { ierr2 = compute_sigma_ph(p, r2, r) ;
         return ierr2 ;
       }
    else
       return ierr1 ;
  }

  EOS_Internal_Error EOS_Fluid::compute_w_ps(double p, double s, double& r) const
  { double r2 ;
    EOS_Internal_Error ierr1, ierr2 ;
    ierr1 = compute_h_ps(p, s, r2) ;
    if (ierr1 == EOS_Internal_Error::OK)
       { ierr2 = compute_w_ph(p, r2, r) ;
         return ierr2 ;
       }
    else
       return ierr1 ;
  }

  EOS_Internal_Error EOS_Fluid::compute_g_ps(double p, double s, double& r) const
  { double r2 ;
    EOS_Internal_Error ierr1, ierr2 ;
    ierr1 = compute_h_ps(p, s, r2) ;
    if (ierr1 == EOS_Internal_Error::OK)
       { ierr2 = compute_g_ph(p, r2, r) ;
         return ierr2 ;
       }
    else
       return ierr1 ;
  }

  EOS_Internal_Error EOS_Fluid::compute_f_ps(double p, double s, double& r) const
  { double r2 ;
    EOS_Internal_Error ierr1, ierr2 ;
    ierr1 = compute_h_ps(p, s, r2) ;
    if (ierr1 == EOS_Internal_Error::OK)
       { ierr2 = compute_f_ph(p, r2, r) ;
         return ierr2 ;
       }
    else
       return ierr1 ;
  }

  EOS_Internal_Error EOS_Fluid::compute_pr_ps(double p, double s, double& r) const
  { double r2 ;
    EOS_Internal_Error ierr1, ierr2 ;
    ierr1 = compute_h_ps(p, s, r2) ;
    if (ierr1 == EOS_Internal_Error::OK)
       { ierr2 = compute_pr_ph(p, r2, r) ;
         return ierr2 ;
       }
    else
       return ierr1 ;
  }

  EOS_Internal_Error EOS_Fluid::compute_beta_ps(double p, double s, double& r) const
  { double r2 ;
    EOS_Internal_Error ierr1, ierr2 ;
    ierr1 = compute_h_ps(p, s, r2) ;
    if (ierr1 == EOS_Internal_Error::OK)
       { ierr2 = compute_beta_ph(p, r2, r) ;
         return ierr2 ;
       }
    else
       return ierr1 ;
  }

  EOS_Internal_Error EOS_Fluid::compute_gamma_ps(double p, double s, double& r) const
  { double r2 ;
    EOS_Internal_Error ierr1, ierr2 ;
    ierr1 = compute_h_ps(p, s, r2) ;
    if (ierr1 == EOS_Internal_Error::OK)
       { ierr2 = compute_gamma_ph(p, r2, r) ;
         return ierr2 ;
       }
    else
       return ierr1 ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_T_d_p_h_ph(double p, double h, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_T_ph(vm, h, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_T_ph(vp, h, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_T_d_h_p_ph(double p, double h, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = h*(1.e0-epsilon);
    double vp = h*(1.e0+epsilon);
    double rm, rp ;
    ierrm = compute_T_ph(p, vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_T_ph(p, vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*h) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_rho_d_p_h_ph(double p, double h, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_rho_ph(vm, h, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_rho_ph(vp, h, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_rho_d_h_p_ph(double p, double h, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = h*(1.e0-epsilon) ;
    double vp = h*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_rho_ph(p, vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_rho_ph(p, vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*h) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_u_d_p_h_ph(double p, double h, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_u_ph(vm, h, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_u_ph(vp, h, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_u_d_h_p_ph(double p, double h, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = h*(1.e0-epsilon) ;
    double vp = h*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_u_ph(p, vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_u_ph(p, vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*h) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_s_d_p_h_ph(double p, double h, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_s_ph(vm, h, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_s_ph(vp, h, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_s_d_h_p_ph(double p, double h, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = h*(1.e0-epsilon) ;
    double vp = h*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_s_ph(p, vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_s_ph(p, vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*h) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_mu_d_p_h_ph(double p, double h, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_mu_ph(vm, h, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_mu_ph(vp, h, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_mu_d_h_p_ph(double p, double h, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = h*(1.e0-epsilon) ;
    double vp = h*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_mu_ph(p, vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_mu_ph(p, vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*h) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_lambda_d_p_h_ph(double p, double h, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_lambda_ph(vm, h, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_lambda_ph(vp, h, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_lambda_d_h_p_ph(double p, double h, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = h*(1.e0-epsilon) ;
    double vp = h*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_lambda_ph(p, vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_lambda_ph(p, vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*h) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cp_d_p_h_ph(double p, double h, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_cp_ph(vm, h, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_cp_ph(vp, h, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cp_d_h_p_ph(double p, double h, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = h*(1.e0-epsilon) ;
    double vp = h*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_cp_ph(p, vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_cp_ph(p, vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*h) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cv_d_p_h_ph(double p, double h, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_cv_ph(vm, h, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_cv_ph(vp, h, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cv_d_h_p_ph(double p, double h, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = h*(1.e0-epsilon) ;
    double vp = h*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_cv_ph(p, vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_cv_ph(p, vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*h) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_sigma_d_p_h_ph(double p, double h, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_sigma_ph(vm, h, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_sigma_ph(vp, h, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_sigma_d_h_p_ph(double p, double h, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = h*(1.e0-epsilon) ;
    double vp = h*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_sigma_ph(p, vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_sigma_ph(p, vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*h) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_w_d_p_h_ph(double p, double h, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_w_ph(vm, h, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_w_ph(vp, h, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_w_d_h_p_ph(double p, double h, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = h*(1.e0-epsilon) ;
    double vp = h*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_w_ph(p, vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_w_ph(p, vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*h) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_g_d_p_h_ph(double p, double h, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_g_ph(vm, h, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_g_ph(vp, h, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_g_d_h_p_ph(double p, double h, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = h*(1.e0-epsilon) ;
    double vp = h*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_g_ph(p, vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_g_ph(p, vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*h) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_f_d_p_h_ph(double p, double h, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_f_ph(vm, h, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_f_ph(vp, h, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_f_d_h_p_ph(double p, double h, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = h*(1.e0-epsilon) ;
    double vp = h*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_f_ph(p, vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_f_ph(p, vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*h) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_pr_d_p_h_ph(double p, double h, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_pr_ph(vm, h, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_pr_ph(vp, h, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_pr_d_h_p_ph(double p, double h, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = h*(1.e0-epsilon) ;
    double vp = h*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_pr_ph(p, vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_pr_ph(p, vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*h) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_beta_d_p_h_ph(double p, double h, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_beta_ph(vm, h, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_beta_ph(vp, h, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_beta_d_h_p_ph(double p, double h, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = h*(1.e0-epsilon) ;
    double vp = h*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_beta_ph(p, vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_beta_ph(p, vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*h) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_gamma_d_p_h_ph(double p, double h, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_gamma_ph(vm, h, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_gamma_ph(vp, h, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_gamma_d_h_p_ph(double p, double h, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = h*(1.e0-epsilon) ;
    double vp = h*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_gamma_ph(p, vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_gamma_ph(p, vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*h) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_h_d_p_T_pT(double p, double T, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_h_pT(vm, T, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_h_pT(vp, T, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_h_d_T_p_pT(double p, double T, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = T*(1.e0-epsilon) ;
    double vp = T*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_h_pT(p, vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_h_pT(p, vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*T) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_rho_d_p_T_pT(double p, double T, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_rho_pT(vm, T, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_rho_pT(vp, T, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_rho_d_T_p_pT(double p, double T, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = T*(1.e0-epsilon) ;
    double vp = T*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_rho_pT(p, vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_rho_pT(p, vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*T) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_u_d_p_T_pT(double p, double T, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_u_pT(vm, T, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_u_pT(vp, T, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_u_d_T_p_pT(double p, double T, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = T*(1.e0-epsilon) ;
    double vp = T*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_u_pT(p, vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_u_pT(p, vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*T) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_s_d_p_T_pT(double p, double T, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_s_pT(vm, T, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_s_pT(vp, T, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_s_d_T_p_pT(double p, double T, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = T*(1.e0-epsilon) ;
    double vp = T*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_s_pT(p, vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_s_pT(p, vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*T) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_mu_d_p_T_pT(double p, double T, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_mu_pT(vm, T, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_mu_pT(vp, T, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_mu_d_T_p_pT(double p, double T, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = T*(1.e0-epsilon) ;
    double vp = T*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_mu_pT(p, vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_mu_pT(p, vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*T) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_lambda_d_p_T_pT(double p, double T, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_lambda_pT(vm, T, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_lambda_pT(vp, T, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_lambda_d_T_p_pT(double p, double T, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = T*(1.e0-epsilon) ;
    double vp = T*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_lambda_pT(p, vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_lambda_pT(p, vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*T) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cp_d_p_T_pT(double p, double T, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_cp_pT(vm, T, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_cp_pT(vp, T, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cp_d_T_p_pT(double p, double T, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = T*(1.e0-epsilon) ;
    double vp = T*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_cp_pT(p, vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_cp_pT(p, vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*T) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cv_d_p_T_pT(double p, double T, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_cv_pT(vm, T, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_cv_pT(vp, T, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cv_d_T_p_pT(double p, double T, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = T*(1.e0-epsilon) ;
    double vp = T*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_cv_pT(p, vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_cv_pT(p, vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*T) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_sigma_d_p_T_pT(double p, double T, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_sigma_pT(vm, T, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_sigma_pT(vp, T, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_sigma_d_T_p_pT(double p, double T, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = T*(1.e0-epsilon) ;
    double vp = T*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_sigma_pT(p, vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_sigma_pT(p, vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*T) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_w_d_p_T_pT(double p, double T, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_w_pT(vm, T, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_w_pT(vp, T, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_w_d_T_p_pT(double p, double T, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = T*(1.e0-epsilon) ;
    double vp = T*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_w_pT(p, vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_w_pT(p, vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*T) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_g_d_p_T_pT(double p, double T, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_g_pT(vm, T, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_g_pT(vp, T, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_g_d_T_p_pT(double p, double T, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = T*(1.e0-epsilon) ;
    double vp = T*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_g_pT(p, vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_g_pT(p, vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*T) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_f_d_p_T_pT(double p, double T, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_f_pT(vm, T, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_f_pT(vp, T, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_f_d_T_p_pT(double p, double T, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = T*(1.e0-epsilon) ;
    double vp = T*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_f_pT(p, vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_f_pT(p, vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*T) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_pr_d_p_T_pT(double p, double T, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_pr_pT(vm, T, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_pr_pT(vp, T, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_pr_d_T_p_pT(double p, double T, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = T*(1.e0-epsilon) ;
    double vp = T*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_pr_pT(p, vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_pr_pT(p, vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*T) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_beta_d_p_T_pT(double p, double T, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_beta_pT(vm, T, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_beta_pT(vp, T, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_beta_d_T_p_pT(double p, double T, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = T*(1.e0-epsilon) ;
    double vp = T*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_beta_pT(p, vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_beta_pT(p, vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*T) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_gamma_d_p_T_pT(double p, double T, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_gamma_pT(vm, T, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_gamma_pT(vp, T, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_gamma_d_T_p_pT(double p, double T, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = T*(1.e0-epsilon) ;
    double vp = T*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_gamma_pT(p, vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_gamma_pT(p, vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*T) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_h_d_p_s_ps(double p, double s, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_h_ps(vm, s, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_h_ps(vp, s, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_h_d_s_p_ps(double p, double s, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = s*(1.e0-epsilon) ;
    double vp = s*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_h_ps(p, vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_h_ps(p, vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*s) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_T_d_p_s_ps(double p, double s, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_T_ps(vm, s, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_T_ps(vp, s, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_T_d_s_p_ps(double p, double s, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = s*(1.e0-epsilon) ;
    double vp = s*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_T_ps(p, vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_T_ps(p, vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*s) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_rho_d_p_s_ps(double p, double s, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_rho_ps(vm, s, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_rho_ps(vp, s, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_rho_d_s_p_ps(double p, double s, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = s*(1.e0-epsilon) ;
    double vp = s*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_rho_ps(p, vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_rho_ps(p, vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*s) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_u_d_p_s_ps(double p, double s, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_u_ps(vm, s, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_u_ps(vp, s, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_u_d_s_p_ps(double p, double s, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = s*(1.e0-epsilon) ;
    double vp = s*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_u_ps(p, vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_u_ps(p, vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*s) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_mu_d_p_s_ps(double p, double s, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_mu_ps(vm, s, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_mu_ps(vp, s, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_mu_d_s_p_ps(double p, double s, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = s*(1.e0-epsilon) ;
    double vp = s*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_mu_ps(p, vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_mu_ps(p, vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*s) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_lambda_d_p_s_ps(double p, double s, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_lambda_ps(vm, s, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_lambda_ps(vp, s, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_lambda_d_s_p_ps(double p, double s, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = s*(1.e0-epsilon) ;
    double vp = s*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_lambda_ps(p, vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_lambda_ps(p, vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*s) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cp_d_p_s_ps(double p, double s, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_cp_ps(vm, s, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_cp_ps(vp, s, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cp_d_s_p_ps(double p, double s, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = s*(1.e0-epsilon) ;
    double vp = s*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_cp_ps(p, vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_cp_ps(p, vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*s) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cv_d_p_s_ps(double p, double s, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_cv_ps(vm, s, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_cv_ps(vp, s, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cv_d_s_p_ps(double p, double s, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = s*(1.e0-epsilon) ;
    double vp = s*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_cv_ps(p, vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_cv_ps(p, vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*s) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_sigma_d_p_s_ps(double p, double s, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_sigma_ps(vm, s, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_sigma_ps(vp, s, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_sigma_d_s_p_ps(double p, double s, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = s*(1.e0-epsilon) ;
    double vp = s*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_sigma_ps(p, vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_sigma_ps(p, vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*s) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_w_d_p_s_ps(double p, double s, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_w_ps(vm, s, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_w_ps(vp, s, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_w_d_s_p_ps(double p, double s, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = s*(1.e0-epsilon) ;
    double vp = s*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_w_ps(p, vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_w_ps(p, vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*s) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_g_d_p_s_ps(double p, double s, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_g_ps(vm, s, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_g_ps(vp, s, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_g_d_s_p_ps(double p, double s, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = s*(1.e0-epsilon) ;
    double vp = s*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_g_ps(p, vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_g_ps(p, vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*s) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_f_d_p_s_ps(double p, double s, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_f_ps(vm, s, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_f_ps(vp, s, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_f_d_s_p_ps(double p, double s, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = s*(1.e0-epsilon) ;
    double vp = s*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_f_ps(p, vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_f_ps(p, vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*s) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_pr_d_p_s_ps(double p, double s, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_pr_ps(vm, s, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_pr_ps(vp, s, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_pr_d_s_p_ps(double p, double s, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = s*(1.e0-epsilon) ;
    double vp = s*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_pr_ps(p, vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_pr_ps(p, vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*s) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_beta_d_p_s_ps(double p, double s, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_beta_ps(vm, s, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_beta_ps(vp, s, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_beta_d_s_p_ps(double p, double s, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = s*(1.e0-epsilon) ;
    double vp = s*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_beta_ps(p, vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_beta_ps(p, vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*s) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_gamma_d_p_s_ps(double p, double s, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_gamma_ps(vm, s, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_gamma_ps(vp, s, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_gamma_d_s_p_ps(double p, double s, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = s*(1.e0-epsilon) ;
    double vp = s*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_gamma_ps(p, vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_gamma_ps(p, vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*s) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_T_sat_p(double p, double& r) const
  { static int loop = 0 ;
  #ifdef _OPENMP
  #pragma omp threadprivate(loop)
  #endif
    if (!loop)
       { loop = 1 ;
         const EOS_Field fin("p", "p",NEPTUNE::p, 1, &p) ;
         EOS_Field fout("T_sat", "T_sat",NEPTUNE::T_sat, 1, &r) ;
         ArrOfInt tmp(1) ;
         EOS_Error_Field ferr(tmp) ;
         compute(fin, fout, ferr) ;
         loop = 0 ;
         return ferr[0] ;
       }
    loop = 0 ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_rho_l_sat_p(double p, double& r) const
  { static int loop = 0 ;
  #ifdef _OPENMP
  #pragma omp threadprivate(loop)
  #endif
    if (!loop)
       { loop = 1 ;
         const EOS_Field fin("p", "p",NEPTUNE::p, 1, &p) ;
         EOS_Field fout("rho_l_sat", "rho_l_sat",NEPTUNE::rho_l_sat, 1, &r) ;
         ArrOfInt tmp(1) ;
         EOS_Error_Field ferr(tmp) ;
         compute(fin, fout, ferr) ;
         loop = 0 ;
         return ferr[0] ;
       }
    loop = 0 ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_rho_v_sat_p(double p, double& r) const
  { static int loop = 0 ;
  #ifdef _OPENMP
  #pragma omp threadprivate(loop)
  #endif
    if (!loop)
       { loop = 1 ;
         const EOS_Field fin("p", "p",NEPTUNE::p, 1, &p) ;
         EOS_Field fout("rho_v_sat", "rho_v_sat",NEPTUNE::rho_v_sat, 1, &r) ;
         ArrOfInt tmp(1) ;
         EOS_Error_Field ferr(tmp) ;
         compute(fin, fout, ferr) ;
         loop = 0 ;
         return ferr[0] ;
       }
    loop = 0 ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_h_l_sat_p(double p, double& r) const
  { static int loop = 0 ;
  #ifdef _OPENMP
  #pragma omp threadprivate(loop)
  #endif
    if (!loop)
       { loop = 1 ;
         const EOS_Field fin("p", "p",NEPTUNE::p, 1, &p) ;
         EOS_Field fout("h_l_sat", "h_l_sat",NEPTUNE::h_l_sat, 1, &r) ;
         ArrOfInt tmp(1) ;
         EOS_Error_Field ferr(tmp) ;
         compute(fin, fout, ferr) ;
         loop = 0 ;
         return ferr[0] ;
       }
    loop = 0 ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_h_v_sat_p(double p, double& r) const
  { static int loop = 0 ;
  #ifdef _OPENMP
  #pragma omp threadprivate(loop)
  #endif
    if (!loop)
       { loop = 1 ;
         const EOS_Field fin("p", "p",NEPTUNE::p, 1, &p) ;
         EOS_Field fout("h_v_sat", "h_v_sat",NEPTUNE::h_v_sat, 1, &r) ;
         ArrOfInt tmp(1) ;
         EOS_Error_Field ferr(tmp) ;
         compute(fin, fout, ferr) ;
         loop = 0 ;
         return ferr[0] ;
       }
    loop = 0 ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_cp_l_sat_p(double p, double& r) const
  { static int loop = 0 ;
  #ifdef _OPENMP
  #pragma omp threadprivate(loop)
  #endif
    if (!loop)
       { loop = 1 ;
         const EOS_Field fin("p", "p",NEPTUNE::p, 1, &p) ;
         EOS_Field fout("cp_l_sat", "cp_l_sat",NEPTUNE::cp_l_sat, 1, &r) ;
         ArrOfInt tmp(1) ;
         EOS_Error_Field ferr(tmp) ;
         compute(fin, fout, ferr) ;
         loop = 0 ;
         return ferr[0] ;
       }
    loop = 0 ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_cp_v_sat_p(double p, double& r) const
  { static int loop = 0 ;
  #ifdef _OPENMP
  #pragma omp threadprivate(loop)
  #endif
    if (!loop)
       { loop = 1 ;
         const EOS_Field fin("p", "p",NEPTUNE::p, 1, &p) ;
         EOS_Field fout("cp_v_sat", "cp_v_sat",NEPTUNE::cp_v_sat, 1, &r) ;
         ArrOfInt tmp(1) ;
         EOS_Error_Field ferr(tmp) ;
         compute(fin, fout, ferr) ;
         loop = 0 ;
         return ferr[0] ;
       }
    loop = 0 ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_rho_l_sat_T(double T, double& r) const
  { static int loop = 0 ;
  #ifdef _OPENMP
  #pragma omp threadprivate(loop)
  #endif
    if (!loop)
       { loop = 1 ;
         const EOS_Field fin("T", "T",NEPTUNE::T, 1, &T) ;
         EOS_Field fout("rho_l_sat", "rho_l_sat",NEPTUNE::rho_l_sat, 1, &r) ;
         ArrOfInt tmp(1) ;
         EOS_Error_Field ferr(tmp) ;
         compute(fin, fout, ferr) ;
         loop = 0 ;
         return ferr[0] ;
       }
    loop = 0 ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_rho_v_sat_T(double T, double& r) const
  { static int loop = 0 ;
  #ifdef _OPENMP
  #pragma omp threadprivate(loop)
  #endif
    if (!loop)
       { loop = 1 ;
         const EOS_Field fin("T", "T",NEPTUNE::T, 1, &T) ;
         EOS_Field fout("rho_v_sat", "rho_v_sat",NEPTUNE::rho_v_sat, 1, &r) ;
         ArrOfInt tmp(1) ;
         EOS_Error_Field ferr(tmp) ;
         compute(fin, fout, ferr) ;
         loop = 0 ;
         return ferr[0] ;
       }
    loop = 0 ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_h_l_sat_T(double T, double& r) const
  { static int loop = 0 ;
  #ifdef _OPENMP
  #pragma omp threadprivate(loop)
  #endif
    if (!loop)
       { loop = 1 ;
         const EOS_Field fin("T", "T",NEPTUNE::T, 1, &T) ;
         EOS_Field fout("h_l_sat", "h_l_sat",NEPTUNE::h_l_sat, 1, &r) ;
         ArrOfInt tmp(1) ;
         EOS_Error_Field ferr(tmp) ;
         compute(fin, fout, ferr) ;
         loop = 0 ;
         return ferr[0] ;
       }
    loop = 0 ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_h_v_sat_T(double T, double& r) const
  { static int loop = 0 ;
  #ifdef _OPENMP
  #pragma omp threadprivate(loop)
  #endif
    if (!loop)
       { loop = 1 ;
         const EOS_Field fin("T", "T",NEPTUNE::T, 1, &T) ;
         EOS_Field fout("h_v_sat", "h_v_sat",NEPTUNE::h_v_sat, 1, &r) ;
         ArrOfInt tmp(1) ;
         EOS_Error_Field ferr(tmp) ;
         compute(fin, fout, ferr) ;
         loop = 0 ;
         return ferr[0] ;
       }
    loop = 0 ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_cp_l_sat_T(double T, double& r) const
  { static int loop = 0 ;
  #ifdef _OPENMP
  #pragma omp threadprivate(loop)
  #endif
    if (!loop)
       { loop = 1 ;
         const EOS_Field fin("T", "T",NEPTUNE::T, 1, &T) ;
         EOS_Field fout("cp_l_sat", "cp_l_sat",NEPTUNE::cp_l_sat, 1, &r) ;
         ArrOfInt tmp(1) ;
         EOS_Error_Field ferr(tmp) ;
         compute(fin, fout, ferr) ;
         loop = 0 ;
         return ferr[0] ;
       }
    loop = 0 ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_cp_v_sat_T(double T, double& r) const
  { static int loop = 0 ;
  #ifdef _OPENMP
  #pragma omp threadprivate(loop)
  #endif
    if (!loop)
       { loop = 1 ;
         const EOS_Field fin("T", "T",NEPTUNE::T, 1, &T) ;
         EOS_Field fout("cp_v_sat", "cp_v_sat",NEPTUNE::cp_v_sat, 1, &r) ;
         ArrOfInt tmp(1) ;
         EOS_Error_Field ferr(tmp) ;
         compute(fin, fout, ferr) ;
         loop = 0 ;
         return ferr[0] ;
       }
    loop = 0 ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_T_sat_d_p_p(double p, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_T_sat_p(vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_T_sat_p(vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_rho_l_sat_d_p_p(double p, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_rho_l_sat_p(vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_rho_l_sat_p(vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_rho_v_sat_d_p_p(double p, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_rho_v_sat_p(vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_rho_v_sat_p(vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_h_l_sat_d_p_p(double p, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_h_l_sat_p(vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_h_l_sat_p(vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_h_v_sat_d_p_p(double p, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_h_v_sat_p(vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_h_v_sat_p(vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cp_l_sat_d_p_p(double p, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_cp_l_sat_p(vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_cp_l_sat_p(vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cp_v_sat_d_p_p(double p, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_cp_v_sat_p(vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_cp_v_sat_p(vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_p_sat_d_T_T(double T, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (T == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = T*(1.e0-epsilon) ;
    double vp = T*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_p_sat_T(vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_p_sat_T(vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*T) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_rho_l_sat_d_T_T(double T, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (T == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = T*(1.e0-epsilon) ;
    double vp = T*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_rho_l_sat_T(vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_rho_l_sat_T(vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*T) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_rho_v_sat_d_T_T(double T, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (T == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = T*(1.e0-epsilon) ;
    double vp = T*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_rho_v_sat_T(vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_rho_v_sat_T(vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*T) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_h_l_sat_d_T_T(double T, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (T == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = T*(1.e0-epsilon) ;
    double vp = T*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_h_l_sat_T(vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_h_l_sat_T(vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*T) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_h_v_sat_d_T_T(double T, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (T == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = T*(1.e0-epsilon) ;
    double vp = T*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_h_v_sat_T(vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_h_v_sat_T(vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*T) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cp_l_sat_d_T_T(double T, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (T == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = T*(1.e0-epsilon) ;
    double vp = T*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_cp_l_sat_T(vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_cp_l_sat_T(vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*T) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cp_v_sat_d_T_T(double T, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (T == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = T*(1.e0-epsilon) ;
    double vp = T*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_cp_v_sat_T(vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_cp_v_sat_T(vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*T) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d2_T_sat_d_p_d_p_p(double p, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_d_T_sat_d_p_p(vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_d_T_sat_d_p_p(vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d2_rho_l_sat_d_p_d_p_p(double p, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_d_rho_l_sat_d_p_p(vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_d_rho_l_sat_d_p_p(vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d2_rho_v_sat_d_p_d_p_p(double p, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_d_rho_v_sat_d_p_p(vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_d_rho_v_sat_d_p_p(vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d2_h_l_sat_d_p_d_p_p(double p, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_d_h_l_sat_d_p_p(vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_d_h_l_sat_d_p_p(vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d2_h_v_sat_d_p_d_p_p(double p, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_d_h_v_sat_d_p_p(vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_d_h_v_sat_d_p_p(vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d2_cp_l_sat_d_p_d_p_p(double p, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_d_cp_l_sat_d_p_p(vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_d_cp_l_sat_d_p_p(vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d2_cp_v_sat_d_p_d_p_p(double p, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_d_cp_v_sat_d_p_p(vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_d_cp_v_sat_d_p_p(vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d2_p_sat_d_T_d_T_T(double T, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (T == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = T*(1.e0-epsilon) ;
    double vp = T*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_d_p_sat_d_T_T(vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_d_p_sat_d_T_T(vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*T) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d2_rho_l_sat_d_T_d_T_T(double T, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (T == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = T*(1.e0-epsilon) ;
    double vp = T*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_d_rho_l_sat_d_T_T(vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_d_rho_l_sat_d_T_T(vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*T) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d2_rho_v_sat_d_T_d_T_T(double T, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (T == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = T*(1.e0-epsilon) ;
    double vp = T*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_d_rho_v_sat_d_T_T(vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_d_rho_v_sat_d_T_T(vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*T) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d2_h_l_sat_d_T_d_T_T(double T, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (T == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = T*(1.e0-epsilon) ;
    double vp = T*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_d_h_l_sat_d_T_T(vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_d_h_l_sat_d_T_T(vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*T) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d2_h_v_sat_d_T_d_T_T(double T, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (T == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = T*(1.e0-epsilon) ;
    double vp = T*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_d_h_v_sat_d_T_T(vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_d_h_v_sat_d_T_T(vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*T) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d2_cp_l_sat_d_T_d_T_T(double T, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (T == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = T*(1.e0-epsilon) ;
    double vp = T*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_d_cp_l_sat_d_T_T(vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_d_cp_l_sat_d_T_T(vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*T) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d2_cp_v_sat_d_T_d_T_T(double T, double& r) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (T == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = T*(1.e0-epsilon) ;
    double vp = T*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_d_cp_v_sat_d_T_T(vm, rm) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_d_cp_v_sat_d_T_T(vp, rp) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*T) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_h_l_lim_p(double p, double& r) const
  { cerr << " *** eos_fluid *** Not_implemented compute_h_l_lim_p,  p = " << p << "  r = " << r << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_h_v_lim_p(double p, double& r) const
  { cerr << " *** eos_fluid *** Not_implemented compute_h_v_lim_p,  p = " << p << "  r = " << r << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_h_l_lim_d_p_p(double p, double& r) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_h_l_lim_d_p_p,  p = " << p << "  r = " << r << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_h_v_lim_d_p_p(double p, double& r) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_h_v_lim_d_p_p,  p = " << p << "  r = " << r << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d2_h_l_lim_d_p_d_p_p(double p, double& r) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d2_h_l_lim_d_p_d_p_p,  p = " << p << "  r = " << r << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d2_h_v_lim_d_p_d_p_p(double p, double& r) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d2_h_v_lim_d_p_d_p_p,  p = " << p << "  r = " << r << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_T_ph(double p, double h, double& r, double c_0,
                 double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_T_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_rho_ph(double p, double h, double& r, double c_0,
                 double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_rho_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_u_ph(double p, double h, double& r, double c_0,
                 double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_u_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_s_ph(double p, double h, double& r, double c_0,
                 double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_s_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_mu_ph(double p, double h, double& r, double c_0,
                 double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_mu_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_lambda_ph(double p, double h, double& r, double c_0,
                 double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_lambda_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_cp_ph(double p, double h, double& r, double c_0,
                 double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_cp_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_cv_ph(double p, double h, double& r, double c_0,
                 double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_cv_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_sigma_ph(double p, double h, double& r, double c_0,
                 double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_sigma_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_w_ph(double p, double h, double& r, double c_0,
                 double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_w_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_g_ph(double p, double h, double& r, double c_0,
                 double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_g_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_f_ph(double p, double h, double& r, double c_0,
                 double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_f_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_pr_ph(double p, double h, double& r, double c_0,
                 double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_pr_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_beta_ph(double p, double h, double& r, double c_0,
                 double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_beta_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_gamma_ph(double p, double h, double& r, double c_0,
                 double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_gamma_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_h_pT(double p, double T, double& r, double c_0,
                 double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_h_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_rho_pT(double p, double T, double& r, double c_0,
                 double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_rho_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_u_pT(double p, double T, double& r, double c_0,
                 double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_u_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_s_pT(double p, double T, double& r, double c_0,
                 double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_s_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_mu_pT(double p, double T, double& r, double c_0,
                 double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_mu_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_lambda_pT(double p, double T, double& r, double c_0,
                 double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_lambda_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_cp_pT(double p, double T, double& r, double c_0,
                 double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_cp_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_cv_pT(double p, double T, double& r, double c_0,
                 double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_cv_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_sigma_pT(double p, double T, double& r, double c_0,
                 double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_sigma_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_w_pT(double p, double T, double& r, double c_0,
                 double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_w_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_g_pT(double p, double T, double& r, double c_0,
                 double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_g_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_f_pT(double p, double T, double& r, double c_0,
                 double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_f_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_pr_pT(double p, double T, double& r, double c_0,
                 double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_pr_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_beta_pT(double p, double T, double& r, double c_0,
                 double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_beta_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_gamma_pT(double p, double T, double& r, double c_0,
                 double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_gamma_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_h_ps(double p, double s, double& r, double c_0,
                 double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_h_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_T_ps(double p, double s, double& r, double c_0,
                 double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_T_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_rho_ps(double p, double s, double& r, double c_0,
                 double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_rho_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_u_ps(double p, double s, double& r, double c_0,
                 double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_u_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_mu_ps(double p, double s, double& r, double c_0,
                 double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_mu_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_lambda_ps(double p, double s, double& r, double c_0,
                 double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_lambda_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_cp_ps(double p, double s, double& r, double c_0,
                 double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_cp_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_cv_ps(double p, double s, double& r, double c_0,
                 double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_cv_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_sigma_ps(double p, double s, double& r, double c_0,
                 double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_sigma_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_w_ps(double p, double s, double& r, double c_0,
                 double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_w_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_g_ps(double p, double s, double& r, double c_0,
                 double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_g_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_f_ps(double p, double s, double& r, double c_0,
                 double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_f_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_pr_ps(double p, double s, double& r, double c_0,
                 double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_pr_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_beta_ps(double p, double s, double& r, double c_0,
                 double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_beta_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_gamma_ps(double p, double s, double& r, double c_0,
                 double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_gamma_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_T_d_p_h_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_T_ph(vm, h, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_T_ph(vp, h, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_T_d_h_p_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = h*(1.e0-epsilon) ;
    double vp = h*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_T_ph(p, vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_T_ph(p, vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*h) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_T_d_c_0_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_T_d_c_0_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_T_d_c_1_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_T_d_c_1_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_T_d_c_2_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_T_d_c_2_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_T_d_c_3_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_T_d_c_3_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_T_d_c_4_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_T_d_c_4_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_rho_d_p_h_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_rho_ph(vm, h, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_rho_ph(vp, h, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_rho_d_h_p_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = h*(1.e0-epsilon) ;
    double vp = h*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_rho_ph(p, vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_rho_ph(p, vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*h) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_rho_d_c_0_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_rho_d_c_0_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_rho_d_c_1_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_rho_d_c_1_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_rho_d_c_2_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_rho_d_c_2_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_rho_d_c_3_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_rho_d_c_3_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_rho_d_c_4_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_rho_d_c_4_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_u_d_p_h_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_u_ph(vm, h, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_u_ph(vp, h, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_u_d_h_p_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = h*(1.e0-epsilon) ;
    double vp = h*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_u_ph(p, vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_u_ph(p, vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*h) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_u_d_c_0_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_u_d_c_0_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_u_d_c_1_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_u_d_c_1_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_u_d_c_2_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_u_d_c_2_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_u_d_c_3_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_u_d_c_3_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_u_d_c_4_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_u_d_c_4_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_s_d_p_h_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_s_ph(vm, h, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_s_ph(vp, h, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_s_d_h_p_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = h*(1.e0-epsilon) ;
    double vp = h*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_s_ph(p, vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_s_ph(p, vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*h) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_s_d_c_0_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_s_d_c_0_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_s_d_c_1_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_s_d_c_1_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_s_d_c_2_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_s_d_c_2_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_s_d_c_3_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_s_d_c_3_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_s_d_c_4_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_s_d_c_4_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_mu_d_p_h_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_mu_ph(vm, h, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_mu_ph(vp, h, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_mu_d_h_p_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = h*(1.e0-epsilon) ;
    double vp = h*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_mu_ph(p, vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_mu_ph(p, vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*h) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_mu_d_c_0_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_mu_d_c_0_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_mu_d_c_1_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_mu_d_c_1_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_mu_d_c_2_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_mu_d_c_2_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_mu_d_c_3_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_mu_d_c_3_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_mu_d_c_4_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_mu_d_c_4_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_lambda_d_p_h_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_lambda_ph(vm, h, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_lambda_ph(vp, h, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_lambda_d_h_p_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = h*(1.e0-epsilon) ;
    double vp = h*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_lambda_ph(p, vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_lambda_ph(p, vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*h) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_lambda_d_c_0_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_lambda_d_c_0_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_lambda_d_c_1_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_lambda_d_c_1_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_lambda_d_c_2_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_lambda_d_c_2_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_lambda_d_c_3_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_lambda_d_c_3_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_lambda_d_c_4_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_lambda_d_c_4_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cp_d_p_h_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_cp_ph(vm, h, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_cp_ph(vp, h, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cp_d_h_p_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = h*(1.e0-epsilon) ;
    double vp = h*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_cp_ph(p, vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_cp_ph(p, vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*h) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cp_d_c_0_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_cp_d_c_0_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cp_d_c_1_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_cp_d_c_1_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cp_d_c_2_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_cp_d_c_2_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cp_d_c_3_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_cp_d_c_3_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cp_d_c_4_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_cp_d_c_4_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cv_d_p_h_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_cv_ph(vm, h, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_cv_ph(vp, h, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cv_d_h_p_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = h*(1.e0-epsilon) ;
    double vp = h*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_cv_ph(p, vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_cv_ph(p, vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*h) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cv_d_c_0_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_cv_d_c_0_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cv_d_c_1_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_cv_d_c_1_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cv_d_c_2_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_cv_d_c_2_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cv_d_c_3_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_cv_d_c_3_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cv_d_c_4_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_cv_d_c_4_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_sigma_d_p_h_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_sigma_ph(vm, h, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_sigma_ph(vp, h, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_sigma_d_h_p_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = h*(1.e0-epsilon) ;
    double vp = h*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_sigma_ph(p, vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_sigma_ph(p, vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*h) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_sigma_d_c_0_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_sigma_d_c_0_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_sigma_d_c_1_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_sigma_d_c_1_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_sigma_d_c_2_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_sigma_d_c_2_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_sigma_d_c_3_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_sigma_d_c_3_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_sigma_d_c_4_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_sigma_d_c_4_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_w_d_p_h_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_w_ph(vm, h, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_w_ph(vp, h, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_w_d_h_p_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = h*(1.e0-epsilon) ;
    double vp = h*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_w_ph(p, vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_w_ph(p, vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*h) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_w_d_c_0_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_compute_d_w_d_c_0_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_w_d_c_1_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_w_d_c_1_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_w_d_c_2_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_w_d_c_2_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_w_d_c_3_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_w_d_c_3_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_w_d_c_4_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_w_d_c_4_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_g_d_p_h_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_g_ph(vm, h, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_g_ph(vp, h, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_g_d_h_p_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = h*(1.e0-epsilon) ;
    double vp = h*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_g_ph(p, vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_g_ph(p, vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*h) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_g_d_c_0_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_g_d_c_0_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_g_d_c_1_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_g_d_c_1_ph_phcompute_d_w_d_c_4_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_g_d_c_2_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_g_d_c_2_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_g_d_c_3_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_g_d_c_3_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_g_d_c_4_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_g_d_c_4_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_f_d_p_h_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_f_ph(vm, h, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_f_ph(vp, h, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_f_d_h_p_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = h*(1.e0-epsilon) ;
    double vp = h*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_f_ph(p, vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_f_ph(p, vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*h) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_f_d_c_0_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_f_d_c_0_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_f_d_c_1_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_f_d_c_1_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_f_d_c_2_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_f_d_c_2_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_f_d_c_3_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_f_d_c_3_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_f_d_c_4_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_f_d_c_4_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_pr_d_p_h_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_pr_ph(vm, h, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_pr_ph(vp, h, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_pr_d_h_p_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = h*(1.e0-epsilon) ;
    double vp = h*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_pr_ph(p, vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_pr_ph(p, vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*h) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_pr_d_c_0_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_pr_d_c_0_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_pr_d_c_1_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_pr_d_c_1_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_pr_d_c_2_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_pr_d_c_2_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_pr_d_c_3_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_pr_d_c_3_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_pr_d_c_4_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_pr_d_c_4_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_beta_d_p_h_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_beta_ph(vm, h, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_beta_ph(vp, h, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_beta_d_h_p_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = h*(1.e0-epsilon) ;
    double vp = h*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_beta_ph(p, vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_beta_ph(p, vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*h) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_beta_d_c_0_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_beta_d_c_0_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_beta_d_c_1_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_beta_d_c_1_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_beta_d_c_2_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_beta_d_c_2_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_beta_d_c_3_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_beta_d_c_3_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_beta_d_c_4_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_beta_d_c_4_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_gamma_d_p_h_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_gamma_ph(vm, h, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_gamma_ph(vp, h, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_gamma_d_h_p_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = h*(1.e0-epsilon) ;
    double vp = h*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_gamma_ph(p, vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_gamma_ph(p, vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*h) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_gamma_d_c_0_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_gamma_d_c_0_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_gamma_d_c_1_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_gamma_d_c_1_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_gamma_d_c_2_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_gamma_d_c_2_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_gamma_d_c_3_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_gamma_d_c_3_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }


  EOS_Internal_Error EOS_Fluid::compute_d_gamma_d_c_4_ph_ph(double p, double h, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_gamma_d_c_4_ph_ph,  p = " << p << "  h = " << h << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_h_d_p_T_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_h_pT(vm, T, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_h_pT(vp, T, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_h_d_T_p_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = T*(1.e0-epsilon) ;
    double vp = T*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_h_pT(p, vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_h_pT(p, vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*T) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_h_d_c_0_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_h_d_c_0_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_h_d_c_1_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_h_d_c_1_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_h_d_c_2_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_h_d_c_2_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_h_d_c_3_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_h_d_c_3_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_h_d_c_4_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_h_d_c_4_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_rho_d_p_T_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_rho_pT(vm, T, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_rho_pT(vp, T, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_rho_d_T_p_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = T*(1.e0-epsilon) ;
    double vp = T*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_rho_pT(p, vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_rho_pT(p, vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*T) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_rho_d_c_0_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_rho_d_c_0_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_rho_d_c_1_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_rho_d_c_1_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_rho_d_c_2_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_rho_d_c_2_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_rho_d_c_3_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_rho_d_c_3_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_rho_d_c_4_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_rho_d_c_4_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_u_d_p_T_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_u_pT(vm, T, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_u_pT(vp, T, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_u_d_T_p_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = T*(1.e0-epsilon) ;
    double vp = T*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_u_pT(p, vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_u_pT(p, vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*T) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_u_d_c_0_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_u_d_c_0_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_u_d_c_1_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_u_d_c_1_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_u_d_c_2_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_u_d_c_2_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_u_d_c_3_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_u_d_c_3_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_u_d_c_4_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_u_d_c_4_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_s_d_p_T_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_s_pT(vm, T, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_s_pT(vp, T, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_s_d_T_p_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = T*(1.e0-epsilon) ;
    double vp = T*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_s_pT(p, vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_s_pT(p, vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*T) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_s_d_c_0_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_s_d_c_0_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_s_d_c_1_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_s_d_c_1_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_s_d_c_2_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_s_d_c_2_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_s_d_c_3_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_s_d_c_3_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_s_d_c_4_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_s_d_c_4_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_mu_d_p_T_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_mu_pT(vm, T, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_mu_pT(vp, T, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_mu_d_T_p_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = T*(1.e0-epsilon) ;
    double vp = T*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_mu_pT(p, vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_mu_pT(p, vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*T) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_mu_d_c_0_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_mu_d_c_0_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_mu_d_c_1_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_mu_d_c_1_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_mu_d_c_2_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_mu_d_c_2_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_mu_d_c_3_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_mu_d_c_3_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_mu_d_c_4_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_mu_d_c_4_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_lambda_d_p_T_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_lambda_pT(vm, T, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_lambda_pT(vp, T, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_lambda_d_T_p_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = T*(1.e0-epsilon) ;
    double vp = T*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_lambda_pT(p, vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_lambda_pT(p, vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*T) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_lambda_d_c_0_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_lambda_d_c_0_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_lambda_d_c_1_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_lambda_d_c_1_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_lambda_d_c_2_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_lambda_d_c_2_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_lambda_d_c_3_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_lambda_d_c_3_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_lambda_d_c_4_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_lambda_d_c_4_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cp_d_p_T_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_cp_pT(vm, T, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_cp_pT(vp, T, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cp_d_T_p_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = T*(1.e0-epsilon) ;
    double vp = T*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_cp_pT(p, vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_cp_pT(p, vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*T) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cp_d_c_0_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_cp_d_c_0_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cp_d_c_1_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_cp_d_c_1_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cp_d_c_2_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_cp_d_c_2_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cp_d_c_3_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_cp_d_c_3_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cp_d_c_4_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_cp_d_c_4_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cv_d_p_T_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_cv_pT(vm, T, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_cv_pT(vp, T, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cv_d_T_p_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = T*(1.e0-epsilon) ;
    double vp = T*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_cv_pT(p, vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_cv_pT(p, vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*T) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cv_d_c_0_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_compute_d_cv_d_c_0_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cv_d_c_1_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_cv_d_c_1_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cv_d_c_2_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_cv_d_c_2_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cv_d_c_3_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_cv_d_c_3_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cv_d_c_4_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_cv_d_c_4_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_sigma_d_p_T_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_sigma_pT(vm, T, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_sigma_pT(vp, T, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_sigma_d_T_p_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = T*(1.e0-epsilon) ;
    double vp = T*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_sigma_pT(p, vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_sigma_pT(p, vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*T) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_sigma_d_c_0_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_sigma_d_c_0_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_sigma_d_c_1_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_sigma_d_c_1_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_sigma_d_c_2_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_sigma_d_c_2_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_sigma_d_c_3_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_sigma_d_c_3_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_sigma_d_c_4_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_sigma_d_c_4_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_w_d_p_T_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_w_pT(vm, T, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_w_pT(vp, T, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_w_d_T_p_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = T*(1.e0-epsilon) ;
    double vp = T*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_w_pT(p, vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_w_pT(p, vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*T) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_w_d_c_0_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_w_d_c_0_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_w_d_c_1_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_w_d_c_1_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_w_d_c_2_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_w_d_c_2_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_w_d_c_3_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_w_d_c_3_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_w_d_c_4_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_w_d_c_4_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_g_d_p_T_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_g_pT(vm, T, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_g_pT(vp, T, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_g_d_T_p_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = T*(1.e0-epsilon) ;
    double vp = T*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_g_pT(p, vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_g_pT(p, vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*T) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_g_d_c_0_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_g_d_c_0_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_g_d_c_1_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_g_d_c_1_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_g_d_c_2_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_g_d_c_2_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_g_d_c_3_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_g_d_c_3_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_g_d_c_4_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_g_d_c_4_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_f_d_p_T_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_f_pT(vm, T, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_f_pT(vp, T, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_f_d_T_p_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = T*(1.e0-epsilon) ;
    double vp = T*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_f_pT(p, vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_f_pT(p, vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*T) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_f_d_c_0_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_f_d_c_0_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_f_d_c_1_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_f_d_c_1_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_f_d_c_2_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_f_d_c_2_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_f_d_c_3_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_f_d_c_3_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_f_d_c_4_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_f_d_c_4_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_pr_d_p_T_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_pr_pT(vm, T, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_pr_pT(vp, T, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_pr_d_T_p_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = T*(1.e0-epsilon) ;
    double vp = T*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_pr_pT(p, vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_pr_pT(p, vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*T) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_pr_d_c_0_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_pr_d_c_0_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_pr_d_c_1_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_pr_d_c_1_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_pr_d_c_2_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_pr_d_c_2_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_pr_d_c_3_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_pr_d_c_3_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_pr_d_c_4_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_pr_d_c_4_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_beta_d_p_T_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_beta_pT(vm, T, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_beta_pT(vp, T, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_beta_d_T_p_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = T*(1.e0-epsilon) ;
    double vp = T*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_beta_pT(p, vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_beta_pT(p, vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*T) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_beta_d_c_0_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_beta_d_c_0_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_beta_d_c_1_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_beta_d_c_1_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_beta_d_c_2_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_beta_d_c_2_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_beta_d_c_3_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_beta_d_c_3_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_beta_d_c_4_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_pr_d_c_4_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_gamma_d_p_T_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_gamma_pT(vm, T, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_gamma_pT(vp, T, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_gamma_d_T_p_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = T*(1.e0-epsilon) ;
    double vp = T*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_gamma_pT(p, vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_gamma_pT(p, vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*T) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_gamma_d_c_0_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_gamma_d_c_0_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_gamma_d_c_1_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_gamma_d_c_1_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_gamma_d_c_2_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_gamma_d_c_2_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_gamma_d_c_3_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_gamma_d_c_3_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_gamma_d_c_4_pT_pT(double p, double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_gamma_d_c_4_pT_pT,  p = " << p << "  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_h_d_p_s_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_h_ps(vm, s, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_h_ps(vp, s, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_h_d_s_p_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = s*(1.e0-epsilon) ;
    double vp = s*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_h_ps(p, vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_h_ps(p, vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*s) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_h_d_c_0_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_h_d_c_0_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_h_d_c_1_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_h_d_c_1_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_h_d_c_2_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_h_d_c_2_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_h_d_c_3_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_h_d_c_3_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_h_d_c_4_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_h_d_c_4_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_T_d_p_s_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_T_ps(vm, s, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_T_ps(vp, s, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_T_d_s_p_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = s*(1.e0-epsilon) ;
    double vp = s*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_T_ps(p, vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_T_ps(p, vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*s) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_T_d_c_0_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_T_d_c_0_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_T_d_c_1_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_T_d_c_1_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_T_d_c_2_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_T_d_c_2_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_T_d_c_3_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_T_d_c_3_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_T_d_c_4_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_T_d_c_4_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_rho_d_p_s_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_rho_ps(vm, s, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_rho_ps(vp, s, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_rho_d_s_p_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = s*(1.e0-epsilon) ;
    double vp = s*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_rho_ps(p, vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_rho_ps(p, vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*s) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_rho_d_c_0_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_rho_d_c_0_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_rho_d_c_1_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_rho_d_c_1_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_rho_d_c_2_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_rho_d_c_2_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_rho_d_c_3_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_rho_d_c_3_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_rho_d_c_4_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_rho_d_c_4_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_u_d_p_s_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_u_ps(vm, s, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_u_ps(vp, s, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_u_d_s_p_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = s*(1.e0-epsilon) ;
    double vp = s*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_u_ps(p, vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_u_ps(p, vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*s) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_u_d_c_0_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_u_d_c_0_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_u_d_c_1_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_u_d_c_1_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_u_d_c_2_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_u_d_c_2_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_u_d_c_3_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_u_d_c_3_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_u_d_c_4_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_u_d_c_4_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_mu_d_p_s_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_mu_ps(vm, s, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_mu_ps(vp, s, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_mu_d_s_p_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = s*(1.e0-epsilon) ;
    double vp = s*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_mu_ps(p, vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_mu_ps(p, vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*s) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_mu_d_c_0_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_mu_d_c_0_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_mu_d_c_1_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_mu_d_c_1_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_mu_d_c_2_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_mu_d_c_2_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_mu_d_c_3_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_mu_d_c_3_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_mu_d_c_4_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_mu_d_c_4_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_lambda_d_p_s_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_lambda_ps(vm, s, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_lambda_ps(vp, s, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_lambda_d_s_p_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = s*(1.e0-epsilon) ;
    double vp = s*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_lambda_ps(p, vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_lambda_ps(p, vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*s) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_lambda_d_c_0_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_lambda_d_c_0_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_lambda_d_c_1_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_lambda_d_c_1_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_lambda_d_c_2_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_lambda_d_c_2_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_lambda_d_c_3_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_lambda_d_c_3_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_lambda_d_c_4_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_lambda_d_c_4_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cp_d_p_s_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_cp_ps(vm, s, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_cp_ps(vp, s, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cp_d_s_p_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = s*(1.e0-epsilon) ;
    double vp = s*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_cp_ps(p, vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_cp_ps(p, vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*s) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cp_d_c_0_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_cp_d_c_0_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cp_d_c_1_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_cp_d_c_1_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cp_d_c_2_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_cp_d_c_2_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cp_d_c_3_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_cp_d_c_3_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cp_d_c_4_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_cp_d_c_4_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cv_d_p_s_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_cv_ps(vm, s, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_cv_ps(vp, s, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cv_d_s_p_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = s*(1.e0-epsilon) ;
    double vp = s*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_cv_ps(p, vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_cv_ps(p, vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*s) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cv_d_c_0_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_compute_d_cv_d_c_0_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cv_d_c_1_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_cv_d_c_1_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cv_d_c_2_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_cv_d_c_2_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cv_d_c_3_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_cv_d_c_3_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cv_d_c_4_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_cv_d_c_4_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_sigma_d_p_s_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_sigma_ps(vm, s, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_sigma_ps(vp, s, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_sigma_d_s_p_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = s*(1.e0-epsilon) ;
    double vp = s*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_sigma_ps(p, vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_sigma_ps(p, vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*s) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_sigma_d_c_0_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_sigma_d_c_0_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_sigma_d_c_1_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_sigma_d_c_1_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_sigma_d_c_2_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_sigma_d_c_2_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_sigma_d_c_3_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_sigma_d_c_3_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_sigma_d_c_4_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_sigma_d_c_4_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_w_d_p_s_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_w_ps(vm, s, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_w_ps(vp, s, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_w_d_s_p_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = s*(1.e0-epsilon) ;
    double vp = s*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_w_ps(p, vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_w_ps(p, vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*s) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_w_d_c_0_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_w_d_c_0_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_w_d_c_1_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_w_d_c_1_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_w_d_c_2_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_w_d_c_2_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_w_d_c_3_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_w_d_c_3_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_w_d_c_4_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_w_d_c_4_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_g_d_p_s_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_g_ps(vm, s, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_g_ps(vp, s, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_g_d_s_p_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = s*(1.e0-epsilon) ;
    double vp = s*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_g_ps(p, vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_g_ps(p, vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*s) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_g_d_c_0_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_g_d_c_0_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_g_d_c_1_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_g_d_c_1_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_g_d_c_2_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_g_d_c_2_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_g_d_c_3_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_g_d_c_3_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_g_d_c_4_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_g_d_c_4_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_f_d_p_s_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_f_ps(vm, s, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_f_ps(vp, s, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_f_d_s_p_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = s*(1.e0-epsilon) ;
    double vp = s*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_f_ps(p, vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_f_ps(p, vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*s) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_f_d_c_0_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_f_d_c_0_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_f_d_c_1_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_f_d_c_1_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_f_d_c_2_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_f_d_c_2_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_f_d_c_3_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_f_d_c_3_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_f_d_c_4_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_f_d_c_4_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_pr_d_p_s_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_pr_ps(vm, s, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_pr_ps(vp, s, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_pr_d_s_p_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = s*(1.e0-epsilon) ;
    double vp = s*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_pr_ps(p, vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_pr_ps(p, vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*s) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_pr_d_c_0_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_pr_d_c_0_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_pr_d_c_1_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_pr_d_c_1_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_pr_d_c_2_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_pr_d_c_2_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_pr_d_c_3_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_pr_d_c_3_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_pr_d_c_4_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_pr_d_c_4_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_beta_d_p_s_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_beta_ps(vm, s, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_beta_ps(vp, s, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_beta_d_s_p_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = s*(1.e0-epsilon) ;
    double vp = s*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_beta_ps(p, vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_beta_ps(p, vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*s) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_beta_d_c_0_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_beta_d_c_0_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_beta_d_c_1_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_beta_d_c_1_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_beta_d_c_2_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_beta_d_c_2_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_beta_d_c_3_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_beta_d_c_3_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_beta_d_c_4_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_beta_d_c_4_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_gamma_d_p_s_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_gamma_ps(vm, s, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_gamma_ps(vp, s, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_gamma_d_s_p_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = s*(1.e0-epsilon) ;
    double vp = s*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_gamma_ps(p, vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_gamma_ps(p, vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*s) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_gamma_d_c_0_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_gamma_d_c_0_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_gamma_d_c_1_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_gamma_d_c_1_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_gamma_d_c_2_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_gamma_d_c_2_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_gamma_d_c_3_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** compute_d_gamma_d_c_3_ps_ps compute_d_beta_d_c_4_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_gamma_d_c_4_ps_ps(double p, double s, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_d_gamma_d_c_4_ps_ps,  p = " << p << "  s = " << s << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }



  EOS_Internal_Error EOS_Fluid::compute_T_sat_p(double p, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_T_sat_p,  p = " << p << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_rho_l_sat_p(double p, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_rho_l_sat_p,  p = " << p << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_rho_v_sat_p(double p, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_rho_v_sat_p,  p = " << p << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_h_l_sat_p(double p, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_h_l_sat_p,  p = " << p << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_h_v_sat_p(double p, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_h_v_sat_p,  p = " << p << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_cp_l_sat_p(double p, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_cp_l_sat_p,  p = " << p << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_cp_v_sat_p(double p, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_cp_v_sat_p,  p = " << p << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_p_sat_T(double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_p_sat_T,  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_rho_l_sat_T(double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_rho_l_sat_T,  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_rho_v_sat_T(double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_rho_v_sat_T,  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_h_l_sat_T(double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_h_l_sat_T,  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_h_v_sat_T(double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_h_v_sat_T,  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_cp_l_sat_T(double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_cp_l_sat_T,  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_cp_v_sat_T(double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { cerr << " *** eos_fluid *** Not_implemented compute_cp_v_sat_T,  T = " << T << "  r = " << r << endl
         << "                          c_0 = " << c_0 << "  c_1 = " << c_1
                                 << "  c_2 = " << c_2 << "  c_3 = " << c_3
                                 << "  c_4 = " << c_4 << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_T_sat_d_p_p(double p, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_T_sat_p(vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_T_sat_p(vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_rho_l_sat_d_p_p(double p, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_rho_l_sat_p(vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_rho_l_sat_p(vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_rho_v_sat_d_p_p(double p, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_rho_v_sat_p(vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_rho_v_sat_p(vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_h_l_sat_d_p_p(double p, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_h_l_sat_p(vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_h_l_sat_p(vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_h_v_sat_d_p_p(double p, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_h_v_sat_p(vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_h_v_sat_p(vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cp_l_sat_d_p_p(double p, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_cp_l_sat_p(vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_cp_l_sat_p(vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cp_v_sat_d_p_p(double p, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_cp_v_sat_p(vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_cp_v_sat_p(vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_p_sat_d_T_T(double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (T == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = T*(1.e0-epsilon) ;
    double vp = T*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_p_sat_T(vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_p_sat_T(vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*T) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_rho_l_sat_d_T_T(double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (T == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = T*(1.e0-epsilon) ;
    double vp = T*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_rho_l_sat_T(vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_rho_l_sat_T(vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*T) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_rho_v_sat_d_T_T(double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (T == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = T*(1.e0-epsilon) ;
    double vp = T*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_rho_v_sat_T(vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_rho_v_sat_T(vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*T) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_h_l_sat_d_T_T(double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (T == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = T*(1.e0-epsilon) ;
    double vp = T*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_h_l_sat_T(vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_h_l_sat_T(vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*T) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_h_v_sat_d_T_T(double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (T == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = T*(1.e0-epsilon) ;
    double vp = T*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_h_v_sat_T(vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_h_v_sat_T(vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*T) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cp_l_sat_d_T_T(double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (T == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = T*(1.e0-epsilon) ;
    double vp = T*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_cp_l_sat_T(vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_cp_l_sat_T(vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*T) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d_cp_v_sat_d_T_T(double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (T == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = T*(1.e0-epsilon) ;
    double vp = T*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_cp_v_sat_T(vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_cp_v_sat_T(vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*T) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d2_T_sat_d_p_d_p_p(double p, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_d_T_sat_d_p_p(vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_d_T_sat_d_p_p(vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d2_rho_l_sat_d_p_d_p_p(double p, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_d_rho_l_sat_d_p_p(vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_d_rho_l_sat_d_p_p(vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d2_rho_v_sat_d_p_d_p_p(double p, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_d_rho_v_sat_d_p_p(vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_d_rho_v_sat_d_p_p(vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d2_h_l_sat_d_p_d_p_p(double p, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_d_h_l_sat_d_p_p(vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_d_h_l_sat_d_p_p(vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d2_h_v_sat_d_p_d_p_p(double p, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_d_h_v_sat_d_p_p(vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_d_h_v_sat_d_p_p(vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d2_cp_l_sat_d_p_d_p_p(double p, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_d_cp_l_sat_d_p_p(vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_d_cp_l_sat_d_p_p(vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d2_cp_v_sat_d_p_d_p_p(double p, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (p == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = p*(1.e0-epsilon) ;
    double vp = p*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_d_cp_v_sat_d_p_p(vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_d_cp_v_sat_d_p_p(vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*p) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d2_p_sat_d_T_d_T_T(double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (T == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = T*(1.e0-epsilon) ;
    double vp = T*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_d_p_sat_d_T_T(vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_d_p_sat_d_T_T(vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*T) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d2_rho_l_sat_d_T_d_T_T(double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (T == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = T*(1.e0-epsilon) ;
    double vp = T*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_d_rho_l_sat_d_T_T(vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_d_rho_l_sat_d_T_T(vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*T) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d2_rho_v_sat_d_T_d_T_T(double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (T == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = T*(1.e0-epsilon) ;
    double vp = T*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_d_rho_v_sat_d_T_T(vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_d_rho_v_sat_d_T_T(vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*T) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d2_h_l_sat_d_T_d_T_T(double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (T == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = T*(1.e0-epsilon) ;
    double vp = T*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_d_h_l_sat_d_T_T(vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_d_h_l_sat_d_T_T(vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*T) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d2_h_v_sat_d_T_d_T_T(double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (T == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = T*(1.e0-epsilon) ;
    double vp = T*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_d_h_v_sat_d_T_T(vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_d_h_v_sat_d_T_T(vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*T) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d2_cp_l_sat_d_T_d_T_T(double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (T == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = T*(1.e0-epsilon) ;
    double vp = T*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_d_cp_l_sat_d_T_T(vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_d_cp_l_sat_d_T_T(vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*T) ;
         return ierrp ;
       }
    return ierrm ;
  }

  EOS_Internal_Error EOS_Fluid::compute_d2_cp_v_sat_d_T_d_T_T(double T, double& r,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  { EOS_Internal_Error ierrm, ierrp ;
    r = 0.e0 ;
    if (T == 0.e0)  return EOS_Internal_Error::EOS_BAD_COMPUTE ;
    double vm = T*(1.e0-epsilon) ;
    double vp = T*(1.e0+epsilon) ;
    double rm, rp ;
    ierrm = compute_d_cp_v_sat_d_T_T(vm, rm, c_0, c_1, c_2, c_3, c_4) ;
    if (ierrm == EOS_Internal_Error::OK)
       { ierrp = compute_d_cp_v_sat_d_T_T(vp, rp, c_0, c_1, c_2, c_3, c_4) ;
         if (ierrp == EOS_Internal_Error::OK)  r = (0.5e0*(rp-rm)) / (epsilon*T) ;
         return ierrp ;
       }
    return ierrm ;
  }
  // + --- Auto --- +

}
