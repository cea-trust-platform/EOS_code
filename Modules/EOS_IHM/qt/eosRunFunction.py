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

import os
import eosComponent
#import eosPrint
import eosPrint4
import eosihm

class EosRun:
    """Quite all the work is done in the constructor.
    It makes up the EOS component call, actually makes this call
    through {eosComponent}, retrieves the results and plots (Plot option)
    (with gnuplot) or writes (Print option) (inside a popup window)
    the thermodynic property. The plot (or writing) associated file
    (.data file) is always written (Write to File option).
    Several consistancy controls on the user inputs are done.
    In order to use {gnuplot}, a two (or three) columns file is written
    (.data file) together with a {gnuplot} data file (.gpt file)
    that reads and plots the thermodynamic property values (.data  file).
    2D plots are persistent, {gnuplot} let back the handling to the user.
    3D plots are not persistent, {gnuplot} keeps the handling so
    3D {gnuplot} facilities are avalaible. The user has to manually close
    the {gnuplot} window to get the handling on the GUI.
    (if someone knows a better solution, please let us know)
    PostScript files are written by {gnuplot} ({gnuplot} can write
    other types of file but this is not the case here)."""
    def __init__(self,parent,selectedFunction,detailOfFunction,runType,ava,plot3D):
        n = len(selectedFunction)
        if n == 0:
            theout = eosPrint4.Ui_Form4(parent=parent,thetext="No Function selected")
            theout.show()
            return
        elif n > 1 and plot3D:
            theout = eosPrint4.Ui_Form4(parent=parent,thetext="Select only one function for a 3D plot")
            theout.show()
            return

        #if plot3D:
            #print "3D PLOTTING"
        #else:
            #print "2D PLOTTING"

#########################################################
# a dictionary for abbreviations for plotting 
        dAbQty={}
        dAbQty["Pressure"]                    = "p"
        dAbQty["Enthalpy"]                    = "h"
        dAbQty["Temperature"]                 = "T"
        dAbQty["Density"]                     = "rho"
        dAbQty["InternalEnergy"]              = "u"
        dAbQty["Entropy"]                     = "s"
        dAbQty["DynamicViscosity"]            = "mu"
        dAbQty["ThermalConductivity"]         = "lambda"
        dAbQty["HeatCapacity"]                = "cp"
        dAbQty["SurfaceTension"]              = "sigma"
        dAbQty["VolumetricThermalExpansion"]  = "beta"
        dAbQty["SpeedOfSound"]                = "w"
        dAbQty["FreeEnthalpy"]                = "g"
        dAbQty["FreeEnergy"]                  = "f"
        dAbQty["Prandtl"]                     = "pr"
        dAbQty["SaturatedLiquidDensity"]      = "rho_l_sat"
        dAbQty["SaturatedVaporDensity"]       = "rho_v_sat"
        dAbQty["SaturatedLiquidEnthalpy"]     = "h_l_sat"
        dAbQty["SaturatedVaporEnthalpy"]      = "h_v_sat"
        dAbQty["SaturatedTemperature"]        = "T_sat"
        dAbQty["SaturatedPressure"]           = "p_sat"
        dAbQty["SaturatedLiquidHeatCapacity"] = "cp_l_sat"
        dAbQty["SaturatedVaporHeatCapacity"]  = "cp_v_sat"
        dAbQty["LimitLiquidEnthalpy"]         = "h_l_lim"
        dAbQty["LimitVaporEnthalpy"]          = "h_v_lim"
#########################################################

        objv = eosihm.EosIhm()
        thefilenames=[]
        for j in range(n):
            i = int(selectedFunction[j])
            li = []
            thmod = detailOfFunction[i]["Thermodynamic_Model"]
            if thmod == "External data":
                m = EosReadFile()
                (x,y) = m.compute(detailOfFunction[i])
            else:
                vmin = float(detailOfFunction[i]["var_min"])
                vmax = float(detailOfFunction[i]["var_max"])
                nstp = int(detailOfFunction[i]["var_step"])
                objv.initValueByStep(vmin,vmax,nstp)
                x = objv.getValueAsList(nstp+1)
                if not plot3D:
                    m = eosComponent.EosComponent(detailOfFunction[i],ava)
                    y = m.compute(objv)
                    if  self.sortAnError(objv,parent)!= 0:
                        rrr  = objv.getErrorAsList(nstp+1)
                        ybis = []
                        xbis = []
                        for k in range(nstp+1):
                            if rrr[k] == 0 :
                                ybis.append(y[k])
                                xbis.append(x[k])
                        x=xbis
                        y=ybis
                        if len(xbis) == 0:return
                        #return
                    
                else:
                    vmin2 = float(detailOfFunction[i]["var_min2"])
                    vmax2 = float(detailOfFunction[i]["var_max2"])
                    nstp2 = int(detailOfFunction[i]["var_step2"])
                    dd = (vmax2 - vmin2)/float(nstp2)
                    sd_param_value_save=detailOfFunction[i]["2d_param_value"]
                    y=[]
                    z=[]
                    zz=[]
                    for k in range(nstp2+1):
                        yk = vmin2+float(k)*dd
                        detailOfFunction[i]["2d_param_value"] = str(yk)
                        objv.initValueByStep(vmin,vmax,nstp)
                        m = eosComponent.EosComponent(detailOfFunction[i],ava)
                        zk = m.compute(objv)
                        if  self.sortAnError(objv,parent)!= 0:
                            detailOfFunction[i]["2d_param_value"]=sd_param_value_save
                            return
                        y.append(yk)
                        z.append(zk)
                        # zz pour C++ : dans zz on balaye d'abord en X
                        # voir objv.putTheArrays
                        for kk in zk:
                            zz.append(kk)
                    
                    detailOfFunction[i]["2d_param_value"]=sd_param_value_save
                    

            if not plot3D:
                for k in range(len(x)):
                    li.append("\n"+str(x[k])+" "+str(y[k]))
            else:
                # print detailOfFunction[i]
                thmod = detailOfFunction[i]["Thermodynamic_Model"]
                fluid = detailOfFunction[i]["Fluid"]
                fldeq = detailOfFunction[i]["Fluid_Equation"]
                func = detailOfFunction[i]["Function"]
                if detailOfFunction[i]["derivative"] == "P":
                    func = "(d_dp)" + func
                elif detailOfFunction[i]["derivative"] == "h":
                    func = "(d_dh)" + func
                thetit = fluid + " " + thmod + " " + fldeq + " " + func

                # write the med file

                thex=detailOfFunction[i]["Variable"]
                they=detailOfFunction[i]["2d_param"]
                medfile = self.getNewFileName("eoscrb","med",1)
                objv.putTheArrays(x, y, zz, len(x), len(y), len(zz), fluid, thmod, fldeq, func, thex, they, medfile)

                # write the X Y Z file for gnuplot
                
                li.append("# " + str(len(x)) + " " + str(len(y)))
                li.append("\n# " + thetit)
                li.append("\n# " + detailOfFunction[i]["Variable"])
                li.append("\n# " + detailOfFunction[i]["2d_param"])
                for k in range(len(x)):
                    for kk in range(len(y)):
                        li.append("\n  " + str(x[k]) + "  " + str(y[kk]) + "  " + str(z[kk][k]))
                    li.append("\n")
                li.append("\n ")

            datafile = self.getNewFileName("eoscrb","data",1)
            f1 = open(datafile,"w")
            f1.writelines(li)
            f1.close()
            thefilenames.append(datafile)


        if runType == "writefile":
            return
        elif runType == "plot":
            if plot3D:
                f2 = open("don2.gpt","w")
                li = []
                i  = int(selectedFunction[0])
                thmod = detailOfFunction[i]["Thermodynamic_Model"]
                fluid = detailOfFunction[i]["Fluid"]
                fldeq = detailOfFunction[i]["Fluid_Equation"]
                func  = detailOfFunction[i]["Function"]
                if detailOfFunction[i]["derivative"] == "P":
                    func = "(d_dp)" + func
                elif detailOfFunction[i]["derivative"] == "h":
                    func = "(d_dh)" + func
                thetit = fluid + " (" + thmod + ", " + fldeq + ") " + func
                # print thetit
                li.append("\nset title \"EOS : " + thetit + "\"")
                li.append("\nset size 1.0 ,1.0")
                # li.append("\nset dgrid3d 30,30,1")
                li.append("\nset nokey")
                li.append("\nset style data  line")
                li.append("\nset view 45,60,1,1")
                li.append("\nset surface")
                li.append("\nset grid") 
                li.append("\nset pm3d")
                li.append("\nset palette")
                varx = detailOfFunction[i]["Variable"]
                vary = detailOfFunction[i]["2d_param"]
                li.append("\nset xlabel \"" + varx + "\"")
                li.append("\nset ylabel \"" + vary + "\"")
                li.append("\nsplot \"" + thefilenames[j] + "\" using 1:2:3 w lines")
                li.append("\npause -1 \"Hit return to continue\"")
                li.append("\nset terminal postscript color")
                psfile = self.getNewFileName("eosplot","ps",1)
                li.append("\nset output '" + psfile + "'")
                li.append("\nreplot")
                f2.writelines(li)
                f2.close()
                os.system ("gnuplot don2.gpt ")


                return
            f2 = open("don2.gpt","w")
            li = []
            li.append("\nset title \"EOS \"")
            li.append("\nset style data  linespoints") 
            li.append("\nset style line 1 lt 1 lw 2 pt 1 ps 2")  
            li.append("\nset style line 2 lt 2 lw 2 pt 2 ps 2") 
            li.append("\nset style line 3 lt 3 lw 2 pt 3 ps 2") 
            li.append("\nset style line 4 lt 4 lw 2 pt 4 ps 2") 
            li.append("\nset style line 5 lt 5 lw 2 pt 5 ps 2") 
            li.append("\nset style line 6 lt 6 lw 2 pt 6 ps 2") 
            li.append("\nset style line 7 lt 7 lw 2 pt 7 ps 2") 
            li.append("\nset style line 8 lt 8 lw 2 pt 8 ps 2")
            var = detailOfFunction[int(selectedFunction[0])]["Variable"]
            li.append("\nset xlabel '" + var + "'")
            fun = ""
            for j in range(1):
                fun = fun + detailOfFunction[int(selectedFunction[j])]["Function"]
                #print "=== eosRunFunction fun:",fun,"==="
                if detailOfFunction[int(selectedFunction[j])]["derivative"] == "P":
                    fun = "(d_dp)" + fun
                elif detailOfFunction[int(selectedFunction[j])]["derivative"] == "h":
                    fun = "(d_dh)" + fun
            if n > 1:fun=""
            li.append("\nset ylabel '" + fun + "'")
            li.append("\nplot \\")
            thetitle = []
            for j in range(n):
                i = int(selectedFunction[j])
                fun = detailOfFunction[i]["Function"]
                fun=dAbQty[fun]
                if detailOfFunction[i]["derivative"] == "P":
                    fun = "(d_dp)" + fun
                elif detailOfFunction[i]["derivative"] == "h":
                    fun = "(d_dh)" + fun
                thmod = detailOfFunction[i]["Thermodynamic_Model"]
                fluid = detailOfFunction[i]["Fluid"]
                fldeq = detailOfFunction[i]["Fluid_Equation"]
                par2 = detailOfFunction[i]["2d_param"]
                val2 = detailOfFunction[i]["2d_param_value"]
                thetit = fun + " " + fluid + " (" + thmod + ", " + fldeq + ") "
                if par2 != "at saturation" and par2 != "at limit":
                    thetit = thetit + par2 + "=" + val2
                thetitle.append(thetit)
                if j == n-1:
                    #print "------one"
                    li.append("\n'" + thefilenames[j] + "' title '" + thetitle[j] + "' ls " + str(j + 1))
                else:
                    #print "--------two"
                    li.append("\n'" + thefilenames[j] + "' title '" + thetitle[j] + "' ls " + str(j + 1) + " ,\\")

            li.append("\nset terminal postscript color ; \\")
            psfile = self.getNewFileName("eosplot","ps",1)
            li.append("\nset output '" + psfile + "' ; \\")
            li.append("\nplot \\")
            for j in range(n):
                if j == n-1:
                    li.append("\n'" + thefilenames[j] + "' title '" + thetitle[j] + "' ls " + str(j + 1))
                else:
                    li.append("\n'" + thefilenames[j] + "' title '" + thetitle[j] + "' ls " + str(j + 1) + " ,\\")
                                
            f2.writelines(li)
            f2.close()
            os.system ("gnuplot -persist don2.gpt &")
            
        elif  runType == "print":
            f3 = open("eosprint.output","w")
            li = []
            for j in range(n):
                i = int(selectedFunction[j])
                thmod = detailOfFunction[i]["Thermodynamic_Model"]
                fluid = detailOfFunction[i]["Fluid"]
                fldeq = detailOfFunction[i]["Fluid_Equation"]
                func = detailOfFunction[i]["Function"]
                var = detailOfFunction[i]["Variable"]
                varmin = detailOfFunction[i]["var_min"]
                varmax = detailOfFunction[i]["var_max"]
                par2 = detailOfFunction[i]["2d_param"]
                val2 = detailOfFunction[i]["2d_param_value"]
                thetit = fluid + " (" + thmod + ", " + fldeq + ") " + func + "(" + var  \
                       + ", from " + varmin + " to " + varmax + ") " + par2
                if par2 != "at saturation" and par2 != "at limit":
                    if not plot3D:
                        thetit = thetit + "=" + val2
                    else:
                        varmin2 = detailOfFunction[i]["var_min2"]
                        varmax2 = detailOfFunction[i]["var_max2"]
                        thetit = thetit + " from " + varmin2 + " to " + varmax2
                li.append("\n" + thetit)
                li.append("\n ")
                f4 = open(thefilenames[j], "r")
                for line in f4.readlines():
                    li.append(line)
                f4.close()
            f3.writelines(li)
            f3.close()
            theout = eosPrint4.Ui_Form3(parent=parent, thefile="eosprint.output")
            theout.show()
            return

    def getNewFileName(self, pref, post, k):
        psname = pref + str(k) + "."+post
        if os.access(psname,os.F_OK) == 1:
            k += 1
            a = self.getNewFileName(pref,post,k)
            return a
        else:
            return psname

    def sortAnError(self, objv, parent):
        ierror = 0
        chaerr = ""
        ierror = objv.getTheErrorI()
        chaerr = objv.getTheErrorC()
        if ierror != 0:
            theout = eosPrint4.Ui_Form4(parent=parent, thetext=chaerr)
            theout.show()
            theout = eosPrint4.Ui_Form3(parent=parent, thefile="le_fichier_des_erreurs")
            theout.show()
        return ierror

class EosReadFile:
    """The {EosReadFile} class owns the {External data} thermodynamic method
    which is only a reading of the property values from a file.
    It is not yet implemented in the GUI."""
    def compute(self,detailOfFunction):
        print ("Reading file:",detailOfFunction["data_filename"])
        x = []
        y = []
        f4 = open(detailOfFunction["data_filename"], "r")
        for line in f4.readlines():
            s = line.split()
            if len(s) == 0:
                continue
            #print "----",s
            xi = s[0]
            yi = s[1]
            x.append(float(xi))
            y.append(float(yi))
        f4.close()
        return (x,y)
