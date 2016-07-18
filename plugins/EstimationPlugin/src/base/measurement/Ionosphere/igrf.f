c igrf.for, version number can be found at the end of this comment.
c-----------------------------------------------------------------------        
C
C Subroutines to compute IGRF parameters for IRI and all functions and 
C subroutines required for this computation, including:
C 	igrf_sub, FINDB0, SHELLG, STOER, FELDG, FELDCOF, GETSHC, INTERSHC, 
C 	EXTRASHC, INITIZE, GEODIP, SPHCAR, GEOMAG, and RECALC  
C
C UNIT number for reading the IGRF coefficients (in GETSHC) is 14
C
C Corrections:
C 11/01/91 SHELLG: lowest starting point for B0 search is 2  
C  1/27/92 Adopted to IGRF-91 coeffcients model
C  2/05/92 Reduce variable names: INTER(P)SHC,EXTRA(P)SHC,INITI(ALI)ZE
C  8/08/95 Updated to IGRF-45-95; new coeff. DGRF90, IGRF95, IGRF95S
C  5/31/00 Updated to IGRF-45-00; new coeff.: IGRF00, IGRF00s
C  4/16/15 Bugfix: Initialize IER to 0 in igrf_sub (Joseph Nicholas)
C  4/16/15 Load dgrf data from memory instead of file (Joseph Nicholas)
C  4/27/15 Error checking bugfix (Joseph Nicholas)
C-Version-mm/dd/yy-Description (Person reporting the correction)
C 2000.01 05/07/01 initial version
C 2000.02 07/11/01 replace feldi(xi,h) by feldi (P. Wilkinson)
C 2000.02 07/11/01 variables EGNR, AGNR,OGNR not used (P. Wilkinson)
c 2000.01 10/28/02 replace TAB/6 blanks, enforce 72/line (D. Simpson)
C 2000.02 11/08/02 change unit for coefficients to 14
C 2000.03 06/05/03 correct DIPL computation (V. Truhlik)
C 2005.00 04/25/05 CALL FELDI and DO 1111 I=1,7 (Alexey Petrov)
C 2005.01 11/10/05 added igrf_dip and geodip (MLAT) 
C 2005.02 11/10/05 updated to IGRF-10 version
C 2005.03 12/21/06 GH2(120) -> GH2(144)
C 2007.00 05/18/07 Release of IRI-2007
C
        subroutine igrf_sub(xlat,xlong,year,height,
     &          xl,icode,dipl,babs, IER)
c-----------------------------------------------------------------------        
c INPUT:
c    xlat      geodatic latitude in degrees
c    xlong     geodatic longitude in degrees
c    year      decimal year (year+month/12.0-0.5 or 
c                  year+day-of-year/365 or ../366 if leap year) 
c    height    height in km
c OUTPUT:
c    xl        L value
c    icode      =1  L is correct; =2  L is not correct;
c               =3  an approximation is used
c    dipl      dip latitude in degrees
c    babs      magnetic field strength in Gauss
c    IER       error   = 0 no error; != 0 having an error
c-----------------------------------------------------------------------        

      REAL              LATI,LONGI
      COMMON/IGRF1/     UMR,ERA,AQUAD,BQUAD
C
      IER = 0

      CALL INITIZE
        lati=xlat
        longi=xlong
c
C----------------CALCULATE PROFILES-----------------------------------
c
        CALL FELDCOF(YEAR,DIMO,IER)
        IF (IER .NE. 0) THEN
            RETURN
        ENDIF 

        CALL FELDG(LATI,LONGI,HEIGHT,BNORTH,BEAST,BDOWN,BABS)
        CALL SHELLG(LATI,LONGI,HEIGHT,DIMO,XL,ICODE,BAB1)
c        DIP=ASIN(BDOWN/BABS)/UMR
c       DEC=ASIN(BEAST/SQRT(BEAST*BEAST+BNORTH*BNORTH))/UMR
c        DIPL=ATAN(0.5*TAN(DIP*UMR))/UMR
       DIPL=ATAN(BDOWN/2.0/sqrt(BNORTH*BNORTH+BEAST*BEAST))/umr
      RETURN
      END
c
c
      subroutine igrf_dip(xlat,xlong,year,height,dip,dipl,ymodip, IER)
c-----------------------------------------------------------------------        
c INPUT:
c    xlat      geodatic latitude in degrees
c    xlong     geodatic longitude in degrees
c    year      decimal year (year+month/12.0-0.5 or 
c                  year+day-of-year/365 or ../366 if leap year) 
c    height    height in km
c OUTPUT:
c    dip       magnetic inclination (dip) in degrees
c    dipl      dip latitude in degrees
c    ymodip    modified dip latitude = asin{dip/sqrt[dip^2+cos(LATI)]}
c    IER       error  = 0 if no error; != 0 if having an error 
c-----------------------------------------------------------------------        

      COMMON/IGRF1/     UMR,ERA,AQUAD,BQUAD
C
      CALL INITIZE
c
C----------------CALCULATE PROFILES-----------------------------------
c
		xlati = xlat
		xlongi = xlong
		h = height
        CALL FELDCOF(YEAR,DIMO,IER)
        IF (IER .NE. 0) THEN
            RETURN
        ENDIF

        CALL FELDG(XLATI,XLONGI,H,BNORTH,BEAST,BDOWN,BABS)
        DIP=ASIN(BDOWN/BABS)
        dipdiv=DIP/SQRT(DIP*DIP+cos(XLATI*UMR))
        IF(ABS(dipdiv).GT.1.) dipdiv=SIGN(1.,dipdiv)
        SMODIP=ASIN(dipdiv)
        
c       DEC=ASIN(BEAST/SQRT(BEAST*BEAST+BNORTH*BNORTH))/UMR
c       DIPL1=ATAN(0.5*TAN(DIP))/UMR

      DIPL=ATAN(BDOWN/2.0/sqrt(BNORTH*BNORTH+BEAST*BEAST))/umr
      YMODIP=SMODIP/UMR                            
      DIP=DIP/UMR
      RETURN
      END
c
c
C SHELLIG.FOR, Version 2.0, January 1992
C
C 11/01/91 SHELLG: lowest starting point for B0 search is 2  
C  1/27/92 Adopted to IGRF-91 coeffcients model
C  2/05/92 Reduce variable-names: INTER(P)SHC,EXTRA(P)SHC,INITI(ALI)ZE
C  8/08/95 Updated to IGRF-45-95; new coeff. DGRF90, IGRF95, IGRF95S
C  5/31/00 Updated to IGRF-45-00; new coeff.: IGRF00, IGRF00s
C*********************************************************************
C  SUBROUTINES FINDB0, SHELLG, STOER, FELDG, FELDCOF, GETSHC,        *
C       INTERSHC, EXTRASHC, INITIZE                                  *
C*********************************************************************
C*********************************************************************
C
C
      SUBROUTINE FINDB0(STPS,BDEL,VALUE,BEQU,RR0)
C--------------------------------------------------------------------
C FINDS SMALLEST MAGNETIC FIELD STRENGTH ON FIELD LINE
C
C INPUT:   STPS   STEP SIZE FOR FIELD LINE TRACING
C       COMMON/FIDB0/
C          SP     DIPOLE ORIENTED COORDINATES FORM SHELLG; P(1,*),
C                 P(2,*), P(3,*) CLOSEST TO MAGNETIC EQUATOR 
C          BDEL   REQUIRED ACCURACY  = [ B(LAST) - BEQU ] / BEQU  
C                 B(LAST)  IS FIELD STRENGTH BEFORE BEQU
C
C OUTPUT:  VALUE  =.FALSE., IF BEQU IS NOT MINIMAL VALUE ON FIELD LINE
C          BEQU   MAGNETIC FIELD STRENGTH AT MAGNETIC EQUATOR
C          RR0    EQUATORIAL RADIUS NORMALIZED TO EARTH RADIUS
C          BDEL   FINAL ACHIEVED ACCURACY
C--------------------------------------------------------------------
      DIMENSION         P(8,4),SP(3)
      LOGICAL           VALUE
      COMMON/FIDB0/     SP
C
      STEP=STPS
      IRUN=0
7777  IRUN=IRUN+1
      IF(IRUN.GT.5) THEN
        VALUE=.FALSE.
        GOTO 8888
        ENDIF
C*********************FIRST THREE POINTS 
      P(1,2)=SP(1)
      P(2,2)=SP(2)
      P(3,2)=SP(3)
      STEP=-SIGN(STEP,P(3,2))
      CALL STOER(P(1,2),BQ2,R2)
      P(1,3)=P(1,2)+0.5*STEP*P(4,2)
      P(2,3)=P(2,2)+0.5*STEP*P(5,2)
      P(3,3)=P(3,2)+0.5*STEP
      CALL STOER(P(1,3),BQ3,R3)
      P(1,1)=P(1,2)-STEP*(2.*P(4,2)-P(4,3))
      P(2,1)=P(2,2)-STEP*(2.*P(5,2)-P(5,3))
      P(3,1)=P(3,2)-STEP
      CALL STOER(P(1,1),BQ1,R1)
      P(1,3)=P(1,2)+STEP*(20.*P(4,3)-3.*P(4,2)+P(4,1))/18.
      P(2,3)=P(2,2)+STEP*(20.*P(5,3)-3.*P(5,2)+P(5,1))/18.
      P(3,3)=P(3,2)+STEP
      CALL STOER(P(1,3),BQ3,R3)
C******************INVERT SENSE IF REQUIRED
      IF(BQ3.LE.BQ1) GOTO 2
        STEP=-STEP
        R3=R1
        BQ3=BQ1
        DO 1 I=1,5
                ZZ=P(I,1)
                P(I,1)=P(I,3)
1               P(I,3)=ZZ
C******************INITIALIZATION 
2       STEP12=STEP/12.
        VALUE=.TRUE.
        BMIN=1.E4
        BOLD=1.E4
C******************CORRECTOR (FIELD LINE TRACING)
        N=0
5555  P(1,3)=P(1,2)+STEP12*(5.*P(4,3)+8.*P(4,2)-P(4,1))
        N=N+1
      P(2,3)=P(2,2)+STEP12*(5.*P(5,3)+8.*P(5,2)-P(5,1))
C******************PREDICTOR (FIELD LINE TRACING)
      P(1,4)=P(1,3)+STEP12*(23.*P(4,3)-16.*P(4,2)+5.*P(4,1))
      P(2,4)=P(2,3)+STEP12*(23.*P(5,3)-16.*P(5,2)+5.*P(5,1))
      P(3,4)=P(3,3)+STEP
      CALL STOER(P(1,4),BQ3,R3)
        DO 1111 J=1,3
C        DO 1111 I=1,8
        DO 1111 I=1,7
1111    P(I,J)=P(I,J+1)
        B=SQRT(BQ3)
        IF(B.LT.BMIN) BMIN=B
        IF(B.LE.BOLD) THEN
                BOLD=B
                ROLD=1./R3
                SP(1)=P(1,4)
                SP(2)=P(2,4)
                SP(3)=P(3,4)
                GOTO 5555
                ENDIF
        IF(BOLD.NE.BMIN) THEN
                VALUE=.FALSE.
                ENDIF
        BDELTA=(B-BOLD)/BOLD
        IF(BDELTA.GT.BDEL) THEN
                STEP=STEP/10.
                GOTO 7777
                ENDIF
8888    RR0=ROLD
        BEQU=BOLD
        BDEL=BDELTA
        RETURN
        END
C
C
      SUBROUTINE SHELLG(GLAT,GLON,ALT,DIMO,FL,ICODE,B0)
c-----------------------------------------------------------------------        
C CALCULATES L-VALUE FOR SPECIFIED GEODAETIC COORDINATES, ALTITUDE
C AND GEMAGNETIC FIELD MODEL.
C REF: G. KLUGE, EUROPEAN SPACE OPERATIONS CENTER, INTERNAL NOTE 
C      NO. 67, 1970.
C      G. KLUGE, COMPUTER PHYSICS COMMUNICATIONS 3, 31-35, 1972
c-----------------------------------------------------------------------        
C CHANGES (D. BILITZA, NOV 87):
C   - USING CORRECT DIPOL MOMENT I.E.,DIFFERENT COMMON/MODEL/
C   - USING IGRF EARTH MAGNETIC FIELD MODELS FROM 1945 TO 1990
c-----------------------------------------------------------------------        
C  INPUT:  ENTRY POINT SHELLG
C             GLAT  GEODETIC LATITUDE IN DEGREES (NORTH)
C             GLON  GEODETIC LONGITUDE IN DEGREES (EAST)
C             ALT   ALTITUDE IN KM ABOVE SEA LEVEL
C
C          ENTRY POINT SHELLC
C             V(3)  CARTESIAN COORDINATES IN EARTH RADII (6371.2 KM)
C                     X-AXIS POINTING TO EQUATOR AT 0 LONGITUDE
C                     Y-AXIS POINTING TO EQUATOR AT 90 LONG.
C                     Z-AXIS POINTING TO NORTH POLE
C
C          DIMO     DIPOL MOMENT IN GAUSS (NORMALIZED TO EARTH RADIUS) 
C
C          COMMON 
C             X(3)    NOT USED
C             H(144)  FIELD MODEL COEFFICIENTS ADJUSTED FOR SHELLG
c-----------------------------------------------------------------------        
C  OUTPUT: FL           L-VALUE
C          ICODE        =1 NORMAL COMPLETION
C                       =2 UNPHYSICAL CONJUGATE POINT (FL MEANINGLESS)
C                       =3 SHELL PARAMETER GREATER THAN LIMIT UP TO
C                          WHICH ACCURATE CALCULATION IS REQUIRED;
C                          APPROXIMATION IS USED.
C          B0           MAGNETIC FIELD STRENGTH IN GAUSS
c-----------------------------------------------------------------------        
      DIMENSION         V(3),U(3,3),P(8,100),SP(3)
      COMMON/IGRF/      X(3),H(144)
      COMMON/FIDB0/     SP
      COMMON/IGRF1/     UMR,ERA,AQUAD,BQUAD
C
C-- RMIN, RMAX ARE BOUNDARIES FOR IDENTIFICATION OF ICODE=2 AND 3
C-- STEP IS STEP SIZE FOR FIELD LINE TRACING
C-- STEQ IS STEP SIZE FOR INTEGRATION
C 
      DATA RMIN,RMAX    /0.05,1.01/
      DATA STEP,STEQ    /0.20,0.03/
        BEQU=1.E10
C*****ENTRY POINT  SHELLG  TO BE USED WITH GEODETIC CO-ORDINATES
      RLAT=GLAT*UMR
      CT=SIN(RLAT)                                              
      ST=COS(RLAT)                                              
      D=SQRT(AQUAD-(AQUAD-BQUAD)*CT*CT)
      X(1)=(ALT+AQUAD/D)*ST/ERA
      X(3)=(ALT+BQUAD/D)*CT/ERA
      RLON=GLON*UMR
      X(2)=X(1)*SIN(RLON)                                       
      X(1)=X(1)*COS(RLON)                                       
      GOTO9                                                     
      ENTRY SHELLC(V,FL,B0)                                     
C*****ENTRY POINT  SHELLC  TO BE USED WITH CARTESIAN CO-ORDINATES
      X(1)=V(1)                                                  
      X(2)=V(2)                                                  
      X(3)=V(3)                                                  
C*****CONVERT TO DIPOL-ORIENTED CO-ORDINATES                     
      DATA U/                 +0.3511737,-0.9148385,-0.1993679,  
     A                        +0.9335804,+0.3583680,+0.0000000,  
     B                        +0.0714471,-0.1861260,+0.9799247/  
9     RQ=1./(X(1)*X(1)+X(2)*X(2)+X(3)*X(3))
      R3H=SQRT(RQ*SQRT(RQ))                                      
      P(1,2)=(X(1)*U(1,1)+X(2)*U(2,1)+X(3)*U(3,1))*R3H           
      P(2,2)=(X(1)*U(1,2)+X(2)*U(2,2)            )*R3H           
      P(3,2)=(X(1)*U(1,3)+X(2)*U(2,3)+X(3)*U(3,3))*RQ            
C*****FIRST THREE POINTS OF FIELD LINE                           
      STEP=-SIGN(STEP,P(3,2))                                    
      CALL STOER(P(1,2),BQ2,R2)                                  
      B0=SQRT(BQ2)                                               
      P(1,3)=P(1,2)+0.5*STEP*P(4,2)                              
      P(2,3)=P(2,2)+0.5*STEP*P(5,2)                              
      P(3,3)=P(3,2)+0.5*STEP                                     
      CALL STOER(P(1,3),BQ3,R3)                                  
      P(1,1)=P(1,2)-STEP*(2.*P(4,2)-P(4,3))                      
      P(2,1)=P(2,2)-STEP*(2.*P(5,2)-P(5,3))                      
      P(3,1)=P(3,2)-STEP                                         
      CALL STOER(P(1,1),BQ1,R1)                                  
      P(1,3)=P(1,2)+STEP*(20.*P(4,3)-3.*P(4,2)+P(4,1))/18.       
      P(2,3)=P(2,2)+STEP*(20.*P(5,3)-3.*P(5,2)+P(5,1))/18.       
      P(3,3)=P(3,2)+STEP                                         
      CALL STOER(P(1,3),BQ3,R3)                                  
C*****INVERT SENSE IF REQUIRED                                   
      IF(BQ3.LE.BQ1)GOTO2                                        
      STEP=-STEP                                                 
      R3=R1                                                      
      BQ3=BQ1                                                    
      DO 1 I=1,7                                                 
      ZZ=P(I,1)                                                  
      P(I,1)=P(I,3)                                              
1     P(I,3)=ZZ                                                  
C*****SEARCH FOR LOWEST MAGNETIC FIELD STRENGTH
2     IF(BQ1.LT.BEQU) THEN
        BEQU=BQ1
        IEQU=1
        ENDIF
      IF(BQ2.LT.BEQU) THEN
        BEQU=BQ2
        IEQU=2
        ENDIF
      IF(BQ3.LT.BEQU) THEN
        BEQU=BQ3
        IEQU=3
        ENDIF
C*****INITIALIZATION OF INTEGRATION LOOPS                        
      STEP12=STEP/12.
      STEP2=STEP+STEP                                            
      STEQ=SIGN(STEQ,STEP)                                       
      FI=0.                                                      
      ICODE=1                                                    
      ORADIK=0.                                                  
      OTERM=0.                                                   
      STP=R2*STEQ                                                
      Z=P(3,2)+STP                                               
      STP=STP/0.75
      P(8,1)=STEP2*(P(1,1)*P(4,1)+P(2,1)*P(5,1))                 
      P(8,2)=STEP2*(P(1,2)*P(4,2)+P(2,2)*P(5,2))
C*****MAIN LOOP (FIELD LINE TRACING)                             
      DO 3 N=3,3333                                              
C*****CORRECTOR (FIELD LINE TRACING)                             
      P(1,N)=P(1,N-1)+STEP12*(5.*P(4,N)+8.*P(4,N-1)-P(4,N-2))    
      P(2,N)=P(2,N-1)+STEP12*(5.*P(5,N)+8.*P(5,N-1)-P(5,N-2))    
C*****PREPARE EXPANSION COEFFICIENTS FOR INTERPOLATION           
C*****OF SLOWLY VARYING QUANTITIES                               
      P(8,N)=STEP2*(P(1,N)*P(4,N)+P(2,N)*P(5,N))                 
      C0=P(1,N-1)**2+P(2,N-1)**2                                 
      C1=P(8,N-1)                                                
      C2=(P(8,N)-P(8,N-2))*0.25                                  
      C3=(P(8,N)+P(8,N-2)-C1-C1)/6.0
      D0=P(6,N-1)                                                
      D1=(P(6,N)-P(6,N-2))*0.5
      D2=(P(6,N)+P(6,N-2)-D0-D0)*0.5                             
      E0=P(7,N-1)
      E1=(P(7,N)-P(7,N-2))*0.5                                   
      E2=(P(7,N)+P(7,N-2)-E0-E0)*0.5                             
C*****INNER LOOP (FOR QUADRATURE)                                
4     T=(Z-P(3,N-1))/STEP                                        
      IF(T.GT.1.)GOTO5                                           
      HLI=0.5*(((C3*T+C2)*T+C1)*T+C0)                            
      ZQ=Z*Z
      R=HLI+SQRT(HLI*HLI+ZQ)
      IF(R.LE.RMIN)GOTO30                               
      RQ=R*R
      FF=SQRT(1.+3.*ZQ/RQ)                              
      RADIK=B0-((D2*T+D1)*T+D0)*R*RQ*FF                 
      IF(R-RMAX)44,44,45                                
45    ICODE=2                                           
      RADIK=RADIK-12.*(R-RMAX)**2                       
44    IF(RADIK+RADIK.LE.ORADIK) GOTO 10
      TERM=SQRT(RADIK)*FF*((E2*T+E1)*T+E0)/(RQ+ZQ)      
      FI=FI+STP*(OTERM+TERM)                            
      ORADIK=RADIK                                      
      OTERM=TERM                                        
      STP=R*STEQ                                        
      Z=Z+STP                                           
      GOTO4                                             
C*****PREDICTOR (FIELD LINE TRACING)                    
5     P(1,N+1)=P(1,N)+STEP12*(23.*P(4,N)-16.*P(4,N-1)+5.*P(4,N-2))  
      P(2,N+1)=P(2,N)+STEP12*(23.*P(5,N)-16.*P(5,N-1)+5.*P(5,N-2))  
      P(3,N+1)=P(3,N)+STEP                                          
      CALL STOER(P(1,N+1),BQ3,R3)                                   
C*****SEARCH FOR LOWEST MAGNETIC FIELD STRENGTH
      IF(BQ3.LT.BEQU) THEN
        IEQU=N+1
        BEQU=BQ3
        ENDIF
3     CONTINUE
10    IF(IEQU.lt.2) IEQU=2 
      SP(1)=P(1,IEQU-1)
      SP(2)=P(2,IEQU-1)
      SP(3)=P(3,IEQU-1)
      IF(ORADIK.LT.1E-15)GOTO11                                     
      FI=FI+STP/0.75*OTERM*ORADIK/(ORADIK-RADIK)              
C
C-- The minimal allowable value of FI was changed from 1E-15 to 1E-12,
C-- because 1E-38 is the minimal allowable arg. for ALOG in our envir.
C-- D. Bilitza, Nov 87.
C
11          FI=0.5*ABS(FI)/SQRT(B0)+1E-12                       
C
C*****COMPUTE L FROM B AND I.  SAME AS CARMEL IN INVAR.  
C
C-- Correct dipole moment is used here. D. Bilitza, Nov 87.
C
      DIMOB0=DIMO/B0
      arg1=alog(FI)
      arg2=alog(DIMOB0)
c      arg = FI*FI*FI/DIMOB0
c      if(abs(arg).gt.88.0) arg=88.0
      XX=3*arg1-arg2
      IF(XX.GT.23.0) GOTO 776   
      IF(XX.GT.11.7) GOTO 775  
      IF(XX.GT.+3.0) GOTO 774    
      IF(XX.GT.-3.0) GOTO 773   
      IF(XX.GT.-22.) GOTO 772  
  771 GG=3.33338E-1*XX+3.0062102E-1                                 
      GOTO777                                                          
  772 GG=((((((((-8.1537735E-14*XX+8.3232531E-13)*XX+1.0066362E-9)*XX+  
     &  8.1048663E-8)*XX+3.2916354E-6)*XX+8.2711096E-5)*XX+ 
     &  1.3714667E-3)*XX+1.5017245E-2)*XX+4.3432642E-1)*XX+
     &  6.2337691E-1                 
      GOTO777                                                           
  773 GG=((((((((2.6047023E-10*XX+2.3028767E-9)*XX-2.1997983E-8)*XX-    
     &  5.3977642E-7)*XX-3.3408822E-6)*XX+3.8379917E-5)*XX+ 
     &  1.1784234E-3)*XX+1.4492441E-2)*XX+4.3352788E-1)*XX+
     &  6.228644E-1                  
      GOTO777                                                           
  774 GG=((((((((6.3271665E-10*XX-3.958306E-8)*XX+9.9766148E-07)*XX-    
     &  1.2531932E-5)*XX+7.9451313E-5)*XX-3.2077032E-4)*XX+ 
     &  2.1680398E-3)*XX+1.2817956E-2)*XX+4.3510529E-1)*XX+
     &  6.222355E-1                  
      GOTO777                                                           
  775 GG=(((((2.8212095E-8*XX-3.8049276E-6)*XX+2.170224E-4)*XX-
     &  6.7310339E-3)*XX+1.2038224E-1)*XX-1.8461796E-1)*XX+
     &  2.0007187E0             
      GOTO777                                                           
  776 GG=XX-3.0460681E0                                                 
  777 FL=EXP(ALOG((1.+EXP(GG))*DIMOB0)/3.0)
      RETURN                                                            
C*****APPROXIMATION FOR HIGH VALUES OF L.                               
30    ICODE=3                                                           
      T=-P(3,N-1)/STEP                                                  
      FL=1./(ABS(((C3*T+C2)*T+C1)*T+C0)+1E-15)                          
      RETURN                                                            
      END                                                               
C
C
      SUBROUTINE STOER(P,BQ,R)                                          
C*******************************************************************
C* SUBROUTINE USED FOR FIELD LINE TRACING IN SHELLG                *
C* CALLS ENTRY POINT FELDI IN GEOMAGNETIC FIELD SUBROUTINE FELDG   *
C*******************************************************************
      DIMENSION         P(7),U(3,3)
      COMMON/IGRF/      XI(3),H(144)
C*****XM,YM,ZM  ARE GEOMAGNETIC CARTESIAN INVERSE CO-ORDINATES          
      ZM=P(3)                                                           
      FLI=P(1)*P(1)+P(2)*P(2)+1E-15
      R=0.5*(FLI+SQRT(FLI*FLI+(ZM+ZM)**2))
      RQ=R*R
      WR=SQRT(R)                                                        
      XM=P(1)*WR                                                        
      YM=P(2)*WR                                                        
C*****TRANSFORM TO GEOGRAPHIC CO-ORDINATE SYSTEM                        
      DATA U/                 +0.3511737,-0.9148385,-0.1993679,         
     A                        +0.9335804,+0.3583680,+0.0000000,         
     B                        +0.0714471,-0.1861260,+0.9799247/         
      XI(1)=XM*U(1,1)+YM*U(1,2)+ZM*U(1,3)                               
      XI(2)=XM*U(2,1)+YM*U(2,2)+ZM*U(2,3)                               
      XI(3)=XM*U(3,1)          +ZM*U(3,3)                               
C*****COMPUTE DERIVATIVES                                               
c      CALL FELDI(XI,H)                                                  
      CALL FELDI
      Q=H(1)/RQ                                                         
      DX=H(3)+H(3)+Q*XI(1)                                              
      DY=H(4)+H(4)+Q*XI(2)                                              
      DZ=H(2)+H(2)+Q*XI(3)                                              
C*****TRANSFORM BACK TO GEOMAGNETIC CO-ORDINATE SYSTEM                  
      DXM=U(1,1)*DX+U(2,1)*DY+U(3,1)*DZ                                 
      DYM=U(1,2)*DX+U(2,2)*DY                                           
      DZM=U(1,3)*DX+U(2,3)*DY+U(3,3)*DZ                                 
      DR=(XM*DXM+YM*DYM+ZM*DZM)/R                                       
C*****FORM SLOWLY VARYING EXPRESSIONS                                   
      P(4)=(WR*DXM-0.5*P(1)*DR)/(R*DZM)                                 
      P(5)=(WR*DYM-0.5*P(2)*DR)/(R*DZM)                                 
      DSQ=RQ*(DXM*DXM+DYM*DYM+DZM*DZM)
      BQ=DSQ*RQ*RQ
      P(6)=SQRT(DSQ/(RQ+3.*ZM*ZM))                                      
      P(7)=P(6)*(RQ+ZM*ZM)/(RQ*DZM)                                     
      RETURN                                                            
      END                                                               
C
C
      SUBROUTINE FELDG(GLAT,GLON,ALT,BNORTH,BEAST,BDOWN,BABS)           
c-----------------------------------------------------------------------        
C CALCULATES EARTH MAGNETIC FIELD FROM SPHERICAL HARMONICS MODEL
C REF: G. KLUGE, EUROPEAN SPACE OPERATIONS CENTRE, INTERNAL NOTE 61, 
C      1970.
c-----------------------------------------------------------------------        
C CHANGES (D. BILITZA, NOV 87):
C   - FIELD COEFFICIENTS IN BINARY DATA FILES INSTEAD OF BLOCK DATA
C   - CALCULATES DIPOL MOMENT
c-----------------------------------------------------------------------        
C  INPUT:  ENTRY POINT FELDG
C               GLAT  GEODETIC LATITUDE IN DEGREES (NORTH)
C               GLON  GEODETIC LONGITUDE IN DEGREES (EAST)
C               ALT   ALTITUDE IN KM ABOVE SEA LEVEL
C
C          ENTRY POINT FELDC
C               V(3)  CARTESIAN COORDINATES IN EARTH RADII (6371.2 KM)
C                       X-AXIS POINTING TO EQUATOR AT 0 LONGITUDE
C                       Y-AXIS POINTING TO EQUATOR AT 90 LONG.
C                       Z-AXIS POINTING TO NORTH POLE
C
C          COMMON BLANK AND ENTRY POINT FELDI ARE NEEDED WHEN USED
C            IN CONNECTION WITH L-CALCULATION PROGRAM SHELLG.
C       
C          COMMON /MODEL/ AND /IGRF1/
C               UMR     = ATAN(1.0)*4./180.   <DEGREE>*UMR=<RADIANT>
C               ERA     EARTH RADIUS FOR NORMALIZATION OF CARTESIAN 
C                       COORDINATES (6371.2 KM)
C               AQUAD, BQUAD   SQUARE OF MAJOR AND MINOR HALF AXIS OF 
C                       EARTH ELLIPSOID AS RECOMMENDED BY INTERNAT. 
C                       ASTRONOMICAL UNION (6378.160, 6356.775 KM).
C               NMAX    MAXIMUM ORDER OF SPHERICAL HARMONICS
C               TIME    YEAR (DECIMAL: 1973.5) FOR WHICH MAGNETIC 
C                       FIELD IS TO BE CALCULATED
C               G(M)    NORMALIZED FIELD COEFFICIENTS (SEE FELDCOF)
C                       M=NMAX*(NMAX+2)
c-----------------------------------------------------------------------        
C  OUTPUT: BABS   MAGNETIC FIELD STRENGTH IN GAUSS
C          BNORTH, BEAST, BDOWN   COMPONENTS OF THE FIELD WITH RESPECT
C                 TO THE LOCAL GEODETIC COORDINATE SYSTEM, WITH AXIS
C                 POINTING IN THE TANGENTIAL PLANE TO THE NORTH, EAST
C                 AND DOWNWARD.   
C-----------------------------------------------------------------------
      DIMENSION         V(3),B(3)   
      CHARACTER*12      NAME
      COMMON/IGRF/      XI(3),H(144)
      COMMON/MODEL/     NAME,NMAX,TIME,G(144)  
      COMMON/IGRF1/     UMR,ERA,AQUAD,BQUAD
C
C-- IS RECORDS ENTRY POINT
C
C*****ENTRY POINT  FELDG  TO BE USED WITH GEODETIC CO-ORDINATES         
      IS=1                                                              
      RLAT=GLAT*UMR
      CT=SIN(RLAT)                                                      
      ST=COS(RLAT)                                                      
      D=SQRT(AQUAD-(AQUAD-BQUAD)*CT*CT)                                 
      RLON=GLON*UMR
      CP=COS(RLON)                                                      
      SP=SIN(RLON)                                                      
       ZZZ=(ALT+BQUAD/D)*CT/ERA
       RHO=(ALT+AQUAD/D)*ST/ERA
       XXX=RHO*CP                                                       
       YYY=RHO*SP                                                       
      GOTO10                                                            

C*****ENTRY POINT  FELDC  TO BE USED WITH CARTESIAN CO-ORDINATES        
      ENTRY FELDC(V,B)                                                  
      IS=2                                                              
      XXX=V(1)                                                          
      YYY=V(2)                                                          
      ZZZ=V(3)                                                          
10    RQ=1./(XXX*XXX+YYY*YYY+ZZZ*ZZZ) 
      XI(1)=XXX*RQ                                                      
      XI(2)=YYY*RQ                                                      
      XI(3)=ZZZ*RQ                                                      
      GOTO20                                                            

C*****ENTRY POINT  FELDI  USED FOR L COMPUTATION                        
      ENTRY FELDI                                                       
      IS=3                                                              
20    IHMAX=NMAX*NMAX+1                                                 
      LAST=IHMAX+NMAX+NMAX                                              
      IMAX=NMAX+NMAX-1                                                  
      DO 8 I=IHMAX,LAST                                                 
8     H(I)=G(I)                                                         
      DO 6 K=1,3,2                                                      
      I=IMAX                                                            
      IH=IHMAX                                                          
1     IL=IH-I                                                           
      F=2./FLOAT(I-K+2)                                                 
      X=XI(1)*F                                                         
      Y=XI(2)*F                                                         
      Z=XI(3)*(F+F)                                                     
      I=I-2                                                             
      IF(I-1)5,4,2                                                      
      	                                                      
2     DO 3 M=3,I,2                                                      
      		H(IL+M+1)=G(IL+M+1)+Z*H(IH+M+1)+X*(H(IH+M+3)-          
     A                H(IH+M-1))-Y*(H(IH+M+2)+H(IH+M-2))           
3     		H(IL+M)=G(IL+M)+Z*H(IH+M)+X*(H(IH+M+2)-                
     A                H(IH+M-2))+Y*(H(IH+M+3)+H(IH+M-1))
                      
4     H(IL+2)=G(IL+2)+Z*H(IH+2)+X*H(IH+4)-Y*(H(IH+3)+H(IH))             
      H(IL+1)=G(IL+1)+Z*H(IH+1)+Y*H(IH+4)+X*(H(IH+3)-H(IH))             
5     H(IL)=G(IL)+Z*H(IH)+2.*(X*H(IH+1)+Y*H(IH+2))                      
      IH=IL                                                             
      IF(I.GE.K)GOTO1                                                   
6     CONTINUE                                                          
                                                          
      IF(IS.EQ.3)RETURN                                                 
                                                      
      S=.5*H(1)+2.*(H(2)*XI(3)+H(3)*XI(1)+H(4)*XI(2))                   
      T=(RQ+RQ)*SQRT(RQ)                                                
      BXXX=T*(H(3)-S*XXX)                                               
      BYYY=T*(H(4)-S*YYY)                                               
      BZZZ=T*(H(2)-S*ZZZ)                                               
                                                    
      IF(IS.EQ.2)GOTO7                                                  
      BABS=SQRT(BXXX*BXXX+BYYY*BYYY+BZZZ*BZZZ)
      BEAST=BYYY*CP-BXXX*SP                                             
      BRHO=BYYY*SP+BXXX*CP                                              
      BNORTH=BZZZ*ST-BRHO*CT                                            
      BDOWN=-BZZZ*CT-BRHO*ST                                            
      RETURN                                                            
                                                                 
7     B(1)=BXXX                                                         
      B(2)=BYYY                                                         
      B(3)=BZZZ                                                         
      RETURN                                                            
      END                                                               
C
C
        SUBROUTINE FELDCOF(YEAR,DIMO, IER)
c-----------------------------------------------------------------------        
C  DETERMINES COEFFICIENTS AND DIPOL MOMENT FROM IGRF MODELS
C
C       INPUT:  YEAR    DECIMAL YEAR FOR WHICH GEOMAGNETIC FIELD IS TO
C                       BE CALCULATED
C       OUTPUT: DIMO    GEOMAGNETIC DIPOL MOMENT IN GAUSS (NORMALIZED 
C                       TO EARTH'S RADIUS) AT THE TIME (YEAR)
C  D. BILITZA, NSSDC, GSFC, CODE 633, GREENBELT, MD 20771, 
C       (301)286-9536   NOV 1987.
C  ### updated to IGRF-10 version -dkb- 11/10/2005
c-----------------------------------------------------------------------        
C ### FILMOD, DTEMOD arrays +1
        CHARACTER*12    FIL1
        DIMENSION       GH1(144),GH2(144),GHA(144)
        DOUBLE PRECISION X,F0,F 
        COMMON/MODEL/   FIL1,NMAX,TIME,GH1
        COMMON/IGRF1/   UMR,ERAD,AQUAD,BQUAD
        include 'common.inc'
C
C ### numye = numye + 1 ; is number of years represented by IGRF
C
        NUMYE=13
C
C  IS=0 FOR SCHMIDT NORMALIZATION   IS=1 GAUSS NORMALIZATION
C  IU  IS INPUT UNIT NUMBER FOR IGRF COEFFICIENT SETS
C
        IU = 14
        IS = 0
C-- DETERMINE IGRF-YEARS FOR INPUT-YEAR
        TIME = YEAR
        IYEA = INT(YEAR/5.)*5
        L = (IYEA - 1945)/5 + 1
        IF(L.LT.1) L=1
        IF(L.GT.NUMYE) L=NUMYE         
        DTE1 = DTEMOD(L)   
        DTE2 = DTEMOD(L+1) 
C-- GET IGRF COEFFICIENTS FOR THE BOUNDARY YEARS
        CALL GETSHC (IU, L, NMAX1, ERAD, GH1, IER)  
        IF (IER .NE. 0) THEN
           RETURN
        END IF
        CALL GETSHC (IU, L+1, NMAX2, ERAD, GH2, IER)  
        IF (IER .NE. 0) THEN
           RETURN
        END IF
C-- DETERMINE IGRF COEFFICIENTS FOR YEAR
        IF (L .LE. NUMYE-1) THEN                        
          CALL INTERSHC (YEAR, DTE1, NMAX1, GH1, DTE2, 
     1          NMAX2, GH2, NMAX, GHA)                        
        ELSE               
          CALL EXTRASHC (YEAR, DTE1, NMAX1, GH1, NMAX2,     
     1          GH2, NMAX, GHA)                                    
        ENDIF 
C-- DETERMINE MAGNETIC DIPOL MOMENT AND COEFFIECIENTS G
        F0=0.D0
        DO 1234 J=1,3
           F = GHA(J) * 1.D-5
           F0 = F0 + F * F
1234    CONTINUE
        DIMO = DSQRT(F0)

        GH1(1) =  0.0
        I=2          
        F0=1.D-5                
        IF(IS.EQ.0) F0=-F0 
        SQRT2=SQRT(2.)      

      DO 9 N=1,NMAX           
        X = N
        F0 = F0 * X * X / (4.D0 * X - 2.D0)               
        IF(IS.EQ.0) F0 = F0 * (2.D0 * X - 1.D0) / X
        F = F0 * 0.5D0                                    
        IF(IS.EQ.0) F = F * SQRT2
        GH1(I) = GHA(I-1) * F0
        I = I+1                                         
      DO 9 M=1,N                                    
        F = F * (X + M) / (X - M + 1.D0)                 
        IF(IS.EQ.0) F = F * DSQRT((X - M + 1.D0) / (X + M))             
        GH1(I) = GHA(I-1) * F
        GH1(I+1) = GHA(I) * F
        I=I+2
9     CONTINUE                                          
        RETURN
        END
C
C
        SUBROUTINE GETSHC (IU, IFILE, NMAX, ERAD, GH, IER)
C ===============================================================               
C                                                                               
C       Version 1.01                                                 
C                                                                               
C       Reads spherical harmonic coefficients from the specified     
C       file into an array.                                          
C                                                                               
C       Input:                                                       
C           IU    - Logical unit number                              
C           IFILE - File index
C                                                                               
C       Output:                                                      
C           NMAX  - Maximum degree and order of model                
C           ERAD  - Earth's radius associated with the spherical     
C                   harmonic coefficients, in the same units as      
C                   elevation                                        
C           GH    - Schmidt quasi-normal internal spherical          
C                   harmonic coefficients                            
C           IER   - Error number: =  0, no error                     
C                                 = -2, records out of order         
C                                 = FORTRAN run-time error number    
C                                                                    
C       A. Zunde                                                     
C       USGS, MS 964, Box 25046 Federal Center, Denver, CO  80225    
C                                                                               
C ===============================================================               
                                                                                
        INCLUDE 'common.inc'
        DIMENSION       GH(*)                                        

        IF (IFILE < 1 .OR. IFILE > MAXFILES_SHC) THEN
            IER = -3
            GOTO 999
        END IF

        NMAX = nmax_shc(IFILE)

        I = 0                                                        
        IREC = 0
        DO 2211 NN = 1, NMAX                                              
            DO 2233 MM = 0, NN
                IREC = IREC + 1
                IF (IREC > MAXRECORDS_SHC) THEN
                    IER = -4
                    GOTO 999
                END IF
                N = NINT(shcfiles(1, IREC, IFILE))
                M = NINT(shcfiles(2, IREC, IFILE))
                G = shcfiles(3, IREC, IFILE)
                H = shcfiles(4, IREC, IFILE)
                IF (NN .NE. N .OR. MM .NE. M) THEN                   
                    IER = -2                                         
                    GOTO 999                                         
                ENDIF                                                
                I = I + 1                                            
                GH(I) = G                                            
                IF (M .NE. 0) THEN                                   
                    I = I + 1                                        
                    GH(I) = H                                        
                ENDIF                                                
2233        CONTINUE                                                    
2211    CONTINUE
        GOTO 998                                                        
c999     print *, 'Error in GETSHC function: file read error !!!'
999
998     RETURN                                                   
        END                                                          
C
C
        SUBROUTINE INTERSHC (DATE, DTE1, NMAX1, GH1, DTE2,          
     1                        NMAX2, GH2, NMAX, GH)                  
                                                                                
C ===============================================================               
C                                                                               
C       Version 1.01                                                 
C                                                                               
C       Interpolates linearly, in time, between two spherical        
C       harmonic models.                                             
C                                                                               
C       Input:                                                       
C           DATE  - Date of resulting model (in decimal year)        
C           DTE1  - Date of earlier model                            
C           NMAX1 - Maximum degree and order of earlier model        
C           GH1   - Schmidt quasi-normal internal spherical          
C                   harmonic coefficients of earlier model           
C           DTE2  - Date of later model                              
C           NMAX2 - Maximum degree and order of later model          
C           GH2   - Schmidt quasi-normal internal spherical          
C                   harmonic coefficients of later model             
C                                                                               
C       Output:                                                      
C           GH    - Coefficients of resulting model                  
C           NMAX  - Maximum degree and order of resulting model      
C                                                                               
C       A. Zunde                                                     
C       USGS, MS 964, Box 25046 Federal Center, Denver, CO  80225    
C                                                                               
C ===============================================================               
                                                                                
        DIMENSION       GH1(*), GH2(*), GH(*)                        
                                                                                
C ---------------------------------------------------------------               
C       The coefficients (GH) of the resulting model, at date        
C       DATE, are computed by linearly interpolating between the     
C       coefficients of the earlier model (GH1), at date DTE1,       
C       and those of the later model (GH2), at date DTE2. If one     
C       model is smaller than the other, the interpolation is        
C       performed with the missing coefficients assumed to be 0.     
C ---------------------------------------------------------------               
                                                                                
        FACTOR = (DATE - DTE1) / (DTE2 - DTE1)                       
                                                                                
        IF (NMAX1 .EQ. NMAX2) THEN                                   
            K = NMAX1 * (NMAX1 + 2)                                  
            NMAX = NMAX1                                             
        ELSE IF (NMAX1 .GT. NMAX2) THEN                              
            K = NMAX2 * (NMAX2 + 2)                                  
            L = NMAX1 * (NMAX1 + 2)                                  
            DO 1122 I = K + 1, L                                          
1122            GH(I) = GH1(I) + FACTOR * (-GH1(I))                  
            NMAX = NMAX1                                             
        ELSE                                                         
            K = NMAX1 * (NMAX1 + 2)                                  
            L = NMAX2 * (NMAX2 + 2)                                  
            DO 1133 I = K + 1, L                                          
1133            GH(I) = FACTOR * GH2(I)                              
            NMAX = NMAX2                                             
        ENDIF                                                        
                                                                                
        DO 1144 I = 1, K                                                  
1144        GH(I) = GH1(I) + FACTOR * (GH2(I) - GH1(I))              
                                                                                
        RETURN                                                       
        END                                                          
C
C
        SUBROUTINE EXTRASHC (DATE, DTE1, NMAX1, GH1, NMAX2,           
     1                        GH2, NMAX, GH)                           
                                                                                
C ===============================================================               
C                                                                               
C       Version 1.01                                                   
C                                                                               
C       Extrapolates linearly a spherical harmonic model with a        
C       rate-of-change model.                                          
C                                                                               
C       Input:                                                         
C           DATE  - Date of resulting model (in decimal year)          
C           DTE1  - Date of base model                                 
C           NMAX1 - Maximum degree and order of base model             
C           GH1   - Schmidt quasi-normal internal spherical            
C                   harmonic coefficients of base model                
C           NMAX2 - Maximum degree and order of rate-of-change         
C                   model                                              
C           GH2   - Schmidt quasi-normal internal spherical            
C                   harmonic coefficients of rate-of-change model      
C                                                                               
C       Output:                                                        
C           GH    - Coefficients of resulting model                    
C           NMAX  - Maximum degree and order of resulting model        
C                                                                               
C       A. Zunde                                                       
C       USGS, MS 964, Box 25046 Federal Center, Denver, CO  80225      
C                                                                               
C ===============================================================               
                                                                                
        DIMENSION       GH1(*), GH2(*), GH(*)                          
                                                                                
C ---------------------------------------------------------------               
C       The coefficients (GH) of the resulting model, at date          
C       DATE, are computed by linearly extrapolating the coef-         
C       ficients of the base model (GH1), at date DTE1, using          
C       those of the rate-of-change model (GH2), at date DTE2. If      
C       one model is smaller than the other, the extrapolation is      
C       performed with the missing coefficients assumed to be 0.       
C ---------------------------------------------------------------               
                                                                                
        FACTOR = (DATE - DTE1)                                         
                                                                                
        IF (NMAX1 .EQ. NMAX2) THEN                                     
            K = NMAX1 * (NMAX1 + 2)                                    
            NMAX = NMAX1                                               
        ELSE IF (NMAX1 .GT. NMAX2) THEN                                
            K = NMAX2 * (NMAX2 + 2)                                    
            L = NMAX1 * (NMAX1 + 2)                                    
            DO 1155 I = K + 1, L                                            
1155            GH(I) = GH1(I)                                         
            NMAX = NMAX1                                               
        ELSE                                                           
            K = NMAX1 * (NMAX1 + 2)                                    
            L = NMAX2 * (NMAX2 + 2)                                    
            DO 1166 I = K + 1, L                                            
1166            GH(I) = FACTOR * GH2(I)                                
            NMAX = NMAX2                                               
        ENDIF                                                          
                                                                                
        DO 1177 I = 1, K                                                    
1177        GH(I) = GH1(I) + FACTOR * GH2(I)                           
                                                                                
        RETURN                                                         
        END                                                            
C
C
        SUBROUTINE INITIZE
C----------------------------------------------------------------
C Initializes the parameters in COMMON/IGRF1/
C
C       UMR     = ATAN(1.0)*4./180.   <DEGREE>*UMR=<RADIANT>
C       ERA     EARTH RADIUS FOR NORMALIZATION OF CARTESIAN 
C                       COORDINATES (6371.2 KM) 
C       EREQU   MAJOR HALF AXIS FOR EARTH ELLIPSOID (6378.160 KM)
C       ERPOL   MINOR HALF AXIS FOR EARTH ELLIPSOID (6356.775 KM)
C       AQUAD   SQUARE OF MAJOR HALF AXIS FOR EARTH ELLIPSOID
C       BQUAD   SQUARE OF MINOR HALF AXIS FOR EARTH ELLIPSOID
C
C ERA, EREQU and ERPOL as recommended by the INTERNATIONAL 
C ASTRONOMICAL UNION .
C-----------------------------------------------------------------
        COMMON/IGRF1/   UMR,ERA,AQUAD,BQUAD
        ERA=6371.2
        EREQU=6378.16
        ERPOL=6356.775
        AQUAD=EREQU*EREQU
        BQUAD=ERPOL*ERPOL
        UMR=ATAN(1.0)*4./180.
        RETURN
        END
C
C
      SUBROUTINE GEODIP(IYR,SLA,SLO,DLA,DLO,J)

C  Calculates dipole geomagnetic coordinates from geocentric coordinates
C  or vice versa.

C                     J=0           J=1
C		INPUT:     J,SLA,SLO     J,DLA,DLO
C		OUTPUT:     DLA,DLO       SLA,SLO

C  Last revision: November 2005 (Vladimir Papitashvili)
C  The code is modifed from GEOCOR written by V.Popov and V.Papitashvili
C  in mid-1980s. 

         COMMON /CONST/UMR 

C  Earth's radius (km) RE = 6371.2

C  The radius of the sphere to compute the coordinates (in Re)
C        RH = (RE + HI)/RE
         R = 1.

         if(j.gt.0) goto 1234
        
         COL = (90.- SLA)*UMR
         RLO = SLO*UMR
      CALL SPHCAR(R,COL,RLO,X,Y,Z,1)
      CALL GEOMAG(X,Y,Z,XM,YM,ZM,1,IYR)
      CALL SPHCAR(RM,TH,PF,XM,YM,ZM,-1)
         SZM = ZM
         DLO = PF/UMR
         DCO = TH/UMR
         DLA = 90.- DCO
         RETURN
          
1234     continue
      COL = (90.- DLA)*UMR
      RLO = DLO*UMR
      CALL SPHCAR(R,COL,RLO,XM,YM,ZM,1)
      CALL GEOMAG(X,Y,Z,XM,YM,ZM,-1,IYR)
      CALL SPHCAR(RM,TH,PF,X,Y,Z,-1)
        SZM = ZM
        SLO = PF/UMR
        SCO = TH/UMR
        SLA = 90.- SCO

      RETURN
      END

C  *********************************************************************

      SUBROUTINE SPHCAR(R,TETA,PHI,X,Y,Z,J)

C   CONVERTS SPHERICAL COORDS INTO CARTESIAN ONES AND VICA VERSA
C    (TETA AND PHI IN RADIANS).

C                  J>0            J<0
C-----INPUT:   J,R,TETA,PHI     J,X,Y,Z
C----OUTPUT:      X,Y,Z        R,TETA,PHI

C  AUTHOR: NIKOLAI A. TSYGANENKO, INSTITUTE OF PHYSICS, ST.-PETERSBURG
C      STATE UNIVERSITY, STARY PETERGOF 198904, ST.-PETERSBURG, RUSSIA
C      (now the NASA Goddard Space Fligth Center, Greenbelt, Maryland)

        IMPLICIT NONE

        REAL R,TETA,PHI,X,Y,Z,SQ

        INTEGER J

      IF(J.GT.0) GOTO 3
      SQ=X**2+Y**2
      R=SQRT(SQ+Z**2)
      IF (SQ.NE.0.) GOTO 2
      PHI=0.
      IF (Z.LT.0.) GOTO 1
      TETA=0.
      RETURN
  1   TETA=3.141592654
      RETURN
  2   SQ=SQRT(SQ)
      PHI=ATAN2(Y,X)
      TETA=ATAN2(SQ,Z)
      IF (PHI.LT.0.) PHI=PHI+6.28318531
      RETURN
  3   SQ=R*SIN(TETA)
      X=SQ*COS(PHI)
      Y=SQ*SIN(PHI)
      Z=R*COS(TETA)

      RETURN
      END

C  *********************************************************************

      SUBROUTINE GEOMAG(XGEO,YGEO,ZGEO,XMAG,YMAG,ZMAG,J,IYR)

C CONVERTS GEOCENTRIC (GEO) TO DIPOLE (MAG) COORDINATES OR VICA VERSA.
C IYR IS YEAR NUMBER (FOUR DIGITS).

C                           J>0                J<0
C-----INPUT:  J,XGEO,YGEO,ZGEO,IYR   J,XMAG,YMAG,ZMAG,IYR
C-----OUTPUT:    XMAG,YMAG,ZMAG        XGEO,YGEO,ZGEO

C  AUTHOR: NIKOLAI A. TSYGANENKO, INSTITUTE OF PHYSICS, ST.-PETERSBURG
C      STATE UNIVERSITY, STARY PETERGOF 198904, ST.-PETERSBURG, RUSSIA
C      (now the NASA Goddard Space Fligth Center, Greenbelt, Maryland)

        IMPLICIT NONE

        REAL XGEO,YGEO,ZGEO,XMAG,YMAG,ZMAG,ST0,CT0,SL0,CL0,CTCL,
     *       STCL,CTSL,STSL,AB(19),BB(8)

        INTEGER J,IYR,K,IY,II

      COMMON/C1/ ST0,CT0,SL0,CL0,CTCL,STCL,CTSL,STSL,AB,K,IY,BB
      DATA II/1/
      IF(IYR.EQ.II) GOTO 1
      II=IYR
      CALL RECALC(II,0,25,0,0)
  1   CONTINUE
      IF(J.LT.0) GOTO 2
      XMAG=XGEO*CTCL+YGEO*CTSL-ZGEO*ST0
      YMAG=YGEO*CL0-XGEO*SL0
      ZMAG=XGEO*STCL+YGEO*STSL+ZGEO*CT0
      RETURN
  2   XGEO=XMAG*CTCL-YMAG*SL0+ZMAG*STCL
      YGEO=XMAG*CTSL+YMAG*CL0+ZMAG*STSL
      ZGEO=ZMAG*CT0-XMAG*ST0

      RETURN
      END

C  *********************************************************************

      SUBROUTINE RECALC(IYR,IDAY,IHOUR,MIN,ISEC)
C  If only IYR is given then CALL RECALC(IYR,0,25,0,0)

C  Modified to accept years from 1900 through 2010 using the DGRF & 
C     IGRF-10 coeficients (updated by V. Papitashvili, November 2005)

C     IYR = YEAR NUMBER (FOUR DIGITS)
C     IHOUR = 25

        IMPLICIT NONE

        REAL ST0,CT0,SL0,CL0,CTCL,STCL,CTSL,STSL,SFI,CFI,SPS,CPS,
     1       SHI,CHI,HI,PSI,XMUT,A11,A21,A31,A12,A22,A32,A13,A23,
     2       A33,DS3,F2,F1,G10,G11,H11,DT,SQ,SQQ,SQR,BA(8)

        INTEGER IYR,IDAY,IHOUR,MIN,ISEC,K,IY

      COMMON/C1/ ST0,CT0,SL0,CL0,CTCL,STCL,CTSL,STSL,SFI,CFI,SPS,CPS,
     * SHI,CHI,HI,PSI,XMUT,A11,A21,A31,A12,A22,A32,A13,A23,A33,DS3,
     * K,IY,BA

C  IYE AND IDE ARE THE CURRENT VALUES OF YEAR AND DAY NUMBER

      IY=IYR
C      IF(IY.LT.1900) IY=1900
C      IF(IY.GT.2010) IY=2010

C  LINEAR INTERPOLATION OF THE GEODIPOLE MOMENT COMPONENTS BETWEEN THE
C  VALUES FOR THE NEAREST EPOCHS:

       IF (IY.LT.1905) THEN                             !1900-1905
           F2=(FLOAT(IY)+FLOAT(IDAY)/365.-1900.)/5.
           F1=1.D0-F2
           G10=31543.*F1+31464.*F2
           G11=-2298.*F1-2298.*F2
           H11= 5922.*F1+5909.*F2
       ELSEIF (IY.LT.1910) THEN                         !1905-1910
           F2=(FLOAT(IY)+FLOAT(IDAY)/365.-1905.)/5.
           F1=1.D0-F2
           G10=31464.*F1+31354.*F2
           G11=-2298.*F1-2297.*F2
           H11= 5909.*F1+5898.*F2
       ELSEIF (IY.LT.1915) THEN                         !1910-1915
           F2=(FLOAT(IY)+FLOAT(IDAY)/365.-1910.)/5.
           F1=1.D0-F2
           G10=31354.*F1+31212.*F2
           G11=-2297.*F1-2306.*F2
           H11= 5898.*F1+5875.*F2
       ELSEIF (IY.LT.1920) THEN                         !1915-1920
           F2=(FLOAT(IY)+FLOAT(IDAY)/365.-1915.)/5.
           F1=1.D0-F2
           G10=31212.*F1+31060.*F2
           G11=-2306.*F1-2317.*F2
           H11= 5875.*F1+5845.*F2
       ELSEIF (IY.LT.1925) THEN                         !1920-1925
           F2=(FLOAT(IY)+FLOAT(IDAY)/365.-1920.)/5.
           F1=1.D0-F2
           G10=31060.*F1+30926.*F2
           G11=-2317.*F1-2318.*F2
           H11= 5845.*F1+5817.*F2
       ELSEIF (IY.LT.1930) THEN                         !1925-1930
           F2=(FLOAT(IY)+FLOAT(IDAY)/365.-1925.)/5.
           F1=1.D0-F2
           G10=30926.*F1+30805.*F2
           G11=-2318.*F1-2316.*F2
           H11= 5817.*F1+5808.*F2
        ELSEIF (IY.LT.1935) THEN                        !1930-1935
           F2=(FLOAT(IY)+FLOAT(IDAY)/365.-1930.)/5.
           F1=1.D0-F2
           G10=30805.*F1+30715.*F2
           G11=-2316.*F1-2306.*F2
           H11= 5808.*F1+5812.*F2
        ELSEIF (IY.LT.1940) THEN                        !1935-1940
           F2=(FLOAT(IY)+FLOAT(IDAY)/365.-1935.)/5.
           F1=1.D0-F2
           G10=30715.*F1+30654.*F2
           G11=-2306.*F1-2292.*F2
           H11= 5812.*F1+5821.*F2
        ELSEIF (IY.LT.1945) THEN                        !1940-1945
           F2=(FLOAT(IY)+FLOAT(IDAY)/365.-1940.)/5.
           F1=1.D0-F2
           G10=30654.*F1+30594.*F2
           G11=-2292.*F1-2285.*F2
           H11= 5821.*F1+5810.*F2
        ELSEIF (IY.LT.1950) THEN                        !1945-1950
           F2=(FLOAT(IY)+FLOAT(IDAY)/365.-1945.)/5.
           F1=1.D0-F2
           G10=30594.*F1+30554.*F2
           G11=-2285.*F1-2250.*F2
           H11= 5810.*F1+5815.*F2
        ELSEIF (IY.LT.1955) THEN                        !1950-1955
           F2=(FLOAT(IY)+FLOAT(IDAY)/365.-1950.)/5.
           F1=1.D0-F2
           G10=30554.*F1+30500.*F2
           G11=-2250.*F1-2215.*F2
           H11= 5815.*F1+5820.*F2
        ELSEIF (IY.LT.1960) THEN                        !1955-1960
           F2=(FLOAT(IY)+FLOAT(IDAY)/365.-1955.)/5.
           F1=1.D0-F2
           G10=30500.*F1+30421.*F2
           G11=-2215.*F1-2169.*F2
           H11= 5820.*F1+5791.*F2
        ELSEIF (IY.LT.1965) THEN                        !1960-1965
           F2=(FLOAT(IY)+FLOAT(IDAY)/365.-1960.)/5.
           F1=1.D0-F2
           G10=30421.*F1+30334.*F2
           G11=-2169.*F1-2119.*F2
           H11= 5791.*F1+5776.*F2
        ELSEIF (IY.LT.1970) THEN                        !1965-1970
           F2=(FLOAT(IY)+FLOAT(IDAY)/365.-1965.)/5.
           F1=1.D0-F2
           G10=30334.*F1+30220.*F2
           G11=-2119.*F1-2068.*F2
           H11= 5776.*F1+5737.*F2
        ELSEIF (IY.LT.1975) THEN                        !1970-1975
           F2=(FLOAT(IY)+FLOAT(IDAY)/365.-1970.)/5.
           F1=1.D0-F2
           G10=30220.*F1+30100.*F2
           G11=-2068.*F1-2013.*F2
           H11= 5737.*F1+5675.*F2
        ELSEIF (IY.LT.1980) THEN                        !1975-1980
           F2=(DFLOAT(IY)+DFLOAT(IDAY)/365.-1975.)/5.
           F1=1.D0-F2
           G10=30100.*F1+29992.*F2
           G11=-2013.*F1-1956.*F2
           H11= 5675.*F1+5604.*F2
        ELSEIF (IY.LT.1985) THEN                        !1980-1985
           F2=(FLOAT(IY)+FLOAT(IDAY)/365.-1980.)/5.
           F1=1.D0-F2
           G10=29992.*F1+29873.*F2
           G11=-1956.*F1-1905.*F2
           H11= 5604.*F1+5500.*F2
        ELSEIF (IY.LT.1990) THEN                        !1985-1990
           F2=(FLOAT(IY)+FLOAT(IDAY)/365.-1985.)/5.
           F1=1.D0-F2
           G10=29873.*F1+29775.*F2
           G11=-1905.*F1-1848.*F2
           H11= 5500.*F1+5406.*F2
        ELSEIF (IY.LT.1995) THEN                        !1990-1995
           F2=(FLOAT(IY)+FLOAT(IDAY)/365.-1990.)/5.
           F1=1.D0-F2
           G10=29775.*F1+29692.*F2
           G11=-1848.*F1-1784.*F2
           H11= 5406.*F1+5306.*F2
        ELSEIF (IY.LT.2000) THEN                        !1995-2000
           F2=(FLOAT(IY)+FLOAT(IDAY)/365.-1995.)/5.
           F1=1.D0-F2
           G10=29692.*F1+29619.4*F2
           G11=-1784.*F1-1728.2*F2
           H11= 5306.*F1+5186.1*F2
        ELSEIF (IY.LT.2005) THEN                        !2000-2005
           F2=(FLOAT(IY)+FLOAT(IDAY)/365.-2000.)/5.
           F1=1.D0-F2
           G10=29619.4*F1+29556.8*F2
           G11=-1728.2*F1-1671.8*F2
           H11= 5186.1*F1+5080.0*F2
        ELSE                                            !2005-2010
           DT=FLOAT(IY)+FLOAT(IDAY)/365.-2005.
           G10=29556.8-8.8*DT
           G11=-1671.8+10.8*DT
           H11= 5080.0-21.3*DT
C           G10=29619.4*F1+29554.63*F2
C           G11=-1728.2*F1-1669.05*F2
C           H11= 5186.1*F1+5077.99*F2
C        ELSEIF (IY.LT.2010) THEN                        !2005-2010
C           F2=(FLOAT(IY)+FLOAT(IDAY)/365.-2005.)/5.
C           F1=1.D0-F2
C           G10=29554.63*F1+29496.5*F2
C           G11=-1669.05*F1-1585.9*F2
C           H11= 5077.99*F1+4945.1*F2
C        ELSE                                            !2010-2015
C           DT=FLOAT(IY)+FLOAT(IDAY)/365.-2010.
C           G10=29496.5-11.4*DT
C           G11=-1585.9+16.7*DT
C           H11= 4945.1-28.8*DT        
        ENDIF

C  NOW CALCULATE THE COMPONENTS OF THE UNIT VECTOR EzMAG IN GEO COORD
C  SYSTEM:
C  SIN(TETA0)*COS(LAMBDA0), SIN(TETA0)*SIN(LAMBDA0), AND COS(TETA0)
C         ST0 * CL0                ST0 * SL0                CT0

      SQ=G11**2+H11**2
      SQQ=SQRT(SQ)
      SQR=SQRT(G10**2+SQ)
      SL0=-H11/SQQ
      CL0=-G11/SQQ
      ST0=SQQ/SQR
      CT0=G10/SQR
      STCL=ST0*CL0
      STSL=ST0*SL0
      CTSL=CT0*SL0
      CTCL=CT0*CL0

C  THE CALCULATIONS ARE TERMINATED IF ONLY GEO-MAG TRANSFORMATION
C  IS TO BE DONE  (IHOUR>24 IS THE AGREED CONDITION FOR THIS CASE):

C   5  IF (IHOUR.GT.24) RETURN
      RETURN
      END
C 
C  
