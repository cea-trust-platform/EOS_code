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



/*!
 * \file EOS_THETIS.hxx
 *
 * \brief Encapsulation of THETIS functions
 */

#ifndef  EOS_THETIS_H
#define  EOS_THETIS_H

#include "EOS/API/EOS_Fluid.hxx"

using namespace NEPTUNE;

namespace NEPTUNE_EOS
{
  //! Encapsulation of THETIS functions
  class EOS_THETIS : public EOS_Fluid
  {
        static const AString tablename;
        static const AString versionname;

  public:

    virtual const AString& table_name() const;
    virtual const AString& version_name() const;
//   const AString version_name;
    
    EOS_THETIS();
    virtual ~EOS_THETIS();

    //! Initialize Thetis 
    using EOS_Fluid::init;
    virtual int init(const Strings& strings);

    //! See EOS_Fluid
    void describe_error(const EOS_Internal_Error err_code, AString & description) const;

    //! See EOS_Fluid
    EOS_Error compute( const EOS_Field& p, 
                       const EOS_Field& h, 
                       EOS_Fields& r, 
                       EOS_Error_Field& errfield) const;
    //! See EOS_Fluid
    EOS_Error compute(const EOS_Field& p, 
                      const EOS_Field& h, 
                      EOS_Field& r, 
                      EOS_Error_Field& errfield) const;
    //! See EOS_Fluid
    EOS_Error compute(const EOS_Field& p, 
                      EOS_Field& r, 
                      EOS_Error_Field& errfield) const;
    //! See EOS_Fluid
    EOS_Error compute(const EOS_Field& p, 
                      EOS_Fields& output, 
                      EOS_Error_Field& errfield) const;
    //! See EOS_Fluid
    EOS_Error compute( const EOS_Fields& input,
                       EOS_Fields& output, 
                       EOS_Error_Field& errfield) const;

  public:

    //! to know if Thetis is already initialized 
    static bool initialized ;

    //! fluid number that Thetis can load
    static const int nbFluidMax;

    //! count Fluids loaded 
    static int fluidCounter;

    //! thetis compute mod
    //!   1 : compute at loading (Default mod)
    //!   2 : compute at first compute
    //!   3 : every time 
    static int computeMod;

  protected :
    
    //! set the fluid number 
    virtual int getFluidNum() const = 0;

    //! give Quality (depend on fluid state)
    virtual void getQuality(ArrOfDouble& quality) const = 0;

    //! give fluid state
    using EOS_Fluid::is_liquid;
    virtual bool is_liquid() const = 0;
    
    //! error treatment
    virtual EOS_Internal_Error errorStudy(const int errorNum, const AString& name) const;

  private :

    //! compute all saturation properties
    EOS_Error calth_all_saturation(const EOS_Field& P,
                                   EOS_Fields& r,
                                   EOS_Error_Field& err) const;
    //! compute all properties
    EOS_Error calth_all(const EOS_Fields& input, 
                        EOS_Fields& r,
                        EOS_Error_Field& err) const;

  };
}

#endif

