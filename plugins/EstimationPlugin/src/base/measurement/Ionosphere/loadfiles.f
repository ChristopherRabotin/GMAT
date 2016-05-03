c     Loads all files into memory so they don't have to be reread on
c     every routine call.
c
c     Changelog:
c       2015-04-16     Created (Joseph Nicholas)
c       2015-04-27     Bugfix in load_ap (Joseph Nicholas)

      block data common_data
          include 'common.inc'
          data FILMOD /'dgrf45.dat', 'dgrf50.dat', 'dgrf55.dat',
     &                 'dgrf60.dat', 'dgrf65.dat', 'dgrf70.dat',
     &                 'dgrf75.dat', 'dgrf80.dat', 'dgrf85.dat',
     &                 'dgrf90.dat', 'dgrf95.dat', 'dgrf00.dat',
     &                 'igrf05.dat', 'igrf05s.dat'/
          data DTEMOD / 1945., 1950., 1955., 1960., 1965., 1970.,
     &                  1975., 1980., 1985., 1990., 1995., 2000.,
     &                  2005., 2010./
      end

      subroutine load_shc(ifile, filename, ierror, errmsg)
          character filename*(*)
          include 'common.inc'

          integer   ierror
          character*256 errmsg

          parameter (iunit = 84)
          character*256 fullpath

c         Special for IRIWeb version
c1001     format('/usr/local/etc/httpd/cgi-bin/models/IRI/',A12)
c1001     format(A12)
c         For GMAT version
 1001     format('./../data/IonosphereData/',A12)

          write(fullpath, 1001) filename

          ierror = 0
          if (ifile > MAXFILES_SHC) then
c              write(*,*) 'ERROR: load_shc: ifile exceeds MAXFILES_SHC'
c              stop 1
             ierror = 1
             errmsg = 'ERROR: load_shc: ifile exceeds MAXFILES_SHC'
             return
          end if

c         Open coefficient file. Read past first header record.        
          open(iunit, file=fullpath, status='old', err = 2001)

c         Read degree and order of model and Earth's radius.           
          read(iunit, *)
          read(iunit, *) nmax_shc(ifile), erad_shc(ifile), yy
c          print * ,'Read ', fullpath
c          print * , nmax_shc(ifile), '  ', erad_shc(ifile), '  ', yy

c         --------------------------------------------------------------
c         Read the coefficient file, arranged as follows:              
c                                                                                 
c                                         N     M     G     H          
c                                         ----------------------       
c                                     /   1     0    GH(1)  -          
c                                    /    1     1    GH(2) GH(3)       
c                                   /     2     0    GH(4)  -          
c                                  /      2     1    GH(5) GH(6)       
c             NMAX*(NMAX+3)/2     /       2     2    GH(7) GH(8)       
c                records          \       3     0    GH(9)  -          
c                                  \      .     .     .     .          
c                                   \     .     .     .     .          
c             NMAX*(NMAX+2)          \    .     .     .     .          
c             elements in GH          \  NMAX  NMAX   .     .          
c                                                                                 
c         N and M are, respectively, the degree and order of the       
c         coefficient.                                                 
c         --------------------------------------------------------------
          irec = 0
          do nn = 1, nmax_shc(ifile)
              do mm = 0, nn
                  irec = irec + 1

                  if (irec > MAXRECORDS_SHC) then
c                      write(*,*)
c     &                    'ERROR: load_shc: irec exceeds MAXRECORDS_SHC'
c                      stop 1
                     ierror = 2
                errmsg = 'ERROR: load_shc: irec exceeds MAXRECORDS_SHC'
                return 
                  end if

                  read(iunit, *) (shcfiles(i, irec, ifile), i=1,4)
c                  print *, shcfiles(1, irec, ifile), '  ', 
c     & shcfiles(2, irec, ifile), '  ',
c     & shcfiles(3, irec, ifile), '  ',
c     & shcfiles(4, irec, ifile)
              end do
          end do
      
          close(iunit)
          errmsg = ''
          return
          
 2001     ierror = 1000 + ierror
          errmsg = 'Error: can not open "'//fullpath
          errmsg = errmsg//'"'
          return
      end

      subroutine load_igrz(filename, ierror, errmsg)
          character filename*(*)
          include 'common.inc'

          integer   ierror
          character*256 errmsg

          parameter (iunit = 84)
          character*256 fullpath

c         Special for IRIWeb version
c1001     format('/usr/local/etc/httpd/cgi-bin/models/IRI/',A)
c1001     format(A)
c         For GMAT version
 1001     format('./../data/IonosphereData/',A)

          write(fullpath, 1001) filename

          ierror = 0
          errmsg = ''
          open(iunit, file=fullpath, status='old', err = 3001)

c         Read the update date, the start date and the end date
c         (mm,yyyy), and get number of data points to read.
          read(iunit, *) iupd_igrz, iupm_igrz, iupy_igrz
          read(iunit, *) imst_igrz, iyst_igrz, imend_igrz, iyend_igrz
          inum_vals = 3 - imst_igrz + (iyend_igrz-iyst_igrz)*12
     &                + imend_igrz

          if (inum_vals > MAXRECORDLEN_IGRZ) then
c              write(*,*)
c     &           'ERROR: load_igrz: inum_vals exceeds MAXRECORDLEN_IGRZ'
c              stop 1
        ierror = 3
        errmsg = 'ERROR: load_igrz: inum_vals exceeds MAXRECORDLEN_IGRZ'
        return
          end if

c         Read data records
          read(iunit, *) (ionoindx_igrz(i), i=1,inum_vals)
          read(iunit, *) (indrz_igrz(i), i=1,inum_vals)

          close(iunit)
          return

 3001     ierror = 1000 + ierror
          errmsg = 'Error: can not open "'//fullpath
          errmsg = errmsg//'"'
          return
      end

      subroutine load_ap(filename, ierror, errmsg)
          character filename*(*)
          include 'common.inc'

          integer   ierror
          character*256 errmsg

          parameter (iunit = 84)
          character*256 fullpath

c         Special for IRIWeb version
c1001     format('/usr/local/etc/httpd/cgi-bin/models/IRI/',A)
c1001     format(A)
c         For GMAT version
 1001     format('./../data/IonosphereData/',A)

          write(fullpath, 1001) filename

          ierror = 0
          errmsg = ''
          open(iunit, file=fullpath, status='old', err = 4001)

          do irec = 1, MAXRECORDS_AP+1
            if (irec > MAXRECORDS_AP) then
c                write(*,*) 'ERROR: load_ap: irec exceeds MAXRECORDS_AP'
c                stop 1
               ierror = 4
               errmsg = 'ERROR: load_ap: irec exceeds MAXRECORDS_AP'
            end if

            read(iunit, 1002, end=2001) (ints_ap(k,irec), k=1,11), 
     &                                  reals_ap(irec)
 1002       format(3I3, 8I3, F5.1)
          end do

 2001     close(iunit)
          num_records_ap = irec - 1
          return

 4001     ierror = 1000 + ierror
          errmsg = 'Error: can not open "'//fullpath
          errmsg = errmsg//'"'
          return
      end

      subroutine load_ccir(imonth, ierror, errmsg)
          include 'common.inc'

          integer   ierror
          character*256 errmsg

          parameter (iunit = 84)
          character*256 fullpath

c         Special for IRIWeb version
c1001     format('/usr/local/etc/httpd/cgi-bin/models/IRI/ccir',
c    &           I2,'.asc')
c1001     format('ccir',I2,'.asc')
 1001     format('./../data/IonosphereData/ccir',I2,'.asc')

          write(fullpath, 1001) imonth+10

          ierror = 0
          errmsg = ''
          if (imonth > MAXFILES_CCIR) then
c              write(*,*)
c     &                  'ERROR: load_ccir: imonth exceeds MAXFILES_CCIR'
c              stop 1
             ierror = 5
             errmsg = 'ERROR: load_ccir: imonth exceeds MAXFILES_CCIR' 
          end if

          open(iunit, file=fullpath, status='old', err = 5001)

          read(iunit, 1002)
     &               (((f2_ccir(i,j,k,imonth), i=1,13), j=1,76), k=1,2),
     &               (((f3_ccir(i,j,k,imonth), i=1, 9), j=1,49), k=1,2)
 1002     format(1X, 4E15.8)

          close(iunit)
          return

 5001     ierror = 1000 + ierror
          errmsg = 'Error: can not open "'//fullpath
          errmsg = errmsg//'"'
          return
      end

      subroutine load_ursi(imonth, ierror, errmsg)
          include 'common.inc'

          integer   ierror
          character*256 errmsg

          parameter (iunit = 84)
          character*256 fullpath

c         Special for IRIWeb version
c1001     format('/usr/local/etc/httpd/cgi-bin/models/IRI/ursi',
c    &           I2,'.asc')
c1001     format('ursi',I2,'.asc')
 1001     format('./../data/IonosphereData/ursi',I2,'.asc')

          write(fullpath, 1001) imonth+10

          ierror = 0
          errmsg = ''
          if (imonth > MAXFILES_URSI) then
c              write(*,*)
c     &                  'ERROR: load_ursi: imonth exceeds MAXFILES_URSI'
c              stop 1
             ierror = 6
             errmsg = 'ERROR: load_ursi: imonth exceeds MAXFILES_URSI' 
          end if

          open(iunit, file=fullpath, status='old', err = 6001)

          read(iunit, 1002)
     &               (((f2_ursi(i,j,k,imonth), i=1,13), j=1,76), k=1,2)
 1002     format(1X, 4E15.8)

          close(iunit)
          return

 6001     ierror = 1000 + ierror
          errmsg = 'Error: can not open "'//fullpath
          errmsg = errmsg//'"'
          return
      end

      subroutine load_all_files(ierror, errmsg)
          include 'common.inc'

          integer   ierror
          character*256 errmsg

          do i = 1, MAXFILES_SHC
              call load_shc(i, FILMOD(i), ierror, errmsg)
              if (ierror .ne. 0) then
                  return
              end if
          end do

          call load_igrz('ig_rz.dat', ierror, errmsg)
          if (ierror .ne. 0) then
             return
          end if

          call load_ap('ap.dat', ierror, errmsg)
          if (ierror .ne. 0) then
             return
          end if

          do i = 1, MAXFILES_CCIR
              call load_ccir(i, ierror, errmsg)
              if (ierror .ne. 0) then
                 return
              end if
          end do

          do i = 1, MAXFILES_URSI
              call load_ursi(i, ierror, errmsg)
              if (ierror .ne. 0) then
                 return
              end if
          end do
      end
