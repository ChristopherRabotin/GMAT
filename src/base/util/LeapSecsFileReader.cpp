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
#include "GmatConstants.hpp"
#include "StringUtil.hpp"
#include "UtilityException.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>

#include <cstdlib>			// Required for GCC 4.3

//#define DEBUG_READ_LEAP_SECS_FILE

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

         bool    isOK           = true;
         Integer numLinesParsed = 0;
         while (!instream.eof())
         {
            std::string line;
            getline(instream,line);
            if (!GmatStringUtil::IsBlank(line, true))
            {
               if (!(isOK = Parse(line))) break;
               numLinesParsed++;
            }
         }

         if (!isOK)
         {
            std::string errMsg = "Unable to read leap second file "
                                 + withFileName + " - file is malformed\n";
            throw UtilityException(errMsg);
         }
         if (numLinesParsed == 0)
         {
            std::string errMsg = "Unable to read leap second file "
                                 + withFileName + " - file contains no data\n";
            throw UtilityException(errMsg);
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
   #ifdef DEBUG_READ_LEAP_SECS_FILE
      MessageInterface::ShowMessage("Entering LeapSecsFileReader::Parse with line = %s\n", line.c_str());
   #endif
   Real jDate, off1, off2, off3;

   // ignore blank lines
   if (GmatStringUtil::IsBlank(line, true))  return true;

   std::istringstream ss(line);
   Integer year, day;
   std::string month, equalsJD, tai_utc, S, plus, mjd, minus, closeParen, X, S2;
   // clear error flags
   ss.clear();
   try
   {
      ss >> year >> month >> day >> equalsJD >> jDate >> tai_utc >> off1 >> S >> plus >> mjd >> minus >> off2 >> closeParen >> X >> off3 >> S2;
      #ifdef DEBUG_READ_LEAP_SECS_FILE
         MessageInterface::ShowMessage("Entering LeapSecsFileReader::Parse - data read from line are: \n");
         MessageInterface::ShowMessage("         year       = %d\n", year);
         MessageInterface::ShowMessage("         month      = %s\n", month.c_str());
         MessageInterface::ShowMessage("         day        = %d\n", day);
         MessageInterface::ShowMessage("         equalsJD   = %s\n", equalsJD.c_str());
         MessageInterface::ShowMessage("   >>>jDate = %12.10f\n", jDate);
         MessageInterface::ShowMessage("         tai_utc    = %s\n", tai_utc.c_str());
         MessageInterface::ShowMessage("   >>>off1  = %12.10f\n", off1);
         MessageInterface::ShowMessage("         S          = %s\n", S.c_str());
         MessageInterface::ShowMessage("         plus       = %s\n", plus.c_str());
         MessageInterface::ShowMessage("         mjd        = %s\n", mjd.c_str());
         MessageInterface::ShowMessage("         minus      = %s\n", minus.c_str());
         MessageInterface::ShowMessage("   >>>off2  = %12.10f\n", off2);
         MessageInterface::ShowMessage("         closeParen = %s\n", closeParen.c_str());
         MessageInterface::ShowMessage("         X          = %s\n", X.c_str());
         MessageInterface::ShowMessage("   >>>off3  = %12.10f\n", off3);
         MessageInterface::ShowMessage("         S2         = %s\n", S2.c_str());
         if (ss.fail()) MessageInterface::ShowMessage(" ------ fail is true ------\n");
         if (ss.bad())  MessageInterface::ShowMessage(" ------ bad  is true ------\n");
         if (ss.eof())  MessageInterface::ShowMessage(" ------ eof  is true ------\n");
      #endif
      // if there was an error reading all of the items from the stream, return false
         // don't check for eof here, as it will return true when it gets to the end of the line
      if (ss.bad() || ss.fail()) return false;
      LeapSecondInformation leapSecInfo = {jDate, off1, off2, off3};
      lookUpTable.push_back(leapSecInfo);
      return true;
   }
   catch (BaseException &be)
   {
      return false;
   }
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
