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

class eosUtil:
    """The {eosUtil} class builds what is avalaible in the EOS component."""

    
    def __init__(self,eosdatapath):
        self.eosdatapath = eosdatapath


    def list_eostm(self):
        """List of Thermodynamic Models (old : method)"""
        
        fname = self.eosdatapath + "/index.eos"
        l_tm = []
        f = open(fname,"r") 
        all_lines = f.readlines() 
        for line in all_lines:
           words = line.split()
           if len(words) > 0:
              l_tm.append(words[0])

        l_tm = list(set(l_tm))
        l_tm.sort()
        return l_tm


    def list_eosfld(self):
        """List of Fluids"""
        
        fname = self.eosdatapath + "/index.eos"
        l_fld = []
        f = open(fname,"r") 
        all_lines = f.readlines() 
        for line in all_lines:
           words = line.split()
           if len(words) > 3:
              l_fld.append(words[3])

        l_fld = list(set(l_fld))
        l_fld.sort()
        return l_fld

    def buildIndexFldDict(self):
        """Dictionnary of Fluids  D[fluid] = [list T.M.] :
        for each fluid  a list of Thermodynamic Models (old : method) is given"""
        
        dict1 = {}

        l_fld = self.list_eosfld()
        for fld in l_fld:
           dict1[fld] = []

        fname = self.eosdatapath + "/index.eos"
        f = open(fname,"r") 
        all_lines = f.readlines() 
        for line in all_lines:
           words = line.split()
           if len(words) > 3:
              tm  = words[0]
              fld = words[3]
              if not tm in dict1[fld]:
                 dict1[fld].append(tm)

        for fld in l_fld:
           dict1[fld] = list(set(dict1[fld]))
           dict1[fld].sort()

        return dict1


    def buildIndexDict(self):
        """Dictionnary of Fluids  and Thermodynamic Models (old : method)
        giving list of Equations of Fluid (old : references) :
        D[fluid][T.M.] = [list E.S.]"""
        
        # first dictionary level
        d = {}
        fname = self.eosdatapath + "/index.eos"
        f = open(fname,"r") 
        all_lines = f.readlines() 
        for line in all_lines:
           words = line.split()
           if len(words) > 0:
              ivu = 0
              for a,b in d.items():
                 if a == words[3]:
                    d[a].append([words[0],words[1]])
                    ivu = 1
                    break
              if ivu == 0:
                 d[words[3]] = [[words[0],words[1]]]

        # second dictionary level
        d2 = {}
        for a,b in d.items():
           dd = {}
           for ll in b:
              ivu = 0
              for aa,bb in dd.items():
                 if ll[0] == aa:
                    dd[aa].append(ll[1])
                    ivu = 1
                    break
              if ivu == 0:
                 dd[ll[0]] = [ll[1]]
           d2[a] = dd
        return d2


    def buildPropDict(self):
        """This method builds a dictionnary of the available thermodynamic
        properties in reading the index.eos.properties file of the EOS
        component. The indexes are the full property names, the values
        are the short property names (the names used in the program)."""

        d ={}
        li = []
        fname = self.eosdatapath+"/index.eos.properties"
        f = open(fname,"r") 
        ll = f.readlines() 
        for l in ll:
            mm = l.split()
            if len(mm) > 0:
                d[mm[1]] = mm[0]
                li.append(mm[1])
        #for a,b in d.items():
            #print a,b
        #print li
        return (d,li)
