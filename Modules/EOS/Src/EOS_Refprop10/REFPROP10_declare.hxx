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
 * REFPROP10_declare.hxx
 *
 *  Created on: 23 avr. 2010
 *  
 */

#ifndef REFPROP10_DECLARE_HXX_
#define REFPROP10_DECLARE_HXX_

#include "EOS/API/EOS_Config.hxx"
#include "system/arch.h"


// COMMON type NCOMP
// M.F. struct str_ncomp { int nc; int ic; } ;


extern "C"
{
// -- COMMON NCOMP
// M.F.      extern struct str_ncomp F77DECLARE(ncomp) ;


// -- SETUP
//    input(nc,hfiles,hfmix,hrf)   output(ierr_10,herr_10)
      void F77DECLARE(setup)(int& nbcomp, const char* hfile, const char* hfmix, 
                             const char* hrf, int& ierr_10, char* herr_10);

// -- SETMIX
//    input(hmxnme,hfmix,hrf)      output(ncc,hfiles,x,ierr_10,herr_10)
      void F77DECLARE(setmix)(const char* hmxnme, const char* hfmix, 
                              const char* hrf, int& ncc, char hfiles[NBCOMPMAX][HC255],
                              double* x, int& ierr_10, char* herr_10);

// -- SETMOD
//    input(nc,htype,hmix,hcomp)   output(ierr_10,herr_10)
      void F77DECLARE(setmod)(int& nc,const char* htype,const char* hmix,
                              const char hcomp[NBCOMPMAX][HC3], int& ierr_10,char* herr_10);

// -- SETREF
//    input(hrf,ixflag,x0,h0,s0,t0,p0)   output(ierr_10,herr_10)
      void F77DECLARE(setref)(const char* hrf, int& ixflag, const double* x0, const double& h0,
                              const double& s0, const double& t0, const double& p0, int& ierr_10, char* herr_10);


// -- SETPATH
//    input(hpth)
      void F77DECLARE(setpath)(const char* hpth);


// -- INFO
//    input (nbcomp)       output(wmm, ttrp, tnbpt, tc, pc, dc, zc, acf, dip, rgas);
      void F77DECLARE(info)(int& nbcomp, double& wmm, double& ttrp, double& tnbpt, 
                            double& tc, double& pc, double& dc, double& zc, double& acf,
                            double& dip, double& rgas);

// -- NAME
//    input(nbcomp)        output(hname,hn80,hcas)
      void F77DECLARE(name)(int& nbcomp, char* hname, char* hn80, char* hcas);

// -- WMOL (molecular weight for a mixture of specified composition)
//    input(x)                 output(wmol)
      double F77DECLARE(wmol)(double* x);

// -- LIMITS
//    input(htyp,x)       output(tmin,tmax,Dmax,pmax)
      void F77DECLARE(limits)(char* htyp, double* x, double& tmin, double& tmax, 
                              double& Dmax, double& pmax);

// -- CRITP
//    input(x)            output(tcrit,pcrit,Dcrit,ierr_10,herr_10)
      void F77DECLARE(critp)(double* x, double& tcrit, double& pcrit,
                             double& Dcrit, int& ierr_10, char* herr_10);

// -- ENTHAL
//    input (t,rho,x)      output(h)
      void F77DECLARE(enthal)(double& t, double& rho, double* x, double& h);

// -- ENTRO
//    input (t,rho,x)      output(s)
      void F77DECLARE(entro)(double& t, double& rho, double* x, double& s);

// -- ENERGY
//    input (t,rho,x)      output(u)
      void F77DECLARE(energy)(double& t, double& rho, double* x, double& u);

// -- SATURATION SATT
//    input (t,x,kph)     output(p,rhol,rhov,xliq,xvap,ierr_10,herr_10)
      void F77DECLARE(satt)(double& t, double* x, int& kph, double& p, 
                            double& rhol, double& rhov, double& xliq,
                            double& xvapint, int& ierr_10, char* herr_10);

// -- SATURATION SATP
//    input (p,x,kph)     output(t,rhol,rhov,xliq,xvap,ierr_10,herr_10)
      void F77DECLARE(satp)(double& p, double* x, int& kph, double& t,
                            double& rhol, double& rhov, double& xliq, 
                            double& xvapint, int& ierr_10, char* herr_10);

// -- SATURATION CSATK
//    input (nbcomp,t,kph)     output(p,rho,csat,ierr_10,herr_10)
       void F77DECLARE(csatk)(int& nbcomp, double& t, int& kph, double& p, 
                              double& rho, double& csat, int& ierr_10, char* herr_10);
// -- CVCP
//    input (t,rho,x)   output(cv,cp)
      void F77DECLARE(cvcp)(double& t, double& rho, double* x, double& cv, 
                            double& cp);

// -- ph : PHFLSH
//    input (p,h,z)             output (t,D,Dl,Dv,x,y,q,e,s,cv,cp,w,ierr_10,herr_10)
      void F77DECLARE(phflsh)(double& p, double& h, double* z, double& t,
                              double& D, double& Dl, double& Dv, double* x, 
                              double* y, double& q, double& e, double& s, 
                              double& cv, double& cp, double& w, int& ierr_10,
                              char* herr_10);

// -- pT : TPFLSH
//    input (p,s,z)      output(t,D,Dl,Dv,x,y,q,e,h,Cv,Cp,w,ierr_10,herr_10)
      void F77DECLARE(psflsh)(double& p, double& s, double* z, double& t,
                              double& D, double& Dl, double& Dv, double* x, 
                              double* y, double& q, double& e, double& h, 
                              double& Cv, double& Cp, double& w, int& ierr_10, 
                              char* herr_10);

// -- ps : PSFLSH
//    input (t,p,z)      output(D,Dl,Dv,x,y,q,e,h,s,Cv,Cp,w,ierr_10,herr_10)
      void F77DECLARE(tpflsh)(double& t, double& p, double* z, double& D, 
                              double& Dl, double& Dv, double* x, double* y, 
                              double& q, double& e, double& h, double& s, 
                              double& Cv, double& Cp, double& w, int& ierr_10, 
                              char* herr_10);

// -- ps : PSFL1
//    input (p,s,z,kph,t,D)      output(t,D,ierr_10,herr_10)
      void F77DECLARE(psfl1)(double& p, double& s, double* z, int& kph, 
                             double& t, double& D, int& ierr_10, char* herr_10);

// -- TRNPRP
//    input (t,rho,x)    output(eta,tcx,ierr_10,herr_10)
      void F77DECLARE(trnprp)(double& t, double& rho, double* x, double& eta, 
                              double& tcx, int& ierr_10, char* herr_10);

// -- STN       compute surface tension (sigma)
//    input (t,rhol,rhov,xl,xv)        output (sigma,ierr_10,herr_10)
      void F77DECLARE(stn)(double& t, double& rhol, double& rhov, double* xl, 
                              double* xv, double& sigma, int& ierr_10, char* herr_10);

// -- pT: TPRHO
//    input (t,p,x,kph,kguess)    output(rho,ierr_10,herr_10)
      void F77DECLARE(tprho)(double& t, double& p, double* x, int& kph, int& kguess,
                             double& rho, int& ierr_10, char* herr_10);

// -- THERM (compute thermal quantities as a function of temperature, density, and compositions)
//    input (t,rho,x)             output(p,e,h,s,cv,cp,w,hjt)
      void F77DECLARE(therm)(double& t, double& rho, double* x, double& p, 
                             double& e, double& h, double& s, double& cv, 
                             double& cp, double& w, double& hjt);

// -- DHD1 (compute partial derivatives of enthalpy w.r.t. t, p, or rho at constant
//          t, p, or rho as a function of temperature, density, and composition)
//    input(t,rho,x)                      output(dhdt_d,dhdt_p,dhdd_t,dhdd_p,dhdp_t,dhdp_d)
      void F77DECLARE(dhd1)(double& t, double& rho, double* x, double& dhdt_d, 
                            double& dhdt_p, double& dhdd_t, double& dhdd_p, 
                            double& dhdp_t, double& dhdp_d);

// -- DERVPVT (compute dro/dP at constant temperature)
//    input(t,rho,x)              output(dpdd, d2tdpd)
      void F77DECLARE(dervpvt) (double &t, double &rho, double *x,
           double &dpdd, double &dpdt, double &d2pdd2, double &d2pdt2, double &d2pdtd,
           double &dddp, double &dddt, double &d2ddp2, double &d2ddt2, double &d2ddpt,
           double &dtdp, double &dtdd, double &d2tdp2, double &d2tdd2, double &d2tdpd);
 

// -- DPTSATK
//    input(nbcomp,t,kph)                     output(p,rho,csat,dpt,ierr_10,herr_10)
      void F77DECLARE(dptsatk)(int& nbcomp, double& t, int& kph, double& p, 
                              double& rho, double& csat, double& dpt, int& ierr_10, 
                              char* herr_10);

// -- AG (compute Helmholtz and Gibbs energies)
//    input(t,rho,x)              output(a,g)
      void F77DECLARE(ag)(double& t, double& rho, double* x, double& a, double& g);

// -- SPNDL (Find the spinodal densities for a given temperature)
//    input(t,x)                                                         output(rhol,rhov,ierr_10,herr_10)
      void F77DECLARE(liqspndl)(double& t, double* x, double& rhol, int& ierr_10, char* herr_10);
      void F77DECLARE(vapspndl)(double& t, double* x, double& rhov, int& ierr_10, char* herr_10);

//
      void F77DECLARE(therm2)(double& t, double& rho, double* x, double& p, 
                              double& e, double& h, double&s, double& cv, 
                              double &cp, double& w, double& Z, double& hjt, 
                              double& A, double& G, double& xkappa, double& beta, 
                              double& dPdrho, double& d2PdD2, double& dPT, 
                              double& drhodT, double& drhodP,double& spare1, 
                              double& spare2, double& spare3, double& spare4);

//
      void F77DECLARE(dpdd)(double& t, double& rho, double* x, double& dpdrho);

//
      void F77DECLARE(dpdt)(double& t, double& rho, double* x, double& dpt);

//
      void F77DECLARE(press)(double& t, double& rho, double* x, double& p);

//
      void F77DECLARE(phfl1)(double& p_refprop, double& h_refprop, double* x, 
                             int& kph, double& T, double& rho_refprop, int& ierr_10,
                             char* herr_10);

}
#endif /* REFPROP_DECLARE_HXX_ */
