#****************************************************************************
# Copyright (c) 2015 - 2016, CEA
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
# 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
# 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#*****************************************************************************

import locale

class EosComponent:
    
    def __init__(self,detailOfFunction,ava):
        """The {EosComponent} class calls the EOS component to compute
        the thermodynamic properties defined by the user data.
        The constructor gets the user data (detailOfFunction).
        The {compute} method gets the values of the user data and calls
        the EOS component. This last appears as an object in the arguments.
        The called methods are {computeValue, getNumberOfValue and
        getValueAsList}."""
        self.eosData = detailOfFunction
        self.physicalName = ava.getThQDictionnary()
        
        thmod  = self.eosData["Thermodynamic_Model"]
        fluid  = self.eosData["Fluid"]
        fldeq  = self.eosData["Fluid_Equation"]
        func   = self.eosData["Function"]
        var    = self.eosData["Variable"]
        varmin = self.eosData["var_min"]
        varmax = self.eosData["var_max"]
        par2   = self.eosData["2d_param"]
        val2   = self.eosData["2d_param_value"]
        thetit = func + " of " + fluid + " (" + thmod + ", "  \
               + fldeq + ") " + var + "(from " + varmin + " to " + varmax + ") "
        if par2 != "at saturation" and par2 != "at limit":
            thetit = thetit + par2 + "=" + val2

        
    def compute(self,objv):
        thmod     = self.eosData["Thermodynamic_Model"]
        fldeq     = self.eosData["Fluid_Equation"]
        eos_thmod = "EOS_" + thmod
            
        var = self.eosData["Variable"]
        var2 = self.physicalName[var]
        func = self.eosData["Function"]
        qty = self.physicalName[func]

        if self.eosData["derivative"] == "P":
            if self.eosData["2d_param"] == "at saturation" or self.eosData["2d_param"] == "at limit" :
                qty = "d_" + qty + "_d_p"
            else:
                qty = "d_" + qty + "_d_p_h"
        elif self.eosData["derivative"] == "h":
            if self.eosData["2d_param"] == "at saturation" or self.eosData["2d_param"] == "at limit" :
                qty = "d_" + qty + "_d_p"
            else:
                qty = "d_" + qty + "_d_h_p"

        par2 = self.eosData["2d_param"]
        val2 = float(self.eosData["2d_param_value"])
        if par2 == "at saturation":
            par22 = "saturation"
            val2 = 0.
        elif par2 == "at limit":
            par22 = "limit"
            val2 = 0.
        elif par2 == "Pressure":
            par22 = self.physicalName["Pressure"]
        elif par2 == "Enthalpy":
            par22 = self.physicalName["Enthalpy"]
        elif par2 == "Temperature":
            par22 = self.physicalName["Temperature"]
        
        #print "===default  =",locale.getdefaultlocale(),"==="
        #print "===CODESET  =",locale.nl_langinfo(locale.CODESET),"==="
        #print "===RADIXCHAR=",locale.nl_langinfo(locale.RADIXCHAR),"==="
        #print "===",locale.str(3.14),"==="
        #print "===",locale.atof("3.14"),"==="
        #
        # setting encoding to en_US.utf8 has been needed to read files
        # properly, at least for the following environment :
        # linux 2.6.32 (ubuntu 10.04)
        # gcc 4.4.3 (==> g++ and gfortran)
        # python 2.6.5
        # Qt 4.6.2
        #
        # default encoding is restored after the call to objv.computeValue
        #
        #locale.setlocale(locale.LC_NUMERIC,'en_US.utf8')
        thedecpt = locale.localeconv()['decimal_point']
        if thedecpt == ',':
            print ("---On a un probleme")
            locale.setlocale(locale.LC_ALL, 'C')
            print ("---On (re)met l'environnement de codage C")

        #print "===RADIXCHAR=",locale.nl_langinfo(locale.RADIXCHAR),"==="
        objv.computeValue(eos_thmod, fldeq, var2, qty, par22, val2)
        #locale.resetlocale(locale.LC_NUMERIC)
        n = objv.getNumberOfValue()
        y = objv.getValueAsList(n)
        return y

