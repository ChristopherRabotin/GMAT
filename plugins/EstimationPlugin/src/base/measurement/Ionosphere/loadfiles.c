/* loadfiles.f -- translated by f2c (version 20000704).
   You must link the resulting object file with the libraries:
	-lf2c -lm   (in that order)
*/

#ifdef __cplusplus
extern "C" {
#endif
#include "f2c.h"

/* Common Block Declarations */

struct {
    integer nmax_shc__[14];
    real erad_shc__[14], shcfiles[11200]	/* was [4][200][14] */;
} shc1_;

#define shc1_1 shc1_

struct shc2_1_ {
    char filmod[168];
};

#define shc2_1 (*(struct shc2_1_ *) &shc2_)

struct shc3_1_ {
    real dtemod[14];
};

#define shc3_1 (*(struct shc3_1_ *) &shc3_)

struct {
    integer iupd_igrz__, iupm_igrz__, iupy_igrz__, imst_igrz__, iyst_igrz__, 
	    imend_igrz__, iyend_igrz__;
    real ionoindx_igrz__[722], indrz_igrz__[722];
} igrz1_;

#define igrz1_1 igrz1_

struct {
    integer num_records_ap__, ints_ap__[440000]	/* was [11][40000] */;
    real reals_ap__[40000];
} ap_;

#define ap_1 ap_

struct {
    real f2_ccir__[23712]	/* was [13][76][2][12] */, f3_ccir__[10584]	
	    /* was [9][49][2][12] */;
} ccir_;

#define ccir_1 ccir_

struct {
    real f2_ursi__[23712]	/* was [13][76][2][12] */;
} ursi_;

#define ursi_1 ursi_

/* Initialized data */

struct {
    char e_1[168];
    } shc2_ = { {'d', 'g', 'r', 'f', '4', '5', '.', 'd', 'a', 't', ' ', ' ', 
	    'd', 'g', 'r', 'f', '5', '0', '.', 'd', 'a', 't', ' ', ' ', 'd', 
	    'g', 'r', 'f', '5', '5', '.', 'd', 'a', 't', ' ', ' ', 'd', 'g', 
	    'r', 'f', '6', '0', '.', 'd', 'a', 't', ' ', ' ', 'd', 'g', 'r', 
	    'f', '6', '5', '.', 'd', 'a', 't', ' ', ' ', 'd', 'g', 'r', 'f', 
	    '7', '0', '.', 'd', 'a', 't', ' ', ' ', 'd', 'g', 'r', 'f', '7', 
	    '5', '.', 'd', 'a', 't', ' ', ' ', 'd', 'g', 'r', 'f', '8', '0', 
	    '.', 'd', 'a', 't', ' ', ' ', 'd', 'g', 'r', 'f', '8', '5', '.', 
	    'd', 'a', 't', ' ', ' ', 'd', 'g', 'r', 'f', '9', '0', '.', 'd', 
	    'a', 't', ' ', ' ', 'd', 'g', 'r', 'f', '9', '5', '.', 'd', 'a', 
	    't', ' ', ' ', 'd', 'g', 'r', 'f', '0', '0', '.', 'd', 'a', 't', 
	    ' ', ' ', 'i', 'g', 'r', 'f', '0', '5', '.', 'd', 'a', 't', ' ', 
	    ' ', 'i', 'g', 'r', 'f', '0', '5', 's', '.', 'd', 'a', 't', ' '} }
	    ;

struct {
    real e_1[14];
    } shc3_ = { (float)1945., (float)1950., (float)1955., (float)1960., (
	    float)1965., (float)1970., (float)1975., (float)1980., (float)
	    1985., (float)1990., (float)1995., (float)2e3, (float)2005., (
	    float)2010. };


/* Table of constant values */

static integer c__1 = 1;
static integer c__9 = 9;
static integer c__3 = 3;
static integer c__4 = 4;

/*     Loads all files into memory so they don't have to be reread on */
/*     every routine call. */

/*     Changelog: */
/*       2015-04-16     Created (Joseph Nicholas) */
/*       2015-04-27     Bugfix in load_ap (Joseph Nicholas) */
/* Subroutine */ int common_data__()
{
    return 0;
} /* common_data__ */

/*     Fortran include file storing common blocks and associated */
/*     parameters. */

/*     Changelog: */
/*       2015-04-16     Created (Joseph Nicholas) */

/* Subroutine */ int load_shc__(integer *ifile, char *filename, ftnlen 
	filename_len)
{
    /* Format strings */
    static char fmt_1001[] = "(\002./../data/IonosphereData/\002,a12)";

    /* System generated locals */
    integer i__1, i__2;
    icilist ici__1;
    olist o__1;
    cllist cl__1;

    /* Builtin functions */
    integer s_wsfi(icilist *), do_fio(integer *, char *, ftnlen), e_wsfi(), 
	    s_wsle(cilist *), do_lio(integer *, integer *, char *, ftnlen), 
	    e_wsle();
    /* Subroutine */ int s_stop(char *, ftnlen);
    integer f_open(olist *), s_rsle(cilist *), e_rsle(), f_clos(cllist *);

    /* Local variables */
    char fullpath[256];
    integer i__, mm, nn, irec;

    /* Fortran I/O blocks */
    static cilist io___2 = { 0, 6, 0, 0, 0 };
    static cilist io___3 = { 0, 84, 0, 0, 0 };
    static cilist io___4 = { 0, 84, 0, 0, 0 };
    static cilist io___8 = { 0, 6, 0, 0, 0 };
    static cilist io___9 = { 0, 84, 0, 0, 0 };


/*     Fortran include file storing common blocks and associated */
/*     parameters. */

/*     Changelog: */
/*       2015-04-16     Created (Joseph Nicholas) */
/*         Special for IRIWeb version */
/* 1001     format('/usr/local/etc/httpd/cgi-bin/models/IRI/',A12) */
/* 1001     format(A12) */
/*         For GMAT version */
/* L1001: */
    ici__1.icierr = 0;
    ici__1.icirnum = 1;
    ici__1.icirlen = 256;
    ici__1.iciunit = fullpath;
    ici__1.icifmt = fmt_1001;
    s_wsfi(&ici__1);
    do_fio(&c__1, filename, filename_len);
    e_wsfi();
    if (*ifile > 14) {
	s_wsle(&io___2);
	do_lio(&c__9, &c__1, "ERROR: load_shc: ifile exceeds MAXFILES_SHC", (
		ftnlen)43);
	e_wsle();
	s_stop("1", (ftnlen)1);
    }
/*         Open coefficient file. Read past first header record. */
    o__1.oerr = 0;
    o__1.ounit = 84;
    o__1.ofnmlen = 256;
    o__1.ofnm = fullpath;
    o__1.orl = 0;
    o__1.osta = "old";
    o__1.oacc = 0;
    o__1.ofm = 0;
    o__1.oblnk = 0;
    f_open(&o__1);
/*         Read degree and order of model and Earth's radius. */
    s_rsle(&io___3);
    e_rsle();
    s_rsle(&io___4);
    do_lio(&c__3, &c__1, (char *)&shc1_1.nmax_shc__[*ifile - 1], (ftnlen)
	    sizeof(integer));
    do_lio(&c__4, &c__1, (char *)&shc1_1.erad_shc__[*ifile - 1], (ftnlen)
	    sizeof(real));
    e_rsle();
/*         -------------------------------------------------------------- */
/*         Read the coefficient file, arranged as follows: */

/*                                         N     M     G     H */
/*                                         ---------------------- */
/*                                     /   1     0    GH(1)  - */
/*                                    /    1     1    GH(2) GH(3) */
/*                                   /     2     0    GH(4)  - */
/*                                  /      2     1    GH(5) GH(6) */
/*             NMAX*(NMAX+3)/2     /       2     2    GH(7) GH(8) */
/*                records          \       3     0    GH(9)  - */
/*                                  \      .     .     .     . */
/*                                   \     .     .     .     . */
/*             NMAX*(NMAX+2)          \    .     .     .     . */
/*             elements in GH          \  NMAX  NMAX   .     . */

/*         N and M are, respectively, the degree and order of the */
/*         coefficient. */
/*         -------------------------------------------------------------- */
    irec = 0;
    i__1 = shc1_1.nmax_shc__[*ifile - 1];
    for (nn = 1; nn <= i__1; ++nn) {
	i__2 = nn;
	for (mm = 0; mm <= i__2; ++mm) {
	    ++irec;
	    if (irec > 200) {
		s_wsle(&io___8);
		do_lio(&c__9, &c__1, "ERROR: load_shc: irec exceeds MAXRECOR\
DS_SHC", (ftnlen)44);
		e_wsle();
		s_stop("1", (ftnlen)1);
	    }
	    s_rsle(&io___9);
	    for (i__ = 1; i__ <= 4; ++i__) {
		do_lio(&c__4, &c__1, (char *)&shc1_1.shcfiles[i__ + (irec + *
			ifile * 200 << 2) - 805], (ftnlen)sizeof(real));
	    }
	    e_rsle();
	}
    }
    cl__1.cerr = 0;
    cl__1.cunit = 84;
    cl__1.csta = 0;
    f_clos(&cl__1);
    return 0;
} /* load_shc__ */

/* Subroutine */ int load_igrz__(char *filename, ftnlen filename_len)
{
    /* Format strings */
    static char fmt_1001[] = "(\002./../data/IonosphereData/\002,a)";

    /* System generated locals */
    integer i__1;
    icilist ici__1;
    olist o__1;
    cllist cl__1;

    /* Builtin functions */
    integer s_wsfi(icilist *), do_fio(integer *, char *, ftnlen), e_wsfi(), 
	    f_open(olist *), s_rsle(cilist *), do_lio(integer *, integer *, 
	    char *, ftnlen), e_rsle(), s_wsle(cilist *), e_wsle();
    /* Subroutine */ int s_stop(char *, ftnlen);
    integer f_clos(cllist *);

    /* Local variables */
    char fullpath[256];
    integer i__, inum_vals__;

    /* Fortran I/O blocks */
    static cilist io___12 = { 0, 84, 0, 0, 0 };
    static cilist io___13 = { 0, 84, 0, 0, 0 };
    static cilist io___15 = { 0, 6, 0, 0, 0 };
    static cilist io___16 = { 0, 84, 0, 0, 0 };
    static cilist io___18 = { 0, 84, 0, 0, 0 };


/*     Fortran include file storing common blocks and associated */
/*     parameters. */

/*     Changelog: */
/*       2015-04-16     Created (Joseph Nicholas) */
/*         Special for IRIWeb version */
/* 1001     format('/usr/local/etc/httpd/cgi-bin/models/IRI/',A) */
/* 1001     format(A) */
/*         For GMAT version */
/* L1001: */
    ici__1.icierr = 0;
    ici__1.icirnum = 1;
    ici__1.icirlen = 256;
    ici__1.iciunit = fullpath;
    ici__1.icifmt = fmt_1001;
    s_wsfi(&ici__1);
    do_fio(&c__1, filename, filename_len);
    e_wsfi();
    o__1.oerr = 0;
    o__1.ounit = 84;
    o__1.ofnmlen = 256;
    o__1.ofnm = fullpath;
    o__1.orl = 0;
    o__1.osta = "old";
    o__1.oacc = 0;
    o__1.ofm = 0;
    o__1.oblnk = 0;
    f_open(&o__1);
/*         Read the update date, the start date and the end date */
/*         (mm,yyyy), and get number of data points to read. */
    s_rsle(&io___12);
    do_lio(&c__3, &c__1, (char *)&igrz1_1.iupd_igrz__, (ftnlen)sizeof(integer)
	    );
    do_lio(&c__3, &c__1, (char *)&igrz1_1.iupm_igrz__, (ftnlen)sizeof(integer)
	    );
    do_lio(&c__3, &c__1, (char *)&igrz1_1.iupy_igrz__, (ftnlen)sizeof(integer)
	    );
    e_rsle();
    s_rsle(&io___13);
    do_lio(&c__3, &c__1, (char *)&igrz1_1.imst_igrz__, (ftnlen)sizeof(integer)
	    );
    do_lio(&c__3, &c__1, (char *)&igrz1_1.iyst_igrz__, (ftnlen)sizeof(integer)
	    );
    do_lio(&c__3, &c__1, (char *)&igrz1_1.imend_igrz__, (ftnlen)sizeof(
	    integer));
    do_lio(&c__3, &c__1, (char *)&igrz1_1.iyend_igrz__, (ftnlen)sizeof(
	    integer));
    e_rsle();
    inum_vals__ = 3 - igrz1_1.imst_igrz__ + (igrz1_1.iyend_igrz__ - 
	    igrz1_1.iyst_igrz__) * 12 + igrz1_1.imend_igrz__;
    if (inum_vals__ > 722) {
	s_wsle(&io___15);
	do_lio(&c__9, &c__1, "ERROR: load_igrz: inum_vals exceeds MAXRECORDL\
EN_IGRZ", (ftnlen)53);
	e_wsle();
	s_stop("1", (ftnlen)1);
    }
/*         Read data records */
    s_rsle(&io___16);
    i__1 = inum_vals__;
    for (i__ = 1; i__ <= i__1; ++i__) {
	do_lio(&c__4, &c__1, (char *)&igrz1_1.ionoindx_igrz__[i__ - 1], (
		ftnlen)sizeof(real));
    }
    e_rsle();
    s_rsle(&io___18);
    i__1 = inum_vals__;
    for (i__ = 1; i__ <= i__1; ++i__) {
	do_lio(&c__4, &c__1, (char *)&igrz1_1.indrz_igrz__[i__ - 1], (ftnlen)
		sizeof(real));
    }
    e_rsle();
    cl__1.cerr = 0;
    cl__1.cunit = 84;
    cl__1.csta = 0;
    f_clos(&cl__1);
    return 0;
} /* load_igrz__ */

/* Subroutine */ int load_ap__(char *filename, ftnlen filename_len)
{
    /* Format strings */
    static char fmt_1001[] = "(\002./../data/IonosphereData/\002,a)";
    static char fmt_1002[] = "(3i3,8i3,f5.1)";

    /* System generated locals */
    integer i__1;
    icilist ici__1;
    olist o__1;
    cllist cl__1;

    /* Builtin functions */
    integer s_wsfi(icilist *), do_fio(integer *, char *, ftnlen), e_wsfi(), 
	    f_open(olist *), s_wsle(cilist *), do_lio(integer *, integer *, 
	    char *, ftnlen), e_wsle();
    /* Subroutine */ int s_stop(char *, ftnlen);
    integer s_rsfe(cilist *), e_rsfe(), f_clos(cllist *);

    /* Local variables */
    char fullpath[256];
    integer k, irec;

    /* Fortran I/O blocks */
    static cilist io___21 = { 0, 6, 0, 0, 0 };
    static cilist io___22 = { 0, 84, 1, fmt_1002, 0 };


/*     Fortran include file storing common blocks and associated */
/*     parameters. */

/*     Changelog: */
/*       2015-04-16     Created (Joseph Nicholas) */
/*         Special for IRIWeb version */
/* 1001     format('/usr/local/etc/httpd/cgi-bin/models/IRI/',A) */
/* 1001     format(A) */
/*         For GMAT version */
/* L1001: */
    ici__1.icierr = 0;
    ici__1.icirnum = 1;
    ici__1.icirlen = 256;
    ici__1.iciunit = fullpath;
    ici__1.icifmt = fmt_1001;
    s_wsfi(&ici__1);
    do_fio(&c__1, filename, filename_len);
    e_wsfi();
    o__1.oerr = 0;
    o__1.ounit = 84;
    o__1.ofnmlen = 256;
    o__1.ofnm = fullpath;
    o__1.orl = 0;
    o__1.osta = "old";
    o__1.oacc = 0;
    o__1.ofm = 0;
    o__1.oblnk = 0;
    f_open(&o__1);
    for (irec = 1; irec <= 40001; ++irec) {
	if (irec > 40000) {
	    s_wsle(&io___21);
	    do_lio(&c__9, &c__1, "ERROR: load_ap: irec exceeds MAXRECORDS_AP",
		     (ftnlen)42);
	    e_wsle();
	    s_stop("1", (ftnlen)1);
	}
	i__1 = s_rsfe(&io___22);
	if (i__1 != 0) {
	    goto L2001;
	}
	for (k = 1; k <= 11; ++k) {
	    i__1 = do_fio(&c__1, (char *)&ap_1.ints_ap__[k + irec * 11 - 12], 
		    (ftnlen)sizeof(integer));
	    if (i__1 != 0) {
		goto L2001;
	    }
	}
	i__1 = do_fio(&c__1, (char *)&ap_1.reals_ap__[irec - 1], (ftnlen)
		sizeof(real));
	if (i__1 != 0) {
	    goto L2001;
	}
	i__1 = e_rsfe();
	if (i__1 != 0) {
	    goto L2001;
	}
    }
L2001:
    cl__1.cerr = 0;
    cl__1.cunit = 84;
    cl__1.csta = 0;
    f_clos(&cl__1);
    ap_1.num_records_ap__ = irec - 1;
    return 0;
} /* load_ap__ */

/* Subroutine */ int load_ccir__(integer *imonth)
{
    /* Format strings */
    static char fmt_1001[] = "(\002./../data/IonosphereData/ccir\002,i2,\002\
.asc\002)";
    static char fmt_1002[] = "(1x,4e15.8)";

    /* System generated locals */
    integer i__1;
    icilist ici__1;
    olist o__1;
    cllist cl__1;

    /* Builtin functions */
    integer s_wsfi(icilist *), do_fio(integer *, char *, ftnlen), e_wsfi(), 
	    s_wsle(cilist *), do_lio(integer *, integer *, char *, ftnlen), 
	    e_wsle();
    /* Subroutine */ int s_stop(char *, ftnlen);
    integer f_open(olist *), s_rsfe(cilist *), e_rsfe(), f_clos(cllist *);

    /* Local variables */
    char fullpath[256];
    integer i__, j, k;

    /* Fortran I/O blocks */
    static cilist io___25 = { 0, 6, 0, 0, 0 };
    static cilist io___26 = { 0, 84, 0, fmt_1002, 0 };


/*     Fortran include file storing common blocks and associated */
/*     parameters. */

/*     Changelog: */
/*       2015-04-16     Created (Joseph Nicholas) */
/*         Special for IRIWeb version */
/* 1001     format('/usr/local/etc/httpd/cgi-bin/models/IRI/ccir', */
/*    &           I2,'.asc') */
/* 1001     format('ccir',I2,'.asc') */
/* L1001: */
    ici__1.icierr = 0;
    ici__1.icirnum = 1;
    ici__1.icirlen = 256;
    ici__1.iciunit = fullpath;
    ici__1.icifmt = fmt_1001;
    s_wsfi(&ici__1);
    i__1 = *imonth + 10;
    do_fio(&c__1, (char *)&i__1, (ftnlen)sizeof(integer));
    e_wsfi();
    if (*imonth > 12) {
	s_wsle(&io___25);
	do_lio(&c__9, &c__1, "ERROR: load_ccir: imonth exceeds MAXFILES_CCIR",
		 (ftnlen)46);
	e_wsle();
	s_stop("1", (ftnlen)1);
    }
    o__1.oerr = 0;
    o__1.ounit = 84;
    o__1.ofnmlen = 256;
    o__1.ofnm = fullpath;
    o__1.orl = 0;
    o__1.osta = "old";
    o__1.oacc = 0;
    o__1.ofm = 0;
    o__1.oblnk = 0;
    f_open(&o__1);
    s_rsfe(&io___26);
    for (k = 1; k <= 2; ++k) {
	for (j = 1; j <= 76; ++j) {
	    for (i__ = 1; i__ <= 13; ++i__) {
		do_fio(&c__1, (char *)&ccir_1.f2_ccir__[i__ + (j + (k + (*
			imonth << 1)) * 76) * 13 - 2978], (ftnlen)sizeof(real)
			);
	    }
	}
    }
    for (k = 1; k <= 2; ++k) {
	for (j = 1; j <= 49; ++j) {
	    for (i__ = 1; i__ <= 9; ++i__) {
		do_fio(&c__1, (char *)&ccir_1.f3_ccir__[i__ + (j + (k + (*
			imonth << 1)) * 49) * 9 - 1333], (ftnlen)sizeof(real))
			;
	    }
	}
    }
    e_rsfe();
    cl__1.cerr = 0;
    cl__1.cunit = 84;
    cl__1.csta = 0;
    f_clos(&cl__1);
    return 0;
} /* load_ccir__ */

/* Subroutine */ int load_ursi__(integer *imonth)
{
    /* Format strings */
    static char fmt_1001[] = "(\002./../data/IonosphereData/ursi\002,i2,\002\
.asc\002)";
    static char fmt_1002[] = "(1x,4e15.8)";

    /* System generated locals */
    integer i__1;
    icilist ici__1;
    olist o__1;
    cllist cl__1;

    /* Builtin functions */
    integer s_wsfi(icilist *), do_fio(integer *, char *, ftnlen), e_wsfi(), 
	    s_wsle(cilist *), do_lio(integer *, integer *, char *, ftnlen), 
	    e_wsle();
    /* Subroutine */ int s_stop(char *, ftnlen);
    integer f_open(olist *), s_rsfe(cilist *), e_rsfe(), f_clos(cllist *);

    /* Local variables */
    char fullpath[256];
    integer i__, j, k;

    /* Fortran I/O blocks */
    static cilist io___31 = { 0, 6, 0, 0, 0 };
    static cilist io___32 = { 0, 84, 0, fmt_1002, 0 };


/*     Fortran include file storing common blocks and associated */
/*     parameters. */

/*     Changelog: */
/*       2015-04-16     Created (Joseph Nicholas) */
/*         Special for IRIWeb version */
/* 1001     format('/usr/local/etc/httpd/cgi-bin/models/IRI/ursi', */
/*    &           I2,'.asc') */
/* 1001     format('ursi',I2,'.asc') */
/* L1001: */
    ici__1.icierr = 0;
    ici__1.icirnum = 1;
    ici__1.icirlen = 256;
    ici__1.iciunit = fullpath;
    ici__1.icifmt = fmt_1001;
    s_wsfi(&ici__1);
    i__1 = *imonth + 10;
    do_fio(&c__1, (char *)&i__1, (ftnlen)sizeof(integer));
    e_wsfi();
    if (*imonth > 12) {
	s_wsle(&io___31);
	do_lio(&c__9, &c__1, "ERROR: load_ursi: imonth exceeds MAXFILES_URSI",
		 (ftnlen)46);
	e_wsle();
	s_stop("1", (ftnlen)1);
    }
    o__1.oerr = 0;
    o__1.ounit = 84;
    o__1.ofnmlen = 256;
    o__1.ofnm = fullpath;
    o__1.orl = 0;
    o__1.osta = "old";
    o__1.oacc = 0;
    o__1.ofm = 0;
    o__1.oblnk = 0;
    f_open(&o__1);
    s_rsfe(&io___32);
    for (k = 1; k <= 2; ++k) {
	for (j = 1; j <= 76; ++j) {
	    for (i__ = 1; i__ <= 13; ++i__) {
		do_fio(&c__1, (char *)&ursi_1.f2_ursi__[i__ + (j + (k + (*
			imonth << 1)) * 76) * 13 - 2978], (ftnlen)sizeof(real)
			);
	    }
	}
    }
    e_rsfe();
    cl__1.cerr = 0;
    cl__1.cunit = 84;
    cl__1.csta = 0;
    f_clos(&cl__1);
    return 0;
} /* load_ursi__ */

/* Subroutine */ int load_all_files__()
{
    extern /* Subroutine */ int load_shc__(integer *, char *, ftnlen);
    integer i__;
    extern /* Subroutine */ int load_ccir__(integer *), load_igrz__(char *, 
	    ftnlen), load_ursi__(integer *), load_ap__(char *, ftnlen);

/*     Fortran include file storing common blocks and associated */
/*     parameters. */

/*     Changelog: */
/*       2015-04-16     Created (Joseph Nicholas) */
    for (i__ = 1; i__ <= 14; ++i__) {
	load_shc__(&i__, shc2_1.filmod + (i__ - 1) * 12, (ftnlen)12);
    }
    load_igrz__("ig_rz.dat", (ftnlen)9);
    load_ap__("ap.dat", (ftnlen)6);
    for (i__ = 1; i__ <= 12; ++i__) {
	load_ccir__(&i__);
    }
    for (i__ = 1; i__ <= 12; ++i__) {
	load_ursi__(&i__);
    }
    return 0;
} /* load_all_files__ */

#ifdef __cplusplus
	}
#endif
