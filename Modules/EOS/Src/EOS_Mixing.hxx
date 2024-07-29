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



#ifndef EOS_Mixing_H
#define EOS_Mixing_H

#include "EOS/API/EOS.hxx"
#include "EOS/API/EOS_Fluid.hxx"
#include "EOS/API/EOS_Field.hxx"
#include "EOS/API/EOS_Fields.hxx"
#include "EOS/API/EOS_Config.hxx"
#ifdef WITH_PLUGIN_CATHARE
#include "EOS/Src/EOS_Cathare/CATHARE_Water.hxx"
#endif
using namespace NEPTUNE    ;
using NEPTUNE::EOS_Fluid   ;
using NEPTUNE::EOS_Error   ;
using NEPTUNE::EOS         ;
using NEPTUNE::ArrOfDouble ;
#ifdef WITH_PLUGIN_CATHARE
using namespace CATHARE ;  // provisional for EOS_Mixing
#endif

namespace NEPTUNE_EOS
{
  class EOS_Mixing :  public NEPTUNE::EOS_Fluid
  {
    enum MixingType {
        Cathare,
        Cathare2,
        WithPerfectGas,
        Unsupported,
    };
    static const AString tablename ;
    
    public:
      EOS_Mixing(int i=1) ;
      const AString& table_name() const ;
      int init(const Strings& ) ;
      int set_components(EOS **components, int nb) ;
      virtual ~EOS_Mixing() ;
      const EOS& operator[](int) const ;
      EOS& operator[](int) ;
      void set_component(int i, const EOS& fluid) ;
      int set_alpha(const ArrOfDouble&) ;
      const ArrOfDouble& get_alpha() const ;
      int set_alpha(int, double) ;
      double get_alpha(int) const ;
      // EOS_Fluid methods :
      virtual EOS_Error compute( const EOS_Fields& input, 
                                 EOS_Field& r,
                                 EOS_Error_Field& errfield) const;
      virtual EOS_Error compute( const EOS_Fields& input, 
                                 EOS_Fields& r,
                                 EOS_Error_Field& errfield) const;
    private:
      void set_compute_mode();
      void set_mixing_reference_state();
      EOS_Error compute_cathare2_mix(const EOS_Fields& input,
                                    EOS_Fields& r,
                                    EOS_Error_Field& errfield) const;
      EOS_Error compute_cathare_mix(const EOS_Fields& input,
                                    EOS_Fields& r,
                                    EOS_Error_Field& errfield) const;
      EOS_Error compute_perfect_gas(const EOS_Fields& input,
                                    EOS_Fields& r,
                                    EOS_Error_Field& errfield) const;
    public:
      virtual EOS_Internal_Error calcamix_T_ph(double p, double h, double&
                                     , double C_0
                                     , double C_1=0
                                     , double C_2=0
                                     , double C_3=0
                                     , double C_4=0
                                     ) const ;
      virtual int compute_pv_hv_ph(double P, double h, double &Pv, double &hv,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const;

//! use for surcharged operator<<
      void fluid_description(std::ostream& flux) const
      { 
        //flux << "  Thermodynamique Model : " << this->info_thermmodel() << endl ;
        //flux << endl ;
    
        if (nb_fluids != 1)
        {
          flux << " Main fluid Model : " << (*this)[0].info_thermmodel() << endl ;
          flux << (*this)[0] << endl ;
    
          for(int i=1; i<nb_fluids; i++)
          { 
            flux << " Non-condensable gas n." << i << " Model : " << (*this)[i].info_thermmodel() << endl ;
            flux << (*this)[i] << endl ;
          }
        }
    
        flux << endl ;
      }
    protected :
      int nb_fluids ;
      EOS **the_fluids ;
      ArrOfDouble alpha ;
#ifdef WITH_PLUGIN_CATHARE
      CATHARE_Water CATHARE_water ;  // provisional for EOS_Mixing  
#endif
      //
      //  Other methods
      //
      //! see Language
      virtual ostream& print_On (ostream& stream=cout) const ;
      //! see Language
      virtual istream& read_On (istream& stream=cin) ;
      //! see Language
      virtual const Type_Info& get_Type_Info () const ;
    private: 
      static int type_Id ;
      MixingType compute_mode = MixingType::Unsupported;
      //unordered_map<string, string> info_args ;
      const double rPerfectGas = 8.31447 ;
      // const double HVS7 = 2766.43e3 ; not used
      // const double TSS7 = 164.93e0 ; not used
      // const double TSS7K = 438.08e0 ; not used
  };
}
#endif
