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

      SUBROUTINE F77MAIN()
c     ====================
c
      INTEGER          PP,PTL,PHL,PCPL,PTTL
      INTEGER          NCEL
      PARAMETER       (NCEL=3)
      DOUBLE PRECISION TP(NCEL),TTL(NCEL),THL(NCEL)
      DOUBLE PRECISION TCPL(NCEL),TTTL(NCEL)
      DOUBLE PRECISION TABSK
      PARAMETER       (TABSK=273.15)
c
      INTEGER          LC,NL
      PARAMETER       (NL=4)
      INTEGER          LP,LTL,LHL,LCPL
      DOUBLE PRECISION A(NL*NCEL)
c
c     
c     == Test 1 ==
c
c     -- init
      DO IEL=1,NCEL
         TP(IEL)   = 1.D5
         TTL(IEL)  = 50. + 5.*IEL + TABSK
         THL(IEL)  = -1.
         TCPL(IEL) = -1.
         TTTL(IEL) = -1.
      ENDDO
c
c     -- create ArrOfDouble
      CALL adopt_array_of_double(TP, NCEL, PP)
      CALL adopt_array_of_double(TTL, NCEL, PTL)
      CALL adopt_array_of_double(THL, NCEL, PHL)
      CALL adopt_array_of_double(TCPL, NCEL, PCPL)
      CALL adopt_array_of_double(TTTL, NCEL, PTTL)
c
c     -- call methods
      CALL f_compute_h_pt(PP,PTL,PHL)
      CALL f_compute_fields_ph(PP,PHL,PTTL,PCPL)

c     -- delete
      CALL delete_object(PP)
      CALL delete_object(PTL)
      CALL delete_object(PHL)
      CALL delete_object(PCPL)
      CALL delete_object(PTTL)
c
c
c     == Test 2 ==
c
c     -- init
      LC   = 1
      LP   = LC
      LC   = LC + NCEL
      LHL  = LC
      LC   = LC + NCEL
      LTL  = LC
      LC   = LC + NCEL
      LCPL = LC
      LC   = LC + NCEL - 1
c
      IF (LC.LE.(NL*NCEL)) THEN
c
         DO IEL=1,NCEL
            A(LP+IEL-1)  = 1.D5
            A(LTL+IEL-1) = 50. + 5.*IEL + TABSK
            A(LHL+IEL-1) = -1.
            A(LCPL+IEL-1) = -1.
            TTTL(IEL) = -1.
         ENDDO
c
c     -- create ArrOfDouble
         CALL adopt_array_of_double(A(LP)  , NCEL, PP)
         CALL adopt_array_of_double(A(LTL) , NCEL, PTL)
         CALL adopt_array_of_double(A(LHL) , NCEL, PHL)
         CALL adopt_array_of_double(A(LCPL), NCEL, PCPL)
         CALL adopt_array_of_double(TTTL   , NCEL, PTTL)
c
c     -- call methods
         CALL f_compute_h_pt(PP,PTL,PHL)
         CALL f_compute_fields_ph(PP,PHL,PTTL,PCPL)
c
c     -- delete
         CALL delete_object(PP)
         CALL delete_object(PTL)
         CALL delete_object(PHL)
         CALL delete_object(PCPL)
         CALL delete_object(PTTL)
c
      ENDIF
c
      RETURN 
      END
