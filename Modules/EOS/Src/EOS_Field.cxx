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




#include "Language/API/Types_Info.hxx"
#include "EOS/API/EOS_Field.hxx"

namespace NEPTUNE
{
  static RegisteredClass& EOS_Field_create()
  { return *(new EOS_Field()) ;
  }

  int EOS_Field::type_Id = (RegisterType("EOS_Field", 
                            "A Field for EOS",
                            sizeof(EOS_Field),
                            EOS_Field_create)) ;

  ostream& EOS_Field::print_On (ostream& stream) const
  { stream << " EOS_Field : property title = " << property_title 
           << "   property name = "   << property_name 
           << "   property number = " << property_number
           << "   data : "            << data  << endl ;
    return stream ;
  }

  istream& EOS_Field::read_On (istream& stream)
  { return stream ;
  }

  const Type_Info& EOS_Field::get_Type_Info () const
  { return (Types_Info::instance())[type_Id] ;
  }

  EOS_Field::EOS_Field(const char* const namet, 
                       const char* const namep,
                       ArrOfDouble& x) :
  data(x.size(), &x[0]),
  property_title(namet),
  property_name(namep) 
  { property_number = gen_property_number(namep) ;
  }

  EOS_Field::EOS_Field(const EOS_Field &f):
  UObject(),
  data(f.data.size(), &f.data[0]),
  property_title(f.property_title), 
  property_name(f.property_name),
  property_number(f.property_number)
  {
  }

  EOS_Field& EOS_Field::operator = (const EOS_Field &f)
  { data.attach(f.get_data()) ;
    property_title   = f.property_title  ;
    property_name    = f.property_name   ;
    property_number  = f.property_number ;
    return *this ;
  }

  int EOS_Field::init(const char* const namet, 
                      const char* const namep,
                      ArrOfDouble &x) 
  { property_title  = namet ;
    property_name   = namep ;
    property_number = gen_property_number(namep)   ;
    data            = ArrOfDouble(x.size(), &x[0]) ;
    return good ;
  }

  ArrOfDouble& EOS_Field::set_data()
  { return data ;
  }

  const ArrOfDouble& EOS_Field::get_data() const
  { return data ;
  }

  const AString& EOS_Field::get_property_title() const
  { return property_title ;
  }

  const AString & EOS_Field::get_property_name() const
  { return property_name ;
  }

  const int & EOS_Field::get_property_number() const
  { return property_number ;
  }

  AString EOS_Field::get_propname_int() const
  { char propconv[PROPNAME_MSIZE] ;
    eostp_strcov(property_name.aschar(), propconv) ;
    return propconv ;
  }

  EOS_thermprop EOS_Field::get_property() const
  { return nam2num_thermprop(property_name.aschar()) ;
  }
  
  EOS_thermprop EOS_Field::get_der_property() const
  { return nam2num_dthermprop(property_name.aschar()) ;
  }

  EOS_saturprop EOS_Field::get_sat_property() const
  { return nam2num_saturprop(property_name.aschar()) ;
  }

  EOS_saturprop EOS_Field::get_der_sat_property() const
  { return nam2num_dsaturprop(property_name.aschar()) ;
  }

  EOS_saturprop EOS_Field::get_der2_sat_property() const
  { return nam2num_d2saturprop(property_name.aschar()) ;
  }

  EOS_splimprop EOS_Field::get_lim_property() const
  { return nam2num_splimprop(property_name.aschar()) ;
  }

  EOS_camixprop EOS_Field::get_camix_property() const
  { return nam2num_camixprop(property_name.aschar()) ;
  }

  EOS_camixprop EOS_Field::get_der_camix_property() const
  { return nam2num_dcamixprop(property_name.aschar()) ;
  }

  EOS_camixprop EOS_Field::get_der2_camix_property() const
  { return nam2num_d2camixprop(property_name.aschar()) ;
  }

  EOS_c2iapprop EOS_Field::get_c2iap_property() const
  { return nam2num_c2iapprop(property_name.aschar()) ;
  }

  EOS_c2iapprop EOS_Field::get_der_c2iap_property() const
  { return nam2num_dc2iapprop(property_name.aschar()) ;
  }

}
