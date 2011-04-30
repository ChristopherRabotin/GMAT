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
 *
 * @note The MJD-JD offset used is GmatTimeConstants::JD_NOV_17_1858
 */
//------------------------------------------------------------------------------

#include "LeapSecsFileReader.hpp"
#include "MessageInterface.hpp"
#include "StringTokenizer.hpp"
#include "GmatConstants.hpp"
#include "UtilityException.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>

#include <cstdlib>			// Required for GCC 4.3


//---------------------------------
// static data
//---------------------------------
// hard coded for now - need to allow user to set later
//std::string LeapSecsFileReader::withFileName = "tai-utc.dat";

//---------------------------------
// public
//---------------------------------
//------------------------------------------------------------------------------
//  LeapSecsFileReader()
//------------------------------------------------------------------------------
/**
 *  Constructor.
 */
//------------------------------------------------------------------------------
LeapSecsFileReader::LeapSecsFileReader(const std::string &fileName) :
withFileName     (fileName)
{
   isInitialized = false;
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
// bool Initialize()
//------------------------------------------------------------------------------
bool LeapSecsFileReader::Initialize()
{
   try
   {
      if (!isInitialized)
      {
         std::ifstream instream;

         instream.open(withFileName.c_str());

         if (instream == NULL)
         {
            std::string errMsg = "Unable to locate leap second file "
                                 + withFileName + "\n";
            throw UtilityException(errMsg);
         }

         while (!instream.eof())
         {
            std::string line;
            getline(instream,line);
            Parse(line);
         }

         instream.close();
      }
   }
   catch (...)
   {
      //MessageInterface::PopupMessage(Gmat::WARNING_,
      //                               "Unknown Error in LeapSecondFileReader");
      // re-throw the exception
      throw;
   }

   isInitialized = true;
   return isInitialized;
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
   if (isInitialized)
   {
      Real jd = utcMjd + GmatTimeConstants::JD_MJD_OFFSET;

      // look up each entry in the table to see if value is greater then the
      // julian date
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
