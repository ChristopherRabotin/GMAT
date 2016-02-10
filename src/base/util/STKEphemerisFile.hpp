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
// Created: 2016.01.29
//
/**
 * Writes a spacecraft orbit ephemeris to a file in STK format.
 */
//------------------------------------------------------------------------------
#ifndef STKEphemerisFile_hpp
#define STKEphemerisFile_hpp

#include "gmatdefs.hpp"
#include <fstream>

class GMAT_API STKEphemerisFile
{
public:
   /// class methods
   STKEphemerisFile();
   STKEphemerisFile(const STKEphemerisFile &copy);
   STKEphemerisFile& operator=(const STKEphemerisFile &copy);
   virtual ~STKEphemerisFile();
   
   void InitializeData(); // Move to protected?
   
   /// Open the ephemeris file for reading/writing
   bool OpenForRead(const std::string &filename, const std::string &ephemType);
   bool OpenForWrite(const std::string &filename, const std::string &ephemType);
   void CloseForRead();
   void CloseForWrite();
   
   void SetVersion(const std::string &version);
   bool SetHeaderForWriting(const std::string &fieldName,
                            const std::string &value);
   
   bool WriteHeader();
   bool WriteBlankLine();
   bool WriteString(const std::string &str);
   bool WriteDataSegment(const EpochArray &epochArray, const StateArray &stateArray, 
                         bool canFinalize = false);
   void FinalizeEphemeris();
   
protected:

   bool        firstTimeWriting;
   
   Real        scenarioEpochA1Mjd;
   Real        coordinateSystemEpochA1Mjd;
   Real        beginSegmentTimeA1Mjd;
   Real        endSegmentTimeA1Mjd;
   Real        lastEpochWrote;
   
   Integer     numberOfEphemPoints;
   
   // Header fields
   std::string stkVersion;           // Required
   std::string dummyNumberOfEphemPoints;
   std::string scenarioEpochUtcGreg; // Required
   std::string centralBody;
   std::string coordinateSystem;
   std::string coordinateSystemEpochStr;
   std::string beginSegmentTimeStr;
   std::string endSegmentTimeStr;
   
   // The file name for read/write
   std::string stkFileNameForRead;
   std::string stkFileNameForWrite;
   
   // Ephemeris type for read/write
   std::string ephemTypeForRead;
   std::string ephemTypeForWrite;
   
   // File position for updating number of ephem points
   std::ofstream::pos_type numEphemPointsBegPos;
   
    // File input/output streams
   std::ifstream stkInStream;
   std::ofstream stkOutStream;
   
  // Epoch and state buffer for read
   RealArray     a1MjdArray;
   StateArray    stateArray;
   
   // For ephemeris writing
   void WriteTimePosVel(const EpochArray &epochArray, const StateArray &stateArray);
   void WriteTimePos(const EpochArray &epochArray, const StateArray &stateArray);
   
   // Time conversion
   std::string A1ModJulianToUtcGregorian(Real epochInDays, Integer format);
};

#endif // STKEphemerisFile_hpp
