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



// therm_properties.hxx

#ifndef THERM_PROPERTIES_H
#define THERM_PROPERTIES_H

namespace NEPTUNE
{
#if  __cplusplus >= 201103L
  static vector<typrop> thermprop =
#else
  static typrop t_thermprop[] =
#endif
    { { "P"             ,      "Pressure"                               },         
      { "h"             ,      "specific enthalpy"                      },
      { "T"             ,      "Temperature"                            },        
      { "rho"           ,      "density"                                },      
      { "u"             ,      "specific internal energy"               },        
      { "s"             ,      "specific entropy"                       },        
      { "mu"            ,      "dynamic viscosity"                      },       
      { "lambda"        ,      "thermal conductivity"                   },   
      { "cp"            ,      "specific isobaric heat capacity"        },       
      { "cv"            ,      "specific isochoric heat capacity"       },       
      { "sigma"         ,      "surface tension"                        },    
      { "w"             ,      "speed of sound"                         },         
      { "g"             ,      "specific free enthalpy"                 },        
      { "f"             ,      "specific free energy"                   },        
      { "Pr"            ,      "Prandtl number"                         },       
      { "beta"          ,      "isobaric thermal expansion coefficient" },     
      { "gamma"         ,      "cp / cv ratio"                          },

      { "[dT/dP]h"      ,      "[dT/dP]h"                               }, 
      { "[dT/dh]P"      ,      "[dT/dh]P"                               }, 
      { "[drho/dP]h"    ,      "[drho/dP]h"                             },
      { "[drho/dh]P"    ,      "[drho/dh]P"                             },
      { "[drho/dT]P"    ,      "[drho/dT]P"                             },
      { "[drho/dP]T"    ,      "[drho/dP]T"                             },
      { "[du/dP]h"      ,      "[du/dP]h"                               }, 
      { "[du/dh]P"      ,      "[du/dh]P"                               }, 
      { "[ds/dP]h"      ,      "[ds/dP]h"                               }, 
      { "[ds/dh]P"      ,      "[ds/dh]P"                               }, 
      { "[dmu/dP]h"     ,      "[dmu/dP]h"                              },
      { "[dmu/dh]P"     ,      "[dmu/dh]P"                              },
      { "[dlambda/dP]h" ,      "[dlambda/dP]h"                          },
      { "[dlambda/dh]P" ,      "[dlambda/dh]P"                          },
      { "[dcp/dP]h"     ,      "[dcp/dP]h"                              },
      { "[dcp/dh]P"     ,      "[dcp/dh]P"                              },
      { "[dcv/dP]h"     ,      "[dcv/dP]h"                              },
      { "[dcv/dh]P"     ,      "[dcv/dh]P"                              },
      { "[dsigma/dP]h"  ,      "[dsigma/dP]h"                           },
      { "[dsigma/dh]P"  ,      "[dsigma/dh]P"                           },
      { "[dwdP]h"       ,      "[dwdP]h"                                },  
      { "[dw/dh]P"      ,      "[dw/dh]P"                               }, 
      { "[dg/dP]h"      ,      "[dg/dP]h"                               }, 
      { "[dg/dh]P"      ,      "[dg/dh]P"                               }, 
      { "[df/dP]h"      ,      "[df/dP]h"                               }, 
      { "[df/dh]P"      ,      "[df/dh]P"                               }, 
      { "[dPr/dP]h"     ,      "[dPr/dP]h"                              },
      { "[dPr/dh]P"     ,      "[dPr/dh]P"                              },
      { "[dbeta/dP]h"   ,      "[dbeta/dP]h"                            },
      { "[dbeta/dh]P"   ,      "[dbeta/dh]P"                            },
      { "[dgamma/dP]h"  ,      "[dgamma/dP]h"                           },
      { "[dgamma/dh]P"  ,      "[dgamma/dh]P"                           },
      { "[du/dT]P"      ,      "[du/dT]P"                               }, 
      { "[ds/dT]P"      ,      "[ds/dT]P"                               }, 
      { "[ds/dP]T"      ,      "[ds/dP]T"                               }, 
      { "[dmu/dT]P"     ,      "[dmu/dT]P"                              },
      { "[dmu/dP]T"     ,      "[dmu/dP]T"                              },
      { "[dlambda/dP]T" ,      "[dlambda/dP]T"                          },
      { "[dlambda/dT]P" ,      "[dlambda/dT]P"                          },
      { "[dcp/dT]P"     ,      "[dcp/dT]P"                              },
      { "[dcp/dP]T"     ,      "[dcp/dP]T"                              },
      { "[dcv/dT]P"     ,      "[dcv/dT]P"                              },
      { "[dcv/dP]T"     ,      "[dcv/dP]T"                              },
      { "[dsigma/dT]P"  ,      "[dsigma/dT]P"                           },
      { "[dw/dT]P"      ,      "[dw/dT]P"                               },
      { "[dw/dP]T"      ,      "[dw/dP]T"                               }, 
      { "[dg/dT]P"      ,      "[dg/dT]P"                               }, 
      { "[df/dT]P"      ,      "[df/dT]P"                               }, 
      { "[dPrdT]P"      ,      "[dPrdT]P"                               }, 
      { "[dbeta/dT]P"   ,      "[dbeta/dT]P"                            },
      { "[dgamma/dT]P"  ,      "[dgamma/dT]P"                           },
      { "[dh/dT]P"      ,      "[dh/dT]P"                               }, 
      { "[dh/dP]T"      ,      "[dh/dP]T"                               }, 
      { "[dh/ds]P"      ,      "[dh/ds]P"                               }, 
      { "[dh/dP]s"      ,      "[dh/dP]s"                               }  
    };
#if  __cplusplus <  201103L
 static std::vector<typrop> thermprop(t_thermprop, 
                                      t_thermprop + sizeof(t_thermprop) / sizeof(typrop) ) ;
#endif


  enum EOS_thermprop
    { NotATProperty = -1 ,

      p      ,
      firstTProperty = p,
      h      ,
      T      ,
      rho    ,
      u      ,
      s      ,
      mu     ,
      lambda ,
      cp     ,
      cv     ,
      sigma  ,
      w      ,
      g      ,
      f      ,
      pr     ,
      beta   ,
      gamma  ,

      d_T_d_p_h = gamma + 2,
      d_T_d_h_p      ,
      d_rho_d_p_h    ,
      d_rho_d_h_p    ,
      d_u_d_p_h      ,
      d_u_d_h_p      ,
      d_s_d_p_h      ,
      d_s_d_h_p      ,
      d_mu_d_p_h     ,
      d_mu_d_h_p     ,
      d_lambda_d_p_h ,
      d_lambda_d_h_p ,
      d_cp_d_p_h     ,
      d_cp_d_h_p     ,
      d_cv_d_p_h     ,
      d_cv_d_h_p     ,
      d_sigma_d_p_h  ,
      d_sigma_d_h_p  ,
      d_w_d_p_h      ,
      d_w_d_h_p      ,
      d_g_d_p_h      ,
      d_g_d_h_p      ,
      d_f_d_p_h      ,
      d_f_d_h_p      ,
      d_pr_d_p_h     ,
      d_pr_d_h_p     ,
      d_beta_d_p_h   ,
      d_beta_d_h_p   ,
      d_gamma_d_p_h  ,
      d_gamma_d_h_p  ,
      d_rho_d_p_T    ,
      d_rho_d_T_p    ,
      d_u_d_T_p      ,
      d_s_d_T_p      ,
      d_s_d_p_T      ,
      d_mu_d_T_p     ,
      d_mu_d_p_T     ,
      d_lambda_d_p_T ,
      d_lambda_d_T_p ,
      d_cp_d_T_p     ,
      d_cp_d_p_T     ,
      d_cv_d_T_p     ,
      d_cv_d_p_T     ,
      d_sigma_d_T_p  ,
      d_w_d_T_p      ,
      d_w_d_p_T      ,
      d_g_d_T_p      ,
      d_f_d_T_p      ,
      d_pr_d_T_p     ,
      d_beta_d_T_p   ,
      d_gamma_d_T_p  ,
      d_h_d_T_p      ,
      d_h_d_p_T      ,
      d_h_d_p_s      ,
      d_h_d_s_p      ,
      lastTProperty = d_h_d_s_p
    };
  enum EOS_limthermprop
    { EOS_TPROP    =    p ,
      EOS_TPROPDER =  d_T_d_p_h
    };

  EOS_thermprop nam2num_thermprop(const char* const name)  ;
  EOS_thermprop nam2num_dthermprop(const char* const name) ;

}
#include "EOS/API/therm_properties_i.hxx"
#endif
