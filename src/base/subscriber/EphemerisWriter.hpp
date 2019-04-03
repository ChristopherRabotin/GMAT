//$Id$
//------------------------------------------------------------------------------
//                                  EphemerisWriter
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2018 United States Government as represented by the
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
// Created: 2015.11.17
//
/**
 * Base class for writing spacecraft orbit states to an ephemeris file.
 */
//------------------------------------------------------------------------------
#ifndef EphemerisWriter_hpp
#define EphemerisWriter_hpp

#include "Spacecraft.hpp"
#include "CoordinateSystem.hpp"
#include "CoordinateConverter.hpp"
#include <iostream>
#include <fstream>

class GMAT_API EphemerisWriter
{
public:
   EphemerisWriter(const std::string &name, const std::string &type = "EphemerisWriter");
   virtual ~EphemerisWriter();
   EphemerisWriter(const EphemerisWriter &);
   EphemerisWriter& operator=(const EphemerisWriter&);
   
   // Methods for this class
   void  SetFileName(const std::string &currFileName,
                     const std::string &currFullName,
                     const std::string &prevFullName);
   void  SetSpacecraft(Spacecraft *sc);
   void  SetDataCoordSystem(CoordinateSystem *dataCS);
   void  SetOutCoordSystem(CoordinateSystem *outCS);
   void  SetInitialData(const std::string &iniEpoch, const std::string &finEpoch,
                        const std::string &stpSize, Real stepInSecs,
                        bool useFixedStep, const std::string &interpName,
                        Integer interpOrder);
   void  SetInitialTime(Real iniEpochA1Mjd, Real finEpochA1Mjd);
   void  SetIsEphemGlobal(bool isGlobal);
   void  SetIsEphemLocal(bool isLocal);
   void  SetBackgroundGeneration(bool inBackground);
   void  SetRunFlags(bool finalize, bool endOfRun, bool isFinalized);
   void  SetOrbitData(Real epochInDays, Real state[6]);
   void  SetEpochAndDirection(Real prvEpochInSecs, Real curEpochInSecs,
                              Real prvPropDir, Real curPropDir);
   void  SetPropIndicator(Integer propInd);
   void  SetEventEpoch(Real epochInSecs);
   void  SetWriteNewSegment(bool newSeg);
   bool  SetStepSize(Real stepSzInSecs);
   
   bool  SkipFunctionData();
   void  HandleEndOfRun();
   // bool  HandlePropDirectionChange();
   
   bool  IsEphemFileOpened();
   bool  IsFirstTimeWriting();
   bool  IsDataEmpty();
   bool  HasFinalEpochProcessed();
   bool  IsFinalized();
   
   // Methods can be overridden by subclasses
   virtual void  CreateEphemerisFile(bool useDefaultFileName,
                                     const std::string &stType,
                                     const std::string &outFormat);
   virtual void  InitializeData(bool saveEpochInfo = false);
   virtual bool  IsBackwardPropAllowed(Real propDirection);
   
   //------------------------------------------------------------------------------
   // virtual void HandleOrbitData()
   //------------------------------------------------------------------------------
   /** Handles writing orbit data includes checking epoch to write if writing at
    * fixed step size.
    */
   virtual void  HandleOrbitData() = 0;
   
   //------------------------------------------------------------------------------
   // virtual void StartNewSegment(const std::string &comments,
   //                              bool saveEpochInfo,
   //                              bool writeAfterData,
   //                              bool ignoreBlankComments)
   //------------------------------------------------------------------------------
   /**
    * Finishes writing remaining data and resets flags to start new segment.
    */
   virtual void  StartNewSegment(const std::string &comments,
                                 bool saveEpochInfo,
                                 bool writeAfterData,
                                 bool ignoreBlankComments) = 0;
   
   //------------------------------------------------------------------------------
   // virtual void FinishUpWriting()
   //------------------------------------------------------------------------------
   /**
    * Finishes up writing remaiing data and do appropriate action.
    */
   virtual void  FinishUpWriting() = 0;

   
   // Need to be able to close background SPKs and leave ready for appending
   // Finalization
   virtual void  CloseEphemerisFile(bool done = true, bool writeMetaData = true);
   virtual bool  InsufficientDataPoints();
   
   // virtual bool  Validate();
   virtual bool  Initialize();
   
   virtual EphemerisWriter* Clone(void) const = 0;
   virtual void  Copy(const EphemerisWriter* orig);
   
protected:

   //@note Should match FileType in EphemerisFile.hpp for now
   enum FileType
   {
      CCSDS_OEM, CCSDS_AEM, SPK_ORBIT, SPK_ATTITUDE, CODE500_EPHEM,
      STK_TIMEPOSVEL, UNKNOWN_FILE_TYPE
   };
   
   std::string ephemName;
   std::string ephemType;
   
   FileType    fileType;

   // Maximum segment size used for bufferring data
   UnsignedInt maxSegmentSize;
   
   Spacecraft       *spacecraft;
   CoordinateSystem *dataCoordSystem;
   CoordinateSystem *outCoordSystem;
   
   // for buffering ephemeris data
   EpochArray  a1MjdArray;
   StateArray  stateArray;
   
   std::string spacecraftName;
   std::string spacecraftId;
   std::string prevFileName;
   std::string fileName;
   std::string fullPathFileName;
   std::string interpolatorName;
   std::string outCoordSystemName;
   std::string outputFormat;
   std::string stateType;
   std::string epochFormat;
   std::string initialEpochStr;
   std::string finalEpochStr;
   std::string stepSize;
   std::string currComments;

   Integer     interpolationOrder;
   Integer     interpolatorStatus;
   Integer     toggleStatus;
   Integer     propIndicator;
   Real        prevPropDirection;
   Real        currPropDirection;
   Real        stepSizeInA1Mjd;
   Real        stepSizeInSecs;
   Real        initialEpochA1Mjd;
   Real        finalEpochA1Mjd;
   Real        blockBeginA1Mjd;
   Real        nextOutEpochInSecs;
   Real        nextReqEpochInSecs;
   Real        currEpochInDays;
   Real        currEpochInSecs;
   Real        prevEpochInSecs;
   Real        prevProcTime;
   Real        lastEpochWrote;
   Real        attEpoch;
   Real        maneuverEpochInDays;
   Real        eventEpochInSecs;
   Real        currState[6];
   Real        attQuat[4];

   bool        canFinalize;
   bool        isEndOfRun;
   bool        isInitialized;
   bool        isFinalized;
   bool        isEphemGlobal;
   bool        isEphemLocal;
   bool        usingDefaultFileName;
   bool        generateInBackground;
   bool        isEphemFileOpened;
   bool        firstTimeWriting;
   bool        writingNewSegment;
   bool        useFixedStepSize;
   bool        interpolateInitialState;
   bool        interpolateFinalState;
   bool        createInterpolator;
   bool        writeOrbit;
   bool        writeAttitude;
   bool        finalEpochReached;
   bool        finalEpochProcessed;
   bool        writeDataInDataCS;
   bool        writeCommentAfterData;
   
   /// Indicates whether or not there was data 'left over' that was not enough
   /// to write to ephemeris file (It is currently used by the background SPK)
   bool        insufficientDataPoints;
   
   CoordinateConverter coordConverter;
   
   /// for maneuver handling
   ObjectArray maneuversHandled;
   
   /// output data stream
   std::ofstream dstream;
   
   //------------------------------------------------------------------------------
   // virtual void BufferOrbitData(Real epochInDays, const Real state[6])
   //------------------------------------------------------------------------------
   /** Handles buffering orbit data.
    */
   virtual void BufferOrbitData(Real epochInDays, const Real state[6]) = 0;
   
   // Initialization
   bool         OpenTextEphemerisFile(const std::string &fn);
   
   // Time and data
   bool         CheckInitialAndFinalEpoch();
   
   virtual void HandleWriteOrbit();
   virtual void WriteOrbit(Real reqEpochInSecs, const Real state[6]);
   virtual void WriteOrbitAt(Real reqEpochInSecs, const Real state[6]);
   
   virtual void HandleWriteAttitude();
   virtual void GetAttitude();
   virtual void WriteAttitudeToFile();
   
   std::string  ToString(const StringArray &strList);
   std::string  GetBackwardPropWarning();
   
   // General writing
   virtual void WriteHeader();
   virtual void WriteMetaData();
   virtual void WriteDataComments(const std::string &comments, bool isErrorMsg,
                              bool ignoreBlank = true, bool writeKeyword = true);
   void         WriteStringToFile(const std::string &str);
   
   // General data handling
   void         ClearOrbitData();
   virtual void FindNextOutputEpoch(Real reqEpochInSecs, Real &outEpochInSecs,
                                    Real stateToWrite[6]);
   void         WriteOrbitData(Real reqEpochInSecs, const Real state[6]);
   
   // CoordinateSystem conversion
   void         ConvertState(Real epochInDays, const Real inState[6],
                             Real outState[6]);
   
   // Time formatting
   std::string  ToUtcGregorian(Real epoch, bool inDays = false, Integer format = 2);
   
   // Error message formatting
   void FormatErrorMessage(std::string &ephemMsg, std::string &errMsg);
   
   // Debug output
   void         DebugWriteTime(const std::string &msg, Real epoch, bool inDays = false,
                               Integer format = 2);
   void         DebugWriteOrbit(const std::string &msg, Real epoch, const Real state[6],
                                bool inDays = false, bool logOnly = false);
   void         DebugWriteOrbit(const std::string &msg, A1Mjd *epochInDays,
                                Rvector6 *state, bool logOnly = false);
   void         DebugWriteState(const std::string &msg, const Real state[6]);
};

#endif // EphemerisWriter_hpp
