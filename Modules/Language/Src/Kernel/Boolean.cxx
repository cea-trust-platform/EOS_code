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




#include <stdlib.h>
// Boolean
#include "Language/API/Boolean.hxx"

// Class Boolean 

namespace NEPTUNE
{
  Boolean::Boolean(const Boolean &right)
  {
    value=right.value;
  }

  Boolean::Boolean ()
  {
    value=False;
  }

  Boolean::Boolean (int i)
  {
    if(i==0)
      value=False;
    else
      value=True;
  }


  Boolean::~Boolean()
  {
  }


  Boolean & Boolean::operator=(const Boolean &right)
  {
    value=right.value;
    return *this;
  }


  int Boolean::operator==(const Boolean &right) const
  {
    return value==right.value;
  }

  int Boolean::operator!=(const Boolean &right) const
  {
    return value!=right.value;
  }


  ostream & operator<<(ostream &stream,const Boolean &right)
  {
    if (right.value==True) stream << "True";
    else stream << "False";
    return stream;
  }

  istream & operator>>(istream &stream,Boolean &object)
  {
    char str;
    stream>> str;
    if(str=='T') object.value=True;
    else if(str=='F') object.value=False;
    else
      {
	cerr << "An error occured while reading a Boolean " << endl
	     << " We read " << str << " and where waiting one string from "
	     << " T F " << endl;
	exit(-1);
      }
    return stream;
  }
}
