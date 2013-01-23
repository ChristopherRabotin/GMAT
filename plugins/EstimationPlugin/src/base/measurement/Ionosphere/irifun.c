/* irifun.f -- translated by f2c (version 20000704).
   You must link the resulting object file with the libraries:
	-lf2c -lm   (in that order)
*/

#ifdef __cplusplus
extern "C" {
#endif
#include "f2c.h"

/* Common Block Declarations */

union {
    struct {
	real umr;
    } _1;
    struct {
	real faktor;
    } _2;
    struct {
	real dtr;
    } _3;
} const_;

#define const_1 (const_._1)
#define const_2 (const_._2)
#define const_3 (const_._3)

struct {
    real argmax;
} argexp_;

#define argexp_1 argexp_

struct {
    real humr, dumr;
} const1_;

#define const1_1 const1_

union {
    struct {
	real hmf2, xnmf2, hmf1;
	logical f1reg;
    } _1;
    struct {
	real hmf2, xnmf2, xhmf1;
	logical f1reg;
    } _2;
} block1_;

#define block1_1 (block1_._1)
#define block1_2 (block1_._2)

union {
    struct {
	real beta, eta, delta, zeta;
    } _1;
    struct {
	real beta, eta, del, zeta;
    } _2;
} blo10_;

#define blo10_1 (blo10_._1)
#define blo10_2 (blo10_._2)

struct {
    real b2top, tc3;
    integer itopn;
    real alg10, hcor1;
} blo11_;

#define blo11_1 blo11_

union {
    struct {
	real b0, b1, c1;
    } _1;
    struct {
	real b0, b1, d1f1;
    } _2;
} block2_;

#define block2_1 (block2_._1)
#define block2_2 (block2_._2)

struct {
    real hz, t, hst;
} block3_;

#define block3_1 block3_

struct {
    real hme, xnme, hef;
} block4_;

#define block4_1 block4_

struct {
    logical night;
    real e[4];
} block5_;

#define block5_1 block5_

struct {
    real hmd, xnmd, hdx;
} block6_;

#define block6_1 block6_

struct {
    real d1, xkk, fp30, fp3u, fp1, fp2;
} block7_;

#define block7_1 block7_

struct {
    real ah[7], ate1, st[6], d__[5];
} blote_;

#define blote_1 blote_

struct {
    real hs, tnhs, xsm[4], mm[5], g[4];
    integer m;
} block8_;

#define block8_1 block8_

struct {
    real xsm1, tex, tlbd, sig;
} blotn_;

#define blotn_1 blotn_

struct {
    integer konsol;
} iounit_;

#define iounit_1 iounit_

struct {
    integer kf, n;
} mflux_;

#define mflux_1 mflux_

/* Table of constant values */

static real c_b3 = (float)394.5;
static real c_b5 = (float)100.;
static real c_b6 = (float)300.;
static real c_b18 = (float)1.;
static doublereal c_b30 = 10.;
static integer c__6 = 6;
static doublereal c_b55 = .33333333333333331;
static integer c__8 = 8;
static integer c__4 = 4;
static integer c__1 = 1;
static integer c__0 = 0;
static integer c__2 = 2;
static integer c__3 = 3;
static integer c__9 = 9;
static integer c__76 = 76;
static integer c__13 = 13;
static integer c__988 = 988;
static integer c__7 = 7;
static integer c__49 = 49;
static integer c__441 = 441;
static doublereal c_b207 = 2.36;
static doublereal c_b209 = .25;
static doublereal c_b212 = 2.7;
static real c_b306 = (float)2.;
static real c_b310 = (float).1;
static real c_b311 = (float).15;

/* irifun.for, version number can be found at the end of this comment. */
/* ----------------------------------------------------------------------- */

/* Functions and subroutines for the International Reference */
/* Ionosphere model. These functions and subroutines are called by */
/* IRI_SUB subroutine (IRISUB.FOR). */

/* ----------------------------------------------------------------------- */

/* i/o units:  6   messages (during execution) to Konsol (Monitor) */
/*            10   CCIR and URSI coefficients */
/*            11   alternate unit for message file (if jf(12)=.false.) */
/*            12   solar/ionospheric indices: ig_rz.dat */
/*            13   magnetic indices: ap.dat */
/*            14   igrf coefficients */

/* ----------------------------------------------------------------------- */
/* changes from IRIFU9 to IRIF10: */
/*       SOCO for solar zenith angle */
/*       ACOS and ASIN argument forced to be within -1 / +1 */
/*       EPSTEIN functions corrected for large arguments */
/* ----------------------------------------------------------------------- */
/* changes from IRIF10 to IRIF11: */
/*       LAY subroutines introduced */
/*       TEBA corrected for 1400 km */
/* ----------------------------------------------------------------------- */
/* changes from IRIF11 to IRIF12: */
/*       Neutral temperature subroutines now in CIRA86.FOR */
/*       TEDER changed */
/*       All names with 6 or more characters replaced */
/*       10/29/91 XEN: 10^ in loop, instead of at the end */
/*       1/21/93 B0_TAB instead of B0POL */
/*       9/22/94 Alleviate underflow condition in IONCOM exp() */
/* ----------------------------------------------------------------------- */
/* changes from IRIF12 to IRIF13: */
/*        9/18/95 MODA: add leap year and number of days in month */
/*        9/29/95 replace F2out with FOUT and XMOUT. */
/*       10/ 5/95 add TN and DTNDH; earlier in CIRA86.FOR */
/*       10/ 6/95 add TCON for reading indices */
/*       10/20/95 MODA: IN=1 MONTH=IMO */
/*       10/20/95 TCON: now includes RZ interpolation */
/*       11/05/95 IONCOM->IONCO1, added IONCOM_new, IONCO2 */
/*       11/05/95 LSTID added for strom-time updating */
/*       11/06/95 ROGUL: transition 20. instead of 15. */
/*       12/01/95 add UT_LT for (date-)correct UT<->LT conversion */
/*       01/16/96 TCON: add IMST to SAVE statement */
/*       02/02/96 ROGUL: 15. reinstated */
/*       02/07/96 UT_LT: ddd, dddend integer, no leap year 2000 */
/*       03/15/96 ZERO: finding delta for topside */
/*       03/18/96 UT_LT: mode=1, change of year */
/*       12/09/96 since 2000 is leap, delete y/100*100 condition */
/*       04/25/97 XMDED: minimal value also daytime */
/*       05/18/98 TCON: changes to IG_RZ (update date); -R = Cov */
/*       05/19/98 Replaced IONCO2&APROK; HEI,XHI in IONCOM_NEW */
/*       10/01/98 added INITIALIZE */
/*       04/30/99 MODA: reset bb(2)=28 */
/*       11/08/99 avoid negative x value in function XE2. Set x=0. */
/*       11/09/99 added COMMON/const1/humr,dumr also for CIRA86 */
/*       11/30/99 EXIT in APROK replaced with GOTO (N. Smirnova) */
/* ----------------------------------------------------------------------- */
/* changes from IRIF13 to IRIFUN: */
/* -Version-mm/dd/yy-description [person reporting correction] */
/* 2000.01 05/09/00 Include B0_98 subroutine to replace B0_NEW and B0POL */
/* 2000.02 05/18/00 Include Elteik and spharm_ik for Te */
/* 2000.03 06/09/00 Include xe3_1, xe4_1, xe_1 */
/* 2000.04 06/11/00 Include f1_c1, f1_prob, modified fof1ed */
/* 2000.05 10/30/00 Include vdrift */
/* 2000.06 04/15/01 Include IGRF_SUB subroutine for IK Te model */
/* 2000.07 05/07/01 Include storm subroutine STORM and Ap access s/w */
/* 2000.08 09/07/01 APF: if(j1.eq.j2) -> if(IY.eq.j2) [P. Wilkinson] */
/* 2000.09 09/07/01 CONVER: LO2 = MOD(LO1,20)+1 [P. Webb,D. Pesnell] */
/* 2000.10 02/20/02 CONVER/DATA: 105.78 -> 015.78 [A. Shovkoplyas] */
/* 2000.11 10/28/02 replace TAB/6 blanks, enforce 72/line [D. Simpson] */
/* 2000.12 11/08/02 removing unused variables (corr); apf0 removed */
/* 2000.13 11/26/02 apf() using keyed access to ap.dat file; apf->apf1 */
/* 2000.14 11/27/02 changed F1_PROB; always 6 preceeding spaces */
/* 2005.01 03/09/05 CALION,INVDPC,CALNE for new Ne, Ni models */
/* 2005.01 11/14/05 APF_ONLY for F107D; */
/* 2005.01 11/14/05 spreadf_brazil; added constraint 0<=P<=1 */
/* 2005.02 05/11/06 NeQuick: XE1,TOPQ, M3000HM; stormvd, */
/* 2005.02 03/27/07 STORM: hourly interpolation of Ap  [A. Oinats] */
/* 2007.00 05/18/07 Release of IRI-2007 */
/* 2007.01 09/19/07 vdrift et al.: without *8 (no change in results) */
/* 2007.01 02/07/09 IONLOW: N+ correction [V. Truhlik] */
/* 2007.02 03/30/09 NMDED: avoid exp underflow [K. Choi] */
/* 2007.02 03/30/09 spreadf_brazil: bspl2f et al b(20->30) [Tab Ji] */
/* 2007.02 03/30/09 APF_ONLY: Compute monthly F10.7 */
/* 2007.03 05/26/09 APF_ONLY: replace i with 1 and IMN with ID [R.Conde] */
/* 2007.04 07/10/09 CONVER/DATA: 015.78 -> 005.78 [E. Araujo] */
/* 2007.05 07/23/09 STORM/CONVER: long. discont. [R. Conde, E. Araujo] */
/* 2007.05 07/23/09 APF,APF_ONLY: use YearBegin from ap.dat [R. Conde] */
/* 2007.06 02/03/10 APF: text if eof; clean-up APF and APF_only */

/* ************************************************************** */
/* ********** INTERNATIONAL REFERENCE IONOSPHERE **************** */
/* ************************************************************** */
/* ****************  FUNCTIONS,SUBROUTINES  ********************* */
/* ************************************************************** */
/* ** initialize:	INITIALIZE */
/* ** NE:         XE1,TOPQ,ZERO,DXE1N,XE2,XE3_1,XE4_1,XE5,XE6, */
/* **             XE_1,CALNE */
/* ** TE/TI:      ELTEIK,SPHARM_IK,TEBA,SPHARM,ELTE,TEDE,TI,TEDER, */
/* **		        TN,DTNDH */
/* ** NI:         RPID,RDHHE,RDNO,KOEFP1,KOEFP2,KOEFP3,SUFE,IONCO2, */
/* **             APROK,IONCOMP,IONCO1,CALION,IONLOW,IONHIGH,INVDPC */
/* ** PEAKS:      FOUT,XMOUT,HMF2ED,FOF1ED,f1_c1,f1_prob,FOEEDI,XMDED, */
/* **		        GAMMA1 */
/* ** PROFILE PAR:B0_98,TAL,VALGUL */
/* ** MAG. FIELD: GGM,FIELDG,CONVER(Geom. Corrected Latitude) */
/* ** FUNCTIONS:  REGFA1 */
/* ** TIME:       SOCO,HPOL,MODA,UT_LT */
/* ** EPSTEIN:    RLAY,D1LAY,D2LAY,EPTR,EPST,EPSTEP,EPLA */
/* ** LAY:        XE2TO5,XEN,VALGUL,ROGUL,LNGLSN,LSKNM,INILAY */
/* ** INDICES:    TCON,APF,APF_ONLY */
/* ** Storm:   	LSTID,storm */
/* ** ion drift:  vdrift,bspl4_time,bspl4_long,g,stormvd, */
/* **             bspl4_ptime */
/* ** spread-F:	spreadf_brazil,bspl2f,bspl2l,bspl2s,bspl4t */
/* ************************************************************** */

/* ************************************************************** */
/* ***  -------------------ADDRESSES------------------------  *** */
/* ***  I  PROF. K. RAWER             DR. D. BILITZA       I  *** */
/* ***  I  HERRENSTR. 43              GSFC CODE 632        I  *** */
/* ***  I  7801 MARCH 1               GREENBELT MD 20771   I  *** */
/* ***  I  F.R.G.                     USA                  I  *** */
/* ***  ----------------------------------------------------  *** */
/* ************************************************************** */
/* ************************************************************** */


/* Subroutine */ int initialize_()
{
    /* Builtin functions */
    double atan(doublereal);

    /* Local variables */
    static real pi;

    argexp_1.argmax = (float)88.;
    pi = atan((float)1.) * (float)4.;
    const_1.umr = pi / (float)180.;
    const1_1.humr = pi / (float)12.;
    const1_1.dumr = pi / (float)182.5;
    return 0;
} /* initialize_ */


/* ************************************************************* */
/* *************** ELECTRON DENSITY **************************** */
/* ************************************************************* */

doublereal xe1_(real *h__)
{
    /* System generated locals */
    real ret_val;

    /* Builtin functions */
    double r_sign(real *, real *), exp(doublereal);

    /* Local variables */
    static real dxdh, xred, tcor;
    extern doublereal eptr_(real *, real *, real *), topq_(real *, real *, 
	    real *, real *);
    static real eptr1, eptr2, x, y, x0, rco, xmx0;

/* ---------------------------------------------------------------- */
/* REPRESENTING ELECTRON DENSITY(M-3) IN THE TOPSIDE IONOSPHERE */
/* (H=HMF2....1000 KM) BY HARMONIZED BENT-MODEL ADMITTING */
/* VARIABILITY OFGLOBAL PARAMETER ETA,ZETA,BETA,DELTA WITH */
/* GEOM. LATITUDE, SMOOTHED SOLAR FLUX AND CRITICAL FREQUENCY */
/* (SEE MAIN PROGRAM). */
/* [REF.:K.RAWER,S.RAMAKRISHNAN,1978] */
/* ---------------------------------------------------------------- */
    if (blo11_1.itopn == 2) {
	ret_val = topq_(h__, &block1_1.xnmf2, &block1_1.hmf2, &blo11_1.b2top);
	return ret_val;
    }
    dxdh = ((float)1e3 - block1_1.hmf2) / (float)700.;
    x0 = (float)300. - blo10_1.delta;
    xmx0 = (*h__ - block1_1.hmf2) / dxdh;
    x = xmx0 + x0;
    eptr1 = eptr_(&x, &blo10_1.beta, &c_b3) - eptr_(&x0, &blo10_1.beta, &c_b3)
	    ;
    eptr2 = eptr_(&x, &c_b5, &c_b6) - eptr_(&x0, &c_b5, &c_b6);
    y = blo10_1.beta * blo10_1.eta * eptr1 + blo10_1.zeta * (eptr2 * (float)
	    100. - xmx0);
    y *= dxdh;
    if (dabs(y) > argexp_1.argmax) {
	y = r_sign(&argexp_1.argmax, &y);
    }
    tcor = (float)0.;
    if (blo11_1.itopn == 1 && *h__ > blo11_1.hcor1) {
	xred = *h__ - blo11_1.hcor1;
	rco = blo11_1.tc3 * xred;
	tcor = rco * blo11_1.alg10;
    }
    ret_val = block1_1.xnmf2 * exp(-y + tcor);
    return ret_val;
} /* xe1_ */



doublereal topq_(real *h__, real *no, real *hmax, real *ho)
{
    /* System generated locals */
    real ret_val, r__1;

    /* Builtin functions */
    double exp(doublereal);

    /* Local variables */
    static real z__, g1, ee, dh, ep;

/* ---------------------------------------------------------------- */
/*  NeQuick formula */
/* ---------------------------------------------------------------- */
    dh = *h__ - *hmax;
    g1 = dh * (float).125;
    z__ = dh / (*ho * (g1 * (float)100. / (*ho * (float)100. + g1) + (float)
	    1.));
    if (z__ > (float)40.) {
	ret_val = (float)0.;
	return ret_val;
    }
    ee = exp(z__);
    if (ee > (float)1e7) {
	ep = (float)4. / ee;
    } else {
/* Computing 2nd power */
	r__1 = ee + (float)1.;
	ep = ee * (float)4. / (r__1 * r__1);
    }
    ret_val = *no * ep;
    return ret_val;
} /* topq_ */



doublereal zero_(real *delta)
{
    /* System generated locals */
    real ret_val;

    /* Builtin functions */
    double exp(doublereal);

    /* Local variables */
    static real z1, z2, arg1;

/* FOR A PEAK AT X0 THE FUNCTION ZERO HAS TO BE EQUAL TO 0. */
    arg1 = *delta / (float)100.;
    if (dabs(arg1) < argexp_1.argmax) {
	z1 = (float)1. / (exp(arg1) + (float)1.);
    } else if (arg1 < (float)0.) {
	z1 = (float)1.;
    } else {
	z1 = (float)0.;
    }
    arg1 = (*delta + (float)94.5) / blo10_2.beta;
    if (dabs(arg1) < argexp_1.argmax) {
	z2 = (float)1. / (exp(arg1) + (float)1.);
    } else if (arg1 < (float)0.) {
	z2 = (float)1.;
    } else {
	z2 = (float)0.;
    }
    ret_val = blo10_2.zeta * ((float)1. - z1) - blo10_2.eta * z2;
    return ret_val;
} /* zero_ */



doublereal dxe1n_(real *h__)
{
    /* System generated locals */
    real ret_val;

    /* Local variables */
    extern doublereal epst_(real *, real *, real *);
    static real epst1, epst2, x, x0;

/* LOGARITHMIC DERIVATIVE OF FUNCTION XE1 (KM-1). */
    x0 = (float)300. - blo10_1.delta;
    x = (*h__ - block1_1.hmf2) / ((float)1e3 - block1_1.hmf2) * (float)700. + 
	    x0;
    epst2 = epst_(&x, &c_b5, &c_b6);
    epst1 = epst_(&x, &blo10_1.beta, &c_b3);
    ret_val = -blo10_1.eta * epst1 + blo10_1.zeta * ((float)1. - epst2);
    return ret_val;
} /* dxe1n_ */



doublereal xe2_(real *h__)
{
    /* System generated locals */
    real ret_val;
    doublereal d__1, d__2;

    /* Builtin functions */
    double pow_dd(doublereal *, doublereal *), exp(doublereal), cosh(
	    doublereal);

    /* Local variables */
    static real x, z__;

/* ELECTRON DENSITY FOR THE BOTTOMSIDE F-REGION (HMF1...HMF2). */
    x = (block1_1.hmf2 - *h__) / block2_1.b0;
    if (x <= (float)0.) {
	x = (float)0.;
    }
    d__1 = (doublereal) x;
    d__2 = (doublereal) block2_1.b1;
    z__ = pow_dd(&d__1, &d__2);
    if (z__ > argexp_1.argmax) {
	z__ = argexp_1.argmax;
    }
    ret_val = block1_1.xnmf2 * exp(-z__) / cosh(x);
    return ret_val;
} /* xe2_ */



doublereal xe3_1__(real *h__)
{
    /* System generated locals */
    real ret_val;
    doublereal d__1, d__2;

    /* Builtin functions */
    double pow_dd(doublereal *, doublereal *);

    /* Local variables */
    static real h1bar;
    extern doublereal xe2_(real *);

/* ELECTRON DENSITY FOR THE F1-LAYER (HZ.....HMF1) */
/* USING THE NEW DEFINED F1-LAYER FUNCTION (Reinisch and Huang, Advances */
/* in Space Research, Volume 25, Number 1, 81-88, 2000) */

    h1bar = *h__;
    if (block1_1.f1reg) {
	d__1 = (doublereal) ((block1_1.hmf1 - *h__) / block1_1.hmf1);
	d__2 = (doublereal) (block2_2.d1f1 + (float)1.);
	h1bar = block1_1.hmf1 * ((float)1. - pow_dd(&d__1, &d__2));
    }
    ret_val = xe2_(&h1bar);
    return ret_val;
} /* xe3_1__ */



doublereal xe4_1__(real *h__)
{
    /* System generated locals */
    real ret_val;

    /* Builtin functions */
    double r_sign(real *, real *), sqrt(doublereal);

    /* Local variables */
    extern doublereal xe3_1__(real *);
    static real h1bar;

/* ELECTRON DENSITY FOR THE INTERMEDIATE REGION (HEF...HZ) */
/* USING THE NEW DEFINED FUNCTION */

    if (block3_1.hst < (float)0.) {
	ret_val = block4_1.xnme + block3_1.t * (*h__ - block4_1.hef);
	return ret_val;
    }
    if (block3_1.hst == block4_1.hef) {
	h1bar = *h__;
    } else {
	h1bar = block3_1.hz + block3_1.t * (float).5 - r_sign(&c_b18, &
		block3_1.t) * sqrt(block3_1.t * (block3_1.t * (float).25 + 
		block3_1.hz - *h__));
    }
    ret_val = xe3_1__(&h1bar);
    return ret_val;
} /* xe4_1__ */



doublereal xe5_(real *h__)
{
    /* System generated locals */
    real ret_val;

    /* Builtin functions */
    double exp(doublereal);

    /* Local variables */
    static real t1, t3;

/* ELECTRON DENSITY FOR THE E AND VALLEY REGION (HME..HEF). */
    t3 = *h__ - block4_1.hme;
    t1 = t3 * t3 * (block5_1.e[0] + t3 * (block5_1.e[1] + t3 * (block5_1.e[2] 
	    + t3 * block5_1.e[3])));
    if (block5_1.night) {
	goto L100;
    }
    ret_val = block4_1.xnme * (t1 + 1);
    return ret_val;
L100:
    ret_val = block4_1.xnme * exp(t1);
    return ret_val;
} /* xe5_ */



doublereal xe6_(real *h__)
{
    /* System generated locals */
    real ret_val;
    doublereal d__1, d__2;

    /* Builtin functions */
    double exp(doublereal), pow_dd(doublereal *, doublereal *);

    /* Local variables */
    static real z__, fp3;

/* ELECTRON DENSITY FOR THE D REGION (HA...HME). */
    if (*h__ > block6_1.hdx) {
	goto L100;
    }
    z__ = *h__ - block6_1.hmd;
    fp3 = block7_1.fp3u;
    if (z__ > (float)0.) {
	fp3 = block7_1.fp30;
    }
    ret_val = block6_1.xnmd * exp(z__ * (block7_1.fp1 + z__ * (block7_1.fp2 + 
	    z__ * fp3)));
    return ret_val;
L100:
    z__ = block4_1.hme - *h__;
    d__1 = (doublereal) z__;
    d__2 = (doublereal) block7_1.xkk;
    ret_val = block4_1.xnme * exp(-block7_1.d1 * pow_dd(&d__1, &d__2));
    return ret_val;
} /* xe6_ */



doublereal xe_1__(real *h__)
{
    /* System generated locals */
    real ret_val;

    /* Local variables */
    extern doublereal xe3_1__(real *), xe4_1__(real *), xe1_(real *), xe2_(
	    real *), xe5_(real *), xe6_(real *);
    static real hmf1;

/* ELECTRON DENSITY BEETWEEN HA(KM) AND 1000 KM */
/* SUMMARIZING PROCEDURES  NE1....6; */
    if (block1_2.f1reg) {
	hmf1 = block1_2.xhmf1;
    } else {
	hmf1 = block1_2.hmf2;
    }
    if (*h__ < block1_2.hmf2) {
	goto L100;
    }
    ret_val = xe1_(h__);
    return ret_val;
L100:
    if (*h__ < hmf1) {
	goto L300;
    }
    ret_val = xe2_(h__);
    return ret_val;
L300:
    if (*h__ < block3_1.hz) {
	goto L400;
    }
    ret_val = xe3_1__(h__);
    return ret_val;
L400:
    if (*h__ < block4_1.hef) {
	goto L500;
    }
    ret_val = xe4_1__(h__);
    return ret_val;
L500:
    if (*h__ < block4_1.hme) {
	goto L600;
    }
    ret_val = xe5_(h__);
    return ret_val;
L600:
    ret_val = xe6_(h__);
    return ret_val;
} /* xe_1__ */



/* Subroutine */ int calne_(integer *crd, real *invdip, real *fl, real *dimo, 
	real *b0, real *dipl, real *mlt, real *alt, integer *ddd, real *f107, 
	real *nne)
{
    /* Initialized data */

    static struct {
	real e_1[8];
	integer fill_2[4];
	real e_3[8];
	integer fill_4[4];
	real e_5[8];
	integer fill_6[4];
	real e_7[8];
	integer fill_8[4];
	real e_9[8];
	integer fill_10[4];
	real e_11[8];
	integer fill_12[4];
	real e_13[8];
	integer fill_14[4];
	real e_15[8];
	integer fill_16[4];
	real e_17[8];
	integer fill_18[4];
	real e_19[8];
	integer fill_20[4];
	real e_21[8];
	integer fill_22[4];
	real e_23[8];
	integer fill_24[4];
	real e_25[8];
	integer fill_26[4];
	real e_27[8];
	integer fill_28[4];
	real e_29[8];
	integer fill_30[4];
	real e_31[8];
	integer fill_32[4];
	real e_33[8];
	integer fill_34[4];
	real e_35[8];
	integer fill_36[4];
	real e_37[8];
	integer fill_38[4];
	real e_39[8];
	integer fill_40[4];
	real e_41[8];
	integer fill_42[4];
	real e_43[8];
	integer fill_44[4];
	real e_45[8];
	integer fill_46[4];
	real e_47[8];
	integer fill_48[4];
	real e_49[8];
	integer fill_50[4];
	real e_51[8];
	integer fill_52[4];
	real e_53[8];
	integer fill_54[4];
	real e_55[8];
	integer fill_56[4];
	real e_57[8];
	integer fill_58[4];
	real e_59[8];
	integer fill_60[4];
	real e_61[8];
	integer fill_62[4];
	real e_63[8];
	integer fill_64[4];
	real e_65[8];
	integer fill_66[4];
	real e_67[8];
	integer fill_68[4];
	real e_69[8];
	integer fill_70[4];
	real e_71[8];
	integer fill_72[4];
	real e_73[8];
	integer fill_74[4];
	real e_75[8];
	integer fill_76[4];
	real e_77[8];
	integer fill_78[4];
	real e_79[8];
	integer fill_80[4];
	real e_81[8];
	integer fill_82[4];
	real e_83[8];
	integer fill_84[4];
	real e_85[8];
	integer fill_86[4];
	real e_87[8];
	integer fill_88[4];
	real e_89[8];
	integer fill_90[4];
	real e_91[8];
	integer fill_92[4];
	real e_93[8];
	integer fill_94[4];
	real e_95[8];
	integer fill_96[4];
	real e_97[8];
	integer fill_98[4];
	} equiv_37 = { (float)11.654, (float)10.964, (float)10.369, (float)
		10.044, (float)11.447, (float)10.747, (float)10.23, (float)
		9.8437, {0}, (float)-2.2826e-8, (float)-1.5354e-9, (float)
		1.9844e-8, (float)-4.8215e-10, (float).33033, (float).2354, (
		float).18549, (float).28312, {0}, (float)-.29373, (float)
		-.17357, (float)-.20005, (float)-.3576, (float)-.38532, (
		float)-.16245, (float)-.17478, (float)-.47686, {0}, (float)
		-3.4268e-10, (float)2.4852e-8, (float)4.6147e-8, (float)
		2.1775e-8, (float)-.22359, (float)-.015474, (float).023684, (
		float).14238, {0}, (float).064972, (float).1255, (float)
		.13227, (float).082508, (float).21158, (float).084395, (float)
		.049509, (float)-.0096995, {0}, (float)-4.1631e-8, (float)
		-5.3015e-9, (float)1.8945e-8, (float)4.7923e-9, (float)
		-.013881, (float).036128, (float)-.079218, (float)-.048066, {
		0}, (float).002504, (float)-8.5408e-4, (float).11316, (float)
		.094327, (float)-.029286, (float)6.6939e-4, (float).12211, (
		float).1203, {0}, (float)-.23607, (float)-.27902, (float)
		-.18108, (float)-.054271, (float)-.23339, (float)-.23379, (
		float)-.19639, (float)-.078194, {0}, (float)3.3224e-9, (float)
		4.7844e-9, (float)2.7934e-9, (float)5.2633e-10, (float)
		.0077235, (float).034316, (float)-.03928, (float).013805, {0},
		 (float)-6.5151e-4, (float).007144, (float)-.014698, (float)
		.011187, (float).05723, (float).0060415, (float).031281, (
		float).008247, {0}, (float)4.38e-9, (float)1.8258e-9, (float)
		4.1471e-9, (float)-5.1303e-9, (float)-.034982, (float)
		.0050101, (float).014507, (float).0046119, {0}, (float)
		5.517e-4, (float)-.0023096, (float).0035627, (float)-.021555, 
		(float).011379, (float)-.0034666, (float)-.010088, (float)
		-.036194, {0}, (float)3.3359e-9, (float)2.5878e-9, (float)
		2.7073e-9, (float)5.7277e-10, (float).0025918, (float)
		-.0077481, (float).0057436, (float).0028395, {0}, (float)
		-.38492, (float)-.27308, (float)-.072201, (float)-.0072596, (
		float)-.33942, (float)-.30157, (float)-.12489, (float)
		-.026545, {0}, (float)7.7351e-9, (float)4.6071e-9, (float)
		1.7208e-9, (float)-6.4714e-10, (float).049621, (float)
		-.0057407, (float).012205, (float).0011682, {0}, (float)
		.048326, (float).048916, (float).039133, (float)-.010944, (
		float).06018, (float).063775, (float).0089282, (float).011891,
		 {0}, (float)-2.4344e-9, (float)-9.9364e-9, (float)-3.6288e-9,
		 (float)1.2469e-9, (float)-.0079666, (float).010736, (float)
		-.011558, (float)-.0024794, {0}, (float)-.0092751, (float)
		-.038824, (float)-.003979, (float).0012361, (float).002157, (
		float)-.0040436, (float).0061501, (float).014748, {0}, (float)
		4.8353e-9, (float)4.5371e-9, (float)5.9338e-10, (float)
		3.2732e-11, (float)-.0080235, (float).007492, (float)
		-.0052639, (float)-.0099502, {0}, (float).10247, (float)
		.0039659, (float)-.044768, (float)-.060089, (float).0807, (
		float)-.019215, (float)-.023903, (float).018415, {0}, (float)
		-7.7373e-10, (float)-1.9952e-9, (float)-1.135e-9, (float)
		5.949e-10, (float).03504, (float).010255, (float)-.0090686, (
		float)-.033637, {0}, (float)-.0043734, (float)-5.8555e-4, (
		float)-.0019636, (float)-.0024869, (float)-.0044115, (float)
		.0051722, (float)-2.4549e-5, (float).01099, {0}, (float)
		-1.4332e-10, (float)1.4458e-9, (float)1.6649e-9, (float)
		-1.2181e-10, (float).006931, (float)-.0050936, (float)
		6.6842e-6, (float)-.010111, {0}, (float)-.0014617, (float)
		.0069817, (float).0063878, (float)-8.3456e-4, (float).0055866,
		 (float)-8.485e-4, (float)-2.8069e-4, (float).0029349, {0}, (
		float)-.038595, (float).041239, (float).013926, (float).02139,
		 (float).037179, (float)-.056382, (float)-.042259, (float)
		-.025495, {0}, (float)2.2162e-9, (float)-5.4244e-10, (float)
		-2.8892e-10, (float)3.7568e-10, (float).018795, (float)
		.016156, (float)-.0087094, (float)-.043211, {0}, (float)
		.020572, (float).010611, (float).0050022, (float).0099164, (
		float).015728, (float).014547, (float).0080624, (float)
		.011705, {0}, (float)-2.2251e-9, (float)-1.3677e-9, (float)
		-8.1142e-11, (float)-1.1158e-9, (float).0079808, (float)
		-.0035661, (float)-.0024729, (float)-.010655, {0}, (float)
		.0037842, (float).0028933, (float).0013966, (float)8.9177e-4, 
		(float)-.0037419, (float).0051087, (float)-5.7736e-4, (float)
		.0051037, {0}, (float)-.075166, (float).028197, (float)
		.035646, (float).013681, (float).017878, (float)-.023719, (
		float).0061173, (float).015616, {0}, (float)6.2465e-10, (
		float)-1.3422e-9, (float)-8.2139e-10, (float)3.0065e-10, (
		float)-.0094949, (float)8.3592e-4, (float).0014685, (float)
		.0031856, {0}, (float).0016258, (float)-.0085109, (float)
		-.0016012, (float).0026197, (float).008762, (float).0031225, (
		float)-.0019136, (float)-.0036023, {0}, (float)2.5023e-10, (
		float)9.2018e-10, (float)-1.0333e-10, (float)-2.8667e-10, (
		float)-.0022723, (float).0019821, (float)-6.6155e-4, (float)
		-9.4473e-4, {0}, (float).019937, (float)-.020264, (float)
		.021714, (float).012303, (float).02158, (float).026843, (
		float)-.016568, (float).0085888, {0}, (float)-1.5199e-9, (
		float)-8.7024e-10, (float)-1.7686e-9, (float)-2.7819e-10, (
		float).030423, (float).0026876, (float)-.0066461, (float)
		.0024022, {0}, (float)-.0043, (float)-.0022666, (float)
		-.0027486, (float).0010276, (float)-.001615, (float)-.0062698,
		 (float)-.0029717, (float).0055707, {0}, (float)5.8564e-10, (
		float)-3.236e-10, (float)-1.6622e-10, (float)-4.8067e-10, (
		float)-.0020801, (float)-2.7731e-4, (float)-5.9281e-4, (float)
		-1.65e-5, {0}, (float).026435, (float).0048919, (float).01423,
		 (float).018107, (float)-.026775, (float)-.018704, (float)
		.0059044, (float)-.0057463, {0}, (float)1.3539e-9, (float)
		-7.2078e-10, (float)6.645e-11, (float)-6.3629e-11, (float)
		.0047439, (float).0022622, (float)-.0015221, (float).0052602, 
		{0}, (float).0052811, (float)-.0030536, (float)6.071e-4, (
		float).001416, (float)-6.2996e-4, (float).0011103, (float)
		-.0010024, (float)-.0010524, {0}, (float).024115, (float)
		-.0077757, (float).010634, (float).013259, (float)-.038774, (
		float).0074859, (float).004364, (float)-.0045751, {0}, (float)
		7.1913e-10, (float)-7.6475e-10, (float)-7.634e-10, (float)
		-4.2105e-10, (float)-.0010191, (float)-.0015696, (float)
		-1.2153e-5, (float).0030727, {0}, (float).0028906, (float)
		-.0038334, (float)-.0028168, (float)3.9284e-4, (float)
		-5.9711e-4, (float)-.0014674, (float)-4.6287e-4, (float)
		-.0016474, {0}, (float)-.010944, (float).0044619, (float)
		4.7432e-4, (float)-.0011911, (float)-.00618, (float)-.012356, 
		(float)-.0014742, (float)-.0031363, {0}, (float)4.8498e-11, (
		float)-3.4009e-10, (float)-1.1803e-11, (float)1.2981e-11, (
		float).0043409, (float)-6.2705e-5, (float)3.7337e-4, (float)
		4.2265e-4, {0}, (float)5.0294e-4, (float).0034805, (float)
		-4.2779e-4, (float)-7.4254e-4, (float)-.010281, (float)
		-.009027, (float)1.2299e-4, (float)-5.1937e-4, {0}, (float)
		-2.3208e-11, (float)-3.3311e-10, (float)-3.4642e-10, (float)
		2.7134e-11, (float).0023239, (float)-2.1341e-4, (float)
		-1.8383e-4, (float).0012478, {0}, (float).0096733, (float)
		-.0050825, (float)-.0011471, (float).0032463, (float)
		-.0094754, (float)-.014714, (float)3.4953e-4, (float)
		-.0021649, {0}, (float)-.006112, (float)-.0026849, (float)
		.0050097, (float)-.0030059, (float)-.012886, (float)-.0026567,
		 (float)-.0013791, (float)-5.2399e-4 };

#define dneh ((real *)&equiv_37)

    static real dnel[441]	/* was [3][3][49] */ = { (float)11.062,(float)
	    10.41,(float)10.158,(float)10.967,(float)10.407,(float)10.166,0.0,
	    0.0,0.0,(float)-3.0911e-8,(float)-5.5863e-8,(float)-1.9548e-8,(
	    float).40368,(float).3515,(float).23694,0.0,0.0,0.0,(float)
	    -.38235,(float)-.28735,(float)-.36749,(float)-.30547,(float)
	    -.26152,(float)-.27429,0.0,0.0,0.0,(float)1.1313e-8,(float)
	    -6.7549e-8,(float)-8.0357e-9,(float)-.13178,(float).03821,(float)
	    .1152,0.0,0.0,0.0,(float).14829,(float)-.039147,(float).025141,(
	    float).1221,(float).025637,(float).030762,0.0,0.0,0.0,(float)
	    -6.3573e-8,(float)-7.4702e-8,(float)-3.2793e-8,(float).047599,(
	    float)-.031519,(float)-.082451,0.0,0.0,0.0,(float)-.031902,(float)
	    -.07756,(float).064338,(float).045964,(float).093065,(float)
	    .13504,0.0,0.0,0.0,(float)-.24123,(float)-.2677,(float)-.20881,(
	    float)-.2694,(float)-.20338,(float)-.14885,0.0,0.0,0.0,(float)
	    2.3955e-9,(float)2.6217e-9,(float)2.2857e-9,(float).050649,(float)
	    .024015,(float).0063431,0.0,0.0,0.0,(float)-.034781,(float)
	    -.02678,(float)-.018004,(float).020835,(float).048189,(float)
	    .072066,0.0,0.0,0.0,(float)1.3383e-9,(float)-1.8328e-9,(float)
	    4.1065e-9,(float).013125,(float)-.01231,(float)-.014211,0.0,0.0,
	    0.0,(float)-.024546,(float)-.03837,(float)-.009227,(float)
	    -.026758,(float)-.012074,(float)-.012608,0.0,0.0,0.0,(float)
	    1.8658e-9,(float)1.132e-9,(float)2.3974e-9,(float).0083855,(float)
	    .0063295,(float).012061,0.0,0.0,0.0,(float)-.26632,(float)-.18013,
	    (float)-.14469,(float)-.30743,(float)-.16583,(float)-.10051,0.0,
	    0.0,0.0,(float)4.8835e-9,(float)3.8029e-9,(float)5.8152e-10,(
	    float)-.035169,(float)-.021338,(float)7.1648e-4,0.0,0.0,0.0,(
	    float).0029025,(float)-.031418,(float)-.071135,(float).068584,(
	    float).039239,(float).029195,0.0,0.0,0.0,(float)-4.6079e-9,(float)
	    -6.7073e-9,(float)2.8358e-9,(float).022068,(float).0053636,(float)
	    .0053905,0.0,0.0,0.0,(float)-.032911,(float)-.057161,(float)
	    -.025824,(float)-.022965,(float)-.012633,(float)-.017088,0.0,0.0,
	    0.0,(float)2.0026e-9,(float)2.4874e-10,(float)1.2833e-9,(float)
	    -.012112,(float).00144,(float).014637,0.0,0.0,0.0,(float).076453,(
	    float).059222,(float).044022,(float).056553,(float).068102,(float)
	    .021114,0.0,0.0,0.0,(float)8.8255e-10,(float)-1.2232e-9,(float)
	    1.9468e-9,(float)-.0057016,(float)-.0063957,(float)-.0079212,0.0,
	    0.0,0.0,(float).0010963,(float)-.0014529,(float).017507,(float)
	    .008737,(float).0090268,(float).0084367,0.0,0.0,0.0,(float)
	    -2.9776e-10,(float)9.9371e-12,(float)-1.7886e-9,(float)-.0030176,(
	    float)-.0089534,(float)-.016341,0.0,0.0,0.0,(float)-.001217,(
	    float).0036985,(float).0016676,(float)9.2492e-4,(float)7.3362e-4,(
	    float).0049292,0.0,0.0,0.0,(float)-.038531,(float)-.0039165,(
	    float).0056214,(float)-.04243,(float)-.038612,(float)-.022852,0.0,
	    0.0,0.0,(float)-1.7732e-10,(float)3.3004e-9,(float)1.7403e-9,(
	    float)-.014924,(float)-.018755,(float)1.2029e-4,0.0,0.0,0.0,(
	    float)-.0012508,(float).003805,(float).0068439,(float).007493,(
	    float).0053402,(float)2.7544e-4,0.0,0.0,0.0,(float)6.1993e-10,(
	    float)-7.624e-10,(float)-9.6757e-10,(float)-.005169,(float)
	    .0047158,(float).011996,0.0,0.0,0.0,(float)-1.9205e-4,(float)
	    -.0040473,(float)5.7429e-5,(float)9.9006e-4,(float)7.3675e-4,(
	    float)-1.6057e-5,0.0,0.0,0.0,(float)-.0091547,(float).013918,(
	    float).0041441,(float).01521,(float).02728,(float).03187,0.0,0.0,
	    0.0,(float)6.1475e-10,(float)-9.8974e-11,(float)4.7678e-10,(float)
	    -7.8933e-4,(float).001765,(float).0036162,0.0,0.0,0.0,(float)
	    .0031384,(float).0010506,(float).002627,(float)-.0021342,(float)
	    -.0035763,(float)-.0032132,0.0,0.0,0.0,(float)-3.7043e-10,(float)
	    -8.294e-10,(float)5.0045e-11,(float)-4.1959e-4,(float).0017527,(
	    float).0014402,0.0,0.0,0.0,(float).036801,(float).019068,(float)
	    .026003,(float).0091102,(float)-.017144,(float).013314,0.0,0.0,
	    0.0,(float)1.1714e-9,(float)3.1352e-9,(float)1.9442e-9,(float)
	    .0014167,(float)-.0013996,(float)-.0018461,0.0,0.0,0.0,(float)
	    .0037754,(float).0073603,(float).0061356,(float)-.0030343,(float)
	    .0027811,(float).0058733,0.0,0.0,0.0,(float)-2.6027e-10,(float)
	    6.3141e-12,(float)-1.8709e-10,(float)5.129e-4,(float)-.0014789,(
	    float)-.0028286,0.0,0.0,0.0,(float)-.0020821,(float)1.8285e-5,(
	    float)-.0042284,(float)-.01713,(float)-.0021127,(float)-.0061876,
	    0.0,0.0,0.0,(float)3.3725e-10,(float)2.3434e-10,(float)
	    -5.4793e-10,(float).0036913,(float).0040656,(float).0052857,0.0,
	    0.0,0.0,(float).0014911,(float)3.944e-4,(float)-8.4581e-4,(float)
	    -3.4739e-4,(float)2.7334e-4,(float)-.0014054,0.0,0.0,0.0,(float)
	    .0087638,(float).0088034,(float).011288,(float)8.7815e-4,(float)
	    -.0021757,(float)-.0041018,0.0,0.0,0.0,(float)4.5218e-10,(float)
	    1.188e-9,(float)-5.9374e-10,(float).0055685,(float).002767,(float)
	    -3.0424e-4,0.0,0.0,0.0,(float)3.2066e-4,(float).0021945,(float)
	    -7.3269e-4,(float)-.0012357,(float)-4.2398e-5,(float)3.4306e-4,
	    0.0,0.0,0.0,(float)-.0036726,(float)-.004336,(float)-.0016501,(
	    float)-.021451,(float)-.012504,(float)-.0064164,0.0,0.0,0.0,(
	    float)1.0395e-10,(float)1.1249e-10,(float)1.2251e-10,(float)
	    .0025966,(float)5.5083e-4,(float)3.4624e-4,0.0,0.0,0.0,(float)
	    .0029062,(float)6.2337e-4,(float)8.1305e-4,(float)-.0082735,(
	    float)-.0063574,(float)-.0035857,0.0,0.0,0.0,(float)2.783e-10,(
	    float)-9.1034e-11,(float)-3.6491e-10,(float).0026567,(float)
	    .0019005,(float).0015636,0.0,0.0,0.0,(float)-.0037388,(float)
	    -.0024846,(float)-.001568,(float)-.017557,(float)-.014233,(float)
	    -.0060995,0.0,0.0,0.0,(float).0033715,(float).0033931,(float)
	    -4.4728e-5,(float)-.0037272,(float)-.0028291,(float)-.0055164 };

    /* System generated locals */
    doublereal d__1;

    /* Builtin functions */
    double r_lg10(real *), pow_dd(doublereal *, doublereal *);

    /* Local variables */
    static real nneh, nnel;
    extern /* Subroutine */ int nelow_(integer *, real *, real *, real *, 
	    real *, real *, real *, real *, integer *, real *, real *), 
	    nehigh_(integer *, real *, real *, real *, real *, real *, real *,
	     real *, integer *, real *, real *);

/* Version 1.0 (released 30.6.2004) */
/* CALNE calculates electron density in the outer */
/* ionosphere with account of solar activity (F107 index). */
/* CALNE uses subroutines NELOW and NEHIGH. */
/* Linearly interpolates for solar activity. */
/* Inputs: CRD - 0 .. INVDIP */
/*               1 .. FL, DIMO, B0, DIPL (used for calculation INVDIP inside) */
/*         INVDIP - "mix" coordinate of the dip latitude and of */
/*                    the invariant latitude; */
/*                    positive northward, in deg, range <-90.0;90.0> */
/*         FL, DIMO, BO - McIlwain L parameter, dipole moment in */
/*                        Gauss, magnetic field strength in Gauss - */
/*                        parameters needed for invariant latitude */
/*                        calculation */
/*         DIPL - dip latitude */
/*                positive northward, in deg, range <-90.0;90.0> */
/*         MLT - magnetic local time (central dipole) */
/*               in hours, range <0;24) */
/*         ALT - altitude above the Earth's surface; */
/*               in km, range <350;2000> */
/*         DDD - day of year; range <0;365> */
/*         F107 - F107 index */
/* Output: NNE - electron density [m-3] */
/* Versions:  1.0 FORTRAN */
/* Author of the code: */
/*         Vladimir Truhlik */
/*         Institute of Atm. Phys. */
/*         Bocni II. */
/*         141 31 Praha 4, Sporilov */
/*         Czech Republic */
/*         e-mail: vtr@ufa.cas.cz */
/*         tel/fax: +420 267103058 */
/* /////////////////////coefficients high solar activity//////////////////////// */
/* //////////////////////////////////NE///////////////////////////////////////// */
/*     550km equinox */
/*     550km June solstice */
/*     900km equinox */
/*     900km June solstice */
/*     1500km equinox */
/*     1500km June solstice */
/*     2400km equinox */
/*     2400km June solstice */
/* //////////////////////////////////////////////////////////////////////////////////// */
/* /////////////////////coefficients low solar activity//////////////////////////////// */
/* //////////////////////////////////Ne//////////////////////////////////////////////// */
/*     400km equinox */
/*     400km June solstice */
/*     650km equinox */
/*     650km June solstice */
/*     1000km equinox */
/*     1000km June solstice */
/* //////////////////////////////////////////////////////////////////////////////////// */
/* ///////////////////////////////solar minimum//////////////////////////////////////// */
    nelow_(crd, invdip, fl, dimo, b0, dipl, mlt, alt, ddd, dnel, &nnel);
/* ///////////////////////////////solar maximum//////////////////////////////////////// */
    nehigh_(crd, invdip, fl, dimo, b0, dipl, mlt, alt, ddd, dneh, &nneh);
/*     interpolation (in logarithm) */
    *nne = (r_lg10(&nneh) - r_lg10(&nnel)) / (float)115. * (*f107 - (float)
	    85.) + r_lg10(&nnel);
    d__1 = (doublereal) (*nne);
    *nne = pow_dd(&c_b30, &d__1);
    return 0;
} /* calne_ */

#undef dneh


/* Subroutine */ int nelow_(integer *crd, real *invdip, real *fl, real *dimo, 
	real *b0, real *dipl, real *mlt, real *alt, integer *ddd, real *d__, 
	real *nne)
{
    /* Initialized data */

    static integer mirreq[49] = { 1,-1,1,-1,1,-1,1,1,-1,1,-1,1,-1,1,-1,1,-1,1,
	    -1,1,-1,1,-1,1,1,-1,1,-1,1,1,-1,1,-1,1,-1,1,-1,1,-1,1,1,-1,1,1,-1,
	    1,-1,1,1 };

    /* System generated locals */
    doublereal d__1;

    /* Builtin functions */
    double log(doublereal), pow_dd(doublereal *, doublereal *);

    /* Local variables */
    static real n0a100, n0b100, n0a400, n0b400, n0a650, n0b650;
    static integer seza, sezb;
    static real dtor;
    extern doublereal eptr_(real *, real *, real *);
    static real rmlt, c__[49];
    static integer i__, sezai, sezbi;
    static real invdp;
    extern /* Subroutine */ int spharm_ik__(real *, integer *, integer *, 
	    real *, real *);
    static real aa, bb, ah[3], st[2];
    extern doublereal invdpc_(real *, real *, real *, real *, real *);
    static real rcolat, n400, n650, st1, st2, n1000, ano[3], dno[1], sum, 
	    n100a, n100b, n400a, n400b, n650a, n650b;
    static integer ddda, dddb, dddd;

/* NELOW calculates electron density in the outer */
/* ionosphere for a low solar activity (F107 < 100). */
/* Based on spherical harmonics approximation of electron ion density */
/* (by AE-C, AE-D, and AE-E) at altitudes centred on 400km, 650km, and 1000km. */
/* For intermediate altitudes an interpolation is used. */
/* Recommended altitude range: 350-2000 km!!! */
/* For days between seasons centred at (21.3. = 79; 21.6. = 171; */
/* 23.9. 265; 21.12. = 354) relative ion density is linearly interpolated. */
/* Inputs: CRD - 0 .. INVDIP */
/*               1 .. FL, DIMO, B0, DIPL (used for calculation INVDIP inside) */
/*         INVDIP - "mix" coordinate of the dip latitude and of */
/*                    the invariant latitude; */
/*                    positive northward, in deg, range <-90.0;90.0> */
/*         FL, DIMO, BO - McIlwain L parameter, dipole moment in */
/*                        Gauss, magnetic field strength in Gauss - */
/*                        parameters needed for invariant latitude */
/*                        calculation */
/*         DIPL - dip latitude */
/*                positive northward, in deg, range <-90.0;90.0> */
/*         MLT - magnetic local time (central dipole) */
/*               in hours, range <0;24) */
/*         ALT - altitude above the Earth's surface; */
/*               in km, range <350;2000> */
/*         DDD - day of year; range <0;365> */
/*         D - coefficints of spherical harmonics for a given ion */
/* Output: NNE - Ne density */
    /* Parameter adjustments */
    d__ -= 13;

    /* Function Body */
/* //////////////////////////////////////////////////////////////////////////////////// */
    dtor = (float).01745329252;
/*     coefficients for mirroring */
    for (i__ = 1; i__ <= 49; ++i__) {
	d__[(i__ * 3 + 3) * 3 + 1] = d__[(i__ * 3 + 2) * 3 + 1] * mirreq[i__ 
		- 1];
	d__[(i__ * 3 + 3) * 3 + 2] = d__[(i__ * 3 + 2) * 3 + 2] * mirreq[i__ 
		- 1];
/* L10: */
	d__[(i__ * 3 + 3) * 3 + 3] = d__[(i__ * 3 + 2) * 3 + 3] * mirreq[i__ 
		- 1];
    }
    if (*crd == 1) {
	invdp = invdpc_(fl, dimo, b0, dipl, &dtor);
    } else if (*crd == 0) {
	invdp = *invdip;
    } else {
	return 0;
    }
    rmlt = *mlt * dtor * (float)15.;
    rcolat = ((float)90. - invdp) * dtor;
    spharm_ik__(c__, &c__6, &c__6, &rcolat, &rmlt);
/*     21.3. - 20.6. */
    if (*ddd >= 79 && *ddd < 171) {
	seza = 1;
	sezb = 2;
	ddda = 79;
	dddb = 171;
	dddd = *ddd;
    }
/*     21.6. - 22.9. */
    if (*ddd >= 171 && *ddd < 265) {
	seza = 2;
	sezb = 4;
	ddda = 171;
	dddb = 265;
	dddd = *ddd;
    }
/*     23.9. - 20.12. */
    if (*ddd >= 265 && *ddd < 354) {
	seza = 4;
	sezb = 3;
	ddda = 265;
	dddb = 354;
	dddd = *ddd;
    }
/*     21.12. - 20.3. */
    if (*ddd >= 354 || *ddd < 79) {
	seza = 3;
	sezb = 1;
	ddda = 354;
	dddb = 444;
	dddd = *ddd;
	if (*ddd >= 354) {
	    dddd = *ddd;
	} else {
	    dddd = *ddd + 365;
	}
    }
    sezai = (seza - 1) % 3 + 1;
    sezbi = (sezb - 1) % 3 + 1;
/*     400km level */
    n0a400 = (float)0.;
    n0b400 = (float)0.;
    for (i__ = 1; i__ <= 49; ++i__) {
	n0a400 += c__[i__ - 1] * d__[(sezai + i__ * 3) * 3 + 1];
/* L30: */
	n0b400 += c__[i__ - 1] * d__[(sezbi + i__ * 3) * 3 + 1];
    }
    n400a = n0a400;
    n400b = n0b400;
    n400 = (n400b - n400a) / (dddb - ddda) * (dddd - ddda) + n400a;
/*     650km level */
    n0a650 = (float)0.;
    n0b650 = (float)0.;
    for (i__ = 1; i__ <= 49; ++i__) {
	n0a650 += c__[i__ - 1] * d__[(sezai + i__ * 3) * 3 + 2];
/* L70: */
	n0b650 += c__[i__ - 1] * d__[(sezbi + i__ * 3) * 3 + 2];
    }
    n650a = n0a650;
    n650b = n0b650;
    n650 = (n650b - n650a) / (dddb - ddda) * (dddd - ddda) + n650a;
/*     1000km level */
    n0a100 = (float)0.;
    n0b100 = (float)0.;
    for (i__ = 1; i__ <= 49; ++i__) {
	n0a100 += c__[i__ - 1] * d__[(sezai + i__ * 3) * 3 + 3];
/* L110: */
	n0b100 += c__[i__ - 1] * d__[(sezbi + i__ * 3) * 3 + 3];
    }
    n100a = n0a100;
    n100b = n0b100;
    n1000 = (n100b - n100a) / (dddb - ddda) * (dddd - ddda) + n100a;
/*   n(O+) and n(N+) should not increase above 650 km */
/*       IF(((ION.eq.0).or.(ION.eq.3)).and.(N1000.gt.N650)) */
/*     &       N1000=N650 */
/*   n(H+) should not increase decrease above 650 km */
/*       if(((ION.eq.1).and.(N1000.gt.N650)) N1000=N650 */
/*      IF (ALT .LT. 650) NO=(N650-N400)/250.0*(ALT-400)+N400 */
/*      IF (ALT .GE. 650) NO=(N1000-N650)/350.0*(ALT-650)+N650 */
/*      NION=10**NO */
    ano[0] = n400;
    ano[1] = n650;
    ano[2] = n1000;
    ah[0] = (float)400.;
    ah[1] = (float)650.;
    ah[2] = (float)1e3;
    dno[0] = (float)20.;
    st1 = (ano[1] - ano[0]) / (ah[1] - ah[0]);
    i__ = 2;
    st2 = (ano[i__] - ano[i__ - 1]) / (ah[i__] - ah[i__ - 1]);
    ano[i__ - 1] -= (st2 - st1) * dno[i__ - 2] * log((float)2.);
    for (i__ = 1; i__ <= 2; ++i__) {
/* L220: */
	st[i__ - 1] = (ano[i__] - ano[i__ - 1]) / (ah[i__] - ah[i__ - 1]);
    }
    argexp_1.argmax = (float)88.;
    sum = ano[0] + st[0] * (*alt - ah[0]);
    i__ = 1;
    aa = eptr_(alt, &dno[i__ - 1], &ah[i__]);
    bb = eptr_(ah, &dno[i__ - 1], &ah[i__]);
    sum += (st[i__] - st[i__ - 1]) * (aa - bb) * dno[i__ - 1];
    d__1 = (doublereal) sum;
    *nne = pow_dd(&c_b30, &d__1);
    return 0;
} /* nelow_ */

/* Subroutine */ int nehigh_(integer *crd, real *invdip, real *fl, real *dimo,
	 real *b0, real *dipl, real *mlt, real *alt, integer *ddd, real *d__, 
	real *nne)
{
    /* Initialized data */

    static integer mirreq[49] = { 1,-1,1,-1,1,-1,1,1,-1,1,-1,1,-1,1,-1,1,-1,1,
	    -1,1,-1,1,-1,1,1,-1,1,-1,1,1,-1,1,-1,1,-1,1,-1,1,-1,1,1,-1,1,1,-1,
	    1,-1,1,1 };

    /* System generated locals */
    doublereal d__1;

    /* Builtin functions */
    double log(doublereal), pow_dd(doublereal *, doublereal *);

    /* Local variables */
    static real n0a150, n0b150, n0a250, n0a900, n0a550, n0b550, n0b900, 
	    n0b250;
    static integer seza, sezb;
    static real dtor;
    extern doublereal eptr_(real *, real *, real *);
    static real rmlt, c__[49];
    static integer i__, sezai, sezbi;
    static real invdp;
    extern /* Subroutine */ int spharm_ik__(real *, integer *, integer *, 
	    real *, real *);
    static real aa, bb, ah[4], st[3];
    extern doublereal invdpc_(real *, real *, real *, real *, real *);
    static real rcolat, n900, n550, st1, st2, n1500, n2500, ano[4], dno[2], 
	    sum, n150a, n150b, n250a, n900a, n550a, n550b, n900b, n250b;
    static integer ddda, dddb, dddd;

/* NEHIGH calculates electron density in the outer */
/* ionosphere for high solar activity (F107 >= 150). */
/* Based on spherical harmonics approximation of electron density */
/* (by IK24 and IK25) at altitudes centred on 550km, 900km, 1500km, and 2400km. */
/* For intermediate altitudes a interpolation is used. */
/* Recommended altitude range: 500-3000 km!!! */
/* For days between seasons centred at (21.3. = 79; 21.6. = 171; */
/* 23.9. 265; 21.12. = 354) relative ion density is linearly interpolated. */
/* Inputs: CRD - 0 .. INVDIP */
/*               1 .. FL, DIMO, B0, DIPL (used for calculation INVDIP inside) */
/*         INVDIP - "mix" coordinate of the dip latitude and of */
/*                    the invariant latitude; */
/*                    positive northward, in deg, range <-90.0;90.0> */
/*         FL, DIMO, BO - McIlwain L parameter, dipole moment in */
/*                        Gauss, magnetic field strength in Gauss - */
/*                        parameters needed for invariant latitude */
/*                        calculation */
/*         DIPL - dip latitude */
/*                positive northward, in deg, range <-90.0;90.0> */
/*         MLT - magnetic local time (central dipole) */
/*               in hours, range <0;24) */
/*         ALT - altitude above the Earth's surface; */
/*               in km, range <500;3000> */
/*         DDD - day of year; range <0;365> */
/*         D - coefficints of spherical harmonics for a given ion */
/* Output: NNE - electron density [m-3] */
    /* Parameter adjustments */
    d__ -= 17;

    /* Function Body */
/* //////////////////////////////////////////////////////////////////////////////////// */
    dtor = (float).01745329252;
/*     coefficients for mirroring */
    for (i__ = 1; i__ <= 49; ++i__) {
	d__[(i__ * 3 + 3 << 2) + 1] = d__[(i__ * 3 + 2 << 2) + 1] * mirreq[
		i__ - 1];
	d__[(i__ * 3 + 3 << 2) + 2] = d__[(i__ * 3 + 2 << 2) + 2] * mirreq[
		i__ - 1];
	d__[(i__ * 3 + 3 << 2) + 3] = d__[(i__ * 3 + 2 << 2) + 3] * mirreq[
		i__ - 1];
/* L10: */
	d__[(i__ * 3 + 3 << 2) + 4] = d__[(i__ * 3 + 2 << 2) + 4] * mirreq[
		i__ - 1];
    }
    if (*crd == 1) {
	invdp = invdpc_(fl, dimo, b0, dipl, &dtor);
    } else if (*crd == 0) {
	invdp = *invdip;
    } else {
	return 0;
    }
    rmlt = *mlt * dtor * (float)15.;
    rcolat = ((float)90. - invdp) * dtor;
    spharm_ik__(c__, &c__6, &c__6, &rcolat, &rmlt);
/*     21.3. - 20.6. */
    if (*ddd >= 79 && *ddd < 171) {
	seza = 1;
	sezb = 2;
	ddda = 79;
	dddb = 171;
	dddd = *ddd;
    }
/*     21.6. - 22.9. */
    if (*ddd >= 171 && *ddd < 265) {
	seza = 2;
	sezb = 4;
	ddda = 171;
	dddb = 265;
	dddd = *ddd;
    }
/*     23.9. - 20.12. */
    if (*ddd >= 265 && *ddd < 354) {
	seza = 4;
	sezb = 3;
	ddda = 265;
	dddb = 354;
	dddd = *ddd;
    }
/*     21.12. - 20.3. */
    if (*ddd >= 354 || *ddd < 79) {
	seza = 3;
	sezb = 1;
	ddda = 354;
	dddb = 444;
	dddd = *ddd;
	if (*ddd >= 354) {
	    dddd = *ddd;
	} else {
	    dddd = *ddd + 365;
	}
    }
    sezai = (seza - 1) % 3 + 1;
    sezbi = (sezb - 1) % 3 + 1;
/*     550km level */
    n0a550 = (float)0.;
    n0b550 = (float)0.;
    for (i__ = 1; i__ <= 49; ++i__) {
	n0a550 += c__[i__ - 1] * d__[(sezai + i__ * 3 << 2) + 1];
/* L30: */
	n0b550 += c__[i__ - 1] * d__[(sezbi + i__ * 3 << 2) + 1];
    }
    n550a = n0a550;
    n550b = n0b550;
    n550 = (n550b - n550a) / (dddb - ddda) * (dddd - ddda) + n550a;
/*     900km level */
    n0a900 = (float)0.;
    n0b900 = (float)0.;
    for (i__ = 1; i__ <= 49; ++i__) {
	n0a900 += c__[i__ - 1] * d__[(sezai + i__ * 3 << 2) + 2];
/* L70: */
	n0b900 += c__[i__ - 1] * d__[(sezbi + i__ * 3 << 2) + 2];
    }
    n900a = n0a900;
    n900b = n0b900;
    n900 = (n900b - n900a) / (dddb - ddda) * (dddd - ddda) + n900a;
/*     1500km level */
    n0a150 = (float)0.;
    n0b150 = (float)0.;
    for (i__ = 1; i__ <= 49; ++i__) {
	n0a150 += c__[i__ - 1] * d__[(sezai + i__ * 3 << 2) + 3];
/* L110: */
	n0b150 += c__[i__ - 1] * d__[(sezbi + i__ * 3 << 2) + 3];
    }
    n150a = n0a150;
    n150b = n0b150;
    n1500 = (n150b - n150a) / (dddb - ddda) * (dddd - ddda) + n150a;
/*     2500km level */
    n0a250 = (float)0.;
    n0b250 = (float)0.;
    for (i__ = 1; i__ <= 49; ++i__) {
	n0a250 += c__[i__ - 1] * d__[(sezai + i__ * 3 << 2) + 4];
/* L150: */
	n0b250 += c__[i__ - 1] * d__[(sezbi + i__ * 3 << 2) + 4];
    }
    n250a = n0a250;
    n250b = n0b250;
    n2500 = (n250b - n250a) / (dddb - ddda) * (dddd - ddda) + n250a;
/*      IF (ALT .LT. 900) NO=(N900-N550)/350.0*(ALT-550)+N550 */
/*      IF ((ALT .GE. 900) .AND. (ALT .LT. 1500)) */
/*     &  NO=(N1500-N900)/600.0*(ALT-900)+N900 */
/*      IF (ALT .GE. 1500) NO=(N2500-N1500)/1000.0*(ALT-1500)+N1500 */
    ano[0] = n550;
    ano[1] = n900;
    ano[2] = n1500;
    ano[3] = n2500;
    ah[0] = (float)550.;
    ah[1] = (float)900.;
    ah[2] = (float)1500.;
    ah[3] = (float)2250.;
    dno[0] = (float)20.;
    dno[1] = (float)20.;
    st1 = (ano[1] - ano[0]) / (ah[1] - ah[0]);
    for (i__ = 2; i__ <= 3; ++i__) {
	st2 = (ano[i__] - ano[i__ - 1]) / (ah[i__] - ah[i__ - 1]);
	ano[i__ - 1] -= (st2 - st1) * dno[i__ - 2] * log((float)2.);
/* L200: */
	st1 = st2;
    }
    for (i__ = 1; i__ <= 3; ++i__) {
/* L220: */
	st[i__ - 1] = (ano[i__] - ano[i__ - 1]) / (ah[i__] - ah[i__ - 1]);
    }
    argexp_1.argmax = (float)88.;
    sum = ano[0] + st[0] * (*alt - ah[0]);
    for (i__ = 1; i__ <= 2; ++i__) {
	aa = eptr_(alt, &dno[i__ - 1], &ah[i__]);
	bb = eptr_(ah, &dno[i__ - 1], &ah[i__]);
/* L230: */
	sum += (st[i__] - st[i__ - 1]) * (aa - bb) * dno[i__ - 1];
    }
    d__1 = (doublereal) sum;
    *nne = pow_dd(&c_b30, &d__1);
    return 0;
} /* nehigh_ */



/* ********************************************************** */
/* ***************** ELECTRON TEMPERATURE ******************** */
/* ********************************************************** */

/* Subroutine */ int elteik_(integer *crd, integer *f107y, integer *seasy, 
	real *invdip, real *fl, real *dimo, real *b0, real *dipl, real *mlt, 
	real *alt, integer *ddd, real *f107, real *te, real *sigte)
{
    /* Initialized data */

    static doublereal b[8] = { 1.259921,-.1984259,-.04686632,-.01314096,
	    -.00308824,8.2777e-4,-.00105877,.00183142 };
    static integer mirreq[81] = { 1,-1,1,-1,1,-1,1,-1,1,1,-1,1,-1,1,-1,1,-1,1,
	    -1,1,-1,1,-1,1,-1,1,-1,1,-1,1,-1,1,1,-1,1,-1,1,-1,1,1,-1,1,-1,1,
	    -1,1,-1,1,-1,1,-1,1,-1,1,-1,1,1,-1,1,-1,1,1,-1,1,-1,1,-1,1,-1,1,
	    -1,1,1,-1,1,1,-1,1,-1,1,1 };
    static struct {
	real e_1[8];
	integer fill_2[4];
	real e_3[8];
	integer fill_4[4];
	real e_5[8];
	integer fill_6[4];
	real e_7[8];
	integer fill_8[4];
	real e_9[8];
	integer fill_10[4];
	real e_11[8];
	integer fill_12[4];
	real e_13[8];
	integer fill_14[4];
	real e_15[8];
	integer fill_16[4];
	real e_17[8];
	integer fill_18[4];
	real e_19[8];
	integer fill_20[4];
	real e_21[8];
	integer fill_22[4];
	real e_23[8];
	integer fill_24[4];
	real e_25[8];
	integer fill_26[4];
	real e_27[8];
	integer fill_28[4];
	real e_29[8];
	integer fill_30[4];
	real e_31[8];
	integer fill_32[4];
	real e_33[8];
	integer fill_34[4];
	real e_35[8];
	integer fill_36[4];
	real e_37[8];
	integer fill_38[4];
	real e_39[8];
	integer fill_40[4];
	real e_41[8];
	integer fill_42[4];
	real e_43[8];
	integer fill_44[4];
	real e_45[8];
	integer fill_46[4];
	real e_47[8];
	integer fill_48[4];
	real e_49[8];
	integer fill_50[4];
	real e_51[8];
	integer fill_52[4];
	real e_53[8];
	integer fill_54[4];
	real e_55[8];
	integer fill_56[4];
	real e_57[8];
	integer fill_58[4];
	real e_59[8];
	integer fill_60[4];
	real e_61[8];
	integer fill_62[4];
	real e_63[8];
	integer fill_64[4];
	real e_65[8];
	integer fill_66[4];
	real e_67[8];
	integer fill_68[4];
	real e_69[8];
	integer fill_70[4];
	real e_71[8];
	integer fill_72[4];
	real e_73[8];
	integer fill_74[4];
	real e_75[8];
	integer fill_76[4];
	real e_77[8];
	integer fill_78[4];
	real e_79[8];
	integer fill_80[4];
	real e_81[8];
	integer fill_82[4];
	real e_83[8];
	integer fill_84[4];
	real e_85[8];
	integer fill_86[4];
	real e_87[8];
	integer fill_88[4];
	real e_89[8];
	integer fill_90[4];
	real e_91[8];
	integer fill_92[4];
	real e_93[8];
	integer fill_94[4];
	real e_95[8];
	integer fill_96[4];
	real e_97[8];
	integer fill_98[4];
	real e_99[8];
	integer fill_100[4];
	real e_101[8];
	integer fill_102[4];
	real e_103[8];
	integer fill_104[4];
	real e_105[8];
	integer fill_106[4];
	real e_107[8];
	integer fill_108[4];
	real e_109[8];
	integer fill_110[4];
	real e_111[8];
	integer fill_112[4];
	real e_113[8];
	integer fill_114[4];
	real e_115[8];
	integer fill_116[4];
	real e_117[8];
	integer fill_118[4];
	real e_119[8];
	integer fill_120[4];
	real e_121[8];
	integer fill_122[4];
	real e_123[8];
	integer fill_124[4];
	real e_125[8];
	integer fill_126[4];
	real e_127[8];
	integer fill_128[4];
	real e_129[8];
	integer fill_130[4];
	real e_131[8];
	integer fill_132[4];
	real e_133[8];
	integer fill_134[4];
	real e_135[8];
	integer fill_136[4];
	real e_137[8];
	integer fill_138[4];
	real e_139[8];
	integer fill_140[4];
	real e_141[8];
	integer fill_142[4];
	real e_143[8];
	integer fill_144[4];
	real e_145[8];
	integer fill_146[4];
	real e_147[8];
	integer fill_148[4];
	real e_149[8];
	integer fill_150[4];
	real e_151[8];
	integer fill_152[4];
	real e_153[8];
	integer fill_154[4];
	real e_155[8];
	integer fill_156[4];
	real e_157[8];
	integer fill_158[4];
	real e_159[8];
	integer fill_160[4];
	real e_161[8];
	integer fill_162[4];
	} equiv_192 = { (float)2118.5, (float)2625.3, (float)2922.8, (float)
		3373.8, (float)2073.2, (float)2596.4, (float)2962.1, (float)
		3289., {0}, (float)-.9096, (float)-1.4102, (float)-1.0193, (
		float)-.14579, (float)118.58, (float)273.69, (float)396.88, (
		float)234.06, {0}, (float)874.79, (float)1057.8, (float)
		500.52, (float)290.57, (float)703.41, (float)718.93, (float)
		296.52, (float)232.42, {0}, (float).53665, (float).60533, (
		float)1.9156, (float)-.89423, (float)406.06, (float)447.51, (
		float)277.82, (float)168.45, {0}, (float)72.315, (float)
		-201.22, (float)-88.798, (float)-178.18, (float)-154.1, (
		float)-389.08, (float)-135., (float)-379.64, {0}, (float)
		-1.4522, (float)-2.5531, (float)4.4598, (float)1.3821, (float)
		59.091, (float)6.4097, (float)-28.285, (float)-23.183, {0}, (
		float)-68.231, (float)-98.647, (float)22.407, (float)-89.134, 
		(float)-73.198, (float)-50.443, (float)18.036, (float)-67.725,
		 {0}, (float)-.17205, (float)1.5826, (float)2.305, (float)
		-.31758, (float)-100.67, (float)79.006, (float)-82.958, (
		float)98.597, {0}, (float)-95.868, (float)-140.76, (float)
		-3.8484, (float)-67.731, (float)-70.712, (float)-95.54, (
		float)-39.738, (float)28.565, {0}, (float)-537.4, (float)
		-917.64, (float)-1295.9, (float)-1437.2, (float)-633.23, (
		float)-981.62, (float)-1205.8, (float)-1370.5, {0}, (float)
		-.55762, (float).52004, (float)-.8733, (float).062781, (float)
		47.818, (float)9.4112, (float)68.198, (float)75.544, {0}, (
		float)-137., (float)-101.94, (float)27.027, (float)46.214, (
		float)-76.003, (float)-39.071, (float)22.164, (float)132.35, {
		0}, (float)-.6688, (float)-.53743, (float)-.60724, (float)
		-.040312, (float)-49.778, (float)-12.918, (float)-15.473, (
		float).19927, {0}, (float)-.64616, (float)46.185, (float)
		28.496, (float)-7.0553, (float)67.586, (float)60.87, (float)
		19.76, (float)-1.3429, {0}, (float)-.014436, (float).30271, (
		float)-.52927, (float)-.057943, (float)-7.3829, (float)23.039,
		 (float)13.793, (float)-.98459, {0}, (float)-13.574, (float)
		-16.661, (float)5.7042, (float)-15.921, (float)-7.0501, (
		float)-31.332, (float)-22.13, (float)-1.7632, {0}, (float)
		-.0051068, (float)-.43778, (float)-.25504, (float).07697, (
		float)13.663, (float)8.3557, (float)2.2585, (float)-4.2301, {
		0}, (float)191.76, (float)276.36, (float)-39.328, (float)
		-53.597, (float)126.61, (float)131., (float)-95.227, (float)
		-211.62, {0}, (float)-.35708, (float).099112, (float)1.8577, (
		float)-3.2922, (float)-49.014, (float)-32.576, (float)-79.347,
		 (float).55279, {0}, (float)-16.8, (float)-27.807, (float)
		25.325, (float)-57.514, (float)-44.884, (float)-96.546, (
		float)-18.617, (float)-95.367, {0}, (float).53899, (float)
		-.36236, (float)1.057, (float)-2.2513, (float)-27.004, (float)
		5.8694, (float)-14.626, (float)-12.788, {0}, (float)27.73, (
		float)92.38, (float)39.141, (float)27.316, (float)-5.4925, (
		float)71.925, (float)28.497, (float)40.782, {0}, (float)
		.21264, (float).55131, (float).012857, (float)-.49774, (float)
		-14.39, (float)10.059, (float)9.7753, (float)4.8734, {0}, (
		float)-1.1162, (float)-5.7407, (float)4.4854, (float)10.223, (
		float)14.573, (float)-9.7102, (float)5.1487, (float)-5.0869, {
		0}, (float)-.2102, (float).12985, (float)-1.0437, (float)
		.15541, (float)14.89, (float)-.18709, (float)-3.0464, (float)
		-4.8522, {0}, (float)-470.27, (float)-514.8, (float)-491.04, (
		float)-510.8, (float)-222.43, (float)-398.06, (float)-503.02, 
		(float)-525.66, {0}, (float)-.22692, (float)-.32325, (float)
		-.062939, (float).15422, (float)46.961, (float)-30.957, (
		float)32.524, (float)60.198, {0}, (float)-8.4973, (float)
		-42.135, (float)-41.205, (float)-26.147, (float)21.169, (
		float)-7.8514, (float)-6.2727, (float)-20.405, {0}, (float)
		-.12278, (float).04636, (float).3222, (float).4506, (float)
		.63799, (float)2.9864, (float)-2.7864, (float)30.107, {0}, (
		float)4.92, (float)-.47321, (float)-10.599, (float).77452, (
		float)-1.4838, (float)4.7169, (float)4.3374, (float)1.9692, {
		0}, (float)-.014939, (float)-.20887, (float)-.18242, (float)
		-.03268, (float)6.6321, (float)-6.3766, (float)-1.0944, (
		float)4.2044, {0}, (float)2.6356, (float)-2.3466, (float)
		.9116, (float)-.74846, (float).41318, (float)4.5467, (float)
		1.0615, (float)-1.6327, {0}, (float)-93.732, (float)-341.32, (
		float)-354.25, (float)-213.99, (float)-156.42, (float)-217.13,
		 (float)-149.55, (float)-111.21, {0}, (float).17524, (float)
		-.81632, (float)-.16068, (float)-1.0117, (float)113., (float)
		27.924, (float)4.5911, (float)-35.375, {0}, (float)-8.027, (
		float)20.252, (float)-1.3725, (float)15.173, (float)-8.9014, (
		float)6.9898, (float)13.417, (float)5.2049, {0}, (float)
		.048917, (float).31349, (float).096252, (float)-.72407, (
		float)-4.9933, (float)6.4391, (float)-3.8776, (float)15.725, {
		0}, (float)4.1404, (float)-3.2548, (float)6.6323, (float)
		3.2157, (float)7.3107, (float)-2.9495, (float)-.08603, (float)
		10.054, {0}, (float).21455, (float)-.02972, (float).10111, (
		float)-.09554, (float)9.4305, (float)1.3537, (float)4.9763, (
		float)3.7913, {0}, (float).65988, (float)-2.512, (float)
		1.5864, (float)2.0921, (float)-1.5203, (float)3.0092, (float)
		1.7831, (float)-.16254, {0}, (float)239.73, (float)312.81, (
		float)266.98, (float)338.03, (float)193.25, (float)246.62, (
		float)285.93, (float)149.83, {0}, (float).0067066, (float)
		-.044738, (float)-.13164, (float).32007, (float)-22.07, (
		float)-4.3575, (float)-41.944, (float)-40.229, {0}, (float)
		32.942, (float)4.413, (float)25.957, (float)-25.597, (float)
		-6.2284, (float)-8.7582, (float)2.543, (float)1.9216, {0}, (
		float).10491, (float)-.031125, (float).18661, (float)-.018768,
		 (float)-.10572, (float)-5.6902, (float)4.1294, (float)
		-2.5669, {0}, (float)-1.1019, (float)1.3784, (float).95289, (
		float)-4.3258, (float)-3.6677, (float)-1.844, (float)5.5434, (
		float)-1.3679, {0}, (float)-.0089674, (float).041569, (float)
		.12827, (float)-.094149, (float)-6.1689, (float)2.0276, (
		float)2.1086, (float)1.9556, {0}, (float)-258.71, (float)
		-109.8, (float)-20.802, (float)-112.74, (float)-68.242, (
		float)-154.56, (float)-79.921, (float)-52.991, {0}, (float)
		-.40511, (float).70001, (float)-.50001, (float)-.55848, (
		float)-29.507, (float)-4.1566, (float)8.916, (float)12.377, {
		0}, (float)18.509, (float)4.0861, (float)1.7802, (float)
		-19.836, (float)13.496, (float)1.594, (float)-14.931, (float)
		-18.352, {0}, (float)-.06559, (float)-.1667, (float)-.22686, (
		float)-.19519, (float)7.9801, (float)-2.261, (float)6.7306, (
		float)-.27843, {0}, (float)3.3369, (float)-4.852, (float)
		-.99243, (float)-2.95, (float)-1.7483, (float)-1.879, (float)
		-7.4849, (float)-2.6711, {0}, (float)-.014607, (float).036492,
		 (float)-.088453, (float)-.065675, (float).42264, (float)
		-.20589, (float)2.0152, (float)1.027, {0}, (float)193.45, (
		float)180.57, (float)169.38, (float)275.06, (float)132.32, (
		float)166.12, (float)244.35, (float)-17.185, {0}, (float)
		.45946, (float).20765, (float).15072, (float)-.094098, (float)
		9.753, (float)-2.8783, (float)-27.216, (float)-1.0283, {0}, (
		float)-17.988, (float)8.8382, (float)-1.2249, (float)18.588, (
		float)-3.3159, (float)-.097839, (float)-6.5424, (float)
		-13.286, {0}, (float).065776, (float)-.028354, (float)
		.0059212, (float)-.1546, (float)-3.8368, (float).71863, (
		float)-.22848, (float)1.4314, {0}, (float)-1.7171, (float)
		-1.634, (float)2.1008, (float).81344, (float).73997, (float)
		.224, (float).6678, (float)-.53584, {0}, (float)32.232, (
		float)174.92, (float)149.81, (float)184.79, (float)127.38, (
		float)89.569, (float)-5.021, (float)-67.809, {0}, (float)
		-.13975, (float)-.70915, (float)-.030983, (float)-.39938, (
		float).43677, (float)-8.7802, (float)-1.6876, (float)-9.6756, 
		{0}, (float)-7.9626, (float)9.6069, (float)-2.3113, (float)
		13.448, (float)-.43172, (float)-2.9563, (float)-3.8484, (
		float)-4.6022, {0}, (float)-.089984, (float).12205, (float)
		-.016773, (float).088669, (float)3.2624, (float)-1.5339, (
		float).48596, (float)1.8112, {0}, (float)-.52076, (float)
		.37218, (float).50314, (float)3.904, (float).24603, (float)
		-.69508, (float)1.2499, (float)-.0061062, {0}, (float)-67.466,
		 (float)-57.398, (float)-28.87, (float)-53.105, (float)
		-152.97, (float)-89.068, (float)-128.23, (float)26.33, {0}, (
		float)-.024359, (float)-.29121, (float)-.12562, (float).20753,
		 (float).3813, (float)2.4977, (float)-4.7155, (float)16.726, {
		0}, (float)1.2132, (float)-4.3088, (float)5.5983, (float)
		-3.5771, (float)-5.6753, (float).56978, (float)-8.9457, (
		float)3.9444, {0}, (float)-9.7418e-4, (float).056543, (float)
		.036513, (float)-.014386, (float)-.17841, (float)1.0112, (
		float)-.26414, (float)2.7303, {0}, (float)166.62, (float)
		60.417, (float)-32.056, (float)133.06, (float)14.078, (float)
		107.25, (float)-10.88, (float)47.634, {0}, (float).30197, (
		float).25141, (float)-.35823, (float).22388, (float)8.1371, (
		float)-8.8621, (float)17.04, (float)-32.729, {0}, (float)
		.3487, (float)2.2588, (float)-.71938, (float)-.43013, (float)
		-1.0298, (float)-4.8559, (float)-1.3495, (float)1.2863, {0}, (
		float).0020862, (float)-.065538, (float)-.030424, (float)
		.058347, (float)-.87611, (float)-.85244, (float).046478, (
		float)-.87681, {0}, (float)-100.91, (float)-160.84, (float)
		-59.756, (float)-72.119, (float)18.043, (float)-62.185, (
		float)-55.357, (float)5.8417, {0}, (float)-.11583, (float)
		.32163, (float)-.31999, (float).0015187, (float)-20.388, (
		float)7.2504, (float)29.895, (float)-11.842, {0}, (float)
		-.64353, (float)-3.5354, (float).38169, (float)2.1445, (float)
		3.5874, (float)1.8318, (float)-2.6035, (float)1.7747, {0}, (
		float)-45.914, (float)-39.077, (float)57.626, (float)1.5871, (
		float)-68.187, (float)-79.203, (float)-3.7697, (float)49.485, 
		{0}, (float)-.29075, (float)-.044285, (float).34086, (float)
		.34653, (float)12.396, (float)2.8472, (float)-5.3693, (float)
		-9.5141, {0}, (float)-1.3476, (float)-3.4426, (float)5.0895, (
		float)1.8858, (float)1.4384, (float)-.13483, (float).53346, (
		float)-1.7739, {0}, (float)-25.206, (float)26.798, (float)
		1.2152, (float)11.009, (float)-18.542, (float)21.818, (float)
		14.172, (float)28.452, {0}, (float).055027, (float).067368, (
		float).14523, (float)-.10913, (float)9.8107, (float)-4.2387, (
		float)5.3848, (float)-8.8851, {0}, (float)-106.63, (float)
		-67.479, (float)-60.723, (float).13126, (float)-1.7652, (
		float)-57.925, (float)-23.065, (float)-4.4171, {0}, (float)
		.13394, (float).098125, (float)-.015587, (float).16358, (
		float)-10.545, (float)2.5552, (float)-5.2616, (float)4.1421, {
		0}, (float)27.825, (float)70.079, (float)52.597, (float)
		33.089, (float)12.793, (float)46.6, (float)22.592, (float)
		-26.145, {0}, (float)33.973, (float)21.724, (float)-6.7261, (
		float)49.158, (float)24.878, (float)34.333, (float)-15.174, (
		float)-44.075 };

#define d__ ((real *)&equiv_192)

    static integer mf107[49] = { 1,-1,1,-1,1,-1,1,1,-1,1,-1,1,-1,1,-1,1,-1,1,
	    -1,1,-1,1,-1,1,1,-1,1,-1,1,1,-1,1,-1,1,-1,1,-1,1,-1,1,1,-1,1,1,-1,
	    1,-1,1,1 };
    static struct {
	real e_1[8];
	integer fill_2[4];
	real e_3[8];
	integer fill_4[4];
	real e_5[8];
	integer fill_6[4];
	real e_7[8];
	integer fill_8[4];
	real e_9[8];
	integer fill_10[4];
	real e_11[8];
	integer fill_12[4];
	real e_13[8];
	integer fill_14[4];
	real e_15[8];
	integer fill_16[4];
	real e_17[8];
	integer fill_18[4];
	real e_19[8];
	integer fill_20[4];
	real e_21[8];
	integer fill_22[4];
	real e_23[8];
	integer fill_24[4];
	real e_25[8];
	integer fill_26[4];
	real e_27[8];
	integer fill_28[4];
	real e_29[8];
	integer fill_30[4];
	real e_31[8];
	integer fill_32[4];
	real e_33[8];
	integer fill_34[4];
	real e_35[8];
	integer fill_36[4];
	real e_37[8];
	integer fill_38[4];
	real e_39[8];
	integer fill_40[4];
	real e_41[8];
	integer fill_42[4];
	real e_43[8];
	integer fill_44[4];
	real e_45[8];
	integer fill_46[4];
	real e_47[8];
	integer fill_48[4];
	real e_49[8];
	integer fill_50[4];
	real e_51[8];
	integer fill_52[4];
	real e_53[8];
	integer fill_54[4];
	real e_55[8];
	integer fill_56[4];
	real e_57[8];
	integer fill_58[4];
	real e_59[8];
	integer fill_60[4];
	real e_61[8];
	integer fill_62[4];
	real e_63[8];
	integer fill_64[4];
	real e_65[8];
	integer fill_66[4];
	real e_67[8];
	integer fill_68[4];
	real e_69[8];
	integer fill_70[4];
	real e_71[8];
	integer fill_72[4];
	real e_73[8];
	integer fill_74[4];
	real e_75[8];
	integer fill_76[4];
	real e_77[8];
	integer fill_78[4];
	real e_79[8];
	integer fill_80[4];
	real e_81[8];
	integer fill_82[4];
	real e_83[8];
	integer fill_84[4];
	real e_85[8];
	integer fill_86[4];
	real e_87[8];
	integer fill_88[4];
	real e_89[8];
	integer fill_90[4];
	real e_91[8];
	integer fill_92[4];
	real e_93[8];
	integer fill_94[4];
	real e_95[8];
	integer fill_96[4];
	real e_97[8];
	integer fill_98[4];
	} equiv_193 = { (float)2.1792, (float)3.3758, (float)1.478, (float)
		1.4513, (float)3.2236, (float)1.7678, (float)1.3888, (float)
		.86854, {0}, (float)-2.1275e-7, (float)-4.8705e-7, (float)
		-5.4689e-9, (float)-2.7903e-8, (float).7858, (float)-.12523, (
		float)1.1103, (float)-.099343, {0}, (float)-.30369, (float)
		-1.4844, (float)-.35681, (float)-2.1232, (float).040518, (
		float)-.031971, (float)1.6445, (float).017241, {0}, (float)
		-3.1861e-7, (float)-7.914e-7, (float)3.569e-7, (float)
		1.2079e-7, (float)-.078758, (float)-1.1885, (float).73508, (
		float).41379, {0}, (float)-.40109, (float)-1.201, (float)
		1.4442, (float).28524, (float)-.74039, (float)-.20748, (float)
		.099423, (float)1.0034, {0}, (float)-2.3529e-7, (float)
		-4.3759e-7, (float)-1.8771e-7, (float)-4.0135e-8, (float)
		-.55023, (float)-.62911, (float)-.12668, (float)-.71819, {0}, 
		(float)-.518, (float)-.61125, (float)-.58029, (float).68231, (
		float).22661, (float)-.69839, (float)1.1793, (float)-1.0619, {
		0}, (float)-.021268, (float)-2.4395, (float).63736, (float)
		.435, (float)-.17191, (float).14225, (float).32611, (float)
		-.44765, {0}, (float)6.5788e-11, (float)8.4646e-9, (float)
		8.5859e-8, (float)1.2651e-8, (float)-.45161, (float).029845, (
		float)-.13932, (float)-1.0826, {0}, (float).32767, (float)
		.50103, (float)1.7747, (float).56992, (float).077189, (float)
		-.074911, (float).34714, (float)-6.5641e-4, {0}, (float)
		-3.9204e-8, (float)1.5398e-9, (float)-1.896e-7, (float)
		-4.285e-8, (float).024208, (float).1808, (float)-.040398, (
		float)-.23379, {0}, (float).0060337, (float).1914, (float)
		-.0096585, (float).15039, (float).075019, (float)-.0013199, (
		float).28188, (float)-.41201, {0}, (float)1.8103e-9, (float)
		4.5214e-8, (float)-7.3912e-9, (float)-1.3229e-8, (float)
		.0054542, (float)-.11076, (float)-.2184, (float).082089, {0}, 
		(float)-.031331, (float)2.2253, (float)-.67418, (float).09429,
		 (float)1.8923, (float)-.0055801, (float).30676, (float)
		2.2647, {0}, (float)1.3981e-8, (float)-7.6345e-8, (float)
		-2.0098e-8, (float)-2.4997e-8, (float)-.86309, (float)-.63437,
		 (float).16201, (float)-.10331, {0}, (float).48055, (float)
		-.95472, (float)-.23251, (float)-.77275, (float)-.073536, (
		float).19093, (float)-.14507, (float).13969, {0}, (float)
		-3.5571e-8, (float)6.7707e-8, (float)4.3772e-8, (float)
		5.429e-8, (float)-.1541, (float).12925, (float).33787, (float)
		.34386, {0}, (float).09851, (float).0053013, (float).086224, (
		float)-.12859, (float)-.12294, (float).08625, (float)-.04693, 
		(float)-.14873, {0}, (float)3.0118e-9, (float)-5.0649e-8, (
		float)1.5699e-8, (float)9.4636e-9, (float)-.11886, (float)
		.17273, (float).054924, (float).048438, {0}, (float)-.077406, 
		(float)-.58881, (float).30123, (float).52854, (float)-2.0559, 
		(float)-.1501, (float).39874, (float)-.22364, {0}, (float)
		1.4673e-8, (float)1.1959e-8, (float)-2.8897e-8, (float)
		-2.1479e-9, (float)-.04364, (float)-.12204, (float).06987, (
		float).0033408, {0}, (float)-.11584, (float).031184, (float)
		-.10286, (float).092855, (float)-.22578, (float).047007, (
		float).023651, (float)-.057226, {0}, (float)2.6239e-8, (float)
		-2.4972e-9, (float)7.3823e-9, (float)-2.7594e-8, (float)
		.0063167, (float).058541, (float)-.10188, (float)-.087379, {0}
		, (float)-.037815, (float)-.0044759, (float).035984, (float)
		.0074358, (float).058175, (float).0069108, (float).031284, (
		float).10073, {0}, (float)-.75343, (float)-1.9022, (float)
		.99977, (float)-.45033, (float)-.55051, (float).33109, (float)
		1.0411, (float).16683, {0}, (float)-4.5728e-9, (float)
		3.8763e-9, (float)-2.5975e-8, (float)-7.4883e-10, (float)
		-.30664, (float).1405, (float)-.41114, (float)-.13735, {0}, (
		float)-.22223, (float)-.10882, (float).022399, (float)
		-.039305, (float)-.049306, (float).08014, (float)-.10548, (
		float).019385, {0}, (float)-1.0071e-9, (float)3.0299e-8, (
		float)-1.5372e-8, (float)2.2454e-8, (float).052511, (float)
		-.034096, (float)-.023342, (float)-.027082, {0}, (float)
		-.068871, (float)-.034453, (float).029522, (float)-.044181, (
		float).014045, (float)-.0083293, (float).0091528, (float)
		.031018, {0}, (float)-.088247, (float).13296, (float).11931, (
		float).053266, (float).22313, (float)-.10468, (float).092067, 
		(float).067087, {0}, (float)6.3637e-9, (float)3.1168e-8, (
		float)-4.4525e-9, (float)8.3007e-9, (float).19961, (float)
		.1078, (float)-.10666, (float)-.0079098, {0}, (float).037528, 
		(float).014878, (float).041555, (float)-.018869, (float)
		-.0089555, (float)-.023355, (float).026649, (float).047972, {
		0}, (float)-1.4578e-9, (float)-8.3137e-9, (float)5.0431e-9, (
		float)2.7783e-10, (float)-.04083, (float).021534, (float)
		-.037639, (float)-.043783, {0}, (float).086601, (float)
		-.47944, (float).066187, (float)-.052278, (float)-.74402, (
		float).05951, (float)-.18572, (float).31339, {0}, (float)
		4.082e-8, (float)-2.8746e-8, (float)1.3181e-8, (float)
		2.5128e-8, (float)-.03452, (float).0043653, (float)-.066661, (
		float).041214, {0}, (float).13803, (float).035825, (float)
		.061906, (float).056681, (float)-.11823, (float)-.022744, (
		float).094229, (float).0081151, {0}, (float)6.0607e-9, (float)
		-1.2058e-8, (float)-8.3984e-10, (float)3.031e-9, (float)
		-.04151, (float).042113, (float).03184, (float)-.027117, {0}, 
		(float)-.028557, (float).30506, (float).058869, (float)
		-.11503, (float).12895, (float)-.078295, (float).0058198, (
		float).031667, {0}, (float)-4.8644e-9, (float)-9.5413e-9, (
		float)-9.2211e-9, (float)3.5058e-8, (float).038704, (float)
		-.042225, (float).0048141, (float)-.013346, {0}, (float)
		.0015314, (float).0071259, (float)-.0078035, (float).020314, (
		float).0053682, (float)-.0019368, (float).0094962, (float)
		.0082864, {0}, (float)-.080197, (float).0070469, (float)
		-.074955, (float).044499, (float).079895, (float)-.093049, (
		float)-.11103, (float).20433, {0}, (float)-1.5931e-8, (float)
		-2.3296e-9, (float)3.8595e-8, (float)1.0183e-9, (float).12443,
		 (float)-.0097186, (float)-.02174, (float).0090147, {0}, (
		float)-.016002, (float)-.0088034, (float).02126, (float)
		.016406, (float).0025079, (float)-.015204, (float)-7.2465e-4, 
		(float).014602, {0}, (float)-.029124, (float)-.028111, (float)
		.035597, (float)6.9291e-4, (float)-.038743, (float)-.0051251, 
		(float)-.033437, (float).056125, {0}, (float)4.8067e-9, (
		float)-2.4131e-9, (float)5.3006e-9, (float)6.8229e-9, (float)
		-.021658, (float)-.015143, (float)-.016689, (float)-.0052173, 
		{0}, (float).0090446, (float).0056115, (float).040952, (float)
		-.027379, (float).068794, (float)-.01367, (float)-.076956, (
		float).028963, {0}, (float)1.2273e-8, (float)-4.8153e-9, (
		float)2.2709e-8, (float)5.845e-9, (float).036839, (float)
		-.016588, (float)-.010285, (float)-.012959, {0}, (float)
		-.030496, (float)-6.945e-5, (float).018248, (float)-.014324, (
		float)-.041913, (float)-.011932, (float)-.057708, (float)
		.031247, {0}, (float).0078786, (float).011739, (float)
		-.017679, (float)-.014532, (float)-.0035795, (float)-.027587, 
		(float)-.1031, (float)-.035943 };

#define fa ((real *)&equiv_193)

    static struct {
	real e_1[8];
	integer fill_2[4];
	real e_3[8];
	integer fill_4[4];
	real e_5[8];
	integer fill_6[4];
	real e_7[8];
	integer fill_8[4];
	real e_9[8];
	integer fill_10[4];
	real e_11[8];
	integer fill_12[4];
	real e_13[8];
	integer fill_14[4];
	real e_15[8];
	integer fill_16[4];
	real e_17[8];
	integer fill_18[4];
	real e_19[8];
	integer fill_20[4];
	real e_21[8];
	integer fill_22[4];
	real e_23[8];
	integer fill_24[4];
	real e_25[8];
	integer fill_26[4];
	real e_27[8];
	integer fill_28[4];
	real e_29[8];
	integer fill_30[4];
	real e_31[8];
	integer fill_32[4];
	real e_33[8];
	integer fill_34[4];
	real e_35[8];
	integer fill_36[4];
	real e_37[8];
	integer fill_38[4];
	real e_39[8];
	integer fill_40[4];
	real e_41[8];
	integer fill_42[4];
	real e_43[8];
	integer fill_44[4];
	real e_45[8];
	integer fill_46[4];
	real e_47[8];
	integer fill_48[4];
	real e_49[8];
	integer fill_50[4];
	real e_51[8];
	integer fill_52[4];
	real e_53[8];
	integer fill_54[4];
	real e_55[8];
	integer fill_56[4];
	real e_57[8];
	integer fill_58[4];
	real e_59[8];
	integer fill_60[4];
	real e_61[8];
	integer fill_62[4];
	real e_63[8];
	integer fill_64[4];
	real e_65[8];
	integer fill_66[4];
	real e_67[8];
	integer fill_68[4];
	real e_69[8];
	integer fill_70[4];
	real e_71[8];
	integer fill_72[4];
	real e_73[8];
	integer fill_74[4];
	real e_75[8];
	integer fill_76[4];
	real e_77[8];
	integer fill_78[4];
	real e_79[8];
	integer fill_80[4];
	real e_81[8];
	integer fill_82[4];
	real e_83[8];
	integer fill_84[4];
	real e_85[8];
	integer fill_86[4];
	real e_87[8];
	integer fill_88[4];
	real e_89[8];
	integer fill_90[4];
	real e_91[8];
	integer fill_92[4];
	real e_93[8];
	integer fill_94[4];
	real e_95[8];
	integer fill_96[4];
	real e_97[8];
	integer fill_98[4];
	} equiv_194 = { (float)-419.34, (float)-672.22, (float)-252.47, (
		float)-421.57, (float)-626.95, (float)-399.11, (float)-280.05,
		 (float)-198.99, {0}, (float)7.5753e-5, (float)1.1078e-4, (
		float)-1.0611e-5, (float)-1.2843e-5, (float)-17.178, (float)
		59.796, (float)-98.261, (float)83.014, {0}, (float)300.92, (
		float)290.62, (float)35.396, (float)116.77, (float)114.05, (
		float)41.54, (float)-399.83, (float)-123.64, {0}, (float)
		1.1961e-4, (float)1.8966e-4, (float)-8.7667e-5, (float)
		-1.9182e-5, (float)171.27, (float)234.66, (float)-26.619, (
		float)-49.417, {0}, (float)168.97, (float)290.21, (float)
		-296.34, (float)6.6617, (float)137.52, (float)40.871, (float)
		8.2338, (float)-179.56, {0}, (float)8.2549e-5, (float)
		9.973e-5, (float)1.6567e-5, (float)-2.7718e-5, (float)136.41, 
		(float)145.29, (float)17.867, (float)184.31, {0}, (float)
		84.754, (float)148.05, (float)77.489, (float)-105.27, (float)
		-70.975, (float)121.04, (float)-177.85, (float)306.99, {0}, (
		float)-.074083, (float)490.25, (float)-117.99, (float)-17.184,
		 (float)14.633, (float)-46.233, (float)-101.22, (float)27.139,
		 {0}, (float)-1.8769e-6, (float)-1.4587e-5, (float)-1.362e-5, 
		(float)-3.4177e-7, (float)52.949, (float)11.266, (float)
		91.687, (float)212.87, {0}, (float)-87.576, (float)-62.774, (
		float)-312.33, (float)-72.191, (float)-28.16, (float)27.21, (
		float)-87.297, (float)23.659, {0}, (float)1.3779e-5, (float)
		-5.6489e-6, (float)3.1358e-5, (float)-1.1358e-7, (float)
		-30.614, (float)-36.695, (float)-2.8501, (float)42.878, {0}, (
		float)7.9487, (float)-23.959, (float)-10.114, (float)-18.024, 
		(float)-27.29, (float)-6.5125, (float)-39.83, (float)80.452, {
		0}, (float)-2.355e-6, (float)3.4145e-6, (float)-4.9641e-6, (
		float)-1.2578e-7, (float)1.8518, (float)19.678, (float)45.134,
		 (float)-28.811, {0}, (float)-71.043, (float)-492.72, (float)
		103.63, (float)-66.518, (float)-354.65, (float)-63.622, (
		float)-106.69, (float)-418.89, {0}, (float)-3.4455e-6, (float)
		1.593e-5, (float)4.2123e-6, (float)6.2438e-6, (float)178.95, (
		float)160.88, (float)-25.633, (float)11.639, {0}, (float)
		-75.485, (float)183.63, (float)27.824, (float)230.27, (float)
		20.545, (float)-41.296, (float)7.1774, (float)-4.1313, {0}, (
		float)1.3096e-5, (float)-9.9393e-6, (float)-9.7994e-6, (float)
		-1.7359e-5, (float)56.681, (float)-7.7296, (float)-74.302, (
		float)-75.002, {0}, (float)-4.7573, (float)-7.0072, (float)
		-19.069, (float)40.8, (float)34.224, (float)-20.603, (float)
		15.876, (float)34.022, {0}, (float)1.9409e-6, (float)
		4.4609e-6, (float)-2.8885e-6, (float)2.1467e-6, (float)26.772,
		 (float)-37.075, (float)-1.7359, (float)-22.607, {0}, (float)
		75.447, (float)86.865, (float)-74.787, (float)-137.15, (float)
		371.67, (float)24.707, (float)-91.679, (float)18.021, {0}, (
		float)-2.3927e-6, (float)-2.9546e-6, (float)7.6758e-6, (float)
		-8.5554e-7, (float)9.1041, (float)32.399, (float)-19.696, (
		float)8.1737, {0}, (float)19.243, (float)-3.1763, (float)
		10.608, (float)-17.205, (float)21.472, (float)-6.7277, (float)
		3.0117, (float)14.083, {0}, (float)2.423e-6, (float)
		-1.2566e-7, (float)-6.8638e-7, (float)5.0757e-6, (float)
		-.43844, (float)-6.0272, (float)20.649, (float)19.836, {0}, (
		float)3.6926, (float)-.58817, (float)-11.534, (float).22927, (
		float)-12.918, (float)-3.463, (float)-9.3364, (float)-18.668, 
		{0}, (float)159.66, (float)396.87, (float)-183.12, (float)
		80.476, (float)81.502, (float)-55.263, (float)-226.7, (float)
		-62.033, {0}, (float)6.5501e-6, (float)-6.5285e-6, (float)
		1.1865e-6, (float)-2.7589e-6, (float)38.268, (float)-25.945, (
		float)97.904, (float)10.904, {0}, (float)43.22, (float)10.005,
		 (float)-9.6891, (float)3.3827, (float)1.0318, (float)-14.906,
		 (float)15.046, (float)-1.3, {0}, (float)-2.1216e-6, (float)
		-2.5441e-6, (float)3.8221e-6, (float)-4.066e-6, (float)
		-7.3473, (float)4.1722, (float)-3.3399, (float)3.8715, {0}, (
		float)14.776, (float)4.6315, (float)-7.1519, (float)13.065, (
		float)-5.108, (float)3.4384, (float)-.87702, (float)-7.8479, {
		0}, (float)25.894, (float)-27.349, (float)-27.741, (float)
		-2.0229, (float)-46.51, (float)22.157, (float)-12.437, (float)
		21.208, {0}, (float)-8.0222e-9, (float)5.5261e-6, (float)
		-5.9749e-6, (float)2.192e-6, (float)-30.223, (float)-20.458, (
		float)22.864, (float)-5.9507, {0}, (float)-6.4715, (float)
		-1.1131, (float)-9.3219, (float)10.875, (float)5.3605, (float)
		2.8784, (float)-3.6244, (float)-13.782, {0}, (float)
		-3.7197e-7, (float)1.9219e-6, (float)1.9756e-6, (float)
		-6.1136e-7, (float)9.3017, (float)-4.4228, (float)6.7245, (
		float)7.2326, {0}, (float)11.891, (float)85.274, (float)
		-16.897, (float)7.201, (float)127.1, (float)19.416, (float)
		16.524, (float)-53.504, {0}, (float)-9.2441e-6, (float)
		-3.1671e-6, (float)-3.0522e-7, (float)-5.4154e-6, (float)
		4.5835, (float)-15.663, (float)23.854, (float)-5.7353, {0}, (
		float)-28.838, (float)-9.8191, (float)-8.7396, (float)-8.7107,
		 (float)25.091, (float)6.5339, (float)-23.351, (float)-1.3774,
		 {0}, (float)2.6401e-6, (float)7.434e-7, (float)1.0363e-7, (
		float)-4.2033e-7, (float)6.7618, (float)-7.9988, (float)
		-7.1407, (float)6.5094, {0}, (float)-.83915, (float)-67.348, (
		float)-21.515, (float)24.655, (float)-24.027, (float)14.137, (
		float)-1.8966, (float)-7.5963, {0}, (float)-6.8336e-7, (float)
		-1.2024e-6, (float)1.1544e-6, (float)-7.7856e-6, (float)
		-8.2618, (float)1.6251, (float)3.3737, (float)3.8696, {0}, (
		float).41298, (float)-.53796, (float)1.3571, (float)-4.6298, (
		float).081357, (float).6489, (float)-3.0866, (float)-.2007, {
		0}, (float)21.763, (float)-14.772, (float)15.544, (float)
		-16.797, (float)-18.817, (float)4.8511, (float)9.1835, (float)
		-38.873, {0}, (float)-1.2271e-6, (float)8.2916e-6, (float)
		-3.6035e-6, (float)1.357e-6, (float)-20.141, (float)2.1505, (
		float)1.9951, (float)-2.7285, {0}, (float)2.7614, (float)
		1.9457, (float)-2.9642, (float)-3.2055, (float)-.31208, (
		float)2.9129, (float)-.40653, (float)-2.3107, {0}, (float)
		8.317, (float)8.3258, (float)-8.9441, (float)-.54947, (float)
		1.8021, (float)-.67762, (float).19179, (float)-9.2363, {0}, (
		float)-1.7405e-6, (float)3.5652e-7, (float)-1.5169e-6, (float)
		1.2722e-6, (float)2.7531, (float).57748, (float)5.1555, (
		float)1.1762, {0}, (float).9756, (float)-3.424, (float)
		-8.5483, (float)8.1156, (float)-14.198, (float)3.7354, (float)
		8.6789, (float)-4.0574, {0}, (float)-1.4005e-6, (float)
		5.481e-7, (float)-1.6971e-6, (float)5.5588e-6, (float)-8.6175,
		 (float)1.0259, (float)2.9229, (float)3.3439, {0}, (float)
		3.5829, (float).29803, (float)-8.4912, (float)2.0961, (float)
		8.0561, (float)1.5804, (float)8.9559, (float)-7.0163, {0}, (
		float).75525, (float)-4.8309, (float)3.1723, (float)6.3166, (
		float)-2.6929, (float)1.5189, (float)21.825, (float)10.312 };

#define fb ((real *)&equiv_194)

    static real sz[400]	/* was [4][4][25] */ = { (float)-22.484,(float)11.725,
	    (float)48.197,(float)162.89,(float)81.616,(float)66.929,(float)
	    66.565,(float)54.078,(float)33.939,(float)84.35,(float).33717,(
	    float)116.24,(float)-32.521,(float)80.799,(float)-16.095,(float)
	    200.01,(float)15.678,(float)-45.645,(float)15.085,(float)197.16,(
	    float)-78.931,(float)-63.062,(float)-78.329,(float)102.57,(float)
	    41.603,(float)10.359,(float)115.54,(float)262.77,(float)77.815,(
	    float)-109.34,(float)99.417,(float)95.915,(float)-197.82,(float)
	    4.5733,(float)-25.638,(float)271.76,(float)-148.77,(float)-31.518,
	    (float)26.828,(float)32.942,(float)63.171,(float)35.955,(float)
	    45.506,(float)128.54,(float)-99.172,(float)54.753,(float)-14.653,(
	    float)162.46,(float)181.37,(float)18.517,(float)11.104,(float)
	    120.47,(float)-21.824,(float)39.008,(float)-30.595,(float)-27.484,
	    (float)28.139,(float)-53.957,(float)-.75635,(float)-44.443,(float)
	    117.71,(float)23.566,(float)-46.748,(float)29.653,(float)-118.41,(
	    float)-23.715,(float)-19.587,(float)-33.004,(float)-22.731,(float)
	    2.2092,(float)18.69,(float)-42.371,(float)-12.545,(float)-55.573,(
	    float)-18.256,(float)-104.11,(float)-49.189,(float)-64.691,(float)
	    -3.1115,(float)-6.3464,(float)-46.499,(float)-15.863,(float)
	    48.883,(float)-152.13,(float)-44.445,(float)-11.085,(float)9.4627,
	    (float)38.394,(float)26.427,(float)62.484,(float)-6.5165,(float)
	    -69.986,(float)42.987,(float)1.1268,(float)-50.367,(float)-28.476,
	    (float)-80.932,(float)25.084,(float)12.754,(float)-63.033,(float)
	    -1.3629,(float)-28.826,(float)-29.049,(float)-21.161,(float)
	    -59.703,(float)-15.365,(float)35.243,(float)-89.133,(float)6.1923,
	    (float)6.9659,(float)-5.526,(float)-90.499,(float)28.705,(float)
	    -8.8654,(float)-7.4101,(float)-23.628,(float)-5.1986,(float)
	    11.331,(float)29.074,(float)4.3066,(float)-5.234,(float)5.6723,(
	    float)-41.46,(float)-20.989,(float)19.022,(float)-11.639,(float)
	    3.1643,(float)-6.396,(float)-15.15,(float)-4.2437,(float)-17.884,(
	    float)1.525,(float)9.5949,(float)-3.195,(float)2.4002,(float)
	    -27.098,(float)-14.02,(float)-6.6484,(float)4.8214,(float)-7.994,(
	    float)-1.4961,(float)-9.282,(float).18895,(float)-1.9869,(float)
	    40.453,(float)22.447,(float)-9.7165,(float)-28.945,(float)113.26,(
	    float)29.916,(float)-8.8067,(float)105.62,(float)-.63531,(float)
	    78.126,(float)31.778,(float)-47.922,(float)56.301,(float)111.94,(
	    float)-42.743,(float)33.675,(float)-130.51,(float)20.916,(float)
	    38.258,(float)-84.696,(float)-29.675,(float)-4.7622,(float)31.262,
	    (float)-46.081,(float)36.48,(float)49.952,(float)52.812,(float)
	    11.982,(float)33.056,(float)-87.659,(float)15.466,(float)82.809,(
	    float)24.204,(float)-25.716,(float)3.6539,(float)-16.515,(float)
	    -41.54,(float)-14.363,(float)-.36726,(float)18.271,(float)3.6464,(
	    float)-2.9937,(float)-13.524,(float)13.529,(float)-3.4998,(float)
	    46.876,(float)22.751,(float)-17.643,(float)-8.7547,(float)-6.0094,
	    (float)-4.929,(float)-1.2776,(float)-8.2913,(float)-4.0448,(float)
	    9.7701,(float)-15.616,(float)5.6044,(float)7.2634,(float)-7.4173,(
	    float)7.9148,(float)25.603,(float)-20.595,(float)23.019,(float)
	    .93427,(float)16.474,(float)21.711,(float)107.93,(float)4.2591,(
	    float)-75.447,(float)-14.279,(float)32.131,(float)6.8675,(float)
	    19.248,(float)-10.773,(float)-39.91,(float)22.318,(float)-41.21,(
	    float)-41.123,(float)-12.153,(float)15.153,(float).92847,(float)
	    -12.222,(float)9.0909,(float)8.2797,(float)-2.8678,(float)-9.1323,
	    (float)-.86902,(float)-.11678,(float)-1.171,(float)2.0048,(float)
	    8.4328,(float)37.615,(float)-.59928,(float)24.208,(float)-9.4305,(
	    float)7.0991,(float)5.0067,(float).14226,(float)3.3659,(float)
	    -8.3759,(float)22.539,(float)1.6311,(float)-1.3888,(float)2.9773,(
	    float)4.0883,(float)-.2907,(float)-.86133,(float)7.5449,(float)
	    -6.517,(float)-15.497,(float)7.7048,(float)9.2573,(float)-16.276,(
	    float)5.0453,(float)-8.4915,(float)55.75,(float)-43.198,(float)
	    1.0317,(float)38.983,(float)6.5244,(float)30.011,(float)-17.277,(
	    float)-14.775,(float)-13.719,(float)-30.348,(float)-45.482,(float)
	    -10.472,(float)-29.729,(float)-17.811,(float)-5.2144,(float)
	    46.583,(float)18.242,(float)-6.1286,(float)-14.682,(float)-15.296,
	    (float)9.0964,(float)-4.8281,(float)-4.1386,(float)4.1764,(float)
	    -12.865,(float)-.29433,(float)13.202,(float)-10.872,(float)
	    -21.289,(float)3.3205,(float)12.274,(float)6.6691,(float)-2.7894,(
	    float)10.387,(float)3.6067,(float)-6.0608,(float)4.4221,(float)
	    -3.2089,(float)-1.66,(float)-.77575,(float)-7.4193,(float)-2.9917,
	    (float)-14.486,(float)4.7621,(float)-4.9093,(float)1.5727,(float)
	    -2.9295,(float)26.471,(float)27.438,(float)23.967,(float)-.96292,(
	    float)16.429,(float)1.4783,(float)9.7707,(float)22.237,(float)
	    6.3293,(float)-15.604,(float)14.373,(float)-2.7499,(float)-1.5255,
	    (float)-1.4336,(float)4.4872,(float).053619,(float)1.6831,(float)
	    -2.13,(float)4.4365,(float)3.7949,(float)-5.603,(float).45843,(
	    float)-1.4347,(float)-3.153,(float)1.2135,(float)-11.663,(float)
	    -1.0131,(float)-5.8915,(float)-1.0996,(float)-5.041,(float)11.533,
	    (float)5.8705,(float)-11.973,(float)-4.2789,(float)-10.083,(float)
	    3.1022,(float)20.101,(float)-12.944,(float)-3.2377,(float)-14.685,
	    (float)-7.7471,(float)-2.1478,(float)-1.8499,(float)-.090391,(
	    float)-2.404,(float)-6.0288,(float)-2.8452,(float)1.2787,(float)
	    7.0796,(float)-2.6639,(float)-6.7061,(float)-.46328,(float)
	    -7.1151,(float)2.9291,(float)-3.2303,(float)1.1209,(float)3.7529,(
	    float)4.4852,(float)-2.0904,(float)-.28128,(float)1.1287,(float)
	    -10.345,(float)8.4677,(float)3.4613,(float)5.3906,(float)4.3236,(
	    float)1.4691,(float)9.0868,(float)5.1599,(float)-3.6895,(float)
	    8.3354,(float)4.0532,(float)-8.6309,(float)-12.419,(float)1.1991,(
	    float)1.9866,(float)-6.4519,(float)6.9714,(float)10.316,(float)
	    2.0811,(float).26667,(float)-2.3532,(float)3.1579,(float)4.5608,(
	    float)4.3186,(float)-5.6245,(float)2.8745,(float)1.5677,(float)
	    -7.8509,(float)-7.583,(float).12403,(float)1.386,(float)-.24907,(
	    float)-2.693 };

    /* System generated locals */
    real r__1;
    doublereal d__1, d__2, d__3, d__4, d__5, d__6, d__7, d__8;

    /* Builtin functions */
    double pow_dd(doublereal *, doublereal *), sqrt(doublereal), acos(
	    doublereal), sin(doublereal), cos(doublereal), r_sign(real *, 
	    real *);

    /* Local variables */
    static real alfa, t0a150, beta, t0b150, t0a900, t0a550, t0b550, t1a550, 
	    t1b550, t2a550, t2b550, t3a550, t3b550, t0b900, t1a900, t1b900, 
	    t2a900, t2b900, t3a900, t3b900, t1a150, t1b150, t2a150;
    static integer seza, sezb;
    static real t2b150, t3a150, t3b150, t0a250, dtor, invl, t0b250, t1a250, 
	    t1b250, t2a250, rmlt, t2b250, t3a250, t3b250;
    static doublereal a;
    static real c__[82];
    static integer i__;
    static real rdipl;
    static integer sezai, sezbi;
    static real invdp, rinvl;
    extern /* Subroutine */ int spharm_ik__(real *, integer *, integer *, 
	    real *, real *);
    static real rcolat, t900, t550, asa, t1500, t2500, csz[25], cf107[49], 
	    t150a, t150b, t900a, t550a, t550b, t900b, t250a, t250b;
    static integer ddda, dddb, dddd;

/* Version 2000 (released 30.12.1999) */
/* Empirical model of electron temperature (Te) in the outer ionosphere */
/* for high solar activity conditions (F107 >= 100). */
/* Based on spherical harmonics approximation of measured */
/* Te (by IK19, IK24, and IK25 satellites) at altitudes centred on 550km, */
/* 900km, 1500km, and 2500km. For intermediate altitudes a linear */
/* interpolation is used. Recommended altitude range: 500-3000 km!!! */
/* Linear extrapolation is used for altitude ranges <500;550)km */
/* and (2500;3000> km. For days between seasons centred at */
/* (21.3. = 79; 21.6. = 171; 23.9. 265; 21.12. = 354) Te is */
/* linearly interpolated. */
/* Inputs: CRD - 0 .. INVDIP */
/*               1 .. FL, DIMO, B0, DIPL (used for calculation INVDIP */
/* 						inside) */
/*         F107Y - 0 .. F107 correction NOT included */
/*                 1 .. F107 correction included */
/*         SEASY - 0 .. seasonal correction NOT included */
/*                 1 .. seasonal correction included */
/*         INVDIP - "mix" coordinate of the dip latitude and of */
/*                    the invariant latitude; */
/*                    positive northward, in deg, range <-90.0;90.0> */
/*         FL, DIMO, BO - McIlwain L parameter, dipole moment in */
/*                        Gauss, magnetic field strength in Gauss - */
/*                        parameters needed for invariant latitude */
/*                        calculation */
/*         DIPL - dip latitude */
/*                positive northward, in deg, range <-90.0;90.0> */
/*         MLT - magnetic local time (central dipole) */
/*               in hours, range <0;24) */
/*         ALT - altitude above the Earth's surface; */
/*               in km, range <500;3000> */
/*         DDD - day of year; range <0;365> */
/*         F107 - daily solar radio flux; */
/*                high solar activity; range <100;250> */
/* Output: TE - electron temperature in K */
/*         SIGTE - standard deviation of TE in K */
/*                 (not yet included) */
/* Versions: 1.00 (IDL) the first version Te=Te(invl,mlt,alt,season) */
/*           1.50 (IDL) corrected IK19 Te at 900km for possible */
/*                      Ne > 2E11 m-3 */
/*           2.00 (IDL) F107 included as a linear perturbation on global */
/* 			Te pattern */
/*                      Te=Te(invlat,mlt,alt,season,F107) */
/*           3.00 (IDL) invdipl introduced */
/*           2000 (IDL,FORTRAN) correction for seasons included */
/* Authors of the model (Te measurements (1), data processing (2), model */
/*                       formulation and building (3)): */
/*                V. Truhlik (2,3), L. Triskova (2,3), J. Smilauer (1,2), */
/*                          (Institute of Atm. Phys., Prague) */
/*                                 and V.V. Afonin (1) */
/*                                      (IKI, Moscow) */
/* Author of the code: */
/*         Vladimir Truhlik */
/*         Institute of Atm. Phys. */
/*         Bocni II. */
/*         141 31 Praha 4, Sporilov */
/*         Czech Republic */
/*         e-mail: vtr@ufa.cas.cz */
/*         tel/fax: +420 67103058, +420 602 273111 / +420 72 762528 */
/* //////////////////////coefficients - main model part/////////////// */
/*     550km equinox */
/*     550km June solstice */
/*     900km equinox */
/*     900km June solstice */
/*     1500km equinox */
/*     1500km June solstice */
/*     2500km equinox */
/*     2500km June solstice */
/* ////////////coefficients - F107 correction////////////////////// */
/*     550km equinox */
/*     550km June solstice */
/*     900km equinox */
/*     900km June solstice */
/*     1500km equinox */
/*     1500km June solstice */
/*     2500km equinox */
/*     2500km June solstice */
/* ///////////coefficients - seasonal correction//////////////// */
/*     550km March equinox */
/*     550km June solstice */
/*     550km December solstice */
/*     550km September equinox */
/*     900km March equinox */
/*     900km June solstice */
/*     900km December solstice */
/*     900km September equinox */
/*     1500km March equinox */
/*     1500km June solstice */
/*     1500km December solstice */
/*     1500km September equinox */
/*     2500km March equinox */
/*     2500km June solstice */
/*     2500km December solstice */
/*     2500km September equinox */
/* ////////////////////////////////////////////////////////////// */
    dtor = (float).01745329252;
/*     coefficients for mirroring */
    for (i__ = 1; i__ <= 81; ++i__) {
	d__[(i__ * 3 + 3 << 2) - 16] = d__[(i__ * 3 + 2 << 2) - 16] * mirreq[
		i__ - 1];
	d__[(i__ * 3 + 3 << 2) - 15] = d__[(i__ * 3 + 2 << 2) - 15] * mirreq[
		i__ - 1];
	d__[(i__ * 3 + 3 << 2) - 14] = d__[(i__ * 3 + 2 << 2) - 14] * mirreq[
		i__ - 1];
/* L10: */
	d__[(i__ * 3 + 3 << 2) - 13] = d__[(i__ * 3 + 2 << 2) - 13] * mirreq[
		i__ - 1];
    }
    for (i__ = 1; i__ <= 49; ++i__) {
	fa[(i__ * 3 + 3 << 2) - 16] = fa[(i__ * 3 + 2 << 2) - 16] * mf107[i__ 
		- 1];
	fb[(i__ * 3 + 3 << 2) - 16] = fb[(i__ * 3 + 2 << 2) - 16] * mf107[i__ 
		- 1];
	fa[(i__ * 3 + 3 << 2) - 15] = fa[(i__ * 3 + 2 << 2) - 15] * mf107[i__ 
		- 1];
	fb[(i__ * 3 + 3 << 2) - 15] = fb[(i__ * 3 + 2 << 2) - 15] * mf107[i__ 
		- 1];
	fa[(i__ * 3 + 3 << 2) - 14] = fa[(i__ * 3 + 2 << 2) - 14] * mf107[i__ 
		- 1];
	fb[(i__ * 3 + 3 << 2) - 14] = fb[(i__ * 3 + 2 << 2) - 14] * mf107[i__ 
		- 1];
	fa[(i__ * 3 + 3 << 2) - 13] = fa[(i__ * 3 + 2 << 2) - 13] * mf107[i__ 
		- 1];
/* L20: */
	fb[(i__ * 3 + 3 << 2) - 13] = fb[(i__ * 3 + 2 << 2) - 13] * mf107[i__ 
		- 1];
    }
    if (*crd == 1) {

/* calculation of INVDIP from FL, DIMO, BO, and DIPL invariant */
/* latitude calculated by highly accurate polynomial expansion */

	d__1 = (doublereal) (*dimo / *b0);
	a = pow_dd(&d__1, &c_b55) / *fl;
/* Computing 2nd power */
	d__1 = a;
/* Computing 3rd power */
	d__2 = a;
/* Computing 4th power */
	d__3 = a, d__3 *= d__3;
/* Computing 5th power */
	d__4 = a, d__5 = d__4, d__4 *= d__4;
/* Computing 6th power */
	d__6 = a, d__6 *= d__6;
/* Computing 7th power */
	d__7 = a, d__8 = d__7, d__7 *= d__7, d__8 *= d__7;
	asa = a * (b[0] + b[1] * a + b[2] * (d__1 * d__1) + b[3] * (d__2 * (
		d__2 * d__2)) + b[4] * (d__3 * d__3) + b[5] * (d__5 * (d__4 * 
		d__4)) + b[6] * (d__6 * (d__6 * d__6)) + b[7] * (d__8 * (d__7 
		* d__7)));
	if (asa > (float)1.) {
	    asa = (float)1.;
	}
/*      invariant latitude (absolute value) */
	rinvl = acos(sqrt(asa));
	invl = rinvl / dtor;
	rdipl = *dipl * dtor;
/* Computing 3rd power */
	r__1 = sin((dabs(rdipl)));
	alfa = r__1 * (r__1 * r__1);
/* Computing 3rd power */
	r__1 = cos(rinvl);
	beta = r__1 * (r__1 * r__1);
	invdp = (alfa * r_sign(&c_b18, dipl) * invl + beta * *dipl) / (alfa + 
		beta);
    } else if (*crd == 0) {
	invdp = *invdip;
    } else {
	return 0;
    }
    rmlt = *mlt * dtor * (float)15.;
    rcolat = ((float)90. - invdp) * dtor;
    spharm_ik__(c__, &c__8, &c__8, &rcolat, &rmlt);
    spharm_ik__(cf107, &c__6, &c__6, &rcolat, &rmlt);
    spharm_ik__(csz, &c__4, &c__4, &rcolat, &rmlt);
/*     21.3. - 20.6. */
    if (*ddd >= 79 && *ddd < 171) {
	seza = 1;
	sezb = 2;
	ddda = 79;
	dddb = 171;
	dddd = *ddd;
    }
/*     21.6. - 22.9. */
    if (*ddd >= 171 && *ddd < 265) {
	seza = 2;
	sezb = 4;
	ddda = 171;
	dddb = 265;
	dddd = *ddd;
    }
/*     23.9. - 20.12. */
    if (*ddd >= 265 && *ddd < 354) {
	seza = 4;
	sezb = 3;
	ddda = 265;
	dddb = 354;
	dddd = *ddd;
    }
/*     21.12. - 20.3. */
    if (*ddd >= 354 || *ddd < 79) {
	seza = 3;
	sezb = 1;
	ddda = 354;
	dddb = 444;
	dddd = *ddd;
	if (*ddd >= 354) {
	    dddd = *ddd;
	} else {
	    dddd = *ddd + 365;
	}
    }
    sezai = (seza - 1) % 3 + 1;
    sezbi = (sezb - 1) % 3 + 1;
    if (*alt < (float)900.) {
/*     550km level */
	t0a550 = (float)0.;
	t0b550 = (float)0.;
	t1a550 = (float)0.;
	t1b550 = (float)0.;
	t2a550 = (float)0.;
	t2b550 = (float)0.;
	t3a550 = (float)0.;
	t3b550 = (float)0.;
	for (i__ = 1; i__ <= 81; ++i__) {
	    t0a550 += c__[i__ - 1] * d__[(sezai + i__ * 3 << 2) - 16];
/* L30: */
	    t0b550 += c__[i__ - 1] * d__[(sezbi + i__ * 3 << 2) - 16];
	}
	for (i__ = 1; i__ <= 49; ++i__) {
	    t1a550 += cf107[i__ - 1] * fa[(sezai + i__ * 3 << 2) - 16];
/* L40: */
	    t1b550 += cf107[i__ - 1] * fa[(sezbi + i__ * 3 << 2) - 16];
	}
	for (i__ = 1; i__ <= 49; ++i__) {
	    t2a550 += cf107[i__ - 1] * fb[(sezai + i__ * 3 << 2) - 16];
/* L50: */
	    t2b550 += cf107[i__ - 1] * fb[(sezbi + i__ * 3 << 2) - 16];
	}
	for (i__ = 1; i__ <= 25; ++i__) {
	    t3a550 += csz[i__ - 1] * sz[(seza + (i__ << 2) << 2) - 20];
/* L60: */
	    t3b550 += csz[i__ - 1] * sz[(sezb + (i__ << 2) << 2) - 20];
	}
	t550a = t0a550 + *f107y * (t1a550 * *f107 + t2a550) + *seasy * t3a550;
	t550b = t0b550 + *f107y * (t1b550 * *f107 + t2b550) + *seasy * t3b550;
	t550 = (t550b - t550a) / (dddb - ddda) * (dddd - ddda) + t550a;
    }
    if (*alt < (float)1500.) {
/*     900km level */
	t0a900 = (float)0.;
	t0b900 = (float)0.;
	t1a900 = (float)0.;
	t1b900 = (float)0.;
	t2a900 = (float)0.;
	t2b900 = (float)0.;
	t3a900 = (float)0.;
	t3b900 = (float)0.;
	for (i__ = 1; i__ <= 81; ++i__) {
	    t0a900 += c__[i__ - 1] * d__[(sezai + i__ * 3 << 2) - 15];
/* L70: */
	    t0b900 += c__[i__ - 1] * d__[(sezbi + i__ * 3 << 2) - 15];
	}
	for (i__ = 1; i__ <= 49; ++i__) {
	    t1a900 += cf107[i__ - 1] * fa[(sezai + i__ * 3 << 2) - 15];
/* L80: */
	    t1b900 += cf107[i__ - 1] * fa[(sezbi + i__ * 3 << 2) - 15];
	}
	for (i__ = 1; i__ <= 49; ++i__) {
	    t2a900 += cf107[i__ - 1] * fb[(sezai + i__ * 3 << 2) - 15];
/* L90: */
	    t2b900 += cf107[i__ - 1] * fb[(sezbi + i__ * 3 << 2) - 15];
	}
	for (i__ = 1; i__ <= 25; ++i__) {
	    t3a900 += csz[i__ - 1] * sz[(seza + (i__ << 2) << 2) - 19];
/* L100: */
	    t3b900 += csz[i__ - 1] * sz[(sezb + (i__ << 2) << 2) - 19];
	}
	t900a = t0a900 + *f107y * (t1a900 * *f107 + t2a900) + *seasy * t3a900;
	t900b = t0b900 + *f107y * (t1b900 * *f107 + t2b900) + *seasy * t3b900;
	t900 = (t900b - t900a) / (dddb - ddda) * (dddd - ddda) + t900a;
    }
    if (*alt > (float)900.) {
/*     1500km level */
	t0a150 = (float)0.;
	t0b150 = (float)0.;
	t1a150 = (float)0.;
	t1b150 = (float)0.;
	t2a150 = (float)0.;
	t2b150 = (float)0.;
	t3a150 = (float)0.;
	t3b150 = (float)0.;
	for (i__ = 1; i__ <= 81; ++i__) {
	    t0a150 += c__[i__ - 1] * d__[(sezai + i__ * 3 << 2) - 14];
/* L110: */
	    t0b150 += c__[i__ - 1] * d__[(sezbi + i__ * 3 << 2) - 14];
	}
	for (i__ = 1; i__ <= 49; ++i__) {
	    t1a150 += cf107[i__ - 1] * fa[(sezai + i__ * 3 << 2) - 14];
/* L120: */
	    t1b150 += cf107[i__ - 1] * fa[(sezbi + i__ * 3 << 2) - 14];
	}
	for (i__ = 1; i__ <= 49; ++i__) {
	    t2a150 += cf107[i__ - 1] * fb[(sezai + i__ * 3 << 2) - 14];
/* L130: */
	    t2b150 += cf107[i__ - 1] * fb[(sezbi + i__ * 3 << 2) - 14];
	}
	for (i__ = 1; i__ <= 25; ++i__) {
	    t3a150 += csz[i__ - 1] * sz[(seza + (i__ << 2) << 2) - 18];
/* L140: */
	    t3b150 += csz[i__ - 1] * sz[(sezb + (i__ << 2) << 2) - 18];
	}
	t150a = t0a150 + *f107y * (t1a150 * *f107 + t2a150) + *seasy * t3a150;
	t150b = t0b150 + *f107y * (t1b150 * *f107 + t2b150) + *seasy * t3b150;
	t1500 = (t150b - t150a) / (dddb - ddda) * (dddd - ddda) + t150a;
    }
    if (*alt >= (float)1500.) {
/*     2500km level */
	t0a250 = (float)0.;
	t0b250 = (float)0.;
	t1a250 = (float)0.;
	t1b250 = (float)0.;
	t2a250 = (float)0.;
	t2b250 = (float)0.;
	t3a250 = (float)0.;
	t3b250 = (float)0.;
	for (i__ = 1; i__ <= 81; ++i__) {
	    t0a250 += c__[i__ - 1] * d__[(sezai + i__ * 3 << 2) - 13];
/* L150: */
	    t0b250 += c__[i__ - 1] * d__[(sezbi + i__ * 3 << 2) - 13];
	}
	for (i__ = 1; i__ <= 49; ++i__) {
	    t1a250 += cf107[i__ - 1] * fa[(sezai + i__ * 3 << 2) - 13];
/* L160: */
	    t1b250 += cf107[i__ - 1] * fa[(sezbi + i__ * 3 << 2) - 13];
	}
	for (i__ = 1; i__ <= 49; ++i__) {
	    t2a250 += cf107[i__ - 1] * fb[(sezai + i__ * 3 << 2) - 13];
/* L170: */
	    t2b250 += cf107[i__ - 1] * fb[(sezbi + i__ * 3 << 2) - 13];
	}
	for (i__ = 1; i__ <= 25; ++i__) {
	    t3a250 += csz[i__ - 1] * sz[(seza + (i__ << 2) << 2) - 17];
/* L180: */
	    t3b250 += csz[i__ - 1] * sz[(sezb + (i__ << 2) << 2) - 17];
	}
	t250a = t0a250 + *f107y * (t1a250 * *f107 + t2a250) + *seasy * t3a250;
	t250b = t0b250 + *f107y * (t1b250 * *f107 + t2b250) + *seasy * t3b250;
	t2500 = (t250b - t250a) / (dddb - ddda) * (dddd - ddda) + t250a;
    }
    if (*alt < (float)900.) {
	*te = (t900 - t550) / (float)350. * (*alt - 550) + t550;
    }
    if (*alt >= (float)900. && *alt < (float)1500.) {
	*te = (t1500 - t900) / (float)600. * (*alt - 900) + t900;
    }
    if (*alt >= (float)1500.) {
	*te = (t2500 - t1500) / (float)1e3 * (*alt - 1500) + t1500;
    }
    return 0;
} /* elteik_ */

#undef fb
#undef fa
#undef d__




/* Subroutine */ int spharm_ik__(real *c__, integer *l, integer *m, real *
	colat, real *az)
{
    /* System generated locals */
    integer i__1, i__2;

    /* Builtin functions */
    double cos(doublereal), sin(doublereal), pow_ri(real *, integer *);

    /* Local variables */
    static integer i__, k, n;
    static real x, y;
    static integer mt;
    static real caz, saz;

/* CALCULATES THE COEFFICIENTS OF THE SPHERICAL HARMONIC */
/* FROM IRI 95 MODEL */
/* NOTE: COEFFICIENTS CORRESPONDING TO COS, SIN SWAPPED!!! */
    /* Parameter adjustments */
    --c__;

    /* Function Body */
    c__[1] = (float)1.;
    k = 2;
    x = cos(*colat);
    c__[k] = x;
    ++k;
    i__1 = *l;
    for (i__ = 2; i__ <= i__1; ++i__) {
	c__[k] = (((i__ << 1) - 1) * x * c__[k - 1] - (i__ - 1) * c__[k - 2]) 
		/ i__;
/* L10: */
	++k;
    }
    y = sin(*colat);
    i__1 = *m;
    for (mt = 1; mt <= i__1; ++mt) {
	caz = cos(mt * *az);
	saz = sin(mt * *az);
	c__[k] = pow_ri(&y, &mt);
	++k;
	if (mt == *l) {
	    goto L16;
	}
	c__[k] = c__[k - 1] * x * ((mt << 1) + 1);
	++k;
	if (mt + 1 == *l) {
	    goto L16;
	}
	i__2 = *l;
	for (i__ = mt + 2; i__ <= i__2; ++i__) {
	    c__[k] = (((i__ << 1) - 1) * x * c__[k - 1] - (i__ + mt - 1) * 
		    c__[k - 2]) / (i__ - mt);
/* L15: */
	    ++k;
	}
L16:
	n = *l - mt + 1;
	i__2 = n;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    c__[k] = c__[k - n] * saz;
	    c__[k - n] *= caz;
/* L18: */
	    ++k;
	}
/* L20: */
    }
    return 0;
} /* spharm_ik__ */



/* Subroutine */ int teba_(real *dipl, real *slt, integer *ns, real *te)
{
    /* Initialized data */

    static real c__[648]	/* was [4][2][81] */ = { (float)3.1,(float)
	    3.136,(float)3.372,(float)3.574,(float)3.13654,(float)3.144,(
	    float)3.367,(float)3.574,(float)-.003215,(float).006498,(float)
	    .01006,(float)0.,(float).006796,(float).008571,(float).01038,(
	    float)-.005639,(float).244,(float).2289,(float).1436,(float)
	    .07537,(float).181413,(float).2539,(float).1407,(float).07094,(
	    float)-4.613e-4,(float).01859,(float).002023,(float)0.,(float)
	    .08564,(float).06937,(float).03622,(float)-.03347,(float)-.01711,(
	    float)-.03328,(float)-.05166,(float)-.08459,(float)-.032856,(
	    float)-.01667,(float)-.03144,(float)-.0861,(float).02605,(float)
	    -.004889,(float).009606,(float)0.,(float)-.003508,(float).02249,(
	    float).0112,(float)-.02877,(float)-.09546,(float)-.03054,(float)
	    -.05596,(float)-.0294,(float)-.01438,(float)-.04162,(float)
	    -.05674,(float)-.03154,(float).01794,(float)-.01773,(float)
	    4.914e-4,(float)0.,(float)-.02454,(float).01201,(float).03219,(
	    float)-.002847,(float).0127,(float)-.01728,(float)-.003124,(float)
	    .04547,(float).002745,(float).02435,(float).001288,(float).01235,(
	    float).02791,(float).06555,(float)-.04713,(float)-.05321,(float)
	    .05284,(float).05232,(float)-.05799,(float)-.05966,(float).01536,(
	    float).01775,(float)-.007371,(float)0.,(float).01136,(float)
	    .02521,(float)-.004609,(float)-.003236,(float)-.006629,(float)
	    -.02488,(float)-.004823,(float).004328,(float)-.01956,(float)
	    -.0199,(float).003252,(float)3.795e-4,(float)-.003616,(float)
	    -.009498,(float)-.002213,(float)0.,(float)-.005805,(float)
	    -.007671,(float)-2.859e-4,(float)-8.634e-4,(float).01229,(float)
	    .01493,(float).006569,(float).006022,(float).002801,(float).01264,
	    (float).01226,(float).003377,(float)4.147e-4,(float).00281,(float)
	    -1.962e-4,(float)0.,(float)-.001211,(float)-.001551,(float)
	    -.004539,(float)-1.071e-4,(float).001447,(float).002406,(float)
	    3.309e-4,(float)-9.168e-4,(float).004127,(float)-.001928,(float)
	    .00131,(float)-.002151,(float)-4.453e-4,(float).005436,(float)
	    -3.908e-4,(float)0.,(float).002909,(float).003652,(float)
	    -5.603e-4,(float)-4.057e-4,(float)-.1853,(float)-.2115,(float)
	    -.2836,(float)-.1768,(float)-.25751,(float)-.2019,(float)-.311,(
	    float)-.1783,(float)-.01245,(float).007007,(float).007829,(float)
	    0.,(float)-.0037915,(float).005697,(float)-.001268,(float).0126,(
	    float)-.03675,(float)-.05129,(float).01175,(float).0294,(float)
	    -.0136,(float)-.03159,(float).01539,(float).02835,(float).004965,(
	    float)-.007327,(float)9.919e-4,(float)0.,(float)-.013225,(float)
	    -.01451,(float).003146,(float)-.00242,(float).00546,(float).02402,
	    (float).006589,(float)5.902e-4,(float).01202,(float).02868,(float)
	    .007787,(float).003002,(float).008117,(float).004772,(float)
	    .002045,(float)0.,(float).01256,(float).01377,(float)-.00143,(
	    float)-.004684,(float)-.01002,(float)-.007374,(float)-.007346,(
	    float)-.009047,(float)-.012165,(float)-.004383,(float)-.00482,(
	    float)-.006756,(float)5.466e-4,(float)-3.835e-4,(float)-8.9e-4,(
	    float)0.,(float).01326,(float).01172,(float).002924,(float)
	    -7.493e-4,(float)-.03087,(float)-.05013,(float)-.0347,(float)
	    -.06555,(float)-.07123,(float)-.05683,(float)-.09981,(float)
	    -.06147,(float)-.003435,(float).002866,(float)-.004977,(float)0.,(
	    float)5.793e-4,(float).003593,(float)-.007838,(float)-.005636,(
	    float)-1.107e-4,(float).002216,(float).00147,(float)-.001033,(
	    float).001537,(float).003571,(float)-1.663e-4,(float)-.001234,(
	    float).002199,(float)2.412e-4,(float)-2.823e-6,(float)0.,(float)
	    .006914,(float).003282,(float)4.769e-4,(float)-.001613,(float)
	    4.115e-4,(float).002094,(float)6.465e-4,(float).001674,(float)
	    -.004173,(float).001732,(float).004148,(float)-6.353e-5,(float)
	    6.061e-4,(float).00122,(float)-1.448e-4,(float)0.,(float)1.052e-4,
	    (float)-4.921e-4,(float)-.001008,(float)-2.503e-4,(float)2.916e-4,
	    (float)-1.703e-4,(float).001401,(float)2.802e-4,(float)-5.765e-4,(
	    float)-.001165,(float)-9.79e-4,(float)-1.729e-4,(float)-.06584,(
	    float)-.1082,(float)-.08988,(float)-.06786,(float)-.04041,(float)
	    -.1066,(float)-.09049,(float)-.07148,(float).004729,(float)
	    -.004992,(float)-3.293e-5,(float)0.,(float)-.001752,(float)
	    -.01892,(float)-.002994,(float).005326,(float)-.001523,(float)
	    -.004065,(float)-.001848,(float).004193,(float)-.00542,(float)
	    .00357,(float)-.006748,(float).004006,(float)6.689e-4,(float)
	    .003615,(float)4.439e-4,(float)0.,(float)-.00684,(float)-8.631e-4,
	    (float)-9.889e-4,(float)6.484e-4,(float).001031,(float)-.002738,(
	    float)-.001263,(float)-6.448e-4,(float)8.921e-4,(float)-.001876,(
	    float).001488,(float)-1.046e-4,(float)5.398e-4,(float)-7.177e-4,(
	    float)3.17e-4,(float)0.,(float)-.002228,(float)-8.414e-5,(float)
	    -.001154,(float)-6.034e-4,(float)-.001924,(float)2.173e-4,(float)
	    -6.227e-4,(float)9.277e-4,(float).001428,(float).002356,(float)
	    -8.412e-5,(float)-9.435e-4,(float)-.04565,(float)-.04373,(float)
	    .01721,(float)-.01634,(float).006635,(float)-.04259,(float)
	    -.01302,(float)-.002385,(float).007244,(float)-.00375,(float)
	    -.00199,(float)0.,(float)-.0048045,(float)-.00322,(float)-.004859,
	    (float).006853,(float)-8.543e-5,(float).005507,(float)-4.627e-4,(
	    float)-.002531,(float)-.001659,(float).004641,(float)-7.172e-4,(
	    float).00151,(float).001052,(float)-.001567,(float)2.897e-6,(
	    float)0.,(float)-9.341e-4,(float)6.223e-4,(float)-9.401e-4,(float)
	    .001319,(float)-6.696e-4,(float)-.001458,(float)-5.454e-4,(float)
	    1.93e-5,(float)2.23e-4,(float)-.00168,(float)9.101e-4,(float)
	    9.049e-5,(float)-7.492e-4,(float)-7.397e-4,(float)3.385e-4,(float)
	    0.,(float)-9.995e-4,(float)-1.243e-4,(float)-1.735e-4,(float)
	    -1.999e-4,(float).04405,(float).07903,(float).08432,(float).0528,(
	    float).04285,(float).07393,(float).07055,(float).03976,(float)
	    .003047,(float).004131,(float)-.001951,(float)0.,(float)-5.211e-4,
	    (float)-.003143,(float).006398,(float).002802,(float).002858,(
	    float).003714,(float).001487,(float).002438,(float)-.003293,(
	    float)-.002362,(float)-.003103,(float)-.00103,(float)-1.465e-4,(
	    float).001073,(float).001042,(float)0.,(float).00179,(float)
	    .001235,(float)-9.38e-4,(float)5.599e-4,(float).001195,(float)
	    -8.991e-4,(float)-4.788e-4,(float)-5.292e-4,(float)6.435e-4,(
	    float)-.001551,(float)-4e-4,(float)-4.791e-4,(float)-1.024e-4,(
	    float)2.976e-4,(float)-1.276e-4,(float)0.,(float)-1.891e-4,(float)
	    2.099e-4,(float)-.001165,(float)-8.46e-5,(float).04582,(float)
	    .02623,(float).02373,(float).01555,(float).03844,(float).02299,(
	    float).02713,(float).02683,(float)8.749e-4,(float).002344,(float)
	    .002409,(float)0.,(float).00359,(float).005301,(float)-.001654,(
	    float).00427,(float)3.011e-4,(float)5.608e-4,(float)5.263e-4,(
	    float)-.003259,(float)-8.139e-4,(float)-.004306,(float).002781,(
	    float)5.911e-4,(float)4.473e-4,(float)4.124e-4,(float).001301,(
	    float)0.,(float)-.001996,(float)-.001303,(float)-5.215e-6,(float)
	    2.987e-4,(float)-2.782e-4,(float)1.509e-4,(float)-4.177e-4,(float)
	    -5.998e-4,(float)2.398e-4,(float)7.687e-6,(float)2.258e-4,(float)
	    -2.08e-4,(float).04911,(float).05103,(float).03974,(float).03168,(
	    float).02938,(float).05305,(float).05022,(float).01396,(float)
	    -.01016,(float).00345,(float)1.418e-4,(float)0.,(float).00761,(
	    float).006642,(float).0095,(float)-.001922,(float).0027,(float)
	    .001283,(float)-.001048,(float).002382,(float).00347655,(float)
	    -.001686,(float)4.147e-4,(float)-.001063,(float)-9.304e-4,(float)
	    7.238e-4,(float)-2.982e-4,(float)0.,(float).001707,(float).001048,
	    (float)3.499e-4,(float)3.803e-4,(float)-.001202,(float)-3.464e-5,(
	    float)-3.396e-5,(float)-4.078e-4,(float)2.769e-4,(float)5.958e-4,(
	    float)-6.097e-4,(float)1.343e-4,(float).0221,(float).01663,(float)
	    .0131,(float).02312,(float)-.0157,(float).04341,(float).04118,(
	    float).01771,(float).002566,(float)-.001644,(float).001413,(float)
	    0.,(float)9.83e-4,(float)-8.819e-5,(float).006556,(float)-.001038,
	    (float)-1.22e-4,(float)-7.1e-4,(float)-1.373e-4,(float)1.481e-4,(
	    float)-6.532e-4,(float)-3.33e-4,(float).003793,(float)-4.645e-4,(
	    float)3.987e-4,(float)5.281e-4,(float)2.638e-4,(float)0.,(float)
	    9.29e-5,(float)-2.158e-4,(float)-1.226e-4,(float)-2.481e-4,(float)
	    -.05744,(float)-.02729,(float)-.04171,(float)-.01885,(float)
	    -.02506,(float)-.04106,(float)-.02517,(float)-.02251,(float)
	    .004408,(float).003556,(float)-5.932e-4,(float)0.,(float).004681,(
	    float).004191,(float)1.491e-4,(float)-.0029,(float)-.003497,(
	    float)-.003391,(float)-7.523e-4,(float).001144,(float).001461,(
	    float).002045,(float).001075,(float)-3.977e-4,(float)8.3e-4,(
	    float)-1.787e-4,(float)-6.883e-4,(float)0.,(float)-3.757e-6,(
	    float)-1.437e-4,(float)4.531e-4,(float)-5.16e-4,(float)-.03536,(
	    float).002154,(float)-.02355,(float)-.009952,(float)-.009728,(
	    float)-.01803,(float)-.009012,(float)-.008079,(float)-.008813,(
	    float).006476,(float)5.695e-4,(float)0.,(float).002315,(float)
	    -8.072e-4,(float).003343,(float)-.001528,(float).002423,(float)
	    -8.282e-4,(float)-2.219e-5,(float)-5.51e-4,(float)6.377e-4,(float)
	    -4.24e-4,(float).003431,(float)3.06e-4,(float)-.02994,(float)
	    -.02361,(float)-.02301,(float)-.0202,(float)-.01705,(float)-.026,(
	    float)-.02519,(float)-.01582,(float)-.001929,(float)9.557e-4,(
	    float)-9.962e-5,(float)0.,(float).002767,(float)-.002329,(float)
	    3.793e-5,(float)-8.536e-4,(float)-5.268e-4,(float)3.205e-4,(float)
	    -6.761e-4,(float)-7.283e-5,(float)-6.992e-4,(float)5.949e-4,(
	    float)5.973e-4,(float)1.565e-4,(float)-.02228,(float)-.02301,(
	    float).00204,(float)-.01272,(float)-.0115,(float)-.01371,(float)
	    -.01423,(float)-.01252,(float).003385,(float)-8.54e-4,(float)
	    -5.479e-4,(float)0.,(float)-.001644,(float)-.002188,(float)
	    -.00132,(float)2.319e-4,(float).0413,(float)-.01126,(float).02591,
	    (float).002224,(float).003355,(float).01788,(float)-.006048,(
	    float).004311,(float).004876,(float)-.002323,(float)-.002425,(
	    float)0.,(float)-.004326,(float)6.405e-4,(float)-.005005,(float)
	    .001024,(float).02692,(float)-.008582,(float).01583,(float)
	    -.00251,(float).02035,(float).005977,(float)-.0115,(float)
	    1.296e-6,(float).001684,(float).02683,(float).009577,(float)
	    .02434,(float).02985,(float).01333,(float).02574,(float).0179 };

    /* System generated locals */
    integer i__1;
    doublereal d__1;

    /* Builtin functions */
    double pow_dd(doublereal *, doublereal *);

    /* Local variables */
    static integer kend;
    static real a[82];
    static integer i__, j, k;
    static real colat, az;
    static integer is;
    extern /* Subroutine */ int spharm_(real *, integer *, integer *, real *, 
	    real *);
    static real ste;

/* CALCULATES ELECTRON TEMPERATURES TE(1) TO TE(4) AT ALTITUDES */
/* 300, 400, 1400 AND 3000 KM FOR DIP-LATITUDE DIPL/DEG AND */
/* LOCAL SOLAR TIME SLT/H USING THE BRACE-THEIS-MODELS (J. ATMOS. */
/* TERR. PHYS. 43, 1317, 1981); NS IS SEASON IN NORTHERN */
/* HEMISOHERE: IS=1 SPRING, IS=2 SUMMER .... */
/* ALSO CALCULATED ARE THE TEMPERATURES AT 400 KM ALTITUDE FOR */
/* MIDNIGHT (TE(5)) AND NOON (TE(6)). */
    /* Parameter adjustments */
    --te;

    /* Function Body */
    if (*ns < 3) {
	is = *ns;
    } else if (*ns > 3) {
	is = 2;
	*dipl = -(*dipl);
    } else {
	is = 1;
    }
    colat = const_1.umr * ((float)90. - *dipl);
    az = const1_1.humr * *slt;
    spharm_(a, &c__8, &c__8, &colat, &az);
    if (is == 2) {
	kend = 3;
    } else {
	kend = 4;
    }
    i__1 = kend;
    for (k = 1; k <= i__1; ++k) {
	ste = (float)0.;
	for (i__ = 1; i__ <= 81; ++i__) {
/* L1: */
	    ste += a[i__ - 1] * c__[k + (is + (i__ << 1) << 2) - 13];
	}
/* L2: */
	d__1 = (doublereal) ste;
	te[k] = pow_dd(&c_b30, &d__1);
    }
    if (is == 2) {
	*dipl = -(*dipl);
	colat = const_1.umr * ((float)90. - *dipl);
	spharm_(a, &c__8, &c__8, &colat, &az);
	ste = (float)0.;
	for (i__ = 1; i__ <= 81; ++i__) {
/* L11: */
	    ste += a[i__ - 1] * c__[((i__ << 1) + 2 << 2) - 9];
	}
	d__1 = (doublereal) ste;
	te[4] = pow_dd(&c_b30, &d__1);
    }
/* ---------- TEMPERATURE AT 400KM AT MIDNIGHT AND NOON */
    for (j = 1; j <= 2; ++j) {
	ste = (float)0.;
	az = const1_1.humr * (j - 1) * (float)12.;
	spharm_(a, &c__8, &c__8, &colat, &az);
	for (i__ = 1; i__ <= 81; ++i__) {
/* L3: */
	    ste += a[i__ - 1] * c__[(is + (i__ << 1) << 2) - 11];
	}
/* L4: */
	d__1 = (doublereal) ste;
	te[j + 4] = pow_dd(&c_b30, &d__1);
    }
    return 0;
} /* teba_ */


/* Subroutine */ int spharm_(real *c__, integer *l, integer *m, real *colat, 
	real *az)
{
    /* System generated locals */
    integer i__1, i__2;

    /* Builtin functions */
    double cos(doublereal), sin(doublereal), pow_ri(real *, integer *);

    /* Local variables */
    static integer i__, k, n;
    static real x, y;
    static integer mt;
    static real caz, saz;

/* CALCULATES THE COEFFICIENTS OF THE SPHERICAL HARMONIC */
/* EXPANSION THAT WAS USED FOR THE BRACE-THEIS-MODELS. */
    /* Parameter adjustments */
    --c__;

    /* Function Body */
    c__[1] = (float)1.;
    k = 2;
    x = cos(*colat);
    c__[k] = x;
    ++k;
    i__1 = *l;
    for (i__ = 2; i__ <= i__1; ++i__) {
	c__[k] = (((i__ << 1) - 1) * x * c__[k - 1] - (i__ - 1) * c__[k - 2]) 
		/ i__;
/* L10: */
	++k;
    }
    y = sin(*colat);
    i__1 = *m;
    for (mt = 1; mt <= i__1; ++mt) {
	caz = cos(mt * *az);
	saz = sin(mt * *az);
	c__[k] = pow_ri(&y, &mt);
	++k;
	if (mt == *l) {
	    goto L16;
	}
	c__[k] = c__[k - 1] * x * ((mt << 1) + 1);
	++k;
	if (mt + 1 == *l) {
	    goto L16;
	}
	i__2 = *l;
	for (i__ = mt + 2; i__ <= i__2; ++i__) {
	    c__[k] = (((i__ << 1) - 1) * x * c__[k - 1] - (i__ + mt - 1) * 
		    c__[k - 2]) / (i__ - mt);
/* L15: */
	    ++k;
	}
L16:
	n = *l - mt + 1;
	i__2 = n;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    c__[k] = c__[k - n] * caz;
	    c__[k - n] *= saz;
/* L18: */
	    ++k;
	}
/* L20: */
    }
    return 0;
} /* spharm_ */



doublereal elte_(real *h__)
{
    /* System generated locals */
    real ret_val;

    /* Local variables */
    extern doublereal eptr_(real *, real *, real *);
    static integer i__;
    static real aa, bb, sum;

/* ---------------------------------------------------------------- */
/* ELECTRON TEMPERATURE PROFILE BASED ON THE TEMPERATURES AT 120 */
/* HMAX,300,400,600,1400,3000 KM ALTITUDE. INBETWEEN CONSTANT */
/* GRADIENT IS ASSUMED. ARGMAX IS MAXIMUM ARGUMENT ALLOWED FOR */
/* EXP-FUNCTION. */
/* ---------------------------------------------------------------- */

    sum = blote_1.ate1 + blote_1.st[0] * (*h__ - blote_1.ah[0]);
    for (i__ = 1; i__ <= 5; ++i__) {
	aa = eptr_(h__, &blote_1.d__[i__ - 1], &blote_1.ah[i__]);
	bb = eptr_(blote_1.ah, &blote_1.d__[i__ - 1], &blote_1.ah[i__]);
/* L1: */
	sum += (blote_1.st[i__] - blote_1.st[i__ - 1]) * (aa - bb) * 
		blote_1.d__[i__ - 1];
    }
    ret_val = sum;
    return ret_val;
} /* elte_ */



doublereal tede_(real *h__, real *den, real *cov)
{
    /* System generated locals */
    real ret_val;

    /* Builtin functions */
    double exp(doublereal);

    /* Local variables */
    static real acov, y, yc;

/* ELECTRON TEMEPERATURE MODEL AFTER BRACE,THEIS . */
/* FOR NEG. COV THE MEAN COV-INDEX (3 SOLAR ROT.) IS EXPECTED. */
/* DEN IS THE ELECTRON DENSITY IN M-3. */
    y = (*h__ * (float)17.01 - (float)2746.) * exp(*h__ * (float)-5.122e-4 + (
	    (float)6.094e-12 - *h__ * (float)3.353e-14) * *den) + (float)
	    1051.;
    acov = dabs(*cov);
    yc = (acov * (float).00202 + (float).117) / (exp(-(acov - (float)102.5) / 
	    (float)5.) + (float)1.) + (float)1.;
    if (*cov < (float)0.) {
	yc = (acov * (float).00169 + (float).123) / (exp(-(acov - (float)115.)
		 / (float)10.) + (float)1.) + (float)1.;
    }
    ret_val = y * yc;
    return ret_val;
} /* tede_ */



/* ************************************************************* */
/* **************** ION TEMPERATURE **************************** */
/* ************************************************************* */


doublereal ti_(real *h__)
{
    /* System generated locals */
    integer i__1;
    real ret_val;

    /* Local variables */
    extern doublereal eptr_(real *, real *, real *);
    static integer i__;
    static real aa, bb, sum;

/* ---------------------------------------------------------------- */
/* ION TEMPERATURE FOR HEIGHTS NOT GREATER 1000 KM AND NOT LESS HS */
/* EXPLANATION SEE FUNCTION RPID. */
/* ---------------------------------------------------------------- */
    sum = block8_1.mm[0] * (*h__ - block8_1.hs) + block8_1.tnhs;
    i__1 = block8_1.m - 1;
    for (i__ = 1; i__ <= i__1; ++i__) {
	aa = eptr_(h__, &block8_1.g[i__ - 1], &block8_1.xsm[i__ - 1]);
	bb = eptr_(&block8_1.hs, &block8_1.g[i__ - 1], &block8_1.xsm[i__ - 1])
		;
/* L100: */
	sum += (block8_1.mm[i__] - block8_1.mm[i__ - 1]) * (aa - bb) * 
		block8_1.g[i__ - 1];
    }
    ret_val = sum;
    return ret_val;
} /* ti_ */



doublereal teder_(real *h__)
{
    /* System generated locals */
    real ret_val;

    /* Local variables */
    static real dtdx;
    extern doublereal dtndh_(real *, real *, real *, real *), tn_(real *, 
	    real *, real *, real *);
    static real tnh;

/* THIS FUNCTION ALONG WITH PROCEDURE REGFA1 ALLOWS TO FIND */
/* THE  HEIGHT ABOVE WHICH TN BEGINS TO BE DIFFERENT FROM TI */
    tnh = tn_(h__, &blotn_1.tex, &blotn_1.tlbd, &blotn_1.sig);
    dtdx = dtndh_(h__, &blotn_1.tex, &blotn_1.tlbd, &blotn_1.sig);
    ret_val = dtdx * (blotn_1.xsm1 - *h__) + tnh;
    return ret_val;
} /* teder_ */



doublereal tn_(real *h__, real *tinf, real *tlbd, real *s)
{
    /* System generated locals */
    real ret_val;

    /* Builtin functions */
    double exp(doublereal);

    /* Local variables */
    static real zg2;

/* -------------------------------------------------------------------- */
/*       Calculate Temperature for MSIS/CIRA-86 model */
/* -------------------------------------------------------------------- */
    zg2 = (*h__ - (float)120.) * (float)6476.77 / (*h__ + (float)6356.77);
    ret_val = *tinf - *tlbd * exp(-(*s) * zg2);
    return ret_val;
} /* tn_ */



doublereal dtndh_(real *h__, real *tinf, real *tlbd, real *s)
{
    /* System generated locals */
    real ret_val;

    /* Builtin functions */
    double exp(doublereal);

    /* Local variables */
    static real zg1, zg2, zg3;

/* --------------------------------------------------------------------- */
    zg1 = *h__ + (float)6356.77;
    zg2 = (float)6476.77 / zg1;
    zg3 = (*h__ - (float)120.) * zg2;
    ret_val = -(*tlbd) * exp(-(*s) * zg3) * (*s / zg1 * (zg3 - zg2));
    return ret_val;
} /* dtndh_ */



/* ************************************************************* */
/* ************* ION RELATIVE PRECENTAGE DENSITY ***************** */
/* ************************************************************* */


doublereal rpid_(real *h__, real *h0, real *n0, integer *m, real *st, integer 
	*id, real *xs)
{
    /* System generated locals */
    integer i__1;
    real ret_val;

    /* Builtin functions */
    double exp(doublereal);

    /* Local variables */
    extern doublereal eptr_(real *, real *, real *);
    static integer i__;
    static real aa, bb, sm, xi, sum;

/* ------------------------------------------------------------------ */
/* D.BILITZA,1977,THIS ANALYTIC FUNCTION IS USED TO REPRESENT THE */
/* RELATIVE PRECENTAGE DENSITY OF ATOMAR AND MOLECULAR OXYGEN IONS. */
/* THE M+1 HEIGHT GRADIENTS ST(M+1) ARE CONNECTED WITH EPSTEIN- */
/* STEP-FUNCTIONS AT THE STEP HEIGHTS XS(M) WITH TRANSITION */
/* THICKNESSES ID(M). RPID(H0,H0,N0,....)=N0. */
/* ARGMAX is the highest allowed argument for EXP in your system. */
/* ------------------------------------------------------------------ */
    /* Parameter adjustments */
    --xs;
    --id;
    --st;

    /* Function Body */
    sum = (*h__ - *h0) * st[1];
    i__1 = *m;
    for (i__ = 1; i__ <= i__1; ++i__) {
	xi = (real) id[i__];
	aa = eptr_(h__, &xi, &xs[i__]);
	bb = eptr_(h0, &xi, &xs[i__]);
/* L100: */
	sum += (st[i__ + 1] - st[i__]) * (aa - bb) * xi;
    }
    if (dabs(sum) < argexp_1.argmax) {
	sm = exp(sum);
    } else if (sum > (float)0.) {
	sm = exp(argexp_1.argmax);
    } else {
	sm = (float)0.;
    }
    ret_val = *n0 * sm;
    return ret_val;
} /* rpid_ */



/* Subroutine */ int rdhhe_(real *h__, real *hb, real *rdoh, real *rdo2h, 
	real *rno, real *pehe, real *rdh, real *rdhe)
{
    static real rest;

/* BILITZA,FEB.82,H+ AND HE+ RELATIVE PERECENTAGE DENSITY BELOW */
/* 1000 KM. THE O+ AND O2+ REL. PER. DENSITIES SHOULD BE GIVEN */
/* (RDOH,RDO2H). HB IS THE ALTITUDE OF MAXIMAL O+ DENSITY. PEHE */
/* IS THE PRECENTAGE OF HE+ IONS COMPARED TO ALL LIGHT IONS. */
/* RNO IS THE RATIO OF NO+ TO O2+DENSITY AT H=HB. */
    *rdhe = (float)0.;
    *rdh = (float)0.;
    if (*h__ <= *hb) {
	goto L100;
    }
    rest = (float)100. - *rdoh - *rdo2h - *rno * *rdo2h;
    *rdh = rest * ((float)1. - *pehe / (float)100.);
    *rdhe = rest * *pehe / (float)100.;
L100:
    return 0;
} /* rdhhe_ */



doublereal rdno_(real *h__, real *hb, real *rdo2h, real *rdoh, real *rno)
{
    /* System generated locals */
    real ret_val;

/* D.BILITZA, 1978. NO+ RELATIVE PERCENTAGE DENSITY ABOVE 100KM. */
/* FOR MORE INFORMATION SEE SUBROUTINE RDHHE. */
    if (*h__ > *hb) {
	goto L200;
    }
    ret_val = (float)100. - *rdo2h - *rdoh;
    return ret_val;
L200:
    ret_val = *rno * *rdo2h;
    return ret_val;
} /* rdno_ */



/* Subroutine */ int koefp1_(real *pg1o)
{
    /* Initialized data */

    static real feld[80] = { (float)-11.,(float)-11.,(float)4.,(float)-11.,(
	    float).08018,(float).13027,(float).04216,(float).25,(float)
	    -.00686,(float).00999,(float)5.113,(float).1,(float)170.,(float)
	    180.,(float).1175,(float).15,(float)-11.,(float)1.,(float)2.,(
	    float)-11.,(float).069,(float).161,(float).254,(float).18,(float)
	    .0161,(float).0216,(float).03014,(float).1,(float)152.,(float)
	    167.,(float).04916,(float).17,(float)-11.,(float)2.,(float)2.,(
	    float)-11.,(float).072,(float).092,(float).014,(float).21,(float)
	    .01389,(float).03863,(float).05762,(float).12,(float)165.,(float)
	    168.,(float).008,(float).258,(float)-11.,(float)1.,(float)3.,(
	    float)-11.,(float).091,(float).088,(float).008,(float).34,(float)
	    .0067,(float).0195,(float).04,(float).1,(float)158.,(float)172.,(
	    float).01,(float).24,(float)-11.,(float)2.,(float)3.,(float)-11.,(
	    float).083,(float).102,(float).045,(float).03,(float).00127,(
	    float).01,(float).05,(float).09,(float)167.,(float)185.,(float)
	    .015,(float).18 };

    static integer i__, k;

/* THIEMANN,1979,COEFFICIENTS PG1O FOR CALCULATING  O+ PROFILES */
/* BELOW THE F2-MAXIMUM. CHOSEN TO APPROACH DANILOV- */
/* SEMENOV'S COMPILATION. */
    /* Parameter adjustments */
    --pg1o;

    /* Function Body */
    k = 0;
    for (i__ = 1; i__ <= 80; ++i__) {
	++k;
/* L10: */
	pg1o[k] = feld[i__ - 1];
    }
    return 0;
} /* koefp1_ */



/* Subroutine */ int koefp2_(real *pg2o)
{
    /* Initialized data */

    static real feld[32] = { (float)1.,(float)-11.,(float)-11.,(float)1.,(
	    float)695.,(float)-7.81e-4,(float)-.00264,(float)2177.,(float)1.,(
	    float)-11.,(float)-11.,(float)2.,(float)570.,(float)-.002,(float)
	    -.0052,(float)1040.,(float)2.,(float)-11.,(float)-11.,(float)1.,(
	    float)695.,(float)-7.86e-4,(float)-.00165,(float)3367.,(float)2.,(
	    float)-11.,(float)-11.,(float)2.,(float)575.,(float)-.00126,(
	    float)-.00524,(float)1380. };

    static integer i__, k;

/* THIEMANN,1979,COEFFICIENTS FOR CALCULATION OF O+ PROFILES */
/* ABOVE THE F2-MAXIMUM (DUMBS,SPENNER:AEROS-COMPILATION) */
    /* Parameter adjustments */
    --pg2o;

    /* Function Body */
    k = 0;
    for (i__ = 1; i__ <= 32; ++i__) {
	++k;
/* L10: */
	pg2o[k] = feld[i__ - 1];
    }
    return 0;
} /* koefp2_ */



/* Subroutine */ int koefp3_(real *pg3o)
{
    /* Initialized data */

    static real feld[80] = { (float)-11.,(float)1.,(float)2.,(float)-11.,(
	    float)160.,(float)31.,(float)130.,(float)-10.,(float)198.,(float)
	    0.,(float).05922,(float)-.07983,(float)-.00397,(float)8.5e-4,(
	    float)-.00313,(float)0.,(float)-11.,(float)2.,(float)2.,(float)
	    -11.,(float)140.,(float)30.,(float)130.,(float)-10.,(float)190.,(
	    float)0.,(float).05107,(float)-.07964,(float)9.7e-4,(float)
	    -.01118,(float)-.02614,(float)-.09537,(float)-11.,(float)1.,(
	    float)3.,(float)-11.,(float)140.,(float)37.,(float)125.,(float)0.,
	    (float)182.,(float)0.,(float).0307,(float)-.04968,(float)-.00248,(
	    float)-.02451,(float)-.00313,(float)0.,(float)-11.,(float)2.,(
	    float)3.,(float)-11.,(float)140.,(float)37.,(float)125.,(float)0.,
	    (float)170.,(float)0.,(float).02806,(float)-.04716,(float)6.6e-4,(
	    float)-.02763,(float)-.02247,(float)-.01919,(float)-11.,(float)
	    -11.,(float)4.,(float)-11.,(float)140.,(float)45.,(float)136.,(
	    float)-9.,(float)181.,(float)-26.,(float).02994,(float)-.04879,(
	    float)-.01396,(float)8.9e-4,(float)-.09929,(float).05589 };

    static integer i__, k;

/* THIEMANN,1979,COEFFICIENTS FOR CALCULATING O2+ PROFILES. */
/* CHOSEN AS TO APPROACH DANILOV-SEMENOV'S COMPILATION. */
    /* Parameter adjustments */
    --pg3o;

    /* Function Body */
    k = 0;
    for (i__ = 1; i__ <= 80; ++i__) {
	++k;
/* L10: */
	pg3o[k] = feld[i__ - 1];
    }
    return 0;
} /* koefp3_ */



/* Subroutine */ int sufe_(real *field, real *rfe, integer *m, real *fe)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__, k;
    static real efe[4];

/* SELECTS THE REQUIRED ION DENSITY PARAMETER SET. */
/* THE INPUT FIELD INCLUDES DIFFERENT SETS OF DIMENSION M EACH */
/* CARACTERISED BY 4 HEADER NUMBERS. RFE(4) SHOULD CONTAIN THE */
/* CHOSEN HEADER NUMBERS.FE(M) IS THE CORRESPONDING SET. */
    /* Parameter adjustments */
    --fe;
    --rfe;
    --field;

    /* Function Body */
    k = 0;
L100:
    for (i__ = 1; i__ <= 4; ++i__) {
	++k;
/* L101: */
	efe[i__ - 1] = field[k];
    }
    i__1 = *m;
    for (i__ = 1; i__ <= i__1; ++i__) {
	++k;
/* L111: */
	fe[i__] = field[k];
    }
    for (i__ = 1; i__ <= 4; ++i__) {
	if (efe[i__ - 1] > (float)-10. && rfe[i__] != efe[i__ - 1]) {
	    goto L100;
	}
/* L120: */
    }
    return 0;
} /* sufe_ */

/* Subroutine */ int ionco2_(real *hei, real *xhi, integer *it, real *f, real 
	*r1, real *r2, real *r3, real *r4)
{
    /* Initialized data */

    static integer j1ms70[7] = { 11,11,10,10,11,9,11 };
    static integer j2ms70[7] = { 13,11,10,11,11,9,11 };
    static real h1s70[91]	/* was [13][7] */ = { (float)75.,(float)85.,(
	    float)90.,(float)95.,(float)100.,(float)120.,(float)130.,(float)
	    200.,(float)220.,(float)250.,(float)270.,(float)0.,(float)0.,(
	    float)75.,(float)85.,(float)90.,(float)95.,(float)100.,(float)
	    120.,(float)130.,(float)200.,(float)220.,(float)250.,(float)270.,(
	    float)0.,(float)0.,(float)75.,(float)85.,(float)90.,(float)95.,(
	    float)100.,(float)115.,(float)200.,(float)220.,(float)250.,(float)
	    270.,(float)0.,(float)0.,(float)0.,(float)75.,(float)80.,(float)
	    95.,(float)100.,(float)120.,(float)140.,(float)200.,(float)220.,(
	    float)250.,(float)270.,(float)0.,(float)0.,(float)0.,(float)75.,(
	    float)80.,(float)95.,(float)100.,(float)120.,(float)150.,(float)
	    170.,(float)200.,(float)220.,(float)250.,(float)270.,(float)0.,(
	    float)0.,(float)75.,(float)80.,(float)95.,(float)100.,(float)140.,
	    (float)200.,(float)220.,(float)250.,(float)270.,(float)0.,(float)
	    0.,(float)0.,(float)0.,(float)75.,(float)80.,(float)85.,(float)
	    95.,(float)100.,(float)110.,(float)145.,(float)200.,(float)220.,(
	    float)250.,(float)270.,(float)0.,(float)0. };
    static real h2s70[91]	/* was [13][7] */ = { (float)75.,(float)80.,(
	    float)90.,(float)95.,(float)100.,(float)120.,(float)130.,(float)
	    140.,(float)150.,(float)200.,(float)220.,(float)250.,(float)270.,(
	    float)75.,(float)80.,(float)90.,(float)95.,(float)100.,(float)
	    120.,(float)130.,(float)200.,(float)220.,(float)250.,(float)270.,(
	    float)0.,(float)0.,(float)75.,(float)80.,(float)90.,(float)95.,(
	    float)100.,(float)115.,(float)200.,(float)220.,(float)250.,(float)
	    270.,(float)0.,(float)0.,(float)0.,(float)75.,(float)80.,(float)
	    95.,(float)100.,(float)120.,(float)140.,(float)150.,(float)200.,(
	    float)220.,(float)250.,(float)270.,(float)0.,(float)0.,(float)75.,
	    (float)80.,(float)95.,(float)100.,(float)120.,(float)150.,(float)
	    170.,(float)200.,(float)220.,(float)250.,(float)270.,(float)0.,(
	    float)0.,(float)75.,(float)80.,(float)95.,(float)100.,(float)140.,
	    (float)200.,(float)220.,(float)250.,(float)270.,(float)0.,(float)
	    0.,(float)0.,(float)0.,(float)75.,(float)80.,(float)90.,(float)
	    95.,(float)100.,(float)110.,(float)145.,(float)200.,(float)220.,(
	    float)250.,(float)270.,(float)0.,(float)0. };
    static real r1ms70[91]	/* was [13][7] */ = { (float)6.,(float)30.,(
	    float)60.,(float)63.,(float)59.,(float)59.,(float)66.,(float)52.,(
	    float)20.,(float)4.,(float)2.,(float)0.,(float)0.,(float)6.,(
	    float)30.,(float)60.,(float)63.,(float)69.,(float)62.,(float)66.,(
	    float)52.,(float)20.,(float)4.,(float)2.,(float)0.,(float)0.,(
	    float)6.,(float)30.,(float)60.,(float)63.,(float)80.,(float)68.,(
	    float)53.,(float)20.,(float)4.,(float)2.,(float)0.,(float)0.,(
	    float)0.,(float)4.,(float)10.,(float)60.,(float)85.,(float)65.,(
	    float)65.,(float)52.,(float)25.,(float)12.,(float)4.,(float)0.,(
	    float)0.,(float)0.,(float)4.,(float)10.,(float)60.,(float)89.,(
	    float)72.,(float)60.,(float)60.,(float)52.,(float)30.,(float)20.,(
	    float)10.,(float)0.,(float)0.,(float)4.,(float)10.,(float)60.,(
	    float)92.,(float)68.,(float)54.,(float)40.,(float)25.,(float)13.,(
	    float)0.,(float)0.,(float)0.,(float)0.,(float)1.,(float)8.,(float)
	    20.,(float)60.,(float)95.,(float)93.,(float)69.,(float)65.,(float)
	    45.,(float)30.,(float)20.,(float)0.,(float)0. };
    static real r2ms70[91]	/* was [13][7] */ = { (float)4.,(float)10.,(
	    float)30.,(float)32.,(float)41.,(float)41.,(float)32.,(float)29.,(
	    float)34.,(float)28.,(float)15.,(float)3.,(float)1.,(float)4.,(
	    float)10.,(float)30.,(float)32.,(float)31.,(float)38.,(float)32.,(
	    float)28.,(float)15.,(float)3.,(float)1.,(float)0.,(float)0.,(
	    float)4.,(float)10.,(float)30.,(float)32.,(float)20.,(float)32.,(
	    float)28.,(float)15.,(float)3.,(float)1.,(float)0.,(float)0.,(
	    float)0.,(float)2.,(float)6.,(float)30.,(float)15.,(float)35.,(
	    float)30.,(float)34.,(float)26.,(float)19.,(float)8.,(float)3.,(
	    float)0.,(float)0.,(float)2.,(float)6.,(float)30.,(float)11.,(
	    float)28.,(float)38.,(float)29.,(float)29.,(float)25.,(float)12.,(
	    float)5.,(float)0.,(float)0.,(float)2.,(float)6.,(float)30.,(
	    float)8.,(float)32.,(float)30.,(float)20.,(float)14.,(float)8.,(
	    float)0.,(float)0.,(float)0.,(float)0.,(float)1.,(float)2.,(float)
	    10.,(float)20.,(float)5.,(float)7.,(float)31.,(float)23.,(float)
	    18.,(float)15.,(float)10.,(float)0.,(float)0. };
    static real rk1ms70[91]	/* was [13][7] */ = { (float)2.4,(float)6.,(
	    float).6,(float)-.8,(float)0.,(float).7,(float)-.2,(float)-1.6,(
	    float)-.533,(float)-.1,(float)-.067,(float)0.,(float)0.,(float)
	    2.4,(float)6.,(float).6,(float)1.2,(float)-.35,(float).4,(float)
	    -.2,(float)-1.6,(float)-.533,(float)-.1,(float)-.067,(float)0.,(
	    float)0.,(float)2.4,(float)6.,(float).6,(float)3.4,(float)-.8,(
	    float)-.176,(float)-1.65,(float)-.533,(float)-.1,(float)-.067,(
	    float)0.,(float)0.,(float)0.,(float)1.2,(float)3.333,(float)5.,(
	    float)-1.,(float)0.,(float)-.216,(float)-1.35,(float)-.433,(float)
	    -.4,(float)-.1,(float)0.,(float)0.,(float)0.,(float)1.2,(float)
	    3.333,(float)5.8,(float)-.85,(float)-.4,(float)0.,(float)-.267,(
	    float)-1.1,(float)-.333,(float)-.4,(float)-.2,(float)0.,(float)0.,
	    (float)1.2,(float)3.333,(float)6.4,(float)-.6,(float)-.233,(float)
	    -.7,(float)-.5,(float)-.6,(float)-.267,(float)0.,(float)0.,(float)
	    0.,(float)0.,(float)1.4,(float)2.4,(float)4.,(float)7.,(float)-.2,
	    (float)-.686,(float)-.072,(float)-1.,(float)-.5,(float)-.5,(float)
	    -.5,(float)0.,(float)0. };
    static real rk2ms70[91]	/* was [13][7] */ = { (float)1.2,(float)2.,(
	    float).4,(float)1.8,(float)0.,(float)-.9,(float)-.3,(float).5,(
	    float)-.12,(float)-.65,(float)-.4,(float)-.1,(float)-.033,(float)
	    1.2,(float)2.,(float).4,(float)-.2,(float).35,(float)-.6,(float)
	    -.057,(float)-.65,(float)-.4,(float)-.1,(float)-.033,(float)0.,(
	    float)0.,(float)1.2,(float)2.,(float).4,(float)-2.4,(float).8,(
	    float)-.047,(float)-.65,(float)-.4,(float)-.1,(float)-.033,(float)
	    0.,(float)0.,(float)0.,(float).8,(float)1.6,(float)-3.,(float)1.,(
	    float)-.25,(float).4,(float)-.16,(float)-.35,(float)-.367,(float)
	    -.25,(float)-.1,(float)0.,(float)0.,(float).8,(float)1.6,(float)
	    -3.8,(float).85,(float).333,(float)-.45,(float)0.,(float)-.2,(
	    float)-.433,(float)-.35,(float)-.1,(float)0.,(float)0.,(float).8,(
	    float)1.6,(float)-4.4,(float).6,(float)-.033,(float)-.5,(float)
	    -.2,(float)-.3,(float)-.2,(float)0.,(float)0.,(float)0.,(float)0.,
	    (float).2,(float).8,(float)2.,(float)-3.,(float).2,(float).686,(
	    float)-.145,(float)-.25,(float)-.1,(float)-.25,(float)-.2,(float)
	    0.,(float)0. };
    static integer j1ms140[7] = { 11,11,10,10,9,9,12 };
    static integer j2ms140[7] = { 11,11,10,9,10,10,12 };
    static real h1s140[91]	/* was [13][7] */ = { (float)75.,(float)85.,(
	    float)90.,(float)95.,(float)100.,(float)120.,(float)130.,(float)
	    140.,(float)200.,(float)220.,(float)250.,(float)0.,(float)0.,(
	    float)75.,(float)85.,(float)90.,(float)95.,(float)100.,(float)
	    120.,(float)130.,(float)140.,(float)200.,(float)220.,(float)250.,(
	    float)0.,(float)0.,(float)75.,(float)85.,(float)90.,(float)95.,(
	    float)100.,(float)120.,(float)140.,(float)200.,(float)220.,(float)
	    250.,(float)0.,(float)0.,(float)0.,(float)75.,(float)80.,(float)
	    95.,(float)100.,(float)120.,(float)140.,(float)200.,(float)220.,(
	    float)250.,(float)270.,(float)0.,(float)0.,(float)0.,(float)75.,(
	    float)80.,(float)95.,(float)100.,(float)120.,(float)200.,(float)
	    220.,(float)250.,(float)270.,(float)0.,(float)0.,(float)0.,(float)
	    0.,(float)75.,(float)80.,(float)95.,(float)100.,(float)130.,(
	    float)200.,(float)220.,(float)250.,(float)270.,(float)0.,(float)
	    0.,(float)0.,(float)0.,(float)75.,(float)80.,(float)85.,(float)
	    95.,(float)100.,(float)110.,(float)140.,(float)180.,(float)200.,(
	    float)220.,(float)250.,(float)270.,(float)0. };
    static real h2s140[91]	/* was [13][7] */ = { (float)75.,(float)80.,(
	    float)90.,(float)95.,(float)100.,(float)120.,(float)130.,(float)
	    155.,(float)200.,(float)220.,(float)250.,(float)0.,(float)0.,(
	    float)75.,(float)80.,(float)90.,(float)95.,(float)100.,(float)
	    120.,(float)130.,(float)160.,(float)200.,(float)220.,(float)250.,(
	    float)0.,(float)0.,(float)75.,(float)80.,(float)90.,(float)95.,(
	    float)100.,(float)120.,(float)165.,(float)200.,(float)220.,(float)
	    250.,(float)0.,(float)0.,(float)0.,(float)75.,(float)80.,(float)
	    95.,(float)100.,(float)120.,(float)180.,(float)200.,(float)250.,(
	    float)270.,(float)0.,(float)0.,(float)0.,(float)0.,(float)75.,(
	    float)80.,(float)95.,(float)100.,(float)120.,(float)160.,(float)
	    200.,(float)220.,(float)250.,(float)270.,(float)0.,(float)0.,(
	    float)0.,(float)75.,(float)80.,(float)95.,(float)100.,(float)130.,
	    (float)160.,(float)200.,(float)220.,(float)250.,(float)270.,(
	    float)0.,(float)0.,(float)0.,(float)75.,(float)80.,(float)90.,(
	    float)95.,(float)100.,(float)110.,(float)140.,(float)180.,(float)
	    200.,(float)220.,(float)250.,(float)270.,(float)0. };
    static real r1ms140[91]	/* was [13][7] */ = { (float)6.,(float)30.,(
	    float)60.,(float)63.,(float)59.,(float)59.,(float)66.,(float)66.,(
	    float)38.,(float)14.,(float)1.,(float)0.,(float)0.,(float)6.,(
	    float)30.,(float)60.,(float)63.,(float)69.,(float)62.,(float)66.,(
	    float)66.,(float)38.,(float)14.,(float)1.,(float)0.,(float)0.,(
	    float)6.,(float)30.,(float)60.,(float)63.,(float)80.,(float)65.,(
	    float)65.,(float)38.,(float)14.,(float)1.,(float)0.,(float)0.,(
	    float)0.,(float)4.,(float)10.,(float)60.,(float)85.,(float)66.,(
	    float)66.,(float)38.,(float)22.,(float)9.,(float)1.,(float)0.,(
	    float)0.,(float)0.,(float)4.,(float)10.,(float)60.,(float)89.,(
	    float)71.,(float)42.,(float)26.,(float)17.,(float)10.,(float)0.,(
	    float)0.,(float)0.,(float)0.,(float)4.,(float)10.,(float)60.,(
	    float)93.,(float)71.,(float)48.,(float)35.,(float)22.,(float)10.,(
	    float)0.,(float)0.,(float)0.,(float)0.,(float)1.,(float)8.,(float)
	    20.,(float)60.,(float)95.,(float)93.,(float)72.,(float)60.,(float)
	    58.,(float)40.,(float)26.,(float)13.,(float)0. };
    static real r2ms140[91]	/* was [13][7] */ = { (float)4.,(float)10.,(
	    float)30.,(float)32.,(float)41.,(float)41.,(float)30.,(float)30.,(
	    float)10.,(float)6.,(float)1.,(float)0.,(float)0.,(float)4.,(
	    float)10.,(float)30.,(float)32.,(float)31.,(float)38.,(float)31.,(
	    float)29.,(float)9.,(float)6.,(float)1.,(float)0.,(float)0.,(
	    float)4.,(float)10.,(float)30.,(float)32.,(float)20.,(float)35.,(
	    float)26.,(float)9.,(float)6.,(float)1.,(float)0.,(float)0.,(
	    float)0.,(float)2.,(float)6.,(float)30.,(float)15.,(float)34.,(
	    float)24.,(float)10.,(float)5.,(float)1.,(float)0.,(float)0.,(
	    float)0.,(float)0.,(float)2.,(float)6.,(float)30.,(float)11.,(
	    float)28.,(float)37.,(float)21.,(float)14.,(float)8.,(float)5.,(
	    float)0.,(float)0.,(float)0.,(float)2.,(float)6.,(float)30.,(
	    float)7.,(float)29.,(float)36.,(float)29.,(float)20.,(float)13.,(
	    float)5.,(float)0.,(float)0.,(float)0.,(float)1.,(float)2.,(float)
	    10.,(float)20.,(float)5.,(float)7.,(float)28.,(float)32.,(float)
	    28.,(float)20.,(float)14.,(float)7.,(float)0. };
    static real rk1ms140[91]	/* was [13][7] */ = { (float)2.4,(float)6.,(
	    float).6,(float)-.8,(float)0.,(float).7,(float)0.,(float)-.467,(
	    float)-1.2,(float)-.433,(float)0.,(float)0.,(float)0.,(float)2.4,(
	    float)6.,(float).6,(float)1.2,(float)-.35,(float).4,(float)0.,(
	    float)-.467,(float)-1.2,(float)-.433,(float)0.,(float)0.,(float)
	    0.,(float)2.4,(float)6.,(float).6,(float)3.4,(float)-.75,(float)
	    0.,(float)-.45,(float)-1.2,(float)-.433,(float)0.,(float)0.,(
	    float)0.,(float)0.,(float)1.2,(float)3.333,(float)5.,(float)-.95,(
	    float)0.,(float)-.467,(float)-.8,(float)-.433,(float)-.4,(float)
	    0.,(float)0.,(float)0.,(float)0.,(float)1.2,(float)3.333,(float)
	    5.8,(float)-.9,(float)-.363,(float)-.8,(float)-.3,(float)-.35,(
	    float)-.3,(float)0.,(float)0.,(float)0.,(float)0.,(float)1.2,(
	    float)3.333,(float)6.6,(float)-.733,(float)-.329,(float)-.65,(
	    float)-.433,(float)-.6,(float)-.267,(float)0.,(float)0.,(float)0.,
	    (float)0.,(float)1.4,(float)2.4,(float)4.,(float)7.,(float)-.2,(
	    float)-.7,(float)-.3,(float)-.1,(float)-.9,(float)-.467,(float)
	    -.65,(float)-.333,(float)0. };
    static real rk2ms140[91]	/* was [13][7] */ = { (float)1.2,(float)2.,(
	    float).4,(float)1.8,(float)0.,(float)-1.1,(float)0.,(float)-.444,(
	    float)-.2,(float)-.166,(float)0.,(float)0.,(float)0.,(float)1.2,(
	    float)2.,(float).4,(float)-.2,(float).35,(float)-.7,(float)-.067,(
	    float)-.5,(float)-.15,(float)-.166,(float)0.,(float)0.,(float)0.,(
	    float)1.2,(float)2.,(float).4,(float)-2.4,(float).75,(float)-.2,(
	    float)-.486,(float)-.15,(float)-.166,(float)0.,(float)0.,(float)
	    0.,(float)0.,(float).8,(float)1.6,(float)-3.,(float).95,(float)
	    -.167,(float)-.7,(float)-.1,(float)-.2,(float)0.,(float)0.,(float)
	    0.,(float)0.,(float)0.,(float).8,(float)1.6,(float)-3.8,(float)
	    .85,(float).225,(float)-.4,(float)-.35,(float)-.2,(float)-.15,(
	    float)-.133,(float)0.,(float)0.,(float)0.,(float).8,(float)1.6,(
	    float)-4.6,(float).733,(float).233,(float)-.175,(float)-.45,(
	    float)-.233,(float)-.4,(float)-.1,(float)0.,(float)0.,(float)0.,(
	    float).2,(float).8,(float)2.,(float)-3.,(float).2,(float).7,(
	    float).1,(float)-.2,(float)-.4,(float)-.2,(float)-.35,(float)
	    -.167,(float)0. };
    static integer j1mr70[7] = { 12,12,12,9,10,11,13 };
    static integer j2mr70[7] = { 9,9,10,13,12,11,11 };
    static real h1r70[91]	/* was [13][7] */ = { (float)75.,(float)80.,(
	    float)90.,(float)95.,(float)100.,(float)120.,(float)140.,(float)
	    180.,(float)200.,(float)220.,(float)250.,(float)270.,(float)0.,(
	    float)75.,(float)80.,(float)90.,(float)95.,(float)100.,(float)
	    120.,(float)145.,(float)180.,(float)200.,(float)220.,(float)250.,(
	    float)270.,(float)0.,(float)75.,(float)80.,(float)90.,(float)95.,(
	    float)100.,(float)120.,(float)145.,(float)180.,(float)200.,(float)
	    220.,(float)250.,(float)270.,(float)0.,(float)75.,(float)95.,(
	    float)100.,(float)110.,(float)140.,(float)180.,(float)200.,(float)
	    250.,(float)270.,(float)0.,(float)0.,(float)0.,(float)0.,(float)
	    75.,(float)95.,(float)125.,(float)150.,(float)185.,(float)195.,(
	    float)200.,(float)220.,(float)250.,(float)270.,(float)0.,(float)
	    0.,(float)0.,(float)75.,(float)95.,(float)100.,(float)150.,(float)
	    160.,(float)170.,(float)190.,(float)200.,(float)220.,(float)250.,(
	    float)270.,(float)0.,(float)0.,(float)75.,(float)80.,(float)85.,(
	    float)95.,(float)100.,(float)140.,(float)160.,(float)170.,(float)
	    190.,(float)200.,(float)220.,(float)250.,(float)270. };
    static real h2r70[91]	/* was [13][7] */ = { (float)75.,(float)95.,(
	    float)100.,(float)120.,(float)180.,(float)200.,(float)220.,(float)
	    250.,(float)270.,(float)0.,(float)0.,(float)0.,(float)0.,(float)
	    75.,(float)95.,(float)100.,(float)120.,(float)180.,(float)200.,(
	    float)220.,(float)250.,(float)270.,(float)0.,(float)0.,(float)0.,(
	    float)0.,(float)75.,(float)95.,(float)100.,(float)120.,(float)
	    130.,(float)190.,(float)200.,(float)220.,(float)250.,(float)270.,(
	    float)0.,(float)0.,(float)0.,(float)75.,(float)80.,(float)85.,(
	    float)95.,(float)100.,(float)110.,(float)130.,(float)180.,(float)
	    190.,(float)200.,(float)220.,(float)250.,(float)270.,(float)75.,(
	    float)80.,(float)85.,(float)95.,(float)100.,(float)125.,(float)
	    150.,(float)190.,(float)200.,(float)220.,(float)250.,(float)270.,(
	    float)0.,(float)75.,(float)80.,(float)85.,(float)95.,(float)100.,(
	    float)150.,(float)190.,(float)200.,(float)220.,(float)250.,(float)
	    270.,(float)0.,(float)0.,(float)75.,(float)85.,(float)95.,(float)
	    100.,(float)140.,(float)180.,(float)190.,(float)200.,(float)220.,(
	    float)250.,(float)270.,(float)0.,(float)0. };
    static real r1mr70[91]	/* was [13][7] */ = { (float)13.,(float)17.,(
	    float)57.,(float)57.,(float)30.,(float)53.,(float)58.,(float)38.,(
	    float)33.,(float)14.,(float)6.,(float)2.,(float)0.,(float)13.,(
	    float)17.,(float)57.,(float)57.,(float)37.,(float)56.,(float)56.,(
	    float)38.,(float)33.,(float)14.,(float)6.,(float)2.,(float)0.,(
	    float)13.,(float)17.,(float)57.,(float)57.,(float)47.,(float)58.,(
	    float)55.,(float)37.,(float)33.,(float)14.,(float)6.,(float)2.,(
	    float)0.,(float)5.,(float)65.,(float)54.,(float)58.,(float)58.,(
	    float)38.,(float)33.,(float)9.,(float)1.,(float)0.,(float)0.,(
	    float)0.,(float)0.,(float)5.,(float)65.,(float)65.,(float)54.,(
	    float)40.,(float)40.,(float)45.,(float)26.,(float)17.,(float)10.,(
	    float)0.,(float)0.,(float)0.,(float)5.,(float)65.,(float)76.,(
	    float)56.,(float)57.,(float)48.,(float)44.,(float)51.,(float)35.,(
	    float)22.,(float)10.,(float)0.,(float)0.,(float)3.,(float)11.,(
	    float)35.,(float)75.,(float)90.,(float)65.,(float)63.,(float)54.,(
	    float)54.,(float)50.,(float)40.,(float)26.,(float)13. };
    static real r2mr70[91]	/* was [13][7] */ = { (float)7.,(float)43.,(
	    float)70.,(float)47.,(float)15.,(float)17.,(float)10.,(float)4.,(
	    float)0.,(float)0.,(float)0.,(float)0.,(float)0.,(float)7.,(float)
	    43.,(float)63.,(float)44.,(float)17.,(float)17.,(float)10.,(float)
	    4.,(float)0.,(float)0.,(float)0.,(float)0.,(float)0.,(float)7.,(
	    float)43.,(float)53.,(float)42.,(float)42.,(float)13.,(float)17.,(
	    float)10.,(float)4.,(float)0.,(float)0.,(float)0.,(float)0.,(
	    float)3.,(float)5.,(float)26.,(float)34.,(float)46.,(float)42.,(
	    float)41.,(float)23.,(float)16.,(float)16.,(float)10.,(float)1.,(
	    float)0.,(float)3.,(float)5.,(float)26.,(float)34.,(float)35.,(
	    float)35.,(float)42.,(float)25.,(float)22.,(float)14.,(float)8.,(
	    float)5.,(float)0.,(float)3.,(float)5.,(float)26.,(float)34.,(
	    float)24.,(float)41.,(float)31.,(float)26.,(float)20.,(float)13.,(
	    float)5.,(float)0.,(float)0.,(float)3.,(float)15.,(float)15.,(
	    float)10.,(float)35.,(float)35.,(float)30.,(float)34.,(float)20.,(
	    float)14.,(float)7.,(float)0.,(float)0. };
    static real rk1mr70[91]	/* was [13][7] */ = { (float).8,(float)4.,(
	    float)0.,(float)-5.4,(float)1.15,(float).25,(float)-.5,(float)
	    -.25,(float)-.95,(float)-.267,(float)-.2,(float)-.067,(float)0.,(
	    float).8,(float)4.,(float)0.,(float)-4.,(float).95,(float)0.,(
	    float)-.514,(float)-.25,(float)-.95,(float)-.267,(float)-.2,(
	    float)-.067,(float)0.,(float).8,(float)4.,(float)0.,(float)-2.,(
	    float).55,(float)-.12,(float)-.514,(float)-.2,(float)-.95,(float)
	    -.267,(float)-.2,(float)-.067,(float)0.,(float)3.,(float)-2.2,(
	    float).4,(float)0.,(float)-.5,(float)-.25,(float)-.48,(float)-.4,(
	    float)-.033,(float)0.,(float)0.,(float)0.,(float)0.,(float)3.,(
	    float)0.,(float)-.44,(float)-.466,(float)0.,(float)1.,(float)-.95,
	    (float)-.3,(float)-.35,(float)-.3,(float)0.,(float)0.,(float)0.,(
	    float)3.,(float)2.2,(float)-.4,(float).1,(float)-.9,(float)-.2,(
	    float).7,(float)-.8,(float)-.433,(float)-.6,(float)-.267,(float)
	    0.,(float)0.,(float)1.6,(float)4.8,(float)4.,(float)3.,(float)
	    -.625,(float)-.1,(float)-.9,(float)0.,(float)-.4,(float)-.5,(
	    float)-.467,(float)-.65,(float)-.3 };
    static real rk2mr70[91]	/* was [13][7] */ = { (float)1.8,(float)5.4,(
	    float)-1.15,(float)-.533,(float).1,(float)-.35,(float)-.2,(float)
	    -.2,(float)0.,(float)0.,(float)0.,(float)0.,(float)0.,(float)1.8,(
	    float)4.,(float)-.95,(float)-.45,(float)0.,(float)-.35,(float)-.2,
	    (float)-.2,(float)0.,(float)0.,(float)0.,(float)0.,(float)0.,(
	    float)1.8,(float)2.,(float)-.55,(float)0.,(float)-.483,(float).4,(
	    float)-.35,(float)-.2,(float)-.2,(float)0.,(float)0.,(float)0.,(
	    float)0.,(float).4,(float)4.2,(float).8,(float)2.4,(float)-.4,(
	    float)-.05,(float)-.36,(float)-.7,(float)0.,(float)-.3,(float)-.3,
	    (float)-.05,(float)0.,(float).4,(float)4.2,(float).8,(float).2,(
	    float)0.,(float).28,(float)-.425,(float)-.3,(float)-.4,(float)-.2,
	    (float)-.15,(float)-.133,(float)0.,(float).4,(float)4.2,(float).8,
	    (float)-2.,(float).34,(float)-.25,(float)-.5,(float)-.3,(float)
	    -.233,(float)-.4,(float)-.1,(float)0.,(float)0.,(float)1.2,(float)
	    0.,(float)-1.,(float).625,(float)0.,(float)-.5,(float).4,(float)
	    -.7,(float)-.2,(float)-.35,(float)-.167,(float)0.,(float)0. };
    static integer j1mr140[7] = { 12,12,11,12,9,9,13 };
    static integer j2mr140[7] = { 10,9,10,12,13,13,12 };
    static real h1r140[91]	/* was [13][7] */ = { (float)75.,(float)80.,(
	    float)90.,(float)95.,(float)100.,(float)115.,(float)130.,(float)
	    145.,(float)200.,(float)220.,(float)250.,(float)270.,(float)0.,(
	    float)75.,(float)80.,(float)90.,(float)95.,(float)100.,(float)
	    110.,(float)120.,(float)145.,(float)200.,(float)220.,(float)250.,(
	    float)270.,(float)0.,(float)75.,(float)80.,(float)90.,(float)95.,(
	    float)100.,(float)115.,(float)150.,(float)200.,(float)220.,(float)
	    250.,(float)270.,(float)0.,(float)0.,(float)75.,(float)95.,(float)
	    100.,(float)120.,(float)130.,(float)140.,(float)150.,(float)190.,(
	    float)200.,(float)220.,(float)250.,(float)270.,(float)0.,(float)
	    75.,(float)95.,(float)120.,(float)150.,(float)190.,(float)200.,(
	    float)220.,(float)250.,(float)270.,(float)0.,(float)0.,(float)0.,(
	    float)0.,(float)75.,(float)95.,(float)100.,(float)145.,(float)
	    190.,(float)200.,(float)220.,(float)250.,(float)270.,(float)0.,(
	    float)0.,(float)0.,(float)0.,(float)75.,(float)80.,(float)85.,(
	    float)95.,(float)100.,(float)120.,(float)160.,(float)170.,(float)
	    190.,(float)200.,(float)220.,(float)250.,(float)270. };
    static real h2r140[91]	/* was [13][7] */ = { (float)75.,(float)95.,(
	    float)100.,(float)115.,(float)130.,(float)175.,(float)200.,(float)
	    220.,(float)250.,(float)270.,(float)0.,(float)0.,(float)0.,(float)
	    75.,(float)95.,(float)100.,(float)110.,(float)175.,(float)200.,(
	    float)220.,(float)250.,(float)270.,(float)0.,(float)0.,(float)0.,(
	    float)0.,(float)75.,(float)95.,(float)100.,(float)115.,(float)
	    130.,(float)180.,(float)200.,(float)220.,(float)250.,(float)270.,(
	    float)0.,(float)0.,(float)0.,(float)75.,(float)80.,(float)85.,(
	    float)95.,(float)100.,(float)120.,(float)130.,(float)190.,(float)
	    200.,(float)220.,(float)250.,(float)270.,(float)0.,(float)75.,(
	    float)80.,(float)85.,(float)95.,(float)100.,(float)120.,(float)
	    140.,(float)160.,(float)190.,(float)200.,(float)220.,(float)250.,(
	    float)270.,(float)75.,(float)80.,(float)85.,(float)95.,(float)
	    100.,(float)145.,(float)165.,(float)180.,(float)190.,(float)200.,(
	    float)220.,(float)250.,(float)270.,(float)75.,(float)85.,(float)
	    95.,(float)100.,(float)120.,(float)145.,(float)170.,(float)190.,(
	    float)200.,(float)220.,(float)250.,(float)270.,(float)0. };
    static real r1mr140[91]	/* was [13][7] */ = { (float)13.,(float)17.,(
	    float)57.,(float)57.,(float)28.,(float)51.,(float)56.,(float)56.,(
	    float)12.,(float)8.,(float)1.,(float)0.,(float)0.,(float)13.,(
	    float)17.,(float)57.,(float)57.,(float)36.,(float)46.,(float)55.,(
	    float)56.,(float)10.,(float)8.,(float)1.,(float)0.,(float)0.,(
	    float)13.,(float)17.,(float)57.,(float)57.,(float)46.,(float)56.,(
	    float)55.,(float)12.,(float)8.,(float)1.,(float)0.,(float)0.,(
	    float)0.,(float)5.,(float)65.,(float)54.,(float)59.,(float)56.,(
	    float)56.,(float)53.,(float)23.,(float)16.,(float)13.,(float)3.,(
	    float)1.,(float)0.,(float)5.,(float)65.,(float)65.,(float)54.,(
	    float)29.,(float)16.,(float)16.,(float)10.,(float)2.,(float)0.,(
	    float)0.,(float)0.,(float)0.,(float)5.,(float)65.,(float)76.,(
	    float)58.,(float)36.,(float)25.,(float)20.,(float)12.,(float)7.,(
	    float)0.,(float)0.,(float)0.,(float)0.,(float)3.,(float)11.,(
	    float)35.,(float)75.,(float)91.,(float)76.,(float)58.,(float)49.,(
	    float)45.,(float)32.,(float)28.,(float)20.,(float)12. };
    static real r2mr140[91]	/* was [13][7] */ = { (float)7.,(float)43.,(
	    float)72.,(float)49.,(float)44.,(float)14.,(float)7.,(float)4.,(
	    float)1.,(float)0.,(float)0.,(float)0.,(float)0.,(float)7.,(float)
	    43.,(float)64.,(float)51.,(float)14.,(float)7.,(float)4.,(float)
	    1.,(float)0.,(float)0.,(float)0.,(float)0.,(float)0.,(float)7.,(
	    float)43.,(float)54.,(float)44.,(float)44.,(float)13.,(float)7.,(
	    float)4.,(float)1.,(float)0.,(float)0.,(float)0.,(float)0.,(float)
	    3.,(float)5.,(float)26.,(float)34.,(float)46.,(float)41.,(float)
	    44.,(float)9.,(float)11.,(float)7.,(float)2.,(float)1.,(float)0.,(
	    float)3.,(float)5.,(float)26.,(float)34.,(float)35.,(float)35.,(
	    float)40.,(float)40.,(float)16.,(float)14.,(float)9.,(float)5.,(
	    float)2.,(float)3.,(float)5.,(float)26.,(float)34.,(float)24.,(
	    float)40.,(float)40.,(float)32.,(float)19.,(float)20.,(float)10.,(
	    float)7.,(float)3.,(float)3.,(float)15.,(float)15.,(float)9.,(
	    float)24.,(float)35.,(float)40.,(float)28.,(float)28.,(float)20.,(
	    float)10.,(float)8.,(float)0. };
    static real rk1mr140[91]	/* was [13][7] */ = { (float).8,(float)4.,(
	    float)0.,(float)-5.8,(float)1.533,(float).333,(float)0.,(float)
	    -.8,(float)-.2,(float)-.233,(float)-.05,(float)0.,(float)0.,(
	    float).8,(float)4.,(float)0.,(float)-4.2,(float)1.3,(float).6,(
	    float).04,(float)-.836,(float)-.1,(float)-.233,(float)-.05,(float)
	    0.,(float)0.,(float).8,(float)4.,(float)0.,(float)-2.2,(float)
	    .667,(float)-.029,(float)-.86,(float)-.2,(float)-.233,(float)-.05,
	    (float)0.,(float)0.,(float)0.,(float)3.,(float)-2.2,(float).25,(
	    float)-.3,(float)0.,(float)-.3,(float)-.75,(float)-.7,(float)-.15,
	    (float)-.333,(float)-.1,(float)-.033,(float)0.,(float)3.,(float)
	    0.,(float)-.367,(float)-.625,(float)-1.3,(float)0.,(float)-.2,(
	    float)-.4,(float)-.067,(float)0.,(float)0.,(float)0.,(float)0.,(
	    float)3.,(float)2.2,(float)-.4,(float)-.489,(float)-1.1,(float)
	    -.25,(float)-.267,(float)-.25,(float)-.2,(float)0.,(float)0.,(
	    float)0.,(float)0.,(float)1.6,(float)4.8,(float)4.,(float)3.2,(
	    float)-.75,(float)-.45,(float)-.9,(float)-.2,(float)-1.3,(float)
	    -.2,(float)-.267,(float)-.4,(float)-.3 };
    static real rk2mr140[91]	/* was [13][7] */ = { (float)1.8,(float)5.8,(
	    float)-1.533,(float)-.333,(float)-.667,(float)-.28,(float)-.15,(
	    float)-.1,(float)-.05,(float)0.,(float)0.,(float)0.,(float)0.,(
	    float)1.8,(float)4.2,(float)-1.3,(float)-.569,(float)-.28,(float)
	    -.15,(float)-.1,(float)-.05,(float)0.,(float)0.,(float)0.,(float)
	    0.,(float)0.,(float)1.8,(float)2.2,(float)-.667,(float)0.,(float)
	    -.62,(float)-.3,(float)-.15,(float)-.1,(float)-.05,(float)0.,(
	    float)0.,(float)0.,(float)0.,(float).4,(float)4.2,(float).8,(
	    float)2.4,(float)-.25,(float).3,(float)-.583,(float).2,(float)-.2,
	    (float)-.167,(float)-.05,(float)-.033,(float)0.,(float).4,(float)
	    4.2,(float).8,(float).02,(float)0.,(float).25,(float)0.,(float)
	    -.6,(float)-.2,(float)-.25,(float)-.133,(float)-.15,(float)-.067,(
	    float).4,(float)4.2,(float).8,(float)-2.,(float).356,(float)0.,(
	    float)-.533,(float)-1.3,(float).1,(float)-.5,(float)-.1,(float)
	    -.2,(float)-.1,(float)1.2,(float)0.,(float)-1.2,(float).75,(float)
	    .44,(float).2,(float)-.6,(float)0.,(float)-.4,(float)-.333,(float)
	    -.1,(float)-.2,(float)0. };
    static integer j1mw70[7] = { 13,13,13,13,9,8,9 };
    static integer j2mw70[7] = { 10,10,11,11,9,8,11 };
    static real h1w70[91]	/* was [13][7] */ = { (float)75.,(float)80.,(
	    float)85.,(float)95.,(float)100.,(float)110.,(float)125.,(float)
	    145.,(float)180.,(float)200.,(float)220.,(float)250.,(float)270.,(
	    float)75.,(float)80.,(float)85.,(float)95.,(float)100.,(float)
	    110.,(float)120.,(float)150.,(float)180.,(float)200.,(float)220.,(
	    float)250.,(float)270.,(float)75.,(float)80.,(float)85.,(float)
	    95.,(float)100.,(float)110.,(float)120.,(float)155.,(float)180.,(
	    float)200.,(float)220.,(float)250.,(float)270.,(float)75.,(float)
	    80.,(float)90.,(float)100.,(float)110.,(float)120.,(float)140.,(
	    float)160.,(float)190.,(float)200.,(float)220.,(float)250.,(float)
	    270.,(float)75.,(float)80.,(float)90.,(float)110.,(float)150.,(
	    float)200.,(float)220.,(float)250.,(float)270.,(float)0.,(float)
	    0.,(float)0.,(float)0.,(float)75.,(float)80.,(float)90.,(float)
	    100.,(float)150.,(float)200.,(float)250.,(float)270.,(float)0.,(
	    float)0.,(float)0.,(float)0.,(float)0.,(float)75.,(float)80.,(
	    float)90.,(float)100.,(float)120.,(float)130.,(float)140.,(float)
	    200.,(float)270.,(float)0.,(float)0.,(float)0.,(float)0. };
    static real h2w70[91]	/* was [13][7] */ = { (float)75.,(float)90.,(
	    float)95.,(float)100.,(float)110.,(float)125.,(float)190.,(float)
	    200.,(float)250.,(float)270.,(float)0.,(float)0.,(float)0.,(float)
	    75.,(float)90.,(float)95.,(float)100.,(float)110.,(float)125.,(
	    float)190.,(float)200.,(float)250.,(float)270.,(float)0.,(float)
	    0.,(float)0.,(float)75.,(float)90.,(float)95.,(float)100.,(float)
	    110.,(float)120.,(float)145.,(float)190.,(float)200.,(float)250.,(
	    float)270.,(float)0.,(float)0.,(float)75.,(float)80.,(float)95.,(
	    float)100.,(float)110.,(float)120.,(float)150.,(float)200.,(float)
	    220.,(float)250.,(float)270.,(float)0.,(float)0.,(float)75.,(
	    float)80.,(float)90.,(float)95.,(float)110.,(float)145.,(float)
	    200.,(float)250.,(float)270.,(float)0.,(float)0.,(float)0.,(float)
	    0.,(float)75.,(float)80.,(float)90.,(float)100.,(float)140.,(
	    float)150.,(float)200.,(float)250.,(float)0.,(float)0.,(float)0.,(
	    float)0.,(float)0.,(float)75.,(float)80.,(float)85.,(float)90.,(
	    float)100.,(float)120.,(float)130.,(float)140.,(float)160.,(float)
	    200.,(float)270.,(float)0.,(float)0. };
    static real r1mw70[91]	/* was [13][7] */ = { (float)28.,(float)35.,(
	    float)65.,(float)65.,(float)28.,(float)44.,(float)46.,(float)50.,(
	    float)25.,(float)25.,(float)10.,(float)5.,(float)0.,(float)28.,(
	    float)35.,(float)65.,(float)65.,(float)36.,(float)49.,(float)47.,(
	    float)47.,(float)25.,(float)25.,(float)10.,(float)5.,(float)0.,(
	    float)28.,(float)35.,(float)65.,(float)65.,(float)48.,(float)54.,(
	    float)51.,(float)43.,(float)25.,(float)25.,(float)10.,(float)5.,(
	    float)0.,(float)16.,(float)24.,(float)66.,(float)54.,(float)58.,(
	    float)50.,(float)50.,(float)38.,(float)25.,(float)25.,(float)10.,(
	    float)5.,(float)0.,(float)16.,(float)24.,(float)66.,(float)66.,(
	    float)46.,(float)30.,(float)20.,(float)6.,(float)3.,(float)0.,(
	    float)0.,(float)0.,(float)0.,(float)16.,(float)24.,(float)66.,(
	    float)76.,(float)49.,(float)32.,(float)12.,(float)7.,(float)0.,(
	    float)0.,(float)0.,(float)0.,(float)0.,(float)6.,(float)19.,(
	    float)67.,(float)91.,(float)64.,(float)68.,(float)60.,(float)40.,(
	    float)12.,(float)0.,(float)0.,(float)0.,(float)0. };
    static real r2mw70[91]	/* was [13][7] */ = { (float)5.,(float)35.,(
	    float)35.,(float)72.,(float)56.,(float)54.,(float)12.,(float)12.,(
	    float)2.,(float)0.,(float)0.,(float)0.,(float)0.,(float)5.,(float)
	    35.,(float)35.,(float)64.,(float)51.,(float)53.,(float)12.,(float)
	    12.,(float)2.,(float)0.,(float)0.,(float)0.,(float)0.,(float)5.,(
	    float)35.,(float)35.,(float)52.,(float)46.,(float)49.,(float)41.,(
	    float)12.,(float)12.,(float)2.,(float)0.,(float)0.,(float)0.,(
	    float)4.,(float)10.,(float)40.,(float)46.,(float)42.,(float)50.,(
	    float)41.,(float)12.,(float)7.,(float)2.,(float)0.,(float)0.,(
	    float)0.,(float)4.,(float)10.,(float)30.,(float)34.,(float)34.,(
	    float)51.,(float)14.,(float)4.,(float)2.,(float)0.,(float)0.,(
	    float)0.,(float)0.,(float)4.,(float)10.,(float)30.,(float)24.,(
	    float)45.,(float)48.,(float)20.,(float)5.,(float)0.,(float)0.,(
	    float)0.,(float)0.,(float)0.,(float)2.,(float)6.,(float)17.,(
	    float)23.,(float)9.,(float)36.,(float)32.,(float)40.,(float)40.,(
	    float)20.,(float)6.,(float)0.,(float)0. };
    static real rk1mw70[91]	/* was [13][7] */ = { (float)1.4,(float)6.,(
	    float)0.,(float)-7.4,(float)1.6,(float).133,(float).2,(float)
	    -.714,(float)0.,(float)-.75,(float)-.167,(float)-.25,(float)0.,(
	    float)1.4,(float)6.,(float)0.,(float)-5.8,(float)1.3,(float)-.2,(
	    float)0.,(float)-.733,(float)0.,(float)-.75,(float)-.167,(float)
	    -.25,(float)0.,(float)1.4,(float)6.,(float)0.,(float)-3.4,(float)
	    .6,(float)-.3,(float)-.229,(float)-.72,(float)0.,(float)-.75,(
	    float)-.167,(float)-.25,(float)0.,(float)1.6,(float)4.2,(float)
	    -1.2,(float).4,(float)-.8,(float)0.,(float)-.6,(float)-.433,(
	    float)0.,(float)-.75,(float)-.167,(float)-.25,(float)0.,(float)
	    1.6,(float)4.2,(float)0.,(float)-.5,(float)-.32,(float)-.5,(float)
	    -.467,(float)-.15,(float)-.1,(float)0.,(float)0.,(float)0.,(float)
	    0.,(float)1.6,(float)4.2,(float)1.,(float)-.54,(float)-.34,(float)
	    -.4,(float)-.25,(float)-.2,(float)0.,(float)0.,(float)0.,(float)
	    0.,(float)0.,(float)2.6,(float)4.8,(float)2.4,(float)-1.35,(float)
	    .4,(float)-.8,(float)-.333,(float)-.4,(float)-.3,(float)0.,(float)
	    0.,(float)0.,(float)0. };
    static real rk2mw70[91]	/* was [13][7] */ = { (float)2.,(float)0.,(
	    float)7.4,(float)-1.6,(float)-.133,(float)-.646,(float)0.,(float)
	    -.2,(float)-.1,(float)0.,(float)0.,(float)0.,(float)0.,(float)2.,(
	    float)0.,(float)5.8,(float)-1.3,(float).133,(float)-.631,(float)
	    0.,(float)-.2,(float)-.1,(float)0.,(float)0.,(float)0.,(float)0.,(
	    float)2.,(float)0.,(float)3.4,(float)-.6,(float).3,(float)-.32,(
	    float)-.644,(float)0.,(float)-.2,(float)-.1,(float)0.,(float)0.,(
	    float)0.,(float)1.2,(float)2.,(float)1.2,(float)-.4,(float).8,(
	    float)-.3,(float)-.58,(float)-.25,(float)-.167,(float)-.1,(float)
	    0.,(float)0.,(float)0.,(float)1.2,(float)2.,(float).8,(float)0.,(
	    float).486,(float)-.673,(float)-.2,(float)-.1,(float)-.066,(float)
	    0.,(float)0.,(float)0.,(float)0.,(float)1.2,(float)2.,(float)-.6,(
	    float).525,(float).3,(float)-.56,(float)-.3,(float)-.1,(float)0.,(
	    float)0.,(float)0.,(float)0.,(float)0.,(float).8,(float)2.2,(
	    float)1.2,(float)-1.4,(float)1.35,(float)-.4,(float).8,(float)0.,(
	    float)-.5,(float)-.2,(float)-.167,(float)0.,(float)0. };
    static integer j1mw140[7] = { 12,11,11,11,11,10,12 };
    static integer j2mw140[7] = { 10,11,11,11,11,10,12 };
    static real h1w140[91]	/* was [13][7] */ = { (float)75.,(float)80.,(
	    float)85.,(float)95.,(float)100.,(float)110.,(float)125.,(float)
	    145.,(float)190.,(float)200.,(float)220.,(float)250.,(float)0.,(
	    float)75.,(float)80.,(float)85.,(float)95.,(float)100.,(float)
	    110.,(float)120.,(float)150.,(float)190.,(float)220.,(float)250.,(
	    float)0.,(float)0.,(float)75.,(float)80.,(float)85.,(float)95.,(
	    float)100.,(float)110.,(float)120.,(float)155.,(float)190.,(float)
	    220.,(float)250.,(float)0.,(float)0.,(float)75.,(float)80.,(float)
	    90.,(float)100.,(float)110.,(float)120.,(float)140.,(float)160.,(
	    float)190.,(float)220.,(float)250.,(float)0.,(float)0.,(float)75.,
	    (float)80.,(float)90.,(float)110.,(float)150.,(float)160.,(float)
	    190.,(float)200.,(float)220.,(float)250.,(float)270.,(float)0.,(
	    float)0.,(float)75.,(float)80.,(float)90.,(float)100.,(float)150.,
	    (float)160.,(float)190.,(float)200.,(float)250.,(float)270.,(
	    float)0.,(float)0.,(float)0.,(float)75.,(float)80.,(float)90.,(
	    float)100.,(float)120.,(float)130.,(float)140.,(float)160.,(float)
	    190.,(float)200.,(float)250.,(float)270.,(float)0. };
    static real h2w140[91]	/* was [13][7] */ = { (float)75.,(float)90.,(
	    float)95.,(float)100.,(float)110.,(float)125.,(float)190.,(float)
	    200.,(float)220.,(float)250.,(float)0.,(float)0.,(float)0.,(float)
	    75.,(float)90.,(float)95.,(float)100.,(float)110.,(float)120.,(
	    float)125.,(float)190.,(float)200.,(float)220.,(float)250.,(float)
	    0.,(float)0.,(float)75.,(float)90.,(float)95.,(float)100.,(float)
	    110.,(float)120.,(float)145.,(float)190.,(float)200.,(float)220.,(
	    float)250.,(float)0.,(float)0.,(float)75.,(float)80.,(float)95.,(
	    float)100.,(float)110.,(float)120.,(float)150.,(float)190.,(float)
	    200.,(float)220.,(float)250.,(float)0.,(float)0.,(float)75.,(
	    float)80.,(float)90.,(float)95.,(float)110.,(float)145.,(float)
	    190.,(float)200.,(float)220.,(float)250.,(float)270.,(float)0.,(
	    float)0.,(float)75.,(float)80.,(float)90.,(float)100.,(float)140.,
	    (float)150.,(float)200.,(float)220.,(float)250.,(float)270.,(
	    float)0.,(float)0.,(float)0.,(float)75.,(float)80.,(float)85.,(
	    float)90.,(float)100.,(float)120.,(float)130.,(float)140.,(float)
	    160.,(float)180.,(float)200.,(float)220.,(float)0. };
    static real r1mw140[91]	/* was [13][7] */ = { (float)28.,(float)35.,(
	    float)65.,(float)65.,(float)28.,(float)44.,(float)46.,(float)50.,(
	    float)9.,(float)6.,(float)2.,(float)0.,(float)0.,(float)28.,(
	    float)35.,(float)65.,(float)65.,(float)36.,(float)49.,(float)47.,(
	    float)47.,(float)8.,(float)2.,(float)0.,(float)0.,(float)0.,(
	    float)28.,(float)35.,(float)65.,(float)65.,(float)48.,(float)54.,(
	    float)51.,(float)43.,(float)8.,(float)2.,(float)0.,(float)0.,(
	    float)0.,(float)16.,(float)24.,(float)66.,(float)54.,(float)58.,(
	    float)50.,(float)50.,(float)42.,(float)8.,(float)2.,(float)0.,(
	    float)0.,(float)0.,(float)16.,(float)24.,(float)66.,(float)66.,(
	    float)46.,(float)49.,(float)9.,(float)10.,(float)7.,(float)2.,(
	    float)0.,(float)0.,(float)0.,(float)16.,(float)24.,(float)66.,(
	    float)76.,(float)49.,(float)54.,(float)10.,(float)14.,(float)4.,(
	    float)1.,(float)0.,(float)0.,(float)0.,(float)6.,(float)19.,(
	    float)67.,(float)91.,(float)64.,(float)68.,(float)60.,(float)58.,(
	    float)11.,(float)20.,(float)5.,(float)2.,(float)0. };
    static real r2mw140[91]	/* was [13][7] */ = { (float)5.,(float)35.,(
	    float)35.,(float)72.,(float)56.,(float)54.,(float)5.,(float)5.,(
	    float)1.,(float)0.,(float)0.,(float)0.,(float)0.,(float)5.,(float)
	    35.,(float)35.,(float)64.,(float)51.,(float)53.,(float)53.,(float)
	    5.,(float)5.,(float)1.,(float)0.,(float)0.,(float)0.,(float)5.,(
	    float)35.,(float)35.,(float)52.,(float)46.,(float)49.,(float)41.,(
	    float)5.,(float)5.,(float)1.,(float)0.,(float)0.,(float)0.,(float)
	    4.,(float)10.,(float)40.,(float)46.,(float)42.,(float)50.,(float)
	    41.,(float)5.,(float)5.,(float)1.,(float)0.,(float)0.,(float)0.,(
	    float)4.,(float)10.,(float)30.,(float)34.,(float)34.,(float)51.,(
	    float)10.,(float)5.,(float)3.,(float)1.,(float)0.,(float)0.,(
	    float)0.,(float)4.,(float)10.,(float)30.,(float)24.,(float)45.,(
	    float)48.,(float)4.,(float)2.,(float)1.,(float)0.,(float)0.,(
	    float)0.,(float)0.,(float)2.,(float)6.,(float)17.,(float)23.,(
	    float)9.,(float)36.,(float)32.,(float)40.,(float)39.,(float)29.,(
	    float)1.,(float)0.,(float)0. };
    static real rk1mw140[91]	/* was [13][7] */ = { (float)1.4,(float)6.,(
	    float)0.,(float)-7.4,(float)1.6,(float).133,(float).2,(float)
	    -.911,(float)-.3,(float)-.2,(float)-.066,(float)0.,(float)0.,(
	    float)1.4,(float)6.,(float)0.,(float)-5.8,(float)1.3,(float)-.2,(
	    float)0.,(float)-.975,(float)-.2,(float)-.066,(float)0.,(float)0.,
	    (float)0.,(float)1.4,(float)6.,(float)0.,(float)-3.4,(float).6,(
	    float)-.3,(float)-.229,(float)-1.,(float)-.2,(float)-.066,(float)
	    0.,(float)0.,(float)0.,(float)1.6,(float)4.2,(float)-1.2,(float)
	    .4,(float)-.8,(float)0.,(float)-.4,(float)-1.133,(float)-.2,(
	    float)-.066,(float)0.,(float)0.,(float)0.,(float)1.6,(float)4.2,(
	    float)0.,(float)-.5,(float).3,(float)-1.133,(float).1,(float)-.15,
	    (float)-.166,(float)-.1,(float)0.,(float)0.,(float)0.,(float)1.6,(
	    float)4.2,(float)1.,(float)-.54,(float).5,(float)-1.466,(float).4,
	    (float)-.2,(float)-.15,(float)-.0333,(float)0.,(float)0.,(float)
	    0.,(float)2.6,(float)4.8,(float)2.4,(float)-1.35,(float).4,(float)
	    -.8,(float)-.1,(float)-1.566,(float).9,(float)-.3,(float)-.15,(
	    float)-.05,(float)0. };
    static real rk2mw140[91]	/* was [13][7] */ = { (float)2.,(float)0.,(
	    float)7.4,(float)-1.6,(float)-.133,(float)-.754,(float)0.,(float)
	    -.2,(float)-.033,(float)0.,(float)0.,(float)0.,(float)0.,(float)
	    2.,(float)0.,(float)5.8,(float)-1.3,(float).2,(float)0.,(float)
	    -.738,(float)0.,(float)-.2,(float)-.033,(float)0.,(float)0.,(
	    float)0.,(float)2.,(float)0.,(float)3.4,(float)-.6,(float).3,(
	    float)-.32,(float)-.8,(float)0.,(float)-.2,(float)-.033,(float)0.,
	    (float)0.,(float)0.,(float)1.2,(float)2.,(float)1.2,(float)-.4,(
	    float).8,(float)-.3,(float)-.9,(float)0.,(float)-.2,(float)-.033,(
	    float)0.,(float)0.,(float)0.,(float)1.2,(float)2.,(float).8,(
	    float)0.,(float).486,(float)-.911,(float)-.5,(float)-.1,(float)
	    -.066,(float)-.05,(float)0.,(float)0.,(float)0.,(float)1.2,(float)
	    2.,(float)-.6,(float).525,(float).3,(float)-.88,(float)-.1,(float)
	    -.033,(float)-.05,(float)0.,(float)0.,(float)0.,(float)0.,(float)
	    .8,(float)2.2,(float)1.2,(float)-1.4,(float)1.35,(float)-.4,(
	    float).8,(float)-.05,(float)-.5,(float)-1.4,(float)-.05,(float)0.,
	    (float)0. };

    /* Builtin functions */
    double r_nint(real *);

    /* Local variables */
    static real h__, z__;
    extern /* Subroutine */ int aprok_(integer *, integer *, real *, real *, 
	    real *, real *, real *, real *, real *, real *, real *, real *);
    static real r170, r270, r1140, r2140;

/* ---------------------------------------------------------------- */
/*     INPUT DATA : */
/*      hei  -  altitude in km */
/*      xhi  -  solar zenith angle in degree */
/*      it   -  season (month) */
/*      F    -  10.7cm solar radio flux */
/*     OUTPUT DATA : */
/*     R1 -  NO+ concentration (in percent) */
/*     R2 -  O2+ concentration (in percent) */
/*     R3 -  Cb+ concentration (in percent) */
/*     R4 -  O+  concentration (in percent) */

/*  A.D. Danilov and N.V. Smirnova, Improving the 75 to 300 km ion */
/*  composition model of the IRI, Adv. Space Res. 15, #2, 171-177, 1995. */

/* ----------------------------------------------------------------- */
    h__ = *hei;
    z__ = *xhi;
    if (z__ < (float)20.) {
	z__ = (float)20.;
    }
    if (z__ > (float)90.) {
	z__ = (float)90.;
    }
    if (*it == 1 || *it == 2 || *it == 11 || *it == 12) {
	if (*f < (float)140.) {
	    aprok_(j1mw70, j2mw70, h1w70, h2w70, r1mw70, r2mw70, rk1mw70, 
		    rk2mw70, &h__, &z__, r1, r2);
	    r170 = *r1;
	    r270 = *r2;
	}
	if (*f > (float)70.) {
	    aprok_(j1mw140, j2mw140, h1w140, h2w140, r1mw140, r2mw140, 
		    rk1mw140, rk2mw140, &h__, &z__, r1, r2);
	    r1140 = *r1;
	    r2140 = *r2;
	}
	if (*f > (float)70. && *f < (float)140.) {
	    *r1 = r170 + (r1140 - r170) * (*f - 70) / 70;
	    *r2 = r270 + (r2140 - r270) * (*f - 70) / 70;
	}
    }
    if (*it == 5 || *it == 6 || *it == 7 || *it == 8) {
	if (*f < (float)140.) {
	    aprok_(j1ms70, j2ms70, h1s70, h2s70, r1ms70, r2ms70, rk1ms70, 
		    rk2ms70, &h__, &z__, r1, r2);
	    r170 = *r1;
	    r270 = *r2;
	}
	if (*f > (float)70.) {
	    aprok_(j1ms140, j2ms140, h1s140, h2s140, r1ms140, r2ms140, 
		    rk1ms140, rk2ms140, &h__, &z__, r1, r2);
	    r1140 = *r1;
	    r2140 = *r2;
	}
	if (*f > (float)70. && *f < (float)140.) {
	    *r1 = r170 + (r1140 - r170) * (*f - 70) / 70;
	    *r2 = r270 + (r2140 - r270) * (*f - 70) / 70;
	}
    }
    if (*it == 3 || *it == 4 || *it == 9 || *it == 10) {
	if (*f < (float)140.) {
	    aprok_(j1mr70, j2mr70, h1r70, h2r70, r1mr70, r2mr70, rk1mr70, 
		    rk2mr70, &h__, &z__, r1, r2);
	    r170 = *r1;
	    r270 = *r2;
	}
	if (*f > (float)70.) {
	    aprok_(j1mr140, j2mr140, h1r140, h2r140, r1mr140, r2mr140, 
		    rk1mr140, rk2mr140, &h__, &z__, r1, r2);
	    r1140 = *r1;
	    r2140 = *r2;
	}
	if (*f > (float)70. && *f < (float)140.) {
	    *r1 = r170 + (r1140 - r170) * (*f - 70) / 70;
	    *r2 = r270 + (r2140 - r270) * (*f - 70) / 70;
	}
    }
    *r3 = (float)0.;
    *r4 = (float)0.;
    if (h__ < (float)100.) {
	*r3 = 100 - (*r1 + *r2);
    }
    if (h__ >= (float)100.) {
	*r4 = 100 - (*r1 + *r2);
    }
    if (*r3 < (float)0.) {
	*r3 = (float)0.;
    }
    if (*r4 < (float)0.) {
	*r4 = (float)0.;
    }
    *r1 = r_nint(r1);
    *r2 = r_nint(r2);
    *r3 = r_nint(r3);
    *r4 = r_nint(r4);
/* L300: */
    return 0;
} /* ionco2_ */



/* Subroutine */ int aprok_(integer *j1m, integer *j2m, real *h1, real *h2, 
	real *r1m, real *r2m, real *rk1m, real *rk2m, real *hei, real *xhi, 
	real *r1, real *r2)
{
    /* Initialized data */

    static real zm[7] = { (float)20.,(float)40.,(float)60.,(float)70.,(float)
	    80.,(float)85.,(float)90. };

    /* System generated locals */
    integer i__1;

    /* Local variables */
    static real h__;
    static integer i__;
    static real z__;
    static integer j1, j2, i1, i2, i3;
    static real h01, h02, r01, r02, r11, r12, rk, rk1, rk2;

/* ----------------------------------------------------------------- */
    /* Parameter adjustments */
    rk2m -= 14;
    rk1m -= 14;
    r2m -= 14;
    r1m -= 14;
    h2 -= 14;
    h1 -= 14;
    --j2m;
    --j1m;

    /* Function Body */
    h__ = *hei;
    z__ = *xhi;
    j1 = 1;
    j2 = 1;
    i1 = 1;
    for (i__ = 1; i__ <= 7; ++i__) {
	i1 = i__;
	if (z__ == zm[i__ - 1]) {
	    j1 = 0;
	}
	if (z__ <= zm[i__ - 1]) {
	    goto L11;
	}
/* L1: */
    }
L11:
    i2 = 1;
    i__1 = j1m[i1];
    for (i__ = 2; i__ <= i__1; ++i__) {
	i2 = i__ - 1;
	if (h__ < h1[i__ + i1 * 13]) {
	    goto L22;
	}
	i2 = j1m[i1];
/* L2: */
    }
L22:
    i3 = 1;
    i__1 = j2m[i1];
    for (i__ = 2; i__ <= i__1; ++i__) {
	i3 = i__ - 1;
	if (h__ < h2[i__ + i1 * 13]) {
	    goto L33;
	}
	i3 = j2m[i1];
/* L3: */
    }
L33:
    r01 = r1m[i2 + i1 * 13];
    r02 = r2m[i3 + i1 * 13];
    rk1 = rk1m[i2 + i1 * 13];
    rk2 = rk2m[i3 + i1 * 13];
    h01 = h1[i2 + i1 * 13];
    h02 = h2[i3 + i1 * 13];
    *r1 = r01 + rk1 * (h__ - h01);
    *r2 = r02 + rk2 * (h__ - h02);
    if (j1 == 1) {
	j1 = 0;
	j2 = 0;
	--i1;
	r11 = *r1;
	r12 = *r2;
	goto L11;
    }
    if (j2 == 0) {
	rk = (z__ - zm[i1 - 1]) / (zm[i1] - zm[i1 - 1]);
	*r1 += (r11 - *r1) * rk;
	*r2 += (r12 - *r2) * rk;
    }
    return 0;
} /* aprok_ */



/* Subroutine */ int ioncomp_(real *xy, integer *id, integer *ismo, real *xm, 
	real *hx, real *zd, real *fd, real *fp, real *fs, real *dion)
{
    static integer iddd;
    static real babs, dipl, dimo;
    extern /* Subroutine */ int igrf_sub__(real *, real *, real *, real *, 
	    real *, integer *, real *, real *);
    static real xmlt, h__;
    static integer i__, icode;
    static real diont[7], xlati, ryear;
    static integer month_sea__;
    extern /* Subroutine */ int ionco2_(real *, real *, integer *, real *, 
	    real *, real *, real *, real *);
    static real ro, xl;
    extern /* Subroutine */ int calion_(integer *, real *, real *, real *, 
	    real *, real *, real *, real *, integer *, real *, real *, real *,
	     real *, real *);
    static real xlongi, xioncomp_h__, ro2, xioncomp_n__, xioncomp_o__, rcl, 
	    cov, xhi, rno, xinvdip, xioncomp_he__;

/* ------------------------------------------------------- */
/*       xy      decimal year */
/*       id       day of year */
/*       ismo    seasonal month (Northern Hemisphere January */
/*                   is ismo=1 and so is Southern H. July) */
/*       xm      MLT in hours */
/*       hx      altitude in km */
/*       zd      solar zenith angle in degrees */
/*       fd      latitude in degrees */
/*       fp      longitude in degrees */
/*       fs      10.7cm solar radio flux */
/*       dion(1)   O+  relative density in percent */
/*       dion(2)   H+  relative density in percent */
/*       dion(3)   N+  relative density in percent */
/*       dion(4)   He+ relative density in percent */
/*       dion(5)   NO+ relative density in percent */
/*       dion(6)   O2+ relative density in percent */
/*       dion(7)   Cluster+ relative density in percent */
/* ------------------------------------------------------- */
    /* Parameter adjustments */
    --dion;

    /* Function Body */
    for (i__ = 1; i__ <= 7; ++i__) {
/* L1122: */
	diont[i__ - 1] = (float)0.;
    }
    xmlt = *xm;
    iddd = *id;
    ryear = *xy;
    month_sea__ = *ismo;
    h__ = *hx;
    xhi = *zd;
    xlati = *fd;
    xlongi = *fp;
    cov = *fs;
    if (h__ > (float)300.) {
	igrf_sub__(&xlati, &xlongi, &ryear, &h__, &xl, &icode, &dipl, &babs);
	if (xl > (float)10.) {
	    xl = (float)10.;
	}
/*        	icd=1    ! compute INVDIP */
	dimo = (float).311653;
	calion_(&c__1, &xinvdip, &xl, &dimo, &babs, &dipl, &xmlt, &h__, &iddd,
		 &cov, &xioncomp_o__, &xioncomp_h__, &xioncomp_he__, &
		xioncomp_n__);
	diont[0] = xioncomp_o__ * (float)100.;
	diont[1] = xioncomp_h__ * (float)100.;
	diont[2] = xioncomp_n__ * (float)100.;
	diont[3] = xioncomp_he__ * (float)100.;
    } else {
	ionco2_(&h__, &xhi, &month_sea__, &cov, &rno, &ro2, &rcl, &ro);
	diont[4] = rno;
	diont[5] = ro2;
	diont[6] = rcl;
	diont[0] = ro;
    }
    for (i__ = 1; i__ <= 7; ++i__) {
	dion[i__] = diont[i__ - 1];
/* L1: */
    }
    return 0;
} /* ioncomp_ */



/* Subroutine */ int ionco1_(real *h__, real *zd, real *fd, real *fs, real *t,
	 real *cn)
{
    /* Initialized data */

    static real po[30]	/* was [5][6] */ = { (float)0.,(float)0.,(float)0.,(
	    float)0.,(float)98.5,(float)0.,(float)0.,(float)0.,(float)0.,(
	    float)320.,(float)0.,(float)0.,(float)0.,(float)0.,(float)
	    -2.59e-4,(float)2.79e-4,(float)-.00333,(float)-.00352,(float)
	    -.00516,(float)-.0247,(float)0.,(float)0.,(float)0.,(float)0.,(
	    float)-2.5e-6,(float).00104,(float)-1.79e-4,(float)-4.29e-5,(
	    float)1.01e-5,(float)-.00127 };
    static real ph[30]	/* was [5][6] */ = { (float)-4.97e-7,(float)-.121,(
	    float)-.131,(float)0.,(float)98.1,(float)355.,(float)-191.,(float)
	    -127.,(float)0.,(float)2040.,(float)0.,(float)0.,(float)0.,(float)
	    0.,(float)-4.79e-6,(float)-2e-4,(float)5.67e-4,(float)2.6e-4,(
	    float)0.,(float)-.00508,(float)0.,(float)0.,(float)0.,(float)0.,(
	    float)0.,(float)0.,(float)0.,(float)0.,(float)0.,(float)0. };
    static real pn[30]	/* was [5][6] */ = { (float).76,(float)-5.62,(float)
	    -4.99,(float)0.,(float)5.79,(float)83.,(float)-369.,(float)-324.,(
	    float)0.,(float)593.,(float)0.,(float)0.,(float)0.,(float)0.,(
	    float)-6.3e-5,(float)-.00674,(float)-.00793,(float)-.00465,(float)
	    0.,(float)-.00326,(float)0.,(float)0.,(float)0.,(float)0.,(float)
	    -1.17e-5,(float).00488,(float)-.00131,(float)-7.03e-4,(float)0.,(
	    float)-.00238 };
    static real phe[30]	/* was [5][6] */ = { (float)-.895,(float)6.1,(float)
	    5.39,(float)0.,(float)8.01,(float)0.,(float)0.,(float)0.,(float)
	    0.,(float)1200.,(float)0.,(float)0.,(float)0.,(float)0.,(float)
	    -1.04e-5,(float).0019,(float)9.53e-4,(float).00106,(float)0.,(
	    float)-.00344,(float)0.,(float)0.,(float)0.,(float)0.,(float)0.,(
	    float)0.,(float)0.,(float)0.,(float)0.,(float)0. };

    /* Builtin functions */
    double cos(doublereal), exp(doublereal);

    /* Local variables */
    static real beth[4], betl[4], f;
    static integer i__, j;
    static real p[120]	/* was [5][6][4] */, s, z__, cm[4], hm[4], hx, alh[4],
	     all[4], arg, var[6];

/* --------------------------------------------------------------- */
/* ion composition model */
/*   A.D. Danilov and A.P. Yaichnikov, A New Model of the Ion */
/*   Composition at 75 to 1000 km for IRI, Adv. Space Res. 5, #7, */
/*   75-79, 107-108, 1985 */

/*       h       altitude in km */
/*       zd      solar zenith angle in degrees */
/*       fd      latitude in degrees */
/*       fs      10.7cm solar radio flux */
/*       t       season (decimal month) */
/*       cn(1)   O+  relative density in percent */
/*       cn(2)   H+  relative density in percent */
/*       cn(3)   N+  relative density in percent */
/*       cn(4)   He+ relative density in percent */
/* Please note: molecular ions are now computed in IONCO2 */
/*       [cn(5)   NO+ relative density in percent */
/*       [cn(6)   O2+ relative density in percent */
/*       [cn(7)   cluster ions  relative density in percent */
/* --------------------------------------------------------------- */

/*        dimension       cn(7),cm(7),hm(7),alh(7),all(7),beth(7), */
/*     &                  betl(7),p(5,6,7),var(6),po(5,6),ph(5,6), */
/*     &                  pn(5,6),phe(5,6),pno(5,6),po2(5,6),pcl(5,6) */
    /* Parameter adjustments */
    --cn;

    /* Function Body */
/*       data pno/-22.4,17.7,-13.4,-4.88,62.3,32.7,0.,19.8,2.07,115., */
/*    &          5*0.,3.94E-3,0.,2.48E-3,2.15E-4,6.67E-3,5*0., */
/*    &          -8.4E-3,0.,-3.64E-3,2.E-3,-2.59E-2/ */
/*       data po2/8.,-12.2,9.9,5.8,53.4,-25.2,0.,-28.5,-6.72,120., */
/*    &          5*0.,-1.4E-2,0.,-9.3E-3,3.3E-3,2.8E-2,5*0.,4.25E-3, */
/*    &          0.,-6.04E-3,3.85E-3,-3.64E-2/ */
/*       data pcl/4*0.,100.,4*0.,75.,10*0.,4*0.,-9.04E-3,-7.28E-3, */
/*    &          2*0.,3.46E-3,-2.11E-2/ */
    z__ = *zd * const_1.umr;
    f = *fd * const_1.umr;
    for (i__ = 1; i__ <= 5; ++i__) {
	for (j = 1; j <= 6; ++j) {
	    p[i__ + (j + 6) * 5 - 36] = po[i__ + j * 5 - 6];
	    p[i__ + (j + 12) * 5 - 36] = ph[i__ + j * 5 - 6];
	    p[i__ + (j + 18) * 5 - 36] = pn[i__ + j * 5 - 6];
	    p[i__ + (j + 24) * 5 - 36] = phe[i__ + j * 5 - 6];
/*               p(i,j,5)=pno(i,j) */
/*               p(i,j,6)=po2(i,j) */
/*               p(i,j,7)=pcl(i,j) */
/* L8: */
	}
    }
    s = (float)0.;
/*       do 5 i=1,7 */
    for (i__ = 1; i__ <= 4; ++i__) {
	for (j = 1; j <= 6; ++j) {
	    var[j - 1] = p[(j + i__ * 6) * 5 - 35] * cos(z__) + p[(j + i__ * 
		    6) * 5 - 34] * cos(f) + p[(j + i__ * 6) * 5 - 33] * cos(((
		    float)300. - *fs) * (float).013) + p[(j + i__ * 6) * 5 - 
		    32] * cos((*t - (float)6.) * (float).52) + p[(j + i__ * 6)
		     * 5 - 31];
/* L7: */
	}
	cm[i__ - 1] = var[0];
	hm[i__ - 1] = var[1];
	all[i__ - 1] = var[2];
	betl[i__ - 1] = var[3];
	alh[i__ - 1] = var[4];
	beth[i__ - 1] = var[5];
	hx = *h__ - hm[i__ - 1];
	if (hx < (float)0.) {
	    goto L1;
	} else if (hx == 0) {
	    goto L2;
	} else {
	    goto L3;
	}
L1:
	arg = hx * (hx * all[i__ - 1] + betl[i__ - 1]);
	cn[i__] = (float)0.;
	if (arg > -argexp_1.argmax) {
	    cn[i__] = cm[i__ - 1] * exp(arg);
	}
	goto L4;
L2:
	cn[i__] = cm[i__ - 1];
	goto L4;
L3:
	arg = hx * (hx * alh[i__ - 1] + beth[i__ - 1]);
	cn[i__] = (float)0.;
	if (arg > -argexp_1.argmax) {
	    cn[i__] = cm[i__ - 1] * exp(arg);
	}
L4:
	if (cn[i__] < cm[i__ - 1] * (float).005) {
	    cn[i__] = (float)0.;
	}
	if (cn[i__] > cm[i__ - 1]) {
	    cn[i__] = cm[i__ - 1];
	}
	s += cn[i__];
/* L5: */
    }
/*       do 6 i=1,7 */
    for (i__ = 1; i__ <= 4; ++i__) {
/* L6: */
	cn[i__] = cn[i__] / s * (float)100.;
    }
    return 0;
} /* ionco1_ */



/* Subroutine */ int calion_(integer *crd, real *invdip, real *fl, real *dimo,
	 real *b0, real *dipl, real *mlt, real *alt, integer *ddd, real *f107,
	 real *no, real *nh, real *nhe, real *nn)
{
    /* Initialized data */

    static struct {
	real e_1[8];
	integer fill_2[4];
	real e_3[8];
	integer fill_4[4];
	real e_5[8];
	integer fill_6[4];
	real e_7[8];
	integer fill_8[4];
	real e_9[8];
	integer fill_10[4];
	real e_11[8];
	integer fill_12[4];
	real e_13[8];
	integer fill_14[4];
	real e_15[8];
	integer fill_16[4];
	real e_17[8];
	integer fill_18[4];
	real e_19[8];
	integer fill_20[4];
	real e_21[8];
	integer fill_22[4];
	real e_23[8];
	integer fill_24[4];
	real e_25[8];
	integer fill_26[4];
	real e_27[8];
	integer fill_28[4];
	real e_29[8];
	integer fill_30[4];
	real e_31[8];
	integer fill_32[4];
	real e_33[8];
	integer fill_34[4];
	real e_35[8];
	integer fill_36[4];
	real e_37[8];
	integer fill_38[4];
	real e_39[8];
	integer fill_40[4];
	real e_41[8];
	integer fill_42[4];
	real e_43[8];
	integer fill_44[4];
	real e_45[8];
	integer fill_46[4];
	real e_47[8];
	integer fill_48[4];
	real e_49[8];
	integer fill_50[4];
	real e_51[8];
	integer fill_52[4];
	real e_53[8];
	integer fill_54[4];
	real e_55[8];
	integer fill_56[4];
	real e_57[8];
	integer fill_58[4];
	real e_59[8];
	integer fill_60[4];
	real e_61[8];
	integer fill_62[4];
	real e_63[8];
	integer fill_64[4];
	real e_65[8];
	integer fill_66[4];
	real e_67[8];
	integer fill_68[4];
	real e_69[8];
	integer fill_70[4];
	real e_71[8];
	integer fill_72[4];
	real e_73[8];
	integer fill_74[4];
	real e_75[8];
	integer fill_76[4];
	real e_77[8];
	integer fill_78[4];
	real e_79[8];
	integer fill_80[4];
	real e_81[8];
	integer fill_82[4];
	real e_83[8];
	integer fill_84[4];
	real e_85[8];
	integer fill_86[4];
	real e_87[8];
	integer fill_88[4];
	real e_89[8];
	integer fill_90[4];
	real e_91[8];
	integer fill_92[4];
	real e_93[8];
	integer fill_94[4];
	real e_95[8];
	integer fill_96[4];
	real e_97[8];
	integer fill_98[4];
	} equiv_390 = { (float)-.012838, (float)-.11873, (float)-.50096, (
		float)-.75121, (float)-.013612, (float)-.114, (float)-.38369, 
		(float)-.8219, {0}, (float)3.3892e-9, (float)-7.9543e-8, (
		float)3.9699e-7, (float)4.7106e-6, (float)-.005555, (float)
		.072284, (float).28162, (float).36727, {0}, (float)-.0049527, 
		(float).088754, (float).54222, (float).98731, (float)
		-.0088001, (float).059514, (float).3341, (float).86943, {0}, (
		float)9.6584e-9, (float)1.2749e-7, (float)-4.2933e-7, (float)
		-1.1517e-5, (float)-.0067878, (float)-.092827, (float)-.44591,
		 (float)-.51681, {0}, (float)-2.8249e-4, (float).037834, (
		float).042261, (float)-.21953, (float)-.0025424, (float)
		.02467, (float).12217, (float).043418, {0}, (float)9.2209e-9, 
		(float)-3.7071e-8, (float)-1.2006e-7, (float)7.7259e-6, (
		float)-6.6143e-4, (float).018185, (float).14901, (float)
		.14628, {0}, (float)-.0015708, (float)-.033659, (float)
		-.13304, (float)-.073014, (float)-.0030189, (float)-.0053528, 
		(float)-.12365, (float)-.1619, {0}, (float)-.0015997, (float)
		-.13753, (float)-.57614, (float)-.34116, (float)-5.1701e-4, (
		float)-.10798, (float)-.33668, (float)-.48689, {0}, (float)
		-1.7077e-10, (float)-6.7705e-8, (float)1.995e-7, (float)
		-1.3414e-6, (float)-3.4954e-4, (float).049821, (float).14641, 
		(float).079939, {0}, (float)6.0805e-4, (float)-.007837, (
		float).042972, (float).038833, (float)-2.9469e-5, (float)
		-.0054609, (float).015156, (float).029131, {0}, (float)
		-3.2035e-9, (float)5.0045e-8, (float)-3.6102e-8, (float)
		-6.9663e-7, (float)4.0382e-5, (float)-.0073866, (float)
		-.042536, (float).0094168, {0}, (float)4.6574e-4, (float)
		.017354, (float).036207, (float).010871, (float)3.1389e-6, (
		float).0055822, (float).026524, (float).016115, {0}, (float)
		-3.0838e-9, (float)-1.2451e-8, (float)-8.1212e-9, (float)
		5.1607e-7, (float)5.8065e-5, (float)-.0022066, (float)
		-.0055077, (float)-.011809, {0}, (float)-.0017719, (float)
		-.055262, (float)-.30608, (float)-.52445, (float).0016808, (
		float)-.0094552, (float)-.11957, (float)-.48438, {0}, (float)
		1.7746e-9, (float)-1.6397e-8, (float)7.6853e-8, (float)
		1.5335e-6, (float)-2.0046e-4, (float)-.0070287, (float)
		.060421, (float).10166, {0}, (float).0020281, (float).0090203,
		 (float).06809, (float).11675, (float).0011039, (float)
		.015038, (float)-.0039161, (float).128, {0}, (float)5.955e-10,
		 (float)-3.1764e-9, (float)-5.5044e-8, (float)-9.3384e-7, (
		float).0010451, (float)-.0049536, (float)-.010382, (float)
		-.061316, {0}, (float)-8.4087e-4, (float)3.22e-4, (float)
		-.0046984, (float)-.0089381, (float)-1.0243e-4, (float)
		-.002197, (float).011853, (float)-.016983, {0}, (float)
		7.3645e-10, (float)4.4309e-9, (float)2.6068e-8, (float)
		1.5077e-7, (float)-1.633e-4, (float).0018018, (float)
		-.0079129, (float).014167, {0}, (float).0016083, (float)
		-.047726, (float)-.15518, (float)-.0049602, (float).0013405, (
		float)-.051372, (float).054055, (float).0035405, {0}, (float)
		-1.902e-10, (float)-1.6271e-8, (float)-2.0579e-8, (float)
		-6.4438e-7, (float)-4.031e-4, (float).015935, (float)-.039953,
		 (float)-.0078288, {0}, (float)-5.6389e-4, (float)-.016568, (
		float)-.021672, (float)-.010283, (float)2.964e-4, (float)
		-7.7984e-4, (float).0071875, (float)-.0032292, {0}, (float)
		2.1437e-10, (float)8.7942e-9, (float)9.6017e-9, (float)
		-3.9542e-8, (float)3.3187e-4, (float)-9.883e-4, (float)
		.0060955, (float).0087041, {0}, (float)-9.153e-5, (float)
		.0039707, (float).0076241, (float).0017114, (float)1.5748e-4, 
		(float)-6.1488e-4, (float)-.0030049, (float)-.0036171, {0}, (
		float)-8.1471e-4, (float)-.013873, (float)-.15461, (float)
		-.11767, (float)-.0010201, (float)-.0054875, (float)-.0045588,
		 (float)-.15451, {0}, (float)-1.4259e-9, (float)-1.3387e-8, (
		float)3.9902e-8, (float)2.1163e-7, (float)-3.4733e-4, (float)
		-.0068829, (float)-.0070102, (float)-.011808, {0}, (float)
		-5.3451e-4, (float)-.005541, (float).0039501, (float)
		-.0014566, (float)-1.8801e-4, (float).009091, (float)
		8.8947e-5, (float).028659, {0}, (float)-6.4989e-10, (float)
		3.276e-9, (float)-9.3205e-9, (float)-2.4207e-7, (float)
		1.337e-4, (float)-.003618, (float).0046705, (float)-2.7932e-4,
		 {0}, (float)9.7174e-5, (float).0015026, (float).0036564, (
		float).001979, (float)-3.8261e-5, (float)4.6157e-4, (float)
		-.0015544, (float)-.0027786, {0}, (float)-9.1365e-4, (float)
		5.413e-4, (float)-.018591, (float)-.0088082, (float)2.5237e-4,
		 (float)-.0037871, (float).052816, (float).15649, {0}, (float)
		3.6711e-10, (float)-4.3665e-9, (float)-1.5121e-8, (float)
		-4.2416e-8, (float)9.9983e-5, (float)-9.0714e-5, (float)
		-.032857, (float)-.0048513, {0}, (float)-2.8037e-5, (float)
		-.0085634, (float)-.010493, (float).0010644, (float)5.3315e-5,
		 (float)6.0385e-4, (float).0075772, (float)-.0024998, {0}, (
		float)7.6651e-11, (float)2.2732e-9, (float)5.6094e-9, (float)
		-2.3959e-9, (float)6.1462e-5, (float)1.6097e-4, (float)
		3.3781e-4, (float)-9.9171e-4, {0}, (float).0015422, (float)
		-.0073394, (float).02303, (float).094817, (float)-2.2161e-4, (
		float)-.0072617, (float).053629, (float)-.080071, {0}, (float)
		1.4647e-10, (float)-9.4292e-9, (float)-4.0654e-8, (float)
		-7.9695e-7, (float)3.8285e-4, (float)-.0019084, (float)
		-.02243, (float)-.0024185, {0}, (float)-1.7107e-5, (float)
		-.0041713, (float)-6.5416e-4, (float)-.0041841, (float)
		3.689e-5, (float).0035375, (float).0024118, (float).0064073, {
		0}, (float)2.1872e-10, (float)2.0113e-9, (float)1.286e-10, (
		float)-5.9703e-8, (float)-2.8965e-6, (float)-8.3529e-4, (
		float)3.1271e-4, (float)-2.6383e-4, {0}, (float)1.4265e-4, (
		float).0025152, (float)-.035348, (float).042488, (float)
		-2.6102e-5, (float)-.007738, (float)-.014393, (float)
		-.0047954, {0}, (float)6.1624e-10, (float)-1.4286e-10, (float)
		1.1426e-8, (float)4.4664e-8, (float)2.1884e-5, (float)
		1.0796e-4, (float)-.0066816, (float)-.008672, {0}, (float)
		1.2512e-4, (float)-.0027636, (float)-.0040794, (float)
		.0016134, (float)5.6101e-5, (float)5.0314e-4, (float).0017099,
		 (float).0015965, {0}, (float)4.8548e-4, (float).0014591, (
		float).024538, (float).045698, (float)3.2898e-4, (float)
		-.013759, (float).0042075, (float)-.0071514, {0}, (float)
		-1.6132e-10, (float)-3.7848e-9, (float)-2.8703e-8, (float)
		-2.0829e-7, (float)4.8422e-5, (float)5.3603e-4, (float)
		-.0020012, (float).0070614, {0}, (float)-1.4022e-5, (float)
		-.0021361, (float).0011241, (float)-.0012046, (float)
		8.9837e-7, (float)6.4259e-4, (float)-6.8627e-4, (float)
		-.0018408, {0}, (float)-1.1307e-4, (float)-.00173, (float)
		.0030503, (float)-1.0719e-4, (float)2.0305e-5, (float)
		-.0064764, (float)-.016996, (float)-.011994, {0}, (float)
		1.6024e-10, (float)-6.3066e-12, (float)7.4212e-10, (float)
		1.853e-7, (float)1.3643e-5, (float)-1.6915e-4, (float)
		.0019841, (float)-.0047379, {0}, (float)2.7979e-4, (float)
		-.001819, (float)-.0091099, (float).0085126, (float)1.1488e-4,
		 (float)-.0073788, (float)-.010077, (float)-.020841, {0}, (
		float)6.1773e-11, (float)-1.1929e-9, (float)-6.342e-9, (float)
		4.9934e-8, (float)4.8798e-5, (float)3.5767e-4, (float)
		-4.0022e-4, (float)-.0029637, {0}, (float)-2.7536e-5, (float)
		-.0024384, (float)-.0024364, (float).0077988, (float)
		-1.7122e-6, (float)-.0038287, (float)-.0035827, (float)
		-.0049867, {0}, (float)3.7412e-5, (float).0045096, (float)
		.0078412, (float).0014431, (float)1.3591e-4, (float)-.0032403,
		 (float).0010792, (float)-.014069 };

#define doh ((real *)&equiv_390)

    static struct {
	real e_1[8];
	integer fill_2[4];
	real e_3[8];
	integer fill_4[4];
	real e_5[8];
	integer fill_6[4];
	real e_7[8];
	integer fill_8[4];
	real e_9[8];
	integer fill_10[4];
	real e_11[8];
	integer fill_12[4];
	real e_13[8];
	integer fill_14[4];
	real e_15[8];
	integer fill_16[4];
	real e_17[8];
	integer fill_18[4];
	real e_19[8];
	integer fill_20[4];
	real e_21[8];
	integer fill_22[4];
	real e_23[8];
	integer fill_24[4];
	real e_25[8];
	integer fill_26[4];
	real e_27[8];
	integer fill_28[4];
	real e_29[8];
	integer fill_30[4];
	real e_31[8];
	integer fill_32[4];
	real e_33[8];
	integer fill_34[4];
	real e_35[8];
	integer fill_36[4];
	real e_37[8];
	integer fill_38[4];
	real e_39[8];
	integer fill_40[4];
	real e_41[8];
	integer fill_42[4];
	real e_43[8];
	integer fill_44[4];
	real e_45[8];
	integer fill_46[4];
	real e_47[8];
	integer fill_48[4];
	real e_49[8];
	integer fill_50[4];
	real e_51[8];
	integer fill_52[4];
	real e_53[8];
	integer fill_54[4];
	real e_55[8];
	integer fill_56[4];
	real e_57[8];
	integer fill_58[4];
	real e_59[8];
	integer fill_60[4];
	real e_61[8];
	integer fill_62[4];
	real e_63[8];
	integer fill_64[4];
	real e_65[8];
	integer fill_66[4];
	real e_67[8];
	integer fill_68[4];
	real e_69[8];
	integer fill_70[4];
	real e_71[8];
	integer fill_72[4];
	real e_73[8];
	integer fill_74[4];
	real e_75[8];
	integer fill_76[4];
	real e_77[8];
	integer fill_78[4];
	real e_79[8];
	integer fill_80[4];
	real e_81[8];
	integer fill_82[4];
	real e_83[8];
	integer fill_84[4];
	real e_85[8];
	integer fill_86[4];
	real e_87[8];
	integer fill_88[4];
	real e_89[8];
	integer fill_90[4];
	real e_91[8];
	integer fill_92[4];
	real e_93[8];
	integer fill_94[4];
	real e_95[8];
	integer fill_96[4];
	real e_97[8];
	integer fill_98[4];
	} equiv_391 = { (float)-3.1678, (float)-1.5293, (float)-.81841, (
		float)-.66978, (float)-2.8141, (float)-1.3652, (float)-.815, (
		float)-.56899, {0}, (float)3.6289e-7, (float)1.9563e-7, (
		float)1.3246e-7, (float)4.5698e-7, (float)-.44836, (float)
		-.37141, (float)-.31034, (float)-.25609, {0}, (float)-.055819,
		 (float)-.71593, (float)-.99407, (float)-1.2361, (float)
		.008276, (float)-.69554, (float)-1.0382, (float)-1.0606, {0}, 
		(float)-1.0303e-6, (float)5.0551e-8, (float)-3.5826e-7, (
		float)-1.5676e-7, (float).19407, (float).21456, (float).22304,
		 (float)-.016164, {0}, (float)-.11119, (float)-.10967, (float)
		-.11572, (float)-.12804, (float).028119, (float).091424, (
		float)-.13799, (float)-.19427, {0}, (float)-1.7286e-7, (float)
		1.4302e-7, (float)-3.8968e-7, (float)-6.0241e-7, (float)
		-.026308, (float).040012, (float).062362, (float).10428, {0}, 
		(float).029466, (float).23166, (float).18954, (float).27591, (
		float)-.015432, (float).093601, (float).26902, (float).19448, 
		{0}, (float).69927, (float).70241, (float).26643, (float)
		.040136, (float).48983, (float).48116, (float).26612, (float)
		.11527, {0}, (float)-1.0867e-7, (float)6.7276e-8, (float)
		-1.2399e-8, (float)-1.157e-7, (float).086385, (float).032264, 
		(float)-.047466, (float).016518, {0}, (float)-.038446, (float)
		-.0039113, (float).022029, (float).021493, (float)-.0752, (
		float).068285, (float).019184, (float).036148, {0}, (float)
		-3.1427e-8, (float)-2.3431e-8, (float)3.5262e-8, (float)
		-4.5863e-8, (float).018661, (float).0098315, (float)-.070576, 
		(float)-.0070688, {0}, (float).014009, (float)-.029279, (
		float)-.025895, (float).0026574, (float).004978, (float)
		-.003408, (float)-.017642, (float).010841, {0}, (float)
		4.5348e-8, (float)-2.168e-8, (float)2.5542e-8, (float)
		1.7588e-8, (float)-.016762, (float)-.0011225, (float).014685, 
		(float).0036512, {0}, (float).26142, (float).30214, (float)
		.17279, (float).15964, (float).18896, (float).1262, (float)
		.077755, (float).10339, {0}, (float)9.8583e-8, (float)
		8.5569e-8, (float)-1.0393e-8, (float)1.247e-7, (float)
		-.086811, (float).033225, (float)-.031315, (float)-.0021704, {
		0}, (float)-.01537, (float)-.059577, (float)-.030719, (float)
		.018918, (float)5.1824e-4, (float)-.04842, (float).019684, (
		float)-.0070271, {0}, (float)-8.8257e-8, (float)-7.4572e-8, (
		float)-6.3703e-9, (float)9.3632e-8, (float)-.0064436, (float)
		-.028323, (float)-.013324, (float).019422, {0}, (float)
		.011618, (float).013847, (float).0075848, (float)-.0024488, (
		float).0035544, (float)-.0022914, (float).0034012, (float)
		.0041677, {0}, (float)9.8907e-8, (float)1.9007e-8, (float)
		-1.4497e-8, (float)2.0725e-8, (float).0094714, (float).007972,
		 (float).037043, (float)-.0066189, {0}, (float)-.054724, (
		float).091829, (float).09787, (float).066975, (float).020099, 
		(float).12455, (float)-.018646, (float).043364, {0}, (float)
		2.2519e-7, (float)-8.0079e-8, (float)-1.3003e-7, (float)
		1.1782e-8, (float).019941, (float)-.042949, (float).026457, (
		float)-.025002, {0}, (float).015621, (float).026453, (float)
		-.0020224, (float).019413, (float).025581, (float)-.0053643, (
		float).0098637, (float).014219, {0}, (float)-1.0045e-8, (
		float)2.1214e-8, (float)-3.9366e-8, (float)6.2877e-9, (float)
		-.0052659, (float).0067441, (float).0026819, (float)-.0022654,
		 {0}, (float).0036467, (float)-.0032018, (float)-.0032416, (
		float).0053689, (float)-.0034378, (float).014582, (float)
		.0017598, (float).005644, {0}, (float)-.12359, (float)-.21978,
		 (float).015904, (float)-.015363, (float)-.12902, (float)
		-.040379, (float)-.021755, (float)-.037866, {0}, (float)
		4.9284e-8, (float)1.733e-9, (float)-1.5451e-8, (float)
		-2.0022e-9, (float).014578, (float).038848, (float).0042552, (
		float)1.0338e-4, {0}, (float)-.011143, (float)-.0011862, (
		float)-.0035652, (float)-.021002, (float)-.0025016, (float)
		-.0032937, (float)-.016633, (float)-.0039639, {0}, (float)
		2.3898e-9, (float)2.2427e-8, (float)-1.3658e-9, (float)
		-6.7078e-9, (float)-.02076, (float).0091013, (float)-.0067044,
		 (float)-.0033311, {0}, (float)-.0058604, (float).0013491, (
		float)-.0077126, (float)-.0029894, (float).0034195, (float)
		-.0075494, (float).0034964, (float)-3.3289e-4, {0}, (float)
		-.001002, (float).028588, (float)-.015924, (float).0061202, (
		float)-.082338, (float)-.06258, (float)-.0048719, (float)
		-.052423, {0}, (float)-1.1869e-7, (float)-5.3489e-8, (float)
		1.6773e-8, (float)1.2119e-8, (float).0050698, (float).011877, 
		(float).005979, (float).002543, {0}, (float)-8.7861e-4, (
		float).011135, (float)-.0037276, (float)-.0022888, (float)
		-3.4081e-4, (float)-.00489, (float)2.3021e-5, (float)
		-.0071279, {0}, (float)-1.0543e-8, (float)-1.3678e-9, (float)
		1.7795e-8, (float)4.3298e-9, (float)-.0027512, (float)
		-.005377, (float).0054778, (float).0032787, {0}, (float)
		.098818, (float).079619, (float).0074727, (float)-.046405, (
		float)-.040443, (float).015066, (float)-.012598, (float)
		.0059375, {0}, (float)7.4183e-8, (float)1.5714e-8, (float)
		3.0856e-8, (float)-5.8474e-8, (float).0022658, (float)
		-9.2034e-4, (float).0099962, (float).0072932, {0}, (float)
		.013692, (float).007193, (float)7.4715e-4, (float)-.0036078, (
		float)-.0080779, (float)-.007237, (float)-.0010507, (float)
		-.0019167, {0}, (float)-1.6456e-8, (float)2.9905e-9, (float)
		7.3372e-9, (float)-1.6963e-8, (float)-.003129, (float)
		-3.5355e-4, (float).0057994, (float)-.0018983, {0}, (float)
		-.038041, (float)-.038042, (float).013716, (float)-.018254, (
		float)-.001535, (float).0086705, (float).020765, (float)
		.0045326, {0}, (float)3.5978e-8, (float)1.1725e-8, (float)
		4.6691e-8, (float)-1.0377e-9, (float).0025313, (float).010198,
		 (float).011593, (float).0068099, {0}, (float)-.0039582, (
		float).0034274, (float).0040197, (float)-.0013715, (float)
		-.003474, (float).0020618, (float)-3.2365e-4, (float)
		-7.4999e-5, {0}, (float).028077, (float)-.0049647, (float)
		-.011936, (float)-.0022111, (float).0082225, (float).05721, (
		float)-.0015353, (float)-.0078634, {0}, (float)-2.2537e-8, (
		float)2.1675e-9, (float)5.822e-9, (float)3.4185e-9, (float)
		-.0026921, (float).0068151, (float).0014942, (float)-.0055813,
		 {0}, (float).0015597, (float).0079079, (float).0038995, (
		float)9.5343e-4, (float).0013247, (float)-.0018874, (float)
		.0019642, (float)-5.3892e-4, {0}, (float)-.003761, (float)
		-.0079338, (float).0072715, (float)-.0022094, (float).0057483,
		 (float).017634, (float).0088836, (float)8.528e-4, {0}, (
		float)-2.4712e-8, (float)3.6623e-8, (float)-2.0243e-8, (float)
		2.7066e-9, (float).0040066, (float).0098946, (float).0042172, 
		(float).0014028, {0}, (float).004046, (float)8.5068e-4, (
		float).0041793, (float)-.0024215, (float).013048, (float)
		.018101, (float)-1.8502e-4, (float).0035032, {0}, (float)
		7.7021e-9, (float)1.7537e-9, (float)-3.2785e-9, (float)
		-3.1546e-9, (float)1.6612e-4, (float).0053287, (float)
		.0023092, (float)-3.7237e-4, {0}, (float)-.0014101, (float)
		.023308, (float).013978, (float)-.0010581, (float).0065345, (
		float).030321, (float).001837, (float)-2.5767e-5, {0}, (float)
		.016789, (float)-.021869, (float)-.0061225, (float)-.0010098, 
		(float).0061813, (float).0068488, (float)-3.7978e-4, (float)
		-8.071e-4 };

#define dhh ((real *)&equiv_391)

    static struct {
	real e_1[8];
	integer fill_2[4];
	real e_3[8];
	integer fill_4[4];
	real e_5[8];
	integer fill_6[4];
	real e_7[8];
	integer fill_8[4];
	real e_9[8];
	integer fill_10[4];
	real e_11[8];
	integer fill_12[4];
	real e_13[8];
	integer fill_14[4];
	real e_15[8];
	integer fill_16[4];
	real e_17[8];
	integer fill_18[4];
	real e_19[8];
	integer fill_20[4];
	real e_21[8];
	integer fill_22[4];
	real e_23[8];
	integer fill_24[4];
	real e_25[8];
	integer fill_26[4];
	real e_27[8];
	integer fill_28[4];
	real e_29[8];
	integer fill_30[4];
	real e_31[8];
	integer fill_32[4];
	real e_33[8];
	integer fill_34[4];
	real e_35[8];
	integer fill_36[4];
	real e_37[8];
	integer fill_38[4];
	real e_39[8];
	integer fill_40[4];
	real e_41[8];
	integer fill_42[4];
	real e_43[8];
	integer fill_44[4];
	real e_45[8];
	integer fill_46[4];
	real e_47[8];
	integer fill_48[4];
	real e_49[8];
	integer fill_50[4];
	real e_51[8];
	integer fill_52[4];
	real e_53[8];
	integer fill_54[4];
	real e_55[8];
	integer fill_56[4];
	real e_57[8];
	integer fill_58[4];
	real e_59[8];
	integer fill_60[4];
	real e_61[8];
	integer fill_62[4];
	real e_63[8];
	integer fill_64[4];
	real e_65[8];
	integer fill_66[4];
	real e_67[8];
	integer fill_68[4];
	real e_69[8];
	integer fill_70[4];
	real e_71[8];
	integer fill_72[4];
	real e_73[8];
	integer fill_74[4];
	real e_75[8];
	integer fill_76[4];
	real e_77[8];
	integer fill_78[4];
	real e_79[8];
	integer fill_80[4];
	real e_81[8];
	integer fill_82[4];
	real e_83[8];
	integer fill_84[4];
	real e_85[8];
	integer fill_86[4];
	real e_87[8];
	integer fill_88[4];
	real e_89[8];
	integer fill_90[4];
	real e_91[8];
	integer fill_92[4];
	real e_93[8];
	integer fill_94[4];
	real e_95[8];
	integer fill_96[4];
	real e_97[8];
	integer fill_98[4];
	} equiv_392 = { (float)-3.0827, (float)-1.71, (float)-1.2078, (float)
		-1.123, (float)-2.976, (float)-2.0393, (float)-1.5728, (float)
		-1.3029, {0}, (float)4.4643e-7, (float)5.254e-7, (float)
		-8.3889e-8, (float)-2.9419e-7, (float)-.86799, (float)-.81007,
		 (float)-.68726, (float)-.43965, {0}, (float)-.34361, (float)
		-.8228, (float)-.86323, (float)-.95822, (float).048259, (
		float)-.43286, (float)-.46811, (float)-.4354, {0}, (float)
		-4.7996e-7, (float)-8.2344e-7, (float)-1.5199e-7, (float)
		-3.4817e-7, (float).48124, (float).30351, (float).024677, (
		float)-.12774, {0}, (float)-.29473, (float)-.40545, (float)
		-.44768, (float)-.35165, (float)-.0092323, (float)-.17024, (
		float)-.45196, (float)-.54683, {0}, (float)1.5576e-7, (float)
		-5.5031e-7, (float)2.509e-7, (float)2.2927e-7, (float)-.17802,
		 (float).046307, (float).15523, (float).089815, {0}, (float)
		.20815, (float).33911, (float).35311, (float).33646, (float)
		.12279, (float)-.002422, (float).30631, (float).24691, {0}, (
		float).40991, (float).25554, (float).1321, (float).089248, (
		float).16776, (float)-.063442, (float).14197, (float).15207, {
		0}, (float)-2.9466e-8, (float)8.4842e-8, (float)-4.213e-9, (
		float)-1.1195e-7, (float).21054, (float)-.001637, (float)
		-.078123, (float).045816, {0}, (float)-.059793, (float)
		.083244, (float)-.002548, (float).038351, (float)-.051912, (
		float).06746, (float)-.017428, (float).042447, {0}, (float)
		3.8185e-8, (float)9.6193e-8, (float)3.0688e-8, (float)
		-7.4946e-8, (float)-.010298, (float)-.010338, (float)-.017897,
		 (float)-.027108, {0}, (float).025958, (float)-.060073, (
		float)-.043178, (float)2.5411e-5, (float).015834, (float)
		-.044583, (float)-.018015, (float).0027242, {0}, (float)
		-5.2957e-8, (float)-2.0823e-7, (float)1.8903e-9, (float)
		2.165e-8, (float)-.0083905, (float).0096863, (float).02721, (
		float)-.0073419, {0}, (float).66694, (float).3006, (float)
		.089046, (float)-.0088392, (float).15721, (float).0053744, (
		float).060701, (float)-.047334, {0}, (float)1.1803e-7, (float)
		7.9876e-8, (float)-1.0776e-8, (float)-1.7418e-8, (float)
		-.02726, (float).0026359, (float)-.13212, (float).050151, {0},
		 (float)-.029476, (float)-.1175, (float).029388, (float)
		.012222, (float)-.067125, (float)-.058995, (float)-.022745, (
		float)-.021149, {0}, (float)1.4225e-8, (float)-2.9524e-8, (
		float)2.2664e-8, (float)4.5854e-9, (float)-.039449, (float)
		-.01631, (float)-.017608, (float)-.023378, {0}, (float)
		-.013761, (float)-.035514, (float)-.011335, (float)-.021108, (
		float).0047324, (float).0073342, (float)-.012909, (float)
		.0043055, {0}, (float)-7.2518e-8, (float)-2.9102e-8, (float)
		-1.4036e-8, (float)3.078e-8, (float).010327, (float)9.4701e-4,
		 (float).020402, (float)-.016985, {0}, (float)-.28967, (float)
		-.045851, (float)-.04987, (float).031391, (float)-.18399, (
		float).069508, (float)-.0040894, (float).015813, {0}, (float)
		-2.0293e-8, (float)-4.7236e-9, (float)-2.0597e-8, (float)
		-9.2939e-9, (float).02086, (float)-.043418, (float).0039555, (
		float)-.016609, {0}, (float).018383, (float).024348, (float)
		.0051163, (float).011868, (float).043145, (float)-.0051968, (
		float)-.0036464, (float).011566, {0}, (float)1.0666e-8, (
		float)4.2832e-9, (float)1.5444e-8, (float)-2.3493e-9, (float)
		-.0049043, (float)-.003068, (float).014643, (float).0062104, {
		0}, (float)-.0098637, (float)-.0074072, (float).0070597, (
		float).0040791, (float)-.0056411, (float).011837, (float)
		-.0055944, (float).0050605, {0}, (float)-.21268, (float)
		-.11252, (float)-.11044, (float)-.0092411, (float)-.24448, (
		float)-.12321, (float)-.08809, (float)-.035118, {0}, (float)
		4.3123e-8, (float)9.5787e-8, (float)3.2974e-8, (float)
		-3.1598e-9, (float).071468, (float).042603, (float)-.01093, (
		float)-.020272, {0}, (float)-.013389, (float).026864, (float)
		.033729, (float)-.0025908, (float)-.012413, (float)-.035437, (
		float)-.0073501, (float)-.0085016, {0}, (float)2.678e-9, (
		float)1.7685e-8, (float)9.666e-9, (float)-9.0595e-9, (float)
		-.013358, (float).016518, (float)-.0056046, (float)-.009016, {
		0}, (float)9.9442e-4, (float)-.0059893, (float)-.020066, (
		float)-.0010797, (float)-.0022098, (float)-.012969, (float)
		.0026169, (float)-.0068417, {0}, (float)-.0078858, (float)
		-.16605, (float)-.047281, (float).0402, (float)-.03976, (
		float).032534, (float)-.016782, (float)-.025045, {0}, (float)
		5.6098e-9, (float)-1.3665e-7, (float)1.0175e-8, (float)
		-2.938e-9, (float)-.0079188, (float).002845, (float).014274, (
		float)-.010184, {0}, (float)-.0049395, (float).012534, (float)
		.0045853, (float)-.0015619, (float)6.9939e-4, (float).0035741,
		 (float)-2.7868e-4, (float)-.0063754, {0}, (float)-4.389e-10, 
		(float)1.1691e-8, (float)3.9224e-9, (float)-2.7627e-10, (
		float)-.0053921, (float)-.0020381, (float).0021147, (float)
		.0060554, {0}, (float)-.083089, (float).047303, (float)
		-.05684, (float).022044, (float)-.055414, (float)-.050507, (
		float)-.018199, (float)-.020355, {0}, (float)5.472e-8, (float)
		6.8012e-10, (float)3.2182e-8, (float)-1.6962e-8, (float)
		.0085107, (float).0082961, (float).0088925, (float)-.0028685, 
		{0}, (float).014305, (float).010472, (float).012884, (float)
		-.0085223, (float)-.009889, (float)3.8507e-4, (float)
		-.0015835, (float)-.0026734, {0}, (float)-9.3089e-9, (float)
		1.9695e-9, (float)6.6859e-9, (float)-3.6076e-9, (float)
		5.1361e-4, (float).0010441, (float).0018784, (float)-.0015766,
		 {0}, (float)-.0068383, (float)-.0082855, (float).042958, (
		float).023448, (float).066272, (float).041571, (float).011248,
		 (float).0064888, {0}, (float)-7.1157e-9, (float)-8.1465e-9, (
		float)7.5059e-9, (float)-4.9645e-9, (float)-.002551, (float)
		-.0029669, (float).0072779, (float)-.0016234, {0}, (float)
		-.0039638, (float).0057602, (float).0050972, (float)
		-5.8718e-4, (float)-.0073423, (float)-7.791e-4, (float)
		4.4115e-4, (float)-.0025859, {0}, (float).0099653, (float)
		.0078478, (float)-.03422, (float)-.015134, (float).0020118, (
		float).022211, (float).0018361, (float).0066457, {0}, (float)
		1.0788e-8, (float)-2.5233e-8, (float)4.4177e-9, (float)
		-8.6918e-9, (float)-.0064885, (float)-.003513, (float)
		-.0011211, (float)-.0014526, {0}, (float)-.0011729, (float)
		.0030308, (float).0066776, (float)-.0023766, (float).0027256, 
		(float).0023044, (float).001106, (float)-.0011457, {0}, (
		float)-.0029558, (float)-.0085876, (float)-.0047692, (float)
		7.2023e-4, (float).0086778, (float).029926, (float).011001, (
		float).0022648, {0}, (float)1.6849e-8, (float)2.5667e-9, (
		float)1.1823e-8, (float)9.0865e-10, (float)-8.981e-4, (float)
		.0026085, (float).0012477, (float)-8.7925e-4, {0}, (float)
		.0012213, (float).0065844, (float)-.0088228, (float).0030388, 
		(float).01035, (float).025759, (float).0090196, (float)
		.0041493, {0}, (float)-8.7021e-9, (float)-1.7771e-8, (float)
		-2.5359e-9, (float)-2.6828e-9, (float)-.0031749, (float)
		3.9804e-4, (float)-5.8886e-4, (float)-2.3712e-4, {0}, (float)
		-.014505, (float)-.0061409, (float)-.011986, (float)4.6501e-4,
		 (float).016338, (float).031126, (float).00973, (float)
		7.9344e-4, {0}, (float)-5.6113e-4, (float)-.010949, (float)
		-.0051509, (float)-7.739e-4, (float)-.0021051, (float).01086, 
		(float).0049376, (float).001845 };

#define dheh ((real *)&equiv_392)

    static struct {
	real e_1[8];
	integer fill_2[4];
	real e_3[8];
	integer fill_4[4];
	real e_5[8];
	integer fill_6[4];
	real e_7[8];
	integer fill_8[4];
	real e_9[8];
	integer fill_10[4];
	real e_11[8];
	integer fill_12[4];
	real e_13[8];
	integer fill_14[4];
	real e_15[8];
	integer fill_16[4];
	real e_17[8];
	integer fill_18[4];
	real e_19[8];
	integer fill_20[4];
	real e_21[8];
	integer fill_22[4];
	real e_23[8];
	integer fill_24[4];
	real e_25[8];
	integer fill_26[4];
	real e_27[8];
	integer fill_28[4];
	real e_29[8];
	integer fill_30[4];
	real e_31[8];
	integer fill_32[4];
	real e_33[8];
	integer fill_34[4];
	real e_35[8];
	integer fill_36[4];
	real e_37[8];
	integer fill_38[4];
	real e_39[8];
	integer fill_40[4];
	real e_41[8];
	integer fill_42[4];
	real e_43[8];
	integer fill_44[4];
	real e_45[8];
	integer fill_46[4];
	real e_47[8];
	integer fill_48[4];
	real e_49[8];
	integer fill_50[4];
	real e_51[8];
	integer fill_52[4];
	real e_53[8];
	integer fill_54[4];
	real e_55[8];
	integer fill_56[4];
	real e_57[8];
	integer fill_58[4];
	real e_59[8];
	integer fill_60[4];
	real e_61[8];
	integer fill_62[4];
	real e_63[8];
	integer fill_64[4];
	real e_65[8];
	integer fill_66[4];
	real e_67[8];
	integer fill_68[4];
	real e_69[8];
	integer fill_70[4];
	real e_71[8];
	integer fill_72[4];
	real e_73[8];
	integer fill_74[4];
	real e_75[8];
	integer fill_76[4];
	real e_77[8];
	integer fill_78[4];
	real e_79[8];
	integer fill_80[4];
	real e_81[8];
	integer fill_82[4];
	real e_83[8];
	integer fill_84[4];
	real e_85[8];
	integer fill_86[4];
	real e_87[8];
	integer fill_88[4];
	real e_89[8];
	integer fill_90[4];
	real e_91[8];
	integer fill_92[4];
	real e_93[8];
	integer fill_94[4];
	real e_95[8];
	integer fill_96[4];
	real e_97[8];
	integer fill_98[4];
	} equiv_393 = { (float)-1.6313, (float)-1.4724, (float)-1.6315, (
		float)-1.7525, (float)-1.595, (float)-1.4816, (float)-1.5647, 
		(float)-1.752, {0}, (float)3.3826e-9, (float)6.2234e-7, (
		float)1.1175e-7, (float)-1.0714e-6, (float).12602, (float)
		.25836, (float).42744, (float).38232, {0}, (float).23816, (
		float).19984, (float).47753, (float).86183, (float).24136, (
		float).25713, (float).47853, (float).81527, {0}, (float)
		2.8373e-7, (float)-6.2436e-7, (float)-1.0014e-7, (float)
		1.1184e-6, (float).16546, (float)-.011671, (float)-.2716, (
		float)-.2887, {0}, (float)-.020961, (float)-.059609, (float)
		-.10097, (float)-.27829, (float).017991, (float)-.11685, (
		float)-.037019, (float)-.1073, {0}, (float)-5.3533e-7, (float)
		5.3063e-8, (float)-1.0216e-7, (float)2.7162e-7, (float)
		-.013398, (float)-.080473, (float).021446, (float).07022, {0},
		 (float).08716, (float).040544, (float)-.053935, (float)
		-.10538, (float).093804, (float)-.025842, (float)-.13457, (
		float)-.16764, {0}, (float).098248, (float)-.029918, (float)
		-.3357, (float)-.13943, (float)-.0077106, (float).017538, (
		float)-.11104, (float)-.21758, {0}, (float)4.4888e-8, (float)
		4.1071e-7, (float)2.0709e-7, (float)-1.1095e-7, (float)
		.014206, (float).03148, (float).087769, (float)-.020171, {0}, 
		(float)-.032244, (float)5.6303e-4, (float).044824, (float)
		.045835, (float)-.0023237, (float)-.0052361, (float).0040593, 
		(float).034368, {0}, (float)-2.0063e-8, (float)-4.7255e-8, (
		float)1.8159e-8, (float)-1.8177e-7, (float)-.0012863, (float)
		-.020114, (float)-.027075, (float)-.0022874, {0}, (float)
		-.020732, (float).0080654, (float).022117, (float).0083712, (
		float)2.3217e-4, (float).010788, (float).0084188, (float)
		.027205, {0}, (float)1.4134e-9, (float)-3.4962e-8, (float)
		-8.4695e-8, (float)1.0937e-7, (float)-.0022888, (float)
		.0060475, (float)-.012262, (float)-.0064735, {0}, (float)
		.06867, (float).016174, (float)-.16189, (float)-.42987, (
		float)-.033507, (float)-.082585, (float)-.064727, (float)
		-.33147, {0}, (float)3.8855e-8, (float)1.0136e-7, (float)
		9.9261e-8, (float)-3.9255e-7, (float).021777, (float).015506, 
		(float).071687, (float).092558, {0}, (float)-.079258, (float)
		.011632, (float).057796, (float).066012, (float)-.02319, (
		float)-.0093973, (float)-.038468, (float).049336, {0}, (float)
		3.6878e-8, (float)-2.4108e-8, (float)4.8814e-8, (float)
		5.4483e-8, (float)-.02655, (float).0069887, (float)-.0079663, 
		(float)-.041876, {0}, (float).030244, (float).010895, (float)
		-.0075845, (float).0090376, (float).0054864, (float).0016035, 
		(float).012872, (float).0050738, {0}, (float)-3.3364e-8, (
		float)-1.4658e-8, (float)-5.1309e-8, (float)7.3468e-8, (float)
		.0063951, (float)-.0046125, (float).0028088, (float).012235, {
		0}, (float)-.066861, (float)-.043236, (float)-.12423, (float)
		.0050653, (float)-.048363, (float).0012645, (float)-.012956, (
		float)-.012631, {0}, (float)3.2622e-8, (float)1.5584e-7, (
		float)1.3321e-7, (float)-6.8615e-9, (float).021732, (float)
		.028444, (float)-.039502, (float)-.026016, {0}, (float)
		.020265, (float)-.0076984, (float)-.016181, (float)-.013051, (
		float)-.002925, (float).0082369, (float)-7.9875e-5, (float)
		.010295, {0}, (float)-1.4791e-8, (float)1.8395e-8, (float)
		-7.7736e-9, (float)-4.6976e-9, (float)-.0079064, (float)
		-.0026763, (float).0061428, (float)-.0060847, {0}, (float)
		.0066954, (float)6.3333e-4, (float).0046002, (float).0039132, 
		(float)-.005498, (float)-7.5445e-4, (float)-.0012074, (float)
		.0022948, {0}, (float).02134, (float)-.0052038, (float)
		-.12281, (float)-.038456, (float).040796, (float).026349, (
		float).0038344, (float)-.1806, {0}, (float)-3.2037e-8, (float)
		1.5845e-7, (float)2.4201e-7, (float)1.6913e-8, (float).013459,
		 (float)-.016736, (float).0033477, (float)-.017793, {0}, (
		float).0090073, (float)-.0068821, (float).014288, (float)
		-8.327e-4, (float).0074514, (float).0013463, (float)-.0033677,
		 (float).0054176, {0}, (float)2.0858e-8, (float)-1.8127e-8, (
		float)1.1896e-8, (float)-3.9315e-8, (float)-.0057685, (float)
		1.7333e-5, (float)-.0030964, (float)2.2462e-4, {0}, (float)
		-.0052152, (float).0012382, (float)-.0053069, (float).002999, 
		(float)2.1205e-4, (float)-9.0561e-4, (float)9.6501e-4, (float)
		.0018786, {0}, (float).039714, (float)-.066883, (float)
		-.018104, (float)-.0059986, (float)-.019001, (float)-.008105, 
		(float).0082409, (float).11605, {0}, (float)-1.9344e-9, (
		float)6.051e-8, (float)-1.9814e-8, (float)6.4241e-9, (float)
		-.0014431, (float)-.0056762, (float)-.028989, (float).0017233,
		 {0}, (float)-8.96e-5, (float)-.0073364, (float)-.0084131, (
		float)-3.6095e-4, (float)6.2449e-4, (float).0031943, (float)
		.0043657, (float)-.0015451, {0}, (float)-3.7487e-9, (float)
		1.1055e-8, (float)2.2753e-9, (float)-4.3472e-9, (float)
		-.0012225, (float)-3.0766e-4, (float)9.4355e-5, (float)
		.0013382, {0}, (float)-.040648, (float)-.02013, (float)
		.0051336, (float).093971, (float).0049048, (float)-.026193, (
		float).01313, (float)-.073353, {0}, (float)1.722e-8, (float)
		1.0636e-7, (float)1.4694e-7, (float)6.1373e-8, (float)
		-.014864, (float)-.012177, (float)-.018814, (float)-3.7852e-4,
		 {0}, (float).005633, (float)-.0043419, (float).0049619, (
		float)-.007076, (float)-.0016067, (float)-.0012289, (float)
		.0027093, (float).0013236, {0}, (float)4.4101e-9, (float)
		-5.693e-9, (float)4.1959e-9, (float)-2.7203e-9, (float)
		-.001502, (float)4.1217e-4, (float)-.0012167, (float)
		-9.6993e-4, {0}, (float).0041397, (float)-.019632, (float)
		-.022226, (float).045148, (float)-.0037769, (float)-.024327, (
		float)-.0057969, (float).018479, {0}, (float)-4.3699e-10, (
		float)2.3733e-8, (float)-5.4731e-8, (float)-1.1452e-8, (float)
		-.0013646, (float)-.0061348, (float)-.0064927, (float)
		-.0056199, {0}, (float)-.0035601, (float)-.0017396, (float)
		-.0052711, (float)-1.6323e-4, (float)-.0014496, (float)
		-.0012113, (float)4.195e-4, (float)8.0329e-4, {0}, (float)
		-.016551, (float).0047721, (float).01304, (float).022101, (
		float)-.012677, (float)-.017103, (float)-.0091788, (float)
		-.017815, {0}, (float)-2.6274e-9, (float)5.6015e-8, (float)
		4.8906e-8, (float)5.663e-9, (float)-.0028348, (float)
		-.0022901, (float)-.0072444, (float).0035429, {0}, (float)
		8.3802e-4, (float)-.0016795, (float).0022301, (float)
		-9.0528e-4, (float)-3.0407e-5, (float)6.3735e-4, (float)
		-.001473, (float)-.0015325, {0}, (float).0012105, (float)
		-4.7359e-4, (float)-2.7938e-4, (float).006739, (float)
		-.0030275, (float)-.012284, (float)-.010845, (float)-.0046303,
		 {0}, (float)-1.0517e-8, (float)-1.4109e-8, (float)-2.2648e-8,
		 (float)1.1191e-9, (float)-.0014841, (float)-.0038308, (float)
		-.0025005, (float)-.0039437, {0}, (float)-.0085131, (float)
		1.4272e-4, (float)-.015969, (float).0057927, (float)-.0067625,
		 (float)-.010159, (float)-.0082969, (float)-.017196, {0}, (
		float)4.5467e-9, (float)2.0978e-8, (float)-6.5911e-9, (float)
		2.2189e-9, (float)-.0022857, (float)-.0017879, (float)
		-.001966, (float)-.0020703, {0}, (float)-.0022753, (float)
		-.0029585, (float).0036104, (float).0089446, (float)-.001918, 
		(float)-.01251, (float)-.0063399, (float)-3.6631e-4, {0}, (
		float)-.0033013, (float).010551, (float).0029534, (float)
		-6.3752e-4, (float)-.0051806, (float)-.010009, (float)
		-.0058925, (float)-.016702 };

#define dnh ((real *)&equiv_393)

    static real dol[441]	/* was [3][3][49] */ = { (float)-.0034295,(
	    float)-.26245,(float)-.89352,(float)-.0075061,(float)-.31262,(
	    float)-.69317,0.0,0.0,0.0,(float)4.8322e-10,(float)8.4041e-7,(
	    float)2.4097e-5,(float).0042214,(float).2164,(float).33146,0.0,
	    0.0,0.0,(float)7.9335e-4,(float).22991,(float).37286,(float)
	    .0032811,(float).29808,(float).59247,0.0,0.0,0.0,(float)1.8237e-9,
	    (float)-2.106e-6,(float)-1.0359e-5,(float)-.0043773,(float)
	    -.29615,(float)-.38841,0.0,0.0,0.0,(float).001032,(float)-.0597,(
	    float).010068,(float)4.1127e-5,(float).064926,(float).15031,0.0,
	    0.0,0.0,(float)-7.3733e-10,(float)5.5301e-6,(float)-1.7035e-5,(
	    float).0027276,(float).0038438,(float).034648,0.0,0.0,0.0,(float)
	    -5.8045e-4,(float).026165,(float)-.020139,(float)-4.5069e-4,(
	    float)-.090777,(float)-.20436,0.0,0.0,0.0,(float)-.0021541,(float)
	    -.34817,(float)-1.0039,(float)-.0069573,(float)-.38422,(float)
	    -.76788,0.0,0.0,0.0,(float)-6.5013e-10,(float)2.5875e-7,(float)
	    8.8331e-6,(float).0032289,(float).13193,(float).11303,0.0,0.0,0.0,
	    (float)-3.1081e-4,(float).017171,(float)-.13731,(float)4.5324e-4,(
	    float).031168,(float)-.030861,0.0,0.0,0.0,(float)-9.8478e-12,(
	    float)-7.7857e-7,(float)4.7035e-6,(float)-5.4022e-4,(float)
	    -.036264,(float).03173,0.0,0.0,0.0,(float)8.0628e-5,(float)
	    -.0064432,(float)-.036637,(float)-2.8733e-4,(float).023774,(float)
	    .058057,0.0,0.0,0.0,(float)2.1821e-10,(float)6.9845e-7,(float)
	    -2.6654e-6,(float)1.7425e-4,(float)-.0093167,(float)-.032321,0.0,
	    0.0,0.0,(float)-.0019044,(float)-.13705,(float)-.20386,(float)
	    -.0044235,(float)-.15581,(float)-.23509,0.0,0.0,0.0,(float)
	    -8.3791e-10,(float)1.07e-6,(float)1.5439e-5,(float)-1.1619e-5,(
	    float).015604,(float).12907,0.0,0.0,0.0,(float)-1.5618e-4,(float)
	    .0075614,(float)-.036661,(float).0013238,(float).023125,(float)
	    -.025901,0.0,0.0,0.0,(float)-4.6595e-11,(float)-5.6468e-7,(float)
	    1.026e-5,(float)-5.1465e-4,(float).01324,(float).0097355,0.0,0.0,
	    0.0,(float)1.5868e-4,(float)8.2754e-4,(float)-.045066,(float)
	    -3.3052e-4,(float)-.0012961,(float).030717,0.0,0.0,0.0,(float)
	    -1.6618e-10,(float)1.9031e-7,(float)-2.8682e-6,(float)3.8804e-4,(
	    float)-.011848,(float)-.02717,0.0,0.0,0.0,(float)7.2724e-5,(float)
	    -.025396,(float)-.17254,(float)6.8531e-4,(float)-.032185,(float)
	    -.10447,0.0,0.0,0.0,(float)-3.1159e-10,(float)-7.2603e-7,(float)
	    -4.9526e-6,(float)-1.9665e-4,(float)-.0051446,(float)-.070094,0.0,
	    0.0,0.0,(float)1.5783e-4,(float)-.017557,(float)-.067622,(float)
	    1.9622e-4,(float).010193,(float)-.026774,0.0,0.0,0.0,(float)
	    -6.1281e-10,(float)2.1324e-7,(float)-1.0877e-6,(float)2.8863e-4,(
	    float)-.0079351,(float).020725,0.0,0.0,0.0,(float)-3.2544e-5,(
	    float)-.0010498,(float)-.012351,(float)-6.5642e-5,(float).0032452,
	    (float)2.486e-4,0.0,0.0,0.0,(float)-5.8917e-4,(float)-.064057,(
	    float)-.10953,(float)-.0028855,(float)-.13403,(float)-.29119,0.0,
	    0.0,0.0,(float)-6.1467e-10,(float)4.6894e-7,(float)7.0854e-6,(
	    float)5.0512e-4,(float)-.0010768,(float).09344,0.0,0.0,0.0,(float)
	    -1.0295e-4,(float)-.009139,(float)-.014296,(float)7.0149e-4,(
	    float).012773,(float)-.0080132,0.0,0.0,0.0,(float)5.7992e-11,(
	    float)9.6493e-9,(float)6.7187e-6,(float)-1.5869e-4,(float)
	    .0029588,(float)-.0019018,0.0,0.0,0.0,(float)5.7641e-5,(float)
	    1.7969e-4,(float)-.020577,(float)-1.5202e-4,(float)-9.6663e-4,(
	    float).0044841,0.0,0.0,0.0,(float)6.7711e-5,(float).070926,(float)
	    .064692,(float).0015939,(float).056863,(float).059359,0.0,0.0,0.0,
	    (float)6.1432e-10,(float)-3.6416e-7,(float)-3.0932e-6,(float)
	    -5.6796e-4,(float)-.021862,(float)-.019317,0.0,0.0,0.0,(float)
	    8.637e-5,(float)-.0069695,(float)-.011908,(float)-1.067e-4,(float)
	    .010585,(float)-.018184,0.0,0.0,0.0,(float)-2.367e-10,(float)
	    8.9469e-8,(float)-1.666e-6,(float)1.2569e-7,(float)-.0031121,(
	    float).0050489,0.0,0.0,0.0,(float)2.2941e-5,(float).02431,(float)
	    .066329,(float)-8.3699e-4,(float)-.033781,(float)-.10292,0.0,0.0,
	    0.0,(float)-4.7808e-11,(float)-1.7204e-7,(float)-7.0914e-7,(float)
	    -8.4185e-5,(float)-.012186,(float).012137,0.0,0.0,0.0,(float)
	    4.7716e-5,(float)-.00545,(float)-.015392,(float)2.4768e-5,(float)
	    .0047556,(float)8.1073e-4,0.0,0.0,0.0,(float)2.1198e-11,(float)
	    9.1857e-8,(float)2.1883e-6,(float)4.6445e-5,(float)-5.703e-4,(
	    float)7.1284e-4,0.0,0.0,0.0,(float)2.8298e-6,(float).021846,(
	    float).0045838,(float)1.9761e-4,(float).012328,(float).0083372,
	    0.0,0.0,0.0,(float)3.6265e-10,(float)8.1249e-8,(float)-8.4412e-7,(
	    float)6.0184e-5,(float)-.0064108,(float).01334,0.0,0.0,0.0,(float)
	    -4.4578e-6,(float)-7.2438e-4,(float)5.4739e-4,(float)-1.0454e-5,(
	    float).0017929,(float)-.0067512,0.0,0.0,0.0,(float)1.7541e-4,(
	    float).021225,(float).028565,(float)3.2389e-4,(float)-.0021691,(
	    float).017102,0.0,0.0,0.0,(float)2.1511e-10,(float)-1.1084e-8,(
	    float)-9.7635e-7,(float)-3.7438e-5,(float)-.0082313,(float)
	    -.0024549,0.0,0.0,0.0,(float)2.5195e-5,(float)-4.9135e-4,(float)
	    .0050974,(float)-3.4186e-5,(float).0019333,(float)-.0017239,0.0,
	    0.0,0.0,(float)-2.7325e-5,(float)-.0015282,(float)-5.1648e-4,(
	    float)-3.5859e-4,(float)-.011646,(float).019465,0.0,0.0,0.0,(
	    float)-2.3875e-11,(float)5.4962e-8,(float)-2.9444e-8,(float)
	    -6.1472e-5,(float)-.0016945,(float)-.0054019,0.0,0.0,0.0,(float)
	    -3.1418e-6,(float)-.0020812,(float)-.0010738,(float)-4.7218e-4,(
	    float)-.027066,(float).048407,0.0,0.0,0.0,(float)4.6152e-11,(
	    float)3.7817e-8,(float)-2.7121e-7,(float)7.6277e-6,(float)
	    1.357e-4,(float)-.0044151,0.0,0.0,0.0,(float)-2.9151e-5,(float)
	    -4.2919e-4,(float).0034379,(float)1.4752e-4,(float)-.0085273,(
	    float)-.019067,0.0,0.0,0.0,(float)3.7913e-5,(float)1.7766e-4,(
	    float)-.0021828,(float)-4.509e-4,(float)-.015711,(float).015001 };
    static real dhl[441]	/* was [3][3][49] */ = { (float)-2.3735,(
	    float)-.75599,(float)-.31869,(float)-2.2303,(float)-.76906,(float)
	    -.46737,0.0,0.0,0.0,(float)-4.1106e-7,(float)-2.3535e-7,(float)
	    1.509e-7,(float)-.2793,(float)-.42549,(float)-.12403,0.0,0.0,0.0,(
	    float)-.15043,(float)-.41761,(float)-.4056,(float)-.16289,(float)
	    -.62598,(float)-.64613,0.0,0.0,0.0,(float)7.105e-8,(float)
	    -2.8901e-8,(float)2.8552e-7,(float).084439,(float).22279,(float)
	    .18831,0.0,0.0,0.0,(float)-.044023,(float).048799,(float)
	    -.0042289,(float).043059,(float).14493,(float).12419,0.0,0.0,0.0,(
	    float)5.19e-8,(float)-1.4005e-7,(float)3.6878e-8,(float)-.0109,(
	    float).10653,(float)-.059911,0.0,0.0,0.0,(float).075167,(float)
	    -.075257,(float).19957,(float).005906,(float).27012,(float).19218,
	    0.0,0.0,0.0,(float).26079,(float).5085,(float).27526,(float)
	    .38539,(float).3871,(float).29484,0.0,0.0,0.0,(float)-5.4677e-8,(
	    float)-5.5769e-8,(float)4.0798e-7,(float)-.097745,(float)-.06164,(
	    float).040311,0.0,0.0,0.0,(float).040421,(float).074268,(float)
	    .11343,(float).018579,(float).046697,(float).098207,0.0,0.0,0.0,(
	    float)8.4393e-9,(float)-4.0953e-8,(float)-1.0028e-7,(float)
	    .014256,(float)-.057444,(float)-.029219,0.0,0.0,0.0,(float)
	    .0042554,(float).02088,(float).051526,(float)6.8489e-4,(float)
	    .020933,(float)-.011908,0.0,0.0,0.0,(float)1.7166e-8,(float)
	    -3.9792e-8,(float)-6.0643e-8,(float)-.0011027,(float).0088119,(
	    float).038705,0.0,0.0,0.0,(float).19018,(float).19637,(float)
	    .092397,(float).23516,(float).14986,(float)-.0018943,0.0,0.0,0.0,(
	    float)-6.0764e-8,(float)3.5212e-8,(float)-2.1866e-7,(float)
	    .038591,(float).021907,(float)-.042874,0.0,0.0,0.0,(float).038687,
	    (float).035924,(float).086528,(float)-.029264,(float)-.018032,(
	    float)-.04606,0.0,0.0,0.0,(float)6.3717e-8,(float)-3.2961e-8,(
	    float)2.346e-8,(float)-.0019275,(float)-.046878,(float)-.034199,
	    0.0,0.0,0.0,(float).0064895,(float)-.011911,(float).036848,(float)
	    .024632,(float)-.016714,(float)-.022322,0.0,0.0,0.0,(float)
	    -4.0891e-8,(float)5.4324e-9,(float)6.2456e-8,(float)-3.0091e-4,(
	    float).042935,(float).025785,0.0,0.0,0.0,(float).038167,(float)
	    .029918,(float).015483,(float)-.024136,(float)-.024183,(float)
	    .025357,0.0,0.0,0.0,(float)1.8652e-8,(float)1.083e-8,(float)
	    -1.7108e-7,(float).021471,(float).02029,(float).027261,0.0,0.0,
	    0.0,(float)-.010038,(float).034518,(float).036333,(float)-.014274,
	    (float)-.010109,(float).0098865,0.0,0.0,0.0,(float)1.9645e-8,(
	    float)-5.4744e-9,(float)-1.0845e-8,(float)-.0029073,(float)
	    .0052285,(float).0016691,0.0,0.0,0.0,(float).0021804,(float)
	    .010144,(float).010259,(float).004065,(float)-.0069878,(float)
	    .0062808,0.0,0.0,0.0,(float)-.014444,(float)-.015615,(float)
	    .004332,(float).026483,(float).042781,(float)-.0091296,0.0,0.0,
	    0.0,(float)6.5822e-9,(float)9.3932e-9,(float)-2.6987e-7,(float)
	    .0039806,(float).005669,(float)-.013707,0.0,0.0,0.0,(float)
	    .0070666,(float).0079662,(float)-.010254,(float)-.01356,(float)
	    -.011873,(float)-.0087014,0.0,0.0,0.0,(float)-8.4659e-9,(float)
	    -4.4936e-9,(float)-8.8644e-8,(float)3.6952e-4,(float)-.0045405,(
	    float)-.0034345,0.0,0.0,0.0,(float)-.0055334,(float)-.0014126,(
	    float)-.002117,(float)5.3108e-4,(float)-.0036632,(float)-.0046994,
	    0.0,0.0,0.0,(float).015556,(float)-.031648,(float)-.021956,(float)
	    -.039132,(float)-.022072,(float)-.025409,0.0,0.0,0.0,(float)
	    1.2414e-8,(float)2.2804e-8,(float)-1.3044e-7,(float).023689,(
	    float).0012209,(float)-.0067929,0.0,0.0,0.0,(float)-.0043661,(
	    float).0062742,(float)-.0068424,(float)-6.6253e-4,(float)
	    -.0058301,(float).0012701,0.0,0.0,0.0,(float)4.4984e-9,(float)
	    -9.5574e-9,(float)-3.8852e-8,(float).0013556,(float)3.1915e-6,(
	    float).0036829,0.0,0.0,0.0,(float).0093627,(float)-.011306,(float)
	    -.016083,(float).023462,(float).010209,(float)-.0029346,0.0,0.0,
	    0.0,(float)-2.4554e-9,(float)1.6269e-9,(float)-2.8545e-8,(float)
	    -.0032744,(float).0019771,(float)-.0021371,0.0,0.0,0.0,(float)
	    -.0036096,(float)2.2078e-4,(float).00478,(float).0014565,(float)
	    .0013042,(float)-.0074109,0.0,0.0,0.0,(float)7.8989e-9,(float)
	    2.8647e-9,(float)4.3092e-8,(float)2.4812e-4,(float).0014817,(
	    float).0014351,0.0,0.0,0.0,(float)-.012352,(float)-.017932,(float)
	    -9.4451e-4,(float)-3.2051e-4,(float).0020904,(float)-.0074944,0.0,
	    0.0,0.0,(float)1.0504e-8,(float)-5.0077e-9,(float)-4.6383e-8,(
	    float).0026108,(float)-7.5031e-4,(float)-.002353,0.0,0.0,0.0,(
	    float)-4.5357e-6,(float)-.0023559,(float).0017086,(float)
	    7.5391e-4,(float).0022646,(float)-.002396,0.0,0.0,0.0,(float)
	    7.8762e-4,(float)-.0092953,(float).0079651,(float)-.019602,(float)
	    .011547,(float)-.0082307,0.0,0.0,0.0,(float)-5.2915e-9,(float)
	    -5.1441e-9,(float)2.1903e-7,(float).0050088,(float).0067321,(
	    float)-4.0801e-4,0.0,0.0,0.0,(float)-.0019697,(float)-.0022185,(
	    float)3.388e-4,(float)8.0312e-4,(float)9.517e-4,(float).001943,
	    0.0,0.0,0.0,(float)4.2966e-4,(float)2.9117e-5,(float)4.5247e-4,(
	    float).013604,(float).017845,(float).001899,0.0,0.0,0.0,(float)
	    5.977e-9,(float)-4.8547e-9,(float)1.0538e-7,(float).0032853,(
	    float).0061338,(float)6.7012e-4,0.0,0.0,0.0,(float).0037248,(
	    float).0044236,(float).0037229,(float).0081745,(float).012967,(
	    float)-.0022565,0.0,0.0,0.0,(float)6.9259e-9,(float)-2.8102e-9,(
	    float)-8.6806e-8,(float).0019033,(float).0023311,(float)
	    -4.5399e-4,0.0,0.0,0.0,(float).0030955,(float)9.3261e-4,(float)
	    -5.2132e-4,(float).016966,(float).029597,(float)-.0017694,0.0,0.0,
	    0.0,(float).0027742,(float).0019538,(float)-5.7966e-4,(float)
	    .012348,(float)-.0022654,(float)-6.3397e-4 };
    static real dhel[441]	/* was [3][3][49] */ = { (float)-2.8533,(
	    float)-1.6103,(float)-1.3192,(float)-3.0612,(float)-2.2374,(float)
	    -1.9424,0.0,0.0,0.0,(float)-2.1986e-7,(float)-1.3811e-7,(float)
	    1.8143e-6,(float)-1.0562,(float)-.74507,(float)-.58403,0.0,0.0,
	    0.0,(float).072644,(float).11838,(float)-.021284,(float).088532,(
	    float)-.14689,(float).34084,0.0,0.0,0.0,(float)2.2489e-7,(float)
	    2.4209e-7,(float)-5.786e-6,(float).085373,(float)-.025946,(float)
	    -.19084,0.0,0.0,0.0,(float)-.18677,(float)-.12965,(float)-.13203,(
	    float).081474,(float).047283,(float)-.054692,0.0,0.0,0.0,(float)
	    -3.9095e-7,(float)-6.5882e-8,(float)1.0367e-6,(float).028747,(
	    float).14143,(float).20943,0.0,0.0,0.0,(float).01085,(float)
	    .13191,(float).087882,(float).13534,(float).32299,(float).0067653,
	    0.0,0.0,0.0,(float).22428,(float).46161,(float).25246,(float)
	    .4358,(float).19366,(float).17048,0.0,0.0,0.0,(float)-1.5216e-8,(
	    float)-2.56e-7,(float)3.054e-7,(float).044317,(float).087688,(
	    float).1956,0.0,0.0,0.0,(float)-.10736,(float).12864,(float)
	    .12671,(float)-.018054,(float).012526,(float).072251,0.0,0.0,0.0,(
	    float)-7.9766e-8,(float)7.8717e-8,(float)7.0752e-7,(float)
	    -.013027,(float)-.02281,(float)-.0013912,0.0,0.0,0.0,(float)
	    -.041078,(float).029376,(float).051175,(float).041295,(float)
	    -.0022711,(float).049874,0.0,0.0,0.0,(float)4.7277e-8,(float)
	    -2.0378e-9,(float)-1.0432e-6,(float)-.0044592,(float)-.011824,(
	    float)-.0051299,0.0,0.0,0.0,(float).6334,(float).37085,(float)
	    .15597,(float).10223,(float)-.048523,(float)-.22192,0.0,0.0,0.0,(
	    float)-1.5529e-8,(float)2.5941e-7,(float)-1.9061e-7,(float).13085,
	    (float)-8.2298e-4,(float)-.13285,0.0,0.0,0.0,(float).0081139,(
	    float).023336,(float).097418,(float)-.077696,(float)-.22395,(
	    float)-.17111,0.0,0.0,0.0,(float)1.4044e-8,(float)-2.5896e-7,(
	    float)2.2626e-6,(float)-.073836,(float)-.034992,(float)-.028852,
	    0.0,0.0,0.0,(float).0058076,(float).070754,(float).063152,(float)
	    .01978,(float).018125,(float).017141,0.0,0.0,0.0,(float)
	    -4.6109e-8,(float)1.5037e-7,(float)-2.4478e-6,(float).015735,(
	    float).01423,(float)-.041047,0.0,0.0,0.0,(float)-.088933,(float)
	    -.16104,(float)-.16992,(float)-.059522,(float)-.027661,(float)
	    .13092,0.0,0.0,0.0,(float)-6.0933e-9,(float)2.5861e-8,(float)
	    -6.0624e-7,(float).048434,(float).023424,(float).029271,0.0,0.0,
	    0.0,(float).0089345,(float).021873,(float)-.016865,(float).01597,(
	    float).0047094,(float).021492,0.0,0.0,0.0,(float)7.8024e-9,(float)
	    -3.9311e-10,(float)2.6681e-7,(float).0035974,(float)-.010739,(
	    float).031053,0.0,0.0,0.0,(float).010524,(float).017883,(float)
	    .0024771,(float)-.003801,(float)-.0034746,(float).017248,0.0,0.0,
	    0.0,(float)-.0708,(float)-.059487,(float)-.039535,(float)-.10031,(
	    float)-.22153,(float)-.17665,0.0,0.0,0.0,(float)3.0069e-8,(float)
	    7.5016e-8,(float)5.9015e-7,(float).017651,(float)-.018756,(float)
	    -.0019095,0.0,0.0,0.0,(float)-.038559,(float)-.0038658,(float)
	    -.054337,(float).012491,(float)-.0071623,(float)-.01009,0.0,0.0,
	    0.0,(float)1.981e-9,(float)-1.5184e-8,(float)-1.4229e-7,(float)
	    .0090502,(float).0046047,(float)-.028027,0.0,0.0,0.0,(float)
	    -.02764,(float)-.022297,(float)-.025864,(float)-.0087768,(float)
	    -.0045071,(float)-.015999,0.0,0.0,0.0,(float)-.038927,(float)
	    -.032125,(float).045711,(float)-.0096294,(float).022894,(float)
	    .069373,0.0,0.0,0.0,(float)2.5396e-8,(float)1.3503e-8,(float)
	    1.4937e-7,(float).018994,(float)-.024285,(float)-.040136,0.0,0.0,
	    0.0,(float)-.0059943,(float)-.0010069,(float)-.01996,(float)
	    .0046892,(float)-.0055847,(float).013345,0.0,0.0,0.0,(float)
	    1.4326e-8,(float)2.3685e-8,(float)-1.0504e-7,(float)2.5748e-4,(
	    float)-9.8852e-4,(float).0095536,0.0,0.0,0.0,(float)-.020216,(
	    float)-.023076,(float).012233,(float)-.0096002,(float).067171,(
	    float)-.010609,0.0,0.0,0.0,(float)-1.0963e-8,(float)1.6395e-8,(
	    float)-6.6122e-8,(float)-.006245,(float)-5.0134e-4,(float)
	    -.022548,0.0,0.0,0.0,(float)-.010778,(float)-.0047465,(float)
	    8.5225e-4,(float).0090447,(float)-.0029361,(float)-.015098,0.0,
	    0.0,0.0,(float)-3.1408e-9,(float)2.1465e-9,(float)8.6051e-9,(
	    float)-.0018477,(float)-.0042558,(float)-.011711,0.0,0.0,0.0,(
	    float)-.0090961,(float)-.021087,(float).027375,(float).018898,(
	    float).015103,(float).026458,0.0,0.0,0.0,(float)-1.6521e-8,(float)
	    -1.0412e-8,(float)6.545e-8,(float)-.0069182,(float)-.009786,(
	    float)-.015922,0.0,0.0,0.0,(float).0053029,(float)-.004445,(float)
	    -.0061311,(float).0020626,(float)-.0018098,(float).0034746,0.0,
	    0.0,0.0,(float)-.033221,(float).0069945,(float).012666,(float)
	    .0047967,(float).018795,(float)-.018009,0.0,0.0,0.0,(float)
	    2.9052e-11,(float)1.608e-8,(float)1.9864e-7,(float)-.0057386,(
	    float).0045063,(float)4.8529e-4,0.0,0.0,0.0,(float)-.0019158,(
	    float).0031289,(float).0028454,(float)-6.0267e-4,(float).0024747,(
	    float)-.0038353,0.0,0.0,0.0,(float).0063675,(float).009476,(float)
	    .01902,(float).025325,(float).011677,(float).0019446,0.0,0.0,0.0,(
	    float)-8.1957e-9,(float)-8.4313e-9,(float)-5.1153e-8,(float)
	    .0011937,(float)-.0042526,(float)-.0074106,0.0,0.0,0.0,(float)
	    .007436,(float)-.0021643,(float).0044576,(float).021619,(float)
	    .014606,(float)-.0039309,0.0,0.0,0.0,(float)-7.6241e-9,(float)
	    -8.3007e-9,(float)-2.0117e-8,(float)-.0013459,(float).0022638,(
	    float)-.0013632,0.0,0.0,0.0,(float).0065228,(float).0023316,(
	    float).0086293,(float).037915,(float).021285,(float)-.0047108,0.0,
	    0.0,0.0,(float).0032586,(float)-.0088253,(float)-.0055447,(float)
	    .017314,(float).013963,(float).0072728 };
    static real dnl[441]	/* was [3][3][49] */ = { (float)-1.7368,(
	    float)-1.5547,(float)-1.7382,(float)-1.7418,(float)-1.5723,(float)
	    -1.4667,0.0,0.0,0.0,(float)-3.0027e-8,(float)6.7091e-7,(float)
	    -6.0635e-7,(float).070525,(float).40251,(float).163,0.0,0.0,0.0,(
	    float).22184,(float).40622,(float).4474,(float).29785,(float)
	    .40235,(float).64765,0.0,0.0,0.0,(float)-6.7089e-7,(float)
	    -5.583e-8,(float)4.7518e-7,(float).011675,(float)-.16855,(float)
	    -.16832,0.0,0.0,0.0,(float)-.10635,(float)-.13901,(float).091015,(
	    float)-.092159,(float)-.14445,(float)-.19603,0.0,0.0,0.0,(float)
	    6.8139e-7,(float)-6.7244e-7,(float)8.6631e-7,(float)-.1295,(float)
	    -.02999,(float).13735,0.0,0.0,0.0,(float).058091,(float).1488,(
	    float)-.0041582,(float)-.058461,(float)-.20387,(float)-.28157,0.0,
	    0.0,0.0,(float)-.062121,(float)-.082658,(float)-.31542,(float)
	    -.13966,(float)-.17979,(float)-.091417,0.0,0.0,0.0,(float)
	    3.1864e-7,(float)2.6659e-7,(float)-8.6896e-7,(float).015042,(
	    float).19726,(float)-.024826,0.0,0.0,0.0,(float).048208,(float)
	    .010021,(float)-.088282,(float).046388,(float)-.068767,(float)
	    -.027273,0.0,0.0,0.0,(float)-2.744e-7,(float)1.3823e-7,(float)
	    1.7889e-7,(float)-.0097774,(float)-.027798,(float).031754,0.0,0.0,
	    0.0,(float)-.0024,(float)-.0074716,(float)-.038424,(float)
	    .0035422,(float).0045989,(float).02012,0.0,0.0,0.0,(float)
	    6.6031e-8,(float)-8.922e-8,(float)5.6129e-8,(float)-.0086287,(
	    float).013561,(float)-.0068389,0.0,0.0,0.0,(float)-.10868,(float)
	    -.13959,(float)-.10561,(float)-.08771,(float)-.12212,(float)
	    -.096535,0.0,0.0,0.0,(float)7.2869e-8,(float)2.0864e-7,(float)
	    1.0863e-7,(float).038852,(float).024244,(float).019137,0.0,0.0,
	    0.0,(float)-.010042,(float)-.010484,(float)-.037752,(float)
	    -.0090949,(float)-.04267,(float)-.032374,0.0,0.0,0.0,(float)
	    -4.6008e-8,(float)4.4276e-8,(float)-1.1781e-7,(float)-.018152,(
	    float).062073,(float).071156,0.0,0.0,0.0,(float).0016205,(float)
	    .019706,(float)-.041151,(float)3.6013e-4,(float)-.0052021,(float)
	    .013993,0.0,0.0,0.0,(float)4.6331e-9,(float)-1.1884e-7,(float)
	    -1.4323e-8,(float).0072089,(float)-.02386,(float)-.038484,0.0,0.0,
	    0.0,(float)-.036398,(float)-.012037,(float)-.038984,(float)
	    .025707,(float)-.064837,(float).031979,0.0,0.0,0.0,(float)
	    1.7139e-7,(float)-6.249e-8,(float)-4.7271e-7,(float).0084327,(
	    float).031524,(float)-.04256,0.0,0.0,0.0,(float)6.2127e-4,(float)
	    -.029276,(float)-.037386,(float)-.0065605,(float)-.018784,(float)
	    -.003819,0.0,0.0,0.0,(float)-4.0138e-8,(float)4.0271e-8,(float)
	    8.5416e-8,(float)4.8306e-4,(float).012402,(float).011174,0.0,0.0,
	    0.0,(float).0012439,(float)-.01231,(float)-.012316,(float)
	    5.0042e-4,(float)1.1119e-5,(float)-.0023599,0.0,0.0,0.0,(float)
	    -.018217,(float)-.0026679,(float).10742,(float)-.031783,(float)
	    -.019303,(float).0034835,0.0,0.0,0.0,(float)4.6073e-8,(float)
	    3.5891e-8,(float)1.5944e-7,(float).0057723,(float)-.022166,(float)
	    -.006433,0.0,0.0,0.0,(float).0037524,(float)-.0065922,(float)
	    -.0028192,(float).008823,(float).011413,(float)-.0014724,0.0,0.0,
	    0.0,(float)-2.0566e-8,(float)-1.6467e-8,(float)-1.2504e-8,(float)
	    -.002626,(float)-.0022769,(float)-.0027194,0.0,0.0,0.0,(float)
	    -.0030241,(float)-3.9885e-4,(float)-.016905,(float)-.0011217,(
	    float).0010334,(float).0018374,0.0,0.0,0.0,(float).027188,(float)
	    .020962,(float).02827,(float).057169,(float)-.0036042,(float)
	    .0079017,0.0,0.0,0.0,(float)6.3375e-8,(float)-5.5785e-8,(float)
	    -1.9685e-7,(float)-.0052793,(float)-.0034116,(float)-.013993,0.0,
	    0.0,0.0,(float)-.0037723,(float)-.0074266,(float).0089762,(float)
	    -.0031765,(float)-.0042969,(float)1.4415e-4,0.0,0.0,0.0,(float)
	    -3.9013e-9,(float)2.5071e-8,(float)2.8706e-8,(float)5.1564e-4,(
	    float).0037668,(float).0018409,0.0,0.0,0.0,(float).0069304,(float)
	    .0224,(float).067218,(float).013519,(float)-.016617,(float)
	    .0015734,0.0,0.0,0.0,(float)5.703e-8,(float)-2.8034e-8,(float)
	    5.7165e-8,(float)-.0039896,(float)-.020903,(float)-.01315,0.0,0.0,
	    0.0,(float)-1.2236e-4,(float)-.0012509,(float)-.0082717,(float)
	    .0017336,(float).011176,(float)-.0043392,0.0,0.0,0.0,(float)
	    -6.4988e-9,(float)-1.9655e-8,(float)3.0388e-8,(float)-2.8791e-4,(
	    float)-.0034928,(float)-8.7189e-4,0.0,0.0,0.0,(float).012409,(
	    float).010287,(float).019133,(float).013395,(float)-.019803,(
	    float)-.0069494,0.0,0.0,0.0,(float)6.2839e-9,(float)2.6129e-8,(
	    float)1.4687e-8,(float)-.0053013,(float)-.0037789,(float)
	    -.0070281,0.0,0.0,0.0,(float)-.0014527,(float)-.0013332,(float)
	    5.5305e-4,(float)-5.6986e-4,(float)2.9788e-4,(float)-.0015472,0.0,
	    0.0,0.0,(float).001437,(float).010731,(float).0079431,(float)
	    .003063,(float)-.03188,(float)7.6997e-4,0.0,0.0,0.0,(float)
	    1.8605e-8,(float)2.4252e-9,(float)4.8793e-8,(float)-.0038017,(
	    float)-.010222,(float)-.0097752,0.0,0.0,0.0,(float)1.8677e-4,(
	    float).0015417,(float).0086786,(float)9.3966e-4,(float).0037033,(
	    float)-.0010346,0.0,0.0,0.0,(float).0039142,(float).0031698,(
	    float).004901,(float)-5.1651e-4,(float)-.030094,(float)-.0042392,
	    0.0,0.0,0.0,(float)-4.8721e-9,(float)1.8012e-8,(float)3.24e-8,(
	    float)-.0030807,(float)-2.2242e-4,(float)-.0073591,0.0,0.0,0.0,(
	    float)-.0041931,(float)-.0031057,(float)-.0038863,(float)
	    -.0044359,(float)-.028816,(float).01061,0.0,0.0,0.0,(float)
	    7.9621e-9,(float)1.534e-8,(float)-2.5953e-8,(float)-.0020503,(
	    float)-.0044691,(float)-.0058767,0.0,0.0,0.0,(float)1.9265e-4,(
	    float).0030302,(float)-.0023986,(float)8.0675e-4,(float)-.022928,(
	    float)2.4544e-4,0.0,0.0,0.0,(float)-.0013332,(float)-.0029436,(
	    float)-.0069512,(float)1.1912e-4,(float)-.016669,(float).0060162 }
	    ;

    /* System generated locals */
    doublereal d__1;

    /* Builtin functions */
    double r_lg10(real *), pow_dd(doublereal *, doublereal *);

    /* Local variables */
    static real nheh, nhel, ntot;
    extern /* Subroutine */ int ionlow_(integer *, real *, real *, real *, 
	    real *, real *, real *, real *, integer *, real *, integer *, 
	    real *);
    static real nhh, nhl, nnh, noh, nnl, nol;
    extern /* Subroutine */ int ionhigh_(integer *, real *, real *, real *, 
	    real *, real *, real *, real *, integer *, real *, integer *, 
	    real *);

/* Version 1.0 (released 20.12.2002) */
/* CALION calculates relative density of O+, H+, He+ and N+  in the outer */
/* ionosphere with regard to solar activity (F107 index). */
/* CALION uses subroutines IONLOW and IONHIGH. */
/* Linearly interpolates for solar activity. */
/* Inputs: CRD - 0 .. INVDIP */
/*               1 .. FL, DIMO, B0, DIPL (used for calculation INVDIP inside) */
/*         INVDIP - "mix" coordinate of the dip latitude and of */
/*                    the invariant latitude; */
/*                    positive northward, in deg, range <-90.0;90.0> */
/*         FL, DIMO, B0 - McIlwain L parameter, dipole moment in */
/*                        Gauss, magnetic field strength in Gauss - */
/*                        parameters needed for invariant latitude */
/*                        calculation */
/*         DIPL - dip latitude */
/*                positive northward, in deg, range <-90.0;90.0> */
/*         MLT - magnetic local time (central dipole) */
/*               in hours, range <0;24) */
/*         ALT - altitude above the Earth's surface; */
/*               in km, range <350;2000> */
/*         DDD - day of year; range <0;365> */
/*         F107 - F107 index */
/* Output: NO,NH,NHE,NN - relative density of O+, H+, He+, and N+ */
/* Versions:  1.0 FORTRAN */
/* Author of the code: */
/*         Vladimir Truhlik */
/*         Institute of Atm. Phys. */
/*         Bocni II. */
/*         141 31 Praha 4, Sporilov */
/*         Czech Republic */
/*         e-mail: vtr@ufa.cas.cz */
/*         tel/fax: +420 267103058, +420 728073539 / +420 272 762528 */
/* /////////////////////coefficients high solar activity//////////////////////// */
/* //////////////////////////////////O+///////////////////////////////////////// */
/*     550km equinox */
/*     550km June solstice */
/*     900km equinox */
/*     900km June solstice */
/*     1500km equinox */
/*     1500km June solstice */
/*     2250km equinox */
/*     2250km June solstice */
/* //////////////////////////////////////////////////////////////////////////////////// */
/* //////////////////////////////////H+//////////////////////////////////////////////// */
/*     550km equinox */
/*     550km June solstice */
/*     900km equinox */
/*     900km June solstice */
/*     1500km equinox */
/*     1500km June solstice */
/*     2250km equinox */
/*     2250km June solstice */
/* //////////////////////////////////////////////////////////////////////////////////// */
/* //////////////////////////////////He+/////////////////////////////////////////////// */
/*     550km equinox */
/*     550km June solstice */
/*     900km equinox */
/*     900km June solstice */
/*     1500km equinox */
/*     1500km June solstice */
/*     2250km equinox */
/*     2250km June solstice */
/* //////////////////////////////////////////////////////////////////////////////////// */
/* ///////////////////////////////////N+/////////////////////////////////////////////// */
/*     550km equinox */
/*     550km June solstice */
/*     900km equinox */
/*     900km June solstice */
/*     1500km equinox */
/*     1500km June solstice */
/*     2250km equinox */
/*     2250km June solstice */
/* //////////////////////////////////////////////////////////////////////////////////// */
/* /////////////////////coefficients low solar activity//////////////////////////////// */
/* //////////////////////////////////O+//////////////////////////////////////////////// */
/*     400km equinox */
/*     400km June solstice */
/*     650km equinox */
/*     650km June solstice */
/*     1000km equinox */
/*     1000km June solstice */
/* //////////////////////////////////////////////////////////////////////////////////// */
/* //////////////////////////////////H+//////////////////////////////////////////////// */
/*     400km equinox */
/*     400km June solstice */
/*     650km equinox */
/*     650km June solstice */
/*     1000km equinox */
/*     1000km June solstice */
/* ///////////////////////////////////////////////////////////////////////////////////// */
/* //////////////////////////////////He+//////////////////////////////////////////////// */
/*     400km equinox */
/*     400km June solstice */
/*     650km equinox */
/*     650km June solstice */
/*     1000km equinox */
/*     1000km June solstice */
/* //////////////////////////////////////////////////////////////////////////////////// */
/* //////////////////////////////////N+//////////////////////////////////////////////// */
/*     400km equinox */
/*     400km June solstice */
/*     650km equinox */
/*     650km June solstice */
/*     1000km equinox */
/*     1000km June solstice */
/* //////////////////////////////////////////////////////////////////////////////////// */
/* ///////////////////////////////solar minimum//////////////////////////////////////// */
    ionlow_(crd, invdip, fl, dimo, b0, dipl, mlt, alt, ddd, dol, &c__0, &nol);
    ionlow_(crd, invdip, fl, dimo, b0, dipl, mlt, alt, ddd, dhl, &c__1, &nhl);
    ionlow_(crd, invdip, fl, dimo, b0, dipl, mlt, alt, ddd, dhel, &c__2, &
	    nhel);
    ionlow_(crd, invdip, fl, dimo, b0, dipl, mlt, alt, ddd, dnl, &c__3, &nnl);
/*     normalization */
    ntot = nol + nhl + nhel + nnl;
    nol /= ntot;
    nhl /= ntot;
    nhel /= ntot;
    nnl /= ntot;
/* ///////////////////////////////solar maximum//////////////////////////////////////// */
    ionhigh_(crd, invdip, fl, dimo, b0, dipl, mlt, alt, ddd, doh, &c__0, &noh)
	    ;
    ionhigh_(crd, invdip, fl, dimo, b0, dipl, mlt, alt, ddd, dhh, &c__1, &nhh)
	    ;
    ionhigh_(crd, invdip, fl, dimo, b0, dipl, mlt, alt, ddd, dheh, &c__2, &
	    nheh);
    ionhigh_(crd, invdip, fl, dimo, b0, dipl, mlt, alt, ddd, dnh, &c__3, &nnh)
	    ;
/*     normalization */
    ntot = noh + nhh + nheh + nnh;
    noh /= ntot;
    nhh /= ntot;
    nheh /= ntot;
    nnh /= ntot;
/*     interpolation (in logarithm) */
    if (*f107 > (float)200.) {
	*f107 = (float)200.;
    }
    if (*f107 < (float)85.) {
	*f107 = (float)85.;
    }
    *no = (r_lg10(&noh) - r_lg10(&nol)) / (float)115. * (*f107 - (float)85.) 
	    + r_lg10(&nol);
    *nh = (r_lg10(&nhh) - r_lg10(&nhl)) / (float)115. * (*f107 - (float)85.) 
	    + r_lg10(&nhl);
    *nhe = (r_lg10(&nheh) - r_lg10(&nhel)) / (float)115. * (*f107 - (float)
	    85.) + r_lg10(&nhel);
    *nn = (r_lg10(&nnh) - r_lg10(&nnl)) / (float)115. * (*f107 - (float)85.) 
	    + r_lg10(&nnl);
/*     percentages */
    d__1 = (doublereal) (*no);
    *no = pow_dd(&c_b30, &d__1);
    d__1 = (doublereal) (*nh);
    *nh = pow_dd(&c_b30, &d__1);
    d__1 = (doublereal) (*nhe);
    *nhe = pow_dd(&c_b30, &d__1);
    d__1 = (doublereal) (*nn);
    *nn = pow_dd(&c_b30, &d__1);
/*     last normalization */
    ntot = *no + *nh + *nhe + *nn;
    *no /= ntot;
    *nh /= ntot;
    *nhe /= ntot;
    *nn /= ntot;
    return 0;
} /* calion_ */

#undef dnh
#undef dheh
#undef dhh
#undef doh


/* Subroutine */ int ionlow_(integer *crd, real *invdip, real *fl, real *dimo,
	 real *b0, real *dipl, real *mlt, real *alt, integer *ddd, real *d__, 
	integer *ion, real *nion)
{
    /* Initialized data */

    static integer mirreq[49] = { 1,-1,1,-1,1,-1,1,1,-1,1,-1,1,-1,1,-1,1,-1,1,
	    -1,1,-1,1,-1,1,1,-1,1,-1,1,1,-1,1,-1,1,-1,1,-1,1,-1,1,1,-1,1,1,-1,
	    1,-1,1,1 };

    /* System generated locals */
    doublereal d__1;

    /* Builtin functions */
    double log(doublereal), pow_dd(doublereal *, doublereal *);

    /* Local variables */
    static real n0a100, n0b100, n0a400, n0b400, n0a650, n0b650;
    static integer seza, sezb;
    static real dtor;
    extern doublereal eptr_(real *, real *, real *);
    static real rmlt, c__[49];
    static integer i__, sezai, sezbi;
    static real invdp;
    extern /* Subroutine */ int spharm_ik__(real *, integer *, integer *, 
	    real *, real *);
    static real aa, bb, ah[3], st[2];
    extern doublereal invdpc_(real *, real *, real *, real *, real *);
    static real rcolat, n400, n650, st1, st2, n1000, ano[3], dno[1], sum, 
	    n100a, n100b, n400a, n400b, n650a, n650b;
    static integer ddda, dddb, dddd;

/* IONLOW calculates relative density of O+, H+, He+ or N+  in the outer */
/* ionosphere for a low solar activity (F107 < 100). */
/* Based on spherical harmonics approximation of relative ion density */
/* (by AE-C, and AE-E) at altitudes centred on 400km, 650km, and 1000km. */
/* For intermediate altitudes an interpolation is used. */
/* Recommended altitude range: 350-2000 km!!! */
/* For days between seasons centred at (21.3. = 79; 21.6. = 171; */
/* 23.9. 265; 21.12. = 354) relative ion density is linearly interpolated. */
/* Inputs: CRD - 0 .. INVDIP */
/*               1 .. FL, DIMO, B0, DIPL (used for calculation INVDIP inside) */
/*         INVDIP - "mix" coordinate of the dip latitude and of */
/*                    the invariant latitude; */
/*                    positive northward, in deg, range <-90.0;90.0> */
/*         FL, DIMO, BO - McIlwain L parameter, dipole moment in */
/*                        Gauss, magnetic field strength in Gauss - */
/*                        parameters needed for invariant latitude */
/*                        calculation */
/*         DIPL - dip latitude */
/*                positive northward, in deg, range <-90.0;90.0> */
/*         MLT - magnetic local time (central dipole) */
/*               in hours, range <0;24) */
/*         ALT - altitude above the Earth's surface; */
/*               in km, range <350;2000> */
/*         DDD - day of year; range <0;365> */
/*         D - coefficints of spherical harmonics for a given ion */
/*         ION - ion species (0...O+, 1...H+, 2...He+, 3...N+) */
/* Output: NION - relative density for a given ion */
    /* Parameter adjustments */
    d__ -= 13;

    /* Function Body */
/* //////////////////////////////////////////////////////////////////////////////////// */
    dtor = (float).01745329252;
/*     coefficients for mirroring */
    for (i__ = 1; i__ <= 49; ++i__) {
	d__[(i__ * 3 + 3) * 3 + 1] = d__[(i__ * 3 + 2) * 3 + 1] * mirreq[i__ 
		- 1];
	d__[(i__ * 3 + 3) * 3 + 2] = d__[(i__ * 3 + 2) * 3 + 2] * mirreq[i__ 
		- 1];
/* L10: */
	d__[(i__ * 3 + 3) * 3 + 3] = d__[(i__ * 3 + 2) * 3 + 3] * mirreq[i__ 
		- 1];
    }
    if (*crd == 1) {
	invdp = invdpc_(fl, dimo, b0, dipl, &dtor);
    } else if (*crd == 0) {
	invdp = *invdip;
    } else {
	return 0;
    }
    rmlt = *mlt * dtor * (float)15.;
    rcolat = ((float)90. - invdp) * dtor;
    spharm_ik__(c__, &c__6, &c__6, &rcolat, &rmlt);
/*     21.3. - 20.6. */
    if (*ddd >= 79 && *ddd < 171) {
	seza = 1;
	sezb = 2;
	ddda = 79;
	dddb = 171;
	dddd = *ddd;
    }
/*     21.6. - 22.9. */
    if (*ddd >= 171 && *ddd < 265) {
	seza = 2;
	sezb = 4;
	ddda = 171;
	dddb = 265;
	dddd = *ddd;
    }
/*     23.9. - 20.12. */
    if (*ddd >= 265 && *ddd < 354) {
	seza = 4;
	sezb = 3;
	ddda = 265;
	dddb = 354;
	dddd = *ddd;
    }
/*     21.12. - 20.3. */
    if (*ddd >= 354 || *ddd < 79) {
	seza = 3;
	sezb = 1;
	ddda = 354;
	dddb = 444;
	dddd = *ddd;
	if (*ddd >= 354) {
	    dddd = *ddd;
	} else {
	    dddd = *ddd + 365;
	}
    }
    sezai = (seza - 1) % 3 + 1;
    sezbi = (sezb - 1) % 3 + 1;
/*     400km level */
    n0a400 = (float)0.;
    n0b400 = (float)0.;
    for (i__ = 1; i__ <= 49; ++i__) {
	n0a400 += c__[i__ - 1] * d__[(sezai + i__ * 3) * 3 + 1];
/* L30: */
	n0b400 += c__[i__ - 1] * d__[(sezbi + i__ * 3) * 3 + 1];
    }
    n400a = n0a400;
    n400b = n0b400;
    n400 = (n400b - n400a) / (dddb - ddda) * (dddd - ddda) + n400a;
/*     650km level */
    n0a650 = (float)0.;
    n0b650 = (float)0.;
    for (i__ = 1; i__ <= 49; ++i__) {
	n0a650 += c__[i__ - 1] * d__[(sezai + i__ * 3) * 3 + 2];
/* L70: */
	n0b650 += c__[i__ - 1] * d__[(sezbi + i__ * 3) * 3 + 2];
    }
    n650a = n0a650;
    n650b = n0b650;
    n650 = (n650b - n650a) / (dddb - ddda) * (dddd - ddda) + n650a;
/*     1000km level */
    n0a100 = (float)0.;
    n0b100 = (float)0.;
    for (i__ = 1; i__ <= 49; ++i__) {
	n0a100 += c__[i__ - 1] * d__[(sezai + i__ * 3) * 3 + 3];
/* L110: */
	n0b100 += c__[i__ - 1] * d__[(sezbi + i__ * 3) * 3 + 3];
    }
    n100a = n0a100;
    n100b = n0b100;
    n1000 = (n100b - n100a) / (dddb - ddda) * (dddd - ddda) + n100a;
/*      IF (ALT .LT. 650) NO=(N650-N400)/250.0*(ALT-400)+N400 */
/*      IF (ALT .GE. 650) NO=(N1000-N650)/350.0*(ALT-650)+N650 */
/*      NION=10**NO */
/* -02/07/09- n(O+) AND n(N+) must not increase above 650km */
    if ((*ion == 0 || *ion == 3) && n1000 > n650) {
	n1000 = n650;
    }
/* -02/07/09- n(H+) must not decrease above 650km */
    if (*ion == 1 && n1000 < n650) {
	n1000 = n650;
    }
    ano[0] = n400;
    ano[1] = n650;
    ano[2] = n1000;
    ah[0] = (float)400.;
    ah[1] = (float)650.;
    ah[2] = (float)1e3;
    dno[0] = (float)20.;
    st1 = (ano[1] - ano[0]) / (ah[1] - ah[0]);
    i__ = 2;
    st2 = (ano[i__] - ano[i__ - 1]) / (ah[i__] - ah[i__ - 1]);
    ano[i__ - 1] -= (st2 - st1) * dno[i__ - 2] * log((float)2.);
    for (i__ = 1; i__ <= 2; ++i__) {
/* L220: */
	st[i__ - 1] = (ano[i__] - ano[i__ - 1]) / (ah[i__] - ah[i__ - 1]);
    }
    argexp_1.argmax = (float)88.;
    sum = ano[0] + st[0] * (*alt - ah[0]);
    i__ = 1;
    aa = eptr_(alt, &dno[i__ - 1], &ah[i__]);
    bb = eptr_(ah, &dno[i__ - 1], &ah[i__]);
    sum += (st[i__] - st[i__ - 1]) * (aa - bb) * dno[i__ - 1];
    d__1 = (doublereal) sum;
    *nion = pow_dd(&c_b30, &d__1);
    return 0;
} /* ionlow_ */

/* Subroutine */ int ionhigh_(integer *crd, real *invdip, real *fl, real *
	dimo, real *b0, real *dipl, real *mlt, real *alt, integer *ddd, real *
	d__, integer *ion, real *nion)
{
    /* Initialized data */

    static integer mirreq[49] = { 1,-1,1,-1,1,-1,1,1,-1,1,-1,1,-1,1,-1,1,-1,1,
	    -1,1,-1,1,-1,1,1,-1,1,-1,1,1,-1,1,-1,1,-1,1,-1,1,-1,1,1,-1,1,1,-1,
	    1,-1,1,1 };

    /* System generated locals */
    doublereal d__1;

    /* Builtin functions */
    double log(doublereal), pow_dd(doublereal *, doublereal *);

    /* Local variables */
    static real n0a150, n0b150, n0a250, n0a900, n0a550, n0b550, n0b900, 
	    n0b250;
    static integer seza, sezb;
    static real dtor;
    extern doublereal eptr_(real *, real *, real *);
    static real rmlt, c__[49];
    static integer i__, sezai, sezbi;
    static real invdp;
    extern /* Subroutine */ int spharm_ik__(real *, integer *, integer *, 
	    real *, real *);
    static real aa, bb, ah[4], st[3];
    extern doublereal invdpc_(real *, real *, real *, real *, real *);
    static real rcolat, n900, n550, st1, st2, n1500, n2500, ano[4], dno[2], 
	    sum, n150a, n150b, n250a, n900a, n550a, n550b, n900b, n250b;
    static integer ddda, dddb, dddd;

/* IONHIGH calculates relative density of O+, H+, He+ or N+  in the outer */
/* ionosphere for high solar activity conditions (F107 >= 100). */
/* Based on spherical harmonics approximation of relative ion density */
/* (by IK24) at altitudes centred on 550km, 900km, 1500km, and 2250km. */
/* For intermediate altitudes a interpolation is used. */
/* Recommended altitude range: 500-3000 km!!! */
/* For days between seasons centred at (21.3. = 79; 21.6. = 171; */
/* 23.9. 265; 21.12. = 354) relative ion density is linearly interpolated. */
/* Inputs: CRD - 0 .. INVDIP */
/*               1 .. FL, DIMO, B0, DIPL (used for calculation INVDIP inside) */
/*         INVDIP - "mix" coordinate of the dip latitude and of */
/*                    the invariant latitude; */
/*                    positive northward, in deg, range <-90.0;90.0> */
/*         FL, DIMO, BO - McIlwain L parameter, dipole moment in */
/*                        Gauss, magnetic field strength in Gauss - */
/*                        parameters needed for invariant latitude */
/*                        calculation */
/*         DIPL - dip latitude */
/*                positive northward, in deg, range <-90.0;90.0> */
/*         MLT - magnetic local time (central dipole) */
/*               in hours, range <0;24) */
/*         ALT - altitude above the Earth's surface; */
/*               in km, range <500;3000> */
/*         DDD - day of year; range <0;365> */
/*         D - coefficints of spherical harmonics for a given ion */
/*         ION - ion species (0...O+, 1...H+, 2...He+, 3...N+) */
/* Output: NION - relative density for a given ion */
    /* Parameter adjustments */
    d__ -= 17;

    /* Function Body */
/* //////////////////////////////////////////////////////////////////////////////////// */
    dtor = (float).01745329252;
/*     coefficients for mirroring */
    for (i__ = 1; i__ <= 49; ++i__) {
	d__[(i__ * 3 + 3 << 2) + 1] = d__[(i__ * 3 + 2 << 2) + 1] * mirreq[
		i__ - 1];
	d__[(i__ * 3 + 3 << 2) + 2] = d__[(i__ * 3 + 2 << 2) + 2] * mirreq[
		i__ - 1];
	d__[(i__ * 3 + 3 << 2) + 3] = d__[(i__ * 3 + 2 << 2) + 3] * mirreq[
		i__ - 1];
/* L10: */
	d__[(i__ * 3 + 3 << 2) + 4] = d__[(i__ * 3 + 2 << 2) + 4] * mirreq[
		i__ - 1];
    }
    if (*crd == 1) {
	invdp = invdpc_(fl, dimo, b0, dipl, &dtor);
    } else if (*crd == 0) {
	invdp = *invdip;
    } else {
	return 0;
    }
    rmlt = *mlt * dtor * (float)15.;
    rcolat = ((float)90. - invdp) * dtor;
    spharm_ik__(c__, &c__6, &c__6, &rcolat, &rmlt);
/*     21.3. - 20.6. */
    if (*ddd >= 79 && *ddd < 171) {
	seza = 1;
	sezb = 2;
	ddda = 79;
	dddb = 171;
	dddd = *ddd;
    }
/*     21.6. - 22.9. */
    if (*ddd >= 171 && *ddd < 265) {
	seza = 2;
	sezb = 4;
	ddda = 171;
	dddb = 265;
	dddd = *ddd;
    }
/*     23.9. - 20.12. */
    if (*ddd >= 265 && *ddd < 354) {
	seza = 4;
	sezb = 3;
	ddda = 265;
	dddb = 354;
	dddd = *ddd;
    }
/*     21.12. - 20.3. */
    if (*ddd >= 354 || *ddd < 79) {
	seza = 3;
	sezb = 1;
	ddda = 354;
	dddb = 444;
	dddd = *ddd;
	if (*ddd >= 354) {
	    dddd = *ddd;
	} else {
	    dddd = *ddd + 365;
	}
    }
    sezai = (seza - 1) % 3 + 1;
    sezbi = (sezb - 1) % 3 + 1;
/*     550km level */
    n0a550 = (float)0.;
    n0b550 = (float)0.;
    for (i__ = 1; i__ <= 49; ++i__) {
	n0a550 += c__[i__ - 1] * d__[(sezai + i__ * 3 << 2) + 1];
/* L30: */
	n0b550 += c__[i__ - 1] * d__[(sezbi + i__ * 3 << 2) + 1];
    }
    n550a = n0a550;
    n550b = n0b550;
    n550 = (n550b - n550a) / (dddb - ddda) * (dddd - ddda) + n550a;
/*     900km level */
    n0a900 = (float)0.;
    n0b900 = (float)0.;
    for (i__ = 1; i__ <= 49; ++i__) {
	n0a900 += c__[i__ - 1] * d__[(sezai + i__ * 3 << 2) + 2];
/* L70: */
	n0b900 += c__[i__ - 1] * d__[(sezbi + i__ * 3 << 2) + 2];
    }
    n900a = n0a900;
    n900b = n0b900;
    n900 = (n900b - n900a) / (dddb - ddda) * (dddd - ddda) + n900a;
/*     1500km level */
    n0a150 = (float)0.;
    n0b150 = (float)0.;
    for (i__ = 1; i__ <= 49; ++i__) {
	n0a150 += c__[i__ - 1] * d__[(sezai + i__ * 3 << 2) + 3];
/* L110: */
	n0b150 += c__[i__ - 1] * d__[(sezbi + i__ * 3 << 2) + 3];
    }
    n150a = n0a150;
    n150b = n0b150;
    n1500 = (n150b - n150a) / (dddb - ddda) * (dddd - ddda) + n150a;
/*     2500km level */
    n0a250 = (float)0.;
    n0b250 = (float)0.;
    for (i__ = 1; i__ <= 49; ++i__) {
	n0a250 += c__[i__ - 1] * d__[(sezai + i__ * 3 << 2) + 4];
/* L150: */
	n0b250 += c__[i__ - 1] * d__[(sezbi + i__ * 3 << 2) + 4];
    }
    n250a = n0a250;
    n250b = n0b250;
    n2500 = (n250b - n250a) / (dddb - ddda) * (dddd - ddda) + n250a;
/*      IF (ALT .LT. 900) NO=(N900-N550)/350.0*(ALT-550)+N550 */
/*      IF ((ALT .GE. 900) .AND. (ALT .LT. 1500)) */
/*     &  NO=(N1500-N900)/600.0*(ALT-900)+N900 */
/*      IF (ALT .GE. 1500) NO=(N2500-N1500)/1000.0*(ALT-1500)+N1500 */
/*     O+ AND N+ may not increase above 1500km */
    if ((*ion == 0 || *ion == 3) && n2500 > n1500) {
	n2500 = n1500;
    }
/*     H+ may not decrease above 1500km */
    if (*ion == 1 && n2500 < n1500) {
	n2500 = n1500;
    }
    ano[0] = n550;
    ano[1] = n900;
    ano[2] = n1500;
    ano[3] = n2500;
    ah[0] = (float)550.;
    ah[1] = (float)900.;
    ah[2] = (float)1500.;
    ah[3] = (float)2250.;
    dno[0] = (float)20.;
    dno[1] = (float)20.;
    st1 = (ano[1] - ano[0]) / (ah[1] - ah[0]);
    for (i__ = 2; i__ <= 3; ++i__) {
	st2 = (ano[i__] - ano[i__ - 1]) / (ah[i__] - ah[i__ - 1]);
	ano[i__ - 1] -= (st2 - st1) * dno[i__ - 2] * log((float)2.);
/* L200: */
	st1 = st2;
    }
    for (i__ = 1; i__ <= 3; ++i__) {
/* L220: */
	st[i__ - 1] = (ano[i__] - ano[i__ - 1]) / (ah[i__] - ah[i__ - 1]);
    }
    argexp_1.argmax = (float)88.;
    sum = ano[0] + st[0] * (*alt - ah[0]);
    for (i__ = 1; i__ <= 2; ++i__) {
	aa = eptr_(alt, &dno[i__ - 1], &ah[i__]);
	bb = eptr_(ah, &dno[i__ - 1], &ah[i__]);
/* L230: */
	sum += (st[i__] - st[i__ - 1]) * (aa - bb) * dno[i__ - 1];
    }
    d__1 = (doublereal) sum;
    *nion = pow_dd(&c_b30, &d__1);
    return 0;
} /* ionhigh_ */



doublereal invdpc_(real *fl, real *dimo, real *b0, real *dipl, real *dtor)
{
    /* Initialized data */

    static doublereal b[8] = { 1.259921,-.1984259,-.04686632,-.01314096,
	    -.00308824,8.2777e-4,-.00105877,.00183142 };

    /* System generated locals */
    real ret_val, r__1;
    doublereal d__1, d__2, d__3, d__4, d__5, d__6, d__7, d__8;

    /* Builtin functions */
    double pow_dd(doublereal *, doublereal *), sqrt(doublereal), acos(
	    doublereal), sin(doublereal), cos(doublereal), r_sign(real *, 
	    real *);

    /* Local variables */
    static real alfa, beta, invl;
    static doublereal a;
    static real rdipl, rinvl, asa;

/*      calculation of INVDIP from FL, DIMO, BO, and DIPL */
/*      invariant latitude calculated by highly */
/*      accurate polynomial expansion */
    d__1 = (doublereal) (*dimo / *b0);
    a = pow_dd(&d__1, &c_b55) / *fl;
/* Computing 2nd power */
    d__1 = a;
/* Computing 3rd power */
    d__2 = a;
/* Computing 4th power */
    d__3 = a, d__3 *= d__3;
/* Computing 5th power */
    d__4 = a, d__5 = d__4, d__4 *= d__4;
/* Computing 6th power */
    d__6 = a, d__6 *= d__6;
/* Computing 7th power */
    d__7 = a, d__8 = d__7, d__7 *= d__7, d__8 *= d__7;
    asa = a * (b[0] + b[1] * a + b[2] * (d__1 * d__1) + b[3] * (d__2 * (d__2 *
	     d__2)) + b[4] * (d__3 * d__3) + b[5] * (d__5 * (d__4 * d__4)) + 
	    b[6] * (d__6 * (d__6 * d__6)) + b[7] * (d__8 * (d__7 * d__7)));
    if (asa > (float)1.) {
	asa = (float)1.;
    }
/*      invariant latitude (absolute value) */
    rinvl = acos(sqrt(asa));
    invl = rinvl / *dtor;
    rdipl = *dipl * *dtor;
/* Computing 3rd power */
    r__1 = sin((dabs(rdipl)));
    alfa = r__1 * (r__1 * r__1);
/* Computing 3rd power */
    r__1 = cos(rinvl);
    beta = r__1 * (r__1 * r__1);
    ret_val = (alfa * r_sign(&c_b18, dipl) * invl + beta * *dipl) / (alfa + 
	    beta);
    return ret_val;
} /* invdpc_ */



/* ************************************************************* */
/* ************* PEAK VALUES ELECTRON DENSITY ****************** */
/* ************************************************************* */


doublereal fout_(real *xmodip, real *xlati, real *xlongi, real *ut, real *ff0)
{
    /* Initialized data */

    static integer qf[9] = { 11,11,8,4,1,0,0,0,0 };

    /* System generated locals */
    real ret_val;

    /* Local variables */
    extern doublereal gamma1_(real *, real *, real *, real *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, real *);

/* CALCULATES CRITICAL FREQUENCY FOF2/MHZ USING SUBROUTINE GAMMA1. */
/* XMODIP = MODIFIED DIP LATITUDE, XLATI = GEOG. LATITUDE, XLONGI= */
/* LONGITUDE (ALL IN DEG.), MONTH = MONTH, UT =  UNIVERSAL TIME */
/* (DEC. HOURS), FF0 = ARRAY WITH RZ12-ADJUSTED CCIR/URSI COEFF. */
/* D.BILITZA,JULY 85. */
    /* Parameter adjustments */
    --ff0;

    /* Function Body */
    ret_val = gamma1_(xmodip, xlati, xlongi, ut, &c__6, qf, &c__9, &c__76, &
	    c__13, &c__988, &ff0[1]);
    return ret_val;
} /* fout_ */



doublereal xmout_(real *xmodip, real *xlati, real *xlongi, real *ut, real *
	xm0)
{
    /* Initialized data */

    static integer qm[7] = { 6,7,5,2,1,0,0 };

    /* System generated locals */
    real ret_val;

    /* Local variables */
    extern doublereal gamma1_(real *, real *, real *, real *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, real *);

/* CALCULATES PROPAGATION FACTOR M3000 USING THE SUBROUTINE GAMMA1. */
/* XMODIP = MODIFIED DIP LATITUDE, XLATI = GEOG. LATITUDE, XLONGI= */
/* LONGITUDE (ALL IN DEG.), MONTH = MONTH, UT =  UNIVERSAL TIME */
/* (DEC. HOURS), XM0 = ARRAY WITH RZ12-ADJUSTED CCIR/URSI COEFF. */
/* D.BILITZA,JULY 85. */
    /* Parameter adjustments */
    --xm0;

    /* Function Body */
    ret_val = gamma1_(xmodip, xlati, xlongi, ut, &c__4, qm, &c__7, &c__49, &
	    c__9, &c__441, &xm0[1]);
    return ret_val;
} /* xmout_ */



doublereal hmf2ed_(real *xmagbr, real *r__, real *x, real *xm3)
{
    /* System generated locals */
    real ret_val;

    /* Builtin functions */
    double exp(doublereal);

    /* Local variables */
    static real delm, f1, f2, f3;

/* CALCULATES THE PEAK HEIGHT HMF2/KM FOR THE MAGNETIC */
/* LATITUDE XMAGBR/DEG. AND THE SMOOTHED ZUERICH SUNSPOT */
/* NUMBER R USING CCIR-M3000 XM3 AND THE RATIO X=FOF2/FOE. */
/* [REF. D.BILITZA ET AL., TELECOMM.J., 46, 549-553, 1979] */
/* D.BILITZA,1980. */
    f1 = *r__ * (float).00232 + (float).222;
    f2 = (float)1.2 - exp(*r__ * (float).0239) * (float).0116;
    f3 = (*r__ - (float)25.) * (float).096 / (float)150.;
    delm = f1 * ((float)1. - *r__ / (float)150. * exp(-(*xmagbr) * *xmagbr / (
	    float)1600.)) / (*x - f2) + f3;
    ret_val = (float)1490. / (*xm3 + delm) - (float)176.;
    return ret_val;
} /* hmf2ed_ */



doublereal xm3000hm_(real *xmagbr, real *r__, real *x, real *hmf2)
{
    /* System generated locals */
    real ret_val;

    /* Builtin functions */
    double exp(doublereal);

    /* Local variables */
    static real delm, f1, f2, f3;

/* CALCULATES THE PROPAGATION FACTOR M3000 FOR THE MAGNETIC LATITUDE */
/* XMAGBR/DEG. AND THE SMOOTHED ZUERICH SUNSPOT NUMBER R USING THE */
/* PEAK HEIGHT HMF2/KM AND THE RATIO X=FOF2/FOE. Reverse of HMF2ED. */
/* [REF. D.BILITZA ET AL., TELECOMM.J., 46, 549-553, 1979] */
/* D.BILITZA,1980. */
    f1 = *r__ * (float).00232 + (float).222;
    f2 = (float)1.2 - exp(*r__ * (float).0239) * (float).0116;
    f3 = (*r__ - (float)25.) * (float).096 / (float)150.;
    delm = f1 * ((float)1. - *r__ / (float)150. * exp(-(*xmagbr) * *xmagbr / (
	    float)1600.)) / (*x - f2) + f3;
    ret_val = (float)1490. / (*hmf2 + (float)176.) - delm;
    return ret_val;
} /* xm3000hm_ */



doublereal fof1ed_(real *ylati, real *r__, real *chi)
{
    /* System generated locals */
    real ret_val;
    doublereal d__1, d__2;

    /* Builtin functions */
    double cos(doublereal), pow_dd(doublereal *, doublereal *);

    /* Local variables */
    static real chim, xmue, chi100, f0, fs, f100, dla, chi0, fof1;

/* -------------------------------------------------------------- */
/* CALCULATES THE F1 PEAK PLASMA FREQUENCY (FOF1/MHZ) */
/* FOR   DIP-LATITUDE (YLATI/DEGREE) */
/*       SMOOTHED ZURICH SUNSPOT NUMBER (R) */
/*       SOLAR ZENITH ANGLE (CHI/DEGREE) */
/* REFERENCE: */
/*       E.D.DUCHARME ET AL., RADIO SCIENCE 6, 369-378, 1971 */
/*                                      AND 8, 837-839, 1973 */
/*       HOWEVER WITH MAGNETIC DIP LATITUDE INSTEAD OF GEOMAGNETIC */
/*       DIPOLE LATITUDE, EYFRIG, 1979 */
/* --------------------------------------------- D. BILITZA, 1988. */
    ret_val = (float)0.;
    if (*chi > (float)90.) {
	return ret_val;
    }
    dla = *ylati;
    f0 = dla * ((float).0058 - dla * (float)1.2e-4) + (float)4.35;
    f100 = dla * ((float).011 - dla * (float)2.3e-4) + (float)5.348;
    fs = f0 + (f100 - f0) * *r__ / (float)100.;
    xmue = dla * ((float).0046 - dla * (float)5.4e-5) + (float).093 + *r__ * (
	    float)3e-4;
    d__1 = (doublereal) cos(*chi * const_1.umr);
    d__2 = (doublereal) xmue;
    fof1 = fs * pow_dd(&d__1, &d__2);
    chi0 = dla * (float).349504 + (float)49.84733;
    chi100 = dla * (float).509932 + (float)38.96113;
    chim = chi0 + (chi100 - chi0) * *r__ / (float)100.;
    if (*chi > chim) {
	fof1 = -fof1;
    }
    ret_val = fof1;
    return ret_val;
} /* fof1ed_ */



doublereal f1_c1__(real *xmodip, real *hour, real *suxnon, real *saxnon)
{
    /* System generated locals */
    real ret_val;

    /* Builtin functions */
    double exp(doublereal), cos(doublereal);

    /* Local variables */
    static real dela, c1old, c1, pi, absmdp;

/* F1 layer shape parameter C1 after Reinisch and Huang, Advances in */
/* Space Research, Volume 25, Number 1, 81-88, 2000. */
    pi = const_1.umr * (float)180.;
    absmdp = dabs(*xmodip);
    dela = (float)4.32;
    if (absmdp >= (float)18.) {
	dela = exp(-(absmdp - (float)30.) / (float)10.) + (float)1.;
    }
    c1old = (float).11 / dela + (float).09;
    if (*suxnon == *saxnon) {
	c1 = c1old * (float)2.5;
    } else {
	c1 = c1old * (float)2.5 * cos((*hour - (float)12.) / (*suxnon - *
		saxnon) * pi);
    }
    if (c1 < (float)0.) {
	c1 = (float)0.;
    }
    ret_val = c1;
    return ret_val;
} /* f1_c1__ */



/* Subroutine */ int f1_prob__(real *sza, real *glat, real *rz12, real *
	f1prob, real *f1probl)
{
    /* System generated locals */
    doublereal d__1, d__2;

    /* Builtin functions */
    double cos(doublereal), pow_dd(doublereal *, doublereal *);

    /* Local variables */
    static real xarg, f1prl, a, b, c__, gamma, f1pr;

/* Occurrence probability of F1 layer after Scotto et al., Advances in */
/* Space Research, Volume 20, Number 9, 1773-1775, 1997. */
/* Input: solar zenith angle (sza) in degrees, geomagnetic latitude */
/* (glat) in degrees, 12-month running mean of sunspot number (Rz12). */
/* Output: F1 occurrence probability without L-condition cases (f1prob) */
/* and with L-condition cases (f1probl) */
    xarg = cos(*sza * const_1.umr) * (float).5 + (float).5;
    a = *rz12 * (float).0854 + (float)2.98;
    b = (float).0107 - *rz12 * (float).0022;
    c__ = *rz12 * (float)1.47e-5 - (float)2.56e-4;
    gamma = a + (b + c__ * *glat) * *glat;
    d__1 = (doublereal) xarg;
    d__2 = (doublereal) gamma;
    f1pr = pow_dd(&d__1, &d__2);
    if (f1pr < (float).001) {
	f1pr = (float)0.;
    }
    *f1prob = f1pr;
    d__1 = (doublereal) xarg;
    f1prl = pow_dd(&d__1, &c_b207);
    if (f1prl < (float).001) {
	f1prl = (float)0.;
    }
    *f1probl = f1prl;
    return 0;
} /* f1_prob__ */



doublereal foeedi_(real *cov, real *xhi, real *xhim, real *xlati)
{
    /* System generated locals */
    real ret_val;
    doublereal d__1, d__2;

    /* Builtin functions */
    double cos(doublereal), pow_dd(doublereal *, doublereal *), exp(
	    doublereal), log(doublereal);

    /* Local variables */
    static real xhic, smin, r4foe, a, b, c__, d__, sl, sm, sp;

/* ------------------------------------------------------- */
/* CALCULATES FOE/MHZ BY THE EDINBURGH-METHOD. */
/* INPUT: MONTHLY MEAN 10.7CM SOLAR RADIO FLUX measured at ground level */
/* (COV), GEOGRAPHIC LATITUDE (XLATI/DEG), SOLAR ZENITH ANGLE (XHI/DEG */
/* AND XHIM/DEG AT NOON). */
/* REFERENCE: */
/*       KOURIS-MUGGELETON, CCIR DOC. 6/3/07, 1973 */
/*       TROST, J. GEOPHYS. RES. 84, 2736, 1979 (was used */
/*               to improve the nighttime varition) */
/* D.BILITZA--------------------------------- AUGUST 1986. */
/* variation with solar activity (factor A) ............... */
    a = (*cov - (float)66.) * (float).0094 + (float)1.;
/* variation with noon solar zenith angle (B) and with latitude (C) */
    sl = cos(*xlati * const_1.umr);
    if (*xlati < (float)32.) {
	sm = sl * (float)1.92 - (float)1.93;
	c__ = sl * (float)116. + (float)23.;
    } else {
	sm = (float).11 - sl * (float).49;
	c__ = sl * (float)35. + (float)92.;
    }
    if (*xhim >= (float)90.) {
	*xhim = (float)89.999;
    }
    d__1 = (doublereal) cos(*xhim * const_1.umr);
    d__2 = (doublereal) sm;
    b = pow_dd(&d__1, &d__2);
/* variation with solar zenith angle (D) .......................... */
    if (*xlati > (float)12.) {
	sp = (float)1.2;
    } else {
	sp = (float)1.31;
    }
/* adjusted solar zenith angle during nighttime (XHIC) ............. */
    xhic = *xhi - log(exp((*xhi - (float)89.98) / (float)3.) + (float)1.) * (
	    float)3.;
    d__1 = (doublereal) cos(xhic * const_1.umr);
    d__2 = (doublereal) sp;
    d__ = pow_dd(&d__1, &d__2);
/* determine foE**4 ................................................ */
    r4foe = a * b * c__ * d__;
/* minimum allowable foE (sqrt[SMIN])............................... */
    smin = (*cov - (float)60.) * (float).0015 + (float).121;
    smin *= smin;
    if (r4foe < smin) {
	r4foe = smin;
    }
    d__1 = (doublereal) r4foe;
    ret_val = pow_dd(&d__1, &c_b209);
    return ret_val;
} /* foeedi_ */



doublereal xmded_(real *xhi, real *r__, real *yw)
{
    /* System generated locals */
    real ret_val;
    doublereal d__1;

    /* Builtin functions */
    double cos(doublereal), pow_dd(doublereal *, doublereal *), exp(
	    doublereal);

    /* Local variables */
    static real y, yy, ymd, yyy;

/* D. BILITZA, 1978, CALCULATES ELECTRON DENSITY OF D MAXIMUM. */
/* XHI/DEG. IS SOLAR ZENITH ANGLE, R SMOOTHED ZURICH SUNSPOT NUMBER */
/* AND YW/M-3 THE ASSUMED CONSTANT NIGHT VALUE. */
/* [REF.: D.BILITZA, WORLD DATA CENTER A REPORT UAG-82,7,BOULDER,1981] */
/* corrected 4/25/97 - D. Bilitza */


    if (*xhi >= (float)90.) {
	goto L100;
    }
    y = *r__ * (float)8.8e6 + (float)6.05e8;
    yy = cos(*xhi * const_1.umr);
    d__1 = (doublereal) yy;
    yyy = (float)-.1 / pow_dd(&d__1, &c_b212);
    if (yyy < (float)-40.) {
	ymd = (float)0.;
    } else {
	ymd = y * exp(yyy);
    }
    if (ymd < *yw) {
	ymd = *yw;
    }
    ret_val = ymd;
    return ret_val;
L100:
    ret_val = *yw;
    return ret_val;
} /* xmded_ */



doublereal gamma1_(real *smodip, real *slat, real *slong, real *hour, integer 
	*iharm, integer *nq, integer *k1, integer *m, integer *mm, integer *
	m3, real *sfe)
{
    /* System generated locals */
    integer i__1, i__2;
    real ret_val;

    /* Builtin functions */
    double sin(doublereal), cos(doublereal);

    /* Local variables */
    static doublereal coef[100], c__[12];
    static integer i__, j, l;
    static doublereal s[12];
    static integer index;
    static real s0, s1, s3, s2, xsinx[13];
    static integer mi, np;
    static real ss, hou;
    static doublereal sum;

/* CALCULATES GAMMA1=FOF2 OR M3000 USING CCIR NUMERICAL MAP */
/* COEFFICIENTS SFE(M3) FOR MODIFIED DIP LATITUDE (SMODIP/DEG) */
/* GEOGRAPHIC LATITUDE (SLAT/DEG) AND LONGITUDE (SLONG/DEG) */
/* AND UNIVERSIAL TIME (HOUR/DECIMAL HOURS). */
/* NQ(K1) IS AN INTEGER ARRAY GIVING THE HIGHEST DEGREES IN */
/* LATITUDE FOR EACH LONGITUDE HARMONIC. */
/* M=1+NQ1+2(NQ2+1)+2(NQ3+1)+... . */
/* SHEIKH,4.3.77. */
    /* Parameter adjustments */
    --nq;
    --sfe;

    /* Function Body */
    hou = (*hour * (float)15. - (float)180.) * const_1.umr;
    s[0] = sin(hou);
    c__[0] = cos(hou);
    i__1 = *iharm;
    for (i__ = 2; i__ <= i__1; ++i__) {
	c__[i__ - 1] = c__[0] * c__[i__ - 2] - s[0] * s[i__ - 2];
	s[i__ - 1] = c__[0] * s[i__ - 2] + s[0] * c__[i__ - 2];
/* L250: */
    }
    i__1 = *m;
    for (i__ = 1; i__ <= i__1; ++i__) {
	mi = (i__ - 1) * *mm;
	coef[i__ - 1] = sfe[mi + 1];
	i__2 = *iharm;
	for (j = 1; j <= i__2; ++j) {
	    coef[i__ - 1] = coef[i__ - 1] + sfe[mi + (j << 1)] * s[j - 1] + 
		    sfe[mi + (j << 1) + 1] * c__[j - 1];
/* L300: */
	}
    }
    sum = coef[0];
    ss = sin(*smodip * const_1.umr);
    s3 = ss;
    xsinx[0] = (float)1.;
    index = nq[1];
    i__2 = index;
    for (j = 1; j <= i__2; ++j) {
	sum += coef[j] * ss;
	xsinx[j] = ss;
	ss *= s3;
/* L350: */
    }
    xsinx[nq[1] + 1] = ss;
    np = nq[1] + 1;
    ss = cos(*slat * const_1.umr);
    s3 = ss;
    i__2 = *k1;
    for (j = 2; j <= i__2; ++j) {
	s0 = *slong * (j - (float)1.) * const_1.umr;
	s1 = cos(s0);
	s2 = sin(s0);
	index = nq[j] + 1;
	i__1 = index;
	for (l = 1; l <= i__1; ++l) {
	    ++np;
	    sum += coef[np - 1] * xsinx[l - 1] * ss * s1;
	    ++np;
	    sum += coef[np - 1] * xsinx[l - 1] * ss * s2;
/* L450: */
	}
	ss *= s3;
/* L400: */
    }
    ret_val = sum;
    return ret_val;
} /* gamma1_ */



/* ************************************************************ */
/* ***************** PROFILE PARAMETERS *********************** */
/* ************************************************************ */


doublereal b0_98__(real *hour, real *sax, real *sux, integer *nseasn, real *
	r__, real *zlo, real *zmodip)
{
    /* Initialized data */

    static real b0f[48]	/* was [2][4][2][3] */ = { (float)201.,(float)68.,(
	    float)210.,(float)61.,(float)192.,(float)68.,(float)199.,(float)
	    67.,(float)240.,(float)80.,(float)245.,(float)83.,(float)233.,(
	    float)71.,(float)230.,(float)65.,(float)108.,(float)65.,(float)
	    142.,(float)81.,(float)110.,(float)68.,(float)77.,(float)75.,(
	    float)124.,(float)98.,(float)164.,(float)100.,(float)120.,(float)
	    94.,(float)96.,(float)112.,(float)78.,(float)81.,(float)94.,(
	    float)84.,(float)81.,(float)81.,(float)65.,(float)70.,(float)102.,
	    (float)87.,(float)127.,(float)91.,(float)109.,(float)88.,(float)
	    81.,(float)78. };
    static real zx[5] = { (float)45.,(float)72.,(float)90.,(float)108.,(float)
	    135. };
    static real dd[5] = { (float)3.,(float)3.,(float)3.,(float)3.,(float)3. };

    /* System generated locals */
    integer i__1;
    real ret_val;

    /* Local variables */
    extern doublereal hpol_(real *, real *, real *, real *, real *, real *, 
	    real *);
    static real dsum;
    extern doublereal eptr_(real *, real *, real *);
    static real g[6];
    static integer i__;
    static real aa, bb, dayval;
    static integer jseasn;
    static real zz, nitval, zx1, zz0, zx2, zx3, zx4, zx5, bfd[6]	/* 
	    was [2][3] */, bfr[12]	/* was [2][2][3] */;
    static integer isd, isl, iss;
    static real sum;
    static integer num_lat__;

/* ----------------------------------------------------------------- */
/* Interpolation procedure for bottomside thickness parameter B0. */
/* Array B0F(ILT,ISEASON,IR,ILATI) distinguishes between day and */
/* night (ILT=1,2), four seasons (ISEASON is northern season with */
/* ISEASON=1 northern spring), low and high solar activity Rz12=10, */
/* 100 (IR=1,2), and modified dip latitudes of 0, 18 and 45 */
/* degress (ILATI=1,2,3). In the DATA statement the first value */
/* corresponds to B0F(1,1,1,1), the second to B0F(2,1,1,1), the */
/* third to B0F(1,2,1,1) and so on. */

/* input: */
/*       hour    LT in decimal hours */
/*       SAX     time of sunrise in decimal hours */
/*       SUX     time of sunset in decimal hours */
/*       nseasn  season in northern hemisphere (1=spring) */
/*       R       12-month running mean of sunspot number */
/*       ZLO     longitude */
/*       ZMODIP  modified dip latitude */

/* JUNE 1989 --------------------------------------- Dieter Bilitza */

/* Updates (B0_new -> B0_98): */

/* 01/98 corrected to include a smooth transition at the modip equator */
/*       and no discontinuity at the equatorial change in season. */
/* 09/98 new B0 values incl values at the magnetic equator */
/* 10/98 longitude as input to determine if magnetic equator in northern */
/*         or southern hemisphere */

    num_lat__ = 3;
/* jseasn is southern hemisphere season */
    jseasn = *nseasn + 2;
    if (jseasn > 4) {
	jseasn += -4;
    }
    zz = *zmodip + (float)90.;
    zz0 = (float)0.;
/* Interpolation in Rz12: linear from 10 to 100 */
    i__1 = num_lat__;
    for (isl = 1; isl <= i__1; ++isl) {
	for (isd = 1; isd <= 2; ++isd) {
	    bfr[isd + ((isl << 1) + 1 << 1) - 7] = b0f[isd + (*nseasn + ((isl 
		    << 1) + 1 << 2) << 1) - 27] + (b0f[isd + (*nseasn + ((isl 
		    << 1) + 2 << 2) << 1) - 27] - b0f[isd + (*nseasn + ((isl 
		    << 1) + 1 << 2) << 1) - 27]) / (float)90. * (*r__ - (
		    float)10.);
	    bfr[isd + ((isl << 1) + 2 << 1) - 7] = b0f[isd + (jseasn + ((isl 
		    << 1) + 1 << 2) << 1) - 27] + (b0f[isd + (jseasn + ((isl 
		    << 1) + 2 << 2) << 1) - 27] - b0f[isd + (jseasn + ((isl <<
		     1) + 1 << 2) << 1) - 27]) / (float)90. * (*r__ - (float)
		    10.);
/* L7034: */
	}
/* Interpolation day/night with transitions at SAX (sunrise) */
/* and SUX (sunset) for northern/southern hemisphere iss=1/2 */
	for (iss = 1; iss <= 2; ++iss) {
	    dayval = bfr[(iss + (isl << 1) << 1) - 6];
	    nitval = bfr[(iss + (isl << 1) << 1) - 5];
	    bfd[iss + (isl << 1) - 3] = hpol_(hour, &dayval, &nitval, sax, 
		    sux, &c_b18, &c_b18);
/* L7033: */
	}
/* L7035: */
    }
/* Interpolation with epstein-transitions in modified dip latitude. */
/* Transitions at +/-18 and +/-45 degrees; constant above +/-45. */

/* g(1:5) are the latitudinal slopes of B0; */
/*       g(1) is for the region from -90 to -45 degrees */
/*       g(2) is for the region from -45 to -18 degrees */
/*       g(3) is for the region from -18 to   0 degrees */
/*       g(4) is for the region from   0 to  18 degrees */
/*       g(5) is for the region from  18 to  45 degrees */
/*       g(6) is for the region from  45 to  90 degrees */

/* B0 =  bfd(2,3) at modip = -45, */
/*       bfd(2,2) at modip = -18, */
/*       bfd(2,1) or bfd(1,1) at modip = 0, */
/*       bfd(1,2) at modip = 20, */
/*       bfd(1,3) at modip = 45. */
/* If the Longitude is between 200 and 320 degrees than the modip */
/* equator is in the southern hemisphere and bfd(2,1) is used at the */
/* equator, otherwise bfd(1,1) is used. */

    zx1 = bfd[5];
    zx2 = bfd[3];
    zx3 = bfd[0];
    if (*zlo > (float)200. && *zlo < (float)320.) {
	zx3 = bfd[1];
    }
    zx4 = bfd[2];
    zx5 = bfd[4];
    g[0] = (float)0.;
    g[1] = (zx2 - zx1) / (float)27.;
    g[2] = (zx3 - zx2) / (float)18.;
    g[3] = (zx4 - zx3) / (float)18.;
    g[4] = (zx5 - zx4) / (float)27.;
    g[5] = (float)0.;
/*        bb0 = bfd(2,3) */
/*      SUM = bb0 */
    sum = zx1;
    for (i__ = 1; i__ <= 5; ++i__) {
	aa = eptr_(&zz, &dd[i__ - 1], &zx[i__ - 1]);
	bb = eptr_(&zz0, &dd[i__ - 1], &zx[i__ - 1]);
	dsum = (g[i__] - g[i__ - 1]) * (aa - bb) * dd[i__ - 1];
	sum += dsum;
/* L1: */
    }
    ret_val = sum;
    return ret_val;
} /* b0_98__ */



/* Subroutine */ int tal_(real *shabr, real *sdelta, real *shbr, real *sdtdh0,
	 logical *aus6, real *spt)
{
    /* Builtin functions */
    double log(doublereal), sqrt(doublereal);

    /* Local variables */
    static real b, c__, z1, z2, z3, z4;

/* CALCULATES THE COEFFICIENTS SPT FOR THE POLYNOMIAL */
/* Y(X)=1+SPT(1)*X**2+SPT(2)*X**3+SPT(3)*X**4+SPT(4)*X**5 */
/* TO FIT THE VALLEY IN Y, REPRESENTED BY: */
/* Y(X=0)=1, THE X VALUE OF THE DEEPEST VALLEY POINT (SHABR), */
/* THE PRECENTAGE DEPTH (SDELTA), THE WIDTH (SHBR) AND THE */
/* DERIVATIVE DY/DX AT THE UPPER VALLEY BOUNDRY (SDTDH0). */
/* IF THERE IS AN UNWANTED ADDITIONAL EXTREMUM IN THE VALLEY */
/* REGION, THEN AUS6=.TRUE., ELSE AUS6=.FALSE.. */
/* FOR -SDELTA THE COEFF. ARE CALCULATED FOR THE FUNCTION */
/* Y(X)=EXP(SPT(1)*X**2+...+SPT(4)*X**5). */
    /* Parameter adjustments */
    --spt;

    /* Function Body */
    z1 = -(*sdelta) / (*shabr * (float)100. * *shabr);
    if (*sdelta > (float)0.) {
	goto L500;
    }
    *sdelta = -(*sdelta);
    z1 = log((float)1. - *sdelta / (float)100.) / (*shabr * *shabr);
L500:
    z3 = *sdtdh0 / (*shbr * (float)2.);
    z4 = *shabr - *shbr;
    spt[4] = (z1 * (*shbr - *shabr * (float)2.) * *shbr + z3 * z4 * *shabr) * 
	    (float)2. / (*shabr * *shbr * z4 * z4 * z4);
    spt[3] = z1 * (*shbr * (float)2. - *shabr * (float)3.) / (*shabr * z4 * 
	    z4) - (*shabr * (float)2. + *shbr) * spt[4];
    spt[2] = z1 * (float)-2. / *shabr - *shabr * (float)2. * spt[3] - *shabr *
	     (float)3. * *shabr * spt[4];
    spt[1] = z1 - *shabr * (spt[2] + *shabr * (spt[3] + *shabr * spt[4]));
    *aus6 = FALSE_;
    b = spt[3] * (float)4. / (spt[4] * (float)5.) + *shabr;
    c__ = spt[1] * (float)-2. / (spt[4] * 5 * *shabr);
    z2 = b * b / (float)4. - c__;
    if (z2 < (float)0.) {
	goto L300;
    }
    z3 = sqrt(z2);
    z1 = b / (float)2.;
    z2 = -z1 + z3;
    if (z2 > (float)0. && z2 < *shbr) {
	*aus6 = TRUE_;
    }
    if (dabs(z3) > (float)1e-15) {
	goto L400;
    }
    z2 = c__ / z2;
    if (z2 > (float)0. && z2 < *shbr) {
	*aus6 = TRUE_;
    }
    return 0;
L400:
    z2 = -z1 - z3;
    if (z2 > (float)0. && z2 < *shbr) {
	*aus6 = TRUE_;
    }
L300:
    return 0;
} /* tal_ */



/* Subroutine */ int valgul_(real *xhi, real *hvb, real *vwu, real *vwa, real 
	*vdp)
{
    /* Builtin functions */
    double cos(doublereal), log(doublereal);

    /* Local variables */
    static real cs, abc, arl, zzz;

/* --------------------------------------------------------------------- */
/*   CALCULATES E-F VALLEY PARAMETERS; T.L. GULYAEVA, ADVANCES IN */
/*   SPACE RESEARCH 7, #6, 39-48, 1987. */

/*       INPUT:  XHI     SOLAR ZENITH ANGLE [DEGREE] */

/*       OUTPUT: VDP     VALLEY DEPTH  (NVB/NME) */
/*               VWU     VALLEY WIDTH  [KM] */
/*               VWA     VALLEY WIDTH  (SMALLER, CORRECTED BY RAWER) */
/*               HVB     HEIGHT OF VALLEY BASE [KM] */
/* ----------------------------------------------------------------------- */


    cs = cos(const_1.umr * *xhi) + (float).1;
    abc = dabs(cs);
    *vdp = cs * (float).45 / (abc + (float).1) + (float).55;
    arl = (abc + (float).1 + cs) / (abc + (float).1 - cs);
    zzz = log(arl);
    *vwu = (float)45. - zzz * (float)10.;
    *vwa = (float)45. - zzz * (float)5.;
    *hvb = (float)1e3 / (cs * (float).224 + (float)7.024 + abc * (float).966);
    return 0;
} /* valgul_ */



/* ************************************************************ */
/* *************** EARTH MAGNETIC FIELD *********************** */
/* ************************************************************** */


/* Subroutine */ int ggm_(integer *art, real *long__, real *lati, real *mlong,
	 real *mlat)
{
    /* Builtin functions */
    double cos(doublereal), sin(doublereal), r_sign(real *, real *), asin(
	    doublereal), acos(doublereal);

    /* Local variables */
    static real ci, si, cbg, cbm, clg, clm, sbg, sbm, slg, slm, ylg, zpi;

/* CALCULATES GEOMAGNETIC LONGITUDE (MLONG) AND LATITUDE (MLAT) */
/* FROM GEOGRAFIC LONGITUDE (LONG) AND LATITUDE (LATI) FOR ART=0 */
/* AND REVERSE FOR ART=1. ALL ANGLES IN DEGREE. */
/* LATITUDE:-90 TO 90. LONGITUDE:0 TO 360 EAST. */
    zpi = const_2.faktor * (float)360.;
    cbg = const_2.faktor * (float)11.4;
    ci = cos(cbg);
    si = sin(cbg);
    if (*art == 0) {
	goto L10;
    }
    cbm = cos(*mlat * const_2.faktor);
    sbm = sin(*mlat * const_2.faktor);
    clm = cos(*mlong * const_2.faktor);
    slm = sin(*mlong * const_2.faktor);
    sbg = sbm * ci - cbm * clm * si;
    if (dabs(sbg) > (float)1.) {
	sbg = r_sign(&c_b18, &sbg);
    }
    *lati = asin(sbg);
    cbg = cos(*lati);
    slg = cbm * slm / cbg;
    clg = (sbm * si + cbm * clm * ci) / cbg;
    if (dabs(clg) > (float)1.) {
	clg = r_sign(&c_b18, &clg);
    }
    *long__ = acos(clg);
    if (slg < (float)0.) {
	*long__ = zpi - *long__;
    }
    *lati /= const_2.faktor;
    *long__ /= const_2.faktor;
    *long__ += (float)-69.8;
    if (*long__ < (float)0.) {
	*long__ += (float)360.;
    }
    return 0;
L10:
    ylg = *long__ + (float)69.8;
    cbg = cos(*lati * const_2.faktor);
    sbg = sin(*lati * const_2.faktor);
    clg = cos(ylg * const_2.faktor);
    slg = sin(ylg * const_2.faktor);
    sbm = sbg * ci + cbg * clg * si;
    if (dabs(sbm) > (float)1.) {
	sbm = r_sign(&c_b18, &sbm);
    }
    *mlat = asin(sbm);
    cbm = cos(*mlat);
    slm = cbg * slg / cbm;
    clm = (-sbg * si + cbg * clg * ci) / cbm;
    if (dabs(clm) > (float)1.) {
	clm = r_sign(&c_b18, &clm);
    }
    *mlong = acos(clm);
    if (slm < (float)0.) {
	*mlong = zpi - *mlong;
    }
    *mlat /= const_2.faktor;
    *mlong /= const_2.faktor;
    return 0;
} /* ggm_ */



/* Subroutine */ int fieldg_(real *dlat, real *dlong, real *alt, real *x, 
	real *y, real *z__, real *f, real *dip, real *dec, real *smodip)
{
    /* Initialized data */

    static real fel1[72] = { (float)0.,(float).1506723,(float).0101742,(float)
	    -.0286519,(float).0092606,(float)-.0130846,(float).0089594,(float)
	    -.0136808,(float)-1.508e-4,(float)-.0093977,(float).013065,(float)
	    .002052,(float)-.0121956,(float)-.0023451,(float)-.0208555,(float)
	    .0068416,(float)-.0142659,(float)-.0093322,(float)-.0021364,(
	    float)-.007891,(float).0045586,(float).0128904,(float)-2.951e-4,(
	    float)-.0237245,(float).0289493,(float).0074605,(float)-.0105741,(
	    float)-5.116e-4,(float)-.0105732,(float)-.0058542,(float).0033268,
	    (float).0078164,(float).0211234,(float).0099309,(float).0362792,(
	    float)-.020107,(float)-.004635,(float)-.0058722,(float).0011147,(
	    float)-.0013949,(float)-.0108838,(float).0322263,(float)-.014739,(
	    float).0031247,(float).0111986,(float)-.0109394,(float).0058112,(
	    float).2739046,(float)-.0155682,(float)-.0253272,(float).0163782,(
	    float).020573,(float).0022081,(float).0112749,(float)-.0098427,(
	    float).0072705,(float).0195189,(float)-.0081132,(float)-.0071889,(
	    float)-.057997,(float)-.0856642,(float).188426,(float)-.7391512,(
	    float).1210288,(float)-.0241888,(float)-.0052464,(float)-.0096312,
	    (float)-.0044834,(float).0201764,(float).0258343,(float).0083033,(
	    float).0077187 };
    static real fel2[72] = { (float).0586055,(float).0102236,(float)-.0396107,
	    (float)-.016786,(float)-.2019911,(float)-.5810815,(float).0379916,
	    (float)3.7508268,(float)1.813303,(float)-.056425,(float)-.0557352,
	    (float).1335347,(float)-.0142641,(float)-.1024618,(float).0970994,
	    (float)-.075183,(float)-.1274948,(float).0402073,(float).038629,(
	    float).1883088,(float).183896,(float)-.7848989,(float).7591817,(
	    float)-.9302389,(float)-.856096,(float).663325,(float)-4.6363869,(
	    float)-13.2599277,(float).1002136,(float).0855714,(float)
	    -.0991981,(float)-.0765378,(float)-.0455264,(float).1169326,(
	    float)-.2604067,(float).1800076,(float)-.2223685,(float)-.6347679,
	    (float).5334222,(float)-.3459502,(float)-.1573697,(float).8589464,
	    (float)1.781599,(float)-6.3347645,(float)-3.1513653,(float)
	    -9.992775,(float)13.3327637,(float)-35.4897308,(float)37.3466339,(
	    float)-.5257398,(float).0571474,(float)-.5421217,(float).240477,(
	    float)-.1747774,(float)-.3433644,(float).4829708,(float).3935944,(
	    float).4885033,(float).8488121,(float)-.7640999,(float)-1.8884945,
	    (float)3.2930784,(float)-7.3497229,(float).1672821,(float)
	    -.2306652,(float)10.5782146,(float)12.6031065,(float)8.6579742,(
	    float)215.5209961,(float)-27.141922,(float)22.3405762,(float)
	    1108.6394043 };

    /* System generated locals */
    integer i__1;

    /* Builtin functions */
    double sin(doublereal), cos(doublereal), sqrt(doublereal), r_sign(real *, 
	    real *), asin(doublereal);

    /* Local variables */
    static integer imax;
    static real rlat;
    static integer nmax, last;
    static real d__, g[144], h__[144];
    static integer i__, k, m;
    static real s;
    static integer ihmax;
    static real rlong, zdivf, f1, ydivs, x1, y1, z1;
    static integer ih;
    static real cp;
    static integer il;
    static real ct, xi[3], sp, rq, st, xt, dipdiv, rho, xxx, yyy, brh0, zzz;

/* THIS IS A SPECIAL VERSION OF THE POGO 68/10 MAGNETIC FIELD */
/* LEGENDRE MODEL. TRANSFORMATION COEFF. G(144) VALID FOR 1973. */
/* INPUT: DLAT, DLONG=GEOGRAPHIC COORDINATES/DEG.(-90/90,0/360), */
/*        ALT=ALTITUDE/KM. */
/* OUTPUT: F TOTAL FIELD (GAUSS), Z DOWNWARD VERTICAL COMPONENT */
/*        X,Y COMPONENTS IN THE EQUATORIAL PLANE (X TO ZERO LONGITUDE). */
/*        DIP INCLINATION ANGLE(DEGREE). SMODIP RAWER'S MODFIED DIP. */
/* SHEIK,1977. */
    k = 0;
    for (i__ = 1; i__ <= 72; ++i__) {
	++k;
	g[k - 1] = fel1[i__ - 1];
/* L10: */
	g[k + 71] = fel2[i__ - 1];
    }
    rlat = *dlat * const_1.umr;
    ct = sin(rlat);
    st = cos(rlat);
    nmax = 11;
    d__ = sqrt((float)40680925. - ct * (float)272336. * ct);
    rlong = *dlong * const_1.umr;
    cp = cos(rlong);
    sp = sin(rlong);
    zzz = (*alt + (float)40408589. / d__) * ct / (float)6371.2;
    rho = (*alt + (float)40680925. / d__) * st / (float)6371.2;
    xxx = rho * cp;
    yyy = rho * sp;
    rq = (float)1. / (xxx * xxx + yyy * yyy + zzz * zzz);
    xi[0] = xxx * rq;
    xi[1] = yyy * rq;
    xi[2] = zzz * rq;
    ihmax = nmax * nmax + 1;
    last = ihmax + nmax + nmax;
    imax = nmax + nmax - 1;
    i__1 = last;
    for (i__ = ihmax; i__ <= i__1; ++i__) {
/* L100: */
	h__[i__ - 1] = g[i__ - 1];
    }
    for (k = 1; k <= 3; k += 2) {
	i__ = imax;
	ih = ihmax;
L300:
	il = ih - i__;
	f1 = (float)2. / (i__ - k + (float)2.);
	x1 = xi[0] * f1;
	y1 = xi[1] * f1;
	z1 = xi[2] * (f1 + f1);
	i__ += -2;
	if (i__ - 1 < 0) {
	    goto L400;
	}
	if (i__ - 1 == 0) {
	    goto L500;
	}
	i__1 = i__;
	for (m = 3; m <= i__1; m += 2) {
	    h__[il + m] = g[il + m] + z1 * h__[ih + m] + x1 * (h__[ih + m + 2]
		     - h__[ih + m - 2]) - y1 * (h__[ih + m + 1] + h__[ih + m 
		    - 3]);
	    h__[il + m - 1] = g[il + m - 1] + z1 * h__[ih + m - 1] + x1 * (
		    h__[ih + m + 1] - h__[ih + m - 3]) + y1 * (h__[ih + m + 2]
		     + h__[ih + m - 2]);
/* L600: */
	}
L500:
	h__[il + 1] = g[il + 1] + z1 * h__[ih + 1] + x1 * h__[ih + 3] - y1 * (
		h__[ih + 2] + h__[ih - 1]);
	h__[il] = g[il] + z1 * h__[ih] + y1 * h__[ih + 3] + x1 * (h__[ih + 2] 
		- h__[ih - 1]);
L400:
	h__[il - 1] = g[il - 1] + z1 * h__[ih - 1] + (x1 * h__[ih] + y1 * h__[
		ih + 1]) * (float)2.;
/* L700: */
	ih = il;
	if (i__ >= k) {
	    goto L300;
	}
/* L200: */
    }
    s = h__[0] * (float).5 + (h__[1] * xi[2] + h__[2] * xi[0] + h__[3] * xi[1]
	    ) * (float)2.;
    xt = (rq + rq) * sqrt(rq);
    *x = xt * (h__[2] - s * xxx);
    *y = xt * (h__[3] - s * yyy);
    *z__ = xt * (h__[1] - s * zzz);
    *f = sqrt(*x * *x + *y * *y + *z__ * *z__);
    brh0 = *y * sp + *x * cp;
    *y = *y * cp - *x * sp;
    *x = *z__ * st - brh0 * ct;
    *z__ = -(*z__) * ct - brh0 * st;
    zdivf = *z__ / *f;
    if (dabs(zdivf) > (float)1.) {
	zdivf = r_sign(&c_b18, &zdivf);
    }
    *dip = asin(zdivf);
    ydivs = *y / sqrt(*x * *x + *y * *y);
    if (dabs(ydivs) > (float)1.) {
	ydivs = r_sign(&c_b18, &ydivs);
    }
    *dec = asin(ydivs);
    dipdiv = *dip / sqrt(*dip * *dip + st);
    if (dabs(dipdiv) > (float)1.) {
	dipdiv = r_sign(&c_b18, &dipdiv);
    }
    *smodip = asin(dipdiv);
    *dip /= const_1.umr;
    *dec /= const_1.umr;
    *smodip /= const_1.umr;
    return 0;
} /* fieldg_ */



/* ************************************************************ */
/* *********** INTERPOLATION AND REST *************************** */
/* ************************************************************** */


/* Subroutine */ int regfa1_(real *x11, real *x22, real *fx11, real *fx22, 
	real *eps, real *fw, E_fp f, logical *schalt, real *x)
{
    /* System generated locals */
    real r__1;

    /* Local variables */
    static logical k, links;
    static real f1, f2;
    static logical l1;
    static real x1, x2, ep;
    static integer ng;
    static real dx, fx;
    static integer lfd;

/* REGULA-FALSI-PROCEDURE TO FIND X WITH F(X)-FW=0. X1,X2 ARE THE */
/* STARTING VALUES. THE COMUTATION ENDS WHEN THE X-INTERVAL */
/* HAS BECOME LESS THAN EPS . IF SIGN(F(X1)-FW)= SIGN(F(X2)-FW) */
/* THEN SCHALT=.TRUE. */
    *schalt = FALSE_;
    ep = *eps;
    x1 = *x11;
    x2 = *x22;
    f1 = *fx11 - *fw;
    f2 = *fx22 - *fw;
    k = FALSE_;
    ng = 2;
    lfd = 0;
    if (f1 * f2 <= (float)0.) {
	goto L200;
    }
    *x = (float)0.;
    *schalt = TRUE_;
    return 0;
L200:
    *x = (x1 * f2 - x2 * f1) / (f2 - f1);
    goto L400;
L300:
    l1 = links;
    dx = (x2 - x1) / ng;
    if (! links) {
	dx *= ng - 1;
    }
    *x = x1 + dx;
L400:
    fx = (*f)(x) - *fw;
    ++lfd;
    if (lfd > 20) {
	ep *= (float)10.;
	lfd = 0;
    }
    links = f1 * fx > (float)0.;
    k = ! k;
    if (links) {
	x1 = *x;
	f1 = fx;
    } else {
	x2 = *x;
	f2 = fx;
    }
    if ((r__1 = x2 - x1, dabs(r__1)) <= ep) {
	goto L800;
    }
    if (k) {
	goto L300;
    }
    if (links && ! l1 || ! links && l1) {
	ng <<= 1;
    }
    goto L200;
L800:
    return 0;
} /* regfa1_ */



/* ****************************************************************** */
/* ********** ZENITH ANGLE, DAY OF YEAR, TIME *********************** */
/* ****************************************************************** */


/* Subroutine */ int soco_(integer *ld, real *t, real *flat, real *elon, real 
	*height, real *declin, real *zenith, real *sunrse, real *sunset)
{
    /* Initialized data */

    static real p1 = (float).017203534;
    static real p2 = (float).034407068;
    static real p3 = (float).051610602;
    static real p4 = (float).068814136;
    static real p6 = (float).103221204;

    /* Builtin functions */
    double sin(doublereal), cos(doublereal), r_sign(real *, real *), acos(
	    doublereal), sqrt(doublereal);

    /* Local variables */
    static real chih, cosx, wlon, a, b, h__, dc, fa, ch, td, te, tf, et, 
	    secphi, cosphi, dcl, phi, eqt;

/* -------------------------------------------------------------------- */
/*       s/r to calculate the solar declination, zenith angle, and */
/*       sunrise & sunset times  - based on Newbern Smith's algorithm */
/*       [leo mcnamara, 1-sep-86, last modified 16-jun-87] */
/*       {dieter bilitza, 30-oct-89, modified for IRI application} */

/* in:   ld      local day of year */
/*       t       local hour (decimal) */
/*       flat    northern latitude in degrees */
/*       elon    east longitude in degrees */
/* 		height	height in km */

/* out:  declin      declination of the sun in degrees */
/*       zenith      zenith angle of the sun in degrees */
/*       sunrse      local time of sunrise in hours */
/*       sunset      local time of sunset in hours */
/* ------------------------------------------------------------------- */

/* amplitudes of Fourier coefficients  --  1955 epoch................. */

/* s/r is formulated in terms of WEST longitude....................... */
    wlon = (float)360. - *elon;

/* time of equinox for 1980........................................... */
    td = *ld + (*t + wlon / (float)15.) / (float)24.;
    te = td + (float).9369;

/* declination of the sun.............................................. */
    dcl = sin(p1 * (te - (float)82.242)) * (float)23.256 + sin(p2 * (te - (
	    float)44.855)) * (float).381 + sin(p3 * (te - (float)23.355)) * (
	    float).167 - sin(p4 * (te + (float)11.97)) * (float).013 + sin(p6 
	    * (te - (float)10.41)) * (float).011 + (float).339137;
    *declin = dcl;
    dc = dcl * const_3.dtr;

/* the equation of time................................................ */
    tf = te - (float).5;
    eqt = sin(p1 * (tf - (float)4.)) * (float)-7.38 - sin(p2 * (tf + (float)
	    9.)) * (float)9.87 + sin(p3 * (tf - (float)53.)) * (float).27 - 
	    cos(p4 * (tf - (float)17.)) * (float).2;
    et = eqt * const_3.dtr / (float)4.;

    fa = *flat * const_3.dtr;
    phi = const1_1.humr * (*t - (float)12.) + et;

    a = sin(fa) * sin(dc);
    b = cos(fa) * cos(dc);
    cosx = a + b * cos(phi);
    if (dabs(cosx) > (float)1.) {
	cosx = r_sign(&c_b18, &cosx);
    }
    *zenith = acos(cosx) / const_3.dtr;

/* calculate sunrise and sunset times --  at the ground........... */
/* see Explanatory Supplement to the Ephemeris (1961) pg 401...... */
/* sunrise at height h metres is at............................... */
    h__ = *height * (float)1e3;
    chih = sqrt(h__) * (float).0347 + (float)90.83;
/* this includes corrections for horizontal refraction and........ */
/* semi-diameter of the solar disk................................ */
    ch = cos(chih * const_3.dtr);
    cosphi = (ch - a) / b;
/* if abs(secphi) > 1., sun does not rise/set..................... */
/* allow for sun never setting - high latitude summer............. */
    secphi = (float)999999.;
    if (cosphi != (float)0.) {
	secphi = (float)1. / cosphi;
    }
    *sunset = (float)99.;
    *sunrse = (float)99.;
    if (secphi > (float)-1. && secphi <= (float)0.) {
	return 0;
    }
/* allow for sun never rising - high latitude winter.............. */
    *sunset = (float)-99.;
    *sunrse = (float)-99.;
    if (secphi > (float)0. && secphi < (float)1.) {
	return 0;
    }

    if (cosphi > (float)1.) {
	cosphi = r_sign(&c_b18, &cosphi);
    }
    phi = acos(cosphi);
    et /= const1_1.humr;
    phi /= const1_1.humr;
    *sunrse = (float)12. - phi - et;
    *sunset = phi + (float)12. - et;
    if (*sunrse < (float)0.) {
	*sunrse += (float)24.;
    }
    if (*sunset >= (float)24.) {
	*sunset += (float)-24.;
    }

    return 0;
} /* soco_ */



doublereal hpol_(real *hour, real *tw, real *xnw, real *sa, real *su, real *
	dsa, real *dsu)
{
    /* System generated locals */
    real ret_val;

    /* Local variables */
    extern doublereal epst_(real *, real *, real *);

/* ------------------------------------------------------- */
/* PROCEDURE FOR SMOOTH TIME-INTERPOLATION USING EPSTEIN */
/* STEP FUNCTION AT SUNRISE (SA) AND SUNSET (SU). THE */
/* STEP-WIDTH FOR SUNRISE IS DSA AND FOR SUNSET DSU. */
/* TW,NW ARE THE DAY AND NIGHT VALUE OF THE PARAMETER TO */
/* BE INTERPOLATED. SA AND SU ARE TIME OF SUNRIES AND */
/* SUNSET IN DECIMAL HOURS. */
/* BILITZA----------------------------------------- 1979. */
    if (dabs(*su) > (float)25.) {
	if (*su > (float)0.) {
	    ret_val = *tw;
	} else {
	    ret_val = *xnw;
	}
	return ret_val;
    }
    ret_val = *xnw + (*tw - *xnw) * epst_(hour, dsa, sa) + (*xnw - *tw) * 
	    epst_(hour, dsu, su);
    return ret_val;
} /* hpol_ */



/* Subroutine */ int moda_(integer *in, integer *iyear, integer *month, 
	integer *iday, integer *idoy, integer *nrdaymo)
{
    /* Initialized data */

    static integer mm[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };

    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer mobe, i__, moold, mosum, imo;

/* ------------------------------------------------------------------- */
/* CALCULATES DAY OF YEAR (IDOY, ddd) FROM YEAR (IYEAR, yy or yyyy), */
/* MONTH (MONTH, mm) AND DAY OF MONTH (IDAY, dd) IF IN=0, OR MONTH */
/* AND DAY FROM YEAR AND DAY OF YEAR IF IN=1. NRDAYMO is an output */
/* parameter providing the number of days in the specific month. */
/* ------------------------------------------------------------------- */
    imo = 0;
    mobe = 0;

/*  leap year rule: years evenly divisible by 4 are leap years, except */
/*  years also evenly divisible by 100 are not leap years, except years */
/*  also evenly divisible by 400 are leap years. The year 2000 therefore */
/*  is a leap year. The 100 and 400 year exception rule */
/*     if((iyear/4*4.eq.iyear).and.(iyear/100*100.ne.iyear)) mm(2)=29 */
/*  will become important again in the year 2100 which is not a leap */
/*  year. */

    mm[1] = 28;
    if (*iyear / 4 << 2 == *iyear) {
	mm[1] = 29;
    }
    if (*in > 0) {
	goto L5;
    }
    mosum = 0;
    if (*month > 1) {
	i__1 = *month - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
/* L1234: */
	    mosum += mm[i__ - 1];
	}
    }
    *idoy = mosum + *iday;
    *nrdaymo = mm[*month - 1];
    return 0;
L5:
    ++imo;
    if (imo > 12) {
	goto L55;
    }
    moold = mobe;
    *nrdaymo = mm[imo - 1];
    mobe += *nrdaymo;
    if (mobe < *idoy) {
	goto L5;
    }
L55:
    *month = imo;
    *iday = *idoy - moold;
    return 0;
} /* moda_ */



/* Subroutine */ int ut_lt__(integer *mode, real *ut, real *slt, real *glong, 
	integer *iyyy, integer *ddd)
{
    static real xlong;
    static integer dddend;

/* ----------------------------------------------------------------- */
/* Converts Universal Time UT (decimal hours) into Solar Local Time */
/* SLT (decimal hours) for given date (iyyy is year, e.g. 1995; ddd */
/* is day of year, e.g. 1 for Jan 1) and geodatic longitude in degrees. */
/* For mode=0 UT->LT and for mode=1 LT->UT */
/* Please NOTE that iyyy and ddd are input as well as output parameters */
/* since the determined LT may be for a day before or after the UT day. */
/* ------------------------------------------------- bilitza nov 95 */
    xlong = *glong;
    if (*glong > (float)180.) {
	xlong = *glong - 360;
    }
    if (*mode != 0) {
	goto L1;
    }

/* UT ---> LT */

    *slt = *ut + xlong / (float)15.;
    if (*slt >= (float)0. && *slt <= (float)24.) {
	goto L2;
    }
    if (*slt > (float)24.) {
	goto L3;
    }
    *slt += (float)24.;
    --(*ddd);
    if ((real) (*ddd) < (float)1.) {
	--(*iyyy);
	*ddd = 365;

/* leap year if evenly divisible by 4 and not by 100, except if evenly */
/* divisible by 400. Thus 2000 will be a leap year. */

	if (*iyyy / 4 << 2 == *iyyy) {
	    *ddd = 366;
	}
    }
    goto L2;
L3:
    *slt += (float)-24.;
    ++(*ddd);
    dddend = 365;
    if (*iyyy / 4 << 2 == *iyyy) {
	dddend = 366;
    }
    if (*ddd > dddend) {
	++(*iyyy);
	*ddd = 1;
    }
    goto L2;

/* LT ---> UT */

L1:
    *ut = *slt - xlong / (float)15.;
    if (*ut >= (float)0. && *ut <= (float)24.) {
	goto L2;
    }
    if (*ut > (float)24.) {
	goto L5;
    }
    *ut += (float)24.;
    --(*ddd);
    if ((real) (*ddd) < (float)1.) {
	--(*iyyy);
	*ddd = 365;
	if (*iyyy / 4 << 2 == *iyyy) {
	    *ddd = 366;
	}
    }
    goto L2;
L5:
    *ut += (float)-24.;
    ++(*ddd);
    dddend = 365;
    if (*iyyy / 4 << 2 == *iyyy) {
	dddend = 366;
    }
    if (*ddd > dddend) {
	++(*iyyy);
	*ddd = 1;
    }
L2:
    return 0;
} /* ut_lt__ */



/* ********************************************************************* */
/* ************************ EPSTEIN FUNCTIONS ************************** */
/* ********************************************************************* */
/* REF:  H. G. BOOKER, J. ATMOS. TERR. PHYS. 39, 619-623, 1977 */
/*       K. RAWER, ADV. SPACE RES. 4, #1, 11-15, 1984 */
/* ********************************************************************* */


doublereal rlay_(real *x, real *xm, real *sc, real *hx)
{
    /* System generated locals */
    real ret_val;

    /* Local variables */
    extern doublereal eptr_(real *, real *, real *), epst_(real *, real *, 
	    real *);
    static real y1, y1m, y2m;

/* -------------------------------------------------------- RAWER  LAYER */
    y1 = eptr_(x, sc, hx);
    y1m = eptr_(xm, sc, hx);
    y2m = epst_(xm, sc, hx);
    ret_val = y1 - y1m - (*x - *xm) * y2m / *sc;
    return ret_val;
} /* rlay_ */



doublereal d1lay_(real *x, real *xm, real *sc, real *hx)
{
    /* System generated locals */
    real ret_val;

    /* Local variables */
    extern doublereal epst_(real *, real *, real *);

/* ------------------------------------------------------------ dLAY/dX */
    ret_val = (epst_(x, sc, hx) - epst_(xm, sc, hx)) / *sc;
    return ret_val;
} /* d1lay_ */



doublereal d2lay_(real *x, real *xm, real *sc, real *hx)
{
    /* System generated locals */
    real ret_val;

    /* Local variables */
    extern doublereal epla_(real *, real *, real *);

/* ---------------------------------------------------------- d2LAY/dX2 */
    ret_val = epla_(x, sc, hx) / (*sc * *sc);
    return ret_val;
} /* d2lay_ */



doublereal eptr_(real *x, real *sc, real *hx)
{
    /* System generated locals */
    real ret_val;

    /* Builtin functions */
    double exp(doublereal), log(doublereal);

    /* Local variables */
    static real d1;

/* --------------------------------------------------------- TRANSITION */
    d1 = (*x - *hx) / *sc;
    if (dabs(d1) < argexp_1.argmax) {
	goto L1;
    }
    if (d1 > (float)0.) {
	ret_val = d1;
    } else {
	ret_val = (float)0.;
    }
    return ret_val;
L1:
    ret_val = log(exp(d1) + (float)1.);
    return ret_val;
} /* eptr_ */



doublereal epst_(real *x, real *sc, real *hx)
{
    /* System generated locals */
    real ret_val;

    /* Builtin functions */
    double exp(doublereal);

    /* Local variables */
    static real d1;

/* -------------------------------------------------------------- STEP */
    d1 = (*x - *hx) / *sc;
    if (dabs(d1) < argexp_1.argmax) {
	goto L1;
    }
    if (d1 > (float)0.) {
	ret_val = (float)1.;
    } else {
	ret_val = (float)0.;
    }
    return ret_val;
L1:
    ret_val = (float)1. / (exp(-d1) + (float)1.);
    return ret_val;
} /* epst_ */



doublereal epstep_(real *y2, real *y1, real *sc, real *hx, real *x)
{
    /* System generated locals */
    real ret_val;

    /* Local variables */
    extern doublereal epst_(real *, real *, real *);

/* ---------------------------------------------- STEP FROM Y1 TO Y2 */
    ret_val = *y1 + (*y2 - *y1) * epst_(x, sc, hx);
    return ret_val;
} /* epstep_ */



doublereal epla_(real *x, real *sc, real *hx)
{
    /* System generated locals */
    real ret_val;

    /* Builtin functions */
    double exp(doublereal);

    /* Local variables */
    static real d0, d1, d2;

/* ------------------------------------------------------------ PEAK */
    d1 = (*x - *hx) / *sc;
    if (dabs(d1) < argexp_1.argmax) {
	goto L1;
    }
    ret_val = (float)0.;
    return ret_val;
L1:
    d0 = exp(d1);
    d2 = d0 + (float)1.;
    ret_val = d0 / (d2 * d2);
    return ret_val;
} /* epla_ */



doublereal xe2to5_(real *h__, real *hmf2, integer *nl, real *hx, real *sc, 
	real *amp)
{
    /* System generated locals */
    integer i__1;
    real ret_val;
    doublereal d__1;

    /* Builtin functions */
    double pow_dd(doublereal *, doublereal *);

    /* Local variables */
    extern doublereal rlay_(real *, real *, real *, real *);
    static real ylay, zlay;
    static integer i__;
    static real sum;

/* ---------------------------------------------------------------------- */
/* NORMALIZED ELECTRON DENSITY (N/NMF2) FOR THE MIDDLE IONOSPHERE FROM */
/* HME TO HMF2 USING LAY-FUNCTIONS. */
/* ---------------------------------------------------------------------- */
    /* Parameter adjustments */
    --amp;
    --sc;
    --hx;

    /* Function Body */
    sum = (float)1.;
    i__1 = *nl;
    for (i__ = 1; i__ <= i__1; ++i__) {
	ylay = amp[i__] * rlay_(h__, hmf2, &sc[i__], &hx[i__]);
	d__1 = (doublereal) ylay;
	zlay = pow_dd(&c_b30, &d__1);
/* L1: */
	sum *= zlay;
    }
    ret_val = sum;
    return ret_val;
} /* xe2to5_ */



doublereal xen_(real *h__, real *hmf2, real *xnmf2, real *hme, integer *nl, 
	real *hx, real *sc, real *amp)
{
    /* System generated locals */
    real ret_val;

    /* Local variables */
    extern doublereal xe2to5_(real *, real *, integer *, real *, real *, real 
	    *), xe1_(real *), xe6_(real *);

/* ---------------------------------------------------------------------- */
/* ELECTRON DENSITY WITH NEW MIDDLE IONOSPHERE */
/* ---------------------------------------------------------------------- */

    /* Parameter adjustments */
    --amp;
    --sc;
    --hx;

    /* Function Body */
    if (*h__ < *hmf2) {
	goto L100;
    }
    ret_val = xe1_(h__);
    return ret_val;
L100:
    if (*h__ < *hme) {
	goto L200;
    }
    ret_val = *xnmf2 * xe2to5_(h__, hmf2, nl, &hx[1], &sc[1], &amp[1]);
    return ret_val;
L200:
    ret_val = xe6_(h__);
    return ret_val;
} /* xen_ */



/* Subroutine */ int rogul_(integer *iday, real *xhi, real *sx, real *gro)
{
    /* Builtin functions */
    double cos(doublereal), exp(doublereal);

    /* Local variables */
    static real xs;

/* --------------------------------------------------------------------- */
/*   CALCULATES RATIO H0.5/HMF2 FOR HALF-DENSITY POINT (NE(H0.5)=0.5* */
/*   NMF2) T. GULYAEVA, ADVANCES IN SPACE RESEARCH 7, #6, 39-48, 1987. */

/*       INPUT:  IDAY    DAY OF YEAR */
/*               XHI     SOLAR ZENITH ANGLE [DEGREE] */

/*       OUTPUT: GRO     RATIO OF HALF DENSITY HEIGHT TO F PEAK HEIGHT */
/*               SX      SMOOTHLY VARYING SEASON PARAMTER (SX=1 FOR */
/*                       DAY=1; SX=3 FOR DAY=180; SX=2 FOR EQUINOX) */
/* --------------------------------------------------------------------- */

    *sx = (float)2. - cos(*iday * const1_1.dumr);
    xs = (*xhi - *sx * (float)20.) / (float)15.;
    *gro = (float).8 - (float).2 / (exp(xs) + (float)1.);
/* same as gro=0.6+0.2/(1+exp(-xs)) */
    return 0;
} /* rogul_ */



/* Subroutine */ int lnglsn_(integer *n, real *a, real *b, logical *aus)
{
    /* System generated locals */
    integer i__1, i__2, i__3;
    real r__1;

    /* Local variables */
    static real amax;
    static integer imax, k, l, m, nn, izg;
    static real hsp, azv[10];

/* -------------------------------------------------------------------- */
/* SOLVES QUADRATIC SYSTEM OF LINEAR EQUATIONS: */

/*       INPUT:  N       NUMBER OF EQUATIONS (= NUMBER OF UNKNOWNS) */
/*               A(N,N)  MATRIX (LEFT SIDE OF SYSTEM OF EQUATIONS) */
/*               B(N)    VECTOR (RIGHT SIDE OF SYSTEM) */

/*       OUTPUT: AUS     =.TRUE.   NO SOLUTION FOUND */
/*                       =.FALSE.  SOLUTION IS IN  A(N,J) FOR J=1,N */
/* -------------------------------------------------------------------- */


    /* Parameter adjustments */
    --b;
    a -= 6;

    /* Function Body */
    nn = *n - 1;
    *aus = FALSE_;
    i__1 = *n - 1;
    for (k = 1; k <= i__1; ++k) {
	imax = k;
	l = k;
	izg = 0;
	amax = (r__1 = a[k + k * 5], dabs(r__1));
L110:
	++l;
	if (l > *n) {
	    goto L111;
	}
	hsp = (r__1 = a[l + k * 5], dabs(r__1));
	if (hsp < (float)1e-8) {
	    ++izg;
	}
	if (hsp <= amax) {
	    goto L110;
	}
L111:
	if (dabs(amax) >= (float)1e-10) {
	    goto L133;
	}
	*aus = TRUE_;
	return 0;
L133:
	if (imax == k) {
	    goto L112;
	}
	i__2 = *n;
	for (l = k; l <= i__2; ++l) {
	    azv[l] = a[imax + l * 5];
	    a[imax + l * 5] = a[k + l * 5];
/* L2: */
	    a[k + l * 5] = azv[l];
	}
	azv[0] = b[imax];
	b[imax] = b[k];
	b[k] = azv[0];
L112:
	if (izg == *n - k) {
	    goto L1;
	}
	amax = (float)1. / a[k + k * 5];
	azv[0] = b[k] * amax;
	i__2 = *n;
	for (m = k + 1; m <= i__2; ++m) {
/* L3: */
	    azv[m] = a[k + m * 5] * amax;
	}
	i__2 = *n;
	for (l = k + 1; l <= i__2; ++l) {
	    amax = a[l + k * 5];
	    if (dabs(amax) < (float)1e-8) {
		goto L4;
	    }
	    a[l + k * 5] = (float)0.;
	    b[l] -= azv[0] * amax;
	    i__3 = *n;
	    for (m = k + 1; m <= i__3; ++m) {
/* L5: */
		a[l + m * 5] -= amax * azv[m];
	    }
L4:
	    ;
	}
L1:
	;
    }
    for (k = *n; k >= 1; --k) {
	amax = (float)0.;
	if (k < *n) {
	    i__1 = *n;
	    for (l = k + 1; l <= i__1; ++l) {
/* L7: */
		amax += a[k + l * 5] * a[*n + l * 5];
	    }
	}
	if ((r__1 = a[k + k * 5], dabs(r__1)) < (float)1e-6) {
	    a[*n + k * 5] = (float)0.;
	} else {
	    a[*n + k * 5] = (b[k] - amax) / a[k + k * 5];
	}
/* L6: */
    }
    return 0;
} /* lnglsn_ */



/* Subroutine */ int lsknm_(integer *n, integer *m, integer *m0, integer *m1, 
	real *hm, real *sc, real *hx, real *w, real *x, real *y, real *var, 
	logical *sing)
{
    /* System generated locals */
    integer i__1, i__2, i__3;

    /* Local variables */
    extern doublereal rlay_(real *, real *, real *, real *), d1lay_(real *, 
	    real *, real *, real *), d2lay_(real *, real *, real *, real *);
    static integer i__, j, k, m01;
    extern /* Subroutine */ int lnglsn_(integer *, real *, real *, logical *);
    static real ali[25]	/* was [5][5] */, bli[5], scm, xli[50]	/* was [5][10]
	     */;

/* -------------------------------------------------------------------- */
/*   DETERMINES LAY-FUNCTIONS AMPLITUDES FOR A NUMBER OF CONSTRAINTS: */

/*       INPUT:  N       NUMBER OF AMPLITUDES ( LAY-FUNCTIONS) */
/*               M       NUMBER OF CONSTRAINTS */
/*               M0      NUMBER OF POINT CONSTRAINTS */
/*               M1      NUMBER OF FIRST DERIVATIVE CONSTRAINTS */
/*               HM      F PEAK ALTITUDE  [KM] */
/*               SC(N)   SCALE PARAMETERS FOR LAY-FUNCTIONS  [KM] */
/*               HX(N)   HEIGHT PARAMETERS FOR LAY-FUNCTIONS  [KM] */
/*               W(M)    WEIGHT OF CONSTRAINTS */
/*               X(M)    ALTITUDES FOR CONSTRAINTS  [KM] */
/*               Y(M)    LOG(DENSITY/NMF2) FOR CONSTRAINTS */

/*       OUTPUT: VAR(M)  AMPLITUDES */
/*               SING    =.TRUE.   NO SOLUTION */
/* --------------------------------------------------------------------- */


    /* Parameter adjustments */
    --var;
    --hx;
    --sc;
    --y;
    --x;
    --w;

    /* Function Body */
    m01 = *m0 + *m1;
    scm = (float)0.;
    for (j = 1; j <= 5; ++j) {
	bli[j - 1] = (float)0.;
	for (i__ = 1; i__ <= 5; ++i__) {
/* L1: */
	    ali[j + i__ * 5 - 6] = (float)0.;
	}
    }
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = *m0;
	for (k = 1; k <= i__2; ++k) {
/* L3: */
	    xli[i__ + k * 5 - 6] = rlay_(&x[k], hm, &sc[i__], &hx[i__]);
	}
	i__2 = m01;
	for (k = *m0 + 1; k <= i__2; ++k) {
/* L4: */
	    xli[i__ + k * 5 - 6] = d1lay_(&x[k], hm, &sc[i__], &hx[i__]);
	}
	i__2 = *m;
	for (k = m01 + 1; k <= i__2; ++k) {
/* L5: */
	    xli[i__ + k * 5 - 6] = d2lay_(&x[k], hm, &sc[i__], &hx[i__]);
	}
/* L2: */
    }
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	i__2 = *m;
	for (k = 1; k <= i__2; ++k) {
	    bli[j - 1] += w[k] * y[k] * xli[j + k * 5 - 6];
	    i__3 = *n;
	    for (i__ = 1; i__ <= i__3; ++i__) {
/* L6: */
		ali[j + i__ * 5 - 6] += w[k] * xli[i__ + k * 5 - 6] * xli[j + 
			k * 5 - 6];
	    }
	}
/* L7: */
    }
    lnglsn_(n, ali, bli, sing);
    if (! (*sing)) {
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) {
/* L8: */
	    var[i__] = ali[*n + i__ * 5 - 6];
	}
    }
    return 0;
} /* lsknm_ */



/* Subroutine */ int inilay_(logical *night, logical *f1reg, real *xnmf2, 
	real *xnmf1, real *xnme, real *vne, real *hmf2, real *hmf1, real *hme,
	 real *hv1, real *hv2, real *hhalf, real *hxl, real *scl, real *amp, 
	integer *iqual)
{
    /* Builtin functions */
    double r_lg10(real *);

    /* Local variables */
    static real hfff, xfff;
    extern doublereal epst_(real *, real *, real *);
    static logical ssin;
    static real alg102, hxl1t, alogf, xhalf;
    extern /* Subroutine */ int lsknm_(integer *, integer *, integer *, 
	    integer *, real *, real *, real *, real *, real *, real *, real *,
	     logical *);
    static real alogef, ww[8], xx[8], yy[8];
    static integer numcon, nc0, nc1, numlay;
    static real zet, scl0;

/* ------------------------------------------------------------------- */
/* CALCULATES AMPLITUDES FOR LAY FUNCTIONS */
/* D. BILITZA, DECEMBER 1988 */

/* INPUT:        NIGHT   LOGICAL VARIABLE FOR DAY/NIGHT DISTINCTION */
/*               F1REG   LOGICAL VARIABLE FOR F1 OCCURRENCE */
/*               XNMF2   F2 PEAK ELECTRON DENSITY [M-3] */
/*               XNMF1   F1 PEAK ELECTRON DENSITY [M-3] */
/*               XNME    E  PEAK ELECTRON DENSITY [M-3] */
/*               VNE     ELECTRON DENSITY AT VALLEY BASE [M-3] */
/*               HMF2    F2 PEAK ALTITUDE [KM] */
/*               HMF1    F1 PEAK ALTITUDE [KM] */
/*               HME     E  PEAK ALTITUDE [KM] */
/*               HV1     ALTITUDE OF VALLEY TOP [KM] */
/*               HV2     ALTITUDE OF VALLEY BASE [KM] */
/*               HHALF   ALTITUDE OF HALF-F2-PEAK-DENSITY [KM] */

/* OUTPUT:       HXL(4)  HEIGHT PARAMETERS FOR LAY FUNCTIONS [KM] */
/*               SCL(4)  SCALE PARAMETERS FOR LAY FUNCTIONS [KM] */
/*               AMP(4)  AMPLITUDES FOR LAY FUNCTIONS */
/*               IQUAL   =0 ok, =1 ok using second choice for HXL(1) */
/*                       =2 NO SOLUTION */
/* --------------------------------------------------------------- */

/* constants -------------------------------------------------------- */
    /* Parameter adjustments */
    --amp;
    --scl;
    --hxl;

    /* Function Body */
    numlay = 4;
    nc1 = 2;
    alg102 = r_lg10(&c_b306);

/* constraints: xx == height     yy == log(Ne/NmF2)    ww == weights */
/* ----------------------------------------------------------------- */
    alogf = r_lg10(xnmf2);
    alogef = r_lg10(xnme) - alogf;
    xhalf = *xnmf2 / (float)2.;
    xx[0] = *hhalf;
    xx[1] = *hv1;
    xx[2] = *hv2;
    xx[3] = *hme;
    xx[4] = *hme - (*hv2 - *hme);
    yy[0] = -alg102;
    yy[1] = alogef;
    yy[2] = r_lg10(vne) - alogf;
    yy[3] = alogef;
    yy[4] = yy[2];
    yy[6] = (float)0.;
    ww[1] = (float)1.;
    ww[2] = (float)2.;
    ww[3] = (float)5.;

/* geometric paramters for LAY ------------------------------------- */
/* difference to earlier version:  HXL(3) = HV2 + SCL(3) */

    scl0 = ((*hmf2 - *hhalf) * (float).216 + (float)56.8) * (float).7;
    scl[1] = scl0 * (float).8;
    scl[2] = (float)10.;
    scl[3] = (float)9.;
    scl[4] = (float)6.;
    hxl[3] = *hv2;

/* DAY CONDITION-------------------------------------------------- */
/* earlier tested:       HXL(2) = HMF1 + SCL(2) */

    if (*night) {
	goto L7711;
    }
    numcon = 8;
    hxl[1] = *hmf2 * (float).9;
    hxl1t = *hhalf;
    hxl[2] = *hmf1;
    hxl[4] = *hme - scl[4];
    xx[5] = *hmf1;
    xx[6] = *hv2;
    xx[7] = *hme;
    yy[7] = (float)0.;
    ww[4] = (float)1.;
    ww[6] = (float)50.;
    ww[7] = (float)500.;
/* without F-region ---------------------------------------------- */
    if (*f1reg) {
	goto L100;
    }
    hxl[2] = (*hmf2 + *hhalf) / (float)2.;
    yy[5] = (float)0.;
    ww[5] = (float)0.;
    ww[0] = (float)1.;
    goto L7722;
/* with F-region -------------------------------------------- */
L100:
    yy[5] = r_lg10(xnmf1) - alogf;
    ww[5] = (float)3.;
    if ((*xnmf1 - xhalf) * (*hmf1 - *hhalf) < (float)0.) {
	ww[0] = (float).5;
    } else {
	zet = yy[0] - yy[5];
	ww[0] = epst_(&zet, &c_b310, &c_b311);
    }
    if (*hhalf > *hmf1) {
	hfff = *hmf1;
	xfff = *xnmf1;
    } else {
	hfff = *hhalf;
	xfff = xhalf;
    }
    goto L7722;

/* NIGHT CONDITION--------------------------------------------------- */
/* different HXL,SCL values were tested including: */
/*       SCL(1) = HMF2 * 0.15 - 27.1     HXL(2) = 200. */
/*       HXL(2) = HMF1 + SCL(2)          HXL(3) = 140. */
/*       SCL(3) = 5.                     HXL(4) = HME + SCL(4) */
/*       HXL(4) = 105. */

L7711:
    numcon = 7;
    hxl[1] = *hhalf;
    hxl1t = *hmf2 * (float).4 + (float)30.;
    hxl[2] = (*hmf2 + *hv1) / (float)2.;
    hxl[4] = *hme;
    xx[5] = *hv2;
    xx[6] = *hme;
    yy[5] = (float)0.;
    ww[0] = (float)1.;
    ww[2] = (float)3.;
    ww[4] = (float).5;
    ww[5] = (float)50.;
    ww[6] = (float)500.;
    hfff = *hhalf;
    xfff = xhalf;

/* are valley-top and bottomside point compatible ? ------------- */

L7722:
    if ((*hv1 - hfff) * (*xnme - xfff) < (float)0.) {
	ww[1] = (float).5;
    }
    if (*hv1 <= *hv2 + (float)5.) {
	ww[1] = (float).5;
    }

/* DETERMINE AMPLITUDES----------------------------------------- */

    nc0 = numcon - nc1;
    *iqual = 0;
L2299:
    lsknm_(&numlay, &numcon, &nc0, &nc1, hmf2, &scl[1], &hxl[1], ww, xx, yy, &
	    amp[1], &ssin);
    if (*iqual > 0) {
	goto L1937;
    }
    if (dabs(amp[1]) > (float)10. || ssin) {
	*iqual = 1;
	hxl[1] = hxl1t;
	goto L2299;
    }
L1937:
    if (ssin) {
	*iqual = 2;
    }
    return 0;
} /* inilay_ */



/* Subroutine */ int tcon_(integer *yr, integer *mm, integer *day, integer *
	idn, real *rz, real *ig, real *rsn, integer *nmonth)
{
    /* Format strings */
    static char fmt_8000[] = "(1x,i10,\002** OUT OF RANGE **\002/,5x,\002The\
 file IG_RZ.DAT which contains the indices Rz12\002,\002 and IG12\002/5x,\
\002currently only covers the time period\002,\002 (yymm) : \002,i6,\002-\
\002,i6)";

    /* System generated locals */
    integer i__1;
    olist o__1;
    cllist cl__1;

    /* Builtin functions */
    integer f_open(olist *), s_rsle(cilist *), do_lio(integer *, integer *, 
	    char *, ftnlen), e_rsle();
    double sqrt(doublereal);
    integer f_clos(cllist *), s_wsfe(cilist *), do_fio(integer *, char *, 
	    ftnlen), e_wsfe();

    /* Local variables */
    extern /* Subroutine */ int moda_(integer *, integer *, integer *, 
	    integer *, integer *, integer *);
    static integer midm, iupd;
    static real covr;
    static integer iupm, imst, iupy, iyst;
    static real ionoindx[722];
    static integer i__, iflag, imend, iyend;
    static real indrz[722];
    static integer iytmp, iymst, inum_vals__, jj;
    static real zi;
    static integer iymend, nrdaym, num;
    static real rrr;
    static integer idd1, idd2, imm2, iyy2;

    /* Fortran I/O blocks */
    static cilist io___719 = { 0, 12, 0, 0, 0 };
    static cilist io___723 = { 0, 12, 0, 0, 0 };
    static cilist io___731 = { 0, 12, 0, 0, 0 };
    static cilist io___734 = { 0, 12, 0, 0, 0 };
    static cilist io___741 = { 0, 0, 0, fmt_8000, 0 };


/* ---------------------------------------------------------------- */
/* input:        yr,mm,day       year(yyyy),month(mm),day(dd) */
/*               idn             day of year(ddd) */
/* output:       rz(3)           12-month-smoothed solar sunspot number */
/*               ig(3)           12-month-smoothed IG index */
/*               rsn             interpolation parameter */
/*               nmonth          previous or following month depending */
/*                               on day */

/* rz(1), ig(1) contain the indices for the month mm and rz(2), ig(2) */
/* for the previous month (if day less than 15) or for the following */
/* month (otherwise). These indices are for the mid of the month. The */
/* indices for the given day are obtained by linear interpolation and */
/* are stored in rz(3) and ig(3). */

/* the indices are obtained from the indices file ig_rz.dat that is */
/* read in subroutine initialize and stored in COMMON/indices/ */
/* ---------------------------------------------------------------- */

/* Rz12 and IG are determined from the file IG_RZ.DAT which has the */
/* following structure: */
/* day, month, year of the last update of this file, */
/* start month, start year, end month, end year, */
/* the 12 IG indices (13-months running mean) for the first year, */
/* the 12 IG indices for the second year and so on until the end year, */
/* the 12 Rz indices (13-months running mean) for the first year, */
/* the 12 Rz indices for the second year and so on until the end year. */
/* The inteporlation procedure also requires the IG and Rz values for */
/* the month preceeding the start month and the IG and Rz values for the */
/* month following the end month. These values are also included in */
/* IG_RZ. */

/* A negative Rz index means that the given index is the 13-months- */
/* running mean of the solar radio flux (F10.7). The close correlation */
/* between (Rz)12 and (F10.7)12 is used to derive the (Rz)12 indices. */

/* An IG index of -111 indicates that no IG values are available for the */
/* time period. In this case a correlation function between (IG)12 and */
/* (Rz)12 is used to obtain (IG)12. */

/* The computation of the 13-month-running mean for month M requires the */
/* indices for the six months preceeding M and the six months following */
/* M (month: M-6, ..., M+6). To calculate the current running mean one */
/* therefore requires predictions of the indix for the next six months. */
/* Starting from six months before the UPDATE DATE (listed at the top of */
/* the file) and onward the indices are therefore based on indices */
/* predictions. */

    /* Parameter adjustments */
    --ig;
    --rz;

    /* Function Body */
    if (iflag == 0) {
	o__1.oerr = 0;
	o__1.ounit = 12;
	o__1.ofnm = "./../data/IonosphereData/ig_rz.dat";				// made changes by Tuan Nguyen on Jan 11, 2013
	o__1.ofnmlen = strlen(o__1.ofnm);	// o__1.ofnmlen = 9;	// made changes by Tuan Nguyen
	o__1.orl = 0;
	o__1.osta = "old";
	o__1.oacc = 0;
	o__1.ofm = 0;
	o__1.oblnk = 0;
	f_open(&o__1);
/* -web- special for web version */
/*          open(unit=12,file= */
/*     *'/usr/local/etc/httpd/cgi-bin/models/IRI/ig_rz.dat', */
/*     *status='old') */
/* Read the update date, the start date and the end date (mm,yyyy), and */
/* get number of data points to read. */
	s_rsle(&io___719);
	do_lio(&c__3, &c__1, (char *)&iupd, (ftnlen)sizeof(integer));
	do_lio(&c__3, &c__1, (char *)&iupm, (ftnlen)sizeof(integer));
	do_lio(&c__3, &c__1, (char *)&iupy, (ftnlen)sizeof(integer));
	e_rsle();
	s_rsle(&io___723);
	do_lio(&c__3, &c__1, (char *)&imst, (ftnlen)sizeof(integer));
	do_lio(&c__3, &c__1, (char *)&iyst, (ftnlen)sizeof(integer));
	do_lio(&c__3, &c__1, (char *)&imend, (ftnlen)sizeof(integer));
	do_lio(&c__3, &c__1, (char *)&iyend, (ftnlen)sizeof(integer));
	e_rsle();
	iymst = iyst * 100 + imst;
	iymend = iyend * 100 + imend;
/* inum_vals= 12-imst+1+(iyend-iyst-1)*12 +imend + 2 */
/*            1st year \ full years       \last y\ before & after */
	inum_vals__ = 3 - imst + (iyend - iyst) * 12 + imend;
/* Read all the ionoindx and indrz values */
	s_rsle(&io___731);
	i__1 = inum_vals__;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    do_lio(&c__4, &c__1, (char *)&ionoindx[i__ - 1], (ftnlen)sizeof(
		    real));
	}
	e_rsle();
	s_rsle(&io___734);
	i__1 = inum_vals__;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    do_lio(&c__4, &c__1, (char *)&indrz[i__ - 1], (ftnlen)sizeof(real)
		    );
	}
	e_rsle();
	i__1 = inum_vals__;
	for (jj = 1; jj <= i__1; ++jj) {
	    rrr = indrz[jj - 1];
	    if (rrr < (float)0.) {
		covr = dabs(rrr);
		rrr = sqrt(covr + (float)85.12) * (float)33.52 - (float)
			408.99;
		if (rrr < (float)0.) {
		    rrr = (float)0.;
		}
		indrz[jj - 1] = rrr;
	    }
	    if (ionoindx[jj - 1] > (float)-90.) {
		goto L1;
	    }
	    zi = ((float)1.4683266 - rrr * (float).00267690893) * rrr - (
		    float)12.349154;
	    if (zi > (float)274.) {
		zi = (float)274.;
	    }
	    ionoindx[jj - 1] = zi;
L1:
	    ;
	}
	cl__1.cerr = 0;
	cl__1.cunit = 12;
	cl__1.csta = 0;
	f_clos(&cl__1);
	iflag = 1;
    }
    iytmp = *yr * 100 + *mm;
    if (iytmp < iymst || iytmp > iymend) {
	if (iounit_1.konsol > 1) {
	    io___741.ciunit = iounit_1.konsol;
	    s_wsfe(&io___741);
	    do_fio(&c__1, (char *)&iytmp, (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&iymst, (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&iymend, (ftnlen)sizeof(integer));
	    e_wsfe();
	}
	*nmonth = -1;
	return 0;
    }
/*       num=12-imst+1+(yr-iyst-1)*12+mm+1 */
    num = 2 - imst + (*yr - iyst) * 12 + *mm;
    rz[1] = indrz[num - 1];
    ig[1] = ionoindx[num - 1];
    midm = 15;
    if (*mm == 2) {
	midm = 14;
    }
    moda_(&c__0, yr, mm, &midm, &idd1, &nrdaym);
    if (*day < midm) {
	goto L1926;
    }
    imm2 = *mm + 1;
    if (imm2 > 12) {
	imm2 = 1;
	iyy2 = *yr + 1;
	idd2 = 380;
/*               if((yr/4*4.eq.yr).and.(yr/100*100.ne.yr)) idd2=381 */
	if (*yr / 4 << 2 == *yr) {
	    idd2 = 381;
	}
	if (*yr / 4 << 2 == *yr) {
	    idd2 = 381;
	}
    } else {
	iyy2 = *yr;
	midm = 15;
	if (imm2 == 2) {
	    midm = 14;
	}
	moda_(&c__0, &iyy2, &imm2, &midm, &idd2, &nrdaym);
    }
    rz[2] = indrz[num];
    ig[2] = ionoindx[num];
    *rsn = (*idn - idd1) * (float)1. / (idd2 - idd1);
    rz[3] = rz[1] + (rz[2] - rz[1]) * *rsn;
    ig[3] = ig[1] + (ig[2] - ig[1]) * *rsn;
    goto L1927;
L1926:
    imm2 = *mm - 1;
    if (imm2 < 1) {
	imm2 = 12;
	idd2 = -16;
	iyy2 = *yr - 1;
    } else {
	iyy2 = *yr;
	midm = 15;
	if (imm2 == 2) {
	    midm = 14;
	}
	moda_(&c__0, &iyy2, &imm2, &midm, &idd2, &nrdaym);
    }
    rz[2] = indrz[num - 2];
    ig[2] = ionoindx[num - 2];
    *rsn = (*idn - idd2) * (float)1. / (idd1 - idd2);
    rz[3] = rz[2] + (rz[1] - rz[2]) * *rsn;
    ig[3] = ig[2] + (ig[1] - ig[2]) * *rsn;
L1927:
    *nmonth = imm2;
    return 0;
} /* tcon_ */



/* Subroutine */ int lstid_(real *fi, integer *icez, real *r__, real *ae, 
	real *tm, real *sax, real *sux, real *ts70, real *df0f2, real *dhf2)
{
    /* Initialized data */

    static real y1[84] = { (float)150.,(float)250.,(float)207.8,(float)140.7,(
	    float)158.3,(float)87.2,(float)158.,(float)150.,(float)250.,(
	    float)207.8,(float)140.7,(float)158.3,(float)87.2,(float)158.,(
	    float)115.,(float)115.,(float)183.5,(float)144.2,(float)161.4,(
	    float)151.9,(float)272.4,(float)115.,(float)115.,(float)183.5,(
	    float)144.2,(float)161.4,(float)151.9,(float)272.4,(float)64.,(
	    float)320.,(float)170.6,(float)122.3,(float)139.,(float)79.6,(
	    float)180.6,(float)64.,(float)320.,(float)170.6,(float)122.3,(
	    float)139.,(float)79.6,(float)180.6,(float)72.,(float)84.,(float)
	    381.9,(float)20.1,(float)75.1,(float)151.2,(float)349.5,(float)
	    120.,(float)252.,(float)311.2,(float)241.,(float)187.4,(float)
	    230.1,(float)168.7,(float)245.,(float)220.,(float)294.7,(float)
	    181.2,(float)135.5,(float)237.7,(float)322.,(float)170.,(float)
	    110.,(float)150.2,(float)136.3,(float)137.4,(float)177.,(float)
	    114.,(float)170.,(float)314.,(float)337.8,(float)155.5,(float)
	    157.4,(float)196.7,(float)161.8,(float)100.,(float)177.,(float)
	    159.8,(float)165.6,(float)137.5,(float)132.2,(float)94.3 };
    static real y2[84] = { (float)2.5,(float)2.,(float)1.57,(float)2.02,(
	    float)2.12,(float)1.46,(float)2.46,(float)2.5,(float)2.,(float)
	    1.57,(float)2.02,(float)2.12,(float)1.46,(float)2.46,(float)2.3,(
	    float)1.6,(float)1.68,(float)1.65,(float)2.09,(float)2.25,(float)
	    2.82,(float)2.3,(float)1.6,(float)1.68,(float)1.65,(float)2.09,(
	    float)2.25,(float)2.82,(float).8,(float)2.,(float)1.41,(float)
	    1.57,(float)1.51,(float)1.46,(float)2.2,(float).8,(float)2.,(
	    float)1.41,(float)1.57,(float)1.51,(float)1.46,(float)2.2,(float)
	    3.7,(float)1.8,(float)3.21,(float)3.31,(float)2.61,(float)2.82,(
	    float)2.34,(float)2.8,(float)3.2,(float)3.32,(float)3.33,(float)
	    2.96,(float)3.43,(float)2.44,(float)3.5,(float)2.8,(float)2.37,(
	    float)2.79,(float)2.26,(float)3.4,(float)2.28,(float)3.9,(float)
	    2.,(float)2.22,(float)1.98,(float)2.33,(float)3.07,(float)1.56,(
	    float)3.7,(float)3.,(float)3.3,(float)2.99,(float)3.57,(float)
	    2.98,(float)3.02,(float)2.6,(float)2.8,(float)1.66,(float)2.04,(
	    float)1.91,(float)1.49,(float).43 };
    static real y3[84] = { (float)-1.8,(float)-1.9,(float)-1.42,(float)-1.51,(
	    float)-1.53,(float)-1.05,(float)-1.66,(float)-1.8,(float)-1.9,(
	    float)-1.42,(float)-1.51,(float)-1.53,(float)-1.05,(float)-1.66,(
	    float)-1.5,(float)-1.3,(float)-1.46,(float)-1.39,(float)-1.53,(
	    float)-1.59,(float)-1.9,(float)-1.5,(float)-1.3,(float)-1.46,(
	    float)-1.39,(float)-1.53,(float)-1.59,(float)-1.9,(float)-.7,(
	    float)-2.,(float)-1.41,(float)-1.09,(float)-1.22,(float)-.84,(
	    float)-1.32,(float)-.7,(float)-2.,(float)-1.41,(float)-1.09,(
	    float)-1.22,(float)-.84,(float)-1.32,(float)-1.7,(float)-1.,(
	    float)-2.08,(float)-1.8,(float)-1.35,(float)-1.55,(float)-1.79,(
	    float)-1.5,(float)-2.,(float)-2.08,(float)-2.16,(float)-1.86,(
	    float)-2.19,(float)-1.7,(float)-2.2,(float)-1.7,(float)-1.57,(
	    float)-1.62,(float)-1.19,(float)-1.89,(float)-1.47,(float)-1.9,(
	    float)-1.5,(float)-1.26,(float)-1.23,(float)-1.52,(float)-1.89,(
	    float)-1.02,(float)-1.7,(float)-1.7,(float)-1.76,(float)-1.43,(
	    float)-1.66,(float)-1.54,(float)-1.24,(float)-1.1,(float)-1.5,(
	    float)-1.09,(float)-1.23,(float)-1.11,(float)-1.14,(float)-.4 };
    static real y4[84] = { (float)-2.,(float)-5.,(float)-5.,(float)0.,(float)
	    0.,(float)0.,(float)2.,(float)-2.,(float)-5.,(float)-5.,(float)0.,
	    (float)0.,(float)0.,(float)2.,(float)-5.,(float)-5.,(float)6.,(
	    float)0.,(float)1.,(float)5.,(float)2.,(float)-5.,(float)-5.,(
	    float)6.,(float)0.,(float)1.,(float)5.,(float)2.,(float)0.,(float)
	    -7.,(float)-3.,(float)-6.,(float)2.,(float)2.,(float)3.,(float)0.,
	    (float)-7.,(float)-3.,(float)-6.,(float)2.,(float)2.,(float)3.,(
	    float)-5.,(float)-1.,(float)-11.,(float)-6.,(float)0.,(float)-5.,(
	    float)-6.,(float)-5.,(float)-10.,(float)1.,(float)4.,(float)-6.,(
	    float)-2.,(float)1.,(float)2.,(float)-13.,(float)-10.,(float)0.,(
	    float)-8.,(float)10.,(float)-16.,(float)0.,(float)-3.,(float)-7.,(
	    float)-2.,(float)-2.,(float)4.,(float)2.,(float)-11.,(float)-12.,(
	    float)-13.,(float)0.,(float)0.,(float)7.,(float)0.,(float)-8.,(
	    float)6.,(float)-1.,(float)-5.,(float)-7.,(float)4.,(float)-4. };
    static real y5[28] = { (float)0.,(float)0.,(float)-.1,(float)-.19,(float)
	    -.19,(float)-.25,(float)-.06,(float)0.,(float)0.,(float)-.31,(
	    float)-.28,(float)-.27,(float)-.06,(float).02,(float)0.,(float)0.,
	    (float).18,(float)-.07,(float)-.2,(float)-.1,(float).3,(float)0.,(
	    float)0.,(float)-.24,(float)-.5,(float)-.4,(float)-.27,(float)
	    -.48 };
    static real y6[28] = { (float)0.,(float)0.,(float)-3.5e-4,(float)-2.8e-4,(
	    float)-3.3e-4,(float)-2.3e-4,(float)-7e-4,(float)0.,(float)0.,(
	    float)-3e-4,(float)-2.5e-4,(float)-3e-4,(float)-6e-4,(float)
	    -7.3e-4,(float)0.,(float)0.,(float)-.0011,(float)-6e-4,(float)
	    -3e-4,(float)-5e-4,(float)-.0015,(float)0.,(float)0.,(float)-8e-4,
	    (float)-.003,(float)-2e-4,(float)-5e-4,(float)-3e-4 };

    /* System generated locals */
    doublereal d__1, d__2;

    /* Builtin functions */
    integer s_wsle(cilist *), do_lio(integer *, integer *, char *, ftnlen), 
	    e_wsle();
    double exp(doublereal), pow_dd(doublereal *, doublereal *);

    /* Local variables */
    static real a[84]	/* was [7][2][3][2] */, b[84]	/* was [7][2][3][2] */
	    , c__[84]	/* was [7][2][3][2] */, d__[84]	/* was [7][2][3][2] */
	    ;
    static integer i__, j, k, m, n;
    static real t, a1[28]	/* was [7][2][2] */, b1[28]	/* was [7][2][
	    2] */;
    static integer n1;
    static real ts, df1, df2, dh1, dh2;
    static integer inn;

    /* Fortran I/O blocks */
    static cilist io___768 = { 0, 6, 0, 0, 0 };
    static cilist io___770 = { 0, 6, 0, 0, 0 };


/* ***************************************************************** */
/*   COMPUTER PROGRAM FOR UPDATING FOF2 AND HMF2 FOR EFFECTS OF */
/*   THE LARGE SCALE SUBSTORM. */

/*   P.V.Kishcha, V.M.Shashunkina, E.E.Goncharova, Modelling of the */
/*   ionospheric effects of isolated and consecutive substorms on */
/*   the basis of routine magnetic data, Geomagn. and Aeronomy v.32, */
/*   N.3, 172-175, 1992. */

/*   P.V.Kishcha et al. Updating the IRI ionospheric model for */
/*   effects of substorms, Adv. Space Res.(in press) 1992. */

/*   Address: Dr. Pavel V. Kishcha, */
/*            Institute of Terrestrial Magnetism,Ionosphere and Radio */
/*            Wave Propagation, Russian Academy of Sciences, */
/*            142092, Troitsk, Moscow Region, Russia */

/* ***       INPUT PARAMETERS: */
/*       FI ------ GEOMAGNETIC LATITUDE, */
/*       ICEZ ---- INDEX OF SEASON(1-WINTER AND EQUINOX,2-SUMMER), */
/*       R ------- ZURICH SUNSPOT NUMBER, */
/*       AE ------ MAXIMUM AE-INDEX REACHED DURING SUBSTORM, */
/*       TM ------ LOCAL TIME, */
/*       SAX,SUX - TIME OF SUNSET AND SUNRISE, */
/*       TS70 ---- ONSET TIME (LT) OF SUBSTORMS ONSET */
/*                        STARTING ON FI=70 DEGR. */
/* ***      OUTPUT PARAMETERS: */
/*       DF0F2,DHF2- CORRECTIONS TO foF2 AND hmF2 FROM IRI OR */
/*                   OBSERVATIONAL MEDIAN  OF THOSE VALUES. */
/* ***************************************************************** */
    inn = 0;
    if (*ts70 > (float)12. && *tm < *sax) {
	inn = 1;
    }
    if (*fi < (float)0.) {
	*fi = dabs(*fi);
    }
    n = 0;
    for (m = 1; m <= 2; ++m) {
	for (k = 1; k <= 3; ++k) {
	    for (j = 1; j <= 2; ++j) {
		for (i__ = 1; i__ <= 7; ++i__) {
		    ++n;
		    a[i__ + (j + (k + m * 3 << 1)) * 7 - 64] = y1[n - 1];
		    b[i__ + (j + (k + m * 3 << 1)) * 7 - 64] = y2[n - 1];
		    c__[i__ + (j + (k + m * 3 << 1)) * 7 - 64] = y3[n - 1];
/* L1: */
		    d__[i__ + (j + (k + m * 3 << 1)) * 7 - 64] = y4[n - 1];
		}
	    }
	}
    }
    n1 = 0;
    for (m = 1; m <= 2; ++m) {
	for (j = 1; j <= 2; ++j) {
	    for (i__ = 1; i__ <= 7; ++i__) {
		++n1;
		a1[i__ + (j + (m << 1)) * 7 - 22] = y5[n1 - 1];
/* L2: */
		b1[i__ + (j + (m << 1)) * 7 - 22] = y6[n1 - 1];
	    }
	}
    }
    if (*fi > (float)65. || *ae < (float)500.) {
	s_wsle(&io___768);
	do_lio(&c__9, &c__1, "LSTID are for AE>500. and ABS(FI)<65.", (ftnlen)
		37);
	e_wsle();
	goto L4;
    }
    ts = *ts70 + (*fi * (float)-1.5571 + (float)109.) / (float)60.;
    if (ts < *sux && ts > *sax) {
	s_wsle(&io___770);
	do_lio(&c__9, &c__1, " LSTID are only at night", (ftnlen)24);
	e_wsle();
	goto L4;
    }
    if (inn == 1) {
	*tm += (float)24.;
    }
    if (ts >= *tm || ts < *tm - (float)5.) {
/*        WRITE(*,*)'LSTID are onli if  TM-5.<TS<TM ;Here TS=',TS, */
/*     &     'TM=',TM */
	goto L4;
    }
    for (i__ = 1; i__ <= 7; ++i__) {
	if (*fi >= (i__ - 1) * (float)10. - (float)5. && *fi < (i__ - 1) * (
		float)10. + (float)5.) {
	    goto L8;
	}
/* L7: */
    }
L8:
    j = *icez;
    if (*ae >= (float)500. && *ae <= (float)755.) {
	k = 1;
    }
    if (*ae > (float)755. && *ae < (float)1e3) {
	k = 2;
    }
    if (*ae >= (float)1e3) {
	k = 3;
    }
    m = -1;
    if (*r__ <= (float)20.) {
	m = 1;
    }
    if (*r__ >= (float)120.) {
	m = 2;
    }
    t = *tm - ts;
    if (m < 0) {
	goto L3;
    }
/*        WRITE(*,*)'A1=',A1(I,J,M),' B1=',B1(I,J,M) */
/*        WRITE(*,*)'A=',A(I,J,K,M),' B=',B(I,J,K,M),' C=',C(I,J,K,M), */
/*     *'D=',D(I,J,K,M) */
    *df0f2 = a1[i__ + (j + (m << 1)) * 7 - 22] + b1[i__ + (j + (m << 1)) * 7 
	    - 22] * *ae;
    d__1 = (doublereal) t;
    d__2 = (doublereal) b[i__ + (j + (k + m * 3 << 1)) * 7 - 64];
    *dhf2 = a[i__ + (j + (k + m * 3 << 1)) * 7 - 64] * pow_dd(&d__1, &d__2) * 
	    exp(c__[i__ + (j + (k + m * 3 << 1)) * 7 - 64] * t) + d__[i__ + (
	    j + (k + m * 3 << 1)) * 7 - 64];
    goto L5;
L3:
    df1 = a1[i__ + (j + 2) * 7 - 22] + b1[i__ + (j + 2) * 7 - 22] * *ae;
    df2 = a1[i__ + (j + 4) * 7 - 22] + b1[i__ + (j + 4) * 7 - 22] * *ae;
    *df0f2 = df1 + (df2 - df1) * (*r__ - (float)20.) / (float)100.;
    d__1 = (doublereal) t;
    d__2 = (doublereal) b[i__ + (j + (k + 3 << 1)) * 7 - 64];
    dh1 = a[i__ + (j + (k + 3 << 1)) * 7 - 64] * pow_dd(&d__1, &d__2) * exp(
	    c__[i__ + (j + (k + 3 << 1)) * 7 - 64] * t) + d__[i__ + (j + (k + 
	    3 << 1)) * 7 - 64];
    d__1 = (doublereal) t;
    d__2 = (doublereal) b[i__ + (j + (k + 6 << 1)) * 7 - 64];
    dh2 = a[i__ + (j + (k + 6 << 1)) * 7 - 64] * pow_dd(&d__1, &d__2) * exp(
	    c__[i__ + (j + (k + 6 << 1)) * 7 - 64] * t) + d__[i__ + (j + (k + 
	    6 << 1)) * 7 - 64];
    *dhf2 = dh1 + (dh2 - dh1) * (*r__ - (float)20.) / (float)100.;
    goto L5;
L4:
    *dhf2 = (float)0.;
    *df0f2 = (float)0.;
L5:
    if (inn == 1) {
	*tm += (float)-24.;
    }
    return 0;
} /* lstid_ */



/* Subroutine */ int apf_(integer *iyyyy, integer *imn, integer *id, real *
	hour, integer *iap)
{
    /* Initialized data */

    static integer lm[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };

    /* Format strings */
    static char fmt_10[] = "(3i3,8i3,f5.1)";
    static char fmt_100[] = "(1x,\002Date is outside range of Ap indices fil\
e.\002,\002 STORM model is turned off.\002)";

    /* System generated locals */
    integer i__1;
    olist o__1;
    cllist cl__1;

    /* Builtin functions */
    integer f_open(olist *), s_rdfe(cilist *), do_fio(integer *, char *, 
	    ftnlen), e_rdfe(), f_clos(cllist *), s_wsfe(cilist *), e_wsfe();

    /* Local variables */
    static integer iiap[8];
    static real f;
    static integer i__, iybeg, ihour, j1, j2, i9, jd, is, jy, jmn, nyd, iss;

    /* Fortran I/O blocks */
    static cilist io___777 = { 1, 13, 0, fmt_10, 1 };
    static cilist io___788 = { 1, 13, 0, fmt_10, 0 };
    static cilist io___792 = { 1, 13, 0, fmt_10, 0 };
    static cilist io___794 = { 1, 13, 0, fmt_10, 0 };
    static cilist io___795 = { 0, 0, 0, fmt_100, 0 };


/* -------------------------------------------------------------------- */
/* Finds 3-hourly Ap indices for IRI-storm for given year IYYYY (yyyy), */
/* month (IMN), day (ID), and UT (HOUR, decimal hours). The indices are */
/* stored in IAP(13) providing the 13 3-hourly indices prior to HOUR. */
/* The 3-hour UT intervals during the day are: (0-3),)3-6),)6-9),9-12, */
/* 12-15,15-18,18-21,)21-24(. */
/* If date is outside the range of the Ap indices file than iap(1)=-5 */
/* -------------------------------------------------------------------- */
    /* Parameter adjustments */
    --iap;

    /* Function Body */
    o__1.oerr = 0;
    o__1.ounit = 13;
    o__1.ofnm = "./../data/IonosphereData/ap.dat";			// made changes by Tuan Nguyen on Jan 11, 2013
    o__1.ofnmlen = strlen(o__1.ofnm);	// o__1.ofnmlen = 6;// made changes by Tuan Nguyen
    o__1.orl = 39;
    o__1.osta = "OLD";
    o__1.oacc = "DIRECT";
    o__1.ofm = "FORMATTED";
    o__1.oblnk = 0;
    f_open(&o__1);
/* -web-sepcial vfor web version */
/*        OPEN(13,FILE='/var/www/omniweb/cgi/vitmo/IRI/ap.dat', */
    i__1 = s_rdfe(&io___777);
    if (i__1 != 0) {
	goto L21;
    }
    i__1 = do_fio(&c__1, (char *)&jy, (ftnlen)sizeof(integer));
    if (i__1 != 0) {
	goto L21;
    }
    i__1 = do_fio(&c__1, (char *)&jmn, (ftnlen)sizeof(integer));
    if (i__1 != 0) {
	goto L21;
    }
    i__1 = do_fio(&c__1, (char *)&jd, (ftnlen)sizeof(integer));
    if (i__1 != 0) {
	goto L21;
    }
    i__1 = do_fio(&c__8, (char *)&iiap[0], (ftnlen)sizeof(integer));
    if (i__1 != 0) {
	goto L21;
    }
    i__1 = do_fio(&c__1, (char *)&f, (ftnlen)sizeof(real));
    if (i__1 != 0) {
	goto L21;
    }
    i__1 = e_rdfe();
    if (i__1 != 0) {
	goto L21;
    }
    iybeg = jy + 1900;
    cl__1.cerr = 0;
    cl__1.cunit = 13;
    cl__1.csta = 0;
    f_clos(&cl__1);
    for (i__ = 1; i__ <= 8; ++i__) {
	iap[i__] = -1;
    }
    if (*iyyyy < iybeg) {
	goto L21;
    }
/* file starts at Jan 1, 1958 */
    o__1.oerr = 0;
    o__1.ounit = 13;
    o__1.ofnm = "./../data/IonosphereData/ap.dat";			// made changes by Tuan Nguyen on Jan 11, 2013
    o__1.ofnmlen = strlen(o__1.ofnm);	//o__1.ofnmlen = 6;	// made changes by Tuan Nguyen
    o__1.orl = 39;
    o__1.osta = "OLD";
    o__1.oacc = "DIRECT";
    o__1.ofm = "FORMATTED";
    o__1.oblnk = 0;
    f_open(&o__1);
/* -web-sepcial vfor web version */
/*      OPEN(13,FILE='/usr/local/etc/httpd/cgi-bin/models/IRI/ap.dat', */
    is = 0;
    if (*iyyyy > iybeg) {
	i__1 = *iyyyy - 1;
	for (i__ = iybeg; i__ <= i__1; ++i__) {
	    nyd = 365;
	    if (i__ / 4 << 2 == i__) {
		nyd = 366;
	    }
	    is += nyd;
	}
    }
    lm[1] = 28;
    if (*iyyyy / 4 << 2 == *iyyyy) {
	lm[1] = 29;
    }
    i__1 = *imn - 1;
    for (i__ = 1; i__ <= i__1; ++i__) {
	is += lm[i__ - 1];
    }
    is += *id;
    ihour = (integer) (*hour / (float)3.) + 1;
    if (ihour > 8) {
	ihour = 8;
    }
    if ((is << 3) + ihour < 13) {
	goto L21;
    }
/* at least 13 indices available */
    io___788.cirec = is;
    i__1 = s_rdfe(&io___788);
    if (i__1 != 0) {
	goto L21;
    }
    i__1 = do_fio(&c__1, (char *)&jy, (ftnlen)sizeof(integer));
    if (i__1 != 0) {
	goto L21;
    }
    i__1 = do_fio(&c__1, (char *)&jmn, (ftnlen)sizeof(integer));
    if (i__1 != 0) {
	goto L21;
    }
    i__1 = do_fio(&c__1, (char *)&jd, (ftnlen)sizeof(integer));
    if (i__1 != 0) {
	goto L21;
    }
    i__1 = do_fio(&c__8, (char *)&iiap[0], (ftnlen)sizeof(integer));
    if (i__1 != 0) {
	goto L21;
    }
    i__1 = do_fio(&c__1, (char *)&f, (ftnlen)sizeof(real));
    if (i__1 != 0) {
	goto L21;
    }
    i__1 = e_rdfe();
    if (i__1 != 0) {
	goto L21;
    }
    for (i9 = 1; i9 <= 8; ++i9) {
	if (iiap[i9 - 1] < -2) {
	    goto L21;
	}
    }
    j1 = 13 - ihour;
    i__1 = ihour;
    for (i__ = 1; i__ <= i__1; ++i__) {
	iap[j1 + i__] = iiap[i__ - 1];
    }
    iss = is - 1;
    io___792.cirec = iss;
    i__1 = s_rdfe(&io___792);
    if (i__1 != 0) {
	goto L21;
    }
    i__1 = do_fio(&c__1, (char *)&jy, (ftnlen)sizeof(integer));
    if (i__1 != 0) {
	goto L21;
    }
    i__1 = do_fio(&c__1, (char *)&jmn, (ftnlen)sizeof(integer));
    if (i__1 != 0) {
	goto L21;
    }
    i__1 = do_fio(&c__1, (char *)&jd, (ftnlen)sizeof(integer));
    if (i__1 != 0) {
	goto L21;
    }
    i__1 = do_fio(&c__8, (char *)&iiap[0], (ftnlen)sizeof(integer));
    if (i__1 != 0) {
	goto L21;
    }
    i__1 = do_fio(&c__1, (char *)&f, (ftnlen)sizeof(real));
    if (i__1 != 0) {
	goto L21;
    }
    i__1 = e_rdfe();
    if (i__1 != 0) {
	goto L21;
    }
    for (i9 = 1; i9 <= 8; ++i9) {
	if (iiap[i9 - 1] < -2) {
	    goto L21;
	}
    }
    if (ihour > 4) {
	i__1 = j1;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    iap[i__] = iiap[8 - j1 + i__ - 1];
	}
    } else {
	j2 = 5 - ihour;
	for (i__ = 1; i__ <= 8; ++i__) {
	    iap[j2 + i__] = iiap[i__ - 1];
	}
	iss = is - 2;
	io___794.cirec = iss;
	i__1 = s_rdfe(&io___794);
	if (i__1 != 0) {
	    goto L21;
	}
	i__1 = do_fio(&c__1, (char *)&jy, (ftnlen)sizeof(integer));
	if (i__1 != 0) {
	    goto L21;
	}
	i__1 = do_fio(&c__1, (char *)&jmn, (ftnlen)sizeof(integer));
	if (i__1 != 0) {
	    goto L21;
	}
	i__1 = do_fio(&c__1, (char *)&jd, (ftnlen)sizeof(integer));
	if (i__1 != 0) {
	    goto L21;
	}
	i__1 = do_fio(&c__8, (char *)&iiap[0], (ftnlen)sizeof(integer));
	if (i__1 != 0) {
	    goto L21;
	}
	i__1 = do_fio(&c__1, (char *)&f, (ftnlen)sizeof(real));
	if (i__1 != 0) {
	    goto L21;
	}
	i__1 = e_rdfe();
	if (i__1 != 0) {
	    goto L21;
	}
	for (i9 = 1; i9 <= 8; ++i9) {
	    if (iiap[i9 - 1] < -2) {
		goto L21;
	    }
	}
	i__1 = j2;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    iap[i__] = iiap[8 - j2 + i__ - 1];
	}
    }
    cl__1.cerr = 0;
    cl__1.cunit = 13;
    cl__1.csta = 0;
    f_clos(&cl__1);
    goto L20;
L21:
    if (iounit_1.konsol > 1) {
	io___795.ciunit = iounit_1.konsol;
	s_wsfe(&io___795);
	e_wsfe();
    }
    iap[1] = -5;
L20:
    return 0;
} /* apf_ */



/* Subroutine */ int apf_only__(integer *iyyyy, integer *imn, integer *id, 
	real *f107d, real *f107m)
{
    /* Initialized data */

    static integer lm[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };

    /* Format strings */
    static char fmt_10[] = "(3i3,8i3,f5.1)";
    static char fmt_100[] = "(1x,\002Date is outside range of F10.7D indices\
 file\002,\002 (F10.7D = F10.7M = F10.7RM12).\002)";

    /* System generated locals */
    integer i__1, i__2;
    olist o__1;
    cllist cl__1;

    /* Builtin functions */
    integer f_open(olist *), s_rdfe(cilist *), do_fio(integer *, char *, 
	    ftnlen), e_rdfe(), f_clos(cllist *), s_wsfe(cilist *), e_wsfe();

    /* Local variables */
    static integer iiap[8], mend;
    static real f;
    static integer i__, iybeg, jd, is, jy, jmn, nyd;
    static real sum;

    /* Fortran I/O blocks */
    static cilist io___797 = { 1, 13, 0, fmt_10, 1 };
    static cilist io___809 = { 1, 13, 0, fmt_10, 0 };
    static cilist io___810 = { 0, 0, 0, fmt_100, 0 };


/* -------------------------------------------------------------------- */
/* Finds daily and monthly F10.7 index, F107D and F107M, for given year */
/* (IYYYY/yyyy), month (IMN/mm), and day (ID/dd) using AP.DAT file on */
/* UNIT=13. Is used for vdrift and foeedi. */
/* If date is outside the range of indices file than F107D=-5 */
/* -------------------------------------------------------------------- */
    o__1.oerr = 0;
    o__1.ounit = 13;
    o__1.ofnm = "./../data/IonosphereData/ap.dat";			// made changes by Tuan Nguyen on Jan 11, 2013
    o__1.ofnmlen = strlen(o__1.ofnm);	//o__1.ofnmlen = 6;	// made changes by Tuan Nguyen
    o__1.orl = 39;
    o__1.osta = "OLD";
    o__1.oacc = "DIRECT";
    o__1.ofm = "FORMATTED";
    o__1.oblnk = 0;
    f_open(&o__1);
/* -web-sepcial vfor web version */
/*        OPEN(13,FILE='/var/www/omniweb/cgi/vitmo/IRI/ap.dat', */
    i__1 = s_rdfe(&io___797);
    if (i__1 != 0) {
	goto L21;
    }
    i__1 = do_fio(&c__1, (char *)&jy, (ftnlen)sizeof(integer));
    if (i__1 != 0) {
	goto L21;
    }
    i__1 = do_fio(&c__1, (char *)&jmn, (ftnlen)sizeof(integer));
    if (i__1 != 0) {
	goto L21;
    }
    i__1 = do_fio(&c__1, (char *)&jd, (ftnlen)sizeof(integer));
    if (i__1 != 0) {
	goto L21;
    }
    i__1 = do_fio(&c__8, (char *)&iiap[0], (ftnlen)sizeof(integer));
    if (i__1 != 0) {
	goto L21;
    }
    i__1 = do_fio(&c__1, (char *)&f, (ftnlen)sizeof(real));
    if (i__1 != 0) {
	goto L21;
    }
    i__1 = e_rdfe();
    if (i__1 != 0) {
	goto L21;
    }
    iybeg = jy + 1900;
    cl__1.cerr = 0;
    cl__1.cunit = 13;
    cl__1.csta = 0;
    f_clos(&cl__1);
    if (*iyyyy < iybeg) {
	goto L21;
    }
/* AP.DAT starts at Jan 1, 1958 */
    o__1.oerr = 0;
    o__1.ounit = 13;
    o__1.ofnm = "./../data/IonosphereData/ap.dat";			// made changes by Tuan Nguyen on Jan 11, 2013
    o__1.ofnmlen = strlen(o__1.ofnm);	//o__1.ofnmlen = 6;	// made changes by Tuan Nguyen
    o__1.orl = 39;
    o__1.osta = "OLD";
    o__1.oacc = "DIRECT";
    o__1.ofm = "FORMATTED";
    o__1.oblnk = 0;
    f_open(&o__1);
/* -web-sepcial vfor web version */
/*      OPEN(13,FILE='/var/www/omniweb/cgi/vitmo/IRI/ap.dat', */
    is = 0;
    i__1 = *iyyyy - 1;
    for (i__ = iybeg; i__ <= i__1; ++i__) {
	nyd = 365;
	if (i__ / 4 << 2 == i__) {
	    nyd = 366;
	}
/* leap year */
	is += nyd;
    }
    lm[1] = 28;
    if (*iyyyy / 4 << 2 == *iyyyy) {
	lm[1] = 29;
    }
/* leap year */
    i__1 = *imn - 1;
    for (i__ = 1; i__ <= i__1; ++i__) {
	is += lm[i__ - 1];
    }
    sum = (float)0.;
    mend = lm[*imn - 1];
    i__1 = mend;
    for (i__ = 1; i__ <= i__1; ++i__) {
	++is;
	io___809.cirec = is;
	i__2 = s_rdfe(&io___809);
	if (i__2 != 0) {
	    goto L21;
	}
	i__2 = do_fio(&c__1, (char *)&jy, (ftnlen)sizeof(integer));
	if (i__2 != 0) {
	    goto L21;
	}
	i__2 = do_fio(&c__1, (char *)&jmn, (ftnlen)sizeof(integer));
	if (i__2 != 0) {
	    goto L21;
	}
	i__2 = do_fio(&c__1, (char *)&jd, (ftnlen)sizeof(integer));
	if (i__2 != 0) {
	    goto L21;
	}
	i__2 = do_fio(&c__8, (char *)&iiap[0], (ftnlen)sizeof(integer));
	if (i__2 != 0) {
	    goto L21;
	}
	i__2 = do_fio(&c__1, (char *)&f, (ftnlen)sizeof(real));
	if (i__2 != 0) {
	    goto L21;
	}
	i__2 = e_rdfe();
	if (i__2 != 0) {
	    goto L21;
	}
	if (f < (float)-4.) {
	    goto L21;
	}
	if (i__ == *id) {
	    *f107d = f;
	}
	sum += f;
    }
    *f107m = sum / mend;
    cl__1.cerr = 0;
    cl__1.cunit = 13;
    cl__1.csta = 0;
    f_clos(&cl__1);
    goto L20;
L21:
    if (iounit_1.konsol > 1) {
	io___810.ciunit = iounit_1.konsol;
	s_wsfe(&io___810);
	e_wsfe();
    }
    *f107d = (float)-111.;
L20:
    return 0;
} /* apf_only__ */



/* ----------------------STORM MODEL -------------------------------- */

/* Subroutine */ int conver_(real *rga, real *rgo, real *rgma)
{
    /* Initialized data */

    static real cormag[1820]	/* was [20][91] */ = { (float)163.68,(float)
	    163.68,(float)163.68,(float)163.68,(float)163.68,(float)163.68,(
	    float)163.68,(float)163.68,(float)163.68,(float)163.68,(float)
	    163.68,(float)163.68,(float)163.68,(float)163.68,(float)163.68,(
	    float)163.68,(float)163.68,(float)163.68,(float)163.68,(float)
	    163.68,(float)162.6,(float)163.12,(float)163.64,(float)164.18,(
	    float)164.54,(float)164.9,(float)165.16,(float)165.66,(float)166.,
	    (float)165.86,(float)165.2,(float)164.38,(float)163.66,(float)
	    162.94,(float)162.42,(float)162.,(float)161.7,(float)161.7,(float)
	    161.8,(float)162.14,(float)161.2,(float)162.18,(float)163.26,(
	    float)164.44,(float)165.62,(float)166.6,(float)167.42,(float)
	    167.8,(float)167.38,(float)166.82,(float)166.,(float)164.66,(
	    float)163.26,(float)162.16,(float)161.18,(float)160.4,(float)
	    159.94,(float)159.8,(float)159.98,(float)160.44,(float)159.8,(
	    float)161.14,(float)162.7,(float)164.5,(float)166.26,(float)167.9,
	    (float)169.18,(float)169.72,(float)169.36,(float)168.24,(float)
	    166.7,(float)164.8,(float)162.9,(float)161.18,(float)159.74,(
	    float)158.6,(float)157.94,(float)157.8,(float)157.98,(float)
	    158.72,(float)158.4,(float)160.1,(float)162.02,(float)164.28,(
	    float)166.64,(float)169.,(float)170.8,(float)171.72,(float)171.06,
	    (float)169.46,(float)167.1,(float)164.64,(float)162.18,(float)
	    160.02,(float)158.2,(float)156.8,(float)156.04,(float)155.8,(
	    float)156.16,(float)157.02,(float)157.,(float)158.96,(float)
	    161.24,(float)163.86,(float)166.72,(float)169.8,(float)172.42,(
	    float)173.72,(float)172.82,(float)170.34,(float)167.3,(float)
	    164.22,(float)161.34,(float)158.74,(float)156.6,(float)155.,(
	    float)154.08,(float)153.9,(float)154.36,(float)155.36,(float)
	    155.5,(float)157.72,(float)160.36,(float)163.32,(float)166.6,(
	    float)170.2,(float)173.7,(float)175.64,(float)174.18,(float)170.8,
	    (float)167.1,(float)163.56,(float)160.24,(float)157.36,(float)
	    154.96,(float)153.1,(float)152.08,(float)151.92,(float)152.46,(
	    float)153.76,(float)154.1,(float)156.52,(float)159.36,(float)
	    162.52,(float)166.24,(float)170.3,(float)174.62,(float)177.48,(
	    float)175.04,(float)170.82,(float)166.6,(float)162.7,(float)
	    159.02,(float)155.88,(float)153.22,(float)151.2,(float)150.08,(
	    float)149.92,(float)150.64,(float)152.2,(float)152.8,(float)
	    155.32,(float)158.28,(float)161.7,(float)165.58,(float)170.,(
	    float)174.84,(float)178.46,(float)175.18,(float)170.38,(float)
	    165.8,(float)161.64,(float)157.8,(float)154.38,(float)151.52,(
	    float)149.3,(float)148.18,(float)148.02,(float)148.92,(float)
	    150.6,(float)151.4,(float)154.08,(float)157.18,(float)160.68,(
	    float)164.78,(float)169.4,(float)174.34,(float)177.44,(float)
	    174.28,(float)169.44,(float)164.7,(float)160.34,(float)156.3,(
	    float)152.78,(float)149.72,(float)147.4,(float)146.18,(float)
	    146.04,(float)147.12,(float)149.04,(float)150.1,(float)152.88,(
	    float)156.,(float)159.58,(float)163.78,(float)168.5,(float)173.28,
	    (float)175.6,(float)172.86,(float)168.14,(float)163.4,(float)
	    158.98,(float)154.88,(float)151.1,(float)147.98,(float)145.5,(
	    float)144.18,(float)144.14,(float)145.4,(float)147.48,(float)
	    148.8,(float)151.68,(float)154.88,(float)158.48,(float)162.68,(
	    float)167.4,(float)171.76,(float)173.6,(float)171.12,(float)
	    166.68,(float)162.,(float)157.48,(float)153.28,(float)149.5,(
	    float)146.18,(float)143.5,(float)142.18,(float)142.24,(float)
	    143.68,(float)145.98,(float)147.5,(float)150.54,(float)153.68,(
	    float)157.28,(float)161.42,(float)166.1,(float)170.1,(float)
	    171.48,(float)169.22,(float)164.98,(float)160.4,(float)155.88,(
	    float)151.68,(float)147.8,(float)144.34,(float)141.6,(float)
	    140.18,(float)140.26,(float)141.98,(float)144.62,(float)146.3,(
	    float)149.34,(float)152.48,(float)155.98,(float)160.08,(float)
	    164.6,(float)168.34,(float)169.38,(float)167.2,(float)163.18,(
	    float)158.6,(float)154.18,(float)149.98,(float)146.02,(float)
	    142.54,(float)139.7,(float)138.18,(float)138.46,(float)140.26,(
	    float)143.16,(float)145.1,(float)148.14,(float)151.18,(float)
	    154.6,(float)158.68,(float)163.1,(float)166.48,(float)167.28,(
	    float)165.18,(float)161.32,(float)156.9,(float)152.48,(float)
	    148.28,(float)144.32,(float)140.74,(float)137.8,(float)136.22,(
	    float)136.48,(float)138.64,(float)141.76,(float)143.9,(float)
	    146.98,(float)149.98,(float)153.3,(float)157.24,(float)161.4,(
	    float)164.52,(float)165.16,(float)162.86,(float)159.42,(float)
	    155.,(float)150.68,(float)146.48,(float)142.52,(float)138.94,(
	    float)135.9,(float)134.22,(float)134.68,(float)137.02,(float)
	    140.4,(float)142.7,(float)145.84,(float)148.76,(float)151.92,(
	    float)155.74,(float)159.7,(float)162.52,(float)162.96,(float)
	    160.98,(float)157.42,(float)153.1,(float)148.84,(float)144.68,(
	    float)140.82,(float)137.2,(float)134.,(float)132.32,(float)132.8,(
	    float)135.42,(float)139.1,(float)141.6,(float)144.74,(float)
	    147.46,(float)150.52,(float)154.2,(float)158.,(float)160.46,(
	    float)160.76,(float)158.86,(float)155.36,(float)151.2,(float)
	    146.94,(float)142.88,(float)139.02,(float)135.4,(float)132.1,(
	    float)130.32,(float)131.,(float)133.8,(float)137.74,(float)140.5,(
	    float)143.58,(float)146.24,(float)149.12,(float)152.6,(float)
	    156.2,(float)158.4,(float)158.66,(float)156.76,(float)153.36,(
	    float)149.3,(float)145.04,(float)141.08,(float)137.3,(float)133.6,
	    (float)130.3,(float)128.42,(float)129.12,(float)132.28,(float)
	    136.44,(float)139.3,(float)142.48,(float)144.94,(float)147.64,(
	    float)150.48,(float)154.3,(float)156.34,(float)156.36,(float)
	    154.56,(float)151.26,(float)147.3,(float)143.14,(float)139.2,(
	    float)135.5,(float)131.9,(float)128.4,(float)126.52,(float)127.32,
	    (float)130.76,(float)135.18,(float)138.2,(float)141.28,(float)
	    143.72,(float)146.24,(float)149.26,(float)152.4,(float)154.24,(
	    float)154.16,(float)152.36,(float)149.16,(float)145.3,(float)
	    141.24,(float)137.3,(float)133.7,(float)130.1,(float)126.6,(float)
	    124.62,(float)125.54,(float)129.16,(float)133.92,(float)137.1,(
	    float)140.18,(float)142.42,(float)144.66,(float)147.62,(float)
	    150.5,(float)152.18,(float)151.96,(float)150.16,(float)147.1,(
	    float)143.3,(float)139.24,(float)135.5,(float)131.9,(float)128.36,
	    (float)124.8,(float)122.72,(float)123.74,(float)127.64,(float)
	    132.62,(float)135.9,(float)139.02,(float)141.12,(float)143.18,(
	    float)145.92,(float)148.6,(float)149.98,(float)149.76,(float)
	    148.04,(float)145.,(float)141.2,(float)137.3,(float)133.6,(float)
	    130.1,(float)126.6,(float)123.,(float)120.86,(float)121.96,(float)
	    126.12,(float)131.36,(float)134.8,(float)137.88,(float)139.8,(
	    float)141.68,(float)144.08,(float)146.6,(float)147.88,(float)
	    147.56,(float)145.84,(float)142.9,(float)139.2,(float)135.3,(
	    float)131.7,(float)128.28,(float)124.86,(float)121.3,(float)
	    118.96,(float)120.18,(float)124.7,(float)130.16,(float)133.6,(
	    float)136.72,(float)138.48,(float)140.1,(float)142.38,(float)
	    144.6,(float)145.72,(float)145.34,(float)143.64,(float)140.8,(
	    float)137.1,(float)133.3,(float)129.72,(float)126.48,(float)123.1,
	    (float)119.5,(float)117.16,(float)118.48,(float)123.18,(float)
	    128.86,(float)132.4,(float)135.42,(float)137.08,(float)138.5,(
	    float)140.54,(float)142.6,(float)143.52,(float)143.06,(float)
	    141.44,(float)138.7,(float)135.1,(float)131.3,(float)127.82,(
	    float)124.58,(float)121.4,(float)117.7,(float)115.26,(float)116.7,
	    (float)121.66,(float)127.6,(float)131.2,(float)134.22,(float)
	    135.66,(float)136.82,(float)138.7,(float)140.6,(float)141.36,(
	    float)140.86,(float)139.24,(float)136.5,(float)133.,(float)129.3,(
	    float)125.92,(float)122.78,(float)119.6,(float)116.,(float)113.4,(
	    float)114.92,(float)120.16,(float)126.3,(float)130.,(float)132.92,
	    (float)134.24,(float)135.14,(float)136.8,(float)138.6,(float)
	    139.16,(float)138.64,(float)137.12,(float)134.4,(float)130.9,(
	    float)127.2,(float)123.92,(float)120.96,(float)117.9,(float)114.2,
	    (float)111.56,(float)113.12,(float)118.64,(float)124.9,(float)
	    128.7,(float)131.56,(float)132.74,(float)133.44,(float)134.9,(
	    float)136.5,(float)137.,(float)136.36,(float)134.82,(float)132.3,(
	    float)128.7,(float)125.16,(float)121.94,(float)119.06,(float)
	    116.1,(float)112.5,(float)109.7,(float)111.42,(float)117.14,(
	    float)123.6,(float)127.3,(float)130.16,(float)131.22,(float)
	    131.66,(float)133.,(float)134.5,(float)134.8,(float)134.14,(float)
	    132.62,(float)130.14,(float)126.6,(float)123.06,(float)119.94,(
	    float)117.16,(float)114.3,(float)110.7,(float)107.8,(float)109.64,
	    (float)115.62,(float)122.24,(float)125.9,(float)128.76,(float)
	    129.62,(float)129.96,(float)131.06,(float)132.4,(float)132.6,(
	    float)131.86,(float)130.42,(float)128.,(float)124.5,(float)120.96,
	    (float)117.96,(float)115.26,(float)112.54,(float)108.9,(float)
	    105.94,(float)107.86,(float)114.02,(float)120.84,(float)124.05,(
	    float)126.79,(float)127.55,(float)127.83,(float)128.9,(float)
	    130.21,(float)130.41,(float)129.71,(float)128.33,(float)125.96,(
	    float)122.49,(float)118.96,(float)115.97,(float)113.26,(float)
	    110.52,(float)106.89,(float)104.01,(float)106.,(float)112.21,(
	    float)119.06,(float)122.19,(float)124.82,(float)125.48,(float)
	    125.69,(float)126.73,(float)128.03,(float)128.22,(float)127.55,(
	    float)126.23,(float)123.92,(float)120.47,(float)116.97,(float)
	    113.97,(float)111.26,(float)108.5,(float)104.89,(float)102.08,(
	    float)104.14,(float)110.41,(float)117.29,(float)120.34,(float)
	    122.85,(float)123.4,(float)123.56,(float)124.57,(float)125.84,(
	    float)126.03,(float)125.4,(float)124.14,(float)121.88,(float)
	    118.46,(float)114.97,(float)111.98,(float)109.26,(float)106.48,(
	    float)102.88,(float)100.15,(float)102.28,(float)108.6,(float)
	    115.51,(float)118.49,(float)120.88,(float)121.33,(float)121.42,(
	    float)122.4,(float)123.65,(float)123.84,(float)123.24,(float)
	    122.04,(float)119.83,(float)116.45,(float)112.97,(float)109.98,(
	    float)107.26,(float)104.46,(float)100.87,(float)98.22,(float)
	    100.42,(float)106.79,(float)113.74,(float)116.63,(float)118.91,(
	    float)119.26,(float)119.29,(float)120.24,(float)121.47,(float)
	    121.65,(float)121.09,(float)119.95,(float)117.79,(float)114.43,(
	    float)110.98,(float)107.99,(float)105.26,(float)102.44,(float)
	    98.87,(float)96.29,(float)98.56,(float)104.98,(float)111.96,(
	    float)114.78,(float)116.94,(float)117.19,(float)117.15,(float)
	    118.07,(float)119.28,(float)119.46,(float)118.93,(float)117.86,(
	    float)115.75,(float)112.42,(float)108.98,(float)106.,(float)
	    103.26,(float)100.42,(float)96.86,(float)94.36,(float)96.7,(float)
	    103.18,(float)110.19,(float)112.93,(float)114.97,(float)115.12,(
	    float)115.02,(float)115.91,(float)117.09,(float)117.27,(float)
	    116.78,(float)115.76,(float)113.71,(float)110.41,(float)106.98,(
	    float)104.,(float)101.26,(float)98.4,(float)94.85,(float)92.43,(
	    float)94.84,(float)101.37,(float)108.41,(float)111.07,(float)113.,
	    (float)113.04,(float)112.88,(float)113.74,(float)114.91,(float)
	    115.08,(float)114.62,(float)113.67,(float)111.67,(float)108.39,(
	    float)104.99,(float)102.01,(float)99.26,(float)96.38,(float)92.85,
	    (float)90.51,(float)92.97,(float)99.56,(float)106.64,(float)
	    109.22,(float)111.03,(float)110.97,(float)110.75,(float)111.58,(
	    float)112.72,(float)112.89,(float)112.47,(float)111.57,(float)
	    109.63,(float)106.38,(float)102.99,(float)100.01,(float)97.26,(
	    float)94.36,(float)90.84,(float)88.58,(float)91.11,(float)97.75,(
	    float)104.86,(float)107.37,(float)109.06,(float)108.9,(float)
	    108.61,(float)109.41,(float)110.53,(float)110.7,(float)110.31,(
	    float)109.48,(float)107.59,(float)104.37,(float)100.99,(float)
	    98.02,(float)95.26,(float)92.34,(float)88.83,(float)86.65,(float)
	    89.25,(float)95.95,(float)103.09,(float)105.51,(float)107.09,(
	    float)106.83,(float)106.48,(float)107.25,(float)108.35,(float)
	    108.51,(float)108.16,(float)107.39,(float)105.55,(float)102.35,(
	    float)99.,(float)96.03,(float)93.26,(float)90.32,(float)86.83,(
	    float)84.72,(float)87.39,(float)94.14,(float)101.31,(float)103.66,
	    (float)105.12,(float)104.76,(float)104.34,(float)105.08,(float)
	    106.16,(float)106.32,(float)106.,(float)105.29,(float)103.5,(
	    float)100.34,(float)97.,(float)94.03,(float)91.26,(float)88.3,(
	    float)84.82,(float)82.79,(float)85.53,(float)92.33,(float)99.54,(
	    float)101.81,(float)103.15,(float)102.68,(float)102.21,(float)
	    102.92,(float)103.97,(float)104.13,(float)103.85,(float)103.2,(
	    float)101.46,(float)98.33,(float)95.,(float)92.04,(float)89.26,(
	    float)86.28,(float)82.81,(float)80.86,(float)83.67,(float)90.52,(
	    float)97.76,(float)99.95,(float)101.18,(float)100.61,(float)
	    100.07,(float)100.75,(float)101.79,(float)101.94,(float)101.69,(
	    float)101.1,(float)99.42,(float)96.31,(float)93.01,(float)90.04,(
	    float)87.26,(float)84.26,(float)80.81,(float)78.93,(float)81.81,(
	    float)88.72,(float)95.99,(float)98.1,(float)99.21,(float)98.54,(
	    float)97.94,(float)98.59,(float)99.6,(float)99.75,(float)99.54,(
	    float)99.01,(float)97.38,(float)94.3,(float)91.01,(float)88.05,(
	    float)85.26,(float)82.24,(float)78.8,(float)77.,(float)79.95,(
	    float)86.91,(float)94.21,(float)96.25,(float)97.24,(float)96.47,(
	    float)95.81,(float)96.43,(float)97.41,(float)97.56,(float)97.39,(
	    float)96.92,(float)95.34,(float)92.29,(float)89.01,(float)86.06,(
	    float)83.26,(float)80.22,(float)76.79,(float)75.07,(float)78.09,(
	    float)85.1,(float)92.43,(float)94.39,(float)95.27,(float)94.4,(
	    float)93.67,(float)94.26,(float)95.23,(float)95.37,(float)95.23,(
	    float)94.82,(float)93.3,(float)90.27,(float)87.02,(float)84.06,(
	    float)81.26,(float)78.2,(float)74.79,(float)73.14,(float)76.23,(
	    float)83.3,(float)90.66,(float)92.54,(float)93.3,(float)92.32,(
	    float)91.54,(float)92.1,(float)93.04,(float)93.18,(float)93.08,(
	    float)92.73,(float)91.26,(float)88.26,(float)85.02,(float)82.07,(
	    float)79.26,(float)76.18,(float)72.78,(float)71.21,(float)74.37,(
	    float)81.49,(float)88.88,(float)90.69,(float)91.33,(float)90.25,(
	    float)89.4,(float)89.93,(float)90.85,(float)90.99,(float)90.92,(
	    float)90.63,(float)89.21,(float)86.25,(float)83.02,(float)80.07,(
	    float)77.26,(float)74.16,(float)70.77,(float)69.28,(float)72.51,(
	    float)79.68,(float)87.11,(float)88.83,(float)89.36,(float)88.18,(
	    float)87.27,(float)87.77,(float)88.67,(float)88.8,(float)88.77,(
	    float)88.54,(float)87.17,(float)84.23,(float)81.03,(float)78.08,(
	    float)75.26,(float)72.14,(float)68.77,(float)67.35,(float)70.65,(
	    float)77.87,(float)85.33,(float)86.98,(float)87.39,(float)86.11,(
	    float)85.13,(float)85.6,(float)86.48,(float)86.61,(float)86.61,(
	    float)86.45,(float)85.13,(float)82.22,(float)79.03,(float)76.09,(
	    float)73.26,(float)70.12,(float)66.76,(float)65.42,(float)68.79,(
	    float)76.07,(float)83.56,(float)85.13,(float)85.42,(float)84.04,(
	    float)83.,(float)83.44,(float)84.29,(float)84.42,(float)84.46,(
	    float)84.35,(float)83.09,(float)80.21,(float)77.03,(float)74.09,(
	    float)71.26,(float)68.1,(float)64.75,(float)63.49,(float)66.93,(
	    float)74.26,(float)81.78,(float)83.27,(float)83.45,(float)81.96,(
	    float)80.86,(float)81.27,(float)82.11,(float)82.23,(float)82.3,(
	    float)82.26,(float)81.05,(float)78.19,(float)75.04,(float)72.1,(
	    float)69.26,(float)66.08,(float)62.75,(float)61.57,(float)65.06,(
	    float)72.45,(float)80.01,(float)81.42,(float)81.48,(float)79.89,(
	    float)78.73,(float)79.11,(float)79.92,(float)80.04,(float)80.15,(
	    float)80.16,(float)79.01,(float)76.18,(float)73.04,(float)70.1,(
	    float)67.26,(float)64.06,(float)60.74,(float)59.64,(float)63.2,(
	    float)70.64,(float)78.23,(float)79.57,(float)79.51,(float)77.82,(
	    float)76.59,(float)76.94,(float)77.73,(float)77.85,(float)77.99,(
	    float)78.07,(float)76.97,(float)74.17,(float)71.04,(float)68.11,(
	    float)65.26,(float)62.04,(float)58.73,(float)57.71,(float)61.34,(
	    float)68.84,(float)76.46,(float)77.71,(float)77.54,(float)75.75,(
	    float)74.46,(float)74.78,(float)75.55,(float)75.66,(float)75.84,(
	    float)75.98,(float)74.93,(float)72.15,(float)69.05,(float)66.12,(
	    float)63.26,(float)60.02,(float)56.73,(float)55.78,(float)59.48,(
	    float)67.03,(float)74.68,(float)75.86,(float)75.57,(float)73.68,(
	    float)72.32,(float)72.61,(float)73.36,(float)73.47,(float)73.68,(
	    float)73.88,(float)72.88,(float)70.14,(float)67.05,(float)64.12,(
	    float)61.26,(float)58.,(float)54.72,(float)53.85,(float)57.62,(
	    float)65.22,(float)72.91,(float)74.01,(float)73.6,(float)71.6,(
	    float)70.19,(float)70.45,(float)71.17,(float)71.28,(float)71.53,(
	    float)71.79,(float)70.84,(float)68.13,(float)65.05,(float)62.13,(
	    float)59.26,(float)55.98,(float)52.71,(float)51.92,(float)55.76,(
	    float)63.41,(float)71.13,(float)72.15,(float)71.63,(float)69.53,(
	    float)68.05,(float)68.28,(float)68.99,(float)69.09,(float)69.37,(
	    float)69.69,(float)68.8,(float)66.11,(float)63.06,(float)60.13,(
	    float)57.26,(float)53.96,(float)50.71,(float)49.99,(float)53.9,(
	    float)61.61,(float)69.36,(float)70.3,(float)69.66,(float)67.46,(
	    float)65.92,(float)66.12,(float)66.8,(float)66.9,(float)67.22,(
	    float)67.6,(float)66.76,(float)64.1,(float)61.06,(float)58.14,(
	    float)55.26,(float)51.94,(float)48.7,(float)48.06,(float)52.04,(
	    float)59.8,(float)67.58,(float)67.7,(float)67.06,(float)65.08,(
	    float)63.72,(float)63.98,(float)64.6,(float)64.8,(float)65.12,(
	    float)65.6,(float)64.86,(float)62.4,(float)59.26,(float)56.24,(
	    float)53.18,(float)49.84,(float)46.6,(float)46.12,(float)50.12,(
	    float)57.52,(float)64.8,(float)64.9,(float)64.42,(float)62.7,(
	    float)61.62,(float)61.78,(float)62.4,(float)62.6,(float)63.04,(
	    float)63.58,(float)63.,(float)60.6,(float)57.46,(float)54.42,(
	    float)51.18,(float)47.7,(float)44.6,(float)44.22,(float)48.02,(
	    float)55.06,(float)61.92,(float)62.1,(float)61.72,(float)60.32,(
	    float)59.5,(float)59.68,(float)60.2,(float)60.46,(float)60.94,(
	    float)61.58,(float)61.,(float)58.7,(float)55.66,(float)52.52,(
	    float)49.18,(float)45.6,(float)42.5,(float)42.22,(float)46.,(
	    float)52.6,(float)58.98,(float)59.2,(float)59.18,(float)58.12,(
	    float)57.32,(float)57.48,(float)58.,(float)58.3,(float)58.84,(
	    float)59.48,(float)59.04,(float)56.9,(float)53.86,(float)50.62,(
	    float)47.1,(float)43.5,(float)40.5,(float)40.28,(float)43.98,(
	    float)50.22,(float)56.18,(float)56.4,(float)56.64,(float)55.84,(
	    float)55.2,(float)55.38,(float)55.8,(float)56.16,(float)56.84,(
	    float)57.48,(float)57.04,(float)55.1,(float)52.06,(float)48.7,(
	    float)45.1,(float)41.4,(float)38.4,(float)38.28,(float)41.88,(
	    float)47.94,(float)53.44,(float)53.7,(float)54.14,(float)53.56,(
	    float)53.1,(float)53.24,(float)53.7,(float)54.06,(float)54.74,(
	    float)55.38,(float)55.14,(float)53.2,(float)50.26,(float)46.8,(
	    float)43.1,(float)39.34,(float)36.4,(float)36.38,(float)39.96,(
	    float)45.56,(float)50.84,(float)51.1,(float)51.7,(float)51.36,(
	    float)51.,(float)51.14,(float)51.5,(float)51.96,(float)52.64,(
	    float)53.38,(float)53.08,(float)51.3,(float)48.36,(float)44.9,(
	    float)41.02,(float)37.24,(float)34.4,(float)34.38,(float)37.86,(
	    float)43.28,(float)48.2,(float)48.5,(float)49.26,(float)49.18,(
	    float)48.9,(float)49.04,(float)49.4,(float)49.86,(float)50.64,(
	    float)51.28,(float)51.08,(float)49.4,(float)46.46,(float)42.98,(
	    float)39.02,(float)35.14,(float)32.4,(float)32.48,(float)35.72,(
	    float)41.,(float)45.7,(float)46.,(float)46.96,(float)46.98,(float)
	    46.8,(float)46.94,(float)47.3,(float)47.76,(float)48.54,(float)
	    49.28,(float)49.08,(float)47.4,(float)44.56,(float)41.08,(float)
	    37.02,(float)33.14,(float)30.4,(float)30.58,(float)33.84,(float)
	    38.72,(float)43.2,(float)43.5,(float)44.62,(float)44.8,(float)
	    44.8,(float)44.94,(float)45.2,(float)45.76,(float)46.54,(float)
	    47.18,(float)46.98,(float)45.5,(float)42.66,(float)39.08,(float)
	    35.02,(float)31.14,(float)28.4,(float)28.58,(float)31.82,(float)
	    36.52,(float)40.8,(float)41.2,(float)42.32,(float)42.54,(float)
	    42.7,(float)42.84,(float)43.2,(float)43.66,(float)44.44,(float)
	    45.08,(float)44.98,(float)43.5,(float)40.76,(float)37.08,(float)
	    33.04,(float)29.04,(float)26.4,(float)26.68,(float)29.82,(float)
	    34.34,(float)38.4,(float)38.8,(float)40.12,(float)40.6,(float)
	    40.7,(float)40.84,(float)41.1,(float)41.62,(float)42.34,(float)
	    42.98,(float)42.88,(float)41.5,(float)38.76,(float)35.18,(float)
	    31.04,(float)27.14,(float)24.5,(float)24.78,(float)27.7,(float)
	    32.14,(float)36.06,(float)36.5,(float)37.88,(float)38.5,(float)
	    38.68,(float)38.84,(float)39.1,(float)39.56,(float)40.34,(float)
	    40.88,(float)40.82,(float)39.4,(float)36.76,(float)33.18,(float)
	    29.12,(float)25.14,(float)22.5,(float)22.88,(float)25.9,(float)
	    29.96,(float)33.86,(float)34.3,(float)35.68,(float)36.42,(float)
	    36.68,(float)36.84,(float)37.1,(float)37.56,(float)38.24,(float)
	    38.88,(float)38.72,(float)37.4,(float)34.76,(float)31.18,(float)
	    27.12,(float)23.14,(float)20.6,(float)20.98,(float)23.9,(float)
	    27.88,(float)31.66,(float)32.1,(float)33.58,(float)34.32,(float)
	    34.68,(float)34.84,(float)35.1,(float)35.56,(float)36.24,(float)
	    36.78,(float)36.62,(float)35.3,(float)32.72,(float)29.18,(float)
	    25.14,(float)21.24,(float)18.7,(float)19.08,(float)21.9,(float)
	    25.88,(float)29.42,(float)29.9,(float)31.48,(float)32.32,(float)
	    32.68,(float)32.84,(float)33.1,(float)33.56,(float)34.22,(float)
	    34.68,(float)34.42,(float)33.2,(float)30.72,(float)27.28,(float)
	    23.22,(float)19.34,(float)16.8,(float)17.24,(float)20.,(float)
	    23.78,(float)27.32,(float)27.7,(float)29.38,(float)30.24,(float)
	    30.68,(float)30.94,(float)31.2,(float)31.66,(float)32.22,(float)
	    32.58,(float)32.32,(float)31.1,(float)28.62,(float)25.28,(float)
	    21.32,(float)17.48,(float)15.,(float)15.38,(float)18.18,(float)
	    21.8,(float)25.22,(float)25.7,(float)27.28,(float)28.24,(float)
	    28.78,(float)29.04,(float)29.3,(float)29.66,(float)30.22,(float)
	    30.5,(float)30.22,(float)29.,(float)26.62,(float)23.3,(float)
	    19.42,(float)15.64,(float)13.1,(float)13.54,(float)16.28,(float)
	    19.8,(float)23.12,(float)23.6,(float)25.24,(float)26.24,(float)
	    26.78,(float)27.14,(float)27.4,(float)27.76,(float)28.22,(float)
	    28.4,(float)28.12,(float)26.8,(float)24.52,(float)21.3,(float)
	    17.52,(float)13.78,(float)11.3,(float)11.74,(float)14.48,(float)
	    17.9,(float)21.12,(float)21.6,(float)23.24,(float)24.34,(float)
	    24.88,(float)25.24,(float)25.5,(float)25.86,(float)26.22,(float)
	    26.4,(float)25.98,(float)24.7,(float)22.48,(float)19.4,(float)
	    15.72,(float)12.04,(float)9.5,(float)9.94,(float)12.58,(float)
	    16.02,(float)19.12,(float)19.6,(float)21.24,(float)22.34,(float)
	    22.98,(float)23.34,(float)23.7,(float)24.,(float)24.3,(float)24.4,
	    (float)23.88,(float)22.6,(float)20.48,(float)17.52,(float)14.,(
	    float)10.34,(float)7.8,(float)8.18,(float)10.88,(float)14.22,(
	    float)17.18,(float)17.6,(float)19.34,(float)20.44,(float)21.16,(
	    float)21.54,(float)21.9,(float)22.16,(float)22.4,(float)22.32,(
	    float)21.78,(float)20.6,(float)18.48,(float)15.62,(float)12.2,(
	    float)8.68,(float)6.,(float)6.44,(float)9.18,(float)12.42,(float)
	    15.28,(float)15.8,(float)17.44,(float)18.54,(float)19.26,(float)
	    19.74,(float)20.1,(float)20.3,(float)20.5,(float)20.32,(float)
	    19.72,(float)18.5,(float)16.54,(float)13.84,(float)10.68,(float)
	    7.14,(float)4.4,(float)4.74,(float)7.58,(float)10.74,(float)13.48,
	    (float)14.,(float)15.54,(float)16.74,(float)17.46,(float)17.94,(
	    float)18.3,(float)18.5,(float)18.58,(float)18.32,(float)17.72,(
	    float)16.5,(float)14.64,(float)12.24,(float)9.18,(float)5.84,(
	    float)2.9,(float)3.3,(float)6.16,(float)9.14,(float)11.84,(float)
	    12.3,(float)13.78,(float)14.94,(float)15.66,(float)16.24,(float)
	    16.5,(float)16.7,(float)16.7,(float)16.42,(float)5.78,(float)14.6,
	    (float)12.9,(float)10.66,(float)7.86,(float)4.88,(float)1.6,(
	    float)1.72,(float)4.96,(float)7.84,(float)10.24,(float)10.7,(
	    float)12.14,(float)13.24,(float)13.96,(float)14.44,(float)14.8,(
	    float)14.9,(float)14.88,(float)14.52,(float)13.92,(float)12.8,(
	    float)11.3,(float)9.28,(float)6.94,(float)4.32,(float)1.8,(float)
	    1.94,(float)4.34,(float)6.78,(float)8.94,(float)9.4,(float)10.58,(
	    float)11.64,(float)12.36,(float)12.74,(float)13.1,(float)13.2,(
	    float)13.08,(float)12.72,(float)12.12,(float)11.1,(float)9.86,(
	    float)8.3,(float)6.5,(float)4.6,(float)3.1,(float)3.16,(float)4.5,
	    (float)6.2,(float)7.9,(float)8.4,(float)9.42,(float)10.14,(float)
	    10.76,(float)11.14,(float)11.4,(float)11.4,(float)11.38,(float)
	    11.02,(float)10.46,(float)9.7,(float)8.72,(float)7.64,(float)6.46,
	    (float)5.42,(float)4.6,(float)4.7,(float)5.34,(float)6.24,(float)
	    7.36,(float)7.9,(float)8.46,(float)8.92,(float)9.28,(float)9.54,(
	    float)9.7,(float)9.7,(float)9.68,(float)9.42,(float)9.06,(float)
	    8.6,(float)8.08,(float)7.56,(float)7.02,(float)6.56,(float)6.3,(
	    float)6.3,(float)6.52,(float)6.96,(float)7.38,(float)8.15,(float)
	    8.15,(float)8.15,(float)8.15,(float)8.15,(float)8.15,(float)8.15,(
	    float)8.15,(float)8.15,(float)8.15,(float)8.15,(float)8.15,(float)
	    8.15,(float)8.15,(float)8.15,(float)8.15,(float)8.15,(float)8.15,(
	    float)8.15,(float)8.15 };

    static real gmla, rlan, x, y;
    static integer la1, la2;
    static real gm1, gm2, gm3, gm4;
    static integer lo1, lo2;
    static real rla, rlo;

/*     This subroutine converts a geographic latitude and longitude */
/*     location to a corrected geomagnetic latitude. */

/*     INPUT: */
/*       geographic latitude   -90. to +90. */
/*       geographic longitude  0. to 360. positive east from Greenwich. */

/*     OUTPUT: */
/*       corrected geomagnetic latitude	-90. to +90. */
/*     Data Input */
    rlan = *rga;
    rlo = *rgo;
/*     From "normal" geographic latitude */
/*     to angle from South Pole. */
    rla = rlan + 90;
    if (rlo == (float)360.) {
	rlo = (float)0.;
    }
/*     PROXIMITY */
/*     coefficients of the latitudinal points */
    la1 = (integer) (rla / 2) + 1;
    la2 = la1 + 1;
    if (la2 > 91) {
	la2 = 91;
    }
/*     coefficients of the longitudinal points */
    lo1 = (integer) (rlo / 18) + 1;
/* orr      LO2 = LO1 + 1 */
    lo2 = lo1 % 20 + 1;
/*     Four points of Geomagnetic Coordinates */
    gm1 = cormag[lo1 + la1 * 20 - 21];
    gm2 = cormag[lo1 + la2 * 20 - 21];
    gm3 = cormag[lo2 + la1 * 20 - 21];
    gm4 = cormag[lo2 + la2 * 20 - 21];
/*     latitudinal points */
/*      X1 = ABS(rla - (INT(rla))) */
/*      X2 = 2. - X1 */
    x = rla / (float)2. - (integer) (rla / (float)2.);
/*     longitudinal points */
/*      Y1 = ABS(rlo - (INT(rlo))) */
/*      Y2 = 18. - Y1 */
    y = rlo / (float)18. - (integer) (rlo / (float)18.);
/*     X AND Y VALUES */
/*      x = X1 / (X1 + X2) */
/*      y = Y1 / (Y1 + Y2) */
/*     INTERPOLATION */
    gmla = gm1 * (1 - x) * (1 - y) + gm2 * (1 - y) * x + gm3 * y * (1 - x) + 
	    gm4 * x * y;
/*     OUTPUT OF THE PROGRAM */
/*     From corrected geomagnetic latitude from North Pole */
/*     to "normal"  geomagnetic latitude. */
    *rgma = (float)90. - gmla;
    return 0;
} /* conver_ */



/* Subroutine */ int storm_(integer *ap, real *rga, real *rgo, integer *coor, 
	real *rgma, integer *ut, integer *doy, real *cf)
{
    /* Initialized data */

    static real c4[20] = { (float)0.,(float)0.,(float)0.,(float)0.,(float)0.,(
	    float)0.,(float)0.,(float)0.,(float)0.,(float)0.,(float)0.,(float)
	    0.,(float)0.,(float)0.,(float)0.,(float)0.,(float)0.,(float)0.,(
	    float)0.,(float)0. };
    static real c3[20] = { (float)0.,(float)0.,(float)0.,(float)0.,(float)0.,(
	    float)-9.44e-12,(float)0.,(float)3.04e-12,(float)0.,(float)
	    9.32e-12,(float)-1.07e-11,(float)0.,(float)0.,(float)0.,(float)
	    1.09e-11,(float)0.,(float)0.,(float)0.,(float)0.,(float)-1.01e-11 
	    };
    static real c2[20] = { (float)1.16e-8,(float)0.,(float)0.,(float)-1.46e-8,
	    (float)0.,(float)9.86e-8,(float)2.25e-8,(float)-1.67e-8,(float)
	    -1.62e-8,(float)-9.42e-8,(float)1.17e-7,(float)4.32e-8,(float)
	    3.97e-8,(float)3.13e-8,(float)-8.04e-8,(float)3.91e-8,(float)
	    2.58e-8,(float)3.45e-8,(float)4.76e-8,(float)1.13e-7 };
    static real c1[20] = { (float)-9.17e-5,(float)-1.37e-5,(float)0.,(float)
	    7.14e-5,(float)0.,(float)-3.21e-4,(float)-1.66e-4,(float)-4.1e-5,(
	    float)1.36e-4,(float)2.29e-4,(float)-3.89e-4,(float)-3.08e-4,(
	    float)-2.81e-4,(float)-1.9e-4,(float)4.76e-5,(float)-2.8e-4,(
	    float)-2.07e-4,(float)-2.91e-4,(float)-3.3e-4,(float)-4.04e-4 };
    static real c0[20] = { (float)1.0136,(float)1.0478,(float)1.,(float)
	    1.0258,(float)1.,(float)1.077,(float)1.0543,(float)1.0103,(float)
	    .99927,(float).96876,(float)1.0971,(float)1.0971,(float)1.0777,(
	    float)1.1134,(float)1.0237,(float)1.0703,(float)1.0248,(float)
	    1.0945,(float)1.1622,(float)1.1393 };
    static real fap[36] = { (float)0.,(float)0.,(float).037037037,(float)
	    .074074074,(float).111111111,(float).148148148,(float).185185185,(
	    float).222222222,(float).259259259,(float).296296296,(float)
	    .333333333,(float).37037037,(float).407407407,(float).444444444,(
	    float).481481481,(float).518518519,(float).555555556,(float)
	    .592592593,(float).62962963,(float).666666667,(float).703703704,(
	    float).740740741,(float).777777778,(float).814814815,(float)
	    .851851852,(float).888888889,(float).925925926,(float).962962963,(
	    float)1.,(float).66666667,(float).33333334,(float)0.,(float)
	    .333333,(float).666666,(float)1.,(float).7 };
    static integer code[48]	/* was [8][6] */ = { 3,4,5,4,3,2,1,2,3,2,1,2,
	    3,4,5,4,8,7,6,7,8,9,10,9,13,12,11,12,13,14,15,14,18,17,16,17,18,
	    19,20,19,18,17,16,17,18,19,20,19 };

    /* System generated locals */
    real r__1, r__2, r__3;

    /* Builtin functions */
    integer s_wsle(cilist *), do_lio(integer *, integer *, char *, ftnlen), 
	    e_wsle();

    /* Local variables */
    static real facl, facs, rapf;
    static integer i__, j, k, dayno, l1, l2, n1, n2, n3, n4, s1, s2;
    static real rl, rs, cf1, cf2;
    extern /* Subroutine */ int conver_(real *, real *, real *);
    static real cf3, cf4;
    static integer ape[39];
    static real rap, cf300;

    /* Fortran I/O blocks */
    static cilist io___833 = { 0, 6, 0, 0, 0 };
    static cilist io___834 = { 0, 6, 0, 0, 0 };
    static cilist io___835 = { 0, 6, 0, 0, 0 };
    static cilist io___836 = { 0, 6, 0, 0, 0 };
    static cilist io___840 = { 0, 6, 0, 0, 0 };
    static cilist io___841 = { 0, 6, 0, 0, 0 };
    static cilist io___842 = { 0, 6, 0, 0, 0 };
    static cilist io___843 = { 0, 6, 0, 0, 0 };
    static cilist io___846 = { 0, 6, 0, 0, 0 };
    static cilist io___847 = { 0, 6, 0, 0, 0 };
    static cilist io___848 = { 0, 6, 0, 0, 0 };
    static cilist io___849 = { 0, 6, 0, 0, 0 };
    static cilist io___850 = { 0, 6, 0, 0, 0 };
    static cilist io___851 = { 0, 6, 0, 0, 0 };
    static cilist io___852 = { 0, 6, 0, 0, 0 };
    static cilist io___853 = { 0, 6, 0, 0, 0 };
    static cilist io___854 = { 0, 6, 0, 0, 0 };
    static cilist io___855 = { 0, 6, 0, 0, 0 };


/* ---------------------------------------------------------------------- */
/*      Fortran code to obtain the foF2 storm-time correction factor at */
/*      a given location and time, using the current and the 12 previous */
/*      ap values as input. */

/*      ap ---> (13 elements integer array). Array with the preceeding */
/*              13 value of the 3-hourly ap index. The 13th value */
/*              in the array will contain the ap at the UT of interest, */
/*              the 12th value will contain the 1st three hourly interval */
/*              preceeding the time of interest, and so on to the 1st */
/*              ap value at the earliest time. */
/*     coor --> (integer). If coor = 2, rga should contain the */
/*                         geomagnetic latitude. */
/*                         If coor = 1, rga should contain the */
/*                         geographic latitude. */
/*     rga ---> (real, -90 to 90) geographic or geomagnetic latitude. */
/*     rgo ---> (real, 0 to 360, positive east from Greenwich.) */
/*                           geographic longitude, only used if coor=1. */
/*     ut  ---> (integer, hours 00 to 23) Universal Time of interest. */
/*     doy ---> (integer, 1 to 366)Day of the year. */
/*     cf  ---> (real) The output; the storm-time correction factor used */
/*              to scale foF2, foF2 * cf. */

/*     This model and computer code was developed by E. Araujo-Pradere, */
/*     T. Fuller-Rowell and M. Condrescu, SEC, NOAA, Boulder, USA */
/*     Ref: */
/*     T. Fuller-Rowell, E. Araujo-Pradere, and M. Condrescu, An */
/*       Empirical Ionospheric Storm-Time Ionospheric Correction Model, */
/*       Adv. Space Res. 8, 8, 15-24, 2000. */
/* ---------------------------------------------------------------------- */
/*     DIMENSIONS AND COEFFICIENTS VALUES */
    /* Parameter adjustments */
    --ap;

    /* Function Body */
/*      CALLING THE PROGRAM TO CONVERT TO GEOMAGNETIC COORDINATES */
    if (*coor == 1) {
	conver_(rga, rgo, rgma);
    } else if (*coor == 2) {
	*rgma = *rga;
    } else {
	s_wsle(&io___833);
	do_lio(&c__9, &c__1, " ", (ftnlen)1);
	e_wsle();
	s_wsle(&io___834);
	do_lio(&c__9, &c__1, " ", (ftnlen)1);
	e_wsle();
	s_wsle(&io___835);
	do_lio(&c__9, &c__1, "   Wrong Coordinates Selection -------- >>", (
		ftnlen)42);
	do_lio(&c__3, &c__1, (char *)&(*coor), (ftnlen)sizeof(integer));
	e_wsle();
	s_wsle(&io___836);
	do_lio(&c__9, &c__1, " ", (ftnlen)1);
	e_wsle();
	goto L100;
    }
/* FROM 3-HOURLY TO HOURLY ap (New, interpolates between the three hourly ap values) */
    ape[0] = ap[1];
    ape[1] = ap[1];
    ape[37] = ap[13];
    ape[38] = ap[13];
    for (k = 1; k <= 13; ++k) {
	i__ = k * 3 - 1;
	ape[i__ - 1] = ap[k];
    }
    for (k = 1; k <= 12; ++k) {
	i__ = k * 3;
	ape[i__ - 1] = ((ap[k] << 1) + ap[k + 1]) / (float)3.;
    }
    for (k = 2; k <= 13; ++k) {
	i__ = k * 3 - 2;
	ape[i__ - 1] = (ap[k - 1] + (ap[k] << 1)) / (float)3.;
    }
/*     FROM 3-HOURLY TO HOURLY ap (old version without interpolation) */
/*      i = 1 */
/*      DO 10 k = 1,13 */
/*         DO j = 1,3 */
/*            ape(i) = ap(k) */
/*            i = i + 1 */
/*            END DO */
/* 10    CONTINUE */
/*     TO OBTAIN THE INTEGRAL OF ap. */
/*     INTEGRAL OF ap */
    if (*ut == 24) {
	*ut = 0;
    }
    if (*ut == 0 || *ut == 3 || *ut == 6 || *ut == 9 || *ut == 12 || *ut == 
	    15 || *ut == 18 || *ut == 21) {
	k = 1;
    } else if (*ut == 1 || *ut == 4 || *ut == 7 || *ut == 10 || *ut == 13 || *
	    ut == 16 || *ut == 19 || *ut == 22) {
	k = 2;
    } else if (*ut == 2 || *ut == 5 || *ut == 8 || *ut == 11 || *ut == 14 || *
	    ut == 17 || *ut == 20 || *ut == 23) {
	k = 3;
    } else {
	s_wsle(&io___840);
	do_lio(&c__9, &c__1, " ", (ftnlen)1);
	e_wsle();
	s_wsle(&io___841);
	do_lio(&c__9, &c__1, " ", (ftnlen)1);
	e_wsle();
	s_wsle(&io___842);
	do_lio(&c__9, &c__1, "  Wrong Universal Time value -------- >>", (
		ftnlen)40);
	do_lio(&c__3, &c__1, (char *)&(*ut), (ftnlen)sizeof(integer));
	e_wsle();
	s_wsle(&io___843);
	do_lio(&c__9, &c__1, " ", (ftnlen)1);
	e_wsle();
	goto L100;
    }
    rap = (float)0.;
    for (j = 1; j <= 36; ++j) {
	rap += fap[j - 1] * ape[k + j - 1];
    }
    if (rap <= (float)200.) {
	*cf = (float)1.;
	goto L100;
    }
    if (*doy > 366 || *doy < 1) {
	s_wsle(&io___846);
	do_lio(&c__9, &c__1, " ", (ftnlen)1);
	e_wsle();
	s_wsle(&io___847);
	do_lio(&c__9, &c__1, " ", (ftnlen)1);
	e_wsle();
	s_wsle(&io___848);
	do_lio(&c__9, &c__1, " ", (ftnlen)1);
	e_wsle();
	s_wsle(&io___849);
	do_lio(&c__9, &c__1, "      Wrong Day of Year value --- >>", (ftnlen)
		36);
	do_lio(&c__3, &c__1, (char *)&(*doy), (ftnlen)sizeof(integer));
	e_wsle();
	s_wsle(&io___850);
	do_lio(&c__9, &c__1, " ", (ftnlen)1);
	e_wsle();
	goto L100;
    }
    if (*rgma > (float)90. || *rgma < (float)-90.) {
	s_wsle(&io___851);
	do_lio(&c__9, &c__1, " ", (ftnlen)1);
	e_wsle();
	s_wsle(&io___852);
	do_lio(&c__9, &c__1, " ", (ftnlen)1);
	e_wsle();
	s_wsle(&io___853);
	do_lio(&c__9, &c__1, " ", (ftnlen)1);
	e_wsle();
	s_wsle(&io___854);
	do_lio(&c__9, &c__1, "   Wrong GEOMAGNETIC LATITUDE value --- >>", (
		ftnlen)42);
	do_lio(&c__4, &c__1, (char *)&(*rgma), (ftnlen)sizeof(real));
	e_wsle();
	s_wsle(&io___855);
	do_lio(&c__9, &c__1, " ", (ftnlen)1);
	e_wsle();
	goto L100;
    }
/*      write(6,*)rgma */
    dayno = *doy;
    if (*rgma < (float)0.) {
	dayno = *doy + 172;
	if (dayno > 365) {
	    dayno += -365;
	}
    }
    if (dayno >= 82) {
	rs = (dayno - (float)82.) / (float)45.6 + (float)1.;
    }
    if (dayno < 82) {
	rs = (dayno + (float)283.) / (float)45.6 + (float)1.;
    }
    s1 = rs;
    facs = rs - s1;
    s2 = s1 + 1;
    if (s2 == 9) {
	s2 = 1;
    }
/*      write(6,*)s1,s2,rs */
    *rgma = dabs(*rgma);
    rl = (*rgma + (float)10.) / (float)20. + 1;
    if (rl == (float)6.) {
	rl = (float)5.9;
    }
    l1 = rl;
    facl = rl - l1;
    l2 = l1 + 1;
/*      write(6,*)l1,l2,rl */
/*     FACTORS CALCULATIONS */
    if (rap < (float)300.) {
	rapf = (float)300.;
	n1 = code[s1 + (l1 << 3) - 9];
/* Computing 4th power */
	r__1 = rapf, r__1 *= r__1;
/* Computing 3rd power */
	r__2 = rapf;
/* Computing 2nd power */
	r__3 = rapf;
	cf1 = c4[n1 - 1] * (r__1 * r__1) + c3[n1 - 1] * (r__2 * (r__2 * r__2))
		 + c2[n1 - 1] * (r__3 * r__3) + c1[n1 - 1] * rapf + c0[n1 - 1]
		;
	n2 = code[s1 + (l2 << 3) - 9];
/* Computing 4th power */
	r__1 = rapf, r__1 *= r__1;
/* Computing 3rd power */
	r__2 = rapf;
/* Computing 2nd power */
	r__3 = rapf;
	cf2 = c4[n2 - 1] * (r__1 * r__1) + c3[n2 - 1] * (r__2 * (r__2 * r__2))
		 + c2[n2 - 1] * (r__3 * r__3) + c1[n2 - 1] * rapf + c0[n2 - 1]
		;
	n3 = code[s2 + (l1 << 3) - 9];
/* Computing 4th power */
	r__1 = rapf, r__1 *= r__1;
/* Computing 3rd power */
	r__2 = rapf;
/* Computing 2nd power */
	r__3 = rapf;
	cf3 = c4[n3 - 1] * (r__1 * r__1) + c3[n3 - 1] * (r__2 * (r__2 * r__2))
		 + c2[n3 - 1] * (r__3 * r__3) + c1[n3 - 1] * rapf + c0[n3 - 1]
		;
	n4 = code[s2 + (l2 << 3) - 9];
/* Computing 4th power */
	r__1 = rapf, r__1 *= r__1;
/* Computing 3rd power */
	r__2 = rapf;
/* Computing 2nd power */
	r__3 = rapf;
	cf4 = c4[n4 - 1] * (r__1 * r__1) + c3[n4 - 1] * (r__2 * (r__2 * r__2))
		 + c2[n4 - 1] * (r__3 * r__3) + c1[n4 - 1] * rapf + c0[n4 - 1]
		;
/*     INTERPOLATION */
	cf300 = cf1 * (1 - facs) * (1 - facl) + cf2 * (1 - facs) * facl + cf3 
		* facs * (1 - facl) + cf4 * facs * facl;
	*cf = (cf300 - (float)1.) * rap / (float)100. - cf300 * (float)2. + (
		float)3.;
	goto L100;
    }
    n1 = code[s1 + (l1 << 3) - 9];
/*      write(6,*)n1 */
/* Computing 4th power */
    r__1 = rap, r__1 *= r__1;
/* Computing 3rd power */
    r__2 = rap;
/* Computing 2nd power */
    r__3 = rap;
    cf1 = c4[n1 - 1] * (r__1 * r__1) + c3[n1 - 1] * (r__2 * (r__2 * r__2)) + 
	    c2[n1 - 1] * (r__3 * r__3) + c1[n1 - 1] * rap + c0[n1 - 1];
    n2 = code[s1 + (l2 << 3) - 9];
/* Computing 4th power */
    r__1 = rap, r__1 *= r__1;
/* Computing 3rd power */
    r__2 = rap;
/* Computing 2nd power */
    r__3 = rap;
    cf2 = c4[n2 - 1] * (r__1 * r__1) + c3[n2 - 1] * (r__2 * (r__2 * r__2)) + 
	    c2[n2 - 1] * (r__3 * r__3) + c1[n2 - 1] * rap + c0[n2 - 1];
    n3 = code[s2 + (l1 << 3) - 9];
/* Computing 4th power */
    r__1 = rap, r__1 *= r__1;
/* Computing 3rd power */
    r__2 = rap;
/* Computing 2nd power */
    r__3 = rap;
    cf3 = c4[n3 - 1] * (r__1 * r__1) + c3[n3 - 1] * (r__2 * (r__2 * r__2)) + 
	    c2[n3 - 1] * (r__3 * r__3) + c1[n3 - 1] * rap + c0[n3 - 1];
    n4 = code[s2 + (l2 << 3) - 9];
/* Computing 4th power */
    r__1 = rap, r__1 *= r__1;
/* Computing 3rd power */
    r__2 = rap;
/* Computing 2nd power */
    r__3 = rap;
    cf4 = c4[n4 - 1] * (r__1 * r__1) + c3[n4 - 1] * (r__2 * (r__2 * r__2)) + 
	    c2[n4 - 1] * (r__3 * r__3) + c1[n4 - 1] * rap + c0[n4 - 1];
/*     INTERPOLATION */
    *cf = cf1 * (1 - facs) * (1 - facl) + cf2 * (1 - facs) * facl + cf3 * 
	    facs * (1 - facl) + cf4 * facs * facl;
L100:
    return 0;
} /* storm_ */



/* **************************************************************************** */

/* Subroutine */ int vdrift_(real *xt, real *xl, real *param, real *y)
{
    /* Initialized data */

    static integer index_t__ = 13;
    static integer index_l__ = 8;
    static integer nfunc = 6;
    static real coeff1[594] = { (float)-10.80592,(float)-9.63722,(float)
	    -11.52666,(float)-.05716,(float)-.06288,(float).03564,(float)
	    -5.80962,(float)-7.86988,(float)-8.50888,(float)-.05194,(float)
	    -.05798,(float)-.00138,(float)2.09876,(float)-19.99896,(float)
	    -5.11393,(float)-.0537,(float)-.06585,(float).03171,(float)
	    -10.22653,(float)-3.62499,(float)-14.85924,(float)-.04023,(float)
	    -.0119,(float)-.09656,(float)-4.8518,(float)-26.26264,(float)
	    -6.20501,(float)-.05342,(float)-.05174,(float).02419,(float)
	    -13.98936,(float)-18.10416,(float)-9.30503,(float)-.01969,(float)
	    -.03132,(float)-.01984,(float)-18.36633,(float)-24.44898,(float)
	    -16.69001,(float).02033,(float)-.03414,(float)-.02062,(float)
	    -20.27621,(float)-16.95623,(float)-36.58234,(float).01445,(float)
	    -.02044,(float)-.08297,(float)1.4445,(float)5.53004,(float)
	    4.55166,(float)-.02356,(float)-.04267,(float).05023,(float)
	    5.50589,(float)7.05381,(float)1.94387,(float)-.03147,(float)
	    -.03548,(float).01166,(float)3.24165,(float)10.05002,(float)
	    4.26218,(float)-.03419,(float)-.02651,(float).07456,(float)
	    7.02218,(float).06708,(float)-11.31012,(float)-.03252,(float)
	    -.01021,(float)-.09008,(float)-3.47588,(float)-2.82534,(float)
	    -4.17668,(float)-.03719,(float)-.01519,(float).06507,(float)
	    -4.02607,(float)-11.19563,(float)-10.52923,(float)-.00592,(float)
	    -.01286,(float)-.00477,(float)-11.47478,(float)-9.57758,(float)
	    -10.36887,(float).04555,(float)-.02249,(float).00528,(float)
	    -14.19283,(float)7.86422,(float)-8.76821,(float).05758,(float)
	    -.02398,(float)-.04075,(float)14.5889,(float)36.63322,(float)
	    27.57497,(float).01358,(float)-.02316,(float).04723,(float)
	    12.53122,(float)29.38367,(float)21.40356,(float)-7.1e-4,(float)
	    -.00553,(float).01484,(float)18.64421,(float)26.27327,(float)
	    18.32704,(float).00578,(float).03349,(float).11249,(float)4.53014,
	    (float)6.15099,(float)7.41935,(float)-.0286,(float)-.00395,(float)
	    -.08394,(float)14.29422,(float)9.77569,(float)2.85689,(float)
	    -.00107,(float).04263,(float).10739,(float)7.17246,(float)4.40242,
	    (float)-1.00794,(float)8.9e-4,(float).01436,(float).00626,(float)
	    7.75487,(float)5.01928,(float)4.36908,(float).03952,(float)
	    -.00614,(float).03039,(float)10.25556,(float)8.82631,(float)
	    24.21745,(float).05492,(float)-.02968,(float).00177,(float)
	    21.86648,(float)24.03218,(float)39.82008,(float).0049,(float)
	    -.01281,(float)-.01715,(float)19.18547,(float)23.97403,(float)
	    34.44242,(float).01978,(float).01564,(float)-.02434,(float)
	    26.30614,(float)14.22662,(float)31.16844,(float).06495,(float)
	    .1959,(float).05631,(float)21.09354,(float)25.56253,(float)
	    29.91629,(float)-.04397,(float)-.08079,(float)-.07903,(float)
	    28.30202,(float)16.80567,(float)38.63945,(float).05864,(float)
	    .16407,(float).07622,(float)22.68528,(float)25.91119,(float)
	    40.45979,(float)-.03185,(float)-.01039,(float)-.01206,(float)
	    31.98703,(float)24.46271,(float)38.13028,(float)-.08738,(float)
	    -.0028,(float).01322,(float)46.67387,(float)16.80171,(float)
	    22.7719,(float)-.13643,(float)-.05277,(float)-.01982,(float)
	    13.87476,(float)20.52521,(float)5.22899,(float).00485,(float)
	    -.04357,(float).0997,(float)21.46928,(float)13.55871,(float)
	    10.23772,(float)-.04457,(float).01307,(float).06589,(float)
	    16.18181,(float)16.0296,(float)9.28661,(float)-.01225,(float)
	    .14623,(float)-.0157,(float)18.16289,(float)-1.5823,(float)
	    14.54986,(float)-.00375,(float)-8.7e-4,(float).04991,(float)
	    10.00292,(float)11.82653,(float).44417,(float)-.00768,(float)
	    .1594,(float)-.01775,(float)12.15362,(float)5.65843,(float)
	    -1.94855,(float)-.00689,(float).03851,(float).04851,(float)
	    -1.25167,(float)9.05439,(float).74164,(float).01065,(float).03153,
	    (float).02433,(float)-15.46799,(float)18.23132,(float)27.4532,(
	    float).00899,(float)-1.7e-4,(float).03385,(float)2.70396,(float)
	    -.87077,(float)6.11476,(float)-8.1e-4,(float).05167,(float)
	    -.08932,(float)3.21321,(float)-1.06622,(float)5.43623,(float)
	    .01942,(float).05449,(float)-.03084,(float)17.79267,(float)
	    -3.44694,(float)7.10702,(float).04734,(float)-.00945,(float)
	    .11516,(float).46435,(float)6.78467,(float)4.27231,(float)-.02122,
	    (float).10922,(float)-.03331,(float)15.31708,(float)1.70927,(
	    float)7.99584,(float).07462,(float).07515,(float).08934,(float)
	    4.19893,(float)6.01231,(float)8.04861,(float).04023,(float).14767,
	    (float)-.04308,(float)9.97541,(float)5.99412,(float)5.93588,(
	    float).06611,(float).12144,(float)-.02124,(float)13.02837,(float)
	    10.2995,(float)-4.862,(float).04521,(float).10715,(float)-.05465,(
	    float)5.26779,(float)7.09019,(float)1.76617,(float).09339,(float)
	    .22256,(float).09222,(float)9.1781,(float)5.27558,(float)5.45022,(
	    float).14749,(float).11616,(float).10418,(float)9.26391,(float)
	    4.19982,(float)12.6625,(float).11334,(float).02532,(float).18919,(
	    float)13.18695,(float)6.06564,(float)11.87835,(float).26347,(
	    float).02858,(float).14801,(float)10.08476,(float)6.14899,(float)
	    17.62618,(float).09331,(float).08832,(float).28208,(float)
	    10.75302,(float)7.09244,(float)13.90643,(float).09556,(float)
	    .16652,(float).22751,(float)6.70338,(float)11.97698,(float)
	    18.51413,(float).15873,(float).18936,(float).15705,(float)5.68102,
	    (float)23.81606,(float)20.65174,(float).1993,(float).15645,(float)
	    .08151,(float)29.61644,(float)5.49433,(float)48.90934,(float)
	    .7071,(float).40791,(float).26325,(float)17.11994,(float)19.6538,(
	    float)44.8881,(float).4551,(float).41689,(float).22398,(float)
	    8.457,(float)34.54442,(float)27.25364,(float).40867,(float).37223,
	    (float).22374,(float)-2.30305,(float)32.0066,(float)47.75799,(
	    float).02178,(float).43626,(float).30187,(float)8.98134,(float)
	    33.0182,(float)33.09674,(float).33703,(float).33242,(float).41156,
	    (float)14.27619,(float)20.70858,(float)50.10005,(float).30115,(
	    float).3257,(float).45061,(float)14.44685,(float)16.14272,(float)
	    45.40065,(float).37552,(float).31419,(float).30129,(float)6.19718,
	    (float)18.89559,(float)28.24927,(float).08864,(float).41627,(
	    float).19993,(float)7.70847,(float)-2.36281,(float)-21.41381,(
	    float).13766,(float).05113,(float)-.11631,(float)-9.07236,(float)
	    3.76797,(float)-20.49962,(float).03343,(float).0863,(float).00188,
	    (float)-8.58113,(float)5.06009,(float)-6.23262,(float).04967,(
	    float).03334,(float).24214,(float)-27.85742,(float)8.34615,(float)
	    -27.72532,(float)-.08935,(float).15905,(float)-.03655,(float)
	    2.77234,(float).14626,(float)-4.01786,(float).22338,(float)
	    -.04478,(float).1865,(float)5.61364,(float)-3.82235,(float)
	    -16.72282,(float).26456,(float)-.03119,(float)-.08376,(float)
	    13.35847,(float)-6.11518,(float)-16.50327,(float).28957,(float)
	    -.01345,(float)-.19223,(float)-5.3729,(float)-.09562,(float)
	    -27.27889,(float).00266,(float).22823,(float)-.35585,(float)
	    -15.29676,(float)-18.36622,(float)-24.62948,(float)-.31299,(float)
	    -.23832,(float)-.08463,(float)-23.37099,(float)-13.69954,(float)
	    -26.71177,(float)-.19654,(float)-.18522,(float)-.20679,(float)
	    -26.33762,(float)-15.96657,(float)-42.51953,(float)-.13575,(float)
	    -.00329,(float)-.28355,(float)-25.4214,(float)-14.14291,(float)
	    -21.91748,(float)-.2096,(float)-.19176,(float)-.32593,(float)
	    -23.36042,(float)-23.89895,(float)-46.0527,(float)-.10336,(float)
	    .0303,(float)-.21839,(float)-19.46259,(float)-21.27918,(float)
	    -32.38143,(float)-.17673,(float)-.15484,(float)-.11226,(float)
	    -19.06169,(float)-21.1324,(float)-34.01677,(float)-.25497,(float)
	    -.16878,(float)-.11004,(float)-18.39463,(float)-16.11516,(float)
	    -19.55804,(float)-.19834,(float)-.23271,(float)-.25699,(float)
	    -19.93482,(float)-17.56433,(float)-18.58818,(float).06508,(float)
	    -.18075,(float).02796,(float)-23.64078,(float)-18.77269,(float)
	    -22.77715,(float)-.02456,(float)-.12238,(float).02959,(float)
	    -12.44508,(float)-21.06941,(float)-19.36011,(float).02746,(float)
	    -.16329,(float).19792,(float)-26.34187,(float)-19.78854,(float)
	    -24.06651,(float)-.07299,(float)-.03082,(float)-.03535,(float)
	    -10.71667,(float)-26.04401,(float)-16.59048,(float).0285,(float)
	    -.0968,(float).15143,(float)-18.40481,(float)-23.3777,(float)
	    -16.3145,(float)-.03989,(float)-.00729,(float)-.01688,(float)
	    -9.68886,(float)-20.59304,(float)-18.46657,(float).01092,(float)
	    -.07901,(float).03422,(float)-.06685,(float)-19.2459,(float)
	    -29.35494,(float).12265,(float)-.24792,(float).05978,(float)
	    -15.32341,(float)-9.0732,(float)-13.76101,(float)-.17018,(float)
	    -.15122,(float)-.06144,(float)-14.68939,(float)-14.82251,(float)
	    -13.65846,(float)-.11173,(float)-.1441,(float)-.07133,(float)
	    -18.38628,(float)-18.94631,(float)-19.00893,(float)-.08062,(float)
	    -.14481,(float)-.12949,(float)-16.15328,(float)-17.40999,(float)
	    -14.08705,(float)-.08485,(float)-.06896,(float)-.11583,(float)
	    -14.50295,(float)-16.91671,(float)-25.25793,(float)-.06814,(float)
	    -.13727,(float)-.12213,(float)-10.92188,(float)-14.10852,(float)
	    -24.43877,(float)-.09375,(float)-.11638,(float)-.09053,(float)
	    -11.64716,(float)-14.9202,(float)-19.99063,(float)-.14792,(float)
	    -.08681,(float)-.12085,(float)-24.09766,(float)-16.14519,(float)
	    -8.05683,(float)-.24065,(float)-.05877,(float)-.23726,(float)
	    -25.18396,(float)-15.02034,(float)-15.50531,(float)-.12236,(float)
	    -.0961,(float)-.00529,(float)-15.27905,(float)-19.36708,(float)
	    -12.94046,(float)-.08571,(float)-.0956,(float)-.03544,(float)
	    -7.48927,(float)-16.00753,(float)-13.02842,(float)-.07862,(float)
	    -.1011,(float)-.05807 };
    static real coeff2[30] = { (float)-13.06383,(float)-27.98698,(float)
	    -18.80004,(float)-.05875,(float)-.03737,(float)-.11214,(float)
	    -13.6737,(float)-16.44925,(float)-16.12632,(float)-.07228,(float)
	    -.09322,(float)-.05652,(float)-22.61245,(float)-21.24717,(float)
	    -18.09933,(float)-.05197,(float)-.07477,(float)-.05235,(float)
	    -27.09189,(float)-21.85181,(float)-20.34676,(float)-.05123,(float)
	    -.05683,(float)-.07214,(float)-27.09561,(float)-22.76383,(float)
	    -25.41151,(float)-.10272,(float)-.02058,(float)-.1672 };

    /* System generated locals */
    integer i__1, i__2, i__3;

    /* Local variables */
    static real bspl4;
    extern /* Subroutine */ int g_(real *, real *, real *);
    static integer i__, j;
    static real coeff[624], funct[6];
    extern doublereal bspl4_time__(integer *, real *), bspl4_long__(integer *,
	     real *);
    static integer il, kk, ind;

/* ------------------------------------------------------------------- */
/*       SUBROUTINE CALCULATES EQUATORIAL VERTICAL DRIFT AS DESCRIBED */
/*       IN SCHERLIESS AND FEJER, JGR, 104, 6829-6842, 1999 */

/*       INPUT:   XT: SOLAR LOCAL TIME  [h] */
/*                XL: GEOGRAPHIC LONGITUDE (+ EAST) [degrees] */

/*           PARAM: 2-DIM ARRAY (DOY,F10.7CM) */
/*                  DOY     :Day of Year has to run from 1 to 365(366) */
/*                  F10.7cm : F10.7cm solar flux (daily value) */

/*       OUTPUT:   Y: EQUATORIAL VERTICAL DRIFT [m/s] */

/* ------------------------------------------------------------------- */
/*        IMPLICIT REAL*8 (A-H,O-Z) */
/*        real*8 param(2),coeff(624),coeff1(594),coeff2(30),funct(6) */
/*        real*8 xt,xl,y */
/*        real*8 bspl4,bspl4_time,bspl4_long */
    /* Parameter adjustments */
    --param;

    /* Function Body */
    for (i__ = 1; i__ <= 594; ++i__) {
	coeff[i__ - 1] = coeff1[i__ - 1];
    }
    for (i__ = 1; i__ <= 30; ++i__) {
	coeff[i__ + 593] = coeff2[i__ - 1];
    }
    g_(&param[1], funct, xl);
    *y = (float)0.;
    i__1 = index_t__;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = index_l__;
	for (il = 1; il <= i__2; ++il) {
	    kk = index_l__ * (i__ - 1) + il;
	    i__3 = nfunc;
	    for (j = 1; j <= i__3; ++j) {
		ind = nfunc * (kk - 1) + j;
		bspl4 = bspl4_time__(&i__, xt) * bspl4_long__(&il, xl);
		*y += bspl4 * funct[j - 1] * coeff[ind - 1];
	    }
	}
    }
    return 0;
} /* vdrift_ */



/*        real*8 function bspl4_time(i,x1) */
doublereal bspl4_time__(integer *i__, real *x1)
{
    /* Initialized data */

    static real t_t__[40] = { (float)0.,(float)2.75,(float)4.75,(float)5.5,(
	    float)6.25,(float)7.25,(float)10.,(float)14.,(float)17.25,(float)
	    18.,(float)18.75,(float)19.75,(float)21.,(float)24.,(float)26.75,(
	    float)28.75,(float)29.5,(float)30.25,(float)31.25,(float)34.,(
	    float)38.,(float)41.25,(float)42.,(float)42.75,(float)43.75,(
	    float)45.,(float)48.,(float)50.75,(float)52.75,(float)53.5,(float)
	    54.25,(float)55.25,(float)58.,(float)62.,(float)65.25,(float)66.,(
	    float)66.75,(float)67.75,(float)69.,(float)72. };

    /* System generated locals */
    integer i__1, i__2;
    real ret_val;

    /* Local variables */
    static real b[400]	/* was [20][20] */;
    static integer j, k;
    static real x;
    static integer order;

/*       ************************************************* */
/*        implicit REAL*8 (A-H,O-Z) */
/*        real*8 t_t(0:39) */
/*        real*8 x,b(20,20),x1 */
    order = 4;
    x = *x1;
    if (*i__ >= 0) {
	if (x < t_t__[*i__]) {
	    x += 24;
	}
    }
    i__1 = *i__ + order - 1;
    for (j = *i__; j <= i__1; ++j) {
	if (x >= t_t__[j] && x < t_t__[j + 1]) {
	    b[j - 1] = (float)1.;
	} else {
	    b[j - 1] = (float)0.;
	}
    }
    i__1 = order;
    for (j = 2; j <= i__1; ++j) {
	i__2 = *i__ + order - j;
	for (k = *i__; k <= i__2; ++k) {
	    b[k + j * 20 - 21] = (x - t_t__[k]) / (t_t__[k + j - 1] - t_t__[k]
		    ) * b[k + (j - 1) * 20 - 21];
	    b[k + j * 20 - 21] += (t_t__[k + j] - x) / (t_t__[k + j] - t_t__[
		    k + 1]) * b[k + 1 + (j - 1) * 20 - 21];
	}
    }
    ret_val = b[*i__ + order * 20 - 21];
    return ret_val;
} /* bspl4_time__ */



doublereal bspl4_long__(integer *i__, real *x1)
{
    /* Initialized data */

    static real t_l__[25] = { (float)0.,(float)10.,(float)100.,(float)190.,(
	    float)200.,(float)250.,(float)280.,(float)310.,(float)360.,(float)
	    370.,(float)460.,(float)550.,(float)560.,(float)610.,(float)640.,(
	    float)670.,(float)720.,(float)730.,(float)820.,(float)910.,(float)
	    920.,(float)970.,(float)1e3,(float)1030.,(float)1080. };

    /* System generated locals */
    integer i__1, i__2;
    real ret_val;

    /* Local variables */
    static real b[400]	/* was [20][20] */;
    static integer j, k;
    static real x;
    static integer order;

/*        real*8 function bspl4_long(i,x1) */
/*       ************************************************* */
/*       implicit real*8 (A-H,O-Z) */
/*        real*8 t_l(0:24) */
/*        real*8 x,b(20,20),x1 */
    order = 4;
    x = *x1;
    if (*i__ >= 0) {
	if (x < t_l__[*i__]) {
	    x += 360;
	}
    }
    i__1 = *i__ + order - 1;
    for (j = *i__; j <= i__1; ++j) {
	if (x >= t_l__[j] && x < t_l__[j + 1]) {
	    b[j - 1] = (float)1.;
	} else {
	    b[j - 1] = (float)0.;
	}
    }
    i__1 = order;
    for (j = 2; j <= i__1; ++j) {
	i__2 = *i__ + order - j;
	for (k = *i__; k <= i__2; ++k) {
	    b[k + j * 20 - 21] = (x - t_l__[k]) / (t_l__[k + j - 1] - t_l__[k]
		    ) * b[k + (j - 1) * 20 - 21];
	    b[k + j * 20 - 21] += (t_l__[k + j] - x) / (t_l__[k + j] - t_l__[
		    k + 1]) * b[k + 1 + (j - 1) * 20 - 21];
	}
    }
    ret_val = b[*i__ + order * 20 - 21];
    return ret_val;
} /* bspl4_long__ */



/* Subroutine */ int g_(real *param, real *funct, real *x)
{
    /* System generated locals */
    real r__1, r__2;

    /* Builtin functions */
    double exp(doublereal);

    /* Local variables */
    static real flux, a;
    static integer i__;
    static real sigma, cflux, gauss;

/*       ************************************************* */
/*        implicit real*8 (A-H,O-Z) */
/*        real*8 param(2),funct(6) */
/*        real*8 x,a,sigma,gauss,flux,cflux */
/*       ************************************************* */
    /* Parameter adjustments */
    --funct;
    --param;

    /* Function Body */
    flux = param[2];
    if (param[2] <= (float)75.) {
	flux = (float)75.;
    }
    if (param[2] >= (float)230.) {
	flux = (float)230.;
    }
    cflux = flux;
    a = (float)0.;
    if (param[1] >= (float)120. && param[1] <= (float)240.) {
	a = (float)170.;
    }
    if (param[1] >= (float)120. && param[1] <= (float)240.) {
	sigma = (float)60.;
    }
    if (param[1] <= (float)60. || param[1] >= (float)300.) {
	a = (float)170.;
    }
    if (param[1] <= (float)60. || param[1] >= (float)300.) {
	sigma = (float)40.;
    }
    if (flux <= (float)95. && a != (float)0.) {
/* Computing 2nd power */
	r__1 = *x - a;
/* Computing 2nd power */
	r__2 = sigma;
	gauss = exp(r__1 * r__1 * (float)-.5 / (r__2 * r__2));
	cflux = gauss * (float)95. + (1 - gauss) * flux;
    }
/*       ************************************************* */
/*       ************************************************* */
    for (i__ = 1; i__ <= 6; ++i__) {
	funct[i__] = (float)0.;
    }
/*       ************************************************* */
/*       ************************************************* */
    if (param[1] >= (float)135. && param[1] <= (float)230.) {
	funct[1] = (float)1.;
    }
    if (param[1] <= (float)45. || param[1] >= (float)320.) {
	funct[2] = (float)1.;
    }
    if (param[1] > (float)75. && param[1] < (float)105.) {
	funct[3] = (float)1.;
    }
    if (param[1] > (float)260. && param[1] < (float)290.) {
	funct[3] = (float)1.;
    }
/*       ************************************************* */
    if (param[1] >= (float)45. && param[1] <= (float)75.) {
/* W-E */
	funct[2] = (float)1. - (param[1] - (float)45.) / (float)30.;
	funct[3] = 1 - funct[2];
    }
    if (param[1] >= (float)105. && param[1] <= (float)135.) {
/* E-S */
	funct[3] = (float)1. - (param[1] - (float)105.) / (float)30.;
	funct[1] = 1 - funct[3];
    }
    if (param[1] >= (float)230. && param[1] <= (float)260.) {
/* S-E */
	funct[1] = (float)1. - (param[1] - (float)230.) / (float)30.;
	funct[3] = 1 - funct[1];
    }
    if (param[1] >= (float)290. && param[1] <= (float)320.) {
/* E-W */
	funct[3] = (float)1. - (param[1] - (float)290.) / (float)30.;
	funct[2] = 1 - funct[3];
    }
/*       ************************************************* */
    funct[4] = (cflux - 140) * funct[1];
    funct[5] = (cflux - 140) * funct[2];
    funct[6] = (flux - 140) * funct[3];
/*       ************************************************* */
    return 0;
} /* g_ */



/* Subroutine */ int stormvd_(integer *flag__, integer *ip, real *ae, real *
	slt, real *promptvd, real *dynamovd, real *vd)
{
    /* Initialized data */

    static real coff1[45]	/* was [5][9] */ = { (float).0124,(float)
	    -.0168,(float)-.0152,(float)-.0174,(float)-.0704,(float)-.009,(
	    float)-.0022,(float)-.0107,(float).0152,(float)-.0674,(float)
	    .0275,(float).0051,(float)-.0132,(float).002,(float)-.011,(float)
	    -.0022,(float).0044,(float).0095,(float).0036,(float)-.0206,(
	    float).0162,(float)7e-4,(float).0085,(float)-.014,(float).0583,(
	    float).0181,(float).0185,(float)-.0109,(float)-.0031,(float)
	    -.0427,(float)-.0057,(float)2e-4,(float).0086,(float).0149,(float)
	    .2637,(float)-.0193,(float).0035,(float).0117,(float).0099,(float)
	    .3002,(float)-.0492,(float)-.0201,(float).0338,(float).0099,(
	    float).0746 };
    static real coff15[54]	/* was [6][9] */ = { (float).0177,(float)
	    .0118,(float)-6e-4,(float)-.0152,(float)-.0174,(float)-.0704,(
	    float).0051,(float)-.0074,(float)-.0096,(float)-.0107,(float)
	    .0152,(float)-.0674,(float).0241,(float).0183,(float).0122,(float)
	    -.0132,(float).002,(float)-.011,(float).0019,(float)-.001,(float)
	    1e-4,(float).0095,(float).0036,(float)-.0206,(float).017,(float)
	    .0183,(float).0042,(float).0085,(float)-.014,(float).0583,(float)
	    .0086,(float).0189,(float).02,(float)-.0109,(float)-.0031,(float)
	    -.0427,(float)-.007,(float)-.0053,(float)-.009,(float).0086,(
	    float).0149,(float).2637,(float)-.0326,(float)-.0101,(float).0076,
	    (float).0117,(float).0099,(float).3002,(float)-.047,(float)-.0455,
	    (float)-.0274,(float).0338,(float).0099,(float).0746 };

    /* Builtin functions */
    double exp(doublereal);
    integer s_wsle(cilist *), do_lio(integer *, integer *, char *, ftnlen), 
	    e_wsle();

    /* Local variables */
    static real alfa, beta, ae1_12__, aed1_6__;
    static integer i__, j;
    static real aed7_12__, ae7_12s__, aed22_28__, daet_30__, aed1_6s__, 
	    daet_90__, daet_75__, aed7_12s__, aed22_28p__, aed22_28s__;
    extern doublereal bspl4_ptime__(integer *, real *);
    static real daet_7p5__, ae1_6__;

    /* Fortran I/O blocks */
    static cilist io___930 = { 0, 6, 0, 0, 0 };


/* ******************************************************************* */
/*  Empirical vertical disturbance drifts model */
/*  After Fejer and Scherliess, JGR, 102, 24047-24056,1997 */
/* ********************************************************************* */
/*  INPUT: */
/*    AE: AE(in nT) in 1 hour or 15 minute resolution; */
/*    SLT: Local time(in hrs) for wanted Vd; */
/*  OUTPUT: */
/*    PromptVd: Prompt penetration vertical drifts at given conditions; */
/*    DynamoVd: Disturbane dynamo vertical drifts at given conditions; */
/*    Vd: PromptVd+DynamoVd; */
/* ********************************************************************* */
/*       IMPLICIT REAL*8(A-H,O-Z) */
/*       REAL*8 AE(1:366*24*4),Coff1(1:5,1:9),Coff15(1:6,1:9) */
    /* Parameter adjustments */
    --ae;

    /* Function Body */
/* CCCCCCCCCCCCCCCC**Define to variables**CCCCCCCCCCCCCCCCCCCCC */
/* To 1 h time resolution: */
/* dAEt_30=AE(t)-AE(t-1 hour); */
/* dAEt_90=AE(t-1 hour)-AE(t-2 hour); */
/* C */
/* To 15 MIN time resolution : */
/* dAEt_7P5=AE(t)-AE(t-15min); */
/* dAEt_30=AE(t-15)-AE(t-45min); */
/* dAEt_75=AE(t-45)-AE(t-105min); */
/* C */
/*  Following variables are the same to two resolution: */
/* AE1_6=average(AE(1-6hours)); */
/* AE7_12=average(AE(7-12hours)); */
/* AE1_12=average(AE(1-12hours)); */
/* AEd1_6=average(X(AE(1-6hours)-130 nT)); */
/* AEd7_12=average(X(AE(7-12hours)-130 nT)); */
/* AEd1_12=average(X(AE(1-12hours)-130 nT)); */
/* AEd22_28=average(X(AE(22-28hours)-130 nT)); */
/* Here X(a)=a, a>0; =0, a<=0; */
/* Alfa=0,            AE1_6<200 nT; */
/*      AE1_6/100-2, 200 nT<AE1_6<200 nT; */
/*      1,            AE1_6>300 nT; */
/* Beta=exp(-AE1_12/90),  AE1_12>=70nT; */
/*      0.46,              AE1_12<70 nT; */
/* CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCccccccc */
/* ***************************************************** */
/* C        FLAG>0--> 1 h time resolution */
/* **************************************************** */
    if (*flag__ > 0) {

	daet_30__ = ae[*ip] - ae[*ip - 1];
	daet_90__ = ae[*ip - 1] - ae[*ip - 2];

	ae1_6__ = (float)0.;
	aed1_6__ = (float)0.;
	for (i__ = -1; i__ >= -6; --i__) {
	    ae1_6__ += ae[*ip + i__];
	    aed1_6s__ = ae[*ip + i__] - 130.;
	    if (aed1_6s__ <= 0.) {
		aed1_6s__ = (float)0.;
	    }
	    aed1_6__ += aed1_6s__;
	}
	ae1_6__ /= 6.;
	aed1_6__ /= 6.;

	aed7_12__ = (float)0.;
	for (i__ = -7; i__ >= -12; --i__) {
	    aed7_12s__ = ae[*ip + i__] - 130.;
	    if (aed7_12s__ <= 0.) {
		ae7_12s__ = (float)0.;
	    }
	    aed7_12__ += aed7_12s__;
	}
	aed7_12__ /= 6.;

	ae1_12__ = (float)0.;
	for (i__ = -1; i__ >= -12; --i__) {
	    ae1_12__ += ae[*ip + i__];
	}
	ae1_12__ /= 12.;

	aed22_28__ = (float)0.;
	for (i__ = -22; i__ >= -28; --i__) {
	    aed22_28s__ = ae[*ip + i__] - 130.;
	    if (aed22_28s__ <= 0.) {
		aed22_28s__ = (float)0.;
	    }
	    aed22_28__ += aed22_28s__;
	}
	aed22_28__ /= 7.;
	aed22_28p__ = aed22_28__ - 200.;
	if (aed22_28p__ <= 0.) {
	    aed22_28p__ = (float)0.;
	}
/* C */
	if (ae1_6__ > 300.) {
	    alfa = (float)1.;
	} else if (ae1_6__ > 200.) {
	    alfa = ae1_6__ / 100. - 2.;
	} else {
	    alfa = (float)0.;
	}
/* C */
	if (ae1_12__ >= 70.) {
	    beta = exp(-ae1_12__ / 90.);
	} else {
	    beta = (float).46;
	}
	*promptvd = (float)0.;
	for (j = 1; j <= 9; ++j) {
	    *promptvd += (coff1[j * 5 - 5] * daet_30__ + coff1[j * 5 - 4] * 
		    daet_90__) * bspl4_ptime__(&j, slt);
	}
	*dynamovd = (float)0.;
	for (j = 1; j <= 9; ++j) {
	    *dynamovd += (coff1[j * 5 - 3] * aed1_6__ + coff1[j * 5 - 2] * 
		    alfa * aed7_12__ + coff1[j * 5 - 1] * beta * aed22_28p__) 
		    * bspl4_ptime__(&j, slt);
	}
	*vd = *promptvd + *dynamovd;
	return 0;
/* 1 h time resolution end; */
/* ******************************************************************** */
/*                  15 min time resolution */
/* ******************************************************************** */
    } else {
	daet_7p5__ = ae[*ip] - ae[*ip - 1];
	daet_30__ = ae[*ip - 1] - ae[*ip - 3];
	daet_75__ = ae[*ip - 3] - ae[*ip - 7];
	ae1_6__ = (float)0.;
	aed1_6__ = (float)0.;
	for (i__ = -4; i__ >= -24; --i__) {
	    ae1_6__ += ae[*ip + i__];
	    aed1_6s__ = ae[*ip + i__] - (float)130.;
	    if (aed1_6s__ <= (float)0.) {
		aed1_6s__ = (float)0.;
	    }
	    aed1_6__ += aed1_6s__;
	}
	ae1_6__ /= 21.;
	aed1_6__ /= 21.;
/* C */
	aed7_12__ = (float)0.;
	for (i__ = -28; i__ >= -48; --i__) {
	    aed7_12s__ = ae[*ip + i__] - (float)130.;
	    if (aed7_12s__ <= (float)0.) {
		aed7_12s__ = (float)0.;
	    }
	    aed7_12__ += aed7_12s__;
	}
	aed7_12__ /= 21.;
/* C */
	ae1_12__ = (float)0.;
	for (i__ = -4; i__ >= -48; --i__) {
	    ae1_12__ += ae[*ip + i__];
	}
	ae1_12__ /= 45.;
/* C */
	aed22_28__ = (float)0.;
	for (i__ = -88; i__ >= -112; --i__) {
	    aed22_28s__ = ae[*ip + i__] - (float)130.;
	    if (aed22_28s__ <= (float)0.) {
		aed22_28s__ = (float)0.;
	    }
	    aed22_28__ += aed22_28s__;
	}
	aed22_28__ /= 25.;
	aed22_28p__ = aed22_28__ - 200.;
	if (aed22_28p__ <= 0.) {
	    aed22_28p__ = (float)0.;
	}
/*         AE1_6=0.0D0 */
/*         AEd1_6=0.0D0 */
/*         AEd7_12=0.0D0 */
/*         AEd22_28P=0.0D0 */
/*         AE1_12=0.0D0 */
/*         dAEt_7P5=400.D0 */
/*         dAEt_30=0.D0 */
/*         dAEt_75=0.D0 */
/* C */
	if (ae1_6__ > 300.) {
	    alfa = (float)1.;
	} else if (ae1_6__ > 200.) {
	    alfa = ae1_6__ / 100. - 2.;
	} else {
	    alfa = (float)0.;
	}
/* C */
	if (ae1_12__ >= 70.) {
	    beta = exp(-ae1_12__ / 90.);
	} else {
	    beta = (float).46;
	}
/* C */
	*promptvd = (float)0.;
	for (j = 1; j <= 9; ++j) {
	    *promptvd += (coff15[j * 6 - 6] * daet_7p5__ + coff15[j * 6 - 5] *
		     daet_30__ + coff15[j * 6 - 4] * daet_75__) * 
		    bspl4_ptime__(&j, slt);
	}
	*dynamovd = (float)0.;
	s_wsle(&io___930);
	do_lio(&c__4, &c__1, (char *)&aed1_6__, (ftnlen)sizeof(real));
	do_lio(&c__4, &c__1, (char *)&aed7_12__, (ftnlen)sizeof(real));
	do_lio(&c__4, &c__1, (char *)&aed22_28p__, (ftnlen)sizeof(real));
	do_lio(&c__4, &c__1, (char *)&alfa, (ftnlen)sizeof(real));
	do_lio(&c__4, &c__1, (char *)&beta, (ftnlen)sizeof(real));
	e_wsle();
	for (j = 1; j <= 9; ++j) {
	    *dynamovd += (coff15[j * 6 - 3] * aed1_6__ + coff15[j * 6 - 2] * 
		    alfa * aed7_12__ + coff15[j * 6 - 1] * beta * aed22_28p__)
		     * bspl4_ptime__(&j, slt);
	}
	*vd = *promptvd + *dynamovd;
    }
    return 0;
} /* stormvd_ */



doublereal bspl4_ptime__(integer *i__, real *x1)
{
    /* Initialized data */

    static real t_t__[28] = { (float)0.,(float)3.,(float)4.5,(float)6.,(float)
	    9.,(float)12.,(float)15.,(float)18.,(float)21.,(float)24.,(float)
	    27.,(float)28.5,(float)30.,(float)33.,(float)36.,(float)39.,(
	    float)42.,(float)45.,(float)48.,(float)51.,(float)52.5,(float)54.,
	    (float)57.,(float)60.,(float)63.,(float)66.,(float)69.,(float)72. 
	    };

    /* System generated locals */
    integer i__1, i__2;
    real ret_val;

    /* Local variables */
    static real b[400]	/* was [20][20] */;
    static integer j, k;
    static real x;
    static integer order;

/*       real*8 function bspl4_ptime(i,x1) */
/* ************************************************* */
/*       IMPLICIT REAL*8 (A-H,O-Z) */
/*       real*8 t_t(0:27) */
/*       real*8 x,b(20,20),x1 */

    order = 4;
    x = *x1;
    if (*i__ >= 0) {
	if (x < t_t__[*i__]) {
	    x += 24;
	}
    }
    i__1 = *i__ + order - 1;
    for (j = *i__; j <= i__1; ++j) {
	if (x >= t_t__[j] && x < t_t__[j + 1]) {
	    b[j - 1] = (float)1.;
	} else {
	    b[j - 1] = (float)0.;
	}
    }

    i__1 = order;
    for (j = 2; j <= i__1; ++j) {
	i__2 = *i__ + order - j;
	for (k = *i__; k <= i__2; ++k) {
	    b[k + j * 20 - 21] = (x - t_t__[k]) / (t_t__[k + j - 1] - t_t__[k]
		    ) * b[k + (j - 1) * 20 - 21];
	    b[k + j * 20 - 21] += (t_t__[k + j] - x) / (t_t__[k + j] - t_t__[
		    k + 1]) * b[k + 1 + (j - 1) * 20 - 21];
	}
    }
    ret_val = b[*i__ + order * 20 - 21];
    return ret_val;
} /* bspl4_ptime__ */


/* *************************************************************************** */

/* Subroutine */ int spreadf_brazil__(integer *idoy, integer *idiy, real *
	f107, real *geolat, real *osfbr)
{
    /* Initialized data */

    static real coef_sfa__[684] = { (float).07,(float).13,(float)0.,(float)0.,
	    (float)0.,(float)0.,(float)0.,(float)0.,(float)0.,(float).05,(
	    float).04,(float).03,(float).06,(float).07,(float).02,(float).03,(
	    float).03,(float).07,(float).06,(float).07,(float).21,(float).28,(
	    float).34,(float).16,(float).12,(float)0.,(float).02,(float).02,(
	    float).04,(float).05,(float).02,(float).11,(float).19,(float).31,(
	    float).31,(float).11,(float).14,(float).16,(float).03,(float)0.,(
	    float)0.,(float).02,(float)0.,(float)0.,(float).05,(float).55,(
	    float).61,(float).28,(float).26,(float).1,(float).15,(float).23,(
	    float).07,(float).06,(float).03,(float).03,(float).41,(float).88,(
	    float).89,(float).65,(float).19,(float).18,(float).17,(float).1,(
	    float).14,(float).15,(float).03,(float).14,(float).46,(float).72,(
	    float).71,(float).53,(float).57,(float).38,(float).3,(float).14,(
	    float)0.,(float).04,(float).03,(float).02,(float).21,(float).84,(
	    float).87,(float).72,(float).79,(float).6,(float).65,(float).7,(
	    float).29,(float).19,(float).19,(float).32,(float).73,(float).96,(
	    float).99,(float).84,(float).75,(float).78,(float).79,(float).7,(
	    float).63,(float).24,(float).28,(float).53,(float).75,(float).77,(
	    float).75,(float).85,(float).78,(float).51,(float).59,(float).24,(
	    float)0.,(float).07,(float).05,(float).06,(float).33,(float).92,(
	    float).96,(float).89,(float).9,(float).84,(float).86,(float).81,(
	    float).33,(float).27,(float).23,(float).47,(float).9,(float)1.,(
	    float)1.,(float).96,(float).96,(float).89,(float).92,(float).84,(
	    float).8,(float).27,(float).35,(float).61,(float).81,(float).93,(
	    float).86,(float).97,(float).84,(float).65,(float).75,(float).25,(
	    float)0.,(float).04,(float).08,(float).06,(float).53,(float).93,(
	    float).96,(float).94,(float).95,(float).84,(float).91,(float).71,(
	    float).18,(float).17,(float).21,(float).42,(float).92,(float).99,(
	    float).97,(float).92,(float).92,(float).93,(float).92,(float).67,(
	    float).58,(float).21,(float).38,(float).55,(float).83,(float).9,(
	    float).89,(float).97,(float).84,(float).71,(float).91,(float).21,(
	    float).02,(float).07,(float).03,(float).03,(float).6,(float).95,(
	    float).96,(float).92,(float).97,(float).91,(float).92,(float).67,(
	    float).11,(float).08,(float).09,(float).23,(float).9,(float).99,(
	    float).99,(float).96,(float).96,(float).93,(float).98,(float).63,(
	    float).25,(float).08,(float).12,(float).41,(float).79,(float).95,(
	    float).98,(float).99,(float).86,(float).8,(float).94,(float).22,(
	    float).02,(float).04,(float).03,(float).03,(float).63,(float).95,(
	    float).96,(float).94,(float).98,(float).9,(float).91,(float).59,(
	    float).1,(float).04,(float).07,(float).15,(float).83,(float).97,(
	    float).97,(float).9,(float).92,(float).93,(float).95,(float).57,(
	    float).12,(float).03,(float).05,(float).23,(float).74,(float).94,(
	    float).94,(float).99,(float).84,(float).84,(float).9,(float).24,(
	    float).02,(float).07,(float).07,(float).03,(float).6,(float).95,(
	    float).96,(float).97,(float).93,(float).82,(float).83,(float).51,(
	    float).08,(float).07,(float).09,(float).09,(float).71,(float).95,(
	    float).92,(float).87,(float).91,(float).91,(float).89,(float).5,(
	    float).14,(float).03,(float).06,(float).14,(float).61,(float).84,(
	    float).89,(float).94,(float).77,(float).82,(float).84,(float).34,(
	    float).1,(float).11,(float).12,(float).06,(float).43,(float).87,(
	    float).94,(float).97,(float).91,(float).77,(float).68,(float).42,(
	    float).06,(float).08,(float).1,(float).04,(float).51,(float).78,(
	    float).71,(float).77,(float).85,(float).88,(float).77,(float).35,(
	    float).16,(float).05,(float).08,(float).15,(float).53,(float).7,(
	    float).6,(float).89,(float).85,(float).71,(float).72,(float).26,(
	    float).16,(float).17,(float).08,(float).15,(float).38,(float).73,(
	    float).91,(float).91,(float).89,(float).68,(float).53,(float).26,(
	    float).06,(float).12,(float).08,(float).09,(float).32,(float).63,(
	    float).67,(float).77,(float).81,(float).79,(float).59,(float).21,(
	    float).14,(float).03,(float).06,(float).09,(float).23,(float).51,(
	    float).34,(float).79,(float).88,(float).66,(float).59,(float).16,(
	    float).18,(float).15,(float).16,(float).16,(float).33,(float).67,(
	    float).75,(float).88,(float).8,(float).64,(float).52,(float).16,(
	    float).04,(float).09,(float).04,(float).09,(float).24,(float).47,(
	    float).53,(float).5,(float).73,(float).69,(float).48,(float).11,(
	    float).14,(float).03,(float).03,(float).03,(float).2,(float).37,(
	    float).28,(float).54,(float).81,(float).64,(float).49,(float).18,(
	    float).12,(float).17,(float).16,(float).19,(float).31,(float).57,(
	    float).7,(float).83,(float).76,(float).57,(float).52,(float).13,(
	    float).04,(float).06,(float).05,(float).08,(float).21,(float).49,(
	    float).47,(float).39,(float).69,(float).66,(float).43,(float).11,(
	    float).1,(float).02,(float)0.,(float).03,(float).16,(float).39,(
	    float).24,(float).35,(float).77,(float).45,(float).39,(float).1,(
	    float).1,(float).13,(float).15,(float).18,(float).29,(float).57,(
	    float).7,(float).69,(float).71,(float).49,(float).54,(float).2,(
	    float).05,(float).06,(float).05,(float).06,(float).27,(float).42,(
	    float).36,(float).42,(float).61,(float).59,(float).5,(float).08,(
	    float).06,(float).02,(float).03,(float).02,(float).16,(float).4,(
	    float).17,(float).31,(float).68,(float).3,(float).28,(float).13,(
	    float).1,(float).16,(float).14,(float).08,(float).19,(float).5,(
	    float).63,(float).62,(float).63,(float).45,(float).51,(float).13,(
	    float).06,(float).07,(float).04,(float).06,(float).27,(float).42,(
	    float).28,(float).35,(float).68,(float).53,(float).57,(float).15,(
	    float).05,(float)0.,(float)0.,(float).05,(float).31,(float).33,(
	    float).18,(float).22,(float).59,(float).32,(float).21,(float).06,(
	    float).1,(float).16,(float).12,(float).1,(float).19,(float).41,(
	    float).55,(float).54,(float).69,(float).43,(float).43,(float).15,(
	    float).06,(float).05,(float).05,(float).08,(float).29,(float).39,(
	    float).23,(float).29,(float).57,(float).51,(float).56,(float).13,(
	    float).06,(float)0.,(float)0.,(float).05,(float).34,(float).27,(
	    float).19,(float).24,(float).49,(float).16,(float).13,(float).09,(
	    float).04,(float).11,(float).11,(float).05,(float).17,(float).32,(
	    float).49,(float).49,(float).6,(float).42,(float).38,(float).11,(
	    float).06,(float).04,(float).07,(float).07,(float).25,(float).36,(
	    float).21,(float).25,(float).65,(float).48,(float).53,(float).17,(
	    float).05,(float)0.,(float)0.,(float).11,(float).29,(float).14,(
	    float).2,(float).22,(float).44,(float).16,(float).18,(float).07,(
	    float).04,(float).04,(float).07,(float).03,(float).12,(float).23,(
	    float).39,(float).43,(float).57,(float).4,(float).35,(float).14,(
	    float).06,(float).03,(float).04,(float).07,(float).18,(float).27,(
	    float).14,(float).15,(float).45,(float).5,(float).5,(float).19,(
	    float).06,(float)0.,(float).02,(float).05,(float).26,(float).19,(
	    float).15,(float).18,(float).23,(float).09,(float).12,(float).06,(
	    float).04,(float).02,(float).02,(float).02,(float).1,(float).03,(
	    float).14,(float).26,(float).39,(float).34,(float).22,(float).07,(
	    float).03,(float)0.,(float).04,(float).01,(float).15,(float).01,(
	    float).04,(float).14,(float).41,(float).39,(float).35,(float).13,(
	    float).02,(float)0.,(float)0.,(float).06,(float).17,(float).07,(
	    float).06,(float).14,(float).07,(float).02,(float).03,(float)0.,(
	    float)0.,(float)0.,(float)0.,(float)0.,(float)0.,(float).01,(
	    float).03,(float).08,(float).19,(float).14,(float).14,(float)0.,(
	    float).03,(float).01,(float).02,(float)0.,(float).09,(float)0.,(
	    float).01,(float)0.,(float).18,(float).09,(float).16,(float).08,(
	    float).01,(float)0.,(float).02,(float).02,(float).15,(float)0.,(
	    float).03,(float).04 };
    static real coef_sfb__[684] = { (float)0.,(float)0.,(float)0.,(float)0.,(
	    float)0.,(float)0.,(float)0.,(float)0.,(float)0.,(float)0.,(float)
	    0.,(float)0.,(float)0.,(float)0.,(float)0.,(float)0.,(float)0.,(
	    float)0.,(float)0.,(float).03,(float)0.,(float)0.,(float)0.,(
	    float)0.,(float)0.,(float)0.,(float)0.,(float)0.,(float)0.,(float)
	    0.,(float)0.,(float).02,(float).01,(float)0.,(float)0.,(float)0.,(
	    float)0.,(float)0.,(float)0.,(float)0.,(float)0.,(float)0.,(float)
	    0.,(float)0.,(float)0.,(float)0.,(float)0.,(float)0.,(float)0.,(
	    float).01,(float)0.,(float)0.,(float)0.,(float)0.,(float)0.,(
	    float).01,(float).01,(float)0.,(float)0.,(float)0.,(float)0.,(
	    float)0.,(float)0.,(float)0.,(float)0.,(float)0.,(float)0.,(float)
	    .05,(float).03,(float)0.,(float).02,(float)0.,(float)0.,(float)0.,
	    (float)0.,(float)0.,(float)0.,(float)0.,(float)0.,(float)0.,(
	    float)0.,(float)0.,(float)0.,(float)0.,(float)0.,(float).04,(
	    float)0.,(float).01,(float)0.,(float)0.,(float)0.,(float).01,(
	    float).01,(float).05,(float)0.,(float)0.,(float)0.,(float)0.,(
	    float)0.,(float)0.,(float)0.,(float)0.,(float)0.,(float).04,(
	    float)0.,(float).03,(float).03,(float)0.,(float).01,(float)0.,(
	    float)0.,(float)0.,(float)0.,(float)0.,(float)0.,(float)0.,(float)
	    0.,(float)0.,(float).01,(float)0.,(float).01,(float).04,(float)
	    .04,(float).03,(float)0.,(float).01,(float)0.,(float).01,(float)
	    0.,(float).27,(float).14,(float).06,(float).05,(float).04,(float)
	    .02,(float)0.,(float)0.,(float)0.,(float)0.,(float).04,(float).09,
	    (float).48,(float).43,(float).27,(float).05,(float).04,(float).01,
	    (float)0.,(float)0.,(float)0.,(float)0.,(float)0.,(float)0.,(
	    float).13,(float).16,(float).06,(float).26,(float).12,(float).29,(
	    float).04,(float).01,(float).02,(float)0.,(float).01,(float).08,(
	    float).65,(float).56,(float).45,(float).43,(float).42,(float).42,(
	    float).09,(float)0.,(float).02,(float)0.,(float)0.,(float).34,(
	    float).67,(float).73,(float).72,(float).1,(float).05,(float).04,(
	    float)0.,(float).01,(float)0.,(float)0.,(float)0.,(float)0.,(
	    float).18,(float).39,(float).15,(float).61,(float).37,(float).51,(
	    float).06,(float).01,(float).02,(float).01,(float).01,(float).18,(
	    float).72,(float).63,(float).8,(float).77,(float).66,(float).7,(
	    float).19,(float)0.,(float).02,(float).02,(float).02,(float).41,(
	    float).68,(float).88,(float).85,(float).24,(float).11,(float).08,(
	    float)0.,(float).01,(float)0.,(float)0.,(float)0.,(float)0.,(
	    float).28,(float).51,(float).29,(float).75,(float).48,(float).57,(
	    float).11,(float)0.,(float).02,(float).01,(float).01,(float).19,(
	    float).77,(float).77,(float).88,(float).89,(float).81,(float).74,(
	    float).21,(float).02,(float).02,(float).02,(float).02,(float).42,(
	    float).71,(float).93,(float).95,(float).49,(float).3,(float).19,(
	    float)0.,(float)0.,(float)0.,(float)0.,(float).01,(float).06,(
	    float).38,(float).64,(float).48,(float).86,(float).6,(float).62,(
	    float).12,(float)0.,(float).02,(float).01,(float)0.,(float).18,(
	    float).81,(float).84,(float).94,(float).88,(float).79,(float).7,(
	    float).26,(float).03,(float).02,(float).02,(float).02,(float).36,(
	    float).61,(float).98,(float).93,(float).6,(float).46,(float).31,(
	    float).03,(float)0.,(float).01,(float)0.,(float)0.,(float).09,(
	    float).5,(float).71,(float).58,(float).9,(float).65,(float).66,(
	    float).1,(float)0.,(float).02,(float).01,(float).02,(float).15,(
	    float).69,(float).8,(float).86,(float).84,(float).75,(float).64,(
	    float).09,(float).03,(float)0.,(float)0.,(float).04,(float).26,(
	    float).54,(float).78,(float).92,(float).62,(float).59,(float).44,(
	    float).01,(float)0.,(float).01,(float)0.,(float)0.,(float).13,(
	    float).52,(float).77,(float).63,(float).84,(float).67,(float).63,(
	    float).11,(float)0.,(float)0.,(float).03,(float).03,(float).18,(
	    float).65,(float).75,(float).84,(float).81,(float).63,(float).47,(
	    float).06,(float).02,(float)0.,(float)0.,(float).05,(float).14,(
	    float).49,(float).76,(float).91,(float).58,(float).63,(float).47,(
	    float).09,(float)0.,(float).07,(float).01,(float).04,(float).15,(
	    float).48,(float).68,(float).61,(float).79,(float).63,(float).55,(
	    float).12,(float).01,(float).01,(float).02,(float).05,(float).13,(
	    float).57,(float).51,(float).63,(float).72,(float).54,(float).43,(
	    float).11,(float).02,(float)0.,(float)0.,(float).09,(float).16,(
	    float).39,(float).59,(float).72,(float).46,(float).55,(float).39,(
	    float).07,(float).01,(float).03,(float).03,(float).06,(float).15,(
	    float).37,(float).51,(float).5,(float).61,(float).43,(float).38,(
	    float).11,(float).01,(float).03,(float).02,(float).03,(float).1,(
	    float).38,(float).38,(float).6,(float).58,(float).42,(float).38,(
	    float).15,(float).02,(float)0.,(float)0.,(float).11,(float).13,(
	    float).24,(float).41,(float).51,(float).36,(float).36,(float).21,(
	    float).04,(float).04,(float).03,(float).06,(float).05,(float).06,(
	    float).26,(float).39,(float).43,(float).43,(float).31,(float).24,(
	    float).09,(float).02,(float)0.,(float).02,(float).02,(float).06,(
	    float).24,(float).24,(float).4,(float).53,(float).19,(float).28,(
	    float).13,(float).02,(float).02,(float).02,(float).09,(float).13,(
	    float).17,(float).24,(float).4,(float).32,(float).27,(float).17,(
	    float).03,(float).04,(float).02,(float).04,(float).03,(float).06,(
	    float).13,(float).34,(float).36,(float).42,(float).31,(float).2,(
	    float).09,(float).03,(float)0.,(float).02,(float).01,(float).07,(
	    float).19,(float).24,(float).32,(float).44,(float).1,(float).23,(
	    float).13,(float).03,(float).02,(float)0.,(float).09,(float).12,(
	    float).17,(float).21,(float).33,(float).32,(float).23,(float).16,(
	    float)0.,(float).02,(float).04,(float).03,(float).03,(float).06,(
	    float).15,(float).29,(float).34,(float).36,(float).26,(float).28,(
	    float).07,(float).01,(float)0.,(float).01,(float).02,(float).04,(
	    float).19,(float).17,(float).27,(float).34,(float).14,(float).26,(
	    float).09,(float).03,(float).02,(float)0.,(float).06,(float).13,(
	    float).09,(float).16,(float).22,(float).29,(float).21,(float).15,(
	    float)0.,(float).02,(float).02,(float).02,(float).03,(float).11,(
	    float).16,(float).26,(float).28,(float).29,(float).22,(float).27,(
	    float).05,(float).01,(float)0.,(float).01,(float).01,(float).02,(
	    float).14,(float).09,(float).19,(float).25,(float).19,(float).25,(
	    float).07,(float).02,(float).02,(float)0.,(float)0.,(float).09,(
	    float).07,(float).12,(float).15,(float).23,(float).2,(float).16,(
	    float)0.,(float).03,(float).04,(float)0.,(float)0.,(float).08,(
	    float).09,(float).21,(float).18,(float).22,(float).21,(float).19,(
	    float).02,(float).02,(float)0.,(float).01,(float).03,(float).04,(
	    float).08,(float).06,(float).14,(float).2,(float).12,(float).23,(
	    float).02,(float)0.,(float).02,(float)0.,(float)0.,(float).05,(
	    float).05,(float).09,(float).11,(float).14,(float).16,(float).13,(
	    float)0.,(float).03,(float).04,(float)0.,(float)0.,(float).05,(
	    float).05,(float).04,(float).09,(float).09,(float).13,(float).16,(
	    float).03,(float).01,(float)0.,(float).01,(float).03,(float).01,(
	    float).03,(float).04,(float).1,(float).14,(float).09,(float).17,(
	    float).02,(float).02,(float)0.,(float)0.,(float).02,(float).04,(
	    float).04,(float).03,(float).07,(float)0.,(float).11,(float).09,(
	    float)0.,(float).02,(float)0.,(float)0.,(float)0.,(float).01,(
	    float)0.,(float).02,(float).02,(float).02,(float).06,(float).11,(
	    float)0.,(float)0.,(float)0.,(float)0.,(float).01,(float)0.,(
	    float)0.,(float).01,(float).02,(float).06,(float).09,(float).13,(
	    float)0.,(float).02,(float)0.,(float).03,(float).02,(float).03,(
	    float).01,(float).02,(float).01 };

    static real sosf[2304]	/* was [2][32][3][12] */, osft, bspl4;
    static integer i__, j, k, l, m;
    static real param[3];
    extern doublereal bspl2f_(integer *, real *), bspl2l_(integer *, real *), 
	    bspl2s_(integer *, real *), bspl4t_(integer *, real *);
    static integer kc, il, kk, jl, ml, it, iii;
    static real slt;

/* ********************************************************************* */

/*       SUBROUTINE CALCULATES PERCENTAGE OF SPREAD F OCCURRENCE OVER */
/*       BRAZILIAN SECTOR AS DESCRIBED IN: */
/*       ABDU ET AL., Advances in Space Research, 31(3), */
/*       703-716, 2003 */

/*    INPUT: */
/*         IDOY: DAY OF YEAR (1 TO 365/366) */
/*         IDIY: DAYS IN YEAR (365 OR 366) */
/*         F107: F10.7 cm SOLAR FLUX (DAILY VALUE) */
/*         GEOLAT: BRAZILIAN GEOGRAPHIC LATITUDE BETWEEN -4 AND -22.5 */

/*    OUTPUT: */
/*         OSFBR(25): PERCENTAGE OF SPREAD F OCCURRENCE FOR 25 TIME */
/*                    STEPS FROM LT=18 TO LT=7 ON THE NEXT DAY IN */
/*                    STEPS OF 0.5 HOURS. */

/* ********************************************************************* */

    /* Parameter adjustments */
    --osfbr;

    /* Function Body */


    param[0] = (real) (*idoy);
    param[1] = *f107;
    param[2] = *geolat;
    mflux_1.n = *idiy - 365;

    if (param[0] <= (float)31.) {
	mflux_1.kf = 1;
    }
    if (param[0] > (float)31. && param[0] <= (real) (mflux_1.n + 59)) {
	mflux_1.kf = 2;
    }
    if (param[0] > (real) (mflux_1.n + 59) && param[0] <= (real) (mflux_1.n + 
	    90)) {
	mflux_1.kf = 3;
    }
    if (param[0] > (real) (mflux_1.n + 90) && param[0] <= (real) (mflux_1.n + 
	    120)) {
	mflux_1.kf = 4;
    }
    if (param[0] > (real) (mflux_1.n + 120) && param[0] <= (real) (mflux_1.n 
	    + 151)) {
	mflux_1.kf = 5;
    }
    if (param[0] > (real) (mflux_1.n + 151) && param[0] <= (real) (mflux_1.n 
	    + 181)) {
	mflux_1.kf = 6;
    }
    if (param[0] > (real) (mflux_1.n + 181) && param[0] <= (real) (mflux_1.n 
	    + 212)) {
	mflux_1.kf = 7;
    }
    if (param[0] > (real) (mflux_1.n + 212) && param[0] <= (real) (mflux_1.n 
	    + 243)) {
	mflux_1.kf = 8;
    }
    if (param[0] > (real) (mflux_1.n + 243) && param[0] <= (real) (mflux_1.n 
	    + 273)) {
	mflux_1.kf = 9;
    }
    if (param[0] > (real) (mflux_1.n + 273) && param[0] <= (real) (mflux_1.n 
	    + 304)) {
	mflux_1.kf = 10;
    }
    if (param[0] > (real) (mflux_1.n + 304) && param[0] <= (real) (mflux_1.n 
	    + 334)) {
	mflux_1.kf = 11;
    }
    if (param[0] > (real) (mflux_1.n + 334) && param[0] <= (real) (mflux_1.n 
	    + 365)) {
	mflux_1.kf = 12;
    }

    for (i__ = 1; i__ <= 32; ++i__) {
	for (j = 1; j <= 3; ++j) {
	    for (k = 1; k <= 12; ++k) {
		sosf[(i__ + (j + k * 3 << 5) << 1) - 258] = (float)0.;
		sosf[(i__ + (j + k * 3 << 5) << 1) - 257] = (float)0.;
	    }
	}
    }

    kc = 0;
    for (i__ = 5; i__ <= 23; ++i__) {
	for (j = 1; j <= 3; ++j) {
	    for (k = 1; k <= 12; ++k) {
		++kc;
		sosf[(i__ + (j + k * 3 << 5) << 1) - 258] = coef_sfa__[kc - 1]
			;
		sosf[(i__ + (j + k * 3 << 5) << 1) - 257] = coef_sfb__[kc - 1]
			;
	    }
	}
    }
    kk = 0;
    for (it = 1600; it <= 3200; it += 50) {
	slt = it / (float)100.;
	osft = (float)0.;
	for (i__ = 1; i__ <= 23; ++i__) {
	    il = i__ + 3;
	    if (il > 23) {
		il += -23;
	    }
	    for (j = 1; j <= 12; ++j) {
		jl = j + 2;
		if (jl > 12) {
		    jl += -12;
		}
		for (m = 1; m <= 3; ++m) {
		    ml = m + 1;
		    if (ml > 3) {
			ml += -3;
		    }
		    for (l = 1; l <= 2; ++l) {
			bspl4 = bspl4t_(&i__, &slt) * bspl2s_(&j, param) * 
				bspl2l_(&l, &param[2]) * bspl2f_(&m, &param[1]
				);
			osft += bspl4 * sosf[l + (il + (ml + jl * 3 << 5) << 
				1) - 259];
		    }
		}
	    }
	}
	if (slt > (float)17.98 && slt < (float)30.01) {
	    ++kk;
	    osfbr[kk] = osft;
	}
    }


    for (iii = 1; iii <= 25; ++iii) {
	if (osfbr[iii] > (float)1.) {
	    osfbr[iii] = (float)1.;
	}
	if (osfbr[iii] < (float)0.) {
	    osfbr[iii] = (float)0.;
	}
    }
    return 0;
} /* spreadf_brazil__ */


/* ********************************************************************* */
doublereal bspl4t_(integer *i__, real *t1)
{
    /* Initialized data */

    static real tt[79] = { (float)16.,(float)16.5,(float)17.,(float)17.5,(
	    float)18.,(float)18.5,(float)19.,(float)19.5,(float)20.,(float)
	    20.5,(float)21.,(float)22.,(float)23.,(float)24.,(float)25.,(
	    float)26.,(float)27.,(float)27.5,(float)28.,(float)28.5,(float)
	    29.,(float)29.5,(float)30.,(float)30.5,(float)31.,(float)32.,(
	    float)40.,(float)40.5,(float)41.,(float)41.5,(float)42.,(float)
	    42.5,(float)43.,(float)43.5,(float)44.,(float)44.5,(float)45.,(
	    float)46.,(float)47.,(float)48.,(float)49.,(float)50.,(float)51.,(
	    float)51.5,(float)52.,(float)52.5,(float)53.,(float)53.5,(float)
	    54.,(float)54.5,(float)55.,(float)56.,(float)64.,(float)64.5,(
	    float)65.,(float)65.5,(float)66.,(float)66.5,(float)67.,(float)
	    67.5,(float)68.,(float)68.5,(float)69.,(float)70.,(float)71.,(
	    float)72.,(float)73.,(float)74.,(float)75.,(float)75.5,(float)76.,
	    (float)76.5,(float)77.,(float)77.5,(float)78.,(float)78.5,(float)
	    79.,(float)80.,(float)88. };

    /* System generated locals */
    integer i__1;
    real ret_val;

    /* Local variables */
    static real b[900]	/* was [30][30] */;
    static integer j, k;
    static real t;

/* ********************************************************************* */


    t = *t1;
    if (*i__ >= 0 && t < tt[*i__]) {
	t += (float)24.;
    }
    i__1 = *i__ + 3;
    for (j = *i__; j <= i__1; ++j) {
	if (t >= tt[j] && t < tt[j + 1]) {
	    b[j - 1] = (float)1.;
	} else {
	    b[j - 1] = (float)0.;
	}
    }
    for (j = 2; j <= 4; ++j) {
	i__1 = *i__ + 4 - j;
	for (k = *i__; k <= i__1; ++k) {
	    b[k + j * 30 - 31] = (t - tt[k]) / (tt[k + j - 1] - tt[k]) * b[k 
		    + (j - 1) * 30 - 31];
	    b[k + j * 30 - 31] += (tt[k + j] - t) / (tt[k + j] - tt[k + 1]) * 
		    b[k + 1 + (j - 1) * 30 - 31];
	}
    }

    ret_val = b[*i__ + 89];

    return ret_val;

} /* bspl4t_ */


/* ***************************************************************** */
doublereal bspl2s_(integer *i__, real *t1)
{
    /* Initialized data */

    static real ts[37] = { (float)15.,(float)46.,(float)74.,(float)105.,(
	    float)135.,(float)166.,(float)196.,(float)227.,(float)258.,(float)
	    288.,(float)319.,(float)349.,(float)380.,(float)411.,(float)439.,(
	    float)470.,(float)500.,(float)531.,(float)561.,(float)592.,(float)
	    623.,(float)653.,(float)684.,(float)714.,(float)745.,(float)776.,(
	    float)804.,(float)835.,(float)865.,(float)896.,(float)926.,(float)
	    957.,(float)988.,(float)1018.,(float)1049.,(float)1079.,(float)
	    1110. };

    /* System generated locals */
    integer i__1;
    real ret_val;

    /* Local variables */
    static real b[900]	/* was [30][30] */;
    static integer j, k;
    static real t;

/* ***************************************************************** */


    t = *t1;
    if (*i__ >= 0 && t < ts[*i__]) {
	t += (float)365.;
    }
    i__1 = *i__ + 1;
    for (j = *i__; j <= i__1; ++j) {
	if (t >= ts[j] && t < ts[j + 1]) {
	    b[j - 1] = (float)1.;
	} else {
	    b[j - 1] = (float)0.;
	}
    }

    for (j = 2; j <= 2; ++j) {
	i__1 = *i__ + 4 - j;
	for (k = *i__; k <= i__1; ++k) {
	    b[k + j * 30 - 31] = (t - ts[k]) / (ts[k + j - 1] - ts[k]) * b[k 
		    + (j - 1) * 30 - 31];
	    b[k + j * 30 - 31] += (ts[k + j] - t) / (ts[k + j] - ts[k + 1]) * 
		    b[k + 1 + (j - 1) * 30 - 31];
	}
    }

    ret_val = b[*i__ + 29];
    return ret_val;
} /* bspl2s_ */


/* ****************************************************************** */
doublereal bspl2l_(integer *i__, real *t1)
{
    /* Initialized data */

    static real ts[7] = { (float)94.,(float)112.5,(float)454.,(float)472.5,(
	    float)814.,(float)832.5,(float)1174. };

    /* System generated locals */
    integer i__1;
    real ret_val;

    /* Local variables */
    static real b[900]	/* was [30][30] */;
    static integer j, k;
    static real t;

/* ****************************************************************** */


    t = *t1;
    if (*i__ >= 0 && t < ts[*i__]) {
	t += (float)360.;
    }
    i__1 = *i__ + 1;
    for (j = *i__; j <= i__1; ++j) {
	if (t >= ts[j] && t < ts[j + 1]) {
	    b[j - 1] = (float)1.;
	} else {
	    b[j - 1] = (float)0.;
	}
    }

    for (j = 2; j <= 2; ++j) {
	i__1 = *i__ + 2 - j;
	for (k = *i__; k <= i__1; ++k) {
	    b[k + j * 30 - 31] = (t - ts[k]) / (ts[k + j - 1] - ts[k]) * b[k 
		    + (j - 1) * 30 - 31];
	    b[k + j * 30 - 31] += (ts[k + j] - t) / (ts[k + j] - ts[k + 1]) * 
		    b[k + 1 + (j - 1) * 30 - 31];
	}
    }

    ret_val = b[*i__ + 29];

    return ret_val;

} /* bspl2l_ */


/* ************************************************************************ */
doublereal bspl2f_(integer *i__, real *t1)
{
    /* Initialized data */

    static integer ifnodes1[12] = { 78,77,75,79,80,77,78,80,76,81,78,78 };
    static integer ifnodes2[12] = { 144,140,139,142,139,146,142,139,150,151,
	    150,157 };
    static integer ifnodes3[12] = { 214,211,201,208,213,220,203,209,213,215,
	    236,221 };

    /* System generated locals */
    integer i__1;
    real ret_val;

    /* Local variables */
    static real b[900]	/* was [30][30] */;
    static integer j, k;
    static real t, ts[10];

/* ************************************************************************ */


    ts[0] = (real) ifnodes1[(0 + (0 + (mflux_1.kf - 1 << 2))) / 4];
    ts[1] = (real) ifnodes2[mflux_1.kf - 1];
    ts[2] = (real) ifnodes3[mflux_1.kf - 1];
    ts[3] = ts[1] + 367;
    ts[4] = ts[2] + 367;
    ts[5] = ts[3] + 367;
    ts[6] = ts[4] + 367;
    ts[7] = ts[5] + 367;
    ts[8] = ts[6] + 367;
    ts[9] = ts[7] + 367;

    t = *t1;
    if (*i__ >= 0 && t < ts[*i__]) {
	t += (float)367.;
    }
    i__1 = *i__ + 1;
    for (j = *i__; j <= i__1; ++j) {
	if (t >= ts[j] && t < ts[j + 1]) {
	    b[j - 1] = (float)1.;
	} else {
	    b[j - 1] = (float)0.;
	}
    }

    for (j = 2; j <= 2; ++j) {
	i__1 = *i__ + 2 - j;
	for (k = *i__; k <= i__1; ++k) {
	    b[k + j * 30 - 31] = (t - ts[k]) / (ts[k + j - 1] - ts[k]) * b[k 
		    + (j - 1) * 30 - 31];
	    b[k + j * 30 - 31] += (ts[k + j] - t) / (ts[k + j] - ts[k + 1]) * 
		    b[k + 1 + (j - 1) * 30 - 31];
	}
    }

    ret_val = b[*i__ + 29];
    return ret_val;
} /* bspl2f_ */

#ifdef __cplusplus
	}
#endif
