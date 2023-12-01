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



#include "Language/API/Language.hxx"
#include "Functions/API/Functions.hxx"

#include <fstream>

using namespace NEPTUNE;

static Strings var(2);
inline int rot(const AFunction& psi,
	AString& u,
	AString& v)
{
  v=(psi.derivative(0));
  AString dpsidy(psi.derivative(1));
  u="-(";
  u+=dpsidy;
  u+=")";
  return 1;
}
inline int curl(const AFunction& u,
	 const AFunction& v,
	 AString& curl)
{
  curl="(";
  curl+=u.derivative(0);
  curl+=")-(";
  curl+=v.derivative(1);
  curl+=")";
  return 1;
}
inline int div(const AFunction& u,
	const AFunction& v,
	AString& divu)
{
  divu="(";
  divu+=u.derivative(0);
  divu+=")+(";
  divu+=v.derivative(1);
  divu+=")";
  return 1;
}
inline int grad(const AFunction& u,
		AString& gradux,
		AString& graduy)
{
  gradux=u.derivative(0);
  graduy=u.derivative(1);
  return 1;
}
inline int tau(const AFunction& u,
	const AFunction& v,
	AString& ux,
	AString& uy,
	AString& vx,
	AString& vy
	)
{
  ux=u.derivative(0);
  uy=u.derivative(1);
  vx=v.derivative(0);
  vy=v.derivative(1);
  return 1;
}
inline int mult(const AFunction& u,
	 const AFunction& v,
	 AString& uv)
{
  uv="(";
  uv+=u.asString();
  uv+="*";
  uv+=v.asString();
  uv+=")";
  return 1;
}
inline int somme(const AFunction& u,
	 const AFunction& v,
	 AString& uv)
{
  uv="(";
  uv+=u.asString();
  uv+="+";
  uv+=v.asString();
  uv+=")";
  return 1;
}
inline int carre(const AFunction& u,
		 const AFunction& v,
		 AString& uv)
{
  uv="(";
  uv+=u.asString();
  uv+=")^2+(";
  uv+=v.asString();
  uv+="^2)";
  return 1;
}
inline int conv(const AFunction& u,
	  const AFunction& v,
	 AString& convx,
	 AString& convy)
{
  AString ux;
  AString uy;
  AString vx;
  AString vy;
  tau(u, v, ux, uy, vx, vy);
  AFunction dudx(ux, var);
  AFunction dudy(uy, var);
  AFunction dvdx(vx, var);
  AFunction dvdy(vy, var);
  AString ududx;
  mult (u, dudx, ududx);
  AString vdudy;
  mult (v, dudy, vdudy);
  somme(AFunction(ududx, var), 
	AFunction(vdudy, var),
	convx);
  AString udvdx;
  mult (u, dvdx, udvdx);
  AString vdvdy;
  mult (v, dvdy, vdvdy);
  somme(AFunction(udvdx, var), 
	AFunction(vdvdy, var),
	convy);
  return 1;
}

inline int write(int N, double h,
	  const AFunction& f, 
	  std::ofstream& fic)
{
  for(int i=0; i<N; i++)
    {
      double xi=i*h;
      fic <<std::endl;
      for(int j=0; j<N; j++)
	{
	  double yj=j*h;
	  fic << xi << " "
	      << yj << " "
	      << f.get_value(xi, yj)
	      << std::endl;
	}
    }
  return 1;
}

inline int test()
{
  std::cout << "TEST" << std::endl;
  AFunction u("COS(x)*-1", var);
  std::cout << u.asString() << std::endl;
  AFunction v("SIN(y)*-1", var);
  std::cout << v.asString() << std::endl;
  AString div_str;
  div(u, v, div_str);
  std::cout << div_str << std::endl;
  AFunction div(div_str, var);
  std::cout << div.asString() << std::endl;
  return 1;
}
