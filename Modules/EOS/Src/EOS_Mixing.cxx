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



#include "EOS_Mixing.hxx"
#include "Language/API/RegisterType.hxx"
#include "Language/API/Object_i.hxx"
#include "Language/API/Types_Info.hxx"
#ifdef WITH_PLUGIN_CATHARE2
#include "EOS/Src/EOS_Cathare2/EOS_Cathare2Vapor.hxx"
#include "EOS/Src/EOS_Cathare2/EOS_CathareIncondensableGas.hxx"
#endif

using NEPTUNE::RegisterType ;

static void totab(ArrOfDouble& C
                  , double C_0
                  , double C_1=0
                  , double C_2=0
                  , double C_3=0
                  , double C_4=0)
{ int n = C.size() ;
  C[0] = C_0 ;
  if(n > 1)  C[1] = C_1 ;
  if(n > 2)  C[2] = C_2 ;
  if(n > 3)  C[3] = C_3 ;
  if(n > 4)  C[4] = C_4 ;
}

namespace NEPTUNE_EOS
{
  const AString EOS_Mixing::tablename("EOS_Mixing") ;

  static RegisteredClass& EOS_Mixing_create()
  { return *(new EOS_Mixing()) ;
  }

  const AString& EOS_Mixing::table_name() const
  { return tablename;
  }

  ostream& EOS_Mixing::print_On (ostream& stream) const
  { return stream ;
  }

  istream& EOS_Mixing::read_On (istream& stream)
  { return stream ;
  }

  const Type_Info& EOS_Mixing::get_Type_Info () const
  { return (Types_Info::instance())[type_Id] ;
  }

  int EOS_Mixing::type_Id = (RegisterType("EOS_Mixing", "EOS_Fluid",
                                          "Equation Of States",
                                          sizeof(EOS_Mixing),
                                          EOS_Mixing_create)) ;

  EOS_Mixing::EOS_Mixing(int i) :
  nb_fluids(i), the_fluids(new EOS*[i]), alpha(i)
#ifdef WITH_PLUGIN_CATHARE
  ,CATHARE_water(this)  // provisional for EOS_Mixing
#endif
  { alpha[0] = 1.e0 ;
  }

  int EOS_Mixing::init(const Strings&)
  { return EOS_Error::good ;
  }

  int EOS_Mixing::set_components(EOS **components, int nb)
  { nb_fluids = nb ;
    if (the_fluids)  delete[] the_fluids ;
    the_fluids = new EOS*[nb];
    for(int i=0; i<nb; i++) {
       the_fluids[i] = components[i] ;
    }
    set_compute_mode();
    set_mixing_reference_state();
   return 1 ;
  }

  void EOS_Mixing::set_compute_mode() {
    if (    ((*the_fluids[0]).table_name()    == AString("Cathare")) 
         && ((*the_fluids[0]).equation_name() == AString("WaterVapor")) ) {
        compute_mode = MixingType::Cathare;
    } else {
        bool are_perfectgas = true;
        for (int i=1; i<nb_fluids; i++) {
            if ((*the_fluids[i]).table_name() != AString("PerfectGas")) {
                are_perfectgas = false;
                break;
            }
        }
        if (are_perfectgas) {
            compute_mode = MixingType::WithPerfectGas;
        } else if (    (*the_fluids[0]).table_name() == AString("Cathare2") 
                && (*the_fluids[0]).phase_name() == AString("Vapor") ) {
            compute_mode = MixingType::Cathare2;
        } else {
            compute_mode = MixingType::Unsupported;
        }
    }
  }

  void EOS_Mixing::set_mixing_reference_state() 
  { if ((compute_mode == MixingType::WithPerfectGas) && ((*the_fluids[0]).table_name() == AString("Cathare2")))
    {
        EOS_Internal_Error err;
        double href, Tref, pref;
        err = (*the_fluids[0]).fluid().get_h_ref(href);
        err = (*the_fluids[0]).fluid().get_T_ref(Tref);
        err = (*the_fluids[0]).fluid().get_p_ref(pref);
        
        for (int i=1; i<nb_fluids; i++)
        {
            (*the_fluids[i]).fluid().set_reference_state(href,0.e0,Tref,pref);
        }
    }
  }

  EOS_Mixing::~EOS_Mixing()
  { if (the_fluids)  delete[] the_fluids ;
  }

  EOS& EOS_Mixing::operator[](int i)
  { return *(the_fluids[i]) ;
  }

  const EOS& EOS_Mixing::operator[](int i) const
  { return *(the_fluids[i]) ;
  }

  void EOS_Mixing::set_component(int i, const EOS& fluid)
  { the_fluids[i]=&((EOS&)(fluid));
    if (compute_mode == MixingType::Unsupported) {
        set_compute_mode();
    } else if (i > 0 && compute_mode == MixingType::WithPerfectGas) {
        if (fluid.table_name() != AString("PerfectGas")) {
            compute_mode = MixingType::Unsupported;
        }
    }
  }

  int EOS_Mixing::set_alpha(const ArrOfDouble& arr)
  { if (arr.size() != nb_fluids)  return 0 ;
    alpha = arr ;
    return 1 ;
  }

  const ArrOfDouble& EOS_Mixing::get_alpha() const
  { return alpha ;
  }

  int EOS_Mixing::set_alpha(int i, double x)
  { alpha[i] = x ;
    return 1 ;
  }

  double EOS_Mixing::get_alpha(int i) const
  { return alpha[i] ;
  }

  EOS_Error EOS_Mixing::compute(const EOS_Fields& input, 
                                EOS_Field& r,
                                EOS_Error_Field& errfield) const
  { // warning : must be changed to avoid copy
    EOS_Fields output(1) ;
    output[0] = r ;
    return compute(input, output, errfield) ;
  }

  EOS_Error EOS_Mixing::compute(const EOS_Fields& input, 
                                EOS_Fields& r,
                                EOS_Error_Field& errfield) const
  {
    switch(compute_mode) {
        case MixingType::Cathare:
            return compute_cathare_mix(input, r, errfield);
        case MixingType::Cathare2:
            return compute_cathare2_mix(input, r, errfield);
        case MixingType::WithPerfectGas:
            return compute_perfect_gas(input, r, errfield);
        case MixingType::Unsupported:
            { errfield = EOS_Internal_Error::NOT_IMPLEMENTED ;
              return EOS_Error::error ;
            }
    }
    return EOS_Error::error;
  }
  EOS_Error EOS_Mixing::compute_cathare2_mix(const EOS_Fields& input,
                                EOS_Fields& r,
                                EOS_Error_Field& errfield) const
  { //EOS_Error cr ;
    //int nb_infields = input.size() ;
    // provisional for EOS_Mixing
#ifdef WITH_PLUGIN_CATHARE2
     ArrOfDouble tprxr  (nb_fluids - 1) ;
     ArrOfDouble tprxcp0(nb_fluids - 1) ;
     ArrOfDouble tprxcp1(nb_fluids - 1) ;
     ArrOfDouble tprxcp2(nb_fluids - 1) ;
     ArrOfDouble tprxcp3(nb_fluids - 1) ;
     ArrOfDouble tprxcp4(nb_fluids - 1) ;
     ArrOfDouble tprxcp5(nb_fluids - 1) ;
     ArrOfDouble tprxcp6(nb_fluids - 1) ;
     ArrOfDouble tprxm  (nb_fluids - 1) ;
     ArrOfDouble tprxl0 (nb_fluids - 1) ;
     ArrOfDouble tprxl1 (nb_fluids - 1) ;
     ArrOfDouble tprxl2 (nb_fluids - 1) ;
     ArrOfDouble tprxm0 (nb_fluids - 1) ;
     ArrOfDouble tprxm1 (nb_fluids - 1) ;
     ArrOfDouble tprxm2 (nb_fluids - 1) ;
     ArrOfDouble tprxdv (nb_fluids - 1) ;
     for (int i=1; i<nb_fluids; i++) 
        {  if ((*the_fluids[i]).table_name() != AString("CathareIncondensableGas")) 
              { errfield = EOS_Internal_Error::NOT_IMPLEMENTED ;
                return EOS_Error::error ;
              }
          EOS_CathareIncondensableGas* incond = (EOS_CathareIncondensableGas*) &(the_fluids[i]->fluid()) ;
          tprxr  (i-1) = incond->get_prxr  () ;
          tprxcp0(i-1) = incond->get_prxcp0() ;
          tprxcp1(i-1) = incond->get_prxcp1() ;
          tprxcp2(i-1) = incond->get_prxcp2() ;
          tprxcp3(i-1) = incond->get_prxcp3() ;
          tprxcp4(i-1) = incond->get_prxcp4() ;
          tprxcp5(i-1) = incond->get_prxcp5() ;
          tprxcp6(i-1) = incond->get_prxcp6() ;
          tprxm  (i-1) = incond->get_prxm  () ;
          tprxl0 (i-1) = incond->get_prxl0 () ;
          tprxl1 (i-1) = incond->get_prxl1 () ;
          tprxl2 (i-1) = incond->get_prxl2 () ;
          tprxm0 (i-1) = incond->get_prxm0 () ;
          tprxm1 (i-1) = incond->get_prxm1 () ;
          tprxm2 (i-1) = incond->get_prxm2 () ;
          tprxdv (i-1) = incond->get_prxdv () ;
        }
     EOS_Cathare2Vapor* cathare = (EOS_Cathare2Vapor*) &(the_fluids[0]->fluid()) ;
     cathare->set_mixing_properties(nb_fluids - 1, tprxr, tprxcp0, tprxcp1, tprxcp2, tprxcp3, tprxcp4,
                                    tprxcp5, tprxcp6, tprxm, tprxl0, tprxl1, tprxl2, tprxm0, tprxm1,
                                    tprxm2, tprxdv) ;

     EOS_Error e = cathare->calc2_mixing(nb_fluids - 1, input, r, errfield) ;
     cathare->set_mixing_properties() ;
     return e ;
#else
     (void) r;
     (void) input;
     (void) errfield;
     return EOS_Error::error ;
#endif
  }

  EOS_Error EOS_Mixing::compute_cathare_mix(const EOS_Fields& input,
                                EOS_Fields& r,
                                EOS_Error_Field& errfield) const
  {
    // provisional for EOS_Mixing
#ifdef WITH_PLUGIN_CATHARE
        EOS_Error cr ;
        int nb_infields = input.size() ;
        if ((nb_infields >= 4) && (nb_infields <= 7) ) 
        { int nincx = 4 ;
          ArrOfDouble tprxr(nincx)  ;
          ArrOfDouble tprxcp(nincx) ;
          ArrOfDouble tprxl0(nincx) ;
          ArrOfDouble tprxl1(nincx) ;
          ArrOfDouble tprxl2(nincx) ;
          ArrOfDouble tprxm0(nincx) ;
          ArrOfDouble tprxm1(nincx) ;
          ArrOfDouble tprxm2(nincx) ;
          for (int i=1; i<nb_fluids; i++) 
             { tprxr(i-1)  = (*the_fluids[i]).get_prxr()  ;
               tprxcp(i-1) = (*the_fluids[i]).get_prxcp() ;
               tprxl0(i-1) = (*the_fluids[i]).get_prxl0() ;
               tprxl1(i-1) = (*the_fluids[i]).get_prxl1() ;
               tprxl2(i-1) = (*the_fluids[i]).get_prxl2() ;
               tprxm0(i-1) = (*the_fluids[i]).get_prxm0() ;
               tprxm1(i-1) = (*the_fluids[i]).get_prxm1() ;
               tprxm2(i-1) = (*the_fluids[i]).get_prxm2() ;
             }
          cr = CATHARE_water.calca_all_mixing(input, r, errfield,
                             tprxr ,tprxcp, tprxl0, tprxl1,
                             tprxl2, tprxm0, tprxm1, tprxm2) ;
          return cr ;
        }
     else 
        { errfield = EOS_Internal_Error::NOT_IMPLEMENTED ;
          return EOS_Error::error ;
        }
#else
     (void) r;
     (void) input;
     (void) errfield;
     return EOS_Error::error ;
#endif
  }

  EOS_Error EOS_Mixing::compute_perfect_gas(const EOS_Fields& input,
                                EOS_Fields& r,
                                EOS_Error_Field& errfield) const
  { //EOS_Error cr ;
    int nb_infields  = input.size()  ;         // number of input  fields
    int nb_outfields = r.size() ;         // number of output fields
    int nsca  = errfield.size() ;                 // number of points
    errfield = EOS_Internal_Error::OK ; // initialization of error index
    
    // Test unicity of number of points by field : nsca
    for (int i_in=0; i_in<nb_infields; i_in++)
       { if (nsca != input[i_in].size())
          { errfield = EOS_Internal_Error::DATA_NUMBER ;
            return EOS_Error::error ;
          }
       }
    for (int i_out=0; i_out<nb_outfields; i_out++)
       { if (nsca != r[i_out].size())
          { errfield = EOS_Internal_Error::DATA_NUMBER ;
            return EOS_Error::error ;
          }
       }
    
    // Test existence of input main thermodynamic variables (P, T, h)
    int id_P  = 0 ;
    int id_T  = 0 ;
    int id_h  = 0 ;
    int id_c  = 0 ;
    int iP    = 0 ;
    int iT    = 0 ;
    int ih    = 0 ;
    int ic[5] ; for (int i=0; i<5; i++)  ic[i] = -1 ;
    int idim  = 0 ;

    for (int i_in=0; i_in<nb_infields; i_in++)
       {
          switch(input[i_in].get_property_number())
            {
              case NEPTUNE::p_sat:
                 idim++    ;
                 id_P++    ;
                 iP = i_in ;
                 break     ;
              case NEPTUNE::T_sat:
                 idim++    ;
                 id_T++    ;
                 iT = i_in ;
                 break     ;
              case NEPTUNE::p:
              case NEPTUNE::p_lim:
                 idim++    ;
                 id_P++    ;
                 iP = i_in ;
                 break     ;
              case NEPTUNE::T:
                 idim++    ;
                 id_T++    ;
                 iT = i_in ;
                 break     ;
              case NEPTUNE::h:
                 idim++    ;
                 id_h++    ;
                 ih = i_in ;
                 break     ;
              case NEPTUNE::c_0:
                 id_c++       ;
                 ic[0] = i_in ;
                 break        ;
              case NEPTUNE::c_1:
                 id_c++       ;
                 ic[1] = i_in ;
                 break        ;
              case NEPTUNE::c_2:
                 id_c++       ;
                 ic[2] = i_in ;
                 break        ;
              case NEPTUNE::c_3:
                 id_c++       ;
                 ic[3] = i_in ;
                 break        ;
              case NEPTUNE::c_4:
                 id_c++       ;
                 ic[4] = i_in ;
                 break        ;
              default:
                 break ;
            }
       }
       
    if (   (idim < 1) || (2 < idim)
        || (1 < id_P) || (1 < id_T) || (1 < id_h)
        || (id_c == 0) || (4 < id_c) || (id_c != nb_fluids)
        || (idim == 1 && id_P == 0 && id_T == 0)
        || (idim == 2 && 0 < id_T && 0 < id_h) )
       { errfield = EOS_Internal_Error::INPUT_PROPERTY ;
         return EOS_Error::error ;
       }
    if (idim == 1)
    {
      //cout << "EOS_Mixing fields : saturation curve" << endl;
      (*this)[0].fluid().compute(input[0],r,errfield);
      return errfield.find_worst_error().generic_error();
    }
    else if (idim == 2 && id_P == 1 && id_h == 1)
    {
      //cout << "EOS_Mixing fields : plan ph" << endl;
      //
      // algorithme complet c2
      //
      
      // calcul Pv, hv = f(P, h)
      ArrOfDouble xc0(nsca,0.e0);
      ArrOfDouble xc1(nsca,0.e0);
      ArrOfDouble xc2(nsca,0.e0);
      ArrOfDouble xc3(nsca,0.e0);
      ArrOfDouble xc4(nsca,0.e0);
      EOS_Field C0("c_0","c_0",xc0);
      EOS_Field C1("c_1","c_1",xc1);
      EOS_Field C2("c_2","c_2",xc2);
      EOS_Field C3("c_3","c_3",xc3);
      EOS_Field C4("c_4","c_4",xc4);
      
      if (ic[0] != -1) C0 = input[ic[0]];
      if (ic[1] != -1) C1 = input[ic[1]];
      if (ic[2] != -1) C2 = input[ic[2]];
      if (ic[3] != -1) C3 = input[ic[3]];
      if (ic[4] != -1) C4 = input[ic[4]];
      
      ArrOfDouble xPv(nsca);
      EOS_Field Pv("Pv","p",xPv);
      ArrOfDouble xhv(nsca);
      EOS_Field hv("hv","h",xhv);
      
      for (int i=0; i<nsca; i++)
      {
        this->compute_pv_hv_ph(input[iP][i], input[ih][i], Pv[i], hv[i], C0[i], C1[i], C2[i], C3[i], C4[i]); 
      }
      //
      // Calcul de toutes les proprietes standards, apres convergence calcul Pv, Hv
      //
      EOS_Fields fsin_vap (2);
      fsin_vap[0] = Pv;
      fsin_vap[1] = hv;

      // calcul saturation Tsat, Hsat = f(P)
      ArrOfDouble xtsatp(nsca), xhlsatp(nsca), xhvsatp(nsca), xrholsatp(nsca);
      ArrOfDouble xrhovsatp(nsca), xcplsatp(nsca), xcpvsatp(nsca);
      ArrOfDouble xdtsatdp(nsca), xdhlsatdp(nsca), xdhvsatdp(nsca), xdrholsatdp(nsca);
      ArrOfDouble xdrhovsatdp(nsca), xdcplsatdp(nsca), xdcpvsatdp(nsca), xd2tsatdp(nsca);
      EOS_Fields fsout_satp (15);
      EOS_Field tsatp ("out_satp1", "T_sat", xtsatp);
      EOS_Field hlsatp ("out_satp2", "h_l_sat", xhlsatp);
      EOS_Field hvsatp ("out_satp3", "h_v_sat", xhvsatp);
      EOS_Field rholsatp ("out_satp4", "rho_l_sat", xrholsatp);
      EOS_Field rhovsatp ("out_satp5", "rho_v_sat", xrhovsatp);
      EOS_Field cplsatp ("out_satp6", "cp_l_sat", xcplsatp);
      EOS_Field cpvsatp ("out_satp7", "cp_v_sat", xcpvsatp);
      EOS_Field dtsatdp ("out_satp8", "d_T_sat_d_p", xdtsatdp);
      EOS_Field dhlsatdp ("out_satp9", "d_h_l_sat_d_p", xdhlsatdp);
      EOS_Field dhvsatdp ("out_satp10", "d_h_v_sat_d_p", xdhvsatdp);
      EOS_Field drholsatdp ("out_satp11", "d_rho_l_sat_d_p", xdrholsatdp);
      EOS_Field drhovsatdp ("out_satp12", "d_rho_v_sat_d_p", xdrhovsatdp);
      EOS_Field dcplsatdp ("out_satp13", "d_cp_l_sat_d_p", xdcplsatdp);
      EOS_Field dcpvsatdp ("out_satp14", "d_cp_v_sat_d_p", xdcpvsatdp);
      EOS_Field d2tsatdp ("out_satp15", "d2_T_sat_d_p_d_p", xd2tsatdp);
      fsout_satp[0] = tsatp;
      fsout_satp[1] = hlsatp;
      fsout_satp[2] = hvsatp;
      fsout_satp[3] = rholsatp;
      fsout_satp[4] = rhovsatp;
      fsout_satp[5] = cplsatp;
      fsout_satp[6] = cpvsatp;
      fsout_satp[7] = dtsatdp;
      fsout_satp[8] = dhlsatdp;
      fsout_satp[9] = dhvsatdp;
      fsout_satp[10] = drholsatdp;
      fsout_satp[11] = drhovsatdp;
      fsout_satp[12] = dcplsatdp;
      fsout_satp[13] = dcpvsatdp;
      fsout_satp[14] = d2tsatdp;
  
      (*this)[0].fluid().compute(input[0],fsout_satp,errfield);

      // calcul saturation Tsatpv, Hsatpv = f(Pv)
      ArrOfDouble xtsatpv(nsca), xhlsatpv(nsca), xhvsatpv(nsca), xrholsatpv(nsca);
      ArrOfDouble xrhovsatpv(nsca), xcplsatpv(nsca), xcpvsatpv(nsca);
      ArrOfDouble xdtsatdpv(nsca), xdhlsatdpv(nsca), xdhvsatdpv(nsca), xdrholsatdpv(nsca);
      ArrOfDouble xdrhovsatdpv(nsca), xdcplsatdpv(nsca), xdcpvsatdpv(nsca), xd2tsatdpv(nsca);
      EOS_Fields fsout_satpv (15);
      EOS_Field tsatpv ("out_satpv1", "T_sat", xtsatpv);
      EOS_Field hlsatpv ("out_satpv2", "h_l_sat", xhlsatpv);
      EOS_Field hvsatpv ("out_satpv3", "h_v_sat", xhvsatpv);
      EOS_Field rholsatpv ("out_satpv4", "rho_l_sat", xrholsatpv);
      EOS_Field rhovsatpv ("out_satpv5", "rho_v_sat", xrhovsatpv);
      EOS_Field cplsatpv ("out_satpv6", "cp_l_sat", xcplsatpv);
      EOS_Field cpvsatpv ("out_satpv7", "cp_v_sat", xcpvsatpv);
      EOS_Field dtsatdpv ("out_satpv8", "d_T_sat_d_p", xdtsatdpv);
      EOS_Field dhlsatdpv ("out_satpv9", "d_h_l_sat_d_p", xdhlsatdpv);
      EOS_Field dhvsatdpv ("out_satpv10", "d_h_v_sat_d_p", xdhvsatdpv);
      EOS_Field drholsatdpv ("out_satpv11", "d_rho_l_sat_d_p", xdrholsatdpv);
      EOS_Field drhovsatdpv ("out_satpv12", "d_rho_v_sat_d_p", xdrhovsatdpv);
      EOS_Field dcplsatdpv ("out_satpv13", "d_cp_l_sat_d_p", xdcplsatdpv);
      EOS_Field dcpvsatdpv ("out_satpv14", "d_cp_v_sat_d_p", xdcpvsatdpv);
      EOS_Field d2tsatdpv ("out_satpv15", "d2_T_sat_d_p_d_p", xd2tsatdpv);
      fsout_satpv[0] = tsatpv;
      fsout_satpv[1] = hlsatpv;
      fsout_satpv[2] = hvsatpv;
      fsout_satpv[3] = rholsatpv;
      fsout_satpv[4] = rhovsatpv;
      fsout_satpv[5] = cplsatpv;
      fsout_satpv[6] = cpvsatpv;
      fsout_satpv[7] = dtsatdpv;
      fsout_satpv[8] = dhlsatdpv;
      fsout_satpv[9] = dhvsatdpv;
      fsout_satpv[10] = drholsatdpv;
      fsout_satpv[11] = drhovsatdpv;
      fsout_satpv[12] = dcplsatdpv;
      fsout_satpv[13] = dcpvsatdpv;
      fsout_satpv[14] = d2tsatdpv;
      
      (*this)[0].fluid().compute(fsin_vap[0],fsout_satpv,errfield);

      // calcul des proprietes vapeur
      ArrOfDouble xtg(nsca),xdtgpv(nsca),xdtghv(nsca),xrv(nsca),xdrvpv(nsca),xdrvhv(nsca);
      ArrOfDouble xcpv(nsca), xdcpvpv(nsca), xdcpvhv(nsca);
      ArrOfDouble xlambdav(nsca), xdlambdavpv(nsca), xdlambdavhv(nsca);
      ArrOfDouble xmuv(nsca), xdmuvpv(nsca), xdmuvhv(nsca), xsigma(nsca), xdsigmapv(nsca);
      EOS_Fields fsout_vap (17);
      EOS_Field tg ("out_vap1", "T", xtg);
      EOS_Field dtgpv ("out_vap2", "d_T_d_p_h", xdtgpv);
      EOS_Field dtghv ("out_vap3", "d_T_d_h_p", xdtghv);
      EOS_Field rv ("out_vap4", "rho", xrv);
      EOS_Field drvpv ("out_vap5", "d_rho_d_p_h", xdrvpv);
      EOS_Field drvhv ("out_vap6", "d_rho_d_h_p", xdrvhv);
      EOS_Field cpv ("out_vap7", "cp", xcpv);
      EOS_Field dcpvpv ("out_vap8", "d_cp_d_p_h", xdcpvpv);
      EOS_Field dcpvhv ("out_vap9", "d_cp_d_h_p", xdcpvhv);
      EOS_Field lambdav ("out_vap10", "lambda", xlambdav);
      EOS_Field dlambdavpv ("out_vap11", "d_lambda_d_p_h", xdlambdavpv);
      EOS_Field dlambdavhv ("out_vap12", "d_lambda_d_h_p", xdlambdavhv);
      EOS_Field muv ("out_vap13", "mu", xmuv);
      EOS_Field dmuvpv ("out_vap14", "d_mu_d_p_h", xdmuvpv);
      EOS_Field dmuvhv ("out_vap15", "d_mu_d_h_p", xdmuvhv);
      EOS_Field sigma ("out_vap16", "sigma", xsigma);
      EOS_Field dsigmapv ("out_vap17", "d_sigma_d_p_h", xdsigmapv);
      fsout_vap[0] = tg;
      fsout_vap[1] = dtgpv;
      fsout_vap[2] = dtghv;
      fsout_vap[3] = rv;
      fsout_vap[4] = drvpv;
      fsout_vap[5] = drvhv;
      fsout_vap[6] = cpv;
      fsout_vap[7] = dcpvpv;
      fsout_vap[8] = dcpvhv;
      fsout_vap[9] = lambdav;
      fsout_vap[10] = dlambdavpv;
      fsout_vap[11] = dlambdavhv;
      fsout_vap[12] = muv;
      fsout_vap[13] = dmuvpv;
      fsout_vap[14] = dmuvhv;
      fsout_vap[15] = sigma;
      fsout_vap[16] = dsigmapv;
      // calcul proprietes vapeur
      (*this)[0].fluid().compute(fsin_vap,fsout_vap,errfield);
      //
      // Calcul des proprietes de melange
      //
      ArrOfDouble xdpvdp(nsca), xdpvdh(nsca), xdhvdp(nsca), xdhvdh(nsca);
      ArrOfDouble xdtgdp(nsca), xdtgdh(nsca), xrg(nsca), xdrgdp(nsca), xdrgdh(nsca), xdrvdp(nsca), xdrvdh(nsca);
      ArrOfDouble xcpg(nsca), xdcpdp(nsca), xdcpdh(nsca);
      ArrOfDouble xlambdag(nsca), xdlambdagdp(nsca), xdlambdagdh(nsca);
      ArrOfDouble xmug(nsca), xdmudp(nsca), xdmudh(nsca), xdsigmadp(nsca), xdsigmadh(nsca);
      ArrOfDouble xprgr(nsca), xxnc(nsca), xmnc(nsca), xrnc(nsca), xdncv(nsca), xddncv1(nsca), xddncv3(nsca);
      // proprietes incondensables
      ArrOfDouble px[4] = {ArrOfDouble(nsca,0.e0),ArrOfDouble(nsca,0.e0),ArrOfDouble(nsca,0.e0),ArrOfDouble(nsca,0.e0)};
      ArrOfDouble hx[4] = {ArrOfDouble(nsca,0.e0),ArrOfDouble(nsca,0.e0),ArrOfDouble(nsca,0.e0),ArrOfDouble(nsca,0.e0)};
      ArrOfDouble dpvdx[4] = {ArrOfDouble(nsca,0.e0),ArrOfDouble(nsca,0.e0),ArrOfDouble(nsca,0.e0),ArrOfDouble(nsca,0.e0)};
      ArrOfDouble dhvdx[4] = {ArrOfDouble(nsca,0.e0),ArrOfDouble(nsca,0.e0),ArrOfDouble(nsca,0.e0),ArrOfDouble(nsca,0.e0)};
      ArrOfDouble dtgdx[4] = {ArrOfDouble(nsca,0.e0),ArrOfDouble(nsca,0.e0),ArrOfDouble(nsca,0.e0),ArrOfDouble(nsca,0.e0)};
      ArrOfDouble drgdx[4] = {ArrOfDouble(nsca,0.e0),ArrOfDouble(nsca,0.e0),ArrOfDouble(nsca,0.e0),ArrOfDouble(nsca,0.e0)};
      ArrOfDouble drvdx[4] = {ArrOfDouble(nsca,0.e0),ArrOfDouble(nsca,0.e0),ArrOfDouble(nsca,0.e0),ArrOfDouble(nsca,0.e0)};
      ArrOfDouble dcpdx[4] = {ArrOfDouble(nsca,0.e0),ArrOfDouble(nsca,0.e0),ArrOfDouble(nsca,0.e0),ArrOfDouble(nsca,0.e0)};
      ArrOfDouble dlambdadx[4] = {ArrOfDouble(nsca,0.e0),ArrOfDouble(nsca,0.e0),ArrOfDouble(nsca,0.e0),ArrOfDouble(nsca,0.e0)};
      ArrOfDouble dmudx[4] = {ArrOfDouble(nsca,0.e0),ArrOfDouble(nsca,0.e0),ArrOfDouble(nsca,0.e0),ArrOfDouble(nsca,0.e0)};
      ArrOfDouble dsigmadx[4] = {ArrOfDouble(nsca,0.e0),ArrOfDouble(nsca,0.e0),ArrOfDouble(nsca,0.e0),ArrOfDouble(nsca,0.e0)};
      ArrOfDouble ddncvdx[4] = {ArrOfDouble(nsca,0.e0),ArrOfDouble(nsca,0.e0),ArrOfDouble(nsca,0.e0),ArrOfDouble(nsca,0.e0)};
      ArrOfDouble drncdx[4] = {ArrOfDouble(nsca,0.e0),ArrOfDouble(nsca,0.e0),ArrOfDouble(nsca,0.e0),ArrOfDouble(nsca,0.e0)};
      ArrOfDouble dmncdx[4] = {ArrOfDouble(nsca,0.e0),ArrOfDouble(nsca,0.e0),ArrOfDouble(nsca,0.e0),ArrOfDouble(nsca,0.e0)};
      EOS_Field dpv1("dpvdp", "d_p_0_d_p_h", xdpvdp);
      EOS_Field dpv3("dpvdh", "d_p_0_d_h_p", xdpvdh);
      EOS_Field dhv1("dhvdp", "d_h_0_d_p_h", xdhvdp);
      EOS_Field dhv3("dhvdh", "d_h_0_d_h_p", xdhvdh);
      EOS_Field dtg1("dTdp", "d_T_d_p_h", xdtgdp);
      EOS_Field dtg3("dTdh", "d_T_d_h_p", xdtgdh);
      EOS_Field rg("rho", "rho", xrg);
      EOS_Field drg1("drhodp", "d_rho_d_p_h", xdrgdp);
      EOS_Field drg3("drhodh", "d_rho_d_h_p", xdrgdh);
      EOS_Field drv1("drhovdp", "d_rho_0_d_p_h", xdrvdp);
      EOS_Field drv3("drhovdh", "d_rho_0_d_h_p", xdrvdh);
      EOS_Field cpg("cpg", "cp", xcpg);
      EOS_Field dcpg1("dcpdp", "d_cp_d_p_h", xdcpdp);
      EOS_Field dcpg3("dcpdh", "d_cp_d_h_p", xdcpdh);
      EOS_Field lambdag("lambdag", "lambda", xlambdag);
      EOS_Field dlambdag1("dlambdadp", "d_lambda_d_p_h", xdlambdagdp);
      EOS_Field dlambdag3("dlambdadh", "d_lambda_d_h_p", xdlambdagdh);
      EOS_Field mug("mug", "mu", xmug);
      EOS_Field dmug1("dmudp", "d_mu_d_p_h", xdmudp);
      EOS_Field dmug3("dmudh", "d_mu_d_h_p", xdmudh);
      EOS_Field prgr("prgr", "prgr", xprgr);
      EOS_Field xnc("xnc", "xnc", xxnc);
      EOS_Field mnc("mnc", "mnc", xmnc);
      EOS_Field rnc("rnc", "rnc", xrnc);
      EOS_Field dncv("dncv", "dncv", xdncv);
      EOS_Field ddncv1("ddncv1", "d_dncv_d_p_h", xddncv1);
      EOS_Field ddncv3("ddncv3", "d_dncv_d_h_p", xddncv3);
      EOS_Field dsigma1("dsigma1", "d_sigma_d_p_h", xdsigmadp);
      EOS_Field dsigma3("dsigma3", "d_sigma_d_h_p", xdsigmadh);
      EOS_Field p1("p1", "p_1", px[0]);
      EOS_Field p2("p2", "p_2", px[1]);
      EOS_Field p3("p3", "p_3", px[2]);
      EOS_Field p4("p4", "p_4", px[3]);
      EOS_Field h1("h1", "h_1", hx[0]);
      EOS_Field h2("h2", "h_2", hx[1]);
      EOS_Field h3("h3", "h_3", hx[2]);
      EOS_Field h4("h4", "h_4", hx[3]);
      EOS_Field dpvdx1("dpvdx1", "d_p_0_d_c_1_ph", dpvdx[0]);
      EOS_Field dpvdx2("dpvdx2", "d_p_0_d_c_2_ph", dpvdx[1]);
      EOS_Field dpvdx3("dpvdx3", "d_p_0_d_c_3_ph", dpvdx[2]);
      EOS_Field dpvdx4("dpvdx4", "d_p_0_d_c_4_ph", dpvdx[3]);
      EOS_Field dhvdx1("dhvdx1", "d_h_0_d_c_1_ph", dhvdx[0]);
      EOS_Field dhvdx2("dhvdx2", "d_h_0_d_c_2_ph", dhvdx[1]);
      EOS_Field dhvdx3("dhvdx3", "d_h_0_d_c_3_ph", dhvdx[2]);
      EOS_Field dhvdx4("dhvdx4", "d_h_0_d_c_4_ph", dhvdx[3]);
      EOS_Field dtgdx1("dtgdx1", "d_T_d_c_1_ph", dtgdx[0]);
      EOS_Field dtgdx2("dtgdx2", "d_T_d_c_2_ph", dtgdx[1]);
      EOS_Field dtgdx3("dtgdx3", "d_T_d_c_3_ph", dtgdx[2]);
      EOS_Field dtgdx4("dtgdx4", "d_T_d_c_4_ph", dtgdx[3]);
      EOS_Field drgdx1("drgdx1", "d_rho_d_c_1_ph", drgdx[0]);
      EOS_Field drgdx2("drgdx2", "d_rho_d_c_2_ph", drgdx[1]);
      EOS_Field drgdx3("drgdx3", "d_rho_d_c_3_ph", drgdx[2]);
      EOS_Field drgdx4("drgdx4", "d_rho_d_c_4_ph", drgdx[3]);
      EOS_Field drvdx1("drvdx1", "d_rho_0_d_c_1_ph", drvdx[0]);
      EOS_Field drvdx2("drvdx2", "d_rho_0_d_c_2_ph", drvdx[1]);
      EOS_Field drvdx3("drvdx3", "d_rho_0_d_c_3_ph", drvdx[2]);
      EOS_Field drvdx4("drvdx4", "d_rho_0_d_c_4_ph", drvdx[3]);
      EOS_Field dcpdx1("dcpdx1", "d_cp_d_c_1_ph", dcpdx[0]);
      EOS_Field dcpdx2("dcpdx2", "d_cp_d_c_2_ph", dcpdx[1]);
      EOS_Field dcpdx3("dcpdx3", "d_cp_d_c_3_ph", dcpdx[2]);
      EOS_Field dcpdx4("dcpdx4", "d_cp_d_c_4_ph", dcpdx[3]);
      EOS_Field dlambdadx1("dlambdadx1", "d_lambda_d_c_1_ph", dlambdadx[0]);
      EOS_Field dlambdadx2("dlambdadx2", "d_lambda_d_c_2_ph", dlambdadx[1]);
      EOS_Field dlambdadx3("dlambdadx3", "d_lambda_d_c_3_ph", dlambdadx[2]);
      EOS_Field dlambdadx4("dlambdadx4", "d_lambda_d_c_4_ph", dlambdadx[3]);
      EOS_Field dmudx1("dmudx1", "d_mu_d_c_1_ph", dmudx[0]);
      EOS_Field dmudx2("dmudx2", "d_mu_d_c_2_ph", dmudx[1]);
      EOS_Field dmudx3("dmudx3", "d_mu_d_c_3_ph", dmudx[2]);
      EOS_Field dmudx4("dmudx4", "d_mu_d_c_4_ph", dmudx[3]);
      EOS_Field dsigmadx1("dsigmadx1", "d_sigma_d_c_1_ph", dsigmadx[0]);
      EOS_Field dsigmadx2("dsigmadx2", "d_sigma_d_c_2_ph", dsigmadx[1]);
      EOS_Field dsigmadx3("dsigmadx3", "d_sigma_d_c_3_ph", dsigmadx[2]);
      EOS_Field dsigmadx4("dsigmadx4", "d_sigma_d_c_4_ph", dsigmadx[3]);
      EOS_Field ddncvdx1("ddncvdx1", "d_dncv_d_c_1_ph", ddncvdx[0]);
      EOS_Field ddncvdx2("ddncvdx2", "d_dncv_d_c_2_ph", ddncvdx[1]);
      EOS_Field ddncvdx3("ddncvdx3", "d_dncv_d_c_3_ph", ddncvdx[2]);
      EOS_Field ddncvdx4("ddncvdx4", "d_dncv_d_c_4_ph", ddncvdx[3]);
      EOS_Field drncdx1("drncdx1", "d_rnc_d_c_1_ph", drncdx[0]);
      EOS_Field drncdx2("drncdx2", "d_rnc_d_c_2_ph", drncdx[1]);
      EOS_Field drncdx3("drncdx3", "d_rnc_d_c_3_ph", drncdx[2]);
      EOS_Field drncdx4("drncdx4", "d_rnc_d_c_4_ph", drncdx[3]);
      EOS_Field dmncdx1("dmncdx1", "d_mnc_d_c_1_ph", dmncdx[0]);
      EOS_Field dmncdx2("dmncdx2", "d_mnc_d_c_2_ph", dmncdx[1]);
      EOS_Field dmncdx3("dmncdx3", "d_mnc_d_c_3_ph", dmncdx[2]);
      EOS_Field dmncdx4("dmncdx4", "d_mnc_d_c_4_ph", dmncdx[3]);
      // variables intermediaires
      double dfdpv, dfdhv, dgdpv, dgdhv, dcpgtg, cpj, lambdaj, dlambdaj, muj, dmuj;
      double mm0;
      EOS_Internal_Error err ;
      err = (*this)[0].fluid().get_mm(mm0);
      double fldr = rPerfectGas/mm0;
      double flddv = 13.1e0; //TODO : attention ! valable pour l'eau, quid des autres fluides ??
      for (int i=0; i<nsca; i++)
      {
        ArrOfDouble dncvj(nb_fluids,0.e0) ;
        ArrOfDouble c(nb_fluids) ;
        totab(c, C0[i] ,C1[i], C2[i], C3[i], C4[i]) ;
        //
        double mmj;
        double cpsum=0.e0;
        double xrsum=0.e0;
        double dlambdagtg=0.e0;
        //double dlambdagpv=0.e0;
        double dmugtg=0.e0;
        //double dmugpv=0.e0;
        xnc[i]=0.e0;
        mnc[i]=0.e0;
        dncv[i]=0.e0;
        for(int j=1; j<nb_fluids; j++)
        { 
          xnc[i] += c[j] + 1.e-99;
          err = (*this)[j].fluid().compute_cp_pT(input[iP][i],tg[i],cpj);
          err = (*this)[j].fluid().get_mm(mmj);
          mnc[i] += (c[j] + 1.e-99)/(mmj*1.e3);
          cpsum += c[j]*cpj;
          xrsum += c[j]*(*this)[j].fluid().get_prxr();
        }
        // prgr
        prgr[i] = c[0]*fldr + xrsum;
        // mnc & rnc
        mnc[i] = xnc[i]/mnc[i];
        rnc[i] = rPerfectGas*1.e3/mnc[i];
        //
        double croipv = tg[i]*drvpv[i] + rv[i]*dtgpv[i];
        double croihv = tg[i]*drvhv[i] + rv[i]*dtghv[i];
        dfdpv = cpsum*dtgpv[i];
        dfdhv = cpsum*dtghv[i] + c[0];
        dgdpv = xrsum*croipv + c[0];
        dgdhv = xrsum*croihv;
        double usden = 1.e0/(dfdpv*dgdhv - dfdhv*dgdpv);
        dfdpv = dfdpv*usden;
        dfdhv = dfdhv*usden;
        dgdpv = dgdpv*usden;
        dgdhv = dgdhv*usden;
        // derivatives of PV and HV w.r.t P and HG
        dpv1[i] = -c[0]*dfdhv;
        dpv3[i] = dgdhv;
        dhv1[i] = c[0]*dfdpv;
        dhv3[i] = -dgdpv;
        // derivatives of TG w.r.t P and HG
        dtg1[i] = dtgpv[i]*dpv1[i] + dtghv[i]*dhv1[i];
        dtg3[i] = dtgpv[i]*dpv3[i] + dtghv[i]*dhv3[i];
        // rho_gaz and derivatives w.r.t P and HG
        drv1[i] = drvpv[i]*dpv1[i] + drvhv[i]*dhv1[i];
        drv3[i] = drvpv[i]*dpv3[i] + drvhv[i]*dhv3[i];
        rg[i] = (input[iP][i] - Pv[i] + fldr*rv[i]*tg[i])/(prgr[i]*tg[i]);
        drg1[i] = (1.e0 - dpv1[i] + fldr*(drv1[i]*tg[i] + rv[i]*dtg1[i]) - rg[i]*prgr[i]*dtg1[i])/(prgr[i]*tg[i]);
        drg3[i] = (-dpv3[i] + fldr*(drv3[i]*tg[i] + rv[i]*dtg3[i]) - rg[i]*prgr[i]*dtg3[i])/(prgr[i]*tg[i]);
        // cpg
        cpg[i] = cpv[i]*c[0];
        dcpgtg = 0.e0;
        //
        lambdag[i] = lambdav[i]*c[0]*fldr;
        //
        mug[i] = muv[i]*c[0]*fldr;
        //
        for(int j=1; j<nb_fluids; j++)
        {
          err = (*this)[j].fluid().compute_h_pT(input[iP][i],tg[i],hx[j-1][i]);
          px[j-1][i]= (*this)[j].fluid().get_prxr()*tg[i]*rv[i];
          dpvdx[j-1][i] = (input[iP][i] - Pv[i] + px[j-1][i])*dfdhv + (hv[i] - hx[j-1][i])*dgdhv;
          dhvdx[j-1][i] = -(input[iP][i] - Pv[i] + px[j-1][i])*dfdpv - (hv[i] - hx[j-1][i])*dgdpv;
          // derivatives of TG w.r.t. Xj
          dtgdx[j-1][i] = dtgpv[i]*dpvdx[j-1][i] + dtghv[i]*dhvdx[j-1][i];
          // derivatives of rho_vap, rho_gaz w.r.t. Xj
          drvdx[j-1][i] = drvpv[i]*dpvdx[j-1][i] + drvhv[i]*dhvdx[j-1][i];
          drgdx[j-1][i] = (-dpvdx[j-1][i] + fldr*(drvdx[j-1][i]*tg[i] + rv[i]*dtgdx[j-1][i]) - rg[i]*(dtgdx[j-1][i]*prgr[i] + tg[i]*((*this)[j].fluid().get_prxr() - fldr)))/(prgr[i]*tg[i]);
          //
          dmncdx[j-1][i] = mnc[i]/xnc[i]*(1.e0 - mnc[i]/(*this)[j].fluid().get_prxm());
          drncdx[j-1][i] = -rPerfectGas*1.e3/(mnc[i]*mnc[i])*dmncdx[j-1][i];
          //
          px[j-1][i]= (*this)[j].fluid().get_prxr()*tg[i]*rg[i]*c[j];
          //
          err = (*this)[j].fluid().compute_cp_pT(input[iP][i],tg[i],cpj);
          cpg[i] = cpg[i] + c[j]*cpj;
          //
          err = (*this)[j].fluid().compute_lambda_pT(input[iP][i],tg[i],lambdaj);
          err = (*this)[j].fluid().compute_d_lambda_d_T_p_pT(input[iP][i],tg[i],dlambdaj);
          lambdag[i] = lambdag[i] + lambdaj*c[j]*(*this)[j].fluid().get_prxr();
          dlambdagtg = dlambdagtg + dlambdaj*c[j]*(*this)[j].fluid().get_prxr();
          //
          err = (*this)[j].fluid().compute_mu_pT(input[iP][i],tg[i],muj);
          err = (*this)[j].fluid().compute_d_mu_d_T_p_pT(input[iP][i],tg[i],dmuj);
          mug[i] = mug[i] + muj*c[j]*(*this)[j].fluid().get_prxr();
          dmugtg = dmugtg + dmuj*c[j]*(*this)[j].fluid().get_prxr();
          //
          err = (*this)[j].fluid().get_mm(mmj);
          err = (*this)[0].fluid().get_mm(mm0);
          double meq = 2.e0/(1.e0/(mmj*1.e3) + 1.e0/(mm0*1.e3));
          double usden2 = 0.0143e0/(sqrt(meq)*pow((pow((*this)[j].fluid().get_prxdv(),(1.e0/3.e0)) + pow(flddv,(1.e0/3.e0))),2)); // valable pour tout fluide ?
          dncvj[j] = usden2*pow(tg[i],1.75e0)/input[iP][i];
          dncv[i] = dncv[i] + (c[j] + 1.e-99)/dncvj[j];
        }
        //
        dcpg1[i] = (dcpvpv[i]*dpv1[i] + dcpvhv[i]*dhv1[i])*c[0];
        dcpg3[i] = (dcpvpv[i]*dpv3[i] + dcpvhv[i]*dhv3[i])*c[0];
        //
        lambdag[i] = lambdag[i]/prgr[i]; // Attention, notion de valeur minimale non introduite (voir Cathare)
        dlambdag1[i] = ((dlambdavpv[i]*dpv1[i] + dlambdavhv[i]*dhv1[i])*c[0]*fldr + dlambdagtg*dtg1[i])/prgr[i];
        dlambdag3[i] = ((dlambdavpv[i]*dpv3[i] + dlambdavhv[i]*dhv3[i])*c[0]*fldr + dlambdagtg*dtg3[i])/prgr[i];
        //
        mug[i] = mug[i]/prgr[i]; // Attention, notion de valeur minimale non introduite (voir Cathare)
        dmug1[i] = ((dmuvpv[i]*dpv1[i] + dmuvhv[i]*dhv1[i])*c[0]*fldr + dmugtg*dtg1[i])/prgr[i];
        dmug3[i] = ((dmuvpv[i]*dpv3[i] + dmuvhv[i]*dhv3[i])*c[0]*fldr + dmugtg*dtg3[i])/prgr[i];
        //
        dsigma1[i] = dsigmapv[i]*dpv1[i];
        dsigma3[i] = dsigmapv[i]*dpv3[i];
        //
        dncv[i] = xnc[i]/dncv[i];
        ddncv1[i] = dncv[i]*(1.75e0*dtg1[i]/tg[i] - 1.e0/input[iP][i]);
        ddncv3[i] = dncv[i]*1.75e0*dtg3[i]/tg[i];
        for(int j=1; j<nb_fluids; j++)
        {
          err = (*this)[j].fluid().compute_cp_pT(input[iP][i],tg[i],cpj);
          err = (*this)[j].fluid().compute_lambda_pT(input[iP][i],tg[i],lambdaj);
          err = (*this)[j].fluid().compute_mu_pT(input[iP][i],tg[i],muj);
          dcpdx[j-1][i] = (dcpvpv[i]*dpvdx[j-1][i] + dcpvhv[i]*dhvdx[j-1][i])*c[0] + cpj - cpv[i] + dcpgtg*dtgdx[j-1][i];
          //
          dlambdadx[j-1][i] = ((dlambdavpv[i]*dpvdx[j-1][i] + dlambdavhv[i]*dhvdx[j-1][i])*c[0]*fldr + dlambdagtg*dtgdx[j-1][i] + lambdaj*(*this)[j].fluid().get_prxr() - lambdav[i]*fldr + (fldr - (*this)[j].fluid().get_prxr())*lambdag[i])/prgr[i];
          //
          dmudx[j-1][i] = ((dmuvpv[i]*dpvdx[j-1][i] + dmuvpv[i]*dpvdx[j-1][i])*c[0]*fldr + dmugtg*dtgdx[j-1][i] + muj*(*this)[j].fluid().get_prxr() - muv[i]*fldr + (fldr - (*this)[j].fluid().get_prxr())*mug[i])/prgr[i];
          //
          dsigmadx[j-1][i] = dsigmapv[i]*dpvdx[j-1][i];
          //
          ddncvdx[j-1][i] = dncv[i]/xnc[i]*(1.e0 -dncv[i]/dncvj[j]) + dncv[i]*1.75e0*dtgdx[j-1][i]/tg[i];
        }

        for(int k=0; k<nb_outfields; k++)
        {
          switch (r[k].get_property_number())
          {
             case NEPTUNE::T :
               r[k][i]=tg[i];
               break;
             case NEPTUNE::d_T_d_p_h :
               r[k][i]=dtg1[i];
               break;
             case NEPTUNE::d_T_d_h_p :
               r[k][i]=dtg3[i];
               break;
             case NEPTUNE::rho :
               r[k][i]=rg[i];
               break;
             case NEPTUNE::d_rho_d_p_h :
               r[k][i]=drg1[i];
               break;
             case NEPTUNE::d_rho_d_h_p :
               r[k][i]=drg3[i];
               break;
             case NEPTUNE::cp :
               r[k][i]=cpg[i];
               break;
             case NEPTUNE::d_cp_d_p_h :
               r[k][i]=dcpg1[i];
               break;
             case NEPTUNE::d_cp_d_h_p :
               r[k][i]=dcpg3[i];
               break;
             case NEPTUNE::lambda :
               r[k][i]=lambdag[i];
               break;
             case NEPTUNE::d_lambda_d_p_h :
               r[k][i]=dlambdag1[i];
               break;
             case NEPTUNE::d_lambda_d_h_p :
               r[k][i]=dlambdag3[i];
               break;
             case NEPTUNE::mu :
               r[k][i]=mug[i];
               break;
             case NEPTUNE::d_mu_d_p_h :
               r[k][i]=dmug1[i];
               break;
             case NEPTUNE::d_mu_d_h_p :
               r[k][i]=dmug3[i];
               break;
             case NEPTUNE::dncv :
               r[k][i]=dncv[i];
               break;
             case NEPTUNE::d_dncv_d_p_h :
               r[k][i]=ddncv1[i];
               break;
             case NEPTUNE::d_dncv_d_h_p :
               r[k][i]=ddncv3[i];
               break;
             case NEPTUNE::sigma :
               r[k][i]=sigma[i];
               break;
             case NEPTUNE::d_sigma_d_p_h :
               r[k][i]=dsigma1[i];
               break;
             case NEPTUNE::d_sigma_d_h_p :
               r[k][i]=dsigma3[i];
               break;
             case NEPTUNE::p_0 : // mixing, vapor
               r[k][i]=Pv[i];
               break;
             case NEPTUNE::h_0 :
               r[k][i]=hv[i];
               break;
             case NEPTUNE::rho_0 :
               r[k][i]=rv[i];
               break;
             case NEPTUNE::cp_0 :
               r[k][i]=cpv[i];
               break;
             case NEPTUNE::lambda_0 :
               r[k][i]=lambdav[i];
               break;
             case NEPTUNE::mu_0 :
               r[k][i]=muv[i];
               break;
             case NEPTUNE::prgr :
               r[k][i]=prgr[i];
               break;
             case NEPTUNE::rnc :
               r[k][i]=rnc[i];
               break;
             case NEPTUNE::mnc :
               r[k][i]=mnc[i];
               break;
             case NEPTUNE::xnc :
               r[k][i]=xnc[i];
               break;
             case NEPTUNE::d_p_0_d_p_h : // mixing, vapor derivation w.r.t (P,h)
               r[k][i]=dpv1[i];
               break;
             case NEPTUNE::d_p_0_d_h_p :
               r[k][i]=dpv3[i];
               break;
             case NEPTUNE::d_h_0_d_p_h :
               r[k][i]=dhv1[i];
               break;
             case NEPTUNE::d_h_0_d_h_p :
               r[k][i]=dhv3[i];
               break;
             case NEPTUNE::d_rho_0_d_p_h :
               r[k][i]=drv1[i];
               break;
             case NEPTUNE::d_rho_0_d_h_p :
               r[k][i]=drv3[i];
               break;
             case NEPTUNE::d_rho_0_d_p_0_h : // mixing, vapor derivation w.r.t (Pv,hv)
               r[k][i]=drvpv[i];
               break;
             case NEPTUNE::d_rho_0_d_h_0_p :
               r[k][i]=drvhv[i];
               break;
             case NEPTUNE::d_cp_0_d_p_0_h :
               r[k][i]=dcpvpv[i];
               break;
             case NEPTUNE::d_cp_0_d_h_0_p :
               r[k][i]=dcpvhv[i];
               break;
             case NEPTUNE::d_lambda_0_d_p_0_h :
               r[k][i]=dlambdavpv[i];
               break;
             case NEPTUNE::d_lambda_0_d_h_0_p :
               r[k][i]=dlambdavhv[i];
               break;
             case NEPTUNE::d_mu_0_d_p_0_h :
               r[k][i]=dmuvpv[i];
               break;
             case NEPTUNE::d_mu_0_d_h_0_p :
               r[k][i]=dmuvhv[i];
               break;
             case NEPTUNE::d_sigma_d_p_0_h :
               r[k][i]=dsigmapv[i];
               break;
             case NEPTUNE::d_T_d_p_0_h :
               r[k][i]=dtgpv[i];
               break;
             case NEPTUNE::d_T_d_h_0_p :
               r[k][i]=dtghv[i];
               break;
             case NEPTUNE::p_1 : // mixing, incondensable gas
               r[k][i]=p1[i];
               break;
             case NEPTUNE::p_2 :
               r[k][i]=p2[i];
               break;
             case NEPTUNE::p_3 :
               r[k][i]=p3[i];
               break;
             case NEPTUNE::p_4 :
               r[k][i]=p4[i];
               break;
             case NEPTUNE::h_1 :
               r[k][i]=h1[i];
               break;
             case NEPTUNE::h_2 :
               r[k][i]=h2[i];
               break;
             case NEPTUNE::h_3 :
               r[k][i]=h3[i];
               break;
             case NEPTUNE::h_4 :
               r[k][i]=h4[i];
               break;
             case NEPTUNE::d_p_0_d_c_1_ph : // mixing, derivation incondensable gas
               r[k][i]=dpvdx1[i];
               break;
             case NEPTUNE::d_p_0_d_c_2_ph :
               r[k][i]=dpvdx2[i];
               break;
             case NEPTUNE::d_p_0_d_c_3_ph :
               r[k][i]=dpvdx3[i];
               break;
             case NEPTUNE::d_p_0_d_c_4_ph :
               r[k][i]=dpvdx4[i];
               break;
             case NEPTUNE::d_h_0_d_c_1_ph :
               r[k][i]=dhvdx1[i];
               break;
             case NEPTUNE::d_h_0_d_c_2_ph :
               r[k][i]=dhvdx2[i];
               break;
             case NEPTUNE::d_h_0_d_c_3_ph :
               r[k][i]=dhvdx3[i];
               break;
             case NEPTUNE::d_h_0_d_c_4_ph :
               r[k][i]=dhvdx4[i];
               break;
             case NEPTUNE::d_T_d_c_1_ph :
               r[k][i]=dtgdx1[i];
               break;
             case NEPTUNE::d_T_d_c_2_ph :
               r[k][i]=dtgdx2[i];
               break;
             case NEPTUNE::d_T_d_c_3_ph :
               r[k][i]=dtgdx3[i];
               break;
             case NEPTUNE::d_T_d_c_4_ph :
               r[k][i]=dtgdx4[i];
               break;
             case NEPTUNE::d_rho_d_c_1_ph :
               r[k][i]=drgdx1[i];
               break;
             case NEPTUNE::d_rho_d_c_2_ph :
               r[k][i]=drgdx2[i];
               break;
             case NEPTUNE::d_rho_d_c_3_ph :
               r[k][i]=drgdx3[i];
               break;
             case NEPTUNE::d_rho_d_c_4_ph :
               r[k][i]=drgdx4[i];
               break;
             case NEPTUNE::d_rho_0_d_c_1_ph :
               r[k][i]=drvdx1[i];
               break;
             case NEPTUNE::d_rho_0_d_c_2_ph :
               r[k][i]=drvdx2[i];
               break;
             case NEPTUNE::d_rho_0_d_c_3_ph :
               r[k][i]=drvdx3[i];
               break;
             case NEPTUNE::d_rho_0_d_c_4_ph :
               r[k][i]=drvdx4[i];
               break;
             case NEPTUNE::d_cp_d_c_1_ph :
               r[k][i]=dcpdx1[i];
               break;
             case NEPTUNE::d_cp_d_c_2_ph :
               r[k][i]=dcpdx2[i];
               break;
             case NEPTUNE::d_cp_d_c_3_ph :
               r[k][i]=dcpdx3[i];
               break;
             case NEPTUNE::d_cp_d_c_4_ph :
               r[k][i]=dcpdx4[i];
               break;
             case NEPTUNE::d_lambda_d_c_1_ph :
               r[k][i]=dlambdadx1[i];
               break;
             case NEPTUNE::d_lambda_d_c_2_ph :
               r[k][i]=dlambdadx2[i];
               break;
             case NEPTUNE::d_lambda_d_c_3_ph :
               r[k][i]=dlambdadx3[i];
               break;
             case NEPTUNE::d_lambda_d_c_4_ph :
               r[k][i]=dlambdadx4[i];
               break;
             case NEPTUNE::d_mu_d_c_1_ph :
               r[k][i]=dmudx1[i];
               break;
             case NEPTUNE::d_mu_d_c_2_ph :
               r[k][i]=dmudx2[i];
               break;
             case NEPTUNE::d_mu_d_c_3_ph :
               r[k][i]=dmudx3[i];
               break;
             case NEPTUNE::d_mu_d_c_4_ph :
               r[k][i]=dmudx4[i];
               break;
             case NEPTUNE::d_sigma_d_c_1_ph :
               r[k][i]=dsigmadx1[i];
               break;
             case NEPTUNE::d_sigma_d_c_2_ph :
               r[k][i]=dsigmadx2[i];
               break;
             case NEPTUNE::d_sigma_d_c_3_ph :
               r[k][i]=dsigmadx3[i];
               break;
             case NEPTUNE::d_sigma_d_c_4_ph :
               r[k][i]=dsigmadx4[i];
               break;
             case NEPTUNE::d_dncv_d_c_1_ph :
               r[k][i]=ddncvdx1[i];
               break;
             case NEPTUNE::d_dncv_d_c_2_ph :
               r[k][i]=ddncvdx2[i];
               break;
             case NEPTUNE::d_dncv_d_c_3_ph :
               r[k][i]=ddncvdx3[i];
               break;
             case NEPTUNE::d_dncv_d_c_4_ph :
               r[k][i]=ddncvdx4[i];
               break;
             case NEPTUNE::d_rnc_d_c_1_ph :
               r[k][i]=drncdx1[i];
               break;
             case NEPTUNE::d_rnc_d_c_2_ph :
               r[k][i]=drncdx2[i];
               break;
             case NEPTUNE::d_rnc_d_c_3_ph :
               r[k][i]=drncdx3[i];
               break;
             case NEPTUNE::d_rnc_d_c_4_ph :
               r[k][i]=drncdx4[i];
               break;
             case NEPTUNE::d_mnc_d_c_1_ph :
               r[k][i]=dmncdx1[i];
               break;
             case NEPTUNE::d_mnc_d_c_2_ph :
               r[k][i]=dmncdx2[i];
               break;
             case NEPTUNE::d_mnc_d_c_3_ph :
               r[k][i]=dmncdx3[i];
               break;
             case NEPTUNE::d_mnc_d_c_4_ph :
               r[k][i]=dmncdx4[i];
               break;
             case NEPTUNE::T_sat : // saturation w.r.t P
               r[k][i]=tsatp[i];
               break;
             case NEPTUNE::h_l_sat :
               r[k][i]=hlsatp[i];
               break;
             case NEPTUNE::h_v_sat :
               r[k][i]=hvsatp[i];
               break;
             case NEPTUNE::rho_l_sat :
               r[k][i]=rholsatp[i];
               break;
             case NEPTUNE::rho_v_sat :
               r[k][i]=rhovsatp[i];
               break;
             case NEPTUNE::cp_l_sat :
               r[k][i]=cplsatp[i];
               break;
             case NEPTUNE::cp_v_sat :
               r[k][i]=cpvsatp[i];
               break;
             case NEPTUNE::d_T_sat_d_p : // derivation saturation w.r.t Pv
               r[k][i]=dtsatdp[i];
               break;
             case NEPTUNE::d_h_l_sat_d_p :
               r[k][i]=dhlsatdp[i];
               break;
             case NEPTUNE::d_h_v_sat_d_p :
               r[k][i]=dhvsatdp[i];
               break;
             case NEPTUNE::d_cp_l_sat_d_p :
               r[k][i]=dcplsatdp[i];
               break;
             case NEPTUNE::d_cp_v_sat_d_p :
               r[k][i]=dcpvsatdp[i];
               break;
             case NEPTUNE::d_rho_l_sat_d_p :
               r[k][i]=drholsatdp[i];
               break;
             case NEPTUNE::d_rho_v_sat_d_p :
               r[k][i]=drhovsatdp[i];
               break;
             case NEPTUNE::d2_T_sat_d_p_d_p :
               r[k][i]=d2tsatdp[i];
               break;
             case NEPTUNE::T_sat_0 : // saturation w.r.t. Pv
               r[k][i]=tsatpv[i];
               break;
             case NEPTUNE::h_l_sat_0 :
               r[k][i]=hlsatpv[i];
               break;
             case NEPTUNE::h_v_sat_0 :
               r[k][i]=hvsatpv[i];
               break;
             case NEPTUNE::rho_l_sat_0 :
               r[k][i]=rholsatpv[i];
               break;
             case NEPTUNE::rho_v_sat_0 :
               r[k][i]=rhovsatpv[i];
               break;
             case NEPTUNE::cp_l_sat_0 :
               r[k][i]=cplsatpv[i];
               break;
             case NEPTUNE::cp_v_sat_0 :
               r[k][i]=cpvsatpv[i];
               break;
             case NEPTUNE::d_T_sat_0_d_p_0_h : // derivation saturation w.r.t Pv
               r[k][i]=dtsatdpv[i];
               break;
             case NEPTUNE::d_h_l_sat_0_d_p_0_h :
               r[k][i]=dhlsatdpv[i];
               break;
             case NEPTUNE::d_h_v_sat_0_d_p_0_h :
               r[k][i]=dhvsatdpv[i];
               break;
             case NEPTUNE::d_rho_l_sat_0_d_p_0_h :
               r[k][i]=drholsatdpv[i];
               break;
             case NEPTUNE::d_rho_v_sat_0_d_p_0_h :
               r[k][i]=drhovsatdpv[i];
               break;
             case NEPTUNE::d_cp_l_sat_0_d_p_0_h :
               r[k][i]=dcplsatdpv[i];
               break;
             case NEPTUNE::d_cp_v_sat_0_d_p_0_h :
               r[k][i]=dcpvsatdpv[i];
               break;
             case NEPTUNE::d2_T_sat_0_d_p_0_d_p_0 :
               r[k][i]=d2tsatdpv[i];
               break;
             default :
               break;
          }
        }
      }
      
    }
    else if (idim == 2 && id_P == 1 && id_T == 1)
    {
      //cout << "EOS_Mixing fields : plan pT" << endl;

      ArrOfDouble xc0(nsca,0.e0);
      ArrOfDouble xc1(nsca,0.e0);
      ArrOfDouble xc2(nsca,0.e0);
      ArrOfDouble xc3(nsca,0.e0);
      ArrOfDouble xc4(nsca,0.e0);
      EOS_Field C0("c_0","c_0",xc0);
      EOS_Field C1("c_1","c_1",xc1);
      EOS_Field C2("c_2","c_2",xc2);
      EOS_Field C3("c_3","c_3",xc3);
      EOS_Field C4("c_4","c_4",xc4);
      
      if (ic[0] != -1) C0 = input[ic[0]];
      if (ic[1] != -1) C1 = input[ic[1]];
      if (ic[2] != -1) C2 = input[ic[2]];
      if (ic[3] != -1) C3 = input[ic[3]];
      if (ic[4] != -1) C4 = input[ic[4]];
      
      ArrOfDouble xhg(nsca,0.e0);
      EOS_Field hg("hg","h",xhg);

      EOS_Fields fsin_ph (nb_infields);
      fsin_ph[0] = input[iP];
      fsin_ph[1] = hg;
      for (int i=2; i<nb_infields; i++)
      {
        fsin_ph[i] = input[i];
      }
      
      // calcul saturation Tsatp,hvsatp = f(P)
      ArrOfDouble xtsatp(nsca),xhvsatp(nsca);
      EOS_Field tsatp("tsatp","T_sat",xtsatp);
      EOS_Field hvsatp("hvsatp","h_v_sat",xhvsatp);

      EOS_Fields fsout_sat (2);
      fsout_sat[0] = tsatp;
      fsout_sat[1] = hvsatp;
    
      (*this)[0].fluid().compute(input[iP],fsout_sat,errfield);

      // initialisation hg
      ArrOfDouble c(nb_fluids) ;
      EOS_Internal_Error err ;
      for (int i=0; (i < nsca); i++)
      {
        totab(c, C0[i], C1[i], C2[i], C3[i], C4[i]) ;
        double hjsum=0.e0;
        double hj=0.e0;
        for(int j=1; j<nb_fluids; j++)
        { 
          err = (*this)[j].fluid().compute_h_pT(input[iP][i],input[iT][i],hj);
          hjsum += c[j]*hj;
        }
        hg[i] = (hvsatp[i]+(input[iT][i]-tsatp[i])*2.e3)*C0[i] + hjsum; // Attention valeur de "CPCST" specifique a l'eau
      }
      
      double hmax;
      //double hmin;
      err =(*this)[0].fluid().get_h_max(hmax);
      //err =(*this)[0].fluid().get_h_min(hmin);

      ArrOfDouble xtg(nsca),xdtg3(nsca);
      EOS_Field tg("tg","T",xtg);
      EOS_Field dtg3("dtg3","d_T_d_h_p",xdtg3);

      EOS_Fields fsout_tmp (2);
      fsout_tmp[0] = tg;
      fsout_tmp[1] = dtg3;

      int nb_iter_max = 50 ;
      double epsn = 1.e-2 ;
      double tdif = 2.e0*epsn ;
      vector<int> indic(nsca,1) ;
      int tindic ;
      for(int k=0; k<nb_iter_max; k++)
      {
        // calcul plan (p,H)
        this->compute_perfect_gas(fsin_ph,fsout_tmp,errfield);
        
        // verification convergence
        for(int i=0; i<nsca; i++)
        {
          if (indic[i] == 1)
          {
            tdif   = input[iT][i] - tg[i] ;
            hg[i] = hg[i] + tdif/dtg3[i] ; // H = H + dT / [∂T/∂h]P
            hg[i] = std::min(hg[i],hmax) ;
            //hg[i] = std::max(hg[i],hmin) ;
            if (fabs(tdif) <= epsn)  indic[i] = 0 ;
            if ((fabs(tdif) > epsn) && (k >= nb_iter_max)) 
            {
              cout << "EOS_Mixing solver : No convergence in hg computation" << endl;
              return EOS_Error::error ;
            }
          }
        }
        tindic = 0 ;
        for(int i=0; i<nsca; i++) tindic += indic[i] ;
        if (tindic == 0) break ;
      }
      
      // calcul final en fonction de p et H
      this->compute_perfect_gas(fsin_ph,r,errfield);

      for(int i=0; i<nsca; i++)
      {
        for(int k=0; k<nb_outfields; k++)
        {
          switch (r[k].get_property_number())
          {
             case NEPTUNE::h :
               r[k][i]=hg[i];
               break;
          }
        }
      }

    }    
    else
    {
      cout << "EOS_Mixing : input field must be (p,h,X) or (p,T,X) and not :" << input[0].get_property_name() << " and " << input[1].get_property_name() << endl;
      errfield = EOS_Internal_Error::INPUT_PROPERTY ;
      return EOS_Error::error ;
    }

    return EOS_Error::good ;
   
  }

  // provisional for EOS_Mixing
  EOS_Internal_Error EOS_Mixing::calcamix_T_ph(double p, double h, double& T
               , double C_0
               , double C_1
               , double C_2
               , double C_3
               , double C_4
               ) const
  { T = 0.e0 ;
    double Ti = 0.e0 ;
    EOS_Internal_Error err = EOS_Internal_Error::OK ;
    ArrOfDouble C(nb_fluids) ;
    totab(C, C_0 ,C_1, C_2, C_3, C_4) ;
    for(int i=0; i<nb_fluids; i++)
       { EOS_Internal_Error err2 = (*this)[i].fluid().compute_T_ph(p, h, Ti) ;
         err = worst_internal_error(err, err2) ;
         T += C[i]*Ti ;
       }
    return err ;
  }
//
// Methodes algorithme cathare
// Hypothesis :
//    mixture follows Dalton law
//    noncondensible gas are perfect
//
  // TODO faire proprement les erreurs (EOS_Internal_Error)
  int EOS_Mixing::compute_pv_hv_ph(double P, double h, double &Pv, double &hv,
                 double c_0, double c_1, double c_2, double c_3, double c_4) const
  {
    ArrOfDouble c(nb_fluids) ;
    totab(c, c_0 ,c_1, c_2, c_3, c_4) ;
    //
    // compute cpsum=sum(Ci*cpi)
    //
    // init    hi7sum=sum(Ci*hi)
    //
    double cpsum=0.e0;
    double xrsum=0.e0;
    double hi7sum=0.e0;
    double cpi=0.e0;
    double hi=0.e0;
    double epspp=1.e3;
    double valp=epspp;
    double valh=1.e0;
    EOS_Internal_Error err;
    double tcrit;
    err = (*this)[0].fluid().get_T_crit(tcrit);
    double Tinit=tcrit;
    for(int i=1; i<nb_fluids; i++)
      { 
        err = (*this)[i].fluid().compute_cp_pT(P,Tinit,cpi); //TODO modify this function as independant of P&T
        err = (*this)[i].fluid().compute_h_pT(P,Tinit,hi);
        cpsum += c[i]*cpi;
        xrsum += c[i]*(*this)[i].fluid().get_prxr();
        hi7sum += c[i]*hi - c[i]*cpi*(Tinit-273.15); // pour coller au hi7sum de c2
        valp = std::min(valp, epspp*c[i]);
      }
    
    //
    // init Pv
    //
    
    Pv = c[0]*P;
    Pv = std::max(Pv,1.e-10);
    
    //
    // init hv
    //
    
    // calcul saturation Tsatpv, Hsatpv = f(Pv)
    double Tsatpv, hvsatpv;
    int n=1;
    ArrOfDouble xpv(n),xtsatpv(n),xhvsatpv(n);
    ArrOfDouble xhv(n),xtg(n),xdtgpv(n),xdtghv(n),xrv(n),xdrvpv(n),xdrvhv(n);
    ArrOfInt xerr(n);
    xpv[0]=Pv;
    EOS_Field fin_sat ("in_sat1", "p", xpv);
    EOS_Field fout_sat1 ("out_sat1", "T_sat", xtsatpv);
    EOS_Field fout_sat2 ("out_sat2", "h_v_sat", xhvsatpv);
    EOS_Error_Field ferr_sat(xerr);
    
    EOS_Fields fsin_sat (1);
    fsin_sat[0] = fin_sat;
    EOS_Fields fsout_sat (2);
    fsout_sat[0] = fout_sat1;
    fsout_sat[1] = fout_sat2;
    
    (*this)[0].fluid().compute(fsin_sat,fsout_sat,ferr_sat);
    Tsatpv = xtsatpv[0];
    hvsatpv = xhvsatpv[0];

    valp = std::min(valp, epspp*c[0]);
    valp = std::max(valp, 5.e0);

    // calcul hv
    double acpv = 2.e3 + Pv*1.e-4; // TODO d'ou sortent ces valeurs en brut ? Attention aux autres fluides que l'eau...
    double zerhvs = hvsatpv - acpv*(Tsatpv-273.15);
    double zdeno = cpsum + c[0]*acpv;
    double atv = (h - zerhvs*c[0] - hi7sum) / zdeno;
    hv = acpv*atv + zerhvs;
    xhv[0]=hv;
    
    //
    // Newton method
    // Resolution of F(Pv, hv)=0 and G(Pv,hv)=0
    //
    
    double fff, ggg, croipv, croihv, dfdpv, dfdhv, dgdpv, dgdhv, usden; // variables intermediaires
    double dpvr = 2*valp;
    double dhvr = 2*valh;
    double Tg, dtgpv, dtghv, rv, drvpv, drvhv;
    int k;
    //int ierr;
    int nb_iter_max = 50 ;
    for(k = 0; (k < nb_iter_max) && ((fabs(dpvr) > valp) || (fabs(dhvr) > valh)); k++) // Tests convergence
    {
      EOS_Field fin_tmp1 ("in_tmp1", "p", xpv);
      EOS_Field fin_tmp2 ("in_tmp2", "h", xhv);
      EOS_Field fout_tmp1 ("out_tmp1", "T", xtg);
      EOS_Field fout_tmp2 ("out_tmp2", "d_T_d_p_h", xdtgpv);
      EOS_Field fout_tmp3 ("out_tmp3", "d_T_d_h_p", xdtghv);
      EOS_Field fout_tmp4 ("out_tmp4", "rho", xrv);
      EOS_Field fout_tmp5 ("out_tmp5", "d_rho_d_p_h", xdrvpv);
      EOS_Field fout_tmp6 ("out_tmp6", "d_rho_d_h_p", xdrvhv);
      EOS_Error_Field ferr_tmp(xerr);
      
      EOS_Fields fsin_tmp (2);
      fsin_tmp[0] = fin_tmp1;
      fsin_tmp[1] = fin_tmp2;
      EOS_Fields fsout_tmp (6);
      fsout_tmp[0] = fout_tmp1;
      fsout_tmp[1] = fout_tmp2;
      fsout_tmp[2] = fout_tmp3;
      fsout_tmp[3] = fout_tmp4;
      fsout_tmp[4] = fout_tmp5;
      fsout_tmp[5] = fout_tmp6;
      
      (*this)[0].fluid().compute(fsin_tmp,fsout_tmp,ferr_tmp);
      Tg = xtg[0];
      dtgpv = xdtgpv[0];
      dtghv = xdtghv[0];
      rv = xrv[0];
      drvpv = xdrvpv[0];
      drvhv = xdrvhv[0]; //TODO : pas beau...
      
      // calcul hi7sum
      hi7sum=0.e0;
      for(int i=1; i<nb_fluids; i++)
      { 
        err = (*this)[i].fluid().compute_h_pT(P,Tg,hi);
        hi7sum += c[i]*hi;// attention c'est different du hi7sum de c2: hi7sum = hi7sum(c2) + cpsum*Tg
      }
      
      // resolution du systeme 
      fff = hi7sum + c[0]*hv - h; // cpsum*Tg inclut dans hi7sum
      ggg = xrsum*Tg*rv + c[0]*(Pv - P);
      croipv = Tg*drvpv + rv*dtgpv;
      croihv = Tg*drvhv + rv*dtghv;
      dfdpv = cpsum*dtgpv; // Attention ici !! valable pour cp constant
      dfdhv = cpsum*dtghv + c[0]; // Attention ici !! valable pour cp constant
      dgdpv = xrsum*croipv + c[0];
      dgdhv = xrsum*croihv;
      usden = 1 / (dfdpv*dgdhv - dfdhv*dgdpv);
      dpvr = (dfdhv*ggg - dgdhv*fff) * usden;
      dhvr = (dgdpv*fff - dfdpv*ggg) * usden;
      // calcul Pv, hv
      Pv = Pv + dpvr;
      Pv = std::max(Pv , 1.e-10);
      Pv = std::max(Pv , 1.e-2*c[0]);
      hv = hv + dhvr;
      xpv[0]=Pv;
      xhv[0]=hv;
    }
    //
    // Verification convergence TODO implementer erreur propre
    //
    if ( (k >= nb_iter_max) && ((fabs(dpvr) > valp) || (fabs(dhvr) > valh)) )
    {
      cout << "EOS_Mixing solver : No convergence in (Pv, hv) computation" << endl;
      return EOS_Error::error ;
    }
    return EOS_Error::good;
  }

}
