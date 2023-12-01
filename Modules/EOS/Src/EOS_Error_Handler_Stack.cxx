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



#include "EOS/API/EOS_Error_Handler.hxx"
#include "EOS/API/EOS_Error_Handler_Stack.hxx"

namespace NEPTUNE
{
  // --------------------------------------------------------------
  // Definition and implementation of EOS_Error_Handler_Stack_Elt

  //! One element of the EOS_Error_Handler_Stack
  //! This structure owns the memory of handler_ but not next_.
  class EOS_Error_Handler_Stack_Elt
  {
  public:
    EOS_Error_Handler_Stack_Elt(const EOS_Error_Handler     & handler, 
                                EOS_Error_Handler_Stack_Elt * next);
    ~EOS_Error_Handler_Stack_Elt();
    //! handler_ is never NULL, pointed object is owned by the structure
    const EOS_Error_Handler     * handler_;
    //! next_ is NULL at stack tail. pointed object is NOT owned by the structure
    EOS_Error_Handler_Stack_Elt * next_;
  private:
    //! copy constructor not permitted
    EOS_Error_Handler_Stack_Elt(const EOS_Error_Handler_Stack_Elt &);
    //! operator= not permitted
    EOS_Error_Handler_Stack_Elt & operator=(const EOS_Error_Handler_Stack_Elt &);
  };

  //! Constructor (create a local copy of handler, store "next" pointer as is)
  EOS_Error_Handler_Stack_Elt::
  EOS_Error_Handler_Stack_Elt(const EOS_Error_Handler     & handler, 
                              EOS_Error_Handler_Stack_Elt * next) :
    handler_(& handler.duplicate()), next_(next)
  {
  }

  //! Destructor: destroy handler_ (does NOT destroy next_)
  EOS_Error_Handler_Stack_Elt::
  ~EOS_Error_Handler_Stack_Elt()
  {
    delete handler_;
  }

  // --------------------------------------------------------------
  // Implementation of EOS_Error_Handler_Stack

  //! Create an empty stack
  EOS_Error_Handler_Stack::
  EOS_Error_Handler_Stack() :
    top_of_stack_(0)
  {
  }

  //! Destroy all stack elements
  EOS_Error_Handler_Stack::
  ~EOS_Error_Handler_Stack()
  {
    while (top_of_stack_)
      pop();
  }

  //! Add a new element to stack (make a copy of the handler)
  void EOS_Error_Handler_Stack::
  push(const EOS_Error_Handler & handler)
  {
    EOS_Error_Handler_Stack_Elt * next = top_of_stack_;
    top_of_stack_ = new EOS_Error_Handler_Stack_Elt(handler, next);
  }

  //! Return a pointer to the most recently pushed error handler.
  //! This is not a copy: pointed object is still owned by the stack.
  //! Pointer is valid until stack object is destroyed with pop()
  //! Pointer is NULL if stack is empty.
  const EOS_Error_Handler * EOS_Error_Handler_Stack::
  top_of_stack() const
  {
    if (top_of_stack_)
      return top_of_stack_->handler_;
    else
      return 0;
  }

  //! Destroys the top of stack element.
  void EOS_Error_Handler_Stack::
  pop()
  {
    if (top_of_stack_) {
      EOS_Error_Handler_Stack_Elt * old = top_of_stack_;
      top_of_stack_ = top_of_stack_->next_;
      delete old;
    }
  }
}
