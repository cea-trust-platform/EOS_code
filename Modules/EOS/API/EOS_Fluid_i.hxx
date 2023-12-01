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



//
#ifndef EOS_Fluid_i_H
#define EOS_Fluid_i_H

namespace NEPTUNE
{
  inline int EOS_Fluid::set_components(EOS** components, int nb)
  { cerr << "Not allowed for non mixing EOS!" << endl;
    cerr << " components : " << components << "  nb = " << nb << endl ;
    assert(0);
    exit(-1);
    return -1;
  }

  inline void EOS_Fluid::set_reference_state(double h, double s, double T, double p)
  { cerr << "Not implemented for non PerfectGas EOS!" << endl;
    assert(0);
    exit(-1);
    return ;
  }


  // provisional for EOS_Mixing 
  inline double EOS_Fluid::get_prxr()
  { cerr << "Not allowed for non EOS_PerfectGas EOS !" << endl;
    assert(0);
    exit(-1);
    return -1;    
  }

  // provisional for EOS_Mixing 
  inline double EOS_Fluid::get_prxcp()
  { cerr << "Not allowed for non EOS_PerfectGas EOS !" << endl;
    assert(0);
    exit(-1);
    return -1;    
  }

  // provisional for EOS_Mixing 
  inline double EOS_Fluid::get_prxm()
  { cerr << "Not allowed for non EOS_PerfectGas EOS !" << endl;
    assert(0);
    exit(-1);
    return -1;    
  }

  // provisional for EOS_Mixing 
  inline double EOS_Fluid::get_prxl0()
  { cerr << "Not allowed for non EOS_PerfectGas EOS !" << endl;
    assert(0);
    exit(-1);
    return -1;    
  }

  // provisional for EOS_Mixing 
  inline double EOS_Fluid::get_prxl1()
  { cerr << "Not allowed for non EOS_PerfectGas EOS !" << endl;
    assert(0);
    exit(-1);
    return -1;    
  }

  // provisional for EOS_Mixing 
  inline double EOS_Fluid::get_prxl2()
  { cerr << "Not allowed for non EOS_PerfectGas EOS !" << endl;
    assert(0);
    exit(-1);
    return -1;    
  }

  // provisional for EOS_Mixing 
  inline double EOS_Fluid::get_prxm0()
  { cerr << "Not allowed for non EOS_PerfectGas EOS !" << endl;
    assert(0);
    exit(-1);
    return -1;    
  }

  // provisional for EOS_Mixing 
  inline double EOS_Fluid::
  get_prxm1()
  { cerr << "Not allowed for non EOS_PerfectGas EOS !" << endl;
    assert(0);
    exit(-1);
    return -1;    
  }

  // provisional for EOS_Mixing 
  inline double EOS_Fluid::get_prxm2()
  {
    cerr << "Not allowed for non EOS_PerfectGas EOS !" << endl;
    assert(0);
    exit(-1);
    return -1;    
  }
  //
  // provisional for EOS_Mixing 
  inline double EOS_Fluid::get_prxdv()
  { cerr << "Not allowed for non EOS_PerfectGas EOS !" << endl;
    assert(0);
    exit(-1);
    return -1;    
  }

  inline void EOS_Fluid::fluid_description(std::ostream& flux) const
  { flux<<"Model table      : " << this->table_name() << endl ;
    if (this->version_name()  != "Unknown") flux << "Version           : "<< this->version_name()  << endl ;
    if (this->fluid_name()    != "Unknown") flux << "Fluid             : "<< this->fluid_name()    << endl ;
    if (this->equation_name() != "Unknown") flux << "Equation of fluid : "<< this->equation_name() << endl ;
    
    double pmax, pmin ;
    if (this->get_p_min(pmin) ==  EOS_Internal_Error::OK)  flux << "p_min : " << pmin << " "<< endl ;
    if (this->get_p_max(pmax) ==  EOS_Internal_Error::OK)  flux << "p_max : " << pmax << endl ;
    
    double Tmax, Tmin ;
    if (this->get_T_min(Tmin) ==  EOS_Internal_Error::OK)  flux << "T_min : " << Tmin << " "<< endl ;
    if (this->get_T_max(Tmax) ==  EOS_Internal_Error::OK)  flux << "T_max : " << Tmax << endl ;
    
    double hmax, hmin ;
    if (this->get_h_min(hmin) ==  EOS_Internal_Error::OK)  flux << "h_min : " << hmin << " "<< endl ;
    if (this->get_h_max(hmax) ==  EOS_Internal_Error::OK)  flux << "h_max : " << hmax << endl ;
    
    flux<<endl;
  }
}
#endif
