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
 * Writes a spacecraft orbit states or attitude to an ephemeris file in
 * CCSDS, SPK, or Code-500 format.
 */
//------------------------------------------------------------------------------

#include "EphemerisFile.hpp"
#include "CelestialBody.hpp"
#include "Publisher.hpp"             // for Instance()
#include "FileManager.hpp"           // for GetPathname()
#include "SubscriberException.hpp"   // for exception
#include "StringUtil.hpp"            // for ToString()
#include "FileUtil.hpp"              // for ParseFileExtension()
#include "TimeSystemConverter.hpp"   // for ValidateTimeFormat()
#include "LagrangeInterpolator.hpp"  // for LagrangeInterpolator
#include "RealUtilities.hpp"         // for IsEven()
#include "MessageInterface.hpp"
#include "TimeTypes.hpp"
#include "AttitudeConversionUtility.hpp"
#include <sstream>                   // for <<, std::endl

#include "Code500EphemerisFile.hpp"

#ifdef __USE_SPICE__
#include "SpiceOrbitKernelWriter.hpp"
#endif

// Currently we can't use DataFile for 2011a release so commented out
// Actually we want to put this flag in BuildEnv.mk but it is very close to
// release so added it here and Moderator.cpp
//#define __USE_DATAFILE__

//#define DEBUG_EPHEMFILE
//#define DEBUG_EPHEMFILE_ACTION
//#define DEBUG_EPHEMFILE_SET
//#define DEBUG_EPHEMFILE_INIT
//#define DEBUG_EPHEMFILE_OPEN
//#define DEBUG_EPHEMFILE_SPICE
//#define DEBUG_EPHEMFILE_CCSDS
//#define DEBUG_CODE500_CREATE
//#define DEBUG_EPHEMFILE_CODE500
//#define DEBUG_CODE500_OUTPUT
//#define DEBUG_EPHEMFILE_INTERPOLATOR
//#define DEBUG_EPHEMFILE_BUFFER
//#define DEBUG_EPHEMFILE_TIME
//#define DEBUG_EPHEMFILE_ORBIT
//#define DEBUG_EPHEMFILE_WRITE
//#define DEBUG_EPHEMFILE_FINISH
//#define DEBUG_EPHEMFILE_RESTART
//#define DEBUG_EPHEMFILE_EVENTS
//#define DEBUG_EPHEMFILE_COMMENTS
//#define DEBUG_EPHEMFILE_HEADER
//#define DEBUG_EPHEMFILE_METADATA
//#define DEBUG_EPHEMFILE_TEXT
//#define DEBUG_EPHEMFILE_SOLVER_DATA
//#define DEBUG_INTERPOLATOR_TRACE
//#define DEBUG_EPHEMFILE_CONVERT_STATE
//#define DEBUG_FILE_PATH
//#define DBGLVL_EPHEMFILE_DATA 2
//#define DBGLVL_EPHEMFILE_DATA_LABELS 1
//#define DBGLVL_EPHEMFILE_MANEUVER 2
//#define DBGLVL_EPHEMFILE_PROPAGATOR_CHANGE 2
//#define DBGLVL_EPHEMFILE_SC_PROPERTY_CHANGE 2
//#define DBGLVL_EPHEMFILE_REF_OBJ 1

//#ifndef DEBUG_MEMORY
//#define DEBUG_MEMORY
//#endif

#ifdef DEBUG_MEMORY
#include "MemoryTracker.hpp"
#endif

//---------------------------------
// static data
//---------------------------------
StringArray EphemerisFile::fileFormatList;
StringArray EphemerisFile::epochFormatList;
StringArray EphemerisFile::initialEpochList;
StringArray EphemerisFile::finalEpochList;
StringArray EphemerisFile::stepSizeList;
StringArray EphemerisFile::stateTypeList;
StringArray EphemerisFile::writeEphemerisList;
StringArray EphemerisFile::interpolatorTypeList;
StringArray EphemerisFile::outputFormatList;

const std::string
EphemerisFile::PARAMETER_TEXT[EphemerisFileParamCount - SubscriberParamCount] =
{
   "Spacecraft",            // SPACECRAFT
   "Filename",              // FILENAME
   "FullPathFileName",      // FULLPATH_FILENAME
   "FileFormat",            // FILE_FORMAT
   "EpochFormat",           // EPOCH_FORMAT
   "InitialEpoch",          // INITIAL_EPOCH
   "FinalEpoch",            // FINAL_EPOCH
   "StepSize",              // STEP_SIZE
   "Interpolator",          // INTERPOLATOR
   "InterpolationOrder",    // INTERPOLATION_ORDER
   "StateType",             // STATE_TYPE
   "CoordinateSystem",      // COORDINATE_SYSTEM
   "OutputFormat",          // OUTPUT_FORMAT
   "WriteEphemeris",        // WRITE_EPHEMERIS
   "FileName",              // FILE_NAME - deprecated
};

const Gmat::ParameterType
EphemerisFile::PARAMETER_TYPE[EphemerisFileParamCount - SubscriberParamCount] =
{
   Gmat::OBJECT_TYPE,       // SPACECRAFT
   Gmat::FILENAME_TYPE,     // FILENAME
   Gmat::FILENAME_TYPE,     // FULLPATH_FILENAME
   Gmat::ENUMERATION_TYPE,  // FILE_FORMAT
   Gmat::ENUMERATION_TYPE,  // EPOCH_FORMAT
   Gmat::ENUMERATION_TYPE,  // INITIAL_EPOCH
   Gmat::ENUMERATION_TYPE,  // FINAL_EPOCH
   Gmat::ENUMERATION_TYPE,  // STEP_SIZE
   Gmat::OBJECT_TYPE,       // INTERPOLATOR
   Gmat::INTEGER_TYPE,      // INTERPOLATION_ORDER
   Gmat::ENUMERATION_TYPE,  // STATE_TYPE
   Gmat::OBJECT_TYPE,       // COORDINATE_SYSTEM
   Gmat::ENUMERATION_TYPE,  // OUTPUT_FORMAT
   Gmat::BOOLEAN_TYPE,      // WRITE_EPHEMERIS
   Gmat::STRING_TYPE,       // FILE_NAME - deprecated
};


//------------------------------------------------------------------------------
// EphemerisFile(const std::string &name, const std::string &type = "EphemerisFile")
//------------------------------------------------------------------------------
/**
 * Default constructor
 */
//------------------------------------------------------------------------------
EphemerisFile::EphemerisFile(const std::string &name, const std::string &type) :
   Subscriber           (type, name),
   maxSegmentSize       (1000),
   spacecraft           (NULL),
   outCoordSystem       (NULL),
   interpolator         (NULL),
   spkWriter            (NULL),
   code500EphemFile     (NULL),
   outputPath           (""),
   fullPathFileName     (""),
   spacecraftName       (""),
   spacecraftId         (""),
   prevFileName         (""),
   fileName             (""),
   fileFormat           ("CCSDS-OEM"),
   epochFormat          ("UTCGregorian"),
   ccsdsEpochFormat     ("UTC"),
   initialEpoch         ("InitialSpacecraftEpoch"),
   finalEpoch           ("FinalSpacecraftEpoch"),
   stepSize             ("IntegratorSteps"),
   interpolatorName     ("Lagrange"),
   stateType            ("Cartesian"),
   outCoordSystemName   ("EarthMJ2000Eq"),
   outputFormat         ("PC"),
   writeEphemeris       (true),
   usingDefaultFileName (true),
   generateInBackground (false),
   prevPropName         (""),
   currPropName         (""),
   currComments         (""),
   metaDataStart        (-999.999),
   metaDataStop         (-999.999),
   metaDataStartStr     (""),
   metaDataStopStr      (""),
   writeMetaDataOption  (0),
   metaDataBegPosition  (0),
   metaDataEndPosition  (0),
   interpolationOrder   (7),
   interpolatorStatus   (-1),
   initialCount         (0),
   waitCount            (0),
   afterFinalEpochCount (0),
   toggleStatus         (0),
   propIndicator        (0),
   prevPropDirection    (0.0),
   currPropDirection    (0.0),
   stepSizeInA1Mjd      (-999.999),
   stepSizeInSecs       (-999.999),
   initialEpochA1Mjd    (-999.999),
   finalEpochA1Mjd      (-999.999),
   blockBeginA1Mjd      (-999.999),
   nextOutEpochInSecs   (-999.999),
   nextReqEpochInSecs   (-999.999),
   currEpochInDays      (-999.999),
   currEpochInSecs      (-999.999),
   prevEpochInSecs      (-999.999),
   prevProcTime         (-999.999),
   lastEpochWrote       (-999.999),
   attEpoch             (-999.999),
   maneuverEpochInDays  (-999.999),
   eventEpochInSecs     (-999.999),
   firstTimeWriting     (true),
   firstTimeMetaData    (true),
   saveMetaDataStart    (true),
   writingNewSegment    (true),
   continuousSegment    (false),
   useFixedStepSize     (false),
   interpolateInitialState(false),
   interpolateFinalState(false),
   createInterpolator   (false),
   writeOrbit           (false),
   writeAttitude        (false),
   finalEpochReached    (false),
   handleFinalEpoch     (true),
   finalEpochProcessed  (false),
   writeDataInDataCS    (true),
   processingLargeStep  (false),
   spkWriteFailed       (false),   // true),
   code500WriteFailed   (true),
   writeCommentAfterData (true),
   checkForLargeTimeGap (false),
   isEphemFileOpened    (false),
   numSPKSegmentsWritten (0),
   insufficientSPKData   (false)
{
   #ifdef DEBUG_EPHEMFILE
   MessageInterface::ShowMessage
      ("EphemerisFile::EphemerisFile() <%p>'%s' entered\n", this, GetName().c_str());
   #endif
   
   objectTypes.push_back(Gmat::EPHEMERIS_FILE);
   objectTypeNames.push_back("EphemerisFile");
   objectTypeNames.push_back("FileOutput");
   parameterCount = EphemerisFileParamCount;
   
   // // Should I give non-blank fileName?
   // if (fileName == "")
   //    fileName = name + ".eph";
   
   // Get full path file name (LOJ: 2014.06.24)
   // The new GmatBase static method GetFullPathFileName() now handles empty file name
   #ifdef DEBUG_FILE_PATH
   MessageInterface::ShowMessage
      ("EphemerisFile::EphemerisFile() '%s' calling GmatBase::GetFullPathFileName()\n",
       GetName().c_str());
   #endif
   
   fullPathFileName =
      GmatBase::GetFullPathFileName(fileName, GetName(), fileName, "EPHEM_OUTPUT_FILE", false, ".oem");
   prevFileName = fileName;
   
   // Available enumeration type list, since it is static data, clear it first
   fileFormatList.clear();
   fileFormatList.push_back("CCSDS-OEM");
   // CCSDS-AEM not allowed in 2010 release (bug 2219)
   //fileFormatList.push_back("CCSDS-AEM");
   fileFormatList.push_back("SPK");
   fileFormatList.push_back("Code-500");
   
   epochFormatList.clear();
   epochFormatList.push_back("UTCGregorian");
   epochFormatList.push_back("UTCModJulian");
   epochFormatList.push_back("TAIGregorian");
   epochFormatList.push_back("TAIModJulian");
   epochFormatList.push_back("TTGregorian");
   epochFormatList.push_back("TTModJulian");
   epochFormatList.push_back("A1Gregorian");
   epochFormatList.push_back("A1ModJulian");
   
   initialEpochList.clear();
   initialEpochList.push_back("InitialSpacecraftEpoch");
   
   finalEpochList.clear();
   finalEpochList.push_back("FinalSpacecraftEpoch");
   
   stepSizeList.clear();
   stepSizeList.push_back("IntegratorSteps");

   // Cartesian is the only allowed state type for the 2010 release (bug 2219)
   stateTypeList.clear();
   stateTypeList.push_back("Cartesian");
   //stateTypeList.push_back("Quaternion");
   
   writeEphemerisList.clear();
   writeEphemerisList.push_back("Yes");
   writeEphemerisList.push_back("No");
   
   interpolatorTypeList.clear();
   interpolatorTypeList.push_back("Lagrange");
   interpolatorTypeList.push_back("Hermite");
   
   // SLERP not allowed in 2010 release (Bug 2219)
   //interpolatorTypeList.push_back("SLERP");
   
   outputFormatList.clear();
   outputFormatList.push_back("PC");
   outputFormatList.push_back("UNIX");
   
   #ifdef DEBUG_EPHEMFILE
   MessageInterface::ShowMessage
      ("fileName='%s', fullPathFileName='%s'\n", fileName.c_str(), fullPathFileName.c_str());
   MessageInterface::ShowMessage
      ("EphemerisFile::EphemerisFile() <%p>'%s' leaving\n", this, GetName().c_str());
   #endif
}


//------------------------------------------------------------------------------
// ~EphemerisFile()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
EphemerisFile::~EphemerisFile()
{
   #ifdef DEBUG_EPHEMFILE
   MessageInterface::ShowMessage
      ("EphemerisFile::~EphemerisFile() <%p>'%s' entered\n", this, GetName().c_str());
   #endif
   
   if (interpolator != NULL)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (interpolator, interpolator->GetName(), "EphemerisFile::~EphemerisFile()()",
          "deleting local interpolator");
      #endif
      delete interpolator;
   }
   
   #ifdef __USE_SPICE__
   #ifdef DEBUG_EPHEMFILE_SPICE
   MessageInterface::ShowMessage
      ("   spkWriter=<%p>, spkWriteFailed=%s\n", spkWriter, (spkWriteFailed? "true":"false"));
   #endif
   if (spkWriter != NULL)
   {
      if (!spkWriteFailed)
         FinalizeSpkFile();
      
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (spkWriter, "SPK writer", "EphemerisFile::~EphemerisFile()()",
          "deleting local SPK writer");
      #endif
      delete spkWriter;
   }
   #endif
   
   // Delete Code500 ephemeris
   if (code500EphemFile)
      delete code500EphemFile;
   
   // Close CCSDS ephemeris
   dstream.flush();
   dstream.close();
   
   #ifdef DEBUG_EPHEMFILE
   MessageInterface::ShowMessage
      ("EphemerisFile::~EphemerisFile() <%p>'%s' leaving\n", this, GetName().c_str());
   #endif
}


//------------------------------------------------------------------------------
// EphemerisFile(const EphemerisFile &ef)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 */
//------------------------------------------------------------------------------
EphemerisFile::EphemerisFile(const EphemerisFile &ef) :
   Subscriber           (ef),
   maxSegmentSize       (ef.maxSegmentSize),
   spacecraft           (ef.spacecraft),
   outCoordSystem       (ef.outCoordSystem),
   interpolator         (NULL),
   spkWriter            (NULL),
   code500EphemFile     (NULL),
   outputPath           (ef.outputPath),
   fullPathFileName     (ef.fullPathFileName),
   spacecraftName       (ef.spacecraftName),
   spacecraftId         (ef.spacecraftId),
   prevFileName         (ef.prevFileName),
   fileName             (ef.fileName),
   fileFormat           (ef.fileFormat),
   epochFormat          (ef.epochFormat),
   ccsdsEpochFormat     (ef.ccsdsEpochFormat),
   initialEpoch         (ef.initialEpoch),
   finalEpoch           (ef.finalEpoch),
   stepSize             (ef.stepSize),
   interpolatorName     (ef.interpolatorName),
   stateType            (ef.stateType),
   outCoordSystemName   (ef.outCoordSystemName),
   outputFormat         (ef.outputFormat),
   writeEphemeris       (ef.writeEphemeris),
   usingDefaultFileName (ef.usingDefaultFileName),
   generateInBackground (ef.generateInBackground),
   prevPropName         (ef.prevPropName),
   currPropName         (ef.currPropName),
   currComments         (ef.currComments),
   metaDataStart        (ef.metaDataStart),
   metaDataStop         (ef.metaDataStop),
   metaDataStartStr     (ef.metaDataStartStr),
   metaDataStopStr      (ef.metaDataStopStr),
   writeMetaDataOption  (0),
   metaDataBegPosition  (0),
   metaDataEndPosition  (0),
   interpolationOrder   (ef.interpolationOrder),
   interpolatorStatus   (ef.interpolatorStatus),
   initialCount         (ef.initialCount),
   waitCount            (ef.waitCount),
   afterFinalEpochCount (ef.afterFinalEpochCount),
   toggleStatus         (ef.toggleStatus),
   propIndicator        (ef.propIndicator),
   prevPropDirection    (ef.prevPropDirection),
   currPropDirection    (ef.currPropDirection),
   stepSizeInA1Mjd      (ef.stepSizeInA1Mjd),
   stepSizeInSecs       (ef.stepSizeInSecs),
   initialEpochA1Mjd    (ef.initialEpochA1Mjd),
   finalEpochA1Mjd      (ef.finalEpochA1Mjd),
   blockBeginA1Mjd      (ef.blockBeginA1Mjd),
   nextOutEpochInSecs   (ef.nextOutEpochInSecs),
   nextReqEpochInSecs   (ef.nextReqEpochInSecs),
   currEpochInDays      (ef.currEpochInDays),
   currEpochInSecs      (ef.currEpochInSecs),
   prevEpochInSecs      (ef.prevEpochInSecs),
   prevProcTime         (ef.prevProcTime),
   lastEpochWrote       (ef.lastEpochWrote),
   attEpoch             (ef.attEpoch),
   maneuverEpochInDays  (ef.maneuverEpochInDays),
   eventEpochInSecs     (ef.eventEpochInSecs),
   firstTimeWriting     (ef.firstTimeWriting),
   firstTimeMetaData    (ef.firstTimeMetaData),
   saveMetaDataStart    (ef.saveMetaDataStart),
   writingNewSegment    (ef.writingNewSegment),
   continuousSegment    (ef.continuousSegment),
   useFixedStepSize     (ef.useFixedStepSize),
   interpolateInitialState(ef.interpolateInitialState),
   interpolateFinalState(ef.interpolateFinalState),
   createInterpolator   (ef.createInterpolator),
   writeOrbit           (ef.writeOrbit),
   writeAttitude        (ef.writeAttitude),
   finalEpochReached    (ef.finalEpochReached),
   handleFinalEpoch     (ef.handleFinalEpoch),
   finalEpochProcessed  (ef.finalEpochProcessed),
   writeDataInDataCS    (ef.writeDataInDataCS),
   processingLargeStep  (ef.processingLargeStep),
   spkWriteFailed       (ef.spkWriteFailed),
   code500WriteFailed   (ef.code500WriteFailed),
   writeCommentAfterData (ef.writeCommentAfterData),
   checkForLargeTimeGap (ef.checkForLargeTimeGap),
   isEphemFileOpened    (ef.isEphemFileOpened),
   numSPKSegmentsWritten (ef.numSPKSegmentsWritten),
   insufficientSPKData   (ef.insufficientSPKData)
{
   coordConverter = ef.coordConverter;
}


//------------------------------------------------------------------------------
// EphemerisFile& EphemerisFile::operator=(const EphemerisFile& ef)
//------------------------------------------------------------------------------
/**
 * The assignment operator
 */
//------------------------------------------------------------------------------
EphemerisFile& EphemerisFile::operator=(const EphemerisFile& ef)
{
   if (this == &ef)
      return *this;
   
   Subscriber::operator=(ef);
   
   maxSegmentSize       = ef.maxSegmentSize;
   spacecraft           = ef.spacecraft;
   outCoordSystem       = ef.outCoordSystem;
   interpolator         = NULL;
   spkWriter            = NULL;
   code500EphemFile     = NULL;
   outputPath           = ef.outputPath;
   fullPathFileName     = ef.fullPathFileName;
   spacecraftName       = ef.spacecraftName;
   spacecraftId         = ef.spacecraftId;
   prevFileName         = ef.prevFileName;
   fileName             = ef.fileName;
   fileFormat           = ef.fileFormat;
   epochFormat          = ef.epochFormat;
   ccsdsEpochFormat     = ef.ccsdsEpochFormat;
   initialEpoch         = ef.initialEpoch;
   finalEpoch           = ef.finalEpoch;
   stepSize             = ef.stepSize;
   interpolatorName     = ef.interpolatorName;
   stateType            = ef.stateType;
   outCoordSystemName   = ef.outCoordSystemName;
   outputFormat         = ef.outputFormat;
   writeEphemeris       = ef.writeEphemeris;
   usingDefaultFileName = ef.usingDefaultFileName;
   generateInBackground = ef.generateInBackground;
   prevPropName         = ef.prevPropName;
   currPropName         = ef.currPropName;
   currComments         = ef.currComments;
   metaDataStart        = ef.metaDataStart;
   metaDataStop         = ef.metaDataStop;
   metaDataStartStr     = ef.metaDataStartStr;
   metaDataStopStr      = ef.metaDataStopStr;
   writeMetaDataOption  = 0;
   metaDataBegPosition  = 0;
   metaDataEndPosition  = 0;
   interpolationOrder   = ef.interpolationOrder;
   interpolatorStatus   = ef.interpolatorStatus;
   initialCount         = ef.initialCount;
   waitCount            = ef.waitCount;
   afterFinalEpochCount = ef.afterFinalEpochCount;
   toggleStatus         = ef.toggleStatus;
   propIndicator        = ef.propIndicator;
   prevPropDirection    = ef.prevPropDirection;
   currPropDirection    = ef.currPropDirection;
   stepSizeInA1Mjd      = ef.stepSizeInA1Mjd;
   stepSizeInSecs       = ef.stepSizeInSecs;
   initialEpochA1Mjd    = ef.initialEpochA1Mjd;
   finalEpochA1Mjd      = ef.finalEpochA1Mjd;
   blockBeginA1Mjd      = ef.blockBeginA1Mjd;
   nextOutEpochInSecs   = ef.nextOutEpochInSecs;
   nextReqEpochInSecs   = ef.nextReqEpochInSecs;
   currEpochInDays      = ef.currEpochInDays;
   currEpochInSecs      = ef.currEpochInSecs;
   prevEpochInSecs      = ef.prevEpochInSecs;
   prevProcTime         = ef.prevProcTime;
   lastEpochWrote       = ef.lastEpochWrote;
   attEpoch             = ef.attEpoch;
   maneuverEpochInDays  = ef.maneuverEpochInDays;
   eventEpochInSecs     = ef.eventEpochInSecs;
   firstTimeWriting     = ef.firstTimeWriting;
   firstTimeMetaData    = ef.firstTimeMetaData;
   saveMetaDataStart    = ef.saveMetaDataStart;
   writingNewSegment    = ef.writingNewSegment;
   continuousSegment    = ef.continuousSegment;
   useFixedStepSize     = ef.useFixedStepSize;
   interpolateInitialState= ef.interpolateInitialState;
   interpolateFinalState= ef.interpolateFinalState;
   createInterpolator   = ef.createInterpolator;
   writeOrbit           = ef.writeOrbit;
   writeAttitude        = ef.writeAttitude;
   finalEpochReached    = ef.finalEpochReached;
   handleFinalEpoch     = ef.handleFinalEpoch;
   finalEpochProcessed  = ef.finalEpochProcessed;
   writeDataInDataCS    = ef.writeDataInDataCS;
   processingLargeStep  = ef.processingLargeStep;
   spkWriteFailed       = ef.spkWriteFailed;
   code500WriteFailed   = ef.code500WriteFailed;
   writeCommentAfterData = ef.writeCommentAfterData;
   checkForLargeTimeGap = ef.checkForLargeTimeGap;
   isEphemFileOpened    = ef.isEphemFileOpened;
   coordConverter       = ef.coordConverter;
   numSPKSegmentsWritten = ef.numSPKSegmentsWritten;
   insufficientSPKData   = ef.insufficientSPKData;
   
   return *this;
}

//---------------------------------
// methods for this class
//---------------------------------

//------------------------------------------------------------------------------
// std::string GetProperFileName(const std::string &fName, const std::string &fType)
//------------------------------------------------------------------------------
std::string EphemerisFile::GetProperFileName(const std::string &fName,
                                             const std::string &fType,
                                             bool setFileName)
{
   #ifdef DEBUG_EPHEMFILE_EXTENSION
   MessageInterface::ShowMessage
      ("EphemerisFile::GetProperFileName() entered, fName='%s', fType='%s', "
       "setFileName=%d\n", fName.c_str(), fType.c_str(), setFileName);
   #endif
   
   std::string expFileName = fName;
   std::string defaultExt = ".eph";
   if (fType == "SPK")
      defaultExt = ".bsp";
   else if (fType == "CCSDS-OEM")
      defaultExt = ".oem";
   else if (fType == "CCSDS-AEM")
      defaultExt = ".aem";
   
   std::string parsedExt = GmatFileUtil::ParseFileExtension(fName, true);
   if (parsedExt != "" && parsedExt != defaultExt)
   {
      // Replace file extension only for SPK ephem type
      if (fType == "SPK")
      {
         std::string ofname = expFileName;
         expFileName = GmatStringUtil::Replace(expFileName, parsedExt, defaultExt);
         if (setFileName)
         {
            MessageInterface::ShowMessage
               ("*** WARNING *** %s file extension should be \"%s\", so "
                "file name '%s' changed to '%s'\n", fType.c_str(), defaultExt.c_str(),
                ofname.c_str(), expFileName.c_str());
            
            // Set filename and full path filename
            fileName = expFileName;
            fullPathFileName = GmatStringUtil::Replace(fullPathFileName, parsedExt, defaultExt);
         }
      }
      else
      {
         // We don't want to change the extension for other format such as Code-500
         // but we want to change it if creating new from the GUI. If setFileName
         // is set to false, this method is called from the GUI.
         if (!setFileName)
         {
            expFileName = GmatStringUtil::Replace(expFileName, parsedExt, defaultExt);
         }
      }
   }
   else if (parsedExt == "")
   {
      std::string ofname = expFileName;
      expFileName = expFileName + defaultExt;
      if (setFileName)
      {
         MessageInterface::ShowMessage
            ("*** WARNING *** %s file extension should be \"%s\", so "
             "file name '%s' changed to '%s'\n", fType.c_str(), defaultExt.c_str(),
             ofname.c_str(), expFileName.c_str());
         
         // Set filename and full path filename
         fileName = expFileName;
         fullPathFileName = fullPathFileName + defaultExt;
      }
   }
   
   #ifdef DEBUG_EPHEMFILE_EXTENSION
   MessageInterface::ShowMessage
      ("EphemerisFile::GetProperFileName() returning '%s'\n", expFileName.c_str());
   #endif
   return expFileName;
}

//------------------------------------------------------------------------------
// void SetProperFileExtension()
//------------------------------------------------------------------------------
void EphemerisFile::SetProperFileExtension()
{
   #ifdef DEBUG_EPHEMFILE_OPEN
   MessageInterface::ShowMessage
      ("EphemerisFile::SetProperFileExtension() fileName='%s;, fileFormat='%s'\n", fileName.c_str(),
       fileFormat.c_str());
   #endif   

   // Pass true for setting filename and full path filename
   std::string properFileName = GetProperFileName(fileName, fileFormat, true);
   
   #ifdef DEBUG_EPHEMFILE_OPEN
   MessageInterface::ShowMessage
      ("EphemerisFile::SetProperFileExtension() leaving, fullPathFileName\n   %s\n",
       fullPathFileName.c_str());
   #endif
}


//------------------------------------------------------------------------------
// void ValidateParameters(bool forInitialization)
//------------------------------------------------------------------------------
void EphemerisFile::ValidateParameters(bool forInitialization)
{
   #ifdef DEBUG_EPHEMFILE_INIT
   MessageInterface::ShowMessage
      ("EphemerisFile::ValidateParameters() <%p>'%s' entered, forInitialization=%d\n",
       this, GetName().c_str(), forInitialization);
   #endif
   
   if (fileFormat == "SPK")
   {
      if (stateType == "Quaternion")
         throw SubscriberException
            ("Currently GMAT only supports writing orbit states in SPK format");
      
      if (interpolatorName == "Hermite" && GmatMathUtil::IsEven(interpolationOrder))
         throw SubscriberException
            ("The SPK file interpolation order must be an odd number when using "
             "Hermite interpolator");
   }
   else
   {
      // check for FileFormat and StateType
      if ((fileFormat == "CCSDS-OEM" && stateType == "Quaternion") ||
          (fileFormat == "CCSDS-AEM" && stateType == "Cartesian") ||
          (fileFormat == "Code-500" && stateType == "Quaternion"))
         throw SubscriberException
            ("FileFormat \"" + fileFormat + "\" and StateType " + "\"" + stateType +
             "\" does not match for the EphemerisFile \"" + GetName() + "\"");
      
      // check interpolator type
      if (stepSize != "IntegratorSteps")
      {
         // check for StateType Cartesian and Interpolator
         if (stateType == "Cartesian" && interpolatorName != "Lagrange")
            throw SubscriberException
               ("The Interpolator must be \"Lagrange\" for StateType of \"Cartesian\" for "
                "the EphemerisFile \"" + GetName() + "\"");
         
         // check for StateType Quaternion and Interpolator
         if (stateType == "Quaternion" && interpolatorName != "SLERP")
            throw SubscriberException
               ("The Interpolator must be \"SLERP\" for StateType of \"Quaternion\" for "
                "the EphemerisFile \"" + GetName() + "\"");
      }
      
      if (fileFormat == "Code-500")
      {
         if (!useFixedStepSize)
            throw SubscriberException
               ("Code-500 ephemeris file \"" + GetName() + "\" requires fixed step size");
         
         // Give default step size for code-500
         if (stepSize == "IntegratorSteps")
         {
            stepSize = "60";
            stepSizeInSecs = 60.0;
         }
      }
   }
   
   // By this time, coordinate system should not be NULL, so check it
   if (outCoordSystem == NULL)
      throw SubscriberException
         ("The CoordinateSystem \"" + outCoordSystemName + "\" has not been set for "
          "the EphemerisFile \"" + GetName() + "\"");
   
   // Do some validation
   if (fileFormat == "SPK" || fileFormat == "Code-500")
   {
      #ifdef DEBUG_EPHEMFILE_INIT
      MessageInterface::ShowMessage
         ("   outCoordSystem=<%p>'%s'\n", outCoordSystem, outCoordSystem->GetName().c_str());
      #endif
      if (!outCoordSystem->AreAxesOfType("MJ2000EqAxes"))
      {
         SubscriberException se;
         se.SetDetails("%s ephemeris file \"%s\" only allows coordinate system "
                       "with MJ2000Eq Axis", fileFormat.c_str(), GetName().c_str());
         throw se;
      }
      
      // Check for valid central body for Code500 ephem
      // 1=Earth, 2=Luna(Earth Moon), 3=Sun, 4=Mars, 5=Jupiter, 6=Saturn, 7=Uranus,
      // 8=Neptune, 9=Pluto, 10=Mercury, 11=Venus
      if (fileFormat == "Code-500")
      {
         std::string origin = outCoordSystem->GetOriginName();
         if (origin != "Earth" && origin != "Luna" && origin != "Sun" &&
             origin != "Mars" && origin != "Jupiter" && origin != "Saturn" &&
             origin != "Uranus" && origin != "Neptune" && origin != "Pluto" &&
             origin != "Mercury" && origin != "Venus")
         {
            SubscriberException se;
            se.SetDetails("%s ephemeris file \"%s\" only allows coordinate system "
                          "with Sun, Planet or Luna(Earth Moon) origin", fileFormat.c_str(),
                          GetName().c_str());
            throw se;
         }
      }
   }
   
   // Catch invalid initial epoch early if spacecraft is not NULL
   if (spacecraft != NULL)
      ConvertInitialAndFinalEpoch();
   
   // If for initialization, all other pointers should have been set, so check it
   if (forInitialization)
   {
      // check for NULL pointers
      if (spacecraft == NULL)
         throw SubscriberException
            ("The Spacecraft \"" + spacecraftName + "\" has not been set for "
             "the EphemerisFile \"" + GetName() + "\"");
      
      if (theDataCoordSystem == NULL)
         throw SubscriberException
            ("The internal CoordinateSystem which orbit data represents has not been set for "
             "the EphemerisFile \"" + GetName() + "\"");
   }
   
   if (forInitialization)
   {
      #ifdef DEBUG_EPHEMFILE_INIT
      MessageInterface::ShowMessage
         ("   spacecraft=<%p>'%s', outCoordSystem=<%p>'%s', theDataCoordSystem=<%p>'%s'\n",
          spacecraft, spacecraft->GetName().c_str(), outCoordSystem,
          outCoordSystem->GetName().c_str(), theDataCoordSystem,
          theDataCoordSystem->GetName().c_str());
      #endif
   }
   
   #ifdef DEBUG_EPHEMFILE_INIT
   MessageInterface::ShowMessage
      ("EphemerisFile::ValidateParameters() <%p>'%s' leaving\n", this, GetName().c_str());
   #endif
}


void EphemerisFile::SetBackgroundGeneration(bool inBackground)
{
   generateInBackground = inBackground;
}


//----------------------------------
// methods inherited from Subscriber
//----------------------------------

//------------------------------------------------------------------------------
// virtual void SetProvider(GmatBase *provider, Real epochInMjd = -999.999)
//------------------------------------------------------------------------------
void EphemerisFile::SetProvider(GmatBase *provider, Real epochInMjd)
{
   Subscriber::SetProvider(provider, epochInMjd);
   HandlePropagatorChange(provider, epochInMjd);
}


//----------------------------------
// methods inherited from GmatBase
//----------------------------------

//------------------------------------------------------------------------------
// virtual bool Validate()
//------------------------------------------------------------------------------
bool EphemerisFile::Validate()
{
   #ifdef DEBUG_EPHEMFILE_INIT
   MessageInterface::ShowMessage
      ("EphemerisFile::Validate() <%p>'%s' entered, fileFormat='%s', spacecraft=<%p>'%s', "
       "outCoordSystem=<%p>'%s'\n", this, GetName().c_str(), fileFormat.c_str(), spacecraft,
       spacecraft ? spacecraft->GetName().c_str() : "NULL", outCoordSystem,
       outCoordSystem ? outCoordSystem->GetName().c_str() : "NULL");
   #endif
   
   ValidateParameters(false);
   
   #ifdef DEBUG_EPHEMFILE_INIT
   MessageInterface::ShowMessage
      ("EphemerisFile::Validate() <%p>'%s' returning true\n", this, GetName().c_str());
   #endif
   return true;
}


//------------------------------------------------------------------------------
// virtual bool Initialize()
//------------------------------------------------------------------------------
bool EphemerisFile::Initialize()
{
   #ifdef DEBUG_EPHEMFILE_INIT
   MessageInterface::ShowMessage
      ("EphemerisFile::Initialize() <%p>'%s' entered, spacecraftName='%s', active=%d, "
       "writeEphemeris=%d, isInitialized=%d\n   fileFormat='%s', stateType='%s', "
       "outputFormat='%s'\n", this, GetName().c_str(), spacecraftName.c_str(), active,
       writeEphemeris, isInitialized, fileFormat.c_str(), stateType.c_str(),
       outputFormat.c_str());
   #endif
   
   if (isInitialized)
   {
      #ifdef DEBUG_EPHEMFILE_INIT
      MessageInterface::ShowMessage
         ("EphemerisFile::Initialize() <%p>'%s' is already initialized so just returning true\n",
          this, GetName().c_str());
      #endif
      return true;
   }
   
   Subscriber::Initialize();
   
   // Do some validation, reset flags and clear buffers
   ValidateParameters(true);
   
   // Maximum segment size for CCSDS or SPK is initially set to 1000
   maxSegmentSize = 1000;
   
   // Set FileType
   if (fileFormat == "CCSDS-OEM")
      fileType = CCSDS_OEM;
   else if (fileFormat == "CCSDS-AEM")
      fileType = CCSDS_AEM;
   else if (fileFormat == "SPK" && stateType == "Cartesian")
      fileType = SPK_ORBIT;
   else if (fileFormat == "SPK" && stateType == "Quaternion")
      fileType = SPK_ATTITUDE;
   else if (fileFormat == "Code-500")
   {
      fileType = CODE500_EPHEM;
      maxSegmentSize = 50; // 50 orbit states per data record
      
      // This is get caught in ValidateParamters()
      // // Check for step size, only fixed step size is allowed
      // if (!useFixedStepSize)
      //    throw SubscriberException
      //       ("Code-500 ephemeris file \"" + GetName() + "\" requires fixed step size");
   }
   else
      throw SubscriberException
         ("FileFormat \"" + fileFormat + "\" is not valid");
   
   // Set interpolation flag for first and final state
   if (stepSize == "IntegratorSteps")
   {
      if (initialEpoch != "InitialSpacecraftEpoch")
         interpolateInitialState = true;
      
      if (finalEpoch != "FinalSpacecraftEpoch")
         interpolateFinalState = true;
   }
   
   // Do some more initialization here
   if (fileType == CCSDS_OEM || fileType == CODE500_EPHEM)
   {
      if (stepSizeInSecs == -999.999)
         stepSizeInSecs = 60.0;
   }
   
   // Initialize data
   writeMetaDataOption = 0;
   metaDataBegPosition = 0;
   metaDataEndPosition = 0;
   firstTimeWriting  = true;
   firstTimeMetaData = true;
   saveMetaDataStart = true;
   prevPropName = "";
   InitializeData();
   maneuversHandled.clear();
   
   // Get correct file name including file extension
   // We don't want to override fileName
   //fileName = SetProperFileExtension();
   //SetProperFileExtension();
   
   #ifdef DEBUG_EPHEMFILE_INIT
   MessageInterface::ShowMessage
      ("   fileType=%d, spacecraft=<%p>'%s', outCoordSystem=<%p>'%s'\n", fileType,
       spacecraft, spacecraft->GetName().c_str(), outCoordSystem,
       outCoordSystem->GetName().c_str());
   #endif
   
   // If active and not initialized already, open ephemeris file
   if (active && !isInitialized)
   {
      // We don't want to open ephemeris file here, since EphemerisFile inside
      // a function can be initialized multiple times. (LOJ: 2015.07.02)
      #if 0
      if (!OpenTextEphemerisFile())
      {
         #ifdef DEBUG_EPHEMFILE_INIT
         MessageInterface::ShowMessage
            ("EphemerisFile::Initialize() <%p>'%s' returning false\n",
             this, GetName().c_str());
         #endif
         throw SubscriberException
            ("Failed to open EphemerisFile \"" + fullPathFileName + "\"\n");
      }
      #endif
      
      isInitialized = true;
   }
   
   // Determine orbit or attitude, set to boolean to avoid string comparison
   if (stateType == "Cartesian")
      writeOrbit = true;
   else
      writeAttitude = true;
   
   // Determine output coordinate system, set to boolean to avoid string comparison
   // We don't need conversion for SPK_ORBIT. SpiceOrbitKernelWriter assumes it is in
   // J2000Eq frame for now
//   if ((fileType == CCSDS_OEM || fileType == CODE500_EPHEM) &&
//       theDataCoordSystem->GetName() != outCoordSystemName)
   if (theDataCoordSystem->GetName() != outCoordSystemName)
      writeDataInDataCS = false;
   
   // Convert inital and final epoch to A1ModJulian
   Real satInitialEpoch = ConvertInitialAndFinalEpoch();
   
   if (!useFixedStepSize)
   {      
      // If user defined initial epoch is the same or less than spacecraft
      // initial epoch, no interpolation is needed for the first state
      if (initialEpochA1Mjd == satInitialEpoch)
      {
         interpolateInitialState = false;
         initialEpochA1Mjd = -999.999;
         #ifdef DEBUG_EPHEMFILE_INIT
         MessageInterface::ShowMessage
            ("   ==> User defined initial epoch is equal to SC epoch, "
             "so turning off interpolateInitialState\n");
         #endif
      }
      else
      {
         // If using user defined initial epoch, make integrator step size to
         // difference between initialEpoch and spacecraft initial epoch
         if (initialEpoch != "InitialSpacecraftEpoch")
            stepSizeInSecs = (initialEpochA1Mjd - satInitialEpoch) * GmatTimeConstants::SECS_PER_DAY;
         
         #ifdef DEBUG_EPHEMFILE_INIT
         MessageInterface::ShowMessage
            ("   ==> User defined initial epoch is greater than SC epoch\n   "
             "initialEpochA1Mjd=%.12f, satInitialEpoch=%.12f, stepSizeInSecs=%.12f\n",
             initialEpochA1Mjd, satInitialEpoch, stepSizeInSecs);
         #endif
      }
   }
   
   
   #ifdef DEBUG_EPHEMFILE_INIT
   MessageInterface::ShowMessage
      ("   useFixedStepSize=%d, interpolateInitialState=%d, interpolateFinalState=%d\n",
       useFixedStepSize, interpolateInitialState, interpolateFinalState);
   #endif
   
   // Check if interpolator needs to be created
   if (useFixedStepSize || interpolateInitialState || interpolateFinalState)
      createInterpolator = true;
   else
      createInterpolator = false;
   
   
   // Create interpolator if needed
   if (createInterpolator)
      CreateInterpolator();
   
   // Set solver iteration option to none. We only writes solutions to a file
   mSolverIterOption = SI_NONE;

   #if 0
   // Create ephemeris file
   if (writeEphemeris)
      CreateEphemerisFile();
   #endif
   
   // Clear maneuvers handled array
   maneuversHandled.clear();
   
   #ifdef DEBUG_EPHEMFILE_INIT
   MessageInterface::ShowMessage
      ("EphemerisFile::Initialize() <%p>'%s' returning true, writeOrbit=%d, writeAttitude=%d, "
       "useFixedStepSize=%d,\n   writeDataInDataCS=%d, initialEpochA1Mjd=%.9f, "
       "finalEpochA1Mjd=%.9f, stepSizeInSecs=%.9f\n", this, GetName().c_str(), writeOrbit,
       writeAttitude, useFixedStepSize, writeDataInDataCS, initialEpochA1Mjd, finalEpochA1Mjd,
       stepSizeInSecs);
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the EphemerisFile.
 *
 * @return clone of the EphemerisFile.
 *
 */
//------------------------------------------------------------------------------
GmatBase* EphemerisFile::Clone(void) const
{
   return (new EphemerisFile(*this));
}


//---------------------------------------------------------------------------
// void Copy(const GmatBase* orig)
//---------------------------------------------------------------------------
/**
 * Sets this object to match another one.
 * 
 * @param orig The original that is being copied.
 */
//---------------------------------------------------------------------------
void EphemerisFile::Copy(const GmatBase* orig)
{
   operator=(*((EphemerisFile *)(orig)));
}


//------------------------------------------------------------------------------
// virtual bool TakeAction(const std::string &action,
//                         const std::string &actionData = "");
//------------------------------------------------------------------------------
/**
 * This method performs action.
 *
 * @param <action> action to perform
 * @param <actionData> action data associated with action
 * @return true if action successfully performed
 *
 */
//------------------------------------------------------------------------------
bool EphemerisFile::TakeAction(const std::string &action,
                               const std::string &actionData)
{
   #ifdef DEBUG_EPHEMFILE_ACTION
   MessageInterface::ShowMessage
      ("EphemerisFile::TakeAction() this=<%p>'%s' entered, action='%s', actionData='%s', "
       "isInitialized=%d, firstTimeWriting=%d\n", this, GetName().c_str(), action.c_str(),
       actionData.c_str(), isInitialized, firstTimeWriting);
   #endif
   
   bool retval = false;
   if (action == "Clear")
   {
      retval = true;
   }
   else if (action == "Finalize")
   {
      // Finish up writing if action is finalize. Finalize action is usually
      // set when function run completes (LOJ: 2015.07.02)
      if (writeEphemeris && isInitialized)
      {
         isEndOfRun = true;
         FinishUpWriting(true);
      }
      retval = true;
   }
   else if (action == "ToggleOn")
   {
      // Create ephemeris file in case it was initially turned off and toggled on
      CreateEphemerisFile();
      
      // If it was last toggled off
      if (fileType == CODE500_EPHEM && toggleStatus == 2 && !firstTimeWriting)
         checkForLargeTimeGap = true;
      else
         checkForLargeTimeGap = false;
      
      toggleStatus = 1;
      writeEphemeris = true;
      retval = true;
   }
   else if (action == "ToggleOff")
   {
      // Check if it needs to check for large time gap for Code500
      if (fileType == CODE500_EPHEM && !firstTimeWriting)
         checkForLargeTimeGap = true;
      else
         checkForLargeTimeGap = false;
      
      toggleStatus = 2;
      writeEphemeris = false;
      // If toggle off, finish writing ephemeris and restart interpolation
      //LOJ: Write continuous ephemeris if CODE500_EPHEM
      if (fileType != CODE500_EPHEM)
         RestartInterpolation("", false, true, true);
      retval = true;
   }
   else if (action == "ChangeTypeName")
   {
      typeName = actionData;
      retval = true;
   }
   
   #ifdef DEBUG_EPHEMFILE_ACTION
   MessageInterface::ShowMessage
      ("EphemerisFile::TakeAction() this=<%p>'%s', action='%s', actionData='%s' "
       "returning %d\n", this, GetName().c_str(), action.c_str(), actionData.c_str(), retval);
   #endif
   return retval;
}


//---------------------------------------------------------------------------
//  bool RenameRefObject(const Gmat::ObjectType type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
bool EphemerisFile::RenameRefObject(const Gmat::ObjectType type,
                                    const std::string &oldName,
                                    const std::string &newName)
{
   if (type != Gmat::SPACECRAFT && type != Gmat::COORDINATE_SYSTEM)
      return true;
   
   if (type == Gmat::SPACECRAFT)
   {
      if (spacecraftName == oldName)
         spacecraftName = newName;
   }
   else if (type == Gmat::COORDINATE_SYSTEM)
   {
      if (outCoordSystemName == oldName)
         outCoordSystemName = newName;
   }
   
   return Subscriber::RenameRefObject(type, oldName, newName);
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
std::string EphemerisFile::GetParameterText(const Integer id) const
{
    if (id >= SubscriberParamCount && id < EphemerisFileParamCount)
        return PARAMETER_TEXT[id - SubscriberParamCount];
    else
        return Subscriber::GetParameterText(id);
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
Integer EphemerisFile::GetParameterID(const std::string &str) const
{
   for (Integer i = SubscriberParamCount; i < EphemerisFileParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - SubscriberParamCount])
      {
         if (i == FILE_NAME)
            WriteDeprecatedMessage(i);
         return i;
      }
   }
   return Subscriber::GetParameterID(str);
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
Gmat::ParameterType EphemerisFile::GetParameterType(const Integer id) const
{
    if (id >= SubscriberParamCount && id < EphemerisFileParamCount)
        return PARAMETER_TYPE[id - SubscriberParamCount];
    else
        return Subscriber::GetParameterType(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
std::string EphemerisFile::GetParameterTypeString(const Integer id) const
{
   if (id >= SubscriberParamCount && id < EphemerisFileParamCount)
      return EphemerisFile::PARAM_TYPE_STRING[GetParameterType(id)];
   else
      return Subscriber::GetParameterTypeString(id);
}


//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param <id> Description for the parameter.
 *
 * @return true if the parameter is read only, false (the default) if not,
 *         throws if the parameter is out of the valid range of values.
 */
//---------------------------------------------------------------------------
bool EphemerisFile::IsParameterReadOnly(const Integer id) const
{
   if (id == SOLVER_ITERATIONS)
      return true;
   if (id == FILE_NAME)
      return true;
   // Disable state type until it is selectable -- currently must be Cartesian
   if (id == STATE_TYPE)
      return true;
   if (id == FULLPATH_FILENAME)
      return true;
   // Disable interpolator type until it is selectable -- currently set by
   // ephem file format
   // Commented to fix GMT-4022
   //if (id == INTERPOLATOR)
   //   return true;
   
   return Subscriber::IsParameterReadOnly(id);
}

//------------------------------------------------------------------------------
// bool IsParameterCommandModeSettable(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Tests to see if an object property can be set in Command mode
 *
 * @param id The ID of the object property
 *
 * @return true if the property can be set in command mode, false if not.
 */
//------------------------------------------------------------------------------
bool EphemerisFile::IsParameterCommandModeSettable(const Integer id) const
{
   // Override this one from the parent class
   if (id == FILENAME || id == WRITE_EPHEMERIS)
      return true;
   
   // And let the parent class handle its own
   return Subscriber::IsParameterCommandModeSettable(id);
}

//---------------------------------------------------------------------------
// Gmat::ObjectType GetPropertyObjectType(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieves object type of parameter of given id.
 *
 * @param <id> ID for the parameter.
 *
 * @return parameter ObjectType
 */
//---------------------------------------------------------------------------
Gmat::ObjectType EphemerisFile::GetPropertyObjectType(const Integer id) const
{
   switch (id)
   {
   case SPACECRAFT:
      return Gmat::SPACECRAFT;
   case INTERPOLATOR:
      return Gmat::INTERPOLATOR;
   case COORDINATE_SYSTEM:
      return Gmat::COORDINATE_SYSTEM;
   default:
      return Subscriber::GetPropertyObjectType(id);
   }
}


//---------------------------------------------------------------------------
// const StringArray& GetPropertyEnumStrings(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieves eumeration symbols of parameter of given id.
 *
 * @param <id> ID for the parameter.
 *
 * @return list of enumeration symbols
 */
//---------------------------------------------------------------------------
const StringArray& EphemerisFile::GetPropertyEnumStrings(const Integer id) const
{
   switch (id)
   {
   case FILE_FORMAT:
      return fileFormatList;
   case EPOCH_FORMAT:
      return epochFormatList;
   case INITIAL_EPOCH:
      return initialEpochList;
   case FINAL_EPOCH:
      return finalEpochList;
   case STEP_SIZE:
      return stepSizeList;
   case STATE_TYPE:
      return stateTypeList;
   case INTERPOLATOR:
      return interpolatorTypeList;
   case OUTPUT_FORMAT:
      return outputFormatList;
   default:
      return Subscriber::GetPropertyEnumStrings(id);
   }
}


//------------------------------------------------------------------------------
// bool GetBooleanParameter(const Integer id) const
//------------------------------------------------------------------------------
bool EphemerisFile::GetBooleanParameter(const Integer id) const
{
   switch (id)
   {
   case WRITE_EPHEMERIS:
      return writeEphemeris;
   default:
      return Subscriber::GetBooleanParameter(id);
   }
}


//------------------------------------------------------------------------------
// bool SetBooleanParameter(const Integer id, const bool value)
//------------------------------------------------------------------------------
bool EphemerisFile::SetBooleanParameter(const Integer id, const bool value)
{
   switch (id)
   {
   case WRITE_EPHEMERIS:
      writeEphemeris = value;
      active = value;
      return writeEphemeris;
   default:
      return Subscriber::SetBooleanParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// Integer GetIntegerParameter(const Integer id) const
//------------------------------------------------------------------------------
Integer EphemerisFile::GetIntegerParameter(const Integer id) const
{
   switch (id)
   {
   case INTERPOLATION_ORDER:
      return interpolationOrder;
   default:
      return Subscriber::GetIntegerParameter(id);
   }
}


//------------------------------------------------------------------------------
// Integer SetIntegerParameter(const Integer id, const Integer value)
//------------------------------------------------------------------------------
Integer EphemerisFile::SetIntegerParameter(const Integer id, const Integer value)
{
   switch (id)
   {
   case INTERPOLATION_ORDER:
   {
      bool violatesHermiteOddness = false;
      if (interpolatorName == "Hermite")
      {
         // Make sure the number is odd
         Integer roundTrip = (Integer)((value / 2) * 2);
         if (roundTrip == value)  // Number is even
            violatesHermiteOddness = true;
      }

      if ((value >= 1 && value <= 10) && !violatesHermiteOddness)
      {
         interpolationOrder = value;
         return value;
      }
      else
      {
         SubscriberException se;
         if (interpolatorName == "Hermite")
         {
            se.SetDetails(errorMessageFormat.c_str(),
                          GmatStringUtil::ToString(value, 1).c_str(),
                          GetParameterText(INTERPOLATION_ORDER).c_str(),
                          "1 <= Odd Integer Number <= 10");
         }
         else
         {
            se.SetDetails(errorMessageFormat.c_str(),
                          GmatStringUtil::ToString(value, 1).c_str(),
                          GetParameterText(INTERPOLATION_ORDER).c_str(),
                          "1 <= Integer Number <= 10");
         }
         throw se;
      }
   }
   default:
      return Subscriber::SetIntegerParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
std::string EphemerisFile::GetStringParameter(const Integer id) const
{
   switch (id)
   {
   case SPACECRAFT:
      return spacecraftName;
   case FILENAME:
      return fileName;
   case FULLPATH_FILENAME:
      return fullPathFileName;
   case FILE_FORMAT:
      return fileFormat;
   case EPOCH_FORMAT:
      return epochFormat;
   case INITIAL_EPOCH:
      return initialEpoch;
   case FINAL_EPOCH:
      return finalEpoch;
   case STEP_SIZE:
      return stepSize;
   case INTERPOLATOR:
      return interpolatorName;
   case STATE_TYPE:
      return stateType;
   case COORDINATE_SYSTEM:
      return outCoordSystemName;
   case OUTPUT_FORMAT:
      return outputFormat;
   case FILE_NAME:
      WriteDeprecatedMessage(id);
      return fileName;
   default:
      return Subscriber::GetStringParameter(id);
   }
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
std::string EphemerisFile::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
bool EphemerisFile::SetStringParameter(const Integer id, const std::string &value)
{
   #ifdef DEBUG_EPHEMFILE_SET
   MessageInterface::ShowMessage
      ("EphemerisFile::SetStringParameter() this=<%p>'%s' entered, id=%d, "
       "value='%s'\n   isInitialized=%d, firstTimeWriting=%d\n", this, GetName().c_str(),
       id, value.c_str(), isInitialized, firstTimeWriting);
   #endif
   
   switch (id)
   {
   case SPACECRAFT:
      spacecraftName = value;
      return true;
   case FILENAME:
   {
      #ifdef DEBUG_EPHEMFILE_SET
      MessageInterface::ShowMessage
         ("EphemerisFile::SetStringParameter() Setting Filename '%s' to "
          "EphemerisFile '%s', isInitialized=%d\n", value.c_str(), instanceName.c_str(),
          isInitialized);
      #endif
      
      // Validate filename
      if (!GmatFileUtil::IsValidFileName(value))
      {
         std::string msg = GmatFileUtil::GetInvalidFileNameMessage(1);
         SubscriberException se;
         se.SetDetails(errorMessageFormat.c_str(), value.c_str(), "Filename", msg.c_str());
         throw se;
      }
      
      // If already initialized, close ephemeris file first
      if (isInitialized)
      {
         #ifdef DEBUG_EPHEMFILE_INIT
         MessageInterface::ShowMessage
            ("EphemerisFile::Initialize() <%p>'%s' is already initialized so finish "
             "it first, if ephemeris file is opened, isEphemFileOpened=%d\n", this,
             GetName().c_str(), isEphemFileOpened);
         #endif
         if (isEphemFileOpened)
            FinishUpWriting(true);
      }
      
      prevFileName = fileName;
      fileName = value;
      fullPathFileName =
         GmatBase::GetFullPathFileName(fileName, GetName(), fileName, "EPHEM_OUTPUT_FILE",
                                       false, ".eph", false, true);
      
      // If filename is set in resource mode, set prevFileName
      if (!isInitialized)
         prevFileName = fileName;
      
      usingDefaultFileName = false;
      #ifdef DEBUG_EPHEMFILE_SET
      MessageInterface::ShowMessage
         ("   prevFileName='%s'\n   fileName='%s'\n   fullPathFileName='%s'\n",
          prevFileName.c_str(), fileName.c_str(), fullPathFileName.c_str());
      MessageInterface::ShowMessage
         ("EphemerisFile::SetStringParameter() this=<%p>'%s' returning true\n",
          this, GetName().c_str());
      #endif
      return true;
   }
   // Interpolator is now set along with file format (bug 2219)
   case FILE_FORMAT:
      if (find(fileFormatList.begin(), fileFormatList.end(), value) !=
          fileFormatList.end())
      {
         fileFormat = value;
         
         // Code to link interpolator selection to file type
         if (fileFormat == "CCSDS-OEM")
            interpolatorName = "Lagrange";
         else if (fileFormat == "SPK")
            interpolatorName = "Hermite";
         else if (fileFormat == "Code-500")
         {
            interpolatorName = "Lagrange";
            // Also set default fixed step size to 60.0
            stepSize = "60.0";
            useFixedStepSize = true;
            createInterpolator = true;
         }
         return true;
      }
      else
      {
         HandleError(FILE_FORMAT, value, fileFormatList);
      }
   case EPOCH_FORMAT:
      if (find(epochFormatList.begin(), epochFormatList.end(), value) !=
          epochFormatList.end())
      {
         epochFormat = value;
         return true;
      }
      else
      {
         HandleError(EPOCH_FORMAT, value, epochFormatList);
      }
   case INITIAL_EPOCH:
      if (find(initialEpochList.begin(), initialEpochList.end(), value) !=
          initialEpochList.end())
      {
         initialEpoch = value;
         return true;
      }
      else
      {
         return SetEpoch(INITIAL_EPOCH, value, initialEpochList);
      }
   case FINAL_EPOCH:
      if (find(finalEpochList.begin(), finalEpochList.end(), value) !=
          finalEpochList.end())
      {
         finalEpoch = value;
         return true;
      }
      else
      {
         return SetEpoch(FINAL_EPOCH, value, finalEpochList);
      }
   case STEP_SIZE:
      if (find(stepSizeList.begin(), stepSizeList.end(), value) !=
          stepSizeList.end())
      {
         stepSize = value;
         useFixedStepSize = false;
         return true;
      }
      else
      {
         return SetStepSize(STEP_SIZE, value, stepSizeList);
      }
   // Interpolator is now set along with file format (bug 2219); if the parm is
   // passed in, just ensure compatibility
   case INTERPOLATOR:
      if (fileFormat == "CCSDS-OEM" || fileFormat == "Code-500")
      {
         if (value != "Lagrange")
            throw SubscriberException("Cannot set interpolator \"" + value +
                  "\" on the EphemerisFile named \"" + instanceName + "\"; " +
                  fileFormat + " ephemerides require \"Lagrange\" interpolators");
      }
      else if (fileFormat == "SPK")
      {
         if (value != "Hermite")
            throw SubscriberException("Cannot set interpolator \"" + value +
                  "\" on the EphemerisFile named \"" + instanceName +
                  "\"; SPK ephemerides require \"Hermite\" interpolators");
      }
      else
         throw SubscriberException("The interpolator \"" + value +
               "\" on the EphemerisFile named \"" + instanceName +
               "\" cannot be set; set the file format to set the interpolator");
      return true;
   case STATE_TYPE:
      if (find(stateTypeList.begin(), stateTypeList.end(), value) !=
          stateTypeList.end())
      {
         stateType = value;
         return true;
      }
      else
      {
         HandleError(STATE_TYPE, value, stateTypeList);
      }
   case COORDINATE_SYSTEM:
      outCoordSystemName = value;
      return true;
   case OUTPUT_FORMAT:
      if (find(outputFormatList.begin(), outputFormatList.end(), value) !=
          outputFormatList.end())
      {
         outputFormat = value;
         return true;
      }
      else
      {
         HandleError(OUTPUT_FORMAT, value, outputFormatList);
      }
   case FILE_NAME:
      WriteDeprecatedMessage(id);
      // Changed to call SetStringParameter(FILENAME, value) (LOJ: 2014.06.23)
      // fileName = value;
      // fullPathFileName = fileName;
      // if (fileName.find("/") == fileName.npos &&
      //     fileName.find("\\") == fileName.npos)
      //    fullPathFileName = outputPath + fileName;
      // return true;
      return SetStringParameter(FILENAME, value);
      
   default:
      return Subscriber::SetStringParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label,
//                         const std::string &value)
//------------------------------------------------------------------------------
bool EphemerisFile::SetStringParameter(const std::string &label,
                                       const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// virtual GmatBase* GetRefObject(const Gmat::ObjectType type,
//                                const std::string &name)
//------------------------------------------------------------------------------
GmatBase* EphemerisFile::GetRefObject(const Gmat::ObjectType type,
                                      const std::string &name)
{
   if (type == Gmat::SPACECRAFT)
      return spacecraft;
   
   if (type == Gmat::COORDINATE_SYSTEM)
      return outCoordSystem;
   
   return Subscriber::GetRefObject(type, name);
}


//------------------------------------------------------------------------------
// virtual bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//                           const std::string &name = "")
//------------------------------------------------------------------------------
bool EphemerisFile::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                 const std::string &name)
{
   #if DBGLVL_EPHEMFILE_REF_OBJ
   MessageInterface::ShowMessage
      ("EphemerisFile::SetRefObject() <%p>'%s' entered, obj=%p, name=%s, objtype=%s, "
       "objname=%s\n", this, GetName().c_str(), obj, name.c_str(), obj->GetTypeName().c_str(),
       obj->GetName().c_str());
   #endif
   
   if (type == Gmat::SPACECRAFT && name == spacecraftName)
   {
      spacecraft = (Spacecraft*)obj;
      if (spacecraft)
         spacecraftId = spacecraft->GetStringParameter("Id");
      return true;
   }
   else if (type == Gmat::COORDINATE_SYSTEM && name == outCoordSystemName)
   {
      outCoordSystem = (CoordinateSystem*)obj;
      return true;
   }
   
   return Subscriber::SetRefObject(obj, type, name);
}


//------------------------------------------------------------------------------
// virtual const StringArray& GetRefObjectNameArray(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
const StringArray& EphemerisFile::GetRefObjectNameArray(const Gmat::ObjectType type)
{
   refObjectNames.clear();
   
   if (type == Gmat::SPACECRAFT || type == Gmat::UNKNOWN_OBJECT)
      refObjectNames.push_back(spacecraftName);
   
   if (type == Gmat::COORDINATE_SYSTEM || type == Gmat::UNKNOWN_OBJECT)
      refObjectNames.push_back(outCoordSystemName);
   
   return refObjectNames;
}


//--------------------------------------
// protected methods
//--------------------------------------

//------------------------------------------------------------------------------
// void InitializeData(bool saveEpochInfo = true)
//------------------------------------------------------------------------------
void EphemerisFile::InitializeData(bool saveEpochInfo)
{
   #ifdef DEBUG_EPHEMFILE_RESTART
   MessageInterface::ShowMessage
      ("===== EphemerisFile::InitializeData() '%s' entered, saveEpochInfo=%d\n",
       GetName().c_str(), saveEpochInfo);
   #endif
   
   epochsOnWaiting.clear();
   
   if (interpolator != NULL)
      interpolator->Clear();
   
   initialCount         = 0;
   waitCount            = 0;
   afterFinalEpochCount = 0;
   
   if (!saveEpochInfo)
   {
      currEpochInDays      = -999.999;
      currEpochInSecs      = -999.999;
   }
   
   blockBeginA1Mjd      = -999.999;
   nextOutEpochInSecs   = -999.999;
   nextReqEpochInSecs   = -999.999;
   prevEpochInSecs      = -999.999;
   prevProcTime         = -999.999;
   lastEpochWrote       = -999.999;
   eventEpochInSecs     = -999.999;
   writingNewSegment    = true;
   continuousSegment    = false;
   firstTimeMetaData    = true;
   saveMetaDataStart    = true;
   
   #ifdef DEBUG_EPHEMFILE_RESTART
   MessageInterface::ShowMessage
      ("===== EphemerisFile::InitializeData() '%s' leaving\n", GetName().c_str());
   #endif
}


//------------------------------------------------------------------------------
// void CreateInterpolator()
//------------------------------------------------------------------------------
void EphemerisFile::CreateInterpolator()
{
   #ifdef DEBUG_EPHEMFILE_INTERPOLATOR
   MessageInterface::ShowMessage
      ("EphemerisFile::CreateInterpolator() entered, interpolatorName='%s', "
       "interpolator=<%p>'%s'\n", interpolatorName.c_str(),
       interpolator, interpolator ? interpolator->GetName().c_str() : "NULL");
   #endif
   
   // If ephemeris output type is SPK, no need to create interpolator
   if (fileType == SPK_ORBIT || fileType == SPK_ATTITUDE)
      return;
   
   // If interpolator is not NULL, delete it first
   if (interpolator != NULL)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (interpolator, interpolator->GetName(), "EphemerisFile::CreateInterpolator()",
          "deleting local interpolator");
      #endif
      delete interpolator;
      interpolator = NULL;
   }
   
   // Create Interpolator
   if (interpolatorName == "Lagrange")
   {
      interpolator = new LagrangeInterpolator(instanceName+"_Lagrange", 6,
                                              interpolationOrder);
      #ifdef DEBUG_INTERPOLATOR_TRACE
      MessageInterface::ShowMessage
         ("===> CreateInterpolator() calling interpolator->SetForceInterpolation(false)\n");
      #endif
      // Set force interpolation to false to collect more data if needed
      interpolator->SetForceInterpolation(false);
      
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Add
         (interpolator, interpolator->GetName(), "EphemerisFile::CreateInterpolator()",
          "interpolator = new LagrangeInterpolator()");
      #endif
   }
   else if (interpolatorName == "SLERP")
   {
      throw SubscriberException("The SLERP Interpolator is not ready\n");
      //interpolator = new SLERP;
   }
   
   #ifdef DEBUG_EPHEMFILE_INTERPOLATOR
   MessageInterface::ShowMessage
      ("EphemerisFile::CreateInterpolator() leaving, interpolator=<%p>'%s'\n",
       interpolator, interpolator ? interpolator->GetName().c_str() : "NULL");
   #endif
}


//------------------------------------------------------------------------------
// void EphemerisFile::CreateEphemerisFile()
//------------------------------------------------------------------------------
void EphemerisFile::CreateEphemerisFile()
{
   if (fileType == SPK_ORBIT)
      CreateSpiceKernelWriter();
   else if (fileType == CODE500_EPHEM)
      CreateCode500EphemerisFile();
}


//------------------------------------------------------------------------------
// void CreateSpiceKernelWriter()
//------------------------------------------------------------------------------
void EphemerisFile::CreateSpiceKernelWriter()
{
   #ifdef DEBUG_EPHEMFILE_SPICE
   MessageInterface::ShowMessage
      ("EphemerisFile::CreateSpiceKernelWriter() entered, spkWriter=<%p>, "
       "isInitialized=%d, firstTimeWriting=%d\n   prevFileName='%s', "
       "fileName='%s'\n", spkWriter, isInitialized, firstTimeWriting,
       prevFileName.c_str(), fileName.c_str());
   #endif
   
   //=======================================================
   #ifdef __USE_SPICE__
   //=======================================================
   // If spkWriter is not NULL, delete it first
   // Do we really want to delete here? This causes to only write
   // segments after toggle on. So all previous data are wiped out.
   // So check for file name change in the mission sequence
   // before creating new spkWriter (LOJ: 2015.10.14)
   if (spkWriter != NULL)
   {
      if (prevFileName != fileName)
      {
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (spkWriter, "spkWriter", "EphemerisFile::CreateSpiceKernelWriter()",
             "deleting local spkWriter");
         #endif
         delete spkWriter;
         spkWriter = NULL;
      }
   }
   
   // Create spkWrite if it is NULL (LOJ: 2015.10.14)
   if (spkWriter == NULL)
   {
      std::string name = spacecraft->GetName();
      std::string centerName = outCoordSystem->GetOriginName();
      Integer objNAIFId = spacecraft->GetIntegerParameter("NAIFId");
      Integer centerNAIFId = (outCoordSystem->GetOrigin())->GetIntegerParameter("NAIFId");
      
      #ifdef DEBUG_EPHEMFILE_SPICE
      MessageInterface::ShowMessage
         ("   Creating SpiceOrbitKernelWriter with name='%s', centerName='%s', "
          "objNAIFId=%d, centerNAIFId=%d\n   fileName='%s', interpolationOrder=%d\n",
          name.c_str(), centerName.c_str(), objNAIFId, centerNAIFId,
          fileName.c_str(), interpolationOrder);
      #endif
      
      try
      {
         spkWriter =
            new SpiceOrbitKernelWriter(name, centerName, objNAIFId, centerNAIFId,
                                       fullPathFileName, interpolationOrder, "J2000");
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Add
            (spkWriter, "spkWriter", "EphemerisFile::CreateSpiceKernelWriter()",
             "spkWriter = new SpiceOrbitKernelWriter()");
         #endif
      }
      catch (BaseException &e)
      {
         // Keep from setting a warning
         e.GetMessageType();
         
         #ifdef DEBUG_EPHEMFILE_SPICE
         MessageInterface::ShowMessage(
            "  Error creating SpiceOrbitKernelWriter: %s", (e.GetFullMessage()).c_str());
         #endif
         throw;
      }
   }
   
   
   //=======================================================
   #else
   //=======================================================
   MessageInterface::ShowMessage
      ("*** WARNING *** Use of SpiceOrbitKernelWriter is turned off\n");
   //=======================================================
   #endif
   //=======================================================
   
   #ifdef DEBUG_EPHEMFILE_SPICE
   MessageInterface::ShowMessage
      ("EphemerisFile::CreateSpiceKernelWriter() leaving, spkWriter=<%p>\n",
       spkWriter);
   #endif
}


//------------------------------------------------------------------------------
// void CreateCode500EphemerisFile()
//------------------------------------------------------------------------------
void EphemerisFile::CreateCode500EphemerisFile()
{
   #ifdef DEBUG_CODE500_CREATE
   MessageInterface::ShowMessage
      ("\nEphemerisFile::CreateCode500EphemerisFile() this=<%p>'%s' entered, code500EphemFile=<%p>,"
       " outputFormat='%s'\n", this, GetName().c_str(), code500EphemFile, outputFormat.c_str());
   #endif
   
   // If code500EphemFile is not NULL, delete it first
   if (code500EphemFile != NULL)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (code500EphemFile, "code500EphemFile", "EphemerisFile::CreateCode500EphemerisFile()",
          "deleting local code500EphemFile");
      #endif
      delete code500EphemFile;
      code500EphemFile = NULL;
   }
   
   Real        satId       = 101.0;   // dummy for now
   std::string timeSystem  = "UTC";   // Figure out time system here
   std::string sourceId    = "GTDS";
   //std::string centralBody = spacecraft->GetOriginName();
   std::string centralBody = outCoordSystem->GetOriginName();
   int ephemOutputFormat = 1;
   if (outputFormat == "UNIX")
      ephemOutputFormat = 2;
   
   #ifdef DEBUG_CODE500_CREATE
   MessageInterface::ShowMessage
      ("   Creating Code500EphemerisFile with satId=%f, timeSystem='%s', sourceId='%s', "
       "centralBody='%s', ephemOutputFormat=%d\n", satId, timeSystem.c_str(), sourceId.c_str(),
       centralBody.c_str(), ephemOutputFormat);
   #endif
   
   try
   {
      code500EphemFile =
         //new Code500EphemerisFile(fileName, satId, timeSystem, sourceId, centralBody, 2, ephemOutputFormat);
         new Code500EphemerisFile(fullPathFileName, satId, timeSystem, sourceId, centralBody, 2, ephemOutputFormat);
      
      // Set origin mu to code500 ephem so that it can do conversion
      SpacePoint *origin = outCoordSystem->GetOrigin();
      if (!origin->IsOfType("CelestialBody"))
      {
         SubscriberException se;
         se.SetDetails("Output coordinate system for Code500 ephemeris file must"
                       "have a celestial body origin\n");
         throw se;
      }
      Real cbMu = ((CelestialBody*) origin)->GetGravitationalConstant();
      code500EphemFile->SetCentralBodyMu(cbMu);
      code500EphemFile->SetTimeIntervalBetweenPoints(stepSizeInSecs);
   }
   catch (BaseException &e)
   {
      // Keep from getting a compiler warning about e not being used
      e.GetMessageType();
      
      #ifdef DEBUG_CODE500_CREATE
      MessageInterface::ShowMessage
         ("Error opening Code500EphemerisFile: %s", (e.GetFullMessage()).c_str());
      #endif
      throw;
   }
   
   #ifdef DEBUG_MEMORY
   MemoryTracker::Instance()->Add
      (code500EphemFile, "code500EphemFile", "EphemerisFile::CreateCode500EphemerisFile()",
       "code500EphemFile = new SpiceOrbitKernelWriter()");
   #endif
   
   #ifdef DEBUG_CODE500_CREATE
   MessageInterface::ShowMessage
      ("EphemerisFile::CreateCode500EphemerisFile() leaving, code500EphemFile=<%p>\n",
       code500EphemFile);
   #endif
}


//------------------------------------------------------------------------------
// bool OpenTextEphemerisFile()
//------------------------------------------------------------------------------
bool EphemerisFile::OpenTextEphemerisFile()
{
   #ifdef DEBUG_EPHEMFILE_OPEN
   MessageInterface::ShowMessage
      ("EphemerisFile::OpenTextEphemerisFile() <%p>'%s' entered, fileName = %s\n",
       this, GetName().c_str(), fileName.c_str());
   #endif
   
   // If default file name is used, write informatinal message about the file location (LOJ: 2014.06.24)
   if (usingDefaultFileName)
      MessageInterface::ShowMessage
         ("*** The output file '%s' will be written as \n                    '%s'\n",
          fileName.c_str(), fullPathFileName.c_str());
   
   //fileName = SetProperFileExtension();
   SetProperFileExtension();
   bool retval = true;
   
   #if !defined(__USE_DATAFILE__) || defined(DEBUG_EPHEMFILE_TEXT)
   // Close the stream if it is open
   if (dstream.is_open())
      dstream.close();
   
   std::string fileNameToOpen = fullPathFileName;
   std::string debugFileName;
   bool openDebugFile = false;
   
   // For CCSDS file, DEBUG_EPHEMERIS_TEXT has no effect if not using DATAFILE
   // since it writes to a text file as debug is turned on
   if (fileType == CCSDS_OEM || fileType == CCSDS_AEM)
   {
      #if defined(__USE_DATAFILE__) && defined(DEBUG_EPHEMFILE_TEXT)
      debugFileName = fileNameToOpen + ".txt";
      openDebugFile = true;
      #elif !defined(__USE_DATAFILE__)
      debugFileName = fileNameToOpen;
      openDebugFile = true;
      #endif
   }
   else
   {
      #ifdef DEBUG_EPHEMFILE_TEXT
      debugFileName = fileNameToOpen + ".txt";
      openDebugFile = true;
      #endif
   }
   
   if (openDebugFile)
   {
      dstream.open(debugFileName.c_str());
      if (dstream.is_open())
      {
         retval = true;
         #ifdef DEBUG_EPHEMFILE_TEXT
         MessageInterface::ShowMessage
            ("   '%s' is opened for debug\n", debugFileName.c_str());
         #endif
      }
      else
      {
         retval = false;
         #ifdef DEBUG_EPHEMFILE_TEXT
         MessageInterface::ShowMessage
            ("   '%s' was failed open\n", debugFileName.c_str());
         #endif
      }
   }
   #endif
   
   // Open CCSDS output file
   if (fileType == CCSDS_OEM)
   {
      #ifdef DEBUG_EPHEMFILE_OPEN
      MessageInterface::ShowMessage("   About to open CCSDS output file\n");
      #endif
      
      if (!OpenCcsdsEphemerisFile())
         return false;
   }
   
   #ifdef DEBUG_EPHEMFILE_OPEN
   MessageInterface::ShowMessage
      ("EphemerisFile::OpenTextEphemerisFile() returning %d\n", retval);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// void CloseEphemerisFile(bool done = true, writeMetaData = true)
//------------------------------------------------------------------------------
void EphemerisFile::CloseEphemerisFile(bool done, bool writeMetaData)
{
   // Close SPK file
   #ifdef __USE_SPICE__
   #ifdef DEBUG_EPHEMFILE_SPICE
   MessageInterface::ShowMessage
      ("EphemerisFile::CloseEphemerisFile() spkWriter=<%p>, spkWriteFailed=%s\n",
       spkWriter, (spkWriteFailed? "true":"false"));
   #endif
   if (spkWriter != NULL)
   {
      if (!spkWriteFailed)
         FinalizeSpkFile(done, writeMetaData);
      
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (spkWriter, "SPK writer", "EphemerisFile::~EphemerisFile()()",
          "deleting local SPK writer");
      #endif
      if (done)
      {
         delete spkWriter;
         spkWriter = NULL;
      }
   }
   #endif
   
   // Close CCSDS file
   dstream.flush();
   dstream.close();
}

//------------------------------------------------------------------------------
// bool InsufficientSPKData()
//------------------------------------------------------------------------------
bool EphemerisFile::InsufficientSPKData()
{
   return insufficientSPKData;
}


//------------------------------------------------------------------------------
// Real ConvertInitialAndFinalEpoch()
//------------------------------------------------------------------------------
/**
 * Converts initial and final epoch to A1ModJulian time format since spacecraft
 * uses this time format
 *
 * @throws SubscriberException if initial epoch is before spacecraft initial epoch
 */
//------------------------------------------------------------------------------
Real EphemerisFile::ConvertInitialAndFinalEpoch()
{
   #ifdef DEBUG_CONVERT_INITIAL_FINAL_EPOCH
   MessageInterface::ShowMessage
      ("EphemerisFile::ConvertInitialAndFinalEpoch() <%p>'%s' entered\n",
       this, GetName().c_str());
   #endif
   
   // Convert initial and final epoch to A1ModJulian format if needed.
   // Currently spacecraft uses A1ModJulian as output epoch
   Real dummyA1Mjd = -999.999;
   std::string epochStr;
   
   // Convert initial epoch to A1Mjd
   if (initialEpoch != "InitialSpacecraftEpoch")
      TimeConverterUtil::Convert(epochFormat, dummyA1Mjd, initialEpoch,
                                 "A1ModJulian", initialEpochA1Mjd, epochStr);
   
   // Convert final epoch to A1Mjd
   if (finalEpoch != "FinalSpacecraftEpoch")
      TimeConverterUtil::Convert(epochFormat, dummyA1Mjd, finalEpoch,
                                 "A1ModJulian", finalEpochA1Mjd, epochStr);
   
   // Check if ephemeris initial epoch is before the spacecraft initial epoch
   Real satInitialEpoch = spacecraft->GetEpoch();
   if ((initialEpoch != "InitialSpacecraftEpoch") && initialEpochA1Mjd < satInitialEpoch)
   {
      SubscriberException se;
      se.SetDetails("Initial epoch (%f) of ephemeris file \"%s\" cannot be before "
                    "initial epoch (%f) of spacecraft \"%s\"\n", initialEpochA1Mjd,
                    GetName().c_str(), satInitialEpoch, spacecraft->GetName().c_str());
      throw se;
   }
   
   // Check if ephemeris final epoch is greater than initial epoch
   if ((initialEpoch != "InitialSpacecraftEpoch") && (finalEpoch != "FinalSpacecraftEpoch"))
   {
      if (initialEpochA1Mjd > finalEpochA1Mjd)
      {
         SubscriberException se;
         se.SetDetails("Initial epoch (%f) of ephemeris file \"%s\" cannot be greater than "
                       "final epoch (%f)\n", initialEpochA1Mjd, GetName().c_str(),
                       finalEpochA1Mjd);
         throw se;
      }
   }
   
   #ifdef DEBUG_CONVERT_INITIAL_FINAL_EPOCH
   MessageInterface::ShowMessage
      ("EphemerisFile::ConvertInitialAndFinalEpoch() <%p>'%s' returning "
       "initialEpochA1Mjd=%.15f, finalEpochA1Mjd=%.15f, satInitialEpoch=%.15f\n",
       this, GetName().c_str(), initialEpochA1Mjd, finalEpochA1Mjd, satInitialEpoch);
   #endif
   return satInitialEpoch;
}


//------------------------------------------------------------------------------
// bool CheckInitialAndFinalEpoch()
//------------------------------------------------------------------------------
bool EphemerisFile::CheckInitialAndFinalEpoch()
{
   #ifdef DEBUG_EPHEMFILE_WRITE
   MessageInterface::ShowMessage
      ("\nEphemerisFile::CheckInitialAndFinalEpoch() entered, currEpochInDays=%.15f\n   "
       "initialEpochA1Mjd=%.15f, finalEpochA1Mjd=%.15f, finalEpochReached=%d, finalEpochProcessed=%d\n",
       currEpochInDays, initialEpochA1Mjd, finalEpochA1Mjd, finalEpochReached, finalEpochProcessed);
   #endif
   
   // Check initial and final epoch for writing, dat[0] is epoch
   bool writeData = false;
   bool initialEpochReached = true;
   finalEpochReached = false;
   
   // From InitialSpacecraftEpoch to FinalSpacecraftEpoch
   if (initialEpochA1Mjd == -999.999 && finalEpochA1Mjd == -999.999)
   {
      writeData = true;
   }
   // From InitialSpacecraftEpoch to user specified final epoch
   else if (initialEpochA1Mjd == -999.999 && finalEpochA1Mjd != -999.999)
   {
      if (currEpochInDays <= finalEpochA1Mjd)
         writeData = true;
      
      if (currEpochInDays > finalEpochA1Mjd)
         finalEpochReached = true;
   }
   // From user specified initial epoch to FinalSpacecraftEpoch
   else if (initialEpochA1Mjd != -999.999 && finalEpochA1Mjd == -999.999)
   {
      if (currEpochInDays >= initialEpochA1Mjd)
         writeData = true;
      else
         initialEpochReached = false;
   }
   // From user specified initial epoch to user specified final epoch
   else
   {
      // Use tolerance of -1.0e-11 when checking for time to write (GMT-4079 fix)
      //if ((currEpochInDays >= initialEpochA1Mjd) && (currEpochInDays <= finalEpochA1Mjd))
      if (((currEpochInDays - initialEpochA1Mjd) >= -1.0e-11) &&
          (finalEpochA1Mjd - currEpochInDays) >= -1.0e-11)
         writeData = true;
      
      if (currEpochInDays > finalEpochA1Mjd)
         finalEpochReached = true;
   }
   
   // Buffer a few more (interpolationOrder + 1) data after finalEpochReached
   //if (finalEpochReached) //LOJ: do not write data if final epoch already written
   if (finalEpochReached && !finalEpochProcessed)
   {
      writeData = true;
      afterFinalEpochCount++;
      if (afterFinalEpochCount > (interpolationOrder + 1))
         writeData = false;
   }
   
   #ifdef DEBUG_EPHEMFILE_WRITE
   MessageInterface::ShowMessage
      ("EphemerisFile::CheckInitialAndFinalEpoch() returning writeData=%d, "
       "finalEpochReached=%d, afterFinalEpochCount=%d, initial epoch %s\n", writeData,
       finalEpochReached, afterFinalEpochCount, initialEpochReached ? "reached" : "not reached");
   #endif
   
   return writeData;
}


//------------------------------------------------------------------------------
// void HandleSpkOrbitData(bool writeData, bool timeToWrite)
//------------------------------------------------------------------------------
void EphemerisFile::HandleSpkOrbitData(bool writeData, bool timeToWrite)
{
   #ifdef DEBUG_EPHEMFILE_SPICE
   MessageInterface::ShowMessage
      ("EphemerisFile::HandleSpkOrbitData() entered, writeData=%d, timeToWrite=%d, "
       "currEpochInDays = %.13lf, \n   firstTimeWriting=%d, writingNewSegment=%d\n",
       writeData, timeToWrite, currEpochInDays, firstTimeWriting, writingNewSegment);
   #endif
   
   if (writeData)
   {
      bool bufferData = false;
      
      if ((a1MjdArray.empty()) ||
          (!a1MjdArray.empty() && currEpochInDays > a1MjdArray.back()->GetReal()))
         bufferData = true;
      
      if (bufferData)
      {
         Real outState[6];
         // Convert if necessary
         if (!writeDataInDataCS)
            ConvertState(currEpochInDays, currState, outState);
         else
            for (unsigned int ii = 0; ii < 6; ii++)
              outState[ii] = currState[ii];

//         BufferOrbitData(currEpochInDays, currState);
         BufferOrbitData(currEpochInDays, outState);
         
         #ifdef DEBUG_EPHEMFILE_SPICE
         DebugWriteOrbit("In HandleSpkOrbitData:", currEpochInDays, currState, true, true);
         #endif
      }
      
      // Set flags (GMT-3745 SPK writing fix)
      if (firstTimeWriting)
         firstTimeWriting = false;
      
      if (writingNewSegment)
         writingNewSegment = false;
   }
   
   #ifdef DEBUG_EPHEMFILE_SPICE
   MessageInterface::ShowMessage
      ("EphemerisFile::HandleSpkOrbitData() leaving, firstTimeWriting=%d, writingNewSegment=%d\n",
       firstTimeWriting, writingNewSegment);
   #endif
}


//------------------------------------------------------------------------------
// void HandleCcsdsOrbitData(bool writeData, bool timeToWrite)
//------------------------------------------------------------------------------
void EphemerisFile::HandleCcsdsOrbitData(bool writeData, bool timeToWrite)
{
   #ifdef DEBUG_EPHEMFILE_CCSDS
   MessageInterface::ShowMessage
      ("EphemerisFile::HandleCcsdsOrbitData() entered, writeData=%d, timeToWrite=%d\n",
       writeData, timeToWrite);
   #endif
   
   // LagrangeInterpolator's maximum buffer size is set to 80 which can hold
   // 80 min of data assuming average of 60 sec data interveval.
   // Check at least 10 min interval for large step size, since interpolater
   // buffer size is limited
   if (!timeToWrite)
   {
      if ((currEpochInSecs - prevProcTime) > 600.0)
      {
         #ifdef DEBUG_EPHEMFILE_CCSDS
         MessageInterface::ShowMessage
            ("   ==> 10 min interval is over, so setting timeToWrite to true\n");
         #endif
         
         timeToWrite = true;
      }
   }
   
   #ifdef DEBUG_EPHEMFILE_CCSDS
   MessageInterface::ShowMessage
      ("   timeToWrite=%d, writingNewSegment=%d\n", timeToWrite, writingNewSegment);
   #endif
   
   if (timeToWrite)
      prevProcTime = currEpochInSecs;
   
   //------------------------------------------------------------
   // write data to file
   //------------------------------------------------------------
   // Now actually write data
   if (writeData && timeToWrite)
   {
      if (firstTimeWriting)
         WriteHeader();
      
      if (writingNewSegment)
      {
         //WriteComments("********** NEW SEGMENT **********");
         #ifdef DEBUG_EPHEMFILE_WRITE
         DebugWriteTime
            ("********** WRITING NEW SEGMENT AT currEpochInSecs = ", currEpochInSecs, false, 2);
         #endif
         
         writeMetaDataOption = 1;
         saveMetaDataStart = true;
         WriteCcsdsOrbitDataSegment();
      }
      
      if (fileType == CCSDS_AEM && (firstTimeWriting || writingNewSegment))
         WriteString("DATA_START\n");
      
      if (writeOrbit)
      {
         HandleWriteOrbit();
      }
      else if (writeAttitude)
      {
         WriteAttitude();
      }
      
      if (firstTimeWriting)
         firstTimeWriting = false;
      
      if (writingNewSegment)
         writingNewSegment = false;
   }
   else
   {
      #ifdef DEBUG_EPHEMFILE_CCSDS
      MessageInterface::ShowMessage
         ("   It %s time to write but initial epoch has %s\n", timeToWrite ? "is" : "is not",
          writeData ? "reached" : "not reached");
      #endif
   }
   
   #ifdef DEBUG_EPHEMFILE_CCSDS
   MessageInterface::ShowMessage
      ("EphemerisFile::HandleCcsdsOrbitData() leaving, firstTimeWriting=%d, "
       "writingNewSegment=%d\n", firstTimeWriting, writingNewSegment);
   #endif
}


//------------------------------------------------------------------------------
// void HandleCode500OrbitData(bool writeData, bool timeToWrite)
//------------------------------------------------------------------------------
void EphemerisFile::HandleCode500OrbitData(bool writeData, bool timeToWrite)
{
   #ifdef DEBUG_EPHEMFILE_CODE500
   MessageInterface::ShowMessage
      ("EphemerisFile::HandleCode500OrbitData() entered, writeData=%d, currEpochInDays = %.13lf, \n"
       "firstTimeWriting=%d, writingNewSegment=%d\n", writeData, currEpochInDays, firstTimeWriting,
       writingNewSegment);
   #endif
      
   // LagrangeInterpolator's maximum buffer size is set to 80 which can hold
   // 80 min of data assuming average of 60 sec data interveval.
   // Check at least 10 min interval for large step size, since interpolater
   // buffer size is limited
   if (!timeToWrite)
   {
      if ((currEpochInSecs - prevProcTime) > 600.0)
      {
         #ifdef DEBUG_EPHEMFILE_CODE500
         MessageInterface::ShowMessage
            ("   ==> 10 min interval is over, so setting timeToWrite to true\n");
         #endif
         
         timeToWrite = true;
      }
   }
   
   #ifdef DEBUG_EPHEMFILE_CODE500
   MessageInterface::ShowMessage
      ("   timeToWrite=%d, writingNewSegment=%d\n", timeToWrite, writingNewSegment);
   #endif
   
   if (timeToWrite)
      prevProcTime = currEpochInSecs;
   
   //------------------------------------------------------------
   // write data to file
   //------------------------------------------------------------
   // Now actually write data
   if (writeData && timeToWrite)
   {
      if (firstTimeWriting)
         WriteHeader();
      
      if (writingNewSegment)
      {
         //WriteComments("********** NEW SEGMENT **********");
         #ifdef DEBUG_EPHEMFILE_WRITE
         DebugWriteTime
            ("********** WRITING NEW SEGMENT AT currEpochInSecs = ", currEpochInSecs, false, 2);
         #endif
         
         WriteCode500OrbitDataSegment();
      }
      
      if (writeOrbit)
      {
         HandleWriteOrbit();
      }
      
      if (firstTimeWriting)
         firstTimeWriting = false;
      
      if (writingNewSegment)
         writingNewSegment = false;
   }
   
   #ifdef DEBUG_EPHEMFILE_CODE500
   MessageInterface::ShowMessage
      ("EphemerisFile::HandleCode500OrbitData() leaving, firstTimeWriting=%d, writingNewSegment=%d\n",
       firstTimeWriting, writingNewSegment);
   #endif
}


//------------------------------------------------------------------------------
// void HandleWriteOrbit()
//------------------------------------------------------------------------------
void EphemerisFile::HandleWriteOrbit()
{
   #ifdef DEBUG_EPHEMFILE_WRITE
   MessageInterface::ShowMessage
      ("EphemerisFile::HandleWriteOrbit() entered, useFixedStepSize=%d, "
       "interpolateInitialState=%d, interpolateFinalState=%d\n", useFixedStepSize,
       interpolateInitialState, interpolateFinalState);
   DebugWriteTime("   nextReqEpochInSecs = ", nextReqEpochInSecs);
   #endif
   
   if (useFixedStepSize)
   {
      WriteOrbitAt(nextReqEpochInSecs, currState);
   }
   else if (interpolateInitialState)
   {
      WriteOrbitAt(nextReqEpochInSecs, currState);
      if (nextReqEpochInSecs == (initialEpochA1Mjd * GmatTimeConstants::SECS_PER_DAY))
      {
         #ifdef DEBUG_EPHEMFILE_WRITE
         MessageInterface::ShowMessage
            ("===> State at initial epoch has been written, so turning off "
             "interpolateInitialState flag\n");
         #endif
         interpolateInitialState = false;
         // Reset to write integrator steps
         initialEpochA1Mjd = -999.999;
         nextOutEpochInSecs = -999.999;
      }
   }
   else if (interpolateFinalState)
   {
      if (currEpochInDays < finalEpochA1Mjd)
         WriteOrbit(currEpochInSecs, currState);
      else
         WriteOrbitAt(nextReqEpochInSecs, currState);
   }
   else
   {
      WriteOrbit(currEpochInSecs, currState);
   }
   
   #ifdef DEBUG_EPHEMFILE_WRITE
   MessageInterface::ShowMessage("EphemerisFile::HandleWriteOrbit() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void RestartInterpolation(const std::string &comments = "", bool writeAfterData = true,
//                           bool canFinalize = false, bool ignoreBlankComments = true)
//------------------------------------------------------------------------------
/**
 * Resets interpolator to start new segments of data.
 */
//------------------------------------------------------------------------------
void EphemerisFile::RestartInterpolation(const std::string &comments, bool saveEpochInfo,
                                         bool writeAfterData, bool canFinalize,
                                         bool ignoreBlankComments)
{
   #ifdef DEBUG_EPHEMFILE_RESTART
   MessageInterface::ShowMessage
      ("===== EphemerisFile::RestartInterpolation() entered\n   comments='%s'\n   "
       "saveEpochInfo=%d, writeAfterData=%d, ignoreBlankComments=%d, canFinalize=%d, firstTimeWriting=%d\n",
       comments.c_str(), saveEpochInfo, writeAfterData, ignoreBlankComments, canFinalize, firstTimeWriting);
   #endif
   
   // If no first data has written out yet, just return
   if (firstTimeWriting)
   {
      #ifdef DEBUG_EPHEMFILE_RESTART
      MessageInterface::ShowMessage
         ("EphemerisFile::RestartInterpolation() returning, no first data written out yet\n");
      #endif
      return;
   }
   
   #ifdef DEBUG_EPHEMFILE_RESTART
   MessageInterface::ShowMessage
      ("EphemerisFile::RestartInterpolation() Calling FinishUpWriting(), canFinalize=%d\n",
       canFinalize);
   #endif
   
   // Write data for the rest of times on waiting
   FinishUpWriting(canFinalize);
   
   // For CCSDS data, comments are written from
   // CcsdsEphemerisFile::WriteRealCcsdsOrbitDataSegment(), so just set comments here
   writeCommentAfterData = writeAfterData;
   currComments = comments;
   
   // If not using DataFile and writing text ehem file, write comments here
   #if !defined(__USE_DATAFILE__) || defined(DEBUG_EPHEMFILE_TEXT)
   WriteComments(comments, ignoreBlankComments);
   #endif
   
   if (spkWriter != NULL)
   {
      Integer mnSz   = spkWriter->GetMinNumberOfStates();
      Integer numPts = (Integer) a1MjdArray.size();
      if (!generateInBackground || (numPts >= mnSz))
      {
         if (!writeAfterData)
            WriteComments(comments, ignoreBlankComments);

         WriteSpkOrbitDataSegment();

         if (writeAfterData)
            WriteComments(comments, ignoreBlankComments);

         insufficientSPKData = false; // there was enough data
         currComments = "";
      }
      else if (generateInBackground && (numPts > 1))
      {
         #ifdef DEBUG_EPHEMFILE_SPICE
         MessageInterface::ShowMessage("NOT WRITING SPK data - only %d points available!!!\n",
               numPts);
         #endif
         insufficientSPKData = true; // data is available, but has not been written yet
      }
   }
   else if (code500EphemFile != NULL)
   {
      WriteCode500OrbitDataSegment();
   }
   
   // Initialize data
   InitializeData(saveEpochInfo);
   
   #ifdef DEBUG_EPHEMFILE_RESTART
   MessageInterface::ShowMessage
      ("===== EphemerisFile::RestartInterpolation() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// bool IsTimeToWrite(Real epochInSecs, const Real state[6])
//------------------------------------------------------------------------------
/*
 * Determines if it is time to write to ephemeris file based on the step size.
 *
 * @param epochInSecs Epoch in seconds
 */
//------------------------------------------------------------------------------
bool EphemerisFile::IsTimeToWrite(Real epochInSecs, const Real state[6])
{
   #ifdef DEBUG_EPHEMFILE_TIME
   MessageInterface::ShowMessage
      ("EphemerisFile::IsTimeToWrite() entered, writingNewSegment=%d, state[0]=%.15f\n",
       writingNewSegment, state[0]);
   DebugWriteTime("   prevEpochInSecs = ", prevEpochInSecs);
   DebugWriteTime("       epochInSecs = ", epochInSecs);
   MessageInterface::ShowMessage
      ("   useFixedStepSize=%d, interpolateInitialState=%d, interpolateFinalState=%d, "
       "finalEpochProcessed=%d\n", useFixedStepSize, interpolateInitialState, interpolateFinalState,
       finalEpochProcessed);
   #endif
   
   if (finalEpochProcessed)
   {
      #ifdef DEBUG_EPHEMFILE_TIME
      MessageInterface::ShowMessage
         ("EphemerisFile::IsTimeToWrite() returning false, the final epoch has been processed\n");
      #endif
      return false;
   }
   
   bool retval = true;
   // If writing at specified interval step, do checking
   if (useFixedStepSize || interpolateInitialState || interpolateFinalState)
   {
      // Add data points
      if (writeOrbit)
      {
         #ifdef DEBUG_EPHEMFILE_TIME
         MessageInterface::ShowMessage("   Checking if new data points need to be added\n");
         DebugWriteTime("       epochInSecs = ", epochInSecs);
         DebugWriteTime("   prevEpochInSecs = ", prevEpochInSecs);
         #endif
         
         // If current epoch is greater than previous epoch, add data to interpolator
         if ((epochInSecs > prevEpochInSecs) ||
             ((epochInSecs <= prevEpochInSecs) && currPropDirection == -1.0))
         {
            #ifdef DEBUG_EPHEMFILE_TIME
            DebugWriteTime("   ===== Adding to interpolator, epochInSecs = ", epochInSecs);
            #endif
            #ifdef DEBUG_INTERPOLATOR_TRACE
            MessageInterface::ShowMessage
               ("   ===> IsTimeToWrite() calling interpolator->AddPoint(epochInSecs, state)\n");
            #endif
            interpolator->AddPoint(epochInSecs, state);
            prevEpochInSecs = epochInSecs;
         }
         else
         {
            #ifdef DEBUG_EPHEMFILE_TIME
            MessageInterface::ShowMessage
               ("   ========== skipping epoch <= prevEpochInSecs epochInSecs=%.15f, prevEpochInSecs=%.15f\n",
                epochInSecs, prevEpochInSecs);
            #endif
         }
      }
      else if (writeAttitude)
      {
         #ifdef DEBUG_EPHEMFILE_TIME
         MessageInterface::ShowMessage("Adding points to interpolator is todo work\n");
         #endif
      }
      
      #ifdef DEBUG_EPHEMFILE_TIME
      MessageInterface::ShowMessage
         ("   ===== processingLargeStep=%d, finalEpochReached=%d, handleFinalEpoch=%d, "
          "waitCount=%d\n", processingLargeStep, finalEpochReached, handleFinalEpoch, waitCount);
      #endif
      
      // If step size is to large, we may miss the data points since interpolator
      // buffer size is limited. So do additional process here.
      // If user defined final epoch is reached, process data (for GMT-3342 fix)
      if (processingLargeStep  || (finalEpochReached && handleFinalEpoch))
      {
         waitCount++;
         
         if (waitCount >= interpolationOrder / 2)
         {
            #ifdef DEBUG_EPHEMFILE_TIME
            MessageInterface::ShowMessage
               ("   waitCount=%d, Calling ProcessEpochsOnWaiting()\n", waitCount);
            #endif
            
            ProcessEpochsOnWaiting(false);
            waitCount = 0;
            
            if (processingLargeStep)
               processingLargeStep = false;
            
            if (finalEpochReached && handleFinalEpoch)
               handleFinalEpoch = false;
         }
      }
      
      #ifdef DEBUG_EPHEMFILE_TIME
      MessageInterface::ShowMessage("   Computing next output time\n");
      #endif
      
      // compute next output time
      if (writingNewSegment)
      {
         nextOutEpochInSecs = epochInSecs;
         #ifdef DEBUG_EPHEMFILE_TIME
         DebugWriteTime("   nextOutEpochInSecs = ", epochInSecs);
         #endif
         // If user initial epoch is defined, set it to nextOutEpochInSecs
         //if (initialEpochA1Mjd != -999.999)
         // If user initial epoch is defined and it is first time writng a segment
         // set it to nextOutEpochInSecs (LOJ: 2014.04.15)
         if (initialEpochA1Mjd != -999.999 && firstTimeWriting)
         {
            nextOutEpochInSecs = initialEpochA1Mjd * GmatTimeConstants::SECS_PER_DAY;
            #ifdef DEBUG_EPHEMFILE_TIME
            MessageInterface::ShowMessage
               ("   Using user defined initial epoch and first time writing a segment\n");
            DebugWriteTime("   nextOutEpochInSecs = ", epochInSecs);
            #endif
         }
         
         nextReqEpochInSecs = nextOutEpochInSecs;
         #ifdef DEBUG_EPHEMFILE_TIME
         MessageInterface::ShowMessage
            ("   ===== Writing new segment, setting new nextOutEpochInSecs and nextReqEpochInSecs to %.15f, %s\n",
             nextReqEpochInSecs, ToUtcGregorian(nextReqEpochInSecs, false, 2).c_str());
         #endif
         retval = true;
      }
      else
      {
         #ifdef DEBUG_EPHEMFILE_TIME
         DebugWriteTime("          epochInSecs = ", epochInSecs);
         DebugWriteTime("   nextOutEpochInSecs = ", nextOutEpochInSecs);
         MessageInterface::ShowMessage("   handleFinalEpoch=%d\n", handleFinalEpoch);
         #endif
         
         if (useFixedStepSize)
         {
            // If final user defined epoch needs to be handled and received the next
            // output epoch, then add a new next epoch. We don't need to keep adding
            // the new next output epoch when final epoch has already written out. (LOJ: 2012.12.18)
            if (handleFinalEpoch && (((epochInSecs >= nextOutEpochInSecs) && currPropDirection == 1.0) ||
                                     ((epochInSecs <= nextOutEpochInSecs) && currPropDirection == -1.0)))
            {
               nextOutEpochInSecs = nextOutEpochInSecs + (stepSizeInSecs * currPropDirection);
               AddNextEpochToWrite(nextOutEpochInSecs, "   ===== Adding nextOutEpochInSecs to epochsOnWaiting, ");
               
               // Handle step size less than integrator step size
               Real nextOut = nextOutEpochInSecs;
               while (((nextOut <= epochInSecs) && currPropDirection == 1.0) ||
                      ((nextOut >= epochInSecs) && currPropDirection == -1.0))
               {
                  // Compute new output time
                  nextOut = nextOut + (stepSizeInSecs * currPropDirection);
                  AddNextEpochToWrite(nextOut, "   ===== Adding nextOut to epochsOnWaiting, ");
               }
               retval = true;
            }
            else
            {
               retval = false;
            }
         }
         else if (interpolateFinalState)
         {
            if (epochInSecs >= finalEpochA1Mjd * GmatTimeConstants::SECS_PER_DAY)
            {
               nextOutEpochInSecs = finalEpochA1Mjd * GmatTimeConstants::SECS_PER_DAY;
               nextReqEpochInSecs = nextOutEpochInSecs;
               AddNextEpochToWrite(nextOutEpochInSecs, "   ===== Adding nextOutEpochInSecs to epochsOnWaiting, ");
               #ifdef DEBUG_EPHEMFILE_TIME
               MessageInterface::ShowMessage
                  ("   ===== current epoch passed final epoch, so setting %f to nextOutEpochInSecs\n",
                   nextOutEpochInSecs);
               #endif
            }
         }
      }
   }
   
   #ifdef DEBUG_EPHEMFILE_TIME
   MessageInterface::ShowMessage
      ("EphemerisFile::IsTimeToWrite() returning %d\n", retval);
   DebugWriteTime("   nextOutEpochInSecs = ", nextOutEpochInSecs);
   #endif
   return retval;
}


//------------------------------------------------------------------------------
// void WriteOrbit(Real reqEpochInSecs, const Real state[6])
//------------------------------------------------------------------------------
/**
 * Writes spacecraft orbit data to a ephemeris file.
 *
 * @param reqEpochInSecs Requested epoch to write in seconds 
 * @param state State to write 
 */
//------------------------------------------------------------------------------
void EphemerisFile::WriteOrbit(Real reqEpochInSecs, const Real state[6])
{
   #ifdef DEBUG_EPHEMFILE_WRITE
   MessageInterface::ShowMessage("EphemerisFile::WriteOrbit() entered\n");
   DebugWriteTime("   currEpochInSecs = ", currEpochInSecs);
   DebugWriteTime("    reqEpochInSecs = ", reqEpochInSecs);
   MessageInterface::ShowMessage
      ("   currState[0:2]=%.15f, %.15f, %.15f\n       state[0:2]=%.15f, %.15f, %.15f\n",
       currState[0], currState[1], currState[2], state[0], state[1], state[2]);
   #endif
   
   Real stateToWrite[6];
   for (int i=0; i<6; i++)
      stateToWrite[i] = state[i];
   
   Real outEpochInSecs = reqEpochInSecs;

   if (useFixedStepSize)
   {
      // If the difference between current epoch and requested epoch is less 
      // than 1.0e-6, write out current state (LOJ: 2010.09.30)
      if (GmatMathUtil::Abs(currEpochInSecs - reqEpochInSecs) < 1.0e-6)
      {
         outEpochInSecs = currEpochInSecs;
         nextOutEpochInSecs = outEpochInSecs + (stepSizeInSecs * currPropDirection);
         
         #ifdef DEBUG_EPHEMFILE_WRITE
         MessageInterface::ShowMessage
            ("***** The difference between current epoch and requested epoch is less "
             "than 1.0e-6, so writing current state\n");
         DebugWriteTime("   outEpochInSecs = ", outEpochInSecs);
         DebugWriteTime("     nextOutEpochInSecs = ", nextOutEpochInSecs);
         DebugWriteOrbit("   =====> current state ", currEpochInSecs, currState,
                         false, true);
         #endif
         
         for (int i=0; i<6; i++)
            stateToWrite[i] = currState[i];
         
         // Erase requested epoch from the epochs on waiting list if found (LOJ: 2010.02.28)
         RemoveEpochAlreadyWritten(reqEpochInSecs, "   =====> WriteOrbit() now erasing ");
         AddNextEpochToWrite(nextOutEpochInSecs, "   =====> Adding nextOutEpochInSecs to epochsOnWaiting");
      }
   }
   
   WriteOrbitData(outEpochInSecs, stateToWrite);
   
   #ifdef DEBUG_EPHEMFILE_WRITE
   DebugWriteTime("   Setting lastEpochWrote to outEpochInsecs ", outEpochInSecs);
   #endif
   
   lastEpochWrote = outEpochInSecs;
   
   // If user defined final epoch has been written out, finalize
   if (finalEpoch != "FinalSpacecraftEpoch" &&
       (lastEpochWrote >= (finalEpochA1Mjd * GmatTimeConstants::SECS_PER_DAY)))
   {
      #ifdef DEBUG_EPHEMFILE_WRITE
      MessageInterface::ShowMessage("==========> Completed writing the final epoch\n");
      #endif
      finalEpochProcessed = true;
   }
   
   #ifdef DEBUG_EPHEMFILE_WRITE
   MessageInterface::ShowMessage
      ("EphemerisFile::WriteOrbit() leaving, lastEpochWrote=%.15f, %s\n", lastEpochWrote,
       ToUtcGregorian(lastEpochWrote).c_str());
   #endif
}


//------------------------------------------------------------------------------
// void WriteOrbitAt(Real reqEpochInSecs, const Real state[6])
//------------------------------------------------------------------------------
/**
 * Writes spacecraft orbit data to a ephemeris file at requested epoch
 *
 * @param reqEpochInSecs Requested epoch to write state in seconds
 * @param state State to write 
 */
//------------------------------------------------------------------------------
void EphemerisFile::WriteOrbitAt(Real reqEpochInSecs, const Real state[6])
{
   #ifdef DEBUG_EPHEMFILE_ORBIT
   MessageInterface::ShowMessage
      ("EphemerisFile::WriteOrbitAt() entered, writingNewSegment=%d, reqEpochInSecs=%.15f, "
       "%s, state[0]=%.15f\n", writingNewSegment, reqEpochInSecs,
       ToUtcGregorian(reqEpochInSecs).c_str(), state[0]);
   #endif
   
   if (writingNewSegment)
   {
      WriteOrbit(reqEpochInSecs, state);
   }
   else
   {
      // Process epochs on waiting
      ProcessEpochsOnWaiting(false);
   }
   
   #ifdef DEBUG_EPHEMFILE_ORBIT
   MessageInterface::ShowMessage("EphemerisFile::WriteOrbitAt() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void GetAttitude()
//------------------------------------------------------------------------------
void EphemerisFile::GetAttitude()
{
   // Get spacecraft attitude in direction cosine matrix
   attEpoch = spacecraft->GetEpoch();
   Rmatrix33 dcm = spacecraft->GetAttitude(attEpoch);
   Rvector quat = AttitudeConversionUtility::ToQuaternion(dcm);
   for (int i = 0; i < 4; i++)
      attQuat[i] = quat[i];
}


//------------------------------------------------------------------------------
// void WriteAttitude()
//------------------------------------------------------------------------------
void EphemerisFile::WriteAttitude()
{
   GetAttitude();
   
   char strBuff[200];
   sprintf(strBuff, "%16.10f  %19.15f  %19.15f  %19.15f  %19.15f\n",
           attEpoch, attQuat[0], attQuat[1], attQuat[2], attQuat[3]);
   dstream << strBuff;
}


//------------------------------------------------------------------------------
// void FinishUpWriting(bool canFinalize = true)
//------------------------------------------------------------------------------
/*
 * Finishes up writing data at epochs on waiting.
 *
 * @param  canFinalize  If this flag is true it will process epochs on waiting
 *                      and write or overwrite metadata depends on the metadata
 *                      write option [true]
 */
//------------------------------------------------------------------------------
void EphemerisFile::FinishUpWriting(bool canFinalize)
{
   #ifdef DEBUG_EPHEMFILE_FINISH
   MessageInterface::ShowMessage
      ("EphemerisFile::FinishUpWriting() '%s' entered, canFinalize=%d, isFinalized=%d, "
       "firstTimeWriting=%d\n   interpolatorStatus=%d, continuousSegment=%d, isEndOfRun=%d\n",
       GetName().c_str(), canFinalize, isFinalized, firstTimeWriting, interpolatorStatus,
       continuousSegment, isEndOfRun);
   DebugWriteTime("    lastEpochWrote = ", lastEpochWrote);
   DebugWriteTime("   currEpochInSecs = ", currEpochInSecs);
   #ifdef DEBUG_EPHEMFILE_FINISH_MORE
   DebugWriteEpochsOnWaiting("   ");
   #endif
   MessageInterface::ShowMessage
      ("   There is(are) %d data point(s) in the buffer, spkWriter=<%p>\n", a1MjdArray.size(),
       spkWriter);
   #endif
   
   if (!isFinalized)
   {
      #ifdef DEBUG_EPHEMFILE_FINISH
      MessageInterface::ShowMessage
         ("   It is not finalized yet, so trying to write the remainder of data\n");
      #endif
      
      if (fileType == CCSDS_OEM || fileType == CCSDS_AEM)
      {
         FinishUpWritingCCSDS(canFinalize);
      }
      else if (fileType == SPK_ORBIT)
      {
         FinishUpWritingSPK(canFinalize);
      }
      else if (fileType == CODE500_EPHEM)
      {
         FinishUpWritingCode500(canFinalize);
      }
      
      if (canFinalize)
      {
         if (isEndOfRun)
         {
            // Close ephemeris file (GMT-4049 fix)
            CloseEphemerisFile();
            
            // Check for user defined final epoch (GMT-4108 fix)
            if (finalEpochA1Mjd != -999.999)
            {
               if (currEpochInDays < finalEpochA1Mjd)
               {
                  MessageInterface::ShowMessage
                     ("*** WARNING *** Run ended at %f before the user defined final epoch of %f\n",
                      currEpochInDays, finalEpochA1Mjd);
               }
            }
         }
         
         isFinalized = true;
      }
   }
   
   #ifdef DEBUG_EPHEMFILE_FINISH
   MessageInterface::ShowMessage
      ("EphemerisFile::FinishUpWriting() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void FinishUpWritingCCSDS(bool canFinalize = true)
//------------------------------------------------------------------------------
void EphemerisFile::FinishUpWritingCCSDS(bool canFinalize)
{
   #ifdef DEBUG_EPHEMFILE_FINISH
   MessageInterface::ShowMessage
      ("EphemerisFile::FinishUpWritingCCSDS() entered, canFinalize=%d, interpolatorStatus=%d\n",
       canFinalize, interpolatorStatus);
   #endif
   
   if (interpolator != NULL && useFixedStepSize)
   {
      // Check for not enough data points for interpolation
      #ifdef DEBUG_INTERPOLATOR_FINISH
      MessageInterface::ShowMessage
         ("===> FinishUpWritingCCSDS() checking for not enough data points\n"
          "   metaDataStartStr='%s', currEpochInSecs='%s', canFinalize=%d, interpolatorStatus=%d\n",
          metaDataStartStr.c_str(), ToUtcGregorian(currEpochInSecs).c_str(), canFinalize,
          interpolatorStatus);
      #endif
      
      if (canFinalize && interpolatorStatus == -1)
      {
         #ifdef DEBUG_EPHEMFILE_FINISH
         MessageInterface::ShowMessage
            ("initialEpochA1Mjd=%f, currEpochInDays=%f\n", initialEpochA1Mjd, currEpochInDays);
         MessageInterface::ShowMessage
            ("===> FinishUpWritingCCSDS() checking if user defined initial epoch has not reached yet\n");
         #endif
         
         // Check for user defined initial epoch has reached yet
         if (initialEpochA1Mjd != -999.999 && (currEpochInDays < initialEpochA1Mjd))
         {
            if (a1MjdArray.empty() && !isEndOfRun)
            {
               #ifdef DEBUG_EPHEMFILE_FINISH
               MessageInterface::ShowMessage
                  ("EphemerisFile::FinishUpWritingCCSDS() leaving, no further processing is needed "
                   "current epoch < initial user epoch\n");
               #endif
               return;
            }
         }
         
         isFinalized = true;
         std::string ccsdsMsg, errMsg;
         
         FormatErrorMessage(ccsdsMsg, errMsg);
         
         // Clear last MetaData with COMMENT
         ClearLastCcsdsOemMetaData(ccsdsMsg);
         
         // Throw an exception         
         #ifdef DEBUG_EPHEMFILE_FINISH
         MessageInterface::ShowMessage
            ("EphemerisFile::FinishUpWritingCCSDS() throwing exception, a1MjdArray.size()=%d\n"
             "   errMsg = %s\n", a1MjdArray.size(), errMsg.c_str());
         #endif
         
         throw SubscriberException(errMsg);
      }
      
      // Finish up final data
      ProcessFinalDataOnWaiting();
   }
   
   writeCommentAfterData = false;
   
   //WriteComments("********** FINAL SEGMENT **********");
   #ifdef DEBUG_EPHEMFILE_WRITE
   DebugWriteTime
      ("********** WRITING FINAL SEGMENT AT currEpochInSecs = ", currEpochInSecs, false, 2);
   #endif
   if (canFinalize)
   {
      writeMetaDataOption = 2;
      saveMetaDataStart = true;
      if (continuousSegment)
         saveMetaDataStart = false;
   }
   else
   {
      writeMetaDataOption = 0;
      if (firstTimeMetaData)
         writeMetaDataOption = 2; // Overwrite previous meta data
      saveMetaDataStart = true;
      if (continuousSegment)
         saveMetaDataStart = false;
   }
   
   WriteCcsdsOrbitDataSegment();
   
   #if !defined(__USE_DATAFILE__) || defined(DEBUG_EPHEMFILE_TEXT)
      if (fileType == CCSDS_AEM)
         WriteString("DATA_STOP\n");
   #endif
      
   #ifdef DEBUG_EPHEMFILE_FINISH
   MessageInterface::ShowMessage("EphemerisFile::FinishUpWritingCCSDS() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void FinishUpWritingSPK(bool canFinalize = true)
//------------------------------------------------------------------------------
void EphemerisFile::FinishUpWritingSPK(bool canFinalize)
{
   #ifdef DEBUG_EPHEMFILE_FINISH
   MessageInterface::ShowMessage
      ("EphemerisFile::FinishUpWritingSPK) entered, canFinalize=%d\n", canFinalize);
   #endif
   try 
   {
      if (spkWriter != NULL)
      {
         Integer mnSz   = spkWriter->GetMinNumberOfStates();
         Integer numPts = (Integer) a1MjdArray.size();
         // if we are generating SPK files in the background and there
         // are not enough states for the interpolation, we DO NOT
         // want to try to write and trigger the SPICE error;
         // for user-specified SPK files, we DO want to present
         // errors to the user.
         if (!generateInBackground || (numPts >= mnSz))
         {
            #ifdef DEBUG_EPHEMFILE_SPICE
            MessageInterface::ShowMessage("FinishUpWritingSPK::about to write SPK orbit data segment\n");
            #endif

            // Write a segment and delete data array pointers
            WriteSpkOrbitDataSegment();

            insufficientSPKData = false;
            #ifdef DEBUG_EPHEMFILE_SPICE
            MessageInterface::ShowMessage("   DONE writing SPK orbit data segment\n");
            #endif
         }
         // background SPKs need to know if there was data unwritten
         // will have 1 point from the last segment in the beginning of this
         // set of data
         else if (generateInBackground && (numPts > 1))
         {
            #ifdef DEBUG_EPHEMFILE_SPICE
            MessageInterface::ShowMessage("NOT WRITING SPK data - only %d points available!!!\n",
                  numPts);
            #endif
            insufficientSPKData = true; // data is available, but has not been written yet
         }
//      }
//         WriteSpkOrbitDataSegment();
      }
      else
      {
         #ifdef __USE_SPICE__
         if (a1MjdArray.size() > 0)
         {
            throw SubscriberException
               ("*** INTERNAL ERROR *** SPK Writer is NULL in "
                "EphemerisFile::FinishUpWritingSPK()\n");
         }
         #endif
      }
   }
   catch (BaseException &ex)
   {
      // Catch and ignore exceptions thrown from util, since we manage them later
      #ifdef DEBUG_EPHEMFILE_WRITE
      MessageInterface::ShowMessage
         ("Caught the exception %s\n", ex.GetFullMessage().c_str());
      #endif
   }
   
   #ifdef DEBUG_EPHEMFILE_FINISH
   MessageInterface::ShowMessage("EphemerisFile::FinishUpWritingSPK() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void FinishUpWritingCode500(bool canFinalize = true)
//------------------------------------------------------------------------------
void EphemerisFile::FinishUpWritingCode500(bool canFinalize)
{
   #ifdef DEBUG_EPHEMFILE_FINISH
   MessageInterface::ShowMessage
      ("EphemerisFile::FinishUpWritingCode500() entered, canFinalize=%d\n", canFinalize);
   #endif
   
   if (interpolator != NULL)
   {
      #ifdef DEBUG_INTERPOLATOR_TRACE
      MessageInterface::ShowMessage
         ("===> FinishUpWritingCode500() checking for not enough data points\n"
          "   metaDataStartStr='%s', currEpochInSecs='%s'\n", metaDataStartStr.c_str(),
          ToUtcGregorian(currEpochInSecs).c_str());
      #endif
      
      // First check for not enough data points for interpolation
      if (canFinalize && interpolatorStatus == -1)
      {
         isFinalized = true;
         std::string ephemMsg, errMsg;
         FormatErrorMessage(ephemMsg, errMsg);
         throw SubscriberException(errMsg);
      }
      
      // Process final data on waiting
      ProcessFinalDataOnWaiting();
   }
   
   // Write final data
   if (code500EphemFile != NULL)
   {
      WriteCode500OrbitDataSegment(canFinalize);
      if (canFinalize)
         FinalizeCode500Ephemeris();
   }
   else
   {
      if (a1MjdArray.size() > 0)
      {
         throw SubscriberException
            ("*** INTERNAL ERROR *** Code500EphemFile is NULL in "
             "EphemerisFile::FinishUpWritingCode500()\n");
      }
   }
   
   #ifdef DEBUG_EPHEMFILE_FINISH
   MessageInterface::ShowMessage("EphemerisFile::FinishUpWritingCode500() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void ProcessFinalDataOnWaiting(bool canFinalize = true)
//------------------------------------------------------------------------------
void EphemerisFile::ProcessFinalDataOnWaiting(bool canFinalize)
{
   #ifdef DEBUG_INTERPOLATOR_TRACE
   MessageInterface::ShowMessage
      ("EphemerisFile::ProcessFinalDataOnWaiting() entered, canFinalize=%d\n",
       canFinalize);
   MessageInterface::ShowMessage
      ("===> ProcessFinalDataOnWaiting() calling interpolator->SetForceInterpolation(true)\n");
   #endif
   interpolator->SetForceInterpolation(true);
   ProcessEpochsOnWaiting(true, !canFinalize);
   #ifdef DEBUG_INTERPOLATOR_TRACE
   MessageInterface::ShowMessage
      ("===> ProcessFinalDataOnWaiting() calling interpolator->SetForceInterpolation(false)\n");
   #endif
   interpolator->SetForceInterpolation(false);
   
   // When running more than 5 days or so, the last epoch to process is a few
   // milliseconds after the last epoch received, so the interpolator flags
   // as epoch after the last buffered epoch, so handle last data point here.
   // If there is 1 epoch left and the difference between the current epoch
   // is less than 1.0e-6 then use the current epoch
   if (epochsOnWaiting.size() == 1)
   {
      Real lastEpoch = epochsOnWaiting.back();
      if (GmatMathUtil::Abs(lastEpoch - currEpochInSecs) < 1.0e-6)
      {
         #ifdef DEBUG_EPHEMFILE_TIME
         DebugWriteTime
            ("   ===== Removing last epoch and adding currEpochInSecs to "
             "epochsOnWaiting, currEpochInSecs = ", currEpochInSecs);
         #endif
         epochsOnWaiting.pop_back();
         epochsOnWaiting.push_back(currEpochInSecs);
         #ifdef DEBUG_INTERPOLATOR_TRACE
         MessageInterface::ShowMessage
            ("===> ProcessFinalDataOnWaiting() calling interpolator->SetForceInterpolation(true)\n");
         #endif
         interpolator->SetForceInterpolation(true);
         ProcessEpochsOnWaiting(true);
         #ifdef DEBUG_INTERPOLATOR_TRACE
         MessageInterface::ShowMessage
            ("===> ProcessFinalDataOnWaiting() calling interpolator->SetForceInterpolation(false)\n");
         #endif
         interpolator->SetForceInterpolation(false);
      }
   }
   
   // Write last data received if not written yet(Do attitude later)
   if (canFinalize)
   {
      // Write last data received only for CCSDS not for Code500 (GMT-3997 fix)
      if (fileType == CCSDS_OEM && useFixedStepSize)
      {
         #ifdef DEBUG_EPHEMFILE_FINISH
         MessageInterface::ShowMessage("   finalEpochA1Mjd=%f\n", finalEpochA1Mjd);
         #endif
         // If not using user defined final epock, do more checking for the final data
         if (finalEpochA1Mjd == -999.999)
         {
            // Check if current data needs to be written out
            if (currEpochInSecs > lastEpochWrote + 1.0e-6)
            {
               #ifdef DEBUG_EPHEMFILE_FINISH
               MessageInterface::ShowMessage
                  ("   ===> %.15f > %.15f and not using user defined final epoch, so writing final data\n",
                   currEpochInSecs, lastEpochWrote);
               #endif
               WriteOrbit(currEpochInSecs, currState);
            }
         }
         else
         {
            #ifdef DEBUG_EPHEMFILE_FINISH
            MessageInterface::ShowMessage("   ===> process final data on waiting\n");
            #endif
            ProcessEpochsOnWaiting(false, false);
         }
      }
   }
   #ifdef DEBUG_INTERPOLATOR_TRACE
   MessageInterface::ShowMessage
      ("EphemerisFile::ProcessFinalDataOnWaiting() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void ProcessEpochsOnWaiting(bool checkFinalEpoch = false)
//------------------------------------------------------------------------------
/*
 * Process epochs on waiting.
 *
 * @param checkFinalEpoch Set to true if checking for final epoch
 *
 */
//------------------------------------------------------------------------------
void EphemerisFile::ProcessEpochsOnWaiting(bool checkFinalEpoch, bool checkEventEpoch)
{
   #ifdef DEBUG_EPHEMFILE_ORBIT
   MessageInterface::ShowMessage
      ("EphemerisFile::ProcessEpochsOnWaiting() entered, checkFinalEpoch=%d, "
       "checkEventEpoch=%d, finalEpochReached=%d\n   prevPropDirection=%f, currPropDirection=%f, "
       "propIndicator=%d\n", checkFinalEpoch, checkEventEpoch, finalEpochReached, prevPropDirection,
       currPropDirection, propIndicator);
   DebugWriteTime("   currEpochInSecs = ", currEpochInSecs);
   #ifdef DEBUG_EPHEMFILE_ORBIT_MORE
   DebugWriteEpochsOnWaiting("   ");
   #endif
   #endif
   
   Real estimates[6];
   Real reqEpochInSecs = 0.0;
   Integer finishDirection = 1; // forward
   
   if (propIndicator == 2 || propIndicator == 4)
      finishDirection = 2; // backward
   else if (propIndicator == 0 && currPropDirection == -1.0)
      finishDirection = 2; // backward
   
   #ifdef DEBUG_EPHEMFILE_ORBIT
   MessageInterface::ShowMessage("   finishDirection=%d\n", finishDirection);
   #endif
   
   RealArray::iterator iter = epochsOnWaiting.begin();
   while (iter != epochsOnWaiting.end())
   {
      reqEpochInSecs = *iter;
      
      #ifdef DEBUG_EPHEMFILE_ORBIT
      DebugWriteTime("    reqEpochInSecs = ", reqEpochInSecs);
      DebugWriteTime("   currEpochInSecs = ", currEpochInSecs);
      #endif
      
      // Do not write after the final epoch
      if (checkFinalEpoch)
      {
         if ( (((reqEpochInSecs + 1.0e-6) > currEpochInSecs) && finishDirection == 1) ||
              (((reqEpochInSecs + 1.0e-6) < currEpochInSecs) && finishDirection == 2) )
         {
            #ifdef DEBUG_EPHEMFILE_ORBIT
            MessageInterface::ShowMessage
               ("   =====> reqEpochInSecs %.15f %s currEpochInSecs %.15f so exiting while "
                "loop for %s prop\n", reqEpochInSecs, finishDirection == 1 ? ">" : "<",
                currEpochInSecs, finishDirection == 1 ? "forward" : "backward");
            #endif
            
            break;
         }
      }
      
      // We don't want to write epoch that matches event epoch which the beginning epoch
      // of the new segment
      if (checkEventEpoch)
      {
         if ( ((reqEpochInSecs >= eventEpochInSecs) && currPropDirection == 1.0) ||
              ((reqEpochInSecs <= eventEpochInSecs) && currPropDirection == -1.0) )
         {
            #ifdef DEBUG_EPHEMFILE_ORBIT
            MessageInterface::ShowMessage
               ("   =====> reqEpochInSecs %.15f %s eventEpochInSecs %.15f so exiting while "
                "loop for %s prop\n", reqEpochInSecs, currPropDirection == 1.0 ? ">=" : "<=",
                currEpochInSecs, currPropDirection == 1.0 ? "forward" : "backward");
            #endif
            break;
         }
      }
      
      #ifdef DEBUG_EPHEMFILE_ORBIT
      DebugWriteTime("   finalEpochA1Mjd = ", finalEpochA1Mjd, true);
      DebugWriteTime("   Checking to see if epoch is before user defined final epoch, "
                     "reqEpochInSecs = ", reqEpochInSecs);
      #endif
      
      if ((finalEpochA1Mjd != -999.999) && finalEpochReached &&
          (((reqEpochInSecs > finalEpochA1Mjd * GmatTimeConstants::SECS_PER_DAY) && currPropDirection == 1.0) ||
           ((reqEpochInSecs < finalEpochA1Mjd * GmatTimeConstants::SECS_PER_DAY) && currPropDirection == -1.0)) )
      {
         #ifdef DEBUG_EPHEMFILE_ORBIT
         MessageInterface::ShowMessage
            ("   =====> currEpochOnWaiting > User defined final epoch, so exiting while loop\n");
         #endif
         break;
      }
      
      #ifdef DEBUG_EPHEMFILE_ORBIT
      DebugWriteTime("   Checking to see if epoch is feasible reqEpochInSecs = ", reqEpochInSecs);
      #endif
      
      #ifdef DEBUG_INTERPOLATOR_TRACE
      MessageInterface::ShowMessage
         ("===> ProcessEpochsOnWaiting() calling interpolator->IsInterpolationFeasible(reqEpochInSecs)\n");
      #endif
      Integer retval = interpolator->IsInterpolationFeasible(reqEpochInSecs);
      interpolatorStatus = retval;
      
      #ifdef DEBUG_EPHEMFILE_ORBIT
      MessageInterface::ShowMessage
         ("   =====> interpolation feasibility at reqEpochInSecs %.15f is %d\n",
          reqEpochInSecs, retval);
      #endif
      
      if (retval == 1)
      {
         // Now interpolate at epoch
         #ifdef DEBUG_EPHEMFILE_ORBIT
         MessageInterface::ShowMessage
            ("   =====> now try interpolating at epoch %.15f\n", reqEpochInSecs);
         #endif
         #ifdef DEBUG_INTERPOLATOR_TRACE
         MessageInterface::ShowMessage
            ("===> ProcessEpochsOnWaiting() calling interpolator->Interpolate(reqEpochInSecs, estimates)\n");
         #endif
         if (interpolator->Interpolate(reqEpochInSecs, estimates))
         {
            WriteOrbit(reqEpochInSecs, estimates);
            RemoveEpochAlreadyWritten
               (reqEpochInSecs, "   =====> ProcessEpochsOnWaiting() now erasing ");
         }
         else
         {
            // Check if interpolation needs to be forced
            if (initialCount <= interpolationOrder/2)
            {
               initialCount++;
               
               #ifdef DEBUG_EPHEMFILE_ORBIT
               MessageInterface::ShowMessage
                  ("   =====> Forcing to interpolate at epoch %.15f, initialCount=%d\n",
                   reqEpochInSecs, initialCount);
               #endif
               
               // Since time should be in order, force process epochs on waiting.
               // First few request time can not be placed in the middle of the buffer.
               #ifdef DEBUG_INTERPOLATOR_TRACE
               MessageInterface::ShowMessage
                  ("===> ProcessEpochsOnWaiting() calling interpolator->SetForceInterpolation(true)\n");
               #endif
               interpolator->SetForceInterpolation(true);
               ProcessEpochsOnWaiting(false);
               #ifdef DEBUG_INTERPOLATOR_TRACE
               MessageInterface::ShowMessage
                  ("===> ProcessEpochsOnWaiting() calling interpolator->SetForceInterpolation(false)\n");
               #endif
               interpolator->SetForceInterpolation(false);
            }
            else
            {
               #ifdef DEBUG_EPHEMFILE_ORBIT
               MessageInterface::ShowMessage
                  ("   initialCount (%d) <= interpolationOrder/2 + 1 (%d)\n", initialCount,
                   interpolationOrder/2 + 1);
               DebugWriteTime
                  ("   =====> epoch failed to interpolate so exiting while loop, reqEpochInSecs = ",
                   reqEpochInSecs);
               #endif
               break;
            }
         }
      }
      else
      {
         // If epoch is after the last data, collect number of order points
         // and process before epoch becomes out of the first data range
         if (retval ==  -3)
         {
            #ifdef DEBUG_EPHEMFILE_ORBIT
            MessageInterface::ShowMessage("   Setting processingLargeStep to true\n");
            #endif
            processingLargeStep = true;
         }
         
         // @todo Is there more checking needs here?
         #ifdef DEBUG_EPHEMFILE_ORBIT
         DebugWriteTime
            ("   =====> epoch is not feasible so exiting while loop, reqEpochInSecs = ", reqEpochInSecs);
         #endif
         break;
      }
      
      // Note: Don't need iter++, since the data is erased as it is written to a file,
      // otherwise it will skip one data.
   }
   
   #ifdef DEBUG_EPHEMFILE_ORBIT
   MessageInterface::ShowMessage
      ("EphemerisFile::ProcessEpochsOnWaiting() leaving\n");
   DebugWriteEpochsOnWaiting("   ");
   #endif
}


//------------------------------------------------------------------------------
// bool SetEpoch(const std::string &value)
//------------------------------------------------------------------------------
bool EphemerisFile::SetEpoch(Integer id, const std::string &value,
                             const StringArray &allowedValues)
{
   #ifdef DEBUG_EPHEMFILE_SET
   MessageInterface::ShowMessage
      ("EphemerisFile::SetEpoch() entered, id=%d, value='%s', "
       "epochFormat='%s'\n", id, value.c_str(), epochFormat.c_str());
   #endif
   
   try
   {
      TimeConverterUtil::ValidateTimeFormat(epochFormat, value);
   }
   catch (BaseException &)
   {
      if (epochFormat.find("Gregorian") != epochFormat.npos)
      {
         std::string rangeMsg = epochFormat;
         rangeMsg += " [" + DateUtil::EARLIEST_VALID_GREGORIAN;
         rangeMsg += " to " + DateUtil::LATEST_VALID_GREGORIAN;
         rangeMsg += "]";
         HandleError(id, value, allowedValues, " or value in " + rangeMsg);
      }
      else
      {
         std::string rangeMsg = "[" + DateUtil::EARLIEST_VALID_MJD;
         rangeMsg += " <= " + epochFormat;
         rangeMsg += " <= " + DateUtil::LATEST_VALID_MJD + "]";
         HandleError(id, value, allowedValues, " or value in " + rangeMsg);
      }
   }
   
   if (id == INITIAL_EPOCH)
      initialEpoch = value;
   else if (id == FINAL_EPOCH)
      finalEpoch = value;
   
   #ifdef DEBUG_EPHEMFILE_SET
   MessageInterface::ShowMessage
      ("EphemerisFile::SetEpoch() returning true, initialEpoch='%s', finalEpoch='%s'\n",
       initialEpoch.c_str(), finalEpoch.c_str());
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// bool SetStepSize(Integer id, const std::string &value, const StringArray &allowedValues)
//------------------------------------------------------------------------------
/*
 * Sets real value step size.
 *
 * @param value step size value string
 *
 * @exception SubscriberException is thrown if value not converted to real number
 *            or value is negative
 */
//------------------------------------------------------------------------------
bool EphemerisFile::SetStepSize(Integer id, const std::string &value,
                                const StringArray &allowedValues)
{
   #ifdef DEBUG_EPHEMFILE_SET
   MessageInterface::ShowMessage
      ("EphemerisFile::SetStepSize() entered, id=%d, value='%s'\n",
       id, value.c_str());
   #endif
   
   Real rval = -999.999;
   bool isReal = GmatStringUtil::ToReal(value, rval);
   if (!isReal || rval <= 0.0)
   {
      HandleError(id, value, allowedValues, " or Real Number > 0.0");
   }
   
   stepSize = value;
   stepSizeInSecs = rval;
   stepSizeInA1Mjd = stepSizeInSecs / GmatTimeConstants::SECS_PER_DAY;
   
   useFixedStepSize = true;
   createInterpolator = true;
   
   #ifdef DEBUG_EPHEMFILE_SET
   MessageInterface::ShowMessage
      ("EphemerisFile::SetStepSize() leaving, stepSize='%s', stepSizeInA1Mjd=%.15f, "
       "stepSizeInSecs=%.15f\n", stepSize.c_str(), stepSizeInA1Mjd, stepSizeInSecs);
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// void HandleError(Integer id, const std::string &value,
//                  const StringArray &allowedValues, const std::string &additionalMsg)
//------------------------------------------------------------------------------
void EphemerisFile::HandleError(Integer id, const std::string &value,
                                const StringArray &allowedValues,
                                const std::string &additionalMsg)
{
   std::string allowedList = ToString(allowedValues);
   SubscriberException se;
   se.SetDetails(errorMessageFormat.c_str(), value.c_str(),
                 GetParameterText(id).c_str(),
                 (allowedList + additionalMsg).c_str());
   throw se;
}


//------------------------------------------------------------------------------
// std::string ToString(const StringArray &strList)
//------------------------------------------------------------------------------
/**
 * Converts std::string array to std::string separated by comma.
 */
//------------------------------------------------------------------------------
std::string EphemerisFile::ToString(const StringArray &strList)
{
   std::string str = "";
   std::string delimiter = ", ";
   if (strList.size() > 0)
   {
      str = strList[0];
      
      for (unsigned int i=1; i<strList.size(); i++)
         str = str + delimiter + strList[i];
   }
   
   return str;
}


//------------------------------------------------------------------------------
// void WriteString(const std::string &str)
//------------------------------------------------------------------------------
void EphemerisFile::WriteString(const std::string &str)
{
   // For now write it text file
   dstream << str;
   dstream.flush();
}


//------------------------------------------------------------------------------
// void WriteHeader()
//------------------------------------------------------------------------------
void EphemerisFile::WriteHeader()
{
   if (fileType == CCSDS_OEM || fileType == CCSDS_AEM)
      WriteCcsdsHeader();
   else if (fileType == SPK_ORBIT)
      WriteSpkHeader();
}


//------------------------------------------------------------------------------
// void WriteMetaData()
//------------------------------------------------------------------------------
void EphemerisFile::WriteMetaData()
{
   if (fileType == CCSDS_OEM)
      WriteCcsdsOemMetaData();
   else if (fileType == CCSDS_AEM)
      WriteCcsdsAemMetaData();
   else if (fileType == SPK_ORBIT)
      WriteSpkOrbitMetaData();
}


//------------------------------------------------------------------------------
// void WriteComments(const std::string &comments, bool ignoreBlankComments = true,
//                    bool writeKeyword = true)
//------------------------------------------------------------------------------
/**
 * Writes comments to specific file.
 */
//------------------------------------------------------------------------------
void EphemerisFile::WriteComments(const std::string &comments, bool ignoreBlankComments,
                                  bool writeKeyword)
{
   #ifdef DEBUG_EPHEMFILE_COMMENTS
   MessageInterface::ShowMessage
      ("WriteComments() entered, comments='%s', ignoreBlankComments=%d\n",
       comments.c_str(), ignoreBlankComments);
   #endif
   
   if (comments == "" && ignoreBlankComments)
   {
      #ifdef DEBUG_EPHEMFILE_COMMENTS
      MessageInterface::ShowMessage("WriteComments() just leaving\n");
      #endif
      return;
   }
   
   if (fileType == CCSDS_OEM || fileType == CCSDS_AEM)
      WriteCcsdsComments(comments, writeKeyword);
   else if (fileType == SPK_ORBIT)
      WriteSpkComments(comments);
   
   #ifdef DEBUG_EPHEMFILE_COMMENTS
   MessageInterface::ShowMessage("WriteComments() wrote comment and leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void BufferOrbitData(Real epochInDays, const Real state[6])
//------------------------------------------------------------------------------
void EphemerisFile::BufferOrbitData(Real epochInDays, const Real state[6])
{
   #ifdef DEBUG_EPHEMFILE_BUFFER
   MessageInterface::ShowMessage
      ("BufferOrbitData() entered, epochInDays=%.15f, state[0]=%.15f\n", epochInDays,
       state[0]);
   DebugWriteTime("   epochInDays = ", epochInDays, true, 2);
   #endif
   
   // if buffer is full, dump the data
   if (a1MjdArray.size() >= maxSegmentSize)
   {
      if (fileType == CCSDS_OEM)
      {
         writeMetaDataOption = 0;
         saveMetaDataStart = false;
         continuousSegment = true;
         if (firstTimeMetaData)
            saveMetaDataStart = true;
         #ifdef DEBUG_EPHEMFILE_WRITE
         DebugWriteTime("********** WRITING CONTINUOUS SEGMENT AT epochInDays = ", epochInDays, true, 2);
         #endif
         WriteCcsdsOrbitDataSegment();
         firstTimeMetaData = false;
      }
      else if (fileType == SPK_ORBIT)
      {
         // Save last data to become first data of next segment
         A1Mjd *a1mjd  = new A1Mjd(*a1MjdArray.back());
         Rvector6 *rv6 = new Rvector6(*stateArray.back());
         
         // Write a segment and delete data array pointers
         WriteSpkOrbitDataSegment();
         
         // Add saved data to arrays
         a1MjdArray.push_back(a1mjd);
         stateArray.push_back(rv6);
      }
      else if (fileType == CODE500_EPHEM)
      {
         WriteCode500OrbitDataSegment();
      }
   }
   
   // Add new data point
   A1Mjd *a1mjd = new A1Mjd(epochInDays);
   Rvector6 *rv6 = new Rvector6(state);
   a1MjdArray.push_back(a1mjd);
   stateArray.push_back(rv6);
   
   #ifdef DEBUG_EPHEMFILE_BUFFER
   MessageInterface::ShowMessage
      ("BufferOrbitData() leaving, there is(are) %d data point(s)\n", a1MjdArray.size());
   #endif
} // BufferOrbitData()


//------------------------------------------------------------------------------
// void ClearOrbitData()
//------------------------------------------------------------------------------
void EphemerisFile::ClearOrbitData()
{
   #ifdef DEBUG_EPHEMFILE_BUFFER
   MessageInterface::ShowMessage
      ("ClearOrbitData() entered, there is(are) %d data point(s) to clear\n", a1MjdArray.size());
   #endif
   EpochArray::iterator ei;
   for (ei = a1MjdArray.begin(); ei != a1MjdArray.end(); ++ei)
      delete (*ei);
   
   StateArray::iterator si;
   for (si = stateArray.begin(); si != stateArray.end(); ++si)
      delete (*si);
   
   a1MjdArray.clear();
   stateArray.clear();
}


//------------------------------------------------------------------------------
// virtual bool OpenRealCcsdsEphemerisFile()
//------------------------------------------------------------------------------
bool EphemerisFile::OpenRealCcsdsEphemerisFile()
{
   MessageInterface::ShowMessage
      ("**** ERROR **** No implementation found for OpenRealCcsdsEphemerisFile()\n");
   return false;
}


//------------------------------------------------------------------------------
// virtual void WriteRealCcsdsHeader()
//------------------------------------------------------------------------------
void EphemerisFile::WriteRealCcsdsHeader()
{
   #ifdef __USE_DATAFILE__
   MessageInterface::ShowMessage
      ("**** ERROR **** No implementation found for WriteRealCcsdsHeader()\n");
   #endif
}


//------------------------------------------------------------------------------
// virtual void WriteRealCcsdsOrbitDataSegment()
//------------------------------------------------------------------------------
void EphemerisFile::WriteRealCcsdsOrbitDataSegment()
{
   #ifdef __USE_DATAFILE__
   MessageInterface::ShowMessage
      ("**** ERROR **** No implementation found for WriteRealCcsdsOrbitDataSegment()\n");
   #elif !defined(__USE_DATAFILE__) || defined(DEBUG_EPHEMFILE_TEXT)
   // Since array is deleted from CcsdsEphemerisFile::WriteRealCcsdsOrbitDataSegment()
   // delete orbit data here
   ClearOrbitData();
   #endif
}


//------------------------------------------------------------------------------
// virtual void WriteRealCcsdsOemMetaData()
//------------------------------------------------------------------------------
void EphemerisFile::WriteRealCcsdsOemMetaData()
{
   #ifdef __USE_DATAFILE__
   MessageInterface::ShowMessage
      ("**** ERROR **** No implementation found for WriteRealCcsdsOemMetaData()\n");
   #endif
}


//------------------------------------------------------------------------------
// virtual void WriteRealCcsdsAemMetaData()
//------------------------------------------------------------------------------
void EphemerisFile::WriteRealCcsdsAemMetaData()
{
   #ifdef __USE_DATAFILE__
   MessageInterface::ShowMessage
      ("**** ERROR **** No implementation found for WriteRealCcsdsAemMetaData()\n");
   #endif
}


//------------------------------------------------------------------------------
// virtual void WriteRealCcsdsAemData(Real reqEpochInSecs, const Real quat[4])
//------------------------------------------------------------------------------
void EphemerisFile::WriteRealCcsdsAemData(Real reqEpochInSecs, const Real quat[4])
{
   #ifdef __USE_DATAFILE__
   MessageInterface::ShowMessage
      ("**** ERROR **** No implementation found for WriteRealCcsdsAemData()\n");
   #endif
}


//------------------------------------------------------------------------------
// virtual void WriteRealCcsdsComments(const std::string &comments)
//------------------------------------------------------------------------------
void EphemerisFile::WriteRealCcsdsComments(const std::string &comments)
{
   #ifdef __USE_DATAFILE__
   MessageInterface::ShowMessage
      ("**** ERROR **** No implementation found for WriteRealCcsdsComments()\n");
   #endif
}


//------------------------------------------------------------------------------
// bool OpenCcsdsEphemerisFile()
//------------------------------------------------------------------------------
bool EphemerisFile::OpenCcsdsEphemerisFile()
{
   #ifdef DEBUG_EPHEMFILE_OPEN
   MessageInterface::ShowMessage
      //("CcsdsEphemerisFile::EphemerisFile() entered, fileName = %s\n", fileName.c_str());
      ("CcsdsEphemerisFile::EphemerisFile() entered, fileName = %s\n", fullPathFileName.c_str());
   #endif
   
   bool retval = false;
   
   #ifdef __USE_DATAFILE__
   // Open CCSDS output file
   retval = OpenRealCcsdsEphemerisFile();
   #else
   retval = true;
   #endif
   
   #ifdef DEBUG_EPHEMFILE_OPEN
   MessageInterface::ShowMessage
      ("CcsdsEphemerisFile::OpenCcsdsEphemerisFile() returning %d\n", retval);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// void ClearLastCcsdsOemMetaData(const std::string &comments)
//------------------------------------------------------------------------------
void EphemerisFile::ClearLastCcsdsOemMetaData(const std::string &comments)
{
   #ifdef DEBUG_EPHEMFILE_METADATA
   MessageInterface::ShowMessage
      ("EphemerisFile::ClearLastCcsdsOemMetaData() entered\n   comments='%s', "
       "firstTimeMetaData=%d\n", comments.c_str(), firstTimeMetaData);
   #endif
   
   //=================================================================
   #if !defined(__USE_DATAFILE__) || defined(DEBUG_EPHEMFILE_TEXT)
   //=================================================================
   
   // Go to beginning of the last meta data position
   #ifdef DEBUG_EPHEMFILE_METADATA
   MessageInterface::ShowMessage
      ("===> metaDataBegPosition=%ld, metaDataEndPosition=%ld\n",
       (long)metaDataBegPosition, (long)metaDataEndPosition);
   #endif
   dstream.seekp(metaDataBegPosition, std::ios_base::beg);
   
   std::stringstream ss("");
   ss << std::endl;
   
   if (comments != "")
      WriteComments(comments);
   
   // Clear with blanks
   long length = (long)metaDataEndPosition - (long)metaDataBegPosition;
   for (long i = 0; i < length; i++)
      ss << " ";
   
   #ifdef DEBUG_EPHEMFILE_METADATA
   MessageInterface::ShowMessage("   ==> Writing following META data:\n%s\n", ss.str().c_str());
   #endif
   
   WriteString(ss.str());
   
   //=================================================================
   #endif
   //=================================================================
   
   // Is there a way to clear last meta data using CcsdsEphemerisFile?
   
   #ifdef DEBUG_EPHEMFILE_METADATA
   MessageInterface::ShowMessage
      ("EphemerisFile::ClearLastCcsdsOemMetaData() leaving, firstTimeMetaData=%d\n",
       firstTimeMetaData);
   #endif
}


//------------------------------------------------------------------------------
// void WriteCcsdsHeader()
//------------------------------------------------------------------------------
void EphemerisFile::WriteCcsdsHeader()
{
   #ifdef DEBUG_EPHEMFILE_HEADER
   MessageInterface::ShowMessage("EphemerisFile::WriteCcsdsHeader() entered\n");
   #endif
   
   #if !defined(__USE_DATAFILE__) || defined(DEBUG_EPHEMFILE_TEXT)
   std::string creationTime = GmatTimeUtil::FormatCurrentTime(2);
   std::string originator = "GMAT USER";
   
   std::stringstream ss("");
   
   if (fileType == CCSDS_OEM)
      ss << "CCSDS_OEM_VERS = 1.0" << std::endl;
   else
      ss << "CCSDS_AEM_VERS = 1.0" << std::endl;
   
   ss << "CREATION_DATE  = " << creationTime << std::endl;
   ss << "ORIGINATOR     = " << originator << std::endl;
   
   WriteString(ss.str());
   #endif
   
   WriteRealCcsdsHeader();
   
   #ifdef DEBUG_EPHEMFILE_HEADER
   MessageInterface::ShowMessage("EphemerisFile::WriteCcsdsHeader() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void WriteCcsdsOrbitDataSegment()
//------------------------------------------------------------------------------
/**
 * Writes CCSDS orbit data segment
 */
//------------------------------------------------------------------------------
void EphemerisFile::WriteCcsdsOrbitDataSegment()
{
   #ifdef DEBUG_EPHEMFILE_CCSDS
   MessageInterface::ShowMessage
      ("=====> WriteCcsdsOrbitDataSegment() entered\n   writeMetaDataOption=%d, "
       "saveMetaDataStart=%d, firstTimeMetaData=%d, continuousSegment=%d, "
       "a1MjdArray.size()=%d\n", writeMetaDataOption, saveMetaDataStart, firstTimeMetaData,
       continuousSegment, a1MjdArray.size());
   #endif
   
   if (a1MjdArray.empty())
   {
      if (writeMetaDataOption == 1)
      {
         #ifdef DEBUG_EPHEMFILE_CCSDS
         MessageInterface::ShowMessage("***** array is empty so writing dummy meta data time\n");
         #endif
         metaDataStartStr = "YYYY-MM-DDTHH:MM:SS.SSS";
         metaDataStopStr = "YYYY-MM-DDTHH:MM:SS.SSS";
         WriteCcsdsOemMetaData();
      }
      
      #ifdef DEBUG_EPHEMFILE_CCSDS
      MessageInterface::ShowMessage
         ("=====> WriteCcsdsOrbitDataSegment() leaving, array is empty\n");
      #endif
      return;
   }
   
   if (saveMetaDataStart)
   {
      // Do not overwrite meta data start time (LOJ: 2013.03.21 GMT-3716 Fix)
      if (metaDataStartStr == "YYYY-MM-DDTHH:MM:SS.SSS")
      {
         metaDataStart = (a1MjdArray.front())->GetReal();
         metaDataStartStr = ToUtcGregorian(metaDataStart, true, 2);
      }
   }
   metaDataStop  = (a1MjdArray.back())->GetReal();
   metaDataStopStr = ToUtcGregorian(metaDataStop, true, 2);
   
   if (writeMetaDataOption == 1)
   {
      WriteCcsdsOemMetaData();
      firstTimeMetaData = false;
   }
   
   #if !defined(__USE_DATAFILE__) || defined(DEBUG_EPHEMFILE_TEXT)
   for (UnsignedInt i = 0; i < a1MjdArray.size(); i++)
      DebugWriteOrbit("In WriteCcsdsOrbitDataSegment:", a1MjdArray[i], stateArray[i]);
   #endif
   
   if (writeMetaDataOption == 2)
   {
      dstream.seekp(metaDataBegPosition, std::ios_base::beg);
      
      WriteCcsdsOemMetaData();
      firstTimeMetaData = false;
      dstream.seekp(0, std::ios_base::end);
   }
   
   #ifdef DEBUG_EPHEMFILE_CCSDS
   MessageInterface::ShowMessage
      ("=====> Writing %d CCSDS orbit data points\n", a1MjdArray.size());
   DebugWriteTime("   First data, metaDataStart = ", metaDataStart, true);
   DebugWriteTime("     Last data, metaDataStop = ", metaDataStop, true);
   #endif
   
   WriteRealCcsdsOrbitDataSegment();
   
   #ifdef DEBUG_EPHEMFILE_CCSDS
   MessageInterface::ShowMessage
      ("=====> WriteCcsdsOrbitDataSegment() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void WriteCcsdsOemMetaData()
//------------------------------------------------------------------------------
void EphemerisFile::WriteCcsdsOemMetaData()
{
   #ifdef DEBUG_EPHEMFILE_METADATA
   MessageInterface::ShowMessage
      ("EphemerisFile::WriteCcsdsOemMetaData() entered, firstTimeMetaData=%d\n",
       firstTimeMetaData);
   #endif
   
   //=================================================================
   #if !defined(__USE_DATAFILE__) || defined(DEBUG_EPHEMFILE_TEXT)
   //=================================================================
   
   // Save meta data begin position
   metaDataBegPosition = dstream.tellp();
   
   #ifdef DEBUG_EPHEMFILE_METADATA
   MessageInterface::ShowMessage
      ("   ==> Setting metaDataBegPosition=%ld\n", (long)metaDataBegPosition);
   MessageInterface::ShowMessage
      ("   metaDataStartStr='%s', metaDataStopStr='%s'\n", metaDataStartStr.c_str(),
       metaDataStopStr.c_str());
   MessageInterface::ShowMessage
      ("   spacecraft=<%p>'%s', outCoordSystem=<%p>'%s'\n", spacecraft,
       spacecraft ? spacecraft->GetName().c_str() : "NULL", outCoordSystem,
       outCoordSystem ? outCoordSystem->GetName().c_str() : "NULL");
   #endif
   
   std::string origin = "UNKNOWN";
   std::string csType = "UNKNOWN";
   //std::string objId  = spacecraft->GetStringParameter("Id");
   
   if (outCoordSystem)
   {
      csType = outCoordSystem->GetStringParameter("Axes");
      if (csType == "MJ2000Eq")
         csType = "EME2000";
      origin = outCoordSystem->GetStringParameter("Origin");
      if (origin == "Luna")
         origin = "Moon";
   }
   
   Integer actualInterpOrder = interpolationOrder;
   Integer numData = a1MjdArray.size();
   
   #ifdef DEBUG_EPHEMFILE_METADATA
   MessageInterface::ShowMessage
      ("   actualInterpOrder=%d, numData=%d\n", actualInterpOrder, numData);
   #endif
   
   if (writeMetaDataOption == 2)
   {
      if (numData <= interpolationOrder)
      {
         actualInterpOrder = numData - 1;
         if (numData < 2)
            actualInterpOrder = 1;
      }
   }
   
   // Format interpolation order, width of 2 with left justified
   char interpOrdBuff[5];
   sprintf(interpOrdBuff, "%-2d", actualInterpOrder);
   
   std::stringstream ss("");
   ss << std::endl;
   ss << "META_START" << std::endl;
   ss << "OBJECT_NAME          = " << spacecraftName << std::endl;
   //ss << "OBJECT_ID            = " << objId << std::endl;
   ss << "OBJECT_ID            = " << spacecraftId << std::endl;
   ss << "CENTER_NAME          = " << origin << std::endl;
   ss << "REF_FRAME            = " << csType << std::endl;
   ss << "TIME_SYSTEM          = " << ccsdsEpochFormat << std::endl;
   ss << "START_TIME           = " << metaDataStartStr << std::endl;
   ss << "USEABLE_START_TIME   = " << metaDataStartStr << std::endl;
   ss << "USEABLE_STOP_TIME    = " << metaDataStopStr << std::endl;
   ss << "STOP_TIME            = " << metaDataStopStr << std::endl;
   ss << "INTERPOLATION        = " << interpolatorName << std::endl;
   ss << "INTERPOLATION_DEGREE = " << interpOrdBuff << std::endl;
   ss << "META_STOP" << std::endl << std::endl;
   
   #ifdef DEBUG_EPHEMFILE_METADATA
   MessageInterface::ShowMessage("   ==> Writing following META data:\n%s\n", ss.str().c_str());
   #endif
   
   WriteString(ss.str());
   
   // Save meta data end position
   metaDataEndPosition = dstream.tellp();
   
   if (writeMetaDataOption == 2)
   {
      if (numData < 2)
      {
         #ifdef DEBUG_EPHEMFILE_METADATA
         MessageInterface::ShowMessage("   ==> numData is less than 2, so writing COMMENT\n");
         #endif
         WriteComments
            ("There should be at least two data points when writing at integrator steps.\n");
      }
   }
   else
   {
      #ifdef DEBUG_EPHEMFILE_METADATA
      MessageInterface::ShowMessage("   ==> Writing empty comment to reserve spaces\n");
      #endif
      // Reserve space for COMMENT section for interpolation order warning,
      // since meta data is written out after data records are written
      std::string comment =
         "                                                                          \n";
      WriteComments(comment, false, false);
   }
   
   // //=================================================================
   // #else
   // //=================================================================
   
   // #ifdef DEBUG_EPHEMFILE_METADATA
   // MessageInterface::ShowMessage("   ==> No META data is written out\n");
   // #endif
   
   //=================================================================
   #endif
   //=================================================================
   
   // Write CCSDS OEM META using CcsdsEphemerisFile plugin
   WriteRealCcsdsOemMetaData();
   
   #ifdef DEBUG_EPHEMFILE_METADATA
   MessageInterface::ShowMessage("EphemerisFile::WriteCcsdsOemMetaData() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void WriteOrbitData(Real reqEpochInSecs, const Real state[6])
//------------------------------------------------------------------------------
void EphemerisFile::WriteOrbitData(Real reqEpochInSecs, const Real state[6])
{
   #ifdef DEBUG_EPHEMFILE_WRITE
   DebugWriteTime
      ("EphemerisFile::WriteOrbitData() entered, reqEpochInSecs = ", reqEpochInSecs);
   MessageInterface::ShowMessage
      ("state[0:2]=%.15f, %.15f, %.15f\n", reqEpochInSecs, state[0], state[1], state[2]);
   #endif
   
   Real outState[6];
   for (int i = 0; i < 6; i++)
      outState[i] = state[i];
   
   // Since CCSDS utilities do no convert to desired CoordinateSystem,
   // so convert it here
   if (!writeDataInDataCS)
      ConvertState(reqEpochInSecs/GmatTimeConstants::SECS_PER_DAY, state, outState);
   
   BufferOrbitData(reqEpochInSecs/GmatTimeConstants::SECS_PER_DAY, outState);
   
   #ifdef DEBUG_EPHEMFILE_WRITE
   MessageInterface::ShowMessage("EphemerisFile::WriteOrbitData() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void WriteCcsdsAemMetaData()
//------------------------------------------------------------------------------
void EphemerisFile::WriteCcsdsAemMetaData()
{
   #if !defined(__USE_DATAFILE__) || defined(DEBUG_EPHEMFILE_TEXT)
   //std::string objId  = spacecraft->GetStringParameter("Id");
   //std::string origin = spacecraft->GetOriginName();
   std::string origin = outCoordSystem->GetOriginName();
   std::string csType = "UNKNOWN";
   GmatBase *cs = (GmatBase*)(spacecraft->GetRefObject(Gmat::COORDINATE_SYSTEM, ""));
   if (cs)
      csType = cs->GetTypeName();
   
   std::stringstream ss("");
   ss << "META_START" << std::endl;
   ss << "OBJECT_NAME = " << spacecraftName << std::endl;
   ss << "OBJECT_ID = " << spacecraftId << std::endl;
   ss << "CENTER_NAME = " << origin << std::endl;
   ss << "REF_FRAME_A = " << csType << std::endl;
   ss << "REF_FRAME_B = " << "@TODO_REFB" << std::endl;
   ss << "TIME_SYSTEM = " << ccsdsEpochFormat << std::endl;
   ss << "START_TIME = " << "@TODO_START" << std::endl;
   ss << "USEABLE_START_TIME = " << "@TODO_USTART" << std::endl;
   ss << "USEABLE_STOP_TIME = " << "@TODO_USTOP" << std::endl;
   ss << "STOP_TIME = " << "@TODO_STOP" << std::endl;
   ss << "ATTITUDE_TYPE = " << "@TODO_STOP" << std::endl;
   ss << "QUATERNION_TYPE = " << "@TODO_STOP" << std::endl;
   ss << "INTERPOLATION_METHOD = " << interpolatorName << std::endl;
   ss << "INTERPOLATION_DEGREE = " << interpolationOrder << std::endl;
   ss << "META_STOP" << std::endl << std::endl;
   
   WriteString(ss.str());
   #endif
   
   WriteRealCcsdsAemMetaData();
}


//------------------------------------------------------------------------------
// void WriteCcsdsAemData(Real reqEpochInSecs, const Real quat[4])
//------------------------------------------------------------------------------
void EphemerisFile::WriteCcsdsAemData(Real reqEpochInSecs, const Real quat[4])
{
   WriteRealCcsdsAemData(reqEpochInSecs, quat);
}


//------------------------------------------------------------------------------
// void WriteCcsdsComments(const std::string &comments, bool writeKeyword = true)
//------------------------------------------------------------------------------
/**
 * Writes actual COMMENT section
 *
 * @param  comments  Comments to write
 * @param  writeKeyword  Writes COMMENT keyword followed by comments, 
 *                       writes blank otherwise
 */
//------------------------------------------------------------------------------
void EphemerisFile::WriteCcsdsComments(const std::string &comments, bool writeKeyword)
{
   std::string ccsdsComments = "COMMENT  " + comments;
   if (!writeKeyword)
      ccsdsComments = "         " + comments;
      
   #if !defined(__USE_DATAFILE__) || defined(DEBUG_EPHEMFILE_TEXT)
   WriteString("\n" + ccsdsComments + "\n");
   #endif
   
   WriteRealCcsdsComments(ccsdsComments);
}


//------------------------------------------------------------------------------
// void WriteSpkHeader()
//------------------------------------------------------------------------------
void EphemerisFile::WriteSpkHeader()
{
   #ifdef DEBUG_EPHEMFILE_TEXT
   std::string creationTime = GmatTimeUtil::FormatCurrentTime(2);
   std::stringstream ss("");
   
   ss << "SPK ORBIT DATA" << std::endl;
   ss << "CREATION_DATE  = " << creationTime << std::endl;
   ss << "ORIGINATOR     = GMAT USER" << std::endl;
   
   WriteString(ss.str());
   #endif
}


//------------------------------------------------------------------------------
// void WriteSpkOrbitDataSegment()
//------------------------------------------------------------------------------
/**
 * Writes orbit data segment to SPK file and deletes data array
 */
//------------------------------------------------------------------------------
void EphemerisFile::WriteSpkOrbitDataSegment()
{
   #ifdef DEBUG_EPHEMFILE_SPICE
   MessageInterface::ShowMessage
      ("=====> WriteSpkOrbitDataSegment() entered, a1MjdArray.size()=%d, "
       "stateArray.size()=%d\n", a1MjdArray.size(), stateArray.size());
   #endif
   
   #ifdef __USE_SPICE__
   if (a1MjdArray.size() > 0)
   {
      if (spkWriter == NULL)
         throw SubscriberException
            ("*** INTERNAL ERROR *** SPK Writer is NULL in "
             "EphemerisFile::WriteSpkOrbitDataSegment()\n");

      A1Mjd *start = a1MjdArray.front();
      A1Mjd *end   = a1MjdArray.back();

      #ifdef DEBUG_EPHEMFILE_SPICE
      MessageInterface::ShowMessage
         ("   Writing start=%.15f, end=%.15f\n", start->GetReal(), end->GetReal());
      MessageInterface::ShowMessage("Here are epochs and states:\n");
      for (unsigned int ii = 0; ii < a1MjdArray.size(); ii++)
      {
         A1Mjd *t = a1MjdArray[ii];
         Real time = t->GetReal();
         Rvector6 *st = stateArray[ii];
         MessageInterface::ShowMessage
            ("[%3d] %12.10f  %s  %s\n", ii, time, ToUtcGregorian(time, true).c_str(),
             (st->ToString()).c_str());
      }
      #endif

      #ifdef DEBUG_EPHEMFILE_TEXT
      WriteString("\n");
      for (unsigned int ii = 0; ii < a1MjdArray.size(); ii++)
         DebugWriteOrbit("In WriteSpkOrbitDataSegment:", a1MjdArray[ii], stateArray[ii]);
      #endif

      spkWriteFailed = false;
      try
      {
         spkWriter->WriteSegment(*start, *end, stateArray, a1MjdArray);
         ClearOrbitData();
         numSPKSegmentsWritten++;
         insufficientSPKData = false;
      }
      catch (BaseException &e)
      {
         ClearOrbitData();
         spkWriteFailed = true;
         dstream.flush();
         dstream.close();
         #ifdef DEBUG_EPHEMFILE_SPICE
         MessageInterface::ShowMessage("**** ERROR **** " + e.GetFullMessage());
         #endif
         e.SetFatal(true);
         throw;
      }
   }
   #endif
   
   #ifdef DEBUG_EPHEMFILE_SPICE
   MessageInterface::ShowMessage
      ("=====> WriteSpkOrbitDataSegment() leaving\n");
   #endif
}



//------------------------------------------------------------------------------
// void WriteSpkOrbitMetaData()
//------------------------------------------------------------------------------
void EphemerisFile::WriteSpkOrbitMetaData()
{
   #ifdef DEBUG_EPHEMFILE_SPICE
   MessageInterface::ShowMessage("=====> WriteSpkOrbitMetaData() entered\n");
   #endif
   
   std::string objId  = spacecraft->GetStringParameter("Id");
   //std::string origin = spacecraft->GetOriginName();
   std::string origin = outCoordSystem->GetOriginName();
   std::string csType = "UNKNOWN";
   GmatBase *cs = (GmatBase*)(spacecraft->GetRefObject(Gmat::COORDINATE_SYSTEM, ""));
   if (cs)
      csType = (cs->GetRefObject(Gmat::AXIS_SYSTEM, ""))->GetTypeName();
   
   std::stringstream ss("");
   ss << std::endl;
   ss << "META_START" << std::endl;
   ss << "OBJECT_NAME = " << spacecraftName << std::endl;
   ss << "OBJECT_ID = " << objId << std::endl;
   ss << "CENTER_NAME = " << origin << std::endl;
   ss << "REF_FRAME = " << csType << std::endl;
   ss << "TIME_SYSTEM = " << epochFormat << std::endl;
   ss << "START_TIME = " << "@TODO_START" << std::endl;
   ss << "USEABLE_START_TIME = " << "@TODO_USTART" << std::endl;
   ss << "USEABLE_STOP_TIME = " << "@TODO_USTOP" << std::endl;
   ss << "STOP_TIME = " << "@TODO_STOP" << std::endl;
   ss << "INTERPOLATION = " << interpolatorName << std::endl;
   ss << "INTERPOLATION_DEGREE = " << interpolationOrder << std::endl;
   ss << "META_STOP" << std::endl << std::endl;
   
   #ifdef DEBUG_EPHEMFILE_TEXT
   WriteString(ss.str());
   #endif
   
   WriteSpkComments(ss.str());
   
   #ifdef DEBUG_EPHEMFILE_SPICE
   MessageInterface::ShowMessage("=====> WriteSpkOrbitMetaData() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void WriteSpkComments(const std::string &comments)
//------------------------------------------------------------------------------
void EphemerisFile::WriteSpkComments(const std::string &comments)
{
   #ifdef DEBUG_EPHEMFILE_SPICE
   MessageInterface::ShowMessage("=====> WriteSpkComments() entered\n");
   #endif
   
   #ifdef DEBUG_EPHEMFILE_TEXT
   WriteString("\nCOMMENT  " + comments + "\n");
   #endif
   
   #ifdef __USE_SPICE__
   if (a1MjdArray.empty() && !writeCommentAfterData)
   {
      spkWriteFailed = true;
      MessageInterface::ShowMessage
         ("**** TODO **** EphemerisFile::WriteSpkComments() There must be at "
          "least one segment before this comment \"" + comments + "\" is written\n");
      return;
   }
   
   try
   {
      spkWriter->AddMetaData(comments);
   }
   catch (BaseException &e)
   {
      // Keep from setting a warning
      e.GetMessageType();

      spkWriteFailed = true;
      #ifdef DEBUG_EPHEMFILE_SPICE
      MessageInterface::ShowMessage("spkWriter->AddMetaData() failed\n");
      MessageInterface::ShowMessage(e.GetFullMessage());
      #endif
      throw;
   }
   #endif
   
   #ifdef DEBUG_EPHEMFILE_SPICE
   MessageInterface::ShowMessage("=====> WriteSpkComments() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void FinalizeSpkFile(bool done = true, writeMetaData = true)
//------------------------------------------------------------------------------
void EphemerisFile::FinalizeSpkFile(bool done, bool writeMetaData)
{
   #ifdef DEBUG_EPHEMFILE_SPICE
   MessageInterface::ShowMessage("=====> FinalizeSpkFile() entered\n");
   MessageInterface::ShowMessage("   and size of data is %d\n",
         (Integer) a1MjdArray.size());
   #endif
   
   #ifdef __USE_SPICE__
   try
   {
      if (!a1MjdArray.empty())
      {
         Integer mnSz   = spkWriter->GetMinNumberOfStates();
         Integer numPts = (Integer) a1MjdArray.size();
         // if we are generating SPK files in the background and there
         // are not enough states for the interpolation, we DO NOT
         // want to try to write and trigger the SPICE error;
         // for user-specified SPK files, we DO want to present
         // errors to the user.
         if (!generateInBackground || (numPts >= mnSz))
         {
            #ifdef DEBUG_EPHEMFILE_SPICE
            MessageInterface::ShowMessage("   about to write SPK orbit data segment\n");
            #endif
            // Save last data to become first data of next segment - since we may start up
            // a new SPK file after this one
            A1Mjd *a1mjd  = new A1Mjd(*a1MjdArray.back());
            Rvector6 *rv6 = new Rvector6(*stateArray.back());

            // Write a segment and delete data array pointers
            WriteSpkOrbitDataSegment();

            // Add saved data to arrays if we are not done yet
            if (!done)
            {
               a1MjdArray.push_back(a1mjd);
               stateArray.push_back(rv6);
            }
            insufficientSPKData = false;
//            WriteSpkOrbitDataSegment();
            #ifdef DEBUG_EPHEMFILE_SPICE
            MessageInterface::ShowMessage("   DONE writing SPK orbit data segment\n");
            #endif
         }
         // background SPKs need to know if there was data unwritten
         // will have 1 point from the last segment in the beginning of this
         // set of data
         else if (generateInBackground && (numPts > 1))
         {
            #ifdef DEBUG_EPHEMFILE_SPICE
            MessageInterface::ShowMessage("NOT WRITING SPK data - only %d points available!!!\n",
                  numPts);
            #endif
            insufficientSPKData = true; // data is available, but has not been written yet
         }
      }

      #ifdef DEBUG_EPHEMFILE_SPICE
      MessageInterface::ShowMessage("   about to call FinalizeKernel!!!\n");
      #endif
      spkWriter->FinalizeKernel(done, writeMetaData);
      // so we recreate next time - for background SPKs only
      if (!done) isEphemFileOpened = false;
   }
   catch (BaseException &e)
   {
      // Keep from setting a warning
      e.GetMessageType();

      ClearOrbitData();
      spkWriteFailed = true;
      #ifdef DEBUG_EPHEMFILE_SPICE
      MessageInterface::ShowMessage("spkWriter->FinalizeSpkFile() failed\n");
      MessageInterface::ShowMessage(e.GetFullMessage());
      #endif
      throw;
   }
   #endif
   
   #ifdef DEBUG_EPHEMFILE_SPICE
   MessageInterface::ShowMessage("=====> FinalizeSpkFile() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void WriteCode500OrbitDataSegment(bool canFinalize = false)
//------------------------------------------------------------------------------
/**
 * Writes orbit data segment to Code500 file and deletes data array
 */
//------------------------------------------------------------------------------
void EphemerisFile::WriteCode500OrbitDataSegment(bool canFinalize)
{
   #ifdef DEBUG_EPHEMFILE_CODE500
   MessageInterface::ShowMessage
      ("=====> WriteCode500OrbitDataSegment() entered, canFinalize=%d, isEndOfRun=%d, "
       "a1MjdArray.size()=%d, stateArray.size()=%d\n", canFinalize, isEndOfRun,
       a1MjdArray.size(), stateArray.size());
   #endif
   
   if (a1MjdArray.size() > 0)
   {
      if (code500EphemFile == NULL)
         throw SubscriberException
            ("*** INTERNAL ERROR *** Code500 Ephem Writer is NULL in "
             "EphemerisFile::WriteCode500OrbitDataSegment()\n");
      
      #ifdef DEBUG_EPHEMFILE_CODE500
      A1Mjd *start = a1MjdArray.front();
      A1Mjd *end   = a1MjdArray.back();
      MessageInterface::ShowMessage
         ("   Writing start=%.15f, end=%.15f\n", start->GetReal(), end->GetReal());
      MessageInterface::ShowMessage("There are %d epochs and states:\n", a1MjdArray.size());
      for (unsigned int ii = 0; ii < a1MjdArray.size(); ii++)
      {
         A1Mjd *tm = a1MjdArray[ii];
         Real time = tm->GetReal();
         Rvector6 *sv = stateArray[ii];
         std::string format = "% 1.15e  ";
         MessageInterface::ShowMessage
            ("[%3d] %12.10f  %s  %s\n", ii, time, ToUtcGregorian(time, true).c_str(),
             (sv->ToString(format, 6)).c_str());
      }
      #endif
      
      #ifdef DEBUG_EPHEMFILE_TEXT
      WriteString("\n");
      for (unsigned int ii = 0; ii < a1MjdArray.size(); ii++)
         DebugWriteOrbit("In WriteCode500OrbitDataSegment:", a1MjdArray[ii], stateArray[ii]);
      #endif
      
      code500WriteFailed = false;
      try
      {
         #ifdef DEBUG_EPHEMFILE_CODE500
         MessageInterface::ShowMessage
            (".....Calling code500EphemFile->WriteDataSegment() isEndOfRun=%d, "
             "epochsOnWaiting.size()=%d\n", isEndOfRun, epochsOnWaiting.size());
         DebugWriteTime("   ", epochsOnWaiting.back());
         #endif
         // Check if Code500 ephemeris file can be finalized (GMT-4060 fix)
         bool finalize = isEndOfRun && canFinalize;
         //code500EphemFile->WriteDataSegment(a1MjdArray, stateArray, isEndOfRun);
         code500EphemFile->WriteDataSegment(a1MjdArray, stateArray, finalize);
         ClearOrbitData();
      }
      catch (BaseException &e)
      {
         ClearOrbitData();
         code500WriteFailed = true;
         dstream.flush();
         dstream.close();
         #ifdef DEBUG_EPHEMFILE_CODE500
         MessageInterface::ShowMessage("**** ERROR **** " + e.GetFullMessage());
         #endif
         e.SetFatal(true);
         throw;
      }
   }
   
   #ifdef DEBUG_EPHEMFILE_CODE500
   MessageInterface::ShowMessage
      ("=====> WriteCode500OrbitDataSegment() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// FinalizeCode500Ephemeris()
//------------------------------------------------------------------------------
void EphemerisFile::FinalizeCode500Ephemeris()
{
   #ifdef DEBUG_EPHEMFILE_FINISH
   MessageInterface::ShowMessage("EphemerisFile::FinalizeCode500Ephemeris() entered\n");
   #endif
   
   if (code500EphemFile == NULL)
      throw SubscriberException
         ("*** INTERNAL ERROR *** Code500 Ephem Writer is NULL in "
          "EphemerisFile::FinalizeCode500Ephemeris()\n");
   
   // Write any final header data
   code500EphemFile->FinalizeHeaders();
   
   #ifdef DEBUG_CODE500_OUTPUT
   // For for debugging
   if (isEndOfRun)
   {
      MessageInterface::ShowMessage
         ("===> EphemerisFile::FinalizeCode500Ephemeris() calling code500EphemFile "
          "for debug output\n   fullPathFileName = '%s'\n", fullPathFileName.c_str());
      bool swapByteOrder = false;
      if (outputFormat == "UNIX")
         swapByteOrder = true;
      code500EphemFile->OpenForRead(fullPathFileName, 1, 1);
      code500EphemFile->SetSwapEndian(swapByteOrder, 1);
      code500EphemFile->ReadHeader1(1);
      code500EphemFile->ReadDataRecords(-999, 2);
      code500EphemFile->CloseForRead();
   }
   #endif
   
   // Close Code500 ephemeris file
   code500EphemFile->CloseForWrite();
   
   #ifdef DEBUG_EPHEMFILE_FINISH
   MessageInterface::ShowMessage("EphemerisFile::FinalizeCode500Ephemeris() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// RealArray::iterator FindEpochOnWaiting(Real epochInSecs, const std::string &msg = "")
//------------------------------------------------------------------------------
/**
 * Finds epoch from epochsOnWaiting list.
 * It uses 1.0e-6 tolerance to find matching epoch.
 */
//------------------------------------------------------------------------------
RealArray::iterator EphemerisFile::FindEpochOnWaiting(Real epochInSecs,
                                                      const std::string &msg)
{
   #ifdef DEBUG_FIND_EPOCH
   MessageInterface::ShowMessage("FindEpochOnWaiting() entered\n");
   DebugWriteTime("   epochInSecs = ", epochInSecs);
   DebugWriteEpochsOnWaiting();
   #endif
   
   // Find matching epoch
   RealArray::iterator iterFound = epochsOnWaiting.begin();
   while (iterFound != epochsOnWaiting.end())
   {
      #ifdef DEBUG_FIND_EPOCH
      DebugWriteTime("      iterFound, epoch = ", *iterFound);
      #endif
      
      if (GmatMathUtil::Abs(*iterFound - epochInSecs) < 1.0e-6)
      {
         #ifdef DEBUG_EPHEMFILE_ORBIT
         DebugWriteTime(msg + " epoch = ", *iterFound);
         #endif
         return iterFound;
      }
      iterFound++;
   }
   
   return epochsOnWaiting.end();
}


//------------------------------------------------------------------------------
// void RemoveEpochAlreadyWritten(Real epochInSecs, const std::string &msg = "")
//------------------------------------------------------------------------------
/**
 * Erases epoch already processed from epochsOnWaiting list. It uses 1.0e-6
 * tolerance to find matching epoch.
 */
//------------------------------------------------------------------------------
void EphemerisFile::RemoveEpochAlreadyWritten(Real epochInSecs, const std::string &msg)
{
   // Find matching epoch
   RealArray::iterator iterFound = epochsOnWaiting.begin();
   while (iterFound != epochsOnWaiting.end())
   {
      if (GmatMathUtil::Abs(*iterFound - epochInSecs) < 1.0e-6)
      {
         #ifdef DEBUG_EPHEMFILE_ORBIT
         DebugWriteTime(msg + " epoch = ", *iterFound);
         #endif
         // erase returns the next one
         iterFound = epochsOnWaiting.erase(iterFound);
      }
      else
         ++iterFound;
   }
}


//------------------------------------------------------------------------------
// void AddNextEpochToWrite(Real epochInSecs, const std::string &msg);
//------------------------------------------------------------------------------
/**
 * Adds epoch to write to epochsOnWaiting list. It uses 1.0e-6
 * tolerance to find matching epoch.
 */
//------------------------------------------------------------------------------
void EphemerisFile::AddNextEpochToWrite(Real epochInSecs, const std::string &msg)
{
   if (FindEpochOnWaiting(epochInSecs, msg) == epochsOnWaiting.end())
   {
      #ifdef DEBUG_EPHEMFILE_TIME
      DebugWriteTime(msg + " epochInSecs = ", epochInSecs);
      #endif
      epochsOnWaiting.push_back(epochInSecs);
      nextOutEpochInSecs = epochInSecs;
   }
   else
   {
      #ifdef DEBUG_EPHEMFILE_TIME
      DebugWriteTime("   ========== skipping redundant epochInSecs = ", epochInSecs);
      #endif
   }
}


//------------------------------------------------------------------------------
//bool IsEventFeasible(bool checkForNoData = true)
//------------------------------------------------------------------------------
/**
 * Checks if events can occur. Events other than maneuver can only occur after
 * valid data is received, so checkForNoData should be set appropriately.
 */
//------------------------------------------------------------------------------
bool EphemerisFile::IsEventFeasible(bool checkForNoData)
{
   #ifdef DEBUG_EPHEMFILE_EVENTS
   MessageInterface::ShowMessage
      ("EphemerisFile::IsEventFeasible() '%s' entered, checkForNoData=%d, active=%d, "
       "firstTimeWriting=%d, currEpochInSecs=%f, a1MjdArray.size()=%d\n", GetName().c_str(),
       checkForNoData, active, firstTimeWriting, currEpochInSecs, a1MjdArray.size());
   #endif
   
   if (!active)
   {
      #ifdef DEBUG_EPHEMFILE_EVENTS
      MessageInterface::ShowMessage
         ("EphemerisFile::IsEventFeasible() '%s' returning false, it is not active\n",
          GetName().c_str());
      #endif
      return false;
   }
   
   if (finalEpochProcessed)
   {
      #ifdef DEBUG_EPHEMFILE_EVENTS
      MessageInterface::ShowMessage
         ("EphemerisFile::IsEventFeasible() '%s' returning false, user defined final "
          "epoch has been processed\n", GetName().c_str());
      #endif
      return false;
   }
   
   // Check if any valid data received if not maneuver event
   if (checkForNoData)
   {
      if (firstTimeWriting && currEpochInSecs == -999.999 && a1MjdArray.empty())
      {
         #ifdef DEBUG_EPHEMFILE_EVENTS
         MessageInterface::ShowMessage
            ("EphemerisFile::IsEventFeasible() '%s' returning false, no valid data received yet\n",
             GetName().c_str());
         #endif
         return false;
      }
   }
   
   #ifdef DEBUG_EPHEMFILE_EVENTS
   MessageInterface::ShowMessage
      ("EphemerisFile::IsEventFeasible() '%s' returning true\n", GetName().c_str());
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// void ConvertState(Real epochInDays, const Real inState[6], Real outState[6])
//------------------------------------------------------------------------------
void EphemerisFile::ConvertState(Real epochInDays, const Real inState[6],
                                 Real outState[6])
{
   #ifdef DEBUG_EPHEMFILE_CONVERT_STATE   
   DebugWriteOrbit("In ConvertState(in):", epochInDays, inState, true, true);
   #endif
   
   coordConverter.Convert(A1Mjd(epochInDays), inState, theDataCoordSystem,
                          outState, outCoordSystem, true);
   
   #ifdef DEBUG_EPHEMFILE_CONVERT_STATE   
   DebugWriteOrbit("In ConvertState(out):", epochInDays, outState, true, true);
   #endif
}


//------------------------------------------------------------------------------
// std::string ToUtcGregorian(Real epoch, bool inDays = false, Integer format = 1)
//------------------------------------------------------------------------------
/**
 * Formats epoch in either in days or seconds to desired format.
 *
 * @param  epoch  Epoch in days or seconds
 * @param  inDays  Flag indicating epoch is in days
 * @param  format  Desired output format [1]
 *                 1 = "01 Jan 2000 11:59:28.000"
 *                 2 = "2000-01-01T11:59:28.000"
 */
//------------------------------------------------------------------------------
std::string EphemerisFile::ToUtcGregorian(Real epoch, bool inDays, Integer format)
{
   if (epoch == -999.999)
      return "-999.999";

   
   Real toMjd;
   std::string epochStr;
   
   Real epochInDays = epoch;
   if (!inDays)
      epochInDays = epoch / GmatTimeConstants::SECS_PER_DAY;
   
   std::string outFormat = epochFormat;
   if (format == 2)
      outFormat = "UTCGregorian";
   
   // Convert current epoch to specified format
   TimeConverterUtil::Convert("A1ModJulian", epochInDays, "", outFormat,
                              toMjd, epochStr, format);
   
   if (epochStr == "")
   {
      MessageInterface::ShowMessage
         ("**** ERROR **** EphemerisFile::ToUtcGregorian() Cannot convert epoch %.10f %s "
          "to UTCGregorian\n", epoch, inDays ? "days" : "secs");
      
      epochStr = "EpochError";
   }
   
   return epochStr;
}


//------------------------------------------------------------------------------
// void FormatErrorMessage(std::string &ephemMsg, std::string &errMsg)
//------------------------------------------------------------------------------
void EphemerisFile::FormatErrorMessage(std::string &ephemMsg, std::string &errMsg)
{
   std::string commonMsg = "There is not enough data available to generate the current block of "
      "ephemeris";
   std::string ephemFileStr = " to EphemerisFile: \"" + fileName + "\"";
   
   Real timeSpanInSecs = (currEpochInDays - blockBeginA1Mjd) * GmatTimeConstants::SECS_PER_DAY;

   #ifdef DEBUG_TIME_SPAN
   DebugWriteTime("=> blockBeginA1Mjd = ", blockBeginA1Mjd, true, 2);
   DebugWriteTime("=> currEpochInDays = ", currEpochInDays, true, 2);
   MessageInterface::ShowMessage("=> timeSpanInSecs  = %f\n", timeSpanInSecs);
   #endif
   
   // Format error message
   if (initialEpochA1Mjd != -999.999 && (currEpochInDays < initialEpochA1Mjd))
   {
      std::string initialEpochStr = ToUtcGregorian(initialEpochA1Mjd, true, 2);
      std::string currentEpochStr = ToUtcGregorian(currEpochInDays, true, 2);
      std::string detailedMsg = ". The block ended at " + currentEpochStr + "(" +
         GmatStringUtil::ToString(currEpochInDays) + ") before the user defined initial epoch of " +
         initialEpochStr + "(" + GmatStringUtil::ToString(initialEpochA1Mjd) + ").";
      ephemMsg = commonMsg + detailedMsg;
      errMsg = commonMsg + ephemFileStr + detailedMsg;
   }
   else if (timeSpanInSecs < stepSizeInSecs)
   {
      std::string blockBeginEpochStr = ToUtcGregorian(blockBeginA1Mjd, true, 2);
      std::string currentEpochStr = ToUtcGregorian(currEpochInDays, true, 2);
      std::string detailedMsg = ".  The data time span (" + blockBeginEpochStr + " - " +
         currentEpochStr + ") is less than the step size of " +
         GmatStringUtil::ToString(stepSizeInSecs, 2, true) + " seconds.";
      ephemMsg = commonMsg + detailedMsg;
      errMsg = commonMsg + ephemFileStr + detailedMsg;
   }
   else
   {
      std::string detailedMsg1 = " at the requested interpolation order. ";
      std::string detailedMsg2 = "Number of required points is " +
         GmatStringUtil::ToString(interpolationOrder + 1, 1) + ", but received " +
         GmatStringUtil::ToString(interpolator->GetPointCount(), 1) + ". ";
      std::string detailedMsg3 = "There should be at least one data point more than interpolation order.";
      ephemMsg = commonMsg + detailedMsg1 + detailedMsg3;
      errMsg = commonMsg + ephemFileStr + detailedMsg1 + detailedMsg2 + detailedMsg3;
   }
}

//------------------------------------------------------------------------------
// void DebugWriteTime(const std::string &msg, Real epoch, bool inDays = false,
//                     Integer format = 1)
//------------------------------------------------------------------------------
/**
 * Writes debug output of time
 *
 * @param  msg  Message to write
 * @param  epoch  Epoch in days or seconds
 * @param  inDays  Flag indicating epoch is in days
 * @param  format  Format of the time to be written out
 *                 1 = "01 Jan 2000 11:59:28.000"
 *                 2 = "2000-01-01T11:59:28.000"
 */
//------------------------------------------------------------------------------
void EphemerisFile::DebugWriteTime(const std::string &msg, Real epoch, bool inDays,
                                   Integer format)
{
   if (epoch == -999.999)
   {
      MessageInterface::ShowMessage("%s%f\n", msg.c_str(), epoch);
      return;
   }
   
   Real epochInDays = epoch;
   if (!inDays)
      epochInDays = epoch / GmatTimeConstants::SECS_PER_DAY;
   
   std::string epochStr = ToUtcGregorian(epochInDays, true, format);
   
   MessageInterface::ShowMessage
      ("%s%.15f, %.15f, '%s'\n", msg.c_str(), epoch, epochInDays, epochStr.c_str());
}


//------------------------------------------------------------------------------
// void DebugWriteOrbit(const std::string &msg, Real epoch, const Real state[6],
//                      bool inDays = false, bool logOnly = false)
//------------------------------------------------------------------------------
void EphemerisFile::DebugWriteOrbit(const std::string &msg, Real epoch,
                                    const Real state[6], bool inDays, bool logOnly)
{
   Real reqEpochInDays = epoch;
   if (!inDays)
      reqEpochInDays = epoch / GmatTimeConstants::SECS_PER_DAY;
   
   Rvector6 inState(state);
   Rvector6 outState(state);
   
   std::string epochStr = ToUtcGregorian(reqEpochInDays, true, 2);
   
   if (logOnly)
   {
      MessageInterface::ShowMessage
         ("%s\n%s\n% 1.15e  % 1.15e  % 1.15e\n% 1.15e  % 1.15e  % 1.15e\n",
          msg.c_str(), epochStr.c_str(), outState[0], outState[1], outState[2],
          outState[3], outState[4], outState[5]);
   }
   else
   {
      char strBuff[200];
      sprintf(strBuff, "%s  % 1.15e  % 1.15e  % 1.15e  % 1.15e  % 1.15e  % 1.15e\n",
              epochStr.c_str(), outState[0], outState[1], outState[2], outState[3],
              outState[4], outState[5]);
      dstream << strBuff;
      dstream.flush();
   }
}


//------------------------------------------------------------------------------
// void DebugWriteOrbit(const std::string &msg, A1Mjd *epochInDays, Rvector6 *state,
//                      bool logOnly = false)
//------------------------------------------------------------------------------
void EphemerisFile::DebugWriteOrbit(const std::string &msg, A1Mjd *epochInDays,
                                    Rvector6 *state, bool logOnly)
{
   DebugWriteOrbit(msg, epochInDays->GetReal(), state->GetDataVector(), true, logOnly);
}


//------------------------------------------------------------------------------
// void DebugWriteEpochsOnWaiting(const std::string &msg = "")
//------------------------------------------------------------------------------
void EphemerisFile::DebugWriteEpochsOnWaiting(const std::string &msg)
{
   MessageInterface::ShowMessage
      ("%sThere are %d epochs on waiting\n", msg.c_str(), epochsOnWaiting.size());
   for (UnsignedInt i = 0; i < epochsOnWaiting.size(); i++)
      DebugWriteTime("      ", epochsOnWaiting[i]);
}


//------------------------------------------------------------------------------
// void WriteDeprecatedMessage(Integer id) const
//------------------------------------------------------------------------------
/**
 * Writes deprecated field message per GMAT session
 */
//------------------------------------------------------------------------------
void EphemerisFile::WriteDeprecatedMessage(Integer id) const
{
   // Write only one message per session
   static bool writeFileNameMsg = true;
   
   switch (id)
   {
   case FILE_NAME:
      if (writeFileNameMsg)
      {
         MessageInterface::ShowMessage
            (deprecatedMessageFormat.c_str(), "FileName", GetName().c_str(),
             "Filename");
         writeFileNameMsg = false;
      }
      break;
   default:
      break;
   }
}

//--------------------------------------
// methods inherited from Subscriber
//--------------------------------------

//------------------------------------------------------------------------------
// bool Distribute(int len)
//------------------------------------------------------------------------------
bool EphemerisFile::Distribute(int len)
{
   return true;
}


//------------------------------------------------------------------------------
// bool Distribute(const Real * dat, Integer len)
//------------------------------------------------------------------------------
/*
 * Handles distributed data from Subscriber::ReceiveData() through
 * Publisher::Publish(). Asssumes first data dat[0] is data epoch in A1Mjd.
 *
 * @param dat Data received 
 */
//------------------------------------------------------------------------------
bool EphemerisFile::Distribute(const Real * dat, Integer len)
{
   #if DBGLVL_EPHEMFILE_DATA > 0
   MessageInterface::ShowMessage
      ("======================================================================\n"
       "EphemerisFile::Distribute() this=<%p>'%s' entered\n", this, GetName().c_str());
   MessageInterface::ShowMessage
      ("   len=%d, active=%d, writeEphemeris=%d, isEndOfReceive=%d, isEndOfDataBlock=%d, "
       "isEndOfRun=%d\n   runstate=%d, prevRunState=%d, isManeuvering=%d, firstTimeWriting=%d, "
       "propDirection=%f\n", len, active, writeEphemeris, isEndOfReceive, isEndOfDataBlock,
       isEndOfRun, runstate, prevRunState, isManeuvering, firstTimeWriting, propDirection);
   if (len > 0)
   {
      DebugWriteTime("   ", dat[0], true);
      //MessageInterface::ShowMessage("   dat[0]=%.15f, dat[1]=%.15f\n", dat[0], dat[1]);
      MessageInterface::ShowMessage("   dat[] = [");
      for (Integer i = 0; i < len; ++i)
      {
         //MessageInterface::ShowMessage("%.15le", dat[i]);
         MessageInterface::ShowMessage("%.15f", dat[i]);
         if (i == len-1)
            MessageInterface::ShowMessage("]\n");
         else
            MessageInterface::ShowMessage(", ");
      }
   }
   #endif
   #if DBGLVL_EPHEMFILE_DATA > 1
   MessageInterface::ShowMessage("   fileName='%s'\n", fileName.c_str());
   #endif
   
   // If EphemerisFile was toggled off, start new segment (LOJ: 2010.09.30)
   if (!active)
   {
      writingNewSegment = true;
      #if DBGLVL_EPHEMFILE_DATA > 0
      MessageInterface::ShowMessage
         ("=====> EphemerisFile::Distribute() '%s' returning true, it is toggled off\n",
          GetName().c_str());
      #endif
      return true;
   }
   
   if (!isEphemFileOpened)
   {
      // Open text EphemerisFile for debug or CCSDS if not already opened
      if (!OpenTextEphemerisFile())
      {
         #ifdef DEBUG_EPHEMFILE_INIT
         MessageInterface::ShowMessage
            ("**** ERROR **** EphemerisFile::Distribute() <%p>'%s' throwing exception: Failed to "
             "open EphemerisFile\n", this, GetName().c_str(), fullPathFileName.c_str());
         #endif
         throw SubscriberException
            ("Failed to open EphemerisFile \"" + fullPathFileName + "\"\n");
      }
      
      // Create binary ephemeris file
      if (writeEphemeris && (fileType == SPK_ORBIT || fileType == CODE500_EPHEM))
      {
         #ifdef DEBUG_EPHEMFILE_INIT
         MessageInterface::ShowMessage
            ("===> EphemerisFile::Distribute() <%p>'%s' creating EphemerisFile of type '%s'\n",
             this, GetName().c_str(), fileFormat.c_str());
         #endif
         CreateEphemerisFile();
      }
      isEphemFileOpened = true;
   }
   
   // If end of run received, finishup writing
   if (isEndOfRun)
   {
      // End of run, blank out the last comment if needed
      #ifdef DEBUG_EPHEMFILE_FINISH
      MessageInterface::ShowMessage("=====> End of run\n");
      std::string lastEpochWroteStr = ToUtcGregorian(lastEpochWrote);
      std::string currEpochStr = ToUtcGregorian(currEpochInSecs);
      std::string prevEpochStr = ToUtcGregorian(prevEpochInSecs);
      MessageInterface::ShowMessage
         ("EphemerisFile::Distribute() this=<%p>'%s', isEndOfReceive=%d, "
          "isEndOfDataBlock=%d, isEndOfRun=%d, len=%d, firstTimeWriting=%d\n"
          "   lastEpochWrote=%s, currEpochInSecs=%s, prevEpochInSecs=%s\n"
          "   a1MjdArray.size()=%d\n", this, GetName().c_str(), isEndOfReceive,
          isEndOfDataBlock, isEndOfRun, len, firstTimeWriting, lastEpochWroteStr.c_str(),
          currEpochStr.c_str(), prevEpochStr.c_str(), a1MjdArray.size());
      #endif
      
      // If not first time and there is data to process, finish up writing
      if (!firstTimeWriting && !a1MjdArray.empty())
      {
         #ifdef DEBUG_EPHEMFILE_FINISH
         MessageInterface::ShowMessage("EphemerisFile::Distribute() Calling FinishUpWriting()\n");
         #endif

         try
         {
            FinishUpWriting();
         }
         catch (BaseException &be)
         {
            #ifdef DEBUG_EPHEMFILE_FINISH
            MessageInterface::ShowMessage
               ("EphemerisFile::Distribute() returning true after writing error "
                "message\n   FinishUpWriting() threw an exception\n  message = %s\n",
                be.GetFullMessage().c_str());
            #endif
            MessageInterface::ShowMessage("%s\n", be.GetFullMessage().c_str());
            return true;
         }
      }
      
      #if DBGLVL_EPHEMFILE_DATA > 0
      MessageInterface::ShowMessage("EphemerisFile::Distribute() returning true\n");
      #endif
      return true;
   }
   
   if (len == 0)
   {
      #if DBGLVL_EPHEMFILE_DATA
      MessageInterface::ShowMessage
         ("EphemerisFile::Distribute() Just returning true, data length is zero\n");
      #endif
      return true;
   }
   
   isFinalized = false;
   
   //------------------------------------------------------------
   // if solver is running, just return
   //------------------------------------------------------------
   if (runstate == Gmat::SOLVING)
   {
      #ifdef DEBUG_EPHEMFILE_SOLVER_DATA
      MessageInterface::ShowMessage
         ("EphemerisFile::Distribute() Just returning true; solver is running\n");
      #endif
      
      return true;
   }
   
   // Skip data if data publishing command such as Propagate is inside a function
   // and this EphemerisFile is not a global nor a local object (i.e declared in the main script)
   // (LOJ: 2015.08.13)
   if (currentProvider && currentProvider->TakeAction("IsInFunction"))
   {
      #if DBGLVL_EPHEMFILE_DATA
      MessageInterface::ShowMessage
         ("   Data is published from the function, '%s' IsGlobal:%s, IsLocal:%s\n",
          GetName().c_str(), IsGlobal() ? "Yes" : "No", IsLocal() ? "Yes" : "No");
      #endif
      
      bool skipData = false;
      if (spacecraft)
      {
         #if DBGLVL_EPHEMFILE_DATA
         MessageInterface::ShowMessage
            ("   spacecraft = <%p>[%s]'%s', IsGlobal=%d, IsLocal=%d\n",
             spacecraft, spacecraft->GetTypeName().c_str(), spacecraft->GetName().c_str(),
             spacecraft->IsGlobal(), spacecraft->IsLocal());
         #endif

         // Skip data if EphemerisFile is global and spacecraft is local
         if (IsGlobal() && spacecraft->IsLocal())
         {
            #if DBGLVL_EPHEMFILE_DATA
            MessageInterface::ShowMessage
               ("   Skip data since '%s' is global and spacecraft is local\n",
                GetName().c_str());
            #endif
            skipData = true;
         }
         // Skip data if spacecraft is not a global nor a local object
         else if (!(spacecraft->IsGlobal()) && !(spacecraft->IsLocal()))
         {
            #if DBGLVL_EPHEMFILE_DATA
            MessageInterface::ShowMessage
               ("   Skip data since spacecraft is not a global nor a local object\n");
            #endif
            skipData = true;
         }
      }
      
      if (skipData)
      {
         #if DBGLVL_EPHEMFILE_DATA
         MessageInterface::ShowMessage
            ("EphemerisFile::Distribute() this=<%p>'%s' just returning true\n   data is "
             "from a function and spacecraft is not a global nor a local object\n",
             this, GetName().c_str());
         #endif
         return true;
      }
   }
   
   // Get proper id with data label
   if (theDataLabels.empty())
   {
      #if DBGLVL_EPHEMFILE_DATA
      MessageInterface::ShowMessage
         ("EphemerisFile::Distribute() Just returning true, data labels are empty\n");
      #endif
      return true;
   }
   
   StringArray dataLabels = theDataLabels[0];
   
   #if DBGLVL_EPHEMFILE_DATA_LABELS
   MessageInterface::ShowMessage("   Data labels for %s =\n   ", GetName().c_str());
   for (int j=0; j<(int)dataLabels.size(); j++)
      MessageInterface::ShowMessage("%s ", dataLabels[j].c_str());
   MessageInterface::ShowMessage("\n");
   #endif
   
   Integer idX, idY, idZ;
   Integer idVx, idVy, idVz;
   
   idX  = FindIndexOfElement(dataLabels, spacecraftName + ".X");
   idY  = FindIndexOfElement(dataLabels, spacecraftName + ".Y");
   idZ  = FindIndexOfElement(dataLabels, spacecraftName + ".Z");
   idVx = FindIndexOfElement(dataLabels, spacecraftName + ".Vx");
   idVy = FindIndexOfElement(dataLabels, spacecraftName + ".Vy");
   idVz = FindIndexOfElement(dataLabels, spacecraftName + ".Vz");
   
   #if DBGLVL_EPHEMFILE_DATA_LABELS
   MessageInterface::ShowMessage
      ("   spacecraft='%s', idX=%d, idY=%d, idZ=%d, idVx=%d, idVy=%d, idVz=%d\n",
       spacecraftName.c_str(), idX, idY, idZ, idVx, idVy, idVz);
   #endif
   
   // if any of index not found, just return true
   if (idX  == -1 || idY  == -1 || idZ  == -1 ||
       idVx == -1 || idVy == -1 || idVz == -1)
   {
      #if DBGLVL_EPHEMFILE_DATA
      MessageInterface::ShowMessage
         ("EphemerisFile::Distribute() '%s' Just returning true, data index not found\n",
          GetName().c_str());
      #endif
      return true;
   }
   
   #if DBGLVL_EPHEMFILE_DATA
   MessageInterface::ShowMessage
      ("   %s, epoch = %.15f, X,Y,Z = %.15f, %.15f, %.15f\n   a1MjdArray.size() = %d\n",
       GetName().c_str(), dat[0], dat[idX], dat[idY], dat[idZ], a1MjdArray.size());
   #endif
   
   // Now copy distributed data to data member
   currEpochInDays = dat[0];
   currState[0] = dat[idX];
   currState[1] = dat[idY];
   currState[2] = dat[idZ];
   currState[3] = dat[idVx];
   currState[4] = dat[idVy];
   currState[5] = dat[idVz];
   
   // To compute block time span for use in the error message (LOJ: 2014.04.30)
   // Save block begin time
   if (blockBeginA1Mjd == -999.999)
      blockBeginA1Mjd = currEpochInDays;
   
   #ifdef DEBUG_MANEUVER
   MessageInterface::ShowMessage
      ("   Distribute dat[0]=%.15f, dat[1]=%.15f, dat[4]=%.15f\n", dat[0], dat[1], dat[4]);
   #endif
   
   // Internally all epochs are in seconds to avoid epoch drifting.
   // For long run epochs to process drifts behind the actual.
   prevEpochInSecs = currEpochInSecs;
   currEpochInSecs = currEpochInDays * GmatTimeConstants::SECS_PER_DAY;
   prevPropDirection = currPropDirection;
   currPropDirection = propDirection;
   
   propIndicator = 0;
   
   if (prevPropDirection == 0.0 && currPropDirection == 1.0)
      propIndicator = 1; // Initial forward prop
   else if (prevPropDirection == 0.0 && currPropDirection == -1.0)
      propIndicator = 2; // Initial backward prop
   else if (prevPropDirection == 1.0 && currPropDirection == -1.0)
      propIndicator = 3; // Changed from foward to backward prop
   else if (prevPropDirection == -1.0 && currPropDirection == 1.0)
      propIndicator = 4; // Changed from backward to forward prop
   
   #if DBGLVL_EPHEMFILE_DATA
   DebugWriteTime("   currEpochInSecs = ", currEpochInSecs);
   DebugWriteTime("   prevEpochInSecs = ", prevEpochInSecs);
   MessageInterface::ShowMessage
      ("   currPropDirection=%f, prevPropDirection=%f, propIndicator=%d\n",
       currPropDirection, prevPropDirection, propIndicator);
   if (propIndicator == 1)
      MessageInterface::ShowMessage("   ==> It is initial forward propagation\n");
   else if (propIndicator == 2)
      MessageInterface::ShowMessage("   ==> It is initial backward propagation\n");
   else if (propIndicator == 3)
      MessageInterface::ShowMessage("   ==> It changed to backward propagation\n");
   else if (propIndicator == 4)
      MessageInterface::ShowMessage("   ==> It changed to forward propagation\n");
   #endif
   
   
   // If propagation direction changed, finishup writing current segment
   if (propIndicator > 2)
   {
      if (!firstTimeWriting && !a1MjdArray.empty())
      {
         // Restart the interpolation for CCSDS format
         if ((fileFormat == "CCSDS-OEM" || fileFormat == "CCSDS-AEM"))
         {
            #ifdef DEBUG_EPHEMFILE_FINISH
            MessageInterface::ShowMessage
               ("EphemerisFile::Distribute() prop direction changed, so calling RestartInterpolation()\n");
            #endif
            std::string comment = "This block begins after propagation direction change";
            RestartInterpolation(comment, true, true);
         }
         else
         {
            #ifdef DEBUG_EPHEMFILE_FINISH
            MessageInterface::ShowMessage
               ("EphemerisFile::Distribute() prop direction changed, so calling FinishUpWriting()\n");
            #endif
            FinishUpWriting();
         }
      }
   }
   
   // Check for time going backward (GMT-4066 FIX)
   if (currEpochInSecs < prevEpochInSecs)
   {      
      std::string currTimeStr = ToUtcGregorian(currEpochInSecs);
      std::string prevTimeStr = ToUtcGregorian(prevEpochInSecs);
      std::string msg = "*** WARNING *** The user has generated non-monotonic "
         "invalid ephemeris file \"" + GetName() + "\" starting at " + currTimeStr +
         "; previous time is " + prevTimeStr + ".";
      
      #if DBGLVL_EPHEMFILE_DATA
      MessageInterface::ShowMessage
         ("   current time (%s> < previous time (%s)\n", currTimeStr.c_str(),
          prevTimeStr.c_str());
      #endif
      
      //====================================================
      // Throw an exception for CODE-500
      if (fileType == CODE500_EPHEM)
      {
         throw SubscriberException(msg);
      }
      
      //====================================================
      // Write one time warning and continue for other types
      static bool firstTimeWarning = true;
      
      if (propIndicator >= 3 ||
          (propDirection == -1.0 && (fileFormat != "CCSDS-OEM" && fileFormat != "CCSDS-AEM")))
      {        
         if (firstTimeWarning)
         {
            MessageInterface::ShowMessage(msg);
            firstTimeWarning = false;
         }
      }
      
      // If file format is not CCSDS, just return
      if (fileFormat != "CCSDS-OEM" && fileFormat != "CCSDS-AEM")
      {
         #if DBGLVL_EPHEMFILE_DATA
         MessageInterface::ShowMessage
            ("EphemerisFile::Distribute() Just returning true, file type is not CCSDS\n");
         #endif
         return true;
      }
   } // if (currEpochInSecs >= prevEpochInSecs)
   
   
   // Ignore duplicate data
   if (currEpochInSecs == prevEpochInSecs)
   {
      #ifdef DEBUG_MANEUVER
      MessageInterface::ShowMessage("Ignoring dupe\n");
         #endif
      #if DBGLVL_EPHEMFILE_DATA
      MessageInterface::ShowMessage
         ("EphemerisFile::Distribute() Just returning true, currEpochInSecs (%.15f) "
          "= prevEpochInSecs (%.15f)\n", currEpochInSecs, prevEpochInSecs);
      #endif
      return true;
   }
   
   
   //=================================================================
   //Hold this for now to complete GMT-4066(LOJ: 2013.07.19)
   //=================================================================
   #if 0
   // Check for large time gap if needed
   if (checkForLargeTimeGap)
   {
      #if DBGLVL_EPHEMFILE_DATA
      MessageInterface::ShowMessage("   Checking for large time gap for Code500 ephemeris\n");
      #endif
      
      // Assuming time gap is greater than 0.1 day
      if (currEpochInSecs > ( prevEpochInSecs + 8640.0))
      {
         std::string currTimeStr = ToUtcGregorian(currEpochInSecs);
         std::string prevTimeStr = ToUtcGregorian(prevEpochInSecs);
         SubscriberException se;
         se.SetDetails("Cannot continue ephemeris generation for the EphemerisFile \"%s\".\n   "
                       "It encounterd large time gap between points, so stopping.\n   "
                       "current epoch: %s, previous epoch: %s\n", GetName().c_str(),
                       currTimeStr.c_str(), prevTimeStr.c_str());
         throw se;
      }
   }
   #endif
   //=================================================================
   
   bool processData = false;
#if DBGLVL_EPHEMFILE_DATA
MessageInterface::ShowMessage
   ("EphemerisFile::Distribute() ********* state is %d\n", runstate);
#endif
      
   //------------------------------------------------------------
   // if solver is not running or solver has finished, write data
   //------------------------------------------------------------
   if (runstate == Gmat::RUNNING || runstate == Gmat::SOLVEDPASS)
   {
      #ifdef DEBUG_EPHEMFILE_SOLVER_DATA
      MessageInterface::ShowMessage
         ("EphemerisFile::Distribute() Writing out state with solver's final "
          "solution, runstate=%d, maneuverEpochInDays=%.15f\n", runstate,
          maneuverEpochInDays);
      DebugWriteOrbit("In Distribute:", currEpochInDays, currState, true, true);
      #endif
      #if DBGLVL_EPHEMFILE_DATA
      MessageInterface::ShowMessage
         ("EphemerisFile::Distribute() state is RUNNING or SOLVEDPASS\n");
      #endif
      
      // Check for epoch before maneuver epoch
      // Propagate publishes data with epoch before maneuver epoch
      if (runstate == Gmat::SOLVEDPASS && currEpochInDays < maneuverEpochInDays)
      {
         #ifdef DEBUG_EPHEMFILE_SOLVER_DATA
         MessageInterface::ShowMessage
            ("EphemerisFile::Distribute() Just returning; epoch (%.15f) < maneuver "
             "epoch (%.15f)solver is running\n", currEpochInDays, maneuverEpochInDays);
         #endif
         return true;
      }
      
      if (finalEpochProcessed)
      {
         #if DBGLVL_EPHEMFILE_DATA > 0
         MessageInterface::ShowMessage("=====> User defined final epoch has been processed, so just return\n");
         #endif
         return true;
      }
      
#if DBGLVL_EPHEMFILE_DATA
MessageInterface::ShowMessage
   ("EphemerisFile::Distribute() about to check initial and final epoch\n");
#endif
      // Check user defined initial and final epoch
      processData = CheckInitialAndFinalEpoch();
#if DBGLVL_EPHEMFILE_DATA
MessageInterface::ShowMessage
   ("EphemerisFile::Distribute() checked initial and final epoch\n");
#endif
      
      // Check if it is time to write
      bool timeToWrite = false;
      if (fileType != SPK_ORBIT)
         timeToWrite = IsTimeToWrite(currEpochInSecs, currState);
      
      #if DBGLVL_EPHEMFILE_DATA > 0
      MessageInterface::ShowMessage
         ("   Start writing data, currEpochInDays=%.15f, currEpochInSecs=%.15f, %s\n"
          "   writeOrbit=%d, writeAttitude=%d, processData=%d, timeToWrite=%d\n",
          currEpochInDays, currEpochInSecs, ToUtcGregorian(currEpochInSecs).c_str(),
          writeOrbit, writeAttitude, processData, timeToWrite);
      #endif
      
      // For now we only write Orbit data
      if (fileType == SPK_ORBIT)
         HandleSpkOrbitData(processData, timeToWrite);
      else if (fileType == CCSDS_OEM)
         HandleCcsdsOrbitData(processData, timeToWrite);
      else if (fileType == CODE500_EPHEM)
         HandleCode500OrbitData(processData, timeToWrite);
      else
      {
         throw SubscriberException
            ("*** INTERNAL ERROR *** EphemerisFile type has not set correctly in "
             "EphemerisFile::Distribute()\n");
      }
   } //if (runstate == Gmat::RUNNING || runstate == Gmat::SOLVEDPASS)
   
   // Set previous prop direction
   prevPropDirection = currPropDirection;
   
   #if DBGLVL_EPHEMFILE_DATA > 0
   MessageInterface::ShowMessage
      ("EphemerisFile::Distribute() this=<%p>'%s' returning true, data %s processed\n",
       this, GetName().c_str(), processData ? "has" : "has not");
   #endif
   return true;
}


//------------------------------------------------------------------------------
// virtual void HandleManeuvering(GmatBase *originator, bool maneuvering,
//                                Real epoch, const StringArray &satNames,
//                                const std::string &desc)
//------------------------------------------------------------------------------
/*
 * @see Subscriber
 */
//------------------------------------------------------------------------------
void EphemerisFile::HandleManeuvering(GmatBase *originator, bool maneuvering,
                                      Real epoch, const StringArray &satNames,
                                      const std::string &desc)
{
   #if DBGLVL_EPHEMFILE_MANEUVER
   MessageInterface::ShowMessage
      ("\n==================================================\n"
       "EphemerisFile::HandleManeuvering() '%s' entered, active=%d, originator=<%p><%s>, "
       "maneuvering=%d, maneuver %s, epoch=%.15f, %s\n   satNames.size()=%d, satNames[0]='%s', "
       "desc='%s'\n   prevRunState=%d, runstate=%d, maneuversHandled.size()=%d\n", GetName().c_str(),
       active, originator, originator ? originator->GetTypeName().c_str() : "NULL",
       maneuvering, maneuvering ? "started" : "stopped", epoch, ToUtcGregorian(epoch, true).c_str(),
       satNames.size(), satNames.empty() ? "NULL" : satNames[0].c_str(), desc.c_str(),
       prevRunState, runstate, maneuversHandled.size());
   #endif
   
   if (!IsEventFeasible(false))
   {
      #if DBGLVL_EPHEMFILE_MANEUVER
      MessageInterface::ShowMessage
         ("EphemerisFile::HandleManeuvering() '%s' leaving, the maneuver "
          "event is not feasible at this time\n", GetName().c_str());
      #endif
      return;
   }
   
   if (originator == NULL)
      throw SubscriberException
         ("Cannot continue with ephemeris file writing, the maneuvering burn object is NULL");
   
   // Check spacecraft name first   
   if (find(satNames.begin(), satNames.end(), spacecraftName) == satNames.end())
   {
      #if DBGLVL_EPHEMFILE_MANEUVER > 1
      MessageInterface::ShowMessage
         ("EphemerisFile::HandleManeuvering() '%s' leaving, the spacecraft '%s' is not "
          "writing Ephmeris File\n", GetName().c_str(), spacecraftName.c_str());
      #endif
      return;
   }
   
   bool restart = false;
   // Check if finite maneuver started
   if (runstate == Gmat::RUNNING && prevRunState == Gmat::IDLE && maneuvering)
   {
      #if DBGLVL_EPHEMFILE_MANEUVER > 1
      MessageInterface::ShowMessage("=====> 1 setting to restart\n");
      #endif
      restart = true;
   }
   // Check if finite maneuver ended
   else if (runstate == Gmat::RUNNING && prevRunState == Gmat::RUNNING && !maneuvering)
   {
      #if DBGLVL_EPHEMFILE_MANEUVER > 1
      MessageInterface::ShowMessage("=====> 2 setting to restart\n");
      #endif
      restart = true;
   }
   else
   {
      #if DBGLVL_EPHEMFILE_MANEUVER > 1
      MessageInterface::ShowMessage
         ("   ==> Checking if runstate is SOLVEDPASS and Maneuver already handled\n");
      #endif
      bool doNext = true;
      if (prevRunState == runstate && runstate == Gmat::SOLVEDPASS)
      {
         // Check if the originator already handled
         if (find(maneuversHandled.begin(), maneuversHandled.end(), originator) !=
             maneuversHandled.end())
         {
            #if DBGLVL_EPHEMFILE_MANEUVER
            MessageInterface::ShowMessage
               ("EphemerisFile::HandleManeuvering() prevRunState is "
                "SOLVEDPASS for the same originator\n");
            #endif
            doNext = false;
         }
      }
      
      #if DBGLVL_EPHEMFILE_MANEUVER > 1
      MessageInterface::ShowMessage("   doNext=%d\n", doNext);
      #endif
      if (doNext && (runstate == Gmat::RUNNING || runstate == Gmat::SOLVEDPASS))
      {
         #if DBGLVL_EPHEMFILE_MANEUVER
         MessageInterface::ShowMessage
            ("EphemerisFile::HandleManeuvering() GMAT is not solving or solver has "
             "finished; prevRunState=%d, runstate=%d\n", prevRunState, runstate);
         #endif
         
         if (prevRunState != Gmat::IDLE)
         {            
            // Added to maneuvers handled
            maneuversHandled.push_back(originator);
            #if DBGLVL_EPHEMFILE_MANEUVER > 1
            MessageInterface::ShowMessage("=====> 3 setting to restart\n");
            #endif
            restart = true;
         }
         else
         {
            #if DBGLVL_EPHEMFILE_MANEUVER > 1
            MessageInterface::ShowMessage
               ("EphemerisFile::HandleManeuvering() GMAT is running or solving\n");
            #endif
         }
      }
   }
   
   #if DBGLVL_EPHEMFILE_MANEUVER > 1
   MessageInterface::ShowMessage("   restart=%d\n", restart);
   #endif
   
   // Finish up writing and restart interpolation if restart is needed
   if (restart)
   {
      #ifdef DBGLVL_EPHEMFIL_RESTART
      MessageInterface::ShowMessage
         ("EphemerisFile::HandleManeuvering() Calling FinishUpWriting()\n");
      #endif
      
      //LOJ: Write continuous ephemeris if CODE500_EPHEM
      if (fileType != CODE500_EPHEM)
      {
         FinishUpWriting();
         
         maneuverEpochInDays = epoch;
         
         // Convert current epoch to gregorian format
         std::string epochStr = ToUtcGregorian(epoch, true, 2);
         
         #ifdef DEBUG_EPHEMFILE_RESTART
         MessageInterface::ShowMessage
            ("=====> Burn event, Restarting the interpolation at %s\n", epochStr.c_str());
         #endif
         
         // Restart interpolation
         std::string comment;
         bool writeComment = false;
         if (maneuvering)
            writeComment = true;
         else if (originator->IsOfType("EndFiniteBurn"))
            writeComment = true;
         
         if (writeComment)
            comment = "This block begins after " + desc + " at " + epochStr;
         
         #if DBGLVL_EPHEMFILE_MANEUVER
         MessageInterface::ShowMessage
            ("   writeComment=%d, comment='%s'\n", writeComment, comment.c_str());
         #endif
         
         RestartInterpolation(comment, false, true);
      }
   }
   
   // It is set in the Subscriber. Do we need it here? (LOJ: 2014.04.08)
   //prevRunState = runstate;
   
   #if DBGLVL_EPHEMFILE_MANEUVER
   MessageInterface::ShowMessage
      ("EphemerisFile::HandleManeuvering() '%s' leaving\n", GetName().c_str());
   #endif
}


//------------------------------------------------------------------------------
// virtual void HandlePropagatorChange(GmatBase *provider, Real epochInMjd = -999.999)
//------------------------------------------------------------------------------
void EphemerisFile::HandlePropagatorChange(GmatBase *provider, Real epochInMjd)
{
   #if DBGLVL_EPHEMFILE_PROPAGATOR_CHANGE
   MessageInterface::ShowMessage
      ("\n==================================================\n"
       "EphemerisFile::HandlePropagatorChange() '%s' entered, provider=<%p><%s>, "
       "epochInMjd=%f, active=%d, isEndOfRun=%d\n", GetName().c_str(), provider,
       provider->GetTypeName().c_str(), epochInMjd, active, isEndOfRun);
   #endif
   
   if (!IsEventFeasible())
   {
      #if DBGLVL_EPHEMFILE_PROPAGATOR_CHANGE
      MessageInterface::ShowMessage
         ("EphemerisFile::HandlePropagatorChange() '%s' leaving, the propagator "
          "change is not feasible at this time\n", GetName().c_str());
      #endif
      return;
   }
   
   if (provider == NULL)
      throw SubscriberException
         ("Cannot continue with ephemeris file writing, the provider command is NULL");
   
   if (runstate == Gmat::RUNNING || runstate == Gmat::SOLVEDPASS)
   {
      #if DBGLVL_EPHEMFILE_PROPAGATOR_CHANGE > 1
      MessageInterface::ShowMessage
         ("EphemerisFile::HandlePropagatorChange() GMAT is not solving or solver has finished\n");
      #endif
      
      // Check if propagator name changed on ephemeris file spacecraft
      if (provider->GetTypeName() == "Propagate")
      {
         // Go through propagator list and check if spacecraft found
         StringArray propNames = provider->GetRefObjectNameArray(Gmat::PROP_SETUP);
         Integer scId = provider->GetParameterID("Spacecraft");
         for (UnsignedInt prop = 0; prop < propNames.size(); prop++)
         {
            StringArray satNames = provider->GetStringArrayParameter(scId, prop);
            for (UnsignedInt sat = 0; sat < satNames.size(); sat++)
            {
               if (spacecraftName == satNames[sat])
               {
                  if (currPropName != propNames[prop])
                  {
                     currPropName = propNames[prop];
                     
                     #if DBGLVL_EPHEMFILE_PROPAGATOR_CHANGE
                     MessageInterface::ShowMessage
                        ("The propagator changed from '%s' to '%s'\n", prevPropName.c_str(),
                         currPropName.c_str());
                     #endif
                     
                     if (prevPropName != "")
                     {
                        #ifdef DEBUG_EPHEMFILE_RESTART
                        MessageInterface::ShowMessage
                           ("EphemerisFile::HandlePropagatorChange() Calling FinishUpWriting()\n   "
                            "The propagator changed from '%s' to '%s'\n", prevPropName.c_str(),
                            currPropName.c_str());
                        #endif
                        
                        // Write any data in the buffer (fixes missing lines for GMT-3745)
                        //LOJ: Write continuous ephemeris if CODE500_EPHEM
                        if (fileType != CODE500_EPHEM)
                        {
                           FinishUpWriting();
                           
                           #if DBGLVL_EPHEMFILE_PROPAGATOR_CHANGE
                           MessageInterface::ShowMessage
                              ("=====> Propagator change, Restarting the interpolation\n");
                           #endif
                           
                           // Convert current epoch to gregorian format
                           std::string epochStr;
                           if (epochInMjd != -999.999)
                           {
                              epochStr = ToUtcGregorian(epochInMjd, true, 2);
                              epochStr = " at " + epochStr;
                           }
                           
                           // Restart interpolation
                           std::string comment = "This block begins after propagator change from " +
                              prevPropName + " to " + currPropName + epochStr;
                           
                           RestartInterpolation(comment, false, true);
                        }
                     }
                     else
                     {
                        #if DBGLVL_EPHEMFILE_PROPAGATOR_CHANGE
                        MessageInterface::ShowMessage
                           ("The previous propagator name is blank, so nothing needs to be done\n");
                        #endif
                     }
                     
                     prevPropName = currPropName;
                     
                  }
                  else
                  {
                     #if DBGLVL_EPHEMFILE_PROPAGATOR_CHANGE > 1
                     MessageInterface::ShowMessage
                        ("The propagator is the same as '%s'\n", currPropName.c_str());
                     #endif
                  }
               }
            }
         }
      }
   }
   else
   {
      #if DBGLVL_EPHEMFILE_PROPAGATOR_CHANGE > 1
      MessageInterface::ShowMessage
         ("EphemerisFile::HandlePropagatorChange() GMAT is solving\n");
      #endif
   }
   
   #if DBGLVL_EPHEMFILE_PROPAGATOR_CHANGE
   MessageInterface::ShowMessage
      ("EphemerisFile::HandlePropagatorChange() '%s' leaving, provider=<%p><%s>\n",
       GetName().c_str(), provider, provider->GetTypeName().c_str());
   #endif
}


//------------------------------------------------------------------------------
// virtual void HandleSpacecraftPropertyChange(GmatBase *originator, Real epoch,
//                 const std::string &satName, const std::string &desc)
//------------------------------------------------------------------------------
/**
 * @see Subscriber
 */
//------------------------------------------------------------------------------
void EphemerisFile::HandleSpacecraftPropertyChange(GmatBase *originator, Real epoch,
                                                   const std::string &satName,
                                                   const std::string &desc)
{
   #ifdef DBGLVL_EPHEMFILE_SC_PROPERTY_CHANGE
   MessageInterface::ShowMessage
      ("\n==================================================\n"
       "EphemerisFile::HandleSpacecraftPropertyChange() '%s' entered, originator=<%p>\n   "
       "epoch=%.15f, satName='%s', desc='%s', active=%d\n   firstTimeWriting=%d, "
       "a1MjdArray.size()=%d, currEpochInDays=%f, runstate=%d\n", GetName().c_str(),
       originator, epoch, satName.c_str(), desc.c_str(), active, firstTimeWriting,
       a1MjdArray.size(), currEpochInDays, runstate);
   DebugWriteTime("   event epoch ", epoch, true);
   #endif
      
   if (originator == NULL)
      throw SubscriberException
         ("Cannot continue with ephemeris file writing, the spacecraft of which "
          "property changed is NULL");
   
   bool checkForEmptyData = true;
   if (originator->IsOfType("Vary"))
      checkForEmptyData = false;
   
   if (!IsEventFeasible(checkForEmptyData))
   {
      #if DBGLVL_EPHEMFILE_SC_PROPERTY_CHANGE
      MessageInterface::ShowMessage
         ("EphemerisFile::HandleSpacecraftPropertyChange() '%s' leaving, the spacecraft "
          "property change is not feasible at this time\n", GetName().c_str());
      #endif
      return;
   }
   
   eventEpochInSecs = epoch * GmatTimeConstants::SECS_PER_DAY;
   std::string epochStr = ToUtcGregorian(epoch, true, 2);
   
   if (spacecraftName == satName)
   {
      if (originator->IsOfType("Vary"))
      {
         if (runstate == Gmat::SOLVING)
         {
            #if DBGLVL_EPHEMFILE_SC_PROPERTY_CHANGE
            MessageInterface::ShowMessage
               ("EphemerisFile::HandleSpacecraftPropertyChange() '%s' leaving, "
                "the spacecraft is set from the Vary command and status is SOLVING\n",
                GetName().c_str());
            #endif
            return;
         }
      }
      
      #ifdef DEBUG_EPHEMFILE_RESTART
      MessageInterface::ShowMessage
         ("EphemerisFile::HandleSpacecraftPropertyChange() Calling FinishUpWriting()\n");
      #endif
      
      // Write any data in the buffer
      //LOJ: Write continuous ephemeris if CODE500_EPHEM
      if (fileType != CODE500_EPHEM)
      {
         FinishUpWriting();
         
         // Restart interpolation
         std::string comment = "This block begins after spacecraft setting " +
            desc + " at " + epochStr;
         
         RestartInterpolation(comment, false, true);
      }
   }
   
   #ifdef DBGLVL_EPHEMFILE_SC_PROPERTY_CHANGE
   MessageInterface::ShowMessage
      ("EphemerisFile::HandleSpacecraftPropertyChange() '%s' leaving\n", GetName().c_str());
   #endif
}
