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



/*
 ***********************************************************************
 * Class: StringTokenizer                                              *
 * By Arash Partow - 2000                                              *
 * URL: http://www.partow.net/programming/stringtokenizer/index.html   *
 *                                                                     *
 * Copyright Notice:                                                   *
 * Free use of this library is permitted under the guidelines and      *
 * in accordance with the most current version of the Common Public    *
 * License.                                                            *
 * http://www.opensource.org/licenses/cpl.php                          *
 *                                                                     *
 ***********************************************************************
*/



#ifndef INCLUDE_STRINGTOKENIZER_H
#define INCLUDE_STRINGTOKENIZER_H


#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>


class StringTokenizer
{

   public:

    StringTokenizer(const std::string& _str, const std::string& _delim);
   ~StringTokenizer(){};

    int         countTokens();
    bool        hasMoreTokens();
    std::string nextToken();
    int         nextIntToken();
    double      nextFloatToken();
    std::string nextToken(const std::string& delim);
    std::string remainingString();
    std::string filterNextToken(const std::string& filterStr);

   private:

    std::string  token_str;
    std::string  delim;

};

#endif
