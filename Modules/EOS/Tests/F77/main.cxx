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



#include "system/arch.h"
#include "Language/API/F77Language.hxx"
#include "Language/API/Language.hxx"
#include "EOS/API/EOS.hxx"
#include "EOS/API/EOS_Fields.hxx"
#include "EOS/API/EOS_Error_Field.hxx"
#include "EOS/API/EOS_Config.hxx"


using namespace NEPTUNE;
static EOS* liquid;

// f_compute_h_pt --> compute(p,T,h)
// ---------------------------------

extern "C"
{
void F77DECLARE(f_compute_h_pt)(int& p, int& T, int& h);
}
void F77NAME(f_compute_h_pt)(int& p, int& T, int& h)
{
  cout << "methode  f_compute_h_pT " << endl;

  ArrOfDouble& xp=NEPTUNE_ArrOfDouble(p);
  ArrOfDouble& xT=NEPTUNE_ArrOfDouble(T);
  ArrOfDouble& xh=NEPTUNE_ArrOfDouble(h);

  int sz=xp.size();
  assert(xT.size()==sz);
  assert(xh.size()==sz);
 
  EOS_Field pres("pressure","p",xp);
  EOS_Field temp("temperature","T",xT);
  EOS_Field enthal("enthalpy","h",xh);

  ArrOfInt ierr(sz);
  EOS_Error_Field err(ierr);

  liquid->compute(pres,temp,enthal,err);
  //liquid->compute_h_pT(xp[0],xT[0],xh[0]);

  for(int i=0; i<sz; i++) 
    {
      cout <<" in  "<< pres.get_property_title()   <<" "<<pres[i]   << endl ;
      cout <<" in  "<< temp.get_property_title()   <<" "<<temp[i]   << endl ;	
      cout <<" out "<< enthal.get_property_title() <<" "<<enthal[i] << endl ;
    }
}

//
// f_compute_fields_ph --> compute(p,h,out)
// ----------------------------------------

extern "C"
{
void F77DECLARE(f_compute_fields_ph)(int& p, int& h, int& T, int& cp);
}
void F77NAME(f_compute_fields_ph)(int& p, int& h, int& T, int& cp)
{
  cout << "methode  f_compute_fields_ph " << endl;

  ArrOfDouble& xp=NEPTUNE_ArrOfDouble(p);
  ArrOfDouble& xh=NEPTUNE_ArrOfDouble(h);
  ArrOfDouble& xT=NEPTUNE_ArrOfDouble(T);
  ArrOfDouble& xcp=NEPTUNE_ArrOfDouble(cp);

  int sz=xp.size();
  assert(xh.size()==sz);
  assert(xT.size()==sz);
  assert(xcp.size()==sz);
  
  int nin=2;
  EOS_Fields input(nin);
  input[0]=EOS_Field("pressure","p",xp);
  input[1]=EOS_Field("enthalpy","h",xh);

  int nout=2;
  EOS_Fields output(nout);
  output[0]=EOS_Field("temperature","T",xT);
  output[1]=EOS_Field("cp","cp",xcp);  
  
  ArrOfInt ierr(sz);
  EOS_Error_Field err(ierr);

  liquid->compute(input,output,err);

  for(int i=0; i<sz; i++) 
    {
      for(int j=0; j<nin; j++) 	
	cout <<" in "<<input[j].get_property_title()<<" "<<input[j][i]<<endl;
      for(int j=0; j<nout; j++) 	
	cout <<" out "<<output[j].get_property_title()<<" "<<output[j][i]<<endl;
    }
}


// main
// ----

extern "C"
{
void F77DECLARE(f77main)();
}
int main()
{

  Language_init(); 

  // -- definition de l'objet EOS
#ifdef WITH_PLUGIN_CATHARE
  liquid=new EOS("EOS_Cathare","WaterLiquid");
#else
  liquid=new EOS("EOS_PerfectGas","Hydrogen");
#endif
  // -- appel programme principal fortran --
  cout << "APPEL F77" << endl << endl;
  F77NAME(f77main)();
  cout << endl << "FIN F77" << endl;
  
  Language_finalize();
  return 0;
}
