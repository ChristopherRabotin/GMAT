//$Id$
//------------------------------------------------------------------------------
//                              LeapSecsFileReader
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Allison R. Greene
// Created: 2005/01/26
//
/**
 * Reads time coefficent file, creates a table of coefficients and converts
 * to the utc.
 *
 * File found at : ftp://maia.usno.navy.mil/ser7/tai-utc.dat
 */
//------------------------------------------------------------------------------

#ifndef LeapSecsFileReader_hpp
#define LeapSecsFileReader_hpp

#include "gmatdefs.hpp"
#include "TimeTypes.hpp"

/** 
 * Structure defining internal leap second information.
 *
 * Moved here from inside of the LeapSecsFileReader class to clean up import/
 * export issues with Visual Studio
 */
struct LeapSecondInformation
{
   Real julianDate;         // arg: 2/1/05 assumed to be utc
   Real offset1;
   Real offset2;
   Real offset3;
};    

// Required for Visual Studio dll exports
#ifdef EXPORT_TEMPLATES
   EXPIMP_TEMPLATE template class DECLSPECIFIER std::allocator<LeapSecondInformation>;
   EXPIMP_TEMPLATE template class DECLSPECIFIER std::vector<LeapSecondInformation>;
#endif

class GMAT_API LeapSecsFileReader
{
public:
   LeapSecsFileReader(const std::string &fileName = "tai-utc.dat");
   virtual ~LeapSecsFileReader();
   LeapSecsFileReader(const LeapSecsFileReader& lsfr);
   LeapSecsFileReader& operator=(const LeapSecsFileReader& lsfr);

   bool Initialize();
   Real    NumberOfLeapSecondsFrom(UtcMjd utcMjd);

private:

   bool Parse(std::string line);

   // member data
   bool isInitialized;
   std::vector<LeapSecondInformation> lookUpTable;
   std::string withFileName;
};

#endif // LeapSecsFileReader_hpp
