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



#include "EOS_PerfectGas.hxx"
#include "EOS/API/EOS.hxx"
#include "EOS/API/EOS_Field.hxx"
#include "Language/API/RegisterType.hxx"
#include "Language/API/Types_Info.hxx"
#include "EOS/API/EOS_Config.hxx"
#include "EOS/API/EOS_eosdatadir.hxx"
#include <fstream>

namespace NEPTUNE_EOS
{
  const AString EOS_PerfectGas::tablename("PerfectGas") ;
  const AString EOS_PerfectGas::versionname("V1") ;

  static RegisteredClass& EOS_PerfectGas_create()
  { return *(new EOS_PerfectGas()) ;
  }

  int EOS_PerfectGas::type_Id = (RegisterType("EOS_PerfectGas", "EOS_Fluid",
                                 "Equation Of States for Perfect Gas",
                                 sizeof(EOS_PerfectGas),
                                 EOS_PerfectGas_create)) ;

  const AString& EOS_PerfectGas::table_name()const
  { return tablename ;
  }

  const AString& EOS_PerfectGas::version_name() const
  { return versionname ;
  }
  
  ostream& EOS_PerfectGas::print_On (ostream& stream) const
  { return stream << "r " << r << " gamma " << gamma << endl ;
  }

  istream& EOS_PerfectGas::read_On (istream& stream)
  { return stream ;
  }

  const Type_Info& EOS_PerfectGas::get_Type_Info () const
  { return (Types_Info::instance())[type_Id] ;
  }

  const AString& EOS_PerfectGas::fluid_name() const
  { return FluidStr ;
  }

  int EOS_PerfectGas::init(const Strings& strings)
  {
    FluidStr=AString("unknown") ;   
    href = 0.e0 ;
    sref = 0.e0 ;
    Tref = 0.e0 ;
    Pref = 1.015e5 ;
    int sz=strings.size() ;

    if (sz == 1)
       { AString data_file_name ;
         if (iret_eos_data_dir) return EOS_Error::error ;
         data_file_name  = eos_data_dir.c_str() ;
         data_file_name += "/EOS_PerfectGas/" ;
         AString& file_name  = strings[0] ;
         data_file_name     += file_name ;
         std::ifstream data_file(data_file_name.aschar()) ;
         if (!data_file)
            { cerr << data_file_name << " not found!" << endl;
              assert(0) ;
              exit(-1)  ;
              return EOS_Error::error ;
            }
         // reading data_file
         else
            { //      cerr << "Reading file " << data_file_name << " ... " << std::flush;
              int ic =  0 ;
              int nc = 30 ;
              AString str("EmptyString") ;
              data_file >> str ;
              while(str != "" && ic < nc)
                { if (str=="Fluid")
                    { data_file >> FluidStr;
                      data_file >> str;
                      ic++ ;
                    }
                  else if (str=="xr")
                    { data_file >> prxr;
                      data_file >> str;
                      ic++ ;
                    }
                  else if (str=="xcp")
                    { data_file >> prxcp;
                      data_file >> str;
                      ic++ ;
                    }
                  else if (str=="xm")
                    { data_file >> prxm;
                      data_file >> str;
                      ic++ ;
                    }
                  else if (str=="xl0")
                    { data_file >> prxl0;
                      data_file >> str;
                      ic++ ;
                    }
                  else if (str=="xl1")
                    { data_file >> prxl1;
                      data_file >> str;
                      ic++ ;
                    }
                  else if (str=="xl2")
                    { data_file >> prxl2;
                      data_file >> str;
                      ic++ ;
                    }
                  else if (str=="xm0")
                    { data_file >> prxm0;
                      data_file >> str;
                      ic++ ;
                    }
                  else if (str=="xm1")
                    { data_file >> prxm1;
                      data_file >> str;
                      ic++ ;
                    }
                  else if (str=="xm2")
                    { data_file >> prxm2;
                      data_file >> str;
                      ic++ ;
                    }
                  else if (str=="xdv")
                    { data_file >> prxdv;
                      data_file >> str;
                      ic++ ;
                    }
                  else
                    { cerr << "Ref. " << str << " not recognized !" << endl;
                      assert(0);
                      exit(-1) ;
                      return EOS_Error::error ;
                    }
                }
              r  = prxr  ; 
              cp = prxcp ;
            }
         //        AString index_filename=getenv("NEPTUNE_EOS_DATA");
         //        index_filename+="/EOS_PerfectGas/index.eos";
         //        std::ifstream index_file(index_filename.aschar());
         //        if(!index_file)
         //          {
         //            cerr << index_filename << " File not found!" << endl;
         //            assert(0);
         //            exit(-1);
         //          }
         //        AString str("EmptyString");
         //        const AString& ref_name=strings[0];
         //        int nc=1000;
         //        int ic=0;
         //        while(str!=ref_name && ic<nc)
         //          {
         //            while (str!="Reference" && ic<nc)
         //              {
         //                index_file >> str;
         //                ic++;
         //              }
         //            index_file >> str;
         //          }
         //        if(str==ref_name)
         //          {
         //            AString data_filename=getenv("NEPTUNE_EOS_DATA");
         //            data_filename+="/EOS_PerfectGas/";
         //            index_file >> str;
         //            if (str!="File")
         //              {
         //                cerr << " The keyword File is not found in EOS_PerfectGas/index.eos " << endl;
         //                assert(0);
         //                exit(-1);
         //              }
         //            index_file >> str;
         //            data_filename+=str;             
         //            cout << "Reading " << data_filename << endl;
         //            std::ifstream data_file(data_filename.aschar());
         //            if(data_file)
         //              {
         //                ic=0;
         //                nc=30;
         //                data_file >> str;
         //                while(str!="" && ic<nc)
         //                  {
         //                    if (str=="Fluid")
         //                      {
         //                        data_file >> FluidStr;
         //                        data_file >> str;
         //                        ic++;
         //                      }
         //                    else if (str=="xr")
         //                      {
         //                        data_file >> prxr;
         //                        data_file >> str;
         //                        ic++;
         //                      }
         //                    else if (str=="xcp")
         //                      {
         //                        data_file >> prxcp;
         //                        data_file >> str;
         //                        ic++;
         //                      }
         //                    else if (str=="xm")
         //                      {
         //                        data_file >> prxm;
         //                        data_file >> str;
         //                        ic++;
         //                      }
         //                    else if (str=="xl0")
         //                      {
         //                        data_file >> prxl0;
         //                        data_file >> str;
         //                        ic++;
         //                      }
         //                    else if (str=="xl1")
         //                      {
         //                        data_file >> prxl1;
         //                        data_file >> str;
         //                        ic++;
         //                      }
         //                    else if (str=="xl2")
         //                      {
         //                        data_file >> prxl2;
         //                        data_file >> str;
         //                        ic++;
         //                      }
         //                    else if (str=="xm0")
         //                      {
         //                        data_file >> prxm0;
         //                        data_file >> str;
         //                        ic++;
         //                      }
         //                    else if (str=="xm1")
         //                      {
         //                        data_file >> prxm1;
         //                        data_file >> str;
         //                        ic++;
         //                      }
         //                    else if (str=="xm2")
         //                      {
         //                        data_file >> prxm2;
         //                        data_file >> str;
         //                        ic++;
         //                      }
         //                    else if (str=="xdv")
         //                      {
         //                        data_file >> prxdv;
         //                        data_file >> str;
         //                        ic++;
         //                      }
         //                    else
         //                      {
         //                        cerr << "Ref. " << str << " not recognized !" << endl;
         //                        assert(0);
         //                        exit(-1);
         //                        return EOS_Error::error;
         //                      }
         //                    r=prxr;
         //                    cp=prxcp;
         //                  }
         //              }
         //            else
         //              {
         //                cerr << data_filename << " file not found!" << endl;
         //                assert(0);
         //                exit(-1);
         //                return EOS_Error::error ;
         //              }
         //          }
         //        else
         //          {
         //            cerr << "Sorry not implemented yet ... " << ref_name  << endl;
         //            assert(0);
         //            exit(-1);
         //            return EOS_Error::error ;
         //          }



         //        if(strings[0]==AString("Nitrogen"))
         //          {
         //            FluidStr=AString("Nitrogen");
         //            prxr=297.2E+0;
         //            prxcp= 1056.E+0;
         //            prxm=28.E+0;
         //            prxl0=5.04526315E-3;
         //            prxl1=0.0722206015E-3;
         //            prxl2=-14.24661654E-9;
         //            prxm0= 4.87051242E-6;
         //            prxm1=0.0443825188E-6;
         //            prxm2=-9.058270677E-12;
         //            prxdv=18.5E+0;
         //            r=prxr;
         //            cp=prxcp;
         //            return EOS_Error::good ;
         //          }
         //        if(strings[0]==AString("Hydrogen"))
         //          {
         //            FluidStr=AString("Hydrogen");
         //            prxr=4130.E+0;
         //            prxcp=14500.E+0;
         //            prxm=2.E+0;
         //            prxl0=52.27894741E-3;
         //            prxl1=0.4292590225E-3;
         //            prxl2=-52.69924812E-9;
         //            prxm0=3.2663E-6;
         //            prxm1=0.0191166996E-6;
         //            prxm2=-2.375345865E-12;
         //            prxdv=6.12E0;
         //            r=prxr;
         //            cp=prxcp;
         //            return EOS_Error::good ;
         //          }
         //        if(strings[0]==AString("Helium"))
         //          {
         //            FluidStr=AString("Helium");
         //            prxr=2077.2E+0;
         //            prxcp=5193.E+0;
         //            prxm=4.003E+0;
         //            prxl0=2.7E-2;
         //            prxl1= 4.4E-4;
         //            prxl2=0.0E+0;
         //            prxm0=4.0E-6;
         //            prxm1=5.44E-8;
         //            prxm2=0.0E+0;
         //            prxdv=2.67E+0;
         //            r=prxr;
         //            cp=prxcp;
         //            return EOS_Error::good ;
         //          }
         //        if(strings[0]==AString("Oxygen"))
         //          {
         //            FluidStr=AString("Oxygen");
         //            prxr=263.E+0;
         //            prxcp=57.78E+0;
         //            prxm=31.9988E+0;
         //            prxl0=0.00101E+0;
         //            prxl1=9.11E-05;
         //            prxl2=-2.09E-08;
         //            prxm0=0.000944E+0;
         //            prxm1=4.92E-06;
         //            prxm2=-1.03E-09;
         //            prxdv=16.3E+0;
         //            r=prxr;
         //            cp=prxcp;
         //            return EOS_Error::good ;
         //          }
         //        if(strings[0]==AString("Argon"))
         //          {
         //            FluidStr=AString("Argon");
         //            prxr=2.08100E+02;
         //            prxcp=5.19650E+02;
         //            prxm=4.00000E+01;
         //            prxl0=7.41838E-03;
         //            prxl1=4.19935E-05;
         //            prxl2=-7.61621E-09;
         //            prxm0=5.45741E-06;
         //            prxm1=6.28865E-08;
         //            prxm2=-1.50496E-11;
         //            prxdv=1.62000E+01;
         //            r=prxr;
         //            cp=prxcp;
         //            return EOS_Error::good ;
         //          }
         //        cerr << "Sorry not implemented yet ..." << endl;
         //        assert(0);
         //        exit(-1);
         return EOS_Error::good ;
       }
    else if (sz == 4)
       { if (!(strings[0] == AString("Cp")))  goto usage;
         cp=atof(strings[1].aschar());
         if (!(strings[2] == AString("Cv")))  goto usage;
         FluidStr = AString("User") ;    
         cv = atof(strings[3].aschar()) ;
         gamma = cp/cv ;
         r     = cp-cv ;
         //cerr << "Cv = " << cv << endl;
         //cerr << "Cp = " << cp << endl;
         //cerr << "gamma = " << gamma << endl;
         //cerr << "r = " << r << endl;
         return EOS_Error::good ;
         usage : 
           cerr << "usage init(\"Cp\", \"0.021\",\"Cv\",\"0.02\")" << endl;;
         return EOS_Error::error ;
       }
    else
       { cerr << "Sorry not implemented yet ..." << endl;
         assert(0);
         exit(-1) ;
         return EOS_Error::error ;
       }
  }

  int EOS_PerfectGas::init(const Strings& strings, const Strings& /*values*/,
                           const double href0, const double sref0, const double tref0, const double pref0)
  {
    FluidStr=AString("unknown") ;   
    href = href0 ;
    sref = sref0 ;
    Tref = tref0 ;
    Pref = pref0 ;
    int sz=strings.size() ;

    if (sz == 1)
       { AString data_file_name ;
         if (iret_eos_data_dir) return EOS_Error::error ;
         data_file_name  = eos_data_dir.c_str() ;
         data_file_name += "/EOS_PerfectGas/" ;
         AString& file_name  = strings[0] ;
         data_file_name     += file_name ;
         std::ifstream data_file(data_file_name.aschar()) ;
         if (!data_file)
            { cerr << data_file_name << " not found!" << endl;
              assert(0) ;
              exit(-1)  ;
              return EOS_Error::error ;
            }
         // reading data_file
         else
            { //      cerr << "Reading file " << data_file_name << " ... " << std::flush;
              int ic =  0 ;
              int nc = 30 ;
              AString str("EmptyString") ;
              data_file >> str ;
              while(str != "" && ic < nc)
                { if (str=="Fluid")
                    { data_file >> FluidStr;
                      data_file >> str;
                      ic++ ;
                    }
                  else if (str=="xr")
                    { data_file >> prxr;
                      data_file >> str;
                      ic++ ;
                    }
                  else if (str=="xcp")
                    { data_file >> prxcp;
                      data_file >> str;
                      ic++ ;
                    }
                  else if (str=="xm")
                    { data_file >> prxm;
                      data_file >> str;
                      ic++ ;
                    }
                  else if (str=="xl0")
                    { data_file >> prxl0;
                      data_file >> str;
                      ic++ ;
                    }
                  else if (str=="xl1")
                    { data_file >> prxl1;
                      data_file >> str;
                      ic++ ;
                    }
                  else if (str=="xl2")
                    { data_file >> prxl2;
                      data_file >> str;
                      ic++ ;
                    }
                  else if (str=="xm0")
                    { data_file >> prxm0;
                      data_file >> str;
                      ic++ ;
                    }
                  else if (str=="xm1")
                    { data_file >> prxm1;
                      data_file >> str;
                      ic++ ;
                    }
                  else if (str=="xm2")
                    { data_file >> prxm2;
                      data_file >> str;
                      ic++ ;
                    }
                  else if (str=="xdv")
                    { data_file >> prxdv;
                      data_file >> str;
                      ic++ ;
                    }
                  else
                    { cerr << "Ref. " << str << " not recognized !" << endl;
                      assert(0);
                      exit(-1) ;
                      return EOS_Error::error ;
                    }
                }
              r  = prxr  ; 
              cp = prxcp ;
            }
         return EOS_Error::good ;
       }
    else
       { cerr << "Sorry not implemented yet ..." << endl;
         assert(0);
         exit(-1) ;
         return EOS_Error::error ;
       }
  }

  void EOS_PerfectGas::set_reference_state(double h, double s, double t, double p)
  {
    href = h ;
    sref = s ;
    Tref = t ;
    Pref = p ;
  }
}
