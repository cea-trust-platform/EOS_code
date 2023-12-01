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



#ifndef EOS_CATHAREINCONDENSABLEGAS_HXX
#define EOS_CATHAREINCONDENSABLEGAS_HXX

#include "system/arch.h"
#include "EOS/API/EOS_Fluid.hxx"
#include "EOS/Src/EOS_PerfectGas/EOS_PerfectGas.hxx"
#include "Language/API/Types_Info.hxx"

using namespace NEPTUNE ;

extern "C" 
  { void F77DECLARE(c2_ninccopy)(const char*) ;
    void F77DECLARE(c2_rproga)( const int&, const int&, double&, double&, double&, double&, double&, double&,
                                double&, double&, double&, double&, double&, double&, double&, double&,
                                double&, double&) ;
  }

namespace NEPTUNE_EOS
{
  //! EOS_CathareIncondensableGas : adapter to Cathare2 Incondensable Gas Tables
  class EOS_CathareIncondensableGas : public EOS_PerfectGas
  {
    static const AString tablename ;

  public:

    virtual const AString& table_name() const;
    
    //! Output stream method: see language documentation
    ostream& print_On (ostream& stream) const;
    //! Input stream method: see language documentation
    istream& read_On (istream& stream);
    //! Type information method: see language documentation
    virtual const Type_Info& get_Type_Info () const;

    //! Main constructor for class EOS_CathareIncondensableGas
    EOS_CathareIncondensableGas();

    //! Copy constructor. Should NOT be used.
    EOS_CathareIncondensableGas(const EOS_CathareIncondensableGas&);

    //! EOS_Cathare2 destructor
    virtual ~EOS_CathareIncondensableGas();

    //! Return the fluid name
    /*!
     * \return the fluid name
     */
    virtual const AString& fluid_name() const;

    //! Initialize the EOS_Fluid object
    /*!
     * args[0] must be a incondensable gas name. Authorized names are :
     * Nitrogen, Hydrogen, Oxygen, Air, Argon, Helium, User
     * All other arguments are ignored except if User is chosen.
     * In that case, init expects 16 additional arguments :
     * prxr, prxcp0, prxcp1, prxcp2, prxcp3, prxcp4, prxcp5, prxcp6,
     * prxm, prxl0, prxl1, prxl2, prxm0, prxm1, prxm2, prxdv
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

    //! Return Cp0
    double get_prxcp();
    //! Return Cp0
    double get_prxcp0();
    //! Return Cp1
    double get_prxcp1();
    //! Return Cp2
    double get_prxcp2();
    //! Return Cp3
    double get_prxcp3();
    //! Return Cp4
    double get_prxcp4();
    //! Return Cp5
    double get_prxcp5();
    //! Return Cp6
    double get_prxcp6();
    //! T(p,h)
    virtual EOS_Internal_Error compute_T_ph(double p, double h, double& T) const;
    //! rho(p,h)
    virtual EOS_Internal_Error compute_rho_ph(double p, double h, double& rho) const;
    //! Cp(p,h)
    virtual EOS_Internal_Error compute_Cp_ph(double p, double h, double& Cp) const;
    //! mu(p,h)
    virtual EOS_Internal_Error compute_mu_ph(double p, double h, double& mu) const;
    //! lambda(p,h)
    virtual EOS_Internal_Error compute_lambda_ph(double p, double h, double& lambda) const;
    //! rho(p,T)
    virtual EOS_Internal_Error compute_rho_pT(double p, double T, double& rho) const;
    //! Cp(p,T)
    virtual EOS_Internal_Error compute_Cp_pT(double p, double T, double& Cp) const;
    //! mu(p,T)
    virtual EOS_Internal_Error compute_mu_pT(double p, double T, double& mu) const;
    //! lambda(p,T)
    virtual EOS_Internal_Error compute_lambda_pT(double p, double T, double& lambda) const;

  protected:
    AString fluidname;         //!< fluid name
    AString cathare_name;        //!< Cathare2-source incondensable gas name
    // Specific Heat coeffs
    double prxcp0, prxcp1, prxcp2, prxcp3, prxcp4, prxcp5, prxcp6;
    int nb_args;

  private:
    //! Type identifier: see language documentation
    static int type_Id ;

  };

  inline double EOS_CathareIncondensableGas::get_prxcp()  { return prxcp0; }
  inline double EOS_CathareIncondensableGas::get_prxcp0() { return prxcp0; }
  inline double EOS_CathareIncondensableGas::get_prxcp1() { return prxcp1; }
  inline double EOS_CathareIncondensableGas::get_prxcp2() { return prxcp2; }
  inline double EOS_CathareIncondensableGas::get_prxcp3() { return prxcp3; }
  inline double EOS_CathareIncondensableGas::get_prxcp4() { return prxcp4; }
  inline double EOS_CathareIncondensableGas::get_prxcp5() { return prxcp5; }
  inline double EOS_CathareIncondensableGas::get_prxcp6() { return prxcp6; }

  inline EOS_Internal_Error EOS_CathareIncondensableGas::compute_T_ph(double p, double h, double& T) const
  { cerr << " *** C2_inc *** Not_implemented compute_T_ph,  p = " << p << "  h = " << h
                                 << " :  T = " << T << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  inline EOS_Internal_Error EOS_CathareIncondensableGas::compute_rho_ph(double p, double h, double& rho) const
  { cerr << " *** C2_inc *** Not_implemented compute_rho_ph,  p = " << p << "  h = " << h
                                 << " :  rho = " << rho << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  inline EOS_Internal_Error EOS_CathareIncondensableGas::compute_rho_pT(double p, double T, double& rho) const
  { cerr << " *** C2_inc *** Not_implemented compute_rho_pT,  p = " << p << "  T = " << T
                                 << " :  rho = " << rho << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  inline EOS_Internal_Error EOS_CathareIncondensableGas::compute_Cp_ph(double p, double h, double& Cp) const
  { cerr << " *** C2_inc *** Not_implemented compute_Cp_ph,  p = " << p << "  h = " << h
                                 << " :  Cp = " << Cp << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  inline EOS_Internal_Error EOS_CathareIncondensableGas::compute_Cp_pT(double p, double T, double& Cp) const
  { cerr << " *** C2_inc *** Not_implemented compute_Cp_pT,  p = " << p << "  T = " << T
                                 << " :  Cp = " << Cp << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  inline EOS_Internal_Error EOS_CathareIncondensableGas::compute_mu_ph(double p, double h, double& mu) const
  { cerr << " *** C2_inc *** Not_implemented compute_mu_ph,  p = " << p << "  h = " << h
                                 << " :  mu = " << mu << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  inline EOS_Internal_Error EOS_CathareIncondensableGas::compute_mu_pT(double p, double T, double& mu) const
  { cerr << " *** C2_inc *** Not_implemented compute_mu_pT,  p = " << p << "  T = " << T
                                 << " :  mu = " << mu << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  inline EOS_Internal_Error EOS_CathareIncondensableGas::compute_lambda_ph(double p, double h, double& lambda) const
  { cerr << " *** C2_inc *** Not_implemented compute_lambda_ph,  p = " << p << "  h = " << h
                                 << " :  lambda = " << lambda << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }

  inline EOS_Internal_Error EOS_CathareIncondensableGas::compute_lambda_pT(double p, double T, double& lambda) const
  { cerr << " *** C2_inc *** Not_implemented compute_lambda_pT,  p = " << p << "  T = " << T
                                 << " :  lambda = " << lambda << endl ;
    return EOS_Internal_Error::NOT_IMPLEMENTED ;
  }
}

#endif
