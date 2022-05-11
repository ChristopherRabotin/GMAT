/* mkgrid.f -- translated by f2c (version 19980913).
   You must link the resulting object file with the libraries:
	-lf2c -lm   (in that order)
*/

#include "f2c.h"

/* Table of constant values */

static logical c_true = TRUE_;
static doublereal c_b78 = 0.;
static logical c_false = FALSE_;
static integer c__5 = 5;

/* $Procedure MKGRID ( MKDSK: create plate set from height grid ) */
/* Subroutine */ int mkgrid_(char *infile__, integer *plttyp, char *aunits, 
	char *dunits, integer *corsys, doublereal *corpar, integer *maxnv, 
	integer *maxnp, integer *nv, doublereal *verts, integer *np, integer *
	plates, ftnlen infile_len, ftnlen aunits_len, ftnlen dunits_len)
{
    /* Initialized data */

    static char sysnms[15*4] = "Latitudinal    " "Cylindrical    " "Rectangu"
	    "lar    " "Planetodetic   ";

    /* System generated locals */
    integer i__1;
    doublereal d__1;

    /* Builtin functions */
    double sin(doublereal);
    integer s_rnge(char *, integer, char *, integer);

    /* Local variables */
    integer nmid;
    logical wrap;
    extern /* Subroutine */ int vequ_(doublereal *, doublereal *), zzcapplt_(
	    integer *, logical *, logical *, integer *, integer *, integer *, 
	    integer *), zzgrdplt_(integer *, integer *, logical *, integer *, 
	    integer *);
    integer b, i__, j;
    extern /* Subroutine */ int getg05_(integer *, logical *, logical *, 
	    logical *, logical *, logical *, logical *, doublereal *, 
	    doublereal *, integer *, integer *, doublereal *, doublereal *, 
	    doublereal *, doublereal *), chkin_(char *, ftnlen), errch_(char *
	    , char *, ftnlen, ftnlen), vpack_(doublereal *, doublereal *, 
	    doublereal *, doublereal *), errdp_(char *, doublereal *, ftnlen);
    logical npole;
    integer ncols;
    logical spole;
    integer reqnp, reqnv;
    extern doublereal vnorm_(doublereal *);
    integer nrows;
    extern logical failed_(void);
    doublereal hscale;
    extern doublereal halfpi_(void);
    extern logical return_(void);
    doublereal botlat;
    extern doublereal dpr_(void);
    doublereal colstp, lftcor, refval, rowstp, topcor, toplat;
    integer pltbas, nnorth, npolar, nsouth, polidx, reqrow;
    logical leftrt;
    doublereal sum;
    logical mkncap, mkscap, rowmaj, topdwn;
    extern /* Subroutine */ int chkout_(char *, ftnlen), setmsg_(char *, 
	    ftnlen), errint_(char *, integer *, ftnlen), sigerr_(char *, 
	    ftnlen), convrt_(doublereal *, char *, char *, doublereal *, 
	    ftnlen, ftnlen), mkvarr_(char *, char *, char *, logical *, 
	    logical *, logical *, integer *, doublereal *, doublereal *, 
	    doublereal *, integer *, integer *, doublereal *, doublereal *, 
	    doublereal *, doublereal *, integer *, doublereal *, ftnlen, 
	    ftnlen, ftnlen);

/* $ Abstract */

/*     Create a DSK type 2 plate set from a height grid provided */
/*     in a file. */

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

/*     DSK */

/* $ Keywords */

/*     MKDSK */

/* $ Declarations */

/*     Include file dsk02.inc */

/*     This include file declares parameters for DSK data type 2 */
/*     (plate model). */

/* -       SPICELIB Version 1.0.0 08-FEB-2017 (NJB) */

/*          Updated version info. */

/*           22-JAN-2016 (NJB) */

/*              Now includes spatial index parameters. */

/*           26-MAR-2015 (NJB) */

/*              Updated to increase MAXVRT to 16000002. MAXNPV */
/*              has been changed to (3/2)*MAXPLT. Set MAXVOX */
/*              to 100000000. */

/*           13-MAY-2010 (NJB) */

/*              Updated to reflect new no-record design. */

/*           04-MAY-2010 (NJB) */

/*              Updated for new type 2 segment design. Now uses */
/*              a local parameter to represent DSK descriptor */
/*              size (NB). */

/*           13-SEP-2008 (NJB) */

/*              Updated to remove albedo information. */
/*              Updated to use parameter for DSK descriptor size. */

/*           27-DEC-2006 (NJB) */

/*              Updated to remove minimum and maximum radius information */
/*              from segment layout.  These bounds are now included */
/*              in the segment descriptor. */

/*           26-OCT-2006 (NJB) */

/*              Updated to remove normal, center, longest side, albedo, */
/*              and area keyword parameters. */

/*           04-AUG-2006 (NJB) */

/*              Updated to support coarse voxel grid.  Area data */
/*              have now been removed. */

/*           10-JUL-2006 (NJB) */


/*     Each type 2 DSK segment has integer, d.p., and character */
/*     components.  The segment layout in DAS address space is as */
/*     follows: */


/*        Integer layout: */

/*           +-----------------+ */
/*           | NV              |  (# of vertices) */
/*           +-----------------+ */
/*           | NP              |  (# of plates ) */
/*           +-----------------+ */
/*           | NVXTOT          |  (total number of voxels) */
/*           +-----------------+ */
/*           | VGREXT          |  (voxel grid extents, 3 integers) */
/*           +-----------------+ */
/*           | CGRSCL          |  (coarse voxel grid scale, 1 integer) */
/*           +-----------------+ */
/*           | VOXNPT          |  (size of voxel-plate pointer list) */
/*           +-----------------+ */
/*           | VOXNPL          |  (size of voxel-plate list) */
/*           +-----------------+ */
/*           | VTXNPL          |  (size of vertex-plate list) */
/*           +-----------------+ */
/*           | PLATES          |  (NP 3-tuples of vertex IDs) */
/*           +-----------------+ */
/*           | VOXPTR          |  (voxel-plate pointer array) */
/*           +-----------------+ */
/*           | VOXPLT          |  (voxel-plate list) */
/*           +-----------------+ */
/*           | VTXPTR          |  (vertex-plate pointer array) */
/*           +-----------------+ */
/*           | VTXPLT          |  (vertex-plate list) */
/*           +-----------------+ */
/*           | CGRPTR          |  (coarse grid occupancy pointers) */
/*           +-----------------+ */



/*        D.p. layout: */

/*           +-----------------+ */
/*           | DSK descriptor  |  DSKDSZ elements */
/*           +-----------------+ */
/*           | Vertex bounds   |  6 values (min/max for each component) */
/*           +-----------------+ */
/*           | Voxel origin    |  3 elements */
/*           +-----------------+ */
/*           | Voxel size      |  1 element */
/*           +-----------------+ */
/*           | Vertices        |  3*NV elements */
/*           +-----------------+ */


/*     This local parameter MUST be kept consistent with */
/*     the parameter DSKDSZ which is declared in dskdsc.inc. */


/*     Integer item keyword parameters used by fetch routines: */


/*     Double precision item keyword parameters used by fetch routines: */


/*     The parameters below formerly were declared in pltmax.inc. */

/*     Limits on plate model capacity: */

/*     The maximum number of bodies, vertices and */
/*     plates in a plate model or collective thereof are */
/*     provided here. */

/*     These values can be used to dimension arrays, or to */
/*     use as limit checks. */

/*     The value of MAXPLT is determined from MAXVRT via */
/*     Euler's Formula for simple polyhedra having triangular */
/*     faces. */

/*     MAXVRT is the maximum number of vertices the triangular */
/*            plate model software will support. */


/*     MAXPLT is the maximum number of plates that the triangular */
/*            plate model software will support. */


/*     MAXNPV is the maximum allowed number of vertices, not taking into */
/*     account shared vertices. */

/*     Note that this value is not sufficient to create a vertex-plate */
/*     mapping for a model of maximum plate count. */


/*     MAXVOX is the maximum number of voxels. */


/*     MAXCGR is the maximum size of the coarse voxel grid. */


/*     MAXEDG is the maximum allowed number of vertex or plate */
/*     neighbors a vertex may have. */

/*     DSK type 2 spatial index parameters */
/*     =================================== */

/*        DSK type 2 spatial index integer component */
/*        ------------------------------------------ */

/*           +-----------------+ */
/*           | VGREXT          |  (voxel grid extents, 3 integers) */
/*           +-----------------+ */
/*           | CGRSCL          |  (coarse voxel grid scale, 1 integer) */
/*           +-----------------+ */
/*           | VOXNPT          |  (size of voxel-plate pointer list) */
/*           +-----------------+ */
/*           | VOXNPL          |  (size of voxel-plate list) */
/*           +-----------------+ */
/*           | VTXNPL          |  (size of vertex-plate list) */
/*           +-----------------+ */
/*           | CGRPTR          |  (coarse grid occupancy pointers) */
/*           +-----------------+ */
/*           | VOXPTR          |  (voxel-plate pointer array) */
/*           +-----------------+ */
/*           | VOXPLT          |  (voxel-plate list) */
/*           +-----------------+ */
/*           | VTXPTR          |  (vertex-plate pointer array) */
/*           +-----------------+ */
/*           | VTXPLT          |  (vertex-plate list) */
/*           +-----------------+ */


/*        Index parameters */


/*     Grid extent: */


/*     Coarse grid scale: */


/*     Voxel pointer count: */


/*     Voxel-plate list count: */


/*     Vertex-plate list count: */


/*     Coarse grid pointers: */


/*     Size of fixed-size portion of integer component: */


/*        DSK type 2 spatial index double precision component */
/*        --------------------------------------------------- */

/*           +-----------------+ */
/*           | Vertex bounds   |  6 values (min/max for each component) */
/*           +-----------------+ */
/*           | Voxel origin    |  3 elements */
/*           +-----------------+ */
/*           | Voxel size      |  1 element */
/*           +-----------------+ */



/*        Index parameters */

/*     Vertex bounds: */


/*     Voxel grid origin: */


/*     Voxel size: */


/*     Size of fixed-size portion of double precision component: */


/*     The limits below are used to define a suggested maximum */
/*     size for the integer component of the spatial index. */


/*     Maximum number of entries in voxel-plate pointer array: */


/*     Maximum cell size: */


/*     Maximum number of entries in voxel-plate list: */


/*     Spatial index integer component size: */


/*     End of include file dsk02.inc */


/*     Include file dskdsc.inc */

/*     This include file declares parameters for DSK segment descriptors. */

/* -       SPICELIB Version 1.0.0 08-FEB-2017 (NJB) */

/*           Updated version info. */

/*           22-JAN-2016 (NJB) */

/*              Added parameter for data class 2. Changed name of data */
/*              class 1 parameter. Corrected data class descriptions. */

/*           13-MAY-2010 (NJB) */

/*              Descriptor now contains two ID codes, one for the */
/*              surface, one for the associated ephemeris object. This */
/*              supports association of multiple surfaces with one */
/*              ephemeris object without creating file management */
/*              issues. */

/*              Room was added for coordinate system definition */
/*              parameters. */

/*               Flag arrays and model ID/component entries were deleted. */

/*            11-SEP-2008 (NJB) */


/*     DSK segment descriptors are implemented as an array of d.p. */
/*     numbers.  Note that each integer descriptor datum occupies one */
/*     d.p. value. */




/*     Segment descriptor parameters */

/*     Each segment descriptor occupies a contiguous */
/*     range of DAS d.p. addresses. */

/*        The DSK segment descriptor layout is: */

/*           +---------------------+ */
/*           | Surface ID code     | */
/*           +---------------------+ */
/*           | Center ID code      | */
/*           +---------------------+ */
/*           | Data class code     | */
/*           +---------------------+ */
/*           | Data type           | */
/*           +---------------------+ */
/*           | Ref frame code      | */
/*           +---------------------+ */
/*           | Coord sys code      | */
/*           +---------------------+ */
/*           | Coord sys parameters|  {10 elements} */
/*           +---------------------+ */
/*           | Min coord 1         | */
/*           +---------------------+ */
/*           | Max coord 1         | */
/*           +---------------------+ */
/*           | Min coord 2         | */
/*           +---------------------+ */
/*           | Max coord 2         | */
/*           +---------------------+ */
/*           | Min coord 3         | */
/*           +---------------------+ */
/*           | Max coord 3         | */
/*           +---------------------+ */
/*           | Start time          | */
/*           +---------------------+ */
/*           | Stop time           | */
/*           +---------------------+ */

/*     Parameters defining offsets for segment descriptor elements */
/*     follow. */


/*     Surface ID code: */


/*     Central ephemeris object NAIF ID: */


/*     Data class: */

/*     The "data class" is a code indicating the category of */
/*     data contained in the segment. */


/*     Data type: */


/*     Frame ID: */


/*     Coordinate system code: */


/*     Coordinate system parameter start index: */


/*     Number of coordinate system parameters: */


/*     Ranges for coordinate bounds: */


/*     Coverage time bounds: */


/*     Descriptor size (24): */


/*     Data class values: */

/*        Class 1 indicates a surface that can be represented as a */
/*                single-valued function of its domain coordinates. */

/*                An example is a surface defined by a function that */
/*                maps each planetodetic longitude and latitude pair to */
/*                a unique altitude. */


/*        Class 2 indicates a general surface. Surfaces that */
/*                have multiple points for a given pair of domain */
/*                coordinates---for example, multiple radii for a given */
/*                latitude and longitude---belong to class 2. */



/*     Coordinate system values: */

/*        The coordinate system code indicates the system to which the */
/*        tangential coordinate bounds belong. */

/*        Code 1 refers to the planetocentric latitudinal system. */

/*        In this system, the first tangential coordinate is longitude */
/*        and the second tangential coordinate is latitude. The third */
/*        coordinate is radius. */



/*        Code 2 refers to the cylindrical system. */

/*        In this system, the first tangential coordinate is radius and */
/*        the second tangential coordinate is longitude. The third, */
/*        orthogonal coordinate is Z. */



/*        Code 3 refers to the rectangular system. */

/*        In this system, the first tangential coordinate is X and */
/*        the second tangential coordinate is Y. The third, */
/*        orthogonal coordinate is Z. */



/*        Code 4 refers to the planetodetic/geodetic system. */

/*        In this system, the first tangential coordinate is longitude */
/*        and the second tangential coordinate is planetodetic */
/*        latitude. The third, orthogonal coordinate is altitude. */



/*     End of include file dskdsc.inc */


/*     File: dsktol.inc */


/*     This file contains declarations of tolerance and margin values */
/*     used by the DSK subsystem. */

/*     It is recommended that the default values defined in this file be */
/*     changed only by expert SPICE users. */

/*     The values declared in this file are accessible at run time */
/*     through the routines */

/*        DSKGTL  {DSK, get tolerance value} */
/*        DSKSTL  {DSK, set tolerance value} */

/*     These are entry points of the routine DSKTOL. */

/*        Version 1.0.0 27-FEB-2016 (NJB) */




/*     Parameter declarations */
/*     ====================== */

/*     DSK type 2 plate expansion factor */
/*     --------------------------------- */

/*     The factor XFRACT is used to slightly expand plates read from DSK */
/*     type 2 segments in order to perform ray-plate intercept */
/*     computations. */

/*     This expansion is performed to prevent rays from passing through */
/*     a target object without any intersection being detected. Such */
/*     "false miss" conditions can occur due to round-off errors. */

/*     Plate expansion is done by computing the difference vectors */
/*     between a plate's vertices and the plate's centroid, scaling */
/*     those differences by (1 + XFRACT), then producing new vertices by */
/*     adding the scaled differences to the centroid. This process */
/*     doesn't affect the stored DSK data. */

/*     Plate expansion is also performed when surface points are mapped */
/*     to plates on which they lie, as is done for illumination angle */
/*     computations. */

/*     This parameter is user-adjustable. */


/*     The keyword for setting or retrieving this factor is */


/*     Greedy segment selection factor */
/*     ------------------------------- */

/*     The factor SGREED is used to slightly expand DSK segment */
/*     boundaries in order to select segments to consider for */
/*     ray-surface intercept computations. The effect of this factor is */
/*     to make the multi-segment intercept algorithm consider all */
/*     segments that are sufficiently close to the ray of interest, even */
/*     if the ray misses those segments. */

/*     This expansion is performed to prevent rays from passing through */
/*     a target object without any intersection being detected. Such */
/*     "false miss" conditions can occur due to round-off errors. */

/*     The exact way this parameter is used is dependent on the */
/*     coordinate system of the segment to which it applies, and the DSK */
/*     software implementation. This parameter may be changed in a */
/*     future version of SPICE. */


/*     The keyword for setting or retrieving this factor is */


/*     Segment pad margin */
/*     ------------------ */

/*     The segment pad margin is a scale factor used to determine when a */
/*     point resulting from a ray-surface intercept computation, if */
/*     outside the segment's boundaries, is close enough to the segment */
/*     to be considered a valid result. */

/*     This margin is required in order to make DSK segment padding */
/*     (surface data extending slightly beyond the segment's coordinate */
/*     boundaries) usable: if a ray intersects the pad surface outside */
/*     the segment boundaries, the pad is useless if the intercept is */
/*     automatically rejected. */

/*     However, an excessively large value for this parameter is */
/*     detrimental, since a ray-surface intercept solution found "in" a */
/*     segment can supersede solutions in segments farther from the */
/*     ray's vertex. Solutions found outside of a segment thus can mask */
/*     solutions that are closer to the ray's vertex by as much as the */
/*     value of this margin, when applied to a segment's boundary */
/*     dimensions. */

/*     The keyword for setting or retrieving this factor is */


/*     Surface-point membership margin */
/*     ------------------------------- */

/*     The surface-point membership margin limits the distance */
/*     between a point and a surface to which the point is */
/*     considered to belong. The margin is a scale factor applied */
/*     to the size of the segment containing the surface. */

/*     This margin is used to map surface points to outward */
/*     normal vectors at those points. */

/*     If this margin is set to an excessively small value, */
/*     routines that make use of the surface-point mapping won't */
/*     work properly. */


/*     The keyword for setting or retrieving this factor is */


/*     Angular rounding margin */
/*     ----------------------- */

/*     This margin specifies an amount by which angular values */
/*     may deviate from their proper ranges without a SPICE error */
/*     condition being signaled. */

/*     For example, if an input latitude exceeds pi/2 radians by a */
/*     positive amount less than this margin, the value is treated as */
/*     though it were pi/2 radians. */

/*     Units are radians. */


/*     This parameter is not user-adjustable. */

/*     The keyword for retrieving this parameter is */


/*     Longitude alias margin */
/*     ---------------------- */

/*     This margin specifies an amount by which a longitude */
/*     value can be outside a given longitude range without */
/*     being considered eligible for transformation by */
/*     addition or subtraction of 2*pi radians. */

/*     A longitude value, when compared to the endpoints of */
/*     a longitude interval, will be considered to be equal */
/*     to an endpoint if the value is outside the interval */
/*     differs from that endpoint by a magnitude less than */
/*     the alias margin. */


/*     Units are radians. */


/*     This parameter is not user-adjustable. */

/*     The keyword for retrieving this parameter is */


/*     End of include file dsktol.inc */

/* $ Abstract */

/*     Include Section:  MKDSK Global Parameters */

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

/* $ Author_and_Institution */

/*     N.J. Bachman   (JPL) */

/* $ Version */

/* -    Version 5.0.0, 01-DEC-2020 (NJB) */

/*        Updated number and date in version string parameter VER. */

/* -    Version 4.0.0, 28-FEB-2017 (NJB) */

/*        Added declaration of version string VER. Previously */
/*        this declaration was located in prcinf.for. */

/*        Declarations of parameters */

/*           MAXCEL */
/*           MAXVXP */
/*           MAXNVLS */

/*        were moved to dsk02.inc. */

/*        Declarations of parameters */

/*           ZZMAXV */
/*           ZZMAXP */

/*        were deleted. */


/* -    Version 3.0.0, 20-OCT-2015 (NJB) */

/*        Parameter MAXQ was increased from 512 to 1024. */

/* -    Version 2.0.0, 26-MAR-2015 (NJB) */

/*        Declarations were added for the parameters */

/*           MAXCEL */
/*           MAXVXP */
/*           MXNVLS */


/* -    Version 1.0.0, 04-MAY-2010 (NJB) */

/* -& */

/*     MKDSK version: */


/*     Default time format: */


/*     Command line length: */


/*     SPICELIB cell lower bound: */


/*     Maximum file name length: */


/*     Output file line length: */


/*     Length of string for keyword value processing: */


/*     The maximum 'Q' value for Gaskell shape models. This value */
/*     is always a multiple of 4. */


/*     Input format parameters */


/*     The height grid "plate type" is the fifth format. */


/*     End Include Section:  MKDSK Global Parameters */

/* $ Brief_I/O */

/*     VARIABLE  I/O  DESCRIPTION */
/*     --------  ---  -------------------------------------------------- */
/*     INFILE     I   Name of input file. */
/*     PLTTYP     I   MKDSK input file format code. */
/*     AUNITS     I   Angular units. */
/*     DUNITS     I   Distance units. */
/*     CORSYS     I   Coordinate system. */
/*     CORPAR     I   Coordinate parameters. */
/*     MAXNV      I   Maximum number of vertices. */
/*     MAXNP      I   Maximum number of plates. */
/*     NV         O   Number of vertices. */
/*     VERTS      O   Vertex array. */
/*     NP         O   Number of plates. */
/*     PLATES     O   Plate array. */

/* $ Detailed_Input */

/*     INFILE     is the name of an input data file containing height */
/*                grid data. */

/*     PLTTYP     is the MKDSK code indicating the format of the input */
/*                data file. */

/*     AUNITS     is the name of the angular unit associated with the */
/*                grid coordinates, if the grid coordinate system is */
/*                latitudinal or planetodetic. AUNITS must be supported */
/*                by the SPICELIB routine CONVRT. */

/*     DUNITS     is the name of the distance unit associated with the */
/*                grid coordinates. DUNITS must be supported by the */
/*                SPICELIB routine CONVRT. */

/*     CORSYS     is a DSK subsystem code designating the coordinate */
/*                system of the input coordinates. */

/*     CORPAR     is an array containing parameters associated with */
/*                the input coordinate system. The contents of the */
/*                array are as described in the DSK include file */
/*                dskdsc.inc. */

/*     COORDS     is a pair of domain coordinates: these may be, */

/*                   - planetocentric longitude and latitude */

/*                   - planetodetic longitude and latitude */

/*                   - X and Y */

/*                For a given coordinate system, the order of the */
/*                elements of COORDS is that of the coordinate names in */
/*                the list above. */

/*     MAXNV      I   Maximum number of vertices to return. */

/*     MAXNP      I   Maximum number of plates to return. */


/* $ Detailed_Output */

/*     NV         is the number of vertices in VERTS. */

/*     VERTS      is an array of 3-dimensional vertices corresponding */
/*                to the height grid. */

/*                Units are km. */

/*     NP         is the number of plates in PLATES. */

/*     PLATES     is an array of plates representing a tessellation of */
/*                the height grid. */

/* $ Parameters */

/*     See the MKDSK include files */

/*        mkdsk.inc */
/*        mkdsk02.inc */

/*     and the DSK include file */

/*        dskdsc.inc */


/* $ Exceptions */

/*     1)  If a polar cap is created, either due to a setup file */
/*         command or due to a vertex row having polar latitude, there */
/*         must be at least one non-polar vertex row. If no polar cap is */
/*         created, there must be at least two non-polar vertex rows. */

/*         If either the row or column count is insufficient to define a */
/*         surface, the error SPICE(INVALIDCOUNT) is signaled. */

/*     2)  If longitude wrap is specified for a rectangular coordinate */
/*         system, the error SPICE(SPURIOUSKEYWORD) is signaled. */

/*     3)  If either the north or south polar cap flag is .TRUE., and */
/*         the coordinate system is rectangular, the error */
/*         SPICE(SPURIOUSKEYWORD) is signaled. */

/*     4)  If either the row or column step is not strictly positive, */
/*         the error SPICE(INVALIDSTEP) is signaled. */

/*     5)  If the height scale is not is not strictly positive, */
/*         the error SPICE(INVALIDSCALE) is signaled. */

/*     6)  If the coordinate system is latitudinal and the height */
/*         reference is negative, the error SPICE(INVALIDREFVAL) is */
/*         signaled. */

/*     7)  If the number of vertices that must be created exceeds */
/*         MAXNV, the error SPICE(TOOMANYVERTICES) is signaled. */

/*     8)  If the number of plates that must be created exceeds */
/*         MAXNP, the error SPICE(TOOMANYPLATES) is signaled. */

/*     9)  If the input file format code is not recognized, the */
/*         error SPICE(NOTSUPPORTED) is signaled. */

/*    10)  If an error occurs while reading the input file, the */
/*         error will be diagnosed by routines in the call tree */
/*         of this routine. */

/*    11)  If an error occurs while processing the setup file, the error */
/*         will be diagnosed by routines in the call tree of this */
/*         routine. */

/*    12)  If an error occurs while converting the input data to a */
/*         vertex array, the error will be diagnosed by routines in the */
/*         call tree of this routine. */

/* $ Files */

/*     The file specified by INFILE can have any of the attributes (one */
/*     choice from each row below): */

/*        row-major  or column-major */
/*        top-down   or bottom-up */
/*        left-right or right-left */

/*     The number of tokens per line may vary. The number need have no */
/*     particular relationship to the row or column dimensions of the */
/*     output grid. */

/*     The file must contain only tokens that can be read as double */
/*     precision values. No non-printing characters can be present in */
/*     the file. */

/*     Tokens can be delimited by blanks or commas. Tokens must not be */
/*     split across lines. */

/*     Blank lines are allowed; however, their use is discouraged */
/*     because they'll cause line numbers in diagnostic messages to */
/*     be out of sync with actual line numbers in the file. */

/*     The file must end with a line terminator. */

/* $ Particulars */

/*     None. */

/* $ Examples */

/*     See usage in the MKDSK routine ZZWSEG02. */

/* $ Restrictions */

/*     1) For use only within program MKDSK. */

/* $ Literature_References */

/*     None. */

/* $ Author_and_Institution */

/*     N.J. Bachman    (JPL) */

/* $ Version */

/* -    MKDSK Version 2.0.0, 26-DEC-2021 (NJB) */

/*        Now replaces rows at the poles, if present, with polar */
/*        vertices, and creates polar caps using these vertices and the */
/*        adjacent vertex rows. Polar vertices have heights equal to the */
/*        average height of vertices in the corresponding row. "Make */
/*        cap" commands in the MKDSK setup file are not used to invoke */
/*        this behavior. */
/*        Error checking was added for row latitudes outside the range */
/*        [-pi/2, pi/2], when the coordinate system is latitudinal or */
/*        geodetic. Error checking was added for the case where both */
/*        north and south vertex rows are at the poles, when the */
/*        coordinate system is latitudinal or geodetic. */

/*        Bug fix: corrected computation of required number of plates */
/*        used for overflow detection. */

/*        Corrected description of bad height reference error. */

/* -    MKDSK Version 1.0.0, 25-FEB-2017 (NJB) */

/* -& */

/*     SPICELIB functions */


/*     Local parameters */


/*     Local variables */


/*     Saved variables */


/*     Initial values */

    if (return_()) {
	return 0;
    }
    chkin_("MKGRID", (ftnlen)6);

/*     Initial values of the polar row flags. These flags indicate */
/*     whether a vertex row is present at the indicated pole. */

    npole = FALSE_;
    spole = FALSE_;

/*     If we support the requested grid type, process the data. */
/*     The type is contained in the PLTTYP argument. */

    if (*plttyp == 5) {

/*        Fetch grid parameters from the kernel pool. */

	getg05_(corsys, &wrap, &mkncap, &mkscap, &rowmaj, &topdwn, &leftrt, &
		refval, &hscale, &ncols, &nrows, &lftcor, &topcor, &colstp, &
		rowstp);
	if (failed_()) {
	    chkout_("MKGRID", (ftnlen)6);
	    return 0;
	}

/*        Perform checks common to all coordinate systems. */

/*        First make sure we're working with a recognized system. */

	if (*corsys != 1 && *corsys != 4 && *corsys != 3) {
	    setmsg_("Coordinate system code # is not recognized.", (ftnlen)43)
		    ;
	    errint_("#", corsys, (ftnlen)1);
	    sigerr_("SPICE(NOTSUPPORTED)", (ftnlen)19);
	    chkout_("MKGRID", (ftnlen)6);
	    return 0;
	}
	if (ncols < 2) {
	    setmsg_("Number of columns was #; must have at least two columns"
		    " to create a grid.", (ftnlen)73);
	    errint_("#", &ncols, (ftnlen)1);
	    sigerr_("SPICE(INVALIDCOUNT)", (ftnlen)19);
	    chkout_("MKGRID", (ftnlen)6);
	    return 0;
	}
	if (colstp <= 0.) {
	    setmsg_("Column step must be strictly positive but was #.", (
		    ftnlen)48);
	    errdp_("#", &colstp, (ftnlen)1);
	    sigerr_("SPICE(INVALIDSTEP)", (ftnlen)18);
	    chkout_("MKGRID", (ftnlen)6);
	    return 0;
	}
	if (rowstp <= 0.) {
	    setmsg_("Row step must be strictly positive but was #.", (ftnlen)
		    45);
	    errdp_("#", &rowstp, (ftnlen)1);
	    sigerr_("SPICE(INVALIDSTEP)", (ftnlen)18);
	    chkout_("MKGRID", (ftnlen)6);
	    return 0;
	}
	if (hscale <= 0.) {
	    setmsg_("Height scale must be strictly positive but was #.", (
		    ftnlen)49);
	    errdp_("#", &hscale, (ftnlen)1);
	    sigerr_("SPICE(INVALIDSCALE)", (ftnlen)19);
	    chkout_("MKGRID", (ftnlen)6);
	    return 0;
	}

/*        Let NPOLAR indicate the number of polar vertex rows. We'll use */
/*        this variable for all coordinate systems; it will remain at */
/*        zero when it does not apply. */

	npolar = 0;

/*        For safety, check the parameters that could get us into real */
/*        trouble. */

	if (*corsys == 3) {
	    if (nrows < 2) {
		setmsg_("Number of rows was #; must have at least two rows t"
			"o create a grid using the rectangular coordinate sys"
			"tem.", (ftnlen)107);
		errint_("#", &nrows, (ftnlen)1);
		sigerr_("SPICE(INVALIDCOUNT)", (ftnlen)19);
		chkout_("MKGRID", (ftnlen)6);
		return 0;
	    }

/*           The following error should be caught by GETG05, but we */
/*           check here for safety. */

	    if (wrap) {
		setmsg_("Longitude wrap is not applicable to the rectangular"
			" coordinate system.", (ftnlen)70);
		sigerr_("SPICE(SPURIOUSKEYWORD)", (ftnlen)22);
		chkout_("MKGRID", (ftnlen)6);
		return 0;
	    }
	    if (mkncap || mkscap) {
		setmsg_("Polar cap creation is not applicable to the rectang"
			"ular coordinate system.", (ftnlen)74);
		sigerr_("SPICE(SPURIOUSKEYWORD)", (ftnlen)22);
		chkout_("MKGRID", (ftnlen)6);
		return 0;
	    }
	} else if (*corsys == 1 || *corsys == 4) {

/*           We have latitudinal or planetodetic coordinates. */

/*           Make sure we have enough rows of vertices. We'll need to */
/*           know whether any rows are at the poles. */

/*           We have a special case for grids having rows at the poles. */
/*           We'll replace rows at the poles with single points. We'll */
/*           use only the non-polar rows to form the main grid. */

/*           If the top row is at the north pole, just use one vertex */
/*           from that row and make a polar cap using that vertex and */
/*           the next row to the south. */

	    convrt_(&topcor, aunits, "RADIANS", &toplat, aunits_len, (ftnlen)
		    7);
	    if (toplat > halfpi_() + 1e-12) {
		setmsg_("Northernmost vertex row latitude is # degrees (# ra"
			"dians).", (ftnlen)58);
		d__1 = toplat * dpr_();
		errdp_("#", &d__1, (ftnlen)1);
		errdp_("#", &toplat, (ftnlen)1);
		sigerr_("SPICE(VALUEOUTOFRANGE)", (ftnlen)22);
		chkout_("MKGRID", (ftnlen)6);
		return 0;
	    } else {
/* Computing MIN */
		d__1 = halfpi_();
		toplat = min(d__1,toplat);
	    }
	    if (sin(toplat) == 1.) {

/*              We consider the top row to be at the pole. Note that */
/*              we use a single-precision criterion for latitude */
/*              because use inputs may have round-off errors of at */
/*              the single- precision level. */

		npole = TRUE_;

/*              Indicate we're making a north polar cap. */

		mkncap = TRUE_;
	    }

/*           Convert the latitude of the bottom row to radians. */

	    d__1 = topcor - (nrows - 1) * rowstp;
	    convrt_(&d__1, aunits, "RADIANS", &botlat, aunits_len, (ftnlen)7);
	    if (botlat < -halfpi_() - 1e-12) {
		setmsg_("Southernmost vertex row latitude is # degrees (# ra"
			"dians).", (ftnlen)58);
		d__1 = botlat * dpr_();
		errdp_("#", &d__1, (ftnlen)1);
		errdp_("#", &botlat, (ftnlen)1);
		sigerr_("SPICE(VALUEOUTOFRANGE)", (ftnlen)22);
		chkout_("MKGRID", (ftnlen)6);
		return 0;
	    } else {
/* Computing MAX */
		d__1 = -halfpi_();
		botlat = max(d__1,botlat);
	    }
	    if (sin(botlat) == -1.) {

/*              We consider the bottom row to be at the pole. Note */
/*              that we use a single-precision criterion for latitude */
/*              because use inputs may have round-off errors of at */
/*              the single- precision level. */

		spole = TRUE_;

/*              Indicate we're making a south polar cap. */

		mkscap = TRUE_;
	    }

/*           Check the number of available rows. If we're creating */
/*           at least one polar cap, there must be at least one row */
/*           that's at a latitude between the poles. */

	    if (mkncap || mkscap) {

/*              The default number of required rows is 1, when we create */
/*              a polar cap. Rows at the poles don't count. */

		reqrow = 1;
		if (npole && spole) {
		    reqrow = 3;
		    npolar = 2;
		} else if (npole || spole) {
		    reqrow = 2;
		    npolar = 1;
		}
		if (nrows < reqrow) {
		    setmsg_("Number of vertex grid rows was #; required numb"
			    "er of rows was #. Number of polar rows was #. Th"
			    "ere must be at least 1 non-polar row to create a"
			    " grid having at least one polar cap. Coordinate "
			    "system was #.", (ftnlen)204);
		    errint_("#", &nrows, (ftnlen)1);
		    errint_("#", &reqrow, (ftnlen)1);
		    errint_("#", &npolar, (ftnlen)1);
		    errch_("#", sysnms + ((i__1 = *corsys - 1) < 4 && 0 <= 
			    i__1 ? i__1 : s_rnge("sysnms", i__1, "mkgrid_", (
			    ftnlen)606)) * 15, (ftnlen)1, (ftnlen)15);
		    sigerr_("SPICE(INVALIDCOUNT)", (ftnlen)19);
		    chkout_("MKGRID", (ftnlen)6);
		    return 0;
		}
	    } else {

/*              No polar caps are being created. */

		if (nrows < 2) {
		    setmsg_("Number of rows was #; must have at least two ro"
			    "ws to create a grid using the # coordinate syste"
			    "m when no polar caps are created.", (ftnlen)128);
		    errint_("#", &nrows, (ftnlen)1);
		    errch_("#", sysnms + ((i__1 = *corsys - 1) < 4 && 0 <= 
			    i__1 ? i__1 : s_rnge("sysnms", i__1, "mkgrid_", (
			    ftnlen)624)) * 15, (ftnlen)1, (ftnlen)15);
		    sigerr_("SPICE(INVALIDCOUNT)", (ftnlen)19);
		    chkout_("MKGRID", (ftnlen)6);
		    return 0;
		}
	    }
	}

/*        For latitudinal coordinates, the height reference value */
/*        must be non-negative. This constraint does not apply to the */
/*        height reference value for rectangular coordinates. The height */
/*        reference does not apply to planetodetic coordinates; for that */
/*        system, the reference spheroid serves as the height reference. */

	if (*corsys == 1) {
	    if (refval < 0.) {
		setmsg_("For latitudinal coordinates, the height reference v"
			"alue must be non-negative. It was #.", (ftnlen)87);
		errdp_("#", &refval, (ftnlen)1);
		sigerr_("SPICE(INVALIDREFVAL)", (ftnlen)20);
		chkout_("MKGRID", (ftnlen)6);
		return 0;
	    }
	}

/*        Now proceed with vertex and plate construction. All coordinate */
/*        systems will be handled together. */

/*        Let REQNV and REQNP be, respectively, the numbers of vertices */
/*        and plates we need to create. Make sure we can handle these */
/*        numbers. */

	*nv = nrows * ncols;
	reqnv = *nv;
	reqnp = (nrows - 1 << 1) * (ncols - 1);
	if (wrap) {
	    reqnp += nrows - 1 << 1;
	}
	if (mkncap) {
	    ++reqnv;
	    reqnp = reqnp + ncols - 1;
	    if (wrap) {
		++reqnp;
	    }
	}
	if (mkscap) {
	    ++reqnv;
	    reqnp = reqnp + ncols - 1;
	    if (wrap) {
		++reqnp;
	    }
	}
	if (reqnv > *maxnv) {
	    setmsg_("The number of vertices that must be created is #. The m"
		    "aximum allowed number is #.", (ftnlen)82);
	    errint_("#", &reqnv, (ftnlen)1);
	    errint_("#", maxnv, (ftnlen)1);
	    sigerr_("SPICE(TOOMANYVERTICES)", (ftnlen)22);
	    chkout_("MKGRID", (ftnlen)6);
	    return 0;
	}

/*        Due to Euler's formula for polyhedra (V+F-E = 2), we don't */
/*        expect the condition of the next test to be met. The test */
/*        could be relevant if MAXNP is less than the value derived */
/*        from MAXNV by applying Euler's formula. */

	if (reqnp > *maxnp) {
	    setmsg_("The number of plates that must be created is #. The max"
		    "imum allowed number is #.", (ftnlen)80);
	    errint_("#", &reqnp, (ftnlen)1);
	    errint_("#", maxnp, (ftnlen)1);
	    sigerr_("SPICE(TOOMANYPLATES)", (ftnlen)20);
	    chkout_("MKGRID", (ftnlen)6);
	    return 0;
	}

/*        Create vertices. If we're making a north polar cap, leave */
/*        room for it at the start of the vertex array. We'll create */
/*        that vertex later. */

/*        At this point, we don't yet account for deletion of the */
/*        vertices of the top row, if the top row is at the pole. */

	if (mkncap) {

/*           B is the index of the first vertex to be computed by */
/*           MKVARR. */

	    b = 2;

/*           Update NV to indicate the number of occupied vertices */
/*           of the VERTS array after the MKVARR call below. The first */
/*           vertex is not set yet. */

	    ++(*nv);
	} else {
	    b = 1;
	}
	mkvarr_(infile__, aunits, dunits, &rowmaj, &topdwn, &leftrt, corsys, 
		corpar, &refval, &hscale, &ncols, &nrows, &lftcor, &topcor, &
		colstp, &rowstp, maxnv, &verts[b * 3 - 3], infile_len, 
		aunits_len, dunits_len);
	if (failed_()) {
	    chkout_("MKGRID", (ftnlen)6);
	    return 0;
	}

/*        The output vertices have units of km. */

/*        Make plates. Fill in the polar vertices, if they're needed. */

/*        We create the plates in top-down order, so the polar caps */
/*        will be adjacent to the nearby non-polar plates. */

	pltbas = 1;
	nnorth = 0;
	nsouth = 0;
	*np = 0;
	if (mkncap) {

/*           Create the plates of the north polar cap. Note that the */
/*           polar vertex has not been computed yet. */

	    polidx = 1;
	    zzcapplt_(&ncols, &c_true, &wrap, &pltbas, &polidx, &nnorth, 
		    plates);
	    *np = nnorth;

/*           The north vertex magnitude is the average of the magnitudes */
/*           of the vertices in the top row. */

/*           Note that we still use the vertices in the top row */
/*           to compute this average if the top row is at the pole. */

	    sum = 0.;
	    i__1 = ncols;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		j = b - 1 + i__;
		sum += vnorm_(&verts[j * 3 - 3]);
	    }
	    d__1 = sum / ncols;
	    vpack_(&c_b78, &c_b78, &d__1, verts);
	    if (npole) {

/*              The top row of the grid is at +90 degrees latitude, */
/*              within single precision round-off error. */

/*              Now that we've computed a radius value for the vertex at */
/*              the north pole, we have no need of the vertices in the */
/*              top row. We'll compress them out of the VERTS array. */
/*              Note that the top row starts at the second vertex of */
/*              VERTS. */

		--nrows;
		*nv -= ncols;
		i__1 = *nv;
		for (i__ = 2; i__ <= i__1; ++i__) {
		    vequ_(&verts[(i__ + ncols) * 3 - 3], &verts[i__ * 3 - 3]);
		}
	    }
	}

/*        Create the non-polar grid, if we have enough rows for it. */
/*        First adjust the row count, if necessary, to account for */
/*        deletion of a row at latitude -90 degrees. */

	if (spole) {

/*           The bottom row of the grid is at -90 degrees latitude, */
/*           within single precision round-off error. */

/*           Don't use the bottom row as part of the non-polar grid. */
/*           This row will be used only to determine the south polar */
/*           vertex. */

	    --nrows;
	    *nv -= ncols;
	}

/*        Make the non-polar plates, if any. There must be at least two */
/*        non-polar vertex rows in order for there to be any bands of */
/*        plates that don't contact a pole. Note that the number of */
/*        rows has already been reduced by NPOLAR. */

	if (nrows > 1) {
	    zzgrdplt_(&nrows, &ncols, &wrap, &nmid, &plates[(*np + 1) * 3 - 3]
		    );
	    if (failed_()) {
		chkout_("MKGRID", (ftnlen)6);
		return 0;
	    }
	    *np += nmid;
	    if (mkncap) {

/*              Adjust the vertex indices in the plate set. */

		i__1 = *np;
		for (i__ = nnorth + 1; i__ <= i__1; ++i__) {
		    for (j = 1; j <= 3; ++j) {
			++plates[j + i__ * 3 - 4];
		    }
		}
	    }
	}
	if (mkscap) {

/*           Create the plates of the north polar cap. Note that the */
/*           polar vertex has not been computed yet. */

	    polidx = *nv + 1;
	    pltbas = b - 1 + (nrows - 1) * ncols;
	    zzcapplt_(&ncols, &c_false, &wrap, &pltbas, &polidx, &nsouth, &
		    plates[(*np + 1) * 3 - 3]);
	    *np += nsouth;

/*           The south vertex magnitude is the average of the */
/*           magnitudes of the vertices in the bottom row. */

/*           Note that we still use the vertices in the bottom row */
/*           to compute this average if the bottom row is at the pole. */

	    sum = 0.;
	    i__1 = ncols;
	    for (i__ = 1; i__ <= i__1; ++i__) {

/*              Pick vertices from the row just above the pole. */

		j = pltbas + i__;
		if (spole) {

/*                 Pick values from the row at the south pole. */

		    j += ncols;
		}
		sum += vnorm_(&verts[j * 3 - 3]);
	    }
	    d__1 = -sum / ncols;
	    vpack_(&c_b78, &c_b78, &d__1, &verts[polidx * 3 - 3]);
	    ++(*nv);
	}
    } else {

/*        This error should be caught in RDFFPL. Check here for safety. */

	setmsg_("Input data format type is #; only type # is supported.", (
		ftnlen)54);
	errint_("#", plttyp, (ftnlen)1);
	errint_("#", &c__5, (ftnlen)1);
	sigerr_("SPICE(NOTSUPPORTED)", (ftnlen)19);
	chkout_("MKGRID", (ftnlen)6);
	return 0;
    }
    chkout_("MKGRID", (ftnlen)6);
    return 0;
} /* mkgrid_ */

