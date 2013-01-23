/* irisub.f -- translated by f2c (version 20000704).
   You must link the resulting object file with the libraries:
	-lf2c -lm   (in that order)
*/


#ifdef __cplusplus
extern "C" {
#endif
#include "f2c.h"

/* Common Block Declarations */

struct {
    real umr;
} const_;

#define const_1 const_

struct {
    real humr, dumr;
} const1_;

#define const1_1 const1_

struct {
    real argmax;
} argexp_;

#define argexp_1 argexp_

struct {
    integer icalls, nmono, iyearo, idaynro;
    logical rzino, igino;
    real ut0;
} const2_;

#define const2_1 const2_

struct {
    real hmf2, nmf2, hmf1;
    logical f1reg;
} block1_;

#define block1_1 block1_

struct {
    real b0, b1, c1;
} block2_;

#define block2_1 block2_

struct {
    real hz, t, hst;
} block3_;

#define block3_1 block3_

struct {
    real hme, nme, hef;
} block4_;

#define block4_1 block4_

struct {
    logical enight;
    real e[4];
} block5_;

#define block5_1 block5_

struct {
    real hmd, nmd, hdx;
} block6_;

#define block6_1 block6_

struct {
    real d1, xkk, fp30, fp3u, fp1, fp2;
} block7_;

#define block7_1 block7_

struct {
    real hs, tnhs, xsm[4], mm[5], dti[4];
    integer mxsm;
} block8_;

#define block8_1 block8_

struct {
    real xsm1, texos, tlbdh, sigma;
} blotn_;

#define blotn_1 blotn_

struct {
    real ahh[7], ate1, stte[6], dte[5];
} blote_;

#define blote_1 blote_

struct {
    real beta, eta, delta, zeta;
} blo10_;

#define blo10_1 blo10_

struct {
    real b2top, tc3;
    integer itopn;
    real alg10, hcor1;
} blo11_;

#define blo11_1 blo11_

struct {
    integer konsol;
} iounit_;

#define iounit_1 iounit_

/* Table of constant values */

static integer c__1 = 1;
static integer c__0 = 0;
static real c_b66 = (float)300.;
static real c_b71 = (float)80.;
static real c_b72 = (float)110.;
static real c_b73 = (float)200.;
static real c_b75 = (float)12.;
static integer c__1976 = 1976;
static integer c__882 = 882;
static real c_b121 = (float)1.;
static real c_b125 = (float)1.9;
static real c_b126 = (float)2.6;
static real c_b129 = (float)28.;
static real c_b134 = (float)81.;
static real c_b137 = (float).06;
static real c_b145 = (float)4e8;
static real c_b147 = (float)88.;
static real c_b150 = (float).05;
static real c_b153 = (float)4.6;
static real c_b154 = (float)4.5;
static real c_b157 = (float)-11.5;
static real c_b158 = (float)-4.;
static doublereal c_b161 = 10.;
static real c_b170 = (float).001;
static real c_b188 = (float)0.;
static doublereal c_b195 = 2.;
static real c_b196 = (float)1.5;
static real c_b206 = (float)3.;
static real c_b213 = (float)130.;
static real c_b214 = (float)500.;
static real c_b217 = (float).01;
static integer c__12 = 12;
static integer c__4 = 4;
static integer c__2 = 2;
static real c_b239 = (float)10.;
static real c_b246 = (float)50.;

/* irisub.for, version number can be found at the end of this comment. */
/* ----------------------------------------------------------------------- */

/* Includes subroutines IRI_SUB and IRI_WEB to compute IRI parameters */
/* for specified location, date, time, and altitude range and subroutine */
/* IRI_WEB to computes IRI parameters for specified location, date, time */
/* and variable range; variable can be altitude, latitude, longitude, */
/* year, month, day of month, day of year, or hour (UT or LT). */
/* IRI_WEB requires IRI_SUB. Both subroutines require linking with the */
/* following library files IRIFUN.FOR, IRITEC.FOR, IRIDREG.FOR, */
/* CIRA.FOR, IGRF.FOR */

/* !!!!! Subroutine INITIALIZE has to be called once before calling */
/* !!!!! iri_sub. This is already included in subroutine IRI_WEB which */
/* !!!!! calls iri_sub. */

/* i/o units:  6   messages (during execution) to Konsol (Monitor) */
/*            10   CCIR and URSI coefficients */
/*            11   alternate unit for message file (if jf(12)=.false.) */
/*            12   solar/ionospheric indices: ig_rz.dat */
/*            13   magnetic indices: ap.dat */
/*            14   igrf coefficients */

/* ----------------------------------------------------------------------- */

/* CHANGES FROM  IRIS11.FOR  TO   IRIS12.FOR: */
/*    - CIRA-1986 INSTEAD OF CIRA-1972 FOR NEUTRAL TEMPERATURE */
/*    - 10/30/91 VNER FOR NIGHTTIME LAY-VERSION:  ABS(..) */
/*    - 10/30/91 XNE(..) IN CASE OF LAY-VERSION */
/*    - 10/30/91 CHANGE SSIN=F/T TO IIQU=0,1,2 */
/*    - 10/30/91 Te > Ti > Tn ENFORCED IN FINAL PROFILE */
/*    - 10/30/91 SUB ALL NAMES WITH 6 OR MORE CHARACTERS */
/*    - 10/31/91 CORRECTED HF1 IN HST SEARCH:  NE(HF1)>NME */
/*    - 11/14/91 C1=0 IF NO F1-REGION */
/*    - 11/14/91 CORRECTED HHMIN AND HZ FOR LIN. APP. */
/*    -  1/28/92 RZ12=0 included */
/*    -  1/29/92 NEQV instead of NE between URSIF2 and URSIFO */
/*    -  5/ 1/92 CCIR and URSI input as in IRID12 */
/*    -  9/ 2/94 Decimal month (ZMONTH) for IONCOM */
/*    -  9/ 2/94 Replace B0POL with B0_TAB; better annually */
/*    -  1/ 4/95 DY for h>hmF2 */
/*    -  2/ 2/95 IG for foF2, topside; RZ for hmF2, B0_TAB, foF1, NmD */
/*    -  2/ 2/95 winter no longer exclusive for F1 occurrrence */
/*    -  2/ 2/95 RZ and IG incl as DATA statement; smooth annual var. */
/* CHANGES FROM  IRIS12.FOR  TO   IRIS13.FOR: */
/*    - 10/26/95 incl year as input and corrected MODA; nrm for zmonth */
/*    - 10/26/95 use TCON and month-month interpolation in foF2, hmF2 */
/*    - 10/26/95 TCON only if date changes */
/*    - 11/25/95 take out logicals TOPSI, BOTTO, and BELOWE */
/*    - 12/ 1/95 UT_LT for (date-)correct UT<->LT conversion */
/*    - 12/22/95 Change ZETA cov term to cov < 180; use cov inst covsat */
/*    -  2/23/96 take covmax(R<150) for topside; lyear,.. for lt */
/*    -  3/26/96 topside: 94.5/BETA inst 94.45/..; cov -> covsat(<=188) */
/*    -  5/01/96 No longer DY for h>hmF2 (because of discontinuity) */
/*    - 12/01/96 IRIV13: HOUR for IVAR=1 (height) */
/*    -  4/25/97 D-region: XKK le 10 with D1 calc accordingly. */
/*    -  1/12/97 DS model for lower ion compoistion DY model */
/*    -  5/19/98 seamon=zmonth if lati>0; zmonth= ...(1.0*iday)/.. */
/*    -  5/19/98 DY ion composition model below 300 km now DS model */
/*    -  5/19/98 DS model includes N+, Cl down to 75 km HNIA changed */
/*    -  5/28/98 User input for Rz12, foF1/NmF1, hmF1, foE/NmE, hmE */
/*    -  9/ 2/98 1 instead of 0 in MODA after UT_LT call */
/*    -  4/30/99 constants moved from DATA statement into program */
/*    -  4/30/99 changed konsol-unit to 13 (12 is for IG_RZ). */
/*    -  5/29/99 the limit for IG comp. from Rz12-input is 174 not 274 */
/*    - 11/08/99 jf(18)=t simple UT to LT conversion, otherwise UT_LT */
/*    - 11/09/99 added COMMON/const1/humr,dumr also for CIRA86 */
/* CHANGES FROM  IRIS13.FOR  TO   IRISUB.FOR: */
/* ----------------------------------------------------------------------- */
/* -Version-MM/DD/YY-Description (person reporting correction) */
/* 2000.01 05/09/00 B0_98 replaces B0_TAB and B1: 1.9/day to 2.6/night */
/* 2000.02 06/11/00 including new F1 and indermediate region */
/* 2000.03 10/15/00 include Scherliess-Fejer drift model */
/* 2000.04 10/29/00 include special option for D region models */
/* 2000.05 12/07/00 change name IRIS13 to IRISUB */
/* 2000.06 12/14/00 jf(30),outf(20,100),oarr(50) */
/* 2000.07 03/17/01 include Truhlik-Triskova Te model and IGRF */
/* 2000.08 05/07/01 include Fuller-Rowell-Condrescu storm model */
/* 2000.09 07/09/01 LATI instead of LAT1 in F00 call -------- M. Torkar */
/* 2000.10 07/09/01 sdte instead of dte in ELTEIK call --- P. Wilkinson */
/* 2000.11 09/18/01 correct computation of foF2 for Rz12 user input */
/* 2000.12 09/19/01 Call APF only if different date and time -- P. Webb */
/* 2000.13 10/28/02 replace TAB/6 blanks, enforce 72/line -- D. Simpson */
/* 2000.14 11/08/02 change unit for message file to 11 (13 is Kp) */
/* 2000.15 01/27/03 change F1_prob output; Te-IK for fix h and ELTE(h) */
/* 2000.16 02/04/03 along<0 -> along=along+360; F1 occ for hmf1&foF1 */
/* 2000.17 02/05/03 zyear =12.97 (Dec 31); idayy=#days per year */
/* 2000.18 02/06/03 jf(27) for IG12 user input; all F1 prob in oar */
/* 2000.19 07/14/04 covsat<188 instead of covsat=<f(IG)<188 */
/* 2000.19 02/09/05 declare INVDIP as real ------------------ F. Morgan */
/* 2000.20 11/09/05 replace B0B1 with BCOEF --------------- T. Gulyaeva */
/* 2005.01 11/09/05 new topside ion composition; F107D from file */
/* 2005.02 11/14/05 jf(18) now IGRF/POGO; dip,mlat now IGRF10; */
/* 2005.03 11/15/05 sunrise/sunset/night for D,E,F1,F2; UT_LT removed */
/* 2005.04 05/06/06 FIRI D-region option not tied to peak */
/* 2005.04 05/06/06 Spread-F included, NeQuick included */
/* 2005.05 01/15/07 NeQuick uses CCIR-M3000F2 even if user-hmF2 */
/* 2007.00 05/18/07 Release of IRI-2007 */
/* 2007.01 01/23/08 ryear = .. (daynr-1.0)/idayy ---------- R. Scharroo */
/* 2007.02 10/31/08 outf(100) -> outf(500), numhei=numstp=500 */
/* 2007.03 02/12/09 Jf(24)=.false.-> outf(1,60-140km)=FIRI- M. Friedrich */
/* 2007.04 03/14/09 SOCO(70->80;500->300km) --------------- R. Davidson */
/* 2007.05 03/26/09 call for APF_ONLY includes F107M */
/* 2007.06 08/17/09 STROM off if input; fof2in, fof1in,foein corr */
/* 2007.07 02/03/10 F10.7D = F10.7M = COV if EOF */

/* ***************************************************************** */
/* ********* INTERNATIONAL REFERENCE IONOSPHERE (IRI). ************* */
/* ***************************************************************** */
/* **************** ALL-IN-ONE SUBROUTINE  ************************* */
/* ***************************************************************** */


/* Subroutine */ int iri_sub__(logical *jf, integer *jmag, real *alati, real *
	along, integer *iyyyy, integer *mmdd, real *dhour, real *heibeg, real 
	*heiend, real *heistp, real *outf, real *oarr, integer *ier)
{
    /* Format strings */
    static char fmt_1939[] = "(\002 *Ne* User input of hmF1 is only possible\
 for the LAY-\002,\002version\002)";
    static char fmt_2911[] = "(\002*** IRI parameters are being calculated *\
**\002)";
    static char fmt_9012[] = "(\002Ne, E-F: The LAY-Version is prelimenary\
.\002,\002 Erroneous profile features can occur.\002)";
    static char fmt_9013[] = "(\002Ne, B0: Bottomside thickness is \002,\002\
obtained with Gulyaeva-1987 model.\002)";
    static char fmt_9014[] = "(\002Ne: No upper limit for F10.7 in\002,\002 \
topside formula.\002)";
    static char fmt_9206[] = "(\002Ne: TTS for Topside\002)";
    static char fmt_9204[] = "(\002Ne: Corrected Topside Formula\002)";
    static char fmt_9205[] = "(\002Ne: NeQuick for Topside\002)";
    static char fmt_9015[] = "(\002Ne, foF2/NmF2: provided by user.\002)";
    static char fmt_9016[] = "(\002Ne, foF2: URSI model is used.\002)";
    static char fmt_9017[] = "(\002Ne, foF2: CCIR model is used.\002)";
    static char fmt_9018[] = "(\002Ne, hmF2/M3000F2: provided by user.\002)";
    static char fmt_9019[] = "(\002Ne, foF1/NmF1: provided by user.\002)";
    static char fmt_9021[] = "(\002Ne, hmF1: provided by user.\002)";
    static char fmt_9022[] = "(\002Ne, foE/NmE: provided by user.\002)";
    static char fmt_9023[] = "(\002Ne, hmE: provided by user.\002)";
    static char fmt_9024[] = "(\002Ne, foF1: probability function used.\002)";
    static char fmt_9025[] = "(\002Ne, foF1: L condition cases included.\002)"
	    ;
    static char fmt_9026[] = "(\002Ne, D: IRI-90\002)";
    static char fmt_9027[] = "(\002Ne, D: IRI-90, DRS-95,and FIRI-01)\002)";
    static char fmt_9028[] = "(\002Ne, foF2: Storm model turned off if foF2 \
or\002,\002 NmF2 user input\002)";
    static char fmt_9029[] = "(\002Ne, foF2: storm model included\002)";
    static char fmt_9031[] = "(\002Ion Com.: DS-95 & TTS-03\002)";
    static char fmt_9039[] = "(\002Ion Com.: DS-78 & DY-85\002)";
    static char fmt_9032[] = "(\002Te: Temperature-density correlation is us\
ed.\002)";
    static char fmt_9033[] = "(\002Te: Aeros/ISIS model\002)";
    static char fmt_9034[] = "(\002Te: Interkosmos model\002)";
//    static char fmt_104[] = "(\002ccir\002,i2,\002.asc\002)";
    static char fmt_104[] = "('./../data/IonosphereData/ccir',i2,'.asc')";		// made changes by TUAN NGUYEN on Jan 11, 2013
    static char fmt_4689[] = "(1x,4e15.8)";
//    static char fmt_1144[] = "(\002ursi\002,i2,\002.asc\002)";
    static char fmt_1144[] = "('./../data/IonosphereData/ursi',i2,'.asc')";		// made changes by TUAN NGUYEN on Jan 11, 2013
    static char fmt_8449[] = "(1x////,\002 The file \002,a30,\002is not in y\
our directory.\002)";
    static char fmt_650[] = "(1x,\002*NE* E-REGION VALLEY CAN NOT BE MODEL\
LED\002)";
    static char fmt_11[] = "(1x,\002*NE* HMF1 IS NOT EVALUATED BY THE FUNCTI\
ON XE2\002/1x,\002CORR.: NO F1 REGION, B1=3, C1=0.0\002)";
    static char fmt_100[] = "(1x,\002*NE* HST IS NOT EVALUATED BY THE FUNCTI\
ON XE3\002)";
    static char fmt_901[] = "(6x,\002CORR.: LIN. APP. BETWEEN HZ=\002,f5.1\
,\002 AND HEF=\002,f5.1)";
    static char fmt_7733[] = "(\002*NE* LAY amplitudes found with 2nd choice\
 of HXL(1).\002)";
    static char fmt_7722[] = "(\002*NE* LAY amplitudes could not be found\
.\002)";

    /* System generated locals */
    integer i__1;
    real r__1, r__2;
    doublereal d__1, d__2;
    olist o__1;
    cllist cl__1;

    /* Builtin functions */
    double atan(doublereal), log(doublereal), sqrt(doublereal);
    integer s_wsfe(cilist *), e_wsfe();
    double tan(doublereal), exp(doublereal);
    integer s_wsfi(icilist *), do_fio(integer *, char *, ftnlen), e_wsfi(), 
	    f_open(olist *), s_rsfe(cilist *), e_rsfe(), f_clos(cllist *);
    double cos(doublereal), pow_dd(doublereal *, doublereal *), r_sign(real *,
	     real *);

    /* Local variables */
    static logical sam_date__;
    extern /* Subroutine */ int igrf_dip__(real *, real *, real *, real *, 
	    real *, real *, real *, integer *), igrf_sub__(real *, real *, 
	    real *, real *, real *, integer *, real *, real *, integer *), 
	    apf_only__(integer *, integer *, integer *, real *, real *);
    static logical f1_l_cond__;
    static real d__, f[3];
    static integer i__, j, k, l;
    static real x, y, z__, f2[1976]	/* was [13][76][2] */, r2, x1, z1, z2,
	     z3;
    extern /* Subroutine */ int f00_(real *, real *, integer *, real *, real *
	    , real *, integer *);
    static real stormcorr, ee;
    static integer ii, ki, kk;
    static real ho[4], pi, mo[5];
    static logical dy;
    static real ex, x12;
    extern doublereal ti_(real *);
    static real zi;
    extern doublereal tn_(real *, real *, real *, real *);
    static real xl, ff0[988], b2k, hf1, hf2;
    static integer do2[2];
    static real fm3[882]	/* was [9][49][2] */, f2n[1976]	/* was [13][
	    76][2] */, ho2[2], r2d, x1d, mo2[3];
    extern doublereal xe2_(real *);
    static real ex1;
    extern doublereal xe6_(real *);
    static real r2n, xm0[441], rr2, rr1, x1n, xf1, xf2, ti1, h0o, xl1, dec;
    static integer icd, ddo[4];
    static real elg[7], ate[7], tea[6], amp[4], rif[4], scl[4], dip, xma, yma,
	     hxl[4], zma, bet;
    static logical ext;
    static real cov, xhi, rrr, foe;
    extern /* Subroutine */ int apf_(integer *, integer *, integer *, real *, 
	    integer *);
    static integer kut;
    static real ttt, flu;
    extern /* Subroutine */ int tal_(real *, real *, real *, real *, logical *
	    , real *);
    static real xdx, f107d;
    extern doublereal b0_98__(real *, real *, real *, integer *, real *, real 
	    *, real *);
    static real vkp, hta, sec;
    static integer iyz, idz;
    static real f1pb, ff0n[988], f107m, fof2, eta1, fof1, nmf1, fm3n[882]	
	    /* was [9][49][2] */, f6wa, pf1o[12], pg1o[80], pg2o[32], pg3o[80]
	    , pf2o[4], pf3o[12], xhi1, sud1, xhi2, xhi3, sax1, cos2, xm0n[441]
	    , rr2n, rr1n, f5sw, xe2h, zmp1, zmp2;
    extern doublereal f1_c1__(real *, real *, real *, real *);
    static real hv1r, hv2r, tn120, hte, ett, sux1, tet, ten;
    static integer ijk;
    static real dela, teh2, babs, tid1, tin1, afoe, ahme, hnea;
    extern doublereal xe3_1__(real *);
    static real arig[3];
    static logical dreg;
    static integer kind;
    static logical igin;
    static real dnds[4], lati, dion[7], anme;
    extern /* Subroutine */ int moda_(integer *, integer *, integer *, 
	    integer *, integer *, integer *);
    static real mlat;
    static logical old79;
    static integer iday;
    static real gind, hnee, dell;
    static logical topc;
    extern /* Subroutine */ int tcon_(integer *, integer *, integer *, 
	    integer *, real *, real *, real *, integer *);
    static real xnar[2];
    extern /* Subroutine */ int soco_(integer *, real *, real *, real *, real 
	    *, real *, real *, real *, real *);
    static real sax80, rzar[3], hour;
    static logical topo, rzin;
    extern doublereal fout_(real *, real *, real *, real *, real *);
    static real alg100, rssn, sux80, zm3000;
    static integer midm;
    static real xm3000, afof1, afof2, ahmf2, ahmf1, epin, zmp11, anmf1, anmf2,
	     alog2, zmp22;
    extern doublereal hpol_(real *, real *, real *, real *, real *, real *, 
	    real *);
    static real b2bot;
    static logical gulb0;
    static real f107dx, fof2n, sax110, sax200, nobo2, sax300, seax, f107mx, 
	    grat, oarr1, xm300n, oarr3, oarr5, zfof2, yfof2, zmp111, zmp222, 
	    f1pbw, f1pbl, f1pbo, xdel, sux110, sux200, sux300, vner, hmex, 
	    dxdx, bnmf1, hmf1m, hhmf2;
    static integer iiqu;
    static real utni, tn1ni;
    extern /* Subroutine */ int teba_(real *, real *, integer *, real *);
    static real bcoef, dimo, dipl, sdte, hhalf;
    static integer icode;
    extern doublereal tede_(real *, real *, real *);
    static real hdeep, stte1, aigin, magbr;
    static integer indap[13];
    static real cglat, ddens[55]	/* was [5][11] */, dlndh, param[2];
    static logical foein, hmein, noden;
    extern doublereal teder_(real *);
    static real longi, modip;
    static logical tecon[2];
    static real osfbr[25], mlong;
    static integer daynr, iyear;
    static real xdels[4];
    static integer idayy;
    static logical notem, noion;
    static real arzin;
    static integer jxnar, month;
    static real ryear;
    extern /* Subroutine */ int rogul_(integer *, real *, real *, real *);
    static real width, depth;
    extern doublereal xmded_(real *, real *, real *);
    static real xnehz;
    extern /* Subroutine */ int cira86_(integer *, real *, real *, real *, 
	    real *, real *, real *, real *, real *);
    static real secni;
    extern /* Subroutine */ int ut_lt__(integer *, real *, real *, real *, 
	    integer *, integer *), storm_(integer *, real *, real *, integer *
	    , real *, integer *, integer *, real *);
    extern doublereal hmf2ed_(real *, real *, real *, real *), fof1ed_(real *,
	     real *, real *);
    extern /* Subroutine */ int regfa1_(real *, real *, real *, real *, real *
	    , real *, E_fp, logical *, real *);
    static real texni, signi, tlbdn, hmaxd;
    extern doublereal xmout_(real *, real *, real *, real *, real *);
    static real hmaxn, b0cnew, tmaxd, tmaxn, stte2;
    static logical fof1in, fof2in, hmf2in, hmf1in;
    static real tnahh2, ten1;
    extern doublereal elte_(real *);
    static real tnn1, ti13, ti50;
    extern doublereal dtndh_(real *, real *, real *, real *);
    static real sunde1, xtts, tex, tix, xteti, hnia, hnie;
    extern /* Subroutine */ int koefp1_(real *), koefp2_(real *), koefp3_(
	    real *), sufe_(real *, real *, integer *, real *);
    static logical ursif2;
    static real zzz1;
    static integer msumo;
    static real ho05, hfixo, delx, ymaxx;
    extern doublereal rpid_(real *, real *, real *, integer *, real *, 
	    integer *, real *);
    static real yo2h0o, yoh0o, hfixo2, rdo2mx, ymo2z, aldo21, xhmf1;
    extern /* Subroutine */ int fieldg_(real *, real *, real *, real *, real *
	    , real *, real *, real *, real *, real *);
    extern doublereal foeedi_(real *, real *, real *, real *);
    static real elede;
    extern doublereal xen_(real *, real *, real *, real *, integer *, real *, 
	    real *, real *), xe_1__(real *);
    static real dipl1, dndhbr, hefold, babs1;
    extern /* Subroutine */ int calne_(integer *, real *, real *, real *, 
	    real *, real *, real *, real *, integer *, real *, real *);
    static real tnh;
    static integer seaday;
    static char filnam[120];	//static char filnam[12];	// made changes by Tuan Nguyen
    extern /* Subroutine */ int geodip_(integer *, real *, real *, real *, 
	    real *, integer *);
    static real abslat, absmdp, absmbr;
    static logical dnight, schalt, fnight;
    static integer iuccir;
    static real absmlt, sundec;
    static integer numhei, icoord, nseasn, season;
    static real invdip;
    static logical sam_ut__, teneop;
    static integer nrdaym;
    static real covsat, dndhmx;
    extern /* Subroutine */ int inilay_(logical *, logical *, real *, real *, 
	    real *, real *, real *, real *, real *, real *, real *, real *, 
	    real *, real *, real *, integer *);
    static logical layver;
    static integer nmonth, montho;
    static real xhinon;
    static logical ursifo;
    static real xkkmax, diplat;
    extern /* Subroutine */ int elteik_(integer *, integer *, integer *, real 
	    *, real *, real *, real *, real *, real *, real *, integer *, 
	    real *, real *, real *);
    static real tnahhi, rdomax;
    extern doublereal epstep_(real *, real *, real *, real *, real *);
    static real height, tih, teh, rox, rhx, rnx, rhex, rnox, hourut, ro2x;
    extern /* Subroutine */ int f1_prob__(real *, real *, real *, real *, 
	    real *);
    static real rclust;
    extern /* Subroutine */ int rdhhe_(real *, real *, real *, real *, real *,
	     real *, real *, real *);
    extern doublereal rdno_(real *, real *, real *, real *, real *);
    static real xnorm, edens;
    static integer ierror;
    static real htemp, drift;
    extern /* Subroutine */ int vdrift_(real *, real *, real *, real *);
    static real daynr1;
    extern /* Subroutine */ int spreadf_brazil__(integer *, integer *, real *,
	     real *, real *);
    static integer ispf;
    static logical sam_yea__;
    static real spreadf;
    extern /* Subroutine */ int dregion_(real *, integer *, real *, real *, 
	    real *, real *, real *);
    static logical sam_mon__, sam_doy__;
    static integer iseamon;
    extern /* Subroutine */ int ioncomp_(real *, integer *, integer *, real *,
	     real *, real *, real *, real *, real *, real *);
    static real spfhour;
    static logical f1_ocpro__;

    /* Fortran I/O blocks */
    static cilist io___44 = { 0, 0, 0, fmt_1939, 0 };
    static cilist io___54 = { 0, 0, 0, fmt_2911, 0 };
    static cilist io___55 = { 0, 0, 0, fmt_9012, 0 };
    static cilist io___56 = { 0, 0, 0, fmt_9013, 0 };
    static cilist io___57 = { 0, 0, 0, fmt_9014, 0 };
    static cilist io___58 = { 0, 0, 0, fmt_9206, 0 };
    static cilist io___59 = { 0, 0, 0, fmt_9204, 0 };
    static cilist io___60 = { 0, 0, 0, fmt_9205, 0 };
    static cilist io___61 = { 0, 0, 0, fmt_9015, 0 };
    static cilist io___62 = { 0, 0, 0, fmt_9016, 0 };
    static cilist io___63 = { 0, 0, 0, fmt_9017, 0 };
    static cilist io___64 = { 0, 0, 0, fmt_9018, 0 };
    static cilist io___65 = { 0, 0, 0, fmt_9019, 0 };
    static cilist io___66 = { 0, 0, 0, fmt_9021, 0 };
    static cilist io___67 = { 0, 0, 0, fmt_9022, 0 };
    static cilist io___68 = { 0, 0, 0, fmt_9023, 0 };
    static cilist io___69 = { 0, 0, 0, fmt_9024, 0 };
    static cilist io___70 = { 0, 0, 0, fmt_9025, 0 };
    static cilist io___71 = { 0, 0, 0, fmt_9026, 0 };
    static cilist io___72 = { 0, 0, 0, fmt_9027, 0 };
    static cilist io___73 = { 0, 0, 0, fmt_9028, 0 };
    static cilist io___74 = { 0, 0, 0, fmt_9029, 0 };
    static cilist io___75 = { 0, 0, 0, fmt_9031, 0 };
    static cilist io___76 = { 0, 0, 0, fmt_9039, 0 };
    static cilist io___77 = { 0, 0, 0, fmt_9032, 0 };
    static cilist io___78 = { 0, 0, 0, fmt_9033, 0 };
    static cilist io___79 = { 0, 0, 0, fmt_9034, 0 };
    static icilist io___155 = { 0, filnam, 0, fmt_104, 120, 1 };	// made changes by Tuan Nguyen
    static cilist io___156 = { 0, 0, 0, fmt_4689, 0 };
    static icilist io___159 = { 0, filnam, 0, fmt_1144, 120, 1 };	// made changes by Tuan Nguyen
    static cilist io___160 = { 0, 0, 0, fmt_4689, 0 };
    static icilist io___161 = { 0, filnam, 0, fmt_104, 120, 1 };	// made changes by Tuan Nguyen
    static cilist io___162 = { 0, 0, 0, fmt_4689, 0 };
    static icilist io___165 = { 0, filnam, 0, fmt_1144, 120, 1 };	// made changes by Tuan Nguyen
    static cilist io___166 = { 0, 0, 0, fmt_4689, 0 };
    static cilist io___167 = { 0, 0, 0, fmt_8449, 0 };
    static cilist io___235 = { 0, 0, 0, fmt_650, 0 };
    static cilist io___253 = { 0, 0, 0, fmt_11, 0 };
    static cilist io___254 = { 0, 0, 0, fmt_650, 0 };
    static cilist io___260 = { 0, 0, 0, fmt_100, 0 };
    static cilist io___262 = { 0, 0, 0, fmt_901, 0 };
    static cilist io___272 = { 0, 0, 0, fmt_7733, 0 };
    static cilist io___273 = { 0, 0, 0, fmt_7722, 0 };


/* ----------------------------------------------------------------- */

/* INPUT:  JF(1:30)      true/false switches for several options */
/*         JMAG          =0 geographic   = 1 geomagnetic coordinates */
/*         ALATI,ALONG   LATITUDE NORTH AND LONGITUDE EAST IN DEGREES */
/*         IYYYY         Year as YYYY, e.g. 1985 */
/*         MMDD (-DDD)   DATE (OR DAY OF YEAR AS A NEGATIVE NUMBER) */
/*         DHOUR         LOCAL TIME (OR UNIVERSAL TIME + 25) IN DECIMAL */
/*                          HOURS */
/*         HEIBEG,       HEIGHT RANGE IN KM; maximal 100 heights, i.e. */
/*          HEIEND,HEISTP        int((heiend-heibeg)/heistp)+1.le.100 */

/*    JF switches to turn off/on (.true./.false.) several options */

/*    i       .true.                  .flase.          standard version */
/*    ----------------------------------------------------------------- */
/*    1    Ne computed            Ne not computed                     t */
/*    2    Te, Ti computed        Te, Ti not computed                 t */
/*    3    Ne & Ni computed       Ni not computed                     t */
/*    4    B0 - Table option      B0 - Gulyaeva (1987)                t */
/*    5    foF2 - CCIR            foF2 - URSI                     false */
/*    6    Ni - DS-78 & DY-85     Ni - DS-95 & TTS-03             false */
/*    7    Ne - Tops: f10.7<188   f10.7 unlimited                     t */
/*    8    foF2 from model        foF2 or NmF2 - user input           t */
/*    9    hmF2 from model        hmF2 or M3000F2 - user input        t */
/*   10    Te - Standard          Te - Using Te/Ne correlation        t */
/*   11    Ne - Standard Profile  Ne - Lay-function formalism         t */
/*   12    Messages to unit 6     no messages                         t */
/*   13    foF1 from model        foF1 or NmF1 - user input           t */
/*   14    hmF1 from model        hmF1 - user input (only Lay version)t */
/*   15    foE  from model        foE or NmE - user input             t */
/*   16    hmE  from model        hmE - user input                    t */
/*   17    Rz12 from file         Rz12 - user input                   t */
/*   18    IGRF dip, magbr, modip old FIELDG using POGO68/10 for 1973 t */
/*   19    F1 probability model   critical solar zenith angle (old)   t */
/*   20    standard F1            standard F1 plus L condition        t */
/*   21    ion drift computed     ion drift not computed          false */
/*   22    ion densities in %     ion densities in m-3                t */
/*   23    Te_tops (Aeros,ISIS)   Te_topside (Intercosmos)        false */
/*   24    D-region: IRI-95       Special: 3 D-region models          t */
/*   25    F107D from AP.DAT      F107D user input (oarr(41))         t */
/*   26    foF2 storm model       no storm updating                   t */
/*   27    IG12 from file         IG12 - user input					 t */
/*   28    spread-F probability 	 not computed                    false */
/*   29    IRI01-topside          new options as def. by JF(30)   false */
/*   30    IRI01-topside corr.    NeQuick topside model   	     false */
/*     (29,30) = (t,t) IRIold, (f,t) IRIcor, (f,f) NeQuick, (t,f) TTS */
/*   ------------------------------------------------------------------ */

/*  Depending on the jf() settings additional INPUT parameters may */
/*  be required: */

/*       Setting              INPUT parameter */
/*    ----------------------------------------------------------------- */
/*    jf(8)  =.false.     OARR(1)=user input for foF2/MHz or NmF2/m-3 */
/*    jf(9)  =.false.     OARR(2)=user input for hmF2/km or M(3000)F2 */
/*    jf(10 )=.false.     OARR(15),OARR(16)=user input for Ne(300km), */
/*       Ne(400km)/m-3. Use OARR()=-1 if one of these values is not */
/*       available. If jf(23)=.false. then Ne(300km), Ne(550km)/m-3. */
/*    jf(13) =.false.     OARR(3)=user input for foF1/MHz or NmF1/m-3 */
/*    jf(14) =.false.     OARR(4)=user input for hmF1/km */
/*    jf(15) =.false.     OARR(5)=user input for foE/MHz or NmE/m-3 */
/*    jf(16) =.false.     OARR(6)=user input for hmE/km */
/*    jf(17) =.flase.     OARR(33)=user input for Rz12 */
/*    jf(21) =.true.      OARR(41)=user input for daily F10.7 index */
/*    jf(23) =.false.     OARR(41)=user input for daily F10.7 index */
/*    jf(24) =.false.     OARR(41)=user input for daily F10.7 index */
/*          optional for jf(21:24); default is F10.7D=COV */
/*    jf(25) =.false.     OARR(41)=user input for daily F10.7 index */
/*          if oarr(41).le.0 then 12-month running mean is */
/*          taken from internal file] */
/*    jf(27) =.flase.     OARR(39)=user input for IG12 */


/*  OUTPUT:  OUTF(1:20,1:500) */
/*               OUTF(1,*)  ELECTRON DENSITY/M-3 */
/*               OUTF(2,*)  NEUTRAL TEMPERATURE/K */
/*               OUTF(3,*)  ION TEMPERATURE/K */
/*               OUTF(4,*)  ELECTRON TEMPERATURE/K */
/*               OUTF(5,*)  O+ ION DENSITY/% or /M-3 if jf(22)=f */
/*               OUTF(6,*)  H+ ION DENSITY/% or /M-3 if jf(22)=f */
/*               OUTF(7,*)  HE+ ION DENSITY/% or /M-3 if jf(22)=f */
/*               OUTF(8,*)  O2+ ION DENSITY/% or /M-3 if jf(22)=f */
/*               OUTF(9,*)  NO+ ION DENSITY/% or /M-3 if jf(22)=f */
/*                 AND, IF JF(6)=.FALSE.: */
/*               OUTF(10,*)  CLUSTER IONS DEN/% or /M-3 if jf(22)=f */
/*               OUTF(11,*)  N+ ION DENSITY/% or /M-3 if jf(22)=f */
/*               OUTF(12,*) */
/*               OUTF(13,*) */
/*  if(jf(24)    OUTF(14,1:11) standard IRI-Ne for 60,65,..,110km */
/*     =.false.)        12:22) Friedrich (FIRI) model at these heights */
/*                      23:33) standard Danilov (SW=0, WA=0) */
/*                      34:44) for minor Stratospheric Warming (SW=0.5) */
/*                      45:55) for major Stratospheric Warming (SW=1) */
/*                      56:66) weak Winter Anomaly (WA=0.5) conditions */
/*                      67:77) strong Winter Anomaly (WA=1) conditions */
/*               OUTF(15-20,*)  free */

/*            OARR(1:50)   ADDITIONAL OUTPUT PARAMETERS */

/*      #OARR(1) = NMF2/M-3           #OARR(2) = HMF2/KM */
/*      #OARR(3) = NMF1/M-3           #OARR(4) = HMF1/KM */
/*      #OARR(5) = NME/M-3            #OARR(6) = HME/KM */
/*       OARR(7) = NMD/M-3             OARR(8) = HMD/KM */
/*       OARR(9) = HHALF/KM            OARR(10) = B0/KM */
/*       OARR(11) =VALLEY-BASE/M-3     OARR(12) = VALLEY-TOP/KM */
/*       OARR(13) = TE-PEAK/K          OARR(14) = TE-PEAK HEIGHT/KM */
/*      #OARR(15) = TE-MOD(300KM)     #OARR(16) = TE-MOD(400KM)/K */
/*       OARR(17) = TE-MOD(600KM)      OARR(18) = TE-MOD(1400KM)/K */
/*       OARR(19) = TE-MOD(3000KM)     OARR(20) = TE(120KM)=TN=TI/K */
/*       OARR(21) = TI-MOD(430KM)      OARR(22) = X/KM, WHERE TE=TI */
/*       OARR(23) = SOL ZENITH ANG/DEG OARR(24) = SUN DECLINATION/DEG */
/*       OARR(25) = DIP/deg            OARR(26) = DIP LATITUDE/deg */
/*       OARR(27) = MODIFIED DIP LAT.  OARR(28) = DELA */
/*       OARR(29) = sunrise/dec. hours OARR(30) = sunset/dec. hours */
/*       OARR(31) = ISEASON (1=spring) OARR(32) = NSEASON (northern) */
/*      #OARR(33) = Rz12               OARR(34) = Covington Index */
/*       OARR(35) = B1                 oarr(36) = M(3000)F2 */
/*      $oarr(37) = TEC/m-2           $oarr(38) = TEC_top/TEC*100. */
/*      #OARR(39) = gind (IG12)        oarr(40) = F1 probability (old) */
/*      #OARR(41) = F10.7 daily        oarr(42) = c1 (F1 shape) */
/*       OARR(43) = daynr */
/*       OARR(44) = equatorial vertical ion drift in m/s */
/*       OARR(45) = foF2_storm/foF2_quiet */
/*       OARR(46) = F1 probability without L condition */
/*       OARR(47) = F1 probability with L condition incl. */
/*       OARR(48) = spread-F occurrence probability (Brazilian model) */
/*                # INPUT as well as OUTPUT parameter */
/*                $ special for IRIWeb (only place-holders) */
/* ----------------------------------------------------------------------- */
/* ***************************************************************** */
/* *** THE ALTITUDE LIMITS ARE:  LOWER (DAY/NIGHT)  UPPER        *** */
/* ***     ELECTRON DENSITY         60/80 KM       1000 KM       *** */
/* ***     TEMPERATURES              120 KM        2500/3000 KM  *** */
/* ***     ION DENSITIES             100 KM        1000 KM       *** */
/* ***************************************************************** */
/* ***************************************************************** */
/* *********            INTERNALLY                    ************** */
/* *********       ALL ANGLES ARE IN DEGREE           ************** */
/* *********       ALL DENSITIES ARE IN M-3           ************** */
/* *********       ALL ALTITUDES ARE IN KM            ************** */
/* *********     ALL TEMPERATURES ARE IN KELVIN       ************** */
/* *********     ALL TIMES ARE IN DECIMAL HOURS       ************** */
/* ***************************************************************** */
/* ***************************************************************** */
/* ***************************************************************** */
/* -web-for webversion */
/*      CHARACTER FILNAM*53 */
    /* Parameter adjustments */
    --oarr;
    outf -= 21;
    --jf;

    /* Function Body */
    for (ki = 1; ki <= 20; ++ki) {
	for (kk = 1; kk <= 500; ++kk) {
/* L7397: */
	    outf[ki + kk * 20] = (float)-1.;
	}
    }
    for (kind = 7; kind <= 14; ++kind) {
/* L8398: */
	oarr[kind] = (float)-1.;
    }
    for (kind = 17; kind <= 32; ++kind) {
/* L8378: */
	oarr[kind] = (float)-1.;
    }
    for (kind = 34; kind <= 38; ++kind) {
/* L8478: */
	oarr[kind] = (float)-1.;
    }
    oarr[40] = (float)-1.;
    for (kind = 42; kind <= 50; ++kind) {
/* L8428: */
	oarr[kind] = (float)-1.;
    }

/* PROGRAM CONSTANTS */

    ++const2_1.icalls;
    argexp_1.argmax = (float)88.;
    pi = atan((float)1.) * (float)4.;
    const_1.umr = pi / (float)180.;
    const1_1.humr = pi / (float)12.;
    const1_1.dumr = pi / (float)182.5;
    alog2 = log((float)2.);
    blo11_1.alg10 = log((float)10.);
    alg100 = log((float)100.);
    numhei = (integer) ((r__1 = *heiend - *heibeg, dabs(r__1)) / dabs(*heistp)
	    ) + 1;
    if (numhei > 500) {
	numhei = 500;
    }

/* Code inserted to aleviate block data problem for PC version. */
/* Thus avoiding DATA statement with parameters from COMMON block. */

    xdels[0] = (float)5.;
    xdels[1] = (float)5.;
    xdels[2] = (float)5.;
    xdels[3] = (float)10.;
    dnds[0] = (float).016;
    dnds[1] = (float).01;
    dnds[2] = (float).016;
    dnds[3] = (float).016;
    ddo[0] = 9;
    ddo[1] = 5;
    ddo[2] = 5;
    ddo[3] = 25;
    do2[0] = 5;
    do2[1] = 5;
    xnar[0] = (float)0.;
    xnar[1] = (float)0.;
    blote_1.dte[0] = (float)5.;
    blote_1.dte[1] = (float)5.;
    blote_1.dte[2] = (float)10.;
    blote_1.dte[3] = (float)20.;
    blote_1.dte[4] = (float)20.;
    block8_1.dti[0] = (float)10.;
    block8_1.dti[1] = (float)10.;
    block8_1.dti[2] = (float)20.;
    block8_1.dti[3] = (float)20.;

/* FIRST SPECIFY YOUR COMPUTERS CHANNEL NUMBERS .................... */
/* AGNR=OUTPUT (OUTPUT IS DISPLAYED OR STORED IN FILE OUTPUT.IRI)... */
/* IUCCIR=UNIT NUMBER FOR CCIR COEFFICIENTS ........................ */
/* set konsol=1 if you do not want the konsol information */

    iuccir = 10;
/* -web- special for web version */
/* -web-      KONSOL=1 */
    iounit_1.konsol = 6;
    if (! jf[12]) {
	iounit_1.konsol = 1;
    }
/*        if(.not.jf(12)) then */
/*                konsol=11 */
/*                open(11,file='messages.txt') */
/*                endif */

/* selection of density, temperature and ion composition options ...... */

    noden = ! jf[1];
    notem = ! jf[2];
    noion = ! jf[3];
    if (! noion) {
	noden = FALSE_;
    }
    dy = ! jf[6];
    layver = ! jf[11];
    old79 = ! jf[7];
    gulb0 = ! jf[4];
    f1_ocpro__ = jf[19];
    f1_l_cond__ = FALSE_;
    if (f1_ocpro__) {
	f1_l_cond__ = ! jf[20];
    }
    dreg = jf[24];
    topo = jf[29];
    topc = jf[30];

/* rz12, F10.7D input option .......................................... */

    rzin = ! jf[17];
    if (rzin) {
	arzin = oarr[33];
    } else {
	oarr[33] = (float)-1.;
    }
    igin = ! jf[27];
    if (igin) {
	aigin = oarr[39];
    } else {
	oarr[39] = (float)-1.;
    }
    if (! jf[25]) {
	f107d = oarr[41];
    } else {
	oarr[41] = (float)-1.;
    }

/* Topside density .................................................... */

    if (topo) {
	if (topc) {
	    blo11_1.itopn = 0;
	} else {
	    blo11_1.itopn = 3;
	}
    } else {
	if (topc) {
	    blo11_1.itopn = 1;
	} else {
	    blo11_1.itopn = 2;
	}
    }

/* F2 peak density .................................................... */

    fof2in = ! jf[8];
    if (fof2in) {
	oarr1 = oarr[1];
	afof2 = oarr1;
	anmf2 = oarr1;
	if (oarr1 < (float)100.) {
	    anmf2 = afof2 * (float)1.24e10 * afof2;
	}
	if (oarr1 >= (float)100.) {
	    afof2 = sqrt(anmf2 / (float)1.24e10);
	}
    } else {
	oarr[1] = (float)-1.;
    }
    ursif2 = ! jf[5];

/* F2 peak altitude .................................................. */

    hmf2in = ! jf[9];
    if (hmf2in) {
	ahmf2 = oarr[2];
    } else {
	oarr[2] = (float)-1.;
    }

/* F1 peak density ................................................... */

    fof1in = ! jf[13];
    if (fof1in) {
	oarr3 = oarr[3];
	afof1 = oarr3;
	anmf1 = oarr3;
	if (oarr3 < (float)100.) {
	    anmf1 = afof1 * (float)1.24e10 * afof1;
	}
	if (oarr3 >= (float)100.) {
	    afof1 = sqrt(anmf1 / (float)1.24e10);
	}
    } else {
	oarr[3] = (float)-1.;
    }

/* F1 peak altitude .................................................. */

    hmf1in = ! jf[14];
    if (hmf1in) {
	ahmf1 = oarr[4];
	if (! layver && iounit_1.konsol > 1) {
	    io___44.ciunit = iounit_1.konsol;
	    s_wsfe(&io___44);
	    e_wsfe();
	}
    } else {
	oarr[4] = (float)-1.;
    }

/* E peak density .................................................... */

    foein = ! jf[15];
    if (foein) {
	oarr5 = oarr[5];
	afoe = oarr5;
	anme = oarr5;
	if (oarr5 < (float)100.) {
	    anme = afoe * (float)1.24e10 * afoe;
	}
	if (oarr5 >= (float)100.) {
	    afoe = sqrt(anme / (float)1.24e10);
	}
    } else {
	oarr[5] = (float)-1.;
    }

/* E peak altitude .................................................. */

    hmein = ! jf[16];
    if (hmein) {
	ahme = oarr[6];
    } else {
	oarr[6] = (float)-1.;
    }

/* TE-NE MODEL OPTION .............................................. */

    teneop = ! jf[10];
    if (teneop) {
	for (jxnar = 1; jxnar <= 2; ++jxnar) {
	    xnar[jxnar - 1] = oarr[jxnar + 14];
	    tecon[jxnar - 1] = FALSE_;
/* L8154: */
	    if (xnar[jxnar - 1] > (float)0.) {
		tecon[jxnar - 1] = TRUE_;
	    }
	}
    } else {
	oarr[15] = (float)-1.;
	oarr[16] = (float)-1.;
    }


/* lists the selected options before starting the table */

    if (const2_1.icalls > 1 || iounit_1.konsol == 1) {
	goto L8201;
    }
    io___54.ciunit = iounit_1.konsol;
    s_wsfe(&io___54);
    e_wsfe();
    if (noden) {
	goto L2889;
    }
    if (layver) {
	io___55.ciunit = iounit_1.konsol;
	s_wsfe(&io___55);
	e_wsfe();
    }
    if (gulb0) {
	io___56.ciunit = iounit_1.konsol;
	s_wsfe(&io___56);
	e_wsfe();
    }
    if (old79) {
	io___57.ciunit = iounit_1.konsol;
	s_wsfe(&io___57);
	e_wsfe();
    }
    if (topo && ! topc) {
	io___58.ciunit = iounit_1.konsol;
	s_wsfe(&io___58);
	e_wsfe();
    }
    if (! topo) {
	if (topc) {
	    io___59.ciunit = iounit_1.konsol;
	    s_wsfe(&io___59);
	    e_wsfe();
	} else {
	    io___60.ciunit = iounit_1.konsol;
	    s_wsfe(&io___60);
	    e_wsfe();
	}
    }
    if (fof2in) {
	io___61.ciunit = iounit_1.konsol;
	s_wsfe(&io___61);
	e_wsfe();
	goto L2889;
    }
    if (ursif2) {
	io___62.ciunit = iounit_1.konsol;
	s_wsfe(&io___62);
	e_wsfe();
    } else {
	io___63.ciunit = iounit_1.konsol;
	s_wsfe(&io___63);
	e_wsfe();
    }
    if (hmf2in) {
	io___64.ciunit = iounit_1.konsol;
	s_wsfe(&io___64);
	e_wsfe();
    }
    if (fof1in) {
	io___65.ciunit = iounit_1.konsol;
	s_wsfe(&io___65);
	e_wsfe();
    }
    if (hmf1in && layver) {
	io___66.ciunit = iounit_1.konsol;
	s_wsfe(&io___66);
	e_wsfe();
    }
    if (foein) {
	io___67.ciunit = iounit_1.konsol;
	s_wsfe(&io___67);
	e_wsfe();
    }
    if (hmein) {
	io___68.ciunit = iounit_1.konsol;
	s_wsfe(&io___68);
	e_wsfe();
    }
    if (f1_ocpro__) {
	io___69.ciunit = iounit_1.konsol;
	s_wsfe(&io___69);
	e_wsfe();
    }
    if (f1_l_cond__) {
	io___70.ciunit = iounit_1.konsol;
	s_wsfe(&io___70);
	e_wsfe();
    }
    if (dreg) {
	io___71.ciunit = iounit_1.konsol;
	s_wsfe(&io___71);
	e_wsfe();
    } else {
	io___72.ciunit = iounit_1.konsol;
	s_wsfe(&io___72);
	e_wsfe();
    }
    if (jf[26]) {
	if (fof2in) {
	    io___73.ciunit = iounit_1.konsol;
	    s_wsfe(&io___73);
	    e_wsfe();
	    jf[26] = FALSE_;
	} else {
	    io___74.ciunit = iounit_1.konsol;
	    s_wsfe(&io___74);
	    e_wsfe();
	}
    }
L2889:
    if (! noion && dy) {
	io___75.ciunit = iounit_1.konsol;
	s_wsfe(&io___75);
	e_wsfe();
    }
    if (! noion && ! dy) {
	io___76.ciunit = iounit_1.konsol;
	s_wsfe(&io___76);
	e_wsfe();
    }
    if (notem) {
	goto L8201;
    }
    if (teneop) {
	io___77.ciunit = iounit_1.konsol;
	s_wsfe(&io___77);
	e_wsfe();
    }
    if (jf[23]) {
	io___78.ciunit = iounit_1.konsol;
	s_wsfe(&io___78);
	e_wsfe();
    } else {
	io___79.ciunit = iounit_1.konsol;
	s_wsfe(&io___79);
	e_wsfe();
    }
L8201:
/* CALCULATION OF DAY OF YEAR OR MONTH/DAY AND DECIMAL YEAR */

/*  leap year rule: years evenly divisible by 4 are leap years, except */
/*  years also evenly divisible by 100 are not leap years, except years */
/*  also evenly divisible by 400 are leap years. The year 2000 is a 100 */
/*  and 400 year exception and therefore it is a normal leap year. */
/*  The next 100 year exception will be in the year 2100! */

    iyear = *iyyyy;
    if (iyear < 100) {
	iyear += 1900;
    }
    idayy = 365;
    if (iyear / 4 << 2 == iyear) {
	idayy = 366;
    }
/* leap year */
    if (*mmdd < 0) {
	daynr = -(*mmdd);
	moda_(&c__1, &iyear, &month, &iday, &daynr, &nrdaym);
    } else {
	month = *mmdd / 100;
	iday = *mmdd - month * 100;
	moda_(&c__0, &iyear, &month, &iday, &daynr, &nrdaym);
    }
    ryear = iyear + (daynr - (float)1.) / idayy;

/* CALCULATION OF GEODETIC/GEOMAGNETIC COORDINATES (LATI, LONGI AND */
/* MLAT, MLONG), MAGNETIC INCLINATION (DIP), DIP LATITUDE (MAGBR) */
/* AND MODIFIED DIP (MODIP), ALL IN DEGREES */

    if (*along < (float)0.) {
	*along += (float)360.;
    }
/* -180/180 to 0-360 */
    if (*jmag > 0) {
	mlat = *alati;
	mlong = *along;
    } else {
	lati = *alati;
	longi = *along;
    }

    geodip_(&iyear, &lati, &longi, &mlat, &mlong, jmag);
/*        CALL GGM(JMAG,XLONGI1,XLATI1,XMLONG1,XMLAT1) */

    igrf_dip__(&lati, &longi, &ryear, &c_b66, &dip, &magbr, &modip, ier);
    if (*ier != 0) {
	return 0;
    }
    if (! jf[18]) {
	fieldg_(&lati, &longi, &c_b66, &xma, &yma, &zma, &bet, &dip, &dec, &
		modip);
	magbr = atan(tan(dip * const_1.umr) * (float).5) / const_1.umr;
    }
    abslat = dabs(lati);
    absmlt = dabs(mlat);
    absmdp = dabs(modip);
    absmbr = dabs(magbr);

/* CALCULATION OF UT/LT  ............... */

    if (*dhour <= (float)24.) {
	hour = *dhour;
/* dhour =< 24 is LT */
	hourut = hour - longi / (float)15.;
	if (hourut < (float)0.) {
	    hourut += (float)24.;
	}
    } else {
	hourut = *dhour - (float)25.;
/* dhour>24 is UT+25 */
	hour = hourut + longi / (float)15.;
/* hour becomes LT */
	if (hour > (float)24.) {
	    hour += (float)-24.;
	}
    }

/* season assumes equal length seasons (92 days) with spring */
/* (season=1) starting at day-of-year=47; for lati<0 adjustment */
/* for southern hemisphere is made. some models require the */
/* seasonal month or the seasonal day-of year */

/* zmonth is decimal month (Jan 1 = 1.0 and Dec 31 = 12.97) */
/* 		zmonth = month + (iday*1.-1.)/nrdaym */
/* is not used currently */
    season = (integer) ((daynr + (float)45.) / (float)92.);
    if (season < 1) {
	season = 4;
    }
    nseasn = season;
/* Northern hemisphere season */
    seaday = daynr;
    iseamon = month;

    if (lati >= (float)0.) {
	goto L5592;
    }
    season += -2;
    if (season < 1) {
	season += 4;
    }
    iseamon = month + 6;
    if (iseamon > 12) {
	iseamon += -12;
    }
    seaday = daynr + idayy / (float)2.;
    if (seaday > idayy) {
	seaday -= idayy;
    }

/* 12-month running mean sunspot number (rssn) and Ionospheric Global */
/* index (gind), daily F10.7 cm solar radio flux (f107d) and monthly */
/* F10.7 (cov) index */

L5592:
    sam_mon__ = month == montho;
    sam_yea__ = iyear == const2_1.iyearo;
    sam_doy__ = daynr == const2_1.idaynro;
    sam_date__ = sam_yea__ && sam_doy__;
    sam_ut__ = hourut == const2_1.ut0;
    if (sam_date__ && ! const2_1.rzino && ! rzin && ! igin && ! 
	    const2_1.igino) {
	goto L2910;
    }
    tcon_(&iyear, &month, &iday, &daynr, rzar, arig, &ttt, &nmonth);
    if (nmonth < 0) {
	goto L3330;
    }
/* jump to end of program */
    if (rzin) {
	rrr = arzin;
	rzar[0] = rrr;
	rzar[1] = rrr;
	rzar[2] = rrr;
/*       	zi=-12.349154+(1.4683266-2.67690893e-03*rrr)*rrr */
/*       	if(zi.gt.174.0) zi=174.0 */
/*       	arig(1) = zi */
/*       	arig(2) = zi */
/*       	arig(3) = zi */
    }
    if (igin) {
	zi = aigin;
	arig[0] = zi;
	arig[1] = zi;
	arig[2] = zi;
    }
    rssn = rzar[2];
    gind = arig[2];
    cov = rssn * (rssn * (float)8.9e-4 + (float).728) + (float)63.75;
/*        rlimit=gind */
/*        COVSAT=63.75+rlimit*(0.728+rlimit*0.00089) */
    covsat = cov;
    if (covsat > (float)188.) {
	covsat = (float)188.;
    }
    if (jf[25]) {
	f107d = cov;
	f107m = cov;
	apf_only__(&iyear, &month, &iday, &f107dx, &f107mx);
	if (f107dx > (float)-100.) {
	    f107d = f107dx;
	    f107m = f107mx;
	}
    }

/* CALCULATION OF SOLAR ZENITH ANGLE (XHI/DEG), SUN DECLINATION ANGLE */
/* (SUNDEC),SOLAR ZENITH ANGLE AT NOON (XHINON) AND TIME OF LOCAL */
/* SUNRISE/SUNSET (SAX, SUX; dec. hours) AT 70 KM (D-REGION), 110 KM */
/* (E-REGION), 200 KM (F1-REGION), AND 500 KM (TE, TI). */

L2910:
    soco_(&daynr, &hour, &lati, &longi, &c_b71, &sundec, &xhi1, &sax80, &
	    sux80);
    soco_(&daynr, &hour, &lati, &longi, &c_b72, &sud1, &xhi2, &sax110, &
	    sux110);
    soco_(&daynr, &hour, &lati, &longi, &c_b73, &sud1, &xhi, &sax200, &sux200)
	    ;
    soco_(&daynr, &hour, &lati, &longi, &c_b66, &sud1, &xhi3, &sax300, &
	    sux300);
    soco_(&daynr, &c_b75, &lati, &longi, &c_b72, &sunde1, &xhinon, &sax1, &
	    sux1);
    dnight = FALSE_;
    if (dabs(sax80) > (float)25.) {
	if (sax80 < (float)0.) {
	    dnight = TRUE_;
	}
	goto L9334;
    }
    if (sax80 <= sux80) {
	goto L1386;
    }
    if (hour > sux80 && hour < sax80) {
	dnight = TRUE_;
    }
    goto L9334;
L1386:
    if (hour > sux80 || hour < sax80) {
	dnight = TRUE_;
    }
L9334:
    block5_1.enight = FALSE_;
    if (dabs(sax110) > (float)25.) {
	if (sax110 < (float)0.) {
	    block5_1.enight = TRUE_;
	}
	goto L8334;
    }
    if (sax110 <= sux110) {
	goto L9386;
    }
    if (hour > sux110 && hour < sax110) {
	block5_1.enight = TRUE_;
    }
    goto L8334;
L9386:
    if (hour > sux110 || hour < sax110) {
	block5_1.enight = TRUE_;
    }
L8334:
    fnight = FALSE_;
    if (dabs(sax200) > (float)25.) {
	if (sax200 < (float)0.) {
	    fnight = TRUE_;
	}
	goto L8335;
    }
    if (sax200 <= sux200) {
	goto L7386;
    }
    if (hour > sux200 && hour < sax200) {
	fnight = TRUE_;
    }
    goto L8335;
L7386:
    if (hour > sux200 || hour < sax200) {
	fnight = TRUE_;
    }
L8335:
    fnight = FALSE_;
    if (dabs(sax300) > (float)25.) {
	if (sax300 < (float)0.) {
	    fnight = TRUE_;
	}
	goto L1334;
    }
    if (sax300 <= sux300) {
	goto L7387;
    }
    if (hour > sux300 && hour < sax300) {
	fnight = TRUE_;
    }
    goto L1334;
L7387:
    if (hour > sux300 || hour < sax300) {
	fnight = TRUE_;
    }

/* CALCULATION OF ELECTRON DENSITY PARAMETERS................ */
/* lower height boundary (HNEA), upper boundary (HNEE) */

L1334:
    hnea = (float)65.;
    if (dnight) {
	hnea = (float)80.;
    }
    hnee = (float)2e3;
    if (noden) {
	goto L4933;
    }
    dela = (float)4.32;
    if (absmdp >= (float)18.) {
	dela = exp(-(absmdp - (float)30.) / (float)10.) + (float)1.;
    }
    dell = exp(-(abslat - (float)20.) / (float)10.) + 1;

/* E peak critical frequency (foF2), density (NmE), and height (hmE) */

    if (foein) {
	foe = afoe;
	block4_1.nme = anme;
    } else {
	foe = foeedi_(&cov, &xhi, &xhinon, &abslat);
	block4_1.nme = foe * (float)1.24e10 * foe;
    }
    if (hmein) {
	block4_1.hme = ahme;
    } else {
	block4_1.hme = (float)110.;
    }

/* F2 peak critical frequency foF2, density NmF2, and height hmF2 */

/* READ CCIR AND URSI COEFFICIENT SET FOR CHOSEN MONTH */

    if (fof2in && hmf2in && blo11_1.itopn != 2) {
	goto L501;
    }
    if (ursif2 != ursifo) {
	goto L7797;
    }
    if (! rzin && ! const2_1.rzino && ! igin && ! const2_1.igino) {
	if (sam_mon__ && nmonth == const2_1.nmono && sam_yea__) {
	    goto L4292;
	}
	if (sam_mon__) {
	    goto L4293;
	}
    }

/* the program expects the coefficients files in ASCII format; if you */
/* want to use the binary version of the coefficients, please use the */
/* the statements that are commented-out below and comment-out the */
/* ASCII-related statements. */

L7797:
    ursifo = ursif2;
    s_wsfi(&io___155);
    i__1 = month + 10;
    do_fio(&c__1, (char *)&i__1, (ftnlen)sizeof(integer));
    e_wsfi();
/* -binary- if binary files than use: */
/* -binary-104   FORMAT('ccir',I2,'.bin') */
/* -web- special for web-version: */
/* 104   FORMAT('/usr/local/etc/httpd/cgi-bin/models/IRI/ccir',I2,'.asc') */
/* -web- special for web-version: */

    o__1.oerr = 1;
    o__1.ounit = iuccir;
    o__1.ofnm = filnam;											// made changes by Tuan Nguyen
    o__1.ofnmlen = strlen(o__1.ofnm);	// o__1.ofnmlen = 12;	// made changes by Tuan Nguyen
    o__1.orl = 0;
    o__1.osta = "OLD";
    o__1.oacc = 0;
    o__1.ofm = "FORMATTED";
    o__1.oblnk = 0;
    i__1 = f_open(&o__1);
    if (i__1 != 0) {
	goto L8448;
    }
/* -binary- if binary files than use: */
/* -binary-     &          FORM='UNFORMATTED') */

    io___156.ciunit = iuccir;
    s_rsfe(&io___156);
    do_fio(&c__1976, (char *)&f2[0], (ftnlen)sizeof(real));
    do_fio(&c__882, (char *)&fm3[0], (ftnlen)sizeof(real));
    e_rsfe();
/* -binary- if binary files than use: */
/* -binary-        READ(IUCCIR) F2,FM3 */

    cl__1.cerr = 0;
    cl__1.cunit = iuccir;
    cl__1.csta = 0;
    f_clos(&cl__1);

/* then URSI if chosen .................................... */

    if (ursif2) {
	s_wsfi(&io___159);
	i__1 = month + 10;
	do_fio(&c__1, (char *)&i__1, (ftnlen)sizeof(integer));
	e_wsfi();
/* -web- special for web-version: */
/* 1144  FORMAT('/usr/local/etc/httpd/cgi-bin/models/IRI/ursi',I2,'.asc') */
/* -binary- if binary files than use: */
/* -binary-1144          FORMAT('ursi',I2,'.bin') */
	o__1.oerr = 1;
	o__1.ounit = iuccir;
	o__1.ofnm = filnam;											// made changes by Tuan Nguyen
	o__1.ofnmlen = strlen(o__1.ofnm);	//o__1.ofnmlen = 12;	// made changes by Tuan Nguyen
	o__1.orl = 0;
	o__1.osta = "OLD";
	o__1.oacc = 0;
	o__1.ofm = "FORMATTED";
	o__1.oblnk = 0;
	i__1 = f_open(&o__1);
	if (i__1 != 0) {
	    goto L8448;
	}
/* -binary- if binary files than use: */
/* -binary-     &         FORM='UNFORMATTED') */
	io___160.ciunit = iuccir;
	s_rsfe(&io___160);
	do_fio(&c__1976, (char *)&f2[0], (ftnlen)sizeof(real));
	e_rsfe();
/* -binary- if binary files than use: */
/* -binary-          READ(IUCCIR) F2 */
	cl__1.cerr = 0;
	cl__1.cunit = iuccir;
	cl__1.csta = 0;
	f_clos(&cl__1);
    }

/* READ CCIR AND URSI COEFFICIENT SET FOR NMONTH, i.e. previous */
/* month if day is less than 15 and following month otherwise */

L4293:

/* first CCIR .............................................. */

    s_wsfi(&io___161);
    i__1 = nmonth + 10;
    do_fio(&c__1, (char *)&i__1, (ftnlen)sizeof(integer));
    e_wsfi();
    o__1.oerr = 1;
    o__1.ounit = iuccir;
    o__1.ofnm = filnam;											// made changes by Tuan Nguyen
    o__1.ofnmlen = strlen(o__1.ofnm);	// o__1.ofnmlen = 12;	// made changes by Tuan Nguyen
    o__1.orl = 0;
    o__1.osta = "OLD";
    o__1.oacc = 0;
    o__1.ofm = "FORMATTED";
    o__1.oblnk = 0;
    i__1 = f_open(&o__1);
    if (i__1 != 0) {
	goto L8448;
    }
/* -binary- if binary files than use: */
/* -binary-     &          FORM='unFORMATTED') */
    io___162.ciunit = iuccir;
    s_rsfe(&io___162);
    do_fio(&c__1976, (char *)&f2n[0], (ftnlen)sizeof(real));
    do_fio(&c__882, (char *)&fm3n[0], (ftnlen)sizeof(real));
    e_rsfe();
/* -binary- if binary files than use: */
/* -binary-        READ(IUCCIR) F2N,FM3N */
    cl__1.cerr = 0;
    cl__1.cunit = iuccir;
    cl__1.csta = 0;
    f_clos(&cl__1);

/* then URSI if chosen ..................................... */

    if (ursif2) {
	s_wsfi(&io___165);
	i__1 = nmonth + 10;
	do_fio(&c__1, (char *)&i__1, (ftnlen)sizeof(integer));
	e_wsfi();
	o__1.oerr = 1;
	o__1.ounit = iuccir;
	o__1.ofnm = filnam;											// made changes by Tuan Nguyen
	o__1.ofnmlen = strlen(o__1.ofnm);	//o__1.ofnmlen = 12;	// made changes by Tuan Nguyen
	o__1.orl = 0;
	o__1.osta = "OLD";
	o__1.oacc = 0;
	o__1.ofm = "FORMATTED";
	o__1.oblnk = 0;
	i__1 = f_open(&o__1);
	if (i__1 != 0) {
	    goto L8448;
	}
/* -binary- if binary files than use: */
/* -binary-     &         FORM='unFORMATTED') */
	io___166.ciunit = iuccir;
	s_rsfe(&io___166);
	do_fio(&c__1976, (char *)&f2n[0], (ftnlen)sizeof(real));
	e_rsfe();
/* -binary- if binary files than use: */
/* -binary-          READ(IUCCIR) F2N */
	cl__1.cerr = 0;
	cl__1.cunit = iuccir;
	cl__1.csta = 0;
	f_clos(&cl__1);
    }
    goto L4291;
L8448:
    io___167.ciunit = iounit_1.konsol;
    s_wsfe(&io___167);
    do_fio(&c__1, filnam, (ftnlen)120);	//do_fio(&c__1, filnam, (ftnlen)12);	// made changes by Tuan Nguyen
    e_wsfe();
    goto L3330;

/* LINEAR INTERPOLATION IN SOLAR ACTIVITY. IG12 used for foF2 */

L4291:
    rr2 = arig[0] / (float)100.;
    rr2n = arig[1] / (float)100.;
    rr1 = (float)1. - rr2;
    rr1n = (float)1. - rr2n;
    for (i__ = 1; i__ <= 76; ++i__) {
	for (j = 1; j <= 13; ++j) {
	    k = j + (i__ - 1) * 13;
	    ff0n[k - 1] = f2n[j + (i__ + 76) * 13 - 1002] * rr1n + f2n[j + (
		    i__ + 152) * 13 - 1002] * rr2n;
/* L20: */
	    ff0[k - 1] = f2[j + (i__ + 76) * 13 - 1002] * rr1 + f2[j + (i__ + 
		    152) * 13 - 1002] * rr2;
	}
    }
    rr2 = rzar[0] / (float)100.;
    rr2n = rzar[1] / (float)100.;
    rr1 = (float)1. - rr2;
    rr1n = (float)1. - rr2n;
    for (i__ = 1; i__ <= 49; ++i__) {
	for (j = 1; j <= 9; ++j) {
	    k = j + (i__ - 1) * 9;
	    xm0n[k - 1] = fm3n[j + (i__ + 49) * 9 - 451] * rr1n + fm3n[j + (
		    i__ + 98) * 9 - 451] * rr2n;
/* L30: */
	    xm0[k - 1] = fm3[j + (i__ + 49) * 9 - 451] * rr1 + fm3[j + (i__ + 
		    98) * 9 - 451] * rr2;
	}
    }
L4292:
    zfof2 = fout_(&modip, &lati, &longi, &hourut, ff0);
    fof2n = fout_(&modip, &lati, &longi, &hourut, ff0n);
    zm3000 = xmout_(&modip, &lati, &longi, &hourut, xm0);
    xm300n = xmout_(&modip, &lati, &longi, &hourut, xm0n);
    midm = 15;
    if (month == 2) {
	midm = 14;
    }
    if (iday < midm) {
	yfof2 = fof2n + ttt * (zfof2 - fof2n);
	xm3000 = xm300n + ttt * (zm3000 - xm300n);
    } else {
	yfof2 = zfof2 + ttt * (fof2n - zfof2);
	xm3000 = zm3000 + ttt * (xm300n - zm3000);
    }
L501:
    if (fof2in) {
	fof2 = afof2;
	block1_1.nmf2 = anmf2;
    } else {
	fof2 = yfof2;
	block1_1.nmf2 = fof2 * (float)1.24e10 * fof2;
    }
    if (hmf2in) {
	if (ahmf2 < (float)50.) {
	    xm3000 = ahmf2;
	    r__1 = fof2 / foe;
	    block1_1.hmf2 = hmf2ed_(&magbr, &rssn, &r__1, &xm3000);
	} else {
	    block1_1.hmf2 = ahmf2;
/*          	XM3000=XM3000HM(MAGBR,RSSN,FOF2/FOE,HMF2) */
	}
    } else {
	r__1 = fof2 / foe;
	block1_1.hmf2 = hmf2ed_(&magbr, &rssn, &r__1, &xm3000);
    }

/* stormtime updating */

    stormcorr = (float)-1.;
    if (jf[26] && jf[8]) {
	if (! sam_date__ || ! sam_ut__) {
	    apf_(&iyear, &month, &iday, &hourut, indap);
	}
	if (indap[0] > -2) {
	    icoord = 1;
	    kut = (integer) hourut;
	    storm_(indap, &lati, &longi, &icoord, &cglat, &kut, &daynr, &
		    stormcorr);
	    fof2 *= stormcorr;
	    block1_1.nmf2 = fof2 * (float)1.24e10 * fof2;
	}
    }
    const2_1.nmono = nmonth;
    montho = month;
    const2_1.iyearo = iyear;
    const2_1.idaynro = daynr;
    const2_1.rzino = rzin;
    const2_1.igino = igin;
    const2_1.ut0 = hourut;

/* topside profile parameters ............................. */

    cos2 = cos(mlat * const_1.umr);
    cos2 *= cos2;
    flu = (covsat - (float)40.) / (float)30.;
/* option to use unlimiited F10.7M for the topside */
    if (old79) {
	flu = (cov - (float)40.) / (float)30.;
    }
/* previously: IF(OLD79) ETA1=-0.0070305*COS2 */
    ex = exp(-mlat / (float)15.);
    ex1 = ex + 1;
    epin = ex * (float)4. / (ex1 * ex1);
    eta1 = epin * (float)-.02;
    blo10_1.eta = eta1 + (float).058798 - flu * ((float).014065 - cos2 * (
	    float).0069724) + fof2 * (cos2 * (float).004281 + (float).0024287 
	    - fof2 * (float)1.528e-4);
/*        fluu=flu */
/* orr    if(fluu.gt.flumax) fluu=flumax */
    blo10_1.zeta = (float).078922 - cos2 * (float).0046702 - flu * ((float)
	    .019132 - cos2 * (float).0076545) + fof2 * (cos2 * (float).006029 
	    + (float).0032513 - fof2 * (float)2.0872e-4);
    blo10_1.beta = cos2 * (float)20.253 - (float)128.03 - flu * (cos2 * (
	    float).65896 + (float)8.0755) + fof2 * (cos2 * (float).71458 + (
	    float).44041 - fof2 * (float).042966);
    z__ = exp((float)94.5 / blo10_1.beta);
/* orr    Z=EXP(94.45/BETA) */
    z1 = z__ + 1;
    z2 = z__ / (blo10_1.beta * z1 * z1);
    blo10_1.delta = (blo10_1.eta / z1 - blo10_1.zeta / (float)2.) / (
	    blo10_1.eta * z2 + blo10_1.zeta / (float)400.);

/* Correction term for topside */

    if (blo11_1.itopn == 1) {
	zmp1 = exp(modip / (float)10.);
	zmp11 = zmp1 + (float)1.;
	zmp111 = zmp1 / (zmp11 * zmp11);
	zmp2 = exp(modip / (float)19.);
	zmp22 = zmp2 + (float)1.;
	zmp222 = zmp2 / (zmp22 * zmp22);
	r2n = (float)-.84 - zmp111 * (float)1.6;
	r2d = (float)-.84 - zmp111 * (float).64;
	x1n = (float)230. - zmp222 * (float)700.;
	x1d = (float)550. - zmp222 * (float)1900.;
	r2 = hpol_(&hour, &r2d, &r2n, &sax300, &sux300, &c_b121, &c_b121);
	x1 = hpol_(&hour, &x1d, &x1n, &sax300, &sux300, &c_b121, &c_b121);
	blo11_1.hcor1 = block1_1.hmf2 + x1;
	x12 = (float)1500. - x1;
	blo11_1.tc3 = r2 / x12;
    }

/* NeQuick topside parameters (use CCIR-M3000F2 even if user-hmF2) */

    if (blo11_1.itopn == 2) {
	dndhmx = log(fof2) * (float)1.714 - (float)3.467 + log(xm3000) * (
		float)2.02;
	dndhmx = exp(dndhmx) * (float).01;
	b2bot = fof2 * (float).04774 * fof2 / dndhmx;
	b2k = (float)3.22 - fof2 * (float).0538 - block1_1.hmf2 * (float)
		.00664 + block1_1.hmf2 * (float).113 / b2bot + rssn * (float)
		.00257;
	ee = exp((b2k - (float)1.) * (float)2.);
	b2k = (b2k * ee + (float)1.) / (ee + (float)1.);
	blo11_1.b2top = b2k * b2bot;
    }

/* F layer - bottomside thickness parameter B0 and shape parameters B1 */

    block2_1.b1 = hpol_(&hour, &c_b125, &c_b126, &sax200, &sux200, &c_b121, &
	    c_b121);
    if (gulb0) {
	rogul_(&seaday, &xhi, &seax, &grat);
	if (fnight) {
	    grat = (float).91 - block1_1.hmf2 / (float)4e3;
	}
	bcoef = block2_1.b1 * (block2_1.b1 * (block2_1.b1 * (float).0046 - (
		float).0548) + (float).2546) + (float).3606;
	b0cnew = block1_1.hmf2 * ((float)1. - grat);
	block2_1.b0 = b0cnew / bcoef;
    } else {
	block2_1.b0 = b0_98__(&hour, &sax200, &sux200, &nseasn, &rssn, &longi,
		 &modip);
    }

/* F1 layer height hmF1, critical frequency foF1, peak density NmF1 */

    if (fof1in) {
	fof1 = afof1;
	nmf1 = anmf1;
    } else {
	fof1 = fof1ed_(&absmbr, &rssn, &xhi);
	nmf1 = fof1 * (float)1.24e10 * fof1;
    }
/* F1 layer thickness parameter c1 */
    block2_1.c1 = f1_c1__(&modip, &hour, &sux200, &sax200);
/* F1 occurrence probability with Scotto et al. 1997 */
    f1_prob__(&xhi, &mlat, &rssn, &f1pbw, &f1pbl);
/* F1 occurrence probability Ducharme et al. */
    f1pbo = (float)0.;
    if (fof1in || ! fnight && fof1 > (float)0.) {
	f1pbo = (float)1.;
    }
    if (f1_ocpro__) {
	f1pb = f1pbw;
	if (f1_l_cond__) {
	    f1pb = f1pbl;
	}
	block1_1.f1reg = FALSE_;
	if (fof1in || f1pb >= (float).5) {
	    block1_1.f1reg = TRUE_;
	}
    } else {
	f1pb = f1pbo;
	block1_1.f1reg = FALSE_;
	if (f1pb > (float)0.) {
	    block1_1.f1reg = TRUE_;
	}
    }

/* E-valley: depth, width, height of deepest point (HDEEP), */
/* height of valley top (HEF) */

    xdel = xdels[season - 1] / dela;
    dndhbr = dnds[season - 1] / dela;
    r__1 = (float)10.5 / dela;
    hdeep = hpol_(&hour, &r__1, &c_b129, &sax110, &sux110, &c_b121, &c_b121);
    r__1 = (float)17.8 / dela;
    r__2 = (float)22. / dela + (float)45.;
    width = hpol_(&hour, &r__1, &r__2, &sax110, &sux110, &c_b121, &c_b121);
    depth = hpol_(&hour, &xdel, &c_b134, &sax110, &sux110, &c_b121, &c_b121);
    dlndh = hpol_(&hour, &dndhbr, &c_b137, &sax110, &sux110, &c_b121, &c_b121)
	    ;
    if (depth < (float)1.) {
	goto L600;
    }
    if (block5_1.enight) {
	depth = -depth;
    }
    tal_(&hdeep, &depth, &width, &dlndh, &ext, block5_1.e);
    if (! ext) {
	goto L667;
    }
    if (iounit_1.konsol > 1) {
	io___235.ciunit = iounit_1.konsol;
	s_wsfe(&io___235);
	e_wsfe();
    }
L600:
    width = (float)0.;
L667:
    block4_1.hef = block4_1.hme + width;
    hefold = block4_1.hef;
    vner = ((float)1. - dabs(depth) / (float)100.) * block4_1.nme;

/* Parameters below E  ............................. */

/* L2727: */
    hmex = block4_1.hme - (float)9.;
    block6_1.nmd = xmded_(&xhi, &rssn, &c_b145);
    block6_1.hmd = hpol_(&hour, &c_b134, &c_b147, &sax80, &sux80, &c_b121, &
	    c_b121);
    r__1 = (float).03 / dela + (float).02;
    f[0] = hpol_(&hour, &r__1, &c_b150, &sax80, &sux80, &c_b121, &c_b121);
    f[1] = hpol_(&hour, &c_b153, &c_b154, &sax80, &sux80, &c_b121, &c_b121);
    f[2] = hpol_(&hour, &c_b157, &c_b158, &sax80, &sux80, &c_b121, &c_b121);
    block7_1.fp1 = f[0];
    block7_1.fp2 = -block7_1.fp1 * block7_1.fp1 / (float)2.;
    block7_1.fp30 = (-f[1] * block7_1.fp2 - block7_1.fp1 + (float)1. / f[1]) /
	     (f[1] * f[1]);
    block7_1.fp3u = (-f[2] * block7_1.fp2 - block7_1.fp1 - (float)1. / f[2]) /
	     (f[2] * f[2]);
    block6_1.hdx = block6_1.hmd + f[1];

/* indermediate region between D and E region; parameters xkk */
/* and d1 are found such that the function reaches hdx/xdx/dxdh */

    x = block6_1.hdx - block6_1.hmd;
    xdx = block6_1.nmd * exp(x * (block7_1.fp1 + x * (block7_1.fp2 + x * 
	    block7_1.fp30)));
    dxdx = xdx * (block7_1.fp1 + x * (block7_1.fp2 * (float)2. + x * (float)
	    3. * block7_1.fp30));
    x = block4_1.hme - block6_1.hdx;
    block7_1.xkk = -dxdx * x / (xdx * log(xdx / block4_1.nme));

/* if exponent xkk is larger than xkkmax, then xkk will be set to */
/* xkkmax and d1 will be determined such that the point hdx/xdx is */
/* reached; derivative is no longer continuous. */

    xkkmax = (float)5.;
    if (block7_1.xkk > xkkmax) {
	block7_1.xkk = xkkmax;
	d__1 = (doublereal) x;
	d__2 = (doublereal) block7_1.xkk;
	block7_1.d1 = -log(xdx / block4_1.nme) / pow_dd(&d__1, &d__2);
    } else {
	d__1 = (doublereal) x;
	d__2 = (doublereal) (block7_1.xkk - (float)1.);
	block7_1.d1 = dxdx / (xdx * block7_1.xkk * pow_dd(&d__1, &d__2));
    }

/* compute Danilov et al. (1995) D-region model values */

    if (! dreg) {
	vkp = (float)1.;
	f5sw = (float)0.;
	f6wa = (float)0.;
	dregion_(&xhi, &month, &f107d, &vkp, &f5sw, &f6wa, elg);
	for (ii = 1; ii <= 11; ++ii) {
	    ddens[ii * 5 - 5] = (float)-1.;
	    if (ii < 8) {
		d__1 = (doublereal) (elg[ii - 1] + 6);
		ddens[ii * 5 - 5] = pow_dd(&c_b161, &d__1);
	    }
	}
	f5sw = (float).5;
	f6wa = (float)0.;
	dregion_(&xhi, &month, &f107d, &vkp, &f5sw, &f6wa, elg);
	for (ii = 1; ii <= 11; ++ii) {
	    ddens[ii * 5 - 4] = (float)-1.;
	    if (ii < 8) {
		d__1 = (doublereal) (elg[ii - 1] + 6);
		ddens[ii * 5 - 4] = pow_dd(&c_b161, &d__1);
	    }
	}
	f5sw = (float)1.;
	f6wa = (float)0.;
	dregion_(&xhi, &month, &f107d, &vkp, &f5sw, &f6wa, elg);
	for (ii = 1; ii <= 11; ++ii) {
	    ddens[ii * 5 - 3] = (float)-1.;
	    if (ii < 8) {
		d__1 = (doublereal) (elg[ii - 1] + 6);
		ddens[ii * 5 - 3] = pow_dd(&c_b161, &d__1);
	    }
	}
	f5sw = (float)0.;
	f6wa = (float).5;
	dregion_(&xhi, &month, &f107d, &vkp, &f5sw, &f6wa, elg);
	for (ii = 1; ii <= 11; ++ii) {
	    ddens[ii * 5 - 2] = (float)-1.;
	    if (ii < 8) {
		d__1 = (doublereal) (elg[ii - 1] + 6);
		ddens[ii * 5 - 2] = pow_dd(&c_b161, &d__1);
	    }
	}
	f5sw = (float)0.;
	f6wa = (float)1.;
	dregion_(&xhi, &month, &f107d, &vkp, &f5sw, &f6wa, elg);
	for (ii = 1; ii <= 11; ++ii) {
	    ddens[ii * 5 - 1] = (float)-1.;
	    if (ii < 8) {
		d__1 = (doublereal) (elg[ii - 1] + 6);
		ddens[ii * 5 - 1] = pow_dd(&c_b161, &d__1);
	    }
	}
    }

/* SEARCH FOR HMF1 .................................................. */

    if (layver) {
	goto L6153;
    }
    block1_1.hmf1 = (float)0.;
    if (! block1_1.f1reg) {
	goto L380;
    }
/* omit F1 feature if nmf1*0.9 is smaller than nme */
    bnmf1 = nmf1 * (float).9;
    if (block4_1.nme >= bnmf1) {
	goto L9427;
    }
L9245:
    xe2h = xe2_(&block4_1.hef);
    if (xe2h > bnmf1) {
	block4_1.hef += -1;
	if (block4_1.hef <= block4_1.hme) {
	    goto L9427;
	}
	goto L9245;
    }
    regfa1_(&block4_1.hef, &block1_1.hmf2, &xe2h, &block1_1.nmf2, &c_b170, &
	    nmf1, (E_fp)xe2_, &schalt, &block1_1.hmf1);
    if (! schalt) {
	goto L3801;
    }

/* omit F1 feature .................................................... */

L9427:
    if (iounit_1.konsol > 1) {
	io___253.ciunit = iounit_1.konsol;
	s_wsfe(&io___253);
	e_wsfe();
    }
    block1_1.hmf1 = (float)0.;
    block1_1.f1reg = FALSE_;
/*        NMF1=0. */
/*        C1=0.0 */

/* Determine E-valley parameters if HEF was changed */

L3801:
    if (block4_1.hef != hefold) {
	width = block4_1.hef - block4_1.hme;
	if (block5_1.enight) {
	    depth = -depth;
	}
	tal_(&hdeep, &depth, &width, &dlndh, &ext, block5_1.e);
	if (! ext) {
	    goto L380;
	}
	if (iounit_1.konsol > 1) {
	    io___254.ciunit = iounit_1.konsol;
	    s_wsfe(&io___254);
	    e_wsfe();
	}
	width = (float)0.;
	block4_1.hef = block4_1.hme;
	hefold = block4_1.hef;
	goto L9245;
    }

/* SEARCH FOR HST [NE3(HST)=NME] ...................................... */

L380:
    if (block1_1.f1reg) {
	hf1 = block1_1.hmf1;
	xf1 = nmf1;
    } else {
	hf1 = (block1_1.hmf2 + block4_1.hef) / (float)2.;
	xf1 = xe2_(&hf1);
    }
    hf2 = block4_1.hef;
    xf2 = xe3_1__(&hf2);
    if (xf2 > block4_1.nme) {
	goto L3885;
    }
    regfa1_(&hf1, &hf2, &xf1, &xf2, &c_b170, &block4_1.nme, (E_fp)xe3_1__, &
	    schalt, &block3_1.hst);
    if (schalt) {
	goto L3885;
    }
    block3_1.hz = (block3_1.hst + hf1) / (float)2.;
    d__ = block3_1.hz - block3_1.hst;
    block3_1.t = d__ * d__ / (block3_1.hz - block4_1.hef - d__);
    goto L4933;

/* assume linear interpolation between HZ and HEF .................. */

L3885:
    if (iounit_1.konsol > 1) {
	io___260.ciunit = iounit_1.konsol;
	s_wsfe(&io___260);
	e_wsfe();
    }
    block3_1.hz = (block4_1.hef + hf1) / (float)2.;
    xnehz = xe3_1__(&block3_1.hz);
    if (iounit_1.konsol > 1) {
	io___262.ciunit = iounit_1.konsol;
	s_wsfe(&io___262);
	do_fio(&c__1, (char *)&block3_1.hz, (ftnlen)sizeof(real));
	do_fio(&c__1, (char *)&block4_1.hef, (ftnlen)sizeof(real));
	e_wsfe();
    }
    block3_1.t = (xnehz - block4_1.nme) / (block3_1.hz - block4_1.hef);
    block3_1.hst = (float)-333.;
    goto L4933;

/* LAY-functions for middle ionosphere */

L6153:
    if (hmf1in) {
	hmf1m = ahmf1;
    } else {
	hmf1m = xhi * (float).6428 + (float)165.;
    }
    hhalf = grat * block1_1.hmf2;
    hv1r = block4_1.hme + width;
    hv2r = block4_1.hme + hdeep;
    hhmf2 = block1_1.hmf2;
    inilay_(&fnight, &block1_1.f1reg, &block1_1.nmf2, &nmf1, &block4_1.nme, &
	    vner, &hhmf2, &hmf1m, &block4_1.hme, &hv1r, &hv2r, &hhalf, hxl, 
	    scl, amp, &iiqu);
    if (iiqu == 1 && iounit_1.konsol > 1) {
	io___272.ciunit = iounit_1.konsol;
	s_wsfe(&io___272);
	e_wsfe();
    }
    if (iiqu == 2 && iounit_1.konsol > 1) {
	io___273.ciunit = iounit_1.konsol;
	s_wsfe(&io___273);
	e_wsfe();
    }

/* ---------- CALCULATION OF NEUTRAL TEMPERATURE PARAMETER------- */

L4933:
    hta = (float)120.;
    if (notem) {
	goto L240;
    }
    sec = hourut * (float)3600.;
    cira86_(&daynr, &sec, &lati, &longi, &hour, &cov, &blotn_1.texos, &tn120, 
	    &blotn_1.sigma);
    if (hour != (float)0.) {
	iyz = iyear;
	idz = daynr;
	if (jf[18]) {
	    secni = ((float)24. - longi / 15) * (float)3600.;
	} else {
	    ut_lt__(&c__1, &utni, &c_b188, &longi, &iyz, &idz);
	    secni = utni * (float)3600.;
	}
	cira86_(&daynr, &secni, &lati, &longi, &c_b188, &cov, &texni, &tn1ni, 
		&signi);
    } else {
	texni = blotn_1.texos;
	tn1ni = tn120;
	signi = blotn_1.sigma;
    }
    blotn_1.tlbdh = blotn_1.texos - tn120;
    tlbdn = texni - tn1ni;

/* --------- CALCULATION OF ELECTRON TEMPERATURE PARAMETER-------- */

/* L881: */
/* Te(120km) = Tn(120km) */
    blote_1.ahh[0] = (float)120.;
    ate[0] = tn120;
/* Te-MAXIMUM based on JICAMARCA and ARECIBO data */
/* Computing 2nd power */
    r__1 = mlat / (float)22.41;
    hmaxd = exp(-(r__1 * r__1)) * (float)60. + (float)210.;
    hmaxn = (float)150.;
    blote_1.ahh[1] = hpol_(&hour, &hmaxd, &hmaxn, &sax200, &sux200, &c_b121, &
	    c_b121);
/* Computing 2nd power */
    r__1 = mlat / (float)33.;
    tmaxd = exp(-(r__1 * r__1)) * (float)800. + (float)1500.;
    tmaxn = tn_(&hmaxn, &texni, &tlbdn, &signi) + 20;
    ate[1] = hpol_(&hour, &tmaxd, &tmaxn, &sax200, &sux200, &c_b121, &c_b121);
/* Te(300km), Te(400km) from AE-C, Te(1400km), Te(3000km) from */
/* ISIS, Brace and Theis */
    diplat = magbr;
    teba_(&diplat, &hour, &nseasn, tea);
    icd = 0;
    if (jf[23]) {
/* Te at fixed heights taken from Brace and Theis */
	blote_1.ahh[2] = (float)300.;
	blote_1.ahh[3] = (float)400.;
	blote_1.ahh[4] = (float)600.;
	blote_1.ahh[5] = (float)1400.;
	blote_1.ahh[6] = (float)3e3;
	hte = (float)3e3;
	ate[2] = tea[0];
	ate[3] = tea[1];
	ate[5] = tea[2];
	ate[6] = tea[3];
/* Te(600km) from AEROS, Spenner and Plugge (1979) */
	ett = exp(-mlat / (float)11.35);
	d__1 = (doublereal) (ett + 1);
	tet = (float)2900. - ett * (float)5600. / pow_dd(&d__1, &c_b195);
	ten = (float)1161. / (exp(-(absmlt - (float)45.) / (float)5.) + (
		float)1.) + (float)839.;
	ate[4] = hpol_(&hour, &tet, &ten, &sax300, &sux300, &c_b196, &c_b196);
    } else {
/* Te at fixed heights from IK, Truhlik, Triskova, Smilauer */
	blote_1.ahh[2] = (float)300.;
	blote_1.ahh[3] = (float)550.;
	blote_1.ahh[4] = (float)900.;
	blote_1.ahh[5] = (float)1500.;
	blote_1.ahh[6] = (float)2500.;
	hte = (float)2500.;
	dimo = (float).311653;
/*              icd=1    ! compute INVDIP */
/*              isa=0    ! solar activity correction off */
/*              ise=0    ! season correction off */
	for (ijk = 4; ijk <= 7; ++ijk) {
	    igrf_sub__(&lati, &longi, &ryear, &blote_1.ahh[ijk - 1], &xl, &
		    icode, &dipl, &babs, ier);
	    if (*ier != 0) {
		return 0;
	    }
	    if (xl > (float)10.) {
		xl = (float)10.;
	    }
	    elteik_(&c__1, &c__0, &c__0, &invdip, &xl, &dimo, &babs, &dipl, &
		    hour, &blote_1.ahh[ijk - 1], &daynr, &f107d, &teh2, &sdte)
		    ;
/* L3491: */
	    ate[ijk - 1] = teh2;
	}
	ate[2] = tea[0];
    }
/* Option to use Te = f(Ne) relation at ahh(3), ahh(4) */
    if (teneop) {
	for (i__ = 1; i__ <= 2; ++i__) {
/* L3395: */
	    if (tecon[i__ - 1]) {
		r__1 = -cov;
		ate[i__ + 1] = tede_(&blote_1.ahh[i__ + 1], &xnar[i__ - 1], &
			r__1);
	    }
	}
    }
/* Te corrected and Te > Tn enforced */
    tnahh2 = tn_(&blote_1.ahh[1], &blotn_1.texos, &blotn_1.tlbdh, &
	    blotn_1.sigma);
    if (ate[1] < tnahh2) {
	ate[1] = tnahh2;
    }
    stte1 = (ate[1] - ate[0]) / (blote_1.ahh[1] - blote_1.ahh[0]);
    for (i__ = 2; i__ <= 6; ++i__) {
	tnahhi = tn_(&blote_1.ahh[i__], &blotn_1.texos, &blotn_1.tlbdh, &
		blotn_1.sigma);
	if (ate[i__] < tnahhi) {
	    ate[i__] = tnahhi;
	}
	stte2 = (ate[i__] - ate[i__ - 1]) / (blote_1.ahh[i__] - blote_1.ahh[
		i__ - 1]);
	ate[i__ - 1] -= (stte2 - stte1) * blote_1.dte[i__ - 2] * alog2;
/* L1901: */
	stte1 = stte2;
    }
/* Te gradients STTE are computed for each segment */
    for (i__ = 1; i__ <= 6; ++i__) {
/* L1902: */
	blote_1.stte[i__ - 1] = (ate[i__] - ate[i__ - 1]) / (blote_1.ahh[i__] 
		- blote_1.ahh[i__ - 1]);
    }
    blote_1.ate1 = ate[0];
/* L887: */

/* ------------ CALCULATION OF ION TEMPERATURE PARAMETERS-------- */

/* Ti(430km) during daytime from AEROS data */
    blotn_1.xsm1 = (float)430.;
    block8_1.xsm[0] = blotn_1.xsm1;
    z1 = exp(mlat * (float)-.09);
    z2 = z1 + (float)1.;
    tid1 = (float)1240. - z1 * (float)1400. / (z2 * z2);
    block8_1.mm[1] = hpol_(&hour, &c_b206, &c_b188, &sax300, &sux300, &c_b121,
	     &c_b121);
/* Ti(430km) duirng nighttime from AEROS data */
    z1 = absmlt;
    z2 = z1 * (z1 * (float).024 + (float).47) * const_1.umr;
    z3 = cos(z2);
    tin1 = (float)1200. - r_sign(&c_b121, &z3) * (float)300. * sqrt((dabs(z3))
	    );
/* Ti(430km) for specified time using HPOL */
    ti1 = tin1;
    if (tid1 > tin1) {
	ti1 = hpol_(&hour, &tid1, &tin1, &sax300, &sux300, &c_b121, &c_b121);
    }
/* Tn < Ti < Te enforced */
    ten1 = elte_(&blotn_1.xsm1);
    tnn1 = tn_(&blotn_1.xsm1, &texni, &tlbdn, &signi);
    if (ten1 < tnn1) {
	ten1 = tnn1;
    }
    if (ti1 > ten1) {
	ti1 = ten1;
    }
    if (ti1 < tnn1) {
	ti1 = tnn1;
    }
/* Tangent on Tn profile determines HS */
    ti13 = teder_(&c_b213);
    ti50 = teder_(&c_b214);
    regfa1_(&c_b213, &c_b214, &ti13, &ti50, &c_b217, &ti1, (E_fp)teder_, &
	    schalt, &block8_1.hs);
    if (schalt) {
	block8_1.hs = (float)200.;
    }
    block8_1.tnhs = tn_(&block8_1.hs, &blotn_1.texos, &blotn_1.tlbdh, &
	    blotn_1.sigma);
    block8_1.mm[0] = dtndh_(&block8_1.hs, &blotn_1.texos, &blotn_1.tlbdh, &
	    blotn_1.sigma);
    if (schalt) {
	block8_1.mm[0] = (ti1 - block8_1.tnhs) / (blotn_1.xsm1 - block8_1.hs);
    }
    block8_1.mxsm = 2;
/* XTETI is altitude where Te=Ti */
/* L2391: */
    xtts = (float)500.;
    x = (float)500.;
L2390:
    x += xtts;
    if (x >= blote_1.ahh[6]) {
	goto L240;
    }
    tex = elte_(&x);
    tix = ti_(&x);
    if (tix < tex) {
	goto L2390;
    }
    x -= xtts;
    xtts /= (float)10.;
    if (xtts > (float).1) {
	goto L2390;
    }
    xteti = x + xtts * (float)5.;
/* Ti=Te above XTETI */
    block8_1.mxsm = 3;
    block8_1.mm[2] = blote_1.stte[5];
    block8_1.xsm[1] = xteti;
    if (xteti > blote_1.ahh[5]) {
	goto L240;
    }
    block8_1.mxsm = 4;
    block8_1.mm[2] = blote_1.stte[4];
    block8_1.mm[3] = blote_1.stte[5];
    block8_1.xsm[2] = blote_1.ahh[5];
    if (xteti > blote_1.ahh[4]) {
	goto L240;
    }
    block8_1.mxsm = 5;
    block8_1.dti[0] = (float)5.;
    block8_1.dti[1] = (float)5.;
    block8_1.mm[2] = blote_1.stte[3];
    block8_1.mm[3] = blote_1.stte[4];
    block8_1.mm[4] = blote_1.stte[5];
    block8_1.xsm[2] = blote_1.ahh[4];
    block8_1.xsm[3] = blote_1.ahh[5];

/* CALCULATION OF ION DENSITY PARAMETER.................. */

L240:
    if (noion) {
	goto L141;
    }
    hnia = (float)100.;
    if (dy) {
	hnia = (float)75.;
    }
    hnie = (float)2e3;
    if (dy) {
	goto L141;
    }

/* INPUT OF THE ION DENSITY PARAMETER ARRAYS PF1O,PF2O AND PF3O...... */

    rif[0] = (float)2.;
    if (abslat < (float)30.) {
	rif[0] = (float)1.;
    }
    rif[1] = (float)2.;
    if (cov < (float)100.) {
	rif[1] = (float)1.;
    }
    rif[2] = (real) season;
    if (season == 1) {
	rif[2] = (float)3.;
    }
    rif[3] = (float)1.;
    if (fnight) {
	rif[3] = (float)2.;
    }
    koefp1_(pg1o);
    koefp2_(pg2o);
    koefp3_(pg3o);
    sufe_(pg1o, rif, &c__12, pf1o);
    sufe_(pg2o, rif, &c__4, pf2o);
    sufe_(pg3o, rif, &c__12, pf3o);

/* calculate O+ profile parameters */

    if (fnight) {
	zzz1 = (float)0.;
    } else {
	zzz1 = cos(xhi * const_1.umr);
    }
    msumo = 4;
    rdomax = (float)100.;
    mo[0] = epstep_(pf1o, &pf1o[1], &pf1o[2], &pf1o[3], &zzz1);
    mo[1] = epstep_(&pf1o[4], &pf1o[5], &pf1o[6], &pf1o[7], &zzz1);
    mo[2] = (float)0.;
    ho[0] = epstep_(&pf1o[8], &pf1o[9], &pf1o[10], &pf1o[11], &zzz1);
    ho[1] = (float)290.;
    if (rif[1] == (float)2. && rif[2] == (float)2.) {
	ho[1] = (float)237.;
    }
    ho[3] = pf2o[0];
    ho05 = pf2o[3];
    mo[3] = pf2o[1];
    mo[4] = pf2o[2];

/* adjust gradient MO(4) of O+ profile segment above F peak */

L7100:
    ho[2] = (alg100 - mo[4] * (ho[3] - ho05)) / mo[3] + ho[3];
    if (ho[2] <= ho[1] + (float)20.) {
	mo[3] += (float)-.001;
	goto L7100;
    }
    hfixo = (ho[1] + ho[2]) / (float)2.;

/* find height H0O of maximum O+ relative density */

    delx = (float)5.;
    x = ho[1];
    ymaxx = (float)0.;
L7102:
    x += delx;
    y = rpid_(&x, &hfixo, &rdomax, &msumo, mo, ddo, ho);
    if (y <= ymaxx) {
	if (delx <= (float).1) {
	    goto L7104;
	}
	x -= delx;
	delx /= (float)5.;
    } else {
	ymaxx = y;
    }
    goto L7102;
L7104:
    h0o = x - delx / (float)2.;
L7101:
    if (y < (float)100.) {
	goto L7103;
    }
    rdomax += (float)-.01;
    y = rpid_(&h0o, &hfixo, &rdomax, &msumo, mo, ddo, ho);
    goto L7101;
L7103:
    yo2h0o = (float)100. - y;
    yoh0o = y;

/* calculate parameters for O2+ profile */

    hfixo2 = pf3o[0];
    rdo2mx = pf3o[1];
    for (l = 1; l <= 2; ++l) {
	i__ = l << 1;
	ho2[l - 1] = pf3o[i__] + pf3o[i__ + 1] * zzz1;
/* L7105: */
	mo2[l] = pf3o[i__ + 6] + pf3o[i__ + 7] * zzz1;
    }
    mo2[0] = pf3o[6] + pf3o[7] * zzz1;
    if (hfixo2 > ho2[0]) {
	ymo2z = mo2[1];
    } else {
	ymo2z = mo2[0];
    }
    aldo21 = log(rdo2mx) + ymo2z * (ho2[0] - hfixo2);
    hfixo2 = (ho2[1] + ho2[0]) / (float)2.;
    rdo2mx = exp(aldo21 + mo2[1] * (hfixo2 - ho2[0]));

/* make sure that rd(O2+) is less or equal 100-rd(O+) at O+ maximum */

L7106:
    y = rpid_(&h0o, &hfixo2, &rdo2mx, &c__2, mo2, do2, ho2);
    if (y > yo2h0o) {
	mo2[2] += (float)-.02;
	goto L7106;
    }

/* use ratio of NO+ to O2+ density at O+ maximum to calculate */
/* NO+ density above the O+ maximum (H0O) */

    if (y < (float)1.) {
	nobo2 = (float)0.;
    } else {
	nobo2 = (yo2h0o - y) / y;
    }

/* CALCULATION FOR THE REQUIRED HEIGHT RANGE....................... */
/* In the absence of an F1 layer hmf1=hz since hmf1 is used in XE */

L141:
    xhmf1 = block1_1.hmf1;
    if (block1_1.hmf1 <= (float)0.) {
	block1_1.hmf1 = block3_1.hz;
    }
    height = *heibeg;
    kk = 1;
L300:
    if (noden) {
	goto L330;
    }
    if (height > hnee || height < hnea) {
	goto L330;
    }
    if (layver) {
	elede = (float)-9.;
	if (iiqu < 2) {
	    elede = xen_(&height, &block1_1.hmf2, &block1_1.nmf2, &
		    block4_1.hme, &c__4, hxl, scl, amp);
	}
	outf[kk * 20 + 1] = elede;
	goto L330;
    }
    elede = xe_1__(&height);

/* TTS model option for topside */

    if (blo11_1.itopn == 3 && height >= (float)400.) {
	igrf_sub__(&lati, &longi, &ryear, &height, &xl1, &icode, &dipl1, &
		babs1, ier);
	if (*ier != 0) {
	    return 0;
	}
	if (xl1 > (float)10.) {
	    xl1 = (float)10.;
	}
/*              icd=1    ! compute INVDIP */
	dimo = (float).311653;
	calne_(&c__1, &invdip, &xl1, &dimo, &babs1, &dipl, &hour, &height, &
		daynr, &f107d, &elede);
    }

/* electron density in m-3 in outf(1,*) */

    outf[kk * 20 + 1] = elede;

/* plasma temperatures */

L330:
    if (notem) {
	goto L7108;
    }
    if (height > hte || height < hta) {
	goto L7108;
    }
    tnh = tn_(&height, &blotn_1.texos, &blotn_1.tlbdh, &blotn_1.sigma);
    tih = tnh;
    if (height >= block8_1.hs) {
	tih = ti_(&height);
    }
    teh = elte_(&height);
    if (tih < tnh) {
	tih = tnh;
    }
    if (teh < tih) {
	teh = tih;
    }
    outf[kk * 20 + 2] = tnh;
    outf[kk * 20 + 3] = tih;
    outf[kk * 20 + 4] = teh;

/* ion composition */

L7108:
    if (noion) {
	goto L7118;
    }
    if (height > hnie || height < hnia) {
	goto L7118;
    }
    if (dy) {
	ioncomp_(&ryear, &daynr, &iseamon, &hour, &height, &xhi, &lati, &
		longi, &cov, dion);
	rox = dion[0];
	rhx = dion[1];
	rnx = dion[2];
	rhex = dion[3];
	rnox = dion[4];
	ro2x = dion[5];
	rclust = dion[6];
    } else {
	rox = rpid_(&height, &hfixo, &rdomax, &msumo, mo, ddo, ho);
	ro2x = rpid_(&height, &hfixo2, &rdo2mx, &c__2, mo2, do2, ho2);
	rdhhe_(&height, &h0o, &rox, &ro2x, &nobo2, &c_b239, &rhx, &rhex);
	rnox = rdno_(&height, &h0o, &ro2x, &rox, &nobo2);
	rnx = (float)-1.;
	rclust = (float)-1.;
    }

/* ion densities are given in percent of total electron density; */

    if (jf[22]) {
	xnorm = (float)1.;
    } else {
	xnorm = elede / (float)100.;
    }
    outf[kk * 20 + 5] = rox * xnorm;
    outf[kk * 20 + 6] = rhx * xnorm;
    outf[kk * 20 + 7] = rhex * xnorm;
    outf[kk * 20 + 8] = ro2x * xnorm;
    outf[kk * 20 + 9] = rnox * xnorm;
    outf[kk * 20 + 10] = rclust * xnorm;
    outf[kk * 20 + 11] = rnx * xnorm;

/* D region special: Friedrich&Torkar model in outf(13,*) */

L7118:
    if (! dreg && height <= (float)140.) {
	outf[kk * 20 + 1] = (float)-1.;
	f00_(&height, &lati, &daynr, &xhi, &f107d, &edens, &ierror);
	if (ierror == 0 || ierror == 2) {
	    outf[kk * 20 + 1] = edens;
	}
    }
    height += *heistp;
    ++kk;
    if (kk <= numhei) {
	goto L300;
    }

/* D region special: densities for 11 heights (60,65,70,..,110km) */
/* outf(14,1:11)=IRI-07, outf(14,12:22)=FIRI, */
/* outf(14,23:33)= Danilov et al.(1995) with SW=0,WA=0 */
/* outf(14,34:44)= with SW=0.5,WA=0, */
/* outf(14,45:55)= with SW=1,WA=0, */
/* outf(14,56:66)= with SW=0,WA=0.5, */
/* outf(14,67:77)= with SW=0,WA=1, */

    if (! dreg) {
	for (ii = 1; ii <= 11; ++ii) {
	    htemp = (real) (ii * 5 + 55);
	    outf[ii * 20 + 14] = (float)-1.;
	    if (htemp >= (float)65.) {
		outf[ii * 20 + 14] = xe6_(&htemp);
	    }
	    outf[(ii + 11) * 20 + 14] = (float)-1.;
	    f00_(&htemp, &lati, &daynr, &xhi, &f107d, &edens, &ierror);
	    if (ierror == 0 || ierror == 2) {
		outf[(ii + 11) * 20 + 14] = edens;
	    }
	    outf[(ii + 22) * 20 + 14] = ddens[ii * 5 - 5];
	    outf[(ii + 33) * 20 + 14] = ddens[ii * 5 - 4];
	    outf[(ii + 44) * 20 + 14] = ddens[ii * 5 - 3];
	    outf[(ii + 55) * 20 + 14] = ddens[ii * 5 - 2];
	    outf[(ii + 66) * 20 + 14] = ddens[ii * 5 - 1];
	}
    }

/* equatorial vertical ion drift */

    drift = (float)-1.;
    if (jf[21] && dabs(magbr) < (float)25.) {
	param[0] = (real) daynr;
	param[1] = f107d;
	vdrift_(&hour, &longi, param, &drift);
    }

/* spread-F occurrence probability */

    spreadf = (float)-1.;
    if (! jf[28]) {
	goto L1937;
    }
    if (hour > (float)7.25 && hour < (float)17.75) {
	goto L1937;
    }
    if (dabs(lati) > (float)25.) {
	goto L1937;
    }
    spfhour = hour;
    daynr1 = (real) daynr;
    if (hour < (float)12.) {
	spfhour = hour + (float)24.;
	daynr1 = (real) (daynr - 1);
	if (daynr1 < (float)1.) {
	    daynr1 = (real) idayy;
	}
    }
    spreadf_brazil__(&daynr, &idayy, &f107d, &lati, osfbr);
    ispf = (integer) ((spfhour - (float)17.75) / (float).5) + 1;
    if (ispf > 0 && ispf < 26) {
	spreadf = osfbr[ispf - 1];
    }
L1937:

/* ADDITIONAL PARAMETER FIELD OARR */

    if (noden) {
	goto L6192;
    }
    oarr[1] = block1_1.nmf2;
    oarr[2] = block1_1.hmf2;
    if (block1_1.f1reg) {
	oarr[3] = nmf1;
    }
    if (block1_1.f1reg) {
	oarr[4] = xhmf1;
    }
    oarr[5] = block4_1.nme;
    oarr[6] = block4_1.hme;
    oarr[7] = block6_1.nmd;
    oarr[8] = block6_1.hmd;
    oarr[9] = hhalf;
    oarr[10] = block2_1.b0;
    oarr[11] = vner;
    oarr[12] = block4_1.hef;
L6192:
    if (notem) {
	goto L6092;
    }
    oarr[13] = ate[1];
    oarr[14] = blote_1.ahh[1];
    oarr[15] = ate[2];
    oarr[16] = ate[3];
    oarr[17] = ate[4];
    oarr[18] = ate[5];
    oarr[19] = ate[6];
    oarr[20] = ate[0];
    oarr[21] = ti1;
    oarr[22] = xteti;
L6092:
    oarr[23] = xhi;
    oarr[24] = sundec;
    oarr[25] = dip;
    oarr[26] = magbr;
    oarr[27] = modip;
    oarr[28] = dela;
    oarr[29] = sax200;
    oarr[30] = sux200;
    oarr[31] = (real) season;
    oarr[32] = (real) nseasn;
    oarr[33] = rssn;
    oarr[34] = cov;
    oarr[35] = block2_1.b1;
    oarr[36] = xm3000;
    oarr[39] = gind;
    oarr[40] = f1pbo;
    oarr[41] = f107d;
    oarr[42] = block2_1.c1;
    oarr[43] = (real) daynr;
    oarr[44] = drift;
    oarr[45] = stormcorr;
    oarr[46] = f1pbw;
    oarr[47] = f1pbl;
    oarr[48] = spreadf;
L3330:
    return 0;
} /* iri_sub__ */



/* Subroutine */ int iri_web__(integer *jmag, logical *jf, real *alati, real *
	along, integer *iyyyy, integer *mmdd, integer *iut, real *dhour, real 
	*height, real *h_tec_max__, integer *ivar, real *vbeg, real *vend, 
	real *vstp, real *a, real *b, integer *ier)
{
    /* System generated locals */
    integer i__1;
    real r__1;

    /* Local variables */
    static integer i__, ii, iii;
    static real tec, oar[50], tecb, tect, oarr[50], outf[10000]	/* was [20][
	    500] */, xvar[8], xhour;
    static integer numstp;
    extern /* Subroutine */ int iri_tec__(real *, real *, integer *, real *, 
	    real *, real *), iri_sub__(logical *, integer *, real *, real *, 
	    integer *, integer *, real *, real *, real *, real *, real *, 
	    real *, integer *);

/* ----------------------------------------------------------------------- */
/* changes: */
/*       11/16/99 jf(30) instead of jf(17) */
/*       10/31/08 outf, a, b (100 -> 500) */

/* ----------------------------------------------------------------------- */
/* input:   jmag,alati,along,iyyyy,mmdd,dhour  see IRI_SUB */
/*          height  height in km */
/*          h_tec_max  =0 no TEC otherwise upper boundary for integral */
/*          iut     =1 for UT       =0 for LT */
/*          ivar    =1      altitude */
/*                  =2,3    latitude,longitude */
/*                  =4,5,6  year,month,day */
/*                  =7      day of year */
/*                  =8      hour (UT or LT) */
/*          vbeg,vend,vstp  variable range (begin,end,step) */
/* output:  a       similar to outf in IRI_SUB */
/*          b       similar to oarr in IRI_SUB */

/*          numstp  number of steps; maximal 500 */
/*          IER     error: =0 if no error; !=0 if having an error */
/* ----------------------------------------------------------------------- */
    /* Parameter adjustments */
    b -= 51;
    a -= 21;
    --jf;

    /* Function Body */

    numstp = (integer) ((*vend - *vbeg) / *vstp) + 1;
    if (numstp > 500) {
	numstp = 500;
    }
    for (i__ = 1; i__ <= 50; ++i__) {
/* L6249: */
	oar[i__ - 1] = b[i__ + 50];
    }
    if (*ivar == 1) {
	for (i__ = 1; i__ <= 50; ++i__) {
/* L1249: */
	    oarr[i__ - 1] = oar[i__ - 1];
	}
	xhour = *dhour + *iut * (float)25.;
	iri_sub__(&jf[1], jmag, alati, along, iyyyy, mmdd, &xhour, vbeg, vend,
		 vstp, &a[21], oarr, ier);
	if (*ier != 0) {
	    return 0;
	}
	if (*h_tec_max__ > (float)50.) {
	    iri_tec__(&c_b246, h_tec_max__, &c__2, &tec, &tect, &tecb);
	    oarr[36] = tec;
	    oarr[37] = tect;
	}
	for (i__ = 1; i__ <= 50; ++i__) {
/* L1111: */
	    b[i__ + 50] = oarr[i__ - 1];
	}
	return 0;
    }
    if (*height <= (float)0.) {
	*height = (float)100.;
    }
    xvar[1] = *alati;
    xvar[2] = *along;
    xvar[3] = (real) (*iyyyy);
    xvar[4] = (real) (*mmdd / 100);
    xvar[5] = *mmdd - xvar[4] * 100;
    xvar[6] = (r__1 = *mmdd * (float)1., dabs(r__1));
    xvar[7] = *dhour;
    xvar[*ivar - 1] = *vbeg;
    *alati = xvar[1];
    *along = xvar[2];
    *iyyyy = (integer) xvar[3];
    if (*ivar == 7) {
	*mmdd = -((integer) (*vbeg));
    } else {
	*mmdd = (integer) (xvar[4] * 100 + xvar[5]);
    }
    *dhour = xvar[7] + *iut * (float)25.;
    i__1 = numstp;
    for (i__ = 1; i__ <= i__1; ++i__) {
	for (iii = 1; iii <= 50; ++iii) {
/* L1349: */
	    oarr[iii - 1] = b[iii + i__ * 50];
	}
	iri_sub__(&jf[1], jmag, alati, along, iyyyy, mmdd, dhour, height, 
		height, &c_b121, outf, oarr, ier);
	if (*ier != 0) {
	    return 0;
	}
	if (*h_tec_max__ > (float)50.) {
	    iri_tec__(&c_b246, h_tec_max__, &c__2, &tec, &tect, &tecb);
	    oarr[36] = tec;
	    oarr[37] = tect;
	}
	for (ii = 1; ii <= 20; ++ii) {
/* L2: */
	    a[ii + i__ * 20] = outf[ii - 1];
	}
	for (ii = 1; ii <= 50; ++ii) {
/* L2222: */
	    b[ii + i__ * 50] = oarr[ii - 1];
	}
	xvar[*ivar - 1] += *vstp;
	*alati = xvar[1];
	*along = xvar[2];
	*iyyyy = (integer) xvar[3];
	if (*ivar == 7) {
	    *mmdd = -xvar[6];
	} else {
	    *mmdd = (integer) (xvar[4] * 100 + xvar[5]);
	}
	*dhour = xvar[7] + *iut * (float)25.;
/* L1: */
    }
    return 0;
} /* iri_web__ */

#ifdef __cplusplus
	}
#endif
