C ****************************************************************************
C Copyright (c) 2023, CEA
C All rights reserved.
C
C Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
C 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
C 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
C 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
C
C THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
C IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
C OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
C
C ****************************************************************************/

      SUBROUTINE C2_NINCCOPY ( NAME )
      INCLUDE 'C2_GLOBAL.H'
      INCLUDE 'C2_MAXINC.H'
      INCLUDE 'C2_READIM.H'
      INCLUDE 'C2_RCOMNO.H'
      CHARACTER*8 NAME
      NINC(1,1) = NAME
c     \todo If the optional keyword POLYNCPT is used, IPOLYCP=1
      IPOLYCP(1,1) = 0
      RETURN
      END
