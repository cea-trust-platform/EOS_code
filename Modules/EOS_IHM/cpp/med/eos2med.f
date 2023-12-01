C ****************************************************************************
C Copyright (c) 2023, CEA
C All rights reserved.
C
C Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
C 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
C 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
C 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
C
C THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
C IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
C OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
C
C ****************************************************************************/

      subroutine eos2med (indicex,indicey,valchp,nindx,nindy,
     .                    fluid,method,ref,func,laxex,laxey,medfile)

*  Build cartesian structured MED Mesh on two axes and one scalar field
*
*
      implicit none
      include 'med.hf'
      parameter(med_equivalence=0, med_group=0, med_joint=0) ! correction

*--arguments
      integer       nindx,nindy                          ! axes sizes
      real*8        indicex(nindx),indicey(nindy)        ! coordinates by axes
      real*8        valchp(nindx*nindy)                  ! values of scalar field 
      character*(*) func                                 ! field name
      character*(*) fluid,method,ref,laxex,laxey,medfile ! names
*
      integer       nbval,i
      integer       cret,fid                             ! return code, file logical number
      integer       mdim,sdim                            ! mesh and space size
      character*64  maa                                  ! mesh name
      character*200 desc                                 ! mesh description
      character*16  nomcoo(2)                            ! axes names
      integer       ncomp                                ! number of components in field : one for scalar field
      character*64  fname                                ! field name
      character*16  cname,cunit                          ! names and units for component filed
      real*8        xmin,xmax,ymin,ymax,pond

* Data
      character*16  unicoo(2)   ! units with axes
      data  unicoo /"SI","SI"/      
      character*16  dtunit      ! unit for time step for calculation sequence
      parameter    (dtunit="SI              ")
*
      real*8        MED_UNDEF_DT_USER
      parameter    (MED_UNDEF_DT_USER=0.d0)
      integer       iaff        ! debug print : no:0 yes:>0
      parameter    (iaff=0)


      if (iaff.ne.0) then
         write(*,*) "fortran writing med file (begin) : ",medfile
         write(*,*) "  fluid  = ",fluid
         write(*,*) "  method = ",method
         write(*,*) "  ref    = ",ref
         write(*,*) "  func   = ",func
         write(*,*) "  laxex  = ",laxex
         write(*,*) "  laxey  = ",laxey
      endif

*
*     Data Normalization on axes X, Y
*     -------------------------------
*     ! temporary action (cf. MED)
      if (nindx.ge.2 .and. nindy.ge.2) then
         if (iaff.ne.0) then
            write(*,*) "  X and Y composants are normalized, to improve visualisation"
            write(*,*) "  val X       = ", indicex
            write(*,*) "  val Y       = ", indicey
         endif

         xmin = indicex(1)
         xmax = indicex(nindx)
         if (xmin .eq. xmax) then
            write(*,*) "  ERROR normalisation X  : xmin = xmax"
            call efexit(-1)
         endif
         do i=1,nindx
            indicex(i) = (indicex(i)-xmin)/(xmax-xmin)
         enddo
         
         ymin = indicey(1)
         ymax = indicey(nindy)
         if (ymin .eq. ymax) then
            write(*,*) "  ERROR normalisation Y : ymin = ymax"
            call efexit(-1)
         endif
         
         pond = float(nindy-1)/float(nindx-1)
         do i=1,nindy
            indicey(i) = (indicey(i)-ymin)/(ymax-ymin)*pond
         enddo
         
         if (iaff.ne.0) then
            write(*,*) "  normalized values X = ", indicex
            write(*,*) "  normalized values Y = ", indicey
         endif
      endif
     
*
*     Open MED Mesh file
*     ------------------
*     fid : numéro logique du fichier MED
      if (iaff.ne.0) then
         write(*,*) "  file creation"
         write(*,*) "  name = ",medfile
      endif
      call mfiope(fid,medfile,MED_ACC_CREAT,cret)
      if (iaff.ne.0) then
         write(*,*) "  fid  = ",fid
      endif
      if (cret.ne.0) then
         write(*,*) "  ERROR : file creation"
         write(*,1001) cret
         call efexit(-1)
      endif

*
*     MED Mesh Definition
*     -------------------
*     Initialisation
      mdim = 2
      sdim = 2
      maa  = 'eos cartesian grid'
      desc = 'eos mesh for '//func//' of '//fluid//' '//method
     &     //' '//ref//' f('//laxex//','//laxey//')'
      nomcoo(1) = laxex
      nomcoo(2) = laxey
      if (iaff.ne.0) then
         write(*,*) "  mesh creation description=",desc
      endif
      call mmhcre(fid,maa,mdim,sdim,MED_STRUCTURED_MESH,
     &     desc,'',MED_SORT_DTIT,MED_CARTESIAN,
     &     nomcoo,unicoo,cret)
      if (cret.ne.0) then
         write(*,*) "  ERROR : mesh creation"
         write(*,1001) cret
         call efexit(-1)
      endif
*
*     Définition of structured mesh type
      if (iaff.ne.0) then
         write(*,*) "  writing grid type"
      endif
      call mmhgtw(fid,maa,MED_CARTESIAN_GRID,cret)
      if (cret.ne.0) then
         write(*,*) "  ERROR : writing grid type"
         write(*,1001) cret
         call efexit(-1)
      endif 

*
*     Write axes coordinates
*     ----------------------
*     Axe X    
      if (iaff.ne.0) then
         write(*,*) "  writing axe X"
         write(*,*) "  indice X=",nindx
         write(*,*) "  val    X=",indicex     
      endif
      call mmhgcw(fid,maa,MED_NO_DT,MED_NO_IT,MED_UNDEF_DT_USER, 
     &            1,nindx,indicex,cret)
      if (cret.ne.0) then
         write(*,*) "  ERROR : writing axe X"
         write(*,1001) cret
         call efexit(-1)
      endif      
*
*     Axe Y
      if (iaff.ne.0) then
         write(*,*) "  writing axe Y"
         write(*,*) "  indice Y=",nindy
         write(*,*) "  val    Y=",indicey     
      endif
      call mmhgcw(fid,maa,MED_NO_DT,MED_NO_IT,MED_UNDEF_DT_USER, 
     &            2,nindy,indicey,cret)
      if (cret.ne.0) then
         write(*,*) "  ERROR : writing axe Y"
         write(*,1001) cret
         call efexit(-1)
      endif      

*
*     Floats scalar field definition
*     ------------------------------
*     Buildind
      ncomp=1
      fname=func
      cname=func
      cunit=unicoo(2)
      if (iaff.ne.0) then
         write(*,*) "  create field ",fname
      endif      
      call mfdcre(fid,fname,MED_FLOAT64,ncomp,cname,
     &            cunit,dtunit,maa,cret)
      if (cret.ne.0) then
         write(*,*) "  ERROR : create field",fname
         write(*,1001) cret
         call efexit(-1)
      endif
*
*     Writing
      if (iaff.ne.0) then
         write(*,*) "  write field values"
         write(*,*) "  val=",valchp
      endif     
      nbval = nindx*nindy      
      call mfdrvw(fid,fname,MED_NO_DT,MED_NO_IT,0.,MED_NODE,
     &            MED_NONE,MED_FULL_INTERLACE,MED_ALL_CONSTITUENT,
     &            nbval,valchp,cret)
      if (cret.ne.0) then
         write(*,*) "  ERROR : write field values on vertices"
         write(*,1001) cret
         call efexit(-1)
      endif


*     Close MED Mesh file
*     -------------------
      if (iaff.ne.0) then
         write(*,*) "  close file"
      endif            
      call mficlo(fid,cret)
      if (cret.ne.0) then
         write(*,*) "  ERROR : closing file"
         write(*,1001) cret
         call efexit(-1)
      endif

      if (iaff.ne.0) then
         write(*,*) "fortran writing med file (end) : ",medfile
      endif

 1001 format("(cret=",I6,")")
      return
      end
