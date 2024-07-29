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
 * REFPROP9_declare.hxx
 *
 *  Created on: 23 avr. 2010
 *  
 */

#ifndef REFPROP9_DECLARE_HXX_
#define REFPROP9_DECLARE_HXX_

#include "EOS/API/EOS_Config.hxx"
#include "system/arch.h"


// COMMON type NCOMP
// M.F. struct str_ncomp { int nc; int ic; } ;


extern "C"
{
// -- COMMON NCOMP
// M.F.      extern struct str_ncomp F77DECLARE(ncomp) ;


// -- SETUP
//    input(nc,hfiles,hfmix,hrf)   output(ierr,herr)
      void F77DECLARE(setup_rp9)(int& nbcomp, const char* hfile, const char* hfmix, 
                             const char* hrf, int& ierr, char* herr,
                             long, long, long, long);

// -- SETMIX
//    input(hmxnme,hfmix,hrf)      output(ncc,hfiles,x,ierr,herr)
      void F77DECLARE(setmix_rp9)(const char* hmxnme, const char* hfmix, 
                              const char* hrf, int& ncc, char hfiles[NBCOMPMAX][HC255],
                              double* x, int& ierr, char* herr,
                              long, long, long, long);

// -- SETMOD
//    input(nc,htype,hmix,hcomp)   output(ierr,herr)
      void F77DECLARE(setmod_rp9)(int& nc,const char* htype,const char* hmix,
                              const char hcomp[NBCOMPMAX][HC3], int& ierr,char* herr,
                              long, long, long, long);

// -- SETREF
//    input(hrf,ixflag,x0,h0,s0,t0,p0)   output(ierr,herr)
      void F77DECLARE(setref_rp9)(const char* hrf, int& ixflag, const double* x0, const double& h0,
                              const double& s0, const double& t0, const double& p0, int& ierr, char* herr,
                              long, long);

// -- SETPATH
//    input(hpth)
      void F77DECLARE(setpath_rp9)(const char* hpth, long);


// -- INFO
//    input (nbcomp)       output(wmm, ttrp, tnbpt, tc, pc, dc, zc, acf, dip, rgas);
      void F77DECLARE(info_rp9)(int& nbcomp, double& wmm, double& ttrp, double& tnbpt, 
                            double& tc, double& pc, double& dc, double& zc, double& acf,
                            double& dip, double& rgas);

// -- NAME
//    input(nbcomp)        output(hname,hn80,hcas)
      void F77DECLARE(name_rp9)(int& nbcomp, char* hname, char* hn80, char* hcas,
                                long, long, long);

// -- WMOL (molecular weight for a mixture of specified composition)
//    input(x)                 output(wmol)
      double F77DECLARE(wmol_rp9)(double* x);

// -- LIMITS
//    input(htyp,x)       output(tmin,tmax,Dmax,pmax)
      void F77DECLARE(limits_rp9)(char* htyp, double* x, double& tmin, double& tmax, 
                              double& Dmax, double& pmax);

// -- CRITP
//    input(x)            output(tcrit,pcrit,Dcrit,ierr,herr)
      void F77DECLARE(critp_rp9)(double* x, double& tcrit, double& pcrit,
                             double& Dcrit, int& ierr, char* herr, long);

// -- ENTHAL
//    input (t,rho,x)      output(h)
      void F77DECLARE(enthal_rp9)(double& t, double& rho, double* x, double& h);

// -- ENTRO
//    input (t,rho,x)      output(s)
      void F77DECLARE(entro_rp9)(double& t, double& rho, double* x, double& s);

// -- ENERGY
//    input (t,rho,x)      output(u)
      void F77DECLARE(energy_rp9)(double& t, double& rho, double* x, double& u);

// -- SATURATION SATT
//    input (t,x,kph)     output(p,rhol,rhov,xliq,xvap,ierr,herr)
      void F77DECLARE(satt_rp9)(double& t, double* x, int& kph, double& p, 
                            double& rhol, double& rhov, double& xliq,
                            double& xvapint, int& ierr, char* herr, long);

// -- SATURATION SATP
//    input (p,x,kph)     output(t,rhol,rhov,xliq,xvap,ierr,herr)
      void F77DECLARE(satp_rp9)(double& p, double* x, int& kph, double& t,
                            double& rhol, double& rhov, double& xliq, 
                            double& xvapint, int& ierr, char* herr, long);

// -- SATURATION CSATK
//    input (nbcomp,t,kph)     output(p,rho,csat,ierr,herr)
       void F77DECLARE(csatk_rp9)(int& nbcomp, double& t, int& kph, double& p, 
                              double& rho, double& csat, int& ierr, char* herr, long);
// -- CVCP
//    input (t,rho,x)   output(cv,cp)
      void F77DECLARE(cvcp_rp9)(double& t, double& rho, double* x, double& cv, 
                            double& cp);

// -- ph : PHFLSH
//    input (p,h,z)             output (t,D,Dl,Dv,x,y,q,e,s,cv,cp,w,ierr,herr)
      void F77DECLARE(phflsh_rp9)(double& p, double& h, double* z, double& t,
                              double& D, double& Dl, double& Dv, double* x, 
                              double* y, double& q, double& e, double& s, 
                              double& cv, double& cp, double& w, int& ierr,
                              char* herr, long);

// -- pT : TPFLSH
//    input (p,s,z)      output(t,D,Dl,Dv,x,y,q,e,h,Cv,Cp,w,ierr,herr)
      void F77DECLARE(psflsh_rp9)(double& p, double& s, double* z, double& t,
                              double& D, double& Dl, double& Dv, double* x, 
                              double* y, double& q, double& e, double& h, 
                              double& Cv, double& Cp, double& w, int& ierr, 
                              char* herr, long);

// -- ps : PSFLSH
//    input (t,p,z)      output(D,Dl,Dv,x,y,q,e,h,s,Cv,Cp,w,ierr,herr)
      void F77DECLARE(tpflsh_rp9)(double& t, double& p, double* z, double& D, 
                              double& Dl, double& Dv, double* x, double* y, 
                              double& q, double& e, double& h, double& s, 
                              double& Cv, double& Cp, double& w, int& ierr, 
                              char* herr, long);

// -- ps : PSFL1
//    input (p,s,z,kph,t,D)      output(t,D,ierr,herr)
      void F77DECLARE(psfl1_rp9)(double& p, double& s, double* z, int& kph, 
                             double& t, double& D, int& ierr, char* herr, long);

// -- TRNPRP
//    input (t,rho,x)    output(eta,tcx,ierr,herr)
      void F77DECLARE(trnprp_rp9)(double& t, double& rho, double* x, double& eta, 
                              double& tcx, int& ierr, char* herr, long);

// -- SURTEN    compute surface tension (sigma)
//    input (t,rhol,rhov,xl,xv)        output (sigma,ierr,herr)
      void F77DECLARE(surten_rp9)(double& t, double& rhol, double& rhov, double* xl, 
                              double* xv, double& sigma, int& ierr, char* herr, long);

// -- pT: TPRHO
//    input (t,p,x,kph,kguess)    output(rho,ierr,herr)
      void F77DECLARE(tprho_rp9)(double& t, double& p, double* x, int& kph, int& kguess,
                             double& rho, int& ierr, char* herr, long);

// -- THERM (compute thermal quantities as a function of temperature, density, and compositions)
//    input (t,rho,x)             output(p,e,h,s,cv,cp,w,hjt)
      void F77DECLARE(therm_rp9)(double& t, double& rho, double* x, double& p, 
                             double& e, double& h, double& s, double& cv, 
                             double& cp, double& w, double& hjt);

// -- DHD1 (compute partial derivatives of enthalpy w.r.t. t, p, or rho at constant
//          t, p, or rho as a function of temperature, density, and composition)
//    input(t,rho,x)                      output(dhdt_d,dhdt_p,dhdd_t,dhdd_p,dhdp_t,dhdp_d)
      void F77DECLARE(dhd1_rp9)(double& t, double& rho, double* x, double& dhdt_d, 
                            double& dhdt_p, double& dhdd_t, double& dhdd_p, 
                            double& dhdp_t, double& dhdp_d);

// -- DDDT (compute dro/dt at constant pressure)
//    input(t,rho,x)              output(drhodt)
      void F77DECLARE(dddt_rp9)(double& t, double& rho, double* x, double& drhodt);

// -- DDDP (compute dro/dP at constant temperature)
//    input(t,rho,x)              output(drhodp)
      void F77DECLARE(dddp_rp9)(double& t, double& rho, double* x, double& drhodp);

// -- DPTSATK
//    input(nbcomp,t,kph)                     output(p,rho,csat,dpt,ierr,herr)
      void F77DECLARE(dptsatk_rp9)(int& nbcomp, double& t, int& kph, double& p, 
                              double& rho, double& csat, double& dpt, int& ierr, 
                              char* herr, long);

// -- AG (compute Helmholtz and Gibbs energies)
//    input(t,rho,x)              output(a,g)
      void F77DECLARE(ag_rp9)(double& t, double& rho, double* x, double& a, double& g);

// -- SPNDL (Find the spinodal densities for a given temperature)
//    input(t,x)                                                         output(rhol,rhov,ierr,herr)
      void F77DECLARE(liqspndl_rp9)(double& t, double* x, double& rhol, int& ierr, char* herr, long);
      void F77DECLARE(vapspndl_rp9)(double& t, double* x, double& rhov, int& ierr, char* herr, long);

//
      void F77DECLARE(therm2_rp9)(double& t, double& rho, double* x, double& p, 
                              double& e, double& h, double&s, double& cv, 
                              double &cp, double& w, double& Z, double& hjt, 
                              double& A, double& G, double& xkappa, double& beta, 
                              double& dPdrho, double& d2PdD2, double& dPT, 
                              double& drhodT, double& drhodP,double& spare1, 
                              double& spare2, double& spare3, double& spare4);

//
      void F77DECLARE(dpdd_rp9)(double& t, double& rho, double* x, double& dpdrho);

//
      void F77DECLARE(dpdt_rp9)(double& t, double& rho, double* x, double& dpt);

//
      void F77DECLARE(press_rp9)(double& t, double& rho, double* x, double& p);

//
      void F77DECLARE(phfl1_rp9)(double& p_refprop, double& h_refprop, double* x, 
                             int& kph, double& T, double& rho_refprop, int& ierr,
                             char* herr, long);

}
#endif /* REFPROP9_DECLARE_HXX_ */
