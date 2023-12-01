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



#include "EOS/API/EOS_Std_Error_Handler.hxx"
#include "EOS/API/EOS_Fluid.hxx"
#include "EOS/API/EOS_Field.hxx"
#include "EOS/API/EOS_Fields.hxx"
#include "EOS/API/EOS_Error_Field.hxx"
#include <iostream>
// The following is for sprintf:
#include <stdio.h>
namespace NEPTUNE
{

  //! Error level good means "disable feature"
  const EOS_Error EOS_Std_Error_Handler::disable_feature = EOS_Error::good ;


  //! Default constructor. By default, EOS will exit
  //! on all errors "error", throw no exception,
  //! dump an error message on cerr if "error".
  EOS_Std_Error_Handler::EOS_Std_Error_Handler()
  { no_exit_on_   = EOS_Error::bad   ;      // Exit on "error" only
    no_throw_on_  = EOS_Error::error ;      // Feature disabled
    no_dump_on_   = EOS_Error::bad   ;      // dump on "error" only;
    dump_stream_  = & std::cerr ;
  }


  // Convert "trigger if error level below "error"" given by user application
  // to      "trigger if error level not above "no_error_on"" stored in local
  //           variable.
  // See no_exit_on_ and set_exit_on_error
  static EOS_Error convert_trigger(EOS_Error err)
  { EOS_Error no_error_on = err ;
    switch (err) 
       { case EOS_Error::error :  no_error_on = EOS_Error::bad   ; break ;
         case EOS_Error::bad   :  no_error_on = EOS_Error::ok    ; break ;
         case EOS_Error::ok    :  no_error_on = EOS_Error::good  ; break ;
         case EOS_Error::good  :  no_error_on = EOS_Error::error ; break ;
         default:
            std::cerr << "Internal error in EOS_Std_Error_Handler : error level not implemented "
                      << error << std::endl ;
            exit(-1) ;
       }
    return no_error_on ;
  }
 

  void EOS_Std_Error_Handler::set_exit_on_error(EOS_Error err)
  { no_exit_on_ = convert_trigger(err) ;
  }


  void EOS_Std_Error_Handler::set_throw_on_error(EOS_Error err)
  { no_throw_on_ = convert_trigger(err) ;
  }


  void EOS_Std_Error_Handler::set_dump_on_error(EOS_Error err)
  { no_dump_on_ = convert_trigger(err) ;
  }


  void EOS_Std_Error_Handler::set_dump_stream(std::ostream & os)
  { dump_stream_ = &os  ;
  }


  // Describe EOS_Error.
  static const char * string_err(EOS_Error err)
  { switch(err) 
       { case EOS_Error::error :  return "error" ;
         case EOS_Error::bad   :  return "bad"   ;
         case EOS_Error::ok    :  return "ok"    ;
         case EOS_Error::good  :  return "good"  ;
         default:    return "unknown (update EOS_Std_Error_Handler.cxx)" ;
       }
  }


  // Error handler for single point compute
  EOS_Error EOS_Std_Error_Handler::handle_error_point(const EOS_Fluid &eos_fluid,
                                                      const EOS_Internal_Error err_code,
                                                      const char *source_code_info,
                                                      const double arg1,
                                                      const double arg2,
                                                      const double result)
  { const EOS_Error err = err_code.generic_error() ;
    if (err > no_dump_on_) 
       { std::ostream & os = *dump_stream_ ;

         AString description ;
         eos_fluid.describe_error(err_code, description) ;

         // Compute error code without flags
         int err_partial_code = err_code.get_partial_code() ;

         os << " EOS_Error_dump : " << endl ;
         os << " - generic  error = " << string_err(err)
            << " ; specific_error = " << err_partial_code << endl ;
         os << " - specific error description   : " << description      << endl ;
         os << " - source code info             : " << source_code_info << endl ;
         os << " - arg1 = " << arg1 << " --  arg2 = " << arg2 << endl <<endl;
       }

    // Exit if required
    if (err > no_exit_on_) 
       { ostream & os = *dump_stream_ ;
         os << " EOS_Std_Error_Handler triggers exit()" << endl ;
         exit(-1) ;
       }

    // Throw exception if required
    if (err > no_throw_on_)  throw EOS_Std_Exception(err_code, source_code_info) ;
  
    // Return EOS_Error code
    return err ;
  }


  // Tool function used to dump informations on fields
  static void print_field_names(const LANGUAGE_KERNEL::UObject * const arg, std::ostream & os)
  { char s[100] ;
    const EOS_Field  * field  = dynamic_cast<const EOS_Field *>(arg)  ;
    const EOS_Fields * fields = dynamic_cast<const EOS_Fields *>(arg); 
    if (field) 
       { const AString & name = field->get_property_name() ;
         sprintf(s, "%20s", name.aschar()) ;
         os << s ;
       } 
    else if (fields) 
       { const int n = fields->size() ;
         for (int i = 0; i<n; i++) 
            { const AString & name = (*fields)[i].get_property_title() ;
              sprintf(s, "%20s", name.aschar()) ;
              os << s ;
            }
       }
    else 
       os << "Unknown object type " << endl;
    
  }


  static void print_field_values(const UObject * const arg, int j, ostream & os)
  { char s[100] ;
    const EOS_Field  * field  = dynamic_cast<const EOS_Field *>(arg)  ;
    const EOS_Fields * fields = dynamic_cast<const EOS_Fields *>(arg) ;
    if (field) 
       { const double x = (*field)[j] ;
         sprintf(s, "%20g", x) ;
         os << s ;
       }
    else if (fields) 
       { const int n = fields->size() ;
         for (int i = 0; i<n; i++) 
            { const double x = ((*fields)[i])[j] ;
              sprintf(s, "%20g", x) ;
              os << s ;
            }
       }
    else 
       os << "                    " << endl ;
  }


  // Error handler for field compute
  EOS_Error EOS_Std_Error_Handler::handle_error_field(const EOS_Fluid &eos_fluid,
                                                      const EOS_Error_Field &err_field,
                                                      const char *source_code_info,
                                                      const UObject *arg1,
                                                      const UObject *arg2,
                                                      const UObject *result)
  { const int max_lines_to_dump = 40 ;
    const EOS_Internal_Error ierr = err_field.find_worst_error() ;
    const EOS_Error          err  = ierr.generic_error() ;
  
    // Dump information if required
    if (err > no_dump_on_) 
       { ostream & os = *dump_stream_ ;
    
         os << " EOS_Error_dump_field : " << endl ;
         os << " - generic  error = " << string_err(err)  << endl ;
         os << " - source code info  : " << source_code_info << endl << endl ;
    
         // If too many field points, dump only points with errors
         const int n = err_field.size() ;
         int dump_all_values = 1 ;
         if (n > max_lines_to_dump) 
            { dump_all_values = 0 ;
              os << " Field too big: dumping bad values only. Dump format:" << endl ;
            }
         else
           os << " Complete field dump. Dump format:" << endl ;
         os << " Index     SpecifErr " ;
         if (arg1)  print_field_names(arg1, os) ;
         if (arg2)  print_field_names(arg2, os) ;
         // a modifier ?
         if (result)  print_field_names(result, os) ;
         // a modifier ?
         os << " Description" << endl ;
         int i ;
         // If more than N lines to dump, then abort.
         int count_left = max_lines_to_dump ;
         char s[100] ;
         for (i = 0; i<n; i++) 
            { if (count_left == 0)  break ;
              const EOS_Error e = err_field[i].generic_error() ;
              if (dump_all_values || e > no_dump_on_) 
                 { count_left-- ;
                   sprintf(s,"%9d %9d ", i, err_field[i].get_partial_code()) ;
                   os << s ;
                   if (arg1)  print_field_values(arg1, i, os) ;
                   if (arg2)  print_field_values(arg2, i, os) ;
                   //   if (result)
                   //  print_field_values(result, i, os);
                   AString description ;
                   eos_fluid.describe_error(err_field[i], description) ;
                   os << " " << description << endl ;
                 }
            }
       }
  
    // Exit if required
    if (err > no_exit_on_) 
       { ostream & os = *dump_stream_ ;
         os << "EOS_Std_Error_Handler triggers exit() " 
            << err << " " << no_exit_on_ << endl ; 
         exit(-1) ;
       }

    // Throw exception if required
    if (err > no_throw_on_)  throw EOS_Std_Exception(ierr, source_code_info) ;
  
    // Return EOS_Error code
    return err ;  
  }

  // Duplicates *this and returns a reference to the new instance
  // See also EOS_Error_Handler::duplicate()
  EOS_Error_Handler & EOS_Std_Error_Handler::duplicate() const
  { // Call to copy constructor.
    EOS_Std_Error_Handler *ptr = new EOS_Std_Error_Handler ;
    *ptr = *this ;
    // cerr << "Duplicate : " << no_exit_on_ << " " << ptr->no_exit_on_ << endl;
    return *ptr ;
  }

}
