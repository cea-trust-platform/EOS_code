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



#ifndef Cathare_LiquidWater_hxx
#define Cathare_LiquidWater_hxx

#include "EOS_Cathare_Water.hxx"

using namespace NEPTUNE ;
using namespace CATHARE ;

namespace NEPTUNE_EOS
{
  class EOS_Cathare_LiquidWater: public EOS_Cathare_Water
  { static const AString equationname ;

    public:
      virtual const AString& equation_name() const ;
      
      virtual ostream& print_On (ostream& stream=cout) const ;
      virtual istream& read_On (istream& stream=cin)  ;
      virtual const Type_Info& get_Type_Info () const ;
      virtual int init(const Strings&) ;

      EOS_Error verify_ph() const ;
      
      //  p limits (min)
      virtual EOS_Internal_Error get_p_min(double& p_min) const ;
      //  h limits (min)
      virtual EOS_Internal_Error get_h_min(double& h_min) const ;
      //  T limits (min)
      virtual EOS_Internal_Error get_T_min(double& T_min) const ;
      //  p limits (max)
      virtual EOS_Internal_Error get_p_max(double& p_max) const ;
      //  h limits (max)
      virtual EOS_Internal_Error get_h_max(double& h_max) const ;
      //  T limits (max)
      virtual EOS_Internal_Error get_T_max(double& T_max) const ;
         
      virtual EOS_Error compute(const EOS_Field& p, 
                                const EOS_Field& h, 
                                EOS_Fields& r, 
                                EOS_Error_Field& errfield) const ;
      virtual EOS_Error compute(const EOS_Fields& input, 
                                EOS_Fields& r,
                                EOS_Error_Field& errfield) const ;
    private:
      static int type_Id ;
  };
}
#include "EOS_Cathare_LiquidWater_i.hxx"

#endif

