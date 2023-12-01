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



#ifndef EOS_ERROR_HANDLER_H
#define EOS_ERROR_HANDLER_H

#include "EOS_Error.hxx"

namespace LANGUAGE_KERNEL
{
  class UObject;
}

namespace NEPTUNE
{

  class EOS             ;
  class EOS_Error_Field ;
  class EOS_Fields      ;
  class EOS_Fluid       ;

  //! Base class for EOS error handlers.
  //! The error handler is called from the EOS compute methods.
  //! It converts EOS_Internal_Error errors to a generic EOS_Error
  //! code which is returned to the application and performs
  //! appropriate actions to handle the error.
  //! See EOS_Std_Error_Handler for an example of implementation.
  class EOS_Error_Handler
  { public:

    EOS_Error_Handler(){}          ;
    virtual ~EOS_Error_Handler(){} ;
    //! Error handler for a single point compute function
    //! This function is called from EOS::compute_xxx functions.
    //! err_code is the specific error returned by the compute
    //! function of the EOS_Fluid object.
    //! source_code_info is a string describing the source code where
    //! the error happened (eg. the name of the EOS function called)
    //! The EOS_Error returned by handle_error_point is returned to
    //! the caller of the EOS method.
    //! The error handler may or may not return depending on the
    //! implementation and the value of err_code. It may throw an
    //! exception, dump computed values in a log file, ...
    //! see derived classes.
    virtual EOS_Error handle_error_point(const EOS_Fluid &eos_fluid,
                                         const EOS_Internal_Error err_code,
                                         const char *source_code_info,
                                         const double arg1,
                                         const double arg2,
                                         const double result) = 0 ;
    
    //! Error handler for array compute functions.
    //! arg1, arg2 and result are the arguments passed to the compute functions.
    //! They are either null, EOS_Field or EOS_Fields pointers.
    //! (the error handler implementation should use get_arg method to
    //!  convert the argument to a general EOS_Fields object if needed)
    virtual EOS_Error handle_error_field(const EOS_Fluid &eos_fluid,
                                         const EOS_Error_Field &err_field,
                                         const char *source_code_info,
                                         const LANGUAGE_KERNEL::UObject *arg1,
                                         const LANGUAGE_KERNEL::UObject *arg2,
                                         const LANGUAGE_KERNEL::UObject *result) = 0 ;
    //! This method should be implemented in derived classes
    //! to duplicate the object with
    //!  return *(new derived_EOS_Error_Handler(*this));
    //! The copy constructor should "deep copy" the object.
    //! See EOS_Std_Error_Handler::duplicate
    virtual EOS_Error_Handler & duplicate() const = 0 ;
      
    protected:
  };
}
#endif
