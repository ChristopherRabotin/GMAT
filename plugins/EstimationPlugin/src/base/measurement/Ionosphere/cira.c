/* cira.f -- translated by f2c (version 20000704).
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
    real hr, dr;
} const1_;

#define const1_1 const1_

/* cira.for, version number can be found at the end of this comment. */
/* ----------------------------------------------------------------------- */

/*   Calculates neutral temperature parameters for IRI using the */
/*   MSIS-86/CIRA 1986 Neutral Thermosphere Model. The subroutines */
/*   GTS5, GLOBE5 and GLOBL5 developed by A.E. Hedin (2/26/87) were */
/*   modified for use in IRI --------- D. Bilitza -------- March 1991 */

/* Corrections */
/* 11/09/99 always calculated Legendre; 'if glat' and 'if stl' taken out */
/* 11/09/99 use UMR, dumr and humr from COMMON */
/* Version-mm/dd/yy------------------------------------------------- */
/* 2001.01 05/07/01 start of versioning */
/* 2002.01 28/10/02 replace TAB/6 blanks, enforce 72/line (D. Simpson) */
/* 2007.00 05/18/07 Release of IRI-2007 */
/* ----------------------------------------------------------------------- */

/* Subroutine */ int cira86_(integer *iday, real *sec, real *glat, real *
	glong, real *stl, real *f107a, real *tinf, real *tlb, real *sigma)
{
    /* Initialized data */

    static real xl = (float)1e3;
    static real tll = (float)1e3;

    /* Builtin functions */
    double sin(doublereal), cos(doublereal);

    /* Local variables */
    static real c__, s, ctloc, stloc, c2, c4, f1, f2, f3, g0, s2, t1, t2, t3, 
	    t5, t4, t7, t8, z1, z2, c2tloc, c3tloc, s2tloc, s3tloc, t11, t12, 
	    t14, t71, t72, t81, t82, sr, zz, cd9, dr2, cd11, dfa, cd14, cd32, 
	    cd18, cd39, plg[36]	/* was [9][4] */, t7814;

/* ----------------------------------------------------------------------- */

/*     INPUT: */
/*        IDAY - DAY OF YEAR */
/*        SEC - UT(SEC) */
/*        GLAT - GEODETIC LATITUDE(DEG) */
/*        GLONG - GEODETIC LONGITUDE(DEG) */
/*        STL - LOCAL APPARENT SOLAR TIME(HRS) */
/*        F107A - 3 MONTH AVERAGE OF F10.7 FLUX */

/*     OUTPUT: */
/*        TINF - EXOSPHERIC TEMPERATURE (K) */
/*        TLB - TEMPERATURE AT LOWER BOUNDARY (K) */
/*        SIGMA - SHAPE PARAMETER FOR TEMPERATURE PROFILE */

/* ----------------------------------------------------------------------- */
/*      data umr/1.74E-2/,hr/0.2618/,dr/1.74e-2 */
/* DR,DR2/1.72142E-2,0.0344284/, */
/* SR/7.2722E-5/, */
/* ,HR/.2618/ */
/* ,DGTR/1.74533E-2/ */
/*       dr = hr * 24. / 365. */

    dr2 = const1_1.dr * (float)2.;
    sr = const1_1.hr / (float)3600.;

/* CALCULATE LEGENDRE POLYNOMIALS */

    if (xl == *glat) {
	goto L15;
    }
    c__ = sin(*glat * const_1.umr);
    s = cos(*glat * const_1.umr);
    c2 = c__ * c__;
    c4 = c2 * c2;
    s2 = s * s;
    plg[1] = c__;
    plg[2] = (c2 * (float)3. - (float)1.) * (float).5;
    plg[3] = (c__ * (float)5. * c2 - c__ * (float)3.) * (float).5;
    plg[4] = (c4 * (float)35. - c2 * (float)30. + (float)3.) / (float)8.;
    plg[5] = (c2 * (float)63. * c2 * c__ - c2 * (float)70. * c__ + c__ * (
	    float)15.) / (float)8.;
    plg[10] = s;
    plg[11] = c__ * (float)3. * s;
    plg[12] = (c2 * (float)5. - (float)1.) * (float)1.5 * s;
    plg[13] = (c2 * (float)7. * c__ - c__ * (float)3.) * (float)2.5 * s;
    plg[14] = (c4 * (float)21. - c2 * (float)14. + (float)1.) * (float)1.875 *
	     s;
    plg[15] = (c__ * (float)11. * plg[14] - plg[13] * (float)6.) / (float)5.;
    plg[20] = s2 * (float)3.;
    plg[21] = s2 * (float)15. * c__;
    plg[22] = (c2 * (float)7. - (float)1.) * (float)7.5 * s2;
    plg[23] = c__ * (float)3. * plg[22] - plg[21] * (float)2.;
    plg[30] = s2 * (float)15. * s;
    plg[31] = s2 * (float)105. * s * c__;
    plg[32] = (c__ * (float)9. * plg[31] - plg[30] * (float)7.) / (float)2.;
    plg[33] = (c__ * (float)11. * plg[32] - plg[31] * (float)8.) / (float)3.;
    xl = *glat;
L15:
    if (tll == *stl) {
	goto L16;
    }
    stloc = sin(const1_1.hr * *stl);
    ctloc = cos(const1_1.hr * *stl);
    s2tloc = sin(const1_1.hr * (float)2. * *stl);
    c2tloc = cos(const1_1.hr * (float)2. * *stl);
    s3tloc = sin(const1_1.hr * (float)3. * *stl);
    c3tloc = cos(const1_1.hr * (float)3. * *stl);
    tll = *stl;
L16:

    dfa = *f107a - (float)150.;

/* EXOSPHERIC TEMPERATURE */

/*         F10.7 EFFECT */
    t1 = ((float).00311701 - dfa * (float)6.4111e-6) * dfa;
    f1 = dfa * (float).00426385 + (float)1.;
    f2 = dfa * (float).00511819 + (float)1.;
    f3 = dfa * (float).00292246 + (float)1.;
/*        TIME INDEPENDENT */
    t2 = plg[2] * (float).0385528 + plg[4] * (float).00303445;
/*        SYMMETRICAL ANNUAL AND SEMIANNUAL */
    cd14 = cos(const1_1.dr * (*iday + (float)8.45398));
    cd18 = cos(dr2 * (*iday - (float)125.818));
    cd32 = cos(const1_1.dr * (*iday - (float)30.015));
    cd39 = cos(dr2 * (*iday - (float)2.75905));
    t3 = cd32 * (float).00805486 + cd18 * (float).014237;
/*        ASYMMETRICAL ANNUAL AND SEMIANNUAL */
    t5 = f1 * (plg[1] * (float)-.127371 - plg[3] * (float).0302449) * cd14 - 
	    plg[1] * (float).0192645 * cd39;
/*        DIURNAL */
    t71 = plg[11] * (float).0123512 * cd14;
    t72 = plg[11] * (float)-.00526277 * cd14;
    t7 = (plg[10] * (float)-.105531 - plg[12] * (float).00607134 + t71) * 
	    ctloc + (plg[10] * (float)-.115622 + plg[12] * (float).0020224 + 
	    t72) * stloc;
/*        SEMIDIURNAL */
    t81 = plg[21] * (float).00386578 * cd14;
    t82 = plg[21] * (float).00389146 * cd14;
    t8 = (plg[20] * (float)-5.16278e-4 - plg[22] * (float).00117388 + t81) * 
	    c2tloc + (plg[20] * (float).00990156 - plg[22] * (float)
	    3.54589e-4 + t82) * s2tloc;
/*        TERDIURNAL */
    z1 = plg[31] * cd14;
    z2 = plg[33] * cd14;
    t14 = (plg[30] * (float).00147284 - z1 * (float)1.73933e-4 + z2 * (float)
	    3.65016e-5) * s3tloc + (plg[30] * (float)3.41345e-4 - z1 * (float)
	    1.53218e-4 + z2 * (float)1.15102e-4) * c3tloc;
    t7814 = f2 * (t7 + t8 + t14);
/*        LONGITUDINAL */
    t11 = f3 * ((plg[11] * (float).00562606 + plg[13] * (float).00594053 + 
	    plg[15] * (float).00109358 - plg[10] * (float).00301801 - plg[12] 
	    * (float).00423564 - plg[14] * (float).00248289 + (plg[10] * (
	    float).00189689 + plg[12] * (float).00415654) * cd14) * cos(
	    const_1.umr * *glong) + (plg[11] * (float)-.011654 - plg[13] * (
	    float).00449173 - plg[15] * (float)3.53189e-4 + plg[10] * (float)
	    9.19286e-4 + plg[12] * (float).00216372 + plg[14] * (float)
	    8.63968e-4 + (plg[10] * (float).0118068 + plg[12] * (float)
	    .0033119) * cd14) * sin(const_1.umr * *glong));
/*        UT AND MIXED UT,LONGITUDE */
    t12 = ((float)1. - plg[1] * (float).565411) * cos(sr * (*sec - (float)
	    31137.)) * (plg[1] * (float)-.013341 - plg[3] * (float).0243409 - 
	    plg[5] * (float).0135688) + (plg[21] * (float)8.45583e-4 + plg[23]
	     * (float)5.38706e-4) * cos(sr * (*sec - (float)247.956) + 
	    const_1.umr * (float)2. * *glong);
/*  Exospheric temperature TINF/K  [Eq. A7] */
    *tinf = (t1 + (float)1. + t2 + t3 + t5 + t7814 + t11 + t12) * (float)
	    1041.3 * (float).99604;

/* TEMPERATURE DERIVATIVE AT LOWER BOUNDARY */

/*         F10.7 EFFECT */
    t1 = dfa * (float).00252317;
/*        TIME INDEPENDENT */
    t2 = plg[2] * (float)-.0467542 + plg[4] * (float).12026;
/*        ASYMMETRICAL ANNUAL */
    cd14 = cos(const1_1.dr * (*iday + (float)8.45398));
    t5 = plg[1] * (float)-.13324 * cd14;
/*        SEMIDIURNAL */
    zz = plg[21] * cd14;
    t81 = zz * (float)-.00973404;
    t82 = zz * (float)-7.18482e-4;
    t8 = (plg[20] * (float).0191357 + plg[22] * (float).00787683 + t81) * 
	    c2tloc + (plg[20] * (float).00125429 - plg[22] * (float).00233698 
	    + t82) * s2tloc;
/*  dTn/dh at lower boundary  [Eq. A6] */
    g0 = (t1 + (float)1. + t2 + t5 + t8) * (float)16.6728 * (float).951363;

/* NEUTRAL TEMPERATURE AT LOWER BOUNDARY 120KM */

    cd9 = cos(dr2 * (*iday - (float)89.382));
    cd11 = cos(const1_1.dr * (*iday + (float)8.45398));
    t1 = dfa * (float)5.68478e-4;
    t4 = cd9 * (float).0107674;
    t5 = plg[1] * (float)-.0192414 * cd11;
    t7 = plg[10] * (float)-.02002 * ctloc - plg[10] * (float).00195833 * 
	    stloc;
    t8 = (plg[20] * (float)-.00938391 - plg[22] * (float).00260147 + plg[23] *
	     (float)5.11651e-5 * cd11) * c2tloc + (plg[20] * (float).013148 - 
	    plg[22] * (float)8.08556e-4 + plg[23] * (float).00255717 * cd11) *
	     s2tloc;
/*  Tn at lower boundary 120km   [Eq. A8] */
    *tlb = (t1 + (float)1. + t4 + t5 + t7 + t8) * (float)386. * (float)
	    .976619;
/*  Sigma      [Eq. A5] */
    *sigma = g0 / (*tinf - *tlb);
    return 0;
} /* cira86_ */

#ifdef __cplusplus
	}
#endif
