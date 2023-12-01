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



#include "utils.hxx"
int main()
{
  var[0]="x";
  var[1]="y";
  test();
  AFunction pi("pi", "3.14159265358979323846", "t");
  std::ofstream fic_psi("psi.gnu");
  std::ofstream fic_u("u.gnu");
  std::ofstream fic_v("v.gnu");
  std::ofstream fic_ec("ec.gnu");
  std::ofstream fic_deltaec("deltaec.gnu");
  std::ofstream fic_div("div.gnu");
  std::ofstream fic_rot("rot.gnu");
  std::ofstream fic_rot_rotx("rot_rotx.gnu");
  std::ofstream fic_rot_roty("rot_roty.gnu");
  std::ofstream fic_divdelta("divdelta.gnu");
  std::ofstream fic_rotomega("rotomega.gnu");
  std::ofstream fic_convx("convx.gnu");
  std::ofstream fic_convy("convy.gnu");
  std::ofstream fic_divconv("divconv.gnu");
  std::ofstream fic_rotconv("rotconv.gnu");
  AString fstr;
  std::cout << "Lecture de la fonction ... g(t)" << std::endl;
  std::cin>>fstr;
  AFunction g("g", fstr, "t");
  std::cout << "g = " << g.asString() << std::endl;
  std::cout << "Lecture de la fonction ... omega(t)" << std::endl;
  std::cin>>fstr;
  AFunction omega("omega", fstr, "t");
  std::cout << "omega = " << omega.asString() << std::endl;
  std::cout << "Lecture de la fonction ... f(t)" << std::endl;
  std::cin>>fstr;
  AFunction f("f", fstr, "t");
  std::cout << "f = " << f.asString() << std::endl;
  fstr=f.derivative(0);
  std::cout << "dfstr = " << fstr << std::endl;
  AFunction df("df", fstr, "t");
  std::cout << "df = " << df.asString() << std::endl;
  AFunction psi("psi", "f(x)*f(y)", var);
  std::cout << "psi = " << psi.asString() << std::endl;
  fstr=psi.derivative(0);
  AFunction dpsidx("dpsidx", fstr, var);
  std::cout << "dpsidx = " << dpsidx.asString() << std::endl;
  fstr=psi.derivative(1);
  AFunction dpsidy("dpsidy", fstr, var);
  std::cout << "dpsidy = " << dpsidy.asString() << std::endl;

  int N;
  std::cout << "Lecture du Nombre de points" << std::endl;
  std::cin>>N;
  double h=1./(N-1);
  std::cout << "h = " << h << std::endl;
  AString u_str("-dpsidy(x,y)");
  AString v_str("dpsidx(x,y)");
  rot(psi, u_str, v_str);
  AFunction u(u_str, var);
  std::cout << "u = " << u.asString() << std::endl;
  AFunction v(v_str, var);
  std::cout << "v = " << v.asString() << std::endl;
  AString div_str;
  div(u, v, div_str);
  AFunction divu(div_str, var);
  std::cout << "div = " << divu.asString() << std::endl;
  AString rot_str;
  curl(u, v, rot_str);
  AFunction rotv(rot_str, var);
  std::cout << "rot = " << rotv.asString() << std::endl;
  AString ec_str;
  carre(u, v, ec_str);
  AFunction ec(ec_str, var);
  std::cout << "ec = " << ec.asString() << std::endl;
  AString gradecx_str;
  AString gradecy_str;
  grad(ec, gradecx_str,gradecy_str);
  AFunction gradecx(gradecx_str, var);
  AFunction gradecy(gradecy_str, var);
  AString deltaec_str;
  div(gradecx, gradecy, deltaec_str);
  AFunction deltaec(deltaec_str, var);
  std::cout << "deltaec = " << deltaec.asString() << std::endl;
  AString rotx_str;
  AString roty_str;
  rot(rotv, rotx_str, roty_str);
  AFunction rotx(rotx_str, var);
  std::cout << "rotx = " << rotx.asString() << std::endl;
  AFunction roty(roty_str, var);
  std::cout << "roty = " << roty.asString() << std::endl;
  AString div_rot;
  div(rotx, roty, div_rot);
  AFunction divdelta(div_rot, var);
  std::cout << "divdelta = " << divdelta.asString() << std::endl;
  AString rot_rot;
  curl(rotx, roty, rot_rot);
  AFunction rotomega(rot_rot, var);
  std::cout << "rotomega = " << rotomega.asString() << std::endl;

  AString convx_str;
  AString convy_str;
  conv(u, v, convx_str, convy_str);
  AFunction convx(convx_str, var);
  std::cout << "convx = " << convx.asString() << std::endl;
  AFunction convy(convy_str, var);
  std::cout << "convy = " << convy.asString() << std::endl;
  AString divconv_str;
  div(convx, convy, divconv_str);
  AFunction divconv(divconv_str, var);
  std::cout << "divconv = " << divconv.asString() << std::endl;
  AString rotconv_str;
  curl(convx, convy, rotconv_str);
  AFunction rotconv(rotconv_str, var);
  std::cout << "rotconv = " << rotconv.asString() << std::endl;
  
  write(N, h, psi, fic_psi);
  write(N, h, u, fic_u);
  write(N, h, v, fic_v);
  write(N, h, ec, fic_ec);
  write(N, h, deltaec, fic_deltaec);
  write(N, h, rotv, fic_rot);
  write(N, h, divu, fic_div);
  write(N, h, rotx, fic_rot_rotx);
  write(N, h, roty, fic_rot_roty);
  write(N, h, divdelta, fic_divdelta);  
  write(N, h, rotomega, fic_rotomega);  
  write(N, h, convx, fic_convx);
  write(N, h, convy, fic_convy);
  write(N, h, divconv, fic_divconv);
  write(N, h, rotconv, fic_rotconv);

  return 0;
}
