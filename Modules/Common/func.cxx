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



#include "Common/func.hxx"
// 
// Comparison of two strings
// -------------------------
//
// input  : str1 : alphanumerical string to evaluate with extra characters
//          str2 : reference string in lower case or numerical data
//
// output : eostp_strcmp
//             0 : positive comparison
//             1 : negative comparison
int eostp_strcmp(const char* str1, const char* str2)
{
  short istr1 ;
  while (*str1)
     { istr1 = (short) *str1 ;
       if ((po1_almaj <= istr1) && (istr1 <= po2_almaj))
          { if (((char) (dec_alpha + istr1)) == *str2)
               str2++ ;
            else
               return 1 ;
          }
       else if ((po1_almin <= istr1) && (istr1 <= po2_almin))
          { if (*str1 == *str2)
               str2++ ;
            else
               return 1 ;
          }
       else if ((po1_numer <= istr1) && (istr1 <= po2_numer))
          { if (*str1 == *str2)
               str2++ ;
            else
               return 1 ;
          }
       str1++ ;
     }
  return 0 ;
}

// 
// Convert string : - uppercase to lowercase
// --------------   - numerical character unchanged
//                  - remove others characters 
// input  : strd 
// output : strr 
//
void eostp_strcov(const char* strd, char* strr)
{ short istrd ;
 
  while (*strd)
     { istrd = (short) *strd ;
       if      ((po1_almaj <= istrd) && (istrd <= po2_almaj))
          *(strr++) = (char) (dec_alpha + istrd) ;
       else if ((po1_almin <= istrd) && (istrd <= po2_almin))
          *(strr++) = (char) istrd ;
       else if ((po1_numer <= istrd) && (istrd <= po2_numer))
          *(strr++) = (char) istrd ;
       strd++ ;
     }
  *strr = '\0' ; 
}
