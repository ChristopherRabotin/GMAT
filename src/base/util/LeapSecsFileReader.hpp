//$Header$
//------------------------------------------------------------------------------
//                              LeapSecsFileReader
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
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

class LeapSecsFileReader
{
public:
   LeapSecsFileReader(std::string withFileName = "tai-utc.dat");
   virtual ~LeapSecsFileReader();
   LeapSecsFileReader(const LeapSecsFileReader& lsfr);
   LeapSecsFileReader& operator=(const LeapSecsFileReader& lsfr);
        
   bool    LoadTimeCoeffFile(std::string withFileName);
   Real    NumberOfLeapSecondsFrom(UtcMjd utcMjd);

protected:


private:
   bool Parse(std::string line);
   bool initialized;

   struct LeapSecondInformation
   {
      Real julianDate;         // arg: 2/1/05 assumed to be utc
      Real offset1;
      Real offset2;
      Real offset3;
   };    
   
   std::vector<LeapSecondInformation> lookUpTable;
};

#endif // LeapSecsFileReader_hpp
