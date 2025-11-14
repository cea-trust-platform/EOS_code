#! /usr/bin/env python

import os, sys, tarfile, shutil, tempfile, re, timeit, subprocess, argparse, glob

parser = argparse.ArgumentParser()
parser.add_argument('src')
parser.add_argument('work')
parser.add_argument('dest')
parser.add_argument('openmp', type=int)
parser.add_argument('--verbose', action='store_true')
parser.add_argument('--partial', action='store_true')

args = parser.parse_args()

def UpperLine(lIn):
    if lIn[0] in 'Cc!':
        return ''
    inString = False
    lOut = ''
    for c in lIn:
        if c == "'":
            inString = not inString
        if inString:
            lOut += c
        else:
            if c == '!':
                lOut += '\n'
                break
            lOut += c.upper()
    return lOut

def Symbol(l):
    m = re.match(r" *BLOCK *DATA *([A-Z0-9_]*)", l)
    if m:
        return m.group(1)
    m = re.match(r" *SUBROUTINE *([A-Z0-9_]*)", l)
    if m:
        return m.group(1)
    m = re.match(r" *FUNCTION *([A-Z0-9_]*)", l)
    if m:
        return m.group(1)
    m = re.match(r" *COMMON */ *([A-Z0-9_]*)", l)
    if m:
        return m.group(1)
    return None
    
class Steps:
    
    def __init__(self, srcDir, workDir, destDir, prefix, suffix, partial, verbose):
        
        t0 = timeit.default_timer()
        
        self.prefix = prefix
        self.suffix = suffix
        self.workDir = workDir
        self.destDir = destDir
        
        self.partial = partial
            
        self.verbose = verbose
        self.currentStep = 0
        if self.verbose:
            self.fDebug = open("debug.log", "w")
        else:
            self.fDebug = None
            
        currentDir = os.path.join(self.workDir, "step_" + str(self.currentStep))
        if not os.path.exists(currentDir):
            os.makedirs(currentDir)
        
        for fSrcName in glob.glob(os.path.join(srcDir, '*.FOR')) \
                        + glob.glob(os.path.join(srcDir, '*.INC')):
            if fSrcName.endswith('COMMONS.FOR') or fSrcName.endswith('COMTRN.FOR'):
                fDestName = os.path.join(currentDir, prefix + os.path.basename(fSrcName)[:-4] + '.INC')
            else:
                fDestName = os.path.join(currentDir, prefix + os.path.basename(fSrcName))
            shutil.copyfile(fSrcName, fDestName)
                    
        t1 = timeit.default_timer()
        print('initialize     :   {:8.3f} s'.format(t1-t0))
            
    def upper(self):
        t0 = timeit.default_timer()
        
        previousStep = self.currentStep
        self.currentStep += 1
        
        currentDir = os.path.join(self.workDir, "step_" + str(self.currentStep))
        previousDir = os.path.join(self.workDir, "step_" + str(previousStep))
        
        if not os.path.exists(currentDir):
            os.makedirs(currentDir)
        
        for (root,dirs,files) in os.walk(previousDir,topdown=True):
            for fName in files:
                fSrcName = os.path.join(root, fName)
                fDestName = os.path.join(currentDir, os.path.basename(fSrcName))
        
                with open(fDestName, "w") as fDest:
                    with open(fSrcName, "r") as fSrc:
                        for l in fSrc:
                            l = UpperLine(l)
                            if l.startswith('      INCLUDE'):
                                l = l.upper()
                                l = l.replace('.FOR', '.INC')
                                l = l.replace("INCLUDE '", "INCLUDE '" + self.prefix)
                            fDest.write(l)
                            
        shutil.rmtree(previousDir)
        t1 = timeit.default_timer()
        print('upper          :   {:8.3f} s'.format(t1-t0))
    
    def handleCommons(self):
        """
        COMMON's are marked thread private
        Additional variables added at begin and end of COMMON's (named START_XXX and END_XXX
        to help compute the size of COMMON's')
        """
        t0 = timeit.default_timer()
        
        previousStep = self.currentStep
        self.currentStep += 1
        
        currentDir = os.path.join(self.workDir, "step_" + str(self.currentStep))
        previousDir = os.path.join(self.workDir, "step_" + str(previousStep))
        
        if not os.path.exists(currentDir):
            os.makedirs(currentDir)
                
        currentCommon = False
        currentCommonName = ''
        startCommon = ""
        endCommon = ""
    
        listCommons = set()
        incCommonFiles = set()
    
        for (root,dirs,files) in os.walk(previousDir,topdown=True):
            for fName in files:
                fSrcName = os.path.join(root, fName)
                fDestName = os.path.join(currentDir, os.path.basename(fSrcName))
                with open(fDestName, "w") as fDest:
                    with open(fSrcName, "r") as fSrc:
                        for lSrc in fSrc:
                            lDest = lSrc
                            if currentCommon:
                                if (not lDest[0] in ' 0123456789') or lDest.startswith('      '):
                                    currentCommon = False
                                    lSupp = '     & ,' + endCommon + '\n' + \
                                            'c$omp threadprivate(/' + currentCommonName + '/)\n\n'
                                    fDest.write(lSupp)
                            if lDest[0] == ' ':
                                m = re.match(r" *COMMON */ *([A-Z0-9_]*) *(/)", lDest)
                                if m:
                                    currentCommon = True
                                    currentCommonName = m.group(1)
                                    startCommon = ' START_' + currentCommonName
                                    endCommon = ' END_' + currentCommonName

                                    fDest.write('      double precision' + startCommon + '\n')
                                    fDest.write('      character*1' + endCommon + '\n')
                                    sp = m.span(2)
                                    lDest = lDest[:sp[1]] + startCommon + ',\n     & ' + lDest[sp[1]:]
                                    if fName.endswith('INC'):
                                        listCommons.add(currentCommonName)
                                        incCommonFiles.add('      INCLUDE \'' + fName + '\'\n')
                            elif lDest.find("!$omp threadprivate") == 0:
                                continue
                            fDest.write(lDest)
                    if currentCommon:
                        currentCommon = False
                        lSupp = '     & , ' + endCommon + '\n' + \
                                'c$omp threadprivate(/' + currentCommonName + '/)\n\n'
                        fDest.write(lSupp)
    
        s = ''
        for inc in incCommonFiles:
            if 'COMMONS' in inc:
                s = s + inc
        for inc in incCommonFiles:
            if not 'COMMONS' in inc:
                s = s + inc
                
        shutil.rmtree(previousDir)
        
        t1 = timeit.default_timer()
        print('handle commons :   {:8.3f} s'.format(t1-t0))
        return (listCommons, s)
                
    def listSymbols(self):
          
        t0 = timeit.default_timer()
               
        currentDir = os.path.join(self.workDir, "step_" + str(self.currentStep))
                        
        self.symbs = set()
        for (root,dirs,files) in os.walk(currentDir, topdown=True):
            for fName in files:
                with open(os.path.join(root, fName), "r") as fSrc:
                    for lSrc in fSrc:
                        s = Symbol(lSrc)
                        if s:
                            self.symbs.add(s)
                            if self.verbose:
                                print("listSymbol (", fName, ")", s, file=self.fDebug)                            
        t1 = timeit.default_timer()
        print('list symbols   :   {:8.3f} s'.format(t1-t0))

    def addsuffix(self):
        
        t0 = timeit.default_timer()
        
        self.listSymbols()
        
        previousStep = self.currentStep
        self.currentStep += 1
        
        currentDir = os.path.join(self.workDir, "step_" + str(self.currentStep))
        previousDir = os.path.join(self.workDir, "step_" + str(previousStep))
        
        if not os.path.exists(currentDir):
            os.makedirs(currentDir)
                 
        for (root,dirs,files) in os.walk(previousDir,topdown=True):
            for fName in files:
                fSrcName = os.path.join(root, fName)
                fDestName = os.path.join(currentDir, fName)
                s = ''
                with open(fSrcName, "r") as fSrc:
                    s = fSrc.read()
                    for symb in self.symbs:
                        s = re.sub(r"\b" + symb + r"\b", symb + suffix, s)

                t = s.split("'")
                u = ''
                for i in range(len(t)):
                    r = t[i]
                    if i % 2:
                        r = r.replace(suffix, '') 
                    u += r
                    if i < len(t)-1:
                        u += '\''
                s = u
                with open(fDestName, "w") as fDest:
                    fDest.write(s + "\n")
                    
        shutil.rmtree(previousDir)
        
        t1 = timeit.default_timer()
        print('add suffix     :   {:8.3f} s'.format(t1-t0))

    def save(self):    
        currentDir = os.path.join(self.workDir, "step_" + str(self.currentStep))
        for f in os.listdir(currentDir):
            shutil.copy2(os.path.join(currentDir, f), self.destDir)
        shutil.rmtree(currentDir)

t_start = timeit.default_timer()

prefix='RP9_'
suffix='_RP9'

C = Steps(args.src, args.work, args.dest, prefix, suffix, args.partial, True)
C.upper()
(listCommons, incCommons) = C.handleCommons()
C.addsuffix()
C.save()

if not C.partial:
 
    with open('EOS_Refprop9.patch') as fIn:
        subprocess.run(['patch', '-p1'], stdin=fIn, check=True)

    if args.openmp == 0:
        s = """

      integer function THREAD_NUM_RP9()
      THREAD_NUM_RP9 = 0
      end function THREAD_NUM_RP9

"""
    else:
        s = """

      integer function THREAD_NUM_RP9()
      integer omp_get_thread_num
      THREAD_NUM_RP9 = omp_get_thread_num()
      end function THREAD_NUM_RP9

"""

    s = s + """
      subroutine XNUMCOMMONS_RP9(N)
      integer N
      N = {N}
      end subroutine XNUMCOMMONS_RP9

      subroutine XDEFCOMMONS_RP9(Q0, L, N, NTH)
{INC}
      integer N, NTH
      integer *8 L(N), Q0(N*NTH)
      integer THREAD_NUM_RP9
      
""".format(INC=incCommons, N=len(listCommons))

    s+="\nc$omp parallel shared(Q0, L)\n\n"
    iC = 0
    for sC in listCommons:
        s += "      Q0(THREAD_NUM_RP9()+1 + NTH*{iC}) = LOC(START_{sC})\n".format(iC=iC, sC=sC)
        iC += 1
    s+="\nc$omp end parallel\n\n"

    iC = 0
    for sC in listCommons:
        s += "      L({iC1}) = LOC(END_{sC}) - Q0(1 + NTH*{iC})\n".format(iC=iC, iC1=iC+1, sC=sC)
        iC += 1

    s += "\n      end subroutine XDEFCOMMONS_RP9\n"

    with open("RP9_XCOMMONS.FOR", "w") as f:
        f.write(s)
    
t_end = timeit.default_timer()
print('total {:8.3f} s'.format(t_end - t_start))
