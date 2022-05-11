*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
*     newunit
*     snFileRead       snFileAppend
*     snFileOpenRead   snFileOpenAppend   snFileClose
*
*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

      integer function newunit()
      implicit
     &     none
!     ==================================================================
!     Find next available file unit number.
!     ==================================================================
      integer j, unitno, unitMin, unitMax
      logical opened

      parameter  (unitMin = 10)   ! Range of file units
      parameter  (unitMax = 1000)

      newunit = -1
      do j = unitMin, unitMax
         unitno = j
         inquire(unit=unitno, opened=opened)
         if (.not. opened) then
            newunit = unitno
            return
         end if
      end do

      end ! integer function newunit

*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

      subroutine snFileRead
     &     ( unitno, filename, iostat )
      implicit
     &     none
      integer
     &     unitno, iostat
      character*(*)
     &     filename
!     ==================================================================
!     Wrapper to assign a file unit number and open given file for
!     reading.
!
!     14 Apr 2018: Add openmp critical directives.
!     ==================================================================
      integer
     &     newunit
      external
     &     newunit

!$OMP parallel
!$OMP critical
      unitno = newunit()
      call snFileOpenRead( unitno, filename, iostat )
!$OMP end critical
!$OMP end parallel

      end ! subroutine snFileRead

*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

      subroutine snFileOpenRead
     &     ( unitno, filename, iostat )
      implicit
     &     none
      integer
     &     unitno, iostat
      character*(*)
     &     filename
!     ==================================================================
!     Open file (to read) on given unit number.
!
!     10 Jan 2017: First version.
!     ==================================================================
      integer  len

      call s1trim(filename,len)
      open(unit=unitno, file=filename(1:len),
     &     status='old', action='read',iostat=iostat,err=900)
      rewind(unitno)

 900  return

      end ! subroutine snFileOpenRead

!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

      subroutine snFileAppend
     &     ( unitno, filename, iostat )
      implicit
     &     none
      integer
     &     unitno, iostat
      character*(*)
     &     filename
!     ==================================================================
!     Wrapper assigns a file unit number and opens file for append
!     inside SNOPT library.
!
!     14 Apr 2018: Add openmp directives.
!     ==================================================================
      integer
     &     newunit
      external
     &     newunit

!$OMP parallel
!$OMP critical
      unitno = newunit()
      call snFileOpenAppend( unitno, filename, iostat )
!$OMP end critical
!$OMP end parallel

      end ! subroutine snFileAppend

!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

      subroutine snFileOpenAppend
     &     ( unitno, filename, iostat )
      implicit
     &     none
      integer
     &     unitno, iostat
      character*(*)
     &     filename
!     ==================================================================
!     Open file (to append) on given file unit number.
!
!     14 Apr 2018: Add openmp critical directives.
!     ==================================================================
      integer  len

      call s1trim(filename,len)
      open(unit=unitno, file=filename(1:len),
     &     status='unknown', position='append',err=900,iostat=iostat)

 900  return

      end ! subroutine snFileOpenAppend

!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

      subroutine snFileClose
     &     ( unitno )
      implicit
     &     none
      integer
     &     unitno
!     ==================================================================
!     Close the given unit file.
!
!     10 Jan 2017: First version.
!     ==================================================================

      close(unitno)

      end ! subroutine snFileClose

!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
