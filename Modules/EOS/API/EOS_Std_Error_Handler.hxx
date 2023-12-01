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



#ifndef EOS_Std_Error_Handler_H
#define EOS_Std_Error_Handler_H

// We only need forward declarations for iostream
// If not available, then include <iostream> instead
#include <iosfwd>
#include "EOS/API/EOS_Error_Handler.hxx"

namespace NEPTUNE
{
  //! Standard implementation of the error handler :
  //!  Fully configurable : exit, throw exception or pass through,
  //!   optional dump of parameters to a user specified stream.
  //! See header file for an example.
  // How to use this handler:
  //  
  //  {
  //    EOS_Std_Error_Handler handler; // Declare one
  //    handler.set_exit_on_error(EOS_Error::bad); // Configure (example)
  //    handler.set_dump_on_error(EOS_Error::ok);
  //    handler.set_dump_stream(cerr);
  //    eos.set_error_handler(handler); // Attach handler to eos object
  //  } // End of scope, handler can be destroyed
  //  
  class EOS_Std_Error_Handler : public EOS_Error_Handler
  {
  public:
    EOS_Std_Error_Handler();
    //! Trigger exit() if error level lower than error. See also disable_feature.
    void set_exit_on_error(EOS_Error err);
    //! Trigger throw EOS_Std_Exception if error level lower than error. See also disable_feature.
    void set_throw_on_error(EOS_Error err) ;
    //! Dump informations to dump_stream_ if error level lower than error. See also disable_feature.
    void set_dump_on_error(EOS_Error err) ;
    //! Change dump_stream_ to os.
    void set_dump_stream(std::ostream & os);

    //! See class EOS_Error_Handler::handle_error_point for comments
    EOS_Error handle_error_point(const EOS_Fluid & eos_fluid,
                                 const EOS_Internal_Error err_code,
                                 const char * const source_code_info,
                                 const double arg1,
                                 const double arg2,
                                 const double result);

    //! See class EOS_Error_Handler::handle_error_field for comments
    EOS_Error handle_error_field(const EOS_Fluid & eos_fluid,
                                 const EOS_Error_Field & err_field,
                                 const char * const source_code_info,
                                 const LANGUAGE_KERNEL::UObject * const arg1,
                                 const LANGUAGE_KERNEL::UObject * const arg2,
                                 const LANGUAGE_KERNEL::UObject * const result);
   
    //! Pass this constant to set_xxx_on_error to disable xxx feature:
    //! (example : set_exit_on_error(EOS_Std_Error_Handler::disable_feature)
    //!  => Error handler will never exit.)
    static const EOS_Error disable_feature;

  protected:
    virtual EOS_Error_Handler & duplicate() const;
  private:
    //! The value stored is the minimum error level for the feature
    //! not to be triggered 
    //! Example : 
    //!  if no_exit_on_ == good, exit() is triggered on "error", "bad" and "ok", 
    //!  if no_exit_on_ == error,exit() is never triggered.
    //! See constructor for default value.
    EOS_Error no_exit_on_;
    // ! See no_exit_on_
    EOS_Error no_throw_on_;
    // ! See no_exit_on_
    EOS_Error no_dump_on_;
    //! Stream used to dump informations (see constructor for default value)
    std::ostream *dump_stream_;
  };

  //! When this handler throws an exception, it is of the following type:
  struct EOS_Std_Exception 
     { //! Internal error code
       EOS_Internal_Error err_code;
       //! Where in the code did it happen
       const char *code_info;

       //! Constructor
       EOS_Std_Exception(EOS_Internal_Error err, const char *info) :
         err_code(err), code_info(info) {};
     };
}

#endif
