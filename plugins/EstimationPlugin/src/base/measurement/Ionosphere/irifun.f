c irifun.for, version number can be found at the end of this comment.
c-----------------------------------------------------------------------
C
C Functions and subroutines for the International Reference 
C Ionosphere model. These functions and subroutines are called by
C IRI_SUB subroutine (IRISUB.FOR).
c
c-----------------------------------------------------------------------
C   
c i/o units:  6   messages (during execution) to Konsol (Monitor)
c            10   CCIR and URSI coefficients
c            11   alternate unit for message file (if jf(12)=.false.)
c            12   solar/ionospheric indices: ig_rz.dat        
c            13   magnetic indices: ap.dat
c            14   igrf coefficients
c        
c-----------------------------------------------------------------------
c changes from IRIFU9 to IRIF10:
c       SOCO for solar zenith angle 
c       ACOS and ASIN argument forced to be within -1 / +1
c       EPSTEIN functions corrected for large arguments
c-----------------------------------------------------------------------
c changes from IRIF10 to IRIF11: 
c       LAY subroutines introduced
c       TEBA corrected for 1400 km
c-----------------------------------------------------------------------
c changes from IRIF11 to IRIF12:
C       Neutral temperature subroutines now in CIRA86.FOR 
C       TEDER changed
C       All names with 6 or more characters replaced 
C       10/29/91 XEN: 10^ in loop, instead of at the end
C       1/21/93 B0_TAB instead of B0POL
C       9/22/94 Alleviate underflow condition in IONCOM exp()
c-----------------------------------------------------------------------
c changes from IRIF12 to IRIF13:
C        9/18/95 MODA: add leap year and number of days in month
C        9/29/95 replace F2out with FOUT and XMOUT.
C       10/ 5/95 add TN and DTNDH; earlier in CIRA86.FOR
C       10/ 6/95 add TCON for reading indices
C       10/20/95 MODA: IN=1 MONTH=IMO
C       10/20/95 TCON: now includes RZ interpolation
C       11/05/95 IONCOM->IONCO1, added IONCOM_new, IONCO2
C       11/05/95 LSTID added for strom-time updating
C       11/06/95 ROGUL: transition 20. instead of 15.
C       12/01/95 add UT_LT for (date-)correct UT<->LT conversion
C       01/16/96 TCON: add IMST to SAVE statement
C       02/02/96 ROGUL: 15. reinstated
C       02/07/96 UT_LT: ddd, dddend integer, no leap year 2000
C       03/15/96 ZERO: finding delta for topside
C       03/18/96 UT_LT: mode=1, change of year
C       12/09/96 since 2000 is leap, delete y/100*100 condition
C       04/25/97 XMDED: minimal value also daytime
C       05/18/98 TCON: changes to IG_RZ (update date); -R = Cov
C       05/19/98 Replaced IONCO2&APROK; HEI,XHI in IONCOM_NEW
C       10/01/98 added INITIALIZE
C       04/30/99 MODA: reset bb(2)=28
C       11/08/99 avoid negative x value in function XE2. Set x=0.
C       11/09/99 added COMMON/const1/humr,dumr also for CIRA86
C       11/30/99 EXIT in APROK replaced with GOTO (N. Smirnova)
c-----------------------------------------------------------------------
c changes from IRIF13 to IRIFUN:
C-Version-mm/dd/yy-description [person reporting correction]
C 2000.01 05/09/00 Include B0_98 subroutine to replace B0_NEW and B0POL
C 2000.02 05/18/00 Include Elteik and spharm_ik for Te
C 2000.03 06/09/00 Include xe3_1, xe4_1, xe_1
C 2000.04 06/11/00 Include f1_c1, f1_prob, modified fof1ed
C 2000.05 10/30/00 Include vdrift
C 2000.06 04/15/01 Include IGRF_SUB subroutine for IK Te model
C 2000.07 05/07/01 Include storm subroutine STORM and Ap access s/w
C 2000.08 09/07/01 APF: if(j1.eq.j2) -> if(IY.eq.j2) [P. Wilkinson]
C 2000.09 09/07/01 CONVER: LO2 = MOD(LO1,20)+1 [P. Webb,D. Pesnell]
C 2000.10 02/20/02 CONVER/DATA: 105.78 -> 015.78 [A. Shovkoplyas] 
C 2000.11 10/28/02 replace TAB/6 blanks, enforce 72/line [D. Simpson]
C 2000.12 11/08/02 removing unused variables (corr); apf0 removed
C 2000.13 11/26/02 apf() using keyed access to ap.dat file; apf->apf1
C 2000.14 11/27/02 changed F1_PROB; always 6 preceeding spaces 
C 2005.01 03/09/05 CALION,INVDPC,CALNE for new Ne, Ni models
C 2005.01 11/14/05 APF_ONLY for F107D;  
C 2005.01 11/14/05 spreadf_brazil; added constraint 0<=P<=1 
C 2005.02 05/11/06 NeQuick: XE1,TOPQ, M3000HM; stormvd,
C 2005.02 03/27/07 STORM: hourly interpolation of Ap  [A. Oinats]
C 2007.00 05/18/07 Release of IRI-2007
C 2007.01 09/19/07 vdrift et al.: without *8 (no change in results)
C 2007.01 02/07/09 IONLOW: N+ correction [V. Truhlik]
C 2007.02 03/30/09 NMDED: avoid exp underflow [K. Choi] 
C 2007.02 03/30/09 spreadf_brazil: bspl2f et al b(20->30) [Tab Ji]
C 2007.02 03/30/09 APF_ONLY: Compute monthly F10.7
C 2007.03 05/26/09 APF_ONLY: replace i with 1 and IMN with ID [R.Conde]
C 2007.04 07/10/09 CONVER/DATA: 015.78 -> 005.78 [E. Araujo] 
C 2007.05 07/23/09 STORM/CONVER: long. discont. [R. Conde, E. Araujo] 
C 2007.05 07/23/09 APF,APF_ONLY: use YearBegin from ap.dat [R. Conde] 
C 2007.06 02/03/10 APF: text if eof; clean-up APF and APF_only
C         04/16/15 Bugfix: Added IER parameter to call
C                  to igrf_sub [Joseph Nicholas]
C         04/16/15 Bugfix: IF condition in ELTEIK [Joseph Nicholas]
C         04/16/15 Load data from memory instead of file (J. Nicholas)
C         04/27/15 Error checking bugfixes (J. Nicholas)
C
C**************************************************************  
C********** INTERNATIONAL REFERENCE IONOSPHERE ****************  
C**************************************************************  
C****************  FUNCTIONS,SUBROUTINES  *********************
C**************************************************************
C** initialize:	INITIALIZE 
C** NE:         XE1,TOPQ,ZERO,DXE1N,XE2,XE3_1,XE4_1,XE5,XE6,
C**             XE_1,CALNE
C** TE/TI:      ELTEIK,SPHARM_IK,TEBA,SPHARM,ELTE,TEDE,TI,TEDER,
C**		        TN,DTNDH
C** NI:         RPID,RDHHE,RDNO,KOEFP1,KOEFP2,KOEFP3,SUFE,IONCO2,
C**             APROK,IONCOMP,IONCO1,CALION,IONLOW,IONHIGH,INVDPC
C** PEAKS:      FOUT,XMOUT,HMF2ED,FOF1ED,f1_c1,f1_prob,FOEEDI,XMDED,
C**		        GAMMA1
C** PROFILE PAR:B0_98,TAL,VALGUL
C** MAG. FIELD: GGM,FIELDG,CONVER(Geom. Corrected Latitude)
C** FUNCTIONS:  REGFA1
C** TIME:       SOCO,HPOL,MODA,UT_LT
C** EPSTEIN:    RLAY,D1LAY,D2LAY,EPTR,EPST,EPSTEP,EPLA
C** LAY:        XE2TO5,XEN,VALGUL,ROGUL,LNGLSN,LSKNM,INILAY
C** INDICES:    TCON,APF,APF_ONLY
C** Storm:   	LSTID,storm
C** ion drift:  vdrift,bspl4_time,bspl4_long,g,stormvd,
C**             bspl4_ptime
C** spread-F:	spreadf_brazil,bspl2f,bspl2l,bspl2s,bspl4t
C**************************************************************  
C  
C**************************************************************  
C***  -------------------ADDRESSES------------------------  ***
C***  I  PROF. K. RAWER             DR. D. BILITZA       I  ***
C***  I  HERRENSTR. 43              GSFC CODE 632        I  ***
C***  I  7801 MARCH 1               GREENBELT MD 20771   I  ***
C***  I  F.R.G.                     USA                  I  ***
C***  ----------------------------------------------------  ***
C**************************************************************  
C**************************************************************  
C
C
        subroutine initialize
        COMMON  /CONST/UMR  /ARGEXP/ARGMAX  /const1/humr,dumr
        ARGMAX=88.0
        pi=atan(1.0)*4.
        UMR=pi/180.
        humr=pi/12.
        dumr = pi / 182.5
        return 
        end

C        
C*************************************************************   
C*************** ELECTRON DENSITY ****************************   
C*************************************************************   
C

        FUNCTION XE1(H)    
c----------------------------------------------------------------
C REPRESENTING ELECTRON DENSITY(M-3) IN THE TOPSIDE IONOSPHERE   
C (H=HMF2....1000 KM) BY HARMONIZED BENT-MODEL ADMITTING 
C VARIABILITY OFGLOBAL PARAMETER ETA,ZETA,BETA,DELTA WITH        
C GEOM. LATITUDE, SMOOTHED SOLAR FLUX AND CRITICAL FREQUENCY     
C (SEE MAIN PROGRAM).    
C [REF.:K.RAWER,S.RAMAKRISHNAN,1978]     
c----------------------------------------------------------------
        COMMON  /BLOCK1/HMF2,XNMF2,HMF1,F1REG
     &          /BLO10/BETA,ETA,DELTA,ZETA
     &          /BLO11/B2TOP,TC3,itopn,alg10,hcor1
     &          /ARGEXP/ARGMAX

        logical 	f1reg              

        IF(itopn.eq.2) THEN
          XE1=TOPQ(H,XNMF2,HMF2,B2TOP)
          RETURN
        ENDIF
      
        DXDH = (1000.-HMF2)/700.
        x0 = 300. - delta
        xmx0 = (H-HMF2)/DXDH
        x = xmx0 + x0
        eptr1 = eptr(x,beta,394.5) - eptr(x0,beta,394.5)
        eptr2 = eptr(x,100.,300.0) - eptr(x0,100.,300.0) 

        y = BETA * ETA * eptr1 + ZETA * (100. * eptr2 - xmx0)

        Y = y * dxdh
        if(abs(Y).gt.argmax) Y = sign(argmax,Y)
        TCOR = 0.
        IF(itopn.eq.1.and.h.gt.hcor1) then
             xred = h - hcor1
             rco = tc3 * xred
             TCOR = rco * alg10
             endif
        XE1 = XNMF2 * EXP(-Y+TCOR)                             
        RETURN          
        END             
C
C

        REAL FUNCTION TOPQ(h,No,hmax,Ho)
c----------------------------------------------------------------
c  NeQuick formula
c----------------------------------------------------------------
        REAL No
        PARAMETER (g=0.125,rfac=100.0)
          dh=h-hmax
          g1=g*dh
          z=dh/(Ho*(1.0+rfac*g1/(rfac*Ho+g1)))
          if(z.gt.40) then
            topq=0.0
            return
          endif
          ee=exp(z)
          if (ee.gt.1.0e7) then
            ep=4.0/ee
          else
            ep=4.0*ee/(1.0+ee)**2
          endif
          TOPQ=No*ep
        RETURN
        END

C 
C
        REAL FUNCTION ZERO(DELTA)
C FOR A PEAK AT X0 THE FUNCTION ZERO HAS TO BE EQUAL TO 0.
        COMMON  /BLO10/         BETA,ETA,DEL,ZETA
     &          /ARGEXP/        ARGMAX

        arg1=delta/100.
        if (abs(arg1).lt.argmax) then
                z1=1./(1.+exp(arg1))
        else if (arg1.lt.0) then
                z1=1.
        else
                z1=0.
        endif
        arg1=(delta+94.5)/beta
        if (abs(arg1).lt.argmax) then
                z2=1./(1.+exp(arg1))
        else if (arg1.lt.0) then
                z2=1.
        else
                z2=0.
        endif
        zero=zeta*(1.-z1) - eta*z2
        return
        end
C
C
        FUNCTION DXE1N(H)                            
C LOGARITHMIC DERIVATIVE OF FUNCTION XE1 (KM-1).   
        COMMON    /BLOCK1/HMF2,XNMF2,HMF1,F1REG
     &            /BLO10/BETA,ETA,DELTA,ZETA                    
	    logical f1reg

        x0 = 300. - delta
        X=(H-HMF2)/(1000.0-HMF2)*700.0 + x0
        epst2 = epst(x,100.0,300.0)
        epst1 = epst(x,beta ,394.5)
        DXE1N = - ETA * epst1 + ZETA * (1. - epst2)             
        RETURN          
        END             
C
C
        REAL FUNCTION XE2(H)                         
C ELECTRON DENSITY FOR THE BOTTOMSIDE F-REGION (HMF1...HMF2).                   
        COMMON    /BLOCK1/HMF2,XNMF2,HMF1,F1REG
     &          /BLOCK2/B0,B1,C1        /ARGEXP/ARGMAX
	    logical	f1reg

        X=(HMF2-H)/B0
        if(x.le.0.0) x=0.0
        z=x**b1
        if(z.gt.argmax) z=argmax
        XE2=XNMF2*EXP(-z)/COSH(X)                 
        RETURN          
        END             
C
C
        REAL FUNCTION XE3_1(H)
C ELECTRON DENSITY FOR THE F1-LAYER (HZ.....HMF1)
C USING THE NEW DEFINED F1-LAYER FUNCTION (Reinisch and Huang, Advances 
C in Space Research, Volume 25, Number 1, 81-88, 2000)
        COMMON	/BLOCK1/	HMF2,XNMF2,HMF1,F1REG
     &		/BLOCK2/	B0,B1,D1F1
	    logical	f1reg
C
	    h1bar=h
        if (f1reg) H1BAR=HMF1*(1.0-((HMF1-H)/HMF1)**(1.0+D1F1))
        XE3_1=XE2(H1BAR)
        RETURN
        END
C
C
        REAL FUNCTION XE4_1(H)
C ELECTRON DENSITY FOR THE INTERMEDIATE REGION (HEF...HZ)
C USING THE NEW DEFINED FUNCTION
        COMMON	/BLOCK3/	HZ,T,HST
     &		/BLOCK4/	HME,XNME,HEF
C
	    if(hst.lt.0.0) then
		xe4_1=xnme+t*(h-hef)
		return
		endif
        IF(HST.EQ.HEF) THEN
           H1BAR=H
        ELSE
           H1BAR=HZ+0.5*T-SIGN(1.0,T)*SQRT(T*(0.25*T+HZ-H))
        ENDIF
        XE4_1=XE3_1(H1BAR)
        RETURN
        END
C
C
        REAL FUNCTION XE5(H)                         
C ELECTRON DENSITY FOR THE E AND VALLEY REGION (HME..HEF).   
        LOGICAL NIGHT   
        COMMON    /BLOCK4/        HME,XNME,HEF
     &          /BLOCK5/        NIGHT,E(4)                    
        T3=H-HME        
        T1=T3*T3*(E(1)+T3*(E(2)+T3*(E(3)+T3*E(4))))  
        IF(NIGHT) GOTO 100                           
        XE5=XNME*(1+T1)  
        RETURN          
100     XE5=XNME*EXP(T1)                              
        RETURN          
        END             
C
C
        REAL FUNCTION XE6(H)                         
C ELECTRON DENSITY FOR THE D REGION (HA...HME).    
        COMMON    /BLOCK4/        HME,XNME,HEF
     &          /BLOCK6/        HMD,XNMD,HDX
     &        /BLOCK7/        D1,XKK,FP30,FP3U,FP1,FP2    
        IF(H.GT.HDX) GOTO 100                        
        Z=H-HMD         
        FP3=FP3U        
        IF(Z.GT.0.0) FP3=FP30                        
        XE6=XNMD*EXP(Z*(FP1+Z*(FP2+Z*FP3)))           
        RETURN          
100     Z=HME-H         
        XE6=XNME*EXP(-D1*Z**XKK)
        RETURN          
        END             
C
C
        REAL FUNCTION XE_1(H)                          
C ELECTRON DENSITY BEETWEEN HA(KM) AND 1000 KM     
C SUMMARIZING PROCEDURES  NE1....6;                
        COMMON    /BLOCK1/HMF2,XNMF2,XHMF1,F1REG         
     &          /BLOCK3/HZ,T,HST
     &        /BLOCK4/HME,XNME,HEF
	    logical 	f1reg
	    if(f1reg) then
		   hmf1=xhmf1
	    else
		   hmf1=hmf2
	    endif
        IF(H.LT.HMF2) GOTO 100                       
        XE_1=XE1(H)     
        RETURN          

100     IF(H.LT.HMF1) GOTO 300                       
        XE_1=XE2(H)       
        RETURN          

300     IF(H.LT.HZ) GOTO 400                         
        XE_1=XE3_1(H)       
        RETURN          

400     IF(H.LT.HEF) GOTO 500                        
        XE_1=XE4_1(H)       
        RETURN          

500     IF(H.LT.HME) GOTO 600                        
        XE_1=XE5(H)       
        RETURN          

600     XE_1=XE6(H)       
        RETURN          
        END             
c
c
	SUBROUTINE CALNE(CRD,INVDIP,FL,DIMO,B0,
     &                    DIPL,MLT,ALT,DDD,F107,NNE)
C Version 1.0 (released 30.6.2004)
C CALNE calculates electron density in the outer
C ionosphere with account of solar activity (F107 index).
C CALNE uses subroutines NELOW and NEHIGH.
C Linearly interpolates for solar activity.
C Inputs: CRD - 0 .. INVDIP
C               1 .. FL, DIMO, B0, DIPL (used for calculation INVDIP inside)
C         INVDIP - "mix" coordinate of the dip latitude and of
C                    the invariant latitude;
C                    positive northward, in deg, range <-90.0;90.0>
C         FL, DIMO, BO - McIlwain L parameter, dipole moment in
C                        Gauss, magnetic field strength in Gauss -
C                        parameters needed for invariant latitude
C                        calculation
C         DIPL - dip latitude
C                positive northward, in deg, range <-90.0;90.0>
C         MLT - magnetic local time (central dipole)
C               in hours, range <0;24)
C         ALT - altitude above the Earth's surface;
C               in km, range <350;2000>
C         DDD - day of year; range <0;365>
C         F107 - F107 index
C Output: NNE - electron density [m-3]
C Versions:  1.0 FORTRAN
C Author of the code:
C         Vladimir Truhlik
C         Institute of Atm. Phys.
C         Bocni II.
C         141 31 Praha 4, Sporilov
C         Czech Republic
C         e-mail: vtr@ufa.cas.cz
C         tel/fax: +420 267103058
      REAL INVDIP,FL,DIMO,B0,DIPL,MLT,ALT,F107
      INTEGER CRD,DDD,ION
      REAL NNE,NNEH,NNEL
      DIMENSION  DNEL(3,3,49),DNEH(4,3,49)
C/////////////////////coefficients high solar activity////////////////////////
C//////////////////////////////////NE/////////////////////////////////////////
C     550km equinox
      DATA (DNEH(1,1,J),J=1,49)/ 1.1654E+001,-2.2826E-008,-2.9373E-001,
     &                       -3.4268E-010, 6.4972E-002,-4.1631E-008,
     &                        2.5040E-003,-2.3607E-001, 3.3224E-009,
     &                       -6.5151E-004, 4.3800E-009, 5.5170E-004,
     &                        3.3359E-009,-3.8492E-001, 7.7351E-009,
     &                        4.8326E-002,-2.4344E-009,-9.2751E-003,
     &                        4.8353E-009, 1.0247E-001,-7.7373E-010,
     &                       -4.3734E-003,-1.4332E-010,-1.4617E-003,
     &                       -3.8595E-002, 2.2162E-009, 2.0572E-002,
     &                       -2.2251E-009, 3.7842E-003,-7.5166E-002,
     &                        6.2465E-010, 1.6258E-003, 2.5023E-010,
     &                        1.9937E-002,-1.5199E-009,-4.3000E-003,
     &                        5.8564E-010, 2.6435E-002, 1.3539E-009,
     &                        5.2811E-003, 2.4115E-002, 7.1913E-010,
     &                        2.8906E-003,-1.0944E-002, 4.8498E-011,
     &                        5.0294E-004,-2.3208E-011, 9.6733E-003,
     &                       -6.1120E-003/
C     550km June solstice
      DATA (DNEH(1,2,J),J=1,49)/ 1.1447E+001, 3.3033E-001,-3.8532E-001,
     &                       -2.2359E-001, 2.1158E-001,-1.3881E-002,
     &                       -2.9286E-002,-2.3339E-001, 7.7235E-003,
     &                        5.7230E-002,-3.4982E-002, 1.1379E-002,
     &                        2.5918E-003,-3.3942E-001, 4.9621E-002,
     &                        6.0180E-002,-7.9666E-003, 2.1570E-003,
     &                       -8.0235E-003, 8.0700E-002, 3.5040E-002,
     &                       -4.4115E-003, 6.9310E-003, 5.5866E-003,
     &                        3.7179E-002, 1.8795E-002, 1.5728E-002,
     &                        7.9808E-003,-3.7419E-003, 1.7878E-002,
     &                       -9.4949E-003, 8.7620E-003,-2.2723E-003,
     &                        2.1580E-002, 3.0423E-002,-1.6150E-003,
     &                       -2.0801E-003,-2.6775E-002, 4.7439E-003,
     &                       -6.2996E-004,-3.8774E-002,-1.0191E-003,
     &                       -5.9711E-004,-6.1800E-003, 4.3409E-003,
     &                       -1.0281E-002, 2.3239E-003,-9.4754E-003,
     &                       -1.2886E-002/
C     900km equinox
      DATA (DNEH(2,1,J),J=1,49)/ 1.0964E+001,-1.5354E-009,-1.7357E-001,
     &                        2.4852E-008, 1.2550E-001,-5.3015E-009,
     &                       -8.5408E-004,-2.7902E-001, 4.7844E-009,
     &                        7.1440E-003, 1.8258E-009,-2.3096E-003,
     &                        2.5878E-009,-2.7308E-001, 4.6071E-009,
     &                        4.8916E-002,-9.9364E-009,-3.8824E-002,
     &                        4.5371E-009, 3.9659E-003,-1.9952E-009,
     &                       -5.8555E-004, 1.4458E-009, 6.9817E-003,
     &                        4.1239E-002,-5.4244E-010, 1.0611E-002,
     &                       -1.3677E-009, 2.8933E-003, 2.8197E-002,
     &                       -1.3422E-009,-8.5109E-003, 9.2018E-010,
     &                       -2.0264E-002,-8.7024E-010,-2.2666E-003,
     &                       -3.2360E-010, 4.8919E-003,-7.2078E-010,
     &                       -3.0536E-003,-7.7757E-003,-7.6475E-010,
     &                       -3.8334E-003, 4.4619E-003,-3.4009E-010,
     &                        3.4805E-003,-3.3311E-010,-5.0825E-003,
     &                       -2.6849E-003/
C     900km June solstice
      DATA (DNEH(2,2,J),J=1,49)/ 1.0747E+001, 2.3540E-001,-1.6245E-001,
     &                       -1.5474E-002, 8.4395E-002, 3.6128E-002,
     &                        6.6939E-004,-2.3379E-001, 3.4316E-002,
     &                        6.0415E-003, 5.0101E-003,-3.4666E-003,
     &                       -7.7481E-003,-3.0157E-001,-5.7407E-003,
     &                        6.3775E-002, 1.0736E-002,-4.0436E-003,
     &                        7.4920E-003,-1.9215E-002, 1.0255E-002,
     &                        5.1722E-003,-5.0936E-003,-8.4850E-004,
     &                       -5.6382E-002, 1.6156E-002, 1.4547E-002,
     &                       -3.5661E-003, 5.1087E-003,-2.3719E-002,
     &                        8.3592E-004, 3.1225E-003, 1.9821E-003,
     &                        2.6843E-002, 2.6876E-003,-6.2698E-003,
     &                       -2.7731E-004,-1.8704E-002, 2.2622E-003,
     &                        1.1103E-003, 7.4859E-003,-1.5696E-003,
     &                       -1.4674E-003,-1.2356E-002,-6.2705E-005,
     &                       -9.0270E-003,-2.1341E-004,-1.4714E-002,
     &                       -2.6567E-003/
C     1500km equinox
      DATA (DNEH(3,1,J),J=1,49)/ 1.0369E+001, 1.9844E-008,-2.0005E-001,
     &                        4.6147E-008, 1.3227E-001, 1.8945E-008,
     &                        1.1316E-001,-1.8108E-001, 2.7934E-009,
     &                       -1.4698E-002, 4.1471E-009, 3.5627E-003,
     &                        2.7073E-009,-7.2201E-002, 1.7208E-009,
     &                        3.9133E-002,-3.6288E-009,-3.9790E-003,
     &                        5.9338E-010,-4.4768E-002,-1.1350E-009,
     &                       -1.9636E-003, 1.6649E-009, 6.3878E-003,
     &                        1.3926E-002,-2.8892E-010, 5.0022E-003,
     &                       -8.1142E-011, 1.3966E-003, 3.5646E-002,
     &                       -8.2139E-010,-1.6012E-003,-1.0333E-010,
     &                        2.1714E-002,-1.7686E-009,-2.7486E-003,
     &                       -1.6622E-010, 1.4230E-002, 6.6450E-011,
     &                        6.0710E-004, 1.0634E-002,-7.6340E-010,
     &                       -2.8168E-003, 4.7432E-004,-1.1803E-011,
     &                       -4.2779E-004,-3.4642E-010,-1.1471E-003,
     &                        5.0097E-003/
C     1500km June solstice
      DATA (DNEH(3,2,J),J=1,49)/ 1.0230E+001, 1.8549E-001,-1.7478E-001,
     &                        2.3684E-002, 4.9509E-002,-7.9218E-002,
     &                        1.2211E-001,-1.9639E-001,-3.9280E-002,
     &                        3.1281E-002, 1.4507E-002,-1.0088E-002,
     &                        5.7436E-003,-1.2489E-001, 1.2205E-002,
     &                        8.9282E-003,-1.1558E-002, 6.1501E-003,
     &                       -5.2639E-003,-2.3903E-002,-9.0686E-003,
     &                       -2.4549E-005, 6.6842E-006,-2.8069E-004,
     &                       -4.2259E-002,-8.7094E-003, 8.0624E-003,
     &                       -2.4729E-003,-5.7736E-004, 6.1173E-003,
     &                        1.4685E-003,-1.9136E-003,-6.6155E-004,
     &                       -1.6568E-002,-6.6461E-003,-2.9717E-003,
     &                       -5.9281E-004, 5.9044E-003,-1.5221E-003,
     &                       -1.0024E-003, 4.3640E-003,-1.2153E-005,
     &                       -4.6287E-004,-1.4742E-003, 3.7337E-004,
     &                        1.2299E-004,-1.8383E-004, 3.4953E-004,
     &                       -1.3791E-003/
C     2400km equinox
      DATA (DNEH(4,1,J),J=1,49)/ 1.0044E+001,-4.8215E-010,-3.5760E-001,
     &                        2.1775E-008, 8.2508E-002, 4.7923E-009,
     &                        9.4327E-002,-5.4271E-002, 5.2633E-010,
     &                        1.1187E-002,-5.1303E-009,-2.1555E-002,
     &                        5.7277E-010,-7.2596E-003,-6.4714E-010,
     &                       -1.0944E-002, 1.2469E-009, 1.2361E-003,
     &                        3.2732E-011,-6.0089E-002, 5.9490E-010,
     &                       -2.4869E-003,-1.2181E-010,-8.3456E-004,
     &                        2.1390E-002, 3.7568E-010, 9.9164E-003,
     &                       -1.1158E-009, 8.9177E-004, 1.3681E-002,
     &                        3.0065E-010, 2.6197E-003,-2.8667E-010,
     &                        1.2303E-002,-2.7819E-010, 1.0276E-003,
     &                       -4.8067E-010, 1.8107E-002,-6.3629E-011,
     &                        1.4160E-003, 1.3259E-002,-4.2105E-010,
     &                        3.9284E-004,-1.1911E-003, 1.2981E-011,
     &                       -7.4254E-004, 2.7134E-011, 3.2463E-003,
     &                       -3.0059E-003/
C     2400km June solstice
      DATA (DNEH(4,2,J),J=1,49)/ 9.8437E+000, 2.8312E-001,-4.7686E-001,
     &                        1.4238E-001,-9.6995E-003,-4.8066E-002,
     &                        1.2030E-001,-7.8194E-002, 1.3805E-002,
     &                        8.2470E-003, 4.6119E-003,-3.6194E-002,
     &                        2.8395E-003,-2.6545E-002, 1.1682E-003,
     &                        1.1891E-002,-2.4794E-003, 1.4748E-002,
     &                       -9.9502E-003, 1.8415E-002,-3.3637E-002,
     &                        1.0990E-002,-1.0111E-002, 2.9349E-003,
     &                       -2.5495E-002,-4.3211E-002, 1.1705E-002,
     &                       -1.0655E-002, 5.1037E-003, 1.5616E-002,
     &                        3.1856E-003,-3.6023E-003,-9.4473E-004,
     &                        8.5888E-003, 2.4022E-003, 5.5707E-003,
     &                       -1.6500E-005,-5.7463E-003, 5.2602E-003,
     &                       -1.0524E-003,-4.5751E-003, 3.0727E-003,
     &                       -1.6474E-003,-3.1363E-003, 4.2265E-004,
     &                       -5.1937E-004, 1.2478E-003,-2.1649E-003,
     &                       -5.2399E-004/
C////////////////////////////////////////////////////////////////////////////////////
C/////////////////////coefficients low solar activity//////////////////////////////// 
C//////////////////////////////////Ne////////////////////////////////////////////////
C     400km equinox
      DATA (DNEL(1,1,J),J=1,49)/ 1.1062E+001,-3.0911E-008,-3.8235E-001,
     &                        1.1313E-008, 1.4829E-001,-6.3573E-008,
     &                       -3.1902E-002,-2.4123E-001, 2.3955E-009,
     &                       -3.4781E-002, 1.3383E-009,-2.4546E-002,
     &                        1.8658E-009,-2.6632E-001, 4.8835E-009,
     &                        2.9025E-003,-4.6079E-009,-3.2911E-002,
     &                        2.0026E-009, 7.6453E-002, 8.8255E-010,
     &                        1.0963E-003,-2.9776E-010,-1.2170E-003,
     &                       -3.8531E-002,-1.7732E-010,-1.2508E-003,
     &                        6.1993E-010,-1.9205E-004,-9.1547E-003,
     &                        6.1475E-010, 3.1384E-003,-3.7043E-010,
     &                        3.6801E-002, 1.1714E-009, 3.7754E-003,
     &                       -2.6027E-010,-2.0821E-003, 3.3725E-010,
     &                        1.4911E-003, 8.7638E-003, 4.5218E-010,
     &                        3.2066E-004,-3.6726E-003, 1.0395E-010,
     &                        2.9062E-003, 2.7830E-010,-3.7388E-003,
     &                        3.3715E-003/
C     400km June solstice
      DATA (DNEL(1,2,J),J=1,49)/ 1.0967E+001, 4.0368E-001,-3.0547E-001,
     &                       -1.3178E-001, 1.2210E-001, 4.7599E-002,
     &                        4.5964E-002,-2.6940E-001, 5.0649E-002,
     &                        2.0835E-002, 1.3125E-002,-2.6758E-002,
     &                        8.3855E-003,-3.0743E-001,-3.5169E-002,
     &                        6.8584E-002, 2.2068E-002,-2.2965E-002,
     &                       -1.2112E-002, 5.6553E-002,-5.7016E-003,
     &                        8.7370E-003,-3.0176E-003, 9.2492E-004,
     &                       -4.2430E-002,-1.4924E-002, 7.4930E-003,
     &                       -5.1690E-003, 9.9006E-004, 1.5210E-002,
     &                       -7.8933E-004,-2.1342E-003,-4.1959E-004,
     &                        9.1102E-003, 1.4167E-003,-3.0343E-003,
     &                        5.1290E-004,-1.7130E-002, 3.6913E-003,
     &                       -3.4739E-004, 8.7815E-004, 5.5685E-003,
     &                       -1.2357E-003,-2.1451E-002, 2.5966E-003,
     &                       -8.2735E-003, 2.6567E-003,-1.7557E-002,
     &                       -3.7272E-003/
C     650km equinox
      DATA (DNEL(2,1,J),J=1,49)/ 1.0410E+001,-5.5863E-008,-2.8735E-001,
     &                       -6.7549E-008,-3.9147E-002,-7.4702E-008,
     &                       -7.7560E-002,-2.6770E-001, 2.6217E-009,
     &                       -2.6780E-002,-1.8328E-009,-3.8370E-002,
     &                        1.1320E-009,-1.8013E-001, 3.8029E-009,
     &                       -3.1418E-002,-6.7073E-009,-5.7161E-002,
     &                        2.4874E-010, 5.9222E-002,-1.2232E-009,
     &                       -1.4529E-003, 9.9371E-012, 3.6985E-003,
     &                       -3.9165E-003, 3.3004E-009, 3.8050E-003,
     &                       -7.6240E-010,-4.0473E-003, 1.3918E-002,
     &                       -9.8974E-011, 1.0506E-003,-8.2940E-010,
     &                        1.9068E-002, 3.1352E-009, 7.3603E-003,
     &                        6.3141E-012, 1.8285E-005, 2.3434E-010,
     &                        3.9440E-004, 8.8034E-003, 1.1880E-009,
     &                        2.1945E-003,-4.3360E-003, 1.1249E-010,
     &                        6.2337E-004,-9.1034E-011,-2.4846E-003,
     &                        3.3931E-003/
C     650km June solstice
      DATA (DNEL(2,2,J),J=1,49)/ 1.0407E+001, 3.5150E-001,-2.6152E-001,
     &                        3.8210E-002, 2.5637E-002,-3.1519E-002,
     &                        9.3065E-002,-2.0338E-001, 2.4015E-002,
     &                        4.8189E-002,-1.2310E-002,-1.2074E-002,
     &                        6.3295E-003,-1.6583E-001,-2.1338E-002,
     &                        3.9239E-002, 5.3636E-003,-1.2633E-002,
     &                        1.4400E-003, 6.8102E-002,-6.3957E-003,
     &                        9.0268E-003,-8.9534E-003, 7.3362E-004,
     &                       -3.8612E-002,-1.8755E-002, 5.3402E-003,
     &                        4.7158E-003, 7.3675E-004, 2.7280E-002,
     &                        1.7650E-003,-3.5763E-003, 1.7527E-003,
     &                       -1.7144E-002,-1.3996E-003, 2.7811E-003,
     &                       -1.4789E-003,-2.1127E-003, 4.0656E-003,
     &                        2.7334E-004,-2.1757E-003, 2.7670E-003,
     &                       -4.2398E-005,-1.2504E-002, 5.5083E-004,
     &                       -6.3574E-003, 1.9005E-003,-1.4233E-002,
     &                       -2.8291E-003/
C     1000km equinox
      DATA (DNEL(3,1,J),J=1,49)/ 1.0158E+001,-1.9548E-008,-3.6749E-001,
     &                       -8.0357E-009, 2.5141E-002,-3.2793E-008,
     &                        6.4338E-002,-2.0881E-001, 2.2857E-009,
     &                       -1.8004E-002, 4.1065E-009,-9.2270E-003,
     &                        2.3974E-009,-1.4469E-001, 5.8152E-010,
     &                       -7.1135E-002, 2.8358E-009,-2.5824E-002,
     &                        1.2833E-009, 4.4022E-002, 1.9468E-009,
     &                        1.7507E-002,-1.7886E-009, 1.6676E-003,
     &                        5.6214E-003, 1.7403E-009, 6.8439E-003,
     &                       -9.6757E-010, 5.7429E-005, 4.1441E-003,
     &                        4.7678E-010, 2.6270E-003, 5.0045E-011,
     &                        2.6003E-002, 1.9442E-009, 6.1356E-003,
     &                       -1.8709E-010,-4.2284E-003,-5.4793E-010,
     &                       -8.4581E-004, 1.1288E-002,-5.9374E-010,
     &                       -7.3269E-004,-1.6501E-003, 1.2251E-010,
     &                        8.1305E-004,-3.6491E-010,-1.5680E-003,
     &                       -4.4728E-005/
C     1000km June solstice
      DATA (DNEL(3,2,J),J=1,49)/ 1.0166E+001, 2.3694E-001,-2.7429E-001,
     &                        1.1520E-001, 3.0762E-002,-8.2451E-002,
     &                        1.3504E-001,-1.4885E-001, 6.3431E-003,
     &                        7.2066E-002,-1.4211E-002,-1.2608E-002,
     &                        1.2061E-002,-1.0051E-001, 7.1648E-004,
     &                        2.9195E-002, 5.3905E-003,-1.7088E-002,
     &                        1.4637E-002, 2.1114E-002,-7.9212E-003,
     &                        8.4367E-003,-1.6341E-002, 4.9292E-003,
     &                       -2.2852E-002, 1.2029E-004, 2.7544E-004,
     &                        1.1996E-002,-1.6057E-005, 3.1870E-002,
     &                        3.6162E-003,-3.2132E-003, 1.4402E-003,
     &                        1.3314E-002,-1.8461E-003, 5.8733E-003,
     &                       -2.8286E-003,-6.1876E-003, 5.2857E-003,
     &                       -1.4054E-003,-4.1018E-003,-3.0424E-004,
     &                        3.4306E-004,-6.4164E-003, 3.4624E-004,
     &                       -3.5857E-003, 1.5636E-003,-6.0995E-003,
     &                       -5.5164E-003/
C////////////////////////////////////////////////////////////////////////////////////
C///////////////////////////////solar minimum////////////////////////////////////////
      CALL NELOW(CRD,INVDIP,FL,DIMO,B0,DIPL,MLT,ALT,DDD,DNEL,NNEL)
C///////////////////////////////solar maximum////////////////////////////////////////
      CALL NEHIGH(CRD,INVDIP,FL,DIMO,B0,DIPL,MLT,ALT,DDD,DNEH,NNEH)
C     interpolation (in logarithm)
      NNE=(ALOG10(NNEH)-ALOG10(NNEL))/(200.0-85.0)*(F107-85.0)+
     &     ALOG10(NNEL)
      NNE=10**NNE
      RETURN
      END

      SUBROUTINE NELOW(CRD,INVDIP,FL,DIMO,B0,
     &                   DIPL,MLT,ALT,DDD,D,NNE)
C NELOW calculates electron density in the outer
C ionosphere for a low solar activity (F107 < 100).
C Based on spherical harmonics approximation of electron ion density
C (by AE-C, AE-D, and AE-E) at altitudes centred on 400km, 650km, and 1000km.
C For intermediate altitudes an interpolation is used. 
C Recommended altitude range: 350-2000 km!!!
C For days between seasons centred at (21.3. = 79; 21.6. = 171;
C 23.9. 265; 21.12. = 354) relative ion density is linearly interpolated.
C Inputs: CRD - 0 .. INVDIP
C               1 .. FL, DIMO, B0, DIPL (used for calculation INVDIP inside)
C         INVDIP - "mix" coordinate of the dip latitude and of
C                    the invariant latitude;
C                    positive northward, in deg, range <-90.0;90.0>
C         FL, DIMO, BO - McIlwain L parameter, dipole moment in
C                        Gauss, magnetic field strength in Gauss -
C                        parameters needed for invariant latitude
C                        calculation
C         DIPL - dip latitude
C                positive northward, in deg, range <-90.0;90.0>
C         MLT - magnetic local time (central dipole)
C               in hours, range <0;24)
C         ALT - altitude above the Earth's surface;
C               in km, range <350;2000>
C         DDD - day of year; range <0;365>
C         D - coefficints of spherical harmonics for a given ion
C Output: NNE - Ne density 
      REAL INVDIP,FL,DIMO,B0,DIPL,MLT,ALT,NNE
      INTEGER CRD,DDD
      DIMENSION  D(3,3,49),MIRREQ(49)
      REAL INVDP,INVDPC,DTOR
      REAL RMLT,RCOLAT
      REAL C(49)
      INTEGER SEZA,SEZB,SEZAI,SEZBI,DDDA,DDDB,DDDD
      REAL N0A400,N0B400,N400A,N400B,N400
      REAL N0A650,N0B650,N650A,N650B,N650
      REAL N0A100,N0B100,N100A,N100B,N1000
	REAL ANO(3),AH(3),DNO(1),ST(2)
	COMMON/ARGEXP/ARGMAX
	DATA (MIRREQ(J),J=1,49)/
     &            1,-1, 1,-1, 1,-1, 1, 1,-1, 1,-1, 1,-1, 1,-1, 1,-1,
     &            1,-1, 1,-1, 1,-1, 1, 1,-1, 1,-1, 1, 1,-1, 1,-1, 1,
     &           -1, 1,-1, 1,-1, 1, 1,-1, 1, 1,-1, 1,-1, 1, 1/
C////////////////////////////////////////////////////////////////////////////////////
      DTOR=3.1415926536/180.0
C     coefficients for mirroring
      DO 10 I=1,49
       D(1,3,I)=D(1,2,I)*MIRREQ(I)
       D(2,3,I)=D(2,2,I)*MIRREQ(I)
10     D(3,3,I)=D(3,2,I)*MIRREQ(I)
      IF (CRD .EQ. 1) THEN
       INVDP=INVDPC(FL,DIMO,B0,DIPL,DTOR)
      ELSE IF	(CRD .EQ. 0) THEN
       INVDP=INVDIP
      ELSE
       RETURN
      END IF
      RMLT=MLT*DTOR*15.0
      RCOLAT=(90.0-INVDP)*DTOR
      CALL SPHARM_IK(C,6,6,RCOLAT,RMLT)
C     21.3. - 20.6.
      IF ((DDD .GE. 79) .AND. (DDD .LT. 171)) THEN
       SEZA=1
       SEZB=2
       DDDA=79
       DDDB=171
       DDDD=DDD
      END IF
C     21.6. - 22.9.
      IF ((DDD .GE. 171) .AND. (DDD .LT. 265)) THEN
       SEZA=2
       SEZB=4
       DDDA=171
       DDDB=265
       DDDD=DDD
      END IF
C     23.9. - 20.12.
      IF ((DDD .GE. 265) .AND. (DDD .LT. 354)) THEN
       SEZA=4
       SEZB=3
       DDDA=265
       DDDB=354
       DDDD=DDD
      END IF
C     21.12. - 20.3.
      IF ((DDD .GE. 354) .OR. (DDD .LT. 79)) THEN
       SEZA=3
       SEZB=1
       DDDA=354
       DDDB=365+79
       DDDD=DDD
        IF (DDD .GE. 354) THEN
         DDDD=DDD
        ELSE
         DDDD=DDD+365
        END IF
      END IF
       SEZAI=MOD(SEZA-1,3)+1
       SEZBI=MOD(SEZB-1,3)+1
C     400km level
        N0A400=0.0
        N0B400=0.0
        DO 30 I=1,49
         N0A400=N0A400+C(I)*D(1,SEZAI,I)
30       N0B400=N0B400+C(I)*D(1,SEZBI,I)
        N400A=N0A400
        N400B=N0B400
        N400=(N400B-N400A)/(DDDB-DDDA)*(DDDD-DDDA)+N400A
C     650km level
      N0A650=0.0
	N0B650=0.0
	DO 70 I=1,49
         N0A650=N0A650+C(I)*D(2,SEZAI,I)
70       N0B650=N0B650+C(I)*D(2,SEZBI,I)
	N650A=N0A650
	N650B=N0B650
	N650=(N650B-N650A)/(DDDB-DDDA)*(DDDD-DDDA)+N650A
C     1000km level
      N0A100=0.0
      N0B100=0.0
	DO 110 I=1,49
         N0A100=N0A100+C(I)*D(3,SEZAI,I)
110      N0B100=N0B100+C(I)*D(3,SEZBI,I)
	N100A=N0A100
	N100B=N0B100
	N1000=(N100B-N100A)/(DDDB-DDDA)*(DDDD-DDDA)+N100A

c   n(O+) and n(N+) should not increase above 650 km
c       IF(((ION.eq.0).or.(ION.eq.3)).and.(N1000.gt.N650)) 
c     &       N1000=N650
c   n(H+) should not increase decrease above 650 km
c       if(((ION.eq.1).and.(N1000.gt.N650)) N1000=N650
          
C      IF (ALT .LT. 650) NO=(N650-N400)/250.0*(ALT-400)+N400
C      IF (ALT .GE. 650) NO=(N1000-N650)/350.0*(ALT-650)+N650

C      NION=10**NO

      ANO(1)=N400
	 ANO(2)=N650
	  ANO(3)=N1000
	  
	AH(1)=400.
       AH(2)=650.
        AH(3)=1000.
      
      DNO(1)=20.
      
      ST1=(ANO(2)-ANO(1))/(AH(2)-AH(1))
      I=2
      ST2=(ANO(I+1)-ANO(I))/(AH(I+1)-AH(I))
      ANO(I)=ANO(I)-(ST2-ST1)*DNO(I-1)*ALOG(2.)

      DO 220 I=1,2
220   ST(I)=(ANO(I+1)-ANO(I))/(AH(I+1)-AH(I))

      ARGMAX=88.0
      SUM=ANO(1)+ST(1)*(ALT-AH(1))                     
     
      I=1
	aa = eptr(alt  ,dno(i),ah(i+1))
	bb = eptr(ah(1),dno(i),ah(i+1))
      SUM=SUM+(ST(I+1)-ST(I))*(AA-BB)*DNO(I)
                
      NNE=10**SUM       
      RETURN
      END

      SUBROUTINE NEHIGH(CRD,INVDIP,FL,DIMO,B0,
     &                   DIPL,MLT,ALT,DDD,D,NNE)
C NEHIGH calculates electron density in the outer
C ionosphere for high solar activity (F107 >= 150).
C Based on spherical harmonics approximation of electron density
C (by IK24 and IK25) at altitudes centred on 550km, 900km, 1500km, and 2400km.
C For intermediate altitudes a interpolation is used. 
C Recommended altitude range: 500-3000 km!!!
C For days between seasons centred at (21.3. = 79; 21.6. = 171;
C 23.9. 265; 21.12. = 354) relative ion density is linearly interpolated.
C Inputs: CRD - 0 .. INVDIP
C               1 .. FL, DIMO, B0, DIPL (used for calculation INVDIP inside)
C         INVDIP - "mix" coordinate of the dip latitude and of
C                    the invariant latitude;
C                    positive northward, in deg, range <-90.0;90.0>
C         FL, DIMO, BO - McIlwain L parameter, dipole moment in
C                        Gauss, magnetic field strength in Gauss -
C                        parameters needed for invariant latitude
C                        calculation
C         DIPL - dip latitude
C                positive northward, in deg, range <-90.0;90.0>
C         MLT - magnetic local time (central dipole)
C               in hours, range <0;24)
C         ALT - altitude above the Earth's surface;
C               in km, range <500;3000>
C         DDD - day of year; range <0;365>
C         D - coefficints of spherical harmonics for a given ion
C Output: NNE - electron density [m-3]
      REAL INVDIP,FL,DIMO,B0,DIPL,MLT,ALT,NNE
	INTEGER CRD,DDD
      DIMENSION  D(4,3,49),MIRREQ(49)
      REAL INVDP,INVDPC,DTOR
      REAL RMLT,RCOLAT
      REAL C(49)
      INTEGER SEZA,SEZB,SEZAI,SEZBI,DDDA,DDDB,DDDD
      REAL N0A550,N0B550,N550A,N550B,N550
      REAL N0A900,N0B900,N900A,N900B,N900
      REAL N0A150,N0B150,N150A,N150B,N1500
      REAL N0A250,N0B250,N250A,N250B,N2500
	REAL ANO(4),AH(4),DNO(2),ST(3)
	COMMON/ARGEXP/ARGMAX
	DATA (MIRREQ(J),J=1,49)/
     &            1,-1, 1,-1, 1,-1, 1, 1,-1, 1,-1, 1,-1, 1,-1, 1,-1,
     &            1,-1, 1,-1, 1,-1, 1, 1,-1, 1,-1, 1, 1,-1, 1,-1, 1,
     &           -1, 1,-1, 1,-1, 1, 1,-1, 1, 1,-1, 1,-1, 1, 1/
C////////////////////////////////////////////////////////////////////////////////////
      DTOR=3.1415926536/180.0
C     coefficients for mirroring
      DO 10 I=1,49
       D(1,3,I)=D(1,2,I)*MIRREQ(I)
       D(2,3,I)=D(2,2,I)*MIRREQ(I)
       D(3,3,I)=D(3,2,I)*MIRREQ(I)
10     D(4,3,I)=D(4,2,I)*MIRREQ(I)
      IF (CRD .EQ. 1) THEN
       INVDP=INVDPC(FL,DIMO,B0,DIPL,DTOR)
      ELSE IF	(CRD .EQ. 0) THEN
       INVDP=INVDIP
      ELSE
       RETURN
      END IF
      RMLT=MLT*DTOR*15.0
      RCOLAT=(90.0-INVDP)*DTOR
      CALL SPHARM_IK(C,6,6,RCOLAT,RMLT)
C     21.3. - 20.6.
      IF ((DDD .GE. 79) .AND. (DDD .LT. 171)) THEN
       SEZA=1
       SEZB=2
       DDDA=79
       DDDB=171
       DDDD=DDD
      END IF
C     21.6. - 22.9.
      IF ((DDD .GE. 171) .AND. (DDD .LT. 265)) THEN
       SEZA=2
       SEZB=4
       DDDA=171
       DDDB=265
       DDDD=DDD
      END IF
C     23.9. - 20.12.
      IF ((DDD .GE. 265) .AND. (DDD .LT. 354)) THEN
       SEZA=4
       SEZB=3
       DDDA=265
       DDDB=354
       DDDD=DDD
      END IF
C     21.12. - 20.3.
      IF ((DDD .GE. 354) .OR. (DDD .LT. 79)) THEN
       SEZA=3
       SEZB=1
       DDDA=354
       DDDB=365+79
       DDDD=DDD
        IF (DDD .GE. 354) THEN
         DDDD=DDD
        ELSE
         DDDD=DDD+365
        END IF
      END IF
       SEZAI=MOD(SEZA-1,3)+1
       SEZBI=MOD(SEZB-1,3)+1
C     550km level
        N0A550=0.0
        N0B550=0.0
        DO 30 I=1,49
         N0A550=N0A550+C(I)*D(1,SEZAI,I)
30       N0B550=N0B550+C(I)*D(1,SEZBI,I)
        N550A=N0A550
        N550B=N0B550
        N550=(N550B-N550A)/(DDDB-DDDA)*(DDDD-DDDA)+N550A
C     900km level
      N0A900=0.0
	N0B900=0.0
	DO 70 I=1,49
         N0A900=N0A900+C(I)*D(2,SEZAI,I)
70       N0B900=N0B900+C(I)*D(2,SEZBI,I)
	N900A=N0A900
	N900B=N0B900
	N900=(N900B-N900A)/(DDDB-DDDA)*(DDDD-DDDA)+N900A
C     1500km level
      N0A150=0.0
      N0B150=0.0
	DO 110 I=1,49
         N0A150=N0A150+C(I)*D(3,SEZAI,I)
110      N0B150=N0B150+C(I)*D(3,SEZBI,I)
	N150A=N0A150
	N150B=N0B150
	N1500=(N150B-N150A)/(DDDB-DDDA)*(DDDD-DDDA)+N150A
C     2500km level
      N0A250=0.0
	N0B250=0.0
       DO 150 I=1,49
        N0A250=N0A250+C(I)*D(4,SEZAI,I)
150     N0B250=N0B250+C(I)*D(4,SEZBI,I)
       N250A=N0A250
       N250B=N0B250
       N2500=(N250B-N250A)/(DDDB-DDDA)*(DDDD-DDDA)+N250A

C      IF (ALT .LT. 900) NO=(N900-N550)/350.0*(ALT-550)+N550
C      IF ((ALT .GE. 900) .AND. (ALT .LT. 1500))
C     &  NO=(N1500-N900)/600.0*(ALT-900)+N900
c      IF (ALT .GE. 1500) NO=(N2500-N1500)/1000.0*(ALT-1500)+N1500

      ANO(1)=N550
	 ANO(2)=N900
	  ANO(3)=N1500
	   ANO(4)=N2500

	AH(1)=550.
       AH(2)=900.
        AH(3)=1500.
         AH(4)=2250.
      DNO(1)=20.
       DNO(2)=20.

      ST1=(ANO(2)-ANO(1))/(AH(2)-AH(1))
      DO 200 I=2,3
       ST2=(ANO(I+1)-ANO(I))/(AH(I+1)-AH(I))
       ANO(I)=ANO(I)-(ST2-ST1)*DNO(I-1)*ALOG(2.)
200   ST1=ST2

      DO 220 I=1,3
220   ST(I)=(ANO(I+1)-ANO(I))/(AH(I+1)-AH(I))

      ARGMAX=88.0
      SUM=ANO(1)+ST(1)*(ALT-AH(1))                     
     
      DO 230 I=1,2
	aa = eptr(alt  ,dno(i),ah(i+1))
	bb = eptr(ah(1),dno(i),ah(i+1))
230   SUM=SUM+(ST(I+1)-ST(I))*(AA-BB)*DNO(I)
                
      NNE=10**SUM       
      RETURN
      END
C
C                     
C**********************************************************                     
C***************** ELECTRON TEMPERATURE ********************                    
C**********************************************************                     
C
      SUBROUTINE ELTEIK(CRD,F107Y,SEASY,INVDIP,FL,DIMO,B0,
     &                   DIPL,MLT,ALT,DDD,F107,TE,SIGTE)
C Version 2000 (released 30.12.1999)
C Empirical model of electron temperature (Te) in the outer ionosphere
C for high solar activity conditions (F107 >= 100).
C Based on spherical harmonics approximation of measured
C Te (by IK19, IK24, and IK25 satellites) at altitudes centred on 550km,
C 900km, 1500km, and 2500km. For intermediate altitudes a linear
C interpolation is used. Recommended altitude range: 500-3000 km!!!
C Linear extrapolation is used for altitude ranges <500;550)km
C and (2500;3000> km. For days between seasons centred at
C (21.3. = 79; 21.6. = 171; 23.9. 265; 21.12. = 354) Te is
C linearly interpolated.
C Inputs: CRD - 0 .. INVDIP
C               1 .. FL, DIMO, B0, DIPL (used for calculation INVDIP 
C						inside)
C         F107Y - 0 .. F107 correction NOT included
C                 1 .. F107 correction included
C         SEASY - 0 .. seasonal correction NOT included
C                 1 .. seasonal correction included
C         INVDIP - "mix" coordinate of the dip latitude and of
C                    the invariant latitude;
C                    positive northward, in deg, range <-90.0;90.0>
C         FL, DIMO, BO - McIlwain L parameter, dipole moment in
C                        Gauss, magnetic field strength in Gauss -
C                        parameters needed for invariant latitude
C                        calculation
C         DIPL - dip latitude
C                positive northward, in deg, range <-90.0;90.0>
C         MLT - magnetic local time (central dipole)
C               in hours, range <0;24)
C         ALT - altitude above the Earth's surface;
C               in km, range <500;3000>
C         DDD - day of year; range <0;365>
C         F107 - daily solar radio flux;
C                high solar activity; range <100;250>
C Output: TE - electron temperature in K
C         SIGTE - standard deviation of TE in K
C                 (not yet included)
C Versions: 1.00 (IDL) the first version Te=Te(invl,mlt,alt,season)
C           1.50 (IDL) corrected IK19 Te at 900km for possible 
C                      Ne > 2E11 m-3
C           2.00 (IDL) F107 included as a linear perturbation on global 
C			Te pattern
C                      Te=Te(invlat,mlt,alt,season,F107)
C           3.00 (IDL) invdipl introduced
C           2000 (IDL,FORTRAN) correction for seasons included
C Authors of the model (Te measurements (1), data processing (2), model
C                       formulation and building (3)):
C                V. Truhlik (2,3), L. Triskova (2,3), J. Smilauer (1,2),
C                          (Institute of Atm. Phys., Prague)
C                                 and V.V. Afonin (1)
C                                      (IKI, Moscow)
C Author of the code:
C         Vladimir Truhlik
C         Institute of Atm. Phys.
C         Bocni II.
C         141 31 Praha 4, Sporilov
C         Czech Republic
C         e-mail: vtr@ufa.cas.cz
C         tel/fax: +420 67103058, +420 602 273111 / +420 72 762528
      REAL INVDIP,FL,DIMO,B0,DIPL,MLT,ALT,F107,TE,SIGTE
      INTEGER CRD,F107Y,SEASY,DDD
      DIMENSION  D(4,3,81),MIRREQ(81),FA(4,3,49),FB(4,3,49),MF107(49),
     &           SZ(4,4,25)
      DOUBLE PRECISION B(8),A
      REAL DTOR,ASA,INVL,RINVL,INVDP,RDIPL,ALFA,BETA
      REAL RMLT,RCOLAT
      REAL C(82),CF107(49),CSZ(25)
      INTEGER SEZA,SEZB,SEZAI,SEZBI,DDDA,DDDB,DDDD
      REAL T0A550,T0B550,T1A550,T1B550,T2A550,T2B550,
     &     T3A550,T3B550,T550A,T550B,T550
      REAL T0A900,T0B900,T1A900,T1B900,T2A900,T2B900,
     &     T3A900,T3B900,T900A,T900B,T900
      REAL T0A150,T0B150,T1A150,T1B150,T2A150,T2B150,
     &     T3A150,T3B150,T150A,T150B,T1500
      REAL T0A250,T0B250,T1A250,T1B250,T2A250,T2B250,
     &     T3A250,T3B250,T250A,T250B,T2500
      DATA B/1.259921D0  ,-0.1984259D0 ,-0.04686632D0,-0.01314096D0,
     &      -0.00308824D0, 0.00082777D0,-0.00105877D0, 0.00183142D0/
C//////////////////////coefficients - main model part///////////////
      DATA (MIRREQ(J),J=1,81)/
     &  1,-1, 1,-1, 1,-1, 1,-1, 1, 1,-1, 1,-1, 1,-1, 1,-1, 1,-1, 1,-1,
     &  1,-1, 1,-1, 1,-1, 1,-1, 1,-1, 1, 1,-1, 1,-1, 1,-1, 1, 1,-1, 1,
     & -1, 1,-1, 1,-1, 1,-1, 1,-1, 1,-1, 1,-1, 1, 1,-1, 1,-1, 1, 1,-1,
     &  1,-1, 1,-1, 1,-1, 1,-1, 1, 1,-1, 1, 1,-1, 1,-1, 1, 1/
C     550km equinox
      DATA (D(1,1,J),J=1,81)/ 2.1185E+03,-9.0960E-01, 8.7479E+02,
     &                        5.3665E-01, 7.2315E+01,-1.4522E+00,
     &                       -6.8231E+01,-1.7205E-01,-9.5868E+01,
     &                       -5.3740E+02,-5.5762E-01,-1.3700E+02,
     &                       -6.6880E-01,-6.4616E-01,-1.4436E-02,
     &                       -1.3574E+01,-5.1068E-03, 1.9176E+02,
     &                       -3.5708E-01,-1.6800E+01, 5.3899E-01,
     &                        2.7730E+01, 2.1264E-01,-1.1162E+00,
     &                       -2.1020E-01,-4.7027E+02,-2.2692E-01,
     &                       -8.4973E+00,-1.2278E-01, 4.9200E+00,
     &                       -1.4939E-02, 2.6356E+00,-9.3732E+01,
     &                        1.7524E-01,-8.0270E+00, 4.8917E-02,
     &                        4.1404E+00, 2.1455E-01, 6.5988E-01,
     &                        2.3973E+02, 6.7066E-03, 3.2942E+01,
     &                        1.0491E-01,-1.1019E+00,-8.9674E-03,
     &                       -2.5871E+02,-4.0511E-01, 1.8509E+01,
     &                       -6.5590E-02, 3.3369E+00,-1.4607E-02,
     &                        1.9345E+02, 4.5946E-01,-1.7988E+01,
     &                        6.5776E-02,-1.7171E+00, 3.2232E+01,
     &                       -1.3975E-01,-7.9626E+00,-8.9984E-02,
     &                       -5.2076E-01,-6.7466E+01,-2.4359E-02,
     &                        1.2132E+00,-9.7418E-04, 1.6662E+02,
     &                        3.0197E-01, 3.4870E-01, 2.0862E-03,
     &                       -1.0091E+02,-1.1583E-01,-6.4353E-01,
     &                       -4.5914E+01,-2.9075E-01,-1.3476E+00,
     &                       -2.5206E+01, 5.5027E-02,-1.0663E+02,
     &                        1.3394E-01, 2.7825E+01, 3.3973E+01/
C     550km June solstice
      DATA (D(1,2,J),J=1,81)/ 2.0732E+03, 1.1858E+02, 7.0341E+02,
     &                        4.0606E+02,-1.5410E+02, 5.9091E+01,
     &                       -7.3198E+01,-1.0067E+02,-7.0712E+01,
     &                       -6.3323E+02, 4.7818E+01,-7.6003E+01,
     &                       -4.9778E+01, 6.7586E+01,-7.3829E+00,
     &                       -7.0501E+00, 1.3663E+01, 1.2661E+02,
     &                       -4.9014E+01,-4.4884E+01,-2.7004E+01,
     &                       -5.4925E+00,-1.4390E+01, 1.4573E+01,
     &                        1.4890E+01,-2.2243E+02, 4.6961E+01,
     &                        2.1169E+01, 6.3799E-01,-1.4838E+00,
     &                        6.6321E+00, 4.1318E-01,-1.5642E+02,
     &                        1.1300E+02,-8.9014E+00,-4.9933E+00,
     &                        7.3107E+00, 9.4305E+00,-1.5203E+00,
     &                        1.9325E+02,-2.2070E+01,-6.2284E+00,
     &                       -1.0572E-01,-3.6677E+00,-6.1689E+00,
     &                       -6.8242E+01,-2.9507E+01, 1.3496E+01,
     &                        7.9801E+00,-1.7483E+00, 4.2264E-01,
     &                        1.3232E+02, 9.7530E+00,-3.3159E+00,
     &                       -3.8368E+00, 7.3997E-01, 1.2738E+02,
     &                        4.3677E-01,-4.3172E-01, 3.2624E+00,
     &                        2.4603E-01,-1.5297E+02, 3.8130E-01,
     &                       -5.6753E+00,-1.7841E-01, 1.4078E+01,
     &                        8.1371E+00,-1.0298E+00,-8.7611E-01,
     &                        1.8043E+01,-2.0388E+01, 3.5874E+00,
     &                       -6.8187E+01, 1.2396E+01, 1.4384E+00,
     &                       -1.8542E+01, 9.8107E+00,-1.7652E+00,
     &                       -1.0545E+01, 1.2793E+01, 2.4878E+01/
C     900km equinox
      DATA (D(2,1,J),J=1,81)/ 2.6253E+03,-1.4102E+00, 1.0578E+03,
     &                        6.0533E-01,-2.0122E+02,-2.5531E+00,
     &                       -9.8647E+01, 1.5826E+00,-1.4076E+02,
     &                       -9.1764E+02, 5.2004E-01,-1.0194E+02,
     &                       -5.3743E-01, 4.6185E+01, 3.0271E-01,
     &                       -1.6661E+01,-4.3778E-01, 2.7636E+02,
     &                        9.9112E-02,-2.7807E+01,-3.6236E-01,
     &                        9.2380E+01, 5.5131E-01,-5.7407E+00,
     &                        1.2985E-01,-5.1480E+02,-3.2325E-01,
     &                       -4.2135E+01, 4.6360E-02,-4.7321E-01,
     &                       -2.0887E-01,-2.3466E+00,-3.4132E+02,
     &                       -8.1632E-01, 2.0252E+01, 3.1349E-01,
     &                       -3.2548E+00,-2.9720E-02,-2.5120E+00,
     &                        3.1281E+02,-4.4738E-02, 4.4130E+00,
     &                       -3.1125E-02, 1.3784E+00, 4.1569E-02,
     &                       -1.0980E+02, 7.0001E-01, 4.0861E+00,
     &                       -1.6670E-01,-4.8520E+00, 3.6492E-02,
     &                        1.8057E+02, 2.0765E-01, 8.8382E+00,
     &                       -2.8354E-02,-1.6340E+00, 1.7492E+02,
     &                       -7.0915E-01, 9.6069E+00, 1.2205E-01,
     &                        3.7218E-01,-5.7398E+01,-2.9121E-01,
     &                       -4.3088E+00, 5.6543E-02, 6.0417E+01,
     &                        2.5141E-01, 2.2588E+00,-6.5538E-02,
     &                       -1.6084E+02, 3.2163E-01,-3.5354E+00,
     &                       -3.9077E+01,-4.4285E-02,-3.4426E+00,
     &                        2.6798E+01, 6.7368E-02,-6.7479E+01,
     &                        9.8125E-02, 7.0079E+01, 2.1724E+01/
C     900km June solstice
      DATA (D(2,2,J),J=1,81)/ 2.5964E+03, 2.7369E+02, 7.1893E+02,
     &                        4.4751E+02,-3.8908E+02, 6.4097E+00,
     &                       -5.0443E+01, 7.9006E+01,-9.5540E+01,
     &                       -9.8162E+02, 9.4112E+00,-3.9071E+01,
     &                       -1.2918E+01, 6.0870E+01, 2.3039E+01,
     &                       -3.1332E+01, 8.3557E+00, 1.3100E+02,
     &                       -3.2576E+01,-9.6546E+01, 5.8694E+00,
     &                        7.1925E+01, 1.0059E+01,-9.7102E+00,
     &                       -1.8709E-01,-3.9806E+02,-3.0957E+01,
     &                       -7.8514E+00, 2.9864E+00, 4.7169E+00,
     &                       -6.3766E+00, 4.5467E+00,-2.1713E+02,
     &                        2.7924E+01, 6.9898E+00, 6.4391E+00,
     &                       -2.9495E+00, 1.3537E+00, 3.0092E+00,
     &                        2.4662E+02,-4.3575E+00,-8.7582E+00,
     &                       -5.6902E+00,-1.8440E+00, 2.0276E+00,
     &                       -1.5456E+02,-4.1566E+00, 1.5940E+00,
     &                       -2.2610E+00,-1.8790E+00,-2.0589E-01,
     &                        1.6612E+02,-2.8783E+00,-9.7839E-02,
     &                        7.1863E-01, 2.2400E-01, 8.9569E+01,
     &                       -8.7802E+00,-2.9563E+00,-1.5339E+00,
     &                       -6.9508E-01,-8.9068E+01, 2.4977E+00,
     &                        5.6978E-01, 1.0112E+00, 1.0725E+02,
     &                       -8.8621E+00,-4.8559E+00,-8.5244E-01,
     &                       -6.2185E+01, 7.2504E+00, 1.8318E+00,
     &                       -7.9203E+01, 2.8472E+00,-1.3483E-01,
     &                        2.1818E+01,-4.2387E+00,-5.7925E+01,
     &                        2.5552E+00, 4.6600E+01, 3.4333E+01/
C     1500km equinox
      DATA (D(3,1,J),J=1,81)/ 2.9228E+03,-1.0193E+00, 5.0052E+02,
     &                        1.9156E+00,-8.8798E+01, 4.4598E+00,
     &                        2.2407E+01, 2.3050E+00,-3.8484E+00,
     &                       -1.2959E+03,-8.7330E-01, 2.7027E+01,
     &                       -6.0724E-01, 2.8496E+01,-5.2927E-01,
     &                        5.7042E+00,-2.5504E-01,-3.9328E+01,
     &                        1.8577E+00, 2.5325E+01, 1.0570E+00,
     &                        3.9141E+01, 1.2857E-02, 4.4854E+00,
     &                       -1.0437E+00,-4.9104E+02,-6.2939E-02,
     &                       -4.1205E+01, 3.2220E-01,-1.0599E+01,
     &                       -1.8242E-01, 9.1160E-01,-3.5425E+02,
     &                       -1.6068E-01,-1.3725E+00, 9.6252E-02,
     &                        6.6323E+00, 1.0111E-01, 1.5864E+00,
     &                        2.6698E+02,-1.3164E-01, 2.5957E+01,
     &                        1.8661E-01, 9.5289E-01, 1.2827E-01,
     &                       -2.0802E+01,-5.0001E-01, 1.7802E+00,
     &                       -2.2686E-01,-9.9243E-01,-8.8453E-02,
     &                        1.6938E+02, 1.5072E-01,-1.2249E+00,
     &                        5.9212E-03, 2.1008E+00, 1.4981E+02,
     &                       -3.0983E-02,-2.3113E+00,-1.6773E-02,
     &                        5.0314E-01,-2.8870E+01,-1.2562E-01,
     &                        5.5983E+00, 3.6513E-02,-3.2056E+01,
     &                       -3.5823E-01,-7.1938E-01,-3.0424E-02,
     &                       -5.9756E+01,-3.1999E-01, 3.8169E-01,
     &                        5.7626E+01, 3.4086E-01, 5.0895E+00,
     &                        1.2152E+00, 1.4523E-01,-6.0723E+01,
     &                       -1.5587E-02, 5.2597E+01,-6.7261E+00/
C     1500km June solstice
      DATA (D(3,2,J),J=1,81)/ 2.9621E+03, 3.9688E+02, 2.9652E+02,
     &                        2.7782E+02,-1.3500E+02,-2.8285E+01,
     &                        1.8036E+01,-8.2958E+01,-3.9738E+01,
     &                       -1.2058E+03, 6.8198E+01, 2.2164E+01,
     &                       -1.5473E+01, 1.9760E+01, 1.3793E+01,
     &                       -2.2130E+01, 2.2585E+00,-9.5227E+01,
     &                       -7.9347E+01,-1.8617E+01,-1.4626E+01,
     &                        2.8497E+01, 9.7753E+00, 5.1487E+00,
     &                       -3.0464E+00,-5.0302E+02, 3.2524E+01,
     &                       -6.2727E+00,-2.7864E+00, 4.3374E+00,
     &                       -1.0944E+00, 1.0615E+00,-1.4955E+02,
     &                        4.5911E+00, 1.3417E+01,-3.8776E+00,
     &                       -8.6030E-02, 4.9763E+00, 1.7831E+00,
     &                        2.8593E+02,-4.1944E+01, 2.5430E+00,
     &                        4.1294E+00, 5.5434E+00, 2.1086E+00,
     &                       -7.9921E+01, 8.9160E+00,-1.4931E+01,
     &                        6.7306E+00,-7.4849E+00, 2.0152E+00,
     &                        2.4435E+02,-2.7216E+01,-6.5424E+00,
     &                       -2.2848E-01, 6.6780E-01,-5.0210E+00,
     &                       -1.6876E+00,-3.8484E+00, 4.8596E-01,
     &                        1.2499E+00,-1.2823E+02,-4.7155E+00,
     &                       -8.9457E+00,-2.6414E-01,-1.0880E+01,
     &                        1.7040E+01,-1.3495E+00, 4.6478E-02,
     &                       -5.5357E+01, 2.9895E+01,-2.6035E+00,
     &                       -3.7697E+00,-5.3693E+00, 5.3346E-01,
     &                        1.4172E+01, 5.3848E+00,-2.3065E+01,
     &                       -5.2616E+00, 2.2592E+01,-1.5174E+01/
C     2500km equinox
      DATA (D(4,1,J),J=1,81)/ 3.3738E+03,-1.4579E-01, 2.9057E+02,
     &                       -8.9423E-01,-1.7818E+02, 1.3821E+00,
     &                       -8.9134E+01,-3.1758E-01,-6.7731E+01,
     &                       -1.4372E+03, 6.2781E-02, 4.6214E+01,
     &                       -4.0312E-02,-7.0553E+00,-5.7943E-02,
     &                       -1.5921E+01, 7.6970E-02,-5.3597E+01,
     &                       -3.2922E+00,-5.7514E+01,-2.2513E+00,
     &                        2.7316E+01,-4.9774E-01, 1.0223E+01,
     &                        1.5541E-01,-5.1080E+02, 1.5422E-01,
     &                       -2.6147E+01, 4.5060E-01, 7.7452E-01,
     &                       -3.2680E-02,-7.4846E-01,-2.1399E+02,
     &                       -1.0117E+00, 1.5173E+01,-7.2407E-01,
     &                        3.2157E+00,-9.5540E-02, 2.0921E+00,
     &                        3.3803E+02, 3.2007E-01,-2.5597E+01,
     &                       -1.8768E-02,-4.3258E+00,-9.4149E-02,
     &                       -1.1274E+02,-5.5848E-01,-1.9836E+01,
     &                       -1.9519E-01,-2.9500E+00,-6.5675E-02,
     &                        2.7506E+02,-9.4098E-02, 1.8588E+01,
     &                       -1.5460E-01, 8.1344E-01, 1.8479E+02,
     &                       -3.9938E-01, 1.3448E+01, 8.8669E-02,
     &                        3.9040E+00,-5.3105E+01, 2.0753E-01,
     &                       -3.5771E+00,-1.4386E-02, 1.3306E+02,
     &                        2.2388E-01,-4.3013E-01, 5.8347E-02,
     &                       -7.2119E+01, 1.5187E-03, 2.1445E+00,
     &                        1.5871E+00, 3.4653E-01, 1.8858E+00,
     &                        1.1009E+01,-1.0913E-01, 1.3126E-01,
     &                        1.6358E-01, 3.3089E+01, 4.9158E+01/
C     2500km June solstice
      DATA (D(4,2,J),J=1,81)/ 3.2890E+03, 2.3406E+02, 2.3242E+02,
     &                        1.6845E+02,-3.7964E+02,-2.3183E+01,
     &                       -6.7725E+01, 9.8597E+01, 2.8565E+01,
     &                       -1.3705E+03, 7.5544E+01, 1.3235E+02,
     &                        1.9927E-01,-1.3429E+00,-9.8459E-01,
     &                       -1.7632E+00,-4.2301E+00,-2.1162E+02,
     &                        5.5279E-01,-9.5367E+01,-1.2788E+01,
     &                        4.0782E+01, 4.8734E+00,-5.0869E+00,
     &                       -4.8522E+00,-5.2566E+02, 6.0198E+01,
     &                       -2.0405E+01, 3.0107E+01, 1.9692E+00,
     &                        4.2044E+00,-1.6327E+00,-1.1121E+02,
     &                       -3.5375E+01, 5.2049E+00, 1.5725E+01,
     &                        1.0054E+01, 3.7913E+00,-1.6254E-01,
     &                        1.4983E+02,-4.0229E+01, 1.9216E+00,
     &                       -2.5669E+00,-1.3679E+00, 1.9556E+00,
     &                       -5.2991E+01, 1.2377E+01,-1.8352E+01,
     &                       -2.7843E-01,-2.6711E+00, 1.0270E+00,
     &                       -1.7185E+01,-1.0283E+00,-1.3286E+01,
     &                        1.4314E+00,-5.3584E-01,-6.7809E+01,
     &                       -9.6756E+00,-4.6022E+00, 1.8112E+00,
     &                       -6.1062E-03, 2.6330E+01, 1.6726E+01,
     &                        3.9444E+00, 2.7303E+00, 4.7634E+01,
     &                       -3.2729E+01, 1.2863E+00,-8.7681E-01,
     &                        5.8417E+00,-1.1842E+01, 1.7747E+00,
     &                        4.9485E+01,-9.5141E+00,-1.7739E+00,
     &                        2.8452E+01,-8.8851E+00,-4.4171E+00,
     &                        4.1421E+00,-2.6145E+01,-4.4075E+01/
C////////////coefficients - F107 correction//////////////////////
      DATA (MF107(J),J=1,49)/
     &            1,-1, 1,-1, 1,-1, 1, 1,-1, 1,-1, 1,-1, 1,-1, 1,-1,
     &            1,-1, 1,-1, 1,-1, 1, 1,-1, 1,-1, 1, 1,-1, 1,-1, 1,
     &           -1, 1,-1, 1,-1, 1, 1,-1, 1, 1,-1, 1,-1, 1, 1/
C     550km equinox
      DATA (FA(1,1,J),J=1,49)/
     &         2.1792E+00,-2.1275E-07,-3.0369E-01,-3.1861E-07,
     &        -4.0109E-01,-2.3529E-07,-5.1800E-01,-2.1268E-02,
     &         6.5788E-11, 3.2767E-01,-3.9204E-08, 6.0337E-03,
     &         1.8103E-09,-3.1331E-02, 1.3981E-08, 4.8055E-01,
     &        -3.5571E-08, 9.8510E-02, 3.0118E-09,-7.7406E-02,
     &         1.4673E-08,-1.1584E-01, 2.6239E-08,-3.7815E-02,
     &        -7.5343E-01,-4.5728E-09,-2.2223E-01,-1.0071E-09,
     &        -6.8871E-02,-8.8247E-02, 6.3637E-09, 3.7528E-02,
     &        -1.4578E-09, 8.6601E-02, 4.0820E-08, 1.3803E-01,
     &         6.0607E-09,-2.8557E-02,-4.8644E-09, 1.5314E-03,
     &        -8.0197E-02,-1.5931E-08,-1.6002E-02,-2.9124E-02,
     &         4.8067E-09, 9.0446E-03, 1.2273E-08,-3.0496E-02,
     &         7.8786E-03/
	DATA (FB(1,1,J),J=1,49)/
     &        -4.1934E+02, 7.5753E-05, 3.0092E+02, 1.1961E-04,
     &         1.6897E+02, 8.2549E-05, 8.4754E+01,-7.4083E-02,
     &        -1.8769E-06,-8.7576E+01, 1.3779E-05, 7.9487E+00,
     &        -2.3550E-06,-7.1043E+01,-3.4455E-06,-7.5485E+01,
     &         1.3096E-05,-4.7573E+00, 1.9409E-06, 7.5447E+01,
     &        -2.3927E-06, 1.9243E+01, 2.4230E-06, 3.6926E+00,
     &         1.5966E+02, 6.5501E-06, 4.3220E+01,-2.1216E-06,
     &         1.4776E+01, 2.5894E+01,-8.0222E-09,-6.4715E+00,
     &        -3.7197E-07, 1.1891E+01,-9.2441E-06,-2.8838E+01,
     &         2.6401E-06,-8.3915E-01,-6.8336E-07, 4.1298E-01,
     &         2.1763E+01,-1.2271E-06, 2.7614E+00, 8.3170E+00,
     &        -1.7405E-06, 9.7560E-01,-1.4005E-06, 3.5829E+00,
     &         7.5525E-01/
C     550km June solstice
      DATA (FA(1,2,J),J=1,49)/
     &         3.2236E+00, 7.8580E-01, 4.0518E-02,-7.8758E-02,
     &        -7.4039E-01,-5.5023E-01, 2.2661E-01,-1.7191E-01,
     &        -4.5161E-01, 7.7189E-02, 2.4208E-02, 7.5019E-02,
     &         5.4542E-03, 1.8923E+00,-8.6309E-01,-7.3536E-02,
     &        -1.5410E-01,-1.2294E-01,-1.1886E-01,-2.0559E+00,
     &        -4.3640E-02,-2.2578E-01, 6.3167E-03, 5.8175E-02,
     &        -5.5051E-01,-3.0664E-01,-4.9306E-02, 5.2511E-02,
     &         1.4045E-02, 2.2313E-01, 1.9961E-01,-8.9555E-03,
     &        -4.0830E-02,-7.4402E-01,-3.4520E-02,-1.1823E-01,
     &        -4.1510E-02, 1.2895E-01, 3.8704E-02, 5.3682E-03,
     &         7.9895E-02, 1.2443E-01, 2.5079E-03,-3.8743E-02,
     &        -2.1658E-02, 6.8794E-02, 3.6839E-02,-4.1913E-02,
     &        -3.5795E-03/
	DATA (FB(1,2,J),J=1,49)/
     &        -6.2695E+02,-1.7178E+01, 1.1405E+02, 1.7127E+02,
     &         1.3752E+02, 1.3641E+02,-7.0975E+01, 1.4633E+01,
     &         5.2949E+01,-2.8160E+01,-3.0614E+01,-2.7290E+01,
     &         1.8518E+00,-3.5465E+02, 1.7895E+02, 2.0545E+01,
     &         5.6681E+01, 3.4224E+01, 2.6772E+01, 3.7167E+02,
     &         9.1041E+00, 2.1472E+01,-4.3844E-01,-1.2918E+01,
     &         8.1502E+01, 3.8268E+01, 1.0318E+00,-7.3473E+00,
     &        -5.1080E+00,-4.6510E+01,-3.0223E+01, 5.3605E+00,
     &         9.3017E+00, 1.2710E+02, 4.5835E+00, 2.5091E+01,
     &         6.7618E+00,-2.4027E+01,-8.2618E+00, 8.1357E-02,
     &        -1.8817E+01,-2.0141E+01,-3.1208E-01, 1.8021E+00,
     &         2.7531E+00,-1.4198E+01,-8.6175E+00, 8.0561E+00,
     &        -2.6929E+00/
C     900km equinox
      DATA (FA(2,1,J),J=1,49)/
     &         3.3758E+00,-4.8705E-07,-1.4844E+00,-7.9140E-07,
     &        -1.2010E+00,-4.3759E-07,-6.1125E-01,-2.4395E+00,
     &         8.4646E-09, 5.0103E-01, 1.5398E-09, 1.9140E-01,
     &         4.5214E-08, 2.2253E+00,-7.6345E-08,-9.5472E-01,
     &         6.7707E-08, 5.3013E-03,-5.0649E-08,-5.8881E-01,
     &         1.1959E-08, 3.1184E-02,-2.4972E-09,-4.4759E-03,
     &        -1.9022E+00, 3.8763E-09,-1.0882E-01, 3.0299E-08,
     &        -3.4453E-02, 1.3296E-01, 3.1168E-08, 1.4878E-02,
     &        -8.3137E-09,-4.7944E-01,-2.8746E-08, 3.5825E-02,
     &        -1.2058E-08, 3.0506E-01,-9.5413E-09, 7.1259E-03,
     &         7.0469E-03,-2.3296E-09,-8.8034E-03,-2.8111E-02,
     &        -2.4131E-09, 5.6115E-03,-4.8153E-09,-6.9450E-05,
     &         1.1739E-02/
      DATA (FB(2,1,J),J=1,49)/
     &        -6.7222E+02, 1.1078E-04, 2.9062E+02, 1.8966E-04,
     &         2.9021E+02, 9.9730E-05, 1.4805E+02, 4.9025E+02,
     &        -1.4587E-05,-6.2774E+01,-5.6489E-06,-2.3959E+01,
     &         3.4145E-06,-4.9272E+02, 1.5930E-05, 1.8363E+02,
     &        -9.9393E-06,-7.0072E+00, 4.4609E-06, 8.6865E+01,
     &        -2.9546E-06,-3.1763E+00,-1.2566E-07,-5.8817E-01,
     &         3.9687E+02,-6.5285E-06, 1.0005E+01,-2.5441E-06,
     &         4.6315E+00,-2.7349E+01, 5.5261E-06,-1.1131E+00,
     &         1.9219E-06, 8.5274E+01,-3.1671E-06,-9.8191E+00,
     &         7.4340E-07,-6.7348E+01,-1.2024E-06,-5.3796E-01,
     &        -1.4772E+01, 8.2916E-06, 1.9457E+00, 8.3258E+00,
     &         3.5652E-07,-3.4240E+00, 5.4810E-07, 2.9803E-01,
     &        -4.8309E+00/
C     900km June solstice
      DATA (FA(2,2,J),J=1,49)/
     &         1.7678E+00,-1.2523E-01,-3.1971E-02,-1.1885E+00,
     &        -2.0748E-01,-6.2911E-01,-6.9839E-01, 1.4225E-01,
     &         2.9845E-02,-7.4911E-02, 1.8080E-01,-1.3199E-03,
     &        -1.1076E-01,-5.5801E-03,-6.3437E-01, 1.9093E-01,
     &         1.2925E-01, 8.6250E-02, 1.7273E-01,-1.5010E-01,
     &        -1.2204E-01, 4.7007E-02, 5.8541E-02, 6.9108E-03,
     &         3.3109E-01, 1.4050E-01, 8.0140E-02,-3.4096E-02,
     &        -8.3293E-03,-1.0468E-01, 1.0780E-01,-2.3355E-02,
     &         2.1534E-02, 5.9510E-02, 4.3653E-03,-2.2744E-02,
     &         4.2113E-02,-7.8295E-02,-4.2225E-02,-1.9368E-03,
     &        -9.3049E-02,-9.7186E-03,-1.5204E-02,-5.1251E-03,
     &        -1.5143E-02,-1.3670E-02,-1.6588E-02,-1.1932E-02,
     &        -2.7587E-02/
      DATA (FB(2,2,J),J=1,49)/
     &        -3.9911E+02, 5.9796E+01, 4.1540E+01, 2.3466E+02,
     &         4.0871E+01, 1.4529E+02, 1.2104E+02,-4.6233E+01,
     &         1.1266E+01, 2.7210E+01,-3.6695E+01,-6.5125E+00,
     &         1.9678E+01,-6.3622E+01, 1.6088E+02,-4.1296E+01,
     &        -7.7296E+00,-2.0603E+01,-3.7075E+01, 2.4707E+01,
     &         3.2399E+01,-6.7277E+00,-6.0272E+00,-3.4630E+00,
     &        -5.5263E+01,-2.5945E+01,-1.4906E+01, 4.1722E+00,
     &         3.4384E+00, 2.2157E+01,-2.0458E+01, 2.8784E+00,
     &        -4.4228E+00, 1.9416E+01,-1.5663E+01, 6.5339E+00,
     &        -7.9988E+00, 1.4137E+01, 1.6251E+00, 6.4890E-01,
     &         4.8511E+00, 2.1505E+00, 2.9129E+00,-6.7762E-01,
     &         5.7748E-01, 3.7354E+00, 1.0259E+00, 1.5804E+00,
     &         1.5189E+00/
C     1500km equinox
      DATA (FA(3,1,J),J=1,49)/
     &         1.4780E+00,-5.4689E-09,-3.5681E-01, 3.5690E-07,
     &         1.4442E+00,-1.8771E-07,-5.8029E-01, 6.3736E-01,
     &         8.5859E-08, 1.7747E+00,-1.8960E-07,-9.6585E-03,
     &        -7.3912E-09,-6.7418E-01,-2.0098E-08,-2.3251E-01,
     &         4.3772E-08, 8.6224E-02, 1.5699E-08, 3.0123E-01,
     &        -2.8897E-08,-1.0286E-01, 7.3823E-09, 3.5984E-02,
     &         9.9977E-01,-2.5975E-08, 2.2399E-02,-1.5372E-08,
     &         2.9522E-02, 1.1931E-01,-4.4525E-09, 4.1555E-02,
     &         5.0431E-09, 6.6187E-02, 1.3181E-08, 6.1906E-02,
     &        -8.3984E-10, 5.8869E-02,-9.2211E-09,-7.8035E-03,
     &        -7.4955E-02, 3.8595E-08, 2.1260E-02, 3.5597E-02,
     &         5.3006E-09, 4.0952E-02, 2.2709E-08, 1.8248E-02,
     &        -1.7679E-02/
      DATA (FB(3,1,J),J=1,49)/
     &        -2.5247E+02,-1.0611E-05, 3.5396E+01,-8.7667E-05,
     &        -2.9634E+02, 1.6567E-05, 7.7489E+01,-1.1799E+02,
     &        -1.3620E-05,-3.1233E+02, 3.1358E-05,-1.0114E+01,
     &        -4.9641E-06, 1.0363E+02, 4.2123E-06, 2.7824E+01,
     &        -9.7994E-06,-1.9069E+01,-2.8885E-06,-7.4787E+01,
     &         7.6758E-06, 1.0608E+01,-6.8638E-07,-1.1534E+01,
     &        -1.8312E+02, 1.1865E-06,-9.6891E+00, 3.8221E-06,
     &        -7.1519E+00,-2.7741E+01,-5.9749E-06,-9.3219E+00,
     &         1.9756E-06,-1.6897E+01,-3.0522E-07,-8.7396E+00,
     &         1.0363E-07,-2.1515E+01, 1.1544E-06, 1.3571E+00,
     &         1.5544E+01,-3.6035E-06,-2.9642E+00,-8.9441E+00,
     &        -1.5169E-06,-8.5483E+00,-1.6971E-06,-8.4912E+00,
     &         3.1723E+00/
C     1500km June solstice
      DATA (FA(3,2,J),J=1,49)/
     &         1.3888E+00, 1.1103E+00, 1.6445E+00, 7.3508E-01,
     &         9.9423E-02,-1.2668E-01, 1.1793E+00, 3.2611E-01,
     &        -1.3932E-01, 3.4714E-01,-4.0398E-02, 2.8188E-01,
     &        -2.1840E-01, 3.0676E-01, 1.6201E-01,-1.4507E-01,
     &         3.3787E-01,-4.6930E-02, 5.4924E-02, 3.9874E-01,
     &         6.9870E-02, 2.3651E-02,-1.0188E-01, 3.1284E-02,
     &         1.0411E+00,-4.1114E-01,-1.0548E-01,-2.3342E-02,
     &         9.1528E-03, 9.2067E-02,-1.0666E-01, 2.6649E-02,
     &        -3.7639E-02,-1.8572E-01,-6.6661E-02, 9.4229E-02,
     &         3.1840E-02, 5.8198E-03, 4.8141E-03, 9.4962E-03,
     &        -1.1103E-01,-2.1740E-02,-7.2465E-04,-3.3437E-02,
     &        -1.6689E-02,-7.6956E-02,-1.0285E-02,-5.7708E-02,
     &        -1.0310E-01/
      DATA (FB(3,2,J),J=1,49)/
     &        -2.8005E+02,-9.8261E+01,-3.9983E+02,-2.6619E+01,
     &         8.2338E+00, 1.7867E+01,-1.7785E+02,-1.0122E+02,
     &         9.1687E+01,-8.7297E+01,-2.8501E+00,-3.9830E+01,
     &         4.5134E+01,-1.0669E+02,-2.5633E+01, 7.1774E+00,
     &        -7.4302E+01, 1.5876E+01,-1.7359E+00,-9.1679E+01,
     &        -1.9696E+01, 3.0117E+00, 2.0649E+01,-9.3364E+00,
     &        -2.2670E+02, 9.7904E+01, 1.5046E+01,-3.3399E+00,
     &        -8.7702E-01,-1.2437E+01, 2.2864E+01,-3.6244E+00,
     &         6.7245E+00, 1.6524E+01, 2.3854E+01,-2.3351E+01,
     &        -7.1407E+00,-1.8966E+00, 3.3737E+00,-3.0866E+00,
     &         9.1835E+00, 1.9951E+00,-4.0653E-01, 1.9179E-01,
     &         5.1555E+00, 8.6789E+00, 2.9229E+00, 8.9559E+00,
     &         2.1825E+01/
C     2500km equinox
      DATA (FA(4,1,J),J=1,49)/
     &         1.4513E+00,-2.7903E-08,-2.1232E+00, 1.2079E-07,
     &         2.8524E-01,-4.0135E-08, 6.8231E-01, 4.3500E-01,
     &         1.2651E-08, 5.6992E-01,-4.2850E-08, 1.5039E-01,
     &        -1.3229E-08, 9.4290E-02,-2.4997E-08,-7.7275E-01,
     &         5.4290E-08,-1.2859E-01, 9.4636E-09, 5.2854E-01,
     &        -2.1479E-09, 9.2855E-02,-2.7594E-08, 7.4358E-03,
     &        -4.5033E-01,-7.4883E-10,-3.9305E-02, 2.2454E-08,
     &        -4.4181E-02, 5.3266E-02, 8.3007E-09,-1.8869E-02,
     &         2.7783E-10,-5.2278E-02, 2.5128E-08, 5.6681E-02,
     &         3.0310E-09,-1.1503E-01, 3.5058E-08, 2.0314E-02,
     &         4.4499E-02, 1.0183E-09, 1.6406E-02, 6.9291E-04,
     &         6.8229E-09,-2.7379E-02, 5.8450E-09,-1.4324E-02,
     &        -1.4532E-02/
	DATA (FB(4,1,J),J=1,49)/
     &        -4.2157E+02,-1.2843E-05, 1.1677E+02,-1.9182E-05,
     &         6.6617E+00,-2.7718E-05,-1.0527E+02,-1.7184E+01,
     &        -3.4177E-07,-7.2191E+01,-1.1358E-07,-1.8024E+01,
     &        -1.2578E-07,-6.6518E+01, 6.2438E-06, 2.3027E+02,
     &        -1.7359E-05, 4.0800E+01, 2.1467E-06,-1.3715E+02,
     &        -8.5554E-07,-1.7205E+01, 5.0757E-06, 2.2927E-01,
     &         8.0476E+01,-2.7589E-06, 3.3827E+00,-4.0660E-06,
     &         1.3065E+01,-2.0229E+00, 2.1920E-06, 1.0875E+01,
     &        -6.1136E-07, 7.2010E+00,-5.4154E-06,-8.7107E+00,
     &        -4.2033E-07, 2.4655E+01,-7.7856E-06,-4.6298E+00,
     &        -1.6797E+01, 1.3570E-06,-3.2055E+00,-5.4947E-01,
     &         1.2722E-06, 8.1156E+00, 5.5588E-06, 2.0961E+00,
     &         6.3166E+00/
C     2500km June solstice
      DATA (FA(4,2,J),J=1,49)/
     &         8.6854E-01,-9.9343E-02, 1.7241E-02, 4.1379E-01,
     &         1.0034E+00,-7.1819E-01,-1.0619E+00,-4.4765E-01,
     &        -1.0826E+00,-6.5641E-04,-2.3379E-01,-4.1201E-01,
     &         8.2089E-02, 2.2647E+00,-1.0331E-01, 1.3969E-01,
     &         3.4386E-01,-1.4873E-01, 4.8438E-02,-2.2364E-01,
     &         3.3408E-03,-5.7226E-02,-8.7379E-02, 1.0073E-01,
     &         1.6683E-01,-1.3735E-01, 1.9385E-02,-2.7082E-02,
     &         3.1018E-02, 6.7087E-02,-7.9098E-03, 4.7972E-02,
     &        -4.3783E-02, 3.1339E-01, 4.1214E-02, 8.1151E-03,
     &        -2.7117E-02, 3.1667E-02,-1.3346E-02, 8.2864E-03,
     &         2.0433E-01, 9.0147E-03, 1.4602E-02, 5.6125E-02,
     &        -5.2173E-03, 2.8963E-02,-1.2959E-02, 3.1247E-02,
     &        -3.5943E-02/
	DATA (FB(4,2,J),J=1,49)/
     &        -1.9899E+02, 8.3014E+01,-1.2364E+02,-4.9417E+01,
     &        -1.7956E+02, 1.8431E+02, 3.0699E+02, 2.7139E+01,
     &         2.1287E+02, 2.3659E+01, 4.2878E+01, 8.0452E+01,
     &        -2.8811E+01,-4.1889E+02, 1.1639E+01,-4.1313E+00,
     &        -7.5002E+01, 3.4022E+01,-2.2607E+01, 1.8021E+01,
     &         8.1737E+00, 1.4083E+01, 1.9836E+01,-1.8668E+01,
     &        -6.2033E+01, 1.0904E+01,-1.3000E+00, 3.8715E+00,
     &        -7.8479E+00, 2.1208E+01,-5.9507E+00,-1.3782E+01,
     &         7.2326E+00,-5.3504E+01,-5.7353E+00,-1.3774E+00,
     &         6.5094E+00,-7.5963E+00, 3.8696E+00,-2.0070E-01,
     &        -3.8873E+01,-2.7285E+00,-2.3107E+00,-9.2363E+00,
     &         1.1762E+00,-4.0574E+00, 3.3439E+00,-7.0163E+00,
     &         1.0312E+01/
C///////////coefficients - seasonal correction////////////////
C     550km March equinox
      DATA (SZ(1,1,J),J=1,25)/
     &        -2.2484E+01, 1.5678E+01,-1.9782E+02, 1.8137E+02,
     &        -1.1841E+02,-4.6499E+01,-8.0932E+01, 2.8705E+01,
     &        -1.5150E+01, 4.0453E+01,-1.3051E+02, 2.4204E+01,
     &        -8.7547E+00, 1.6474E+01, 9.2847E-01, 5.0067E+00,
     &        -1.6276E+01,-1.7811E+01, 3.3205E+00, 1.5727E+00,
     &         4.4872E+00, 1.1533E+01,-2.8452E+00, 8.4677E+00,
     &         1.0316E+01/
C     550km June solstice
      DATA (SZ(1,2,J),J=1,25)/
     &         8.1616E+01,-7.8931E+01,-1.4877E+02,-2.1824E+01,
     &        -2.2731E+01,-4.4445E+01,-1.3629E+00,-5.1986E+00,
     &         9.5949E+00, 1.1326E+02,-2.9675E+01,-4.1540E+01,
     &        -8.2913E+00,-7.5447E+01,-2.8678E+00, 2.2539E+01,
     &        -4.3198E+01,-6.1286E+00, 1.0387E+01, 2.3967E+01,
     &         4.4365E+00,-1.0083E+01,-6.7061E+00, 1.4691E+00,
     &         3.1579E+00/
C     550km December solstice
      DATA (SZ(1,3,J),J=1,25)/
     &         3.3939E+01, 4.1603E+01, 6.3171E+01, 2.8139E+01,
     &        -1.2545E+01, 2.6427E+01,-5.9703E+01,-5.2340E+00,
     &        -1.4020E+01,-6.3531E-01, 3.6480E+01, 3.6464E+00,
     &         5.6044E+00, 1.9248E+01,-1.1710E+00, 4.0883E+00,
     &         3.0011E+01,-4.8281E+00,-3.2089E+00, 9.7707E+00,
     &        -1.4347E+00,-3.2377E+00,-3.2303E+00, 8.3354E+00,
     &         2.8745E+00/
C     550km September equinox
      DATA (SZ(1,4,J),J=1,25)/
     &        -3.2521E+01, 7.7815E+01,-9.9172E+01, 1.1771E+02,
     &        -4.9189E+01, 4.2987E+01, 6.1923E+00, 1.9022E+01,
     &        -1.4961E+00, 5.6301E+01, 3.3056E+01,-3.4998E+00,
     &         2.5603E+01,-4.1210E+01,-5.9928E-01,-6.5170E+00,
     &        -3.0348E+01,-2.9433E-01,-2.9917E+00, 1.4373E+01,
     &        -1.0131E+00,-1.8499E+00,-2.0904E+00, 1.1991E+00,
     &         1.2403E-01/
C     900km March equinox
      DATA (SZ(2,1,J),J=1,25)/
     &         1.1725E+01,-4.5645E+01, 4.5733E+00, 1.8517E+01,
     &        -2.3715E+01,-1.5863E+01, 2.5084E+01,-8.8654E+00,
     &        -4.2437E+00, 2.2447E+01, 2.0916E+01,-2.5716E+01,
     &        -6.0094E+00, 2.1711E+01,-1.2222E+01, 1.4226E-01,
     &         5.0453E+00,-5.2144E+00, 1.2274E+01,-2.9295E+00,
     &         5.3619E-02, 5.8705E+00, 1.2787E+00, 3.4613E+00,
     &         2.0811E+00/
C     900km June solstice
      DATA (SZ(2,2,J),J=1,25)/
     &         6.6929E+01,-6.3062E+01,-3.1518E+01, 3.9008E+01,
     &         2.2092E+00,-1.1085E+01,-2.8826E+01, 1.1331E+01,
     &        -3.1950E+00, 2.9916E+01,-4.7622E+00,-1.4363E+01,
     &        -4.0448E+00,-1.4279E+01,-9.1323E+00, 1.6311E+00,
     &         1.0317E+00,-1.4682E+01, 3.6067E+00,-9.6292E-01,
     &         3.7949E+00, 3.1022E+00,-4.6328E-01, 9.0868E+00,
     &         4.5608E+00/
C     900km December solstice
      DATA (SZ(2,3,J),J=1,25)/
     &         8.4350E+01, 1.0359E+01, 3.5955E+01,-5.3957E+01,
     &        -5.5573E+01, 6.2484E+01,-1.5365E+01, 5.6723E+00,
     &        -6.6484E+00, 7.8126E+01, 4.9952E+01,-2.9937E+00,
     &         7.2634E+00,-1.0773E+01, 2.0048E+00,-2.9070E-01,
     &        -1.7277E+01,-4.1386E+00,-1.6600E+00, 2.2237E+01,
     &        -3.1530E+00,-1.4685E+01, 1.1209E+00, 4.0532E+00,
     &         1.5677E+00/
C     900km September equinox
      DATA (SZ(2,4,J),J=1,25)/
     &         8.0799E+01,-1.0934E+02, 5.4753E+01, 2.3566E+01,
     &        -6.4691E+01, 1.1268E+00, 6.9659E+00,-1.1639E+01,
     &        -9.2820E+00, 1.1194E+02,-8.7659E+01, 4.6876E+01,
     &        -2.0595E+01,-4.1123E+01, 2.4208E+01,-1.5497E+01,
     &        -4.5482E+01, 1.3202E+01,-1.4486E+01,-2.7499E+00,
     &        -5.8915E+00,-9.0391E-02,-2.8128E-01, 1.9866E+00,
     &         1.3860E+00/
C     1500km March equinox
      DATA (SZ(3,1,J),J=1,25)/
     &         4.8197E+01, 1.5085E+01,-2.5638E+01, 1.1104E+01,
     &        -1.9587E+01, 4.8883E+01, 1.2754E+01,-7.4101E+00,
     &        -1.7884E+01,-9.7165E+00, 3.8258E+01, 3.6539E+00,
     &        -4.9290E+00, 1.0793E+02, 9.0909E+00, 3.3659E+00,
     &        -8.4915E+00, 4.6583E+01, 6.6691E+00, 2.6471E+01,
     &         1.6831E+00,-1.1973E+01, 7.0796E+00, 5.3906E+00,
     &         2.6667E-01/
C     1500km June solstice
      DATA (SZ(3,2,J),J=1,25)/
     &         6.6565E+01,-7.8329E+01, 2.6828E+01,-3.0595E+01,
     &         1.8690E+01, 9.4627E+00,-2.9049E+01, 2.9074E+01,
     &         2.4002E+00,-8.8067E+00, 3.1262E+01,-3.6726E-01,
     &         9.7701E+00, 3.2131E+01,-8.6902E-01,-1.3888E+00,
     &         3.8983E+01,-1.5296E+01,-6.0608E+00, 1.6429E+01,
     &        -5.6030E+00, 2.0101E+01,-7.1151E+00, 5.1599E+00,
     &         4.3186E+00/
C     1500km December solstice
      DATA (SZ(3,3,J),J=1,25)/
     &         3.3717E-01, 1.1554E+02, 4.5506E+01,-7.5635E-01,
     &        -1.8256E+01,-6.5165E+00, 3.5243E+01,-4.1460E+01,
     &         4.8214E+00, 3.1778E+01, 5.2812E+01,-1.3524E+01,
     &        -7.4173E+00,-3.9910E+01, 8.4328E+00,-8.6133E-01,
     &        -1.4775E+01, 4.1764E+00,-7.7575E-01, 6.3293E+00,
     &         1.2135E+00,-7.7471E+00, 3.7529E+00,-8.6309E+00,
     &        -7.8509E+00/
C     1500km September equinox
      DATA (SZ(3,4,J),J=1,25)/
     &        -1.6095E+01, 9.9417E+01,-1.4653E+01,-4.6748E+01,
     &        -3.1115E+00,-5.0367E+01,-5.5260E+00, 3.1643E+00,
     &         1.8895E-01,-4.2743E+01, 1.5466E+01, 2.2751E+01,
     &         2.3019E+01,-1.2153E+01,-9.4305E+00, 7.7048E+00,
     &        -1.0472E+01,-1.0872E+01, 4.7621E+00,-1.5255E+00,
     &        -1.0996E+00,-2.4040E+00, 1.1287E+00,-6.4519E+00,
     &        -2.4907E-01/
C     2500km March equinox
      DATA (SZ(4,1,J),J=1,25)/
     &         1.6289E+02, 1.9716E+02, 2.7176E+02, 1.2047E+02,
     &        -3.3004E+01,-1.5213E+02,-6.3033E+01,-2.3628E+01,
     &         1.5250E+00,-2.8945E+01,-8.4696E+01,-1.6515E+01,
     &        -1.2776E+00, 4.2591E+00, 8.2797E+00,-8.3759E+00,
     &         5.5750E+01, 1.8242E+01,-2.7894E+00, 2.7438E+01,
     &        -2.1300E+00,-4.2789E+00,-2.6639E+00, 4.3236E+00,
     &        -2.3532E+00/
C     2500km June solstice
      DATA (SZ(4,2,J),J=1,25)/
     &         5.4078E+01, 1.0257E+02, 3.2942E+01,-2.7484E+01,
     &        -4.2371E+01, 3.8394E+01,-2.1161E+01, 4.3066E+00,
     &        -2.7098E+01, 1.0562E+02,-4.6081E+01, 1.8271E+01,
     &        -1.5616E+01, 6.8675E+00,-1.1678E-01, 2.9773E+00,
     &         6.5244E+00, 9.0964E+00, 4.4221E+00, 1.4783E+00,
     &         4.5843E-01,-1.2944E+01, 2.9291E+00,-3.6895E+00,
     &        -5.6245E+00/
C     2500km December solstice
      DATA (SZ(4,3,J),J=1,25)/
     &         1.1624E+02, 2.6277E+02, 1.2854E+02,-4.4443E+01,
     &        -1.0411E+02,-6.9986E+01,-8.9133E+01,-2.0989E+01,
     &        -7.9940E+00,-4.7922E+01, 1.1982E+01, 1.3529E+01,
     &         7.9148E+00, 2.2318E+01, 3.7615E+01, 7.5449E+00,
     &        -1.3719E+01,-1.2865E+01,-7.4193E+00,-1.5604E+01,
     &        -1.1663E+01,-2.1478E+00, 4.4852E+00,-1.2419E+01,
     &        -7.5830E+00/
C     2500km September equinox
      DATA (SZ(4,4,J),J=1,25)/
     &         2.0001E+02, 9.5915E+01, 1.6246E+02, 2.9653E+01,
     &        -6.3464E+00,-2.8476E+01,-9.0499E+01,-6.3960E+00,
     &        -1.9869E+00, 3.3675E+01, 8.2809E+01,-1.7643E+01,
     &         9.3427E-01, 1.5153E+01, 7.0991E+00, 9.2573E+00,
     &        -2.9729E+01,-2.1289E+01,-4.9093E+00,-1.4336E+00,
     &        -5.0410E+00,-6.0288E+00,-1.0345E+01, 6.9714E+00,
     &        -2.6930E+00/
C//////////////////////////////////////////////////////////////
      DTOR=3.1415926536/180.0
C     coefficients for mirroring
      DO 10 I=1,81
       D(1,3,I)=D(1,2,I)*MIRREQ(I)
       D(2,3,I)=D(2,2,I)*MIRREQ(I)
       D(3,3,I)=D(3,2,I)*MIRREQ(I)
10     D(4,3,I)=D(4,2,I)*MIRREQ(I)
      DO 20 I=1,49
       FA(1,3,I)=FA(1,2,I)*MF107(I)
       FB(1,3,I)=FB(1,2,I)*MF107(I)
       FA(2,3,I)=FA(2,2,I)*MF107(I)
       FB(2,3,I)=FB(2,2,I)*MF107(I)
       FA(3,3,I)=FA(3,2,I)*MF107(I)
       FB(3,3,I)=FB(3,2,I)*MF107(I)
       FA(4,3,I)=FA(4,2,I)*MF107(I)
20     FB(4,3,I)=FB(4,2,I)*MF107(I)
      IF (CRD .EQ. 1) THEN
C
C calculation of INVDIP from FL, DIMO, BO, and DIPL invariant 
C latitude calculated by highly accurate polynomial expansion
C      
       A=(DIMO/B0)**(1.0D0/3.0D0)/FL
       ASA=A*(B(1)+B(2)*A+B(3)*A**2+B(4)*A**3+B(5)*A**4+
     &        B(6)*A**5+B(7)*A**6+B(8)*A**7)
       IF (ASA .GT. 1.0) ASA=1.0
C      invariant latitude (absolute value)
       RINVL=ACOS(SQRT(ASA))
       INVL=RINVL/DTOR
       RDIPL=DIPL*DTOR
       ALFA=SIN(ABS(RDIPL))**3
       BETA=COS(RINVL)**3
       INVDP=(ALFA*SIGN(1.0,DIPL)*INVL+BETA*DIPL)/(ALFA+BETA)
      ELSE IF	(CRD .EQ. 0) THEN
       INVDP=INVDIP
      ELSE
       RETURN
      END IF
      RMLT=MLT*DTOR*15.0
      RCOLAT=(90.0-INVDP)*DTOR
      CALL SPHARM_IK(C,8,8,RCOLAT,RMLT)
      CALL SPHARM_IK(CF107,6,6,RCOLAT,RMLT)
      CALL SPHARM_IK(CSZ,4,4,RCOLAT,RMLT)
C     21.3. - 20.6.
      IF ((DDD .GE. 79) .AND. (DDD .LT. 171)) THEN
       SEZA=1
       SEZB=2
       DDDA=79
       DDDB=171
       DDDD=DDD
      END IF
C     21.6. - 22.9.
      IF ((DDD .GE. 171) .AND. (DDD .LT. 265)) THEN
       SEZA=2
       SEZB=4
       DDDA=171
       DDDB=265
       DDDD=DDD
      END IF
C     23.9. - 20.12.
      IF ((DDD .GE. 265) .AND. (DDD .LT. 354)) THEN
       SEZA=4
       SEZB=3
       DDDA=265
       DDDB=354
       DDDD=DDD
      END IF
C     21.12. - 20.3.
      IF ((DDD .GE. 354) .OR. (DDD .LT. 79)) THEN
       SEZA=3
       SEZB=1
       DDDA=354
       DDDB=365+79
       DDDD=DDD
        IF (DDD .GE. 354) THEN
         DDDD=DDD
        ELSE
         DDDD=DDD+365
        END IF
      END IF
       SEZAI=MOD(SEZA-1,3)+1
       SEZBI=MOD(SEZB-1,3)+1
       IF (ALT .LT. 900) THEN
C     550km level
        T0A550=0.0
        T0B550=0.0
        T1A550=0.0
        T1B550=0.0
        T2A550=0.0
        T2B550=0.0
        T3A550=0.0
        T3B550=0.0
        DO 30 I=1,81
         T0A550=T0A550+C(I)*D(1,SEZAI,I)
30       T0B550=T0B550+C(I)*D(1,SEZBI,I)
        DO 40 I=1,49
         T1A550=T1A550+CF107(I)*FA(1,SEZAI,I)
40       T1B550=T1B550+CF107(I)*FA(1,SEZBI,I)
        DO 50 I=1,49
         T2A550=T2A550+CF107(I)*FB(1,SEZAI,I)
50       T2B550=T2B550+CF107(I)*FB(1,SEZBI,I)
        DO 60 I=1,25
         T3A550=T3A550+CSZ(I)*SZ(1,SEZA,I)
60       T3B550=T3B550+CSZ(I)*SZ(1,SEZB,I)
        T550A=T0A550+F107Y*(T1A550*F107+T2A550)+SEASY*T3A550
        T550B=T0B550+F107Y*(T1B550*F107+T2B550)+SEASY*T3B550
        T550=(T550B-T550A)/(DDDB-DDDA)*(DDDD-DDDA)+T550A
       END IF
       IF (ALT .LT. 1500) THEN
C     900km level
        T0A900=0.0
        T0B900=0.0
        T1A900=0.0
        T1B900=0.0
        T2A900=0.0
        T2B900=0.0
        T3A900=0.0
        T3B900=0.0
        DO 70 I=1,81
           T0A900=T0A900+C(I)*D(2,SEZAI,I)
70         T0B900=T0B900+C(I)*D(2,SEZBI,I)
        DO 80 I=1,49
           T1A900=T1A900+CF107(I)*FA(2,SEZAI,I)
80         T1B900=T1B900+CF107(I)*FA(2,SEZBI,I)
        DO 90 I=1,49
           T2A900=T2A900+CF107(I)*FB(2,SEZAI,I)
90         T2B900=T2B900+CF107(I)*FB(2,SEZBI,I)
        DO 100 I=1,25
           T3A900=T3A900+CSZ(I)*SZ(2,SEZA,I)
100        T3B900=T3B900+CSZ(I)*SZ(2,SEZB,I)
        T900A=T0A900+F107Y*(T1A900*F107+T2A900)+SEASY*T3A900
        T900B=T0B900+F107Y*(T1B900*F107+T2B900)+SEASY*T3B900
        T900=(T900B-T900A)/(DDDB-DDDA)*(DDDD-DDDA)+T900A
       END IF
       IF (ALT .GE. 900) THEN
C     1500km level
        T0A150=0.0
        T0B150=0.0
        T1A150=0.0
        T1B150=0.0
        T2A150=0.0
        T2B150=0.0
        T3A150=0.0
        T3B150=0.0
        DO 110 I=1,81
         T0A150=T0A150+C(I)*D(3,SEZAI,I)
110      T0B150=T0B150+C(I)*D(3,SEZBI,I)
        DO 120 I=1,49
         T1A150=T1A150+CF107(I)*FA(3,SEZAI,I)
120      T1B150=T1B150+CF107(I)*FA(3,SEZBI,I)
        DO 130 I=1,49
         T2A150=T2A150+CF107(I)*FB(3,SEZAI,I)
130      T2B150=T2B150+CF107(I)*FB(3,SEZBI,I)
        DO 140 I=1,25
         T3A150=T3A150+CSZ(I)*SZ(3,SEZA,I)
140      T3B150=T3B150+CSZ(I)*SZ(3,SEZB,I)
         T150A=T0A150+F107Y*(T1A150*F107+T2A150)+SEASY*T3A150
         T150B=T0B150+F107Y*(T1B150*F107+T2B150)+SEASY*T3B150
         T1500=(T150B-T150A)/(DDDB-DDDA)*(DDDD-DDDA)+T150A
       END IF
       IF (ALT .GE. 1500) THEN
C     2500km level
        T0A250=0.0
        T0B250=0.0
        T1A250=0.0
        T1B250=0.0
        T2A250=0.0
        T2B250=0.0
        T3A250=0.0
        T3B250=0.0
        DO 150 I=1,81
         T0A250=T0A250+C(I)*D(4,SEZAI,I)
150      T0B250=T0B250+C(I)*D(4,SEZBI,I)
        DO 160 I=1,49
         T1A250=T1A250+CF107(I)*FA(4,SEZAI,I)
160      T1B250=T1B250+CF107(I)*FA(4,SEZBI,I)
        DO 170 I=1,49
         T2A250=T2A250+CF107(I)*FB(4,SEZAI,I)
170      T2B250=T2B250+CF107(I)*FB(4,SEZBI,I)
        DO 180 I=1,25
         T3A250=T3A250+CSZ(I)*SZ(4,SEZA,I)
180      T3B250=T3B250+CSZ(I)*SZ(4,SEZB,I)
        T250A=T0A250+F107Y*(T1A250*F107+T2A250)+SEASY*T3A250
        T250B=T0B250+F107Y*(T1B250*F107+T2B250)+SEASY*T3B250
        T2500=(T250B-T250A)/(DDDB-DDDA)*(DDDD-DDDA)+T250A
       END IF
       IF (ALT .LT. 900) TE=(T900-T550)/350.0*(ALT-550)+T550
       IF ((ALT .GE. 900) .AND. (ALT .LT. 1500))
     &    TE=(T1500-T900)/600.0*(ALT-900)+T900
       IF (ALT .GE. 1500) TE=(T2500-T1500)/1000.0*(ALT-1500)+T1500
       RETURN
       END
c
c
	SUBROUTINE SPHARM_IK(C,L,M,COLAT,AZ)
C CALCULATES THE COEFFICIENTS OF THE SPHERICAL HARMONIC
C FROM IRI 95 MODEL
C NOTE: COEFFICIENTS CORRESPONDING TO COS, SIN SWAPPED!!!
      DIMENSION C(*)
      C(1)=1.
      K=2
      X=COS(COLAT)
      C(K)=X
      K=K+1
      DO 10 I=2,L
      C(K)=((2*I-1)*X*C(K-1)-(I-1)*C(K-2))/I
10    K=K+1
      Y=SIN(COLAT)
      DO 20 MT=1,M
      CAZ=COS(MT*AZ)
      SAZ=SIN(MT*AZ)
      C(K)=Y**MT
      K=K+1
      IF(MT.EQ.L) GOTO 16
      C(K)=C(K-1)*X*(2*MT+1)
      K=K+1
      IF((MT+1).EQ.L) GOTO 16
      DO 15 I=2+MT,L
      C(K)=((2*I-1)*X*C(K-1)-(I+MT-1)*C(K-2))/(I-MT)
15    K=K+1
16    N=L-MT+1
      DO 18 I=1,N
      C(K)=C(K-N)*SAZ
      C(K-N)=C(K-N)*CAZ
18    K=K+1
20    CONTINUE
      RETURN
      END
c
c
      SUBROUTINE TEBA(DIPL,SLT,NS,TE) 
C CALCULATES ELECTRON TEMPERATURES TE(1) TO TE(4) AT ALTITUDES
C 300, 400, 1400 AND 3000 KM FOR DIP-LATITUDE DIPL/DEG AND 
C LOCAL SOLAR TIME SLT/H USING THE BRACE-THEIS-MODELS (J. ATMOS.
C TERR. PHYS. 43, 1317, 1981); NS IS SEASON IN NORTHERN
C HEMISOHERE: IS=1 SPRING, IS=2 SUMMER ....
C ALSO CALCULATED ARE THE TEMPERATURES AT 400 KM ALTITUDE FOR
C MIDNIGHT (TE(5)) AND NOON (TE(6)).   
      DIMENSION C(4,2,81),A(82),TE(6)
      COMMON    /CONST/UMR      /const1/humr,dumr
      DATA (C(1,1,J),J=1,81)/                      
     &.3100E1,-.3215E-2,.2440E+0,-.4613E-3,-.1711E-1,.2605E-1,                  
     &-.9546E-1,.1794E-1,.1270E-1,.2791E-1,.1536E-1,-.6629E-2,                  
     &-.3616E-2,.1229E-1,.4147E-3,.1447E-2,-.4453E-3,-.1853,                    
     &-.1245E-1,-.3675E-1,.4965E-2,.5460E-2,.8117E-2,-.1002E-1,                 
     &.5466E-3,-.3087E-1,-.3435E-2,-.1107E-3,.2199E-2,.4115E-3,                 
     &.6061E-3,.2916E-3,-.6584E-1,.4729E-2,-.1523E-2,.6689E-3,                  
     &.1031E-2,.5398E-3,-.1924E-2,-.4565E-1,.7244E-2,-.8543E-4,                 
     &.1052E-2,-.6696E-3,-.7492E-3,.4405E-1,.3047E-2,.2858E-2,                  
     &-.1465E-3,.1195E-2,-.1024E-3,.4582E-1,.8749E-3,.3011E-3,                  
     &.4473E-3,-.2782E-3,.4911E-1,-.1016E-1,.27E-2,-.9304E-3,                   
     &-.1202E-2,.2210E-1,.2566E-2,-.122E-3,.3987E-3,-.5744E-1,                  
     &.4408E-2,-.3497E-2,.83E-3,-.3536E-1,-.8813E-2,.2423E-2,                   
     &-.2994E-1,-.1929E-2,-.5268E-3,-.2228E-1,.3385E-2,                         
     &.413E-1,.4876E-2,.2692E-1,.1684E-2/          
      DATA (C(1,2,J),J=1,81)/.313654E1,.6796E-2,.181413,.8564E-1,               
     &-.32856E-1,-.3508E-2,-.1438E-1,-.2454E-1,.2745E-2,.5284E-1,               
     &.1136E-1,-.1956E-1,-.5805E-2,.2801E-2,-.1211E-2,.4127E-2,                 
     &.2909E-2,-.25751,-.37915E-2,-.136E-1,-.13225E-1,.1202E-1,                 
     &.1256E-1,-.12165E-1,.1326E-1,-.7123E-1,.5793E-3,.1537E-2,                 
     &.6914E-2,-.4173E-2,.1052E-3,-.5765E-3,-.4041E-1,-.1752E-2,                
     &-.542E-2,-.684E-2,.8921E-3,-.2228E-2,.1428E-2,.6635E-2,-.48045E-2,        
     &-.1659E-2,-.9341E-3,.223E-3,-.9995E-3,.4285E-1,-.5211E-3,                 
     &-.3293E-2,.179E-2,.6435E-3,-.1891E-3,.3844E-1,.359E-2,-.8139E-3,          
     &-.1996E-2,.2398E-3,.2938E-1,.761E-2,.347655E-2,.1707E-2,.2769E-3,         
     &-.157E-1,.983E-3,-.6532E-3,.929E-4,-.2506E-1,.4681E-2,.1461E-2,           
     &-.3757E-5,-.9728E-2,.2315E-2,.6377E-3,-.1705E-1,.2767E-2,                 
     &-.6992E-3,-.115E-1,-.1644E-2,.3355E-2,-.4326E-2,.2035E-1,.2985E-1/        
      DATA (C(2,1,J),J=1,81)/.3136E1,.6498E-2,.2289,.1859E-1,-.3328E-1,         
     &-.4889E-2,-.3054E-1,-.1773E-1,-.1728E-1,.6555E-1,.1775E-1,                
     &-.2488E-1,-.9498E-2,.1493E-1,.281E-2,.2406E-2,.5436E-2,-.2115,            
     &.7007E-2,-.5129E-1,-.7327E-2,.2402E-1,.4772E-2,-.7374E-2,                 
     &-.3835E-3,-.5013E-1,.2866E-2,.2216E-2,.2412E-3,.2094E-2,.122E-2           
     &,-.1703E-3,-.1082,-.4992E-2,-.4065E-2,.3615E-2,-.2738E-2,                 
     &-.7177E-3,.2173E-3,-.4373E-1,-.375E-2,.5507E-2,-.1567E-2,                 
     &-.1458E-2,-.7397E-3,.7903E-1,.4131E-2,.3714E-2,.1073E-2,                  
     &-.8991E-3,.2976E-3,.2623E-1,.2344E-2,.5608E-3,.4124E-3,.1509E-3,          
     &.5103E-1,.345E-2,.1283E-2,.7238E-3,-.3464E-4,.1663E-1,-.1644E-2,          
     &-.71E-3,.5281E-3,-.2729E-1,.3556E-2,-.3391E-2,-.1787E-3,.2154E-2,         
     &.6476E-2,-.8282E-3,-.2361E-1,.9557E-3,.3205E-3,-.2301E-1,                 
     &-.854E-3,-.1126E-1,-.2323E-2,-.8582E-2,.2683E-1/                          
      DATA (C(2,2,J),J=1,81)/.3144E1,.8571E-2,.2539,.6937E-1,-.1667E-1,         
     &.2249E-1,-.4162E-1,.1201E-1,.2435E-1,.5232E-1,.2521E-1,-.199E-1,          
     &-.7671E-2,.1264E-1,-.1551E-2,-.1928E-2,.3652E-2,-.2019,.5697E-2,          
     &-.3159E-1,-.1451E-1,.2868E-1,.1377E-1,-.4383E-2,.1172E-1,                 
     &-.5683E-1,.3593E-2,.3571E-2,.3282E-2,.1732E-2,-.4921E-3,-.1165E-2         
     &,-.1066,-.1892E-1,.357E-2,-.8631E-3,-.1876E-2,-.8414E-4,.2356E-2,         
     &-.4259E-1,-.322E-2,.4641E-2,.6223E-3,-.168E-2,-.1243E-3,.7393E-1,         
     &-.3143E-2,-.2362E-2,.1235E-2,-.1551E-2,.2099E-3,.2299E-1,.5301E-2         
     &,-.4306E-2,-.1303E-2,.7687E-5,.5305E-1,.6642E-2,-.1686E-2,                
     &.1048E-2,.5958E-3,.4341E-1,-.8819E-4,-.333E-3,-.2158E-3,-.4106E-1         
     &,.4191E-2,.2045E-2,-.1437E-3,-.1803E-1,-.8072E-3,-.424E-3,                
     &-.26E-1,-.2329E-2,.5949E-3,-.1371E-1,-.2188E-2,.1788E-1,                  
     &.6405E-3,.5977E-2,.1333E-1/                  
      DATA (C(3,1,J),J=1,81)/.3372E1,.1006E-1,.1436,.2023E-2,-.5166E-1,         
     &.9606E-2,-.5596E-1,.4914E-3,-.3124E-2,-.4713E-1,-.7371E-2,                
     &-.4823E-2,-.2213E-2,.6569E-2,-.1962E-3,.3309E-3,-.3908E-3,                
     &-.2836,.7829E-2,.1175E-1,.9919E-3,.6589E-2,.2045E-2,-.7346E-2             
     &,-.89E-3,-.347E-1,-.4977E-2,.147E-2,-.2823E-5,.6465E-3,                   
     &-.1448E-3,.1401E-2,-.8988E-1,-.3293E-4,-.1848E-2,.4439E-3,                
     &-.1263E-2,.317E-3,-.6227E-3,.1721E-1,-.199E-2,-.4627E-3,                  
     &.2897E-5,-.5454E-3,.3385E-3,.8432E-1,-.1951E-2,.1487E-2,                  
     &.1042E-2,-.4788E-3,-.1276E-3,.2373E-1,.2409E-2,.5263E-3,                  
     &.1301E-2,-.4177E-3,.3974E-1,.1418E-3,-.1048E-2,-.2982E-3,                 
     &-.3396E-4,.131E-1,.1413E-2,-.1373E-3,.2638E-3,-.4171E-1,                  
     &-.5932E-3,-.7523E-3,-.6883E-3,-.2355E-1,.5695E-3,-.2219E-4,               
     &-.2301E-1,-.9962E-4,-.6761E-3,.204E-2,-.5479E-3,.2591E-1,                 
     &-.2425E-2,.1583E-1,.9577E-2/                 
      DATA (C(3,2,J),J=1,81)/.3367E1,.1038E-1,.1407,.3622E-1,-.3144E-1,         
     &.112E-1,-.5674E-1,.3219E-1,.1288E-2,-.5799E-1,-.4609E-2,                  
     &.3252E-2,-.2859E-3,.1226E-1,-.4539E-2,.1310E-2,-.5603E-3,                 
     &-.311,-.1268E-2,.1539E-1,.3146E-2,.7787E-2,-.143E-2,-.482E-2              
     &,.2924E-2,-.9981E-1,-.7838E-2,-.1663E-3,.4769E-3,.4148E-2,                
     &-.1008E-2,-.979E-3,-.9049E-1,-.2994E-2,-.6748E-2,-.9889E-3,               
     &.1488E-2,-.1154E-2,-.8412E-4,-.1302E-1,-.4859E-2,-.7172E-3,               
     &-.9401E-3,.9101E-3,-.1735E-3,.7055E-1,.6398E-2,-.3103E-2,                 
     &-.938E-3,-.4E-3,-.1165E-2,.2713E-1,-.1654E-2,.2781E-2,                    
     &-.5215E-5,.2258E-3,.5022E-1,.95E-2,.4147E-3,.3499E-3,                     
     &-.6097E-3,.4118E-1,.6556E-2,.3793E-2,-.1226E-3,-.2517E-1,                 
     &.1491E-3,.1075E-2,.4531E-3,-.9012E-2,.3343E-2,.3431E-2,                   
     &-.2519E-1,.3793E-4,.5973E-3,-.1423E-1,-.132E-2,-.6048E-2,                 
     &-.5005E-2,-.115E-1,.2574E-1/                 
      DATA (C(4,1,J),J=1,81)/.3574E1,.0,.7537E-1,.0,-.8459E-1,                  
     &0.,-.294E-1,0.,.4547E-1,-.5321E-1,0.,.4328E-2,0.,.6022E-2,                
     &.0,-.9168E-3,.0,-.1768,.0,.294E-1,.0,.5902E-3,.0,-.9047E-2,               
     &.0,-.6555E-1,.0,-.1033E-2,.0,.1674E-2,.0,.2802E-3,-.6786E-1               
     &,.0,.4193E-2,.0,-.6448E-3,.0,.9277E-3,-.1634E-1,.0,-.2531E-2              
     &,.0,.193E-4,.0,.528E-1,.0,.2438E-2,.0,-.5292E-3,.0,.1555E-1               
     &,.0,-.3259E-2,.0,-.5998E-3,.3168E-1,.0,.2382E-2,.0,-.4078E-3              
     &,.2312E-1,.0,.1481E-3,.0,-.1885E-1,.0,.1144E-2,.0,-.9952E-2               
     &,.0,-.551E-3,-.202E-1,.0,-.7283E-4,-.1272E-1,.0,.2224E-2,                 
     &.0,-.251E-2,.2434E-1/                        
      DATA (C(4,2,J),J=1,81)/.3574E1,-.5639E-2,.7094E-1,                        
     &-.3347E-1,-.861E-1,-.2877E-1,-.3154E-1,-.2847E-2,.1235E-1,                
     &-.5966E-1,-.3236E-2,.3795E-3,-.8634E-3,.3377E-2,-.1071E-3,                
     &-.2151E-2,-.4057E-3,-.1783,.126E-1,.2835E-1,-.242E-2,                     
     &.3002E-2,-.4684E-2,-.6756E-2,-.7493E-3,-.6147E-1,-.5636E-2                
     &,-.1234E-2,-.1613E-2,-.6353E-4,-.2503E-3,-.1729E-3,-.7148E-1              
     &,.5326E-2,.4006E-2,.6484E-3,-.1046E-3,-.6034E-3,-.9435E-3,                
     &-.2385E-2,.6853E-2,.151E-2,.1319E-2,.9049E-4,-.1999E-3,                   
     &.3976E-1,.2802E-2,-.103E-2,.5599E-3,-.4791E-3,-.846E-4,                   
     &.2683E-1,.427E-2,.5911E-3,.2987E-3,-.208E-3,.1396E-1,                     
     &-.1922E-2,-.1063E-2,.3803E-3,.1343E-3,.1771E-1,-.1038E-2,                 
     &-.4645E-3,-.2481E-3,-.2251E-1,-.29E-2,-.3977E-3,-.516E-3,                 
     &-.8079E-2,-.1528E-2,.306E-3,-.1582E-1,-.8536E-3,.1565E-3,                 
     &-.1252E-1,.2319E-3,.4311E-2,.1024E-2,.1296E-5,.179E-1/                    
        IF(NS.LT.3) THEN
           IS=NS
        ELSE IF(NS.GT.3) THEN
           IS=2
           DIPL=-DIPL
        ELSE
           IS=1
        ENDIF
      COLAT=UMR*(90.-DIPL)                    
      AZ=humr*SLT    
      CALL SPHARM(A,8,8,COLAT,AZ)
        IF(IS.EQ.2) THEN
           KEND=3
        ELSE
           KEND=4
        ENDIF                  
      DO 2 K=1,KEND      
      STE=0.          
      DO 1 I=1,81     
1       STE=STE+A(I)*C(K,IS,I)                       
2     TE(K)=10.**STE
        IF(IS.EQ.2) THEN
           DIPL=-DIPL
           COLAT=UMR*(90.-DIPL)                    
           CALL SPHARM(A,8,8,COLAT,AZ)
           STE=0.          
           DO 11 I=1,81     
11            STE=STE+A(I)*C(4,2,I)                       
           TE(4)=10.**STE
        ENDIF

C---------- TEMPERATURE AT 400KM AT MIDNIGHT AND NOON
      DO 4 J=1,2      
        STE=0.          
        AZ=humr*(J-1)*12.                           
        CALL SPHARM(A,8,8,COLAT,AZ)                  
        DO 3 I=1,81     
3         STE=STE+A(I)*C(2,IS,I)                       
4       TE(J+4)=10.**STE                             
      RETURN          
      END             
C
      SUBROUTINE SPHARM(C,L,M,COLAT,AZ)            
C CALCULATES THE COEFFICIENTS OF THE SPHERICAL HARMONIC                         
C EXPANSION THAT WAS USED FOR THE BRACE-THEIS-MODELS.                           
      DIMENSION C(82)                              
      C(1)=1.         
      K=2             
      X=COS(COLAT)    
      C(K)=X          
      K=K+1           
      DO 10 I=2,L     
      C(K)=((2*I-1)*X*C(K-1)-(I-1)*C(K-2))/I       
10    K=K+1           
      Y=SIN(COLAT)    
      DO 20 MT=1,M    
      CAZ=COS(MT*AZ)  
      SAZ=SIN(MT*AZ)  
      C(K)=Y**MT      
      K=K+1           
      IF(MT.EQ.L) GOTO 16                          
      C(K)=C(K-1)*X*(2*MT+1)                       
      K=K+1           
      IF((MT+1).EQ.L) GOTO 16                      
      DO 15 I=2+MT,L  
      C(K)=((2*I-1)*X*C(K-1)-(I+MT-1)*C(K-2))/(I-MT)                            
15    K=K+1           
16    N=L-MT+1        
      DO 18 I=1,N     
      C(K)=C(K-N)*CAZ                              
      C(K-N)=C(K-N)*SAZ                            
18    K=K+1           
20    CONTINUE        
      RETURN          
      END             
C
C
      REAL FUNCTION ELTE(H)
c----------------------------------------------------------------
C ELECTRON TEMPERATURE PROFILE BASED ON THE TEMPERATURES AT 120                 
C HMAX,300,400,600,1400,3000 KM ALTITUDE. INBETWEEN CONSTANT                    
C GRADIENT IS ASSUMED. ARGMAX IS MAXIMUM ARGUMENT ALLOWED FOR
C EXP-FUNCTION.
c----------------------------------------------------------------
      COMMON /BLOTE/AH(7),ATE1,ST(6),D(5)
C
      SUM=ATE1+ST(1)*(H-AH(1))                     
      DO 1 I=1,5
        aa = eptr(h    ,d(i),ah(i+1))
        bb = eptr(ah(1),d(i),ah(i+1))
1     SUM=SUM+(ST(I+1)-ST(I))*(AA-BB)*D(I)                
      ELTE=SUM        
      RETURN          
      END             
C
C
      FUNCTION TEDE(H,DEN,COV)                     
C ELECTRON TEMEPERATURE MODEL AFTER BRACE,THEIS .  
C FOR NEG. COV THE MEAN COV-INDEX (3 SOLAR ROT.) IS EXPECTED.                   
C DEN IS THE ELECTRON DENSITY IN M-3.              
      Y=1051.+(17.01*H-2746.)*                     
     &EXP(-5.122E-4*H+(6.094E-12-3.353E-14*H)*DEN) 
      ACOV=ABS(COV)   
      YC=1.+(.117+2.02E-3*ACOV)/(1.+EXP(-(ACOV-102.5)/5.))                      
      IF(COV.LT.0.)   
     &YC=1.+(.123+1.69E-3*ACOV)/(1.+EXP(-(ACOV-115.)/10.))                      
      TEDE=Y*YC       
      RETURN          
      END             
C
C                     
C*************************************************************                  
C**************** ION TEMPERATURE ****************************
C*************************************************************                  
C
C
      REAL FUNCTION TI(H)
c----------------------------------------------------------------
C ION TEMPERATURE FOR HEIGHTS NOT GREATER 1000 KM AND NOT LESS HS               
C EXPLANATION SEE FUNCTION RPID.                   
c----------------------------------------------------------------
      REAL              MM
      COMMON  /BLOCK8/  HS,TNHS,XSM(4),MM(5),G(4),M

      SUM=MM(1)*(H-HS)+TNHS                        
      DO 100 I=1,M-1  
        aa = eptr(h ,g(i),xsm(i))
        bb = eptr(hs,g(i),xsm(i))
100     SUM=SUM+(MM(I+1)-MM(I))*(AA-BB)*G(I)                
      TI=SUM          
      RETURN          
      END             
C
C
      REAL FUNCTION TEDER(H)                       
C THIS FUNCTION ALONG WITH PROCEDURE REGFA1 ALLOWS TO FIND                      
C THE  HEIGHT ABOVE WHICH TN BEGINS TO BE DIFFERENT FROM TI                     
      COMMON    /BLOTN/XSM1,TEX,TLBD,SIG
      TNH = TN(H,TEX,TLBD,SIG)                        
      DTDX = DTNDH(H,TEX,TLBD,SIG)                        
      TEDER = DTDX * ( XSM1 - H ) + TNH                    
      RETURN          
      END             
C
C
      FUNCTION TN(H,TINF,TLBD,S)
C--------------------------------------------------------------------
C       Calculate Temperature for MSIS/CIRA-86 model
C--------------------------------------------------------------------
      ZG2 = ( H - 120. ) * 6476.77 / ( 6356.77 + H )
      TN = TINF - TLBD * EXP ( - S * ZG2 )
      RETURN
      END
C
C
      FUNCTION DTNDH(H,TINF,TLBD,S)
C---------------------------------------------------------------------
      ZG1 = 6356.77 + H
      ZG2 = 6476.77 / ZG1
      ZG3 = ( H - 120. ) * ZG2
      DTNDH = - TLBD * EXP ( - S * ZG3 ) * ( S / ZG1 * ( ZG3 - ZG2 ) )
      RETURN
      END
C
C                     
C*************************************************************                  
C************* ION RELATIVE PRECENTAGE DENSITY *****************                
C*************************************************************                  
C
C
      REAL FUNCTION RPID (H, H0, N0, M, ST, ID, XS)
c------------------------------------------------------------------
C D.BILITZA,1977,THIS ANALYTIC FUNCTION IS USED TO REPRESENT THE                
C RELATIVE PRECENTAGE DENSITY OF ATOMAR AND MOLECULAR OXYGEN IONS.              
C THE M+1 HEIGHT GRADIENTS ST(M+1) ARE CONNECTED WITH EPSTEIN-                  
C STEP-FUNCTIONS AT THE STEP HEIGHTS XS(M) WITH TRANSITION                      
C THICKNESSES ID(M). RPID(H0,H0,N0,....)=N0.       
C ARGMAX is the highest allowed argument for EXP in your system.
c------------------------------------------------------------------
      REAL              N0         
      DIMENSION         ID(4), ST(5), XS(4)                
      COMMON  /ARGEXP/  ARGMAX

      SUM=(H-H0)*ST(1)                             
      DO 100  I=1,M   
              XI=ID(I)
                aa = eptr(h ,xi,xs(i))
                bb = eptr(h0,xi,xs(i))
100           SUM=SUM+(ST(I+1)-ST(I))*(AA-BB)*XI 
      IF(ABS(SUM).LT.ARGMAX) then
        SM=EXP(SUM)
      else IF(SUM.Gt.0.0) then
        SM=EXP(ARGMAX)
      else
        SM=0.0
      endif
      RPID= n0 * SM        
      RETURN          
      END             
C
c
      SUBROUTINE RDHHE (H,HB,RDOH,RDO2H,RNO,PEHE,RDH,RDHE)                      
C BILITZA,FEB.82,H+ AND HE+ RELATIVE PERECENTAGE DENSITY BELOW                  
C 1000 KM. THE O+ AND O2+ REL. PER. DENSITIES SHOULD BE GIVEN                   
C (RDOH,RDO2H). HB IS THE ALTITUDE OF MAXIMAL O+ DENSITY. PEHE                  
C IS THE PRECENTAGE OF HE+ IONS COMPARED TO ALL LIGHT IONS.                     
C RNO IS THE RATIO OF NO+ TO O2+DENSITY AT H=HB.   
      RDHE=0.0        
      RDH=0.0         
      IF(H.LE.HB) GOTO 100                         
      REST=100.0-RDOH-RDO2H-RNO*RDO2H              
      RDH=REST*(1.-PEHE/100.)                      
      RDHE=REST*PEHE/100.                          
100   RETURN          
      END             
C
C
      REAL FUNCTION RDNO(H,HB,RDO2H,RDOH,RNO)      
C D.BILITZA, 1978. NO+ RELATIVE PERCENTAGE DENSITY ABOVE 100KM.                 
C FOR MORE INFORMATION SEE SUBROUTINE RDHHE.       
      IF (H.GT.HB) GOTO 200                        
      RDNO=100.0-RDO2H-RDOH                        
      RETURN          
200   RDNO=RNO*RDO2H  
      RETURN          
      END
C
C
      SUBROUTINE  KOEFP1(PG1O)                     
C THIEMANN,1979,COEFFICIENTS PG1O FOR CALCULATING  O+ PROFILES                  
C BELOW THE F2-MAXIMUM. CHOSEN TO APPROACH DANILOV-                             
C SEMENOV'S COMPILATION.                           
      DIMENSION PG1O(80)                           
      REAL FELD (80)  
      DATA FELD/-11.0,-11.0,4.0,-11.0,0.08018,     
     &0.13027,0.04216,0.25  ,-0.00686,0.00999,     
     &5.113,0.1 ,170.0,180.0,0.1175,0.15,-11.0,    
     &1.0 ,2.0,-11.0,0.069,0.161,0.254,0.18,0.0161,                             
     &0.0216,0.03014,0.1,152.0,167.0,0.04916,      
     &0.17,-11.0,2.0,2.0,-11.0,0.072,0.092,0.014,0.21,                          
     &0.01389,0.03863,0.05762,0.12,165.0,168.0,0.008,                           
     &0.258,-11.0,1.0,3.0,-11.0,0.091,0.088,       
     &0.008,0.34,0.0067,0.0195,0.04,0.1,158.0,172.0,                            
     &0.01,0.24,-11.0,2.0,3.0, -11.0,0.083,0.102,  
     &0.045,0.03,0.00127,0.01,0.05,0.09,167.0,185.0,                            
     &0.015,0.18/     
      K=0             
      DO 10 I=1,80    
      K=K+1           
10    PG1O(K)=FELD(I)                              
      RETURN          
      END             
C
C
      SUBROUTINE KOEFP2(PG2O)                      
C THIEMANN,1979,COEFFICIENTS FOR CALCULATION OF O+ PROFILES                     
C ABOVE THE F2-MAXIMUM (DUMBS,SPENNER:AEROS-COMPILATION)                        
      DIMENSION PG2O(32)                           
      REAL FELD(32)   
      DATA FELD/1.0,-11.0,-11.0,1.0,695.0,-.000781,                             
     &-.00264,2177.0,1.0,-11.0,-11.0,2.0,570.0,    
     &-.002,-.0052,1040.0,2.0,-11.0,-11.0,1.0,695.0,                            
     &-.000786,-.00165,3367.0,2.0,-11.0,-11.0,2.0, 
     &575.0,-.00126,-.00524,1380.0/                
      K=0             
      DO 10 I=1,32    
      K=K+1           
10    PG2O(K)=FELD(I)                              
      RETURN          
      END             
C
C
      SUBROUTINE  KOEFP3(PG3O)                     
C THIEMANN,1979,COEFFICIENTS FOR CALCULATING O2+ PROFILES.                      
C CHOSEN AS TO APPROACH DANILOV-SEMENOV'S COMPILATION.                          
      DIMENSION PG3O(80)                           
      REAL FELD(80)   
      DATA FELD/-11.0,1.0,2.0,-11.0,160.0,31.0,130.0,                           
     &-10.0,198.0,0.0,0.05922,-0.07983,            
     &-0.00397,0.00085,-0.00313,0.0,-11.0,2.0,2.0,-11.0,                        
     &140.0,30.0,130.0,-10.0,                      
     &190.0,0.0,0.05107,-0.07964,0.00097,-0.01118,-0.02614,                     
     &-0.09537,       
     &-11.0,1.0,3.0,-11.0,140.0,37.0,125.0,0.0,182.0,                           
     &0.0,0.0307,-0.04968,-0.00248,                
     &-0.02451,-0.00313,0.0,-11.0,2.0,3.0,-11.0,   
     &140.0,37.0,125.0,0.0,170.0,0.0,              
     &0.02806,-0.04716,0.00066,-0.02763,-0.02247,-0.01919,                      
     &-11.0,-11.0,4.0,-11.0,140.0,45.0,136.0,-9.0, 
     &181.0,-26.0,0.02994,-0.04879,                
     &-0.01396,0.00089,-0.09929,0.05589/           
      K=0             
      DO 10 I=1,80    
      K=K+1           
10    PG3O(K)=FELD(I)                              
      RETURN          
      END             
C
C
      SUBROUTINE SUFE (FIELD,RFE,M,FE)             
C SELECTS THE REQUIRED ION DENSITY PARAMETER SET.
C THE INPUT FIELD INCLUDES DIFFERENT SETS OF DIMENSION M EACH                
C CARACTERISED BY 4 HEADER NUMBERS. RFE(4) SHOULD CONTAIN THE                   
C CHOSEN HEADER NUMBERS.FE(M) IS THE CORRESPONDING SET.                         
      DIMENSION RFE(4),FE(12),FIELD(80),EFE(4)     
      K=0             
100   DO 101 I=1,4    
      K=K+1           
101   EFE(I)=FIELD(K)                              
      DO 111 I=1,M    
      K=K+1           
111   FE(I)=FIELD(K)  
      DO 120 I=1,4    
      IF((EFE(I).GT.-10.0).AND.(RFE(I).NE.EFE(I))) GOTO 100                     
120   CONTINUE        
      RETURN          
      END             

      Subroutine ionco2(hei,xhi,it,F,R1,R2,R3,R4)
*----------------------------------------------------------------
*     INPUT DATA :
*      hei  -  altitude in km
*      xhi  -  solar zenith angle in degree
*      it   -  season (month)
*      F    -  10.7cm solar radio flux
*     OUTPUT DATA :
*     R1 -  NO+ concentration (in percent)
*     R2 -  O2+ concentration (in percent) 
*     R3 -  Cb+ concentration (in percent) 
*     R4 -  O+  concentration (in percent) 
*
*  A.D. Danilov and N.V. Smirnova, Improving the 75 to 300 km ion 
*  composition model of the IRI, Adv. Space Res. 15, #2, 171-177, 1995.
*
*-----------------------------------------------------------------
      dimension j1ms70(7),j2ms70(7),h1s70(13,7),h2s70(13,7),
     *       R1ms70(13,7),R2ms70(13,7),rk1ms70(13,7),rk2ms70(13,7),
     *       j1ms140(7),j2ms140(7),h1s140(13,7),h2s140(13,7), 
     *       R1ms140(13,7),R2ms140(13,7),rk1ms140(13,7),rk2ms140(13,7),
     *       j1mw70(7),j2mw70(7),h1w70(13,7),h2w70(13,7),
     *       R1mw70(13,7),R2mw70(13,7),rk1mw70(13,7),rk2mw70(13,7),
     *       j1mw140(7),j2mw140(7),h1w140(13,7),h2w140(13,7), 
     *       R1mw140(13,7),R2mw140(13,7),rk1mw140(13,7),rk2mw140(13,7),
     *       j1mr70(7),j2mr70(7),h1r70(13,7),h2r70(13,7),
     *       R1mr70(13,7),R2mr70(13,7),rk1mr70(13,7),rk2mr70(13,7),
     *       j1mr140(7),j2mr140(7),h1r140(13,7),h2r140(13,7), 
     *       R1mr140(13,7),R2mr140(13,7),rk1mr140(13,7),rk2mr140(13,7)
      data j1ms70/11,11,10,10,11,9,11/ 
      data j2ms70/13,11,10,11,11,9,11/
      data h1s70/75,85,90,95,100,120,130,200,220,250,270,0,0,
     *        75,85,90,95,100,120,130,200,220,250,270,0,0, 
     *        75,85,90,95,100,115,200,220,250,270,0,0,0,
     *        75,80,95,100,120,140,200,220,250,270,0,0,0,
     *        75,80,95,100,120,150,170,200,220,250,270,0,0,
     *        75,80,95,100,140,200,220,250,270,0,0,0,0,
     *        75,80,85,95,100,110,145,200,220,250,270,0,0/
      data h2s70/75,80,90,95,100,120,130,140,150,200,220,250,270,
     *        75,80,90,95,100,120,130,200,220,250,270,0,0, 
     *        75,80,90,95,100,115,200,220,250,270,0,0,0,
     *        75,80,95,100,120,140,150,200,220,250,270,0,0,
     *        75,80,95,100,120,150,170,200,220,250,270,0,0,
     *        75,80,95,100,140,200,220,250,270,0,0,0,0,
     *        75,80,90,95,100,110,145,200,220,250,270,0,0/
      data R1ms70/6,30,60,63,59,59,66,52,20,4,2,0,0,
     *         6,30,60,63,69,62,66,52,20,4,2,0,0, 
     *         6,30,60,63,80,68,53,20,4,2,0,0,0,
     *         4,10,60,85,65,65,52,25,12,4,0,0,0, 
     *         4,10,60,89,72,60,60,52,30,20,10,0,0, 
     *         4,10,60,92,68,54,40,25,13,0,0,0,0, 
     *         1,8,20,60,95,93,69,65,45,30,20,0,0/ 
      data R2ms70/4,10,30,32,41,41,32,29,34,28,15,3,1,
     *         4,10,30,32,31,38,32,28,15,3,1,0,0,
     *         4,10,30,32,20,32,28,15,3,1,0,0,0,
     *         2,6,30,15,35,30,34,26,19,8,3,0,0,
     *         2,6,30,11,28,38,29,29,25,12,5,0,0,
     *         2,6,30,8,32,30,20,14,8,0,0,0,0,
     *         1,2,10,20,5,7,31,23,18,15,10,0,0/ 
      data rk1ms70/2.4,6.,.6,-.8,0,.7,-.2,-1.6,-.533,-.1,-.067,0,0,
     *         2.4,6.,.6,1.2,-.35,.4,-.2,-1.6,-.533,-.1,-.067,0,0, 
     *         2.4,6.,.6,3.4,-.8,-.176,-1.65,-.533,-.1,-.067,0,0,0,
     *         1.2,3.333,5.,-1.,0,-.216,-1.35,-.433,-.4,-.1,0,0,0,
     *         1.2,3.333,5.8,-.85,-.4,0,-.267,-1.1,-.333,-.4,-.2,0,0, 
     *         1.2,3.333,6.4,-.6,-.233,-.7,-.5,-.6,-.267,0,0,0,0, 
     *         1.4,2.4,4.,7.,-.2,-.686,-.072,-1.,-.5,-.5,-.5,0,0/
      data rk2ms70/1.2,2.,.4,1.8,0,-.9,-.3,.5,-.12,-.65,-.4,-.1,-.033,
     *         1.2,2.,.4,-.2,.35,-.6,-.057,-.65,-.4,-.1,-.033,0,0,
     *         1.2,2.,.4,-2.4,.8,-.047,-.65,-.4,-.1,-.033,0,0,0,
     *         .8,1.6,-3.,1.,-.25,.4,-.16,-.35,-.367,-.25,-.1,0,0,
     *         .8,1.6,-3.8,.85,.333,-.45,0,-.2,-.433,-.35,-.1,0,0,
     *         .8,1.6,-4.4,.6,-.033,-.5,-.2,-.3,-.2,0,0,0,0,
     *         .2,.8,2.,-3.,.2,.686,-.145,-.25,-.1,-.25,-.2,0,0/
      data j1ms140/11,11,10,10,9,9,12/ 
      data j2ms140/11,11,10,9,10,10,12/
      data h1s140/75,85,90,95,100,120,130,140,200,220,250,0,0,
     *        75,85,90,95,100,120,130,140,200,220,250,0,0,
     *        75,85,90,95,100,120,140,200,220,250,0,0,0,
     *        75,80,95,100,120,140,200,220,250,270,0,0,0,
     *        75,80,95,100,120,200,220,250,270,0,0,0,0,
     *        75,80,95,100,130,200,220,250,270,0,0,0,0,
     *        75,80,85,95,100,110,140,180,200,220,250,270,0/
      data h2s140/75,80,90,95,100,120,130,155,200,220,250,0,0,
     *        75,80,90,95,100,120,130,160,200,220,250,0,0,
     *        75,80,90,95,100,120,165,200,220,250,0,0,0,
     *        75,80,95,100,120,180,200,250,270,0,0,0,0,
     *        75,80,95,100,120,160,200,220,250,270,0,0,0,
     *        75,80,95,100,130,160,200,220,250,270,0,0,0,
     *        75,80,90,95,100,110,140,180,200,220,250,270,0/
      data R1ms140/6,30,60,63,59,59,66,66,38,14,1,0,0,
     *         6,30,60,63,69,62,66,66,38,14,1,0,0,
     *         6,30,60,63,80,65,65,38,14,1,0,0,0,
     *         4,10,60,85,66,66,38,22,9,1,0,0,0,
     *         4,10,60,89,71,42,26,17,10,0,0,0,0,
     *         4,10,60,93,71,48,35,22,10,0,0,0,0,
     *         1,8,20,60,95,93,72,60,58,40,26,13,0/ 
      data R2ms140/4,10,30,32,41,41,30,30,10,6,1,0,0,
     *         4,10,30,32,31,38,31,29,9,6,1,0,0,
     *         4,10,30,32,20,35,26,9,6,1,0,0,0,
     *         2,6,30,15,34,24,10,5,1,0,0,0,0,
     *         2,6,30,11,28,37,21,14,8,5,0,0,0,
     *         2,6,30,7,29,36,29,20,13,5,0,0,0,
     *         1,2,10,20,5,7,28,32,28,20,14,7,0/ 
      data rk1ms140/2.4,6.,.6,-.8,0,.7,0,-.467,-1.2,-.433,0,0,0,
     *         2.4,6.,.6,1.2,-.35,.4,0,-.467,-1.2,-.433,0,0,0,    
     *         2.4,6.,.6,3.4,-.75,0,-.45,-1.2,-.433,0,0,0,0,
     *         1.2,3.333,5.,-.95,0,-.467,-.8,-.433,-.4,0,0,0,0,
     *         1.2,3.333,5.8,-.9,-.363,-.8,-.3,-.35,-.3,0,0,0,0,
     *         1.2,3.333,6.6,-.733,-.329,-.65,-.433,-.6,-.267,0,0,0,0,
     *         1.4,2.4,4.,7.,-.2,-.7,-.3,-.1,-.9,-.467,-.65,-.333,0/
      data rk2ms140/1.2,2.,.4,1.8,0,-1.1,0,-.444,-.2,-.166,0,0,0,
     *         1.2,2.,.4,-.2,.35,-.7,-.067,-.5,-.15,-.166,0,0,0,
     *         1.2,2.,.4,-2.4,.75,-.2,-.486,-.15,-.166,0,0,0,0,
     *         .8,1.6,-3.,.95,-.167,-.7,-.1,-.2,0,0,0,0,0,
     *         .8,1.6,-3.8,.85,.225,-.4,-.35,-.2,-.15,-.133,0,0,0,
     *         .8,1.6,-4.6,.733,.233,-.175,-.45,-.233,-.4,-.1,0,0,0, 
     *         .2,.8,2.,-3.,.2,.7,.1,-.2,-.4,-.2,-.35,-.167,0/
      data j1mr70/12,12,12,9,10,11,13/ 
      data j2mr70/9,9,10,13,12,11,11/
      data h1r70/75,80,90,95,100,120,140,180,200,220,250,270,0,
     *        75,80,90,95,100,120,145,180,200,220,250,270,0, 
     *        75,80,90,95,100,120,145,180,200,220,250,270,0,  
     *        75,95,100,110,140,180,200,250,270,0,0,0,0,
     *        75,95,125,150,185,195,200,220,250,270,0,0,0,
     *        75,95,100,150,160,170,190,200,220,250,270,0,0,
     *        75,80,85,95,100,140,160,170,190,200,220,250,270/
      data h2r70/75,95,100,120,180,200,220,250,270,0,0,0,0,
     *        75,95,100,120,180,200,220,250,270,0,0,0,0, 
     *        75,95,100,120,130,190,200,220,250,270,0,0,0, 
     *        75,80,85,95,100,110,130,180,190,200,220,250,270,
     *        75,80,85,95,100,125,150,190,200,220,250,270,0,
     *        75,80,85,95,100,150,190,200,220,250,270,0,0, 
     *        75,85,95,100,140,180,190,200,220,250,270,0,0/
      data R1mr70/13,17,57,57,30,53,58,38,33,14,6,2,0,
     *         13,17,57,57,37,56,56,38,33,14,6,2,0, 
     *         13,17,57,57,47,58,55,37,33,14,6,2,0, 
     *         5,65,54,58,58,38,33,9,1,0,0,0,0, 
     *         5,65,65,54,40,40,45,26,17,10,0,0,0,    
     *         5,65,76,56,57,48,44,51,35,22,10,0,0, 
     *         3,11,35,75,90,65,63,54,54,50,40,26,13/ 
      data R2mr70/7,43,70,47,15,17,10,4,0,0,0,0,0,
     *         7,43,63,44,17,17,10,4,0,0,0,0,0, 
     *         7,43,53,42,42,13,17,10,4,0,0,0,0,
     *         3,5,26,34,46,42,41,23,16,16,10,1,0,
     *         3,5,26,34,35,35,42,25,22,14,8,5,0, 
     *         3,5,26,34,24,41,31,26,20,13,5,0,0,
     *         3,15,15,10,35,35,30,34,20,14,7,0,0/ 
      data rk1mr70/.8,4.,0,-5.4,1.15,.25,-.5,-.25,-.95,-.267,-.2,
     *             -.067,0,
     *         .8,4.,0,-4.,.95,0,-.514,-.25,-.95,-.267,-.2,-.067,0, 
     *         .8,4.,0,-2.,.55,-.12,-.514,-.2,-.95,-.267,-.2,-.067,0, 
     *         3.,-2.2,.4,0,-.5,-.25,-.48,-.4,-.033,0,0,0,0,   
     *         3.,0,-.44,-.466,0,1.0,-.95,-.3,-.35,-.3,0,0,0, 
     *         3.,2.2,-.4,0.1,-.9,-.2,.7,-.8,-.433,-.6,-.267,0,0, 
     *         1.6,4.8,4.,3.,-.625,-.1,-.9,0,-.4,-.5,-.467,-.65,-.3/
      data rk2mr70/1.8,5.4,-1.15,-.533,.1,-.35,-.2,-.2,0,0,0,0,0,
     *         1.8,4.,-.95,-.45,0,-.35,-.2,-.2,0,0,0,0,0,
     *         1.8,2.,-.55,0,-.483,.4,-.35,-.2,-.2,0,0,0,0,    
     *         .4,4.2,.8,2.4,-.4,-.05,-.36,-.7,0,-.3,-.3,-.05,0,
     *         .4,4.2,.8,.2,0,.28,-.425,-.3,-.4,-.2,-.15,-.133,0,  
     *         .4,4.2,.8,-2.,.34,-.25,-.5,-.3,-.233,-.4,-.1,0,0,  
     *         1.2,0,-1.,.625,0,-.5,.4,-.7,-.2,-.35,-.167,0,0/
      data j1mr140/12,12,11,12,9,9,13/ 
      data j2mr140/10,9,10,12,13,13,12/
      data h1r140/75,80,90,95,100,115,130,145,200,220,250,270,0,
     *        75,80,90,95,100,110,120,145,200,220,250,270,0, 
     *        75,80,90,95,100,115,150,200,220,250,270,0,0,
     *        75,95,100,120,130,140,150,190,200,220,250,270,0,
     *        75,95,120,150,190,200,220,250,270,0,0,0,0,
     *        75,95,100,145,190,200,220,250,270,0,0,0,0,
     *        75,80,85,95,100,120,160,170,190,200,220,250,270/
      data h2r140/75,95,100,115,130,175,200,220,250,270,0,0,0,
     *        75,95,100,110,175,200,220,250,270,0,0,0,0, 
     *        75,95,100,115,130,180,200,220,250,270,0,0,0, 
     *        75,80,85,95,100,120,130,190,200,220,250,270,0,
     *        75,80,85,95,100,120,140,160,190,200,220,250,270,
     *        75,80,85,95,100,145,165,180,190,200,220,250,270,
     *        75,85,95,100,120,145,170,190,200,220,250,270,0/
      data R1mr140/13,17,57,57,28,51,56,56,12,8,1,0,0,
     *         13,17,57,57,36,46,55,56,10,8,1,0,0, 
     *         13,17,57,57,46,56,55,12,8,1,0,0,0,
     *         5,65,54,59,56,56,53,23,16,13,3,1,0,
     *         5,65,65,54,29,16,16,10,2,0,0,0,0,
     *         5,65,76,58,36,25,20,12,7,0,0,0,0,
     *         3,11,35,75,91,76,58,49,45,32,28,20,12/ 
      data R2mr140/7,43,72,49,44,14,7,4,1,0,0,0,0,
     *         7,43,64,51,14,7,4,1,0,0,0,0,0, 
     *         7,43,54,44,44,13,7,4,1,0,0,0,0,
     *         3,5,26,34,46,41,44,9,11,7,2,1,0,
     *         3,5,26,34,35,35,40,40,16,14,9,5,2, 
     *         3,5,26,34,24,40,40,32,19,20,10,7,3,
     *         3,15,15,9,24,35,40,28,28,20,10,8,0/ 
      data rk1mr140/.8,4.,0,-5.8,1.533,.333,0,-.8,-.2,-.233,-.05,0,0,
     *         .8,4.,0,-4.2,1.3,.6,.04,-.836,-.1,-.233,-.05,0,0,
     *         .8,4.,0,-2.2,.667,-.029,-.86,-.2,-.233,-.05,0,0,0,         
     *         3.,-2.2,.25,-.3,0,-.3,-.75,-.7,-.15,-.333,-.1,-.033,0,
     *         3.,0,-.367,-.625,-1.3,0,-.2,-.4,-.067,0,0,0,0, 
     *         3.,2.2,-.4,-.489,-1.1,-.25,-.267,-.25,-.2,0,0,0,0,
     *         1.6,4.8,4.,3.2,-.75,-.45,-.9,-.2,-1.3,-.2,-.267,-.4,-.3/
      data rk2mr140/1.8,5.8,-1.533,-.333,-.667,-.28,-.15,-.1,-.05,
     *              0,0,0,0,
     *         1.8,4.2,-1.3,-.569,-.28,-.15,-.1,-.05,0,0,0,0,0,
     *         1.8,2.2,-.667,0,-.62,-.3,-.15,-.1,-.05,0,0,0,0,    
     *         .4,4.2,.8,2.4,-.25,.3,-.583,.2,-.2,-.167,-.05,-.033,0,
     *         .4,4.2,.8,.02,0,.25,0,-.6,-.2,-.25,-.133,-.15,-.067,  
     *         .4,4.2,.8,-2.,.356,0,-.533,-1.3,.1,-.5,-.1,-.2,-.1,  
     *         1.2,0,-1.2,.75,.44,.2,-.6,0,-.4,-.333,-.1,-.2,0/
      data j1mw70/13,13,13,13,9,8,9/ 
      data j2mw70/10,10,11,11,9,8,11/
      data h1w70/75,80,85,95,100,110,125,145,180,200,220,250,270,
     *        75,80,85,95,100,110,120,150,180,200,220,250,270,
     *        75,80,85,95,100,110,120,155,180,200,220,250,270,
     *        75,80,90,100,110,120,140,160,190,200,220,250,270, 
     *        75,80,90,110,150,200,220,250,270,0,0,0,0,
     *        75,80,90,100,150,200,250,270,0,0,0,0,0,
     *        75,80,90,100,120,130,140,200,270,0,0,0,0/
      data h2w70/75,90,95,100,110,125,190,200,250,270,0,0,0,
     *        75,90,95,100,110,125,190,200,250,270,0,0,0, 
     *        75,90,95,100,110,120,145,190,200,250,270,0,0,
     *        75,80,95,100,110,120,150,200,220,250,270,0,0,
     *        75,80,90,95,110,145,200,250,270,0,0,0,0,
     *        75,80,90,100,140,150,200,250,0,0,0,0,0,
     *        75,80,85,90,100,120,130,140,160,200,270,0,0/ 
      data R1mw70/28,35,65,65,28,44,46,50,25,25,10,5,0,
     *         28,35,65,65,36,49,47,47,25,25,10,5,0,
     *         28,35,65,65,48,54,51,43,25,25,10,5,0,
     *         16,24,66,54,58,50,50,38,25,25,10,5,0, 
     *         16,24,66,66,46,30,20,6,3,0,0,0,0,  
     *         16,24,66,76,49,32,12,7,0,0,0,0,0,      
     *         6,19,67,91,64,68,60,40,12,0,0,0,0/ 
      data R2mw70/5,35,35,72,56,54,12,12,2,0,0,0,0,
     *         5,35,35,64,51,53,12,12,2,0,0,0,0, 
     *         5,35,35,52,46,49,41,12,12,2,0,0,0,
     *         4,10,40,46,42,50,41,12,7,2,0,0,0,
     *         4,10,30,34,34,51,14,4,2,0,0,0,0,
     *         4,10,30,24,45,48,20,5,0,0,0,0,0,
     *         2,6,17,23,9,36,32,40,40,20,6,0,0/ 
      data rk1mw70/1.4,6.,0,-7.4,1.6,.133,.2,-.714,0,-.75,-.167,-.25,0,
     *         1.4,6.,0,-5.8,1.3,-.2,0,-.733,0,-.75,-.167,-.25,0,
     *         1.4,6.,0,-3.4,.6,-.3,-.229,-.72,0,-.75,-.167,-.25,0,
     *         1.6,4.2,-1.2,.4,-.8,0,-.6,-.433,0,-.75,-.167,-.25,0,   
     *         1.6,4.2,0,-.5,-.32,-.5,-.467,-.15,-.1,0,0,0,0,
     *         1.6,4.2,1.,-.54,-.34,-.4,-.25,-.2,0,0,0,0,0,      
     *         2.6,4.8,2.4,-1.35,.4,-.8,-.333,-.4,-.3,0,0,0,0/
      data rk2mw70/2.,0,7.4,-1.6,-.133,-.646,0,-.2,-.1,0,0,0,0,
     *         2.,0,5.8,-1.3,.133,-.631,0,-.2,-.1,0,0,0,0,    
     *         2.,0,3.4,-.6,.3,-.32,-.644,0,-.2,-.1,0,0,0,
     *         1.2,2.,1.2,-.4,.8,-.3,-.58,-.25,-.167,-.1,0,0,0,
     *         1.2,2.,.8,0,.486,-.673,-.2,-.1,-.066,0,0,0,0,  
     *         1.2,2.,-.6,.525,.3,-.56,-.3,-.1,0,0,0,0,0,  
     *         .8,2.2,1.2,-1.4,1.35,-.4,.8,0,-.5,-.2,-.167,0,0/
      data j1mw140/12,11,11,11,11,10,12/ 
      data j2mw140/10,11,11,11,11,10,12/
      data h1w140/75,80,85,95,100,110,125,145,190,200,220,250,0,
     *        75,80,85,95,100,110,120,150,190,220,250,0,0,
     *        75,80,85,95,100,110,120,155,190,220,250,0,0,
     *        75,80,90,100,110,120,140,160,190,220,250,0,0,
     *        75,80,90,110,150,160,190,200,220,250,270,0,0,
     *        75,80,90,100,150,160,190,200,250,270,0,0,0,
     *        75,80,90,100,120,130,140,160,190,200,250,270,0/
      data h2w140/75,90,95,100,110,125,190,200,220,250,0,0,0,
     *        75,90,95,100,110,120,125,190,200,220,250,0,0,
     *        75,90,95,100,110,120,145,190,200,220,250,0,0,  
     *        75,80,95,100,110,120,150,190,200,220,250,0,0,
     *        75,80,90,95,110,145,190,200,220,250,270,0,0,
     *        75,80,90,100,140,150,200,220,250,270,0,0,0,
     *        75,80,85,90,100,120,130,140,160,180,200,220,0/ 
      data R1mw140/28,35,65,65,28,44,46,50,9,6,2,0,0,
     *         28,35,65,65,36,49,47,47,8,2,0,0,0,
     *         28,35,65,65,48,54,51,43,8,2,0,0,0,
     *         16,24,66,54,58,50,50,42,8,2,0,0,0, 
     *         16,24,66,66,46,49,9,10,7,2,0,0,0,
     *         16,24,66,76,49,54,10,14,4,1,0,0,0,
     *         6,19,67,91,64,68,60,58,11,20,5,2,0/ 
      data R2mw140/5,35,35,72,56,54,5,5,1,0,0,0,0,
     *         5,35,35,64,51,53,53,5,5,1,0,0,0,
     *         5,35,35,52,46,49,41,5,5,1,0,0,0,    
     *         4,10,40,46,42,50,41,5,5,1,0,0,0,   
     *         4,10,30,34,34,51,10,5,3,1,0,0,0,  
     *         4,10,30,24,45,48,4,2,1,0,0,0,0,
     *         2,6,17,23,9,36,32,40,39,29,1,0,0/ 
      data rk1mw140/1.4,6.,0,-7.4,1.6,.133,.2,-.911,-.3,-.2,-.066,0,0,
     *         1.4,6.,0,-5.8,1.3,-.2,0,-.975,-.2,-.066,0,0,0,
     *         1.4,6.,0,-3.4,.6,-.3,-.229,-1.,-.2,-.066,0,0,0,
     *         1.6,4.2,-1.2,.4,-.8,0,-.4,-1.133,-.2,-.066,0,0,0,
     *         1.6,4.2,0,-.5,.3,-1.133,.1,-.15,-.166,-.1,0,0,0,
     *         1.6,4.2,1.,-.54,.5,-1.466,.4,-.2,-.15,-.0333,0,0,0,
     *         2.6,4.8,2.4,-1.35,.4,-.8,-.1,-1.566,.9,-.3,-.15,-.05,0/
      data rk2mw140/2.,0,7.4,-1.6,-.133,-.754,0,-.2,-.033,0,0,0,0,
     *         2.,0,5.8,-1.3,.2,0,-.738,0,-.2,-.033,0,0,0,
     *         2.,0,3.4,-.6,.3,-.32,-.8,0,-.2,-.033,0,0,0,
     *         1.2,2.,1.2,-.4,.8,-.3,-.9,0,-.2,-.033,0,0,0,
     *         1.2,2.,.8,0,.486,-.911,-.5,-.1,-.066,-.05,0,0,0,
     *         1.2,2.,-.6,.525,.3,-.88,-.1,-.033,-.05,0,0,0,0,
     *         .8,2.2,1.2,-1.4,1.35,-.4,.8,-.05,-.5,-1.4,-.05,0,0/

        h = hei
        z = xhi

         if(z.lt.20)z=20
         if(z.gt.90)z=90
        if((it.eq.1).or.(it.eq.2).or.(it.eq.11).or.(it.eq.12))then
         if(f.lt.140)then
           Call aprok(j1mw70,j2mw70,h1w70,h2w70,R1mw70,R2mw70,        
     *                rk1mw70,rk2mw70,h,z,R1,R2) 
           R170=R1
           R270=R2
           endif
         if(f.gt.70)then
           Call aprok(j1mw140,j2mw140,h1w140,h2w140,R1mw140,R2mw140,        
     *                rk1mw140,rk2mw140,h,z,R1,R2) 
           R1140=R1
           R2140=R2
           endif
         if((f.gt.70).and.(f.lt.140))then
           R1=R170+(R1140-R170)*(f-70)/70
           R2=R270+(R2140-R270)*(f-70)/70
           endif
         endif
        if((it.eq.5).or.(it.eq.6).or.(it.eq.7).or.(it.eq.8))then
         if(f.lt.140)then
           Call aprok(j1ms70,j2ms70,h1s70,h2s70,R1ms70,R2ms70,        
     *                rk1ms70,rk2ms70,h,z,R1,R2) 
           R170=R1
           R270=R2
           endif
         if(f.gt.70)then
           Call aprok(j1ms140,j2ms140,h1s140,h2s140,R1ms140,R2ms140,        
     *                rk1ms140,rk2ms140,h,z,R1,R2) 
           R1140=R1
           R2140=R2
           endif
         if((f.gt.70).and.(f.lt.140))then
           R1=R170+(R1140-R170)*(f-70)/70
           R2=R270+(R2140-R270)*(f-70)/70
           endif
         endif
        if((it.eq.3).or.(it.eq.4).or.(it.eq.9).or.(it.eq.10))then
         if(f.lt.140)then
           Call aprok(j1mr70,j2mr70,h1r70,h2r70,R1mr70,R2mr70,        
     *                rk1mr70,rk2mr70,h,z,R1,R2) 
           R170=R1
           R270=R2
           endif
         if(f.gt.70)then
           Call aprok(j1mr140,j2mr140,h1r140,h2r140,R1mr140,R2mr140,
     *                rk1mr140,rk2mr140,h,z,R1,R2) 
           R1140=R1
           R2140=R2
           endif
         if((f.gt.70).and.(f.lt.140))then
           R1=R170+(R1140-R170)*(f-70)/70
           R2=R270+(R2140-R270)*(f-70)/70
           endif
         endif
        R3=0
        R4=0
        if (h.lt.100) R3=100-(R1+R2)
        if (h.ge.100) R4=100-(R1+R2)
         if(R3.lt.0) R3=0
         if(R4.lt.0) R4=0
        R1=ANINT(R1)
        R2=ANINT(R2)
        R3=ANINT(R3)
        R4=ANINT(R4)
 300   continue
        end
c
c
      Subroutine aprok(j1m,j2m,h1,h2,R1m,R2m,rk1m,rk2m,hei,xhi,R1,R2)
c----------------------------------------------------------------- 
      dimension   zm(7),j1m(7),j2m(7),h1(13,7),h2(13,7),R1m(13,7),
     *            R2m(13,7),rk1m(13,7),rk2m(13,7)
      data        zm/20,40,60,70,80,85,90/
      
        h=hei
        z=xhi

         j1=1
         j2=1
         i1=1
       do 1 i=1,7
         i1=i
        if(z.eq.zm(i)) j1=0
        if(z.le.zm(i)) goto 11
 1     continue
 11    continue
          i2=1
         do 2 i=2,j1m(i1)
          i2=i-1
          if(h.lt.h1(i,i1)) goto 22
          i2=j1m(i1)
 2       continue
 22      continue
          i3=1
         do 3 i=2,j2m(i1)
          i3=i-1
          if(h.lt.h2(i,i1)) goto 33
          i3=j2m(i1)
 3       continue
 33      continue
        R01=R1m(i2,i1)
        R02=R2m(i3,i1)
        rk1=rk1m(i2,i1)
        rk2=rk2m(i3,i1)
        h01=h1(i2,i1)
        h02=h2(i3,i1)
        R1=R01+rk1*(h-h01)
        R2=R02+rk2*(h-h02)
        if(j1.eq.1)then
          j1=0
          j2=0
          i1=i1-1
          R11=R1
          R12=R2
          goto 11
        endif
        if(j2.eq.0)then
          rk=(z-zm(i1))/(zm(i1+1)-zm(i1)) 
          R1=R1+(R11-R1)*rk
          R2=R2+(R12-R2)*rk
        endif
       end
c
c
        subroutine ioncomp(xy,id,ismo,xm,hx,zd,fd,fp,fs,dion)
c-------------------------------------------------------
c       xy      decimal year
c       id       day of year
c       ismo    seasonal month (Northern Hemisphere January 
c                   is ismo=1 and so is Southern H. July)
c       xm      MLT in hours
c       hx      altitude in km
c       zd      solar zenith angle in degrees
c       fd      latitude in degrees
c       fp      longitude in degrees
c       fs      10.7cm solar radio flux
c       dion(1)   O+  relative density in percent
c       dion(2)   H+  relative density in percent
c       dion(3)   N+  relative density in percent
c       dion(4)   He+ relative density in percent
c       dion(5)   NO+ relative density in percent
c       dion(6)   O2+ relative density in percent
c       dion(7)   Cluster+ relative density in percent
c-------------------------------------------------------
        dimension       dion(7),diont(7)
        common  /const/ umr
        do 1122 i=1,7
1122                diont(i)=0.

		xmlt = xm
		iddd = id
		ryear = xy
        month_sea = ismo
        h = hx
        xhi = zd
        xlati = fd
        xlongi = fp
        cov = fs
         if (h.gt.300.) then
        	call igrf_sub(xlati,xlongi,ryear,h,
     &  	    xl,icode,dipl,babs,IER)
        	if(xl.gt.10.) xl=10.
c        	icd=1    ! compute INVDIP
        	dimo=0.311653
			call CALION(1,xinvdip,xl,dimo,babs,dipl,xmlt,h,
     &  	 iddd,cov,xioncomp_O,xioncomp_H,xioncomp_He,xioncomp_N)
       		diont(1)=xioncomp_O*100.
        	diont(2)=xioncomp_H*100.
        	diont(3)=xioncomp_N*100.
        	diont(4)=xioncomp_He*100.
        else
        	call ionco2(h,xhi,month_sea,cov,rno,ro2,rcl,ro)
        	diont(5)=rno
        	diont(6)=ro2
       		diont(7)=rcl
        	diont(1)=ro
        endif
        do 1 i=1,7
                dion(i)=diont(i)
1               continue
        return
        end
c
c
        subroutine ionco1(h,zd,fd,fs,t,cn)
c---------------------------------------------------------------
c ion composition model
c   A.D. Danilov and A.P. Yaichnikov, A New Model of the Ion
c   Composition at 75 to 1000 km for IRI, Adv. Space Res. 5, #7,
c   75-79, 107-108, 1985
c
c       h       altitude in km
c       zd      solar zenith angle in degrees
c       fd      latitude in degrees
c       fs      10.7cm solar radio flux
c       t       season (decimal month) 
c       cn(1)   O+  relative density in percent
c       cn(2)   H+  relative density in percent
c       cn(3)   N+  relative density in percent
c       cn(4)   He+ relative density in percent
c Please note: molecular ions are now computed in IONCO2
c       [cn(5)   NO+ relative density in percent
c       [cn(6)   O2+ relative density in percent
c       [cn(7)   cluster ions  relative density in percent
c---------------------------------------------------------------
c
c        dimension       cn(7),cm(7),hm(7),alh(7),all(7),beth(7),
c     &                  betl(7),p(5,6,7),var(6),po(5,6),ph(5,6),
c     &                  pn(5,6),phe(5,6),pno(5,6),po2(5,6),pcl(5,6)
        dimension       cn(4),cm(4),hm(4),alh(4),all(4),beth(4),
     &                  betl(4),p(5,6,4),var(6),po(5,6),ph(5,6),
     &                  pn(5,6),phe(5,6)

        common  /argexp/argmax
        common  /const/ umr
        data po/4*0.,98.5,4*0.,320.,4*0.,-2.59E-4,2.79E-4,-3.33E-3,
     &          -3.52E-3,-5.16E-3,-2.47E-2,4*0.,-2.5E-6,1.04E-3,
     &          -1.79E-4,-4.29E-5,1.01E-5,-1.27E-3/
        data ph/-4.97E-7,-1.21E-1,-1.31E-1,0.,98.1,355.,-191.,
     &          -127.,0.,2040.,4*0.,-4.79E-6,-2.E-4,5.67E-4,
     &          2.6E-4,0.,-5.08E-3,10*0./
        data pn/7.6E-1,-5.62,-4.99,0.,5.79,83.,-369.,-324.,0.,593.,
     &          4*0.,-6.3E-5,-6.74E-3,-7.93E-3,-4.65E-3,0.,-3.26E-3,
     &          4*0.,-1.17E-5,4.88E-3,-1.31E-3,-7.03E-4,0.,-2.38E-3/
        data phe/-8.95E-1,6.1,5.39,0.,8.01,4*0.,1200.,4*0.,-1.04E-5,
     &          1.9E-3,9.53E-4,1.06E-3,0.,-3.44E-3,10*0./ 
c       data pno/-22.4,17.7,-13.4,-4.88,62.3,32.7,0.,19.8,2.07,115.,
c    &          5*0.,3.94E-3,0.,2.48E-3,2.15E-4,6.67E-3,5*0.,
c    &          -8.4E-3,0.,-3.64E-3,2.E-3,-2.59E-2/
c       data po2/8.,-12.2,9.9,5.8,53.4,-25.2,0.,-28.5,-6.72,120.,
c    &          5*0.,-1.4E-2,0.,-9.3E-3,3.3E-3,2.8E-2,5*0.,4.25E-3,
c    &          0.,-6.04E-3,3.85E-3,-3.64E-2/
c       data pcl/4*0.,100.,4*0.,75.,10*0.,4*0.,-9.04E-3,-7.28E-3,
c    &          2*0.,3.46E-3,-2.11E-2/

        z=zd*umr
        f=fd*umr

        DO 8 I=1,5
        DO 8 J=1,6
                p(i,j,1)=po(i,j)
                p(i,j,2)=ph(i,j)
                p(i,j,3)=pn(i,j)
                p(i,j,4)=phe(i,j)
c               p(i,j,5)=pno(i,j)
c               p(i,j,6)=po2(i,j)
c               p(i,j,7)=pcl(i,j)
8       continue

        s=0.
c       do 5 i=1,7
        do 5 i=1,4
          do 7 j=1,6
                var(j) = p(1,j,i)*cos(z) + p(2,j,i)*cos(f) +
     &                   p(3,j,i)*cos(0.013*(300.-fs)) +
     &                   p(4,j,i)*cos(0.52*(t-6.)) + p(5,j,i)
7         continue
          cm(i)  = var(1)
          hm(i)  = var(2)
          all(i) = var(3)
          betl(i)= var(4)
          alh(i) = var(5)
          beth(i)= var(6)
          hx=h-hm(i)
          if(hx) 1,2,3
1               arg = hx * (hx * all(i) + betl(i)) 
                cn(i) = 0.
                if(arg.gt.-argmax) cn(i) = cm(i) * exp( arg )
                goto 4
2               cn(i) = cm(i)
                goto 4
3               arg = hx * (hx * alh(i) + beth(i)) 
                cn(i) = 0.
                if(arg.gt.-argmax) cn(i) = cm(i) * exp( arg )
4         continue
          if(cn(i).LT.0.005*cm(i)) cn(i)=0.
          if(cn(i).GT.cm(i)) cn(i)=cm(i)
          s=s+cn(i)
5       continue
c       do 6 i=1,7
        do 6 i=1,4
6               cn(i)=cn(i)/s*100.
        return
        end
c
c
	SUBROUTINE CALION(CRD,INVDIP,FL,DIMO,B0,
     &                    DIPL,MLT,ALT,DDD,F107,NO,NH,NHE,NN)
C Version 1.0 (released 20.12.2002)
C CALION calculates relative density of O+, H+, He+ and N+  in the outer
C ionosphere with regard to solar activity (F107 index).
C CALION uses subroutines IONLOW and IONHIGH.
C Linearly interpolates for solar activity.
C Inputs: CRD - 0 .. INVDIP
C               1 .. FL, DIMO, B0, DIPL (used for calculation INVDIP inside)
C         INVDIP - "mix" coordinate of the dip latitude and of
C                    the invariant latitude;
C                    positive northward, in deg, range <-90.0;90.0>
C         FL, DIMO, B0 - McIlwain L parameter, dipole moment in
C                        Gauss, magnetic field strength in Gauss -
C                        parameters needed for invariant latitude
C                        calculation
C         DIPL - dip latitude
C                positive northward, in deg, range <-90.0;90.0>
C         MLT - magnetic local time (central dipole)
C               in hours, range <0;24)
C         ALT - altitude above the Earth's surface;
C               in km, range <350;2000>
C         DDD - day of year; range <0;365>
C         F107 - F107 index
C Output: NO,NH,NHE,NN - relative density of O+, H+, He+, and N+
C Versions:  1.0 FORTRAN
C Author of the code:
C         Vladimir Truhlik
C         Institute of Atm. Phys.
C         Bocni II.
C         141 31 Praha 4, Sporilov
C         Czech Republic
C         e-mail: vtr@ufa.cas.cz
C         tel/fax: +420 267103058, +420 728073539 / +420 272 762528
      REAL INVDIP,FL,DIMO,B0,DIPL,MLT,ALT,F107
	INTEGER CRD,DDD,ION
      REAL NO,NH,NHE,NN,NOH,NHH,NHEH,NNH,NOL,NHL,NHEL,NNL,NTOT
      DIMENSION  DOL(3,3,49),DHL(3,3,49),DHEL(3,3,49),DNL(3,3,49)
      DIMENSION  DOH(4,3,49),DHH(4,3,49),DHEH(4,3,49),DNH(4,3,49)
C/////////////////////coefficients high solar activity////////////////////////
C//////////////////////////////////O+/////////////////////////////////////////
C     550km equinox
      DATA (DOH(1,1,J),J=1,49)/-1.2838E-002, 3.3892E-009,-4.9527E-003,
     &                        9.6584E-009,-2.8249E-004, 9.2209E-009,
     &                       -1.5708E-003,-1.5997E-003,-1.7077E-010,
     &                        6.0805E-004,-3.2035E-009, 4.6574E-004,
     &                       -3.0838E-009,-1.7719E-003, 1.7746E-009,
     &                        2.0281E-003, 5.9550E-010,-8.4087E-004,
     &                        7.3645E-010, 1.6083E-003,-1.9020E-010,
     &                       -5.6389E-004, 2.1437E-010,-9.1530E-005,
     &                       -8.1471E-004,-1.4259E-009,-5.3451E-004,
     &                       -6.4989E-010, 9.7174E-005,-9.1365E-004,
     &                        3.6711E-010,-2.8037E-005, 7.6651E-011,
     &                        1.5422E-003, 1.4647E-010,-1.7107E-005,
     &                        2.1872E-010, 1.4265E-004, 6.1624E-010,
     &                        1.2512E-004, 4.8548E-004,-1.6132E-010,
     &                       -1.4022E-005,-1.1307E-004, 1.6024E-010,
     &                        2.7979E-004, 6.1773E-011,-2.7536E-005,
     &                        3.7412E-005/
C     550km June solstice
      DATA (DOH(1,2,J),J=1,49)/-1.3612E-002,-5.5550E-003,-8.8001E-003,
     &                       -6.7878E-003,-2.5424E-003,-6.6143E-004,
     &                       -3.0189E-003,-5.1701E-004,-3.4954E-004,
     &                       -2.9469E-005, 4.0382E-005, 3.1389E-006,
     &                        5.8065E-005, 1.6808E-003,-2.0046E-004,
     &                        1.1039E-003, 1.0451E-003,-1.0243E-004,
     &                       -1.6330E-004, 1.3405E-003,-4.0310E-004,
     &                        2.9640E-004, 3.3187E-004, 1.5748E-004,
     &                       -1.0201E-003,-3.4733E-004,-1.8801E-004,
     &                        1.3370E-004,-3.8261E-005, 2.5237E-004,
     &                        9.9983E-005, 5.3315E-005, 6.1462E-005,
     &                       -2.2161E-004, 3.8285E-004, 3.6890E-005,
     &                       -2.8965E-006,-2.6102E-005, 2.1884E-005,
     &                        5.6101E-005, 3.2898E-004, 4.8422E-005,
     &                        8.9837E-007, 2.0305E-005, 1.3643E-005,
     &                        1.1488E-004, 4.8798E-005,-1.7122E-006,
     &                        1.3591E-004/
C     900km equinox
      DATA (DOH(2,1,J),J=1,49)/-1.1873E-001,-7.9543E-008, 8.8754E-002,
     &                        1.2749E-007, 3.7834E-002,-3.7071E-008,
     &                       -3.3659E-002,-1.3753E-001,-6.7705E-008,
     &                       -7.8370E-003, 5.0045E-008, 1.7354E-002,
     &                       -1.2451E-008,-5.5262E-002,-1.6397E-008,
     &                        9.0203E-003,-3.1764E-009, 3.2200E-004,
     &                        4.4309E-009,-4.7726E-002,-1.6271E-008,
     &                       -1.6568E-002, 8.7942E-009, 3.9707E-003,
     &                       -1.3873E-002,-1.3387E-008,-5.5410E-003,
     &                        3.2760E-009, 1.5026E-003, 5.4130E-004,
     &                       -4.3665E-009,-8.5634E-003, 2.2732E-009,
     &                       -7.3394E-003,-9.4292E-009,-4.1713E-003,
     &                        2.0113E-009, 2.5152E-003,-1.4286E-010,
     &                       -2.7636E-003, 1.4591E-003,-3.7848E-009,
     &                       -2.1361E-003,-1.7300E-003,-6.3066E-012,
     &                       -1.8190E-003,-1.1929E-009,-2.4384E-003,
     &                        4.5096E-003/
C     900km June solstice
      DATA (DOH(2,2,J),J=1,49)/-1.1400E-001, 7.2284E-002, 5.9514E-002,
     &                       -9.2827E-002, 2.4670E-002, 1.8185E-002,
     &                       -5.3528E-003,-1.0798E-001, 4.9821E-002,
     &                       -5.4609E-003,-7.3866E-003, 5.5822E-003,
     &                       -2.2066E-003,-9.4552E-003,-7.0287E-003,
     &                        1.5038E-002,-4.9536E-003,-2.1970E-003,
     &                        1.8018E-003,-5.1372E-002, 1.5935E-002,
     &                       -7.7984E-004,-9.8830E-004,-6.1488E-004,
     &                       -5.4875E-003,-6.8829E-003, 9.0910E-003,
     &                       -3.6180E-003, 4.6157E-004,-3.7871E-003,
     &                       -9.0714E-005, 6.0385E-004, 1.6097E-004,
     &                       -7.2617E-003,-1.9084E-003, 3.5375E-003,
     &                       -8.3529E-004,-7.7380E-003, 1.0796E-004,
     &                        5.0314E-004,-1.3759E-002, 5.3603E-004,
     &                        6.4259E-004,-6.4764E-003,-1.6915E-004,
     &                       -7.3788E-003, 3.5767E-004,-3.8287E-003,
     &                       -3.2403E-003/
C     1500km equinox
      DATA (DOH(3,1,J),J=1,49)/-5.0096E-001, 3.9699E-007, 5.4222E-001,
     &                       -4.2933E-007, 4.2261E-002,-1.2006E-007,
     &                       -1.3304E-001,-5.7614E-001, 1.9950E-007,
     &                        4.2972E-002,-3.6102E-008, 3.6207E-002,
     &                       -8.1212E-009,-3.0608E-001, 7.6853E-008,
     &                        6.8090E-002,-5.5044E-008,-4.6984E-003,
     &                        2.6068E-008,-1.5518E-001,-2.0579E-008,
     &                       -2.1672E-002, 9.6017E-009, 7.6241E-003,
     &                       -1.5461E-001, 3.9902E-008, 3.9501E-003,
     &                       -9.3205E-009, 3.6564E-003,-1.8591E-002,
     &                       -1.5121E-008,-1.0493E-002, 5.6094E-009,
     &                        2.3030E-002,-4.0654E-008,-6.5416E-004,
     &                        1.2860E-010,-3.5348E-002, 1.1426E-008,
     &                       -4.0794E-003, 2.4538E-002,-2.8703E-008,
     &                        1.1241E-003, 3.0503E-003, 7.4212E-010,
     &                       -9.1099E-003,-6.3420E-009,-2.4364E-003,
     &                        7.8412E-003/
C     1500km June solstice
      DATA (DOH(3,2,J),J=1,49)/-3.8369E-001, 2.8162E-001, 3.3410E-001,
     &                       -4.4591E-001, 1.2217E-001, 1.4901E-001,
     &                       -1.2365E-001,-3.3668E-001, 1.4641E-001,
     &                        1.5156E-002,-4.2536E-002, 2.6524E-002,
     &                       -5.5077E-003,-1.1957E-001, 6.0421E-002,
     &                       -3.9161E-003,-1.0382E-002, 1.1853E-002,
     &                       -7.9129E-003, 5.4055E-002,-3.9953E-002,
     &                        7.1875E-003, 6.0955E-003,-3.0049E-003,
     &                       -4.5588E-003,-7.0102E-003, 8.8947E-005,
     &                        4.6705E-003,-1.5544E-003, 5.2816E-002,
     &                       -3.2857E-002, 7.5772E-003, 3.3781E-004,
     &                        5.3629E-002,-2.2430E-002, 2.4118E-003,
     &                        3.1271E-004,-1.4393E-002,-6.6816E-003,
     &                        1.7099E-003, 4.2075E-003,-2.0012E-003,
     &                       -6.8627E-004,-1.6996E-002, 1.9841E-003,
     &                       -1.0077E-002,-4.0022E-004,-3.5827E-003,
     &                        1.0792E-003/
C     2250km equinox
      DATA (DOH(4,1,J),J=1,49)/-7.5121E-001, 4.7106E-006, 9.8731E-001,
     &                       -1.1517E-005,-2.1953E-001, 7.7259E-006,
     &                       -7.3014E-002,-3.4116E-001,-1.3414E-006,
     &                        3.8833E-002,-6.9663E-007, 1.0871E-002,
     &                        5.1607E-007,-5.2445E-001, 1.5335E-006,
     &                        1.1675E-001,-9.3384E-007,-8.9381E-003,
     &                        1.5077E-007,-4.9602E-003,-6.4438E-007,
     &                       -1.0283E-002,-3.9542E-008, 1.7114E-003,
     &                       -1.1767E-001, 2.1163E-007,-1.4566E-003,
     &                       -2.4207E-007, 1.9790E-003,-8.8082E-003,
     &                       -4.2416E-008, 1.0644E-003,-2.3959E-009,
     &                        9.4817E-002,-7.9695E-007,-4.1841E-003,
     &                       -5.9703E-008, 4.2488E-002, 4.4664E-008,
     &                        1.6134E-003, 4.5698E-002,-2.0829E-007,
     &                       -1.2046E-003,-1.0719E-004, 1.8530E-007,
     &                        8.5126E-003, 4.9934E-008, 7.7988E-003,
     &                        1.4431E-003/
C     2250km June solstice
      DATA (DOH(4,2,J),J=1,49)/-8.2190E-001, 3.6727E-001, 8.6943E-001,
     &                       -5.1681E-001, 4.3418E-002, 1.4628E-001,
     &                       -1.6190E-001,-4.8689E-001, 7.9939E-002,
     &                        2.9131E-002, 9.4168E-003, 1.6115E-002,
     &                       -1.1809E-002,-4.8438E-001, 1.0166E-001,
     &                        1.2800E-001,-6.1316E-002,-1.6983E-002,
     &                        1.4167E-002, 3.5405E-003,-7.8288E-003,
     &                       -3.2292E-003, 8.7041E-003,-3.6171E-003,
     &                       -1.5451E-001,-1.1808E-002, 2.8659E-002,
     &                       -2.7932E-004,-2.7786E-003, 1.5649E-001,
     &                       -4.8513E-003,-2.4998E-003,-9.9171E-004,
     &                       -8.0071E-002,-2.4185E-003, 6.4073E-003,
     &                       -2.6383E-004,-4.7954E-003,-8.6720E-003,
     &                        1.5965E-003,-7.1514E-003, 7.0614E-003,
     &                       -1.8408E-003,-1.1994E-002,-4.7379E-003,
     &                       -2.0841E-002,-2.9637E-003,-4.9867E-003,
     &                       -1.4069E-002/
C////////////////////////////////////////////////////////////////////////////////////	
C//////////////////////////////////H+////////////////////////////////////////////////
C     550km equinox
      DATA (DHH(1,1,J),J=1,49)/-3.1678E+000, 3.6289E-007,-5.5819E-002,
     &                       -1.0303E-006,-1.1119E-001,-1.7286E-007,
     &                        2.9466E-002, 6.9927E-001,-1.0867E-007,
     &                       -3.8446E-002,-3.1427E-008, 1.4009E-002,
     &                        4.5348E-008, 2.6142E-001, 9.8583E-008,
     &                       -1.5370E-002,-8.8257E-008, 1.1618E-002,
     &                        9.8907E-008,-5.4724E-002, 2.2519E-007,
     &                        1.5621E-002,-1.0045E-008, 3.6467E-003,
     &                       -1.2359E-001, 4.9284E-008,-1.1143E-002,
     &                        2.3898E-009,-5.8604E-003,-1.0020E-003,
     &                       -1.1869E-007,-8.7861E-004,-1.0543E-008,
     &                        9.8818E-002, 7.4183E-008, 1.3692E-002,
     &                       -1.6456E-008,-3.8041E-002, 3.5978E-008,
     &                       -3.9582E-003, 2.8077E-002,-2.2537E-008,
     &                        1.5597E-003,-3.7610E-003,-2.4712E-008,
     &                        4.0460E-003, 7.7021E-009,-1.4101E-003,
     &                        1.6789E-002/
C     550km June solstice
      DATA (DHH(1,2,J),J=1,49)/-2.8141E+000,-4.4836E-001, 8.2760E-003,
     &                        1.9407E-001, 2.8119E-002,-2.6308E-002,
     &                       -1.5432E-002, 4.8983E-001, 8.6385E-002,
     &                       -7.5200E-002, 1.8661E-002, 4.9780E-003,
     &                       -1.6762E-002, 1.8896E-001,-8.6811E-002,
     &                        5.1824E-004,-6.4436E-003, 3.5544E-003,
     &                        9.4714E-003, 2.0099E-002, 1.9941E-002,
     &                        2.5581E-002,-5.2659E-003,-3.4378E-003,
     &                       -1.2902E-001, 1.4578E-002,-2.5016E-003,
     &                       -2.0760E-002, 3.4195E-003,-8.2338E-002,
     &                        5.0698E-003,-3.4081E-004,-2.7512E-003,
     &                       -4.0443E-002, 2.2658E-003,-8.0779E-003,
     &                       -3.1290E-003,-1.5350E-003, 2.5313E-003,
     &                       -3.4740E-003, 8.2225E-003,-2.6921E-003,
     &                        1.3247E-003, 5.7483E-003, 4.0066E-003,
     &                        1.3048E-002, 1.6612E-004, 6.5345E-003,
     &                        6.1813E-003/
C     900km equinox
      DATA (DHH(2,1,J),J=1,49)/-1.5293E+000, 1.9563E-007,-7.1593E-001,
     &                        5.0551E-008,-1.0967E-001, 1.4302E-007,
     &                        2.3166E-001, 7.0241E-001, 6.7276E-008,
     &                       -3.9113E-003,-2.3431E-008,-2.9279E-002,
     &                       -2.1680E-008, 3.0214E-001, 8.5569E-008,
     &                       -5.9577E-002,-7.4572E-008, 1.3847E-002,
     &                        1.9007E-008, 9.1829E-002,-8.0079E-008,
     &                        2.6453E-002, 2.1214E-008,-3.2018E-003,
     &                       -2.1978E-001, 1.7330E-009,-1.1862E-003,
     &                        2.2427E-008, 1.3491E-003, 2.8588E-002,
     &                       -5.3489E-008, 1.1135E-002,-1.3678E-009,
     &                        7.9619E-002, 1.5714E-008, 7.1930E-003,
     &                        2.9905E-009,-3.8042E-002, 1.1725E-008,
     &                        3.4274E-003,-4.9647E-003, 2.1675E-009,
     &                        7.9079E-003,-7.9338E-003, 3.6623E-008,
     &                        8.5068E-004, 1.7537E-009, 2.3308E-002,
     &                       -2.1869E-002/
C     900km June solstice
      DATA (DHH(2,2,J),J=1,49)/-1.3652E+000,-3.7141E-001,-6.9554E-001,
     &                        2.1456E-001, 9.1424E-002, 4.0012E-002,
     &                        9.3601E-002, 4.8116E-001, 3.2264E-002,
     &                        6.8285E-002, 9.8315E-003,-3.4080E-003,
     &                       -1.1225E-003, 1.2620E-001, 3.3225E-002,
     &                       -4.8420E-002,-2.8323E-002,-2.2914E-003,
     &                        7.9720E-003, 1.2455E-001,-4.2949E-002,
     &                       -5.3643E-003, 6.7441E-003, 1.4582E-002,
     &                       -4.0379E-002, 3.8848E-002,-3.2937E-003,
     &                        9.1013E-003,-7.5494E-003,-6.2580E-002,
     &                        1.1877E-002,-4.8900E-003,-5.3770E-003,
     &                        1.5066E-002,-9.2034E-004,-7.2370E-003,
     &                       -3.5355E-004, 8.6705E-003, 1.0198E-002,
     &                        2.0618E-003, 5.7210E-002, 6.8151E-003,
     &                       -1.8874E-003, 1.7634E-002, 9.8946E-003,
     &                        1.8101E-002, 5.3287E-003, 3.0321E-002,
     &                        6.8488E-003/
C     1500km equinox
      DATA (DHH(3,1,J),J=1,49)/-8.1841E-001, 1.3246E-007,-9.9407E-001,
     &                       -3.5826E-007,-1.1572E-001,-3.8968E-007,
     &                        1.8954E-001, 2.6643E-001,-1.2399E-008,
     &                        2.2029E-002, 3.5262E-008,-2.5895E-002,
     &                        2.5542E-008, 1.7279E-001,-1.0393E-008,
     &                       -3.0719E-002,-6.3703E-009, 7.5848E-003,
     &                       -1.4497E-008, 9.7870E-002,-1.3003E-007,
     &                       -2.0224E-003,-3.9366E-008,-3.2416E-003,
     &                        1.5904E-002,-1.5451E-008,-3.5652E-003,
     &                       -1.3658E-009,-7.7126E-003,-1.5924E-002,
     &                        1.6773E-008,-3.7276E-003, 1.7795E-008,
     &                        7.4727E-003, 3.0856E-008, 7.4715E-004,
     &                        7.3372E-009, 1.3716E-002, 4.6691E-008,
     &                        4.0197E-003,-1.1936E-002, 5.8220E-009,
     &                        3.8995E-003, 7.2715E-003,-2.0243E-008,
     &                        4.1793E-003,-3.2785E-009, 1.3978E-002,
     &                       -6.1225E-003/
C     1500km June solstice
      DATA (DHH(3,2,J),J=1,49)/-8.1500E-001,-3.1034E-001,-1.0382E+000,
     &                        2.2304E-001,-1.3799E-001, 6.2362E-002,
     &                        2.6902E-001, 2.6612E-001,-4.7466E-002,
     &                        1.9184E-002,-7.0576E-002,-1.7642E-002,
     &                        1.4685E-002, 7.7755E-002,-3.1315E-002,
     &                        1.9684E-002,-1.3324E-002, 3.4012E-003,
     &                        3.7043E-002,-1.8646E-002, 2.6457E-002,
     &                        9.8637E-003, 2.6819E-003, 1.7598E-003,
     &                       -2.1755E-002, 4.2552E-003,-1.6633E-002,
     &                       -6.7044E-003, 3.4964E-003,-4.8719E-003,
     &                        5.9790E-003, 2.3021E-005, 5.4778E-003,
     &                       -1.2598E-002, 9.9962E-003,-1.0507E-003,
     &                        5.7994E-003, 2.0765E-002, 1.1593E-002,
     &                       -3.2365E-004,-1.5353E-003, 1.4942E-003,
     &                        1.9642E-003, 8.8836E-003, 4.2172E-003,
     &                       -1.8502E-004, 2.3092E-003, 1.8370E-003,
     &                       -3.7978E-004/
C     2250km equinox
      DATA (DHH(4,1,J),J=1,49)/-6.6978E-001, 4.5698E-007,-1.2361E+000,
     &                       -1.5676E-007,-1.2804E-001,-6.0241E-007,
     &                        2.7591E-001, 4.0136E-002,-1.1570E-007,
     &                        2.1493E-002,-4.5863E-008, 2.6574E-003,
     &                        1.7588E-008, 1.5964E-001, 1.2470E-007,
     &                        1.8918E-002, 9.3632E-008,-2.4488E-003,
     &                        2.0725E-008, 6.6975E-002, 1.1782E-008,
     &                        1.9413E-002, 6.2877E-009, 5.3689E-003,
     &                       -1.5363E-002,-2.0022E-009,-2.1002E-002,
     &                       -6.7078E-009,-2.9894E-003, 6.1202E-003,
     &                        1.2119E-008,-2.2888E-003, 4.3298E-009,
     &                       -4.6405E-002,-5.8474E-008,-3.6078E-003,
     &                       -1.6963E-008,-1.8254E-002,-1.0377E-009,
     &                       -1.3715E-003,-2.2111E-003, 3.4185E-009,
     &                        9.5343E-004,-2.2094E-003, 2.7066E-009,
     &                       -2.4215E-003,-3.1546E-009,-1.0581E-003,
     &                       -1.0098E-003/
C     2250km June solstice
      DATA (DHH(4,2,J),J=1,49)/-5.6899E-001,-2.5609E-001,-1.0606E+000,
     &                       -1.6164E-002,-1.9427E-001, 1.0428E-001,
     &                        1.9448E-001, 1.1527E-001, 1.6518E-002,
     &                        3.6148E-002,-7.0688E-003, 1.0841E-002,
     &                        3.6512E-003, 1.0339E-001,-2.1704E-003,
     &                       -7.0271E-003, 1.9422E-002, 4.1677E-003,
     &                       -6.6189E-003, 4.3364E-002,-2.5002E-002,
     &                        1.4219E-002,-2.2654E-003, 5.6440E-003,
     &                       -3.7866E-002, 1.0338E-004,-3.9639E-003,
     &                       -3.3311E-003,-3.3289E-004,-5.2423E-002,
     &                        2.5430E-003,-7.1279E-003, 3.2787E-003,
     &                        5.9375E-003, 7.2932E-003,-1.9167E-003,
     &                       -1.8983E-003, 4.5326E-003, 6.8099E-003,
     &                       -7.4999E-005,-7.8634E-003,-5.5813E-003,
     &                       -5.3892E-004, 8.5280E-004, 1.4028E-003,
     &                        3.5032E-003,-3.7237E-004,-2.5767E-005,
     &                       -8.0710E-004/
C////////////////////////////////////////////////////////////////////////////////////	
C//////////////////////////////////He+///////////////////////////////////////////////
C     550km equinox
      DATA (DHEH(1,1,J),J=1,49)/-3.0827E+000, 4.4643E-007,-3.4361E-001,
     &                       -4.7996E-007,-2.9473E-001, 1.5576E-007,
     &                        2.0815E-001, 4.0991E-001,-2.9466E-008,
     &                       -5.9793E-002, 3.8185E-008, 2.5958E-002,
     &                       -5.2957E-008, 6.6694E-001, 1.1803E-007,
     &                       -2.9476E-002, 1.4225E-008,-1.3761E-002,
     &                       -7.2518E-008,-2.8967E-001,-2.0293E-008,
     &                        1.8383E-002, 1.0666E-008,-9.8637E-003,
     &                       -2.1268E-001, 4.3123E-008,-1.3389E-002,
     &                        2.6780E-009, 9.9442E-004,-7.8858E-003,
     &                        5.6098E-009,-4.9395E-003,-4.3890E-010,
     &                       -8.3089E-002, 5.4720E-008, 1.4305E-002,
     &                       -9.3089E-009,-6.8383E-003,-7.1157E-009,
     &                       -3.9638E-003, 9.9653E-003, 1.0788E-008,
     &                       -1.1729E-003,-2.9558E-003, 1.6849E-008,
     &                        1.2213E-003,-8.7021E-009,-1.4505E-002,
     &                       -5.6113E-004/
C     550km June solstice
      DATA (DHEH(1,2,J),J=1,49)/-2.9760E+000,-8.6799E-001, 4.8259E-002,
     &                        4.8124E-001,-9.2323E-003,-1.7802E-001,
     &                        1.2279E-001, 1.6776E-001, 2.1054E-001,
     &                       -5.1912E-002,-1.0298E-002, 1.5834E-002,
     &                       -8.3905E-003, 1.5721E-001,-2.7260E-002,
     &                       -6.7125E-002,-3.9449E-002, 4.7324E-003,
     &                        1.0327E-002,-1.8399E-001, 2.0860E-002,
     &                        4.3145E-002,-4.9043E-003,-5.6411E-003,
     &                       -2.4448E-001, 7.1468E-002,-1.2413E-002,
     &                       -1.3358E-002,-2.2098E-003,-3.9760E-002,
     &                       -7.9188E-003, 6.9939E-004,-5.3921E-003,
     &                       -5.5414E-002, 8.5107E-003,-9.8890E-003,
     &                        5.1361E-004, 6.6272E-002,-2.5510E-003,
     &                       -7.3423E-003, 2.0118E-003,-6.4885E-003,
     &                        2.7256E-003, 8.6778E-003,-8.9810E-004,
     &                        1.0350E-002,-3.1749E-003, 1.6338E-002,
     &                       -2.1051E-003/
C     900km equinox
      DATA (DHEH(2,1,J),J=1,49)/-1.7100E+000, 5.2540E-007,-8.2280E-001,
     &                       -8.2344E-007,-4.0545E-001,-5.5031E-007,
     &                        3.3911E-001, 2.5554E-001, 8.4842E-008,
     &                        8.3244E-002, 9.6193E-008,-6.0073E-002,
     &                       -2.0823E-007, 3.0060E-001, 7.9876E-008,
     &                       -1.1750E-001,-2.9524E-008,-3.5514E-002,
     &                       -2.9102E-008,-4.5851E-002,-4.7236E-009,
     &                        2.4348E-002, 4.2832E-009,-7.4072E-003,
     &                       -1.1252E-001, 9.5787E-008, 2.6864E-002,
     &                        1.7685E-008,-5.9893E-003,-1.6605E-001,
     &                       -1.3665E-007, 1.2534E-002, 1.1691E-008,
     &                        4.7303E-002, 6.8012E-010, 1.0472E-002,
     &                        1.9695E-009,-8.2855E-003,-8.1465E-009,
     &                        5.7602E-003, 7.8478E-003,-2.5233E-008,
     &                        3.0308E-003,-8.5876E-003, 2.5667E-009,
     &                        6.5844E-003,-1.7771E-008,-6.1409E-003,
     &                       -1.0949E-002/
C     900km June solstice
      DATA (DHEH(2,2,J),J=1,49)/-2.0393E+000,-8.1007E-001,-4.3286E-001,
     &                        3.0351E-001,-1.7024E-001, 4.6307E-002,
     &                       -2.4220E-003,-6.3442E-002,-1.6370E-003,
     &                        6.7460E-002,-1.0338E-002,-4.4583E-002,
     &                        9.6863E-003, 5.3744E-003, 2.6359E-003,
     &                       -5.8995E-002,-1.6310E-002, 7.3342E-003,
     &                        9.4701E-004, 6.9508E-002,-4.3418E-002,
     &                       -5.1968E-003,-3.0680E-003, 1.1837E-002,
     &                       -1.2321E-001, 4.2603E-002,-3.5437E-002,
     &                        1.6518E-002,-1.2969E-002, 3.2534E-002,
     &                        2.8450E-003, 3.5741E-003,-2.0381E-003,
     &                       -5.0507E-002, 8.2961E-003, 3.8507E-004,
     &                        1.0441E-003, 4.1571E-002,-2.9669E-003,
     &                       -7.7910E-004, 2.2211E-002,-3.5130E-003,
     &                        2.3044E-003, 2.9926E-002, 2.6085E-003,
     &                        2.5759E-002, 3.9804E-004, 3.1126E-002,
     &                        1.0860E-002/
C     1500km equinox
      DATA (DHEH(3,1,J),J=1,49)/-1.2078E+000,-8.3889E-008,-8.6323E-001,
     &                       -1.5199E-007,-4.4768E-001, 2.5090E-007,
     &                        3.5311E-001, 1.3210E-001,-4.2130E-009,
     &                       -2.5480E-003, 3.0688E-008,-4.3178E-002,
     &                        1.8903E-009, 8.9046E-002,-1.0776E-008,
     &                        2.9388E-002, 2.2664E-008,-1.1335E-002,
     &                       -1.4036E-008,-4.9870E-002,-2.0597E-008,
     &                        5.1163E-003, 1.5444E-008, 7.0597E-003,
     &                       -1.1044E-001, 3.2974E-008, 3.3729E-002,
     &                        9.6660E-009,-2.0066E-002,-4.7281E-002,
     &                        1.0175E-008, 4.5853E-003, 3.9224E-009,
     &                       -5.6840E-002, 3.2182E-008, 1.2884E-002,
     &                        6.6859E-009, 4.2958E-002, 7.5059E-009,
     &                        5.0972E-003,-3.4220E-002, 4.4177E-009,
     &                        6.6776E-003,-4.7692E-003, 1.1823E-008,
     &                       -8.8228E-003,-2.5359E-009,-1.1986E-002,
     &                       -5.1509E-003/
C     1500km June solstice
      DATA (DHEH(3,2,J),J=1,49)/-1.5728E+000,-6.8726E-001,-4.6811E-001,
     &                        2.4677E-002,-4.5196E-001, 1.5523E-001,
     &                        3.0631E-001, 1.4197E-001,-7.8123E-002,
     &                       -1.7428E-002,-1.7897E-002,-1.8015E-002,
     &                        2.7210E-002, 6.0701E-002,-1.3212E-001,
     &                       -2.2745E-002,-1.7608E-002,-1.2909E-002,
     &                        2.0402E-002,-4.0894E-003, 3.9555E-003,
     &                       -3.6464E-003, 1.4643E-002,-5.5944E-003,
     &                       -8.8090E-002,-1.0930E-002,-7.3501E-003,
     &                       -5.6046E-003, 2.6169E-003,-1.6782E-002,
     &                        1.4274E-002,-2.7868E-004, 2.1147E-003,
     &                       -1.8199E-002, 8.8925E-003,-1.5835E-003,
     &                        1.8784E-003, 1.1248E-002, 7.2779E-003,
     &                        4.4115E-004, 1.8361E-003,-1.1211E-003,
     &                        1.1060E-003, 1.1001E-002, 1.2477E-003,
     &                        9.0196E-003,-5.8886E-004, 9.7300E-003,
     &                        4.9376E-003/
C     2250km equinox
      DATA (DHEH(4,1,J),J=1,49)/-1.1230E+000,-2.9419E-007,-9.5822E-001,
     &                       -3.4817E-007,-3.5165E-001, 2.2927E-007,
     &                        3.3646E-001, 8.9248E-002,-1.1195E-007,
     &                        3.8351E-002,-7.4946E-008, 2.5411E-005,
     &                        2.1650E-008,-8.8392E-003,-1.7418E-008,
     &                        1.2222E-002, 4.5854E-009,-2.1108E-002,
     &                        3.0780E-008, 3.1391E-002,-9.2939E-009,
     &                        1.1868E-002,-2.3493E-009, 4.0791E-003,
     &                       -9.2411E-003,-3.1598E-009,-2.5908E-003,
     &                       -9.0595E-009,-1.0797E-003, 4.0200E-002,
     &                       -2.9380E-009,-1.5619E-003,-2.7627E-010,
     &                        2.2044E-002,-1.6962E-008,-8.5223E-003,
     &                       -3.6076E-009, 2.3448E-002,-4.9645E-009,
     &                       -5.8718E-004,-1.5134E-002,-8.6918E-009,
     &                       -2.3766E-003, 7.2023E-004, 9.0865E-010,
     &                        3.0388E-003,-2.6828E-009, 4.6501E-004,
     &                       -7.7390E-004/
C     2250km June solstice
      DATA (DHEH(4,2,J),J=1,49)/-1.3029E+000,-4.3965E-001,-4.3540E-001,
     &                       -1.2774E-001,-5.4683E-001, 8.9815E-002,
     &                        2.4691E-001, 1.5207E-001, 4.5816E-002,
     &                        4.2447E-002,-2.7108E-002, 2.7242E-003,
     &                       -7.3419E-003,-4.7334E-002, 5.0151E-002,
     &                       -2.1149E-002,-2.3378E-002, 4.3055E-003,
     &                       -1.6985E-002, 1.5813E-002,-1.6609E-002,
     &                        1.1566E-002, 6.2104E-003, 5.0605E-003,
     &                       -3.5118E-002,-2.0272E-002,-8.5016E-003,
     &                       -9.0160E-003,-6.8417E-003,-2.5045E-002,
     &                       -1.0184E-002,-6.3754E-003, 6.0554E-003,
     &                       -2.0355E-002,-2.8685E-003,-2.6734E-003,
     &                       -1.5766E-003, 6.4888E-003,-1.6234E-003,
     &                       -2.5859E-003, 6.6457E-003,-1.4526E-003,
     &                       -1.1457E-003, 2.2648E-003,-8.7925E-004,
     &                        4.1493E-003,-2.3712E-004, 7.9344E-004,
     &                        1.8450E-003/
C////////////////////////////////////////////////////////////////////////////////////
C///////////////////////////////////N+///////////////////////////////////////////////
C     550km equinox
      DATA (DNH(1,1,J),J=1,49)/-1.6313E+000, 3.3826E-009, 2.3816E-001,
     &                        2.8373E-007,-2.0961E-002,-5.3533E-007,
     &                        8.7160E-002, 9.8248E-002, 4.4888E-008,
     &                       -3.2244E-002,-2.0063E-008,-2.0732E-002,
     &                        1.4134E-009, 6.8670E-002, 3.8855E-008,
     &                       -7.9258E-002, 3.6878E-008, 3.0244E-002,
     &                       -3.3364E-008,-6.6861E-002, 3.2622E-008,
     &                        2.0265E-002,-1.4791E-008, 6.6954E-003,
     &                        2.1340E-002,-3.2037E-008, 9.0073E-003,
     &                        2.0858E-008,-5.2152E-003, 3.9714E-002,
     &                       -1.9344E-009,-8.9600E-005,-3.7487E-009,
     &                       -4.0648E-002, 1.7220E-008, 5.6330E-003,
     &                        4.4101E-009, 4.1397E-003,-4.3699E-010,
     &                       -3.5601E-003,-1.6551E-002,-2.6274E-009,
     &                        8.3802E-004, 1.2105E-003,-1.0517E-008,
     &                       -8.5131E-003, 4.5467E-009,-2.2753E-003,
     &                       -3.3013E-003/
C     550km June solstice
      DATA (DNH(1,2,J),J=1,49)/-1.5950E+000, 1.2602E-001, 2.4136E-001,
     &                        1.6546E-001, 1.7991E-002,-1.3398E-002,
     &                        9.3804E-002,-7.7106E-003, 1.4206E-002,
     &                       -2.3237E-003,-1.2863E-003, 2.3217E-004,
     &                       -2.2888E-003,-3.3507E-002, 2.1777E-002,
     &                       -2.3190E-002,-2.6550E-002, 5.4864E-003,
     &                        6.3951E-003,-4.8363E-002, 2.1732E-002,
     &                       -2.9250E-003,-7.9064E-003,-5.4980E-003,
     &                        4.0796E-002, 1.3459E-002, 7.4514E-003,
     &                       -5.7685E-003, 2.1205E-004,-1.9001E-002,
     &                       -1.4431E-003, 6.2449E-004,-1.2225E-003,
     &                        4.9048E-003,-1.4864E-002,-1.6067E-003,
     &                       -1.5020E-003,-3.7769E-003,-1.3646E-003,
     &                       -1.4496E-003,-1.2677E-002,-2.8348E-003,
     &                       -3.0407E-005,-3.0275E-003,-1.4841E-003,
     &                       -6.7625E-003,-2.2857E-003,-1.9180E-003,
     &                       -5.1806E-003/
C     900km equinox
      DATA (DNH(2,1,J),J=1,49)/-1.4724E+000, 6.2234E-007, 1.9984E-001,
     &                       -6.2436E-007,-5.9609E-002, 5.3063E-008,
     &                        4.0544E-002,-2.9918E-002, 4.1071E-007,
     &                        5.6303E-004,-4.7255E-008, 8.0654E-003,
     &                       -3.4962E-008, 1.6174E-002, 1.0136E-007,
     &                        1.1632E-002,-2.4108E-008, 1.0895E-002,
     &                       -1.4658E-008,-4.3236E-002, 1.5584E-007,
     &                       -7.6984E-003, 1.8395E-008, 6.3333E-004,
     &                       -5.2038E-003, 1.5845E-007,-6.8821E-003,
     &                       -1.8127E-008, 1.2382E-003,-6.6883E-002,
     &                        6.0510E-008,-7.3364E-003, 1.1055E-008,
     &                       -2.0130E-002, 1.0636E-007,-4.3419E-003,
     &                       -5.6930E-009,-1.9632E-002, 2.3733E-008,
     &                       -1.7396E-003, 4.7721E-003, 5.6015E-008,
     &                       -1.6795E-003,-4.7359E-004,-1.4109E-008,
     &                        1.4272E-004, 2.0978E-008,-2.9585E-003,
     &                        1.0551E-002/
C     900km June solstice
      DATA (DNH(2,2,J),J=1,49)/-1.4816E+000, 2.5836E-001, 2.5713E-001,
     &                       -1.1671E-002,-1.1685E-001,-8.0473E-002,
     &                       -2.5842E-002, 1.7538E-002, 3.1480E-002,
     &                       -5.2361E-003,-2.0114E-002, 1.0788E-002,
     &                        6.0475E-003,-8.2585E-002, 1.5506E-002,
     &                       -9.3973E-003, 6.9887E-003, 1.6035E-003,
     &                       -4.6125E-003, 1.2645E-003, 2.8444E-002,
     &                        8.2369E-003,-2.6763E-003,-7.5445E-004,
     &                        2.6349E-002,-1.6736E-002, 1.3463E-003,
     &                        1.7333E-005,-9.0561E-004,-8.1050E-003,
     &                       -5.6762E-003, 3.1943E-003,-3.0766E-004,
     &                       -2.6193E-002,-1.2177E-002,-1.2289E-003,
     &                        4.1217E-004,-2.4327E-002,-6.1348E-003,
     &                       -1.2113E-003,-1.7103E-002,-2.2901E-003,
     &                        6.3735E-004,-1.2284E-002,-3.8308E-003,
     &                       -1.0159E-002,-1.7879E-003,-1.2510E-002,
     &                       -1.0009E-002/
C     1500km equinox
      DATA (DNH(3,1,J),J=1,49)/-1.6315E+000, 1.1175E-007, 4.7753E-001,
     &                       -1.0014E-007,-1.0097E-001,-1.0216E-007,
     &                       -5.3935E-002,-3.3570E-001, 2.0709E-007,
     &                        4.4824E-002, 1.8159E-008, 2.2117E-002,
     &                       -8.4695E-008,-1.6189E-001, 9.9261E-008,
     &                        5.7796E-002, 4.8814E-008,-7.5845E-003,
     &                       -5.1309E-008,-1.2423E-001, 1.3321E-007,
     &                       -1.6181E-002,-7.7736E-009, 4.6002E-003,
     &                       -1.2281E-001, 2.4201E-007, 1.4288E-002,
     &                        1.1896E-008,-5.3069E-003,-1.8104E-002,
     &                       -1.9814E-008,-8.4131E-003, 2.2753E-009,
     &                        5.1336E-003, 1.4694E-007, 4.9619E-003,
     &                        4.1959E-009,-2.2226E-002,-5.4731E-008,
     &                       -5.2711E-003, 1.3040E-002, 4.8906E-008,
     &                        2.2301E-003,-2.7938E-004,-2.2648E-008,
     &                       -1.5969E-002,-6.5911E-009, 3.6104E-003,
     &                        2.9534E-003/
C     1500km June solstice
      DATA (DNH(3,2,J),J=1,49)/-1.5647E+000, 4.2744E-001, 4.7853E-001,
     &                       -2.7160E-001,-3.7019E-002, 2.1446E-002,
     &                       -1.3457E-001,-1.1104E-001, 8.7769E-002,
     &                        4.0593E-003,-2.7075E-002, 8.4188E-003,
     &                       -1.2262E-002,-6.4727E-002, 7.1687E-002,
     &                       -3.8468E-002,-7.9663E-003, 1.2872E-002,
     &                        2.8088E-003,-1.2956E-002,-3.9502E-002,
     &                       -7.9875E-005, 6.1428E-003,-1.2074E-003,
     &                        3.8344E-003, 3.3477E-003,-3.3677E-003,
     &                       -3.0964E-003, 9.6501E-004, 8.2409E-003,
     &                       -2.8989E-002, 4.3657E-003, 9.4355E-005,
     &                        1.3130E-002,-1.8814E-002, 2.7093E-003,
     &                       -1.2167E-003,-5.7969E-003,-6.4927E-003,
     &                        4.1950E-004,-9.1788E-003,-7.2444E-003,
     &                       -1.4730E-003,-1.0845E-002,-2.5005E-003,
     &                       -8.2969E-003,-1.9660E-003,-6.3399E-003,
     &                       -5.8925E-003/
C     2250km equinox
      DATA (DNH(4,1,J),J=1,49)/-1.7525E+000,-1.0714E-006, 8.6183E-001,
     &                        1.1184E-006,-2.7829E-001, 2.7162E-007,
     &                       -1.0538E-001,-1.3943E-001,-1.1095E-007,
     &                        4.5835E-002,-1.8177E-007, 8.3712E-003,
     &                        1.0937E-007,-4.2987E-001,-3.9255E-007,
     &                        6.6012E-002, 5.4483E-008, 9.0376E-003,
     &                        7.3468E-008, 5.0653E-003,-6.8615E-009,
     &                       -1.3051E-002,-4.6976E-009, 3.9132E-003,
     &                       -3.8456E-002, 1.6913E-008,-8.3270E-004,
     &                       -3.9315E-008, 2.9990E-003,-5.9986E-003,
     &                        6.4241E-009,-3.6095E-004,-4.3472E-009,
     &                        9.3971E-002, 6.1373E-008,-7.0760E-003,
     &                       -2.7203E-009, 4.5148E-002,-1.1452E-008,
     &                       -1.6323E-004, 2.2101E-002, 5.6630E-009,
     &                       -9.0528E-004, 6.7390E-003, 1.1191E-009,
     &                        5.7927E-003, 2.2189E-009, 8.9446E-003,
     &                       -6.3752E-004/
C     2250km June solstice
      DATA (DNH(4,2,J),J=1,49)/-1.7520E+000, 3.8232E-001, 8.1527E-001,
     &                       -2.8870E-001,-1.0730E-001, 7.0220E-002,
     &                       -1.6764E-001,-2.1758E-001,-2.0171E-002,
     &                        3.4368E-002,-2.2874E-003, 2.7205E-002,
     &                       -6.4735E-003,-3.3147E-001, 9.2558E-002,
     &                        4.9336E-002,-4.1876E-002, 5.0738E-003,
     &                        1.2235E-002,-1.2631E-002,-2.6016E-002,
     &                        1.0295E-002,-6.0847E-003, 2.2948E-003,
     &                       -1.8060E-001,-1.7793E-002, 5.4176E-003,
     &                        2.2462E-004, 1.8786E-003, 1.1605E-001,
     &                        1.7233E-003,-1.5451E-003, 1.3382E-003,
     &                       -7.3353E-002,-3.7852E-004, 1.3236E-003,
     &                       -9.6993E-004, 1.8479E-002,-5.6199E-003,
     &                        8.0329E-004,-1.7815E-002, 3.5429E-003,
     &                       -1.5325E-003,-4.6303E-003,-3.9437E-003,
     &                       -1.7196E-002,-2.0703E-003,-3.6631E-004,
     &                       -1.6702E-002/
C////////////////////////////////////////////////////////////////////////////////////
C/////////////////////coefficients low solar activity//////////////////////////////// 
C//////////////////////////////////O+////////////////////////////////////////////////
C     400km equinox
      DATA (DOL(1,1,J),J=1,49)/-3.4295E-003, 4.8322E-010, 7.9335E-004,
     &                        1.8237E-009, 1.0320E-003,-7.3733E-010,
     &                       -5.8045E-004,-2.1541E-003,-6.5013E-010,
     &                       -3.1081E-004,-9.8478E-012, 8.0628E-005,
     &                        2.1821E-010,-1.9044E-003,-8.3791E-010,
     &                       -1.5618E-004,-4.6595E-011, 1.5868E-004,
     &                       -1.6618E-010, 7.2724E-005,-3.1159E-010,
     &                        1.5783E-004,-6.1281E-010,-3.2544E-005,
     &                       -5.8917E-004,-6.1467E-010,-1.0295E-004,
     &                        5.7992E-011, 5.7641E-005, 6.7711E-005,
     &                        6.1432E-010, 8.6370E-005,-2.3670E-010,
     &                        2.2941E-005,-4.7808E-011, 4.7716E-005,
     &                        2.1198E-011, 2.8298E-006, 3.6265E-010,
     &                       -4.4578E-006, 1.7541E-004, 2.1511E-010,
     &                        2.5195E-005,-2.7325E-005,-2.3875E-011,
     &                       -3.1418E-006, 4.6152E-011,-2.9151E-005,
     &                        3.7913E-005/
C     400km June solstice
      DATA (DOL(1,2,J),J=1,49)/-7.5061E-003, 4.2214E-003, 3.2811E-003,
     &                       -4.3773E-003, 4.1127E-005, 2.7276E-003,
     &                       -4.5069E-004,-6.9573E-003, 3.2289E-003,
     &                        4.5324E-004,-5.4022E-004,-2.8733E-004,
     &                        1.7425E-004,-4.4235E-003,-1.1619E-005,
     &                        1.3238E-003,-5.1465E-004,-3.3052E-004,
     &                        3.8804E-004, 6.8531E-004,-1.9665E-004,
     &                        1.9622E-004, 2.8863E-004,-6.5642E-005,
     &                       -2.8855E-003, 5.0512E-004, 7.0149E-004,
     &                       -1.5869E-004,-1.5202E-004, 1.5939E-003,
     &                       -5.6796E-004,-1.0670E-004, 1.2569E-007,
     &                       -8.3699E-004,-8.4185E-005, 2.4768E-005,
     &                        4.6445E-005, 1.9761E-004, 6.0184E-005,
     &                       -1.0454E-005, 3.2389E-004,-3.7438E-005,
     &                       -3.4186E-005,-3.5859E-004,-6.1472E-005,
     &                       -4.7218E-004, 7.6277E-006, 1.4752E-004,
     &                       -4.5090E-004/
C     650km equinox
      DATA (DOL(2,1,J),J=1,49)/-2.6245E-001, 8.4041E-007, 2.2991E-001,
     &                       -2.1060E-006,-5.9700E-002, 5.5301E-006,
     &                        2.6165E-002,-3.4817E-001, 2.5875E-007,
     &                        1.7171E-002,-7.7857E-007,-6.4432E-003,
     &                        6.9845E-007,-1.3705E-001, 1.0700E-006,
     &                        7.5614E-003,-5.6468E-007, 8.2754E-004,
     &                        1.9031E-007,-2.5396E-002,-7.2603E-007,
     &                       -1.7557E-002, 2.1324E-007,-1.0498E-003,
     &                       -6.4057E-002, 4.6894E-007,-9.1390E-003,
     &                        9.6493E-009, 1.7969E-004, 7.0926E-002,
     &                       -3.6416E-007,-6.9695E-003, 8.9469E-008,
     &                        2.4310E-002,-1.7204E-007,-5.4500E-003,
     &                        9.1857E-008, 2.1846E-002, 8.1249E-008,
     &                       -7.2438E-004, 2.1225E-002,-1.1084E-008,
     &                       -4.9135E-004,-1.5282E-003, 5.4962E-008,
     &                       -2.0812E-003, 3.7817E-008,-4.2919E-004,
     &                        1.7766E-004/
C     650km June solstice
      DATA (DOL(2,2,J),J=1,49)/-3.1262E-001, 2.1640E-001, 2.9808E-001,
     &                       -2.9615E-001, 6.4926E-002, 3.8438E-003,
     &                       -9.0777E-002,-3.8422E-001, 1.3193E-001,
     &                        3.1168E-002,-3.6264E-002, 2.3774E-002,
     &                       -9.3167E-003,-1.5581E-001, 1.5604E-002,
     &                        2.3125E-002, 1.3240E-002,-1.2961E-003,
     &                       -1.1848E-002,-3.2185E-002,-5.1446E-003,
     &                        1.0193E-002,-7.9351E-003, 3.2452E-003,
     &                       -1.3403E-001,-1.0768E-003, 1.2773E-002,
     &                        2.9588E-003,-9.6663E-004, 5.6863E-002,
     &                       -2.1862E-002, 1.0585E-002,-3.1121E-003,
     &                       -3.3781E-002,-1.2186E-002, 4.7556E-003,
     &                       -5.7030E-004, 1.2328E-002,-6.4108E-003,
     &                        1.7929E-003,-2.1691E-003,-8.2313E-003,
     &                        1.9333E-003,-1.1646E-002,-1.6945E-003,
     &                       -2.7066E-002, 1.3570E-004,-8.5273E-003,
     &                       -1.5711E-002/
C     1000km equinox
      DATA (DOL(3,1,J),J=1,49)/-8.9352E-001, 2.4097E-005, 3.7286E-001,
     &                       -1.0359E-005, 1.0068E-002,-1.7035E-005,
     &                       -2.0139E-002,-1.0039E+000, 8.8331E-006,
     &                       -1.3731E-001, 4.7035E-006,-3.6637E-002,
     &                       -2.6654E-006,-2.0386E-001, 1.5439E-005,
     &                       -3.6661E-002, 1.0260E-005,-4.5066E-002,
     &                       -2.8682E-006,-1.7254E-001,-4.9526E-006,
     &                       -6.7622E-002,-1.0877E-006,-1.2351E-002,
     &                       -1.0953E-001, 7.0854E-006,-1.4296E-002,
     &                        6.7187E-006,-2.0577E-002, 6.4692E-002,
     &                       -3.0932E-006,-1.1908E-002,-1.6660E-006,
     &                        6.6329E-002,-7.0914E-007,-1.5392E-002,
     &                        2.1883E-006, 4.5838E-003,-8.4412E-007,
     &                        5.4739E-004, 2.8565E-002,-9.7635E-007,
     &                        5.0974E-003,-5.1648E-004,-2.9444E-008,
     &                       -1.0738E-003,-2.7121E-007, 3.4379E-003,
     &                       -2.1828E-003/
C     1000km June solstice
      DATA (DOL(3,2,J),J=1,49)/-6.9317E-001, 3.3146E-001, 5.9247E-001,
     &                       -3.8841E-001, 1.5031E-001, 3.4648E-002,
     &                       -2.0436E-001,-7.6788E-001, 1.1303E-001,
     &                       -3.0861E-002, 3.1730E-002, 5.8057E-002,
     &                       -3.2321E-002,-2.3509E-001, 1.2907E-001,
     &                       -2.5901E-002, 9.7355E-003, 3.0717E-002,
     &                       -2.7170E-002,-1.0447E-001,-7.0094E-002,
     &                       -2.6774E-002, 2.0725E-002, 2.4860E-004,
     &                       -2.9119E-001, 9.3440E-002,-8.0132E-003,
     &                       -1.9018E-003, 4.4841E-003, 5.9359E-002,
     &                       -1.9317E-002,-1.8184E-002, 5.0489E-003,
     &                       -1.0292E-001, 1.2137E-002, 8.1073E-004,
     &                        7.1284E-004, 8.3372E-003, 1.3340E-002,
     &                       -6.7512E-003, 1.7102E-002,-2.4549E-003,
     &                       -1.7239E-003, 1.9465E-002,-5.4019E-003,
     &                        4.8407E-002,-4.4151E-003,-1.9067E-002,
     &                        1.5001E-002/
C////////////////////////////////////////////////////////////////////////////////////
C//////////////////////////////////H+////////////////////////////////////////////////
C     400km equinox
      DATA (DHL(1,1,J),J=1,49)/-2.3735E+000,-4.1106E-007,-1.5043E-001,
     &                        7.1050E-008,-4.4023E-002, 5.1900E-008,
     &                        7.5167E-002, 2.6079E-001,-5.4677E-008,
     &                        4.0421E-002, 8.4393E-009, 4.2554E-003,
     &                        1.7166E-008, 1.9018E-001,-6.0764E-008,
     &                        3.8687E-002, 6.3717E-008, 6.4895E-003,
     &                       -4.0891E-008, 3.8167E-002, 1.8652E-008,
     &                       -1.0038E-002, 1.9645E-008, 2.1804E-003,
     &                       -1.4444E-002, 6.5822E-009, 7.0666E-003,
     &                       -8.4659E-009,-5.5334E-003, 1.5556E-002,
     &                        1.2414E-008,-4.3661E-003, 4.4984E-009,
     &                        9.3627E-003,-2.4554E-009,-3.6096E-003,
     &                        7.8989E-009,-1.2352E-002, 1.0504E-008,
     &                       -4.5357E-006, 7.8762E-004,-5.2915E-009,
     &                       -1.9697E-003, 4.2966E-004, 5.9770E-009,
     &                        3.7248E-003, 6.9259E-009, 3.0955E-003,
     &                        2.7742E-003/
C     400km June solstice
      DATA (DHL(1,2,J),J=1,49)/-2.2303E+000,-2.7930E-001,-1.6289E-001,
     &                        8.4439E-002, 4.3059E-002,-1.0900E-002,
     &                        5.9060E-003, 3.8539E-001,-9.7745E-002,
     &                        1.8579E-002, 1.4256E-002, 6.8489E-004,
     &                       -1.1027E-003, 2.3516E-001, 3.8591E-002,
     &                       -2.9264E-002,-1.9275E-003, 2.4632E-002,
     &                       -3.0091E-004,-2.4136E-002, 2.1471E-002,
     &                       -1.4274E-002,-2.9073E-003, 4.0650E-003,
     &                        2.6483E-002, 3.9806E-003,-1.3560E-002,
     &                        3.6952E-004, 5.3108E-004,-3.9132E-002,
     &                        2.3689E-002,-6.6253E-004, 1.3556E-003,
     &                        2.3462E-002,-3.2744E-003, 1.4565E-003,
     &                        2.4812E-004,-3.2051E-004, 2.6108E-003,
     &                        7.5391E-004,-1.9602E-002, 5.0088E-003,
     &                        8.0312E-004, 1.3604E-002, 3.2853E-003,
     &                        8.1745E-003, 1.9033E-003, 1.6966E-002,
     &                        1.2348E-002/
C     650km equinox
      DATA (DHL(2,1,J),J=1,49)/-7.5599E-001,-2.3535E-007,-4.1761E-001,
     &                       -2.8901E-008, 4.8799E-002,-1.4005E-007,
     &                       -7.5257E-002, 5.0850E-001,-5.5769E-008,
     &                        7.4268E-002,-4.0953E-008, 2.0880E-002,
     &                       -3.9792E-008, 1.9637E-001, 3.5212E-008,
     &                        3.5924E-002,-3.2961E-008,-1.1911E-002,
     &                        5.4324E-009, 2.9918E-002, 1.0830E-008,
     &                        3.4518E-002,-5.4744E-009, 1.0144E-002,
     &                       -1.5615E-002, 9.3932E-009, 7.9662E-003,
     &                       -4.4936E-009,-1.4126E-003,-3.1648E-002,
     &                        2.2804E-008, 6.2742E-003,-9.5574E-009,
     &                       -1.1306E-002, 1.6269E-009, 2.2078E-004,
     &                        2.8647E-009,-1.7932E-002,-5.0077E-009,
     &                       -2.3559E-003,-9.2953E-003,-5.1441E-009,
     &                       -2.2185E-003, 2.9117E-005,-4.8547E-009,
     &                        4.4236E-003,-2.8102E-009, 9.3261E-004,
     &                        1.9538E-003/
C     650km June solstice
      DATA (DHL(2,2,J),J=1,49)/-7.6906E-001,-4.2549E-001,-6.2598E-001,
     &                        2.2279E-001, 1.4493E-001, 1.0653E-001,
     &                        2.7012E-001, 3.8710E-001,-6.1640E-002,
     &                        4.6697E-002,-5.7444E-002, 2.0933E-002,
     &                        8.8119E-003, 1.4986E-001, 2.1907E-002,
     &                       -1.8032E-002,-4.6878E-002,-1.6714E-002,
     &                        4.2935E-002,-2.4183E-002, 2.0290E-002,
     &                       -1.0109E-002, 5.2285E-003,-6.9878E-003,
     &                        4.2781E-002, 5.6690E-003,-1.1873E-002,
     &                       -4.5405E-003,-3.6632E-003,-2.2072E-002,
     &                        1.2209E-003,-5.8301E-003, 3.1915E-006,
     &                        1.0209E-002, 1.9771E-003, 1.3042E-003,
     &                        1.4817E-003, 2.0904E-003,-7.5031E-004,
     &                        2.2646E-003, 1.1547E-002, 6.7321E-003,
     &                        9.5170E-004, 1.7845E-002, 6.1338E-003,
     &                        1.2967E-002, 2.3311E-003, 2.9597E-002,
     &                       -2.2654E-003/
C     1000km equinox
      DATA (DHL(3,1,J),J=1,49)/-3.1869E-001, 1.5090E-007,-4.0560E-001,
     &                        2.8552E-007,-4.2289E-003, 3.6878E-008,
     &                        1.9957E-001, 2.7526E-001, 4.0798E-007,
     &                        1.1343E-001,-1.0028E-007, 5.1526E-002,
     &                       -6.0643E-008, 9.2397E-002,-2.1866E-007,
     &                        8.6528E-002, 2.3460E-008, 3.6848E-002,
     &                        6.2456E-008, 1.5483E-002,-1.7108E-007,
     &                        3.6333E-002,-1.0845E-008, 1.0259E-002,
     &                        4.3320E-003,-2.6987E-007,-1.0254E-002,
     &                       -8.8644E-008,-2.1170E-003,-2.1956E-002,
     &                       -1.3044E-007,-6.8424E-003,-3.8852E-008,
     &                       -1.6083E-002,-2.8545E-008, 4.7800E-003,
     &                        4.3092E-008,-9.4451E-004,-4.6383E-008,
     &                        1.7086E-003, 7.9651E-003, 2.1903E-007,
     &                        3.3880E-004, 4.5247E-004, 1.0538E-007,
     &                        3.7229E-003,-8.6806E-008,-5.2132E-004,
     &                       -5.7966E-004/
C     1000km June solstice
      DATA (DHL(3,2,J),J=1,49)/-4.6737E-001,-1.2403E-001,-6.4613E-001,
     &                        1.8831E-001, 1.2419E-001,-5.9911E-002,
     &                        1.9218E-001, 2.9484E-001, 4.0311E-002,
     &                        9.8207E-002,-2.9219E-002,-1.1908E-002,
     &                        3.8705E-002,-1.8943E-003,-4.2874E-002,
     &                       -4.6060E-002,-3.4199E-002,-2.2322E-002,
     &                        2.5785E-002, 2.5357E-002, 2.7261E-002,
     &                        9.8865E-003, 1.6691E-003, 6.2808E-003,
     &                       -9.1296E-003,-1.3707E-002,-8.7014E-003,
     &                       -3.4345E-003,-4.6994E-003,-2.5409E-002,
     &                       -6.7929E-003, 1.2701E-003, 3.6829E-003,
     &                       -2.9346E-003,-2.1371E-003,-7.4109E-003,
     &                        1.4351E-003,-7.4944E-003,-2.3530E-003,
     &                       -2.3960E-003,-8.2307E-003,-4.0801E-004,
     &                        1.9430E-003, 1.8990E-003, 6.7012E-004,
     &                       -2.2565E-003,-4.5399E-004,-1.7694E-003,
     &                       -6.3397E-004/
C/////////////////////////////////////////////////////////////////////////////////////
C//////////////////////////////////He+////////////////////////////////////////////////
C     400km equinox
      DATA (DHEL(1,1,J),J=1,49)/-2.8533E+000,-2.1986E-007, 7.2644E-002,
     &                        2.2489E-007,-1.8677E-001,-3.9095E-007,
     &                        1.0850E-002, 2.2428E-001,-1.5216E-008,
     &                       -1.0736E-001,-7.9766E-008,-4.1078E-002,
     &                        4.7277E-008, 6.3340E-001,-1.5529E-008,
     &                        8.1139E-003, 1.4044E-008, 5.8076E-003,
     &                       -4.6109E-008,-8.8933E-002,-6.0933E-009,
     &                        8.9345E-003, 7.8024E-009, 1.0524E-002,
     &                       -7.0800E-002, 3.0069E-008,-3.8559E-002,
     &                        1.9810E-009,-2.7640E-002,-3.8927E-002,
     &                        2.5396E-008,-5.9943E-003, 1.4326E-008,
     &                       -2.0216E-002,-1.0963E-008,-1.0778E-002,
     &                       -3.1408E-009,-9.0961E-003,-1.6521E-008,
     &                        5.3029E-003,-3.3221E-002, 2.9052E-011,
     &                       -1.9158E-003, 6.3675E-003,-8.1957E-009,
     &                        7.4360E-003,-7.6241E-009, 6.5228E-003,
     &                        3.2586E-003/
C     400km June solstice
      DATA (DHEL(1,2,J),J=1,49)/-3.0612E+000,-1.0562E+000, 8.8532E-002,
     &                        8.5373E-002, 8.1474E-002, 2.8747E-002,
     &                        1.3534E-001, 4.3580E-001, 4.4317E-002,
     &                       -1.8054E-002,-1.3027E-002, 4.1295E-002,
     &                       -4.4592E-003, 1.0223E-001, 1.3085E-001,
     &                       -7.7696E-002,-7.3836E-002, 1.9780E-002,
     &                        1.5735E-002,-5.9522E-002, 4.8434E-002,
     &                        1.5970E-002, 3.5974E-003,-3.8010E-003,
     &                       -1.0031E-001, 1.7651E-002, 1.2491E-002,
     &                        9.0502E-003,-8.7768E-003,-9.6294E-003,
     &                        1.8994E-002, 4.6892E-003, 2.5748E-004,
     &                       -9.6002E-003,-6.2450E-003, 9.0447E-003,
     &                       -1.8477E-003, 1.8898E-002,-6.9182E-003,
     &                        2.0626E-003, 4.7967E-003,-5.7386E-003,
     &                       -6.0267E-004, 2.5325E-002, 1.1937E-003,
     &                        2.1619E-002,-1.3459E-003, 3.7915E-002,
     &                        1.7314E-002/
C     650km equinox
      DATA (DHEL(2,1,J),J=1,49)/-1.6103E+000,-1.3811E-007, 1.1838E-001,
     &                        2.4209E-007,-1.2965E-001,-6.5882E-008,
     &                        1.3191E-001, 4.6161E-001,-2.5600E-007,
     &                        1.2864E-001, 7.8717E-008, 2.9376E-002,
     &                       -2.0378E-009, 3.7085E-001, 2.5941E-007,
     &                        2.3336E-002,-2.5896E-007, 7.0754E-002,
     &                        1.5037E-007,-1.6104E-001, 2.5861E-008,
     &                        2.1873E-002,-3.9311E-010, 1.7883E-002,
     &                       -5.9487E-002, 7.5016E-008,-3.8658E-003,
     &                       -1.5184E-008,-2.2297E-002,-3.2125E-002,
     &                        1.3503E-008,-1.0069E-003, 2.3685E-008,
     &                       -2.3076E-002, 1.6395E-008,-4.7465E-003,
     &                        2.1465E-009,-2.1087E-002,-1.0412E-008,
     &                       -4.4450E-003, 6.9945E-003, 1.6080E-008,
     &                        3.1289E-003, 9.4760E-003,-8.4313E-009,
     &                       -2.1643E-003,-8.3007E-009, 2.3316E-003,
     &                       -8.8253E-003/
C     650km June solstice
      DATA (DHEL(2,2,J),J=1,49)/-2.2374E+000,-7.4507E-001,-1.4689E-001,
     &                       -2.5946E-002, 4.7283E-002, 1.4143E-001,
     &                        3.2299E-001, 1.9366E-001, 8.7688E-002,
     &                        1.2526E-002,-2.2810E-002,-2.2711E-003,
     &                       -1.1824E-002,-4.8523E-002,-8.2298E-004,
     &                       -2.2395E-001,-3.4992E-002, 1.8125E-002,
     &                        1.4230E-002,-2.7661E-002, 2.3424E-002,
     &                        4.7094E-003,-1.0739E-002,-3.4746E-003,
     &                       -2.2153E-001,-1.8756E-002,-7.1623E-003,
     &                        4.6047E-003,-4.5071E-003, 2.2894E-002,
     &                       -2.4285E-002,-5.5847E-003,-9.8852E-004,
     &                        6.7171E-002,-5.0134E-004,-2.9361E-003,
     &                       -4.2558E-003, 1.5103E-002,-9.7860E-003,
     &                       -1.8098E-003, 1.8795E-002, 4.5063E-003,
     &                        2.4747E-003, 1.1677E-002,-4.2526E-003,
     &                        1.4606E-002, 2.2638E-003, 2.1285E-002,
     &                        1.3963E-002/
C     1000km equinox
      DATA (DHEL(3,1,J),J=1,49)/-1.3192E+000, 1.8143E-006,-2.1284E-002,
     &                       -5.7860E-006,-1.3203E-001, 1.0367E-006,
     &                        8.7882E-002, 2.5246E-001, 3.0540E-007,
     &                        1.2671E-001, 7.0752E-007, 5.1175E-002,
     &                       -1.0432E-006, 1.5597E-001,-1.9061E-007,
     &                        9.7418E-002, 2.2626E-006, 6.3152E-002,
     &                       -2.4478E-006,-1.6992E-001,-6.0624E-007,
     &                       -1.6865E-002, 2.6681E-007, 2.4771E-003,
     &                       -3.9535E-002, 5.9015E-007,-5.4337E-002,
     &                       -1.4229E-007,-2.5864E-002, 4.5711E-002,
     &                        1.4937E-007,-1.9960E-002,-1.0504E-007,
     &                        1.2233E-002,-6.6122E-008, 8.5225E-004,
     &                        8.6051E-009, 2.7375E-002, 6.5450E-008,
     &                       -6.1311E-003, 1.2666E-002, 1.9864E-007,
     &                        2.8454E-003, 1.9020E-002,-5.1153E-008,
     &                        4.4576E-003,-2.0117E-008, 8.6293E-003,
     &                       -5.5447E-003/
C     1000km June solstice
      DATA (DHEL(3,2,J),J=1,49)/-1.9424E+000,-5.8403E-001, 3.4084E-001,
     &                       -1.9084E-001,-5.4692E-002, 2.0943E-001,
     &                        6.7653E-003, 1.7048E-001, 1.9560E-001,
     &                        7.2251E-002,-1.3912E-003, 4.9874E-002,
     &                       -5.1299E-003,-2.2192E-001,-1.3285E-001,
     &                       -1.7111E-001,-2.8852E-002, 1.7141E-002,
     &                       -4.1047E-002, 1.3092E-001, 2.9271E-002,
     &                        2.1492E-002, 3.1053E-002, 1.7248E-002,
     &                       -1.7665E-001,-1.9095E-003,-1.0090E-002,
     &                       -2.8027E-002,-1.5999E-002, 6.9373E-002,
     &                       -4.0136E-002, 1.3345E-002, 9.5536E-003,
     &                       -1.0609E-002,-2.2548E-002,-1.5098E-002,
     &                       -1.1711E-002, 2.6458E-002,-1.5922E-002,
     &                        3.4746E-003,-1.8009E-002, 4.8529E-004,
     &                       -3.8353E-003, 1.9446E-003,-7.4106E-003,
     &                       -3.9309E-003,-1.3632E-003,-4.7108E-003,
     &                        7.2728E-003/
C////////////////////////////////////////////////////////////////////////////////////
C//////////////////////////////////N+////////////////////////////////////////////////
C     400km equinox
      DATA (DNL(1,1,J),J=1,49)/-1.7368E+000,-3.0027E-008, 2.2184E-001,
     &                       -6.7089E-007,-1.0635E-001, 6.8139E-007,
     &                        5.8091E-002,-6.2121E-002, 3.1864E-007,
     &                        4.8208E-002,-2.7440E-007,-2.4000E-003,
     &                        6.6031E-008,-1.0868E-001, 7.2869E-008,
     &                       -1.0042E-002,-4.6008E-008, 1.6205E-003,
     &                        4.6331E-009,-3.6398E-002, 1.7139E-007,
     &                        6.2127E-004,-4.0138E-008, 1.2439E-003,
     &                       -1.8217E-002, 4.6073E-008, 3.7524E-003,
     &                       -2.0566E-008,-3.0241E-003, 2.7188E-002,
     &                        6.3375E-008,-3.7723E-003,-3.9013E-009,
     &                        6.9304E-003, 5.7030E-008,-1.2236E-004,
     &                       -6.4988E-009, 1.2409E-002, 6.2839E-009,
     &                       -1.4527E-003, 1.4370E-003, 1.8605E-008,
     &                        1.8677E-004, 3.9142E-003,-4.8721E-009,
     &                       -4.1931E-003, 7.9621E-009, 1.9265E-004,
     &                       -1.3332E-003/
C     400km June solstice
      DATA (DNL(1,2,J),J=1,49)/-1.7418E+000, 7.0525E-002, 2.9785E-001,
     &                        1.1675E-002,-9.2159E-002,-1.2950E-001,
     &                       -5.8461E-002,-1.3966E-001, 1.5042E-002,
     &                        4.6388E-002,-9.7774E-003, 3.5422E-003,
     &                       -8.6287E-003,-8.7710E-002, 3.8852E-002,
     &                       -9.0949E-003,-1.8152E-002, 3.6013E-004,
     &                        7.2089E-003, 2.5707E-002, 8.4327E-003,
     &                       -6.5605E-003, 4.8306E-004, 5.0042E-004,
     &                       -3.1783E-002, 5.7723E-003, 8.8230E-003,
     &                       -2.6260E-003,-1.1217E-003, 5.7169E-002,
     &                       -5.2793E-003,-3.1765E-003, 5.1564E-004,
     &                        1.3519E-002,-3.9896E-003, 1.7336E-003,
     &                       -2.8791E-004, 1.3395E-002,-5.3013E-003,
     &                       -5.6986E-004, 3.0630E-003,-3.8017E-003,
     &                        9.3966E-004,-5.1651E-004,-3.0807E-003,
     &                       -4.4359E-003,-2.0503E-003, 8.0675E-004,
     &                        1.1912E-004/
C     650km equinox
      DATA (DNL(2,1,J),J=1,49)/-1.5547E+000, 6.7091E-007, 4.0622E-001,
     &                       -5.5830E-008,-1.3901E-001,-6.7244E-007,
     &                        1.4880E-001,-8.2658E-002, 2.6659E-007,
     &                        1.0021E-002, 1.3823E-007,-7.4716E-003,
     &                       -8.9220E-008,-1.3959E-001, 2.0864E-007,
     &                       -1.0484E-002, 4.4276E-008, 1.9706E-002,
     &                       -1.1884E-007,-1.2037E-002,-6.2490E-008,
     &                       -2.9276E-002, 4.0271E-008,-1.2310E-002,
     &                       -2.6679E-003, 3.5891E-008,-6.5922E-003,
     &                       -1.6467E-008,-3.9885E-004, 2.0962E-002,
     &                       -5.5785E-008,-7.4266E-003, 2.5071E-008,
     &                        2.2400E-002,-2.8034E-008,-1.2509E-003,
     &                       -1.9655E-008, 1.0287E-002, 2.6129E-008,
     &                       -1.3332E-003, 1.0731E-002, 2.4252E-009,
     &                        1.5417E-003, 3.1698E-003, 1.8012E-008,
     &                       -3.1057E-003, 1.5340E-008, 3.0302E-003,
     &                       -2.9436E-003/
C     650km June solstice
      DATA (DNL(2,2,J),J=1,49)/-1.5723E+000, 4.0251E-001, 4.0235E-001,
     &                       -1.6855E-001,-1.4445E-001,-2.9990E-002,
     &                       -2.0387E-001,-1.7979E-001, 1.9726E-001,
     &                       -6.8767E-002,-2.7798E-002, 4.5989E-003,
     &                        1.3561E-002,-1.2212E-001, 2.4244E-002,
     &                       -4.2670E-002, 6.2073E-002,-5.2021E-003,
     &                       -2.3860E-002,-6.4837E-002, 3.1524E-002,
     &                       -1.8784E-002, 1.2402E-002, 1.1119E-005,
     &                       -1.9303E-002,-2.2166E-002, 1.1413E-002,
     &                       -2.2769E-003, 1.0334E-003,-3.6042E-003,
     &                       -3.4116E-003,-4.2969E-003, 3.7668E-003,
     &                       -1.6617E-002,-2.0903E-002, 1.1176E-002,
     &                       -3.4928E-003,-1.9803E-002,-3.7789E-003,
     &                        2.9788E-004,-3.1880E-002,-1.0222E-002,
     &                        3.7033E-003,-3.0094E-002,-2.2242E-004,
     &                       -2.8816E-002,-4.4691E-003,-2.2928E-002,
     &                       -1.6669E-002/
C     1000km equinox
      DATA (DNL(3,1,J),J=1,49)/-1.7382E+000,-6.0635E-007, 4.4740E-001,
     &                        4.7518E-007, 9.1015E-002, 8.6631E-007,
     &                       -4.1582E-003,-3.1542E-001,-8.6896E-007,
     &                       -8.8282E-002, 1.7889E-007,-3.8424E-002,
     &                        5.6129E-008,-1.0561E-001, 1.0863E-007,
     &                       -3.7752E-002,-1.1781E-007,-4.1151E-002,
     &                       -1.4323E-008,-3.8984E-002,-4.7271E-007,
     &                       -3.7386E-002, 8.5416E-008,-1.2316E-002,
     &                        1.0742E-001, 1.5944E-007,-2.8192E-003,
     &                       -1.2504E-008,-1.6905E-002, 2.8270E-002,
     &                       -1.9685E-007, 8.9762E-003, 2.8706E-008,
     &                        6.7218E-002, 5.7165E-008,-8.2717E-003,
     &                        3.0388E-008, 1.9133E-002, 1.4687E-008,
     &                        5.5305E-004, 7.9431E-003, 4.8793E-008,
     &                        8.6786E-003, 4.9010E-003, 3.2400E-008,
     &                       -3.8863E-003,-2.5953E-008,-2.3986E-003,
     &                       -6.9512E-003/
C     1000km June solstice
      DATA (DNL(3,2,J),J=1,49)/-1.4667E+000, 1.6300E-001, 6.4765E-001,
     &                       -1.6832E-001,-1.9603E-001, 1.3735E-001,
     &                       -2.8157E-001,-9.1417E-002,-2.4826E-002,
     &                       -2.7273E-002, 3.1754E-002, 2.0120E-002,
     &                       -6.8389E-003,-9.6535E-002, 1.9137E-002,
     &                       -3.2374E-002, 7.1156E-002, 1.3993E-002,
     &                       -3.8484E-002, 3.1979E-002,-4.2560E-002,
     &                       -3.8190E-003, 1.1174E-002,-2.3599E-003,
     &                        3.4835E-003,-6.4330E-003,-1.4724E-003,
     &                       -2.7194E-003, 1.8374E-003, 7.9017E-003,
     &                       -1.3993E-002, 1.4415E-004, 1.8409E-003,
     &                        1.5734E-003,-1.3150E-002,-4.3392E-003,
     &                       -8.7189E-004,-6.9494E-003,-7.0281E-003,
     &                       -1.5472E-003, 7.6997E-004,-9.7752E-003,
     &                       -1.0346E-003,-4.2392E-003,-7.3591E-003,
     &                        1.0610E-002,-5.8767E-003, 2.4544E-004,
     &                        6.0162E-003/
C////////////////////////////////////////////////////////////////////////////////////
C///////////////////////////////solar minimum////////////////////////////////////////
      CALL IONLOW(CRD,INVDIP,FL,DIMO,B0,DIPL,MLT,ALT,DDD,DOL,0,NOL)
      CALL IONLOW(CRD,INVDIP,FL,DIMO,B0,DIPL,MLT,ALT,DDD,DHL,1,NHL)
      CALL IONLOW(CRD,INVDIP,FL,DIMO,B0,DIPL,MLT,ALT,DDD,DHEL,2,NHEL)
      CALL IONLOW(CRD,INVDIP,FL,DIMO,B0,DIPL,MLT,ALT,DDD,DNL,3,NNL)
C     normalization
      NTOT=NOL+NHL+NHEL+NNL
      NOL=NOL/NTOT
      NHL=NHL/NTOT
      NHEL=NHEL/NTOT
      NNL=NNL/NTOT
C///////////////////////////////solar maximum////////////////////////////////////////
      CALL IONHIGH(CRD,INVDIP,FL,DIMO,B0,DIPL,MLT,ALT,DDD,DOH,0,NOH)
      CALL IONHIGH(CRD,INVDIP,FL,DIMO,B0,DIPL,MLT,ALT,DDD,DHH,1,NHH)
      CALL IONHIGH(CRD,INVDIP,FL,DIMO,B0,DIPL,MLT,ALT,DDD,DHEH,2,NHEH)
      CALL IONHIGH(CRD,INVDIP,FL,DIMO,B0,DIPL,MLT,ALT,DDD,DNH,3,NNH)
C     normalization
      NTOT=NOH+NHH+NHEH+NNH
      NOH=NOH/NTOT
      NHH=NHH/NTOT
      NHEH=NHEH/NTOT
      NNH=NNH/NTOT
C     interpolation (in logarithm)
      IF (F107 .GT. 200) F107=200
	IF (F107 .LT. 85) F107=85
      NO=(ALOG10(NOH)-ALOG10(NOL))/(200.0-85.0)*(F107-85.0)+ALOG10(NOL)
      NH=(ALOG10(NHH)-ALOG10(NHL))/(200.0-85.0)*(F107-85.0)+ALOG10(NHL)
      NHE=(ALOG10(NHEH)-ALOG10(NHEL))/(200.0-85.0)*(F107-85.0)+
     &     ALOG10(NHEL)
      NN=(ALOG10(NNH)-ALOG10(NNL))/(200.0-85.0)*(F107-85.0)+ALOG10(NNL)
C     percentages
      NO=10**NO
      NH=10**NH
      NHE=10**NHE
      NN=10**NN
C     last normalization
      NTOT=NO+NH+NHE+NN
      NO=NO/NTOT
      NH=NH/NTOT
      NHE=NHE/NTOT
      NN=NN/NTOT
      RETURN
      END

      SUBROUTINE IONLOW(CRD,INVDIP,FL,DIMO,B0,
     &                   DIPL,MLT,ALT,DDD,D,ION,NION)
C IONLOW calculates relative density of O+, H+, He+ or N+  in the outer
C ionosphere for a low solar activity (F107 < 100).
C Based on spherical harmonics approximation of relative ion density
C (by AE-C, and AE-E) at altitudes centred on 400km, 650km, and 1000km.
C For intermediate altitudes an interpolation is used. 
C Recommended altitude range: 350-2000 km!!!
C For days between seasons centred at (21.3. = 79; 21.6. = 171;
C 23.9. 265; 21.12. = 354) relative ion density is linearly interpolated.
C Inputs: CRD - 0 .. INVDIP
C               1 .. FL, DIMO, B0, DIPL (used for calculation INVDIP inside)
C         INVDIP - "mix" coordinate of the dip latitude and of
C                    the invariant latitude;
C                    positive northward, in deg, range <-90.0;90.0>
C         FL, DIMO, BO - McIlwain L parameter, dipole moment in
C                        Gauss, magnetic field strength in Gauss -
C                        parameters needed for invariant latitude
C                        calculation
C         DIPL - dip latitude
C                positive northward, in deg, range <-90.0;90.0>
C         MLT - magnetic local time (central dipole)
C               in hours, range <0;24)
C         ALT - altitude above the Earth's surface;
C               in km, range <350;2000>
C         DDD - day of year; range <0;365>
C         D - coefficints of spherical harmonics for a given ion
C         ION - ion species (0...O+, 1...H+, 2...He+, 3...N+)
C Output: NION - relative density for a given ion 
      REAL INVDIP,FL,DIMO,B0,DIPL,MLT,ALT,NION
      INTEGER CRD,DDD,ION
      DIMENSION  D(3,3,49),MIRREQ(49)
      REAL INVDP,INVDPC,DTOR
      REAL RMLT,RCOLAT
      REAL C(49)
      INTEGER SEZA,SEZB,SEZAI,SEZBI,DDDA,DDDB,DDDD
      REAL N0A400,N0B400,N400A,N400B,N400
      REAL N0A650,N0B650,N650A,N650B,N650
      REAL N0A100,N0B100,N100A,N100B,N1000
	REAL ANO(3),AH(3),DNO(1),ST(2)
	COMMON/ARGEXP/ARGMAX
	DATA (MIRREQ(J),J=1,49)/
     &            1,-1, 1,-1, 1,-1, 1, 1,-1, 1,-1, 1,-1, 1,-1, 1,-1,
     &            1,-1, 1,-1, 1,-1, 1, 1,-1, 1,-1, 1, 1,-1, 1,-1, 1,
     &           -1, 1,-1, 1,-1, 1, 1,-1, 1, 1,-1, 1,-1, 1, 1/
C////////////////////////////////////////////////////////////////////////////////////
      DTOR=3.1415926536/180.0
C     coefficients for mirroring
      DO 10 I=1,49
       D(1,3,I)=D(1,2,I)*MIRREQ(I)
       D(2,3,I)=D(2,2,I)*MIRREQ(I)
10     D(3,3,I)=D(3,2,I)*MIRREQ(I)
      IF (CRD .EQ. 1) THEN
       INVDP=INVDPC(FL,DIMO,B0,DIPL,DTOR)
      ELSE IF	(CRD .EQ. 0) THEN
       INVDP=INVDIP
      ELSE
       RETURN
      END IF
      RMLT=MLT*DTOR*15.0
      RCOLAT=(90.0-INVDP)*DTOR
      CALL SPHARM_IK(C,6,6,RCOLAT,RMLT)
C     21.3. - 20.6.
      IF ((DDD .GE. 79) .AND. (DDD .LT. 171)) THEN
       SEZA=1
       SEZB=2
       DDDA=79
       DDDB=171
       DDDD=DDD
      END IF
C     21.6. - 22.9.
      IF ((DDD .GE. 171) .AND. (DDD .LT. 265)) THEN
       SEZA=2
       SEZB=4
       DDDA=171
       DDDB=265
       DDDD=DDD
      END IF
C     23.9. - 20.12.
      IF ((DDD .GE. 265) .AND. (DDD .LT. 354)) THEN
       SEZA=4
       SEZB=3
       DDDA=265
       DDDB=354
       DDDD=DDD
      END IF
C     21.12. - 20.3.
      IF ((DDD .GE. 354) .OR. (DDD .LT. 79)) THEN
       SEZA=3
       SEZB=1
       DDDA=354
       DDDB=365+79
       DDDD=DDD
        IF (DDD .GE. 354) THEN
         DDDD=DDD
        ELSE
         DDDD=DDD+365
        END IF
      END IF
       SEZAI=MOD(SEZA-1,3)+1
       SEZBI=MOD(SEZB-1,3)+1
C     400km level
        N0A400=0.0
        N0B400=0.0
        DO 30 I=1,49
         N0A400=N0A400+C(I)*D(1,SEZAI,I)
30       N0B400=N0B400+C(I)*D(1,SEZBI,I)
        N400A=N0A400
        N400B=N0B400
        N400=(N400B-N400A)/(DDDB-DDDA)*(DDDD-DDDA)+N400A
C     650km level
      N0A650=0.0
	N0B650=0.0
	DO 70 I=1,49
         N0A650=N0A650+C(I)*D(2,SEZAI,I)
70       N0B650=N0B650+C(I)*D(2,SEZBI,I)
	N650A=N0A650
	N650B=N0B650
	N650=(N650B-N650A)/(DDDB-DDDA)*(DDDD-DDDA)+N650A
C     1000km level
      N0A100=0.0
      N0B100=0.0
	DO 110 I=1,49
         N0A100=N0A100+C(I)*D(3,SEZAI,I)
110      N0B100=N0B100+C(I)*D(3,SEZBI,I)
	N100A=N0A100
	N100B=N0B100
	N1000=(N100B-N100A)/(DDDB-DDDA)*(DDDD-DDDA)+N100A
          
C      IF (ALT .LT. 650) NO=(N650-N400)/250.0*(ALT-400)+N400
C      IF (ALT .GE. 650) NO=(N1000-N650)/350.0*(ALT-650)+N650

C      NION=10**NO

C-02/07/09- n(O+) AND n(N+) must not increase above 650km
      IF (((ION .EQ. 0) .OR. (ION .EQ. 3)) .AND. (N1000 .GT. N650))
     &      N1000=N650
C-02/07/09- n(H+) must not decrease above 650km
      IF ((ION .EQ. 1) .AND. (N1000 .LT. N650)) N1000=N650

      ANO(1)=N400
	 ANO(2)=N650
	  ANO(3)=N1000
	  
	AH(1)=400.
       AH(2)=650.
        AH(3)=1000.
      
      DNO(1)=20.
      
      ST1=(ANO(2)-ANO(1))/(AH(2)-AH(1))
      I=2
      ST2=(ANO(I+1)-ANO(I))/(AH(I+1)-AH(I))
      ANO(I)=ANO(I)-(ST2-ST1)*DNO(I-1)*ALOG(2.)

      DO 220 I=1,2
220   ST(I)=(ANO(I+1)-ANO(I))/(AH(I+1)-AH(I))

      ARGMAX=88.0
      SUM=ANO(1)+ST(1)*(ALT-AH(1))                     
     
      I=1
	aa = eptr(alt  ,dno(i),ah(i+1))
	bb = eptr(ah(1),dno(i),ah(i+1))
      SUM=SUM+(ST(I+1)-ST(I))*(AA-BB)*DNO(I)
                
      NION=10**SUM       
      RETURN
      END

      SUBROUTINE IONHIGH(CRD,INVDIP,FL,DIMO,B0,
     &                   DIPL,MLT,ALT,DDD,D,ION,NION)
C IONHIGH calculates relative density of O+, H+, He+ or N+  in the outer
C ionosphere for high solar activity conditions (F107 >= 100).
C Based on spherical harmonics approximation of relative ion density
C (by IK24) at altitudes centred on 550km, 900km, 1500km, and 2250km.
C For intermediate altitudes a interpolation is used. 
C Recommended altitude range: 500-3000 km!!!
C For days between seasons centred at (21.3. = 79; 21.6. = 171;
C 23.9. 265; 21.12. = 354) relative ion density is linearly interpolated.
C Inputs: CRD - 0 .. INVDIP
C               1 .. FL, DIMO, B0, DIPL (used for calculation INVDIP inside)
C         INVDIP - "mix" coordinate of the dip latitude and of
C                    the invariant latitude;
C                    positive northward, in deg, range <-90.0;90.0>
C         FL, DIMO, BO - McIlwain L parameter, dipole moment in
C                        Gauss, magnetic field strength in Gauss -
C                        parameters needed for invariant latitude
C                        calculation
C         DIPL - dip latitude
C                positive northward, in deg, range <-90.0;90.0>
C         MLT - magnetic local time (central dipole)
C               in hours, range <0;24)
C         ALT - altitude above the Earth's surface;
C               in km, range <500;3000>
C         DDD - day of year; range <0;365>
C         D - coefficints of spherical harmonics for a given ion
C         ION - ion species (0...O+, 1...H+, 2...He+, 3...N+)
C Output: NION - relative density for a given ion 
      REAL INVDIP,FL,DIMO,B0,DIPL,MLT,ALT,NION
	INTEGER CRD,DDD,ION
      DIMENSION  D(4,3,49),MIRREQ(49)
      REAL INVDP,INVDPC,DTOR
      REAL RMLT,RCOLAT
      REAL C(49)
      INTEGER SEZA,SEZB,SEZAI,SEZBI,DDDA,DDDB,DDDD
      REAL N0A550,N0B550,N550A,N550B,N550
      REAL N0A900,N0B900,N900A,N900B,N900
      REAL N0A150,N0B150,N150A,N150B,N1500
      REAL N0A250,N0B250,N250A,N250B,N2500
	REAL ANO(4),AH(4),DNO(2),ST(3)
	COMMON/ARGEXP/ARGMAX
	DATA (MIRREQ(J),J=1,49)/
     &            1,-1, 1,-1, 1,-1, 1, 1,-1, 1,-1, 1,-1, 1,-1, 1,-1,
     &            1,-1, 1,-1, 1,-1, 1, 1,-1, 1,-1, 1, 1,-1, 1,-1, 1,
     &           -1, 1,-1, 1,-1, 1, 1,-1, 1, 1,-1, 1,-1, 1, 1/
C////////////////////////////////////////////////////////////////////////////////////
      DTOR=3.1415926536/180.0
C     coefficients for mirroring
      DO 10 I=1,49
       D(1,3,I)=D(1,2,I)*MIRREQ(I)
       D(2,3,I)=D(2,2,I)*MIRREQ(I)
       D(3,3,I)=D(3,2,I)*MIRREQ(I)
10     D(4,3,I)=D(4,2,I)*MIRREQ(I)
      IF (CRD .EQ. 1) THEN
       INVDP=INVDPC(FL,DIMO,B0,DIPL,DTOR)
      ELSE IF	(CRD .EQ. 0) THEN
       INVDP=INVDIP
      ELSE
       RETURN
      END IF
      RMLT=MLT*DTOR*15.0
      RCOLAT=(90.0-INVDP)*DTOR
      CALL SPHARM_IK(C,6,6,RCOLAT,RMLT)
C     21.3. - 20.6.
      IF ((DDD .GE. 79) .AND. (DDD .LT. 171)) THEN
       SEZA=1
       SEZB=2
       DDDA=79
       DDDB=171
       DDDD=DDD
      END IF
C     21.6. - 22.9.
      IF ((DDD .GE. 171) .AND. (DDD .LT. 265)) THEN
       SEZA=2
       SEZB=4
       DDDA=171
       DDDB=265
       DDDD=DDD
      END IF
C     23.9. - 20.12.
      IF ((DDD .GE. 265) .AND. (DDD .LT. 354)) THEN
       SEZA=4
       SEZB=3
       DDDA=265
       DDDB=354
       DDDD=DDD
      END IF
C     21.12. - 20.3.
      IF ((DDD .GE. 354) .OR. (DDD .LT. 79)) THEN
       SEZA=3
       SEZB=1
       DDDA=354
       DDDB=365+79
       DDDD=DDD
        IF (DDD .GE. 354) THEN
         DDDD=DDD
        ELSE
         DDDD=DDD+365
        END IF
      END IF
       SEZAI=MOD(SEZA-1,3)+1
       SEZBI=MOD(SEZB-1,3)+1
C     550km level
        N0A550=0.0
        N0B550=0.0
        DO 30 I=1,49
         N0A550=N0A550+C(I)*D(1,SEZAI,I)
30       N0B550=N0B550+C(I)*D(1,SEZBI,I)
        N550A=N0A550
        N550B=N0B550
        N550=(N550B-N550A)/(DDDB-DDDA)*(DDDD-DDDA)+N550A
C     900km level
      N0A900=0.0
	N0B900=0.0
	DO 70 I=1,49
         N0A900=N0A900+C(I)*D(2,SEZAI,I)
70       N0B900=N0B900+C(I)*D(2,SEZBI,I)
	N900A=N0A900
	N900B=N0B900
	N900=(N900B-N900A)/(DDDB-DDDA)*(DDDD-DDDA)+N900A
C     1500km level
      N0A150=0.0
      N0B150=0.0
	DO 110 I=1,49
         N0A150=N0A150+C(I)*D(3,SEZAI,I)
110      N0B150=N0B150+C(I)*D(3,SEZBI,I)
	N150A=N0A150
	N150B=N0B150
	N1500=(N150B-N150A)/(DDDB-DDDA)*(DDDD-DDDA)+N150A
C     2500km level
      N0A250=0.0
	N0B250=0.0
       DO 150 I=1,49
        N0A250=N0A250+C(I)*D(4,SEZAI,I)
150     N0B250=N0B250+C(I)*D(4,SEZBI,I)
       N250A=N0A250
       N250B=N0B250
       N2500=(N250B-N250A)/(DDDB-DDDA)*(DDDD-DDDA)+N250A

C      IF (ALT .LT. 900) NO=(N900-N550)/350.0*(ALT-550)+N550
C      IF ((ALT .GE. 900) .AND. (ALT .LT. 1500))
C     &  NO=(N1500-N900)/600.0*(ALT-900)+N900
c      IF (ALT .GE. 1500) NO=(N2500-N1500)/1000.0*(ALT-1500)+N1500

C     O+ AND N+ may not increase above 1500km 
      IF (((ION .EQ. 0) .OR. (ION .EQ. 3)) .AND. (N2500 .GT. N1500)) 
     & N2500=N1500
C     H+ may not decrease above 1500km 
      IF ((ION .EQ. 1) .AND. (N2500 .LT. N1500)) N2500=N1500
                   
      ANO(1)=N550
	 ANO(2)=N900
	  ANO(3)=N1500
	   ANO(4)=N2500

	AH(1)=550.
       AH(2)=900.
        AH(3)=1500.
         AH(4)=2250.
      DNO(1)=20.
       DNO(2)=20.

      ST1=(ANO(2)-ANO(1))/(AH(2)-AH(1))
      DO 200 I=2,3
       ST2=(ANO(I+1)-ANO(I))/(AH(I+1)-AH(I))
       ANO(I)=ANO(I)-(ST2-ST1)*DNO(I-1)*ALOG(2.)
200   ST1=ST2

      DO 220 I=1,3
220   ST(I)=(ANO(I+1)-ANO(I))/(AH(I+1)-AH(I))

      ARGMAX=88.0
      SUM=ANO(1)+ST(1)*(ALT-AH(1))                     
     
      DO 230 I=1,2
	aa = eptr(alt  ,dno(i),ah(i+1))
	bb = eptr(ah(1),dno(i),ah(i+1))
230   SUM=SUM+(ST(I+1)-ST(I))*(AA-BB)*DNO(I)
                
      NION=10**SUM       
      RETURN
      END
c
c
      REAL FUNCTION INVDPC(FL,DIMO,B0,DIPL,DTOR)
C      calculation of INVDIP from FL, DIMO, BO, and DIPL
C      invariant latitude calculated by highly
C      accurate polynomial expansion
      REAL FL,DIMO,B0,DIPL
	DOUBLE PRECISION B(8),A
      REAL DTOR,ASA,INVL,RINVL,RDIPL,ALFA,BETA
      DATA B/1.259921D0  ,-0.1984259D0 ,-0.04686632D0,-0.01314096D0,
     &      -0.00308824D0, 0.00082777D0,-0.00105877D0, 0.00183142D0/
       A=(DIMO/B0)**(1.0D0/3.0D0)/FL
       ASA=A*(B(1)+B(2)*A+B(3)*A**2+B(4)*A**3+B(5)*A**4+
     &        B(6)*A**5+B(7)*A**6+B(8)*A**7)
       IF (ASA .GT. 1.0) ASA=1.0
C      invariant latitude (absolute value)
       RINVL=ACOS(SQRT(ASA))
       INVL=RINVL/DTOR
       RDIPL=DIPL*DTOR
       ALFA=SIN(ABS(RDIPL))**3
       BETA=COS(RINVL)**3
       INVDPC=(ALFA*SIGN(1.0,DIPL)*INVL+BETA*DIPL)/(ALFA+BETA)
      RETURN
      END
C
C                     
C*************************************************************                  
C************* PEAK VALUES ELECTRON DENSITY ******************                  
C*************************************************************                  
C
C
      real function FOUT(XMODIP,XLATI,XLONGI,UT,FF0)
C CALCULATES CRITICAL FREQUENCY FOF2/MHZ USING SUBROUTINE GAMMA1.      
C XMODIP = MODIFIED DIP LATITUDE, XLATI = GEOG. LATITUDE, XLONGI=
C LONGITUDE (ALL IN DEG.), MONTH = MONTH, UT =  UNIVERSAL TIME 
C (DEC. HOURS), FF0 = ARRAY WITH RZ12-ADJUSTED CCIR/URSI COEFF.
C D.BILITZA,JULY 85.
      DIMENSION FF0(988)
      INTEGER QF(9)
      DATA QF/11,11,8,4,1,0,0,0,0/
      FOUT=GAMMA1(XMODIP,XLATI,XLONGI,UT,6,QF,9,76,13,988,FF0)
      RETURN
      END
C
C
      real function XMOUT(XMODIP,XLATI,XLONGI,UT,XM0)
C CALCULATES PROPAGATION FACTOR M3000 USING THE SUBROUTINE GAMMA1.
C XMODIP = MODIFIED DIP LATITUDE, XLATI = GEOG. LATITUDE, XLONGI=
C LONGITUDE (ALL IN DEG.), MONTH = MONTH, UT =  UNIVERSAL TIME 
C (DEC. HOURS), XM0 = ARRAY WITH RZ12-ADJUSTED CCIR/URSI COEFF.
C D.BILITZA,JULY 85.
      DIMENSION XM0(441)
      INTEGER QM(7)
      DATA QM/6,7,5,2,1,0,0/
      XMOUT=GAMMA1(XMODIP,XLATI,XLONGI,UT,4,QM,7,49,9,441,XM0)
      RETURN
      END
C
C
      REAL FUNCTION HMF2ED(XMAGBR,R,X,XM3)         
C CALCULATES THE PEAK HEIGHT HMF2/KM FOR THE MAGNETIC                           
C LATITUDE XMAGBR/DEG. AND THE SMOOTHED ZUERICH SUNSPOT                         
C NUMBER R USING CCIR-M3000 XM3 AND THE RATIO X=FOF2/FOE.                       
C [REF. D.BILITZA ET AL., TELECOMM.J., 46, 549-553, 1979]                       
C D.BILITZA,1980.     
      F1=(2.32E-3)*R+0.222                         
      F2=1.2-(1.16E-2)*EXP((2.39E-2)*R)            
      F3=0.096*(R-25.0)/150.0                      
      DELM=F1*(1.0-R/150.0*EXP(-XMAGBR*XMAGBR/1600.0))/(X-F2)+F3                
      HMF2ED=1490.0/(XM3+DELM)-176.0               
      RETURN          
      END             
C
C
      REAL FUNCTION XM3000HM(XMAGBR,R,X,HMF2)         
C CALCULATES THE PROPAGATION FACTOR M3000 FOR THE MAGNETIC LATITUDE
C XMAGBR/DEG. AND THE SMOOTHED ZUERICH SUNSPOT NUMBER R USING THE                        
C PEAK HEIGHT HMF2/KM AND THE RATIO X=FOF2/FOE. Reverse of HMF2ED.                      
C [REF. D.BILITZA ET AL., TELECOMM.J., 46, 549-553, 1979]                       
C D.BILITZA,1980.     
      F1=(2.32E-3)*R+0.222                         
      F2=1.2-(1.16E-2)*EXP((2.39E-2)*R)            
      F3=0.096*(R-25.0)/150.0                      
      DELM=F1*(1.0-R/150.0*EXP(-XMAGBR*XMAGBR/1600.0))/(X-F2)+F3                
	  XM3000HM=1490.0/(HMF2+176.0)-DELM
      RETURN          
      END             
C
C
      REAL FUNCTION FOF1ED(YLATI,R,CHI)
c--------------------------------------------------------------
C CALCULATES THE F1 PEAK PLASMA FREQUENCY (FOF1/MHZ)
C FOR   DIP-LATITUDE (YLATI/DEGREE)
c       SMOOTHED ZURICH SUNSPOT NUMBER (R)
c       SOLAR ZENITH ANGLE (CHI/DEGREE)
C REFERENCE: 
c       E.D.DUCHARME ET AL., RADIO SCIENCE 6, 369-378, 1971
C                                      AND 8, 837-839, 1973
c       HOWEVER WITH MAGNETIC DIP LATITUDE INSTEAD OF GEOMAGNETIC
c       DIPOLE LATITUDE, EYFRIG, 1979                    
C--------------------------------------------- D. BILITZA, 1988.   
        COMMON/CONST/UMR
	    fof1ed=0.0
	    if (chi.gt.90.0) return

        DLA =  YLATI
        F0 = 4.35 + DLA * ( 0.0058 - 1.2E-4 * DLA ) 
        F100 = 5.348 + DLA * ( 0.011 - 2.3E-4 * DLA )
        FS = F0 + ( F100 - F0 ) * R / 100.0
        XMUE = 0.093 + DLA * ( 0.0046 - 5.4E-5 * DLA ) + 3.0E-4 * R
        FOF1 = FS * COS( CHI * UMR ) ** XMUE
                CHI0 = 49.84733 + 0.349504 * DLA
                CHI100 = 38.96113 + 0.509932 * DLA
                CHIM = ( CHI0 + ( CHI100 - CHI0 ) * R / 100. )
                IF(CHI.GT.CHIM) FOF1=-FOF1 
        FOF1ED = FOF1     
        RETURN
        END             
C
C
	real function f1_c1(xmodip,hour,suxnon,saxnon)
c F1 layer shape parameter C1 after Reinisch and Huang, Advances in
c Space Research, Volume 25, Number 1, 81-88, 2000.

        common	/const/umr
        pi = umr * 180.
	
        ABSMDP=ABS(XMODIP)
      	DELA=4.32
      	IF(ABSMDP.GE.18.) DELA=1.0+EXP(-(ABSMDP-30.0)/10.0)

      	C1OLD = 0.09 + 0.11/DELA
        if(suxnon.eq.saxnon) then
            c1 = 2.5 * c1old
        else
            c1 = 2.5*c1old*cos((HOUR-12.)/(suxnon-saxnon)*pi)
        endif
      	if(c1.lt.0.0) c1=0.0
	    f1_c1=c1
	    return
	    end
c
c
        subroutine f1_prob (sza,glat,rz12,f1prob,f1probl)
c Occurrence probability of F1 layer after Scotto et al., Advances in
c Space Research, Volume 20, Number 9, 1773-1775, 1997.
c Input: solar zenith angle (sza) in degrees, geomagnetic latitude
c (glat) in degrees, 12-month running mean of sunspot number (Rz12).
c Output: F1 occurrence probability without L-condition cases (f1prob)
c and with L-condition cases (f1probl)
        common /const/umr

	    xarg = 0.5 + 0.5 * cos(sza*umr)
		a = 2.98 + 0.0854 * rz12
		b = 0.0107 - 0.0022 * rz12
		c = -0.000256 + 0.0000147 * rz12
		gamma = a + ( b + c * glat) * glat
	    f1pr = xarg ** gamma
        if(f1pr.lt.1.e-3) f1pr=0.0
        f1prob=f1pr
	    f1prl = xarg ** 2.36
        if(f1prl.lt.1.e-3) f1prl=0.0
        f1probl=f1prl
	    return
	    end
C
C
        REAL FUNCTION FOEEDI(COV,XHI,XHIM,XLATI)
C-------------------------------------------------------
C CALCULATES FOE/MHZ BY THE EDINBURGH-METHOD.      
C INPUT: MONTHLY MEAN 10.7CM SOLAR RADIO FLUX measured at ground level  
C (COV), GEOGRAPHIC LATITUDE (XLATI/DEG), SOLAR ZENITH ANGLE (XHI/DEG 
C AND XHIM/DEG AT NOON).
C REFERENCE: 
C       KOURIS-MUGGELETON, CCIR DOC. 6/3/07, 1973
C       TROST, J. GEOPHYS. RES. 84, 2736, 1979 (was used
C               to improve the nighttime varition)
C D.BILITZA--------------------------------- AUGUST 1986.    
        COMMON/CONST/UMR
C variation with solar activity (factor A) ...............
        A=1.0+0.0094*(COV-66.0)                      
C variation with noon solar zenith angle (B) and with latitude (C)
        SL=COS(XLATI*UMR)
        IF(XLATI.LT.32.0) THEN
                SM=-1.93+1.92*SL                             
                C=23.0+116.0*SL                              
        ELSE
                SM=0.11-0.49*SL                              
                C=92.0+35.0*SL  
        ENDIF
        if(XHIM.ge.90.) XHIM=89.999
        B = COS(XHIM*UMR) ** SM
C variation with solar zenith angle (D) ..........................        
        IF(XLATI.GT.12.0) THEN
                SP=1.2
        ELSE
                SP=1.31         
        ENDIF
C adjusted solar zenith angle during nighttime (XHIC) .............
        XHIC=XHI-3.*ALOG(1.+EXP((XHI-89.98)/3.))   
        D=COS(XHIC*UMR)**SP       
C determine foE**4 ................................................
        R4FOE=A*B*C*D     
C minimum allowable foE (sqrt[SMIN])...............................
        SMIN=0.121+0.0015*(COV-60.)
        SMIN=SMIN*SMIN
        IF(R4FOE.LT.SMIN) R4FOE=SMIN                     
        FOEEDI=R4FOE**0.25                           
        RETURN          
        END   
C
C
        REAL FUNCTION XMDED(XHI,R,YW)                
C D. BILITZA, 1978, CALCULATES ELECTRON DENSITY OF D MAXIMUM.                   
C XHI/DEG. IS SOLAR ZENITH ANGLE, R SMOOTHED ZURICH SUNSPOT NUMBER              
C AND YW/M-3 THE ASSUMED CONSTANT NIGHT VALUE.     
C [REF.: D.BILITZA, WORLD DATA CENTER A REPORT UAG-82,7,BOULDER,1981]
C corrected 4/25/97 - D. Bilitza
c
        COMMON/CONST/UMR
c
        if(xhi.ge.90) goto 100
        Y = 6.05E8 + 0.088E8 * R
        yy = cos ( xhi * umr )
        yyy = -0.1 / ( yy**2.7 ) 
        if (yyy.lt.-40.) then 
        	ymd=0.0
        else
        	ymd = y * exp(yyy)
        endif
        if (ymd.lt.yw) ymd = yw
        xmded=ymd
        RETURN          

100     XMDED=YW        
        RETURN          
        END
C
C
        REAL FUNCTION GAMMA1(SMODIP,SLAT,SLONG,HOUR,
     &                          IHARM,NQ,K1,M,MM,M3,SFE)      
C CALCULATES GAMMA1=FOF2 OR M3000 USING CCIR NUMERICAL MAP                      
C COEFFICIENTS SFE(M3) FOR MODIFIED DIP LATITUDE (SMODIP/DEG)
C GEOGRAPHIC LATITUDE (SLAT/DEG) AND LONGITUDE (SLONG/DEG)  
C AND UNIVERSIAL TIME (HOUR/DECIMAL HOURS).
C NQ(K1) IS AN INTEGER ARRAY GIVING THE HIGHEST DEGREES IN 
C LATITUDE FOR EACH LONGITUDE HARMONIC.                  
C M=1+NQ1+2(NQ2+1)+2(NQ3+1)+... .                  
C SHEIKH,4.3.77.      
      REAL*8 C(12),S(12),COEF(100),SUM             
      DIMENSION NQ(K1),XSINX(13),SFE(M3)           
      COMMON/CONST/UMR
      HOU=(15.0*HOUR-180.0)*UMR                    
      S(1)=SIN(HOU)   
      C(1)=COS(HOU)   
      DO 250 I=2,IHARM                             
      C(I)=C(1)*C(I-1)-S(1)*S(I-1)                 
      S(I)=C(1)*S(I-1)+S(1)*C(I-1)                 
250   CONTINUE        
      DO 300 I=1,M    
      MI=(I-1)*MM     
      COEF(I)=SFE(MI+1)                            
      DO 300 J=1,IHARM                             
      COEF(I)=COEF(I)+SFE(MI+2*J)*S(J)+SFE(MI+2*J+1)*C(J)                       
300   CONTINUE        
      SUM=COEF(1)     
      SS=SIN(SMODIP*UMR)                           
      S3=SS           
      XSINX(1)=1.0    
      INDEX=NQ(1)     
      DO 350 J=1,INDEX                             
      SUM=SUM+COEF(1+J)*SS                         
      XSINX(J+1)=SS   
      SS=SS*S3        
350   CONTINUE        
      XSINX(NQ(1)+2)=SS                            
      NP=NQ(1)+1      
      SS=COS(SLAT*UMR)                             
      S3=SS           
      DO 400 J=2,K1   
      S0=SLONG*(J-1.)*UMR                          
      S1=COS(S0)      
      S2=SIN(S0)      
      INDEX=NQ(J)+1   
      DO 450 L=1,INDEX                             
      NP=NP+1         
      SUM=SUM+COEF(NP)*XSINX(L)*SS*S1              
      NP=NP+1         
      SUM=SUM+COEF(NP)*XSINX(L)*SS*S2              
450   CONTINUE        
      SS=SS*S3        
400   CONTINUE        
      GAMMA1=SUM      
      RETURN          
      END 
C
C                     
C************************************************************                   
C***************** PROFILE PARAMETERS ***********************                   
C************************************************************                 
C
C
        REAL FUNCTION B0_98 ( HOUR, SAX, SUX, NSEASN, R, ZLO, ZMODIP)
C-----------------------------------------------------------------
C Interpolation procedure for bottomside thickness parameter B0.
C Array B0F(ILT,ISEASON,IR,ILATI) distinguishes between day and
C night (ILT=1,2), four seasons (ISEASON is northern season with
C ISEASON=1 northern spring), low and high solar activity Rz12=10,
C 100 (IR=1,2), and modified dip latitudes of 0, 18 and 45
C degress (ILATI=1,2,3). In the DATA statement the first value
C corresponds to B0F(1,1,1,1), the second to B0F(2,1,1,1), the
C third to B0F(1,2,1,1) and so on.
C
C input:
C       hour    LT in decimal hours
C       SAX     time of sunrise in decimal hours
C       SUX     time of sunset in decimal hours
C       nseasn  season in northern hemisphere (1=spring)
C       R       12-month running mean of sunspot number
C       ZLO     longitude
C       ZMODIP  modified dip latitude
C
C JUNE 1989 --------------------------------------- Dieter Bilitza
C
C Updates (B0_new -> B0_98):
C
C 01/98 corrected to include a smooth transition at the modip equator
C       and no discontinuity at the equatorial change in season.
C 09/98 new B0 values incl values at the magnetic equator
C 10/98 longitude as input to determine if magnetic equator in northern 
C         or southern hemisphere
C
      REAL      NITVAL
      DIMENSION B0F(2,4,2,3),bfr(2,2,3),bfd(2,3),zx(5),g(6),dd(5)
      DATA      B0F/201,68,210,61,192,68,199,67,240,80,245,83,
     &              233,71,230,65,108,65,142,81,110,68,77,75,
     &              124,98,164,100,120,94,96,112,78,81,94,84,
     &              81,81,65,70,102,87,127,91,109,88,81,78/
        data    zx/45.,72.,90.,108.,135./,dd/5*3.0/

        num_lat=3

C jseasn is southern hemisphere season
        jseasn=nseasn+2
        if(jseasn.gt.4) jseasn=jseasn-4

        zz = zmodip + 90.
        zz0 = 0.

C Interpolation in Rz12: linear from 10 to 100
        DO 7035 ISL=1,num_lat
          DO 7034 ISD=1,2
            bfr(isd,1,isl) = b0f(isd,nseasn,1,isl) +
     &      (b0f(isd,nseasn,2,isl) - b0f(isd,nseasn,1,isl))/90.*(R-10.)
            bfr(isd,2,isl) = b0f(isd,jseasn,1,isl) +
     &      (b0f(isd,jseasn,2,isl) - b0f(isd,jseasn,1,isl))/90.*(R-10.)
7034      continue
C Interpolation day/night with transitions at SAX (sunrise)
C and SUX (sunset) for northern/southern hemisphere iss=1/2
          do 7033 iss=1,2
                DAYVAL = BFR(1,ISS,ISL)
                NITVAL = BFR(2,ISS,ISL)
                BFD(iss,ISL) = HPOL(HOUR,DAYVAL,NITVAL,SAX,SUX,1.,1.)
7033      continue
7035    continue

C Interpolation with epstein-transitions in modified dip latitude.
C Transitions at +/-18 and +/-45 degrees; constant above +/-45.
C
C g(1:5) are the latitudinal slopes of B0;
C       g(1) is for the region from -90 to -45 degrees
C       g(2) is for the region from -45 to -18 degrees
C       g(3) is for the region from -18 to   0 degrees
C       g(4) is for the region from   0 to  18 degrees
C       g(5) is for the region from  18 to  45 degrees
C       g(6) is for the region from  45 to  90 degrees
C
C B0 =  bfd(2,3) at modip = -45,
C       bfd(2,2) at modip = -18,
C       bfd(2,1) or bfd(1,1) at modip = 0,
C       bfd(1,2) at modip = 20,
C       bfd(1,3) at modip = 45.
C If the Longitude is between 200 and 320 degrees than the modip 
C equator is in the southern hemisphere and bfd(2,1) is used at the 
C equator, otherwise bfd(1,1) is used.
c
        zx1=bfd(2,3)
        zx2=bfd(2,2)
        zx3=bfd(1,1)
        if(zlo.gt.200.0.and.zlo.lt.320) zx3=bfd(2,1)
        zx4=bfd(1,2)
        zx5=bfd(1,3)
        g(1) = 0.
        g(2) = ( zx2 - zx1 ) / 27.
        g(3) = ( zx3 - zx2 ) / 18.
        g(4) = ( zx4 - zx3 ) / 18.
        g(5) = ( zx5 - zx4 ) / 27.
        g(6) = 0.

c        bb0 = bfd(2,3)
c      SUM = bb0
        sum=zx1
      DO 1 I=1,5
        aa = eptr(zz ,dd(i),zx(i))
        bb = eptr(zz0,dd(i),zx(i))
        DSUM = (G(I+1) - G(I)) * (AA-BB) * dd(i)
        SUM = SUM + DSUM
1       continue
      B0_98 = SUM

        RETURN
        END
C
C
      SUBROUTINE TAL(SHABR,SDELTA,SHBR,SDTDH0,AUS6,SPT)                         
C CALCULATES THE COEFFICIENTS SPT FOR THE POLYNOMIAL
C Y(X)=1+SPT(1)*X**2+SPT(2)*X**3+SPT(3)*X**4+SPT(4)*X**5               
C TO FIT THE VALLEY IN Y, REPRESENTED BY:                
C Y(X=0)=1, THE X VALUE OF THE DEEPEST VALLEY POINT (SHABR),                    
C THE PRECENTAGE DEPTH (SDELTA), THE WIDTH (SHBR) AND THE                       
C DERIVATIVE DY/DX AT THE UPPER VALLEY BOUNDRY (SDTDH0).                        
C IF THERE IS AN UNWANTED ADDITIONAL EXTREMUM IN THE VALLEY                     
C REGION, THEN AUS6=.TRUE., ELSE AUS6=.FALSE..     
C FOR -SDELTA THE COEFF. ARE CALCULATED FOR THE FUNCTION                        
C Y(X)=EXP(SPT(1)*X**2+...+SPT(4)*X**5).           
      DIMENSION SPT(4)                             
      LOGICAL AUS6    
      Z1=-SDELTA/(100.0*SHABR*SHABR)               
      IF(SDELTA.GT.0.) GOTO 500                    
      SDELTA=-SDELTA  
      Z1=ALOG(1.-SDELTA/100.)/(SHABR*SHABR)        
500   Z3=SDTDH0/(2.*SHBR)                          
      Z4=SHABR-SHBR   
      SPT(4)=2.0*(Z1*(SHBR-2.0*SHABR)*SHBR+Z3*Z4*SHABR)/                        
     &  (SHABR*SHBR*Z4*Z4*Z4)                        
      SPT(3)=Z1*(2.0*SHBR-3.0*SHABR)/(SHABR*Z4*Z4)-
     &  (2.*SHABR+SHBR)*SPT(4)          
      SPT(2)=-2.0*Z1/SHABR-2.0*SHABR*SPT(3)-3.0*SHABR*SHABR*SPT(4)              
      SPT(1)=Z1-SHABR*(SPT(2)+SHABR*(SPT(3)+SHABR*SPT(4)))                      
      AUS6=.FALSE.    
      B=4.*SPT(3)/(5.*SPT(4))+SHABR                
      C=-2.*SPT(1)/(5*SPT(4)*SHABR)                
      Z2=B*B/4.-C     
      IF(Z2.LT.0.0) GOTO 300                       
      Z3=SQRT(Z2)     
      Z1=B/2.         
      Z2=-Z1+Z3       
      IF(Z2.GT.0.0.AND.Z2.LT.SHBR) AUS6=.TRUE.     
      IF (ABS(Z3).GT.1.E-15) GOTO 400              
      Z2=C/Z2         
      IF(Z2.GT.0.0.AND.Z2.LT.SHBR) AUS6=.TRUE.     
      RETURN          
400   Z2=-Z1-Z3       
      IF(Z2.GT.0.0.AND.Z2.LT.SHBR) AUS6=.TRUE.     
300   RETURN          
      END             
C
C
        SUBROUTINE VALGUL(XHI,HVB,VWU,VWA,VDP)
C --------------------------------------------------------------------- 
C   CALCULATES E-F VALLEY PARAMETERS; T.L. GULYAEVA, ADVANCES IN
C   SPACE RESEARCH 7, #6, 39-48, 1987.
C
C       INPUT:  XHI     SOLAR ZENITH ANGLE [DEGREE]
C       
C       OUTPUT: VDP     VALLEY DEPTH  (NVB/NME)
C               VWU     VALLEY WIDTH  [KM]
C               VWA     VALLEY WIDTH  (SMALLER, CORRECTED BY RAWER)
C               HVB     HEIGHT OF VALLEY BASE [KM]
C -----------------------------------------------------------------------
C
        COMMON  /CONST/UMR
C
        CS = 0.1 + COS(UMR*XHI)
        ABC = ABS(CS)
        VDP = 0.45 * CS / (0.1 + ABC ) + 0.55
        ARL = ( 0.1 + ABC + CS ) / ( 0.1 + ABC - CS)
        ZZZ = ALOG( ARL )
        VWU = 45. - 10. * ZZZ
        VWA = 45. -  5. * ZZZ
        HVB = 1000. / ( 7.024 + 0.224 * CS + 0.966 * ABC )
        RETURN
        END
C
C                     
C************************************************************                   
C*************** EARTH MAGNETIC FIELD ***********************                   
C**************************************************************                 
C
C
      SUBROUTINE GGM(ART,LONG,LATI,MLONG,MLAT)            
C CALCULATES GEOMAGNETIC LONGITUDE (MLONG) AND LATITUDE (MLAT) 
C FROM GEOGRAFIC LONGITUDE (LONG) AND LATITUDE (LATI) FOR ART=0
C AND REVERSE FOR ART=1. ALL ANGLES IN DEGREE.
C LATITUDE:-90 TO 90. LONGITUDE:0 TO 360 EAST.         
      INTEGER ART     
      REAL MLONG,MLAT,LONG,LATI
      COMMON/CONST/FAKTOR
      ZPI=FAKTOR*360.                              
      CBG=11.4*FAKTOR                              
      CI=COS(CBG)     
      SI=SIN(CBG)
      IF(ART.EQ.0) GOTO 10                         
      CBM=COS(MLAT*FAKTOR)                           
      SBM=SIN(MLAT*FAKTOR)                           
      CLM=COS(MLONG*FAKTOR)                          
      SLM=SIN(MLONG*FAKTOR)
      SBG=SBM*CI-CBM*CLM*SI
        IF(ABS(SBG).GT.1.) SBG=SIGN(1.,SBG)
      LATI=ASIN(SBG)
      CBG=COS(LATI)     
      SLG=(CBM*SLM)/CBG  
      CLG=(SBM*SI+CBM*CLM*CI)/CBG
        IF(ABS(CLG).GT.1.) CLG=SIGN(1.,CLG)                  
      LONG=ACOS(CLG)  
      IF(SLG.LT.0.0) LONG=ZPI-LONG
      LATI=LATI/FAKTOR    
      LONG=LONG/FAKTOR  
      LONG=LONG-69.8    
      IF(LONG.LT.0.0) LONG=LONG+360.0                 
      RETURN          
10    YLG=LONG+69.8    
      CBG=COS(LATI*FAKTOR)                           
      SBG=SIN(LATI*FAKTOR)                           
      CLG=COS(YLG*FAKTOR)                          
      SLG=SIN(YLG*FAKTOR)                          
      SBM=SBG*CI+CBG*CLG*SI                        
        IF(ABS(SBM).GT.1.) SBM=SIGN(1.,SBM)
      MLAT=ASIN(SBM)   
      CBM=COS(MLAT)     
      SLM=(CBG*SLG)/CBM                            
      CLM=(-SBG*SI+CBG*CLG*CI)/CBM
        IF(ABS(CLM).GT.1.) CLM=SIGN(1.,CLM) 
      MLONG=ACOS(CLM)
      IF(SLM.LT..0) MLONG=ZPI-MLONG
      MLAT=MLAT/FAKTOR    
      MLONG=MLONG/FAKTOR  
      RETURN          
      END             
C
C
      SUBROUTINE FIELDG(DLAT,DLONG,ALT,X,Y,Z,F,DIP,DEC,SMODIP)                  
C THIS IS A SPECIAL VERSION OF THE POGO 68/10 MAGNETIC FIELD                    
C LEGENDRE MODEL. TRANSFORMATION COEFF. G(144) VALID FOR 1973.                  
C INPUT: DLAT, DLONG=GEOGRAPHIC COORDINATES/DEG.(-90/90,0/360),                 
C        ALT=ALTITUDE/KM.                          
C OUTPUT: F TOTAL FIELD (GAUSS), Z DOWNWARD VERTICAL COMPONENT                  
C        X,Y COMPONENTS IN THE EQUATORIAL PLANE (X TO ZERO LONGITUDE).          
C        DIP INCLINATION ANGLE(DEGREE). SMODIP RAWER'S MODFIED DIP.             
C SHEIK,1977.         
      DIMENSION H(144),XI(3),G(144),FEL1(72),FEL2(72)
      COMMON/CONST/UMR                           
      DATA FEL1/0.0, 0.1506723,0.0101742, -0.0286519, 0.0092606,                
     & -0.0130846, 0.0089594, -0.0136808,-0.0001508, -0.0093977,                
     & 0.0130650, 0.0020520, -0.0121956, -0.0023451, -0.0208555,                
     & 0.0068416,-0.0142659, -0.0093322, -0.0021364, -0.0078910,                
     & 0.0045586,  0.0128904, -0.0002951, -0.0237245,0.0289493,                 
     & 0.0074605, -0.0105741, -0.0005116, -0.0105732, -0.0058542,               
     &0.0033268, 0.0078164,0.0211234, 0.0099309, 0.0362792,                     
     &-0.0201070,-0.0046350,-0.0058722,0.0011147,-0.0013949,                    
     & -0.0108838,  0.0322263, -0.0147390,  0.0031247, 0.0111986,               
     & -0.0109394,0.0058112,  0.2739046, -0.0155682, -0.0253272,                
     &  0.0163782, 0.0205730,  0.0022081, 0.0112749,-0.0098427,                 
     & 0.0072705, 0.0195189, -0.0081132, -0.0071889, -0.0579970,                
     & -0.0856642, 0.1884260,-0.7391512, 0.1210288, -0.0241888,                 
     & -0.0052464, -0.0096312, -0.0044834, 0.0201764,  0.0258343,               
     &0.0083033,  0.0077187/                       
      DATA FEL2/0.0586055,0.0102236,-0.0396107,    
     & -0.0167860, -0.2019911, -0.5810815,0.0379916,  3.7508268,                
     & 1.8133030, -0.0564250, -0.0557352, 0.1335347, -0.0142641,                
     & -0.1024618,0.0970994, -0.0751830,-0.1274948, 0.0402073,                  
     &  0.0386290, 0.1883088,  0.1838960, -0.7848989,0.7591817,                 
     & -0.9302389,-0.8560960, 0.6633250, -4.6363869, -13.2599277,               
     & 0.1002136,  0.0855714,-0.0991981, -0.0765378,-0.0455264,                 
     &  0.1169326, -0.2604067, 0.1800076, -0.2223685, -0.6347679,               
     &0.5334222, -0.3459502,-0.1573697,  0.8589464, 1.7815990,                  
     &-6.3347645, -3.1513653, -9.9927750,13.3327637, -35.4897308,               
     &37.3466339, -0.5257398,  0.0571474, -0.5421217,  0.2404770,               
     & -0.1747774,-0.3433644, 0.4829708,0.3935944, 0.4885033,                   
     &  0.8488121, -0.7640999, -1.8884945, 3.2930784,-7.3497229,                
     & 0.1672821,-0.2306652, 10.5782146, 12.6031065, 8.6579742,                 
     & 215.5209961, -27.1419220,22.3405762,1108.6394043/                        
      K=0             
      DO 10 I=1,72    
      K=K+1           
      G(K)=FEL1(I)    
10    G(72+K)=FEL2(I)                              
      RLAT=DLAT*UMR   
      CT=SIN(RLAT)    
      ST=COS(RLAT)    
      NMAX=11         
      D=SQRT(40680925.0-272336.0*CT*CT)            
      RLONG=DLONG*UMR                              
      CP=COS(RLONG)   
      SP=SIN(RLONG)   
      ZZZ=(ALT+40408589.0/D)*CT/6371.2             
      RHO=(ALT+40680925.0/D)*ST/6371.2             
      XXX=RHO*CP      
      YYY=RHO*SP      
      RQ=1.0/(XXX*XXX+YYY*YYY+ZZZ*ZZZ)             
      XI(1)=XXX*RQ    
      XI(2)=YYY*RQ    
      XI(3)=ZZZ*RQ    
      IHMAX=NMAX*NMAX+1                            
      LAST=IHMAX+NMAX+NMAX                         
      IMAX=NMAX+NMAX-1                             
      DO 100 I=IHMAX,LAST                          
100   H(I)=G(I)       
      DO 200 K=1,3,2  
      I=IMAX          
      IH=IHMAX        
300   IL=IH-I         
      F1=2./(I-K+2.)  
      X1=XI(1)*F1     
      Y1=XI(2)*F1     
      Z1=XI(3)*(F1+F1)                             
      I=I-2           
      IF((I-1).LT.0) GOTO 400                      
      IF((I-1).EQ.0) GOTO 500                      
      DO 600 M=3,I,2  
      H(IL+M+1)=G(IL+M+1)+Z1*H(IH+M+1)+X1*(H(IH+M+3)-H(IH+M-1))-                
     &Y1*(H(IH+M+2)+H(IH+M-2))                     
      H(IL+M)=G(IL+M)+Z1*H(IH+M)+X1*(H(IH+M+2)-H(IH+M-2))+                      
     &Y1*(H(IH+M+3)+H(IH+M-1))                     
600   CONTINUE        
500   H(IL+2)=G(IL+2)+Z1*H(IH+2)+X1*H(IH+4)-Y1*(H(IH+3)+H(IH))                  
      H(IL+1)=G(IL+1)+Z1*H(IH+1)+Y1*H(IH+4)+X1*(H(IH+3)-H(IH))                  
400   H(IL)=G(IL)+Z1*H(IH)+2.0*(X1*H(IH+1)+Y1*H(IH+2))                          
700   IH=IL           
      IF(I.GE.K) GOTO 300                          
200   CONTINUE        
      S=0.5*H(1)+2.0*(H(2)*XI(3)+H(3)*XI(1)+H(4)*XI(2))                         
      XT=(RQ+RQ)*SQRT(RQ)                          
      X=XT*(H(3)-S*XXX)                            
      Y=XT*(H(4)-S*YYY)                            
      Z=XT*(H(2)-S*ZZZ)                            
      F=SQRT(X*X+Y*Y+Z*Z)                          
      BRH0=Y*SP+X*CP  
      Y=Y*CP-X*SP     
      X=Z*ST-BRH0*CT  
      Z=-Z*CT-BRH0*ST 
        zdivf=z/f
        IF(ABS(zdivf).GT.1.) zdivf=SIGN(1.,zdivf)
      DIP=ASIN(zdivf)
        ydivs=y/sqrt(x*x+y*y)  
        IF(ABS(ydivs).GT.1.) ydivs=SIGN(1.,ydivs)
      DEC=ASIN(ydivs)
        dipdiv=DIP/SQRT(DIP*DIP+ST)
        IF(ABS(dipdiv).GT.1.) dipdiv=SIGN(1.,dipdiv)
      SMODIP=ASIN(dipdiv)
      DIP=DIP/UMR     
      DEC=DEC/UMR     
      SMODIP=SMODIP/UMR                            
      RETURN          
      END             
C
C
C************************************************************                   
C*********** INTERPOLATION AND REST ***************************                 
C**************************************************************                 
C
C
      SUBROUTINE REGFA1(X11,X22,FX11,FX22,EPS,FW,F,SCHALT,X) 
C REGULA-FALSI-PROCEDURE TO FIND X WITH F(X)-FW=0. X1,X2 ARE THE                
C STARTING VALUES. THE COMUTATION ENDS WHEN THE X-INTERVAL                      
C HAS BECOME LESS THAN EPS . IF SIGN(F(X1)-FW)= SIGN(F(X2)-FW)                  
C THEN SCHALT=.TRUE.  
      LOGICAL L1,LINKS,K,SCHALT                    
      SCHALT=.FALSE.
      EP=EPS  
      X1=X11          
      X2=X22          
      F1=FX11-FW     
      F2=FX22-FW     
      K=.FALSE.       
      NG=2       
      LFD=0     
      IF(F1*F2.LE.0.0) GOTO 200
        X=0.0           
        SCHALT=.TRUE.   
        RETURN
200   X=(X1*F2-X2*F1)/(F2-F1)                      
      GOTO 400        
300     L1=LINKS        
        DX=(X2-X1)/NG
        IF(.NOT.LINKS) DX=DX*(NG-1)
        X=X1+DX
400   FX=F(X)-FW
      LFD=LFD+1
      IF(LFD.GT.20) THEN
        EP=EP*10.
        LFD=0
      ENDIF 
      LINKS=(F1*FX.GT.0.0)
      K=.NOT.K        
      IF(LINKS) THEN
        X1=X            
        F1=FX           
      ELSE
        X2=X 
        F2=FX 
      ENDIF   
      IF(ABS(X2-X1).LE.EP) GOTO 800               
      IF(K) GOTO 300  
      IF((LINKS.AND.(.NOT.L1)).OR.(.NOT.LINKS.AND.L1)) NG=2*NG                  
      GOTO 200        
800   RETURN          
      END             
C
C
C******************************************************************
C********** ZENITH ANGLE, DAY OF YEAR, TIME ***********************
C******************************************************************
C
C
        subroutine soco (ld,t,flat,Elon,height,
     &          DECLIN, ZENITH, SUNRSE, SUNSET)
c--------------------------------------------------------------------
c       s/r to calculate the solar declination, zenith angle, and
c       sunrise & sunset times  - based on Newbern Smith's algorithm
c       [leo mcnamara, 1-sep-86, last modified 16-jun-87]
c       {dieter bilitza, 30-oct-89, modified for IRI application}
c
c in:   ld      local day of year
c       t       local hour (decimal)
c       flat    northern latitude in degrees
c       elon    east longitude in degrees
c		height	height in km
c
c out:  declin      declination of the sun in degrees
c       zenith      zenith angle of the sun in degrees
c       sunrse      local time of sunrise in hours 
c       sunset      local time of sunset in hours 
c-------------------------------------------------------------------
c
        common/const/   dtr     /const1/humr,dumr
c amplitudes of Fourier coefficients  --  1955 epoch.................
        data    p1,p2,p3,p4,p6 /
     &  0.017203534,0.034407068,0.051610602,0.068814136,0.103221204 /
c
c s/r is formulated in terms of WEST longitude.......................
        wlon = 360. - Elon
c
c time of equinox for 1980...........................................
        td = ld + (t + Wlon/15.) / 24.
        te = td + 0.9369
c
c declination of the sun..............................................
        dcl = 23.256 * sin(p1*(te-82.242)) + 0.381 * sin(p2*(te-44.855))
     &      + 0.167 * sin(p3*(te-23.355)) - 0.013 * sin(p4*(te+11.97))
     &      + 0.011 * sin(p6*(te-10.41)) + 0.339137
        DECLIN = dcl
        dc = dcl * dtr
c
c the equation of time................................................
        tf = te - 0.5
        eqt = -7.38*sin(p1*(tf-4.)) - 9.87*sin(p2*(tf+9.))
     &      + 0.27*sin(p3*(tf-53.)) - 0.2*cos(p4*(tf-17.))
        et = eqt * dtr / 4.
c
        fa = flat * dtr
        phi = humr * ( t - 12.) + et
c
        a = sin(fa) * sin(dc)
        b = cos(fa) * cos(dc)
        cosx = a + b * cos(phi)
        if(abs(cosx).gt.1.) cosx=sign(1.,cosx)
        zenith = acos(cosx) / dtr
c
c calculate sunrise and sunset times --  at the ground...........
c see Explanatory Supplement to the Ephemeris (1961) pg 401......
c sunrise at height h metres is at...............................
		h=height*1000.
        chih = 90.83 + 0.0347 * sqrt(h)
c this includes corrections for horizontal refraction and........
c semi-diameter of the solar disk................................
        ch = cos(chih * dtr)
        cosphi = (ch -a ) / b
c if abs(secphi) > 1., sun does not rise/set.....................
c allow for sun never setting - high latitude summer.............
        secphi = 999999.
        if(cosphi.ne.0.) secphi = 1./cosphi
        sunset = 99.
        sunrse = 99.
        if(secphi.gt.-1.0.and.secphi.le.0.) return
c allow for sun never rising - high latitude winter..............
        sunset = -99.
        sunrse = -99.
        if(secphi.gt.0.0.and.secphi.lt.1.) return
c
        if(cosphi.gt.1.) cosphi=sign(1.,cosphi)
        phi = acos(cosphi)
        et = et / humr
        phi = phi / humr
        sunrse = 12. - phi - et
        sunset = 12. + phi - et
        if(sunrse.lt.0.) sunrse = sunrse + 24.
        if(sunset.ge.24.) sunset = sunset - 24.
c
        return
        end
c
C
      FUNCTION HPOL(HOUR,TW,XNW,SA,SU,DSA,DSU)            
C-------------------------------------------------------
C PROCEDURE FOR SMOOTH TIME-INTERPOLATION USING EPSTEIN  
C STEP FUNCTION AT SUNRISE (SA) AND SUNSET (SU). THE 
C STEP-WIDTH FOR SUNRISE IS DSA AND FOR SUNSET DSU.
C TW,NW ARE THE DAY AND NIGHT VALUE OF THE PARAMETER TO 
C BE INTERPOLATED. SA AND SU ARE TIME OF SUNRIES AND 
C SUNSET IN DECIMAL HOURS.
C BILITZA----------------------------------------- 1979.
        IF(ABS(SU).GT.25.) THEN
                IF(SU.GT.0.0) THEN
                        HPOL=TW
                ELSE
                        HPOL=XNW
                ENDIF
                RETURN
        ENDIF
      HPOL=XNW+(TW-XNW)*EPST(HOUR,DSA,SA)+
     &  (XNW-TW)*EPST(HOUR,DSU,SU) 
      RETURN          
      END       
C      
C
        SUBROUTINE MODA(IN,IYEAR,MONTH,IDAY,IDOY,NRDAYMO)
C-------------------------------------------------------------------
C CALCULATES DAY OF YEAR (IDOY, ddd) FROM YEAR (IYEAR, yy or yyyy), 
C MONTH (MONTH, mm) AND DAY OF MONTH (IDAY, dd) IF IN=0, OR MONTH 
C AND DAY FROM YEAR AND DAY OF YEAR IF IN=1. NRDAYMO is an output 
C parameter providing the number of days in the specific month.
C-------------------------------------------------------------------
        DIMENSION       MM(12)
        DATA            MM/31,28,31,30,31,30,31,31,30,31,30,31/

        IMO=0
        MOBE=0
c
c  leap year rule: years evenly divisible by 4 are leap years, except
c  years also evenly divisible by 100 are not leap years, except years 
c  also evenly divisible by 400 are leap years. The year 2000 therefore 
C  is a leap year. The 100 and 400 year exception rule
c     if((iyear/4*4.eq.iyear).and.(iyear/100*100.ne.iyear)) mm(2)=29
c  will become important again in the year 2100 which is not a leap 
C  year.
c
        mm(2)=28
        if(iyear/4*4.eq.iyear) mm(2)=29

        IF(IN.GT.0) GOTO 5
                mosum=0
                if(month.gt.1) then
                        do 1234 i=1,month-1 
1234                            mosum=mosum+mm(i)
                        endif
                idoy=mosum+iday
                nrdaymo=mm(month)
                RETURN

5       IMO=IMO+1
                IF(IMO.GT.12) GOTO 55
                MOOLD=MOBE
                nrdaymo=mm(imo)
                MOBE=MOBE+nrdaymo
                IF(MOBE.LT.IDOY) GOTO 5
55              MONTH=IMO
                IDAY=IDOY-MOOLD
        RETURN
        END             
c
c
        subroutine ut_lt(mode,ut,slt,glong,iyyy,ddd)
c -----------------------------------------------------------------
c Converts Universal Time UT (decimal hours) into Solar Local Time
c SLT (decimal hours) for given date (iyyy is year, e.g. 1995; ddd
c is day of year, e.g. 1 for Jan 1) and geodatic longitude in degrees.
C For mode=0 UT->LT and for mode=1 LT->UT
c Please NOTE that iyyy and ddd are input as well as output parameters
c since the determined LT may be for a day before or after the UT day.
c ------------------------------------------------- bilitza nov 95
        integer         ddd,dddend

        xlong=glong
        if(glong.gt.180) xlong=glong-360
        if(mode.ne.0) goto 1
c
c UT ---> LT
c
        SLT=UT+xlong/15.
        if((SLT.ge.0.).and.(SLT.le.24.)) goto 2
        if(SLT.gt.24.) goto 3
                SLT=SLT+24.
                ddd=ddd-1
                if(ddd.lt.1.) then
                        iyyy=iyyy-1
                        ddd=365
c
c leap year if evenly divisible by 4 and not by 100, except if evenly
c divisible by 400. Thus 2000 will be a leap year.
c
                        if(iyyy/4*4.eq.iyyy) ddd=366
                        endif
                goto 2
3               SLT=SLT-24.
                ddd=ddd+1
                dddend=365
                if(iyyy/4*4.eq.iyyy) dddend=366
                if(ddd.gt.dddend) then
                        iyyy=iyyy+1
                        ddd=1
                        endif
                goto 2
c
c LT ---> UT
c
1       UT=SLT-xlong/15.
        if((UT.ge.0.).and.(UT.le.24.)) goto 2
        if(UT.gt.24.) goto 5
                UT=UT+24.
                ddd=ddd-1
                if(ddd.lt.1.) then
                        iyyy=iyyy-1
                        ddd=365
                        if(iyyy/4*4.eq.iyyy) ddd=366
                        endif
                goto 2
5               UT=UT-24.
                ddd=ddd+1
                dddend=365
                if(iyyy/4*4.eq.iyyy) dddend=366
                if(ddd.gt.dddend) then
                        iyyy=iyyy+1
                        ddd=1
                        endif
2       return
        end
C
C
C *********************************************************************
C ************************ EPSTEIN FUNCTIONS **************************
C *********************************************************************
C REF:  H. G. BOOKER, J. ATMOS. TERR. PHYS. 39, 619-623, 1977
C       K. RAWER, ADV. SPACE RES. 4, #1, 11-15, 1984
C *********************************************************************
C
C
        REAL FUNCTION  RLAY ( X, XM, SC, HX )
C -------------------------------------------------------- RAWER  LAYER
        Y1  = EPTR ( X , SC, HX )
        Y1M = EPTR ( XM, SC, HX )
        Y2M = EPST ( XM, SC, HX )
        RLAY = Y1 - Y1M - ( X - XM ) * Y2M / SC
        RETURN
        END
C
C
        REAL FUNCTION D1LAY ( X, XM, SC, HX )
C ------------------------------------------------------------ dLAY/dX
        D1LAY = ( EPST(X,SC,HX) - EPST(XM,SC,HX) ) /  SC
        RETURN
        END
C
C
        REAL FUNCTION D2LAY ( X, XM, SC, HX )
C ---------------------------------------------------------- d2LAY/dX2
        D2LAY = EPLA(X,SC,HX) /  (SC * SC)
        RETURN
        END
C
C
        REAL FUNCTION EPTR ( X, SC, HX )
C --------------------------------------------------------- TRANSITION
        COMMON/ARGEXP/ARGMAX
        D1 = ( X - HX ) / SC
        IF (ABS(D1).LT.ARGMAX) GOTO 1
        IF (D1.GT.0.0) THEN
          EPTR = D1
        ELSE
          EPTR = 0.0
        ENDIF
        RETURN
1       EPTR = ALOG ( 1. + EXP( D1 ))
        RETURN
        END
C
C
        REAL FUNCTION EPST ( X, SC, HX )
C -------------------------------------------------------------- STEP
        COMMON/ARGEXP/ARGMAX
        D1 = ( X - HX ) / SC
        IF (ABS(D1).LT.ARGMAX) GOTO 1
        IF (D1.GT.0.0) THEN
          EPST = 1.
        ELSE
          EPST = 0.
        ENDIF
        RETURN
1       EPST = 1. / ( 1. + EXP( -D1 ))
        RETURN
        END
C
C
        REAL FUNCTION EPSTEP ( Y2, Y1, SC, HX, X)
C---------------------------------------------- STEP FROM Y1 TO Y2      
        EPSTEP = Y1 + ( Y2 - Y1 ) * EPST ( X, SC, HX)
        RETURN
        END
C
C
        REAL FUNCTION EPLA ( X, SC, HX )
C ------------------------------------------------------------ PEAK 
        COMMON/ARGEXP/ARGMAX
        D1 = ( X - HX ) / SC
        IF (ABS(D1).LT.ARGMAX) GOTO 1
                EPLA = 0
                RETURN  
1       D0 = EXP ( D1 )
        D2 = 1. + D0
        EPLA = D0 / ( D2 * D2 )
        RETURN
        END
c
c
        FUNCTION XE2TO5(H,HMF2,NL,HX,SC,AMP)
C----------------------------------------------------------------------
C NORMALIZED ELECTRON DENSITY (N/NMF2) FOR THE MIDDLE IONOSPHERE FROM 
C HME TO HMF2 USING LAY-FUNCTIONS.
C----------------------------------------------------------------------
        DIMENSION       HX(NL),SC(NL),AMP(NL)
        SUM = 1.0
        DO 1 I=1,NL
           YLAY = AMP(I) * RLAY( H, HMF2, SC(I), HX(I) )
           zlay=10.**ylay
1          sum=sum*zlay
        XE2TO5 = sum
        RETURN
        END
C
C
        REAL FUNCTION XEN(H,HMF2,XNMF2,HME,NL,HX,SC,AMP)
C----------------------------------------------------------------------
C ELECTRON DENSITY WITH NEW MIDDLE IONOSPHERE
C----------------------------------------------------------------------
        DIMENSION       HX(NL),SC(NL),AMP(NL)
C
        IF(H.LT.HMF2) GOTO 100
                XEN = XE1(H)
                RETURN
100     IF(H.LT.HME) GOTO 200
                XEN = XNMF2 * XE2TO5(H,HMF2,NL,HX,SC,AMP)
                RETURN
200     XEN = XE6(H)
        RETURN
        END
C
C
        SUBROUTINE ROGUL(IDAY,XHI,SX,GRO)
C --------------------------------------------------------------------- 
C   CALCULATES RATIO H0.5/HMF2 FOR HALF-DENSITY POINT (NE(H0.5)=0.5*
C   NMF2) T. GULYAEVA, ADVANCES IN SPACE RESEARCH 7, #6, 39-48, 1987.
C
C       INPUT:  IDAY    DAY OF YEAR
C               XHI     SOLAR ZENITH ANGLE [DEGREE]
C       
C       OUTPUT: GRO     RATIO OF HALF DENSITY HEIGHT TO F PEAK HEIGHT
C               SX      SMOOTHLY VARYING SEASON PARAMTER (SX=1 FOR 
C                       DAY=1; SX=3 FOR DAY=180; SX=2 FOR EQUINOX)
C ---------------------------------------------------------------------
C
        common  /const1/humr,dumr
        SX = 2. - COS ( IDAY * dumr )
        XS = ( XHI - 20. * SX) / 15.
        GRO = 0.8 - 0.2 / ( 1. + EXP(XS) )
c same as gro=0.6+0.2/(1+exp(-xs))
        RETURN
        END
C
C
        SUBROUTINE LNGLSN ( N, A, B, AUS)
C --------------------------------------------------------------------
C SOLVES QUADRATIC SYSTEM OF LINEAR EQUATIONS:
C
C       INPUT:  N       NUMBER OF EQUATIONS (= NUMBER OF UNKNOWNS)
C               A(N,N)  MATRIX (LEFT SIDE OF SYSTEM OF EQUATIONS)
C               B(N)    VECTOR (RIGHT SIDE OF SYSTEM)
C
C       OUTPUT: AUS     =.TRUE.   NO SOLUTION FOUND
C                       =.FALSE.  SOLUTION IS IN  A(N,J) FOR J=1,N
C --------------------------------------------------------------------
C
        DIMENSION       A(5,5), B(5), AZV(10)
        LOGICAL         AUS
C
        NN = N - 1
        AUS = .FALSE.
        DO 1 K=1,N-1
                IMAX = K
                L    = K
                IZG  = 0
                AMAX  = ABS( A(K,K) )
110             L = L + 1
                IF (L.GT.N) GOTO 111
                HSP = ABS( A(L,K) )
                IF (HSP.LT.1.E-8) IZG = IZG + 1
                IF (HSP.LE.AMAX) GOTO 110
111             IF (ABS(AMAX).GE.1.E-10) GOTO 133
                        AUS = .TRUE.
                        RETURN
133             IF (IMAX.EQ.K) GOTO 112
                DO 2 L=K,N
                        AZV(L+1)  = A(IMAX,L)
                        A(IMAX,L) = A(K,L)
2                       A(K,L)    = AZV(L+1)
                AZV(1)  = B(IMAX)
                B(IMAX) = B(K)
                B(K)    = AZV(1)
112             IF (IZG.EQ.(N-K)) GOTO 1
                AMAX = 1. / A(K,K)
                AZV(1) = B(K) * AMAX
                DO 3 M=K+1,N
3                       AZV(M+1) = A(K,M) * AMAX
                DO 4 L=K+1,N
                        AMAX = A(L,K)
                        IF (ABS(AMAX).LT.1.E-8) GOTO 4
                        A(L,K) = 0.0
                        B(L) = B(L) - AZV(1) * AMAX
                        DO 5 M=K+1,N
5                               A(L,M) = A(L,M) - AMAX * AZV(M+1)
4               CONTINUE
1       CONTINUE
        DO 6 K=N,1,-1
                AMAX = 0.0
                IF (K.LT.N) THEN
                        DO 7 L=K+1,N
7                               AMAX = AMAX + A(K,L) * A(N,L)
                        ENDIF
                IF (ABS(A(K,K)).LT.1.E-6) THEN
                        A(N,K) = 0.0
                ELSE
                        A(N,K) = ( B(K) - AMAX ) / A(K,K)
                ENDIF
6       CONTINUE
        RETURN
        END
C
C
        SUBROUTINE LSKNM ( N, M, M0, M1, HM, SC, HX, W,X,Y,VAR,SING)
C --------------------------------------------------------------------
C   DETERMINES LAY-FUNCTIONS AMPLITUDES FOR A NUMBER OF CONSTRAINTS:
C
C       INPUT:  N       NUMBER OF AMPLITUDES ( LAY-FUNCTIONS)
C               M       NUMBER OF CONSTRAINTS
C               M0      NUMBER OF POINT CONSTRAINTS
C               M1      NUMBER OF FIRST DERIVATIVE CONSTRAINTS
C               HM      F PEAK ALTITUDE  [KM]
C               SC(N)   SCALE PARAMETERS FOR LAY-FUNCTIONS  [KM]
C               HX(N)   HEIGHT PARAMETERS FOR LAY-FUNCTIONS  [KM]
C               W(M)    WEIGHT OF CONSTRAINTS
C               X(M)    ALTITUDES FOR CONSTRAINTS  [KM]
C               Y(M)    LOG(DENSITY/NMF2) FOR CONSTRAINTS
C
C       OUTPUT: VAR(M)  AMPLITUDES
C               SING    =.TRUE.   NO SOLUTION
C ---------------------------------------------------------------------
C
        LOGICAL         SING
        DIMENSION       VAR(N), HX(N), SC(N), W(M), X(M), Y(M),
     &                  BLI(5), ALI(5,5), XLI(5,10)
C
        M01=M0+M1
        SCM=0
        DO 1 J=1,5
                BLI(J) = 0.
                DO 1 I=1,5
1                       ALI(J,I) = 0. 
        DO 2 I=1,N
                DO 3 K=1,M0
3                       XLI(I,K) = RLAY( X(K), HM, SC(I), HX(I) )
                DO 4 K=M0+1,M01
4                       XLI(I,K) = D1LAY( X(K), HM, SC(I), HX(I) )
                DO 5 K=M01+1,M
5                       XLI(I,K) = D2LAY( X(K), HM, SC(I), HX(I) )
2       CONTINUE
                DO 7 J=1,N
                DO 6 K=1,M
                        BLI(J) = BLI(J) + W(K) * Y(K) * XLI(J,K)
                        DO 6 I=1,N
6                               ALI(J,I) = ALI(J,I) + W(K) * XLI(I,K) 
     &                                  * XLI(J,K)
7       CONTINUE
        CALL LNGLSN( N, ALI, BLI, SING )
        IF (.NOT.SING) THEN
                DO 8 I=1,N
8                       VAR(I) = ALI(N,I)
                ENDIF
        RETURN
        END
C
C
        SUBROUTINE INILAY(NIGHT,F1REG,XNMF2,XNMF1,XNME,VNE,HMF2,HMF1, 
     &                          HME,HV1,HV2,HHALF,HXL,SCL,AMP,IQUAL)
C-------------------------------------------------------------------
C CALCULATES AMPLITUDES FOR LAY FUNCTIONS
C D. BILITZA, DECEMBER 1988
C
C INPUT:        NIGHT   LOGICAL VARIABLE FOR DAY/NIGHT DISTINCTION
C               F1REG   LOGICAL VARIABLE FOR F1 OCCURRENCE
C               XNMF2   F2 PEAK ELECTRON DENSITY [M-3]
C               XNMF1   F1 PEAK ELECTRON DENSITY [M-3]
C               XNME    E  PEAK ELECTRON DENSITY [M-3]
C               VNE     ELECTRON DENSITY AT VALLEY BASE [M-3]
C               HMF2    F2 PEAK ALTITUDE [KM]
C               HMF1    F1 PEAK ALTITUDE [KM]
C               HME     E  PEAK ALTITUDE [KM]
C               HV1     ALTITUDE OF VALLEY TOP [KM]
C               HV2     ALTITUDE OF VALLEY BASE [KM]
C               HHALF   ALTITUDE OF HALF-F2-PEAK-DENSITY [KM]
C
C OUTPUT:       HXL(4)  HEIGHT PARAMETERS FOR LAY FUNCTIONS [KM] 
C               SCL(4)  SCALE PARAMETERS FOR LAY FUNCTIONS [KM]
C               AMP(4)  AMPLITUDES FOR LAY FUNCTIONS
C               IQUAL   =0 ok, =1 ok using second choice for HXL(1)
C                       =2 NO SOLUTION
C---------------------------------------------------------------  
        DIMENSION       XX(8),YY(8),WW(8),AMP(4),HXL(4),SCL(4)
        LOGICAL         SSIN,NIGHT,F1REG
c
c constants --------------------------------------------------------
                NUMLAY=4
                NC1 = 2
                ALG102=ALOG10(2.)
c
c constraints: xx == height     yy == log(Ne/NmF2)    ww == weights
c -----------------------------------------------------------------
                ALOGF = ALOG10(XNMF2)
                ALOGEF = ALOG10(XNME) - ALOGF
                XHALF=XNMF2/2.
                XX(1) = HHALF
                XX(2) = HV1
                XX(3) = HV2
                XX(4) = HME
                XX(5) = HME - ( HV2 - HME )
                YY(1) = -ALG102
                YY(2) = ALOGEF
                YY(3) = ALOG10(VNE) - ALOGF
                YY(4) = ALOGEF
                YY(5) = YY(3)
                YY(7) = 0.0
                WW(2) = 1.
                WW(3) = 2.
                WW(4) = 5.
c
c geometric paramters for LAY -------------------------------------
c difference to earlier version:  HXL(3) = HV2 + SCL(3)
c
                SCL0 = 0.7 * ( 0.216 * ( HMF2 - HHALF ) + 56.8 )
                SCL(1) = 0.8 * SCL0
                SCL(2) = 10.
                SCL(3) = 9.
                SCL(4) = 6.
                HXL(3) = HV2
                HFFF=HHALF
                XFFF=XHALF
c
C DAY CONDITION--------------------------------------------------
c earlier tested:       HXL(2) = HMF1 + SCL(2)
c 
            IF(NIGHT) GOTO 7711
                NUMCON = 8
                HXL(1) = 0.9 * HMF2
                  HXL1T  = HHALF
                HXL(2) = HMF1
                HXL(4) = HME - SCL(4)
                XX(6) = HMF1
                XX(7) = HV2
                XX(8) = HME
                YY(8) = 0.0
                WW(5) = 1.
                WW(7) = 50.
                WW(8) = 500.
c without F-region ----------------------------------------------
                IF(F1REG) GOTO 100
                        HXL(2)=(HMF2+HHALF)/2.
                        YY(6) = 0.
                        WW(6) = 0.
                        WW(1) = 1.
                        GOTO 7722
c with F-region --------------------------------------------
100             YY(6) = ALOG10(XNMF1) - ALOGF
                WW(6) = 3.
                IF((XNMF1-XHALF)*(HMF1-HHALF).LT.0.0) THEN
                  WW(1)=0.5
                ELSE
                  ZET = YY(1) - YY(6)
                  WW(1) = EPST( ZET, 0.1, 0.15)
                ENDIF
                IF(HHALF.GT.HMF1) THEN
                  HFFF=HMF1
                  XFFF=XNMF1
                ELSE
                  HFFF=HHALF
                  XFFF=XHALF
                ENDIF
                GOTO 7722
c
C NIGHT CONDITION---------------------------------------------------
c different HXL,SCL values were tested including: 
c       SCL(1) = HMF2 * 0.15 - 27.1     HXL(2) = 200.   
c       HXL(2) = HMF1 + SCL(2)          HXL(3) = 140.
c       SCL(3) = 5.                     HXL(4) = HME + SCL(4)
c       HXL(4) = 105.                   
c
7711            NUMCON = 7
                HXL(1) = HHALF
                  HXL1T  = 0.4 * HMF2 + 30.
                HXL(2) = ( HMF2 + HV1 ) / 2.
                HXL(4) = HME
                XX(6) = HV2
                XX(7) = HME
                YY(6) = 0.0
                WW(1) = 1.
                WW(3) = 3.
                WW(5) = 0.5
                WW(6) = 50.
                WW(7) = 500.
                HFFF=HHALF
                XFFF=XHALF
c
C are valley-top and bottomside point compatible ? -------------
C
7722    IF((HV1-HFFF)*(XNME-XFFF).LT.0.0) WW(2)=0.5
        IF(HV1.LE.HV2+5.0) WW(2)=0.5
c
C DETERMINE AMPLITUDES-----------------------------------------
C
            NC0=NUMCON-NC1
            IQUAL=0
2299        CALL LSKNM(NUMLAY,NUMCON,NC0,NC1,HMF2,SCL,HXL,WW,XX,YY,
     &          AMP,SSIN)
                IF(IQUAL.gt.0) GOTO 1937
            IF((ABS(AMP(1)).GT.10.0).OR.(SSIN)) THEN
                IQUAL=1
                HXL(1)=HXL1T
                GOTO 2299
                ENDIF
1937        IF(SSIN) IQUAL=2
            RETURN
            END
c
c
           subroutine tcon(yr,mm,day,idn,rz,ig,rsn,nmonth)
c----------------------------------------------------------------
c input:        yr,mm,day       year(yyyy),month(mm),day(dd)
c               idn             day of year(ddd)
c output:       rz(3)           12-month-smoothed solar sunspot number
c               ig(3)           12-month-smoothed IG index
c               rsn             interpolation parameter
c               nmonth          previous or following month depending
c                               on day
c
c rz(1), ig(1) contain the indices for the month mm and rz(2), ig(2)
c for the previous month (if day less than 15) or for the following
c month (otherwise). These indices are for the mid of the month. The
c indices for the given day are obtained by linear interpolation and
c are stored in rz(3) and ig(3).
c
c the indices are obtained from the indices file ig_rz.dat that is 
c read in subroutine initialize and stored in COMMON/indices/
c----------------------------------------------------------------

           include 'common.inc'

           integer      yr, mm, day, iflag, iyst, iyend,iymst
           integer      imst,iymend
c           real         ionoindx(722),indrz(722)
           real         ionoindx(MAXRECORDLEN_IGRZ)
           real         indrz(MAXRECORDLEN_IGRZ)
           real         ig(3),rz(3)

           common /iounit/konsol

           data iflag/0/
           save         ionoindx,indrz,iflag,iyst,iymst,
     &                  iymend,imst
c
c Rz12 and IG are determined from the file IG_RZ.DAT which has the
c following structure: 
c day, month, year of the last update of this file,
c start month, start year, end month, end year,
c the 12 IG indices (13-months running mean) for the first year, 
c the 12 IG indices for the second year and so on until the end year,
c the 12 Rz indices (13-months running mean) for the first year,
c the 12 Rz indices for the second year and so on until the end year.
c The inteporlation procedure also requires the IG and Rz values for
c the month preceeding the start month and the IG and Rz values for the
c month following the end month. These values are also included in 
c IG_RZ.
c 
c A negative Rz index means that the given index is the 13-months-
C running mean of the solar radio flux (F10.7). The close correlation 
C between (Rz)12 and (F10.7)12 is used to derive the (Rz)12 indices.
c
c An IG index of -111 indicates that no IG values are available for the
c time period. In this case a correlation function between (IG)12 and 
C (Rz)12 is used to obtain (IG)12.
c
c The computation of the 13-month-running mean for month M requires the
c indices for the six months preceeding M and the six months following 
C M (month: M-6, ..., M+6). To calculate the current running mean one 
C therefore requires predictions of the indix for the next six months. 
C Starting from six months before the UPDATE DATE (listed at the top of 
c the file) and onward the indices are therefore based on indices 
c predictions.
c
        if(iflag.eq.0) then
      
c Read the update date, the start date and the end date (mm,yyyy), and
c get number of data points to read.
          iupd = iupd_igrz
          iupm = iupm_igrz
          iupy = iupy_igrz
          imst = imst_igrz
          iyst = iyst_igrz
          imend = imend_igrz
          iyend = iyend_igrz

          iymst=iyst*100+imst
          iymend=iyend*100+imend
c inum_vals= 12-imst+1+(iyend-iyst-1)*12 +imend + 2
c            1st year \ full years       \last y\ before & after
          inum_vals= 3-imst+(iyend-iyst)*12 +imend
c Read all the ionoindx and indrz values
          do i = 1, inum_vals
              ionoindx(i) = ionoindx_igrz(i)
          end do
          do i = 1, inum_vals
              indrz(i) = indrz_igrz(i)
          end do
          do 1 jj=1,inum_vals
                rrr=indrz(jj)
                if(rrr.lt.0.0) then
                        covr=abs(rrr)
                        rrr=33.52*sqrt(covr+85.12)-408.99
                        if(rrr.lt.0.0) rrr=0.0
                        indrz(jj)=rrr
                        endif
                if(ionoindx(jj).gt.-90.) goto 1
                  zi=-12.349154+(1.4683266-2.67690893e-03*rrr)*rrr
                  if(zi.gt.274.0) zi=274.0
                  ionoindx(jj)=zi
1               continue
          iflag = 1
        endif

        iytmp=yr*100+mm
        if (iytmp .lt. iymst .or. iytmp .gt. iymend) then
c               if(konsol.gt.1) write(konsol,8000) iytmp,iymst,
c     &                                            iymend
8000          format(1x,I10,'** OUT OF RANGE **'/,5x,
     &  'The file IG_RZ.DAT which contains the indices Rz12',
     &  ' and IG12'/5x,'currently only covers the time period',
     &  ' (yymm) : ',I6,'-',I6)
               nmonth=-1
               return
               endif

c       num=12-imst+1+(yr-iyst-1)*12+mm+1
        num=2-imst+(yr-iyst)*12+mm

        rz(1)=indrz(num)
        ig(1)=ionoindx(num)
        midm=15
        if(mm.eq.2) midm=14
        call MODA(0,yr,mm,midm,idd1,nrdaym)
        if(day.lt.midm) goto 1926
                imm2=mm+1
                if(imm2.gt.12) then
                        imm2=1
                        iyy2=yr+1
                        idd2=380
c               if((yr/4*4.eq.yr).and.(yr/100*100.ne.yr)) idd2=381
                        if(yr/4*4.eq.yr) idd2=381

                        if(yr/4*4.eq.yr) idd2=381
                else
                        iyy2=yr
                        midm=15
                        if(imm2.eq.2) midm=14
                        call MODA(0,iyy2,imm2,midm,IDD2,nrdaym)
                endif
                rz(2)=indrz(num+1)
                ig(2)=ionoindx(num+1)
                rsn=(idn-idd1)*1./(idd2-idd1)
                rz(3)=rz(1)+(rz(2)-rz(1))*rsn
                ig(3)=ig(1)+(ig(2)-ig(1))*rsn
                goto 1927
1926            imm2=mm-1
                if(imm2.lt.1) then
                        imm2=12
                        idd2=-16
                        iyy2=yr-1
                else
                        iyy2=yr
                        midm=15
                        if(imm2.eq.2) midm=14
                        call MODA(0,iyy2,imm2,midm,IDD2,nrdaym)
                endif
                rz(2)=indrz(num-1)
                ig(2)=ionoindx(num-1)
                rsn=(idn-idd2)*1./(idd1-idd2)
                rz(3)=rz(2)+(rz(1)-rz(2))*rsn
                ig(3)=ig(2)+(ig(1)-ig(2))*rsn

1927    nmonth=imm2
            return
            end
c
c
        subroutine LSTID(FI,ICEZ,R,AE,TM,SAX,SUX,TS70,DF0F2,DHF2)
C*****************************************************************

C   COMPUTER PROGRAM FOR UPDATING FOF2 AND HMF2 FOR EFFECTS OF 
C   THE LARGE SCALE SUBSTORM.
C
C   P.V.Kishcha, V.M.Shashunkina, E.E.Goncharova, Modelling of the
C   ionospheric effects of isolated and consecutive substorms on 
C   the basis of routine magnetic data, Geomagn. and Aeronomy v.32,
C   N.3, 172-175, 1992.
C
C   P.V.Kishcha et al. Updating the IRI ionospheric model for    
C   effects of substorms, Adv. Space Res.(in press) 1992.
C
C   Address: Dr. Pavel V. Kishcha, 
C            Institute of Terrestrial Magnetism,Ionosphere and Radio
C            Wave Propagation, Russian Academy of Sciences, 
C            142092, Troitsk, Moscow Region, Russia
C
C***       INPUT PARAMETERS:
C       FI ------ GEOMAGNETIC LATITUDE,
C       ICEZ ---- INDEX OF SEASON(1-WINTER AND EQUINOX,2-SUMMER),
C       R ------- ZURICH SUNSPOT NUMBER,
C       AE ------ MAXIMUM AE-INDEX REACHED DURING SUBSTORM,
C       TM ------ LOCAL TIME,
C       SAX,SUX - TIME OF SUNSET AND SUNRISE,
C       TS70 ---- ONSET TIME (LT) OF SUBSTORMS ONSET 
C                        STARTING ON FI=70 DEGR.
C***      OUTPUT PARAMETERS:
C       DF0F2,DHF2- CORRECTIONS TO foF2 AND hmF2 FROM IRI OR 
C                   OBSERVATIONAL MEDIAN  OF THOSE VALUES.
C*****************************************************************


        INTEGER ICEZ
        REAL A(7,2,3,2),B(7,2,3,2),C(7,2,3,2),D(7,2,3,2),A1(7,2,2),
     *B1(7,2,2),Y1(84),Y2(84),Y3(84),Y4(84),Y5(28),Y6(28)
        DATA Y1/
     *150.,250.,207.8,140.7,158.3,87.2,158.,
     *150.,250.,207.8,140.7,158.3,87.2,158.,
     *115.,115.,183.5,144.2,161.4,151.9,272.4,
     *115.,115.,183.5,144.2,161.4,151.9,272.4,
     *64.,320.,170.6,122.3,139.,79.6,180.6,
     *64.,320.,170.6,122.3,139.,79.6,180.6,
     *72.,84.,381.9,20.1,75.1,151.2,349.5,
     *120.,252.,311.2,241.,187.4,230.1,168.7,
     *245.,220.,294.7,181.2,135.5,237.7,322.,
     *170.,110.,150.2,136.3,137.4,177.,114.,
     *170.,314.,337.8,155.5,157.4,196.7,161.8,
     *100.,177.,159.8,165.6,137.5,132.2,94.3/
        DATA Y2/
     *2.5,2.0,1.57,2.02,2.12,1.46,2.46,
     *2.5,2.0,1.57,2.02,2.12,1.46,2.46,
     *2.3,1.6,1.68,1.65,2.09,2.25,2.82,
     *2.3,1.6,1.68,1.65,2.09,2.25,2.82,
     *0.8,2.0,1.41,1.57,1.51,1.46,2.2,
     *0.8,2.0,1.41,1.57,1.51,1.46,2.2,
     *3.7,1.8,3.21,3.31,2.61,2.82,2.34,
     *2.8,3.2,3.32,3.33,2.96,3.43,2.44,
     *3.5,2.8,2.37,2.79,2.26,3.4,2.28,
     *3.9,2.,2.22,1.98,2.33,3.07,1.56,
     *3.7,3.,3.3,2.99,3.57,2.98,3.02,
     *2.6,2.8,1.66,2.04,1.91,1.49,0.43/
        DATA Y3/
     *-1.8,-1.9,-1.42,-1.51,-1.53,-1.05,-1.66,
     *-1.8,-1.9,-1.42,-1.51,-1.53,-1.05,-1.66,
     *-1.5,-1.3,-1.46,-1.39,-1.53,-1.59,-1.9,
     *-1.5,-1.3,-1.46,-1.39,-1.53,-1.59,-1.9,
     *-0.7,-2.,-1.41,-1.09,-1.22,-0.84,-1.32,
     *-0.7,-2.,-1.41,-1.09,-1.22,-0.84,-1.32,
     *-1.7,-1.0,-2.08,-1.80,-1.35,-1.55,-1.79,
     *-1.5,-2.,-2.08,-2.16,-1.86,-2.19,-1.70,
     *-2.2,-1.7,-1.57,-1.62,-1.19,-1.89,-1.47,
     *-1.9,-1.5,-1.26,-1.23,-1.52,-1.89,-1.02,
     *-1.7,-1.7,-1.76,-1.43,-1.66,-1.54,-1.24,
     *-1.1,-1.5,-1.09,-1.23,-1.11,-1.14,-0.4/
        DATA Y4/
     *-2.,-5.,-5.,0.,0.,0.,2.,
     *-2.,-5.,-5.,0.,0.,0.,2.,
     *-5.,-5.,6.,0.,1.,5.,2.,
     *-5.,-5.,6.,0.,1.,5.,2.,
     *0.,-7.,-3.,-6.,2.,2.,3.,
     *0.,-7.,-3.,-6.,2.,2.,3.,
     *-5.,-1.,-11.,-6.,0.,-5.,-6.,
     *-5.,-10.,1.,4.,-6.,-2.,1.,
     *2.,-13.,-10.,0.,-8.,10.,-16.,
     *0.,-3.,-7.,-2.,-2.,4.,2.,
     *-11.,-12.,-13.,0.,0.,7.,0.,
     *-8.,6.,-1.,-5.,-7.,4.,-4./
        DATA Y5/
     *0.,0.,-0.1,-0.19,-0.19,-0.25,-0.06,
     *0.,0.,-0.31,-0.28,-0.27,-0.06,0.02,
     *0.,0.,0.18,-0.07,-0.2,-0.1,0.3,
     *0.,0.,-0.24,-0.5,-0.4,-0.27,-0.48/
        DATA Y6/
     *0.,0.,-0.00035,-0.00028,-0.00033,-0.00023,-0.0007,
     *0.,0.,-0.0003,-0.00025,-0.0003,-0.0006,-0.00073,
     *0.,0.,-0.0011,-0.0006,-0.0003,-0.0005,-0.0015,
     *0.,0.,-0.0008,-0.003,-0.0002,-0.0005,-0.0003/

        INN=0
        IF(TS70.GT.12. .AND. TM.LT.SAX)INN=1
        IF(FI.LT.0.)FI=ABS(FI)

        N=0
        DO 001 M=1,2
        DO 001 K=1,3
        DO 001 J=1,2
        DO 001 I=1,7
        N=N+1
        A(I,J,K,M)=Y1(N)
        B(I,J,K,M)=Y2(N)
        C(I,J,K,M)=Y3(N)
 001    D(I,J,K,M)=Y4(N)
        N1=0
        DO 002 M=1,2
        DO 002 J=1,2
        DO 002 I=1,7
        N1=N1+1
        A1(I,J,M)=Y5(N1)
 002    B1(I,J,M)=Y6(N1)
        IF(FI.GT.65..OR.AE.LT.500.)THEN
c        WRITE(*,*)'LSTID are for AE>500. and ABS(FI)<65.'
        GOTO 004
        ENDIF
        TS=TS70+(-1.5571*FI+109.)/60.
        IF(TS.LT.SUX.AND.TS.GT.SAX)THEN
c        WRITE(*,*)' LSTID are only at night'
        GOTO 004
        ENDIF
        IF(INN.EQ.1)TM=TM+24.
        
        IF(TS.GE.TM.OR.TS.LT.TM-5.)THEN
C        WRITE(*,*)'LSTID are onli if  TM-5.<TS<TM ;Here TS=',TS,
C     &     'TM=',TM     
        GOTO 004
        ENDIF
        DO 007 I=1,7
        IF(FI.GE.-5.+10.*(I-1) .AND. FI.LT.5.+10.*(I-1))GOTO 008
 007    CONTINUE
 008    J=ICEZ
        IF(AE.GE.500. .AND. AE.LE.755.)K=1
        IF(AE.GT.755. .AND. AE.LT.1000.)K=2
        IF(AE.GE.1000.)K=3
        M=-1
        IF(R.LE.20.)M=1
        IF(R.GE.120.)M=2
        T=TM-TS
        IF(M.LT.0)GOTO 003
C        WRITE(*,*)'A1=',A1(I,J,M),' B1=',B1(I,J,M)
C        WRITE(*,*)'A=',A(I,J,K,M),' B=',B(I,J,K,M),' C=',C(I,J,K,M),
C     *'D=',D(I,J,K,M)
        DF0F2=A1(I,J,M)+B1(I,J,M)*AE
        DHF2=A(I,J,K,M)*(T**B(I,J,K,M))*EXP(C(I,J,K,M)*T)+D(I,J,K,M)
        GOTO 005
 003    DF1=A1(I,J,1)+B1(I,J,1)*AE
        DF2=A1(I,J,2)+B1(I,J,2)*AE
        DF0F2=DF1+(DF2-DF1)*(R-20.)/100.
        DH1=A(I,J,K,1)*(T**B(I,J,K,1))*EXP(C(I,J,K,1)*T)+D(I,J,K,1)
        DH2=A(I,J,K,2)*(T**B(I,J,K,2))*EXP(C(I,J,K,2)*T)+D(I,J,K,2)
        DHF2=DH1+(DH2-DH1)*(R-20.)/100.
        GOTO 005
 004    DHF2=0.
        DF0F2=0.
 005    CONTINUE
        IF(INN.EQ.1)TM=TM-24.
        RETURN
        END
C
C

        SUBROUTINE APF(IYYYY,IMN,ID,HOUR,IAP)
c--------------------------------------------------------------------
c Finds 3-hourly Ap indices for IRI-storm for given year IYYYY (yyyy), 
c month (IMN), day (ID), and UT (HOUR, decimal hours). The indices are
c stored in IAP(13) providing the 13 3-hourly indices prior to HOUR. 
c The 3-hour UT intervals during the day are: (0-3),)3-6),)6-9),9-12,
c 12-15,15-18,18-21,)21-24(.
c If date is outside the range of the Ap indices file than iap(1)=-5  
c--------------------------------------------------------------------

        include 'common.inc'

        DIMENSION iiap(8),iap(13),lm(12)

        common /iounit/konsol

        DATA LM/31,28,31,30,31,30,31,31,30,31,30,31/

        JY = ints_ap(1,1)
        IYBEG=JY+1900
       
        do i=1,8
              iap(i)=-1
              enddo

        if(iyyyy.lt.IYBEG) goto 21   ! file starts at Jan 1, 1958
                
        is=0
        if(iyyyy.gt.IYBEG) then
            do i=IYBEG,iyyyy-1
                nyd=365
                if(i/4*4.eq.i) nyd=366
                IS=IS+nyd
                enddo
            endif   

        lm(2)=28
        if(iyyyy/4*4.eq.iyyyy) lm(2)=29
        do i=1,IMN-1
              IS=IS+LM(i)
              ENDDO

        IS=IS+ID

        ihour=int(hour/3.)+1
        if(ihour.gt.8) ihour=8

        if(is*8+ihour.lt.13) goto 21   ! at least 13 indices available	
        if (IS < 1 .or. IS > num_records_ap) goto 21
        JY = ints_ap(1,IS)
        JMN = ints_ap(2,IS)
        JD = ints_ap(3,IS)
        do i = 1, 8
            iiap(i) = ints_ap(i+3,IS)
        end do
        F = reals_ap(IS)
        do i9=1,8
        	if(iiap(i9).lt.-2) goto 21
        	enddo
        j1=13-ihour
        do i=1,ihour
           iap(j1+i)=iiap(i)
           enddo
        iss=is-1
        if (ISS < 1 .or. ISS > num_records_ap) goto 21
        JY = ints_ap(1,ISS)
        JMN = ints_ap(2,ISS)
        JD = ints_ap(3,ISS)
        do i = 1, 8
            iiap(i) = ints_ap(i+3,ISS)
        end do
        F = reals_ap(ISS)
        do i9=1,8
        	if(iiap(i9).lt.-2) goto 21
        	enddo
        if(ihour.gt.4) then
              do i=1,j1
                iap(i)=iiap(8-j1+i)
                enddo
        else           
             j2=5-ihour
             do i=1,8
                iap(j2+i)=iiap(i)
                enddo
             iss=is-2
             if (ISS < 1 .or. ISS > num_records_ap) goto 21
             JY = ints_ap(1,ISS)
             JMN = ints_ap(2,ISS)
             JD = ints_ap(3,ISS)
             do i = 1, 8
                 iiap(i) = ints_ap(i+3,ISS)
             end do
             F = reals_ap(ISS)
        	 do i9=1,8
        		if(iiap(i9).lt.-2) goto 21
        		enddo
             do i=1,j2
                iap(i)=iiap(8-j2+i)
                enddo
        endif         
        goto 20
        
c21      if(konsol.gt.1) write(konsol,100)
100     format(1X,'Date is outside range of Ap indices file.',
     &     ' STORM model is turned off.')
21      IAP(1)=-5
      
20    RETURN
      END
C
C
        SUBROUTINE APF_ONLY(IYYYY,IMN,ID,F107D,F107M)
c--------------------------------------------------------------------
c Finds daily and monthly F10.7 index, F107D and F107M, for given year 
c (IYYYY/yyyy), month (IMN/mm), and day (ID/dd) using AP.DAT file on 
c UNIT=13. Is used for vdrift and foeedi.
c If date is outside the range of indices file than F107D=-5  
c--------------------------------------------------------------------

        include 'common.inc'

        DIMENSION iiap(8),lm(12)

        common /iounit/konsol

        DATA LM/31,28,31,30,31,30,31,31,30,31,30,31/

        JY = ints_ap(1,1)
        IYBEG=JY+1900

        if(iyyyy.lt.IYBEG) goto 21   ! AP.DAT starts at Jan 1, 1958

        is=0
        do i=IYBEG,iyyyy-1
            nyd=365
            if(i/4*4.eq.i) nyd=366	! leap year
            IS=IS+nyd
            enddo

        lm(2)=28
        if(iyyyy/4*4.eq.iyyyy) lm(2)=29	  ! leap year
        
        do i=1,IMN-1
              IS=IS+LM(i)
              ENDDO
        
		sum=0.0
        mend=lm(imn)
        do i=1,mend 
        	IS=IS+1
                if (IS < 1 .or. IS > num_records_ap) goto 21
                JY = ints_ap(1,IS)
                JMN = ints_ap(2,IS)
                JD = ints_ap(3,IS)
                do j = 1, 8
                    iiap(j) = ints_ap(j+3,IS)
                end do
                f = reals_ap(IS)
 			if(f.lt.-4.) goto 21
        	if (i.eq.ID) F107D=f
			sum=sum+f
			enddo

        F107M=sum/mend
        
        goto 20

c21      if(konsol.gt.1) write(konsol,100)
100     format(1X,'Date is outside range of F10.7D indices file',
     &    ' (F10.7D = F10.7M = F10.7RM12).')
21      F107D = -111.0
     
20    RETURN
      END

C      
C
C----------------------STORM MODEL --------------------------------
C
      SUBROUTINE CONVER(rga,rgo,rgma)

C     This subroutine converts a geographic latitude and longitude
C     location to a corrected geomagnetic latitude.
C
C     INPUT: 
C       geographic latitude   -90. to +90.
C       geographic longitude  0. to 360. positive east from Greenwich.
C
C     OUTPUT:
C       corrected geomagnetic latitude	-90. to +90.


      DIMENSION CORMAG(20,91)      
      DATA ((CORMAG(i,j),i=1,20),j=1,31)/
     +163.68,163.68,163.68,163.68,163.68,163.68,
     +163.68,163.68,163.68,163.68,163.68,163.68,163.68,163.68,
     +163.68,163.68,163.68,163.68,163.68,163.68,162.60,163.12,
     +163.64,164.18,164.54,164.90,165.16,165.66,166.00,165.86,
     +165.20,164.38,163.66,162.94,162.42,162.00,161.70,161.70,
     +161.80,162.14,161.20,162.18,163.26,164.44,165.62,166.60,
     +167.42,167.80,167.38,166.82,166.00,164.66,163.26,162.16,
     +161.18,160.40,159.94,159.80,159.98,160.44,159.80,161.14,
     +162.70,164.50,166.26,167.90,169.18,169.72,169.36,168.24,
     +166.70,164.80,162.90,161.18,159.74,158.60,157.94,157.80,
     +157.98,158.72,158.40,160.10,162.02,164.28,166.64,169.00,
     +170.80,171.72,171.06,169.46,167.10,164.64,162.18,160.02,
     +158.20,156.80,156.04,155.80,156.16,157.02,157.00,158.96,
     +161.24,163.86,166.72,169.80,172.42,173.72,172.82,170.34,
     +167.30,164.22,161.34,158.74,156.60,155.00,154.08,153.90,
     +154.36,155.36,155.50,157.72,160.36,163.32,166.60,170.20,
     +173.70,175.64,174.18,170.80,167.10,163.56,160.24,157.36,
     +154.96,153.10,152.08,151.92,152.46,153.76,154.10,156.52,
     +159.36,162.52,166.24,170.30,174.62,177.48,175.04,170.82,
     +166.60,162.70,159.02,155.88,153.22,151.20,150.08,149.92,
     +150.64,152.20,152.80,155.32,158.28,161.70,165.58,170.00,
     +174.84,178.46,175.18,170.38,165.80,161.64,157.80,154.38,
     +151.52,149.30,148.18,148.02,148.92,150.60,151.40,154.08,
     +157.18,160.68,164.78,169.40,174.34,177.44,174.28,169.44,
     +164.70,160.34,156.30,152.78,149.72,147.40,146.18,146.04,
     +147.12,149.04,150.10,152.88,156.00,159.58,163.78,168.50,
     +173.28,175.60,172.86,168.14,163.40,158.98,154.88,151.10,
     +147.98,145.50,144.18,144.14,145.40,147.48,148.80,151.68,
     +154.88,158.48,162.68,167.40,171.76,173.60,171.12,166.68,
     +162.00,157.48,153.28,149.50,146.18,143.50,142.18,142.24,
     +143.68,145.98,147.50,150.54,153.68,157.28,161.42,166.10,
     +170.10,171.48,169.22,164.98,160.40,155.88,151.68,147.80,
     +144.34,141.60,140.18,140.26,141.98,144.62,146.30,149.34,
     +152.48,155.98,160.08,164.60,168.34,169.38,167.20,163.18,
     +158.60,154.18,149.98,146.02,142.54,139.70,138.18,138.46,
     +140.26,143.16,145.10,148.14,151.18,154.60,158.68,163.10,
     +166.48,167.28,165.18,161.32,156.90,152.48,148.28,144.32,
     +140.74,137.80,136.22,136.48,138.64,141.76,143.90,146.98,
     +149.98,153.30,157.24,161.40,164.52,165.16,162.86,159.42,
     +155.00,150.68,146.48,142.52,138.94,135.90,134.22,134.68,
     +137.02,140.40,142.70,145.84,148.76,151.92,155.74,159.70,
     +162.52,162.96,160.98,157.42,153.10,148.84,144.68,140.82,
     +137.20,134.00,132.32,132.80,135.42,139.10,141.60,144.74,
     +147.46,150.52,154.20,158.00,160.46,160.76,158.86,155.36,
     +151.20,146.94,142.88,139.02,135.40,132.10,130.32,131.00,
     +133.80,137.74,140.50,143.58,146.24,149.12,152.60,156.20,
     +158.40,158.66,156.76,153.36,149.30,145.04,141.08,137.30,
     +133.60,130.30,128.42,129.12,132.28,136.44,139.30,142.48,
     +144.94,147.64,150.48,154.30,156.34,156.36,154.56,151.26,
     +147.30,143.14,139.20,135.50,131.90,128.40,126.52,127.32,
     +130.76,135.18,138.20,141.28,143.72,146.24,149.26,152.40,
     +154.24,154.16,152.36,149.16,145.30,141.24,137.30,133.70,
     +130.10,126.60,124.62,125.54,129.16,133.92,137.10,140.18,
     +142.42,144.66,147.62,150.50,152.18,151.96,150.16,147.10,
     +143.30,139.24,135.50,131.90,128.36,124.80,122.72,123.74,
     +127.64,132.62,135.90,139.02,141.12,143.18,145.92,148.60,
     +149.98,149.76,148.04,145.00,141.20,137.30,133.60,130.10,
     +126.60,123.00,120.86,121.96,126.12,131.36,134.80,137.88,
     +139.80,141.68,144.08,146.60,147.88,147.56,145.84,142.90,
     +139.20,135.30,131.70,128.28,124.86,121.30,118.96,120.18,
     +124.70,130.16,133.60,136.72,138.48,140.10,142.38,144.60,
     +145.72,145.34,143.64,140.80,137.10,133.30,129.72,126.48,
     +123.10,119.50,117.16,118.48,123.18,128.86,132.40,135.42,
     +137.08,138.50,140.54,142.60,143.52,143.06,141.44,138.70,
     +135.10,131.30,127.82,124.58,121.40,117.70,115.26,116.70,
     +121.66,127.60,131.20,134.22,135.66,136.82,138.70,140.60,
     +141.36,140.86,139.24,136.50,133.00,129.30,125.92,122.78,
     +119.60,116.00,113.40,114.92,120.16,126.30,130.00,132.92,
     +134.24,135.14,136.80,138.60,139.16,138.64,137.12,134.40,
     +130.90,127.20,123.92,120.96,117.90,114.20,111.56,113.12,
     +118.64,124.90,128.70,131.56,132.74,133.44,134.90,136.50,
     +137.00,136.36,134.82,132.30,128.70,125.16,121.94,119.06,
     +116.10,112.50,109.70,111.42,117.14,123.60,127.30,130.16,
     +131.22,131.66,133.00,134.50,134.80,134.14,132.62,130.14,
     +126.60,123.06,119.94,117.16,114.30,110.70,107.80,109.64,
     +115.62,122.24,125.90,128.76,129.62,129.96,131.06,132.40,
     +132.60,131.86,130.42,128.00,124.50,120.96,117.96,115.26,
     +112.54,108.90,105.94,107.86,114.02,120.84/

      DATA ((CORMAG(i,j),i=1,20),j=32,61)/
     +124.05,126.79,
     +127.55,127.83,128.90,130.21,130.41,129.71,128.33,125.96,
     +122.49,118.96,115.97,113.26,110.52,106.89,104.01,106.00,
     +112.21,119.06,122.19,124.82,125.48,125.69,126.73,128.03,
     +128.22,127.55,126.23,123.92,120.47,116.97,113.97,111.26,
     +108.50,104.89,102.08,104.14,110.41,117.29,120.34,122.85,
     +123.40,123.56,124.57,125.84,126.03,125.40,124.14,121.88,
     +118.46,114.97,111.98,109.26,106.48,102.88,100.15,102.28,
     +108.60,115.51,118.49,120.88,121.33,121.42,122.40,123.65,
     +123.84,123.24,122.04,119.83,116.45,112.97,109.98,107.26,
     +104.46,100.87,098.22,100.42,106.79,113.74,116.63,118.91,
     +119.26,119.29,120.24,121.47,121.65,121.09,119.95,117.79,
     +114.43,110.98,107.99,105.26,102.44,098.87,096.29,098.56,
     +104.98,111.96,114.78,116.94,117.19,117.15,118.07,119.28,
     +119.46,118.93,117.86,115.75,112.42,108.98,106.00,103.26,
     +100.42,096.86,094.36,096.70,103.18,110.19,112.93,114.97,
     +115.12,115.02,115.91,117.09,117.27,116.78,115.76,113.71,
     +110.41,106.98,104.00,101.26,098.40,094.85,092.43,094.84,
     +101.37,108.41,111.07,113.00,113.04,112.88,113.74,114.91,
     +115.08,114.62,113.67,111.67,108.39,104.99,102.01,099.26,
     +096.38,092.85,090.51,092.97,099.56,106.64,109.22,111.03,
     +110.97,110.75,111.58,112.72,112.89,112.47,111.57,109.63,
     +106.38,102.99,100.01,097.26,094.36,090.84,088.58,091.11,
     +097.75,104.86,107.37,109.06,108.90,108.61,109.41,110.53,
     +110.70,110.31,109.48,107.59,104.37,100.99,098.02,095.26,
     +092.34,088.83,086.65,089.25,095.95,103.09,105.51,107.09,
     +106.83,106.48,107.25,108.35,108.51,108.16,107.39,105.55,
     +102.35,099.00,096.03,093.26,090.32,086.83,084.72,087.39,
     +094.14,101.31,103.66,105.12,104.76,104.34,105.08,106.16,
     +106.32,106.00,105.29,103.50,100.34,097.00,094.03,091.26,
     +088.30,084.82,082.79,085.53,092.33,099.54,101.81,103.15,
     +102.68,102.21,102.92,103.97,104.13,103.85,103.20,101.46,
     +098.33,095.00,092.04,089.26,086.28,082.81,080.86,083.67,
     +090.52,097.76,099.95,101.18,100.61,100.07,100.75,101.79,
     +101.94,101.69,101.10,099.42,096.31,093.01,090.04,087.26,
     +084.26,080.81,078.93,081.81,088.72,095.99,098.10,099.21,
     +098.54,097.94,098.59,099.60,099.75,099.54,099.01,097.38,
     +094.30,091.01,088.05,085.26,082.24,078.80,077.00,079.95,
     +086.91,094.21,096.25,097.24,096.47,095.81,096.43,097.41,
     +097.56,097.39,096.92,095.34,092.29,089.01,086.06,083.26,
     +080.22,076.79,075.07,078.09,085.10,092.43,094.39,095.27,
     +094.40,093.67,094.26,095.23,095.37,095.23,094.82,093.30,
     +090.27,087.02,084.06,081.26,078.20,074.79,073.14,076.23,
     +083.30,090.66,092.54,093.30,092.32,091.54,092.10,093.04,
     +093.18,093.08,092.73,091.26,088.26,085.02,082.07,079.26,
     +076.18,072.78,071.21,074.37,081.49,088.88,090.69,091.33,
     +090.25,089.40,089.93,090.85,090.99,090.92,090.63,089.21,
     +086.25,083.02,080.07,077.26,074.16,070.77,069.28,072.51,
     +079.68,087.11,088.83,089.36,088.18,087.27,087.77,088.67,
     +088.80,088.77,088.54,087.17,084.23,081.03,078.08,075.26,
     +072.14,068.77,067.35,070.65,077.87,085.33,086.98,087.39,
     +086.11,085.13,085.60,086.48,086.61,086.61,086.45,085.13,
     +082.22,079.03,076.09,073.26,070.12,066.76,065.42,068.79,
     +076.07,083.56,085.13,085.42,084.04,083.00,083.44,084.29,
     +084.42,084.46,084.35,083.09,080.21,077.03,074.09,071.26,
     +068.10,064.75,063.49,066.93,074.26,081.78,083.27,083.45,
     +081.96,080.86,081.27,082.11,082.23,082.30,082.26,081.05,
     +078.19,075.04,072.10,069.26,066.08,062.75,061.57,065.06,
     +072.45,080.01,081.42,081.48,079.89,078.73,079.11,079.92,
     +080.04,080.15,080.16,079.01,076.18,073.04,070.10,067.26,
     +064.06,060.74,059.64,063.20,070.64,078.23,079.57,079.51,
     +077.82,076.59,076.94,077.73,077.85,077.99,078.07,076.97,
     +074.17,071.04,068.11,065.26,062.04,058.73,057.71,061.34,
     +068.84,076.46,077.71,077.54,075.75,074.46,074.78,075.55,
     +075.66,075.84,075.98,074.93,072.15,069.05,066.12,063.26,
     +060.02,056.73,055.78,059.48,067.03,074.68,075.86,075.57,
     +073.68,072.32,072.61,073.36,073.47,073.68,073.88,072.88,
     +070.14,067.05,064.12,061.26,058.00,054.72,053.85,057.62,
     +065.22,072.91,074.01,073.60,071.60,070.19,070.45,071.17,
     +071.28,071.53,071.79,070.84,068.13,065.05,062.13,059.26,
     +055.98,052.71,051.92,055.76,063.41,071.13,072.15,071.63,
     +069.53,068.05,068.28,068.99,069.09,069.37,069.69,068.80,
     +066.11,063.06,060.13,057.26,053.96,050.71,049.99,053.90,
     +061.61,069.36,070.30,069.66,067.46,065.92,066.12,066.80,
     +066.90,067.22,067.60,066.76,064.10,061.06,058.14,055.26,
     +051.94,048.70,048.06,052.04,059.80,067.58/

      DATA ((CORMAG(i,j),i=1,20),j=62,91)/
     +067.70,067.06,
     +065.08,063.72,063.98,064.60,064.80,065.12,065.60,064.86,
     +062.40,059.26,056.24,053.18,049.84,046.60,046.12,050.12,
     +057.52,064.80,064.90,064.42,062.70,061.62,061.78,062.40,
     +062.60,063.04,063.58,063.00,060.60,057.46,054.42,051.18,
     +047.70,044.60,044.22,048.02,055.06,061.92,062.10,061.72,
     +060.32,059.50,059.68,060.20,060.46,060.94,061.58,061.00,
     +058.70,055.66,052.52,049.18,045.60,042.50,042.22,046.00,
     +052.60,058.98,059.20,059.18,058.12,057.32,057.48,058.00,
     +058.30,058.84,059.48,059.04,056.90,053.86,050.62,047.10,
     +043.50,040.50,040.28,043.98,050.22,056.18,056.40,056.64,
     +055.84,055.20,055.38,055.80,056.16,056.84,057.48,057.04,
     +055.10,052.06,048.70,045.10,041.40,038.40,038.28,041.88,
     +047.94,053.44,053.70,054.14,053.56,053.10,053.24,053.70,
     +054.06,054.74,055.38,055.14,053.20,050.26,046.80,043.10,
     +039.34,036.40,036.38,039.96,045.56,050.84,051.10,051.70,
     +051.36,051.00,051.14,051.50,051.96,052.64,053.38,053.08,
     +051.30,048.36,044.90,041.02,037.24,034.40,034.38,037.86,
     +043.28,048.20,048.50,049.26,049.18,048.90,049.04,049.40,
     +049.86,050.64,051.28,051.08,049.40,046.46,042.98,039.02,
     +035.14,032.40,032.48,035.72,041.00,045.70,046.00,046.96,
     +046.98,046.80,046.94,047.30,047.76,048.54,049.28,049.08,
     +047.40,044.56,041.08,037.02,033.14,030.40,030.58,033.84,
     +038.72,043.20,043.50,044.62,044.80,044.80,044.94,045.20,
     +045.76,046.54,047.18,046.98,045.50,042.66,039.08,035.02,
     +031.14,028.40,028.58,031.82,036.52,040.80,041.20,042.32,
     +042.54,042.70,042.84,043.20,043.66,044.44,045.08,044.98,
     +043.50,040.76,037.08,033.04,029.04,026.40,026.68,029.82,
     +034.34,038.40,038.80,040.12,040.60,040.70,040.84,041.10,
     +041.62,042.34,042.98,042.88,041.50,038.76,035.18,031.04,
     +027.14,024.50,024.78,027.70,032.14,036.06,036.50,037.88,
     +038.50,038.68,038.84,039.10,039.56,040.34,040.88,040.82,
     +039.40,036.76,033.18,029.12,025.14,022.50,022.88,025.90,
     +029.96,033.86,034.30,035.68,036.42,036.68,036.84,037.10,
     +037.56,038.24,038.88,038.72,037.40,034.76,031.18,027.12,
     +023.14,020.60,020.98,023.90,027.88,031.66,032.10,033.58,
     +034.32,034.68,034.84,035.10,035.56,036.24,036.78,036.62,
     +035.30,032.72,029.18,025.14,021.24,018.70,019.08,021.90,
     +025.88,029.42,029.90,031.48,032.32,032.68,032.84,033.10,
     +033.56,034.22,034.68,034.42,033.20,030.72,027.28,023.22,
     +019.34,016.80,017.24,020.00,023.78,027.32,027.70,029.38,
     +030.24,030.68,030.94,031.20,031.66,032.22,032.58,032.32,
     +031.10,028.62,025.28,021.32,017.48,015.00,015.38,018.18,
     +021.80,025.22,025.70,027.28,028.24,028.78,029.04,029.30,
     +029.66,030.22,030.50,030.22,029.00,026.62,023.30,019.42,
     +015.64,013.10,013.54,016.28,019.80,023.12,023.60,025.24,
     +026.24,026.78,027.14,027.40,027.76,028.22,028.40,028.12,
     +026.80,024.52,021.30,017.52,013.78,011.30,011.74,014.48,
     +017.90,021.12,021.60,023.24,024.34,024.88,025.24,025.50,
     +025.86,026.22,026.40,025.98,024.70,022.48,019.40,015.72,
     +012.04,009.50,009.94,012.58,016.02,019.12,019.60,021.24,
     +022.34,022.98,023.34,023.70,024.00,024.30,024.40,023.88,
     +022.60,020.48,017.52,014.00,010.34,007.80,008.18,010.88,
     +014.22,017.18,017.60,019.34,020.44,021.16,021.54,021.90,
     +022.16,022.40,022.32,021.78,020.60,018.48,015.62,012.20,
     +008.68,006.00,006.44,009.18,012.42,015.28,015.80,017.44,
     +018.54,019.26,019.74,020.10,020.30,020.50,020.32,019.72,
     +018.50,016.54,013.84,010.68,007.14,004.40,004.74,007.58,
     +010.74,013.48,014.00,015.54,016.74,017.46,017.94,018.30,
     +018.50,018.58,018.32,017.72,016.50,014.64,012.24,009.18,
     +005.84,002.90,003.30,006.16,009.14,011.84,012.30,013.78,
     +014.94,015.66,016.24,016.50,016.70,016.70,016.42,005.78,
     +014.60,012.90,010.66,007.86,004.88,001.60,001.72,004.96,
     +007.84,010.24,010.70,012.14,013.24,013.96,014.44,014.80,
     +014.90,014.88,014.52,013.92,012.80,011.30,009.28,006.94,
     +004.32,001.80,001.94,004.34,006.78,008.94,009.40,010.58,
     +011.64,012.36,012.74,013.10,013.20,013.08,012.72,012.12,
     +011.10,009.86,008.30,006.50,004.60,003.10,003.16,004.50,
     +006.20,007.90,008.40,009.42,010.14,010.76,011.14,011.40,
     +011.40,011.38,011.02,010.46,009.70,008.72,007.64,006.46,
     +005.42,004.60,004.70,005.34,006.24,007.36,007.90,008.46,
     +008.92,009.28,009.54,009.70,009.70,009.68,009.42,009.06,
     +008.60,008.08,007.56,007.02,006.56,006.30,006.30,006.52,
     +006.96,007.38,008.15,008.15,008.15,008.15,008.15,008.15,
     +008.15,008.15,008.15,008.15,008.15,008.15,008.15,008.15,
     +008.15,008.15,008.15,008.15,008.15,008.15/

C     Data Input      
      rlan = rga
      rlo = rgo      
      
C     From "normal" geographic latitude 
C     to angle from South Pole.       
      rla = rlan + 90

      IF (rlo .EQ. 360) THEN
      	rlo = 0
        END IF

C     PROXIMITY

C     coefficients of the latitudinal points		
      LA1 = (INT(rla/2)+1)
      LA2 = LA1 + 1
      if(la2.gt.91) la2=91

C     coefficients of the longitudinal points		
      LO1 = (INT(rlo/18)+1)
corr      LO2 = LO1 + 1
      LO2 = MOD(LO1,20) + 1 

C     Four points of Geomagnetic Coordinates
      gm1 = CORMAG(LO1,LA1)
      gm2 = CORMAG(LO1,LA2) 
      gm3 = CORMAG(LO2,LA1)
      gm4 = CORMAG(LO2,LA2)

C     latitudinal points		
C      X1 = ABS(rla - (INT(rla)))                        
C      X2 = 2. - X1
	  x = (rla/2.0 - (INT(rla/2.0)))

C     longitudinal points		
C      Y1 = ABS(rlo - (INT(rlo)))                        
C      Y2 = 18. - Y1
      y =(rlo/18.0 - (INT(rlo/18.0))) 

C     X AND Y VALUES
C      x = X1 / (X1 + X2)
C      y = Y1 / (Y1 + Y2)

C     INTERPOLATION
      gmla = gm1 * (1 - x) * (1 - y) + gm2 * (1 - y) * (x) + gm3 * (y)
     1 * (1 - x) + gm4 * (x) * (y)

C     OUTPUT OF THE PROGRAM
C     From corrected geomagnetic latitude from North Pole
C     to "normal"  geomagnetic latitude.       
      rgma = 90. - gmla

      END
c
c
      SUBROUTINE STORM(ap,rga,rgo,coor,rgma,ut,doy,cf)
C----------------------------------------------------------------------
C      Fortran code to obtain the foF2 storm-time correction factor at 
C      a given location and time, using the current and the 12 previous
C      ap values as input.
C
C      ap ---> (13 elements integer array). Array with the preceeding
C              13 value of the 3-hourly ap index. The 13th value
C              in the array will contain the ap at the UT of interest,
C              the 12th value will contain the 1st three hourly interval
C              preceeding the time of interest, and so on to the 1st
C              ap value at the earliest time.
C     coor --> (integer). If coor = 2, rga should contain the 
C                         geomagnetic latitude.
C                         If coor = 1, rga should contain the 
C                         geographic latitude.
C     rga ---> (real, -90 to 90) geographic or geomagnetic latitude.
C     rgo ---> (real, 0 to 360, positive east from Greenwich.)
C                           geographic longitude, only used if coor=1.
C     ut  ---> (integer, hours 00 to 23) Universal Time of interest.
C     doy ---> (integer, 1 to 366)Day of the year.
C     cf  ---> (real) The output; the storm-time correction factor used
C              to scale foF2, foF2 * cf.
C
C     This model and computer code was developed by E. Araujo-Pradere,
C     T. Fuller-Rowell and M. Condrescu, SEC, NOAA, Boulder, USA
C     Ref: 
C     T. Fuller-Rowell, E. Araujo-Pradere, and M. Condrescu, An 
C       Empirical Ionospheric Storm-Time Ionospheric Correction Model,
C       Adv. Space Res. 8, 8, 15-24, 2000.
C----------------------------------------------------------------------
      INTEGER ape(39)
      INTEGER ap(13)
      INTEGER ut,doy,dayno,coor,s1,s2,l1,l2
      REAL rgma, rap, rga, rgo, rs, rl

C     DIMENSIONS AND COEFFICIENTS VALUES

      DIMENSION c4(20)
      DIMENSION c3(20)
      DIMENSION c2(20)
      DIMENSION c1(20)
      DIMENSION c0(20)
      DIMENSION fap(36)      
      integer code(8,6)
      
      DATA c4/0.00E+00,0.00E+00,0.00E+00,0.00E+00,0.00E+00,0.00E+00,
     +0.00E+00,0.00E+00,0.00E+00,0.00E+00,0.00E+00,0.00E+00,0.00E+00,
     +0.00E+00,0.00E+00,0.00E+00,0.00E+00,0.00E+00,0.00E+00,0.00E+00/
      
      DATA c3/0.00E+00,0.00E+00,0.00E+00,0.00E+00,0.00E+00,-9.44E-12,
     +0.00E+00,3.04E-12,0.00E+00,9.32E-12,-1.07E-11,0.00E+00,0.00E+00,
     +0.00E+00,1.09E-11,0.00E+00,0.00E+00,0.00E+00,0.00E+00,-1.01E-11/
      
      DATA c2/1.16E-08,0.00E+00,0.00E+00,-1.46E-08,0.00E+00,9.86E-08,
     +2.25E-08,-1.67E-08,-1.62E-08,-9.42E-08,1.17E-07,4.32E-08,3.97E-08,
     +3.13E-08,-8.04E-08,3.91E-08,2.58E-08,3.45E-08,4.76E-08,1.13E-07/
      
      DATA c1/-9.17E-05,-1.37E-05,0.00E+00,7.14E-05,0.00E+00,-3.21E-04,
     +-1.66E-04,-4.10E-05,1.36E-04,2.29E-04,-3.89E-04,-3.08E-04,
     +-2.81E-04,-1.90E-04,4.76E-05,-2.80E-04,-2.07E-04,-2.91E-04,
     +-3.30E-04,-4.04E-04/
      
      DATA c0/1.0136E+00,1.0478E+00,1.00E+00,1.0258E+00,1.00E+00,
     +1.077E+00,1.0543E+00,1.0103E+00,9.9927E-01,9.6876E-01,1.0971E+00,
     +1.0971E+00,1.0777E+00,1.1134E+00,1.0237E+00,1.0703E+00,1.0248E+00,
     +1.0945E+00,1.1622E+00,1.1393E+00/

      DATA fap/0.,0.,0.037037037,0.074074074,0.111111111,0.148148148,
     10.185185185,0.222222222,0.259259259,0.296296296,0.333333333,
     20.37037037,0.407407407,0.444444444,0.481481481,0.518518519,
     30.555555556,0.592592593,0.62962963,0.666666667,0.703703704,
     40.740740741,0.777777778,0.814814815,0.851851852,0.888888889,
     50.925925926,0.962962963,1.,0.66666667,0.33333334,0.,0.333333,
     60.666666,1.,0.7/

      data code/3,4,5,4,3,2,1,2,3,2,1,2,3,4,5,4,8,7,6,7,8,9,10,9,
     *13,12,11,12,13,14,15,14,18,17,16,17,18,19,20,19,18,17,16,17,
     *18,19,20,19/

C      CALLING THE PROGRAM TO CONVERT TO GEOMAGNETIC COORDINATES

       IF (coor .EQ. 1) THEN

           CALL CONVER (rga,rgo,rgma)

       ELSE IF (coor .EQ. 2) THEN
                rgma = rga

       ELSE

c          WRITE (6,*)' '
c          WRITE (6,*)' '
c          WRITE (6,*)'   Wrong Coordinates Selection -------- >>', coor
c          WRITE (6,*)' '
          GOTO 100
       ENDIF

C FROM 3-HOURLY TO HOURLY ap (New, interpolates between the three hourly ap values)

       ape(1)=ap(1)
       ape(2)=ap(1)
       ape(38)=ap(13)
       ape(39)=ap(13)

       DO k = 1,13
          i = (k * 3) - 1
          ape(i) = ap(k)
          END DO

       DO k = 1,12
          i = k * 3
          ape(i) = (ap(k)*2 + ap(k+1))/3.0
          END DO

       DO k = 2,13
          i = (k * 3) - 2
          ape(i) = (ap(k-1) + ap(k)*2)/3.0
          END DO

C     FROM 3-HOURLY TO HOURLY ap (old version without interpolation)
c      i = 1
c      DO 10 k = 1,13
c         DO j = 1,3
c            ape(i) = ap(k)
c            i = i + 1
c            END DO
c10    CONTINUE

C     TO OBTAIN THE INTEGRAL OF ap.
C     INTEGRAL OF ap

      if(ut.eq.24) ut=0
      IF (ut .EQ. 0 .OR. ut .EQ. 3 .OR. ut .EQ. 6 .OR. ut .EQ. 9 .OR.
     1ut .EQ. 12 .OR. ut .EQ. 15 .OR. ut .EQ. 18 .OR. ut .EQ. 21) THEN
          k = 1
      ELSE IF (ut .EQ. 1 .OR. ut .EQ. 4 .OR. ut .EQ. 7 .OR. ut .EQ. 10
     1.OR.ut .EQ. 13 .OR. ut .EQ. 16 .OR. ut .EQ. 19 .OR. ut .EQ. 22)
     2THEN
          k = 2
      ELSE IF (ut .EQ. 2 .OR. ut .EQ. 5 .OR. ut .EQ. 8 .OR. ut .EQ. 11
     1.OR. ut .EQ. 14 .OR. ut .EQ. 17 .OR. ut .EQ. 20 .OR. ut .EQ. 23)
     2THEN
          k = 3

      ELSE

c          WRITE (6,*)' '
c          WRITE (6,*)' '
c          WRITE (6,*)'  Wrong Universal Time value -------- >>', ut
c          WRITE (6,*)' '
          GOTO 100

      END IF

      rap = 0

      DO j = 1,36
      rap = rap + fap(j) * ape(k+j)
      END DO

      if(rap.le.200.)then
      cf=1.0
      goto 100
      end if

      if(doy.gt.366.or.doy.lt.1)then
c          WRITE (6,*)' '
c          WRITE (6,*)' '
c          WRITE (6,*)' '
c          WRITE (6,*)'      Wrong Day of Year value --- >>', doy
c          WRITE (6,*)' '
          GOTO 100
      end if

      if(rgma.gt.90.0.or.rgma.lt.-90.0)then
c          WRITE (6,*)' '
c          WRITE (6,*)' '
c          WRITE (6,*)' '
c          WRITE (6,*)'   Wrong GEOMAGNETIC LATITUDE value --- >>', rgma
c          WRITE (6,*)' '
          GOTO 100
      end if

c      write(6,*)rgma

      dayno=doy
      if(rgma.lt.0.0)then
      dayno=doy+172
      if(dayno.gt.365)dayno=dayno-365
      end if

      if (dayno.ge.82) rs=(dayno-82.)/45.6+1.
      if (dayno.lt.82) rs=(dayno+283.)/45.6+1.
      s1=rs
      facs=rs-s1
      s2=s1+1
      if(s2.eq.9) s2=1
c      write(6,*)s1,s2,rs

      rgma = abs(rgma)

      rl=(rgma+10.)/20.+1
      if(rl.eq.6.0)rl=5.9
      l1=rl
      facl=rl-l1
      l2=l1+1
c      write(6,*)l1,l2,rl

C     FACTORS CALCULATIONS

      if(rap.lt.300.)then
      rapf=300.
      n1=code(s1,l1)
      cf1=c4(n1)*(rapf**4)+c3(n1) * (rapf**3) + c2(n1) * (rapf**2) +
     1c1(n1) * rapf + c0(n1)
      n2=code(s1,l2)
      cf2=c4(n2)*(rapf**4)+c3(n2) * (rapf**3) + c2(n2) * (rapf**2) +
     1c1(n2) * rapf + c0(n2)
      n3=code(s2,l1)
      cf3=c4(n3)*(rapf**4)+c3(n3) * (rapf**3) + c2(n3) * (rapf**2) +
     1c1(n3) * rapf + c0(n3)
      n4=code(s2,l2)
      cf4=c4(n4)*(rapf**4)+c3(n4) * (rapf**3) + c2(n4) * (rapf**2) +
     1c1(n4) * rapf + c0(n4)

C     INTERPOLATION

      cf300=cf1*(1 - facs) * (1 - facl) + cf2 * (1 - facs) * (facl) +
     *cf3 * (facs) * (1 - facl) + cf4 * (facs) * (facl)

      cf = (cf300-1.0)*rap/100.-2.*cf300+3.
      goto 100
      end if

      n1=code(s1,l1)
c      write(6,*)n1
      cf1 = c4(n1) * (rap**4) + c3(n1) * (rap**3) + c2(n1) * (rap**2) +
     1c1(n1) * rap + c0(n1)
      n2=code(s1,l2)
      cf2 = c4(n2) * (rap**4) + c3(n2) * (rap**3) + c2(n2) * (rap**2) +
     1c1(n2) * rap + c0(n2)
      n3=code(s2,l1)
      cf3 = c4(n3) * (rap**4) + c3(n3) * (rap**3) + c2(n3) * (rap**2) +
     1c1(n3) * rap + c0(n3)
      n4=code(s2,l2)
      cf4 = c4(n4) * (rap**4) + c3(n4) * (rap**3) + c2(n4) * (rap**2) +
     1c1(n4) * rap + c0(n4)

C     INTERPOLATION

      cf = cf1 * (1 - facs) * (1 - facl) + cf2 * (1 - facs) * (facl) +
     *cf3 * (facs) * (1 - facl) + cf4 * (facs) * (facl)

100   CONTINUE

      RETURN

      END
C
C
C****************************************************************************
C
        subroutine vdrift(xt,xl,param,y)
C-------------------------------------------------------------------
C       SUBROUTINE CALCULATES EQUATORIAL VERTICAL DRIFT AS DESCRIBED 
C       IN SCHERLIESS AND FEJER, JGR, 104, 6829-6842, 1999
C
C       INPUT:   XT: SOLAR LOCAL TIME  [h]
C                XL: GEOGRAPHIC LONGITUDE (+ EAST) [degrees]
C               
C           PARAM: 2-DIM ARRAY (DOY,F10.7CM)
C                  DOY     :Day of Year has to run from 1 to 365(366)
C                  F10.7cm : F10.7cm solar flux (daily value)
C             
C       OUTPUT:   Y: EQUATORIAL VERTICAL DRIFT [m/s]
C
C-------------------------------------------------------------------
c        IMPLICIT REAL*8 (A-H,O-Z)
        IMPLICIT REAL (A-H,O-Z)
       
c        real*8 param(2),coeff(624),coeff1(594),coeff2(30),funct(6)
c        real*8 xt,xl,y
c        real*8 bspl4,bspl4_time,bspl4_long

        real param(2),coeff(624),coeff1(594),coeff2(30),funct(6)
        real xt,xl,y
        real bspl4,bspl4_time,bspl4_long

        integer i,j,ind,il,kk
        integer index_t,dim_t,index_l,dim_l,index,dim,nfunc
        
        data index_t/13/,dim_t/78/,index_l/8/,dim_l/48/,index/104/,
     *   dim/624/,nfunc/6/

        data coeff1/
     *  -10.80592, -9.63722,-11.52666, -0.05716, -0.06288,  0.03564,
     *   -5.80962, -7.86988, -8.50888, -0.05194, -0.05798, -0.00138,
     *    2.09876,-19.99896, -5.11393, -0.05370, -0.06585,  0.03171,
     *  -10.22653, -3.62499,-14.85924, -0.04023, -0.01190, -0.09656,
     *   -4.85180,-26.26264, -6.20501, -0.05342, -0.05174,  0.02419,
     *  -13.98936,-18.10416, -9.30503, -0.01969, -0.03132, -0.01984,
     *  -18.36633,-24.44898,-16.69001,  0.02033, -0.03414, -0.02062,
     *  -20.27621,-16.95623,-36.58234,  0.01445, -0.02044, -0.08297,
     *    1.44450,  5.53004,  4.55166, -0.02356, -0.04267,  0.05023,
     *    5.50589,  7.05381,  1.94387, -0.03147, -0.03548,  0.01166,
     *    3.24165, 10.05002,  4.26218, -0.03419, -0.02651,  0.07456,
     *    7.02218,  0.06708,-11.31012, -0.03252, -0.01021, -0.09008,
     *   -3.47588, -2.82534, -4.17668, -0.03719, -0.01519,  0.06507,
     *   -4.02607,-11.19563,-10.52923, -0.00592, -0.01286, -0.00477,
     *  -11.47478, -9.57758,-10.36887,  0.04555, -0.02249,  0.00528,
     *  -14.19283,  7.86422, -8.76821,  0.05758, -0.02398, -0.04075,
     *   14.58890, 36.63322, 27.57497,  0.01358, -0.02316,  0.04723,
     *   12.53122, 29.38367, 21.40356, -0.00071, -0.00553,  0.01484,
     *   18.64421, 26.27327, 18.32704,  0.00578,  0.03349,  0.11249,
     *    4.53014,  6.15099,  7.41935, -0.02860, -0.00395, -0.08394,
     *   14.29422,  9.77569,  2.85689, -0.00107,  0.04263,  0.10739,
     *    7.17246,  4.40242, -1.00794,  0.00089,  0.01436,  0.00626,
     *    7.75487,  5.01928,  4.36908,  0.03952, -0.00614,  0.03039,
     *   10.25556,  8.82631, 24.21745,  0.05492, -0.02968,  0.00177,
     *   21.86648, 24.03218, 39.82008,  0.00490, -0.01281, -0.01715,
     *   19.18547, 23.97403, 34.44242,  0.01978,  0.01564, -0.02434,
     *   26.30614, 14.22662, 31.16844,  0.06495,  0.19590,  0.05631,
     *   21.09354, 25.56253, 29.91629, -0.04397, -0.08079, -0.07903,
     *   28.30202, 16.80567, 38.63945,  0.05864,  0.16407,  0.07622,
     *   22.68528, 25.91119, 40.45979, -0.03185, -0.01039, -0.01206,
     *   31.98703, 24.46271, 38.13028, -0.08738, -0.00280,  0.01322,
     *   46.67387, 16.80171, 22.77190, -0.13643, -0.05277, -0.01982,
     *   13.87476, 20.52521,  5.22899,  0.00485, -0.04357,  0.09970,
     *   21.46928, 13.55871, 10.23772, -0.04457,  0.01307,  0.06589,
     *   16.18181, 16.02960,  9.28661, -0.01225,  0.14623, -0.01570,
     *   18.16289, -1.58230, 14.54986, -0.00375, -0.00087,  0.04991,
     *   10.00292, 11.82653,  0.44417, -0.00768,  0.15940, -0.01775,
     *   12.15362,  5.65843, -1.94855, -0.00689,  0.03851,  0.04851,
     *   -1.25167,  9.05439,  0.74164,  0.01065,  0.03153,  0.02433,
     *  -15.46799, 18.23132, 27.45320,  0.00899, -0.00017,  0.03385,
     *    2.70396, -0.87077,  6.11476, -0.00081,  0.05167, -0.08932,
     *    3.21321, -1.06622,  5.43623,  0.01942,  0.05449, -0.03084,
     *   17.79267, -3.44694,  7.10702,  0.04734, -0.00945,  0.11516,
     *    0.46435,  6.78467,  4.27231, -0.02122,  0.10922, -0.03331,
     *   15.31708,  1.70927,  7.99584,  0.07462,  0.07515,  0.08934,
     *    4.19893,  6.01231,  8.04861,  0.04023,  0.14767, -0.04308,
     *    9.97541,  5.99412,  5.93588,  0.06611,  0.12144, -0.02124,
     *   13.02837, 10.29950, -4.86200,  0.04521,  0.10715, -0.05465,
     *    5.26779,  7.09019,  1.76617,  0.09339,  0.22256,  0.09222,
     *    9.17810,  5.27558,  5.45022,  0.14749,  0.11616,  0.10418,
     *    9.26391,  4.19982, 12.66250,  0.11334,  0.02532,  0.18919,
     *   13.18695,  6.06564, 11.87835,  0.26347,  0.02858,  0.14801,
     *   10.08476,  6.14899, 17.62618,  0.09331,  0.08832,  0.28208,
     *   10.75302,  7.09244, 13.90643,  0.09556,  0.16652,  0.22751,
     *    6.70338, 11.97698, 18.51413,  0.15873,  0.18936,  0.15705,
     *    5.68102, 23.81606, 20.65174,  0.19930,  0.15645,  0.08151,
     *   29.61644,  5.49433, 48.90934,  0.70710,  0.40791,  0.26325,
     *   17.11994, 19.65380, 44.88810,  0.45510,  0.41689,  0.22398,
     *    8.45700, 34.54442, 27.25364,  0.40867,  0.37223,  0.22374,
     *   -2.30305, 32.00660, 47.75799,  0.02178,  0.43626,  0.30187,
     *    8.98134, 33.01820, 33.09674,  0.33703,  0.33242,  0.41156,
     *   14.27619, 20.70858, 50.10005,  0.30115,  0.32570,  0.45061,
     *   14.44685, 16.14272, 45.40065,  0.37552,  0.31419,  0.30129,
     *    6.19718, 18.89559, 28.24927,  0.08864,  0.41627,  0.19993,
     *    7.70847, -2.36281,-21.41381,  0.13766,  0.05113, -0.11631,
     *   -9.07236,  3.76797,-20.49962,  0.03343,  0.08630,  0.00188,
     *   -8.58113,  5.06009, -6.23262,  0.04967,  0.03334,  0.24214,
     *  -27.85742,  8.34615,-27.72532, -0.08935,  0.15905, -0.03655,
     *    2.77234,  0.14626, -4.01786,  0.22338, -0.04478,  0.18650,
     *    5.61364, -3.82235,-16.72282,  0.26456, -0.03119, -0.08376,
     *   13.35847, -6.11518,-16.50327,  0.28957, -0.01345, -0.19223,
     *   -5.37290, -0.09562,-27.27889,  0.00266,  0.22823, -0.35585,
     *  -15.29676,-18.36622,-24.62948, -0.31299, -0.23832, -0.08463,
     *  -23.37099,-13.69954,-26.71177, -0.19654, -0.18522, -0.20679,
     *  -26.33762,-15.96657,-42.51953, -0.13575, -0.00329, -0.28355,
     *  -25.42140,-14.14291,-21.91748, -0.20960, -0.19176, -0.32593,
     *  -23.36042,-23.89895,-46.05270, -0.10336,  0.03030, -0.21839,
     *  -19.46259,-21.27918,-32.38143, -0.17673, -0.15484, -0.11226,
     *  -19.06169,-21.13240,-34.01677, -0.25497, -0.16878, -0.11004,
     *  -18.39463,-16.11516,-19.55804, -0.19834, -0.23271, -0.25699,
     *  -19.93482,-17.56433,-18.58818,  0.06508, -0.18075,  0.02796,
     *  -23.64078,-18.77269,-22.77715, -0.02456, -0.12238,  0.02959,
     *  -12.44508,-21.06941,-19.36011,  0.02746, -0.16329,  0.19792,
     *  -26.34187,-19.78854,-24.06651, -0.07299, -0.03082, -0.03535,
     *  -10.71667,-26.04401,-16.59048,  0.02850, -0.09680,  0.15143,
     *  -18.40481,-23.37770,-16.31450, -0.03989, -0.00729, -0.01688,
     *   -9.68886,-20.59304,-18.46657,  0.01092, -0.07901,  0.03422,
     *   -0.06685,-19.24590,-29.35494,  0.12265, -0.24792,  0.05978,
     *  -15.32341, -9.07320,-13.76101, -0.17018, -0.15122, -0.06144,
     *  -14.68939,-14.82251,-13.65846, -0.11173, -0.14410, -0.07133,
     *  -18.38628,-18.94631,-19.00893, -0.08062, -0.14481, -0.12949,
     *  -16.15328,-17.40999,-14.08705, -0.08485, -0.06896, -0.11583,
     *  -14.50295,-16.91671,-25.25793, -0.06814, -0.13727, -0.12213,
     *  -10.92188,-14.10852,-24.43877, -0.09375, -0.11638, -0.09053,
     *  -11.64716,-14.92020,-19.99063, -0.14792, -0.08681, -0.12085,
     *  -24.09766,-16.14519, -8.05683, -0.24065, -0.05877, -0.23726,
     *  -25.18396,-15.02034,-15.50531, -0.12236, -0.09610, -0.00529,
     *  -15.27905,-19.36708,-12.94046, -0.08571, -0.09560, -0.03544,
     *   -7.48927,-16.00753,-13.02842, -0.07862, -0.10110, -0.05807/
        data coeff2/
     *  -13.06383,-27.98698,-18.80004, -0.05875, -0.03737, -0.11214,
     *  -13.67370,-16.44925,-16.12632, -0.07228, -0.09322, -0.05652,
     *  -22.61245,-21.24717,-18.09933, -0.05197, -0.07477, -0.05235,
     *  -27.09189,-21.85181,-20.34676, -0.05123, -0.05683, -0.07214,
     *  -27.09561,-22.76383,-25.41151, -0.10272, -0.02058, -0.16720/

        do i=1,594 
        	coeff(i)=coeff1(i)
        	enddo
        do i=1,30 
        	coeff(i+594)=coeff2(i)
        	enddo

        call g(param,funct,xl)

        y=0.

        do i=1,index_t
          do il=1,index_l
            kk=index_l*(i-1)+il
            do j=1,nfunc
               ind=nfunc*(kk-1)+j
               bspl4=bspl4_time(i,xt)*bspl4_long(il,xl)
               y=y+bspl4*funct(j)*coeff(ind)
            end do
          end do
        end do

       end
C
C
c        real*8 function bspl4_time(i,x1)
        real function bspl4_time(i,x1)
c       *************************************************
c        implicit REAL*8 (A-H,O-Z)
        implicit REAL (A-H,O-Z)
		 
        integer i,order,j,k
c        real*8 t_t(0:39)
c        real*8 x,b(20,20),x1
        real t_t(0:39)
        real x,b(20,20),x1

        data t_t/
     *          0.00,2.75,4.75,5.50,6.25,
     *          7.25,10.00,14.00,17.25,18.00,
     *          18.75,19.75,21.00,24.00,26.75,
     *          28.75,29.50,30.25,31.25,34.00,
     *          38.00,41.25,42.00,42.75,43.75,
     *          45.00,48.00,50.75,52.75,53.50,
     *          54.25,55.25,58.00,62.00,65.25,
     *          66.00,66.75,67.75,69.00,72.00/

        order=4
        x=x1
        if(i.ge.0) then
          if (x.lt.t_t(i-0)) then
              x=x+24
              end if
          end if
        do j=i,i+order-1
           if(x.ge.t_t(j).and.x.lt.t_t(j+1)) then
               b(j,1)=1
           else
	           b(j,1)=0
           end if
        end do

        do j=2,order
          do k=i,i+order-j
            b(k,j)=(x-t_t(k))/(t_t(k+j-1)-t_t(k))*b(k,j-1)
            b(k,j)=b(k,j)+(t_t(k+j)-x)/(t_t(k+j)-t_t(k+1))*
     &                b(k+1,j-1)
          end do
        end do

        bspl4_time=b(i,order)
        end
C
C
        real function bspl4_long(i,x1)
c        real*8 function bspl4_long(i,x1)
c       *************************************************
c       implicit real*8 (A-H,O-Z) 
       implicit real (A-H,O-Z) 

        integer i,order,j,k
c        real*8 t_l(0:24)
c        real*8 x,b(20,20),x1
        real t_l(0:24)
        real x,b(20,20),x1

        data t_l/
     *          0,10,100,190,200,250,280,310,
     *          360,370,460,550,560,610,640,670,
     *          720,730,820,910,920,970,1000,1030,1080/
       
        order=4
        x=x1
        if(i.ge.0) then
          if (x.lt.t_l(i-0)) then
              x=x+360
              end if
          end if
        do j=i,i+order-1
           if(x.ge.t_l(j).and.x.lt.t_l(j+1)) then
              b(j,1)=1
           else
              b(j,1)=0
           end if
        end do

        do j=2,order
          do k=i,i+order-j
             b(k,j)=(x-t_l(k))/(t_l(k+j-1)-t_l(k))*b(k,j-1)
             b(k,j)=b(k,j)+(t_l(k+j)-x)/(t_l(k+j)-t_l(k+1))*
     &                 b(k+1,j-1)
          end do
        end do

        bspl4_long=b(i,order)
        end
C
C
        subroutine g(param,funct,x)
c       *************************************************
c        implicit real*8 (A-H,O-Z)
        implicit real (A-H,O-Z)

        integer i
c        real*8 param(2),funct(6)
c        real*8 x,a,sigma,gauss,flux,cflux
        real param(2),funct(6)
        real x,a,sigma,gauss,flux,cflux

c       *************************************************
        flux=param(2)
        if(param(2).le.75)  flux=75.
        if(param(2).ge.230) flux=230.
        cflux=flux

        a=0.
        if((param(1).ge.120).and.(param(1).le.240)) a=170.
        if((param(1).ge.120).and.(param(1).le.240)) sigma=60
        if((param(1).le.60).or.(param(1).ge.300)) a=170.
        if((param(1).le.60).or.(param(1).ge.300)) sigma=40

        if((flux.le.95).and.(a.ne.0)) then
           gauss=exp(-0.5*((x-a)**2)/sigma**2)
           cflux=gauss*95.+(1-gauss)*flux
           end if
c       *************************************************

c       *************************************************
        do i=1,6
         funct(i)=0.
        end do
c       *************************************************

c       *************************************************
        if((param(1).ge.135).and.(param(1).le.230)) funct(1)=1
        if((param(1).le.45).or.(param(1).ge.320)) funct(2)=1
        if((param(1).gt.75).and.(param(1).lt.105)) funct(3)=1
        if((param(1).gt.260).and.(param(1).lt.290)) funct(3)=1
c       *************************************************

        if((param(1).ge.45).and.(param(1).le.75)) then  ! W-E
            funct(2)=1.-(param(1)-45.)/30.
            funct(3)=1-funct(2)
            end if
        if((param(1).ge.105).and.(param(1).le.135)) then  ! E-S
            funct(3)=1.-(param(1)-105.)/30.
            funct(1)=1-funct(3)
            end if
        if((param(1).ge.230).and.(param(1).le.260)) then  ! S-E
            funct(1)=1.-(param(1)-230.)/30.
            funct(3)=1-funct(1)
            end if
        if((param(1).ge.290).and.(param(1).le.320)) then  ! E-W
            funct(3)=1.-(param(1)-290.)/30.
            funct(2)=1-funct(3)
            end if

c       *************************************************
        funct(4)=(cflux-140)*funct(1)
        funct(5)=(cflux-140)*funct(2)
        funct(6)=(flux-140)*funct(3)
c       *************************************************

        end
c
c
       SUBROUTINE StormVd(FLAG,iP,AE,SLT,PromptVd,DynamoVd,Vd)
C *******************************************************************
C  Empirical vertical disturbance drifts model
C  After Fejer and Scherliess, JGR, 102, 24047-24056,1997
C*********************************************************************
C  INPUT:
C    AE: AE(in nT) in 1 hour or 15 minute resolution;
C    SLT: Local time(in hrs) for wanted Vd;
C  OUTPUT:
C    PromptVd: Prompt penetration vertical drifts at given conditions;
C    DynamoVd: Disturbane dynamo vertical drifts at given conditions;
C    Vd: PromptVd+DynamoVd;
C*********************************************************************

c       IMPLICIT REAL*8(A-H,O-Z)
       IMPLICIT REAL(A-H,O-Z)
c       REAL*8 AE(1:366*24*4),Coff1(1:5,1:9),Coff15(1:6,1:9)
       REAL AE(1:366*24*4),Coff1(1:5,1:9),Coff15(1:6,1:9)
       INTEGER FLAG 
       DATA Coff1/
     @           0.0124,-0.0168,-0.0152,-0.0174,-0.0704,
     @          -0.0090,-0.0022,-0.0107, 0.0152,-0.0674,
     @           0.0275, 0.0051,-0.0132, 0.0020,-0.0110,
     @          -0.0022, 0.0044, 0.0095, 0.0036,-0.0206,
     @           0.0162, 0.0007, 0.0085,-0.0140, 0.0583,
     @           0.0181, 0.0185,-0.0109,-0.0031,-0.0427,
     @          -0.0057, 0.0002, 0.0086, 0.0149, 0.2637,
     @          -0.0193, 0.0035, 0.0117, 0.0099, 0.3002,
     @          -0.0492,-0.0201, 0.0338, 0.0099, 0.0746/

	 DATA Coff15/
     @	        0.0177, 0.0118,-0.0006,-0.0152,-0.0174,-0.0704,
     @	        0.0051,-0.0074,-0.0096,-0.0107, 0.0152,-0.0674,
     @	        0.0241, 0.0183, 0.0122,-0.0132, 0.0020,-0.0110,
     @	        0.0019,-0.0010, 0.0001, 0.0095, 0.0036,-0.0206,
     @	        0.0170, 0.0183, 0.0042, 0.0085,-0.0140, 0.0583,
     @          0.0086, 0.0189, 0.0200,-0.0109,-0.0031,-0.0427,
     @	       -0.0070,-0.0053,-0.0090, 0.0086, 0.0149, 0.2637,
     @	       -0.0326,-0.0101, 0.0076, 0.0117, 0.0099, 0.3002,
     @	       -0.0470,-0.0455,-0.0274, 0.0338, 0.0099, 0.0746/

CCCCCCCCCCCCCCCCC**Define to variables**CCCCCCCCCCCCCCCCCCCCC
C To 1 h time resolution:
C dAEt_30=AE(t)-AE(t-1 hour);
C dAEt_90=AE(t-1 hour)-AE(t-2 hour);
CC
C To 15 MIN time resolution :
C dAEt_7P5=AE(t)-AE(t-15min);
C dAEt_30=AE(t-15)-AE(t-45min);
C dAEt_75=AE(t-45)-AE(t-105min);
CC
C  Following variables are the same to two resolution: 
C AE1_6=average(AE(1-6hours));
C AE7_12=average(AE(7-12hours));
C AE1_12=average(AE(1-12hours));
C AEd1_6=average(X(AE(1-6hours)-130 nT));
C AEd7_12=average(X(AE(7-12hours)-130 nT));
C AEd1_12=average(X(AE(1-12hours)-130 nT));
C AEd22_28=average(X(AE(22-28hours)-130 nT));
C Here X(a)=a, a>0; =0, a<=0;
C Alfa=0,            AE1_6<200 nT;
C      AE1_6/100-2, 200 nT<AE1_6<200 nT;
C      1,            AE1_6>300 nT;
C Beta=exp(-AE1_12/90),  AE1_12>=70nT;
C      0.46,              AE1_12<70 nT;
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCccccccc
C*****************************************************
CC        FLAG>0--> 1 h time resolution
C**************************************************** 

       IF (FLAG.GT.0) THEN
C 
         dAEt_30=AE(iP)-AE(iP-1)
         dAEt_90=AE(iP-1)-AE(iP-2)
C
         AE1_6=0.0D0
         AEd1_6=0.0D0

         DO i=-1,-6,-1
             AE1_6=AE1_6+AE(iP+i)
             AEd1_6S=AE(iP+i)-130.0D0
             IF (AEd1_6S.LE.0.0D0) AEd1_6S=0.0D0
             AEd1_6=AEd1_6+AEd1_6S
         END DO

         AE1_6=AE1_6/6.0D0
         AEd1_6=AEd1_6/6.0D0
C
         AEd7_12=0.0D0

         DO i=-7,-12,-1
            AEd7_12S=AE(iP+i)-130.0D0
            IF (AEd7_12S.LE.0.0D0) AE7_12S=0.0D0
            AEd7_12=AEd7_12+AEd7_12S
         END DO

         AEd7_12=AEd7_12/6.0D0
C
         AE1_12=0.0D0

         DO i=-1,-12,-1
            AE1_12=AE1_12+AE(iP+i)
         END DO

         AE1_12=AE1_12/12.0D0
C
         AEd22_28=0.0D0

         DO i=-22,-28,-1
            AEd22_28S=AE(iP+i)-130.0D0
            IF (AED22_28S.LE.0.0D0) AEd22_28S=0.0D0
            AEd22_28=AEd22_28+AEd22_28S 
         END DO

         AEd22_28=AEd22_28/7.0D0
         AEd22_28P=AEd22_28-200.0D0
         IF (AEd22_28P.LE.0.0D0) AEd22_28P=0.0D0
CC
         IF (AE1_6.GT.300.0D0) THEN  
           Alfa=1.0D0
         ELSE IF (AE1_6.GT.200.0D0) THEN
           ALfa=AE1_6/100.0D0-2.0D0
         ELSE 
           ALfa=0.0D0
         ENDIF
CC
         IF (AE1_12.GE.70.0D0) THEN
            Beta=dexp(-AE1_12/90.0D0)
         ELSE
            Beta=0.46D0
         END IF
         PromptVd=0.0D0
         DO J=1,9
            PromptVd=PromptVd +(Coff1(1,J)*dAEt_30 +Coff1(2,J)*dAEt_90
     #                         )*bspl4_ptime(J,SLT)
         END DO
         DynamoVd=0.0D0
         DO J=1,9
            DynamoVd=DynamoVd+
     #               (Coff1(3,J)*AEd1_6+Coff1(4,J)*Alfa*AEd7_12
     #                +Coff1(5,J)*Beta*AEd22_28P)*bspl4_ptime(J,SLT)
         END DO
         Vd=PromptVd+DynamoVd
         RETURN

C 1 h time resolution end;
C********************************************************************
C                  15 min time resolution
C********************************************************************

       ELSE
         dAEt_7P5=AE(iP)-AE(iP-1)
         dAEt_30=AE(iP-1)-AE(iP-3)
         dAEt_75=AE(iP-3)-AE(iP-7)

         AE1_6=0.0D0
         AEd1_6=0.0D0

         DO i=-4,-24,-1
           AE1_6=AE1_6+AE(iP+i)
           AEd1_6s=AE(iP+i)-130.
           IF (AEd1_6s.LE.0.0) AEd1_6s=0.0
           AEd1_6=AEd1_6+AEd1_6S
         ENDDO

         AE1_6=AE1_6/21.0D0
         AEd1_6=AEd1_6/21.0D0
CC
         AEd7_12=0.0D0 
         DO i=-28,-48,-1
            AEd7_12s=AE(iP+i)-130.0
            IF (AEd7_12s.LE.0) AEd7_12s=0.0
            AEd7_12=AEd7_12+AEd7_12S
         ENDDO
         AEd7_12=AEd7_12/21.0D0
CC
         AE1_12=0.0D0
         DO i=-4,-48,-1
            AE1_12=AE1_12+AE(iP+i)
         END DO
         AE1_12=AE1_12/45.0D0
CC 
	  AEd22_28=0.0D0
          DO i=-88,-112,-1
             AEd22_28s=AE(iP+i)-130.
             IF (AEd22_28s.LE.0) AEd22_28s=0.0
	     AEd22_28=AEd22_28+AEd22_28s
	  ENDDO
          AEd22_28=AEd22_28/25.0D0
	  AEd22_28P=AEd22_28-200.0D0
	  IF (AEd22_28P.LE.0.0D0) AEd22_28P=0.0D0

c         AE1_6=0.0D0
c         AEd1_6=0.0D0
c         AEd7_12=0.0D0 
c         AEd22_28P=0.0D0
c         AE1_12=0.0D0
c         dAEt_7P5=400.D0
c         dAEt_30=0.D0
c         dAEt_75=0.D0
CC
  	  IF (AE1_6.GT.300.0D0) THEN  
             Alfa=1.0D0
          ELSE IF (AE1_6.GT.200.0D0) THEN
             ALfa=AE1_6/100.0D0-2.0D0
          ELSE 
             ALfa=0.0D0
          ENDIF
CC
          IF (AE1_12.GE.70.0D0) THEN
             Beta=dexp(-AE1_12/90.0D0)
          ELSE
             Beta=0.46D0
          END IF
CC
          PromptVd=0.0D0
          DO J=1,9
            PromptVd=PromptVd+(Coff15(1,J)*dAEt_7P5+Coff15(2,J)*dAEt_30
     #                      +Coff15(3,J)*dAEt_75)*bspl4_ptime(J,SLT)
          END DO
          DynamoVd=0.0D0
c	  print*,AEd1_6,AEd7_12,AEd22_28P,Alfa,Beta
          DO J=1,9
             DynamoVd=DynamoVd +(Coff15(4,J)*AEd1_6+
     #                           Coff15(5,J)*Alfa*AEd7_12+
     #                           Coff15(6,J)*Beta*AEd22_28P
     #                          )*bspl4_ptime(J,SLT)
          END DO
          Vd=PromptVd+DynamoVd
       ENDIF
       RETURN
       END                     
C
C
       real function bspl4_ptime(i,x1)
c       real*8 function bspl4_ptime(i,x1)
C *************************************************

c       IMPLICIT REAL*8 (A-H,O-Z)
       IMPLICIT REAL (A-H,O-Z)

       integer i,order,j,k
c       real*8 t_t(0:27)
c       real*8 x,b(20,20),x1
       real t_t(0:27)
       real x,b(20,20),x1

       data t_t/0.00,3.00,4.50,6.00,9.00,12.0,15.0,18.0,21.0,
     *          24.0,27.0,28.5,30.0,33.0,36.0,39.0,42.0,45.0,
     *          48.0,51.0,52.5,54.0,57.0,60.0,63.0,66.0,69.0,72.0/
C
       order=4
       x=x1
       if(i.ge.0) then
          if (x.lt.t_t(i-0)) then
	       x=x+24
          end if
       end if
       do j=i,i+order-1
	   if(x.ge.t_t(j).and.x.lt.t_t(j+1)) then
	       b(j,1)=1
	   else
	       b(j,1)=0
	   end if
       end do
c
       do j=2,order
          do k=i,i+order-j
             b(k,j)=(x-t_t(k))/(t_t(k+j-1)-t_t(k))*b(k,j-1)
             b(k,j)=b(k,j)+(t_t(k+j)-x)/(t_t(k+j)-t_t(k+1))*b(k+1,j-1)
          end do
       end do
       bspl4_ptime=b(i,order)
       return
       end

C
C***************************************************************************
C

       subroutine spreadf_brazil(idoy,idiy,f107,geolat,osfbr)
**********************************************************************       
*
*       SUBROUTINE CALCULATES PERCENTAGE OF SPREAD F OCCURRENCE OVER 
*       BRAZILIAN SECTOR AS DESCRIBED IN:
*       ABDU ET AL., Advances in Space Research, 31(3), 
*       703-716, 2003
*
*    INPUT:
*         IDOY: DAY OF YEAR (1 TO 365/366)
*         IDIY: DAYS IN YEAR (365 OR 366)
*         F107: F10.7 cm SOLAR FLUX (DAILY VALUE)
*         GEOLAT: BRAZILIAN GEOGRAPHIC LATITUDE BETWEEN -4 AND -22.5
*
*    OUTPUT:         
*         OSFBR(25): PERCENTAGE OF SPREAD F OCCURRENCE FOR 25 TIME 
*                    STEPS FROM LT=18 TO LT=7 ON THE NEXT DAY IN
*                    STEPS OF 0.5 HOURS.
*
**********************************************************************
*
         dimension param(3),osfbr(25),coef_sfa(684),coef_sfb(684),
     &             sosf(2,32,3,12)
         common/mflux/kf,n     
         data coef_sfa/
     *   0.07,0.13,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.05,0.04,0.03
     *  ,0.06,0.07,0.02,0.03,0.03,0.07,0.06,0.07,0.21,0.28,0.34,0.16
     *  ,0.12,0.00,0.02,0.02,0.04,0.05,0.02,0.11,0.19,0.31,0.31,0.11
     *  ,0.14,0.16,0.03,0.00,0.00,0.02,0.00,0.00,0.05,0.55,0.61,0.28
     *  ,0.26,0.10,0.15,0.23,0.07,0.06,0.03,0.03,0.41,0.88,0.89,0.65
     *  ,0.19,0.18,0.17,0.10,0.14,0.15,0.03,0.14,0.46,0.72,0.71,0.53
     *  ,0.57,0.38,0.30,0.14,0.00,0.04,0.03,0.02,0.21,0.84,0.87,0.72
     *  ,0.79,0.60,0.65,0.70,0.29,0.19,0.19,0.32,0.73,0.96,0.99,0.84
     *  ,0.75,0.78,0.79,0.70,0.63,0.24,0.28,0.53,0.75,0.77,0.75,0.85
     *  ,0.78,0.51,0.59,0.24,0.00,0.07,0.05,0.06,0.33,0.92,0.96,0.89
     *  ,0.90,0.84,0.86,0.81,0.33,0.27,0.23,0.47,0.90,1.00,1.00,0.96
     *  ,0.96,0.89,0.92,0.84,0.80,0.27,0.35,0.61,0.81,0.93,0.86,0.97
     *  ,0.84,0.65,0.75,0.25,0.00,0.04,0.08,0.06,0.53,0.93,0.96,0.94
     *  ,0.95,0.84,0.91,0.71,0.18,0.17,0.21,0.42,0.92,0.99,0.97,0.92
     *  ,0.92,0.93,0.92,0.67,0.58,0.21,0.38,0.55,0.83,0.90,0.89,0.97
     *  ,0.84,0.71,0.91,0.21,0.02,0.07,0.03,0.03,0.60,0.95,0.96,0.92
     *  ,0.97,0.91,0.92,0.67,0.11,0.08,0.09,0.23,0.90,0.99,0.99,0.96
     *  ,0.96,0.93,0.98,0.63,0.25,0.08,0.12,0.41,0.79,0.95,0.98,0.99
     *  ,0.86,0.80,0.94,0.22,0.02,0.04,0.03,0.03,0.63,0.95,0.96,0.94
     *  ,0.98,0.90,0.91,0.59,0.10,0.04,0.07,0.15,0.83,0.97,0.97,0.90
     *  ,0.92,0.93,0.95,0.57,0.12,0.03,0.05,0.23,0.74,0.94,0.94,0.99
     *  ,0.84,0.84,0.90,0.24,0.02,0.07,0.07,0.03,0.60,0.95,0.96,0.97
     *  ,0.93,0.82,0.83,0.51,0.08,0.07,0.09,0.09,0.71,0.95,0.92,0.87
     *  ,0.91,0.91,0.89,0.50,0.14,0.03,0.06,0.14,0.61,0.84,0.89,0.94
     *  ,0.77,0.82,0.84,0.34,0.10,0.11,0.12,0.06,0.43,0.87,0.94,0.97
     *  ,0.91,0.77,0.68,0.42,0.06,0.08,0.10,0.04,0.51,0.78,0.71,0.77
     *  ,0.85,0.88,0.77,0.35,0.16,0.05,0.08,0.15,0.53,0.70,0.60,0.89
     *  ,0.85,0.71,0.72,0.26,0.16,0.17,0.08,0.15,0.38,0.73,0.91,0.91
     *  ,0.89,0.68,0.53,0.26,0.06,0.12,0.08,0.09,0.32,0.63,0.67,0.77
     *  ,0.81,0.79,0.59,0.21,0.14,0.03,0.06,0.09,0.23,0.51,0.34,0.79
     *  ,0.88,0.66,0.59,0.16,0.18,0.15,0.16,0.16,0.33,0.67,0.75,0.88
     *  ,0.80,0.64,0.52,0.16,0.04,0.09,0.04,0.09,0.24,0.47,0.53,0.50
     *  ,0.73,0.69,0.48,0.11,0.14,0.03,0.03,0.03,0.20,0.37,0.28,0.54
     *  ,0.81,0.64,0.49,0.18,0.12,0.17,0.16,0.19,0.31,0.57,0.70,0.83
     *  ,0.76,0.57,0.52,0.13,0.04,0.06,0.05,0.08,0.21,0.49,0.47,0.39
     *  ,0.69,0.66,0.43,0.11,0.10,0.02,0.00,0.03,0.16,0.39,0.24,0.35
     *  ,0.77,0.45,0.39,0.10,0.10,0.13,0.15,0.18,0.29,0.57,0.70,0.69
     *  ,0.71,0.49,0.54,0.20,0.05,0.06,0.05,0.06,0.27,0.42,0.36,0.42
     *  ,0.61,0.59,0.50,0.08,0.06,0.02,0.03,0.02,0.16,0.40,0.17,0.31
     *  ,0.68,0.30,0.28,0.13,0.10,0.16,0.14,0.08,0.19,0.50,0.63,0.62
     *  ,0.63,0.45,0.51,0.13,0.06,0.07,0.04,0.06,0.27,0.42,0.28,0.35
     *  ,0.68,0.53,0.57,0.15,0.05,0.00,0.00,0.05,0.31,0.33,0.18,0.22
     *  ,0.59,0.32,0.21,0.06,0.10,0.16,0.12,0.10,0.19,0.41,0.55,0.54
     *  ,0.69,0.43,0.43,0.15,0.06,0.05,0.05,0.08,0.29,0.39,0.23,0.29
     *  ,0.57,0.51,0.56,0.13,0.06,0.00,0.00,0.05,0.34,0.27,0.19,0.24
     *  ,0.49,0.16,0.13,0.09,0.04,0.11,0.11,0.05,0.17,0.32,0.49,0.49
     *  ,0.60,0.42,0.38,0.11,0.06,0.04,0.07,0.07,0.25,0.36,0.21,0.25
     *  ,0.65,0.48,0.53,0.17,0.05,0.00,0.00,0.11,0.29,0.14,0.20,0.22
     *  ,0.44,0.16,0.18,0.07,0.04,0.04,0.07,0.03,0.12,0.23,0.39,0.43
     *  ,0.57,0.40,0.35,0.14,0.06,0.03,0.04,0.07,0.18,0.27,0.14,0.15
     *  ,0.45,0.50,0.50,0.19,0.06,0.00,0.02,0.05,0.26,0.19,0.15,0.18
     *  ,0.23,0.09,0.12,0.06,0.04,0.02,0.02,0.02,0.10,0.03,0.14,0.26
     *  ,0.39,0.34,0.22,0.07,0.03,0.00,0.04,0.01,0.15,0.01,0.04,0.14
     *  ,0.41,0.39,0.35,0.13,0.02,0.00,0.00,0.06,0.17,0.07,0.06,0.14
     *  ,0.07,0.02,0.03,0.00,0.00,0.00,0.00,0.00,0.00,0.01,0.03,0.08
     *  ,0.19,0.14,0.14,0.00,0.03,0.01,0.02,0.00,0.09,0.00,0.01,0.00
     *  ,0.18,0.09,0.16,0.08,0.01,0.00,0.02,0.02,0.15,0.00,0.03,0.04/
*
        data coef_sfb/
     *   0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00
     *  ,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.03,0.00,0.00,0.00,0.00
     *  ,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.02,0.01,0.00,0.00,0.00
     *  ,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00
     *  ,0.00,0.01,0.00,0.00,0.00,0.00,0.00,0.01,0.01,0.00,0.00,0.00
     *  ,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.05,0.03,0.00,0.02,0.00
     *  ,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00
     *  ,0.00,0.04,0.00,0.01,0.00,0.00,0.00,0.01,0.01,0.05,0.00,0.00
     *  ,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.04,0.00,0.03,0.03,0.00
     *  ,0.01,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.01,0.00
     *  ,0.01,0.04,0.04,0.03,0.00,0.01,0.00,0.01,0.00,0.27,0.14,0.06
     *  ,0.05,0.04,0.02,0.00,0.00,0.00,0.00,0.04,0.09,0.48,0.43,0.27
     *  ,0.05,0.04,0.01,0.00,0.00,0.00,0.00,0.00,0.00,0.13,0.16,0.06
     *  ,0.26,0.12,0.29,0.04,0.01,0.02,0.00,0.01,0.08,0.65,0.56,0.45
     *  ,0.43,0.42,0.42,0.09,0.00,0.02,0.00,0.00,0.34,0.67,0.73,0.72
     *  ,0.10,0.05,0.04,0.00,0.01,0.00,0.00,0.00,0.00,0.18,0.39,0.15
     *  ,0.61,0.37,0.51,0.06,0.01,0.02,0.01,0.01,0.18,0.72,0.63,0.80
     *  ,0.77,0.66,0.70,0.19,0.00,0.02,0.02,0.02,0.41,0.68,0.88,0.85
     *  ,0.24,0.11,0.08,0.00,0.01,0.00,0.00,0.00,0.00,0.28,0.51,0.29
     *  ,0.75,0.48,0.57,0.11,0.00,0.02,0.01,0.01,0.19,0.77,0.77,0.88
     *  ,0.89,0.81,0.74,0.21,0.02,0.02,0.02,0.02,0.42,0.71,0.93,0.95
     *  ,0.49,0.30,0.19,0.00,0.00,0.00,0.00,0.01,0.06,0.38,0.64,0.48
     *  ,0.86,0.60,0.62,0.12,0.00,0.02,0.01,0.00,0.18,0.81,0.84,0.94
     *  ,0.88,0.79,0.70,0.26,0.03,0.02,0.02,0.02,0.36,0.61,0.98,0.93
     *  ,0.60,0.46,0.31,0.03,0.00,0.01,0.00,0.00,0.09,0.50,0.71,0.58
     *  ,0.90,0.65,0.66,0.10,0.00,0.02,0.01,0.02,0.15,0.69,0.80,0.86
     *  ,0.84,0.75,0.64,0.09,0.03,0.00,0.00,0.04,0.26,0.54,0.78,0.92
     *  ,0.62,0.59,0.44,0.01,0.00,0.01,0.00,0.00,0.13,0.52,0.77,0.63
     *  ,0.84,0.67,0.63,0.11,0.00,0.00,0.03,0.03,0.18,0.65,0.75,0.84
     *  ,0.81,0.63,0.47,0.06,0.02,0.00,0.00,0.05,0.14,0.49,0.76,0.91
     *  ,0.58,0.63,0.47,0.09,0.00,0.07,0.01,0.04,0.15,0.48,0.68,0.61                     
     *  ,0.79,0.63,0.55,0.12,0.01,0.01,0.02,0.05,0.13,0.57,0.51,0.63
     *  ,0.72,0.54,0.43,0.11,0.02,0.00,0.00,0.09,0.16,0.39,0.59,0.72
     *  ,0.46,0.55,0.39,0.07,0.01,0.03,0.03,0.06,0.15,0.37,0.51,0.50
     *  ,0.61,0.43,0.38,0.11,0.01,0.03,0.02,0.03,0.10,0.38,0.38,0.60
     *  ,0.58,0.42,0.38,0.15,0.02,0.00,0.00,0.11,0.13,0.24,0.41,0.51
     *  ,0.36,0.36,0.21,0.04,0.04,0.03,0.06,0.05,0.06,0.26,0.39,0.43
     *  ,0.43,0.31,0.24,0.09,0.02,0.00,0.02,0.02,0.06,0.24,0.24,0.40
     *  ,0.53,0.19,0.28,0.13,0.02,0.02,0.02,0.09,0.13,0.17,0.24,0.40
     *  ,0.32,0.27,0.17,0.03,0.04,0.02,0.04,0.03,0.06,0.13,0.34,0.36
     *  ,0.42,0.31,0.20,0.09,0.03,0.00,0.02,0.01,0.07,0.19,0.24,0.32
     *  ,0.44,0.10,0.23,0.13,0.03,0.02,0.00,0.09,0.12,0.17,0.21,0.33
     *  ,0.32,0.23,0.16,0.00,0.02,0.04,0.03,0.03,0.06,0.15,0.29,0.34
     *  ,0.36,0.26,0.28,0.07,0.01,0.00,0.01,0.02,0.04,0.19,0.17,0.27
     *  ,0.34,0.14,0.26,0.09,0.03,0.02,0.00,0.06,0.13,0.09,0.16,0.22
     *  ,0.29,0.21,0.15,0.00,0.02,0.02,0.02,0.03,0.11,0.16,0.26,0.28
     *  ,0.29,0.22,0.27,0.05,0.01,0.00,0.01,0.01,0.02,0.14,0.09,0.19
     *  ,0.25,0.19,0.25,0.07,0.02,0.02,0.00,0.00,0.09,0.07,0.12,0.15
     *  ,0.23,0.20,0.16,0.00,0.03,0.04,0.00,0.00,0.08,0.09,0.21,0.18
     *  ,0.22,0.21,0.19,0.02,0.02,0.00,0.01,0.03,0.04,0.08,0.06,0.14
     *  ,0.20,0.12,0.23,0.02,0.00,0.02,0.00,0.00,0.05,0.05,0.09,0.11
     *  ,0.14,0.16,0.13,0.00,0.03,0.04,0.00,0.00,0.05,0.05,0.04,0.09
     *  ,0.09,0.13,0.16,0.03,0.01,0.00,0.01,0.03,0.01,0.03,0.04,0.10
     *  ,0.14,0.09,0.17,0.02,0.02,0.00,0.00,0.02,0.04,0.04,0.03,0.07
     *  ,0.00,0.11,0.09,0.00,0.02,0.00,0.00,0.00,0.01,0.00,0.02,0.02
     *  ,0.02,0.06,0.11,0.00,0.00,0.00,0.00,0.01,0.00,0.00,0.01,0.02
     *  ,0.06,0.09,0.13,0.00,0.02,0.00,0.03,0.02,0.03,0.01,0.02,0.01/
*
        param(1)=idoy
        param(2)=f107
        param(3)=geolat
        n=idiy-365
*
        if(param(1).le.31.)kf=1
         if(param(1).gt.31..and.param(1).le.(59+n))kf=2
	      if(param(1).gt.(59+n).and.param(1).le.(90+n))kf=3
           if(param(1).gt.(90+n).and.param(1).le.(120+n))kf=4
            if(param(1).gt.(120+n).and.param(1).le.(151+n))kf=5
	         if(param(1).gt.(151+n).and.param(1).le.(181+n))kf=6
             if(param(1).gt.(181+n).and.param(1).le.(212+n))kf=7
            if(param(1).gt.(212+n).and.param(1).le.(243+n))kf=8
           if(param(1).gt.(243+n).and.param(1).le.(273+n))kf=9
	      if(param(1).gt.(273+n).and.param(1).le.(304+n))kf=10
         if(param(1).gt.(304+n).and.param(1).le.(334+n))kf=11
        if(param(1).gt.(334+n).and.param(1).le.(365+n))kf=12
*
      do i=1,32
       do j=1,3
        do k=1,12
           sosf(1,i,j,k)=0.
           sosf(2,i,j,k)=0.           
        enddo
       enddo
      enddo
*         
      kc=0
      do i=5,23
       do j=1,3
        do k=1,12
           kc=kc+1
           sosf(1,i,j,k)=coef_sfa(kc)
           sosf(2,i,j,k)=coef_sfb(kc) 
        enddo
       enddo
      enddo
           
      kk=0    
      do it=1600,3200,50      
        slt=it/100.
        osft=0.
        do i=1,23
          il=i+3
	      if(il.gt.23)il=il-23
          do j=1,12
	        jl=j+2
	        if(jl.gt.12)jl=jl-12
            do m=1,3
	          ml=m+1
	          if(ml.gt.3)ml=ml-3
              do  l=1,2
                bspl4=bspl4t(i,slt)*bspl2s(j,param(1))*
     &	         bspl2l(l,param(3))*bspl2f(m,param(2))
                osft=osft+bspl4*sosf(l,il,ml,jl)           
              enddo
            enddo
          enddo
        enddo
        if(slt.gt.17.98.and.slt.lt.30.01)then
          kk=kk+1
          osfbr(kk)=osft 
          endif
      enddo
*
*
      do iii=1,25 
         if(osfbr(iii).gt.1.) osfbr(iii)=1.
         if(osfbr(iii).lt.0.) osfbr(iii)=0.
         enddo
      return
      end
*
**********************************************************************
      function bspl4t(i,t1)
**********************************************************************
      dimension tt(0:78),b(30,30)
*
      data tt/16.00,16.50,17.00,17.50,18.00,18.50,19.00,19.50,20.00,
     &  20.50,21.00,22.00,23.00,24.00,25.00,26.00,27.00,27.50,28.00,
     &  28.50,29.00,29.50,30.00,30.50,31.00,32.00,40.00,40.50,41.00,
     &  41.50,42.00,42.50,43.00,43.50,44.00,44.50,45.00,46.00,47.00,
     &  48.00,49.00,50.00,51.00,51.50,52.00,52.50,53.00,53.50,54.00,
     &  54.50,55.00,56.00,64.00,64.50,65.00,65.50,66.00,66.50,67.00,
     &  67.50,68.00,68.50,69.00,70.00,71.00,72.00,73.00,74.00,75.00,
     &  75.50,76.00,76.50,77.00,77.50,78.00,78.50,79.00,80.00,88.00/
*
      t=t1
      if(i.ge.0.and.t.lt.tt(i)) then
         t=t+24.
      endif
      do j=i,i+4-1
        if(t.ge.tt(j).and.t.lt.tt(j+1)) then
           b(j,1)=1.
        else
           b(j,1)=0.
        endif
      enddo
      do j=2,4
        do k=i,i+4-j
          b(k,j)=(t-tt(k))/(tt(k+j-1)-tt(k))*b(k,j-1)
          b(k,j)=b(k,j)+(tt(k+j)-t)/(tt(k+j)-tt(k+1))*b(k+1,j-1)
        enddo
      enddo
*
      bspl4t=b(i,4)
*
      return
*
      end
*
******************************************************************
      function bspl2s(i,t1)
******************************************************************
      dimension ts(0:36),b(30,30)
*
      data ts/ 15,46,74,105,135,166,196,227,258,288,319,349,
     *        380,411,439,470,500,531,561,592,623,653,684,714,
     *        745,776,804,835,865,896,926,957,988,1018,1049,
     *        1079,1110/
*
      t=t1
      if(i.ge.0.and.t.lt.ts(i)) then
         t=t+365.
      endif
      do j=i,i+2-1
        if(t.ge.ts(j).and.t.lt.ts(j+1)) then
           b(j,1)=1.
        else
           b(j,1)=0.
        endif
      enddo
*
      do j=2,2
        do k=i,i+4-j
          b(k,j)=(t-ts(k))/(ts(k+j-1)-ts(k))*b(k,j-1)
          b(k,j)=b(k,j)+(ts(k+j)-t)/(ts(k+j)-ts(k+1))*b(k+1,j-1)
       enddo
      enddo
*
      bspl2s=b(i,2)
      return
      end
*
*******************************************************************
      function bspl2l(i,t1)
*******************************************************************
      dimension ts(0:6),b(30,30)
*
      data ts/ 94.,112.5,454.,472.5,814.,832.5,1174./
*
      t=t1
      if(i.ge.0.and.t.lt.ts(i)) then
         t=t+360.
      endif
      do j=i,i+2-1
        if(t.ge.ts(j).and.t.lt.ts(j+1)) then
           b(j,1)=1.
        else
           b(j,1)=0.
        endif
      enddo
*
      do j=2,2
        do k=i,i+2-j
          b(k,j)=(t-ts(k))/(ts(k+j-1)-ts(k))*b(k,j-1)
          b(k,j)=b(k,j)+(ts(k+j)-t)/(ts(k+j)-ts(k+1))*b(k+1,j-1)
        enddo
      enddo
*
      bspl2l=b(i,2)
*
      return
*
      end
*
*************************************************************************
      function bspl2f(i,t1)
*************************************************************************
       dimension ts(0:9),b(30,30),
     & ifnodes1(12),ifnodes2(12),ifnodes3(12)
       common/mflux/kf,n
*
      data ifnodes1 / 78, 77, 75, 79, 80, 77, 78, 80, 76, 81, 78, 78/
      data ifnodes2 /144,140,139,142,139,146,142,139,150,151,150,157/
      data ifnodes3 /214,211,201,208,213,220,203,209,213,215,236,221/ 
*
	ts(0)=ifnodes1(kf)
        ts(1)=ifnodes2(kf)
	ts(2)=ifnodes3(kf)
	ts(3)=ts(1)+367
        ts(4)=ts(2)+367
	ts(5)=ts(3)+367
	ts(6)=ts(4)+367
        ts(7)=ts(5)+367
	ts(8)=ts(6)+367
        ts(9)=ts(7)+367
*
      t=t1
      if(i.ge.0.and.t.lt.ts(i)) then
         t=t+367.
      endif
      do j=i,i+2-1
        if(t.ge.ts(j).and.t.lt.ts(j+1)) then
           b(j,1)=1.
        else
           b(j,1)=0.
        endif
      enddo
*
      do j=2,2
        do k=i,i+2-j
          b(k,j)=(t-ts(k))/(ts(k+j-1)-ts(k))*b(k,j-1)
          b(k,j)=b(k,j)+(ts(k+j)-t)/(ts(k+j)-ts(k+1))*b(k+1,j-1)
        enddo
      enddo
*
      bspl2f=b(i,2)
      return
      end
C
C
