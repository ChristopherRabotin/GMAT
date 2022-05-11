/* convrt_3.f -- translated by f2c (version 19980913).
   You must link the resulting object file with the libraries:
	-lf2c -lm   (in that order)
*/

#include "f2c.h"

/* Table of constant values */

static integer c__128 = 128;
static doublereal c_b26 = 1.;
static integer c__0 = 0;
static doublereal c_b28 = 0.;
static integer c_n1 = -1;

/* $Procedure CONVRT_3  ( Convert Units ) */
/* Subroutine */ int convrt_3__(doublereal *xin, char *unin, char *unout, 
	doublereal *xout, integer *status, ftnlen unin_len, ftnlen unout_len)
{
    /* Initialized data */

    static doublereal dim[6] = { 0.,1.,1.,1.,1.,1. };
    static logical first = TRUE_;
    static integer nop = 6;
    static char op[2*6] = "  " "( " ") " "* " "**" "/ ";

    /* System generated locals */
    integer i__1, i__2, i__3, i__4;

    /* Builtin functions */
    integer s_rnge(char *, integer, char *, integer);
    double pow_dd(doublereal *, doublereal *);

    /* Local variables */
    static doublereal diff;
    extern /* Subroutine */ int scan_(char *, char *, integer *, integer *, 
	    integer *, integer *, integer *, integer *, integer *, integer *, 
	    ftnlen, ftnlen);
    static doublereal keep;
    static logical done, move;
    static integer mult, b, e, i__, j, l, o, r__, blank;
    static doublereal dimen[6];
    extern /* Subroutine */ int chkin_(char *, ftnlen);
    static integer ident[128], class__;
    static doublereal inval, value;
    static integer oplen[6];
    static doublereal opval[6];
    static logical known;
    static integer start, inout;
    extern logical unitp_(char *, ftnlen);
    static integer opptr[20];
    extern /* Subroutine */ int podbed_(doublereal *, integer *, integer *), 
	    podbgd_(doublereal *), podegd_(doublereal *);
    extern integer bsrchc_(char *, integer *, char *, ftnlen, ftnlen);
    static doublereal dimeni[6];
    static integer clas_s__[134];
    extern /* Subroutine */ int appndd_(doublereal *, doublereal *), podbgi_(
	    integer *), podegi_(integer *);
    static doublereal dimeno[6];
    static integer active;
    extern /* Subroutine */ int appndi_(integer *, integer *);
    static doublereal parsed[134];
    static integer lparen;
    extern /* Subroutine */ int fnducv_(char *, logical *, integer *, 
	    doublereal *, ftnlen), scanpr_(integer *, char *, integer *, 
	    integer *, ftnlen);
    static integer rparen;
    extern /* Subroutine */ int chkout_(char *, ftnlen), ssized_(integer *, 
	    doublereal *), ssizei_(integer *, integer *);
    static doublereal outval;
    static integer ntokns;
    static doublereal expont[134];
    extern logical return_(void);
    static integer beg[128], end[128], div, exp__;

/* $ Abstract */

/*     Convert a quantity in one system of units to another system. */

/* $ Disclaimer */

/*     THIS SOFTWARE AND ANY RELATED MATERIALS WERE CREATED BY THE */
/*     CALIFORNIA INSTITUTE OF TECHNOLOGY (CALTECH) UNDER A U.S. */
/*     GOVERNMENT CONTRACT WITH THE NATIONAL AERONAUTICS AND SPACE */
/*     ADMINISTRATION (NASA). THE SOFTWARE IS TECHNOLOGY AND SOFTWARE */
/*     PUBLICLY AVAILABLE UNDER U.S. EXPORT LAWS AND IS PROVIDED "AS-IS" */
/*     TO THE RECIPIENT WITHOUT WARRANTY OF ANY KIND, INCLUDING ANY */
/*     WARRANTIES OF PERFORMANCE OR MERCHANTABILITY OR FITNESS FOR A */
/*     PARTICULAR USE OR PURPOSE (AS SET FORTH IN UNITED STATES UCC */
/*     SECTIONS 2312-2313) OR FOR ANY PURPOSE WHATSOEVER, FOR THE */
/*     SOFTWARE AND RELATED MATERIALS, HOWEVER USED. */

/*     IN NO EVENT SHALL CALTECH, ITS JET PROPULSION LABORATORY, OR NASA */
/*     BE LIABLE FOR ANY DAMAGES AND/OR COSTS, INCLUDING, BUT NOT */
/*     LIMITED TO, INCIDENTAL OR CONSEQUENTIAL DAMAGES OF ANY KIND, */
/*     INCLUDING ECONOMIC DAMAGE OR INJURY TO PROPERTY AND LOST PROFITS, */
/*     REGARDLESS OF WHETHER CALTECH, JPL, OR NASA BE ADVISED, HAVE */
/*     REASON TO KNOW, OR, IN FACT, SHALL KNOW OF THE POSSIBILITY. */

/*     RECIPIENT BEARS ALL RISK RELATING TO QUALITY AND PERFORMANCE OF */
/*     THE SOFTWARE AND ANY RELATED MATERIALS, AND AGREES TO INDEMNIFY */
/*     CALTECH AND NASA FOR ALL THIRD-PARTY CLAIMS RESULTING FROM THE */
/*     ACTIONS OF RECIPIENT IN THE USE OF THE SOFTWARE. */

/* $ Required_Reading */

/*     None. */

/* $ Keywords */

/*     CONVERSION */
/*     UTILITY */

/* $ Declarations */
/* $ Brief_I/O */

/*     Variable  I/O  Description */
/*     --------  ---  -------------------------------------------------- */
/*     XIN        I   A quantity measured in UNIN units. */
/*     UNIN       I   The units of the input quantity. */
/*     UNOUT      I   The units desired for output. */
/*     XOUT       O   The value of XIN in the UNOUT units. */
/*     STATUS     O   Indicates the success of the operation */

/* $ Detailed_Input */

/*     XIN        is the measurement of a physical quantity in the */
/*                units given by UNIN. */

/*     UNIN       are the units associated with the input quantity */
/*                XIN.  These units should be expressed in terms */
/*                of units of angle, length, time, mass and charge */
/*                (no compound units such as newtons or joules.) */


/*     UNOUT      are the units that will be associated with the */
/*                output quantity XOUT.  UNOUT must be dimensionally */
/*                equivalent to UNIN and, like UNIN, must be expressed */
/*                in terms of units of angle, length, time, mass and */
/*                charge. */

/* $ Detailed_Output */

/*     XOUT       is the number of UNOUT units that are equal to */
/*                XIN units of UNIN.  XOUT may overwrite XIN. */

/*     STATUS     if no problems are encountered in the attempt */
/*                to perform the translation of units STATUS will */
/*                be returned with the value 0. */

/*                If the units input are not recognized STATUS */
/*                will be returned with the value 1. */

/*                If the units for output are not recognized, STATUS */
/*                will be returned with the value 2. */

/*                If units on input are not compatible with output, */
/*                STATUS will be returned with the value 3. */

/* $ Parameters */

/*     None. */

/* $ Exceptions */

/*     1) If either UNIN or UNOUT is not a recognized physical unit, */
/*        the error 'SPICE(BADUNITS)' will be signaled. */

/*     2) If UNIN and UNOUT are not dimensionally equivalent, the */
/*        error 'SPICE(INCOMPATIBLEUNITS)' will be signaled. */

/* $ Files */

/*     None. */

/* $ Particulars */

/*     This routine provides a simple means of converting between */
/*     a quantity expressed in terms of one system of units to */
/*     another system of units.  The fundamental units recognized */
/*     are those recognized by FNDUCV.  The units recognized by */
/*     version 1.0.0 of FNDUCV are: */




/*     If the singular form a unit is not listed, but it is obtained */
/*     from the plural form by dropping a final 'S', you may use the */
/*     singular form.  For example, */

/*        instead of  SECONDS you may use SECOND; */
/*        instead of  MILES   you may use MILE; */
/*        instead of  DEGREES you may use DEGREE. */

/*     Thus the strings 'SECONDS/DEGREE', 'SECOND/DEGREES', */
/*     'SECOND/DEGREE', and 'SECONDS/DEGREES' are all recognized */
/*     and have have the same meaning. */

/* $ Examples */

/*     Suppose you needed to convert a state, PV, from KM and KM/SEC to */
/*     AU and AU/365 days.  The following loop will do the job. */

/*        DO I = 1, 3 */
/*           CALL CONVRT_3 ( PV(I),   'KM',     'AU',           PV(I)   ) */
/*           CALL CONVRT_3 ( PV(I+3), 'KM/SEC', 'AU/(365*DAYS)' PV(I+3) ) */
/*        END DO */

/* $ Restrictions */

/*     None. */

/* $ Literature_References */

/*     None. */

/* $ Author_and_Institution */

/*     W.L. Taber     (JPL) */
/*     B.V. Semenov   (JPL) */

/* $ Version */

/* -    GLLSPICE Version 1.3.0, 09-NOV-2020 (BVS) */

/*        Added missing 'D' to literal DP constants. */

/* -    SPICELIB Version 1.2.0, 28-SEP-2010 (EDW) */

/*        Renamed CLASSS variable CLAS_S to eliminate ifort 11.1 Pro */
/*        compiler error. ifort interpreted CLASSS as 'CLASS S'. */

/* -    SPICELIB Version 1.1.0, 28-MAY-2010 (EDW) */

/*        Minor edit to eliminate unused variable NAME. */

/* -    Beta Version 1.0.0, 31-MAY-1991 (WLT) */

/* -& */

/*     These are the class id's for each of the various entities */
/*     that make up the variables of a unit. */


/*     These are the codes will will use for the various */
/*     operations. */


/*     Scanning Parameters */


/*     SPICELIB functions */


/*     Other functions */


/*       Here is the range of       Character      ASCII code */
/*       initial characters that    ---------      ---------- */
/*       will be used by the        ' '             32 */
/*       "known" marks.             '('             40 */
/*                                  ')'             41 */
/*                                  '*'             42 */
/*                                  '/'             47 */

/*      So the required number of pointers is 47 - 32 + 5 = 20. */


/*     Saved Variables */


/*     Initial Values */


/*     The game is afoot! */

    if (return_()) {
	return 0;
    } else {
	chkin_("CONVRT_3", (ftnlen)8);
    }
    if (first) {
	first = FALSE_;
	scanpr_(&nop, op, oplen, opptr, (ftnlen)2);
	blank = bsrchc_(" ", &nop, op, (ftnlen)1, (ftnlen)2);
	lparen = bsrchc_("(", &nop, op, (ftnlen)1, (ftnlen)2);
	rparen = bsrchc_(")", &nop, op, (ftnlen)1, (ftnlen)2);
	mult = bsrchc_("*", &nop, op, (ftnlen)1, (ftnlen)2);
	exp__ = bsrchc_("**", &nop, op, (ftnlen)2, (ftnlen)2);
	div = bsrchc_("/", &nop, op, (ftnlen)1, (ftnlen)2);
	opval[(i__1 = blank - 1) < 6 && 0 <= i__1 ? i__1 : s_rnge("opval", 
		i__1, "convrt_3__", (ftnlen)349)] = 0.;
	opval[(i__1 = lparen - 1) < 6 && 0 <= i__1 ? i__1 : s_rnge("opval", 
		i__1, "convrt_3__", (ftnlen)350)] = 0.;
	opval[(i__1 = rparen - 1) < 6 && 0 <= i__1 ? i__1 : s_rnge("opval", 
		i__1, "convrt_3__", (ftnlen)351)] = 0.;
	opval[(i__1 = mult - 1) < 6 && 0 <= i__1 ? i__1 : s_rnge("opval", 
		i__1, "convrt_3__", (ftnlen)352)] = 1.;
	opval[(i__1 = exp__ - 1) < 6 && 0 <= i__1 ? i__1 : s_rnge("opval", 
		i__1, "convrt_3__", (ftnlen)353)] = 3.;
	opval[(i__1 = div - 1) < 6 && 0 <= i__1 ? i__1 : s_rnge("opval", i__1,
		 "convrt_3__", (ftnlen)354)] = 2.;
    }

/*     First make sure that both UNIN and UNOUT are recognized */
/*     units. */

    if (! unitp_(unin, unin_len)) {
	*status = 1;
	chkout_("CONVRT_3", (ftnlen)8);
	return 0;
    }
    if (! unitp_(unout, unout_len)) {
	*status = 2;
	chkout_("CONVRT_3", (ftnlen)8);
	return 0;
    }

/*     We will need to keep track of the dimensions associated */
/*     with both input and output units. */

    dimeni[1] = 0.;
    dimeni[2] = 0.;
    dimeni[3] = 0.;
    dimeni[4] = 0.;
    dimeni[5] = 0.;
    dimeno[1] = 0.;
    dimeno[2] = 0.;
    dimeno[3] = 0.;
    dimeno[4] = 0.;
    dimeno[5] = 0.;

/*     We need to parse both the input and output units, we */
/*     do that in the loop that ranges from INPUT to OUTPUT. */

    for (inout = 1; inout <= 2; ++inout) {

/*        Initialize the various pods we will need to use to */
/*        parse this set of units. */

	ssized_(&c__128, parsed);
	ssizei_(&c__128, clas_s__);
	ssized_(&c__128, expont);

/*        Zero out the dimension vector. */

	dimen[1] = 0.;
	dimen[2] = 0.;
	dimen[3] = 0.;
	dimen[4] = 0.;
	dimen[5] = 0.;

/*        We haven't finished scanning this unit yet. */

	done = FALSE_;

/*        We are beginning a group now.  After beginning a group we */
/*        ALWAYS append 1,0,0 and MULTPLY, -1, 0 to the PARSED, */
/*        CLAS_S, and EXPONT pod.  Why ask why?  Well in this case */
/*        we do it because it makes the processing MUCH simpler */
/*        (you'll see). */

	appndd_(&c_b26, parsed);
	appndi_(&c__0, clas_s__);
	appndd_(&c_b28, expont);
	appndd_(&c_b26, parsed);
	appndi_(&c_n1, clas_s__);
	appndd_(&c_b28, expont);

/*        We'll start scanning this string from the first character. */

	start = 1;
	if (inout == 1) {
	    scan_(unin, op, oplen, opptr, &c__128, &start, &ntokns, ident, 
		    beg, end, unin_len, (ftnlen)2);
	} else if (inout == 2) {
	    scan_(unout, op, oplen, opptr, &c__128, &start, &ntokns, ident, 
		    beg, end, unout_len, (ftnlen)2);
	}

/*        For as long as there are tokens to look at... */

	while(ntokns > 0) {

/*           ... examine each in turn, classify it and take */
/*           an appropriate action. */

	    i__1 = ntokns;
	    for (i__ = 1; i__ <= i__1; ++i__) {

/*              If we have a left parenthesis ... */

		if (ident[(i__2 = i__ - 1) < 128 && 0 <= i__2 ? i__2 : s_rnge(
			"ident", i__2, "convrt_3__", (ftnlen)474)] == lparen) 
			{

/*                 We are beginning a group now.  After beginning a */
/*                 group we ALWAYS append 1,0,0 and MULTPLY, -1, 0 to */
/*                 the PARSED, CLAS_S, and EXPONT pod. */

		    podbgd_(parsed);
		    podbgi_(clas_s__);
		    podbgd_(expont);
		    appndd_(&c_b26, parsed);
		    appndi_(&c__0, clas_s__);
		    appndd_(&c_b28, expont);
		    appndd_(&c_b26, parsed);
		    appndi_(&c_n1, clas_s__);
		    appndd_(&c_b28, expont);

/*              ... or if we have an arithmetic operations */

		} else if (ident[(i__2 = i__ - 1) < 128 && 0 <= i__2 ? i__2 : 
			s_rnge("ident", i__2, "convrt_3__", (ftnlen)496)] == 
			mult || ident[(i__3 = i__ - 1) < 128 && 0 <= i__3 ? 
			i__3 : s_rnge("ident", i__3, "convrt_3__", (ftnlen)
			496)] == div || ident[(i__4 = i__ - 1) < 128 && 0 <= 
			i__4 ? i__4 : s_rnge("ident", i__4, "convrt_3__", (
			ftnlen)496)] == exp__) {

/*                 Append the operation to the current group. */

		    appndd_(&opval[(i__3 = ident[(i__2 = i__ - 1) < 128 && 0 
			    <= i__2 ? i__2 : s_rnge("ident", i__2, "convrt_3"
			    "__", (ftnlen)503)] - 1) < 6 && 0 <= i__3 ? i__3 : 
			    s_rnge("opval", i__3, "convrt_3__", (ftnlen)503)],
			     parsed);
		    appndi_(&c_n1, clas_s__);
		    appndd_(&c_b28, expont);

/*              ...or if we have a unit or number ... */

		} else if (ident[(i__2 = i__ - 1) < 128 && 0 <= i__2 ? i__2 : 
			s_rnge("ident", i__2, "convrt_3__", (ftnlen)510)] == 
			0) {

/*                  Look up the class and value for this token, */
/*                  append them to the current group. */

		    b = beg[(i__2 = i__ - 1) < 128 && 0 <= i__2 ? i__2 : 
			    s_rnge("beg", i__2, "convrt_3__", (ftnlen)516)];
		    e = end[(i__2 = i__ - 1) < 128 && 0 <= i__2 ? i__2 : 
			    s_rnge("end", i__2, "convrt_3__", (ftnlen)517)];
		    if (inout == 1) {
			fnducv_(unin + (b - 1), &known, &class__, &value, e - 
				(b - 1));
		    } else if (inout == 2) {
			fnducv_(unout + (b - 1), &known, &class__, &value, e 
				- (b - 1));
		    }
		    appndd_(&value, parsed);
		    appndi_(&class__, clas_s__);
		    appndd_(&dim[(i__2 = class__) < 6 && 0 <= i__2 ? i__2 : 
			    s_rnge("dim", i__2, "convrt_3__", (ftnlen)531)], 
			    expont);

/*              ...or if we have a right parenthesis, close off */
/*              this group by evaluating it, then close the group */
/*              and append the last value computed onto its list */
/*              of value/operation pairs. */

		} else if (ident[(i__2 = i__ - 1) < 128 && 0 <= i__2 ? i__2 : 
			s_rnge("ident", i__2, "convrt_3__", (ftnlen)540)] == 
			rparen) {

/*                 We are ending a group.  It's time to perform all */
/*                 indicated operations in this group.  Note the */
/*                 structure of a completed group is: */

/*                   Value  OP  Value OP Value ... OP Value */

/*                 Thus all operations are at even slots in the */
/*                 group.  The scheme for evaluating this expression */
/*                 is: identify the next operation to perform (more on */
/*                 how to locate the operation in a minute); */

/*                                      Do this one */
/*                                      _____^______ */
/*                                     '            ` */
/*                  Value OP Value OP  Value OP Value  OP Value OP ... */

/*                 replace the three entries by the result. */

/*                     Value OP Value OP  result OP Value OP  ... */

/*                 The hierarchy of operations is */

/*                    1.) exponentiation in left to right order. */

/*                    2.) multiplication and division in left */
/*                        to right order. */

/*                 Since the parsing is from left to right, as we */
/*                 simplify subexpression, we can shift items left */
/*                 to fill in the gaps left by the operator and */
/*                 second value of the expression that was simplified. */

/*                 To do all this we must fist identify the beginning */
/*                 and ends of this group. */

		    podbed_(parsed, &b, &e);

/*                 First handle exponentiation.  So far we haven't */
/*                 moved anything, the ACTIVE left operand is at B; */
/*                 the first operator is located at B+1.  We will let */
/*                 ATOP (at operator) be the logical flag that indicates */
/*                 whether J points to an operator or an operand. */

		    move = FALSE_;
		    active = b;
		    j = b + 1;
		    while(j <= e) {
			if (parsed[(i__2 = j + 5) < 134 && 0 <= i__2 ? i__2 : 
				s_rnge("parsed", i__2, "convrt_3__", (ftnlen)
				593)] == 3.) {

/*                       We are going to simplify an expression */
/*                       of the form  X ** Y to its computed value. */
/*                       This means we will be freeing up room to */
/*                       move items to the left. */


			    move = TRUE_;
			    parsed[(i__2 = active + 5) < 134 && 0 <= i__2 ? 
				    i__2 : s_rnge("parsed", i__2, "convrt_3__"
				    , (ftnlen)604)] = pow_dd(&parsed[(i__3 = 
				    active + 5) < 134 && 0 <= i__3 ? i__3 : 
				    s_rnge("parsed", i__3, "convrt_3__", (
				    ftnlen)604)], &parsed[(i__4 = j + 6) < 
				    134 && 0 <= i__4 ? i__4 : s_rnge("parsed",
				     i__4, "convrt_3__", (ftnlen)604)]);
			    expont[(i__2 = active + 5) < 134 && 0 <= i__2 ? 
				    i__2 : s_rnge("expont", i__2, "convrt_3__"
				    , (ftnlen)605)] = expont[(i__3 = active + 
				    5) < 134 && 0 <= i__3 ? i__3 : s_rnge(
				    "expont", i__3, "convrt_3__", (ftnlen)605)
				    ] * parsed[(i__4 = j + 6) < 134 && 0 <= 
				    i__4 ? i__4 : s_rnge("parsed", i__4, 
				    "convrt_3__", (ftnlen)605)];
			} else {

/*                       If we are moving operators and right */
/*                       operands to the left, now is the time */
/*                       to do it. */

			    if (move) {
				o = active + 1;
				l = active + 2;
				r__ = j + 1;
				parsed[(i__2 = o + 5) < 134 && 0 <= i__2 ? 
					i__2 : s_rnge("parsed", i__2, "convr"
					"t_3__", (ftnlen)621)] = parsed[(i__3 =
					 j + 5) < 134 && 0 <= i__3 ? i__3 : 
					s_rnge("parsed", i__3, "convrt_3__", (
					ftnlen)621)];
				clas_s__[(i__2 = o + 5) < 134 && 0 <= i__2 ? 
					i__2 : s_rnge("clas_s", i__2, "convr"
					"t_3__", (ftnlen)622)] = clas_s__[(
					i__3 = j + 5) < 134 && 0 <= i__3 ? 
					i__3 : s_rnge("clas_s", i__3, "convr"
					"t_3__", (ftnlen)622)];
				expont[(i__2 = o + 5) < 134 && 0 <= i__2 ? 
					i__2 : s_rnge("expont", i__2, "convr"
					"t_3__", (ftnlen)623)] = expont[(i__3 =
					 j + 5) < 134 && 0 <= i__3 ? i__3 : 
					s_rnge("expont", i__3, "convrt_3__", (
					ftnlen)623)];
				parsed[(i__2 = l + 5) < 134 && 0 <= i__2 ? 
					i__2 : s_rnge("parsed", i__2, "convr"
					"t_3__", (ftnlen)625)] = parsed[(i__3 =
					 r__ + 5) < 134 && 0 <= i__3 ? i__3 : 
					s_rnge("parsed", i__3, "convrt_3__", (
					ftnlen)625)];
				clas_s__[(i__2 = l + 5) < 134 && 0 <= i__2 ? 
					i__2 : s_rnge("clas_s", i__2, "convr"
					"t_3__", (ftnlen)626)] = clas_s__[(
					i__3 = r__ + 5) < 134 && 0 <= i__3 ? 
					i__3 : s_rnge("clas_s", i__3, "convr"
					"t_3__", (ftnlen)626)];
				expont[(i__2 = l + 5) < 134 && 0 <= i__2 ? 
					i__2 : s_rnge("expont", i__2, "convr"
					"t_3__", (ftnlen)627)] = expont[(i__3 =
					 r__ + 5) < 134 && 0 <= i__3 ? i__3 : 
					s_rnge("expont", i__3, "convrt_3__", (
					ftnlen)627)];
			    }
			    active += 2;
			}

/*                    Make J point to the next operator. */

			j += 2;
		    }

/*                 Next handle multiplication and division. */

		    e = active;
		    active = b;
		    j = b + 1;
		    while(j <= e) {
			r__ = j + 1;
			class__ = clas_s__[(i__2 = r__ + 5) < 134 && 0 <= 
				i__2 ? i__2 : s_rnge("clas_s", i__2, "convrt"
				"_3__", (ftnlen)652)];
			if (parsed[(i__2 = j + 5) < 134 && 0 <= i__2 ? i__2 : 
				s_rnge("parsed", i__2, "convrt_3__", (ftnlen)
				654)] == 1.) {
			    parsed[(i__2 = active + 5) < 134 && 0 <= i__2 ? 
				    i__2 : s_rnge("parsed", i__2, "convrt_3__"
				    , (ftnlen)656)] = parsed[(i__3 = active + 
				    5) < 134 && 0 <= i__3 ? i__3 : s_rnge(
				    "parsed", i__3, "convrt_3__", (ftnlen)656)
				    ] * parsed[(i__4 = r__ + 5) < 134 && 0 <= 
				    i__4 ? i__4 : s_rnge("parsed", i__4, 
				    "convrt_3__", (ftnlen)656)];
			    dimen[(i__2 = class__) < 6 && 0 <= i__2 ? i__2 : 
				    s_rnge("dimen", i__2, "convrt_3__", (
				    ftnlen)657)] = dimen[(i__3 = class__) < 6 
				    && 0 <= i__3 ? i__3 : s_rnge("dimen", 
				    i__3, "convrt_3__", (ftnlen)657)] + 
				    expont[(i__4 = r__ + 5) < 134 && 0 <= 
				    i__4 ? i__4 : s_rnge("expont", i__4, 
				    "convrt_3__", (ftnlen)657)];
			} else if (parsed[(i__2 = j + 5) < 134 && 0 <= i__2 ? 
				i__2 : s_rnge("parsed", i__2, "convrt_3__", (
				ftnlen)659)] == 2.) {
			    parsed[(i__2 = active + 5) < 134 && 0 <= i__2 ? 
				    i__2 : s_rnge("parsed", i__2, "convrt_3__"
				    , (ftnlen)661)] = parsed[(i__3 = active + 
				    5) < 134 && 0 <= i__3 ? i__3 : s_rnge(
				    "parsed", i__3, "convrt_3__", (ftnlen)661)
				    ] / parsed[(i__4 = r__ + 5) < 134 && 0 <= 
				    i__4 ? i__4 : s_rnge("parsed", i__4, 
				    "convrt_3__", (ftnlen)661)];
			    dimen[(i__2 = class__) < 6 && 0 <= i__2 ? i__2 : 
				    s_rnge("dimen", i__2, "convrt_3__", (
				    ftnlen)662)] = dimen[(i__3 = class__) < 6 
				    && 0 <= i__3 ? i__3 : s_rnge("dimen", 
				    i__3, "convrt_3__", (ftnlen)662)] - 
				    expont[(i__4 = r__ + 5) < 134 && 0 <= 
				    i__4 ? i__4 : s_rnge("expont", i__4, 
				    "convrt_3__", (ftnlen)662)];
			}
			j += 2;
		    }

/*                 Finally, save the first value of the group, end the */
/*                 group, and append the saved value to the previous */
/*                 group. */

		    keep = parsed[(i__2 = active + 5) < 134 && 0 <= i__2 ? 
			    i__2 : s_rnge("parsed", i__2, "convrt_3__", (
			    ftnlen)675)];
		    podegd_(parsed);
		    podegi_(clas_s__);
		    podegd_(expont);
		    appndd_(&keep, parsed);
		    appndi_(&c__0, clas_s__);
		    appndd_(&c_b28, expont);
		}
	    }

/*           Just in case there are any left-overs, scan the */
/*           string for more tokens */

	    if (inout == 1) {
		scan_(unin, op, oplen, opptr, &c__128, &start, &ntokns, ident,
			 beg, end, unin_len, (ftnlen)2);
	    } else if (inout == 2) {
		scan_(unout, op, oplen, opptr, &c__128, &start, &ntokns, 
			ident, beg, end, unout_len, (ftnlen)2);
	    }

/*           If there are no more tokens left, we need to be sure */
/*           to close the last group (the one we opened before we */
/*           had even begun to scan UNIN or UNOUT. */

	    if (ntokns == 0 && ! done) {
		done = TRUE_;
		ntokns = 1;
		ident[0] = rparen;
	    }
	}

/*        Put the result of the parse into the input or output storage */
/*        area as appropriate. */

	if (inout == 1) {
	    dimeni[1] = dimen[1];
	    dimeni[2] = dimen[2];
	    dimeni[3] = dimen[3];
	    dimeni[4] = dimen[4];
	    dimeni[5] = dimen[5];
	    inval = parsed[6];
	} else if (inout == 2) {
	    dimeno[1] = dimen[1];
	    dimeno[2] = dimen[2];
	    dimeno[3] = dimen[3];
	    dimeno[4] = dimen[4];
	    dimeno[5] = dimen[5];
	    outval = parsed[6];
	}

/*        Finally, if this is only the first of the units that needs to */
/*        be parsed, loop back through the code above a second time. */

    }

/*     One final check must be performed.  The input and output */
/*     units must be dimensionally equivalent. */

    for (i__ = 1; i__ <= 5; ++i__) {
	if (dimeni[(i__1 = i__) < 6 && 0 <= i__1 ? i__1 : s_rnge("dimeni", 
		i__1, "convrt_3__", (ftnlen)765)] != dimeno[(i__2 = i__) < 6 
		&& 0 <= i__2 ? i__2 : s_rnge("dimeno", i__2, "convrt_3__", (
		ftnlen)765)]) {
	    diff = dimeni[(i__1 = i__) < 6 && 0 <= i__1 ? i__1 : s_rnge("dim"
		    "eni", i__1, "convrt_3__", (ftnlen)767)] - dimeno[(i__2 = 
		    i__) < 6 && 0 <= i__2 ? i__2 : s_rnge("dimeno", i__2, 
		    "convrt_3__", (ftnlen)767)];
	    *status = 3;
	    chkout_("CONVRT_3", (ftnlen)8);
	    return 0;
	}
    }

/*     That was the last hurdle,  now we can just compute the output. */

    *xout = inval / outval * *xin;
    *status = 0;
    chkout_("CONVRT_3", (ftnlen)8);
    return 0;
} /* convrt_3__ */

