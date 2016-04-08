c cira.for, version number can be found at the end of this comment.
c-----------------------------------------------------------------------
C
C   Calculates neutral temperature parameters for IRI using the
C   MSIS-86/CIRA 1986 Neutral Thermosphere Model. The subroutines
C   GTS5, GLOBE5 and GLOBL5 developed by A.E. Hedin (2/26/87) were
C   modified for use in IRI --------- D. Bilitza -------- March 1991
C
C Corrections
C 11/09/99 always calculated Legendre; 'if glat' and 'if stl' taken out
C 11/09/99 use UMR, dumr and humr from COMMON 
C 04/16/15 Bugfix: Added SAVE PLG to CIRA86 (Joseph Nicholas)
C Version-mm/dd/yy-------------------------------------------------
C 2001.01 05/07/01 start of versioning
C 2002.01 28/10/02 replace TAB/6 blanks, enforce 72/line (D. Simpson)
C 2007.00 05/18/07 Release of IRI-2007
c-----------------------------------------------------------------------
C

      SUBROUTINE CIRA86(IDAY,SEC,GLAT,GLONG,STL,F107A,TINF,TLB,SIGMA)
c-----------------------------------------------------------------------
C
C     INPUT:
C        IDAY - DAY OF YEAR 
C        SEC - UT(SEC)
C        GLAT - GEODETIC LATITUDE(DEG)
C        GLONG - GEODETIC LONGITUDE(DEG)
C        STL - LOCAL APPARENT SOLAR TIME(HRS)
C        F107A - 3 MONTH AVERAGE OF F10.7 FLUX
C
C     OUTPUT: 
C        TINF - EXOSPHERIC TEMPERATURE (K)
C        TLB - TEMPERATURE AT LOWER BOUNDARY (K)
C        SIGMA - SHAPE PARAMETER FOR TEMPERATURE PROFILE
C
c-----------------------------------------------------------------------
        DIMENSION         PLG(9,4)
        common  /const/umr      /const1/hr,dr
        DATA      XL/1000./,TLL/1000./
        SAVE PLG

c      data umr/1.74E-2/,hr/0.2618/,dr/1.74e-2
cDR,DR2/1.72142E-2,0.0344284/,
cSR/7.2722E-5/,
c,HR/.2618/
c,DGTR/1.74533E-2/
c       dr = hr * 24. / 365.
c
        dr2 = dr * 2.
        sr = hr / 3600.

C
C CALCULATE LEGENDRE POLYNOMIALS
C
      IF(XL.EQ.GLAT)   GO TO 15
      C = SIN(GLAT*umr)
      S = COS(GLAT*umr)
      C2 = C*C
      C4 = C2*C2
      S2 = S*S
      PLG(2,1) = C
      PLG(3,1) = 0.5*(3.*C2 -1.)
      PLG(4,1) = 0.5*(5.*C*C2-3.*C)
      PLG(5,1) = (35.*C4 - 30.*C2 + 3.)/8.
      PLG(6,1) = (63.*C2*C2*C - 70.*C2*C + 15.*C)/8.
      PLG(2,2) = S
      PLG(3,2) = 3.*C*S
      PLG(4,2) = 1.5*(5.*C2-1.)*S
      PLG(5,2) = 2.5*(7.*C2*C-3.*C)*S
      PLG(6,2) = 1.875*(21.*C4 - 14.*C2 +1.)*S
      PLG(7,2) = (11.*C*PLG(6,2)-6.*PLG(5,2))/5.
      PLG(3,3) = 3.*S2
      PLG(4,3) = 15.*S2*C
      PLG(5,3) = 7.5*(7.*C2 -1.)*S2
      PLG(6,3) = 3.*C*PLG(5,3)-2.*PLG(4,3)
      PLG(4,4) = 15.*S2*S
      PLG(5,4) = 105.*S2*S*C 
      PLG(6,4)=(9.*C*PLG(5,4)-7.*PLG(4,4))/2.
      PLG(7,4)=(11.*C*PLG(6,4)-8.*PLG(5,4))/3.
      XL=GLAT
   15 CONTINUE
      IF(TLL.EQ.STL)   GO TO 16
      STLOC = SIN(HR*STL)
      CTLOC = COS(HR*STL)
      S2TLOC = SIN(2.*HR*STL)
      C2TLOC = COS(2.*HR*STL)
      S3TLOC = SIN(3.*HR*STL)
      C3TLOC = COS(3.*HR*STL)
      TLL = STL
   16 CONTINUE
C
      DFA=F107A-150.
C
C EXOSPHERIC TEMPERATURE
C
C         F10.7 EFFECT
      T1 =  ( 3.11701E-3 - 0.64111E-5 * DFA ) * DFA
        F1 = 1. + 0.426385E-2 * DFA 
        F2 = 1. + 0.511819E-2 * DFA
        F3 = 1. + 0.292246E-2 * DFA
C        TIME INDEPENDENT
      T2 = 0.385528E-1 * PLG(3,1) + 0.303445E-2 * PLG(5,1)
C        SYMMETRICAL ANNUAL AND SEMIANNUAL
        CD14 = COS( DR  * (IDAY+8.45398) )
        CD18 = COS( DR2 * (IDAY-125.818) )
        CD32 = COS( DR  * (IDAY-30.0150) )
        CD39 = COS( DR2 * (IDAY-2.75905) )
      T3 = 0.805486E-2 * CD32 + 0.14237E-1 * CD18
C        ASYMMETRICAL ANNUAL AND SEMIANNUAL
      T5 = F1 * (-0.127371 * PLG(2,1) - 0.302449E-1 * PLG(4,1) ) * CD14
     &       - 0.192645E-1 * PLG(2,1) * CD39
C        DIURNAL
      T71 =  0.123512E-1 * PLG(3,2) * CD14
      T72 = -0.526277E-2 * PLG(3,2) * CD14
      T7 = ( -0.105531 *PLG(2,2) - 0.607134E-2 *PLG(4,2) + T71 ) *CTLOC
     4   + ( -0.115622 *PLG(2,2) + 0.202240E-2 *PLG(4,2) + T72 ) *STLOC
C        SEMIDIURNAL
      T81 = 0.386578E-2 * PLG(4,3) * CD14
      T82 = 0.389146E-2 * PLG(4,3) * CD14
      T8= (-0.516278E-3 *PLG(3,3) - 0.117388E-2 *PLG(5,3) +T81)*C2TLOC
     3   +( 0.990156E-2 *PLG(3,3) - 0.354589E-3 *PLG(5,3) +T82)*S2TLOC
C        TERDIURNAL
      Z1 =  PLG(5,4) * CD14
      Z2 =  PLG(7,4) * CD14
      T14=(0.147284E-2*PLG(4,4)-0.173933E-3*Z1+0.365016E-4*Z2)*S3TLOC
     2   +(0.341345E-3*PLG(4,4)-0.153218E-3*Z1+0.115102E-3*Z2)*C3TLOC 
      T7814 = F2 * ( T7 + T8 + T14 )
C        LONGITUDINAL
      T11= F3 * (( 0.562606E-2 * PLG(3,2) + 0.594053E-2 * PLG(5,2) + 
     $       0.109358E-2 * PLG(7,2) - 0.301801E-2 * PLG(2,2) - 
     $       0.423564E-2 * PLG(4,2) - 0.248289E-2 * PLG(6,2) + 
     $      (0.189689E-2 * PLG(2,2) + 0.415654E-2 * PLG(4,2)) * CD14
     $     ) * COS(umr*GLONG) +
     $     ( -0.11654E-1 * PLG(3,2) - 0.449173E-2 * PLG(5,2) - 
     $       0.353189E-3 * PLG(7,2) + 0.919286E-3 * PLG(2,2) + 
     $       0.216372E-2 * PLG(4,2) + 0.863968E-3 * PLG(6,2) +
     $      (0.118068E-1 * PLG(2,2) + 0.331190E-2 * PLG(4,2)) * CD14
     $     ) * SIN(umr*GLONG) )
C        UT AND MIXED UT,LONGITUDE
      T12 = ( 1. - 0.565411 * PLG(2,1) ) * COS( SR*(SEC-31137.0) ) *
     $ (-0.13341E-1*PLG(2,1)-0.243409E-1*PLG(4,1)-0.135688E-1*PLG(6,1))
     $ +    ( 0.845583E-3 * PLG(4,3) + 0.538706E-3  * PLG(6,3) ) *
     $      COS( SR * (SEC-247.956) + 2.*umr*GLONG )
C  Exospheric temperature TINF/K  [Eq. A7]
      TINF = 1041.3 * ( 1. + T1+T2+T3+T5+T7814+T11+T12 ) * 0.99604
C
C TEMPERATURE DERIVATIVE AT LOWER BOUNDARY
C
C         F10.7 EFFECT
      T1 =  0.252317E-2 * DFA 
C        TIME INDEPENDENT
      T2 = -0.467542E-1 * PLG(3,1) + 0.12026 * PLG(5,1) 
C        ASYMMETRICAL ANNUAL
        CD14 = COS( DR  * (IDAY+8.45398) )
      T5 = -0.13324 * PLG(2,1)  * CD14
C        SEMIDIURNAL
      ZZ = PLG(4,3) * CD14
      T81 = -0.973404E-2 * ZZ
      T82 = -0.718482E-3 * ZZ
      T8 =(0.191357E-1 *PLG(3,3) + 0.787683E-2 *PLG(5,3) + T81) *C2TLOC
     3  + (0.125429E-2 *PLG(3,3) - 0.233698E-2 *PLG(5,3) + T82) *S2TLOC
C  dTn/dh at lower boundary  [Eq. A6]
      G0 = 0.166728E2 * ( 1. + T1+T2+T5+T8 ) * 0.951363
C
C NEUTRAL TEMPERATURE AT LOWER BOUNDARY 120KM
C
        CD9  = COS( DR2 * (IDAY-89.3820) )
        CD11 = COS( DR  * (IDAY+8.45398) )
      T1 = 0.568478E-3 * DFA
      T4 = 0.107674E-1 * CD9
      T5 =-0.192414E-1 * PLG(2,1) * CD11
      T7 = -0.2002E-1 *PLG(2,2) *CTLOC - 0.195833E-2 *PLG(2,2) *STLOC
      T8 = (-0.938391E-2 * PLG(3,3) - 0.260147E-2 * PLG(5,3)
     $       + 0.511651E-4 * PLG(6,3) * CD11 ) * C2TLOC
     $   + ( 0.131480E-1 * PLG(3,3) - 0.808556E-3 * PLG(5,3)
     $       + 0.255717E-2 * PLG(6,3) * CD11 ) * S2TLOC
C  Tn at lower boundary 120km   [Eq. A8]
      TLB = 386.0 * ( 1. + T1+T4+T5+T7+T8 ) * 0.976619
C  Sigma      [Eq. A5]
      SIGMA = G0 / ( TINF - TLB )
      RETURN
      END
