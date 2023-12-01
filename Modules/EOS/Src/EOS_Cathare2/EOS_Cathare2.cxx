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



#include "EOS_Cathare2.hxx"

using namespace NEPTUNE ;

namespace NEPTUNE_EOS
{
  const AString EOS_Cathare2::tablename("Cathare2")        ;
  const AString EOS_Cathare2::versionname("v25_3_mod9.153") ;
  static AString UnknownStr("Unknown");

  const AString& EOS_Cathare2::table_name()const
  { return tablename ;
  }
  const AString& EOS_Cathare2::version_name()const
  { return versionname ;
  }

  ostream& EOS_Cathare2::print_On (ostream& stream) const
  { return stream ;
  }

  istream& EOS_Cathare2::read_On (istream& stream)
  { return stream ;
  }

  const AString& EOS_Cathare2::fluid_name() const
  { return fluidname ;
  }

  const AString& EOS_Cathare2::equation_name() const
  { return equationname ;
  }

  static RegisteredClass& Cathare2_create()
  { return *(new EOS_Cathare2()) ;
  }

  EOS_Cathare2::EOS_Cathare2(CATHARE2::domain the_phase) :
  pilot(NULL), phase(the_phase), fluidname(UnknownStr)
  {
  }

  EOS_Cathare2::EOS_Cathare2() :
    pilot(NULL), phase(CATHARE2::unknown), fluidname(UnknownStr)
  {
  }

  EOS_Cathare2::EOS_Cathare2(const EOS_Cathare2&)
  { assert(0);
  }

  EOS_Cathare2::~EOS_Cathare2()
  { if (pilot) delete pilot;
    pilot = NULL;
  }

  int EOS_Cathare2::type_Id=(RegisterType
  ( "EOS_Cathare2",
    "EOS_Fluid",
    "Cathare2 Functions",
    sizeof(EOS_Cathare2),
    Cathare2_create
  ));

  const Type_Info& EOS_Cathare2::get_Type_Info () const
  { return (Types_Info::instance())[type_Id] ;
  }

  int EOS_Cathare2::init(const Strings& args) 
  { int i = 0 ;
    if (pilot == NULL) {
      #include "EOS_Cathare2.cxx.fld"
      {
        cerr << "Unknown fluid " << args[0] << endl;
        return EOS_Error::error ;
      }
      fluidname = args[0];
      equationname = args[0];
      equationname += phase_name();
      i++;
    }
    //! \todo Add checks on arguments
    //! \todo Add option POLYNCPT
    for (; i<args.size(); i++) 
       { if (i == args.size()) return EOS_Error::error ;
         EOS_Error err ;
         if ( args[i] == AString("ICAR") || args[i] == AString("icar"))
           err  = pilot->set_icar(atoi(args[++i].aschar()));
         else if ( args[i] == AString("ICARGAS") || args[i] == AString("icargas"))
           err  = pilot->set_icargas(atoi(args[++i].aschar()));
         else if ( args[i] == AString("IENC") || args[i] == AString("ienc"))
           err  = pilot->set_ienc(atoi(args[++i].aschar()));
         else if ( args[i] == AString("IREV") || args[i] == AString("irev"))
           err  = pilot->set_irev(atoi(args[++i].aschar()));
         else if ( args[i] == AString("ITERMIN") || args[i] == AString("itermin"))
           err  = pilot->set_itermin(atoi(args[++i].aschar()));
         else if ( args[i] == AString("LATYPML") || args[i] == AString("latypml"))
           err  = pilot->set_latypml(atoi(args[++i].aschar()));
         else if ( args[i] == AString("MUTYPML") || args[i] == AString("mutypml"))
           err  = pilot->set_mutypml(atoi(args[++i].aschar()));
         else 
           err = EOS_Error::error ;
         if (err != EOS_Error::good) return EOS_Error::error ;
       }
    return EOS_Error::good ;
  }

  int EOS_Cathare2::init(const Strings& args, const Strings& other_args) 
  { int iret ;
    iret = init(args) ;
    if (iret == (int)EOS_Error::good)  iret = init(other_args) ;
    return iret ;
  }

  EOS_Error EOS_Cathare2::compute (const EOS_Field& in, EOS_Fields& out, 
                                   EOS_Error_Field& err) const 
  { assert(pilot) ;

    if ( (in.get_property() == NEPTUNE::p) 
      || (in.get_sat_property() == NEPTUNE::p_sat) 
      || (in.get_lim_property() == NEPTUNE::p_lim) ) 
       { ArrOfInt err_array(in.size()) ;
         EOS_Error_Field err_tmp(err_array) ;
         pilot->verify(in, err, phase) ;
         pilot->calc2_p(in, out, err_tmp) ;
         err.set_worst_error(err_tmp) ;
         return err.find_worst_error().generic_error() ;
       }
    else if (in.get_sat_property() == NEPTUNE::T_sat)
       { ArrOfInt err_array(in.size()) ;
         EOS_Error_Field err_tmp(err_array) ;
         pilot->verify(in, err, phase) ;
         pilot->calc2_t(in, out, err_tmp) ;
         err.set_worst_error(err_tmp) ;
         return err.find_worst_error().generic_error() ;
       } 
    else
       return EOS_Fluid::compute(in, out, err) ;
  }

  EOS_Error EOS_Cathare2::compute (const EOS_Field& in1, const EOS_Field& in2, 
                                   EOS_Fields& out, EOS_Error_Field& err) const 
  { assert(pilot) ;
    ArrOfInt err_array(in1.size()) ;
    EOS_Error_Field err_tmp(err_array) ;

    if ( ( in1.get_property() == NEPTUNE::p) && (in2.get_property() == NEPTUNE::h)) 
       { pilot->verify(in1, err, phase);
         pilot->verify(in2, err_tmp, phase);
         err.set_worst_error(err_tmp);
         pilot->calc2_ph(in1, in2, out, err_tmp);
         err.set_worst_error(err_tmp);
         return err.find_worst_error().generic_error();
       }
    else if ( (in1.get_property() == NEPTUNE::h) && (in2.get_property() == NEPTUNE::p))
       { pilot->verify(in1, err, phase);
         pilot->verify(in2, err_tmp, phase);
         err.set_worst_error(err_tmp);
         pilot->calc2_ph(in2, in1, out, err_tmp);
         err.set_worst_error(err_tmp);
         return err.find_worst_error().generic_error();
       }
    else if ( (in1.get_property() == NEPTUNE::p) && (in2.get_property() == NEPTUNE::T))
       { pilot->verify(in1, err, phase);
         pilot->verify(in2, err_tmp, phase);
         err.set_worst_error(err_tmp);
         pilot->calc2_pt(in1, in2, out, err_tmp);
         err.set_worst_error(err_tmp);
         return err.find_worst_error().generic_error();
       } 
    else if ( (in1.get_property() == NEPTUNE::T) && (in2.get_property() == NEPTUNE::p))
       { pilot->verify(in1, err, phase);
         pilot->verify(in2, err_tmp, phase);
         err.set_worst_error(err_tmp);
         pilot->calc2_pt(in2, in1, out, err_tmp);
         err.set_worst_error(err_tmp);
         return err.find_worst_error().generic_error();
       }
    else
       return EOS_Fluid::compute(in1,in2,out,err);
  }

  EOS_Error EOS_Cathare2::compute (const EOS_Field& p, const EOS_Field& h, EOS_Field& out, EOS_Error_Field& err) const 
  { EOS_Fields fields(1) ;
    fields[0] = out ;
    return compute(p,h,fields,err) ;
  }

  EOS_Error EOS_Cathare2::compute (const EOS_Field& p, EOS_Field& out, EOS_Error_Field& err) const 
  { EOS_Fields fields(1) ;
    fields[0] = out ;
    return compute(p,fields,err) ;
  }

  EOS_Error EOS_Cathare2::compute (const EOS_Fields& in, EOS_Fields& out, EOS_Error_Field& err) const 
  { int nbi=in.size() ;
    if (nbi == 1)
       return compute(in[0],out,err) ;
    else if (nbi == 2)
      return compute(in[0], in[1], out, err) ;
    else
      return EOS_Fluid::compute(in,out,err) ;     
  }

  EOS_Error EOS_Cathare2::compute_Ph(const char* const property_name, double in1, double in2, double& out) const
  { int ierr ;
    EOS_Error er ;

    EOS_Field fin1("P", "P", 1, &in1) ;
    EOS_Field fin2("h", "h", 1, &in2) ;
    EOS_Error_Field ferr(1, &ierr) ;
    EOS_Field fout(property_name, property_name, 1, &out) ;

    er = compute(fin1, fin2, fout, ferr) ;
    return er ;
  }

  EOS_Error EOS_Cathare2::compute_PT(const char* const property_name, double in1, double in2, double& out) const
  { int ierr ;
    EOS_Error er ;

    EOS_Field fin1("P", "P", 1, &in1) ;
    EOS_Field fin2("T", "T", 1, &in2) ;
    EOS_Error_Field ferr(1, &ierr) ;
    EOS_Field fout(property_name, property_name, 1, &out) ;

    er = compute(fin1, fin2, fout, ferr) ;
    return er ;
  }

  EOS_Error EOS_Cathare2::compute_Psat(const char* const property_name, double in, double& out) const
  { int ierr ;
    EOS_Error er ;

    EOS_Field fin("Psat", "Psat", 1, &in) ;
    EOS_Error_Field ferr(1, &ierr) ;
    EOS_Field fout(property_name, property_name, 1, &out) ;

    er = compute(fin, fout, ferr) ;
    return er ;
  }

  EOS_Error EOS_Cathare2::compute_Tsat(const char* const property_name, double in, double& out) const
  { int ierr ;
    EOS_Error er ;

    EOS_Field fin("Tsat", "Tsat", 1, &in) ;
    EOS_Error_Field ferr(1, &ierr) ;
    EOS_Field fout(property_name, property_name, 1, &out) ;

    er = compute(fin, fout, ferr) ;
    return er ;
  }
  
}
