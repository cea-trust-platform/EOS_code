c ======================================================================
c
      subroutine XSETMOD_RP9 (ncomp,htype,hmix,xhcomp,ierr,herr)
      include 'COMMONS.INC'
      character*3 htype,hmix, hcomp(1:ncmax)
      character*(*) xhcomp
      character*255 herr

      do i=1,ncomp
        hcomp(i) = xhcomp(1 + (i-1)*3:i*3)
      enddo
      call SETMOD_RP9(ncomp,htype,hmix,hcomp,ierr,herr)

      end subroutine XSETMOD_RP9
c
c
c ======================================================================
c
      subroutine XSETMIX_RP9 (hMixNme,hFmix,hrf,ncc,xhFiles,z,ierr,herr)
      include 'COMMONS.INC'
      character hrf*3
      character *(*) xhFiles
      character*255 hMixNme,hFmix,herr,hFiles(ncmax)
      dimension z(ncmax)

      call SETMIX_RP9 (hMixNme,hFmix,hrf,ncc,hFiles,z,ierr,herr)

      do i=1,nc
        xhFiles(1 + (i-1)*255:i*255) = hFiles(i)
      enddo

      end subroutine XSETMIX_RP9

