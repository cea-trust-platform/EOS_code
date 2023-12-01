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
#ifndef EOS_IPP_VAPOR_HXX_
#define EOS_IPP_VAPOR_HXX_

#include "EOS/Src/EOS_Ipp/EOS_Ipp.hxx"

using namespace NEPTUNE;

namespace NEPTUNE_EOS
{
        class EOS_Ipp_vapor : public EOS_Ipp
        {
        public :

                virtual int init(const Strings&);
                virtual int init(const Strings&, const Strings&);

                //!(p,T)
                virtual EOS_Internal_Error compute_h_pT(double p, double T, double& h) const;
                //virtual EOS_Internal_Error compute_d_h_d_T_pT(double p, double T, double& h) const;
                
    virtual EOS_Internal_Error compute_rho_pT(double p, double T, double&) const;
    virtual EOS_Internal_Error compute_u_pT(double p, double T, double&) const;
    virtual EOS_Internal_Error compute_s_pT(double p, double T, double&) const;
    virtual EOS_Internal_Error compute_mu_pT(double p, double T, double&) const;
    virtual EOS_Internal_Error compute_lambda_pT(double p, double T, double&) const;
    virtual EOS_Internal_Error compute_cp_pT(double p, double T, double&) const;
    virtual EOS_Internal_Error compute_sigma_pT(double p, double T, double&) const;
    virtual EOS_Internal_Error compute_w_pT(double p, double T, double&) const;
    virtual EOS_Internal_Error compute_g_pT(double p, double T, double&) const;
    virtual EOS_Internal_Error compute_f_pT(double p, double T, double&) const;
    virtual EOS_Internal_Error compute_pr_pT(double p, double T, double&) const;
    virtual EOS_Internal_Error compute_beta_pT(double p, double T, double&) const;
    virtual EOS_Internal_Error compute_gamma_pT(double p, double T, double&) const;

        private:
                static int type_Id;

        };
}

#include "EOS_Ipp_vapor_i.hxx"
#endif /* EOS_IPP_VAPOR_HXX_ */



