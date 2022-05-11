!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
! snopt7_python.f90
!
! To create sig file (*.pyf):
!     f2py snopt7_python.f90 -m snopt7_python -h snopt7_python.pyf
!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

subroutine copyOptions(option, Errors, cw, lencw, iw, leniw, rw, lenrw)
  implicit none
  character(*),     intent(in)    :: option
  integer,          intent(in)    :: lencw, leniw, lenrw
  character,        intent(inout) :: cw(lencw)*8
  integer,          intent(inout) :: iw(leniw)
  double precision, intent(inout) :: rw(lenrw)
  integer,          intent(out)   :: Errors

  !=============================================================================

  call snSet(option, 0, 0, Errors, cw, lencw, iw, leniw, rw, lenrw)

end subroutine copyOptions

!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

subroutine sninit_wrap(prtfile, summOn, cw, lencw, iw, leniw, rw, lenrw)
  implicit none
  character(*),     intent(in)    :: prtfile
  integer,          intent(in)    :: summOn, lencw, leniw, lenrw
  character,        intent(inout) :: cw(lencw)*8
  integer,          intent(inout) :: iw(leniw)
  double precision, intent(inout) :: rw(lenrw)
  !=============================================================================
  !=============================================================================
  integer :: iPrint, iSumm

  if (summOn /= 0) then
     call snInitF &
          (prtfile, 'screen', iPrint, iSumm, &
           cw, lencw, iw, leniw, rw, lenrw)
  else
     call snInitF &
          (prtfile, '', iPrint, iSumm, &
           cw, lencw, iw, leniw, rw, lenrw)
  end if

end subroutine sninit_wrap

!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

subroutine snspec_wrap(info, spcfile, cw, lencw, iw, leniw, rw, lenrw)
  implicit none
  character(*),     intent(in)    :: spcfile
  integer,          intent(in)    :: lencw, leniw, lenrw
  character,        intent(inout) :: cw(lencw)*8
  integer,          intent(inout) :: iw(leniw)
  double precision, intent(inout) :: rw(lenrw)
  integer,          intent(out)   :: info
  !=============================================================================
  !=============================================================================
  call snSpecF(spcfile, info, cw, lencw, iw, leniw, rw, lenrw)

end subroutine snspec_wrap

!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

subroutine snmema_wrap(info, nF, n, nxname, nFname, neA, neG, &
                       mincw, miniw, minrw, &
                       cw, lencw, iw, leniw, rw, lenrw)
  implicit none
  integer,          intent(in)    :: nF, n, nxname, nFname, neA, neG, &
                                     lencw, leniw, lenrw
  character,        intent(inout) :: cw(lencw)*8
  integer,          intent(inout) :: iw(leniw)
  double precision, intent(inout) :: rw(lenrw)
  integer,          intent(out)   :: info, mincw, miniw, minrw
  !=============================================================================
  !=============================================================================

  call snMemA(info, nF, n, nxname, nFname, neA, neG, &
              mincw, miniw, minrw, cw, lencw, iw, leniw, rw, lenrw)

end subroutine snmema_wrap

!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

subroutine snmem_wrap(info, m, n, ne, neG, nnCon, nnJac, nnObj, &
                       mincw, miniw, minrw,                     &
                       cw, lencw, iw, leniw, rw, lenrw)
  implicit none
  integer,          intent(in)    :: m, n, ne, neG, nnCon, nnJac, nnObj, &
                                     lencw, leniw, lenrw
  character,        intent(inout) :: cw(lencw)*8
  integer,          intent(inout) :: iw(leniw)
  double precision, intent(inout) :: rw(lenrw)
  integer,          intent(out)   :: info, mincw, miniw, minrw
  !=============================================================================
  !=============================================================================

  call snMemB(info, m, n, ne, neG, nnCon, nnJac, nnObj, &
              mincw, miniw, minrw, cw, lencw, iw, leniw, rw, lenrw)

end subroutine snmem_wrap

!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

subroutine snjac_wrap(INFO, nF, n, usrfun,                &
                      iAfun, jAvar, lenA, neA, A,         &
                      iGfun, jGvar, lenG, neG,            &
                      x, xlow, xupp, mincw, miniw, minrw, &
                      cu, lencu, iu, leniu, ru, lenru,    &
                      cw, lencw, iw, leniw, rw, lenrw)
  implicit none

  external                        :: usrfun
  integer,          intent(in)    :: nF, n, lenA, lenG, &
                                     lencu, leniu, lenru, &
                                     lencw, leniw, lenrw
  double precision, intent(in)    :: x(n), xlow(n), xupp(n)

  character,        intent(inout) :: cu(lencu)*8, cw(lencw)*8
  integer,          intent(inout) :: iu(leniu), iw(leniw)
  double precision, intent(inout) :: ru(lenru), rw(lenrw)

  integer,          intent(out)   :: info, mincw, miniw, minrw, neA, neG, &
                                     iAfun(lenA), jAvar(lenA), &
                                     iGfun(lenG), jGvar(lenG)
  double precision, intent(out)   :: A(lenA)

  !=============================================================================
  !=============================================================================

  call snJac(INFO, nF, n, usrfun,                &
             iAfun, jAvar, lenA, neA, A,         &
             iGfun, jGvar, lenG, neG,            &
             x, xlow, xupp, mincw, miniw, minrw, &
             cu, lencu, iu, leniu, ru, lenru,    &
             cw, lencw, iw, leniw, rw, lenrw)

  write(6,*) '' 

end subroutine snjac_wrap

!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

subroutine snopta_wrap(start, nF, n, nxname, nFname,           &
                       objUAdd, objRow, prob, usrfun,          &
                       iAfun, jAvar, lenA, neA, A,             &
                       iGfun, jGvar, lenG, neG,                &
                       xlow, xupp, xnames, Flow, Fupp, Fnames, &
                       x, xstate, xmul, F, Fstate, Fmul,       &
                       INFO, itn, mjritn,                      &
                       mincw, miniw, minrw,                    &
                       nS, nInf, sInf, Obj,                    &
                       cu, lencu, iu, leniu, ru, lenru,        &
                       cw, lencw, iw, leniw, rw, lenrw)
  implicit none

  external                        :: usrfun
  character,        intent(in)    :: Prob*8
  integer,          intent(in)    :: start, nF, n, nxname, nFname, &
                                     ObjRow, neA, lenA, lenG, neG, &
                                     iAfun(lenA), jAvar(lenA), &
                                     iGfun(lenG), jGvar(lenG), &
                                     lencu, leniu, lenru, &
                                     lencw, leniw, lenrw
  double precision, intent(in)    :: ObjUAdd, A(lenA), xlow(n), xupp(n), &
                                     Flow(nF), Fupp(nF)
  character,        intent(in)    :: xNames(nxname)*8, Fnames(nFname)*8

  character,        intent(inout) :: cu(lencu)*8, cw(lencw)*8
  integer,          intent(inout) :: xstate(n), Fstate(nF), iu(leniu), iw(leniw)
  double precision, intent(inout) :: x(n), xmul(n), F(nF), Fmul(nF), &
                                     ru(lenru), rw(lenrw)

  integer,          intent(out)   :: info, itn, mjritn, nS, nInf, &
                                     mincw, miniw, minrw
  double precision, intent(out)   :: sInf, Obj

  !=============================================================================
  !=============================================================================

  call snopta(start, nF, n, nxname, nFname,           &
              objUAdd, objRow, trim(Prob), usrfun,    &
              iAfun, jAvar, lenA, neA, A,             &
              iGfun, jGvar, lenG, neG,                &
              xlow, xupp, xnames, Flow, Fupp, Fnames, &
              x, xstate, xmul, F, Fstate, Fmul,       &
              INFO, mincw, miniw, minrw,              &
              nS, nInf, sInf,                         &
              cu, lencu, iu, leniu, ru, lenru,        &
              cw, lencw, iw, leniw, rw, lenrw)

  itn    = iw(421)
  mjritn = iw(422)
  Obj    = 0.0d+0

  if (ObjRow > 0) Obj = F(objRow)

end subroutine snopta_wrap

!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

subroutine snoptb_wrap(Start, m, n, neJ, nNames,        &
                       nnCon, nnObjU, nnJac,            &
                       iObjU, objUAdd, Prob,            &
                       funcon, funobj,                  &
                       valJ, indJ, locJ, bl, bu, Names, &
                       hs, x, pi, rc,                   &
                       INFO, itn, mjritn,               &
                       mincw, miniw, minrw,             &
                       nS, nInf, sInf, obj,             &
                       cu, lencu, iu, leniu, ru, lenru, &
                       cw, lencw, iw, leniw, rw, lenrw)
  implicit none

  external                        :: funcon
  external                        :: funobj

  character(*),     intent(in)    :: Start
  character,        intent(in)    :: Prob*8
  integer,          intent(in)    :: m, n, neJ, nNames, nnCon, nnJac, nnObjU, &
                                     iObjU, indJ(neJ), locJ(n+1), &
                                     lencu, leniu, lenru, &
                                     lencw, leniw, lenrw
  double precision, intent(in)    :: ObjUAdd, valJ(neJ), bl(n+m), bu(n+m)
  character,        intent(in)    :: Names(nNames)*8

  character,        intent(inout) :: cu(lencu)*8, cw(lencw)*8
  integer,          intent(inout) :: hs(n+m), iu(leniu), iw(leniw)
  double precision, intent(inout) :: x(n+m), pi(m), ru(lenru), rw(lenrw)

  integer,          intent(out)   :: info, itn, mjritn, nS, nInf, &
                                     mincw, miniw, minrw
  double precision, intent(out)   :: sInf, Obj, rc(n+m)

  !=============================================================================
  !=============================================================================

  call snoptb(Start, m, n, neJ, nNames,        &
              nnCon, nnObjU, nnJac,            &
              iObjU, objUAdd, trim(Prob),      &
              funcon, funobj,                  &
              valJ, indJ, locJ, bl, bu, Names, &
              hs, x, pi, rc,                   &
              INFO, mincw, miniw, minrw,       &
              nS, nInf, sInf, obj,             &
              cu, lencu, iu, leniu, ru, lenru, &
              cw, lencw, iw, leniw, rw, lenrw)

  itn    = iw(421)
  mjritn = iw(422)

end subroutine snoptb_wrap

!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

subroutine snoptc_wrap(Start, m, n, neJ, nNames,        &
                       nnCon, nnObjU, nnJac,            &
                       iObjU, objUAdd, Prob, usrfunc,   &
                       valJ, indJ, locJ, bl, bu, Names, &
                       hs, x, pi, rc,                   &
                       INFO, itn, mjritn,               &
                       mincw, miniw, minrw,             &
                       nS, nInf, sInf, obj,             &
                       cu, lencu, iu, leniu, ru, lenru, &
                       cw, lencw, iw, leniw, rw, lenrw)
  implicit none

  external                        :: usrfunc

  character(*),     intent(in)    :: Start
  character,        intent(in)    :: Prob*8
  integer,          intent(in)    :: m, n, neJ, nNames, nnCon, nnJac, nnObjU, &
                                     iObjU, indJ(neJ), locJ(n+1), &
                                     lencu, leniu, lenru, &
                                     lencw, leniw, lenrw
  double precision, intent(in)    :: ObjUAdd, valJ(neJ), bl(n+m), bu(n+m)
  character,        intent(in)    :: Names(nNames)*8

  character,        intent(inout) :: cu(lencu)*8, cw(lencw)*8
  integer,          intent(inout) :: hs(n+m), iu(leniu), iw(leniw)
  double precision, intent(inout) :: x(n+m), pi(m), ru(lenru), rw(lenrw)

  integer,          intent(out)   :: info, itn, mjritn, nS, nInf, &
                                     mincw, miniw, minrw
  double precision, intent(out)   :: sInf, Obj, rc(n+m)

  !=============================================================================
  !=============================================================================

  call snoptc(Start, m, n, neJ, nNames,        &
              nnCon, nnObjU, nnJac,            &
              iObjU, objUAdd, trim(Prob),      &
              usrfunc,                         &
              valJ, indJ, locJ, bl, bu, Names, &
              hs, x, pi, rc,                   &
              INFO, mincw, miniw, minrw,       &
              nS, nInf, sInf, obj,             &
              cu, lencu, iu, leniu, ru, lenru, &
              cw, lencw, iw, leniw, rw, lenrw)

  itn    = iw(421)
  mjritn = iw(422)

end subroutine snoptc_wrap

!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

subroutine sqinit_wrap(prtfile, summOn, cw, lencw, iw, leniw, rw, lenrw)
  implicit none
  character(*),     intent(in)    :: prtfile
  integer,          intent(in)    :: summOn, lencw, leniw, lenrw
  character,        intent(inout) :: cw(lencw)*8
  integer,          intent(inout) :: iw(leniw)
  double precision, intent(inout) :: rw(lenrw)
  !=============================================================================
  !=============================================================================
  integer :: iPrint, iSumm

  if (summOn /= 0) then
     call sqInitF(prtfile, 'screen', iPrint, iSumm, &
                  cw, lencw, iw, leniw, rw, lenrw)
  else
     call sqInitF(prtfile, '', iPrint, iSumm, &
                  cw, lencw, iw, leniw, rw, lenrw)
  end if

end subroutine sqinit_wrap

!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

subroutine sqspec_wrap(info, spcfile, cw, lencw, iw, leniw, rw, lenrw)
  implicit none
  character(*),     intent(in)    :: spcfile
  integer,          intent(in)    :: lencw, leniw, lenrw
  character,        intent(inout) :: cw(lencw)*8
  integer,          intent(inout) :: iw(leniw)
  double precision, intent(inout) :: rw(lenrw)
  integer,          intent(out)   :: info
  !=============================================================================
  !=============================================================================
  call sqSpecF(trim(spcfile), info, cw, lencw, iw, leniw, rw, lenrw)

end subroutine sqspec_wrap

!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

subroutine sqopt_wrap(Start, qpHx, m, n, neA, nNames, ncObj, nnH, &
                      iObj, ObjAdd, Prob,                         &
                      valA, indA, locA, bl, bu, cObj, Names,      &
                      eType, hs, x, pi, rc,                       &
                      INFO, itn,                                  &
                      mincw, miniw, minrw,                        &
                      nS, nInf, sInf, Obj,                        &
                      cu, lencu, iu, leniu, ru, lenru,            &
                      cw, lencw, iw, leniw, rw, lenrw)
  implicit none

  external                        :: qpHx

  character(*),     intent(in)    :: Start
  character,        intent(in)    :: Prob*8
  integer,          intent(in)    :: m, n, neA, nNames, ncObj, nnH, iObj, &
                                     eType(n+m), indA(neA), locA(n+1), &
                                     lencu, leniu, lenru, &
                                     lencw, leniw, lenrw
  double precision, intent(in)    :: ObjAdd, valA(neA), &
                                     bl(n+m), bu(n+m), cObj(ncObj)
  character,        intent(in)    :: Names(nNames)*8

  character,        intent(inout) :: cu(lencu)*8, cw(lencw)*8
  integer,          intent(inout) :: hs(n+m), iu(leniu), iw(leniw)
  double precision, intent(inout) :: x(n+m), pi(m), ru(lenru), rw(lenrw)

  integer,          intent(out)   :: info, itn, nS, nInf, &
                                     mincw, miniw, minrw
  double precision, intent(out)   :: sInf, Obj, rc(n+m)

  !=============================================================================
  !=============================================================================

  call sqopt(Start, qpHx, m, n, neA, nNames,              &
             ncObj, nnH, iObj, ObjAdd,                    &
             Prob, valA, indA, locA, bl, bu, cObj, Names, &
             eType, hs, x, pi, rc,                        &
             INFO, mincw, miniw, minrw,                   &
             nS, nInf, sInf, Obj,                         &
             cu, lencu, iu, leniu, ru, lenru,             &
             cw, lencw, iw, leniw, rw, lenrw)

  itn = iw(421)

end subroutine sqopt_wrap

!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

subroutine snend_wrap(cw, lencw, iw, leniw, rw, lenrw)
  integer,          intent(in)    :: lencw, leniw, lenrw
  character,        intent(inout) :: cw(lencw)*8
  integer,          intent(inout) :: iw(leniw)
  double precision, intent(inout) :: rw(lenrw)

  !=============================================================================
  !=============================================================================
  call snEndF(cw, lencw, iw, leniw, rw, lenrw)

end subroutine snend_wrap

!+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
