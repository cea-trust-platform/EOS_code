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



#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
using std::cerr;
using std::endl;
#include <string>
using std::string;
#include "EOS/API/EOS_Config.hxx"
#include "EOS/API/EOS_eosdatadir.hxx"

string eos_data_dir ;
int iret_eos_data_dir = f_eosdatadir(eos_data_dir) ;

int f_eosdatadir(string &rep)
{ int iret = 0 ;

#ifdef EOS_DATA_DIR
  rep=EOS_DATA_DIR ;
#else
  cerr << " Error EOS : EOS DATA Directory is not set at EOS Installation" << endl ;
  iret = 1 ;
#endif
  char* pshell ;
  if      ((pshell=getenv("NEPTUNE_EOS_DATA")))
     rep = pshell ;
  else if ((pshell=getenv("USER_EOS_DATA")))
     rep = pshell ;

  // file or directory exist, it is a directory, it is writable by user
  struct stat sttamp ;
  if (   (stat(rep.c_str(), &sttamp) == -1)
      || ((sttamp.st_mode & S_IFMT)  != S_IFDIR)
      || ((sttamp.st_mode & S_IRWXU) != S_IRWXU) )
     { cerr << " Error EOS : EOS DATA Directory '" << rep << "' is not a writable directory" << endl ;
       iret = 2 ;
     }

  return iret ;
}
