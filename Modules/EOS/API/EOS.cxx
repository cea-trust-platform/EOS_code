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
#include "EOS.hxx"
#include "Language/API/F77Language.hxx"
#include "Language/API/Object_i.hxx"
#include "EOS_Fluid.hxx"
#include "EOS_Field.hxx"
#include "EOS_Error_Field.hxx"
#include "EOS/Src/EOS_Mixing.hxx"
#include "EOS/Src/EOS_PerfectGas/EOS_PerfectGas.hxx"
#include "EOS/Src/EOS_StiffenedGas/EOS_StiffenedGas.hxx"
#include "EOS_Std_Error_Handler.hxx"

//PLUGIN INSERT CATHARE
#ifdef WITH_PLUGIN_CATHARE
#include "EOS/Src/EOS_Cathare/EOS_Cathare_LiquidWater.hxx"
#include "EOS/Src/EOS_Cathare/EOS_Cathare_Steam.hxx"
#include "EOS/Src/EOS_Cathare/EOS_Cathare_FreonR12Vapor.hxx"
#include "EOS/Src/EOS_Cathare/EOS_Cathare_FreonR12Liquid.hxx"
#endif

//PLUGIN INSERT THETIS
#ifdef WITH_PLUGIN_THETIS
#include "EOS/Src/EOS_Thetis/EOS_ThetisFreonR113Liquid.hxx"
#include "EOS/Src/EOS_Thetis/EOS_ThetisFreonR113Vapor.hxx"
#include "EOS/Src/EOS_Thetis/EOS_ThetisFreonR114Liquid.hxx"
#include "EOS/Src/EOS_Thetis/EOS_ThetisFreonR114Vapor.hxx"
#include "EOS/Src/EOS_Thetis/EOS_ThetisFreonR12Liquid.hxx"
#include "EOS/Src/EOS_Thetis/EOS_ThetisFreonR12Vapor.hxx"
#include "EOS/Src/EOS_Thetis/EOS_ThetisWaterLiquid.hxx"
#include "EOS/Src/EOS_Thetis/EOS_ThetisWaterVapor.hxx"
#include "EOS/Src/EOS_Thetis/EOS_ThetisWaterBMPLiquid.hxx"
#include "EOS/Src/EOS_Thetis/EOS_ThetisWaterBMPVapor.hxx"
#include "EOS/Src/EOS_Thetis/EOS_ThetisWaterBPLiquid.hxx"
#include "EOS/Src/EOS_Thetis/EOS_ThetisWaterBPVapor.hxx"
#include "EOS/Src/EOS_Thetis/EOS_ThetisWaterMPLiquid.hxx"
#include "EOS/Src/EOS_Thetis/EOS_ThetisWaterMPVapor.hxx"
#include "EOS/Src/EOS_Thetis/EOS_ThetisAmmoniaLiquid.hxx"
#include "EOS/Src/EOS_Thetis/EOS_ThetisAmmoniaVapor.hxx"
#endif

//PLUGIN INSERT REFPROP9
#ifdef WITH_PLUGIN_REFPROP_9
#include "EOS/Src/EOS_Refprop9/EOS_Refprop9.hxx"
#include "EOS/Src/EOS_Refprop9/EOS_Refprop9_liquid.hxx"
#include "EOS/Src/EOS_Refprop9/EOS_Refprop9_vapor.hxx"
#endif

//PLUGIN INSERT REFPROP10
#ifdef WITH_PLUGIN_REFPROP_10
#include "EOS/Src/EOS_Refprop10/EOS_Refprop10.hxx"
#include "EOS/Src/EOS_Refprop10/EOS_Refprop10_liquid.hxx"
#include "EOS/Src/EOS_Refprop10/EOS_Refprop10_vapor.hxx"
#endif

//PLUGIN INSERT INTERPOLATOR
#ifdef WITH_IPP
#include "EOS/Src/EOS_Ipp/EOS_Ipp.hxx"
#include "EOS/Src/EOS_Ipp/EOS_Ipp_liquid.hxx"
#include "EOS/Src/EOS_Ipp/EOS_Ipp_vapor.hxx"
#endif

//PLUGIN INSERT CATHARE2
#ifdef WITH_PLUGIN_CATHARE2
#include "EOS/Src/EOS_Cathare2/EOS_Cathare2Liquid.hxx"
#include "EOS/Src/EOS_Cathare2/EOS_Cathare2Vapor.hxx"
#include "EOS/Src/EOS_Cathare2/EOS_CathareIncondensableGas.hxx"
#endif

//PLUGIN INSERT HITEC
#ifdef WITH_PLUGIN_HITEC
#include "EOS/Src/EOS_Hitec/EOS_Hitec.hxx"
#endif

//PLUGIN INSERT NAK
#ifdef WITH_PLUGIN_NAK
#include "EOS/Src/EOS_Nak/EOS_Nak.hxx"
#endif


//PLUGIN INSERT FLICA4
#ifdef WITH_PLUGIN_FLICA4
#include "EOS/Src/EOS_Flica4/EOS_FLICA4.hxx"
#endif



#include <fstream>

namespace NEPTUNE
{
   // static EOS_Field &get_field(const Object_ID &Id)
   // {
   //   if(!(set_Objects().is_object(Id)))
   //     {
   //       std::cerr << Id << " is not a valid Object ID" << std::endl;
   //       exit(-1);
   //     }
   //   return dynamic_cast<NEPTUNE::EOS_Field&>(set_Objects().set_object(Id));
   // }
   // static EOS_Fluid &get_fluid(const Object_ID &Id)
   // {
   //   if(!(set_Objects().is_object(Id)))
   //     {
   //       std::cerr << Id << " is not a valid Object ID" << std::endl;
   //       exit(-1);
   //     }
   //   return dynamic_cast<EOS_Fluid&>(set_Objects().set_object(Id));
   // }

// Determine   in return, the class associated with(method_str, ref_str) from index.eos  :  fluid_model_class
const char* EOS::INDEX(const char* const method_str, const char* const ref_str)
{
  char method_str_maj[50], ref_str_maj[50] ;
  char method_str_iem[50], ref_str_iem[50] ;
  char *cpt_method, *cpt_ref ;
  char *cpt0, *cpt, *cpt2, *cpt3 ;

  static const int longmax = 1000 ;
  static const char pref_eos[5] = "EOS_" ;
  char line_indeos[longmax] ;

  // uppercase : method_str_maj, ref_str_maj
  strcpy(method_str_maj,method_str) ;
  cpt = method_str_maj ;
  while ((*cpt = toupper(*cpt)))  cpt++ ;

  strcpy(ref_str_maj,ref_str) ;
  cpt = ref_str_maj ;
  while ((*cpt = toupper(*cpt)))  cpt++ ;

  // pointer  cpt_method, cpt_ref
  cpt0 = method_str_maj ;
  cpt  = strstr(method_str_maj,pref_eos) ;
  if (cpt0 == cpt)  
     cpt_method = method_str_maj + strlen(pref_eos) ;
  else
     cpt_method = method_str_maj ;
  cpt_ref = ref_str_maj ;


  // index.eos file :   name : index_file_name   desc : index_file_desc
  AString index_file_name;
  get_index_file(index_file_name);
  std::ifstream index_file_desc(index_file_name.aschar());
  if (!index_file_desc) 
     { cerr << index_file_name.aschar() << " file not found !"<< endl;
       strcpy(fluid_model_class,"Class Not Found : index.eos file not found ") ;
       return fluid_model_class ;
     }

  // read in index.eos file : method_str_iem, ref_str_iem, fluid_model_class
  while(index_file_desc)
     { index_file_desc.getline(line_indeos,longmax) ;
       if (index_file_desc.eof()) break ;

       cpt = line_indeos ;
       int numw = 0 ;
       while ( (cpt2 = strtok(cpt," ")) )
          { numw++ ;
            if (numw == 1)
               { strcpy(method_str_iem,cpt2) ;  // read and uppercase
                 cpt3 = method_str_iem ;
                 while ((*cpt3 = toupper(*cpt3)))  cpt3++ ;
               }
            else if (numw == 2)                 // read and uppercase
               { strcpy(ref_str_iem,cpt2) ;
                 cpt3 = ref_str_iem ;
                 while ((*cpt3 = toupper(*cpt3)))  cpt3++ ;
               }
            else if (numw == 3)
               strcpy(fluid_model_class,cpt2) ;
            cpt = NULL ;
          }

       if ((strcmp(cpt_method,method_str_iem) == 0) && (strcmp(cpt_ref,ref_str_iem) == 0)) 
          { index_file_desc.close() ;
            return fluid_model_class ;
          }
     }

   // method_str, ref_str   not found
   cerr << " M.T. '" << method_str << "' and E.E. '"  << ref_str << "' not found !"<< endl;
   strcpy(fluid_model_class,"Class Not Found : M.T. and E.E not found") ;
   return fluid_model_class ;
}





   // File index.eos 
   void EOS::get_index_file(AString &index_file_name)
   { if (iret_eos_data_dir) return exit(1) ;
     index_file_name  = eos_data_dir.c_str() ;
     index_file_name += "/index.eos"         ;
   }

   // IN  : tmname_in         : string [EOS_]Thermodynamic Model Name
   //       fename_in         : string Fluid Equation Name
   //
   // OUT : fluid_model_class : class associated with(tmname_in, fename_in) from index.eos
   //       fluid_model_args  : [array of strings] supplementary arguments in index.eos file
   //       therm_model       : Thermodynamic Model Name
   //       fluid_equa        : Fluid Equation Name
   //       eos_therm_model   : EOS Thermodynamic Model Name
   //       tm_data_dir       : Thermodynamic Model Data Directory
   //       tm_version        : Thermodynamic Model version
   //       m_fluid           : Fluid Name
   //       m_phase           : Phase Name
   //       index_data        : error indice
   //
   int EOS::index_data(const char *const tmname_in, const char *const fename_in)
   {
     char tmname_in_maj[50], fename_in_maj[50] ;
     char tmname_read[50], fename_read[50] ;
     char *cpt_tmname_in, *cpt_fename_in ;
     char *cpt0, *cpt, *cpt2, *cpt3 ;
   
     static const int longmax = 1000 ;
     static const char pref_eos[5] = "EOS_" ;
     char line_indeos[longmax] ;
     char mot[100] ;
   
     // uppercase : tmname_in_maj, fename_in_maj
     strcpy(tmname_in_maj,tmname_in) ;
     cpt = tmname_in_maj ;
     while ((*cpt = toupper(*cpt)))  cpt++ ;
   
     strcpy(fename_in_maj,fename_in) ;
     cpt = fename_in_maj ;
     while ((*cpt = toupper(*cpt)))  cpt++ ;
   
     // pointer  cpt_tmname_in, cpt_fename_in
     cpt0 = tmname_in_maj ;
     cpt  = strstr(tmname_in_maj,pref_eos) ;
     if (cpt0 == cpt)  
        cpt_tmname_in = tmname_in_maj + strlen(pref_eos) ;
     else
        cpt_tmname_in = tmname_in_maj ;
     cpt_fename_in = fename_in_maj ;
   
   
     // index.eos file :   name : index_file_name   desc : index_file_desc
     AString index_file_name;
     get_index_file(index_file_name);
     std::ifstream index_file_desc(index_file_name.aschar());
     if (!index_file_desc) 
        { cerr << index_file_name.aschar() << " file not found !"<< endl;
          strcpy(fluid_model_class,"Class Not Found : index.eos file not found ") ;
          return 2 ;
        }
   
     // read in index.eos file : tmname_read, fename_read, fluid_model_class
     while(index_file_desc)
        { index_file_desc.getline(line_indeos,longmax) ;
          if (index_file_desc.eof()) break ;
   
          cpt = line_indeos ;
          int numw = 0 ;
          int nb_args  = 0 ;
          int num_args = 0 ;
          while ( (cpt2 = strtok(cpt," ")) )
             { numw++ ;
               if (numw == 1)                        // Thermodynamic Model Name
                  { strcpy(tmname_read,cpt2) ;       // read tmname_read
                    therm_model = tmname_read ;      // Thermodynamic Model Name
                    cpt3 = tmname_read ;             // uppercase
                    while ((*cpt3 = toupper(*cpt3)))  cpt3++ ;
                  }
               else if (numw == 2)                   // Fluid Equation Name
                  { strcpy(fename_read,cpt2) ;       // read fename_read
                    fluid_equa = fename_read ;       // Fluid Equation Name
                    cpt3 = fename_read ;             // uppercase
                    while ((*cpt3 = toupper(*cpt3)))  cpt3++ ;
                  }
               else if (numw == 3)                   // Class Name
                  strcpy(fluid_model_class,cpt2) ;
               else if (numw == 6)
                  { nb_args  = atoi(cpt2) ;
                    fluid_model_args.resize(nb_args) ;
                  }
               else if (num_args < nb_args)
                  { strcpy(mot,cpt2) ;
                    fluid_model_args[num_args] = AString(mot) ;
                    num_args++ ;
                  }
               cpt = NULL ;
             }
   
          if ((strcmp(cpt_tmname_in,tmname_read) == 0) && (strcmp(cpt_fename_in,fename_read) == 0)) 
             { index_file_desc.close() ;
               // EOS Thermodynamic Model Name
               eos_therm_model  = pref_eos ;
               eos_therm_model += therm_model ;
               // Thermodynamic Model Data Directory
               if (iret_eos_data_dir) return 3 ;
               tm_data_dir  = eos_data_dir ;
               tm_data_dir += "/"          ;
               tm_data_dir += eos_therm_model ;
               // T.M. version : tm_version
               // from  tminfo_file  file reading
               if (strcmp(eos_therm_model.c_str(),"EOS_Refprop+") == 0)
                  { string tminfo_file = tm_data_dir ;
                    tminfo_file += "/info.tm" ;
                    std::ifstream index_file_desc(tminfo_file.c_str());
                    if (!index_file_desc) 
                       { cerr << tminfo_file.c_str() << " file not found !"<< endl;
                         strcpy(fluid_model_class,"Class Not Found : T.M. info. file not found ") ;
                         return 3 ;
                       }
                    index_file_desc.close() ;
                  }
               else
                  tm_version = "scripi-shell amodifier sauf Refprop" ;
                 
               // provisoire
               m_fluid    = "afaire" ;
               m_phase    = "afaire" ;
               #
               return 0 ;
             }


        }
   
      // tmname_in, fename_in   not found
      cerr << " T.M. '" << tmname_in << "' and F.E. '"  << fename_in << "' not found !"<< endl;
      return 1 ;
   }

   // Get Thermodynamic Model Name
   const string EOS::info_thermmodel() const
   { return therm_model ;
   }
   // Get Fluid Equation Name
   const string EOS::info_fluidequa() const
   { return fluid_equa ;
   }
   // Get EOS Thermodynamic Model Name
   const string EOS::info_eosthermmodel() const
   { return eos_therm_model ;
   }
   // Get Thermodynamic Model Data Directory
   const string EOS::info_tmdatadir() const
   { return tm_data_dir ;
   }
   // Get Thermodynamic Model Version
   const string EOS::info_tmversion() const
   { return tm_version ;
   }
   // Get Fluid Name
   const string EOS::info_mfluid() const
   { return m_fluid ;
   }
   // Get Phase Name
   const string EOS::info_mphase() const
   { return m_phase ;
   }


  const AString EOS::eos_version("@EOS_VERSION@");

  EOS::EOS(const char *const str) 
      : Object(str), fluid_model_obj(dynamic_cast<EOS_Fluid&> (set_object(id())))
     { err_handler = new EOS_Std_Error_Handler ;
     }

  EOS::EOS(const char *const tmname_in, const char *const fename_in)
     : Object(INDEX(tmname_in, fename_in)), fluid_model_obj(dynamic_cast<EOS_Fluid&> (set_object(id())))
     { index_data(tmname_in, fename_in) ;
       Object obj(fluid_model_class) ;
       fluid_model_obj = dynamic_cast<EOS_Fluid&> (set_object(obj.id())) ;
       fluid_model_obj.init(fluid_model_args) ;
       err_handler = new EOS_Std_Error_Handler;
     }

  EOS::EOS(const char *const tmname_in, const char *const fename_in, const Strings &init_args)
     : Object(INDEX(tmname_in,fename_in)), fluid_model_obj(dynamic_cast<EOS_Fluid&> (set_object(id())))
     { index_data(tmname_in, fename_in) ;
       Object obj(fluid_model_class) ;
       fluid_model_obj = dynamic_cast<EOS_Fluid&> (set_object(obj.id())) ;
       fluid_model_obj.init(fluid_model_args,init_args) ;
       err_handler = new EOS_Std_Error_Handler ;
     }

  EOS::EOS(const char *const tmname_in, const char *const fename_in, const Strings &init_args, 
           const double h0, const double s0, const double t0, const double p0)
     : Object(INDEX(tmname_in,fename_in)), fluid_model_obj(dynamic_cast<EOS_Fluid&> (set_object(id())))   // init.b.
     { index_data(tmname_in, fename_in) ;
       Object obj(fluid_model_class) ;
       fluid_model_obj = dynamic_cast<EOS_Fluid&> (set_object(obj.id())) ;
       fluid_model_obj.init(fluid_model_args,init_args, h0, s0, t0, p0) ;
       err_handler = new EOS_Std_Error_Handler ;
     }
  EOS::EOS(const char *const str, const Strings &args) 
     : Object(str), fluid_model_obj(dynamic_cast<EOS_Fluid&> (set_object(id())))
     { fluid_model_obj.init(args);
       err_handler = new EOS_Std_Error_Handler ;
     }

  EOS::EOS(const char *const str, const Object &obj)
    : Object(str), fluid_model_obj(dynamic_cast<EOS_Fluid&> (set_object(id())))
     { fluid_model_obj.init(dynamic_cast<const Strings&> (get_object(obj.id())));
       err_handler = new EOS_Std_Error_Handler ;
     }                                

  // Duplicate the "handler", destroy the old handler and store the new one.
  void EOS::set_error_handler(const EOS_Error_Handler  &handler)
     { delete err_handler;
       EOS_Error_Handler  &copy = handler.duplicate();
       err_handler = &copy;
     }

  // Save current error handler for future restore on a stack.
  void EOS::save_error_handler()
     { err_handler_stack_.push(*err_handler);
     }

  void EOS::restore_error_handler()
     { const EOS_Error_Handler  *handler = err_handler_stack_.top_of_stack();
       if (handler) 
          { // If stack not empty:
            set_error_handler(*handler);
            err_handler_stack_.pop();
          } 
       else
          { // Stack empty, reset to default error handler
            set_error_handler(EOS_Std_Error_Handler());
          }
     }

  vector<string> EOS::is_implemented(const char *pprop1, const char *pprop2, 
                                     const vector<string> list_prop, double vprop1, double vprop2)
     { vector<string> list_propv ;

       char propcov1[PROPNAME_MSIZE] ;
       eostp_strcov(pprop1, propcov1) ;
       char propcov2[PROPNAME_MSIZE] ;
       eostp_strcov(pprop2, propcov2) ;

       ArrOfDouble array1(1) ;
       ArrOfDouble array2(1) ;
       ArrOfDouble result(1) ;
       ArrOfInt    errors(1) ;
       array1[0] = vprop1 ;
       array2[0] = vprop2 ;
       EOS_Field field1("TProp1", pprop1, array1) ;
       EOS_Field field2("TProp2", pprop2, array2) ;
       EOS_Error_Field err_field(errors) ;

       int nb_lps = list_prop.size() ;
       const char *pcha ;
       for (int i=0; i<nb_lps; i++)
          { pcha = list_prop[i].c_str() ;
            if (eostp_strcmp(pcha, propcov1) == 0)  continue ;
            if (eostp_strcmp(pcha, propcov2) == 0)  continue ;
            
            EOS_Field fieldr(pcha, pcha, result) ;
            fluid_model_obj.compute(field1, field2, fieldr, err_field) ;
            if (err_field[0] != EOS_Internal_Error::NOT_IMPLEMENTED)
               list_propv.push_back(list_prop[i]) ;
          }

       return list_propv ;
     }
  
  vector<string> EOS::is_implemented(const char *pprop1, const vector<string> list_prop, double vprop1)
     { vector<string> list_propv ;

       char propcov1[PROPNAME_MSIZE] ;
       eostp_strcov(pprop1, propcov1) ;

       ArrOfDouble array(1)  ;
       ArrOfDouble result(1) ;
       ArrOfInt    errors(1) ;
       array[0] = vprop1 ;
       EOS_Field field1("Tprop1", pprop1, array) ;
       EOS_Error_Field err_field(errors) ;

       int nb_lps = list_prop.size() ;
       const char *pcha ;
       for (int i=0; i<nb_lps; i++)
          { pcha = list_prop[i].c_str() ;
            if (eostp_strcmp(pcha, propcov1) == 0)  continue ;

            EOS_Field fieldr(pcha, pcha, result)  ;
            fluid_model_obj.compute(field1,fieldr, err_field) ;
            if (err_field[0] != EOS_Internal_Error::NOT_IMPLEMENTED)
               list_propv.push_back(list_prop[i]) ;
          }
       return list_propv ;
     }

  EOS::~EOS()
  { delete err_handler;
  }
  
  int EOS::test()
  { ::NEPTUNE_EOS::EOS_Mixing mix1;
    ::NEPTUNE_EOS::EOS_PerfectGas x1;
    ::NEPTUNE_EOS::EOS_StiffenedGas x22;
    ::NEPTUNE_EOS::EOS_PerfectGas x23;

    //PLUGIN INSERT CATHARE
#ifdef WITH_PLUGIN_CATHARE
    ::NEPTUNE_EOS::EOS_Cathare_Steam x4;
    ::NEPTUNE_EOS::EOS_Cathare_LiquidWater x5;
    ::NEPTUNE_EOS::EOS_Cathare_FreonR12Liquid x24;
    ::NEPTUNE_EOS::EOS_Cathare_FreonR12Vapor x25;
#endif

    //PLUGIN INSERT REFPROP_9
#ifdef WITH_PLUGIN_REFPROP_9
    ::NEPTUNE_EOS::EOS_Refprop9 x100;
    ::NEPTUNE_EOS::EOS_Refprop9_liquid x101;
    ::NEPTUNE_EOS::EOS_Refprop9_vapor x102;
#endif

    //PLUGIN INSERT REFPROP10
#ifdef WITH_PLUGIN_REFPROP_10
    ::NEPTUNE_EOS::EOS_Refprop10 x200;
    ::NEPTUNE_EOS::EOS_Refprop10_liquid x201;
    ::NEPTUNE_EOS::EOS_Refprop10_vapor x202;
#endif

     //PLUGIN INSERT THETIS
#ifdef WITH_PLUGIN_THETIS
    ::NEPTUNE_EOS::EOS_ThetisFreonR113Liquid x6;
    ::NEPTUNE_EOS::EOS_ThetisFreonR113Vapor x7;
    ::NEPTUNE_EOS::EOS_ThetisFreonR114Liquid x8;
    ::NEPTUNE_EOS::EOS_ThetisFreonR114Vapor x9;
    ::NEPTUNE_EOS::EOS_ThetisFreonR12Liquid x10;
    ::NEPTUNE_EOS::EOS_ThetisFreonR12Vapor x11;
    ::NEPTUNE_EOS::EOS_ThetisWaterLiquid x12;
    ::NEPTUNE_EOS::EOS_ThetisWaterVapor x13;
    ::NEPTUNE_EOS::EOS_ThetisWaterBMPLiquid x14;
    ::NEPTUNE_EOS::EOS_ThetisWaterBMPVapor x15;
    ::NEPTUNE_EOS::EOS_ThetisWaterBPLiquid x16;
    ::NEPTUNE_EOS::EOS_ThetisWaterBPVapor x17;
    ::NEPTUNE_EOS::EOS_ThetisWaterMPLiquid x18;
    ::NEPTUNE_EOS::EOS_ThetisWaterMPVapor x19;
    ::NEPTUNE_EOS::EOS_ThetisAmmoniaLiquid x20;
    ::NEPTUNE_EOS::EOS_ThetisAmmoniaVapor x21;
#endif

    //PLUGIN INSERT INTERPOLATOR
#ifdef WITH_IPP
    ::NEPTUNE_EOS::EOS_Ipp x26;
    ::NEPTUNE_EOS::EOS_Ipp_liquid x27;
    ::NEPTUNE_EOS::EOS_Ipp_vapor x28;
#endif

    //PLUGIN INSERT CATHARE2
#ifdef WITH_PLUGIN_CATHARE2
    ::NEPTUNE_EOS::EOS_Cathare2Liquid x111;
    ::NEPTUNE_EOS::EOS_Cathare2Vapor x112;
    ::NEPTUNE_EOS::EOS_CathareIncondensableGas x113;
#endif

    //PLUGIN INSERT HITEC
#ifdef WITH_PLUGIN_HITEC
    ::NEPTUNE_EOS::EOS_Hitec x114;
#endif

    //PLUGIN INSERT NAK
#ifdef WITH_PLUGIN_NAK
    ::NEPTUNE_EOS::EOS_Nak x115;
#endif

    //PLUGIN INSERT FLICA4
#ifdef WITH_PLUGIN_FLICA4
    ::NEPTUNE_EOS::EOS_FLICA4 flica4;
#endif

    ::NEPTUNE::EOS_Error_Field x999;
 
    return 1;
  }
}
