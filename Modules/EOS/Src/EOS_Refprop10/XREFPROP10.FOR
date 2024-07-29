c ======================================================================
c
      subroutine XSETUP (ncomp,xhFiles,hFmix,hrf,ierr,herr)
      include 'COMMONS.INC'
      include 'COMFEQ.INC'

      integer ncomp
      character*(*) xhFiles
      character*255 hFmix
      character*3 hrf
      integer ierr
      character*255 herr
      character*255 hFiles(ncmax)

      integer i
      do i=1,ncomp
        hFiles(i) = xhFiles(1 + (i-1)*255:i*255)
      enddo
      call SETUP (ncomp,hfiles,hFmix,hrf,ierr,herr)

      end subroutine XSETUP
c
c
c ======================================================================
c
      subroutine XSETMOD (ncomp,htype,hmix,xhcomp,ierr,herr)
      include 'COMMONS.INC'
      character*3 htype,hmix, hcomp(1:ncmax)
      character*(*) xhcomp
      character*255 herr

      do i=1,ncomp
        hcomp(i) = xhcomp(1 + (i-1)*3:i*3)
      enddo
      call SETMOD(ncomp,htype,hmix,hcomp,ierr,herr)

      end subroutine XSETMOD
c
c
c ======================================================================
c
      subroutine XSETMIX (hMixNme,hFmix,hrf,ncc,xhFiles,z,ierr,herr)
      include 'COMMONS.INC'
      character hrf*3
      character *(*) xhFiles
      character*255 hMixNme,hFmix,herr,hFiles(ncmax)
      dimension z(ncmax)

      call SETMIX (hMixNme,hFmix,hrf,ncc,hFiles,z,ierr,herr)

      do i=1,nc
        xhFiles(1 + (i-1)*255:i*255) = hFiles(i)
      enddo

      end subroutine XSETMIX
