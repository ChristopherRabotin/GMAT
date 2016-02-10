//------------------------------------------------------------------------------
//                                  STKEphemerisFile
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0. 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Author: Linda Jun / NASA
// Created: 2016.01.13
//
/**
 * Writes a spacecraft orbit ephemeris to a file in STK format.
 */
//------------------------------------------------------------------------------

#include "STKEphemerisFile.hpp"
#include "FileUtil.hpp"
#include "FileTypes.hpp"
#include "StringUtil.hpp"
#include "TimeTypes.hpp"             // for FormatCurrentTime()
#include "UtilityException.hpp"
#include "MessageInterface.hpp"
#include "TimeSystemConverter.hpp"   // for TimeConverterUtil::Convert()
#include "A1Mjd.hpp"
#include "Rvector6.hpp"
#include <sstream>

//#define DEBUG_STK_FILE
//#define DEBUG_WRITE_DATA_SEGMENT
//#define DEBUG_FINALIZE

//----------------------------
// static data
//----------------------------

//----------------------------
// public methods
//----------------------------

//------------------------------------------------------------------------------
// default constructor
//------------------------------------------------------------------------------
STKEphemerisFile::STKEphemerisFile() :
   stkFileNameForRead  (""),
   stkFileNameForWrite ("")
{
   InitializeData();
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
STKEphemerisFile::STKEphemerisFile(const STKEphemerisFile &copy) :
   stkFileNameForRead  (copy.stkFileNameForRead),
   stkFileNameForWrite (copy.stkFileNameForWrite)
{
   InitializeData();
}

//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
STKEphemerisFile& STKEphemerisFile::operator=(const STKEphemerisFile &copy)
{
   if (&copy == this)
      return *this;
   
   stkFileNameForRead = copy.stkFileNameForRead;
   stkFileNameForWrite = copy.stkFileNameForWrite;
   InitializeData();
   
   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
STKEphemerisFile::~STKEphemerisFile()
{
   if (stkInStream.is_open())
      stkInStream.close();
   
   if (stkOutStream.is_open())
   {
      stkOutStream.flush();
      stkOutStream.close();
   }
}

//------------------------------------------------------------------------------
// void InitializeData()
//------------------------------------------------------------------------------
/**
 * Clears header information.
 */
//------------------------------------------------------------------------------
void STKEphemerisFile::InitializeData()
{
   firstTimeWriting = true;
   
   scenarioEpochA1Mjd  = 0.0;
   coordinateSystemEpochA1Mjd = 0.0;
   beginSegmentTimeA1Mjd = 0.0;
   endSegmentTimeA1Mjd = 0.0;
   lastEpochWrote = -999.999;
   
   numberOfEphemPoints = 0;
   
   // Reserve space for maximum 10 digits
   dummyNumberOfEphemPoints = "          ";
   scenarioEpochUtcGreg = "";
   centralBody = "";
   coordinateSystem = "";
   coordinateSystemEpochStr = "";
   beginSegmentTimeStr = "";
   endSegmentTimeStr = "";
   ephemTypeForRead = "";
   ephemTypeForWrite = "";
   numEphemPointsBegPos = 0;
}

//------------------------------------------------------------------------------
// bool OpenForRead(const std::string &filename, const std::string &ephemType)
//------------------------------------------------------------------------------
/**
 * Opens STK ephemeris (.e) file for reading.
 *
 * @filename File name to open
 * @ephemType Ephemeris type to read, at this time only "TimePos" or "TimePosVel"
 *               is allowed
 */
//------------------------------------------------------------------------------
bool STKEphemerisFile::OpenForRead(const std::string &filename,
                                   const std::string &ephemType)
{
   #ifdef DEBUG_STK_FILE
   MessageInterface::ShowMessage
      ("STKEphemerisFile::OpenForRead() entered, filename='%s'\n", filename.c_str());
   #endif
   
   bool retval = false;
   
   // Check ephem type
   // Currently only TimePos and TimePosVel are allowed
   if (ephemType != "TimePos" && ephemType != "TimePosVel")
   {
      UtilityException ue;
      ue.SetDetails("STKEphemerisFile::OpenForRead() *** INTERNAL ERROR *** "
                    "Only TimePos or TimePosVel is valid for read on STK "
                    "ephemeris file '%s'.", stkFileNameForRead.c_str());
      throw ue;
   }
   
   if (stkInStream.is_open())
      stkInStream.close();
   
   stkFileNameForRead = filename;
   ephemTypeForRead = ephemType;
   stkInStream.open(stkFileNameForRead.c_str());
   
   if (stkInStream.is_open())
   {
      retval = true;
      #ifdef DEBUG_STK_FILE
      MessageInterface::ShowMessage
         ("   Successfully opened '%s' for reading\n", stkFileNameForRead.c_str());
      #endif
   }
   else
   {
      retval = false;
      #ifdef DEBUG_STK_FILE
      MessageInterface::ShowMessage
         ("   Failed open '%s' for reading\n", stkFileNameForRead.c_str());
      #endif
   }
   
   #ifdef DEBUG_STK_FILE
   MessageInterface::ShowMessage
      ("STKEphemerisFile::OpenForRead() returning %d\n", retval);
   #endif
   return retval;
}


//------------------------------------------------------------------------------
// bool OpenForWrite(const std::string &filename, const std::string &ephemType)
//------------------------------------------------------------------------------
/**
 * Opens STK ephemeris (.e) file for writing.
 *
 * @filename File name to open
 * @ephemType Ephemeris type to write, at this time only "TimePos" or "TimePosVel"
 *               is allowed
 */
//------------------------------------------------------------------------------
bool STKEphemerisFile::OpenForWrite(const std::string &filename,
                                    const std::string &ephemType)
{
   #ifdef DEBUG_STK_FILE
   MessageInterface::ShowMessage
      ("STKEphemerisFile::OpenForWrite() entered, filename='%s'\n", filename.c_str());
   #endif
   
   bool retval = false;
   
   // Check ephem type
   // Currently only TimePos and TimePosVel are allowed
   if (ephemType != "TimePos" && ephemType != "TimePosVel")
   {
      UtilityException ue;
      ue.SetDetails("STKEphemerisFile::OpenForWrite() *** INTERNAL ERROR *** "
                    "Only TimePos or TimePosVel is valid for writing to STK "
                    "ephemeris file '%s'.", stkFileNameForWrite.c_str());
      throw ue;
   }
   
   if (stkOutStream.is_open())
      stkOutStream.close();
   
   stkFileNameForWrite = filename;
   ephemTypeForWrite = ephemType;
   
   stkOutStream.open(stkFileNameForWrite.c_str());
   
   if (stkOutStream.is_open())
   {
      retval = true;
      #ifdef DEBUG_STK_FILE
      MessageInterface::ShowMessage
         ("   Successfully opened '%s' for writing\n", stkFileNameForWrite.c_str());
      #endif
   }
   else
   {
      retval = false;
      #ifdef DEBUG_STK_FILE
      MessageInterface::ShowMessage
         ("   Failed open '%s' for writing\n", stkFileNameForWrite.c_str());
      #endif
   }
   
   #ifdef DEBUG_STK_FILE
   MessageInterface::ShowMessage
      ("STKEphemerisFile::OpenForWrite() returning %d\n", retval);
   #endif
   return retval;
}

//------------------------------------------------------------------------------
// void CloseForRead()
//------------------------------------------------------------------------------
void STKEphemerisFile::CloseForRead()
{
   if (stkInStream.is_open())
      stkInStream.close();
}

//------------------------------------------------------------------------------
// void CloseForWrite()
//------------------------------------------------------------------------------
void STKEphemerisFile::CloseForWrite()
{
   if (stkOutStream.is_open())
      stkOutStream.close();
}

//------------------------------------------------------------------------------
// void SetVersion(const std::string &version)
//------------------------------------------------------------------------------
void STKEphemerisFile::SetVersion(const std::string &version)
{
   #ifdef DEBUG_VERSION
   MessageInterface::ShowMessage
      ("STKEphemerisFile::SetVersion() entered, stkVersion='%s'\n",
       stkVersion.c_str());
   #endif
   
   stkVersion = version;
}

//------------------------------------------------------------------------------
// bool SetHeaderForWriting(const std::string &fieldName,
//                          const std::string &value)
//------------------------------------------------------------------------------
/**
 * Sets STK header data for writing. It does not validate the input value.
 *
 * @param fieldName Header field name to be set
 * @param value Value to set to the field
 */
//------------------------------------------------------------------------------
bool STKEphemerisFile::SetHeaderForWriting(const std::string &fieldName,
                                           const std::string &value)
{
   if (fieldName == "Version")
      stkVersion = value;
   else if (fieldName == "CentralBody")
      centralBody = value;
   else if (fieldName == "CoordinateSystem")
      coordinateSystem = value;
   else if (fieldName == "CoordinateSystemEpoch")
      coordinateSystemEpochStr = value;
   else if (fieldName == "BeginSegmentTime")
      beginSegmentTimeStr = value;
   else if (fieldName == "EndSegmentTime")
      endSegmentTimeStr = value;
   else
      throw UtilityException
         ("The field \"" + fieldName + "\" is not valid STK header field.\n"
          "Valid fields are: TBS");
   
   return true;
}

//------------------------------------------------------------------------------
// bool WriteHeader()
//------------------------------------------------------------------------------
/**
 * Formats and writes header to a file. If output stream is not opened,
 * it just returns false.
 */
//------------------------------------------------------------------------------
bool STKEphemerisFile::WriteHeader()
{
   #ifdef DEBUG_WRITE_HEADER
   MessageInterface::ShowMessage
      ("STKEphemerisFile::WriteHeader() entered, stkVersion='%s'\n", stkVersion.c_str());
   #endif
   
   if (!stkOutStream.is_open())
   {
      #ifdef DEBUG_WRITE_HEADER
      MessageInterface::ShowMessage
         ("STKEphemerisFile::WriteHeader() returning false, stkOutStream is not opened\n");
      #endif
      return false;
   }
   
   std::string ephemFormat = "Ephemeris" + ephemTypeForWrite;
   scenarioEpochUtcGreg = A1ModJulianToUtcGregorian(scenarioEpochA1Mjd, 1);
   
   std::stringstream ss("");
   ss << stkVersion << std::endl;
   ss << "BEGIN Ephemeris" << std::endl;
   stkOutStream << ss.str();
   
   numEphemPointsBegPos = stkOutStream.tellp();
   #ifdef DEBUG_WRITE_HEADER
   MessageInterface::ShowMessage("numEphemPointsBegPos=%ld\n", (long)numEphemPointsBegPos);
   #endif
   
   // Clear stream contents
   ss.str("");
   
   // numberOfEphemerisPoints need to be updated later
   ss << "NumberOfEphemerisPoints " << dummyNumberOfEphemPoints << std::endl;
   ss << "ScenarioEpoch           " << scenarioEpochUtcGreg << std::endl;
   ss << "CentralBody             " << centralBody << std::endl;
   ss << "CoordinateSystem        " << coordinateSystem << std::endl;
   ss << std::endl;
   ss << ephemFormat << std::endl;
   ss << std::endl;
   
   stkOutStream << ss.str();
   stkOutStream.flush();
   
   #ifdef DEBUG_WRITE_HEADER
   MessageInterface::ShowMessage
      ("STKEphemerisFile::WriteHeader() returning true\n");
   #endif
   
   return true;
}

//------------------------------------------------------------------------------
// bool WriteBlankLine()
//------------------------------------------------------------------------------
/**
 * Writes blank line to a file.
 */
//------------------------------------------------------------------------------
bool STKEphemerisFile::WriteBlankLine()
{
   if (!stkOutStream.is_open())
      return false;
   
   stkOutStream << std::endl;
   stkOutStream.flush();
   return true;
}

//------------------------------------------------------------------------------
// bool WriteString(const std::string &str)
//------------------------------------------------------------------------------
/**
 * Writes input string to a file.
 */
//------------------------------------------------------------------------------
bool STKEphemerisFile::WriteString(const std::string &str)
{
   #ifdef DEBUG_WRITE_STRING
   MessageInterface::ShowMessage
      ("STKEphemerisFile::WriteString() entered, str='%s'\n", str.c_str());
   #endif
   
   if (!stkOutStream.is_open())
      return false;
   
   stkOutStream << str << std::endl;
   stkOutStream.flush();
   
   #ifdef DEBUG_WRITE_STRING
   MessageInterface::ShowMessage
      ("STKEphemerisFile::WriteString() returning true\n");
   #endif
   return true;
}

//------------------------------------------------------------------------------
// bool WriteDataSegment(const EpochArray &epochArray, const StateArray &stateArray, 
//                       bool canFinalize = false);
//------------------------------------------------------------------------------
bool STKEphemerisFile::WriteDataSegment(const EpochArray &epochArray,
                                        const StateArray &stateArray, 
                                        bool canFinalize)
{
   Integer numPoints = stateArray.size();
   #ifdef DEBUG_WRITE_DATA_SEGMENT
   MessageInterface::ShowMessage
      ("============================================================\n"
       "STKEphemerisFile::WriteDataSegment() entered, numPoints=%d, canFinalize=%d\n",
       numPoints, canFinalize);
   #endif
   
   if (numPoints == 0)
   {
      #ifdef DEBUG_WRITE_DATA_SEGMENT
      MessageInterface::ShowMessage
         ("STKEphemerisFile::WriteDataSegment() just returning true, data size is zero\n");
      #endif
      return true;
   }
   else if (numPoints != stateArray.size())
   {
      UtilityException ue;
      ue.SetDetails("STKEphemerisFile::WriteDataSegment() *** INTERNAL ERROR *** "
                    "Received different number of timeS and stateS. size of time "
                    "array: %d, size of state array: %d\n", epochArray.size(),
                    stateArray.size());
      throw ue;
   }
   
   // If first time writing, save scenario epoch
   if (firstTimeWriting)
   {
      scenarioEpochA1Mjd = (epochArray[0])->GetReal();
      WriteHeader();
      firstTimeWriting = false;
   }
   
   if (ephemTypeForWrite == "TimePosVel")
      WriteTimePosVel(epochArray, stateArray);
   else if (ephemTypeForWrite == "TimePos")
      WriteTimePos(epochArray, stateArray);
   else
   {
      // ephemType has already validated in OpenFile but check anyway
      UtilityException ue;
      ue.SetDetails("STKEphemerisFile::WriteDataSegment() *** INTERNAL ERROR *** "
                    "Only TimePos or TimePosVel is valid for read on STK "
                    "ephemeris file '%s'.", stkFileNameForWrite.c_str());
      throw ue;
   }
   
   // If final data segment received, write end ephemeris
   if (canFinalize)
      FinalizeEphemeris();
   
   #ifdef DEBUG_WRITE_DATA_SEGMENT
   MessageInterface::ShowMessage("STKEphemerisFile::WriteDataSegment() leavng\n");
   #endif
   return true;
}

//----------------------------
// protected methods
//----------------------------

//------------------------------------------------------------------------------
// void WriteTimePos(const EpochArray &epochArray, const StateArray &stateArray)
//------------------------------------------------------------------------------
/**
 * Writes STK ephemeris in EphemerisTimePos format
 */
//------------------------------------------------------------------------------
void STKEphemerisFile::WriteTimePos(const EpochArray &epochArray,
                                    const StateArray &stateArray)
{
   Integer numPoints = stateArray.size();
   
   // Write out to stream
   for (int i = 0; i < numPoints; i++)
   {
      // For multiple segments, end epoch of previous segment may be the same as
      // beginning epoch of new segmnet, so check for duplicate epoch and use the
      // state of new epoch since any maneuver or some spacecraft property update
      // may happened.
      Real epoch = (epochArray[i])->GetReal();
      if (epoch == lastEpochWrote)
         continue;
      
      const Real *outState = (stateArray[i])->GetDataVector();
      Real timeIntervalInSecs = (epoch - scenarioEpochA1Mjd) * 86400.0;
      char strBuff[200];
      sprintf(strBuff, "%1.15e  % 1.15e  % 1.15e  % 1.15e\n",
              timeIntervalInSecs, outState[0], outState[1], outState[2]);
      stkOutStream << strBuff;
      
      #ifdef DEBUG_WRITE_POS
      std::string epochStr = A1ModJulianToUtcGregorian(epochArray[i], 2);
      MessageInterface::ShowMessage("   %s  % 1.15e\n", epochStr, outState[0]);
      #endif
      
      lastEpochWrote = epoch;
      numberOfEphemPoints++;
   }
}

//------------------------------------------------------------------------------
// void WriteTimePosVel(const EpochArray &epochArray, const StateArray &stateArray)
//------------------------------------------------------------------------------
/**
 * Writes STK ephemeris in EphemerisTimePosVel format
 */
//------------------------------------------------------------------------------
void STKEphemerisFile::WriteTimePosVel(const EpochArray &epochArray,
                                       const StateArray &stateArray)
{
   Integer numPoints = stateArray.size();
   
   // Write out to stream
   for (int i = 0; i < numPoints; i++)
   {
      // For multiple segments, end epoch of previous segment may be the same as
      // beginning epoch of new segmnet, so check for duplicate epoch and use the
      // state of new epoch since any maneuver or some spacecraft property update
      // may happened.
      Real epoch = (epochArray[i])->GetReal();
      if (epoch == lastEpochWrote)
         continue;
      
      // Format output using scientific notation
      const Real *outState = (stateArray[i])->GetDataVector();
      Real timeIntervalInSecs = (epoch - scenarioEpochA1Mjd) * 86400.0;
      char strBuff[200];
      sprintf(strBuff, "%1.15e  % 1.15e  % 1.15e  % 1.15e  % 1.15e  % 1.15e  % 1.15e\n",
              timeIntervalInSecs, outState[0], outState[1], outState[2], outState[3],
              outState[4], outState[5]);
      stkOutStream << strBuff;
      
      #ifdef DEBUG_WRITE_POSVEL
      std::string epochStr = A1ModJulianToUtcGregorian(epochArray[i], 2);
      MessageInterface::ShowMessage("   %s  % 1.15e\n", epochStr, outState[0]);
      #endif
      
      lastEpochWrote = epoch;
      numberOfEphemPoints++;
   }
}

//------------------------------------------------------------------------------
// void FinalizeEphemeris()
//------------------------------------------------------------------------------
/**
 * Finalized ephemeris file. It writes number of data points and end ephemeris
 * keyword.
 */
//------------------------------------------------------------------------------
void STKEphemerisFile::FinalizeEphemeris()
{
   #ifdef DEBUG_FINALIZE
   MessageInterface::ShowMessage
      ("STKEphemerisFile::FinalizeEphemeris() entered, numberOfEphemPoints=%d, "
       "numEphemPointsBegPos=%ld\n", numberOfEphemPoints, (long)numEphemPointsBegPos);
   #endif
   
   // Write end ephemeris keyword
   stkOutStream << "END Ephemeris" << std::endl << std::endl;
   stkOutStream.flush();
   
   // Write actual number of ephemeris points
   stkOutStream.seekp(numEphemPointsBegPos, std::ios_base::beg);
   std::stringstream ss("");
   ss << "NumberOfEphemerisPoints " << numberOfEphemPoints;
   stkOutStream << ss.str();
   stkOutStream.flush();
   
   #ifdef DEBUG_FINALIZE
   MessageInterface::ShowMessage("STKEphemerisFile::FinalizeEphemeris() leaving\n");
   #endif
}

//------------------------------------------------------------------------------
// std::string A1ModJulianToUtcGregorian(Real epochInDays, Integer format = 1)
//------------------------------------------------------------------------------
/**
 * Formats epoch in either in days or seconds to desired format.
 *
 * @param  epoch   Epoch in A1 days
 * @param  format  Desired output format [1]
 *                 1 = "01 Jan 2000 11:59:28.000"
 *                 2 = "2000-01-01T11:59:28.000"
 */
//------------------------------------------------------------------------------
std::string STKEphemerisFile::A1ModJulianToUtcGregorian(Real epochInDays, Integer format)
{
   if (epochInDays == -999.999)
      return "-999.999";
   
   Real toMjd;
   std::string epochStr;
   std::string outFormat = "UTCGregorian";
   
   // Convert current epoch to specified format
   TimeConverterUtil::Convert("A1ModJulian", epochInDays, "", outFormat,
                              toMjd, epochStr, format);
   
   if (epochStr == "")
   {
      MessageInterface::ShowMessage
         ("**** ERROR **** EphemerisWriter::ToUtcGregorian() Cannot convert epoch %.10f %s "
          "to UTCGregorian\n", epochInDays, "days");
      
      epochStr = "EpochError";
   }
   
   return epochStr;
}
