//$Header$
//------------------------------------------------------------------------------
//                                  DeFile
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Wendy C. Shoan
// Created: 2004/03/01
//
/**
 * Implementation of the class that handles access to a DE file.
 *
 * @note The actual code that reads the DE file data, interpolates, etc.
 *       is from JPL/JSC (written by David Hoffman of JSC); I have modified
 *       it to not use macros, not print warning messages to the screen,
 *       use MAX_ARRAY_SIZE and arraySize, etc.
 *
 * @note For Build 2, only Sun/Earth/Moon are assumed.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "A1Mjd.hpp"
#include "UtcDate.hpp"
#include "Rvector6.hpp"
#include "SolarSystem.hpp"
#include "PlanetaryEphem.hpp"
#include "PlanetaryEphemException.hpp"
#include "TimeTypes.hpp"
#include "DeFile.hpp"

#if defined (__UNIT_TEST__)
#include <fstream> //for debug output
#endif

// DE file code from JPL/JSC (Hoffman) includes
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

const Integer DeFile::SUN_ID              = 10;
const Integer DeFile::MERCURY_ID          = 0;
const Integer DeFile::VENUS_ID            = 1;
const Integer DeFile::EARTH_ID            = 2;     // Earth-Moon barycenter
const Integer DeFile::MOON_ID             = 9;     // relative to geocenter
const Integer DeFile::MARS_ID             = 3;
const Integer DeFile::JUPITER_ID          = 4;
const Integer DeFile::SATURN_ID           = 5;
const Integer DeFile::URANUS_ID           = 6;
const Integer DeFile::NEPTUNE_ID          = 7;
const Integer DeFile::PLUTO_ID            = 8;

const Integer DeFile::SS_BARY_ID          = 11;
const Integer DeFile::EM_BARY_ID          = 12;
const Integer DeFile::NUTATIONS_ID        = 13;
const Integer DeFile::LIBRATIONS_ID       = 14;

//const Integer DeFile::ARRAY_SIZE_200      = 826;
//const Integer DeFile::ARRAY_SIZE_405      = 1018;
//const Integer DeFile::MAX_ARRAY_SIZE      = 1018;

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  DeFile(Gmat::DeFileType ofType, std::string fileName = "",
//         Gmat::DeFileFormat fmt = DE_BINARY)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the DeFile class
 * (default constructor).
 *
 * @param <ofType>   parameter indicating the type of De File.
 * @param <fileName> parameter indicating the full path nameof the file.
 * @param <fmt>      parameter indicating the format of the file.
 *
 * @note if an ASCII file is input on creation, it will be converted to
 *       a file in native binary format; the binary file will be read, for
 *       efficiency.
 */
//------------------------------------------------------------------------------
DeFile::DeFile(Gmat::DeFileType ofType, std::string fileName,
               Gmat::DeFileFormat fmt) :
PlanetaryEphem(fileName)
{
   defType = ofType;
   InitializeDeFile(fileName, fmt);
}

//------------------------------------------------------------------------------
//  DeFile(const DeFile& def)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the DeFile class using the values of the
 * input De File (copy constructor).
 *
 * @param <def> De file whose values to use to create this DE File.
 */
//------------------------------------------------------------------------------
DeFile::DeFile(const DeFile& def) :
PlanetaryEphem(def)
{
   asciiFileName  = def.asciiFileName;
   binaryFileName = def.binaryFileName;
   defType        = def.defType;
   arraySize      = def.arraySize;

   /// data from JPL/JSC code (Hoffman) ephem_read.c
   H1.data        = (def.H1).data;
   strcpy(H1.pad, (def.H1).pad);
   H2.data        = (def.H2).data;
   strcpy(H2.pad, (def.H2).pad);
   R1             = def.R1;
   Ephemeris_File = def.Ephemeris_File;
   int i;
   for (i=0;i<MAX_ARRAY_SIZE;i++)  Coeff_Array[i] = def.Coeff_Array[i];
   T_beg          = def.T_beg;
   T_end          = def.T_end;
   T_span         = def.T_span;

   EPHEMERIS      = def.EPHEMERIS; 
}

//------------------------------------------------------------------------------
//  DeFile& operator= (const DeFile& def)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the SlpFile class.
 *
 * @param <def> the DeFile object whose data to assign to "this"
 *            DE File.
 *
 * @return "this" DeFile with data of input DeFile def.
 */
//------------------------------------------------------------------------------
DeFile& DeFile::operator=(const DeFile& def)
{
   if (this == &def) return *this;
   PlanetaryEphem::operator=(def);
   asciiFileName  = def.asciiFileName;
   binaryFileName = def.binaryFileName;
   defType        = def.defType;
   arraySize      = def.arraySize;

   /// data from JPL/JSC code (Hoffman) ephem_read.c
   H1.data        = (def.H1).data;
   strcpy(H1.pad, (def.H1).pad);
   H2.data        = (def.H2).data;
   strcpy(H2.pad, (def.H2).pad);
   R1             = def.R1;
   Ephemeris_File = def.Ephemeris_File;
   int i;
   for (i=0;i<MAX_ARRAY_SIZE;i++)  Coeff_Array[i] = def.Coeff_Array[i];
   T_beg          = def.T_beg;
   T_end          = def.T_end;
   T_span         = def.T_span;

   EPHEMERIS      = def.EPHEMERIS;
   return *this;
}

//------------------------------------------------------------------------------
//  ~DeFile()
//------------------------------------------------------------------------------
/**
 * Destructor for the DeFile class.
 */
//------------------------------------------------------------------------------
DeFile::~DeFile()
{
   // nothing to do ... la la la la la
}

//------------------------------------------------------------------------------
//  Integer GetBodyID(std::string bodyName)
//------------------------------------------------------------------------------
/**
* This method returns the ID for the requested body.
 *
 * @param <bodyName> body whose ID is requested.
 *
 * @return the ID for the requested body. Returns -1 if invalid body name.
 *
 */
//------------------------------------------------------------------------------
Integer  DeFile::GetBodyID(std::string bodyName)
{
   if (bodyName == SolarSystem::SUN_NAME)     return DeFile::SUN_ID;
   if (bodyName == SolarSystem::MERCURY_NAME) return DeFile::MERCURY_ID;
   if (bodyName == SolarSystem::VENUS_NAME)   return DeFile::VENUS_ID;
   if (bodyName == SolarSystem::EARTH_NAME)   return DeFile::EARTH_ID;
   if (bodyName == SolarSystem::MOON_NAME)    return DeFile::MOON_ID;
   if (bodyName == SolarSystem::MARS_NAME)    return DeFile::MARS_ID;
   if (bodyName == SolarSystem::JUPITER_NAME) return DeFile::JUPITER_ID;
   if (bodyName == SolarSystem::SATURN_NAME)  return DeFile::SATURN_ID;
   if (bodyName == SolarSystem::URANUS_NAME)  return DeFile::URANUS_ID;
   if (bodyName == SolarSystem::NEPTUNE_NAME) return DeFile::NEPTUNE_ID;
   if (bodyName == SolarSystem::PLUTO_NAME)   return DeFile::PLUTO_ID;

   return -1;
}

//------------------------------------------------------------------------------
//  Real* GetPosVel(Integer forBody, A1Mjd atTime)
//------------------------------------------------------------------------------
/**
 * This method returns the position and velocity of the specified body at the
 * requested time.
 *
 * @param <forbody>  the requested body (ID number on DE file).
 * @param <atTime>   time for which state of the body is requested.
 *
 * @return state of the body at the requested time.
 *
 * @exception <PlanetaryEphemException> thrown if the position or velocity cannot
 *            be obtained due to problems opening or reading the DE Fle;
 *            message is based on error
 */
//------------------------------------------------------------------------------
Real* DeFile::GetPosVel(Integer forBody, A1Mjd atTime)
{
   Real*     result = new Real[6];
   // if we're asking for the Earth state, return 0.0 (since we're
   // currently assuming Earth-Centered Equatorial
   if (forBody == DeFile::EARTH_ID)
   {
      result[0] = 0.0;
      result[1] = 0.0;
      result[2] = 0.0;
      result[3] = 0.0;
      result[4] = 0.0;
      result[5] = 0.0;
      return result;
   }
   
   stateType* rv = new stateType;
   // add to the MJD to get the absolute Julian Date
   //double absJD = atTime.Get() + 2430000.5;
   //double absJD = atTime.Get() + 2430000.0;
   double absJD = atTime.Get() + jdMjdOffset;

   // if we're asking for the moon state, just get it and return it, as
   // it is supposed to be a geocentric state from the DE file

   // interpolate the data to get the state
   Interpolate_State(absJD, forBody, rv);

   if (forBody == DeFile::MOON_ID)
   {
      result[0] = (Real) rv->Position[0]; // temporary
      result[1] = (Real) rv->Position[1];
      result[2] = (Real) rv->Position[2];
      result[3] = (Real) rv->Velocity[0] ; //* GmatTimeUtil::SECS_PER_DAY;
      result[4] = (Real) rv->Velocity[1] ; //* GmatTimeUtil::SECS_PER_DAY;
      result[5] = (Real) rv->Velocity[2] ; //* GmatTimeUtil::SECS_PER_DAY;

      return result;
   }

   // otherwise, we must take the body's state (in Solar System Barycentric
   // coordinates), then get the Earth-Moon state in SSBarycentric,
   // then get the Earth state from that (using the Moon state in
   // geocentric), then figure out the body's state wrt the Earth
   stateType* emrv = new stateType;
   stateType* mrv  = new stateType;

   // earth-moon barycenter rel to solar system barycenter
   Interpolate_State(absJD,(int)DeFile::EARTH_ID, emrv);
   // moon state (geocentric)
   Interpolate_State(absJD,(int)DeFile::MOON_ID, mrv);

   // compute Earth state in Solar System barycenter coordinates
   Rvector6 earthMoonRv(emrv->Position[0],emrv->Position[1],emrv->Position[2],
                        emrv->Velocity[0],emrv->Velocity[1],emrv->Velocity[2]);
   Rvector6 moonRv(mrv->Position[0],mrv->Position[1],mrv->Position[2],
                   mrv->Velocity[0],mrv->Velocity[1],mrv->Velocity[2]);
   Rvector6 earthRv = earthMoonRv - (moonRv / R1.EMRAT);

   // now compute the state of the requested body wrt the Earth
   Rvector6 bodyRv(rv->Position[0],rv->Position[1],rv->Position[2],
                   rv->Velocity[0],rv->Velocity[1],rv->Velocity[2]);
   Rvector6 bodyWrtEarth = bodyRv - earthRv;

   result[0] = bodyWrtEarth.Get(0);
   result[1] = bodyWrtEarth.Get(1);
   result[2] = bodyWrtEarth.Get(2);
   result[3] = bodyWrtEarth.Get(3) ; //* GmatTimeUtil::SECS_PER_DAY;
   result[4] = bodyWrtEarth.Get(4) ; //* GmatTimeUtil::SECS_PER_DAY;
   result[5] = bodyWrtEarth.Get(5) ; //* GmatTimeUtil::SECS_PER_DAY;
   
   return result;
}

//------------------------------------------------------------------------------
//  Integer* GetStartDayAndYear()
//------------------------------------------------------------------------------
/**
 * This method returns the start day-of-year and year of the DE file.
 *
 * @return start day-of-year and year of the DE file.
 */
//------------------------------------------------------------------------------
Integer* DeFile::GetStartDayAndYear()
{
   Integer* res = new Integer[2];
   // first time on header is start time of file in absolute Julian Days
   Real mjd = (Real) R1.timeData[0] - jdMjdOffset;
   A1Mjd *a  = new A1Mjd(mjd);
   UtcDate uTime = a->ToUtcDate();
   Integer y, doy, h, min;
   Real    sec;
   uTime.ToYearDOYHourMinSec(y,doy,h,min,sec);
   res[0]   = doy;
   res[1]   = y;
   return res;
}


//------------------------------------------------------------------------------
//  std::string Convert(std::string deFileNameAscii)
//------------------------------------------------------------------------------
/**
 * This method converts the ASCII DE file (input at creation) to a native
 * binary format.  It assumes that an appropriate header file, with the name
 * header.FMT, where FMT is the type of DE file (e.g. 202), exists in the
 * same directory as the ASCII DE file.
 *
 * @param <deFileNaemAscii> ASCII file name (pull path name).
 *
 * @return the full path name of the binary file; to anme it, thie method will
 *        take the input ASCII file name and add 'Bin' at the end (before the
 *        .FMT).
 */
//------------------------------------------------------------------------------
std::string DeFile::Convert(std::string deFileNameAscii)
{
   // TBD - call VAL methods?  Recode VAL method(s)?
   return deFileNameAscii;
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
// private methods
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// void InitializeDeFile(std::string name)
//------------------------------------------------------------------------------
/**
 * This method initializes the data for the DE file.
 *
 * @exception <PlanetaryEphemException> thrown if there is an error initializing
 *            the DE file.
 */
//------------------------------------------------------------------------------
void DeFile::InitializeDeFile(std::string fName, Gmat::DeFileFormat fileFmt)
{
   if (fileFmt == Gmat::DE_ASCII)
   {
      asciiFileName  = fName;
      binaryFileName = Convert(asciiFileName);
   }
   else
   {
      asciiFileName  = "unknown";
      binaryFileName = fName;
   }

   if (defType == Gmat::DE200)
   {

      arraySize = DeFile::ARRAY_SIZE_200;
      EPHEMERIS = 200;
   }
   else if (defType == Gmat::DE405)
   {
      arraySize = DeFile::ARRAY_SIZE_405;
      EPHEMERIS = 405;
   }
   else
   {
      // ERROR!  Other formats not currently supported!!!
   }
   int worked = Initialize_Ephemeris((char*)binaryFileName.c_str());
   
   if (worked == FAILURE)
   {
      throw PlanetaryEphemException("DE file is not of specified format!!"
                                    "DE file not able to be initialized!");
   }
   itsName                             = binaryFileName;
   strcpy(g_pef_dcb.full_path,binaryFileName.c_str());
   g_pef_dcb.recl                      = arraySize;
   if (Ephemeris_File)  g_pef_dcb.fptr = Ephemeris_File;
   jdMjdOffset          = (double) DeFile::JD_MJD_OFFSET;
}

//------------------------------------------------------------------------------
// private methods from JPL/JSC code ephem_read.c
//------------------------------------------------------------------------------

/******************************************************************************/
/**                                                                          **/
/**  SOURCE FILE: ephem_read.c                                               **/
/**                                                                          **/
/**     This file contains a set of functions and global variables that      **/
/**     implements an ephemeris server program. Client programs can use      **/
/**     use this server to access ephemeris data by calling one of the       **/
/**     following functions:                                                 **/
/**                                                                          **/
/**        Interpolate_Libration -- returns lunar libration angles           **/
/**        Interpolate_Nutation  -- returns (terrestrial) nutation angles    **/
/**        Interpolate_Position  -- returns position of planet               **/
/**        Interpolate_State     -- returns position and velocity of planet  **/
/**                                                                          **/
/**     Note that client programs must make one, and only one, call to the   **/
/**     function Initialize_Ephemeris before any of these other functions    **/
/**     be used. After this call any of the above functions can be called    **/
/**     in any sequence.                                                     **/
/**                                                                          **/
/**  Programmer: David Hoffman/EG5                                           **/
/**              NASA, Johnson Space Center                                  **/
/**              Houston, TX 77058                                           **/
/**              e-mail: david.a.hoffman1@jsc.nasa.gov                       **/
/**                                                                          **/
/**  History:                                                                **/
/**                                                                          **/
/**    8/11/99  Changed made to function Read_Coefficients to ensure that    **/
/**             Offset has a negative value when requested time comes        **/
/**             before the beginning of the current record. Thanks to Gary   **/
/**             Bernstien (garyb@astro.lsa.umich.edu) for pointing out this  **/
/**             serious bug.                                                 **/
/**                                                                          **/
/******************************************************************************/

/**==========================================================================**/
/**  Read_Coefficients                                                       **/
/**                                                                          **/
/**     This function is used by the functions below to read an array of     **/
/**     Tchebeychev coefficients from a binary ephemeris data file.          **/
/**                                                                          **/
/**  Input: Desired record time.                                             **/
/**                                                                          **/
/**  Output: None.                                                           **/
/**                                                                          **/
/**==========================================================================**/

void DeFile::Read_Coefficients( double Time )
{
  double  T_delta = 0.0;
  int     Offset  =  0 ;

  /*--------------------------------------------------------------------------*/
  /*  Find ephemeris data that record contains input time. Note that one, and */
  /*  only one, of the following conditional statements will be true (if both */
  /*  were false, this function would not have been called).                  */
  /*--------------------------------------------------------------------------*/

  if ( Time < T_beg )                    /* Compute backwards location offset */
     {
       T_delta = T_beg - Time;
       Offset  = (int) -ceil(T_delta/T_span); 
     }

  if ( Time > T_end )                    /* Compute forewards location offset */
     {
       T_delta = Time - T_end;
       Offset  = (int) ceil(T_delta/T_span);
     }

  /*--------------------------------------------------------------------------*/
  /*  Retrieve ephemeris data from new record.                                */
  /*--------------------------------------------------------------------------*/

  //fseek(Ephemeris_File,(Offset-1)*ARRAY_SIZE*sizeof(double),SEEK_CUR);
  //fread(&Coeff_Array,sizeof(double),ARRAY_SIZE,Ephemeris_File);

  fseek(Ephemeris_File,(Offset-1)*arraySize*sizeof(double),SEEK_CUR); // wcs
  fread(&Coeff_Array,sizeof(double),arraySize,Ephemeris_File);        // wcs
  
  T_beg  = Coeff_Array[0];
  T_end  = Coeff_Array[1];
  T_span = T_end - T_beg;

  /*--------------------------------------------------------------------------*/
  /*  Debug print (optional)                                                  */
  /*--------------------------------------------------------------------------*/

  //if ( Debug ) 
  //   {
  //     printf("\n  In: Read_Coefficients \n");
  //     printf("\n      ARRAY_SIZE = %4d",ARRAY_SIZE);
  //     printf("\n      Offset  = %3d",Offset);
  //     printf("\n      T_delta = %7.3f",T_delta);
  //     printf("\n      T_Beg   = %7.3f",T_beg);
  //     printf("\n      T_End   = %7.3f",T_end);
  //     printf("\n      T_Span  = %7.3f\n\n",T_span);
  //   }

}

/**==========================================================================**/
/**  Initialize_Ephemeris                                                    **/
/**                                                                          **/
/**     This function must be called once by any program that accesses the   **/
/**     ephemeris data. It opens the ephemeris data file, reads the header   **/
/**     data, loads the first coefficient record into a global array, then   **/
/**     returns a status code that indicates whether or not all of this was  **/
/**     done successfully.                                                   **/
/**                                                                          **/
/**  Input: A character string giving the name of an ephemeris data file.    **/
/**                                                                          **/
/**  Returns: An integer status code.                                        **/
/**                                                                          **/
/**==========================================================================**/

int DeFile::Initialize_Ephemeris( char *fileName )
{
  int headerID;
  
  /*--------------------------------------------------------------------------*/
  /*  Open ephemeris file.                                                    */
  /*--------------------------------------------------------------------------*/

  //loj: Ephemeris_File = fopen(fileName,"r");
  Ephemeris_File = fopen(fileName,"rb"); // loj: 4/14/04


  /*--------------------------------------------------------------------------*/
  /*  Read header & first coefficient array, then return status code.         */
  /*--------------------------------------------------------------------------*/

  if ( Ephemeris_File == NULL ) /*........................No need to continue */
     {
       //printf("\n Unable to open ephemeris file: %s.\n",fileName);
       return FAILURE;
     }
  else 
     { /*.................Read first three header records from ephemeris file */

       //fread(&H1,sizeof(double),ARRAY_SIZE,Ephemeris_File);
       //fread(&H2,sizeof(double),ARRAY_SIZE,Ephemeris_File);
       //fread(&Coeff_Array,sizeof(double),ARRAY_SIZE,Ephemeris_File);
       fread(&H1,sizeof(double),arraySize,Ephemeris_File);           // wcs
       fread(&H2,sizeof(double),arraySize,Ephemeris_File);           // wcs
       fread(&Coeff_Array,sizeof(double),arraySize,Ephemeris_File);  // wcs


            
       /*...............................Store header data in global variables */
       
       R1 = H1.data;

              
       /*..........................................Set current time variables */

       T_beg  = Coeff_Array[0];
       T_end  = Coeff_Array[1];
       T_span = T_end - T_beg;

       /*..............................Convert header ephemeris ID to integer */

       headerID = (int) R1.DENUM;
       
       /*..............................................Debug Print (optional) */

       //if ( Debug ) 
       //   {
       //     printf("\n  In: Initialize_Ephemeris \n");
       //     printf("\n      ARRAY_SIZE = %4d",ARRAY_SIZE);
       //     printf("\n      headerID   = %3d",headerID);
       //     printf("\n      T_Beg      = %7.3f",T_beg);
       //     printf("\n      T_End      = %7.3f",T_end);
       //     printf("\n      T_Span     = %7.3f\n\n",T_span);
       //   }

#if defined (__UNIT_TEST__)
       std::ofstream fout;
       fout.open("TestDeFile.txt");
                     
       fout << " In: Initialize_Ephemeris" << std::endl;
       fout << "     ARRAY_SIZE     " << arraySize << std::endl;
       fout << "     R1.timeData[0] " << R1.timeData[0] << std::endl;
       fout << "     R1.timeData[1] " << R1.timeData[1] << std::endl;
       fout << "     R1.timeData[2] " << R1.timeData[2] << std::endl;
       fout << "     R1.numConst    " << R1.numConst << std::endl;
       fout << "     R1.AU          " << R1.AU << std::endl;
       fout << "     R1.EMRAT       " <<  R1.EMRAT << std::endl;
       fout << "     R1.coeffPtr[0][0] " << R1.coeffPtr[0][0] << std::endl;
       fout << "     R1.coeffPtr[11][2]" << R1.coeffPtr[11][2] << std::endl;
            
       fout << "     R1.DENUM   " << R1.DENUM << std::endl;
       fout << "     headerID   " << headerID << std::endl;
       fout << "     EPHEMERIS  " << EPHEMERIS << std::endl;
       fout << "     T_Beg      " << T_beg << std::endl;
       fout << "     T_End      " << T_end << std::endl;
       fout << "     T_Span     " << T_span << std::endl;

       fout.close();
#endif
  
       /*..................................................Return status code */

       if ( headerID == EPHEMERIS ) 
          {
            return SUCCESS;
          }
       else 
          {
            //printf("\n Opened wrong file: %s",fileName);
            //printf(" for ephemeris: %d.\n",EPHEMERIS);
              // throw PlanetaryEphemException(
              //             "DE file is not of specified format!!");  // wcs
            return FAILURE;
          }
     }
}

/**==========================================================================**/
/**  Interpolate_Libration                                                   **/
/**                                                                          **/
/**     This function computes an array of libration angles from Chebyshev   **/
/**     coefficients read in from an ephemeris data file. The coefficients   **/
/**     are read in from a file by the function Read_Coefficients (when      **/
/**     necessary).                                                          **/
/**                                                                          **/
/**  Inputs:                                                                 **/
/**     Time     -- Time for which position is desired (Julian Date).        **/
/**     Target   -- Solar system body for which position is desired.         **/
/**     Nutation -- Pointer to external array to receive the answer.         **/
/**                                                                          **/
/**  Returns: Nothing explicitly.                                            **/
/**==========================================================================**/

void DeFile::Interpolate_Libration( double Time , int Target , double Libration[3] )
{
  double    A[50] , Cp[50]  , sum[3] , T_break , T_seg , T_sub , Tc;
  int       i , j;
  long int  C , G , N , offset = 0;

  /*--------------------------------------------------------------------------*/
  /* This function only computes librations.                                  */
  /*--------------------------------------------------------------------------*/

  if ( Target != 12 )             /* Also protects against weird input errors */
     {
      // printf("\n This function only computes librations.\n");
       return;
     }
 
  /*--------------------------------------------------------------------------*/
  /* Initialize local coefficient array.                                      */
  /*--------------------------------------------------------------------------*/

  for ( i=0 ; i<50 ; i++ )
      {
        A[i] = 0.0;
      }

  /*--------------------------------------------------------------------------*/
  /* Determine if a new record needs to be input (if so, get it).             */
  /*--------------------------------------------------------------------------*/
    
  if ( Time < T_beg || Time > T_end ) Read_Coefficients(Time);

  /*--------------------------------------------------------------------------*/
  /* Read the coefficients from the binary record.                            */
  /*--------------------------------------------------------------------------*/
  
  C = R1.libratPtr[0] - 1;                   /* Coefficient array entry point */
  N = R1.libratPtr[1];                       /*        Number of coefficients */
  G = R1.libratPtr[2];                       /*    Granules in current record */

  /*...................................................Debug print (optional) */

  //if ( Debug )
  //   {
  //     printf("\n  In: Interpolate_Libration\n");
  //     printf("\n  Target = %2d",Target);
  //     printf("\n  C      = %4d (before)",C);
  //     printf("\n  N      = %4d",N);
  //     printf("\n  G      = %4d\n",G);
  //   }

  /*--------------------------------------------------------------------------*/
  /*  Compute the normalized time, then load the Tchebeyshev coefficients     */
  /*  into array A[]. If T_span is covered by a single granule this is easy.  */
  /*  If not, the granule that contains the interpolation time is found, and  */
  /*  an offset from the array entry point for the libration angles is used   */
  /*  to load the coefficients.                                               */
  /*--------------------------------------------------------------------------*/

  if ( G == 1 ) 
     {
       Tc = 2.0*(Time - T_beg) / T_span - 1.0;
       for (i=C ; i<(C+3*N) ; i++)  A[i-C] = Coeff_Array[i];
     }
  else if ( G > 1 )
     {
       T_sub = T_span / ((double) G);          /* Compute subgranule interval */
       
       for ( j=G ; j>0 ; j-- ) 
           {
             T_break = T_beg + ((double) j-1) * T_sub;
             if ( Time > T_break ) 
                {
                  T_seg  = T_break;
                  offset = j-1;
                  break;
                }
            }
            
       Tc = 2.0*(Time - T_seg) / T_sub - 1.0;
       C  = C + 3 * offset * N;  
       
       for (i=C ; i<(C+3*N) ; i++) A[i-C] = Coeff_Array[i];
     }
  else                                   /* Something has gone terribly wrong */
     {
       //printf("\n Number of granules must be >= 1: check header data.\n");
     }

  /*...................................................Debug print (optional) */

  //if ( Debug )
  //   {
  //     printf("\n  C      = %4d (after)",C);
  //     printf("\n  offset = %4d",offset);
  //     printf("\n  Time   = %12.7f",Time);
  //     printf("\n  T_sub  = %12.7f",T_sub);
  //     printf("\n  T_seg  = %12.7f",T_seg);
  //     printf("\n  Tc     = %12.7f\n",Tc);
  //     printf("\n  Array Coefficients:\n");
  //     for ( i=0 ; i<3*N ; i++ )
  //         {
  //           printf("\n  A[%2d] = % 22.15e",i,A[i]);
  //         }
  //     printf("\n\n");
  //   }

  /*..........................................................................*/

  /*--------------------------------------------------------------------------*/
  /* Compute interpolated the libration.                                      */
  /*--------------------------------------------------------------------------*/
  
  for ( i=0 ; i<3 ; i++ ) 
      {                           
        Cp[0]  = 1.0;                                 /* Begin polynomial sum */
        Cp[1]  = Tc;
        sum[i] = A[i*N] + A[1+i*N]*Tc;

        for ( j=2 ; j<N ; j++ )                                  /* Finish it */
            {
              Cp[j]  = 2.0 * Tc * Cp[j-1] - Cp[j-2];
              sum[i] = sum[i] + A[j+i*N] * Cp[j];
            }
        Libration[i] = sum[i];
      }
      
  return;
}


/**==========================================================================**/
/**  Interpolate_Nutation                                                    **/
/**                                                                          **/
/**     This function computes an array of nutation angles from Chebyshev    **/
/**     coefficients read in from an ephemeris data file. The coefficients   **/
/**     are read in from a file by the function Read_Coefficients (when      **/
/**     necessary).                                                          **/
/**                                                                          **/
/**  Inputs:                                                                 **/
/**     Time     -- Time for which position is desired (Julian Date).        **/
/**     Target   -- Solar system body for which position is desired.         **/
/**     Nutation -- Pointer to external array to receive the angles.         **/
/**                                                                          **/
/**  Returns: Nothing explicitly.                                            **/
/**                                                                          **/
/**==========================================================================**/

void DeFile::Interpolate_Nutation( double Time , int Target , double Nutation[2] )
{
  double    A[50] , Cp[50]  , sum[3] , T_break , T_seg , T_sub , Tc;
  int       i , j;
  long int  C , G , N , offset = 0;

  /*--------------------------------------------------------------------------*/
  /* This function only computes nutations.                                   */
  /*--------------------------------------------------------------------------*/

  if ( Target != 11 )             /* Also protects against weird input errors */
     {
      // printf("\n This function only computes nutations.\n");
       return;
     }
 
  /*--------------------------------------------------------------------------*/
  /* Initialize local coefficient array.                                      */
  /*--------------------------------------------------------------------------*/

  for ( i=0 ; i<50 ; i++ )
      {
        A[i] = 0.0;
      }

  /*--------------------------------------------------------------------------*/
  /* Determine if a new record needs to be input (if so, get it).             */
  /*--------------------------------------------------------------------------*/
    
  if (Time < T_beg || Time > T_end)  Read_Coefficients(Time);

  /*--------------------------------------------------------------------------*/
  /* Read the coefficients from the binary record.                            */
  /*--------------------------------------------------------------------------*/
  
  C = R1.coeffPtr[Target][0] - 1;            /* Coefficient array entry point */
  N = R1.coeffPtr[Target][1];                /*        Number of coefficients */
  G = R1.coeffPtr[Target][2];                /*    Granules in current record */

  /*...................................................Debug print (optional) */

  //if ( Debug )
  //   {
  //     printf("\n  In: Interpolate_Nutation\n");
  //     printf("\n  Target = %2d",Target);
  //     printf("\n  C      = %4d (before)",C);
  //     printf("\n  N      = %4d",N);
  //     printf("\n  G      = %4d\n",G);
  //   }

  /*--------------------------------------------------------------------------*/
  /*  Compute the normalized time, then load the Tchebeyshev coefficients     */
  /*  into array A[]. If T_span is covered by a single granule this is easy.  */
  /*  If not, the granule that contains the interpolation time is found, and  */
  /*  an offset from the array entry point for the nutation angles is used    */
  /*  to load the coefficients.                                               */
  /*--------------------------------------------------------------------------*/

  if ( G == 1 )
     {
       Tc = 2.0*(Time - T_beg) / T_span - 1.0;
       for (i=C ; i<(C+3*N) ; i++)  A[i-C] = Coeff_Array[i];
     }
  else if ( G > 1 )
     {
       T_sub = T_span / ((double) G);          /* Compute subgranule interval */
       
       for ( j=G ; j>0 ; j-- ) 
           {
             T_break = T_beg + ((double) j-1) * T_sub;
             if ( Time > T_break ) 
                {
                  T_seg  = T_break;
                  offset = j-1;
                  break;
                }
            }
            
       Tc = 2.0*(Time - T_seg) / T_sub - 1.0;
       C  = C + 3 * offset * N;
       
       for (i=C ; i<(C+3*N) ; i++) A[i-C] = Coeff_Array[i];
     }
  else                                   /* Something has gone terribly wrong */
     {
       //printf("\n Number of granules must be >= 1: check header data.\n");
     }

  /*...................................................Debug print (optional) */

  //if ( Debug )
  //   {
  //     printf("\n  C      = %4d (after)",C);
  //     printf("\n  offset = %4d",offset);
  //     printf("\n  Time   = %12.7f",Time);
  //     printf("\n  T_sub  = %12.7f",T_sub);
  //     printf("\n  T_seg  = %12.7f",T_seg);
  //     printf("\n  Tc     = %12.7f\n",Tc);
  //     printf("\n  Array Coefficients:\n");
  //     for ( i=0 ; i<3*N ; i++ )
  //         {
  //           printf("\n  A[%2d] = % 22.15e",i,A[i]);
  //         }
  //     printf("\n\n");
  //   }

  /*..........................................................................*/

  /*--------------------------------------------------------------------------*/
  /* Compute interpolated the nutation.                                       */
  /*--------------------------------------------------------------------------*/
  
  for ( i=0 ; i<2 ; i++ )
      {
        Cp[0]  = 1.0;                                 /* Begin polynomial sum */
        Cp[1]  = Tc;
        sum[i] = A[i*N] + A[1+i*N]*Tc;

        for ( j=2 ; j<N ; j++ )                                  /* Finish it */
            {
              Cp[j]  = 2.0 * Tc * Cp[j-1] - Cp[j-2];
              sum[i] = sum[i] + A[j+i*N] * Cp[j];
            }
        Nutation[i] = sum[i];
      }
      
  return;
}

/**==========================================================================**/
/**  Interpolate_Position                                                    **/
/**                                                                          **/
/**     This function computes a position vector for a selected planetary    **/
/**     body from Chebyshev coefficients read in from an ephemeris data      **/
/**     file. These coefficients are read from the data file by calling      **/
/**     the function Read_Coefficients (when necessary).                     **/
/**                                                                          **/
/**  Inputs:                                                                 **/
/**     Time     -- Time for which position is desired (Julian Date).        **/
/**     Target   -- Solar system body for which position is desired.         **/
/**     Position -- Pointer to external array to receive the position.       **/
/**                                                                          **/
/**  Returns: Nothing explicitly.                                            **/
/**                                                                          **/
/**==========================================================================**/

void DeFile::Interpolate_Position( double Time , int Target , double Position[3] )
{
  double    A[50] , Cp[50]  , sum[3] , T_break , T_seg , T_sub , Tc;
  int       i , j;
  long int  C , G , N , offset = 0;

  /*--------------------------------------------------------------------------*/
  /* This function doesn't "do" nutations or librations.                      */
  /*--------------------------------------------------------------------------*/

  if ( Target >= 11 )             /* Also protects against weird input errors */
     {
       //printf("\n This function does not compute nutations or librations.\n");
       return;
     }
 
  /*--------------------------------------------------------------------------*/
  /* Initialize local coefficient array.                                      */
  /*--------------------------------------------------------------------------*/

  for ( i=0 ; i<50 ; i++ )
      {
        A[i] = 0.0;
      }

  /*--------------------------------------------------------------------------*/
  /* Determine if a new record needs to be input (if so, get it).             */
  /*--------------------------------------------------------------------------*/
    
  if (Time < T_beg || Time > T_end)  Read_Coefficients(Time);

  /*--------------------------------------------------------------------------*/
  /* Read the coefficients from the binary record.                            */
  /*--------------------------------------------------------------------------*/
  
  C = R1.coeffPtr[Target][0] - 1;          /*   Coefficient array entry point */
  N = R1.coeffPtr[Target][1];              /* Number of coeff's per component */
  G = R1.coeffPtr[Target][2];              /*      Granules in current record */

  /*...................................................Debug print (optional) */

  //if ( Debug )
  //   {
  //     printf("\n  In: Interpolate_Position\n");
  //     printf("\n  Target = %2d",Target);
  //     printf("\n  C      = %4d (before)",C);
  //     printf("\n  N      = %4d",N);
  //     printf("\n  G      = %4d\n",G);
  //   }

  /*--------------------------------------------------------------------------*/
  /*  Compute the normalized time, then load the Tchebeyshev coefficients     */
  /*  into array A[]. If T_span is covered by a single granule this is easy.  */
  /*  If not, the granule that contains the interpolation time is found, and  */
  /*  an offset from the array entry point for the ephemeris body is used to  */
  /*  load the coefficients.                                                  */
  /*--------------------------------------------------------------------------*/

  if ( G == 1 )
     {
       Tc = 2.0*(Time - T_beg) / T_span - 1.0;
       for (i=C ; i<(C+3*N) ; i++)  A[i-C] = Coeff_Array[i];
     }
  else if ( G > 1 )
     {
       T_sub = T_span / ((double) G);          /* Compute subgranule interval */
       
       for ( j=G ; j>0 ; j-- ) 
           {
             T_break = T_beg + ((double) j-1) * T_sub;
             if ( Time > T_break ) 
                {
                  T_seg  = T_break;
                  offset = j-1;
                  break;
                }
            }
            
       Tc = 2.0*(Time - T_seg) / T_sub - 1.0;
       C  = C + 3 * offset * N;
       
       for (i=C ; i<(C+3*N) ; i++) A[i-C] = Coeff_Array[i];
     }
  else                                   /* Something has gone terribly wrong */
     {
      // printf("\n Number of granules must be >= 1: check header data.\n");
     }

  /*...................................................Debug print (optional) */

  //if ( Debug )
  //   {
  //     printf("\n  C      = %4d (after)",C);
  //     printf("\n  offset = %4d",offset);
  //     printf("\n  Time   = %12.7f",Time);
  //     printf("\n  T_sub  = %12.7f",T_sub);
  //     printf("\n  T_seg  = %12.7f",T_seg);
  //     printf("\n  Tc     = %12.7f\n",Tc);
  //     printf("\n  Array Coefficients:\n");
  //     for ( i=0 ; i<3*N ; i++ )
  //         {
  //           printf("\n  A[%2d] = % 22.15e",i,A[i]);
  //         }
  //     printf("\n\n");
  //   }

  /*..........................................................................*/

  /*--------------------------------------------------------------------------*/
  /* Compute interpolated the position.                                       */
  /*--------------------------------------------------------------------------*/
  
  for ( i=0 ; i<3 ; i++ ) 
      {                           
        Cp[0]  = 1.0;                                 /* Begin polynomial sum */
        Cp[1]  = Tc;
        sum[i] = A[i*N] + A[1+i*N]*Tc;

        for ( j=2 ; j<N ; j++ )                                  /* Finish it */
            {
              Cp[j]  = 2.0 * Tc * Cp[j-1] - Cp[j-2];
              sum[i] = sum[i] + A[j+i*N] * Cp[j];
            }
        Position[i] = sum[i];
      }

  return;
}

/**==========================================================================**/
/**  Interpolate_State                                                       **/
/**                                                                          **/
/**     This function computes position and velocity vectors for a selected  **/
/**     planetary body from Chebyshev coefficients that are read in from an  **/
/**     ephemeris data file. These coefficients are read from the data file  **/
/**     by calling the function Read_Coefficients (when necessary).          **/
/**                                                                          **/
/**  Inputs:                                                                 **/
/**     Time     -- Time for which position is desired (Julian Date).        **/
/**     Target   -- Solar system body for which position is desired.         **/
/**     Position -- Pointer to external array to receive the position.       **/
/**                                                                          **/
/**  Returns: Nothing (explicitly)                                           **/
/**==========================================================================**/

void DeFile::Interpolate_State(double Time , int Target, stateType *p)
{
  double    A[50]   , B[50] , Cp[50] , P_Sum[3] , V_Sum[3] , Up[50] ,
            T_break , T_seg , T_sub  , Tc;
  int       i , j;
  long int  C , G , N , offset = 0;
  stateType X;

  /*--------------------------------------------------------------------------*/
  /* This function doesn't "do" nutations or librations.                      */
  /*--------------------------------------------------------------------------*/

  if ( Target >= 11 )             /* Also protects against weird input errors */
     {
       //printf("\n This function does not compute nutations or librations.\n");
       return;
     }

  /*--------------------------------------------------------------------------*/
  /* Initialize local coefficient array.                                      */
  /*--------------------------------------------------------------------------*/

  for ( i=0 ; i<50 ; i++ )
      {
        A[i] = 0.0;
        B[i] = 0.0;
      }

  /*--------------------------------------------------------------------------*/
  /* Determine if a new record needs to be input.                             */
  /*--------------------------------------------------------------------------*/
  
  if (Time < T_beg || Time > T_end)  Read_Coefficients(Time);

  /*--------------------------------------------------------------------------*/
  /* Read the coefficients from the binary record.                            */
  /*--------------------------------------------------------------------------*/
  
  C = R1.coeffPtr[Target][0] - 1;               /*    Coeff array entry point */
  N = R1.coeffPtr[Target][1];                   /*          Number of coeff's */
  G = R1.coeffPtr[Target][2];                   /* Granules in current record */

  /*...................................................Debug print (optional) */

  //if ( Debug )
  //   {
  //     printf("\n  In: Interpolate_State\n");
  //     printf("\n  Target = %2d",Target);
  //     printf("\n  C      = %4d (before)",C);
  //     printf("\n  N      = %4d",N);
  //     printf("\n  G      = %4d\n",G);
  //   }

  /*--------------------------------------------------------------------------*/
  /*  Compute the normalized time, then load the Tchebeyshev coefficients     */
  /*  into array A[]. If T_span is covered by a single granule this is easy.  */
  /*  If not, the granule that contains the interpolation time is found, and  */
  /*  an offset from the array entry point for the ephemeris body is used to  */
  /*  load the coefficients.                                                  */
  /*--------------------------------------------------------------------------*/
  if ( G == 1 )
     {
       Tc = 2.0*(Time - T_beg) / T_span - 1.0;
       for (i=C ; i<(C+3*N) ; i++)  A[i-C] = Coeff_Array[i];
     }
  else if ( G > 1 )
     {
       T_sub = T_span / ((double) G);          /* Compute subgranule interval */
       for ( j=G ; j>0 ; j-- ) 
           {
             T_break = T_beg + ((double) j-1) * T_sub;
             if ( Time > T_break ) 
                {
                  T_seg  = T_break;
                  offset = j-1;
                  break;
                }
            }

       Tc = 2.0*(Time - T_seg) / T_sub - 1.0;
       C  = C + 3 * offset * N;
       
       for (i=C ; i<(C+3*N) ; i++) A[i-C] = Coeff_Array[i];
     }
  else                                   /* Something has gone terribly wrong */
     {
       //printf("\n Number of granules must be >= 1: check header data.\n");
     }

  /*...................................................Debug print (optional) */

  //if ( Debug )
  //   {
  //     printf("\n  C      = %4d (after)",C);
  //     printf("\n  offset = %4d",offset);
  //     printf("\n  Time   = %12.7f",Time);
  //     printf("\n  T_sub  = %12.7f",T_sub);
  //     printf("\n  T_seg  = %12.7f",T_seg);
  //     printf("\n  Tc     = %12.7f\n",Tc);
  //     printf("\n  Array Coefficients:\n");
  //     for ( i=0 ; i<3*N ; i++ )
  //         {
  //           printf("\n  A[%2d] = % 22.15e",i,A[i]);
  //         }
  //     printf("\n\n");
  //   }

  /*..........................................................................*/

  /*--------------------------------------------------------------------------*/
  /* Compute the interpolated position & velocity                             */
  /*--------------------------------------------------------------------------*/
  
  for ( i=0 ; i<3 ; i++ )                /* Compute interpolating polynomials */
      {

        Cp[0] = 1.0;           
        Cp[1] = Tc;
        Cp[2] = 2.0 * Tc*Tc - 1.0;
        
        Up[0] = 0.0;
        Up[1] = 1.0;
        Up[2] = 4.0 * Tc;

        for ( j=3 ; j<N ; j++ )
            {
              Cp[j] = 2.0 * Tc * Cp[j-1] - Cp[j-2];
              Up[j] = 2.0 * Tc * Up[j-1] + 2.0 * Cp[j-1] - Up[j-2];
            }

        P_Sum[i] = 0.0;           /* Compute interpolated position & velocity */
        V_Sum[i] = 0.0;

        for ( j=N-1 ; j>-1 ; j-- )  P_Sum[i] = P_Sum[i] + A[j+i*N] * Cp[j];
        for ( j=N-1 ; j>0  ; j-- )  V_Sum[i] = V_Sum[i] + A[j+i*N] * Up[j];

        X.Position[i] = P_Sum[i];
        X.Velocity[i] = V_Sum[i] * 2.0 * ((double) G) / (T_span * 86400.0);
      }

  /*--------------------------------------------------------------------------*/
  /*  Return computed values.                                                 */
  /*--------------------------------------------------------------------------*/

  *p = X;
  
  return;
}


//------------------------------------------------------------------------------
// private methods from JPL/JSC code ephem_util.c
//------------------------------------------------------------------------------

/******************************************************************************/
/**                                                                          **/
/**  SOURCE FILE: ephem_util.c                                               **/
/**                                                                          **/
/**    This file contains source code for the utility functions used in the  **/
/**    ephemeris manipulation programs.                                      **/
/**                                                                          **/
/**  Programmer: David Hoffman/EG5                                           **/
/**              NASA, Johnson Space Center                                  **/
/**              Houston, TX 77058                                           **/
/**              e-mail: david.a.hoffman1@jsc.nasa.gov                       **/
/**                                                                          **/
/******************************************************************************/

/**==========================================================================**/
/**  Find_Value                                                              **/
/**                                                                          **/
/**    Given a string, an array of strings, and an array of double precision **/
/**    numbers (both arrays having the same dimension) this function will    **/
/**    scan the string array to find a match and, if a match is found, it    **/
/**    will return the value of the double precision number whose position   **/
/**    in the double precision array corresponds to the location of the      **/
/**    match in the string array (follow all that?).                         **/
/**                                                                          **/
/**  Inputs:                                                                 **/
/**    name        - a string                                                **/
/**    name_array  - an array of strings                                     **/
/**    value_array - a double precision array                                **/
/**                                                                          **/
/**  Output:                                                                 **/
/**    A double precision number                                             **/
/**==========================================================================**/

double DeFile::Find_Value( char    name[], 
                   char    name_array[400][6] , 
                   double  value_array[400]  ) 
{
  double value = 0.0;
  int    found = FALSE;
  char   target[7];
  int    i, j;

  for ( i=0 ; i<400 ; i++ ) 
      {
        /*.....................Convert current name array element to a string */
      
        for ( j=0 ; j<6 ; j++ )  target[j] = name_array[i][j];
            target[6] = '\0';
      
        /*................................See if it matches the string sought */
      
        if ( !strcmp(name,target) ) 
           {
             value = value_array[i];
             found = TRUE;
             break;                                /* No need to keep looking */
           }                                       /*   once a match is found */
      }

  //if ( (i==399) & (found==FALSE) ) Warning(20);             /* Didn't find it */

  return value;
}

/**==========================================================================**/
/**  Gregorian_to_Julian                                                     **/
/**                                                                          **/
/**     This function takes a time,  specified in terms of a Gregorian date  **/
/**     (i.e. a modern calander date) plus a time of day (given in terms of  **/
/**     hours, minutes, and seconds since midnight) and  returns the corre-  **/
/**     sponding Julian date.  The algorithm used to do this calculation is  **/
/**     based on pp 60-61 of: Astronomical Algorithms, Jean Meeus, Willman-  **/
/**     Bell, Richmond VA, 1991.  It has been modified  here to allow users  **/
/**     to input hours, minutes, and seconds instead of fractional days.     **/
/**                                                                          **/
/**     IMPORTANT NOTE: This algorithm, despite the claim made in Meeus, is  **/
/**     only valid for years later than 0 A.D. (or ACE for the politically   **/
/**     correct).                                                            **/
/**                                                                          **/
/**  Inputs:                                                                 **/
/**      year (integer: positive for AD, negative for BC)                    **/
/**     month (integer: 1 through 12)                                        **/
/**       day (integer: 1 through 31)                                        **/
/**      hour (integer: 0 through 24)                                        **/
/**       min (integer: 0 through 60)                                        **/
/**       sec ( double: less than 60.0)                                      **/
/**                                                                          **/
/**  Output:                                                                 **/
/**     Julian date (double)                                                 **/
/**==========================================================================**/

double DeFile::Gregorian_to_Julian( int     year ,  int     month   , 
                            int     day  ,  int     hour    , 
                            int     min  ,  double  seconds )
{
  double  A, B, D, H, JD, M, N, Y;

  /*--------------------------------------------------------------------------*/
  /*  Adjust year & month values (if necessary).                              */
  /*--------------------------------------------------------------------------*/

  if ( month < 3 ) {                                      /* See Muess, p. 61 */
       month = month + 12;
       year  = year - 1;
     }

  /*--------------------------------------------------------------------------*/
  /*  Perform type casts and adjust the day count.                            */
  /*--------------------------------------------------------------------------*/

  Y = (double) year;
  M = (double) month;
  D = (double) day;
  H = (double) hour;
  N = (double) min;

  /*--------------------------------------------------------------------------*/
  /*  Compute the day fraction:                                               */
  /*--------------------------------------------------------------------------*/

  D = D  +  (H / 24.0)  +  (N / 1440.0)  +  (seconds / 86400.0);

  /*--------------------------------------------------------------------------*/
  /*  Compute the Julian date.                                                */
  /*--------------------------------------------------------------------------*/

  A  = floor(Y/100.0);
  B  = 2.0 - A + floor(A/4.0);  
  JD = floor(365.25*(Y+4716.0)) + floor(30.6001*(M+1.0)) + D + B - 1524.5;
  
  return JD;
}
                         

/**==========================================================================**/
/**  Integer modulo function (self-explanatory: returns x mod y)             **/
/**==========================================================================**/

int mod(int x, int y)
{
  double rf, rx, ry;
     int f;
  
  /*--------------------------------------------------------------------------*/
  /* The basic formula can be found on p.38 of Knuth's The Art of Computer    */
  /* Programming, volume 1. I still can't believe that C lacks this function, */
  /* but I can find no mention of it in any of my C books...                  */
  /*--------------------------------------------------------------------------*/

  if ( y != 0 ) 
     {
       rx = (double) x;
       ry = (double) y;
       rf = floor(rx/ry);
       f  = (int) rf;
       return x - y*f;
     }
  else 
       return x;
}

/**==========================================================================**/
/**  Read_File_Line                                                          **/
/**                                                                          **/
/**     Reads in one line from an ASCII file. Specifically, it reads in 80   **/
/**     characters unless it encounters an end-of-line or an end-of-file.    **/
/**     If requested, it also converts FORTRAN scientific notation to the    **/
/**     equivalent C representation (in which case it converts any 'D' in    **/
/**     the line to an 'E').                                                 **/
/**                                                                          **/
/**  Inputs:                                                                 **/
/**     inFile     - pointer to an ASCII file                                **/
/**     filter     - Boolean flag (OK, an integer)                           **/
/**     lineBuffer - an externally defined character array                   **/
/**                                                                          **/
/**  Returns:                                                                **/
/**     Nothing.                                                             **/
/**==========================================================================**/

int DeFile::Read_File_Line( FILE *inFile, int filter, char lineBuffer[82])
{
  char ignore[40];
  char *code;
  int  i, status = 0;

  /*--------------------------------------------------------------------------*/
  /* Read one line (81 characters max) from header file.                      */
  /*--------------------------------------------------------------------------*/
  
  code = fgets(lineBuffer,82,inFile);

  /*--------------------------------------------------------------------------*/
  /*  Protect against lines over 80 characters long.                          */
  /*--------------------------------------------------------------------------*/

  if ( (strlen(lineBuffer) == 81) && (lineBuffer[80] != '\n') )
     {
       fgets(ignore,40,inFile);                 /* Read past next end of line */
       lineBuffer[81] = '\0';
     }

  /*--------------------------------------------------------------------------*/
  /* Convert FORTRAN exponential representation to C representation.          */
  /*--------------------------------------------------------------------------*/
  
  if ( filter == TRUE ) 
     {
       for ( i=0 ; i<82 ; i++ )
           {
             if (lineBuffer[i] == '\0') break;
             if (lineBuffer[i] ==  'D') lineBuffer[i] = 'E';
           }
     }

  /*--------------------------------------------------------------------------*/
  /* Quit when EOF encountered.                                               */
  /*--------------------------------------------------------------------------*/

  if ( code == NULL ) status = EOF;

  return status;

}

/**==========================================================================**/
/**  Read_Group_Header                                                       **/
/**                                                                          **/
/**    Each data group in the header file is preceded by a group header      **/
/**    which consists of a blank line, a line of text, and another blank     **/
/**    line. The line of text consists of the string: GROUP   10XX (where    **/
/**    XX could be 10, 30, 40, 41, or 50). This function reads the group     **/
/**    header and returns an integer that signifies which group was read.    **/
/**                                                                          **/
/**  Input:                                                                  **/
/**    inFile - File pointer                                                 **/
/**                                                                          **/
/**  Returns:                                                                **/
/**    Integer = 1 for GROUP 1010                                            **/
/**            = 2 for GROUP 1030                                            **/
/**            = 3 for GROUP 1040                                            **/
/**            = 4 for GROUP 1041                                            **/
/**            = 5 for GROUP 1050                                            **/
/**==========================================================================**/

int DeFile::Read_Group_Header(FILE *inFile)
{
   int   count   = 0;                               /* Initial values required */
   int   tripCnt = 1;
   char  appStr[2], charIn, headStr[10];
   int   group;

  /*--------------------------------------------------------------------------*/
  /* Build the group header string.                                           */
  /*--------------------------------------------------------------------------*/

  headStr[0] = '\0';                               /* Start with empty string */

  do { 
       /*................................................. Read one character */
       
       charIn = fgetc(inFile);
       
       /*..........................................Do the right thing with it */
       
       if ( isgraph(charIn) )          /* Build sting of non-blank characters */
          {
            appStr[0] = charIn;
            appStr[1] = '\0';
            strcat(headStr,appStr);
          }
       else                                              /* Count line breaks */
          {
            if (charIn == '\n') count = count + 1;
          }
            
       /*.......................................Protect against strange input */
       
       if (feof(inFile)) break;

       if ( ++tripCnt > 247 ) 
          {
            //Warning(21);
            break;
          }
       
     } while (count < 3);

  /*--------------------------------------------------------------------------*/
  /* Determine the group whose data follows.                                  */
  /*--------------------------------------------------------------------------*/

       if (!strcmp(headStr,"GROUP1010")) group = 1;
  else if (!strcmp(headStr,"GROUP1030")) group = 2;
  else if (!strcmp(headStr,"GROUP1040")) group = 3;
  else if (!strcmp(headStr,"GROUP1041")) group = 4;
  else if (!strcmp(headStr,"GROUP1050")) group = 5;
  else                                   group = 0;
  
  return group;
}

