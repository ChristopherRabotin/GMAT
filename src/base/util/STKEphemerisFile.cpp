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
#include "GmatGlobal.hpp"
#include <sstream>
#include <limits>

// We want to use std::numeric_limits<std::streamsize>::max()
#ifdef _MSC_VER  // if Microsoft Visual C++
#undef max
#endif

//#define DEBUG_STK_FILE
//#define DEBUG_INITIALIZE
//#define DEBUG_WRITE_DATA_SEGMENT
//#define DEBUG_FINALIZE
//#define DEBUG_INITIAL_FINAL

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
   #ifdef DEBUG_STK_EPHEMFILE_INIT
   MessageInterface::ShowMessage("STKEphemerisFile::InitializeData() entered\n");
   #endif
   
   firstTimeWriting = true;
   openForTempOutput = true;
   
   scenarioEpochA1Mjd  = 0.0;
   coordinateSystemEpochA1Mjd = 0.0;
   beginSegmentTime = 0.0;
   lastEpochWrote = -999.999;
   beginSegmentArray.clear();
   numberOfEphemPoints = 0;
   interpolationOrder = 0;
   
   scenarioEpochUtcGreg = "";
   interpolationMethod = "";
   centralBody = "";
   coordinateSystem = "";
   coordinateSystemEpochStr = "";
   distanceUnit = "";
   ephemTypeForRead = "";
   ephemTypeForWrite = "";
   stkTempFileName = "";
   numEphemPointsBegPos = 0;
   
   initialSecsFromEpoch = 0.0;
   finalSecsFromEpoch   = 0.0;
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
      ("STKEphemerisFile::OpenForWrite() entered, filename='%s', openForTempOutput=%d\n",
       filename.c_str(), openForTempOutput);
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
   
   // Open temporary file to write STK ephemeris since header data needs to be
   // updated after final data
   if (openForTempOutput)
   {
      stkOutStream.close();
      std::string tempPath = GmatFileUtil::GetTemporaryDirectory();
      std::string fileNameNoPath = GmatFileUtil::ParseFileName(filename);
      stkTempFileName = tempPath + fileNameNoPath;
      #ifdef DEBUG_STK_FILE
      MessageInterface::ShowMessage
         ("          tempPath='%s'\n   stkTempFileName='%s'\n", tempPath.c_str(),
          stkTempFileName.c_str());
      #endif
      stkOutStream.open(stkTempFileName.c_str());
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
// bool GetInitialAndFinalStates(Real &initialA1Mjd, Real &finalA1Mjd,
//                               Rvector6 &initialState, Rvector6 &finalState,
//                               std::string &cbName, std::string &csName)
//------------------------------------------------------------------------------
/**
 * Retrieves initial and final state from STK ephem file. Assumes ephem file
 * is successfully opened via OpenForRead()
 *
 * @param initialA1Mjd  output initial epoch in a1mjd
 * @param finalA1Mjd    output final epoch in a1mjd
 * @param initialState  output initial cartesian state in MJ2000Eq
 * @param finalState    output final cartesian state in MJ2000Eq
 * @param cbName        output central body name
 * @param csName        output coordinate system name
 *
 * @return true if all output parameter values are assigned, false otherwise
 */
//------------------------------------------------------------------------------
bool STKEphemerisFile::GetInitialAndFinalStates(Real &initialA1Mjd, Real &finalA1Mjd,
                                                Rvector6 &initialState, Rvector6 &finalState,
                                                std::string &cbName, std::string &csName)
{
   #ifdef DEBUG_INITIAL_FINAL
   MessageInterface::ShowMessage
      ("STKEphemerisFile::GetInitialAndFinalState() entered\n");
   #endif
   
   // Set defaults for output
   cbName = "Earth";
   csName = "J2000";
   
   // Read headers
   std::string line1, line;
   std::string stkVersionKeyword = "stk.v.";
   std::string beginEphemKeyword = "BEGIN Ephemeris";
   std::string numEphemPointsKeyword = "NumberOfEphemerisPoints";
   std::string scenarioEpochKeyword = "ScenarioEpoch";
   std::string centralBodyKeyword = "CentralBody";
   std::string coordSystemKeyword = "CoordinateSystem";
   std::string timePosVelKeyword = "EphemerisTimePosVel";
   std::string distanceUnitKeyword = "DistanceUnit";
   std::string item;
   std::string::size_type index1;
   
   // Required header fields
   bool stkVersionFound = false;
   bool beginEphemFound = false;
   bool numEphemPointsFound = false;
   bool scenarioEpochFound = false;
   bool timePosVelFound = false;
   
   while (!stkInStream.eof())
   {
      getline(stkInStream, line1);
      #ifdef DEBUG_INITIAL_FINAL_MORE
      MessageInterface::ShowMessage
         ("   ............... line = %s\n", line1.c_str());
      #endif
      
      line = GmatStringUtil::Strip(line1);
      
      // Skip comment line
      if ((line.size() > 1) && (line[0] == '#'))
         continue;
      
      // Now go through each keyword
      if (line.find(stkVersionKeyword) != line.npos)
      {
         index1 = line.find(stkVersionKeyword);
         item = line.substr(index1 + stkVersionKeyword.size());
         item = GmatStringUtil::Strip(item);
         #ifdef DEBUG_INITIAL_FINAL
         MessageInterface::ShowMessage
            ("   %24s%s\n", stkVersionKeyword.c_str(), item.c_str());
         #endif
         if (item.size() > 0)
            stkVersionFound = true;
      }
      else if (line.find(beginEphemKeyword) != line.npos)
      {
         index1 = line.find(beginEphemKeyword);
         item = line.substr(index1 + beginEphemKeyword.size());
         item = GmatStringUtil::Strip(item);
         #ifdef DEBUG_INITIAL_FINAL
         MessageInterface::ShowMessage
            ("   %24s%s\n", beginEphemKeyword.c_str(), item.c_str());
         #endif
         if (item == "")
            beginEphemFound = true;
      }
      else if (line.find(numEphemPointsKeyword) != line.npos)
      {
         numEphemPointsFound = true;
         index1 = line.find(numEphemPointsKeyword);
         item = line.substr(index1 + numEphemPointsKeyword.size());
         item = GmatStringUtil::Strip(item);
         #ifdef DEBUG_INITIAL_FINAL
         MessageInterface::ShowMessage
            ("   %24s : %s\n", numEphemPointsKeyword.c_str(), item.c_str());
         #endif
      }
      else if (line.find(scenarioEpochKeyword) != line.npos)
      {
         scenarioEpochFound = true;
         index1 = line.find(scenarioEpochKeyword);
         item = line.substr(index1 + scenarioEpochKeyword.size());
         item = GmatStringUtil::Strip(item);
         #ifdef DEBUG_INITIAL_FINAL
         MessageInterface::ShowMessage
            ("   %24s : %s\n", scenarioEpochKeyword.c_str(), item.c_str());
         #endif
         scenarioEpochUtcGreg = item;
         // Convert epoch from UTCGregorian to A1Mjd
         if (UTCGregorianToA1ModJulian(item, scenarioEpochA1Mjd))
         {
            #ifdef DEBUG_INITIAL_FINAL
            MessageInterface::ShowMessage
               ("         scenarioEpochA1Mjd : %.12f\n", scenarioEpochA1Mjd);
            #endif
         }
         else
         {
            MessageInterface::ShowMessage
               ("*** ERROR *** Cannot convet ScenarioEpoch '%s' to A1ModJulian read "
                "from ephemeris file '%s'\n", item.c_str(), stkFileNameForRead.c_str());
            return false;
            // UtilityException ue;
            // ue.SetDetails("STKEphemerisFile::GetInitialAndFinalStates() Error converting "
            //               "ScenarioEpoch: '%s' to A1ModJulian on input STK "
            //               "ephemeris file '%s'.", stkFileNameForRead.c_str());
            // throw ue;
         }
      }
      else if (line.find(centralBodyKeyword) != line.npos)
      {
         index1 = line.find(centralBodyKeyword);
         item = line.substr(index1 + centralBodyKeyword.size());
         item = GmatStringUtil::Strip(item);
         #ifdef DEBUG_INITIAL_FINAL
         MessageInterface::ShowMessage
            ("   %24s : %s\n", centralBodyKeyword.c_str(), item.c_str());
         #endif
         centralBody = item;
         cbName = item;
      }
      else if (line.find(coordSystemKeyword) != line.npos)
      {
         index1 = line.find(coordSystemKeyword);
         item = line.substr(index1 + coordSystemKeyword.size());
         item = GmatStringUtil::Strip(item);
         #ifdef DEBUG_INITIAL_FINAL
         MessageInterface::ShowMessage
            ("   %24s : %s\n", coordSystemKeyword.c_str(), item.c_str());
         #endif
         coordinateSystem = item;
         csName = item;
      }
      else if (line.find(distanceUnitKeyword) != line.npos)
      {
         index1 = line.find(distanceUnitKeyword);
         item = line.substr(index1 + distanceUnitKeyword.size());
         item = GmatStringUtil::Strip(item);
         #ifdef DEBUG_INITIAL_FINAL
         MessageInterface::ShowMessage
            ("   %24s : %s\n", distanceUnitKeyword.c_str(), item.c_str());
         #endif
         distanceUnit = item;		// The reader will use this in propagation
      }
      // This field shoud be the last one before reading actual ephem states
      else if (line.find(timePosVelKeyword) != line.npos)
      {
         index1 = line.find(timePosVelKeyword);
         item = line.substr(index1 + timePosVelKeyword.size());
         item = GmatStringUtil::Strip(item);
         #ifdef DEBUG_INITIAL_FINAL
         MessageInterface::ShowMessage
            ("   %24s%s\n", timePosVelKeyword.c_str(), item.c_str());
         #endif
         if (item == "")
            timePosVelFound = true;
         
         break;
      }
   }
   
   if (!stkVersionFound || !beginEphemFound || !numEphemPointsFound ||
       !scenarioEpochFound || !timePosVelFound)
   {
      MessageInterface::ShowMessage
         ("*** ERROR *** Cannot find required header info from '%s'. "
          "Missing one or more of the following keywords:\n   "
          "\" '%s'  '%s'  '%s'  '%s'  '%s' \"\n", stkFileNameForRead.c_str(),
          stkVersionKeyword.c_str(), beginEphemKeyword.c_str(),
          numEphemPointsKeyword.c_str(), scenarioEpochKeyword.c_str(),
          timePosVelKeyword.c_str());
      return false;
   }
   
   // Read initial TimePosVel
   bool firstStateFound = false;
   StringArray items;
   while (!stkInStream.eof())
   {
      // Use cross-platform GetLine
      //GmatFileUtil::GetLine(&stkInStream, line);
      getline(stkInStream, line);
      if (line != "")
      {
         #ifdef DEBUG_INITIAL_FINAL
         MessageInterface::ShowMessage("   first line =\n   '%s'\n", line.c_str());
         #endif
         // Check if line has 7 items
         items = GmatStringUtil::SeparateBy(line, " ");
         if (items.size() != 7)
         {
            MessageInterface::ShowMessage
               ("*** ERROR *** Did not find correct number of elements in the first data\n");
            break;
         }
         else
         {
            if (GetEpochAndState(line, initialSecsFromEpoch, initialState))
            {
               firstStateFound = true;
               #ifdef DEBUG_INITIAL_FINAL
               MessageInterface::ShowMessage
                  ("   initialSecsFromEpoch = %.12f\n   initialState = %s\n",
                   initialSecsFromEpoch, initialState.ToString().c_str());
               #endif
            }
            break;
         }
      }
   }
   
   if (!firstStateFound)
   {
      MessageInterface::ShowMessage("*** ERROR *** There are no ephemeris data points\n");
      return false;
   }
   
   // Read final TimePosVel
   // Last line should be 'END Ephemeris', how can I read last non-blank line before this?
   // This may not be efficient for huge file
   std::string lastLine;
   bool badDataFound = false;
   bool lastStateFound = false;
   while (!stkInStream.eof())
   {
      getline(stkInStream, line);
      if (line != "")
      {
         #ifdef DEBUG_INITIAL_FINAL_MORE
         MessageInterface::ShowMessage("   line =\n   '%s'\n", line.c_str());
         #endif
         if (line.find("END Ephemeris") != line.npos)
         {
            #ifdef DEBUG_INITIAL_FINAL
            MessageInterface::ShowMessage("   'END Ephemeris' found\n");
            #endif
            break;
         }
         else
         {
            // Check if line has 7 items
            StringArray items = GmatStringUtil::SeparateBy(line, " ");
            if (items.size() != 7)
            {
               // Has bad data points
               badDataFound = true;
               MessageInterface::ShowMessage
                  ("*** ERROR *** Did not find correct number of elements in the data\n");
               break;
            }
            else
            {
               lastLine = line;
               lastStateFound = true;
            }
         }
      }
   }
   
   if (!badDataFound)
   {
      if (lastStateFound)
      {
         GetEpochAndState(lastLine, finalSecsFromEpoch, finalState);
         #ifdef DEBUG_INITIAL_FINAL
         MessageInterface::ShowMessage
            ("   finalSecsFromEpoch = %.12f\n   finalState = %s\n", finalSecsFromEpoch,
             finalState.ToString().c_str());
         #endif
      }
      else
      {
         // If there is only one line, set initial to final
         #ifdef DEBUG_INITIAL_FINAL
         MessageInterface::ShowMessage("There are only one ephemeris data point\n");
         #endif
         finalSecsFromEpoch = initialSecsFromEpoch;
         finalState = initialState;
         lastStateFound = true;
      }
   }
   
   // This code block looks more efficient, but how can I read backward so I can
   // get good ephemeris data. There may be blank lines between data lines
   #if 0
   line = GetLastLine();
   bool lastStateFound = false;
   if (line != "")
   {
      #ifdef DEBUG_INITIAL_FINAL
      MessageInterface::ShowMessage("   last line =\n   '%s'\n", line.c_str());
      #endif
      // Check if line has 7 items
      StringArray items = GmatStringUtil::SeparateBy(line, " ");
      if (items.size() != 7)
      {
         MessageInterface::ShowMessage
            ("*** ERROR *** Did not find correct number of elements in the last data\n");
      }
      else
      {
         if (GetEpochAndState(line, finalSecsFromEpoch, finalState))
            lastStateFound = true;
      }
   }
   #endif
   
   // Convert initial and final epoch to A1ModJulian
   initialA1Mjd = scenarioEpochA1Mjd + initialSecsFromEpoch/86400.0;
   finalA1Mjd = scenarioEpochA1Mjd + finalSecsFromEpoch/86400.0;
   
   #ifdef DEBUG_INITIAL_FINAL
   MessageInterface::ShowMessage
      ("STKEphemerisFile::GetInitialAndFinalState() returning %d\n", lastStateFound);
   #endif
   return lastStateFound;
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
// void SetInterpolationOrder(Integer order)
//------------------------------------------------------------------------------
void STKEphemerisFile::SetInterpolationOrder(Integer order)
{
   interpolationOrder = order;
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
   else if (fieldName == "InterpolationMethod")
      interpolationMethod = value;
   else if (fieldName == "CentralBody")
      centralBody = value;
   else if (fieldName == "CoordinateSystem")
      coordinateSystem = value;
   else if (fieldName == "CoordinateSystemEpoch")
      coordinateSystemEpochStr = value;
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
   ss << "# WrittenBy    GMAT " << GmatGlobal::Instance()->GetGmatVersion() << std::endl;
   ss << "BEGIN Ephemeris" << std::endl;
   stkOutStream << ss.str();
   
   numEphemPointsBegPos = stkOutStream.tellp();
   #ifdef DEBUG_WRITE_HEADER
   MessageInterface::ShowMessage("numEphemPointsBegPos=%ld\n", (long)numEphemPointsBegPos);
   #endif
   
   // Clear stream contents
   ss.str("");
   
   ss << "NumberOfEphemerisPoints " << numberOfEphemPoints << std::endl;
   ss << "ScenarioEpoch           " << scenarioEpochUtcGreg << std::endl;
   
   // Write interpolation info if not blank
   if (interpolationMethod != "")
   {
      // Figure out actual interpolation order
      Integer actualInterpOrder = interpolationOrder;
      if (numberOfEphemPoints <= interpolationOrder)
         actualInterpOrder = numberOfEphemPoints - 1;
      if (numberOfEphemPoints == 1)
         actualInterpOrder = 1;
      ss << "InterpolationMethod     " << interpolationMethod << std::endl;
      ss << "InterpolationOrder      " << actualInterpOrder << std::endl;
   }
   
   ss << "CentralBody             " << centralBody << std::endl;
   ss << "CoordinateSystem        " << coordinateSystem << std::endl;
   
   // GMAT writes in km only for now
   ss << "DistanceUnit            Kilometers" << std::endl;

   // Write begin segment times if not empty
   if (!beginSegmentArray.empty())
   {
      char strBuff[200];
      ss << "BEGIN SegmentBoundaryTimes" << std::endl;
      for (unsigned int i = 0; i < beginSegmentArray.size(); i++)
      {
         sprintf(strBuff, "   %1.15e\n", beginSegmentArray[i]);
         ss << strBuff;
      }
      ss << "END SegmentBoundaryTimes" << std::endl;
   }
   
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
      openForTempOutput = true;
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
   {
      FinalizeEphemeris();
   }
   else
   {
      // Indicate new segment starting by writing blank line and the last data
      WriteBlankLine();
      #ifdef DEBUG_WRITE_DATA_SEGMENT
      WriteString("# ============================== new segment");
      #endif
      Integer last = numPoints - 1;
      Real lastEpoch = (epochArray[last])->GetReal();
      WriteTimePosVel(lastEpoch, stateArray[last]);
      if (firstTimeWriting)
         beginSegmentArray.push_back(0.0);
      beginSegmentTime = (lastEpoch - scenarioEpochA1Mjd) * 86400.0;
      beginSegmentArray.push_back(beginSegmentTime);
   }
   
   firstTimeWriting = false;
   
   #ifdef DEBUG_WRITE_DATA_SEGMENT
   MessageInterface::ShowMessage("STKEphemerisFile::WriteDataSegment() leavng\n");
   #endif
   return true;
}

//----------------------------
// protected methods
//----------------------------

//------------------------------------------------------------------------------
// bool GetEpochAndState(const std::string &line, Real &epoch, Rvector6 &state)
//------------------------------------------------------------------------------
bool STKEphemerisFile::GetEpochAndState(const std::string &line, Real &epoch, Rvector6 &state)
{
   #ifdef DEBUG_INITIAL_FINAL
   MessageInterface::ShowMessage
      ("STKEphemerisFile::GetEpochAndState() entered\n   line =\n   '%s'\n", line.c_str());
   #endif
   
   // Parse line
   bool retval = false;
   StringArray items = GmatStringUtil::SeparateBy(line, " ", false, false, false);
   if (items.size() == 7)
   {
      retval = true;
      Real rval;
      #ifdef DEBUG_INITIAL_FINAL
      for (unsigned int i = 0; i < items.size(); i++)
         MessageInterface::ShowMessage("   items[%d] = '%s'\n", i, items[i].c_str());
      #endif
      if (GmatStringUtil::ToReal(items[0], rval))
         epoch = rval;
      else
      {
         retval = false;
         MessageInterface::ShowMessage("*** ERROR *** '%s' is not a valid real number\n", items[0].c_str());
      }
      for (unsigned int i = 1; i < items.size(); i++)
      {
         if (GmatStringUtil::ToReal(items[i], rval))
            state[i-1] = rval;
         else
         {
            retval = false;
            MessageInterface::ShowMessage("*** ERROR *** '%s' is not a valid real number\n", items[i].c_str());
         }
      }
   }
   
   #ifdef DEBUG_INITIAL_FINAL
   MessageInterface::ShowMessage
      ("STKEphemerisFile::GetEpochAndState() returing %d\n", retval);
   #endif
   return retval;
}

//------------------------------------------------------------------------------
// std::string GetLastLine()
//------------------------------------------------------------------------------
std::string STKEphemerisFile::GetLastLine()
{
   std::ifstream::pos_type pos = stkInStream.tellg();
   std::ifstream::pos_type lastPos;
   while (stkInStream >> std::ws && IgnoreLine(lastPos))
      pos = lastPos;
   
   stkInStream.clear();
   stkInStream.seekg(pos);
   
   std::string line;
   std::getline(stkInStream, line);
   return line;
}

//------------------------------------------------------------------------------
// std::istream& IgnoreLine(std::ifstream::pos_type& pos)
//------------------------------------------------------------------------------
std::istream& STKEphemerisFile::IgnoreLine(std::ifstream::pos_type& pos)
{
   pos = stkInStream.tellg();
   return stkInStream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
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
      
      WriteTimePosVel(epoch, stateArray[i]);
      
      // lastEpochWrote = epoch;
      // numberOfEphemPoints++;
   }
}

//------------------------------------------------------------------------------
// void WriteTimePosVel(Real epoch, const Rvector6 *state)
//------------------------------------------------------------------------------
void STKEphemerisFile::WriteTimePosVel(Real epoch, const Rvector6 *state)
{
   // Format output using scientific notation
   const Real *outState = state->GetDataVector();
   Real timeIntervalInSecs = (epoch - scenarioEpochA1Mjd) * 86400.0;
   char strBuff[200];
   sprintf(strBuff, "%1.15e  % 1.15e  % 1.15e  % 1.15e  % 1.15e  % 1.15e  % 1.15e\n",
           timeIntervalInSecs, outState[0], outState[1], outState[2], outState[3],
           outState[4], outState[5]);
   stkOutStream << strBuff;
   
   lastEpochWrote = epoch;
   numberOfEphemPoints++;
   
   #ifdef DEBUG_WRITE_POSVEL
   std::string epochStr = A1ModJulianToUtcGregorian(epochArray[i], 2);
   MessageInterface::ShowMessage("   %s  % 1.15e\n", epochStr, outState[0]);
   #endif
}

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
      
      WriteTimePos(epoch, stateArray[i]);
      
      lastEpochWrote = epoch;
      numberOfEphemPoints++;
   }
}

//------------------------------------------------------------------------------
// void WriteTimePos(Real epoch, const Rvector6 *state)
//------------------------------------------------------------------------------
void STKEphemerisFile::WriteTimePos(Real epoch, const Rvector6 *state)
{
   // Format output using scientific notation
   const Real *outState = state->GetDataVector();
   Real timeIntervalInSecs = (epoch - scenarioEpochA1Mjd) * 86400.0;
   char strBuff[200];
   sprintf(strBuff, "%1.15e  % 1.15e  % 1.15e  % 1.15e\n",
           timeIntervalInSecs, outState[0], outState[1], outState[2]);
   stkOutStream << strBuff;
   
   #ifdef DEBUG_WRITE_POSVEL
   std::string epochStr = A1ModJulianToUtcGregorian(epochArray[i], 2);
   MessageInterface::ShowMessage("   %s  % 1.15e\n", epochStr, outState[0]);
   #endif
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
       "numEphemPointsBegPos=%ld\n   stkTempFileName='%s'", numberOfEphemPoints,
       (long)numEphemPointsBegPos, stkTempFileName.c_str());
   #endif
   
   // Write end ephemeris keyword
   stkOutStream << "END Ephemeris" << std::endl << std::endl;
   stkOutStream.flush();
   
   // Close temp file and copy content to actual STK ephemeris file
   // after writing header data
   stkOutStream.close();   
   
   if (OpenForRead(stkTempFileName, "TimePosVel"))
   {
      openForTempOutput = false;
      OpenForWrite(stkFileNameForWrite, "TimePosVel");
      WriteHeader();   
      
      // Read lines
      std::string line;
      while (!stkInStream.eof())
      {
         // Use cross-platform GetLine
         //GmatFileUtil::GetLine(&stkInStream, line);
         getline(stkInStream, line);
         stkOutStream << line << std::endl;
      }
      
      stkInStream.close();
      stkOutStream.close();
      
      // Delete temp file
      remove(stkTempFileName.c_str());
   }
   else
   {
      MessageInterface::ShowMessage("Failed to open temp file\n");
   }
         
   #ifdef DEBUG_FINALIZE
   MessageInterface::ShowMessage("There are %d segments\n", beginSegmentArray);
   for (unsigned int i = 0; i < beginSegmentArray.size(); i++)
      MessageInterface::ShowMessage("   %.12e\n", beginSegmentArray[i]);
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
   std::string fromType = "A1ModJulian";
   std::string toType = "UTCGregorian";
   
   // Convert current epoch to specified format
   TimeConverterUtil::Convert(fromType, epochInDays, "", toType, toMjd, epochStr, format);
   
   if (epochStr == "")
   {
      MessageInterface::ShowMessage
         ("**** ERROR **** EphemerisWriter::ToUtcGregorian() Cannot convert epoch %.10f %s "
          "to UTCGregorian\n", epochInDays, "days");
      
      epochStr = "EpochError";
   }
   
   return epochStr;
}

//------------------------------------------------------------------------------
// bool UtcGregorianToA1ModJulian(const std::string &utcGreg, Real &a1Mjd)
//------------------------------------------------------------------------------
/**
 * Converts epoch in UTCGregorian to A1ModJulian format. It uses UTCGregorian
 * format of "01 Jan 2000 11:59:28.000."
 *
 * @param  utcGreg  Epoch in UTCGregorian
 * @param  a1Mjd    output epoch in A1ModJulian
 */
//------------------------------------------------------------------------------
bool STKEphemerisFile::UTCGregorianToA1ModJulian(const std::string &utcGreg, Real &a1Mjd)
{
   bool retval = true;
   Real fromMjd = -999.999;
   Real toMjd = -999.999;
   std::string epochStr;
   std::string fromType = "UTCGregorian";
   std::string toType = "A1ModJulian";
   
   // Convert current epoch to specified format
   TimeConverterUtil::Convert(fromType, fromMjd, utcGreg, toType, toMjd, epochStr, 1);
   
   if (epochStr == "")
   {
      MessageInterface::ShowMessage
         ("**** ERROR **** EphemerisWriter::ToUtcGregorian() Cannot convert epoch %s "
          "to A1ModJulian\n", utcGreg.c_str());
      retval = false;
   }
   
   a1Mjd = toMjd;
   return retval;
}
