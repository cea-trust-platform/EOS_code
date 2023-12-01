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

      SUBROUTINE C2_GETFLDPROP (I
     &                      , FLDXPM,FLDXHLM,FLDXHVM,FLDXTLM,FLDXTGM
     1                      , FLDXPP,FLDXHLP,FLDXHVP,FLDXTLP,FLDXTGP
     2                      , FLDXTGPic,FLDXSIP
     3                      , FLUIDM, FLUIDR, FLUIDDV
     4                      , FLDCPCST
     5                      , FLDEPSL,  FLDEPSG, FLDEPSR
     6                      , FLDEPSP,  FLDEPSHL, FLDEPSHG, FLDEPSTL
     6                      , FLDEPSTG, FLDEPSAL, FLDEPSMX, FLDEPSPX
     7                      , FLDEPSV,  FLDEPSTW, FLDEPSMI, FLDEPSMS
     8                      , FLDEPSEI, FLDEPSES, FLDUTOL , FLDUTOG
     9                      , FLDHREF, FLDTREF, FLDTREFK, FLDPCRI )
C
      IMPLICIT NONE
C
C_INPUT
      INTEGER I
      INCLUDE 'C2_GLOBAL.H'
      INCLUDE 'C2_LUTIL.H'
      INCLUDE 'C2_BOUNDS.H'
      INCLUDE 'C2_CONVRG.H'
C
C_OUTPUT
      DOUBLE PRECISION FLDXPM,FLDXHLM,FLDXHVM,FLDXTLM,FLDXTGM
     1                      , FLDXPP,FLDXHLP,FLDXHVP,FLDXTLP,FLDXTGP
     2                      , FLDXTGPic,FLDXSIP
     3                      , FLUIDM, FLUIDR, FLUIDDV
     4                      ,  FLDCPCST
      DOUBLE PRECISION FLDEPSL,  FLDEPSG, FLDEPSR
      DOUBLE PRECISION FLDEPSP,  FLDEPSHL, FLDEPSHG, FLDEPSTL, FLDEPSTG
     &               , FLDEPSAL
     1               , FLDEPSMX, FLDEPSPX, FLDEPSV,  FLDEPSTW
     2               , FLDEPSMI, FLDEPSMS, FLDEPSEI, FLDEPSES
     8               , FLDUTOL , FLDUTOG, FLDHREF, FLDTREF, FLDTREFK, FLDPCRI
C
      FLDXPM = XPM (I)
      FLDXHLM = XHLM (I)
      FLDXHVM = XHVM (I)
      FLDXTLM = XTLM (I)
      FLDXTGM = XTGM (I)
      FLDXPP = XPP (I)
      FLDXHLP = XHLP (I)
      FLDXHVP = XHVP (I)
      FLDXTLP = XTLP (I)
      FLDXTGP = XTGP (I)
      FLDXTGPic = XTGPic (I)
      FLDXSIP = XSIP (I)
      FLUIDR = FLDR (I)
      FLUIDM = FLDM (I)
      FLUIDDV = FLDDV (I)
      FLDHREF = HREF (I)
      FLDTREF = TREF (I)
      FLDTREFK = TREFK (I)
      FLDCPCST = CPCONST (I)
      FLDUTOL = UTOL (I)
      FLDUTOG = UTOG (I)
      FLDEPSL = EPSL (I)
      FLDEPSG = EPSG (I)
      FLDEPSR = EPSR (I)
      FLDEPSP = EPSP (I)
      FLDEPSHL = EPSHL (I)
      FLDEPSHG = EPSHG (I)
      FLDEPSTL = EPSTL (I)
      FLDEPSTG = EPSTG (I)
      FLDEPSAL = EPSAL (I)
      FLDEPSMX = EPSMX (I)
      FLDEPSPX = EPSPX (I)
      FLDEPSV = EPSV (I)
      FLDEPSTW = EPSTW (I)
      FLDEPSMI = EPSMI (I)
      FLDEPSMS = EPSMS (I)
      FLDEPSEI = EPSEI (I)
      FLDEPSES = EPSES (I)
      FLDHREF = HREF (I)
      FLDTREF = TREF (I)
      FLDTREFK = TREFK (I)
      FLDPCRI = XPCRI(I)
C
      RETURN
       END
C
