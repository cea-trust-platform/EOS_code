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



#ifndef EOS_ERROR_H
#define EOS_ERROR_H
#include <assert.h>

namespace NEPTUNE
{
  // Generic error code returned by EOS.
  // For a field, the generic error is the worst
  // error of each computed point.
  // When the computation involves several steps (numerical differentiation,
  // function composition, etc...), the returned code is the worst code
  // ever seen in during the computation. When Newton iterations are used
  // the error obtained during the last iteration is returned.
  // 
  // Enumeration EOS_Error
  // ! until 0.9.x , error=-1 bad=0 ok=1 good=2
  // ! don't use directly an integer of enumeration,
  //   e.g. use  EOS_Error::error  instead of  error
  // EOS_Error::error = 3 : informatic error,          e.g. not implemented
  // EOS_Error::bad   = 2 : probably physically wrong, e.g. Newton did not converge
  // EOS_Error::ok    = 1 : may be physically wrong ,  e.g. out of validity bounds
  // EOS_Error::good  = 0 : everything good
  enum EOS_Error {error = 3, bad = 2, ok = 1, good = 0 } ;

  // EOS_Internal_Error stores error codes internally
  // in the EOS_Fluid object. Internal error codes are specific
  // to each derived class of EOS_Fluid. It can be converted to
  // a generic EOS_Error code with generic_error_code().
  // A description of the error is provided by EOS::describe_error()
  class EOS_Internal_Error
  {
  public:
    // Set the internal error code to "code" (code includes generic error flags)
    EOS_Internal_Error(int code=EOS_Internal_Error::OK, int library_code = 0) ;
    // Set the internal error code to "code", and add the generic error flags
    EOS_Internal_Error(int partial_code, EOS_Error generic_error, int library_code = 0) ;

    // get the complete code (stored as an int value)
    int get_code() const ;
    // get the partial error code (without generic error flags)
    int get_partial_code() const ;
    // get the underlying library error code (mostly for refprop)
    int get_library_code() const ;
    // get the generic error code for this error (from flags)
    EOS_Error generic_error() const ;
    // Standard internal error values 
    // Internal error code no error, everything is fine:
    static const int OK = 0;
    // System wide constant: requested function is not implemented:
    // 0x60000000 = 0110 0...0 (28 zero) en binaire => avec le decalage de 29 bits
    // dans la fonction EOS_Internal_Error::generic_error() on se place dans le cas error ( = 3) de EOS_Error
    static const int NOT_IMPLEMENTED = 0x60000000 ;
    static const int EOS_BAD_COMPUTE = 0x60000001 ;
    static const int DATA_NUMBER     = 0x60000002 ;
    static const int INPUT_PROPERTY  = 0x60000003 ;

  protected:
    // Error code stored as int value
    int code_;
    int library_code;

    // Explicitely define a protected conversion operator to forbid 
    // implicit conversion
    EOS_Internal_Error(EOS_Error) {} ;
  };
  
  inline int operator==(EOS_Internal_Error err1, EOS_Internal_Error err2)
  { return err1.get_code() == err2.get_code() ;
  }

  inline int operator!=(EOS_Internal_Error err1, EOS_Internal_Error err2)
  { return err1.get_code() != err2.get_code() ;
  }

  inline EOS_Error EOS_Internal_Error::generic_error() const
  { //Decalage de 29 bits des bits de l'attribut code_ vers la droite ( >> )
        //sur un entier 31 bits pour le nombre 1 pour le signe
        //=> nombre maximum en binaire retourne = 11 (=3 decimal)
    return (EOS_Error) (code_ >> 29) ;
  }

  inline int EOS_Internal_Error::get_code() const
  { return code_;
  }
  
  inline int EOS_Internal_Error::get_partial_code() const
  { return code_ & 0x1FFFFFFF ;
  }

  inline int EOS_Internal_Error::get_library_code() const
  { return library_code;
  }

  inline EOS_Internal_Error::EOS_Internal_Error(int code, int lib_code)
  { assert(code >= 0) ;
    code_ = code ;
    library_code = lib_code;
  }

  inline EOS_Internal_Error::EOS_Internal_Error(int partial_code, EOS_Error err, int lib_code)
  { assert(partial_code < 0x1FFFFFFF && partial_code >= 0);
    // [the magic formula is not used any more since 1.x.x]
    //assert(err >= -1 && err <= 2);
    //code_ = partial_code + ((2-err) << 29);
    assert(err >= 0 && err <= 3) ;
    code_ = partial_code + (err << 29) ;
    library_code = lib_code;
  }

  inline EOS_Error worst_generic_error(EOS_Error err1, EOS_Error err2) 
  {
    // [the magic formula is not used any more since 1.x.x]
    //return (err1 < err2) ? err1 : err2;
    return (err1 > err2) ? err1 : err2 ;
  }

  inline EOS_Internal_Error worst_internal_error(EOS_Internal_Error err1, EOS_Internal_Error err2) 
  { return (err1.get_code() > err2.get_code()) ? err1 : err2 ;
  }

}
#endif
