//$Header$
// *** File Name : gmatdefs.h
// *** Created   : May 20, 2003
// **************************************************************************
// ***  Developed By  :  Thinking Systems, Inc. (www.thinksysinc.com)     ***
// ***  For:  Flight Dynamics Analysis Branch (Code 595)                  ***
// ***  Under Contract:  P.O.  GSFC S-67573-G                             ***
// ***  Copyright Thinking Systems 2003                                   ***
// ***                                                                    ***
// ***  Header Version: July 12, 2002                                     ***
// **************************************************************************
// Module Type               : ANSI C++ Source
// Development Environment   : Visual C++ 7.0
// Modification History      : 5/20/2003 - D. Conway, Thinking Systems, Inc.
//                             Original delivery
//                             Source code not yet transferred to GSFC
//                           : 2003/09/16 - W. Shoan/GSFC/583
//                             added ObjectTypes enum type in namespace Gmat
//                           : 11/9/2003 - D. Conway, Thinking Systems, Inc.
//                             Added OBJECT_TYPE for member objects
// **************************************************************************

#ifndef GMATDEFS_HPP
#define GMATDEFS_HPP


#include <string>               // For std::string
#include <vector>               // For std::vector


#ifdef _WIN32  // Windows

   #ifdef _MSC_VER  // Microsoft Visual C++

      #define WIN32_LEAN_AND_MEAN  // Exclude rarely-used stuff from Windows headers
      // Windows Header File entry point:
      #include <windows.h>

      #define  _USE_MATH_DEFINES  // makes Msoft define pi
    
   #endif  // End Microsoft C++ specific block

   #ifdef _DYNAMICLINK  // Only used for Windows DLLs
      #ifdef GMAT_EXPORTS
         #define GMAT_API __declspec(dllexport)
      #else
         #define GMAT_API __declspec(dllimport)
      #endif
   #endif

#endif //  End of OS nits

#ifndef GMAT_API
   #define GMAT_API
#endif


typedef double          Real;              // 8 byte float
typedef int             Integer;           // 4 byte signed integer
typedef unsigned char   Byte;              // 1 byte
typedef unsigned int    UnsignedInt;       // 4 byte unsigned integer
typedef std::vector<Real> RealArray;
typedef std::vector<Integer> IntegerArray;
typedef std::vector<std::string> StringArray;

namespace Gmat
{
   enum ObjectType
   {
      SPACECRAFT= 1001,
      GROUND_STATION,
      BURN,
      COMMAND,
      PROPAGATOR,
      FORCE_MODEL,
      PHYSICAL_MODEL,
      INTERPOLATOR,
      SOLAR_SYSTEM,
      CELESTIAL_BODY,
      ATMOSPHERE,
      PARAMETER,
      STOP_CONDITION,
      SOLVER,
      SUBSCRIBER,
      PROP_SETUP,
      REF_FRAME,        //loj: 3/22/04 added
      UNKNOWN_OBJECT
   };


   enum ParameterType
   {
      INTEGER_TYPE,
      UNSIGNED_INT_TYPE,  //wcs: added 2004.06.01
      REAL_TYPE,
      STRING_TYPE,
      STRINGARRAY_TYPE,
      BOOLEAN_TYPE,
      RVECTOR_TYPE,    // not 3- or 6- vector
      RVECTOR3_TYPE,
      RVECTOR6_TYPE,
      RMATRIX_TYPE,    // not 3x3 matrix
      RMATRIX33_TYPE,
      CARTESIAN_TYPE,
      KEPLERIAN_TYPE,
      A1MJD_TYPE,
      UTCDATE_TYPE,
      OBJECT_TYPE,
      TypeCount,
      UNKNOWN_PARAMETER_TYPE = -1
   };

}

#endif //GMATDEFS_HPP

