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



#include "StringTokenizer.hxx"

StringTokenizer::StringTokenizer(const std::string& _str, const std::string& _delim)
{
   if ((_str.length() == 0) || (_delim.length() == 0)) return ;

   token_str = _str ;
   delim     = _delim ;

   //  Remove sequential delimiter
   unsigned int curr_pos = 0 ;

   while(true)
      { if ((curr_pos = token_str.find(delim,curr_pos)) != (unsigned int)std::string::npos)
           { curr_pos += delim.length() ;
             while(token_str.find(delim,curr_pos) == curr_pos)
                { token_str.erase(curr_pos,delim.length()) ;
                }
           }
        else
           break ;
      }

   // Trim leading delimiter
   if (token_str.find(delim,0) == 0)  token_str.erase(0,delim.length()) ;

   // Trim ending delimiter
   curr_pos = 0 ;
   if ((curr_pos = token_str.rfind(delim)) != (unsigned int)std::string::npos)
      { if (curr_pos != (token_str.length() - delim.length()))  return ;
        token_str.erase(token_str.length() - delim.length(),delim.length()) ;
      }

};

int StringTokenizer::countTokens()
{
  // unsigned int prev_pos = 0 ;
  int num_tokens = 0 ;

  if (token_str.length() > 0)
     { num_tokens = 0 ;
       unsigned int curr_pos = 0 ;
       while(true)
          { if ((curr_pos = token_str.find(delim,curr_pos)) != (unsigned int)std::string::npos)
               { num_tokens++ ;
                 // prev_pos  = curr_pos ;
                 curr_pos += delim.length() ;
               }
            else
               break ;
          }
       return ++num_tokens ;
     }
  else
     return 0 ;

};


bool StringTokenizer::hasMoreTokens()
{ return (token_str.length() > 0) ;
};


std::string StringTokenizer::nextToken()
{
  if (token_str.length() == 0)  return "" ;

  std::string  tmp_str = "" ;
  unsigned int pos     = token_str.find(delim,0) ;

  if (pos != (unsigned int)std::string::npos)
     { tmp_str   = token_str.substr(0,pos);
       token_str = token_str.substr(pos+delim.length(),token_str.length()-pos) ;
     }
  else
     { tmp_str   = token_str.substr(0,token_str.length()) ;
       token_str = "";
     }

  return tmp_str ;
};


int StringTokenizer::nextIntToken()
{ return atoi(nextToken().c_str()) ;
};


double StringTokenizer::nextFloatToken()
{ return atof(nextToken().c_str()) ;
};


std::string StringTokenizer::nextToken(const std::string& delimiter)
{
  if (token_str.length() == 0)  return "" ;

  std::string  tmp_str ="" ;
  unsigned int pos  = token_str.find(delimiter,0) ;

  if (pos != (unsigned int)std::string::npos)
     { tmp_str   = token_str.substr(0,pos) ;
       token_str = token_str.substr(pos+delimiter.length(),token_str.length()-pos) ;
     }
  else
     { tmp_str   = token_str.substr(0,token_str.length()) ;
       token_str = "" ;
     }

  return tmp_str ;
};


std::string StringTokenizer::remainingString()
{ return token_str;
};


std::string StringTokenizer::filterNextToken(const std::string& filterStr)
{ std::string  tmp_str    = nextToken() ;
  unsigned int currentPos = 0 ;

  while((currentPos = tmp_str.find(filterStr,currentPos)) != (unsigned int)std::string::npos)
     { tmp_str.erase(currentPos,filterStr.length()) ;
     }

  return tmp_str ;
};
