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



#include "EOS/API/EOS.hxx"
#include "EOS/API/EOS_Fields.hxx"
#include "EOS/API/EOS_Error_Field.hxx"
#include "EOS/API/EOS_Std_Error_Handler.hxx"
#include <stdio.h>
#include <fstream>

using namespace NEPTUNE ; 

int main(int argc, char* argv[])
{ Language_init() ;

  { if (argc != 3)
       { cerr << " 2 arguments must be given !" << endl ;
         return 1 ;
       }
    std::ifstream file(argv[1]) ;
    std::string tmp_header,header ;

    AString  tmp_read ;
    AString  typwrd,mixnam,method,reference,errtyp ;
    AString  method_0,reference_0,motcle_0,ienc_0 ;
    AString  method_1,reference_1 ;
    AString  method_2,reference_2 ;
    AString  method_3,reference_3 ;
    AString  method_4,reference_4 ;
    AString  prop,prop1,prop2 ;
    Strings  data_parameters ;
    Strings  type_parameters ;

    ArrOfInt outerr_C2 ;

    int steps ;
    int parame_max = 30 ;
    int parameters =  0 ;
    int iref       = -1 ;
    int nref       =  0 ;
    int nincl      =  0 ;
    int plancal    =  0 ;
    int inp        =  0 ;
    int inh        =  0 ;
    int inT        =  0 ;
    int ins        =  0 ;
    int inc_0      =  0 ;
    int inc_1      =  0 ;
    int inc_2      =  0 ;
    int inc_3      =  0 ;
    int inc_4      =  0 ;
    int indice     =  0 ;
    int icounts    =  0 ;
    int icount     =  0 ;
    int icounterr  =  0 ;
    int icountpc   =  0 ;
    int undefs     =  0 ;
    int undef      =  0 ;
    int opt ;
    int opt_0      =  0 ;
    int opt_1      =  0 ; 
    int opt_2      =  0 ;
    int opt_3      =  0 ;
    int opt_4      =  0 ; 

    int ninput_C2   =  -1 ;
    int ninputmp_C2 = 100 ;
    EOS_Fields inputmp_C2(ninputmp_C2) ;
    int ninput_EOS = -1 ;

    int noutputs_C2 = -1 ;
    int noutputmps_C2 = 1000 ;
    EOS_Fields outputmps_C2(noutputmps_C2) ;
    int noutputs_EOS ;

    int noutput_C2   =   -1 ;
    int noutputmp_C2 = 1000 ;
    EOS_Fields outputmp_C2(noutputmp_C2) ;
    int noutput_EOS ;

    double   cridiff ;
    cridiff = 1.e-3  ;

    double   pc_C2, pc_EOS, pc_DIF, xstep ;
    double **input_val ;

    input_val = new double* [parame_max] ;
    for (int ip = 0; ip < parame_max; ip++) 
        input_val[ip] = new double[2] ;
    for (int ip = 0; ip < parame_max; ip++)
        for (int ii=0 ; ii<2; ii++)
            input_val[ip][ii] = 0.e0 ;

    // ------------
    // Reading file
    // ------------
    
    if (file) 
       { getline(file, tmp_header) ;
         header = tmp_header + "\n" ;
         getline(file, tmp_header) ;
         header = header + tmp_header + "\n" ;
       }

    while(file)
       { file >> tmp_read ;
         if (tmp_read == "EOS")
            { if (file) file >> typwrd ;
              if (typwrd == "" ) 
                 { cerr << "Error in method definition : " << endl ;
                   return 1 ;
                 }
              if (typwrd == "EOS_Mixing")
                 { mixnam = typwrd ;
                   if (file) file >> nref ;
                   if (nref > 1) 
                      nincl = nref - 1 ;
                   else
                      { cerr << "Unvalid mixnam number : " << nref << endl ;
                        return 1 ;
                      }
                 }
              else
                 { iref++ ;
                   method = typwrd ;

                   if (method == "") 
                      { cerr << "Error in method definition : " << endl ;
                        return 1 ;
                      }
                   if (file) file >> reference ;
                   if (reference == "" )
                      { cerr << "Error in reference definition : " << endl ;
                        return 1 ;
                      }
                   if (file) file >> tmp_read ;
                   if (tmp_read == "options") 
                      { if (file) file >> opt ;
                        if (opt == 1)
                           { if (iref == 0)
                                { if (file) file >> motcle_0 ;
                                  if (file) file >> ienc_0 ;
                                }
                             else
                                { cout << "Unvalid options number for incondensable gas" << endl ;
                                  return 1 ;
                                }
                           }
                        else if (opt > 1)
                           { cout << "More than one option is not yet implemented" << endl ;
                             return 1 ;
                           }
                      }
                   else
                      { cout << "Unvalid keyword : " << tmp_read << endl ;
                        return 1 ;
                      }
                 }
              if (iref == 0)
                 { reference_0 = reference ;
                   method_0    = method ;
                   opt_0       = opt ;
                 }
              else if (iref == 1)
                 { reference_1 = reference ;
                   method_1    = method ;
                   opt_1       = opt ;
                 }
              else if (iref == 2)
                 { reference_2 = reference ;
                   method_2    = method ;
                   opt_2       = opt ;
                 }
              else if (iref == 3)
                 { reference_3 = reference ;
                   method_3    = method ;
                   opt_3       = opt ;
                 }
              else if (iref == 4)
                 { reference_4 = reference ;
                   method_4    = method ;
                   opt_4       = opt ;
                 }
            }

         if (tmp_read == "error") 
            { if (file) file >> errtyp ;
              if ((errtyp != "by_fields") && (errtyp != "by_field") && (errtyp != "ph")) 
                 { cerr << "Error in error definition type : " << errtyp << endl ;
                   return 1 ;
                 }
              if (errtyp == "by_field")
                 { cerr << "Application not yet available " << errtyp << endl ;
                   return 1 ;
                 }
            }

         if (tmp_read == "steps") 
            { if (file) file >> steps ;
              if (steps == 0) 
                 { cerr << "Error in steps definition : " << steps << endl ;
                   return 1 ;
                 }
            }

         if (tmp_read == "parameters") 
            { if (file) file >> parameters ;
              if (parameters < 2 || parameters > parame_max) 
                 { cerr << "Error in parameters definition : " << parameters 
                        << "]2, " << parame_max << "]" << endl ;
                   return 1 ;
                 }

              data_parameters.resize(parameters) ;
              type_parameters.resize(parameters) ;
              for (int ip = 0; ip < parameters; ip++) 
                 { if (file) file >> data_parameters[ip] >> type_parameters[ip] ;
                   if      (type_parameters[ip] == "const")
                      { if (file) file >> input_val[ip][0] ;
                      }
                   else if (type_parameters[ip] == "range")
                      { for (int ii=0 ; ii<2; ii++)
                        { if (file) file >> input_val[ip][ii] ;
                        }
                      }
                 }
            }

         if (tmp_read == "input") 
            { if (file) file >> prop ;
              ninput_C2++ ;
              inputmp_C2[ninput_C2].set_property_name(prop.aschar())  ;
              inputmp_C2[ninput_C2].set_property_title(prop.aschar()) ;
              inputmp_C2[ninput_C2].set_data().resize(steps) ;
              for (int ii=0; ii<steps; ii++) 
                 { if (file) file >> inputmp_C2[ninput_C2][ii] ;
                 }
            }

         if (tmp_read == "output") 
            { if (file) file >> prop1 ;
              if (prop1 != "p_crit" && prop1 != "error" ) 
                 { if (file) file >> prop2 ;
                   if (prop2 == "p") 
                      { noutputs_C2++ ;
                        outputmps_C2[noutputs_C2].set_property_name(prop1.aschar())  ;
                        outputmps_C2[noutputs_C2].set_property_title(prop1.aschar()) ;
                        outputmps_C2[noutputs_C2].set_data().resize(steps) ;
                        for (int ii=0; ii<steps; ii++) 
                           { if (file) file >> outputmps_C2[noutputs_C2][ii] ;
                           }
                      }
                   else if ((prop2 == "ph") || (prop2 == "pT") || (prop2 == "ps")) 
                      { if (prop2 == "ph") plancal = 1 ;     
                        if (prop2 == "pT") plancal = 2 ;     
                        if (prop2 == "ps") plancal = 3 ;     

                        noutput_C2++ ;
                        outputmp_C2[noutput_C2].set_property_name(prop1.aschar())  ;
                        outputmp_C2[noutput_C2].set_property_title(prop1.aschar()) ;
                        outputmp_C2[noutput_C2].set_data().resize(steps) ;
                        for (int ii=0; ii<steps; ii++) 
                           { if (file) file >> outputmp_C2[noutput_C2][ii] ;
                           }
                      } 
                 }
              else if (prop1 == "error")
                 { if (file) file >> tmp_read ;
                   outerr_C2.resize(steps) ;
                   for (int ii = 0; ii<steps; ii++) 
                      { if (file) file >> outerr_C2[ii] ;
                      }
                 }
              else if (prop1 == "p_crit")
                 { if (file) file >> pc_C2 ;
                 }
            }
       }

    //  --------------------------------------------------
    //  - Redimentionnement des input_C2 a la bonne taille
    //  --------------------------------------------------

    EOS_Fields input_C2(ninput_C2+1) ;
    for (int i=0; i< ninput_C2+1; i++)
      input_C2[i] = inputmp_C2[i] ;

    EOS_Fields outputs_C2(noutputs_C2+1) ;
    for (int i=0; i< noutputs_C2+1; i++)
      outputs_C2[i] = outputmps_C2[i] ;

    EOS_Fields output_C2(noutput_C2+1) ;
    for (int i=0; i< noutput_C2+1; i++)
       { output_C2[i] = outputmp_C2[i] ;
         prop = output_C2[i].get_property_name() ;
       }

    //  --------------------------
    //  Computing of the EOS input 
    //  --------------------------

    ninput_EOS = ninput_C2 ;
    EOS_Fields input_EOS(ninput_EOS+1) ;
    EOS_Fields input_DIF(ninput_EOS+1) ;

    int steps1 = steps-1 ;
    for (int ip = 0; ip < parameters; ip++) 
       { prop = data_parameters[ip] ;
         input_EOS[ip].set_property_name(prop.aschar())  ;
         input_EOS[ip].set_property_title(prop.aschar()) ;
         input_EOS[ip].set_data().resize(steps)          ;
         input_DIF[ip].set_property_name(prop.aschar())  ;
         input_DIF[ip].set_property_title(prop.aschar()) ;
         input_DIF[ip].set_data().resize(steps)          ;
         xstep = (input_val[ip][1] - input_val[ip][0]) / steps1 ;
         for (int is=0 ; is < steps ; is++)
            { if (is == 0)
                 { if      (data_parameters[ip] == "p")   inp   = ip ;
                   else if (data_parameters[ip] == "h")   inh   = ip ;
                   else if (data_parameters[ip] == "T")   inT   = ip ;
                   else if (data_parameters[ip] == "s")   ins   = ip ;
                   else if (data_parameters[ip] == "c_0") inc_0 = ip ;
                   else if (data_parameters[ip] == "c_1") inc_1 = ip ;
                   else if (data_parameters[ip] == "c_2") inc_2 = ip ;
                   else if (data_parameters[ip] == "c_3") inc_3 = ip ;
                   else if (data_parameters[ip] == "c_4") inc_4 = ip ;
                 }
              if (type_parameters[ip] == "const") 
                 { input_EOS[ip][is] = input_val[ip][0] ;
                   input_DIF[ip][is] = input_EOS[ip][is] - input_C2[ip][is] ;
                 }
              else if (type_parameters[ip] == "range") 
                 { input_EOS[ip][is] = input_val[ip][0] + is*xstep ;
                   input_DIF[ip][is] = input_EOS[ip][is] - input_C2[ip][is] ;
                 }
            }
       }
    


    // ---------------------
    // creation de EOS fluid
    // ---------------------
    //
    EOS **components = new EOS*[nref] ;
    EOS *fluid[1] ;

    if (nref == 0)
      fluid[0] = new EOS(method_0.aschar(),reference_0.aschar()) ;    
    else
      fluid[0] = new EOS(mixnam.aschar()) ;

    EOS_Std_Error_Handler handler ;
    handler.set_exit_on_error(EOS_Std_Error_Handler::disable_feature) ;
    handler.set_dump_on_error(EOS_Std_Error_Handler::disable_feature) ;
    fluid[0]->save_error_handler() ;
    fluid[0]->set_error_handler(handler) ;


    if (nref > 0)
       { for (int iref=0; iref<nref; iref++)
            { if (iref == 0)
                 { reference = reference_0 ;
                   method    = method_0    ;
                 }
              else if (iref == 1)
                 { reference = reference_1 ;
                   method    = method_1    ;
                 }
              else if (iref == 2)
                 { reference = reference_2 ;
                   method    = method_2    ;
                 }
              else if (iref == 3)
                 { reference = reference_3 ;
                   method    = method_3    ;
                 }
              else if (iref == 4)
                 { reference = reference_4 ;
                   method    = method_4    ;
                 }
              components[iref] = new EOS(method.aschar(),reference.aschar()) ;
            }
         fluid[0]->set_components(components,nref) ;
       }


    if (opt_0 == 1)
       { Strings option(2) ;
         option[0] = motcle_0 ;
         option[1] = ienc_0   ;
         fluid[0]->init(option) ;
       }

    // -------------------------------------------------
    // Saturation value and Fluid properties computation
    // -------------------------------------------------
    //

    ArrOfInt errs_(steps) ;
    EOS_Error_Field errs(errs_) ;
  
    ArrOfInt err_(steps) ;
    EOS_Error_Field err(err_) ;
    EOS_Error crs ;
 
    int nb_incals = 1+nref ;
    EOS_Fields incals(nb_incals) ;

    vector<string> vecs_incals ;

    vecs_incals.push_back("P ") ;
    incals[0] = input_EOS[inp] ;

    if (nref  > 0) 
       { vecs_incals.push_back("c0") ;
         incals[1] = input_EOS[inc_0] ;
       }
    if (nincl > 0) 
       { vecs_incals.push_back("c1") ;
         incals[2] = input_EOS[inc_1] ;
       }
    if (nincl > 1) 
       { vecs_incals.push_back("c2") ;
         incals[3] = input_EOS[inc_2] ;
       }
    if (nincl > 2) 
       { vecs_incals.push_back("c3") ;
         incals[4] = input_EOS[inc_3] ;
       }
    if (nincl > 3) 
       { vecs_incals.push_back("c4") ;
         incals[5] = input_EOS[inc_4] ;
       }
  
    ArrOfInt cans(noutputs_C2+1) ;
    EOS_Fields testvals(1) ;
    EOS_Fields outputs_EOS_tmp(noutputs_C2+1) ;
    int noutputs_EOSb = -1 ;

    if (noutputs_C2 >= 0) 
       { for (int i=0; i<noutputs_C2+1; i++) 
            { cans[i] = -1 ;
              ArrOfDouble xtmp(steps) ;
              testvals[0]=EOS_Field("prop",outputs_C2[i].get_property_name().aschar(),xtmp) ;
              crs=fluid[0]->compute(incals,testvals,errs) ;
              if (crs != NEPTUNE::error)
                 { noutputs_EOSb++ ;
                   cans[i] = noutputs_EOSb ;
                   prop = outputs_C2[i].get_property_name() ;
                   outputs_EOS_tmp[noutputs_EOSb].set_property_name(prop.aschar())  ;
                   outputs_EOS_tmp[noutputs_EOSb].set_property_title(prop.aschar()) ;
                 }
              else
                undefs = 1 ;
            }
       }

    noutputs_EOS = noutputs_EOSb ;
    EOS_Fields outputs_EOS(noutputs_EOS+1) ;
    EOS_Fields outputs_DIF(noutputs_EOS+1) ;
    if (noutputs_EOS+1 > 0)
       {
         for (int i=0; i<noutputs_EOS+1; i++) 
            { prop = outputs_EOS_tmp[i].get_property_name()    ;
              outputs_EOS[i].set_property_name(prop.aschar())  ;
              outputs_EOS[i].set_property_title(prop.aschar()) ;
              outputs_EOS[i].set_data().resize(steps) ;
            }

         
         for (int i=0; i<noutputs_EOS+1; i++) 
            { prop = outputs_EOS[i].get_property_name()        ;
              outputs_DIF[i].set_property_name(prop.aschar())  ;
              outputs_DIF[i].set_property_title(prop.aschar()) ;
              outputs_DIF[i].set_data().resize(steps) ;
            }

         crs = fluid[0]->compute(incals,outputs_EOS,errs) ;
         
         double valdif, vallim, valout ;
         int iti ;
         for (int i=0; i <noutputs_C2+1; i++) 
            { if (cans[i] > -1)
                 { iti = 0 ;
                   for (int j=0 ; j < steps ; j++) 
                      { // outputs_DIF[cans[i]][j] = outputs_EOS[cans[i]][j] - outputs_C2[i][j] ;
                        valdif = outputs_EOS[cans[i]][j] - outputs_C2[i][j] ;
                        vallim = fmax(1.e-8,cridiff*fabs(outputs_C2[i][j])) ;
                        valout = outputs_EOS[cans[i]][j]*outputs_C2[i][j]   ;
                        if ( fabs(valdif) >= vallim || valout < 0.e+0 ) 
                           { icounts++ ;
                             iti++ ;
                             prop = outputs_EOS[cans[i]].get_property_name() ;
                             if (iti == 1) 
                                cout << " Saturation  curve, res.field n." << std::setw(2) << i+1 << " prop. " << std::setw(18) << std::left << prop ;
                             else
                                cout << "                                                          " ;
                             cout << " pt  n." << std::setw(2) << j+1 << " :" ;
                             for (int k=0; k<nb_incals; k++)
                                cout << " " << std::setw(4) << vecs_incals[k]  << " = " << std::setw(12) << incals[k][j] ;
                             cout << endl ;
                             cout << "                                                     " ;
                             cout << "      res :" 
                                  << " cal  = " << std::setw(12) << outputs_EOS[cans[i]][j]
                                  << " ref  = " << std::setw(12) << output_C2[i][j] 
                                  << " DIFF = " << std::setw(12) << valdif
                                  << " MAX  = " << std::setw(12) << cridiff*fabs(outputs_C2[i][j]) << endl << endl ; 
                           }
                      }
                 }
            }
       }

    int nb_incald = 2 + nref ;
    EOS_Fields incald(nb_incald) ;

    vector<string> vecs_incald ;

    vecs_incald.push_back("P ") ;
    incald[0] = input_EOS[inp] ;
    if (plancal == 1) 
       { vecs_incald.push_back("h ") ;
         incald[1] = input_EOS[inh] ;
       }
    else if (plancal == 2)
       { vecs_incald.push_back("T ") ;
         incald[1] = input_EOS[inT] ;
       }
    else if (plancal == 3) 
       { vecs_incald.push_back("s ") ;
         incald[1] = input_EOS[ins] ;
       }
    
    if (nref > 0)  
       { vecs_incald.push_back("c0") ;
         incald[2] = input_EOS[inc_0] ;
       }
    if (nincl > 0)  
       { vecs_incald.push_back("c1") ;
         incald[3] = input_EOS[inc_1] ;
       }
    if (nincl > 1) 
       { vecs_incald.push_back("c2") ;
         incald[4] = input_EOS[inc_2] ;
       }
    if (nincl > 2) 
       { vecs_incald.push_back("c3") ;
         incald[5] = input_EOS[inc_3] ;
       }
    if (nincl > 3) 
       { vecs_incald.push_back("c4") ;
         incald[6] = input_EOS[inc_4] ;
       }
  
    ArrOfInt can(noutput_C2+1) ;
    EOS_Fields testval(1) ;
    EOS_Fields output_EOS_tmp(noutput_C2+1) ;
    int noutput_EOSb = -1 ;

    if (noutput_C2 >= 0) 
       { for (int i=0; i< noutput_C2+1; i++) 
            { can[i] = -1 ;
              ArrOfDouble xtmp(steps);
              testval[0] = EOS_Field("prop",output_C2[i].get_property_name().aschar(),xtmp) ;
              crs = fluid[0]->compute(incald,testval,errs) ;
                  
              if (crs != NEPTUNE::error)
                 { noutput_EOSb++ ;
                   can[i] = noutput_EOSb ;
                   prop = output_C2[i].get_property_name() ;
                   output_EOS_tmp[noutput_EOSb].set_property_name(prop.aschar())  ;
                   output_EOS_tmp[noutput_EOSb].set_property_title(prop.aschar()) ;
                 }
              else
                 undef++ ;
            }
       }

    noutput_EOS = noutput_EOSb ;
    EOS_Fields output_EOS(noutput_EOS+1) ;
    for (int i=0; i<noutput_EOS+1; i++) 
       { prop = output_EOS_tmp[i].get_property_name()    ;
         output_EOS[i].set_property_name(prop.aschar())  ;
         output_EOS[i].set_property_title(prop.aschar()) ;
         output_EOS[i].set_data().resize(steps) ;
       }

    EOS_Fields output_DIF(noutput_EOS+1) ;
    for (int i=0; i<noutput_EOS+1; i++) 
       { prop = output_EOS[i].get_property_name() ;
         output_DIF[i].set_property_name(prop.aschar())  ;
         output_DIF[i].set_property_title(prop.aschar()) ;
         output_DIF[i].set_data().resize(steps) ;
       }

    if (plancal == 1) 
       { fluid[0]->compute(incald,output_EOS, err) ;
         
         double valdif, vallim, valout ;
         int iti ;
         for (int i=0; i<noutput_C2+1; i++) 
            { if (can[i] > -1)
                 { iti = 0 ;
                   for (int j=0; j<steps; j++) 
                      { // output_DIF[can[i]][j] = output_EOS[can[i]][j] - output_C2[i][j] ;
                        valdif = output_EOS[can[i]][j] - output_C2[i][j]   ;
                        vallim = fmax(1.e-8,cridiff*fabs(output_C2[i][j])) ;
                        valout = output_EOS[can[i]][j]*output_C2[i][j]     ;
                        if ( fabs(valdif) >= vallim || valout < 0.e0 ) 
                           { icount++ ;
                             iti++ ;
                             prop = output_EOS[can[i]].get_property_name();
                             if (iti == 1) 
                                cout << " P-h diagram, res.field n." << std::setw(2) << i+1 << " prop. " << std::setw(18) << std::left << prop ;
                             else
                                cout << "                                                     " ;
                             cout << " pt  n." << std::setw(2) << j+1 << " :" ;
                             for (int k=0; k<nb_incald; k++)
                                cout << " " << std::setw(4) << vecs_incald[k]  << " = " << std::setw(12) << incald[k][j] ;
                             cout << endl ;
                             cout << "                                                     " ;
                             cout << "      res :" 
                                  << " cal  = " << std::setw(12) << output_EOS[can[i]][j]
                                  << " ref  = " << std::setw(12) << output_C2[i][j] 
                                  << " DIFF = " << std::setw(12) << valdif
                                  << " MAX  = " << std::setw(12) << cridiff*fabs(output_C2[i][j]) << endl << endl ; 
                           }
                      }
                 }
            }
       }

    else if (plancal == 2 || plancal == 3) 
       { fluid[0]->compute(incald,output_EOS, err) ;
         double valdif, vallim, valout ;
         int iti ;
         for (int i=0; i<noutput_EOS+1; i++) 
            { if (can[i] > -1)
                 { iti = 0 ;
                    for (int j=0 ; j<steps; j++) 
                      { // output_DIF[i][j] = output_EOS[can[i]][j] - output_C2[i][j] ;
                        valdif = output_EOS[can[i]][j] - output_C2[i][j]   ;
                        vallim = fmax(1.e-8,cridiff*fabs(output_C2[i][j])) ;
                        valout = output_EOS[can[i]][j]*output_C2[i][j]     ;
                        if ( fabs(valdif) >= vallim || valout < 0.e0 )
                           { icount++ ;
                              iti++ ;
                             prop = output_EOS[can[i]].get_property_name();
                             if (iti == 1) 
                                { if (plancal == 2)
                                     cout << " P-T diagram, res.field n." << std::setw(2) << i+1 << " prop. " << std::setw(18) << std::left << prop ;
                                  else if (plancal == 3)
                                     cout << " P-s diagram, res.field n." << std::setw(2) << i+1 << " prop. " << std::setw(18) << std::left << prop ;
                                }
                             else
                                cout << "                                                     " ;
                             cout << " val n." << std::setw(2) << j+1 << " : " 
                                  << "  CAL  = " << std::setw(12) << output_EOS[can[i]][j]
                                  << "  REF  = " << std::setw(12) << output_C2[i][j] 
                                  << "  DIFF = " << std::setw(12) << valdif
                                  << "  MAX  = " << std::setw(12) << cridiff*fabs(output_C2[i][j]) << endl ;
                           }
                        
                      }
                 }
            }
       }


    // -----------------
    // PCRIT computation 
    // -----------------
    //
    if (nref == 0)
      fluid[0]->get_p_crit(pc_EOS) ;
    else
      pc_EOS = 0.e0 ;

    pc_DIF = pc_EOS - pc_C2 ;
    if (fabs(pc_DIF) >= fmax(1.e-8,cridiff*fabs(pc_C2)) || pc_EOS*pc_C2 < 0.e0 ) icountpc++ ;


    // -----------------------
    // Worst error computation
    // -----------------------
    //
    ArrOfInt outerr_DIF(steps) ;

    err.set_worst_error(errs) ;

    for (int is=0; is<steps; is++) 
       { outerr_DIF[is] = err[is].get_partial_code() - outerr_C2[is] ;
         if (fabs(outerr_DIF[is]) != 0) icounterr++ ;
       }


    // --------------------
    // Output file writting
    // --------------------
    //

    std::ofstream output_file(argv[2]) ;
    if (output_file)
       { output_file << header ;

         if (nref == 0)
            { if (opt_0 == 1)
                 output_file << "EOS "     << method_0 << " " << reference_0 << " " 
                             << "options " << opt_0    << " " << motcle_0    << " " 
                             << ienc_0 << endl ;
              else
                 output_file << "EOS "     << method_0 << " " << reference_0 << " " 
                             << "options " << opt_0    << endl ;
            }
         else
            { output_file << "EOS " << mixnam << " " << nref << endl ;
              if (opt_0 == 1)
                 output_file << "EOS "     << method_0 << " " << reference_0 << " " 
                             << "options " << opt_0    << " " << motcle_0    << " " 
                             << ienc_0 << endl ;
              else
                 output_file << "EOS "     << method_0 << " " << reference_0 << " " 
                             << "options " << opt_0    << endl ;

              for (int iref=1; iref<nref; iref++)
                 { if (iref == 1)
                      { reference = reference_1 ;
                        method    = method_1   ;
                        opt = opt_1 ;
                      }
                   else if (iref == 2)
                      { reference = reference_2 ;
                        method    = method_2    ;
                        opt = opt_2 ;
                      }
                   else if (iref == 3)
                      { reference = reference_3 ;
                        method    = method_3    ;
                        opt = opt_3 ;
                      }
                   else if (iref == 4)
                      { reference = reference_4 ;
                        method    = method_4    ;
                        opt = opt_4 ;
                      }
                   output_file << "EOS "     << method << " " << reference << " "
                               << "options " << opt << endl ;
                 }
            }

         output_file << "error " << errtyp << endl ;
         output_file << "steps " << steps  << endl ;
         output_file << "parameters " << parameters << endl ;

         for (int ip=0; ip<parameters; ip++)
            { if (data_parameters[ip] == "p" || data_parameters[ip] == "h")
                 output_file << data_parameters[ip] << "   " 
                             << type_parameters[ip] << " " ;
              else
                 output_file << data_parameters[ip] << " " 
                             << type_parameters[ip] << " " ;
              
              if (type_parameters[ip] == "const" )
                indice = 1 ;
              else if (type_parameters[ip] == "range" )
                indice = 2 ;

              for (int i=0; i<indice; i++)
                 output_file << std::setfill(' ') << std::setw(15) << std::right 
                             << std::setprecision(6) << std::scientific 
                             << input_val[ip][i] ;
                
              output_file << endl ;
            }
         output_file << endl ;

         for (int ip=0 ; ip<parameters; ip++)
            { output_file << "input " << data_parameters[ip] << endl ;
              for (int i_step=0; i_step<steps; i_step++)
                output_file << std::setfill(' ') << std::setprecision(6) << std::scientific
                            << std::setw(15)<< std::right << input_C2[ip][i_step]
                            << std::setw(15)<< std::right << input_EOS[ip][i_step]
                            << std::setw(15)<< std::right << input_DIF[ip][i_step] << endl ;
            }

         if (noutputs_C2 >= 0)
            { for (int i_outputs = 0 ; i_outputs < noutputs_C2+1 ; i_outputs++) 
                 { if (cans[i_outputs] == -1)
                      output_file << "                                                 Property not implemented in EOS : " 
                                  << outputs_C2[i_outputs].get_property_name()  << endl ;
                   else
                      { output_file << "output " << outputs_C2[i_outputs].get_property_name() << " " << "p" << endl ;
                        for (int i_step=0; i_step<steps; i_step++)
                            output_file << std::setfill(' ') << std::setprecision(6) << std::scientific
                                    << std::setw(15)<< std::right << outputs_C2[i_outputs][i_step]
                                    << std::setw(15)<< std::right << outputs_EOS[cans[i_outputs]][i_step]
                                    << std::setw(15)<< std::right << outputs_DIF[cans[i_outputs]][i_step] << endl ;
                      }
                 }
            }

         if (noutput_C2 >= 0) 
            { for (int i_output=0; i_output<noutput_C2+1; i_output++) 
                 { if (can[i_output] == -1)
                      output_file << "                                               Property not implemented in EOS : " 
                                  << output_C2[i_output].get_property_name() << endl ;
                    else
                       { if (plancal == 1)    
                            output_file << "output " << output_C2[i_output].get_property_name() 
                                        << " " << "ph" << endl ;
                         else if (plancal == 2) 
                            output_file << "output " << output_C2[i_output].get_property_name()
                                        << " " << "pT" << endl ;
                         else if (plancal == 3) 
                            output_file << "output " << output_C2[i_output].get_property_name() 
                                        << " " << "ps" << endl ;
                       }

                   if (can[i_output] != -1)
                      { for (int i_step=0; i_step<steps; i_step++)
                            output_file << std::setfill(' ') << std::setprecision(6) << std::scientific
                                        << std::setw(15) << std::right << output_C2[i_output][i_step]
                                        << std::setw(15) << std::right << output_EOS[can[i_output]][i_step] 
                                        << std::setw(15) << std::right << output_DIF[can[i_output]][i_step] << endl ;
                      }
                 }
            }

         if (plancal == 1)
            output_file << "output " << "error " << "ph" << endl ;
         else if (plancal == 2)
            output_file << "output " << "error " << "pT" << endl ;
         else if (plancal == 3)
            output_file << "output " << "error " << "ps" << endl ;
         
         for (int i_step=0; i_step<steps; i_step++)
            output_file <<  outerr_C2[i_step] << " "    
                        <<  err[i_step].get_partial_code() << " "    
                        <<  outerr_DIF[i_step] << endl ;

         output_file << "output " << "p_crit" << endl ;
         output_file << std::setfill(' ') << std::setprecision(6) << std::scientific
                 << std::setw(15) << std::right << pc_C2
                 << std::setw(15) << std::right << pc_EOS
                 << std::setw(15) << std::right << pc_DIF << endl ;
            
         if (icounts > 0)
            { output_file << endl ;
              output_file << "Number of major differences between EOS and C2 for saturation properties : " << icounts << endl ;
            }
         else
            { output_file << endl ;
              output_file << "No difference found between EOS and C2 for saturation properties." << endl ;
            }
             
         if (icount > 0)
            output_file << "Number of major differences between EOS and C2 for the other properties : " << icount << endl ;
         else
            output_file << "No difference found between EOS and C2 for the other properties." << endl ;

         if (icounterr > 0)
            { output_file << "Number of major differences between EOS and C2 for the error_field : " << icounterr << endl ;
              output_file << endl ;
            }
         else
            { output_file << "No difference found between EOS and C2 for the error_field." << endl ;
              output_file << endl ;
            }
            
         if (undefs > 0)
            output_file << "Number of saturation properties not implemented : " << undefs << endl ;
         if (undef > 0)
            { output_file << "Number of properties not implemented            : " << undef << endl ;
             output_file << endl ;
            }

         if (icountpc > 0)
            { if (nref == 0)
                 { output_file << "Critical pressure is not matching between EOS and C2." << endl;
                   output_file << endl ;
                 }
              else
                 { output_file << "With incondensable gases critical pressure is not yet implemented. " << endl;
                   output_file << endl ;
                 }
            }
       }

    for (int ip=0; ip<parame_max; ip++)
       delete [] input_val[ip] ;
    delete [] input_val ;
    for (int iref=0; iref<nref; iref++)
      delete components[iref];
    delete fluid[0]  ;
    delete[] components ;
    if ( (icounts > 0) || (icount > 0) || (icounterr > 0) )  return 1 ;

  }

  Language_finalize() ;
  return 0 ;
}
