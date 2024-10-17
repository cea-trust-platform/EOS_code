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
 * EOS_IGen_i.hxx
 *
 *  Created on: 11 mai 2010
 */

#ifndef EOS_IGEN_I_HXX_
#define EOS_IGEN_I_HXX_

namespace NEPTUNE_EOS_IGEN
{
  inline void EOS_IGen::set_method(const char* const str)
  { if (strlen(str) != 0) 
       method = AString(str) ;
    else
       cerr<< "Error set_method : bad value of interpolated EOS Thermodynamic Model" << endl ;
  }

  inline void EOS_IGen::set_reference(const char* const str)
  { if (strlen(str) != 0) 
       reference = AString(str) ;
    else
       cerr<< "Error set_method : bad value of interpolated Equation of State" << endl ;
  }
  
  inline void EOS_IGen::set_extremum(double p_min, double p_max, double T_min, double T_max)
  { if (p_min < p_max  &&  T_min < T_max)
       { pmin = p_min ;
         pmax = p_max ;
         Tmin = T_min ;
         Tmax = T_max ;
       }
    else
       cerr<< "Error set_extremum : bad values of "
           << "first or second thermodynamic properties "
           << "limits for 2D diagram" << endl ;
  }
  
  
  inline EOS_Error EOS_IGen::set_memory_max(int mmax)
  { if (mmax > 0)
       { memory_max = mmax ;
         return EOS_Error::good ;
       }
    else
       { cerr << "Error set_memory_max : bad negative or null value of memory" << endl ;
         return EOS_Error::error ;
       }
  }
  
  
  inline void EOS_IGen::set_tempory_med_name()
  { if (mesh_ph->get_exist())
       { tempory_med_name="mesh_ph" ;
         if (mesh_p->get_exist())  tempory_med_name += "_p" ;
       }
    else if (mesh_p->get_exist())
       tempory_med_name = "mesh_p" ;
    else
       tempory_med_name = "" ;
  }
  
  inline void EOS_IGen::set_file_med_name(AString& file_name)
  { file_med_name=file_name ;
  }
  
  inline void EOS_IGen::set_file_med_name()
  { file_med_name = method ;
    file_med_name += "." ;
    file_med_name += reference ;
    file_med_name += "." ;
    if (mesh_ph->get_exist())
       { char buffer[100] ;
         sprintf (buffer, "ph_field.p%e_%e.h%e_%e.", pmin,pmax,hmin,hmax);
         AString infos(buffer) ;
         file_med_name += infos ;
       }
    
    if (mesh_p->get_exist())
       { char buffer[100] ;
         sprintf (buffer, "p_field.p%e_%e", pmin, pmax) ;
         AString infos(buffer) ;
         file_med_name += infos ;
       }
  }
  
  inline void EOS_IGen::set_obj_Ipp()
  { obj_Ipp = new EOS("EOS_Ipp",tempory_med_name.aschar()) ;
  }
  
  inline void EOS_IGen::set_fluid()
  { fluid = new EOS(method.aschar(),reference.aschar()) ;
  }
  
  inline void EOS_IGen::set_IGen_handler()
  { IGen_handler.set_exit_on_error(EOS_Std_Error_Handler::disable_feature) ;
    IGen_handler.set_dump_on_error(EOS_Std_Error_Handler::disable_feature) ;
    IGen_handler.set_throw_on_error(error) ;
    fluid->save_error_handler() ;
    fluid->set_error_handler(IGen_handler) ;
  }
  
  inline void EOS_IGen::set_mesh_ph(int nb_node_p, int nb_node_h, int level_max)
  { delete mesh_ph ;
    mesh_ph=new EOS_Mesh(nb_node_p, nb_node_h, pmin, pmax, hmin, hmax, level_max);
  }
  
  inline void EOS_IGen::set_mesh_p(int nb_node_p, int level_max)
  { delete mesh_p ;
    mesh_p = new EOS_Mesh(nb_node_p, pmin, pmax, level_max) ;
  }

  inline const AString& EOS_IGen::get_method() const
  { return method ;
  }
  
  inline EOS* EOS_IGen::get_obj_Ipp() const
  { return obj_Ipp ;
  }

  inline const AString& EOS_IGen::get_reference() const
  { return reference ;
  }

  inline EOS_Mesh EOS_IGen::get_mesh_p() const
  { return *mesh_p ;
  }

  inline EOS_Mesh EOS_IGen::get_mesh_ph() const
  { return *mesh_ph ;
  }
  
  inline EOS EOS_IGen::get_fluid() const
  { return *fluid ;
  }
  
  
  inline int EOS_IGen::get_memory_max() const
  { return memory_max ;
  }
  
  inline const vector<EOS_IGen_QI>& EOS_IGen::get_qualities() const
  { return qualities ;
  }

  inline void EOS_IGen::set_list_propi(vector<string>& lpi)
  { list_propi = lpi ;
  }

  inline void EOS_IGen::set_list_propi_sat(vector<string>& lpis)
  { list_propi_sat = lpis ;
  }

  inline void EOS_IGen::set_list_propi_lim(vector<string>& lpil)
  { list_propi_lim = lpil ;
  }

}

#endif /* EOS_IGEN_I_HXX_ */
