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



#ifndef CATHARE2_Sodium_F77_HXX
#define CATHARE2_Sodium_F77_HXX

extern "C" {
  void F77DECLARE(c2_ftsatpna)(int&, double&, 
                            double&, double&, double&,
                            double&, double&, 
                            double&, double&,
                            double&, double&,
                            double&, double&, 
                            double&, double&,
                            double&, double&) ;
  void F77DECLARE(c2_fhliqna)(int&, double&, double&, 
                           double&, double&, double&, 
                           double&, double&, double&,
                           double&, double&, double&,
                           double&, double&, double&, const int&) ;
  void F77DECLARE(c2_usetvd)(int&, double&, double&) ;
  void F77DECLARE(c2_fhvapna)(int&, int&, int&, 
                           double&, double&, 
                           double&, double&, double&, double&, 
                           double&, double&, double&,
                           double&, double&, double&,
                           double&, double&, double&,
                           double&, double&, const int&) ;
  void F77DECLARE(c2_fhxcp)(int&, int&, double&, double&,
                         double&, double&, double&, 
                         double&, double&, double&,
                         double&, double&) ;
  void F77DECLARE(c2_fhxsum)(int&, int&, const int&, double&,
                          double&, double&, double&, double&, 
                          double&, double&, double&, double&, double&,
                          double&, double&) ;
  void F77DECLARE(c2_fhinitna)(int&, double&, double&, double&,
                            double&, double&, double&, double&,
                            double&, double&, double&, double&, double&,
                            double&, double&, double&, double&,
                            double&, double&, double&, double&,
                            double&, double&, double&, double&, 
                            double&, double&, double&, double&, double&) ;
  void F77DECLARE(c2_fhvpna)(int&, int&, double&,
                          double&, double&,
                          double&, double&, double&, double&,
                          double&, double&, double&,
                          double&, double&, double&,
                          double&, double&, double&,
                          double&, double&, const int&) ;
  void F77DECLARE(c2_fhsolv)(int&, int&, double&,
                          double&, double&,
                          double&, double&, double&, double&,
                          double&, double&, double&,
                          double&, double&, double&, double&,
                          double&, double&,
                          int&, double&, double&) ;
  void F77DECLARE(c2_fhgas)(int&, double&, double&,
                         double&, double&,
                         double&, double&, double&, double&,
                         double&, double&, double&,
                         double&, double&, double&, double&, double&,
                         double&, double&, double&, double&,
                         double&, double&, double&, double&,
                         double&, double&, double&,
                         double&, double&, double&, double&) ;
  void F77DECLARE(c2_fhgax)(int&, double&, double&, double&,
                         double&, double&, double&, double&,
                         double&, double&, double&,
                         double&, double&, double&, double&,
                         double&, double&,
                         double&, double&, double&, double&,
                         double&, double&, double&, double&,
                         double&, double&,
                         double&, double&, double&,
                         double&, double&, double&, double&) ;
  void F77DECLARE(c2_fhliqana)(int&,
                            double&, double&, double&,
                            double&, double&, double&,
                            double&, double&, double&) ;
  void F77DECLARE(c2_fhvapana)(int&, 
                            double&, 
                            double&, double&,
                            double&, double&, double&,
                            double&, double&, double&,
                            double&, double&) ;
  void F77DECLARE(c2_fhbasa)(int&, double&, double&,
                          double&, double&, double&, double&,
                          double&, double&, double&, double&) ;
  void F77DECLARE(c2_fhxinit)(int&, double&, double&, double&, double&,
                           double&, double&, double&,
                           double&, double&, double&, double&) ;
  void F77DECLARE(c2_fhgaq)(int&, int&,
                         double&, int&, double&, double&,
                         double&, double&, double&,
                         double&, double&, double&, double&,
                         double&, double&, double&, double&,
                         double&, double&, double&, int&) ;
  void F77DECLARE(c2_fhgaphi)(int&, int&, double&, double&,
                           double&, double&, 
                           double&, double&,
                           double&, double&,
                           double&, double&,
                           double&, double&, double&,
                           double&, double&,
                           double&, double&) ;
  void F77DECLARE(c2_fhgasa)(int&, int&,
                          int&, double&, double&,
                          double&, double&, double&,
                          double&, double&,
                          double&, double&,
                          double&, double&, double&,
                          double&, double&, double&,
                          double&, double&, double&,
                          double&, double&,
                          double&, double&, double&,
                          double&, double&, double&) ;
  void F77DECLARE(c2_fhgaxphi)(int&, int&, 
                            double&, double&, 
                            double&, double&, 
                            double&, double&,
                            double&, double&,
                            double&, double&, double&,
                            double&, double&, 
                            double&, double&, 
                            double&, double&, 
                            double&) ;
  void F77DECLARE(c2_fhgaxa)(int&, int&,
                          int&, double&, double&,
                          double&, double&, double&, 
                          double&, double&, double&, double&, double&,
                          double&, double&, 
                          double&, double&,
                          double&, double&, double&,
                          double&, double&, double&,
                          double&, double&, double&,
                          double&, double&,
                          double&, double&, 
                          double&, double&,
                          double&, double&) ;
  void F77DECLARE(c2_fhdiga)(int&, int&,
                          int&,
                          double&, double&, double&, double&,
                          double&, double&, double&, double&, 
                          double&, double&, double&,
                          double&, double&, double&,
                          double&, double&, double&, double&,
                          double&, double&, double&, 
                          double&, double&, double&,
                          double&, 
                          double&, double&, double&,
                          double&, double&, double&,
                          double&, double&, double&,
                          double&, double&,
                          double&, double&, double&) ;
  void F77DECLARE(c2_fhdixa)(int&, int&,
                          int&, double&,
                          double&, double&,
                          double&, double&, double&,
                          double&, double&, double&, 
                          double&, double&,
                          double&, double&,
                          double&, double&,
                          double&, double&, double&,
                          double&, double&, double&,
                          double&, double&,
                          double&, double&, double&,
                          double&, double&, double&,
                          double&, double&, double&,
                          double&, 
                          double&,
                          double&, double&,
                          double&, double&,
                          double&,
                          double&, double&,
                          double&, double&,
                          double&, double&) ;
  void F77DECLARE(c2_fhdixxa)(int&, int&,
                           int&, 
                           double&, double&, double&,
                           double&, double&,
                           double&, double&,
                           double&, double&) ;
  void F77DECLARE(c2_fhcomp)(int&, int&, double&, double&, double&, double&, double&,
                          double&, double&) ;
  void F77DECLARE(c2_fpsattna)(double&,
                            double&, double&, double&,
                            double&, double&, double&, double&,
                            double&, double&, double&, double&,
                            double&, double&, double&, double&) ;
  void F77DECLARE(c2_ftliqna)(int&, double&, double&,
                           double&, double&, double&,
                           double&, double&, double&,
                           double&, double&, double&) ;
  void F77DECLARE(c2_ftvapna)(int&, int&, int&,
                           double&, double&,
                           double&, double&, double&, double&,
                           double&, double&, 
                           double&, double&, double&, 
                           double&, double&, double&, double&) ;
}

#endif
