      subroutine XSETUP_RP10 (ncomp,xhFiles,hFmix,hrf,ierr,herr)
      include 'RP10_COMMONS.INC'

      integer ncomp
c 5100 = 255*20
      character*5100 xhFiles
      character*255 hFmix
      character*3 hrf
      integer ierr
      character*255 herr
      character*255 hFiles(ncmax)

      integer i
      hFiles(1) = xhFiles(1:255)
      do i=1,ncomp
        hFiles(i) = xhFiles(1 + (i-1)*255:i*255)
      enddo
      call SETUP_RP10 (ncomp,hfiles,hFmix,hrf,ierr,herr)

      end subroutine XSETUP_RP10
c
c
c ======================================================================
c
      subroutine XSETMOD_RP10 (ncomp,htype,hmix,xhcomp,ierr,herr)
      include 'RP10_COMMONS.INC'
      character*3 htype,hmix, hcomp(1:ncmax)
c     80 = 20*4
      character*80 xhcomp
      character*255 herr

      do i=1,ncomp
        hcomp(i) = xhcomp(1 + (i-1)*3:i*3)
      enddo
      call SETMOD_RP10(ncomp,htype,hmix,hcomp,ierr,herr)

      end subroutine XSETMOD_RP10
c
c
c ======================================================================
c
      subroutine XSETMIX_RP10 (hMixNme,hFmix,hrf,ncc,xhFiles,z,ierr,herr)
      include 'RP10_COMMONS.INC'
      character hrf*3
c     5120 = 20*256
      character *5120 xhFiles
      character*255 hMixNme,hFmix,herr,hFiles(ncmax)
      dimension z(ncmax)

      call SETMIX_RP10 (hMixNme,hFmix,hrf,ncc,hFiles,z,ierr,herr)

      do i=1,nc
        xhFiles(1 + (i-1)*255:i*255) = hFiles(i)
      enddo

      end subroutine XSETMIX_RP10

