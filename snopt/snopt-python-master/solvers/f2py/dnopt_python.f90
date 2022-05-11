!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
! dnopt_python.f90
!
! To create sig file (*.pyf):
!     f2py dnopt_python.f90 -m dnopt_python -h dnopt_python.pyf
!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

subroutine copyOptions(option, Errors, cw, lencw, iw, leniw, rw, lenrw)
  implicit none
  character(*),     intent(in)    :: option
  integer,          intent(in)    :: lencw, leniw, lenrw
  character(8),     intent(inout) :: cw(lencw)
  integer,          intent(inout) :: iw(leniw)
  double precision, intent(inout) :: rw(lenrw)
  integer,          intent(out)   :: Errors

  !=============================================================================

  call dnSet(option, 0, 0, Errors, cw, lencw, iw, leniw, rw, lenrw)

end subroutine copyOptions

!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

subroutine dninit_wrap(prtfile, prtlen, summOn, cw, lencw, iw, leniw, rw, lenrw)
  implicit none
  character(*),     intent(in)    :: prtfile
  integer,          intent(in)    :: prtlen, summOn, lencw, leniw, lenrw
  character(8),     intent(inout) :: cw(lencw)
  integer,          intent(inout) :: iw(leniw)
  double precision, intent(inout) :: rw(lenrw)
  !=============================================================================
  !=============================================================================
  integer :: iPrint, iSumm

  if (prtlen > 0) then
     iPrint = 9
     call dnFileOpenAppend(iPrint, trim(prtfile))
  else
     iPrint = 0
  end if

  if (summOn > 0) then
     iSumm = 6
  else
     iSumm = 0
  end if

  call dnBegin(iPrint, iSumm, cw, lencw, iw, leniw, rw, lenrw)

end subroutine dninit_wrap

!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

subroutine dnspec_wrap(info, spcfile, cw, lencw, iw, leniw, rw, lenrw)
  implicit none
  character(*),     intent(in)    :: spcfile
  integer,          intent(in)    :: lencw, leniw, lenrw
  character(8),     intent(inout) :: cw(lencw)
  integer,          intent(inout) :: iw(leniw)
  double precision, intent(inout) :: rw(lenrw)
  integer,          intent(out)   :: info
  !=============================================================================
  !=============================================================================
  integer :: iSpecs

  iSpecs = 4

  call dnFileOpenRead(iSpecs, trim(spcfile))
  call dnSpec(iSpecs, info, cw, lencw, iw, leniw, rw, lenrw)
  close(iSpecs)

  if (info /= 101 .and. info /= 107) return

end subroutine dnspec_wrap

!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

subroutine dnmem_wrap(info, mLCon, mNCon, n, nnJac, nnObj, iObj, &
                      mincw, miniw, minrw,                       &
                      cw, lencw, iw, leniw, rw, lenrw)
  implicit none
  integer,          intent(in)    :: mLCon, mNCon, n, nnJac, nnObj, iObj, &
                                     lencw, leniw, lenrw
  character(8),     intent(inout) :: cw(lencw)
  integer,          intent(inout) :: iw(leniw)
  double precision, intent(inout) :: rw(lenrw)
  integer,          intent(out)   :: info, mincw, miniw, minrw
  !=============================================================================
  !=============================================================================

  call dnMem(info, mLCon, mNCon, n, nnJac, nnObj, iObj, &
             mincw, miniw, minrw, cw, lencw, iw, leniw, rw, lenrw)

end subroutine dnmem_wrap

!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

subroutine dnopt_uncon_wrap              &
    ( Start, n, nnJac, nnObj,            &
      Prob, Names, nNames, iObj, ObjAdd, &
      funobj,                            &
      state, bl, bu,                     &
      fObj, gObj, H, ldH,                &
      Obj, nInf, sInf, x, y,             &
      INFO, itn, mjritn,                 &
      mincw, miniw, minrw,               &
      cu, lencu, iu, leniu, ru, lenru,   &
      cw, lencw, iw, leniw, rw, lenrw)
  implicit none

  external                        :: funcon, funobj

  character(8),     intent(in)    :: Prob
  integer,          intent(in)    :: start, n, nnJac, nnObj, &
                                     nNames, iObj, ldH, &
                                     lencu, leniu, lenru, &
                                     lencw, leniw, lenrw
  double precision, intent(in)    :: ObjAdd, bl(n), bu(n)
  character(8),     intent(in)    :: Names(nNames)

  character(8),     intent(inout) :: cu(lencu), cw(lencw)
  integer,          intent(inout) :: state(n), iu(leniu), iw(leniw)
  double precision, intent(inout) :: H(ldH,*), x(n), y(n), &
                                     ru(lenru), rw(lenrw)

  integer,          intent(out)   :: info, itn, mjritn, nInf, &
                                     mincw, miniw, minrw
  double precision, intent(out)   :: sInf, Obj, fObj, gObj(n)

  !=============================================================================
  !=============================================================================
  integer          :: ldJ, ldA, mLCon, mNCon
  double precision :: A(1,n), Jcon(1,n), fCon(1)
  external         :: dummycon

  mLCon = 0
  mNCon = 0
  ldA   = 1
  ldJ   = 1

  call dnopt(Start, n, mLCon, mNCon, nnJac, nnObj, &
             Prob, Names, nNames, iObj, ObjAdd,    &
             dummycon, funobj,                     &
             state, A, ldA, bl, bu,                &
             fObj, gObj, fCon, Jcon, ldJ, H, ldH,  &
             Obj, nInf, sInf, x, y,                &
             INFO, mincw, miniw, minrw,            &
             cu, lencu, iu, leniu, ru, lenru,      &
             cw, lencw, iw, leniw, rw, lenrw)

  itn    = iw(421)
  mjritn = iw(422)

end subroutine dnopt_uncon_wrap

!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

subroutine dnopt_wrap(Start, n, mLCon, mNCon, nnJac, nnObj, &
                      Prob, Names, nNames, iObj, ObjAdd,    &
                      funcon, funobj,                       &
                      state, A, ldA, bl, bu,                &
                      fObj, gObj, fCon, Jcon, ldJ, H, ldH,  &
                      Obj, nInf, sInf, x, y,                &
                      INFO, itn, mjritn,                    &
                      mincw, miniw, minrw,                  &
                      cu, lencu, iu, leniu, ru, lenru,      &
                      cw, lencw, iw, leniw, rw, lenrw)
  implicit none

  external                        :: funcon, funobj

  character(8),     intent(in)    :: Prob
  integer,          intent(in)    :: start, n, mLCon, mNCon, nnJac, nnObj, &
                                     nNames, iObj, ldA, ldJ, ldH, &
                                     lencu, leniu, lenru, &
                                     lencw, leniw, lenrw
  double precision, intent(in)    :: ObjAdd, bl(n+mlcon+mncon), bu(n+mlcon+mncon)
  character(8),     intent(in)    :: Names(nNames)

  character(8),     intent(inout) :: cu(lencu), cw(lencw)
  integer,          intent(inout) :: state(n+mlcon+mncon), iu(leniu), iw(leniw)
  double precision, intent(inout) :: A(ldA,*), JCon(ldJ,*), H(ldH,*), &
                                     x(n+mlcon+mncon), y(n+mlcon+mncon), &
                                     ru(lenru), rw(lenrw)

  integer,          intent(out)   :: info, itn, mjritn, nInf, &
                                     mincw, miniw, minrw
  double precision, intent(out)   :: sInf, Obj, fObj, gObj(n), fCon(ldJ)

  !=============================================================================
  !=============================================================================

  call dnopt(Start, n, mLCon, mNCon, nnJac, nnObj, &
             Prob, Names, nNames, iObj, ObjAdd,    &
             funcon, funobj,                       &
             state, A, ldA, bl, bu,                &
             fObj, gObj, fCon, Jcon, ldJ, H, ldH,  &
             Obj, nInf, sInf, x, y,                &
             INFO, mincw, miniw, minrw,            &
             cu, lencu, iu, leniu, ru, lenru,      &
             cw, lencw, iw, leniw, rw, lenrw)

  itn    = iw(421)
  mjritn = iw(422)

end subroutine dnopt_wrap

!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

subroutine dnopth_wrap(Start, n, mLCon, mNCon, nnJac, nnObj, &
                       Prob, Names, nNames, iObj, ObjAdd,    &
                       funcon, funobj, funhes,               &
                       state, A, ldA, bl, bu,                &
                       fObj, gObj, fCon, Jcon, ldJ, H, ldH,  &
                       Obj, nInf, sInf, x, y,                &
                       INFO, itn, mjritn,                    &
                       mincw, miniw, minrw,                  &
                       cu, lencu, iu, leniu, ru, lenru,      &
                       cw, lencw, iw, leniw, rw, lenrw)
  implicit none

  external                        :: funcon, funobj, funhes

  character(8),     intent(in)    :: Prob
  integer,          intent(in)    :: start, n, mLCon, mNCon, nnJac, nnObj, &
                                     nNames, iObj, ldA, ldJ, ldH, &
                                     lencu, leniu, lenru, &
                                     lencw, leniw, lenrw
  double precision, intent(in)    :: ObjAdd, bl(n+mlcon+mncon), bu(n+mlcon+mncon)
  character(8),     intent(in)    :: Names(nNames)

  character(8),     intent(inout) :: cu(lencu), cw(lencw)
  integer,          intent(inout) :: state(n+mlcon+mncon), iu(leniu), iw(leniw)
  double precision, intent(inout) :: A(ldA,*), JCon(ldJ,*), H(ldH,*), &
                                     x(n+mlcon+mncon), y(n+mlcon+mncon), &
                                     ru(lenru), rw(lenrw)

  integer,          intent(out)   :: info, itn, mjritn, nInf, &
                                     mincw, miniw, minrw
  double precision, intent(out)   :: sInf, Obj, fObj, gObj(n), fCon(ldJ)

  !=============================================================================
  !=============================================================================

  call dnopth(Start, n, mLCon, mNCon, nnJac, nnObj, &
              Prob, Names, nNames, iObj, ObjAdd,    &
              funcon, funobj, funhes,               &
              state, A, ldA, bl, bu,                &
              fObj, gObj, fCon, Jcon, ldJ, H, ldH,  &
              Obj, nInf, sInf, x, y,                &
              INFO, mincw, miniw, minrw,            &
              cu, lencu, iu, leniu, ru, lenru,      &
              cw, lencw, iw, leniw, rw, lenrw)

  itn    = iw(421)
  mjritn = iw(422)

end subroutine dnopth_wrap

!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

subroutine dqinit_wrap(prtfile, prtlen, summOn, cw, lencw, iw, leniw, rw, lenrw)
  implicit none
  character(*),     intent(in)    :: prtfile
  integer,          intent(in)    :: prtlen, summOn, lencw, leniw, lenrw
  character(8),     intent(inout) :: cw(lencw)
  integer,          intent(inout) :: iw(leniw)
  double precision, intent(inout) :: rw(lenrw)
  !=============================================================================
  !=============================================================================
  integer :: iPrint, iSumm

  if (prtlen > 0) then
     iPrint = 9
     call dnFileOpenAppend(iPrint, trim(prtfile))
  else
     iPrint = 0
  end if

  if (summOn > 0) then
     iSumm = 6
  else
     iSumm = 0
  end if

  call dqBegin(iPrint, iSumm, cw, lencw, iw, leniw, rw, lenrw)

end subroutine dqinit_wrap

!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

subroutine dqspec_wrap(info, spcfile, cw, lencw, iw, leniw, rw, lenrw)
  implicit none
  character(*),     intent(in)    :: spcfile
  integer,          intent(in)    :: lencw, leniw, lenrw
  character(8),     intent(inout) :: cw(lencw)
  integer,          intent(inout) :: iw(leniw)
  double precision, intent(inout) :: rw(lenrw)
  integer,          intent(out)   :: info
  !=============================================================================
  !=============================================================================
  integer :: iSpecs

  iSpecs = 4

  call dnFileOpenRead(iSpecs, trim(spcfile))
  call dqSpec(iSpecs, info, cw, lencw, iw, leniw, rw, lenrw)
  close(iSpecs)

  if (info /= 101 .and. info /= 107) return

end subroutine dqspec_wrap

!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

subroutine dqopt_wrap(Start, n, mCon, nnH, Names, nNames, ncObj, &
                      iObj, ObjAdd, Prob,                        &
                      A, ldA, bl, bu, cObj, H, ldH, qpHx,        &
                      eType, state, x, y,                        &
                      INFO, itn,                                 &
                      mincw, miniw, minrw,                       &
                      Obj, nInf, sInf,                           &
                      cu, lencu, iu, leniu, ru, lenru,           &
                      cw, lencw, iw, leniw, rw, lenrw)
  implicit none

  external                        :: qpHx

  character(*),     intent(in)    :: Start
  character(8),     intent(in)    :: Prob
  integer,          intent(in)    :: n, mCon, nnH, nNames, ncObj, iObj, &
                                     ldA, ldH, eType(n+mcon), &
                                     lencu, leniu, lenru, &
                                     lencw, leniw, lenrw
  double precision, intent(in)    :: ObjAdd, cObj(ncObj), &
                                     bl(n+mcon), bu(n+mcon)
  character(8),     intent(in)    :: Names(nNames)

  character(8),     intent(inout) :: cu(lencu), cw(lencw)
  integer,          intent(inout) :: state(n+mcon), iu(leniu), iw(leniw)
  double precision, intent(inout) :: x(n+mcon), y(n+mcon), &
                                     A(ldA,*), H(ldH,*), &
                                     ru(lenru), rw(lenrw)

  integer,          intent(out)   :: info, itn, nInf, mincw, miniw, minrw
  double precision, intent(out)   :: sInf, Obj

  !=============================================================================
  !=============================================================================
  integer :: nb

  nb = n + mCon

  call dqopt(Start, n, nb, mCon, nnH,                  &
             Names, nNames, ncObj, iObj, ObjAdd, Prob, &
             A, ldA, bl, bu, cObj, H, ldH, qpHx,       &
             eType, state, x, y,                       &
             INFO, mincw, miniw, minrw,                &
             Obj, nInf, sInf,                          &
             cu, lencu, iu, leniu, ru, lenru,          &
             cw, lencw, iw, leniw, rw, lenrw)

  itn = iw(421)

end subroutine dqopt_wrap

!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

subroutine dnend_wrap(iw, leniw)
  integer, intent(in)    :: leniw, iw(leniw)

  !=============================================================================
  !=============================================================================

  close(iw(12))  ! print file
  if (iw(13) /= 6 ) close(iw(13))  ! summary file

end subroutine dnend_wrap

!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

subroutine dummycon(mode, mNCon, nnJac, x, fCon, JCon, ldJ, Status, &
                    cu, lencu, iu, leniu, ru, lenru)
  integer,          intent(in)     :: mNCon, nnJac, ldJ, status, &
                                      lencu, leniu, lenru
  double precision, intent(in)    :: x(nnJac)
  integer,          intent(inout) :: mode, iu(leniu)
  double precision, intent(inout) :: fCon(mNCon), JCon(ldJ,*), ru(lenru)
  character(8),     intent(inout) :: cu(lencu)

end subroutine dummycon

!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
