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
 *
 * @note The MJD-JD offset used is 2400000.5
 */
//------------------------------------------------------------------------------

#include "LeapSecsFileReader.hpp"
#include "MessageInterface.hpp"
#include "StringTokenizer.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
//------------------------------------------------------------------------------
//  LeapSecsFileReader()
//------------------------------------------------------------------------------
/**
 *  Constructor.
 */
//------------------------------------------------------------------------------
LeapSecsFileReader::LeapSecsFileReader(std::string withFileName)
{
   initialized = false;
   LoadTimeCoeffFile(withFileName);
}

//------------------------------------------------------------------------------
//  ~LeapSecsFileReader()
//------------------------------------------------------------------------------
/**
 *  Destructor.
 */
//------------------------------------------------------------------------------
LeapSecsFileReader::~LeapSecsFileReader()
{
}

//------------------------------------------------------------------------------
//  LeapSecsFileReader(const LeapSecsFileReader& tcfr)
//------------------------------------------------------------------------------
/**
 *  Copy constructor.
 */
//------------------------------------------------------------------------------
LeapSecsFileReader::LeapSecsFileReader(const LeapSecsFileReader& lsfr)
{
}

//------------------------------------------------------------------------------
//  bool LoadTimeCoeffFile(std::string withFileName)
//------------------------------------------------------------------------------
/**
 */ 
bool LeapSecsFileReader::LoadTimeCoeffFile(std::string withFileName)
{
//   MessageInterface::ShowMessage("LeapSecsFileReader::LoadTimeCoeffFile()\n");

   std::ifstream instream;
   
   instream.open(withFileName.c_str());

   if (instream == NULL)
      return false;

   while (!instream.eof())
   {
      std::string line;
      getline(instream,line);
//      MessageInterface::ShowMessage("LeapSecsFileReader::LoadTimeCoeffFile %s\n",
//                         line.c_str());
      Parse(line);
   }

   initialized = true;
   instream.close();

//   print out of the table
//   for (std::vector<LeapSecondInformation>::iterator i = lookUpTable.end()-1;
//              i >= lookUpTable.begin(); i--)
//   {
//      MessageInterface::ShowMessage("%f\t%f\t%f\t%f\t\n", (*i).julianDate,
//            (*i).offset1, (*i).offset2, (*i).offset3);
//   }


   return true;
}

//------------------------------------------------------------------------------
// bool Parse()
//------------------------------------------------------------------------------
/**
 * Parses each line to add leap second information to the table
 * 
 * Format of the line is: 
 *       YYYY MMM  D =JD jDate  TAI-UTC= off1 S + (MJD - off2) X off3 S
 * @return true if the file parses successfully
 */
//------------------------------------------------------------------------------
bool LeapSecsFileReader::Parse(std::string line)
{
//   MessageInterface::ShowMessage("LeapSecsFileReader::Parse()\n");
   Real jDate, off1, off2, off3;
    
   StringTokenizer stringToken(line," ");
   Integer count = stringToken.CountTokens();
   
   if (count == 15)
   {
      jDate = atof(stringToken.GetToken(4).c_str());
      off1 = atof(stringToken.GetToken(6).c_str());
      off2 = atof(stringToken.GetToken(11).c_str());
      off3 = atof(stringToken.GetToken(13).c_str());
   
      LeapSecondInformation leapSecInfo = {jDate, off1, off2, off3};
      lookUpTable.push_back(leapSecInfo);
      return true;
   }
   else
      return false;

}

//------------------------------------------------------------------------------
// Real NumberOfLeapSecondsFrom(UtcMjd *utcMjd)
//------------------------------------------------------------------------------
/**
 * Converts utcmjd to utcjd and then looks it up from the table.  If file is not
 * read, 0 is returned.
 * 
 * @return number of leap seconds
 * @note Assumes that JD from table is utcjd.
 */
//------------------------------------------------------------------------------
Real LeapSecsFileReader::NumberOfLeapSecondsFrom(UtcMjd utcMjd)
{
   if (initialized)
   {
      Real jd = utcMjd + GmatTimeUtil::JD_MJD_OFFSET;

      for (std::vector<LeapSecondInformation>::iterator i = lookUpTable.end()-1;
                 i >= lookUpTable.begin(); i--)
      {
          if (jd > (*i).julianDate)
          {
             return ((*i).offset1 + ((utcMjd - (*i).offset2) * (*i).offset3));
          }
      }

      return 0;
   }
   else

      return 0;
}
