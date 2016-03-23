c     Fortran include file storing common blocks and associated
c     parameters.
c
c     Changelog:
c       2015-04-16     Created (Joseph Nicholas)

      parameter (MAXFILES_SHC = 14)
      parameter (MAXRECORDS_SHC = 200)
c     parameter (MAXRECORDLEN_IGRZ = 722)
      parameter (MAXRECORDLEN_IGRZ = 2000)
      parameter (MAXRECORDS_AP = 40000)
      parameter (MAXFILES_CCIR = 12)
      parameter (MAXFILES_URSI = 12)

      character*12 FILMOD

      common /shc1/  nmax_shc(MAXFILES_SHC), erad_shc(MAXFILES_SHC),
     &               shcfiles(4, MAXRECORDS_SHC, MAXFILES_SHC)
      common /shc2/  FILMOD(MAXFILES_SHC)
      common /shc3/  DTEMOD(MAXFILES_SHC)


      real ionoindx_igrz, indrz_igrz
      common /igrz1/ iupd_igrz, iupm_igrz, iupy_igrz,
     &               imst_igrz, iyst_igrz, imend_igrz, iyend_igrz,
     &               ionoindx_igrz(MAXRECORDLEN_IGRZ),
     &               indrz_igrz(MAXRECORDLEN_IGRZ)

      common /ap/    num_records_ap,
     &               ints_ap(11, MAXRECORDS_AP),
     &               reals_ap(MAXRECORDS_AP)

      common /ccir/  f2_ccir(13, 76, 2, MAXFILES_CCIR),
     &               f3_ccir(9, 49, 2, MAXFILES_CCIR)

      common /ursi/  f2_ursi(13, 76, 2, MAXFILES_URSI)
