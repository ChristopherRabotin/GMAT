/* iritec.f -- translated by f2c (version 20000704).
   You must link the resulting object file with the libraries:
	-lf2c -lm   (in that order)
*/

#ifdef __cplusplus
extern "C" {
#endif
#include "f2c.h"

/* Common Block Declarations */

struct {
    real hmf2, xnmf2, hmf1;
    logical f1reg;
} block1_;

#define block1_1 block1_

/* Table of constant values */

static integer c__2 = 2;

/* iritec.for, version number can be found at the end of this comment. */
/* ----------------------------------------------------------------------- */

/* contains IRIT13, IONCORR, IRI_TEC subroutines to computed the */
/* total ionospheric electron content (TEC) */

/* ----------------------------------------------------------------------- */
/* Corrections */

/*  3/25/96 jmag in IRIT13 as input */
/*  8/31/97 hu=hr(i+1) i=6 out of bounds condition corrected */
/*  9/16/98 JF(17) added to input parameters; OUTF(11,50->100) */
/*  ?/ ?/99 Ne(h) restricted to values smaller than NmF2 for topside */
/* 11/15/99 JF(20) instead of JF(17) */
/* 10/16/00 if hr(i) gt hend then hr(i)=hend */
/* 12/14/00 jf(30),outf(20,100),oarr(50) */

/* Version-mm/dd/yy-Description (person reporting correction) */
/* 2000.01 05/07/01 current version */
/* 2000.02 10/28/02 replace TAB/6 blanks, enforce 72/line (D. Simpson) */
/* 2000.03 11/08/02 common block1 in iri_tec with F1reg */
/* 2007.00 05/18/07 Release of IRI-2007 */
/* 2007.01 10/31/08 outf(.,100) -> outf(.,500) */
/* ----------------------------------------------------------------------- */


/* Subroutine */ int irit13_(real *alati, real *along, integer *jmag, logical 
	*jf, integer *iy, integer *md, real *hour, real *hbeg, real *hend, 
	real *tec, real *tecb, real *tect, integer *error)
{
    static real aend, oarr[50], astp, outf[10000]	/* was [20][500] */;
    extern /* Subroutine */ int iri_tec__(real *, real *, integer *, real *, 
	    real *, real *), iri_sub__(logical *, integer *, real *, real *, 
	    integer *, integer *, real *, real *, real *, real *, real *, 
	    real *, integer *);
    static real abeg;

/* ----------------------------------------------------------------------- */
/* Program for numerical integration of IRI-94 profiles from h=100km */
/* to h=alth. */

/*  INPUT:  ALATI,ALONG  LATITUDE NORTH AND LONGITUDE EAST IN DEGREES */
/*          jmag         =0 geographic   =1 geomagnetic coordinates */
/*          jf(1:30)     =.true./.false. flags; explained in IRISUB.FOR */
/*          iy,md        date as yyyy and mmdd (or -ddd) */
/*          hour         decimal hours LT (or UT+25) */
/*          hbeg,hend    upper and lower integration limits in km */

/*  OUTPUT: TEC          Total Electron Content in M-2 */
/*          tecb,tect    percentage of bottomside and topside content */
/* ----------------------------------------------------------------------- */

/*  select various options and choices for IRI-94 */

    /* Parameter adjustments */
    --jf;

    /* Function Body */
    *tec = (float)-111.;
    *tect = (float)-111.;
    *tecb = (float)-111.;

/* initialize IRI parameter in COMMON blocks */

    abeg = *hbeg;
    aend = *hend;
    astp = *hend - *hbeg;
    iri_sub__(&jf[1], jmag, alati, along, iy, md, hour, &abeg, &aend, &astp, 
	    outf, oarr, error);

/*  calculate total electron content (TEC) in m-2 using the */
/*  stepsize selection 2 (highest accuracy) */

    iri_tec__(hbeg, hend, &c__2, tec, tect, tecb);
    return 0;
} /* irit13_ */



doublereal ioncorr_(real *tec, real *f)
{
    /* System generated locals */
    real ret_val;

/* ----------------------------------------------------------------------- */
/* computes ionospheric correction IONCORR (in m) for given vertical */
/* ionospheric electron content TEC (in m-2) and frequency f (in Hz) */
/* ----------------------------------------------------------------------- */
    ret_val = *tec * (float)40.3 / (*f * *f);
    return ret_val;
} /* ioncorr_ */



/* Subroutine */ int iri_tec__(real *hstart, real *hend, integer *istep, real 
	*tectot, real *tectop, real *tecbot)
{
    /* Builtin functions */
    double exp(doublereal), log(doublereal);

    /* Local variables */
    static real delx, ss_t__;
    static logical expo;
    static real step[5], hei_2__, hei_3__, hei_4__, hei_5__;
    static integer num_step__;
    static real h__;
    static integer i__;
    static real xnorm, xntop;
    static integer ia;
    static real hh, hr[6], hu, hx, sumbot, x_2__, x_3__, x_4__, x_5__, sumtop,
	     del_hei__, hei_end__, yne, hss, xkk, hei_top__, top_end__, 
	    ed_2__, ed_3__, ed_4__, ed_5__, yyy;
    extern doublereal xe_1__(real *);
    static real zzz, ss_2__, ss_3__, ss_4__;
    static integer numstep;

/* ----------------------------------------------------------------------- */
/* subroutine to compute the total ionospheric content */
/* INPUT: */
/*   hstart  altitude (in km) where integration should start */
/*   hend    altitude (in km) where integration should end */
/*   istep   =0 [fast, but higher uncertainty <5%] */
/*           =1 [standard, recommended] */
/*           =2 [stepsize of 1 km; best TEC, longest CPU time] */
/* OUTPUT: */
/*   tectot  total ionospheric content in tec-units (10^16 m^-2) */
/*   tectop  topside content (in %) */
/*   tecbot  bottomside content (in %) */

/* The different stepsizes for the numerical integration are */
/* defined as follows (h1=100km, h2=hmF2-10km, h3=hmF2+10km, */
/* h4=hmF2+150km, h5=hmF2+250km): */
/*       istep   h1-h2   h2-h3   h3-h4   h4-h5   h5-hend */
/*       0       2.0km   1.0km   2.5km   exponential approximation */
/*       1       2.0km   1.0km   2.5km   10.0km  30.0km */
/*       2       1.0km   0.5km   1.0km   1.0km   1.0km */

/* ----------------------------------------------------------------------- */
/* test */
    expo = FALSE_;
    numstep = 5;
    xnorm = block1_1.xnmf2 / (float)1e3;
    hr[0] = (float)100.;
    hr[1] = block1_1.hmf2 - (float)10.;
    hr[2] = block1_1.hmf2 + (float)10.;
    hr[3] = block1_1.hmf2 + (float)150.;
    hr[4] = block1_1.hmf2 + (float)250.;
    hr[5] = *hend;
    for (i__ = 2; i__ <= 6; ++i__) {
/* L2918: */
	if (hr[i__ - 1] > *hend) {
	    hr[i__ - 1] = *hend;
	}
    }
    if (*istep == 0) {
	step[0] = (float)2.;
	step[1] = (float)1.;
	step[2] = (float)2.5;
	step[3] = (float)5.;
	if (*hend > hr[4]) {
	    expo = TRUE_;
	}
    }
    if (*istep == 1) {
	step[0] = (float)2.;
	step[1] = (float)1.;
	step[2] = (float)2.5;
	step[3] = (float)10.;
	step[4] = (float)30.;
    }
    if (*istep == 2) {
	step[0] = (float)1.;
	step[1] = (float).5;
	step[2] = (float)1.;
	step[3] = (float)1.;
	step[4] = (float)1.;
    }
    sumtop = (float)0.;
    sumbot = (float)0.;

/* find the starting point for the integration */

    i__ = 0;
    ia = 1;
L3:
    ++i__;
    h__ = hr[i__ - 1];
    if (*hstart > h__) {
	hr[i__ - 1] = *hstart;
	ia = i__;
	goto L3;
    }

/* start the numerical integration */

    i__ = ia;
    h__ = hr[i__ - 1];
    hu = hr[i__];
    delx = step[i__ - 1];
L1:
    h__ += delx;
    hh = h__;
    if (h__ >= hu) {
	delx = hu - h__ + delx;
	hx = hu - delx / (float)2.;
	yne = xe_1__(&hx);
	if (hx > block1_1.hmf2 && yne > block1_1.xnmf2) {
	    yne = block1_1.xnmf2;
	}
	yyy = yne * delx / xnorm;
	++i__;
	if (i__ < 6) {
	    h__ = hr[i__ - 1];
	    hu = hr[i__];
	    delx = step[i__ - 1];
	}
    } else {
	hx = h__ - delx / (float)2.;
	yne = xe_1__(&hx);
	if (hx > block1_1.hmf2 && yne > block1_1.xnmf2) {
	    yne = block1_1.xnmf2;
	}
	yyy = yne * delx / xnorm;
    }
    if (hx <= block1_1.hmf2) {
	sumbot += yyy;
    } else {
	sumtop += yyy;
    }
    if (expo && hh >= hr[3]) {
	goto L5;
    }
    if (hh < *hend && i__ < 6) {
	goto L1;
    }
    zzz = sumtop + sumbot;
    *tectop = sumtop / zzz * (float)100.;
    *tecbot = sumbot / zzz * (float)100.;
    *tectot = zzz * block1_1.xnmf2;
    return 0;
L5:
    num_step__ = 3;
    hei_top__ = hr[3];
    hei_end__ = *hend;
    top_end__ = hei_end__ - hei_top__;
    del_hei__ = top_end__ / num_step__;
    xntop = xe_1__(&hei_end__) / block1_1.xnmf2;
    if (xntop > (float).9999) {
	ss_t__ = top_end__;
	goto L2345;
    }
    hei_2__ = hei_top__;
    hei_3__ = hei_2__ + del_hei__;
    hei_4__ = hei_3__ + del_hei__;
    hei_5__ = hei_end__;
    hss = top_end__ / (float)4.;
/*       hss = 360. */
    xkk = exp(-top_end__ / hss) - (float)1.;
    x_2__ = hei_2__;
    x_3__ = hei_top__ - hss * log(xkk * (hei_3__ - hei_top__) / top_end__ + (
	    float)1.);
    x_4__ = hei_top__ - hss * log(xkk * (hei_4__ - hei_top__) / top_end__ + (
	    float)1.);
    x_5__ = hei_end__;
    ed_2__ = xe_1__(&x_2__) / block1_1.xnmf2;
    if (ed_2__ > (float)1.) {
	ed_2__ = (float)1.;
    }
    ed_3__ = xe_1__(&x_3__) / block1_1.xnmf2;
    if (ed_3__ > (float)1.) {
	ed_3__ = (float)1.;
    }
    ed_4__ = xe_1__(&x_4__) / block1_1.xnmf2;
    if (ed_4__ > (float)1.) {
	ed_4__ = (float)1.;
    }
    ed_5__ = xntop;
    if (ed_3__ == ed_2__) {
	ss_2__ = ed_3__ * (x_3__ - x_2__);
    } else {
	ss_2__ = (ed_3__ - ed_2__) * (x_3__ - x_2__) / log(ed_3__ / ed_2__);
    }
    if (ed_4__ == ed_3__) {
	ss_3__ = ed_4__ * (x_4__ - x_3__);
    } else {
	ss_3__ = (ed_4__ - ed_3__) * (x_4__ - x_3__) / log(ed_4__ / ed_3__);
    }
    if (ed_5__ == ed_4__) {
	ss_4__ = ed_5__ * (x_5__ - x_4__);
    } else {
	ss_4__ = (ed_5__ - ed_4__) * (x_5__ - x_4__) / log(ed_5__ / ed_4__);
    }
    ss_t__ = ss_2__ + ss_3__ + ss_4__;
L2345:
    sumtop += ss_t__ * (float)1e3;
    zzz = sumtop + sumbot;
    *tectop = sumtop / zzz * (float)100.;
    *tecbot = sumbot / zzz * (float)100.;
    *tectot = zzz * block1_1.xnmf2;
    return 0;
} /* iri_tec__ */

#ifdef __cplusplus
	}
#endif
