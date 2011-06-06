//$Id: Ionosphere.hpp 65 2010-06-21 00:10:28Z tdnguye2@NDC $
//------------------------------------------------------------------------------
//                         Ionosphere Model
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Tuan Dang Nguyen
// Created: 2010/06/21
//
/**
 * IRI 2007 ionosphere media correction model.
 */
//------------------------------------------------------------------------------
#ifndef Ionosphere_hpp
#define Ionosphere_hpp

#include "MediaCorrection.hpp"
#include "gmatdefs.hpp"
#include "Rvector3.hpp"

#include "f2c.h"

//#ifdef __linux__
//
//#ifndef integer
//typedef int integer;
//#endif
//
//#ifndef logical
//typedef int logical;
//#endif
//
//#ifndef ftnlen
//typedef int ftnlen;
//#endif
//
//#else
typedef long int integer;
typedef long int logical;
typedef long int ftnlen;
//#endif

typedef float real;
typedef double doublereal;

typedef doublereal (*D_fp)(...), (*E_fp)(...);

class Ionosphere: public MediaCorrection
{
public:
	Ionosphere(const std::string &nomme);
	virtual ~Ionosphere();
   Ionosphere(const Ionosphere& ions);
   Ionosphere& operator=(const Ionosphere& ions);
   virtual GmatBase*    Clone() const;

	bool SetWaveLength(Real lambda);
	bool SetTime(GmatEpoch ep);
	bool SetStationPosition(Rvector3 p);
	bool SetSpacecraftPosition(Rvector3 p);
	bool SetEarthRadius(Real r);

	Real TEC();
	Real BendingAngle();				// specify the change of elevation angle
	virtual RealArray Correction();

private:
	float ElectronDensity(Rvector3 pos2, Rvector3 pos1);
//	float ElectronDensity1(Rvector3 pos2, Rvector3 pos1);

	Real waveLength;		// wave length of the signal
	GmatEpoch epoch;		// time
	Rvector3 stationLoc;		// station location
	Rvector3 spacecraftLoc;	// spacecraft location

	Integer yyyy;
	Integer mmdd;
	Real hours;

	Real earthRadius;

	// IRI_SUB:
	int iri_sub(logical *jf, integer *jmag, real *alati, real *
	along, integer *iyyyy, integer *mmdd, real *dhour, real *heibeg, real
	*heiend, real *heistp, real *outf, real *oarr, integer *ier);

	int iri_web(integer *jmag, logical *jf, real *alati, real *
	along, integer *iyyyy, integer *mmdd, integer *iut, real *dhour, real
	*height, real *h_tec_max__, integer *ivar, real *vbeg, real *vend,
	real *vstp, real *a, real *b, integer *ier);



	// IRI_TEC:
	int irit13(real *alati, real *along, integer *jmag, logical
		*jf, integer *iy, integer *md, real *hour, real *hbeg, real *hend,
		real *tec, real *tecb, real *tect, integer *ier);
	doublereal ioncorr(real *tec, real *f);
	int iri_tec(real *hstart, real *hend, integer *istep, real
		*tectot, real *tectop, real *tecbot);


	// IRI_FUN:
	int initialize_irifun();
	doublereal xe1(real *h);
	doublereal topq(real *h, real *no, real *hmax, real *ho);
	doublereal zero(real *delta);
	doublereal dxe1n(real *h);
	doublereal xe2(real *h);
	doublereal xe3_1(real *h);
	doublereal xe4_1(real *h);
	doublereal xe5(real *h);
	doublereal xe6(real *h);
	doublereal xe_1(real *h);
	int calne(integer *crd, real *invdip, real *fl, real *dimo,
		real *b0, real *dipl, real *mlt, real *alt, integer *ddd, real *f107,
		real *nne);
	int nelow(integer *crd, real *invdip, real *fl, real *dimo,
			real *b0, real *dipl, real *mlt, real *alt, integer *ddd, real *d,
			real *nne);
	int nehigh(integer *crd, real *invdip, real *fl, real *dimo,
		    real *b0, real *dipl, real *mlt, real *alt, integer *ddd, real *d,
		    real *nne);
	int elteik(integer *crd, integer *f107y, integer *seasy,
		real *invdip, real *fl, real *dimo, real *b0, real *dipl, real *mlt,
		real *alt, integer *ddd, real *f107, real *te, real *sigte);
	int spharm_ik(real *c, integer *l, integer *m, real *colat, real *az);
	int teba(real *dipl, real *slt, integer *ns, real *te);
	int spharm(real *c, integer *l, integer *m, real *colat, real *az);
	doublereal elte(real *h);
	doublereal tede(real *h, real *den, real *cov);
	doublereal ti(real *h);
	doublereal teder(real *h);
	doublereal tn(real *h, real *tinf, real *tlbd, real *s);
	doublereal dtndh(real *h, real *tinf, real *tlbd, real *s);
	doublereal rpid(real *h, real *h0, real *n0, integer *m, real *st,
			integer *id, real *xs);
	int rdhhe(real *h, real *hb, real *rdoh, real *rdo2h,
		real *rno, real *pehe, real *rdh, real *rdhe);
	doublereal rdno(real *h, real *hb, real *rdo2h, real *rdoh, real *rno);
	int koefp1(real *pg1o);
	int koefp2(real *pg2o);
	int koefp3(real *pg3o);
	int sufe(real *field, real *rfe, integer *m, real *fe);
	int ionco2(real *hei, real *xhi, integer *it, real *f, real
		*r1, real *r2, real *r3, real *r4);
	int aprok(integer *j1m, integer *j2m, real *h1, real *h2,
		real *r1m, real *r2m, real *rk1m, real *rk2m, real *hei, real *xhi,
		real *r1, real *r2);
	int ioncomp(real *xy, integer *id, integer *ismo, real *xm,
		real *hx, real *zd, real *fd, real *fp, real *fs, real *dion);
	int ionco1(real *h, real *zd, real *fd, real *fs, real *t, real *cn);
	int calion(integer *crd, real *invdip, real *fl, real *dimo,
		 real *b0, real *dipl, real *mlt, real *alt, integer *ddd, real *f107,
		 real *no, real *nh, real *nhe, real *nn);
	int ionlow(integer *crd, real *invdip, real *fl, real *dimo,
		 real *b0, real *dipl, real *mlt, real *alt, integer *ddd, real *d,
		integer *ion, real *nion);
	int ionhigh(integer *crd, real *invdip, real *fl, real *dimo, real *b0,
		real *dipl, real *mlt, real *alt, integer *ddd, real *d, integer *ion,
		real *nion);
	doublereal invdpc(real *fl, real *dimo, real *b0, real *dipl, real *dtor);
	doublereal fout(real *xmodip, real *xlati, real *xlongi, real *ut, real *ff0);
	doublereal xmout(real *xmodip, real *xlati, real *xlongi, real *ut, real *xm0);
	doublereal hmf2ed(real *xmagbr, real *r, real *x, real *xm3);
	doublereal xm3000hm(real *xmagbr, real *r, real *x, real *hmf2);
	doublereal fof1ed(real *ylati, real *r, real *chi);
	doublereal f1_c1(real *xmodip, real *hour, real *suxnon, real *saxnon);
	int f1_prob(real *sza, real *glat, real *rz12, real *f1prob, real *f1probl);
	doublereal foeedi(real *cov, real *xhi, real *xhim, real *xlati);
	doublereal xmded(real *xhi, real *r, real *yw);
	doublereal gamma1(real *smodip, real *slat, real *slong, real *hour,
		integer *iharm, integer *nq, integer *k1, integer *m, integer *mm,
		integer *m3, real *sfe);
	doublereal b0_98(real *hour, real *sax, real *sux, integer *nseasn, real *r,
			real *zlo, real *zmodip);
	int tal(real *shabr, real *sdelta, real *shbr, real *sdtdh0,
		 logical *aus6, real *spt);
	int valgul(real *xhi, real *hvb, real *vwu, real *vwa, real *vdp);
	int ggm(integer *art, real *long__, real *lati, real *mlong, real *mlat);
	int fieldg(real *dlat, real *dlong, real *alt, real *x,
		real *y, real *z, real *f, real *dip, real *dec, real *smodip);
	int regfa1(real *x11, real *x22, real *fx11, real *fx22,
		real *eps, real *fw, E_fp f, logical *schalt, real *x);
	int soco(integer *ld, real *t, real *flat, real *elon, real *height,
			real *declin, real *zenith, real *sunrse, real *sunset);
	doublereal hpol(real *hour, real *tw, real *xnw, real *sa, real *su,
		real *dsa, real *dsu);
	int moda(integer *in, integer *iyear, integer *month,
		integer *iday, integer *idoy, integer *nrdaymo);
	int ut_lt(integer *mode, real *ut, real *slt, real *glong,
		integer *iyyy, integer *ddd);
	doublereal rlay(real *x, real *xm, real *sc, real *hx);
	doublereal d1lay(real *x, real *xm, real *sc, real *hx);
	doublereal d2lay(real *x, real *xm, real *sc, real *hx);
	doublereal eptr(real *x, real *sc, real *hx);
	doublereal epst(real *x, real *sc, real *hx);
	doublereal epstep(real *y2, real *y1, real *sc, real *hx, real *x);
	doublereal epla(real *x, real *sc, real *hx);
	doublereal xe2to5(real *h, real *hmf2, integer *nl, real *hx, real *sc,
		real *amp);
	doublereal xen(real *h, real *hmf2, real *xnmf2, real *hme, integer *nl,
		real *hx, real *sc, real *amp);
	int rogul(integer *iday, real *xhi, real *sx, real *gro);
	int lnglsn(integer *n, real *a, real *b, logical *aus);
	int lsknm(integer *n, integer *m, integer *m0, integer *m1,
		real *hm, real *sc, real *hx, real *w, real *x, real *y, real *var,
		logical *sing);
	int inilay(logical *night, logical *f1reg, real *xnmf2,
		real *xnmf1, real *xnme, real *vne, real *hmf2, real *hmf1, real *hme,
		real *hv1, real *hv2, real *hhalf, real *hxl, real *scl, real *amp,
		integer *iqual);
	int tcon(integer *yr, integer *mm, integer *day, integer *idn,
		real *rz, real *ig, real *rsn, integer *nmonth);
	int lstid(real *fi, integer *icez, real *r, real *ae,
		real *tm, real *sax, real *sux, real *ts70, real *df0f2, real *dhf2);
	int apf(integer *iyyyy, integer *imn, integer *id, real *hour,
		integer *iap);
	int apf_only(integer *iyyyy, integer *imn, integer *id,
		real *f107d, real *f107m);
	int conver(real *rga, real *rgo, real *rgma);
	int storm(integer *ap, real *rga, real *rgo, integer *coor,
		real *rgma, integer *ut, integer *doy, real *cf);
	int vdrift(real *xt, real *xl, real *param, real *y);
	doublereal bspl4_time(integer *i, real *x1);
	doublereal bspl4_long(integer *i, real *x1);
	int g(real *param, real *funct, real *x);
	int stormvd(integer *flag, integer *ip, real *ae, real *slt,
		real *promptvd, real *dynamovd, real *vd);
	doublereal bspl4_ptime(integer *i, real *x1);
	int spreadf_brazil(integer *idoy, integer *idiy, real *f107,
		real *geolat, real *osfbr);
	doublereal bspl4t(integer *i, real *t1);
	doublereal bspl2s(integer *i, real *t1);
	doublereal bspl2l(integer *i, real *t1);
	doublereal bspl2f(integer *i, real *t1);


	// IRI_DREG:
	int dregion(real *z, integer *it, real *f, real *vkp,
		real *f5sw, real *f6wa, real *elg);
	int f00(real *hgt, real *glat1, integer *iday, real *zang,
		real *f107t, real *edens, integer *ierror);


	//IGRF:
	int igrf_sub(real *xlat, real *xlong, real *year, real *height,
		real *xl, integer *icode, real *dipl, real *babs, integer *ier);
	int igrf_dip(real *xlat, real *xlong, real *year, real *height,
		real *dip, real *dipl, real *ymodip, integer *ier);
	int findb0(real *stps, real *bdel, logical *value, real *bequ, real *rr0);
	int shellg_0(int n, real *glat, real *glon, real *alt,
		real *dimo, real *fl, integer *icode, real *b0, real *v);
	int shellg(real *glat, real *glon, real *alt, real *dimo,
		real *fl, integer *icode, real *b0);
	int shellc(real *v, real *fl, real *b0);
	int stoer(real *p, real *bq, real *r);
	int feldg_0(int n, real *glat, real *glon, real *alt,
		real *bnorth, real *beast, real *bdown, real *babs, real *v, real *b);
	int feldg(real *glat, real *glon, real *alt, real *bnorth,
		real *beast, real *bdown, real *babs);
	int feldc(real *v, real *b);
	int feldi();
	int feldcof(real *year, real *dimo, integer *ier);
	int getshc(integer *iu, char *fspec, integer *nmax, real *erad,
		real *gh, integer *ier, ftnlen fspec_len);
	int intershc(real *date, real *dte1, integer *nmax1, real *gh1,
		real *dte2, integer *nmax2, real *gh2, integer *nmax, real *gh);
	int extrashc(real *date, real *dte1, integer *nmax1,
		real *gh1, integer *nmax2, real *gh2, integer *nmax, real *gh);
	int initize();
	int geodip(integer *iyr, real *sla, real *slo, real *dla,
		real *dlo, integer *j);
	int sphcar(real *r, real *teta, real *phi, real *x, real *y,
		real *z, integer *j);
	int geomag(real *xgeo, real *ygeo, real *zgeo, real *xmag,
		real *ymag, real *zmag, integer *j, integer *iyr);
	int recalc(integer *iyr, integer *iday, integer *ihour,
		integer *min, integer *isec);


	// CIRA:
	int cira86(integer *iday, real *sec, real *glat, real *glong,
		real *stl, real *f107a, real *tinf, real *tlb, real *sigma);

	static const Real NUM_OF_INTERVALS;
};
#endif //Ionosphere_hpp_

