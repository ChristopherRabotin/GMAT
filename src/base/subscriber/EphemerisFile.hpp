//$Id$
//------------------------------------------------------------------------------
//                                  EphemerisFile
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
// Created: 2009/09/02
//
/**
 * Writes a spacecraft orbit states or attitude to an ephemeris file either
 * CCSDS or SPK format.
 */
//------------------------------------------------------------------------------
#ifndef EphemerisFile_hpp
#define EphemerisFile_hpp

#include "Subscriber.hpp"
#include "Spacecraft.hpp"
#include "CoordinateSystem.hpp"
#include "CoordinateConverter.hpp"
#include "Interpolator.hpp"
#include <iostream>
#include <fstream>

class SpiceOrbitKernelWriter;
class Code500EphemerisFile;

class GMAT_API EphemerisFile : public Subscriber
{
public:
   EphemerisFile(const std::string &name, const std::string &type = "EphemerisFile");
   virtual ~EphemerisFile();
   EphemerisFile(const EphemerisFile &);
   EphemerisFile& operator=(const EphemerisFile&);
   
   // methods for this class
   virtual std::string  GetProperFileName(const std::string &fName,
                                          const std::string &fType,
                                          bool setFileName);
   virtual void         SetProperFileExtension();
   virtual void         ValidateParameters(bool forInitialization);
   virtual void         SetBackgroundGeneration(bool inBackground);
   
   // Need to be able to close background SPKs and leave ready for appending
   // Finalization
   void                 CloseEphemerisFile(bool done = true, bool writeMetaData = true);
   bool                 InsufficientSPKData();

   // methods inherited from Subscriber
   virtual void         SetProvider(GmatBase *provider, Real epochInMjd = -999.999);
   
   // methods inherited from GmatBase
   virtual bool         Validate();
   virtual bool         Initialize();
   virtual GmatBase*    Clone(void) const;
   virtual void         Copy(const GmatBase* orig);
   
   virtual bool         TakeAction(const std::string &action,
                                   const std::string &actionData = "");
   
   virtual bool         RenameRefObject(const Gmat::ObjectType type,
                                        const std::string &oldName,
                                        const std::string &newName);
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;
   
   virtual bool         IsParameterReadOnly(const Integer id) const;
   virtual bool         IsParameterCommandModeSettable(const Integer id) const;
   
   virtual Gmat::ObjectType
                        GetPropertyObjectType(const Integer id) const;
   virtual const StringArray&
                        GetPropertyEnumStrings(const Integer id) const;
   
   virtual bool         GetBooleanParameter(const Integer id) const;
   virtual bool         SetBooleanParameter(const Integer id,
                                            const bool value);

   virtual Integer      GetIntegerParameter(const Integer id) const;
   virtual Integer      SetIntegerParameter(const Integer id,
                                            const Integer value);
   
   virtual std::string  GetStringParameter(const Integer id) const;
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value);
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value);
   
   virtual GmatBase*    GetRefObject(const Gmat::ObjectType type,
                                     const std::string &name);
   virtual bool         SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                     const std::string &name = "");
   
   virtual const StringArray&
                        GetRefObjectNameArray(const Gmat::ObjectType type);
   
   
protected:

   enum FileType
   {
      CCSDS_OEM, CCSDS_AEM, SPK_ORBIT, SPK_ATTITUDE, CODE500_EPHEM,
   };
   
   // Maximum segment size used for bufferring data
   UnsignedInt            maxSegmentSize;
   
   Spacecraft             *spacecraft;
   CoordinateSystem       *outCoordSystem;
   Interpolator           *interpolator;     // owned object
   SpiceOrbitKernelWriter *spkWriter;        // owned object
   Code500EphemerisFile   *code500EphemFile; // owned object
   
   // for buffering ephemeris data
   EpochArray  a1MjdArray;
   StateArray  stateArray;
   
   /// ephemeris output path from the startup file
   std::string outputPath;
   /// ephemeris full file name including the path
   std::string fullPathFileName;
   std::string spacecraftName;
   std::string spacecraftId;
   std::string prevFileName;
   std::string fileName;
   std::string fileFormat;
   std::string epochFormat;
   std::string ccsdsEpochFormat;
   std::string initialEpoch;
   std::string finalEpoch;
   std::string stepSize;
   std::string interpolatorName;
   std::string stateType;
   std::string outCoordSystemName;
   std::string outputFormat;
   bool writeEphemeris;
   bool usingDefaultFileName;
   bool generateInBackground;
   /// for propagator change
   std::string prevPropName;
   std::string currPropName;
   /// for comments
   std::string currComments;
   
   /// for meta data and block
   Real        metaDataStart;
   Real        metaDataStop;
   std::string metaDataStartStr;
   std::string metaDataStopStr;
   Integer     writeMetaDataOption;
   std::ofstream::pos_type metaDataBegPosition;
   std::ofstream::pos_type metaDataEndPosition;
   
   Integer     interpolationOrder;
   Integer     interpolatorStatus;
   Integer     initialCount;
   Integer     waitCount;
   Integer     afterFinalEpochCount;
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
   RealArray   epochsOnWaiting;
   
   bool        firstTimeWriting;
   bool        firstTimeMetaData;
   bool        saveMetaDataStart;
   bool        writingNewSegment;
   bool        continuousSegment;
   bool        useFixedStepSize;
   bool        interpolateInitialState;
   bool        interpolateFinalState;
   bool        createInterpolator;
   bool        writeOrbit;
   bool        writeAttitude;
   bool        finalEpochReached;
   bool        handleFinalEpoch;
   bool        finalEpochProcessed;
   bool        writeDataInDataCS;
   bool        processingLargeStep;
   bool        spkWriteFailed;
   bool        code500WriteFailed;
   bool        writeCommentAfterData;
   bool        checkForLargeTimeGap;
   bool        isEphemFileOpened;
   
   CoordinateConverter coordConverter;
   
   /// number of SPK segments that have been written
   Integer     numSPKSegmentsWritten;
   /// Indicates whether or not there was data 'left over' that was not enough
   /// to write to the background SPK
   bool        insufficientSPKData;

   FileType    fileType;
   
   /// for maneuver handling
   ObjectArray maneuversHandled;
   
   /// output data stream
   std::ofstream      dstream;
   
   /// Available file format list
   static StringArray fileFormatList;   
   /// Available epoch format list
   static StringArray epochFormatList;   
   /// Available initial epoch list
   static StringArray initialEpochList;
   /// Available final epoch list
   static StringArray finalEpochList;   
   /// Available step size list
   static StringArray stepSizeList;   
   /// Available state type list
   static StringArray stateTypeList;   
   /// Available write ephemeris list
   static StringArray writeEphemerisList;
   /// Available interpolator type list
   static StringArray interpolatorTypeList;
   /// Avilable output format list
   static StringArray outputFormatList;
      
   // Initialization
   void         InitializeData(bool saveEpochInfo = false);
   void         CreateInterpolator();
   void         CreateEphemerisFile();
   void         CreateSpiceKernelWriter();
   void         CreateCode500EphemerisFile();
   bool         OpenTextEphemerisFile();
   
//   // Finalization
//   void         CloseEphemerisFile();
   
   // Time and data
   Real         ConvertInitialAndFinalEpoch();
   bool         CheckInitialAndFinalEpoch();
   void         HandleSpkOrbitData(bool writeData, bool timeToWrite);
   void         HandleCcsdsOrbitData(bool writeData, bool timeToWrite);
   void         HandleCode500OrbitData(bool writeDatda, bool timeToWrite);
   void         HandleWriteOrbit();
   
   // Interpolation
   void         RestartInterpolation(const std::string &comments = "",
                                     bool saveEpochInfo = false,
                                     bool writeAfterData = true,
                                     bool canFinalize = false,
                                     bool ignoreBlankComments = true);
   bool         IsTimeToWrite(Real epochInSecs, const Real state[6]);
   void         WriteOrbit(Real reqEpochInSecs, const Real state[6]);
   void         WriteOrbitAt(Real reqEpochInSecs, const Real state[6]);
   void         GetAttitude();
   void         WriteAttitude();
   void         FinishUpWriting(bool canFinalize = true);
   void         FinishUpWritingCCSDS(bool canFinalize = true);
   void         FinishUpWritingSPK(bool canFinalize = true);
   void         FinishUpWritingCode500(bool canFinalize = true);
   
   void         ProcessFinalDataOnWaiting(bool canFinalize = true);
   void         ProcessEpochsOnWaiting(bool checkFinalEpoch = false,
                                       bool checkEventEpoch = false);
   bool         SetEpoch(Integer id, const std::string &value,
                         const StringArray &allowedValues);
   bool         SetStepSize(Integer id, const std::string &value,
                            const StringArray &allowedValues);
   void         HandleError(Integer id, const std::string &value,
                            const StringArray &allowedValues,
                            const std::string &additionalMsg = "");
   std::string  ToString(const StringArray &strList);
   
   // General writing
   void         WriteString(const std::string &str);
   void         WriteHeader();
   void         WriteMetaData();
   void         WriteComments(const std::string &comments,
                              bool ignoreBlank = true, bool writeKeyword = true);
   
   // General data buffering
   void         BufferOrbitData(Real epochInDays, const Real state[6]);
   void         ClearOrbitData();
   
   // CCSDS file writing for debug and actual
   bool         OpenCcsdsEphemerisFile();
   void         ClearLastCcsdsOemMetaData(const std::string &comments = "");
   void         WriteCcsdsHeader();
   void         WriteCcsdsOrbitDataSegment();
   void         WriteCcsdsOemMetaData();
   void         WriteOrbitData(Real reqEpochInSecs, const Real state[6]);
   void         WriteCcsdsAemMetaData();
   void         WriteCcsdsAemData(Real reqEpochInSecs, const Real quat[4]);
   void         WriteCcsdsComments(const std::string &comments, bool writeKeyword = true);
   
   // CCSDS file actual writing (subclass should overwrite this methods)
   virtual bool OpenRealCcsdsEphemerisFile();
   virtual void WriteRealCcsdsHeader();
   virtual void WriteRealCcsdsOrbitDataSegment();
   virtual void WriteRealCcsdsOemMetaData();
   virtual void WriteRealCcsdsAemMetaData();
   virtual void WriteRealCcsdsAemData(Real reqEpochInSecs, const Real quat[4]);
   virtual void WriteRealCcsdsComments(const std::string &comments);
   
   // SPK file writing
   void         WriteSpkHeader(); // This is for debug
   void         WriteSpkOrbitDataSegment();
   void         WriteSpkOrbitMetaData();
   void         WriteSpkComments(const std::string &comments);
   void         FinalizeSpkFile(bool done = true, bool writeMetaData = true);
   
   // Code500 file writing
   void         WriteCode500OrbitDataSegment(bool canFinalize = false);
   void         FinalizeCode500Ephemeris();
   
   // Epoch handling
   RealArray::iterator
                FindEpochOnWaiting(Real epochInSecs, const std::string &msg);
   void         RemoveEpochAlreadyWritten(Real epochInSecs, const std::string &msg);
   void         AddNextEpochToWrite(Real epochInSecs, const std::string &msg);
   
   // Event checking
   bool         IsEventFeasible(bool checkForNoData = true);
   
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
   void         DebugWriteEpochsOnWaiting(const std::string &msg = "");
   
   // Deprecated field
   void         WriteDeprecatedMessage(Integer id) const;
   
   // methods inherited from Subscriber
   virtual bool Distribute(Integer len);
   virtual bool Distribute(const Real * dat, Integer len);
   virtual void HandleManeuvering(GmatBase *originator, bool maneuvering, Real epoch,
                                  const StringArray &satNames,
                                  const std::string &desc);
   virtual void HandlePropagatorChange(GmatBase *provider, Real epochInMjd);
   virtual void HandleSpacecraftPropertyChange(GmatBase *originator, Real epoch,
                                               const std::string &satName,
                                               const std::string &desc);
   enum
   {
      SPACECRAFT = SubscriberParamCount,
      FILENAME,
      FULLPATH_FILENAME,
      FILE_FORMAT,
      EPOCH_FORMAT,
      INITIAL_EPOCH,
      FINAL_EPOCH,
      STEP_SIZE,
      INTERPOLATOR,
      INTERPOLATION_ORDER,
      STATE_TYPE,
      COORDINATE_SYSTEM,
      OUTPUT_FORMAT,
      WRITE_EPHEMERIS,
      FILE_NAME,                // deprecated
      EphemerisFileParamCount   // Count of the parameters for this class
   };
   
   static const std::string
      PARAMETER_TEXT[EphemerisFileParamCount - SubscriberParamCount];
   static const Gmat::ParameterType
      PARAMETER_TYPE[EphemerisFileParamCount - SubscriberParamCount];
   
};

#endif // EphemerisFile_hpp
