/*
   Port of MSISE90 code from fortran to C/C++
   original fortran by

   Alan E. Hedin,
   PACF::HEDIN
   (301)-286-8393
   Version: 10-MAR-1993 15:31:11.76

   Original port by
   a. i. solutions, Inc.
   (301)-306-1756
   
   Modified for GMAT by 
   D. Conway, Thinking Systems, Inc. 
*/


#include "gmatdefs.hpp"


class MSISE90 
{
public:
	MSISE90(void);
	virtual ~MSISE90(void);

   void GTD6(const Integer IYD,
             const Real SEC,
             const Real ALT,
             const Real GLAT,
             const Real GLONG,
             const Real STL,
             const Real F107A,
             const Real F107,
             const Real (& AP)[7],
             const Integer MASS,
             Real (& D)[8], 
             Real (& T)[2] );
   // Neutral Atmosphere Empirical Model from the surface to lower
   // exosphere  MSISE90 ( JGR, 96, 1159 - 1172, 1991 )
   // A.E.Hedin  2/11/93 
   //  See subroutine GHP6 to specify a pressure rather than altitude.
   //     INPUT:
   //        IYD   - YEAR AND DAY AS YYYYDDD or just DDD ( day of year from 1 to 365)
   //        SEC   - UT( SEC )
   //        ALT   - ALTITUDE( KM )
   //        GLAT  - GEODETIC LATITUDE( DEG )
   //        GLONG - GEODETIC LONGITUDE( DEG )
   //        STL   - LOCAL APPARENT SOLAR TIME( HRS )
   //        F107A - 3 MONTH AVERAGE OF F10.7 FLUX
   //        F107  - DAILY F10.7 FLUX FOR PREVIOUS DAY
   //        AP    - MAGNETIC INDEX( DAILY )  OR WHEN SW[8] = -1.0 :
   //              - ARRAY CONTAINING:
   //             [0] DAILY AP
   //             [1] 3 HR AP INDEX FOR CURRENT TIME
   //             [2] 3 HR AP INDEX FOR 3 HRS BEFORE CURRENT TIME
   //             [3] 3 HR AP INDEX FOR 6 HRS BEFORE CURRENT TIME
   //             [4] 3 HR AP INDEX FOR 9 HRS BEFORE CURRENT TIME
   //             [5] AVERAGE OF EIGHT 3 HR AP INDICIES FROM 12 TO 33 HRS PRIOR
   //                    TO CURRENT TIME
   //             [6] AVERAGE OF EIGHT 3 HR AP INDICIES FROM 36 TO 59 HRS PRIOR
   //                    TO CURRENT TIME
   //        MASS - MASS NUMBER ( ONLY DENSITY FOR SELECTED GAS IS
   //                 CALCULATED.  MASS = 0 IS TEMPERATURE.  MASS = 48 FOR ALL.
   //     Note:  Ut, Local Time, and Longitude are used independently in the
   //            model and are not of equal importance for every situation.  
   //            For the most physically realistic calculation these three
   //            variables should be consistent ( STL = SEC / 3600 + GLONG / 15 ) .
   //            F107, F107A, and AP effects are not large below 80 km 
   //            and these can be set to 150.0, 150.0, and 4.0 respectively.
   //     OUTPUT:
   //        D[0] - HE NUMBER DENSITY( CM - 3 )
   //        D[1] - O  NUMBER DENSITY( CM - 3 )
   //        D[2] - N2 NUMBER DENSITY( CM - 3 )
   //        D[3] - O2 NUMBER DENSITY( CM - 3 )
   //        D[4] - AR NUMBER DENSITY( CM - 3 )                        
   //        D[5] - TOTAL MASS DENSITY( GM / CM3 )
   //        D[6] - H NUMBER DENSITY( CM - 3 )
   //        D[7] - N NUMBER DENSITY( CM - 3 )
   //        T[0] - EXOSPHERIC TEMPERATURE
   //        T[1] - TEMPERATURE AT ALT
   //
   //      TO GET OUTPUT IN M - 3 and KG / M3:   CALL METER6(1) 
   //
   //      O, H, and N set to zero below 72.5 km
   //      Exospheric temperature set to average for altitudes below 120 km.
   //        
   //           The following is for test and special purposes:
   //            TO TURN ON AND OFF PARTICULAR VARIATIONS CALL TSELEC( SW )
   //               WHERE SW IS A 25 ELEMENT ARRAY CONTAINING 0.0 FOR OFF, 1.0 
   //               FOR ON, OR 2.0 FOR MAIN EFFECTS OFF BUT CROSS TERMS ON
   //               FOR THE FOLLOWING VARIATIONS
   //               1 - F10.7 EFFECT ON MEAN  
   //               2 - TIME INDEPENDENT
   //               3 - SYMMETRICAL ANNUAL    
   //               4 - SYMMETRICAL SEMIANNUAL
   //               5 - ASYMMETRICAL ANNUAL   
   //               6 - ASYMMETRICAL SEMIANNUAL
   //               7 - DIURNAL               
   //               8 - SEMIDIURNAL
   //               9 - DAILY AP             
   //              10 - ALL UT / LONG EFFECTS
   //              11 - LONGITUDINAL         
   //              12 - UT AND MIXED UT / LONG
   //              13 - MIXED AP / UT / LONG 
   //              14 - TERDIURNAL
   //              15 - DEPARTURES FROM DIFFUSIVE EQUILIBRIUM
   //              16 - ALL TINF VAR         
   //              17 - ALL TLB VAR
   //              18 - ALL TN1 VAR          
   //              19 - ALL S VAR
   //              20 - ALL TN2 VAR          
   //              21 - ALL NLB VAR
   //              22 - ALL TN3 VAR          
   //              23 - TURBO SCALE HEIGHT VAR
   //
   //              To get current values of SW: CALL TRETRV( SW )
   //

   void GTD6(const Integer IYD,
             const Real SEC,
             const Real ALT,
             const Real GLAT,
             const Real GLONG,
             const Real STL,
             const Real F107A,
             const Real F107,
             const Real & AP,
             const Integer MASS,
             Real (& D)[8], 
             Real (& T)[2] )
   { Real AP7[7] = {0}; AP7[0] = AP; GTD6(IYD,SEC,ALT,GLAT,GLONG,STL,F107A,F107,AP7,MASS,D,T); };

   void GTD6(const int IYD,
             const float SEC,
             const float ALT,
             const float GLAT,
             const float GLONG,
             const float STL,
             const float F107A,
             const float F107,
             const float (& AP)[7],
             const int MASS,
             float (& D)[8], 
             float (& T)[2] )
   { 
      Integer i;
      Real nAP[7]; for (     i = 0; i < 7; i++)  nAP[i] = AP[i];
      Real nD[8];  for (     i = 0; i < 8; i++)   nD[i] = D[i];
      Real nT[2];  for (     i = 0; i < 2; i++)   nT[i] = T[i];
      GTD6(IYD,SEC,ALT,GLAT,GLONG,STL,F107A,F107,nAP,MASS,nD,nT);
   };

   void GHP6(const Integer IYD,
             const Real SEC,
             Real & ALT,
             const Real GLAT,
             const Real GLONG,
             const Real STL,
             const Real F107A,
             const Real F107,
             Real (& AP)[7],
             Real (& D)[8],
             Real (& T)[2], 
             Real & PRESS);

   //      FIND ALTITUDE OF PRESSURE SURFACE ( PRESS )  FROM GTD6
   //    INPUT:
   //       IYD   - YEAR AND DAY AS YYYYDDD
   //       SEC   - UT( SEC)
   //       GLAT  - GEODETIC LATITUDE( DEG)
   //       GLONG - GEODETIC LONGITUDE( DEG)
   //       STL   - LOCAL APPARENT SOLAR TIME( HRS)
   //       F107A - 3 MONTH AVERAGE OF F10.7 FLUX
   //       F107  - DAILY F10.7 FLUX FOR PREVIOUS DAY
   //       AP    - MAGNETIC INDEX( DAILY )  OR WHEN SW[8]= - 1.0 :
   //             - ARRAY CONTAINING:
   //            [0] DAILY AP
   //            [1] 3 HR AP INDEX FOR CURRENT TIME
   //            [2] 3 HR AP INDEX FOR 3 HRS BEFORE CURRENT TIME
   //            [3] 3 HR AP INDEX FOR 6 HRS BEFORE CURRENT TIME
   //            [4] 3 HR AP INDEX FOR 9 HRS BEFORE CURRENT TIME
   //            [5] AVERAGE OF EIGHT 3 HR AP INDICIES FROM 12 TO 33 HRS PRIOR
   //                   TO CURRENT TIME
   //            [6] AVERAGE OF EIGHT 3 HR AP INDICIES FROM 36 TO 59 HRS PRIOR
   //                   TO CURRENT TIME
   //       PRESS - PRESSURE LEVEL( MB)
   //    OUTPUT:
   //       ALT  - ALTITUDE( KM )  
   //       D[0] - HE NUMBER DENSITY( CM - 3)
   //       D[1] - O NUMBER DENSITY( CM - 3)
   //       D[2] - N2 NUMBER DENSITY( CM - 3)
   //       D[3] - O2 NUMBER DENSITY( CM - 3)
   //       D[4] - AR NUMBER DENSITY( CM - 3)
   //       D[5] - TOTAL MASS DENSITY( GM / CM3)
   //       D[6] - H NUMBER DENSITY( CM - 3)
   //       D[7] - N NUMBER DENSITY( CM - 3)
   //       T[0] - EXOSPHERIC TEMPERATURE
   //       T[1] - TEMPERATURE AT ALT

   void TSELEC( const Real (& SV)[25]);
   
   void TRETRV( Real (& SVV)[25] );

   Real MSISE90::GetDL(int index);  // for debug
protected:

   void GLATF(const Real LAT, 
              Real & GV, 
              Real & REFF);

   bool VTST(const Integer IYD,
             const Real SEC,
             const Real GLAT,
             const Real GLONG,
             const Real STL,
             const Real F107A,
             const Real F107,
             const Real AP[8],
             const Integer IC);

   void GTS6(const Integer IYD,
             const Real SEC,
             const Real ALT,
             const Real GLAT,
             const Real GLONG,
             const Real STL,
             const Real F107A,
             const Real F107,
             const Real (& AP)[7],
             const Integer MASS,
             Real (& D)[8], 
             Real (& T)[2]);

   void METER6(const Integer METER);

   Real GLOBE6(const Integer YRD,
                const Real SEC,
                const Real LAT,
                const Real LONG,
                const Real TLOC,
                const Real F107A,
                const Real F107,
                const Real (& AP)[7],
                Real (& P)[150]);

   Real GLOB6S(const Real (& P)[100] ); 

   Real DENSU(const Real ALT,
               const Real DLB,
               const Real TINF,
               const Real TLB,
               const Real XM,
               const Real ALPHA,
               Real & TZ,
               const Real ZLB,
               const Real S2,
               const Integer MN1,
               const Real * ZN1, // ZN1[MN1]
               Real * TN1,       // TN1[MN1]
               Real * TGN1);     // TGN1[MN1]

   Real DENSM(const Real ALT,
               const Real D0,
               const Real XM,
               Real & TZ,
               const Integer MN3,
               const Real * ZN3,  // ZN3[MN3]
               const Real * TN3,  // TN3[MN3]
               const Real * TGN3, // TGN3[MN3]
               const Integer MN2,
               const Real * ZN2,
               const Real * TN2,
               const Real * TGN2);

   void SPLINE(const Real * X, // X[N]
               const Real * Y, // Y[N]
               const Integer N,
               const Real YP1,
               const Real YPN,
               Real * Y2);     // Y2[N]

   void SPLINT(const Real * XA, // XA[N]
               const Real * YA, // YA[N]
               const Real * Y2A,// Y2A[N]
               const Integer N,
               const Real X,
               Real & Y);

   void SPLINI(const Real * XA, //XA[N]
               const Real * YA, //YA[N]
               const Real * Y2A,//Y2A[N]
               const Integer N,
               const Real X,
               Real & YI);

   Real DNET(Real & DD,
              const Real DM,
              const Real ZHM,
              const Real XMM,
              const Real XM);

   Real CCOR(const Real ALT, 
              const Real R,
              const Real H1,
              const Real ZH);

protected:

   // common LSQV
   Integer MP;
   Integer II;
   Integer JG;
   Integer LT;
   Real QPB[50];
   Integer IERR;
   Integer IFUN;
   Integer N;
   Integer J;
   Real DV[60];
   
   // common FIT
   Real TAF;

   // common GTS3C
   Real TLB;
   Real S;
   Real DB04;
   Real DB16;
   Real DB28;
   Real DB32;
   Real DB40;
   Real DB48;
   Real DB01;
   Real ZA;
   Real T0;
   Real Z0;
   Real G0;
   Real RL;
   Real DD;
   Real DB14;
   Real TR12;

   // common MESO6
   Real TN1[5];
   Real TN2[4];
   Real TN3[5];
   Real TGN1[2];
   Real TGN2[2];
   Real TGN3[2];

   // common LOWER6
   static const Real PTM[10];
   static const Real PDM[8][10];

   // common PARM6
   static Real PT[150];
   static Real PD[9][150];
   static Real PS[150];
   static const Real PDL[2][25];
   static const Real PTL[4][100];
   static const Real PMA[10][100];

   // common DATIM6
   // Integer ISD[3];
   // Integer IST[2];
   // Real NAM[2];

   // common DATIME
   // Integer ISDATE[3];
   // Integer ISTIME[2];
   // Real NAME[2];

   // common CSW
   Real SW[25];
   Integer ISW;
   Real SWC[25];

   // common MAVG6
   static const Real PAVGM[10];

   // common DMIX
   Real DM04;
   Real DM16;
   Real DM28;
   Real DM32;
   Real DM40;
   Real DM01;
   Real DM14;

   // common PARMB
   Real GSURF;
   Real RE;

   // common METSEL
   Integer IMR;

   // common TTEST
   Real TINFG; // TINF;
   Real GB;
   Real ROUT;
   Real TT[15]; // T[15];

   // common LPOLY
   Real PLG[4][9];
   Real CTLOC;
   Real STLOC;
   Real C2TLOC;
   Real S2TLOC;
   Real C3TLOC;
   Real S3TLOC;
   Integer IYR;
   Real DAY;
   Real DF;
   Real DFA;
   Real APD;
   Real APDF;
   Real APT[4];
   Real XLONG;
   Real CLONG;
   Real SLONG;

   Real SAV[25];
   Integer IYDL[2];
   Real SECL[2];
   Real GLATL[2];
   Real GLL[2];
   Real STLL[2];
   Real FAL[2];
   Real FL[2];
   Real APL[2][7];
   Real SWL[2][25];
   Real SWCL[2][25];

};

