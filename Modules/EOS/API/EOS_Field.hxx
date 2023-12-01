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




#ifndef EOS_FIELD_H
#define EOS_FIELD_H

#include "EOS_Error.hxx"
#include "EOS_properties.hxx"
#include "Language/API/Language.hxx"
#include "Common/func.hxx"

using LANGUAGE_KERNEL::UObject;

namespace NEPTUNE
{
  //! A field for the EOS module
  class EOS_Field : public UObject
  { public: 
       EOS_Field() ;
       EOS_Field(const EOS_Field&) ;
       //! x_ptr=new double(n);
       //! EOS_Field("Pressure", "P", n, x_ptr);
       EOS_Field(const char* const property_title, 
                 const char* const property_name,
                 int nsz, double* ptr);
       //! ArrOfDouble x(n);
       //! EOS_Field("Pressure", "P", x);
       EOS_Field(const char* const property_title, 
                 const char* const property_name,
                 ArrOfDouble&);
       EOS_Field& operator = (const EOS_Field&);
       int init(const char* const property_title, 
                const char* const property_name,
                ArrOfDouble&);
       //! AField.set_property_title("Pressure");
       void set_property_title(const char* const name);
       //! AField.set_property_name("P");
       void set_property_name(const char* const name) ;
       const AString& get_property_title()      const ;
       const AString& get_property_name()       const ;
       const int&     get_property_number()     const ;
       AString        get_propname_int()        const ;
       EOS_thermprop  get_property()            const ;
       EOS_thermprop  get_der_property()        const ;
       EOS_saturprop  get_sat_property()        const ;
       EOS_saturprop  get_der_sat_property()    const ;
       EOS_saturprop  get_der2_sat_property()   const ;
       EOS_splimprop  get_lim_property()        const ;
       EOS_camixprop  get_camix_property()      const ;
       EOS_camixprop  get_der_camix_property()  const ;
       EOS_camixprop  get_der2_camix_property() const ;
       EOS_c2iapprop  get_c2iap_property()      const ;
       EOS_c2iapprop  get_der_c2iap_property()  const ;
       const double& operator [] (int) const;
       double& operator [] (int);
       ArrOfDouble& set_data();
       const ArrOfDouble& get_data() const;
       virtual std::ostream& print_On (std::ostream& stream=std::cout) const;
       virtual std::istream& read_On (std::istream& stream=std::cin);
       //! see the Language documentation ...
       virtual const Type_Info& get_Type_Info () const;
       int size() const;

    protected :
       ArrOfDouble data ;

    private: 
       int gen_property_number(const char* namep) ;
       static int type_Id ;
       AString property_title  ;
       AString property_name   ;
       int     property_number ;
  };
}
#include "EOS/API/EOS_Field_i.hxx"
#endif
