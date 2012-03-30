#include "sofam.h"
#include <string.h>

int iauD2dtf(char *scale, int ndp, double d1, double d2,
             int *iy, int *im, int *id, int ihmsf[4])
/*
**  - - - - - - - - -
**   i a u D 2 d t f
**  - - - - - - - - -
**
**  Format for output a 2-part Julian Date (or in the case of UTC a
**  quasi-JD form that includes special provision for leap seconds).
**
**  This function is part of the International Astronomical Union's
**  SOFA (Standards of Fundamental Astronomy) software collection.
**
**  Status:  support function.
**
**  Given:
**     scale     char[]  time scale ID (Note 1)
**     ndp       int     resolution (Note 2)
**     d1,d2     double  time as a 2-part Julian Date (Notes 3,4)
**
**  Returned:
**     iy,im,id  int     year, month, day in Gregorian calendar (Note 5)
**     ihmsf     int[4]  hours, minutes, seconds, fraction (Note 1)
**
**  Returned (function value):
**                int       status: +1 = dubious year (Note 5)
**                                   0 = OK
**                                  -1 = unacceptable date (Note 6)
**
**  Notes:
**
**  1) scale identifies the time scale.  Only the value "UTC" (in upper
**     case) is significant, and enables handling of leap seconds (see
**     Note 4).
**
**  2) ndp is the number of decimal places in the seconds field, and can
**     have negative as well as positive values, such as:
**
**     ndp         resolution
**     -4            1 00 00
**     -3            0 10 00
**     -2            0 01 00
**     -1            0 00 10
**      0            0 00 01
**      1            0 00 00.1
**      2            0 00 00.01
**      3            0 00 00.001
**
**     The limits are platform dependent, but a safe range is -5 to +9.
**
**  3) d1+d2 is Julian Date, apportioned in any convenient way between
**     the two arguments, for example where d1 is the Julian Day Number
**     and d2 is the fraction of a day.  In the case of UTC, where the
**     use of JD is problematical, special conventions apply:  see the
**     next note.
**
**  4) JD cannot unambiguously represent UTC during a leap second unless
**     special measures are taken.  The SOFA internal convention is that
**     the quasi-JD day represents UTC days whether the length is 86399,
**     86400 or 86401 SI seconds.
**
**  5) The warning status "dubious year" flags UTCs that predate the
**     introduction of the time scale and that are too far in the future
**     to be trusted.  See iauDat for further details.
**
**  6) For calendar conventions and limitations, see iauCal2jd.
**
**  Called:
**     iauJd2cal    JD to Gregorian calendar
**     iauD2tf      decompose days to hms
**     iauDat       delta(AT) = TAI-UTC
**
**  This revision:  2010 September 7
**
**  SOFA release 2010-12-01
**
**  Copyright (C) 2010 IAU SOFA Board.  See notes at end.
*/
{
   int leap;
   char s;
   int iy1, im1, id1, js, iy2, im2, id2, ihmsf1[4], i;
   double a1, b1, fd, dat1, w, dat2, ddt;


/* The two-part JD. */
   a1 = d1;
   b1 = d2;

/* Provisional calendar date. */
   js = iauJd2cal(a1, b1, &iy1, &im1, &id1, &fd);
   if ( js ) return js < 0 ? -1 : js;

/* Is this a leap second day? */
   leap = 0;
   if ( ! strcmp(scale,"UTC") ) {

   /* TAI-UTC today. */
      js = iauDat(iy1, im1, id1, fd, &dat1);
      if ( js < 0 ) return -1;

   /* TAI-UTC tomorrow (at noon, to avoid rounding effects). */
      js = iauJd2cal(a1+1.5, b1-fd, &iy2, &im2, &id2, &w);
      js = iauDat(iy2, im2, id2, 0.0, &dat2);
      if ( js < 0 ) return -1;

   /* The change in TAI-UTC (seconds). */
      ddt = dat2 - dat1;

   /* If leap second day, scale the fraction of a day into SI. */
      leap = fabs(ddt) > 0.5;
      if (leap) fd += fd * ddt/DAYSEC;
   }

/* Provisional time of day. */
   iauD2tf ( ndp, fd, &s, ihmsf1 );

/* Is this a leap second day? */
   if ( ! leap ) {

   /* No.  Has the time rounded up to 24h? */
      if ( ihmsf1[0] > 23 ) {

      /* Yes.  We will need tomorrow's calendar date. */
         js = iauJd2cal(a1+1.5, b1-fd, &iy2, &im2, &id2, &w);

      /* Use 0h tomorrow. */
         iy1 = iy2;
         im1 = im2;
         id1 = id2;
         for ( i = 0; i < 4; i++ ) {
            ihmsf1[i] = 0;
         }
      }
   } else {

   /* This is a leap second day.  Has the time reached or passed 24h? */
      if ( ihmsf1[0] > 23 ) {

      /* Yes.  Use 23 59 60... */
         ihmsf1[0] = 23;
         ihmsf1[1] = 59;
         ihmsf1[2] = 60;
      }
   }

/* Results. */
   *iy = iy1;
   *im = im1;
   *id = id1;
   for ( i = 0; i < 4; i++ ) {
      ihmsf[i] = ihmsf1[i];
   }

/* Status. */
   return js < 0 ? -1 : js;

/*----------------------------------------------------------------------
**
**  Copyright (C) 2010
**  Standards Of Fundamental Astronomy Board
**  of the International Astronomical Union.
**
**  =====================
**  SOFA Software License
**  =====================
**
**  NOTICE TO USER:
**
**  BY USING THIS SOFTWARE YOU ACCEPT THE FOLLOWING TERMS AND CONDITIONS
**  WHICH APPLY TO ITS USE.
**
**  1. The Software is owned by the IAU SOFA Board ("SOFA").
**
**  2. Permission is granted to anyone to use the SOFA software for any
**     purpose, including commercial applications, free of charge and
**     without payment of royalties, subject to the conditions and
**     restrictions listed below.
**
**  3. You (the user) may copy and distribute SOFA source code to others,
**     and use and adapt its code and algorithms in your own software,
**     on a world-wide, royalty-free basis.  That portion of your
**     distribution that does not consist of intact and unchanged copies
**     of SOFA source code files is a "derived work" that must comply
**     with the following requirements:
**
**     a) Your work shall be marked or carry a statement that it
**        (i) uses routines and computations derived by you from
**        software provided by SOFA under license to you; and
**        (ii) does not itself constitute software provided by and/or
**        endorsed by SOFA.
**
**     b) The source code of your derived work must contain descriptions
**        of how the derived work is based upon, contains and/or differs
**        from the original SOFA software.
**
**     c) The name(s) of all routine(s) in your derived work shall not
**        include the prefix "iau".
**
**     d) The origin of the SOFA components of your derived work must
**        not be misrepresented;  you must not claim that you wrote the
**        original software, nor file a patent application for SOFA
**        software or algorithms embedded in the SOFA software.
**
**     e) These requirements must be reproduced intact in any source
**        distribution and shall apply to anyone to whom you have
**        granted a further right to modify the source code of your
**        derived work.
**
**     Note that, as originally distributed, the SOFA software is
**     intended to be a definitive implementation of the IAU standards,
**     and consequently third-party modifications are discouraged.  All
**     variations, no matter how minor, must be explicitly marked as
**     such, as explained above.
**
**  4. In any published work or commercial products which includes
**     results achieved by using the SOFA software, you shall
**     acknowledge that the SOFA software was used in obtaining those
**     results.
**
**  5. You shall not cause the SOFA software to be brought into
**     disrepute, either by misuse, or use for inappropriate tasks, or
**     by inappropriate modification.
**
**  6. The SOFA software is provided "as is" and SOFA makes no warranty
**     as to its use or performance.   SOFA does not and cannot warrant
**     the performance or results which the user may obtain by using the
**     SOFA software.  SOFA makes no warranties, express or implied, as
**     to non-infringement of third party rights, merchantability, or
**     fitness for any particular purpose.  In no event will SOFA be
**     liable to the user for any consequential, incidental, or special
**     damages, including any lost profits or lost savings, even if a
**     SOFA representative has been advised of such damages, or for any
**     claim by any third party.
**
**  7. The provision of any version of the SOFA software under the terms
**     and conditions specified herein does not imply that future
**     versions will also be made available under the same terms and
**     conditions.
**
**  Correspondence concerning SOFA software should be addressed as
**  follows:
**
**      By email:  sofa@ukho.gov.uk
**      By post:   IAU SOFA Center
**                 HM Nautical Almanac Office
**                 UK Hydrographic Office
**                 Admiralty Way, Taunton
**                 Somerset, TA1 2DN
**                 United Kingdom
**
**--------------------------------------------------------------------*/
}
