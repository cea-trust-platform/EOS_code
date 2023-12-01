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



#ifndef Thetis_HXX
#define Thetis_HXX


static const int CN_NB_MAX_ERR = 25;

extern "C" {

  int TVLOAD (int no_fluide, char **acf_nom_fichier, double titre);

  int TVINIT(int nb_max_fluides,int mode_calcul_coefs);
  int TVPLIM(int no_fluide,int mode_calcul,double *ar_r);

  int TVALPT(int  no_fluide,int n,double *ar_1,double *ar_2,double *ar_3, double *ar_r,int *ai_cr );
  int TVCPPT(int  no_fluide,int n,double *ar_1,double *ar_2,double *ar_3, double *ar_r,int *ai_cr );
  int TVETPT(int  no_fluide,int n,double *ar_1,double *ar_2,double *ar_3, double *ar_r,int *ai_cr );
  int TVHHPT(int  no_fluide,int n,double *ar_1,double *ar_2,double *ar_3, double *ar_r,int *ai_cr );
  int TVROPT(int  no_fluide,int n,double *ar_1,double *ar_2,double *ar_3, double *ar_r,int *ai_cr );
  int TVSSPT(int  no_fluide,int n,double *ar_1,double *ar_2,double *ar_3, double *ar_r,int *ai_cr );
  int TVYYPT(int  no_fluide,int n,double *ar_1,double *ar_2,double *ar_r,int *ai_cr );
  int TVA2PT(int  no_fluide,int n,double *ar_1,double *ar_2,double *ar_3, double *ar_r,int *ai_cr );
  int TVBEPT(int  no_fluide,int n,double *ar_1,double *ar_2,double *ar_3, double *ar_r,int *ai_cr );

  int TVALPH(int  no_fluide,int n,double *ar_1,double *ar_2,double *ar_r,int *ai_cr );
  int TVCPPH(int  no_fluide,int n,double *ar_1,double *ar_2,double *ar_r,int *ai_cr );
  int TVETPH(int  no_fluide,int n,double *ar_1,double *ar_2,double *ar_r,int *ai_cr );
  int TVTTPH(int  no_fluide,int n,double *ar_1,double *ar_2,double *ar_r,int *ai_cr );
  int TVROPH(int  no_fluide,int n,double *ar_1,double *ar_2,double *ar_r,int *ai_cr );
  int TVSSPH(int  no_fluide,int n,double *ar_1,double *ar_2,double *ar_r,int *ai_cr );
  int TVYYPH(int  no_fluide,int n,double *ar_1,double *ar_2,double *ar_r,int *ai_cr );
  int TVA2PH(int  no_fluide,int n,double *ar_1,double *ar_2,double *ar_r,int *ai_cr );
  int TVBEPH(int  no_fluide,int n,double *ar_1,double *ar_2,double *ar_r,int *ai_cr );


  int TVALPS(int  no_fluide,int n,double *ar_1,double *ar_2,double *ar_r,int *ai_cr );
  int TVCPPS(int  no_fluide,int n,double *ar_1,double *ar_2,double *ar_r,int *ai_cr );
  int TVETPS(int  no_fluide,int n,double *ar_1,double *ar_2,double *ar_r,int *ai_cr );
  int TVTTPS(int  no_fluide,int n,double *ar_1,double *ar_2,double *ar_r,int *ai_cr );
  int TVROPS(int  no_fluide,int n,double *ar_1,double *ar_2,double *ar_r,int *ai_cr );
  int TVHHPS(int  no_fluide,int n,double *ar_1,double *ar_2,double *ar_r,int *ai_cr );
  int TVYYPS(int  no_fluide,int n,double *ar_1,double *ar_2,double *ar_r,int *ai_cr );
  int TVA2PS(int  no_fluide,int n,double *ar_1,double *ar_2,double *ar_r,int *ai_cr );
  int TVBEPS(int  no_fluide,int n,double *ar_1,double *ar_2,double *ar_r,int *ai_cr );

  int TVERROR(int istat);

  int TVSSSA(int  no_fluide,int n,double *ar_1,double *ar_r1,double *ar_r2,int *ai_cr );
  int TVHHSA(int  no_fluide,int n,double *ar_1,double *ar_r1,double *ar_r2,int *ai_cr );
  int TVROSA(int  no_fluide,int n,double *ar_1,double *ar_r1,double *ar_r2,int *ai_cr );
  int TVCPSA(int  no_fluide,int n,double *ar_1,double *ar_r1,double *ar_r2,int *ai_cr );
  int TVETSA(int  no_fluide,int n,double *ar_1,double *ar_r1,double *ar_r2,int *ai_cr );
  int TVALSA(int  no_fluide,int n,double *ar_1,double *ar_r1,double *ar_r2,int *ai_cr );
  int TVSIGP(int  no_fluide,int n,double *ar_1,double *ar_r,int *ai_cr );
  int TVTTSA(int  no_fluide,int n,double *ar_1,double *ar_r,int *ai_cr );
  int TVPTSA(int  no_fluide,int n,double *ar_1,double *ar_r,int *ai_cr );
  int TVPSSA(int  no_fluide,int n,double *ar_1,double *ar_r1,double *ar_r2,int *ai_cr );
  int TVPHSA(int  no_fluide,int n,double *ar_1,double *ar_r1,double *ar_r2,int *ai_cr );


  int TVSETP(int  no_fluide,int n,double *ar_1,int *ai_cr );
  int TVSSSM(int  n,double *ar_r1,double *ar_r2);
  int TVTTSM(int  n,double *ar_r1);
  int TVSIGSM(int  n,double *ar_r1);
  int TVROSM(int  n,double *ar_r1,double *ar_r2);
  int TVCPSM(int  n,double *ar_r1,double *ar_r2);
  int TVETSM(int  n,double *ar_r1,double *ar_r2);
  int TVALSM(int  n,double *ar_r1,double *ar_r2);

}

#endif
