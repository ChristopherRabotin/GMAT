//$Header$
//------------------------------------------------------------------------------
//                                  PlanetaryEphem
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Wendy C. Shoan
// Created: 2004/02/18
//
/**
 * This is the base class that defines the interface to planetary ephemeris
 * files.
 */
//------------------------------------------------------------------------------
#ifndef PlanetaryEphem_hpp
#define PlanetaryEphem_hpp

#include "gmatdefs.hpp"
#include "A1Mjd.hpp"

namespace Gmat  // does this belong in DeFIle.hpp???????????????
{
   // different types of DE files
   enum DeFileTypes
   {
      DE_202 = 0,
      DE_403,
      DE_405           // do we need more?
   };
   // different types of DE files
   enum DeFileFormat
   {
      DE_ASCII = 0,
      DE_BINARY
   };
};

class PlanetaryEphem
{
public:

   // default constructor
   PlanetaryEphem(std::string withFileName);
   // copy constructor
   PlanetaryEphem(const PlanetaryEphem& pef);
   // operator=
   PlanetaryEphem& operator=(const PlanetaryEphem& pef);
   // destructor
   virtual ~PlanetaryEphem();

   // method to return the full path name of the planetary ephem file.
   std::string GetName() const;

   // method to return the position and velocity of the specified body
   // at the specified time
   virtual Real* GetPosVel(Integer forBody, A1Mjd atTime) = 0;

   // method to return the day-of-year and year of the start time of the
   // SLP file.
   virtual Integer* GetStartDayAndYear() = 0;

   // maximum length of path name
   static const Integer MAX_PATH_LEN;

   // from Swingby bodies_types.h
   // Max number of bodies that can be modeled
   static const Integer MAX_BODIES = 3;
   // Max number of zonal values that are enterable
   static const Integer MAX_ZONALS;
   // Max length of the name of a potential field name
   static const Integer MAX_POTENTIAL_NAME;
   // The number of bodies normally found on the planetary ephemeris file
   static const Integer NUM_STANDARD_BODIES;

  
protected:
   // file type (from Swingby - added constructors, operator=, destructor
   struct dcb_type
   {
      dcb_type()
   {
         strcpy(full_path, "\0");
         recl  = 0;
         fptr  = NULL;
   }
      dcb_type(const dcb_type& dcb)
   {
         strcpy(full_path, dcb.full_path);
         recl  = dcb.recl;
         fptr  = dcb.fptr;
   }
      dcb_type& operator=(const dcb_type dcb)
   {
         strcpy(full_path, dcb.full_path);
         recl  = dcb.recl;
         fptr  = dcb.fptr;
         return *this;
   }
//      char  full_path[MAX_PATH_LEN+1];
      char  full_path[261];
      long   recl;
      FILE  *fptr;

   };

   // from slp_data_types.h
   // length of planetary ephemeris header records
   Integer lengthOfHeaderRecord;
   // length of planetary ephemeris data records
   Integer lengthOfDataRecord;
   // ??????
   Integer ibepm;

   std::string itsName;

   // file data type
   dcb_type g_pef_dcb;

private:


};
#endif // PlanetaryEphem_hpp
