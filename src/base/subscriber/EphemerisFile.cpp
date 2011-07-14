//$Id$
//------------------------------------------------------------------------------
//                                  EphemerisFile
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Linda Jun / NASA
// Created: 2009/09/02
//
/**
 * Writes a spacecraft orbit states or attitude to an ephemeris file either
 * CCSDS or SPK format.
 */
//------------------------------------------------------------------------------

#include "EphemerisFile.hpp"
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

#ifdef __USE_SPICE__
#include "SpiceOrbitKernelWriter.hpp"
#endif

// Currently we can't use DataFile for 2011a release so commented out
// Actually we want to put this flag in BuildEnv.mk but it is very close to
// release so added it here and Moderator.cpp
//#define __USE_DATAFILE__

//#define DEBUG_EPHEMFILE
//#define DEBUG_EPHEMFILE_SET
//#define DEBUG_EPHEMFILE_INIT
//#define DEBUG_EPHEMFILE_OPEN
//#define DEBUG_EPHEMFILE_SPICE
//#define DEBUG_EPHEMFILE_CCSDS
//#define DEBUG_EPHEMFILE_BUFFER
//#define DEBUG_EPHEMFILE_TIME
//#define DEBUG_EPHEMFILE_ORBIT
//#define DEBUG_EPHEMFILE_WRITE
//#define DEBUG_EPHEMFILE_RESTART
//#define DEBUG_EPHEMFILE_COMMENTS
//#define DEBUG_EPHEMFILE_FINISH
//#define DEBUG_EPHEMFILE_SC_PROPERTY_CHANGE
//#define DEBUG_EPHEMFILE_TEXT
//#define DEBUG_EPHEMFILE_SOLVER_DATA
//#define DBGLVL_EPHEMFILE_DATA 1
//#define DBGLVL_EPHEMFILE_DATA_LABELS 1
//#define DBGLVL_EPHEMFILE_MANEUVER 2
//#define DBGLVL_EPHEMFILE_PROPAGATOR_CHANGE 1

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

const std::string
EphemerisFile::PARAMETER_TEXT[EphemerisFileParamCount - SubscriberParamCount] =
{
   "Spacecraft",            // SPACECRAFT
   "Filename",              // FILENAME
   "FileFormat",            // FILE_FORMAT
   "EpochFormat",           // EPOCH_FORMAT
   "InitialEpoch",          // INITIAL_EPOCH
   "FinalEpoch",            // FINAL_EPOCH
   "StepSize",              // STEP_SIZE
   "Interpolator",          // INTERPOLATOR
   "InterpolationOrder",    // INTERPOLATION_ORDER
   "StateType",             // STATE_TYPE
   "CoordinateSystem",      // COORDINATE_SYSTEM
   "WriteEphemeris",        // WRITE_EPHEMERIS
   "FileName",              // FILE_NAME - deprecated
};

const Gmat::ParameterType
EphemerisFile::PARAMETER_TYPE[EphemerisFileParamCount - SubscriberParamCount] =
{
   Gmat::OBJECT_TYPE,       // SPACECRAFT
   Gmat::FILENAME_TYPE,     // FILENAME
   Gmat::ENUMERATION_TYPE,  // FILE_FORMAT
   Gmat::ENUMERATION_TYPE,  // EPOCH_FORMAT
   Gmat::ENUMERATION_TYPE,  // INITIAL_EPOCH
   Gmat::ENUMERATION_TYPE,  // FINAL_EPOCH
   Gmat::ENUMERATION_TYPE,  // STEP_SIZE
   Gmat::OBJECT_TYPE,       // INTERPOLATOR
   Gmat::INTEGER_TYPE,      // INTERPOLATION_ORDER
   Gmat::ENUMERATION_TYPE,  // STATE_TYPE
   Gmat::OBJECT_TYPE,       // COORDINATE_SYSTEM
   Gmat::BOOLEAN_TYPE,      // WRITE_EPHEMERIS
   Gmat::STRING_TYPE,       // FILE_NAME - deprecated
};


//------------------------------------------------------------------------------
// EphemerisFile(const std::string &name, const std::string &type = "EphemerisFile")
//------------------------------------------------------------------------------
EphemerisFile::EphemerisFile(const std::string &name, const std::string &type) :
   Subscriber          (type, name),
   spacecraft          (NULL),
   outCoordSystem      (NULL),
   interpolator        (NULL),
   spkWriter           (NULL),
   oututPath           (""),
   filePath            (""),
   spacecraftName      (""),
   fileName            (""),
   fileFormat          ("CCSDS-OEM"),
   epochFormat         ("UTCGregorian"),
   ccsdsEpochFormat    ("UTC"),
   initialEpoch        ("InitialSpacecraftEpoch"),
   finalEpoch          ("FinalSpacecraftEpoch"),
   stepSize            ("IntegratorSteps"),
   interpolatorName    ("Lagrange"),
   stateType           ("Cartesian"),
   outCoordSystemName  ("EarthMJ2000Eq"),
   writeEphemeris      (true),
   prevPropName        (""),
   currPropName        (""),
   currComments        (""),
   metaDataStartStr    (""),
   metaDataStopStr     (""),
   interpolationOrder  (7),
   initialCount        (0),
   waitCount           (0),
   stepSizeInA1Mjd     (-999.999),
   stepSizeInSecs      (-999.999),
   initialEpochA1Mjd   (-999.999),
   finalEpochA1Mjd     (-999.999),
   nextOutEpoch        (-999.999),
   nextReqEpoch        (-999.999),
   currEpochInDays     (-999.999),
   currEpochInSecs     (-999.999),
   prevEpochInSecs     (-999.999),
   prevProcTime        (-999.999),
   lastEpochWrote      (-999.999),
   maneuverEpochInDays (-999.999),
   firstTimeWriting    (true),
   writingNewSegment   (true),
   useStepSize         (false),
   writeOrbit          (false),
   writeAttitude       (false),
   writeDataInDataCS   (true),
   processingLargeStep (false),
   spkWriteFailed      (true),
   writeCommentAfterData (true),
   prevRunState        (Gmat::IDLE)
{
   #ifdef DEBUG_EPHEMFILE
   MessageInterface::ShowMessage
      ("EphemerisFile::EphemerisFile() <%p>'%s' entered\n", this, GetName().c_str());
   #endif
   
   objectTypes.push_back(Gmat::EPHEMERIS_FILE);
   objectTypeNames.push_back("EphemerisFile");
   parameterCount = EphemerisFileParamCount;
   
   // Should I give non-blank fileName?
   if (fileName == "")
      fileName = name + ".eph";
   
   // Available enumeration type list, since it is static data, clear it first
   fileFormatList.clear();
   fileFormatList.push_back("CCSDS-OEM");
   // CCSDS-AEM not allowed in 2010 release (bug 2219)
//   fileFormatList.push_back("CCSDS-AEM");
   fileFormatList.push_back("SPK");
   
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
//   stateTypeList.push_back("Quaternion");
   
   writeEphemerisList.clear();
   writeEphemerisList.push_back("Yes");
   writeEphemerisList.push_back("No");
   
   interpolatorTypeList.clear();
   interpolatorTypeList.push_back("Lagrange");
   interpolatorTypeList.push_back("Hermite");

   // SLERP not allowed in 2010 release (Bug 2219)
//   interpolatorTypeList.push_back("SLERP");
      
   #ifdef DEBUG_EPHEMFILE
   MessageInterface::ShowMessage
      ("EphemerisFile::EphemerisFile() <%p>'%s' leaving\n", this, GetName().c_str());
   #endif
}


//------------------------------------------------------------------------------
// ~EphemerisFile()
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
   #ifdef DEBUG_EPHEMFILE
   MessageInterface::ShowMessage
      ("   spkWriter=<%p>, spkWriteFailed=%d\n", spkWriter, spkWriteFailed);
   #endif
   if (spkWriter != NULL && !spkWriteFailed)
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
EphemerisFile::EphemerisFile(const EphemerisFile &ef) :
   Subscriber          (ef),
   spacecraft          (ef.spacecraft),
   outCoordSystem      (ef.outCoordSystem),
   interpolator        (NULL),
   spkWriter           (NULL),
   oututPath           (ef.oututPath),
   filePath            (ef.filePath),
   spacecraftName      (ef.spacecraftName),
   fileName            (ef.fileName),
   fileFormat          (ef.fileFormat),
   epochFormat         (ef.epochFormat),
   ccsdsEpochFormat    (ef.ccsdsEpochFormat),
   initialEpoch        (ef.initialEpoch),
   finalEpoch          (ef.finalEpoch),
   stepSize            (ef.stepSize),
   interpolatorName    (ef.interpolatorName),
   stateType           (ef.stateType),
   outCoordSystemName  (ef.outCoordSystemName),
   writeEphemeris      (ef.writeEphemeris),
   prevPropName        (ef.prevPropName),
   currPropName        (ef.currPropName),
   currComments        (ef.currComments),
   metaDataStartStr    (ef.metaDataStartStr),
   metaDataStopStr     (ef.metaDataStopStr),
   interpolationOrder  (ef.interpolationOrder),
   initialCount        (ef.initialCount),
   waitCount           (ef.waitCount),
   stepSizeInA1Mjd     (ef.stepSizeInA1Mjd),
   stepSizeInSecs      (ef.stepSizeInSecs),
   initialEpochA1Mjd   (ef.initialEpochA1Mjd),
   finalEpochA1Mjd     (ef.finalEpochA1Mjd),
   nextOutEpoch        (ef.nextOutEpoch),
   nextReqEpoch        (ef.nextReqEpoch),
   currEpochInDays     (ef.currEpochInDays),
   currEpochInSecs     (ef.currEpochInSecs),
   prevEpochInSecs     (ef.prevEpochInSecs),
   prevProcTime        (ef.prevProcTime),
   lastEpochWrote      (ef.lastEpochWrote),
   maneuverEpochInDays (ef.maneuverEpochInDays),
   writingNewSegment   (ef.writingNewSegment),
   useStepSize         (ef.useStepSize),
   writeOrbit          (ef.writeOrbit),
   writeAttitude       (ef.writeAttitude),
   writeDataInDataCS   (ef.writeDataInDataCS),
   processingLargeStep (ef.processingLargeStep),
   spkWriteFailed      (ef.spkWriteFailed),
   writeCommentAfterData (ef.writeCommentAfterData),
   prevRunState        (ef.prevRunState)
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
   
   spacecraft          = ef.spacecraft;
   outCoordSystem      = ef.outCoordSystem;
   interpolator        = NULL;
   spkWriter           = NULL;
   oututPath           = ef.oututPath;
   filePath            = ef.filePath;
   spacecraftName      = ef.spacecraftName;
   fileName            = ef.fileName;
   fileFormat          = ef.fileFormat;
   epochFormat         = ef.epochFormat;
   ccsdsEpochFormat    = ef.ccsdsEpochFormat;
   initialEpoch        = ef.initialEpoch;
   finalEpoch          = ef.finalEpoch;
   stepSize            = ef.stepSize;
   interpolatorName    = ef.interpolatorName;
   stateType           = ef.stateType;
   outCoordSystemName  = ef.outCoordSystemName;
   writeEphemeris      = ef.writeEphemeris;
   prevPropName        = ef.prevPropName;
   currPropName        = ef.currPropName;
   currComments        = ef.currComments;
   metaDataStartStr    = ef.metaDataStartStr;
   metaDataStopStr     = ef.metaDataStopStr;
   interpolationOrder  = ef.interpolationOrder;
   initialCount        = ef.initialCount;
   waitCount           = ef.waitCount;
   stepSizeInA1Mjd     = ef.stepSizeInA1Mjd;
   stepSizeInSecs      = ef.stepSizeInSecs;
   initialEpochA1Mjd   = ef.initialEpochA1Mjd;
   finalEpochA1Mjd     = ef.finalEpochA1Mjd;
   nextOutEpoch        = ef.nextOutEpoch;
   nextReqEpoch        = ef.nextReqEpoch;
   currEpochInDays     = ef.currEpochInDays;
   currEpochInSecs     = ef.currEpochInSecs;
   prevEpochInSecs     = ef.prevEpochInSecs;
   prevProcTime        = ef.prevProcTime;
   lastEpochWrote      = ef.lastEpochWrote;
   maneuverEpochInDays = ef.maneuverEpochInDays;
   writingNewSegment   = ef.writingNewSegment;
   useStepSize         = ef.useStepSize;
   writeOrbit          = ef.writeOrbit;
   writeAttitude       = ef.writeAttitude;
   writeDataInDataCS   = ef.writeDataInDataCS;
   processingLargeStep = ef.processingLargeStep;
   spkWriteFailed      = ef.spkWriteFailed;
   writeCommentAfterData = ef.writeCommentAfterData;
   prevRunState        = ef.prevRunState;
   coordConverter      = ef.coordConverter;
   
   return *this;
}

//---------------------------------
// methods for this class
//---------------------------------

//------------------------------------------------------------------------------
// std::string GetFileName()
//------------------------------------------------------------------------------
std::string EphemerisFile::GetFileName()
{
   std::string fname = fileName;

   #ifdef DEBUG_EPHEMFILE_OPEN
   MessageInterface::ShowMessage
      ("EphemerisFile::GetFileName() fname='%s;, fileFormat='%s'\n", fname.c_str(),
       fileFormat.c_str());
   #endif
   
   try
   {
      FileManager *fm = FileManager::Instance();
      oututPath = fm->GetPathname(FileManager::EPHEM_PATH);
      
      if (fileName == "")
      {
         fname = oututPath + instanceName + "." + fileFormat + ".eph";
      }
      else
      {
         // add output path if there is no path
         if (fileName.find("/") == fileName.npos &&
             fileName.find("\\") == fileName.npos)
         {
            fname = oututPath + fileName;
         }
      }
   }
   catch (BaseException &e)
   {
      if (fileName == "")
         fname = instanceName + ".eph";
      
      MessageInterface::ShowMessage(e.GetFullMessage());
   }
   
   // If SPK file, extension should be ".bsp"
   if (fileFormat == "SPK")
   {
      std::string fileExt = GmatFileUtil::ParseFileExtension(fname, true);
      if (fileExt != ".bsp")
      {
         std::string ofname = fname;
         fname = GmatStringUtil::Replace(fname, fileExt, ".bsp");
         MessageInterface::ShowMessage
            ("*** WARNING *** SPK file extension should be \".bsp\", so "
             "file name '%s' changed to '%s'\n", ofname.c_str(), fname.c_str());
      }
   }
   
   #ifdef DEBUG_EPHEMFILE_OPEN
   MessageInterface::ShowMessage
      ("EphemerisFile::GetFileName() returning fname\n   %s\n", fname.c_str());
   #endif
   
   return fname;
}


//------------------------------------------------------------------------------
// void ValidateParameters()
//------------------------------------------------------------------------------
void EphemerisFile::ValidateParameters()
{
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
          (fileFormat == "CCSDS-AEM" && stateType == "Cartesian"))
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
   }
   
   // check for NULL pointers
   if (spacecraft == NULL)
      throw SubscriberException
         ("The Spacecraft \"" + spacecraftName + "\" has not been set for "
          "the EphemerisFile \"" + GetName() + "\"");
   
   if (outCoordSystem == NULL)
      throw SubscriberException
         ("The CoordinateSystem \"" + outCoordSystemName + "\" has not been set for "
          "the EphemerisFile \"" + GetName() + "\"");
   
   if (theDataCoordSystem == NULL)
      throw SubscriberException
         ("The internal CoordinateSystem which orbit data represents has not been set for "
          "the EphemerisFile \"" + GetName() + "\"");
   
   #ifdef DEBUG_EPHEMFILE_INIT
   MessageInterface::ShowMessage
      ("EphemerisFile::ValidateParameters() leaving, spacecraft=<%p>'%s'\n"
       "outCoordSystem=<%p>'%s', theDataCoordSystem=<%p>'%s'\n",
       spacecraft, spacecraft->GetName().c_str(), outCoordSystem,
       outCoordSystem->GetName().c_str(), theDataCoordSystem,
       theDataCoordSystem->GetName().c_str());
   #endif
}


//----------------------------------
// methods inherited from Subscriber
//----------------------------------

//------------------------------------------------------------------------------
// virtual bool Initialize()
//------------------------------------------------------------------------------
bool EphemerisFile::Initialize()
{
   #ifdef DEBUG_EPHEMFILE_INIT
   MessageInterface::ShowMessage
      ("EphemerisFile::Initialize() <%p>'%s' entered, active=%d, isInitialized=%d\n"
       "   fileFormat='%s', stateType='%s'\n", this, GetName().c_str(), active,
       isInitialized, fileFormat.c_str(), stateType.c_str());
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
   ValidateParameters();
   
   // Set FileType
   if (fileFormat == "CCSDS-OEM")
      fileType = CCSDS_OEM;
   else if (fileFormat == "CCSDS-AEM")
      fileType = CCSDS_AEM;
   else if (fileFormat == "SPK" && stateType == "Cartesian")
      fileType = SPK_ORBIT;
   else if (fileFormat == "SPK" && stateType == "Quaternion")
      fileType = SPK_ATTITUDE;
   else
      throw SubscriberException
         ("FileFormat \"" + fileFormat + "\" is not valid");
   
   // Initialize data
   firstTimeWriting = true;
   prevPropName = "";
   InitializeData();
   maneuversHandled.clear();
   
   #ifdef DEBUG_EPHEMFILE_INIT
   MessageInterface::ShowMessage
      ("   fileType=%d, spacecraft=<%p>'%s', outCoordSystem=<%p>'%s'\n", fileType,
       spacecraft, spacecraft->GetName().c_str(), outCoordSystem,
       outCoordSystem->GetName().c_str());
   #endif
   
   // If active and not initialized already, open report file
   if (active && !isInitialized)
   {
      if (!OpenEphemerisFile())
      {
         #ifdef DEBUG_EPHEMFILE_INIT
         MessageInterface::ShowMessage
            ("EphemerisFile::Initialize() <%p>'%s' returning false\n",
             this, GetName().c_str());
         #endif
         throw SubscriberException
            ("Failed to open EphemerisFile \"" + GetFileName() + "\"\n");
         //return false;
      }
      
      isInitialized = true;
   }
   
   // Create interpolator if needed
   CreateInterpolator();
   
   // Determine orbit or attitude, set to boolean to avoid string comparison
   if (stateType == "Cartesian")
      writeOrbit = true;
   else
      writeAttitude = true;
   
   // Determine output coordinate system, set to boolean to avoid string comparison
   // We don't need conversion for SPK_ORBIT. SpiceOrbitKernelWriter assumes it is in
   // J2000Eq frame for now
   if (fileType == CCSDS_OEM &&
       theDataCoordSystem->GetName() != outCoordSystemName)
      writeDataInDataCS = false;
   
   // Determine initial and final epoch in A1ModJulian, this format is what spacecraft
   // currently outputs.
   Real dummyA1Mjd = -999.999;
   std::string epochStr;
   
   if (initialEpoch != "InitialSpacecraftEpoch")
      TimeConverterUtil::Convert(epochFormat, dummyA1Mjd, initialEpoch,
                                 "A1ModJulian", initialEpochA1Mjd, epochStr);
   
   if (finalEpoch != "FinalSpacecraftEpoch")
      TimeConverterUtil::Convert(epochFormat, dummyA1Mjd, finalEpoch,
                                 "A1ModJulian", finalEpochA1Mjd, epochStr);
   
   // Set solver iteration option to none. We only writes solutions to a file
   mSolverIterOption = SI_NONE;
   
   // Create SpiceOrbitKernelWriter
   if (fileType == SPK_ORBIT)
      CreateSpiceKernelWriter();
   
   // Clear maneuvers handled array
   maneuversHandled.clear();
   
   #ifdef DEBUG_EPHEMFILE_INIT
   MessageInterface::ShowMessage
      ("EphemerisFile::Initialize() <%p>'%s' returning true, writeOrbit=%d, "
       "writeAttitude=%d, writeDataInDataCS=%d, initialEpochA1Mjd=%.15f, finalEpochA1Mjd=%.15f\n",
       this, GetName().c_str(), writeOrbit, writeAttitude, writeDataInDataCS,
       initialEpochA1Mjd, finalEpochA1Mjd);
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// virtual void SetProvider(GmatBase *provider)
//------------------------------------------------------------------------------
void EphemerisFile::SetProvider(GmatBase *provider)
{
   Subscriber::SetProvider(provider);
   HandlePropagatorChange(provider);
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
      ("EphemerisFile::TakeAction() action=%s, actionData=%s\n", action.c_str(),
       actionData.c_str());
   #endif
   
   if (action == "Clear")
   {
      return true;
   }
   
   if (action == "Finalize")
   {
      return true;
   }
   
   if (action == "ChangeTypeName")
   {
      typeName = actionData;
      return true;
   }

   return false;
}


//---------------------------------------------------------------------------
//  bool RenameRefObject(const Gmat::ObjectType type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
bool EphemerisFile::RenameRefObject(const Gmat::ObjectType type,
                                 const std::string &oldName,
                                 const std::string &newName)
{
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
   // Disable interpolator type until it is selectable -- currently set by
   // ephem file format
   if (id == INTERPOLATOR)
      return true;
   
   return Subscriber::IsParameterReadOnly(id);
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
//   case WRITE_EPHEMERIS:                      This parameter has boolean type, not string             // made a change
//      return writeEphemerisList;      
   case INTERPOLATOR:
      return interpolatorTypeList;
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
       "value='%s'\n", this, GetName().c_str(), id, value.c_str());
   #endif
   
   switch (id)
   {
   case SPACECRAFT:
      spacecraftName = value;
      return true;
   case FILENAME:
      #ifdef DEBUG_EPHEMFILE_SET
      MessageInterface::ShowMessage
         ("EphemerisFile::SetStringParameter() Setting filename '%s' to "
          "EphemerisFile '%s'\n", value.c_str(), instanceName.c_str());
      #endif

      // Validate filename
      if (!GmatFileUtil::IsValidFileName(value))
      {
         std::string msg = GmatFileUtil::GetInvalidFileNameMessage(1);
         SubscriberException se;
         se.SetDetails(errorMessageFormat.c_str(), value.c_str(), "Filename", msg.c_str());
         throw se;
      }
      
      fileName = value;
      filePath = fileName;
      
      if (fileName.find("/") == fileName.npos &&
          fileName.find("\\") == fileName.npos)
         filePath = oututPath + fileName;
      
      return true;

   // Interpolator is now set along with file format (bug 2219)
   case FILE_FORMAT:
      if (find(fileFormatList.begin(), fileFormatList.end(), value) !=
          fileFormatList.end())
      {
         fileFormat = value;

         // Code to link interpolator selection to file type
         if (fileFormat == "CCSDS-OEM")
            interpolatorName = "Lagrange";
         if (fileFormat == "SPK")
            interpolatorName = "Hermite";

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
         if (SetEpoch(INITIAL_EPOCH, value, initialEpochList))
         {
            initialEpoch = value;
            return true;
         }
         else
            return false;
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
         return true;
      }
      else
      {
         return SetStepSize(STEP_SIZE, value, stepSizeList);
      }
   // Interpolator is now set along with file format (bug 2219); if the parm is
   // passed in, just ensure compatibility
   case INTERPOLATOR:
      if (fileFormat == "CCSDS-OEM")
      {
         if (value != "Lagrange")
            throw SubscriberException("Cannot set interpolator \"" + value +
                  "\" on the EphemerisFile named \"" + instanceName +
                  "\"; CCSDS-OEM ephemerides require Lagrange interpolators");
      }
      else if (fileFormat == "SPK")
      {
         if (value != "Hermite")
            throw SubscriberException("Cannot set interpolator \"" + value +
                  "\" on the EphemerisFile named \"" + instanceName +
                  "\"; SPK ephemerides require Hermite interpolators");
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
   case FILE_NAME:
      WriteDeprecatedMessage(id);
      fileName = value;
      filePath = fileName;
      
      if (fileName.find("/") == fileName.npos &&
          fileName.find("\\") == fileName.npos)
         filePath = oututPath + fileName;
      
      return true;
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
// void InitializeData()
//------------------------------------------------------------------------------
void EphemerisFile::InitializeData()
{
   #ifdef DEBUG_EPHEMFILE_RESTART
   MessageInterface::ShowMessage
      ("===== EphemerisFile::InitializeData() entered\n");
   #endif
   
   epochsOnWaiting.clear();
   
   if (interpolator != NULL)
      interpolator->Clear();
   
   initialCount        = 0;
   waitCount           = 0;
   nextOutEpoch        = -999.999;
   nextReqEpoch        = -999.999;
   currEpochInDays     = -999.999;
   currEpochInSecs     = -999.999;
   prevEpochInSecs     = -999.999;
   prevProcTime        = -999.999;
   lastEpochWrote      = -999.999;
   writingNewSegment   = true;
   
   #ifdef DEBUG_EPHEMFILE_RESTART
   MessageInterface::ShowMessage
      ("===== EphemerisFile::InitializeData() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void CreateInterpolator()
//------------------------------------------------------------------------------
void EphemerisFile::CreateInterpolator()
{
   #ifdef DEBUG_EPHEMFILE_INTERPOLATOR
   MessageInterface::ShowMessage
      ("EphemerisFile::CreateInterpolator() entered, interpolator=<%p>'%s'\n",
       interpolator, interpolator ? interpolator->GetName().c_str() : "NULL");
   #endif

   // if not using step size just return
   if (!useStepSize)
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
// void CreateSpiceKernelWriter()
//------------------------------------------------------------------------------
void EphemerisFile::CreateSpiceKernelWriter()
{
   #ifdef DEBUG_EPHEMFILE_SPICE
   MessageInterface::ShowMessage
      ("EphemerisFile::CreateSpiceKernelWriter() entered, spkWriter=<%p>\n",
       spkWriter);
   #endif
      
   //=======================================================
   #ifdef __USE_SPICE__
   //=======================================================
   // If spkWriter is not NULL, delete it first
   if (spkWriter != NULL)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (spkWriter, "spkWriter", "EphemerisFile::CreateSpiceKernelWriter()",
          "deleting local spkWriter");
      #endif
      delete spkWriter;
      spkWriter = NULL;
   }
   
//   std::string name = instanceName;
   std::string name = spacecraft->GetName();
   std::string centerName = spacecraft->GetOriginName();
   Integer objNAIFId = spacecraft->GetIntegerParameter("NAIFId");
   Integer centerNAIFId = (spacecraft->GetOrigin())->GetIntegerParameter("NAIFId");
   
   #ifdef DEBUG_EPHEMFILE_SPICE
   MessageInterface::ShowMessage
      ("   Creating SpiceOrbitKernelWriter with name='%s', centerName='%s', "
       "objNAIFId=%d, centerNAIFId=%d, fileName='%s', interpolationOrder=%d\n",
       name.c_str(), centerName.c_str(), objNAIFId, centerNAIFId,
       fileName.c_str(), interpolationOrder);
   #endif
   
   try
   {
      spkWriter =
         new SpiceOrbitKernelWriter(name, centerName, objNAIFId, centerNAIFId,
                               fileName, interpolationOrder, "J2000");
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
   
   #ifdef DEBUG_MEMORY
   MemoryTracker::Instance()->Add
      (spkWriter, "spkWriter", "EphemerisFile::CreateSpiceKernelWriter()",
       "spkWriter = new SpiceOrbitKernelWriter()");
   #endif
   
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
// bool OpenEphemerisFile()
//------------------------------------------------------------------------------
bool EphemerisFile::OpenEphemerisFile()
{
   #ifdef DEBUG_EPHEMFILE_OPEN
   MessageInterface::ShowMessage
      ("EphemerisFile::OpenEphemerisFile() entered, fileName = %s\n", fileName.c_str());
   #endif
   
   fileName = GetFileName();
   bool retval = true;
   
   #if !defined(__USE_DATAFILE__) || defined(DEBUG_EPHEMFILE_TEXT)
   // Close the stream if it is open
   if (dstream.is_open())
      dstream.close();
   
   std::string debugFileName;
   bool openDebugFile = false;
   
   if (fileType == CCSDS_OEM || fileType == CCSDS_AEM)
   {
      #if defined(__USE_DATAFILE__) && defined(DEBUG_EPHEMFILE_TEXT)
      debugFileName = fileName + ".txt";
      openDebugFile = true;
      #elif !defined(__USE_DATAFILE__)
      debugFileName = fileName;
      openDebugFile = true;
      #endif
   }
   else
   {
      #ifdef DEBUG_EPHEMFILE_TEXT
      debugFileName = fileName + ".txt";
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
      ("EphemerisFile::OpenEphemerisFile() returning %d\n", retval);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// bool CheckInitialAndFinalEpoch()
//------------------------------------------------------------------------------
bool EphemerisFile::CheckInitialAndFinalEpoch()
{
   #ifdef DEBUG_EPHEMFILE_WRITE
   MessageInterface::ShowMessage
      ("EphemerisFile::CheckInitialAndFinalEpoch() entered\n");
   #endif
   
   // Check initial and final epoch for writing, dat[0] is epoch
   bool writeData = false;
   
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
   }
   // From user specified initial epoch to FinalSpacecraftEpoch
   else if (initialEpochA1Mjd != -999.999 && finalEpochA1Mjd == -999.999)
   {
      if (currEpochInDays >= initialEpochA1Mjd)
         writeData = true;
   }
   // From user specified initial epoch to user specified final epoch
   else
   {
      if (currEpochInDays >= initialEpochA1Mjd && currEpochInDays <= finalEpochA1Mjd)
         writeData = true;
   }
   
   #ifdef DEBUG_EPHEMFILE_WRITE
   MessageInterface::ShowMessage
      ("EphemerisFile::CheckInitialAndFinalEpoch() returning writeData=%d\n", writeData);
   #endif
   
   return writeData;
}


//------------------------------------------------------------------------------
// void HandleCcsdsOrbitData(bool writeData)
//------------------------------------------------------------------------------
void EphemerisFile::HandleCcsdsOrbitData(bool writeData)
{
   #ifdef DEBUG_EPHEMFILE_CCSDS
   MessageInterface::ShowMessage
      ("EphemerisFile::HandleCcsdsOrbitData() entered, writeData=%d\n", writeData);
   #endif
   
   // Check if it is time to write
   bool timeToWrite = IsTimeToWrite(currEpochInSecs, currState);
   
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
   MessageInterface::ShowMessage("   timeToWrite=%d\n", timeToWrite);
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
         WriteCcsdsOrbitDataSegment();
      
      if (fileType == CCSDS_AEM && (firstTimeWriting || writingNewSegment))
         WriteString("DATA_START\n");
      
      if (writeOrbit)
      {
         if (useStepSize)
            WriteOrbitAt(nextReqEpoch, currState);
         else
            WriteOrbit(currEpochInSecs, currState);
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
   
   #ifdef DEBUG_EPHEMFILE_CCSDS
   MessageInterface::ShowMessage("EphemerisFile::HandleCcsdsOrbitData() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void HandleSpkOrbitData(bool writeData)
//------------------------------------------------------------------------------
void EphemerisFile::HandleSpkOrbitData(bool writeData)
{
   #ifdef DEBUG_EPHEMFILE_SPICE
   MessageInterface::ShowMessage
      ("EphemerisFile::HandleSpkOrbitData() entered, writeData=%d\n", writeData);
   #endif
   
   if (writeData)
   {
      bool bufferData = false;
      
      if ((a1MjdArray.empty()) ||
          (!a1MjdArray.empty() && currEpochInDays > a1MjdArray.back()->GetReal()))
         bufferData = true;
      
      if (bufferData)
      {
         BufferOrbitData(currEpochInDays, currState);
         
         #ifdef DEBUG_EPHEMFILE_SPICE
         DebugWriteOrbit("In HandleSpkOrbitData:", currEpochInDays, currState, true, true);
         #endif
      }
   }
   
   #ifdef DEBUG_EPHEMFILE_SPICE
   MessageInterface::ShowMessage("EphemerisFile::HandleSpkOrbitData() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void RestartInterpolation(const std::string &comments = "", bool writeAfterData = true)
//------------------------------------------------------------------------------
/**
 * Resets interpolator to start new segments of data.
 */
//------------------------------------------------------------------------------
void EphemerisFile::RestartInterpolation(const std::string &comments, bool writeAfterData)
{
   #ifdef DEBUG_EPHEMFILE_RESTART
   MessageInterface::ShowMessage
      ("===== EphemerisFile::RestartInterpolation() entered, comments='%s', "
       "writeAfterData=%d\n", comments.c_str(), writeAfterData);
   #endif
   
   // For CCSDS data, comments are written from
   // CcsdsEphemerisFile::WriteRealCcsdsOrbitDataSegment(), so just set comments here
   writeCommentAfterData = writeAfterData;
   currComments = comments;
   
   // If not using DataFile and writing text ehem file, write comments here
   #if !defined(__USE_DATAFILE__) || defined(DEBUG_EPHEMFILE_TEXT)
   WriteComments(comments);
   #endif
   
   if (spkWriter != NULL)
   {
      if (!writeAfterData)
         WriteComments(comments);
      
      WriteSpkOrbitDataSegment();
      
      if (writeAfterData)
         WriteComments(comments);
      
      currComments = "";
   }
   
   InitializeData();
   
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
      ("EphemerisFile::IsTimeToWrite() entered, writingNewSegment=%d, prevEpochInSecs=%.15f\n"
       "   epochInSecs=%.15f, state[0]=%.15f\n", writingNewSegment, prevEpochInSecs,
       epochInSecs, state[0]);
   DebugWriteTime("   current ", epochInSecs);
   #endif
   bool retval = true;
   
   // If writing at specified interval step, do checking
   if (useStepSize)
   {
      // Add data points
      if (writeOrbit)
      {
         #ifdef DEBUG_EPHEMFILE_TIME
         MessageInterface::ShowMessage("   Checking if new data points need to be added\n");
         DebugWriteTime("       epochInSecs, ", epochInSecs);
         DebugWriteTime("   prevEpochInSecs, ", prevEpochInSecs);
         #endif
         
         // If staring new segment, we want add data to interpolator
         if (epochInSecs > prevEpochInSecs)
         {
            #ifdef DEBUG_EPHEMFILE_TIME
            DebugWriteTime("   ===== Adding to interpolator ", epochInSecs);
            #endif
            
            interpolator->AddPoint(epochInSecs, state);
            prevEpochInSecs = epochInSecs;
         }
         else
         {
            #ifdef DEBUG_EPHEMFILE_TIME
            MessageInterface::ShowMessage
               ("   ========== skipping epoch<=prevEpochInSecs epochInSecs=%.15f, prevEpochInSecs=%.15f\n",
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
         ("   ===== processingLargeStep=%d, waitCount=%d\n", processingLargeStep, waitCount);
      #endif
      
      // If step size is to large, we may miss the data points since interpolator
      // buffer size is limited. So do additional process here.
      if (processingLargeStep)
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
            processingLargeStep = false;
         }
      }
      
      #ifdef DEBUG_EPHEMFILE_TIME
      MessageInterface::ShowMessage("   Computing next output time\n");
      #endif
      
      // compute next output time
      if (writingNewSegment)
      {
         #ifdef DEBUG_EPHEMFILE_TIME
         MessageInterface::ShowMessage
            ("   ===== Writing new segment, so setting new nextOutEpoch and nextReqEpoch to %.15f, %s\n",
             epochInSecs, ToUtcGregorian(epochInSecs, false, 2).c_str());
         #endif
         nextOutEpoch = epochInSecs;
         nextReqEpoch = epochInSecs;
         retval = true;
      }
      else
      {
         #ifdef DEBUG_EPHEMFILE_TIME
         DebugWriteTime("    epochInSecs, ", epochInSecs);
         DebugWriteTime("   nextOutEpoch, ", nextOutEpoch);
         #endif
         
         if (epochInSecs >= nextOutEpoch)
         {
            nextOutEpoch = nextOutEpoch + stepSizeInSecs;
            AddNextEpochToWrite(nextOutEpoch, "   ===== Adding nextOutEpoch to epochsOnWaiting, ");
            
            // Handle step size less than integrator step size
            Real nextOut = nextOutEpoch;
            while (nextOut <= epochInSecs)
            {
               // Compute new output time
               nextOut = nextOut + stepSizeInSecs;
               AddNextEpochToWrite(nextOut, "   ===== Adding nextOut to epochsOnWaiting, ");
            }
            retval = true;
         }
         else
         {
            retval = false;
         }
      }
   }
   
   #ifdef DEBUG_EPHEMFILE_TIME
   MessageInterface::ShowMessage
      ("EphemerisFile::IsTimeToWrite() returning %d\n", retval);
   DebugWriteTime("   nextOutEpoch, ", nextOutEpoch);
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
   DebugWriteTime("   currEpochInSecs, ", currEpochInSecs);
   DebugWriteTime("    reqEpochInSecs, ", reqEpochInSecs);
   MessageInterface::ShowMessage
      ("   currState[0]=%.15f, state[0]=%.15f\n", currState[0], state[0]);
   #endif
   
   Real stateToWrite[6];
   for (int i=0; i<6; i++)
      stateToWrite[i] = state[i];
   
   Real outEpochInSecs = reqEpochInSecs;
   
   // If the difference between current epoch and requested epoch is less 
   // than 1.0e-6, write out current state (LOJ: 2010.09.30)
   if (GmatMathUtil::Abs(currEpochInSecs - reqEpochInSecs) < 1.0e-6)
   {
      outEpochInSecs = currEpochInSecs;
      nextOutEpoch = outEpochInSecs + stepSizeInSecs;
      
      #ifdef DEBUG_EPHEMFILE_WRITE
      MessageInterface::ShowMessage
         ("***** The difference between current epoch and requested epoch is less "
          "than 1.0e-6, so writing current state\n");
      DebugWriteTime("   outEpochInSecs, ", outEpochInSecs);
      DebugWriteTime("   nextOutEpoch, ", nextOutEpoch);
      DebugWriteOrbit("   =====> current state ", currEpochInSecs, currState,
                      false, true);
      #endif
      
      for (int i=0; i<6; i++)
         stateToWrite[i] = currState[i];
      
      // Erase requested epoch from the epochs on waiting list if found (LOJ: 2010.02.28)
      RemoveEpochAlreadyWritten(reqEpochInSecs, "   =====> WriteOrbit() now erasing ");
      AddNextEpochToWrite(nextOutEpoch, "   =====> Adding nextOutEpoch to epochsOnWaiting");
   }
   
   WriteCcsdsOemData(outEpochInSecs, stateToWrite);
   lastEpochWrote = outEpochInSecs;
   
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
   Rvector quat = Attitude::ToQuaternion(dcm);
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
// void FinishUpWriting()
//------------------------------------------------------------------------------
/*
 * Finishes up writing data at epochs on waiting
 */
//------------------------------------------------------------------------------
void EphemerisFile::FinishUpWriting()
{
   #ifdef DEBUG_EPHEMFILE_FINISH
   MessageInterface::ShowMessage
      ("EphemerisFile::FinishUpWriting() entered, isFinalized=%d\n   lastEpochWrote= %.15f, "
       "currEpochInSecs=%.15f\n", isFinalized, lastEpochWrote, currEpochInSecs);
   if (lastEpochWrote != -999.999)
      DebugWriteTime("   last    ", lastEpochWrote);
   if (currEpochInSecs != -999.999)
      DebugWriteTime("   current ", currEpochInSecs);
   DebugWriteEpochsOnWaiting("   ");
   MessageInterface::ShowMessage
      ("   There are %d data in the buffer, spkWriter=<%p>\n", a1MjdArray.size(),
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
         if (interpolator != NULL)
         {
            interpolator->SetForceInterpolation(true);
            ProcessEpochsOnWaiting(true);
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
                     ("   ===== Removing last epoch and adding currEpochInSecs to epochsOnWaiting, ",
                      currEpochInSecs);
                  #endif
                  epochsOnWaiting.pop_back();
                  epochsOnWaiting.push_back(currEpochInSecs);
                  interpolator->SetForceInterpolation(true);
                  ProcessEpochsOnWaiting(true);
                  interpolator->SetForceInterpolation(false);
               }
            }
            
            // Write last data received if not written yet(Do attitude later)
            if (fileType == CCSDS_OEM && useStepSize)
            {
               if (currEpochInSecs > lastEpochWrote + 1.0e-6)
               {
                  #ifdef DEBUG_EPHEMFILE_FINISH
                  MessageInterface::ShowMessage
                     ("===> %.15f > %.15f so writing final data\n", currEpochInSecs,
                      lastEpochWrote);
                  #endif
                  WriteOrbit(currEpochInSecs, currState);
               }
            }
         }
         
         writeCommentAfterData = false;
         WriteCcsdsOrbitDataSegment();
         
         #if !defined(__USE_DATAFILE__) || defined(DEBUG_EPHEMFILE_TEXT)
         if (fileType == CCSDS_AEM)
            WriteString("DATA_STOP\n");
         #endif
      }
      else if (fileType == SPK_ORBIT)
      {
         if (spkWriter != NULL)
         {
            WriteSpkOrbitDataSegment();
         }
         else
         {
            #ifdef __USE_SPICE__
            if (a1MjdArray.size() > 0)
            {
               throw SubscriberException
                  ("*** INTERNANL ERROR *** SPK Writer is NULL in "
                   "EphemerisFile::FinishUpWriting()\n");
            }
            #endif
         }
      }
      
      isFinalized = true;
   }
   
   #ifdef DEBUG_EPHEMFILE_FINISH
   MessageInterface::ShowMessage
      ("EphemerisFile::FinishUpWriting() leaving\n");
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
void EphemerisFile::ProcessEpochsOnWaiting(bool checkFinalEpoch)
{
   #ifdef DEBUG_EPHEMFILE_ORBIT
   MessageInterface::ShowMessage
      ("EphemerisFile::ProcessEpochsOnWaiting() entered, checkFinalEpoch=%d\n",
       checkFinalEpoch);
   DebugWriteTime("   currEpochInSecs, ", currEpochInSecs);
   DebugWriteEpochsOnWaiting("   ");
   #endif
   
   Real estimates[6];
   Real reqEpochInSecs = 0.0;
   
   RealArray::iterator iter = epochsOnWaiting.begin();
   while (iter != epochsOnWaiting.end())
   {
      reqEpochInSecs = *iter;
      
      // Do not write after the final epoch
      if (checkFinalEpoch)
      {
         if ( (reqEpochInSecs + 1.0e-6) > currEpochInSecs)
         {
            #ifdef DEBUG_EPHEMFILE_ORBIT
            MessageInterface::ShowMessage
               ("   =====> reqEpochInSecs %.15f > currEpochInSecs %.15f so exiting while "
                "loop\n", reqEpochInSecs, currEpochInSecs);
            #endif
            
            break;
         }
      }
      
      #ifdef DEBUG_EPHEMFILE_ORBIT
      DebugWriteTime("   Checking to see if it is feasible ", reqEpochInSecs);
      #endif
      
      Integer retval = interpolator->IsInterpolationFeasible(reqEpochInSecs);
      
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
                  ("   =====> Forcing to interpolate at epoch %.15f\n", reqEpochInSecs);
               #endif
               
               // Since time should be in order, force process epochs on waiting.
               // First few request time can not be placed in the middle of the buffer.
               interpolator->SetForceInterpolation(true);
               ProcessEpochsOnWaiting(false);
               interpolator->SetForceInterpolation(false);
            }
            else
            {
               #ifdef DEBUG_EPHEMFILE_ORBIT
               MessageInterface::ShowMessage
                  ("   initialCount (%d) <= interpolationOrder/2 + 1 (%d)\n", initialCount,
                   interpolationOrder/2 + 1);
               DebugWriteTime
                  ("   =====> epoch failed to interpolate so exiting while loop, ",
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
            ("   =====> epoch is not feasible so exiting while loop, ", reqEpochInSecs);
         #endif
         break;
      }
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
         HandleError(id, value, allowedValues, " or value in " + epochFormat +
                     " (" + GmatTimeUtil::GetGregorianFormat() + ")");
      else
         HandleError(id, value, allowedValues, " or value in " + epochFormat);
      
   }
   
   if (id == INITIAL_EPOCH)
      initialEpoch = value;
   else if (id == FINAL_EPOCH)
      finalEpoch = value;
   
   return true;
}


//------------------------------------------------------------------------------
// bool SetStepSize(const std::string &value)
//------------------------------------------------------------------------------
/*
 * Sets real value step size.
 *
 * @param value step size value string
 *
 * @exception SubscriberException is thrown if value not converted to real number
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
   
   Real rval;
   if (GmatStringUtil::ToReal(value, rval) == false)
   {
      HandleError(id, value, allowedValues, " or Real Number");
   }
   
   stepSize = value;
   stepSizeInSecs = rval;
   stepSizeInA1Mjd = stepSizeInSecs / GmatTimeConstants::SECS_PER_DAY;
   
   useStepSize = true;
   
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
// void WriteComments(const std::string &comments)
//------------------------------------------------------------------------------
/**
 * Writes comments to specific file.
 */
//------------------------------------------------------------------------------
void EphemerisFile::WriteComments(const std::string &comments)
{
   #ifdef DEBUG_EPHEMFILE_COMMENTS
   MessageInterface::ShowMessage
      ("WriteComments() entered, comments='%s'\n", comments.c_str());
   #endif
   
   if (fileType == CCSDS_OEM || fileType == CCSDS_AEM)
      WriteCcsdsComments(comments);
   else if (fileType == SPK_ORBIT)
      WriteSpkComments(comments);
   
   #ifdef DEBUG_EPHEMFILE_COMMENTS
   MessageInterface::ShowMessage("WriteComments() leaving\n");
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
   DebugWriteTime("   ", epochInDays, true, 2);
   #endif
   
   // if buffer is full, dump the data
   if (a1MjdArray.size() > MAX_SEGMENT_SIZE)
   {
      if (fileType == CCSDS_OEM)
      {
         WriteCcsdsOrbitDataSegment();
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
   }
   
   // Add new data point
   A1Mjd *a1mjd = new A1Mjd(epochInDays);
   Rvector6 *rv6 = new Rvector6(state);
   a1MjdArray.push_back(a1mjd);
   stateArray.push_back(rv6);
   
   #ifdef DEBUG_EPHEMFILE_BUFFER
   MessageInterface::ShowMessage
      ("BufferOrbitData() leaving, there are %d data\n", a1MjdArray.size());
   #endif
}


//------------------------------------------------------------------------------
// void DeleteOrbitData()
//------------------------------------------------------------------------------
void EphemerisFile::DeleteOrbitData()
{
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
   DeleteOrbitData();
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
      ("CcsdsEphemerisFile::EphemerisFile() entered, fileName = %s\n", fileName.c_str());
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
// void WriteCcsdsHeader()
//------------------------------------------------------------------------------
void EphemerisFile::WriteCcsdsHeader()
{
   #if !defined(__USE_DATAFILE__) || defined(DEBUG_EPHEMFILE_TEXT)
   std::string creationTime = GmatTimeUtil::FormatCurrentTime(2);
   std::string originator = "GMAT USER";
   
   std::stringstream ss("");
   
   if (fileType == CCSDS_OEM)
      ss << "CCSDS_OEM_VERS = 1.0" << std::endl;
   else
      ss << "CCSDS_AEM_VERS = 1.0" << std::endl;
   
   ss << "CREATION_DATE = " << creationTime << std::endl;
   ss << "ORIGINATOR = " << originator << std::endl;
   
   WriteString(ss.str());
   #endif
   
   WriteRealCcsdsHeader();
}


//------------------------------------------------------------------------------
// void WriteCcsdsOrbitDataSegment()
//------------------------------------------------------------------------------
void EphemerisFile::WriteCcsdsOrbitDataSegment()
{
   if (a1MjdArray.empty())
   {
      #ifdef DEBUG_EPHEMFILE_CCSDS
      MessageInterface::ShowMessage
         ("=====> WriteCcsdsOrbitDataSegment() leaving, array is empty\n");
      #endif
      return;
   }
   
   Real metaDataStart = (a1MjdArray.front())->GetReal();
   Real metaDataStop  = (a1MjdArray.back())->GetReal();
   metaDataStartStr = ToUtcGregorian(metaDataStart, true, 2);
   metaDataStopStr = ToUtcGregorian(metaDataStop, true, 2);
   
   WriteCcsdsOemMetaData();
   
   #if !defined(__USE_DATAFILE__) || defined(DEBUG_EPHEMFILE_TEXT)
   for (UnsignedInt i = 0; i < a1MjdArray.size(); i++)
      DebugWriteOrbit("In WriteCcsdsOrbitDataSegment:", a1MjdArray[i], stateArray[i]);
   #endif
   
   #ifdef DEBUG_EPHEMFILE_CCSDS
   MessageInterface::ShowMessage
      ("=====> Writing %d CCSDS orbit data points\n", a1MjdArray.size());
   DebugWriteTime("   First data ", metaDataStart, true);
   DebugWriteTime("   Last  data ", metaDataStop, true);
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
   #if !defined(__USE_DATAFILE__) || defined(DEBUG_EPHEMFILE_TEXT)

   std::string origin = "UNKNOWN";
   std::string csType = "UNKNOWN";
   std::string objId  = spacecraft->GetStringParameter("Id");
   
   if (outCoordSystem)
   {
      csType = outCoordSystem->GetStringParameter("Axes");
      origin = outCoordSystem->GetStringParameter("Origin");
      if (origin == "Luna")
         origin = "Moon";
   }
   
   std::stringstream ss("");
   ss << std::endl;
   ss << "META_START" << std::endl;
   ss << "OBJECT_NAME = " << spacecraftName << std::endl;
   ss << "OBJECT_ID = " << objId << std::endl;
   ss << "CENTER_NAME = " << origin << std::endl;
   ss << "REF_FRAME = " << csType << std::endl;
   ss << "TIME_SYSTEM = " << ccsdsEpochFormat << std::endl;
   ss << "START_TIME = " << metaDataStartStr << std::endl;
   ss << "USEABLE_START_TIME = " << metaDataStartStr << std::endl;
   ss << "USEABLE_STOP_TIME = " << metaDataStopStr << std::endl;
   ss << "STOP_TIME = " << metaDataStopStr << std::endl;
   ss << "INTERPOLATION = " << interpolatorName << std::endl;
   ss << "INTERPOLATION_DEGREE = " << interpolationOrder << std::endl;
   ss << "META_STOP" << std::endl << std::endl;
   
   WriteString(ss.str());
   #endif
   
   WriteRealCcsdsOemMetaData();
}


//------------------------------------------------------------------------------
// void WriteCcsdsOemData(Real reqEpochInSecs, const Real state[6])
//------------------------------------------------------------------------------
void EphemerisFile::WriteCcsdsOemData(Real reqEpochInSecs, const Real state[6])
{
   #ifdef DEBUG_EPHEMFILE_CCSDS
   MessageInterface::ShowMessage
      ("EphemerisFile::WriteCcsdsOemData() entered, reqEpochInSecs=%.15f, "
       "state[0]=%.15f\n", reqEpochInSecs, state[0]);
   #endif
   
   Real outState[6];
   for (int i = 0; i < 6; i++)
      outState[i] = state[i];
   
   // Since CCSDS utilities do no convert to desired CoordinateSystem,
   // convert it here
   if (!writeDataInDataCS)
      ConvertState(reqEpochInSecs/GmatTimeConstants::SECS_PER_DAY, state, outState);
   
   BufferOrbitData(reqEpochInSecs/GmatTimeConstants::SECS_PER_DAY, outState);
   
   #ifdef DEBUG_EPHEMFILE_CCSDS
   MessageInterface::ShowMessage("EphemerisFile::WriteCcsdsOemData() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void WriteCcsdsAemMetaData()
//------------------------------------------------------------------------------
void EphemerisFile::WriteCcsdsAemMetaData()
{
   #if !defined(__USE_DATAFILE__) || defined(DEBUG_EPHEMFILE_TEXT)
   std::string objId  = spacecraft->GetStringParameter("Id");
   std::string origin = spacecraft->GetOriginName();
   std::string csType = "UNKNOWN";
   GmatBase *cs = (GmatBase*)(spacecraft->GetRefObject(Gmat::COORDINATE_SYSTEM, ""));
   if (cs)
      csType = cs->GetTypeName();
   
   std::stringstream ss("");
   ss << "META_START" << std::endl;
   ss << "OBJECT_NAME = " << spacecraftName << std::endl;
   ss << "OBJECT_ID = " << objId << std::endl;
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
// void WriteCcsdsComments(const std::string &comments)
//------------------------------------------------------------------------------
void EphemerisFile::WriteCcsdsComments(const std::string &comments)
{
   std::string ccsdsComments = "COMMENT  " + comments;
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
            ("[%3d] %12.10f  %s  %s", ii, time, ToUtcGregorian(time, true).c_str(),
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
         DeleteOrbitData();
      }
      catch (BaseException &e)
      {
         DeleteOrbitData();
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
   std::string origin = spacecraft->GetOriginName();
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
// void FinalizeSpkFile()
//------------------------------------------------------------------------------
void EphemerisFile::FinalizeSpkFile()
{
   #ifdef DEBUG_EPHEMFILE_SPICE
   MessageInterface::ShowMessage("=====> FinalizeSpkFile() entered\n");
   #endif
   
   #ifdef __USE_SPICE__
   try
   {
      if (!a1MjdArray.empty())
      {
         WriteSpkOrbitDataSegment();
      }
      
      spkWriter->FinalizeKernel();
   }
   catch (BaseException &e)
   {
      // Keep from setting a warning
      e.GetMessageType();

      DeleteOrbitData();
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
   DebugWriteTime("   ", epochInSecs);
   DebugWriteEpochsOnWaiting();
   #endif
   
   // Find matching epoch
   RealArray::iterator iterFound = epochsOnWaiting.begin();
   while (iterFound != epochsOnWaiting.end())
   {
      #ifdef DEBUG_FIND_EPOCH
      DebugWriteTime("      iterFound, ", *iterFound);
      #endif
      
      if (GmatMathUtil::Abs(*iterFound - epochInSecs) < 1.0e-6)
      {
         #ifdef DEBUG_EPHEMFILE_ORBIT
         DebugWriteTime(msg, *iterFound);
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
         DebugWriteTime(msg, *iterFound);
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
      DebugWriteTime(msg, epochInSecs);
      #endif
      epochsOnWaiting.push_back(epochInSecs);
      nextOutEpoch = epochInSecs;
   }
   else
   {
      #ifdef DEBUG_EPHEMFILE_TIME
      DebugWriteTime("   ========== skipping redundant ", epochInSecs);
      #endif
   }
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
std::string EphemerisFile::ToUtcGregorian(Real epoch, bool inDays, Integer format)
{
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
// void DebugWriteTime(const std::string &msg, Real epoch, bool inDays = false,
//                     Integer format = 1)
//------------------------------------------------------------------------------
void EphemerisFile::DebugWriteTime(const std::string &msg, Real epoch, bool inDays,
                                   Integer format)
{
   Real epochInDays = epoch;
   if (!inDays)
      epochInDays = epoch / GmatTimeConstants::SECS_PER_DAY;
   
   std::string epochStr = ToUtcGregorian(epochInDays, true, format);
   
   MessageInterface::ShowMessage
      ("%sepoch = %.15f, %.15f, '%s'\n", msg.c_str(), epoch, epochInDays,
       epochStr.c_str());
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
         ("%s\n%s\n%24.14f  %24.14f  %24.14f\n%19.16f  %19.16f  %19.16f\n",
          msg.c_str(), epochStr.c_str(), outState[0], outState[1], outState[2],
          outState[3], outState[4], outState[5]);
   }
   else
   {
      char strBuff[200];
      sprintf(strBuff, "%s  %24.14f  %24.14f  %24.14f  %19.16f  %19.16f  %19.16f\n",
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
       "EphemerisFile::Distribute() this=<%p>'%s' called\n", this, GetName().c_str());
   MessageInterface::ShowMessage
      ("   len=%d, active=%d, isEndOfReceive=%d, isEndOfDataBlock=%d, isEndOfRun=%d\n   "
       "runstate=%d, isManeuvering=%d, firstTimeWriting=%d\n", len, active, isEndOfReceive,
       isEndOfDataBlock, isEndOfRun, runstate, isManeuvering, firstTimeWriting);
   if (len > 0)
   {
      DebugWriteTime("   ", dat[0], true);
      MessageInterface::ShowMessage("   dat[0]=%.15f, dat[1]=%.15f\n", dat[0], dat[1]);
      MessageInterface::ShowMessage("   dat[] = [");
      for (Integer i = 0; i < len; ++i)
      {
         MessageInterface::ShowMessage("%15le", dat[i]);
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
         ("=====> EphemerisFile::Distribute() returning true, it is toggled off\n");
      #endif
      return true;
   }
   
   if (isEndOfReceive && isEndOfDataBlock)
   {
      #ifdef DEBUG_EPHEMFILE_FINISH
      MessageInterface::ShowMessage
         ("EphemerisFile::Distribute() Calling FinishUpWriting(), "
          "isEndOfReceive=%d, len=%d\n", isEndOfReceive, len);
      #endif
      
      FinishUpWriting();
      
      return true;
   }
   
   if (len == 0)
      return true;
   
   isFinalized = false;
   
   //------------------------------------------------------------
   // if solver is running, just return
   //------------------------------------------------------------
   if (runstate == Gmat::SOLVING)
   {
      #ifdef DEBUG_EPHEMFILE_SOLVER_DATA
      MessageInterface::ShowMessage
         ("EphemerisFile::Distribute() Just returning; solver is running\n");
      #endif
      
      return true;
   }
   
   // Get proper id with data label
   if (theDataLabels.empty())
      return true;
   
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
      return true;
   
   #if DBGLVL_EPHEMFILE_DATA
   MessageInterface::ShowMessage
      ("   %s, %.15f, X,Y,Z = %.15f, %.15f, %.15f\n", GetName().c_str(), dat[0],
       dat[idX], dat[idY], dat[idZ]);
   #endif
   
   // Now copy distributed data to data member
   currEpochInDays = dat[0];
   currState[0] = dat[idX];
   currState[1] = dat[idY];
   currState[2] = dat[idZ];
   currState[3] = dat[idVx];
   currState[4] = dat[idVy];
   currState[5] = dat[idVz];
   
   // Internally all epochs are in seconds to avoid epoch drifting.
   // For long run epochs to process drifts behind the actual.
   prevEpochInSecs = currEpochInSecs;
   currEpochInSecs = currEpochInDays * GmatTimeConstants::SECS_PER_DAY;
   
   // Ignore duplicate data
   if (currEpochInSecs == prevEpochInSecs)
   {
      #if DBGLVL_EPHEMFILE_DATA
      MessageInterface::ShowMessage
         ("EphemerisFile::Distribute() Just returning true, currEpochInSecs (%.15f) "
          "= prevEpochInSecs (%.15f)\n", currEpochInSecs, prevEpochInSecs);
      #endif
      return true;
   }
   
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
      
      bool writeData = CheckInitialAndFinalEpoch();
      
      #if DBGLVL_EPHEMFILE_DATA > 0
      MessageInterface::ShowMessage
         ("   Start writing data, currEpochInDays=%.15f, currEpochInSecs=%.15f, %s\n"
          "   writeData=%d, writeOrbit=%d, writeAttitude=%d\n", currEpochInDays,
          currEpochInSecs, ToUtcGregorian(currEpochInSecs).c_str(), writeData,
          writeOrbit, writeAttitude);
      #endif
      
      // For now we only writes Orbit data
      if (fileType == SPK_ORBIT)
         HandleSpkOrbitData(writeData);
      else
         HandleCcsdsOrbitData(writeData);
   }
   
   #if DBGLVL_EPHEMFILE_DATA > 0
   MessageInterface::ShowMessage
      ("EphemerisFile::Distribute() this=<%p>'%s' returning true\n", this,
       GetName().c_str());
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
   #if DBGLVL_EPHEMFILE_MANEUVER > 1
   MessageInterface::ShowMessage
      ("EphemerisFile::HandleManeuvering() '%s' entered, originator=<%p>, maneuver %s, "
       "epoch=%.15f, %s\n   satNames.size()=%d, satNames[0]='%s', desc='%s'\n"
       "   prevRunState=%d, runstate=%d, maneuversHandled.size()=%d\n", GetName().c_str(),
       originator, maneuvering ? "started" : "stopped", epoch,
       ToUtcGregorian(epoch, true).c_str(), satNames.size(),
       satNames.empty() ? "NULL" : satNames[0].c_str(), desc.c_str(), prevRunState,
       runstate, maneuversHandled.size());
   #endif
   
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
   
   if (restart)
   {
      maneuverEpochInDays = epoch;
      
      // Convert current epoch to gregorian format
      std::string epochStr = ToUtcGregorian(epoch, true, 2);
      
      #if DBGLVL_EPHEMFILE_MANEUVER
      MessageInterface::ShowMessage
            ("=====> Restarting the interpolation at %s\n", epochStr.c_str());
      #endif
      
      // Restart interpolation
      RestartInterpolation("This block begins after " + desc + " at " + epochStr + "\n", true);
   }
   
   prevRunState = runstate;
   
   #if DBGLVL_EPHEMFILE_MANEUVER > 1
   MessageInterface::ShowMessage
      ("EphemerisFile::HandleManeuvering() '%s' leaving\n", GetName().c_str());
   #endif
}


//------------------------------------------------------------------------------
// virtual void HandlePropagatorChange(GmatBase *provider)
//------------------------------------------------------------------------------
void EphemerisFile::HandlePropagatorChange(GmatBase *provider)
{
   #if DBGLVL_EPHEMFILE_PROPAGATOR_CHANGE > 1
   MessageInterface::ShowMessage
      ("EphemerisFile::HandlePropagatorChange() entered, provider=<%p><%s>\n",
       provider, provider->GetTypeName().c_str());
   #endif
   
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
                        #if DBGLVL_EPHEMFILE_PROPAGATOR_CHANGE
                        MessageInterface::ShowMessage
                           ("=====> Restarting the interpolation\n");
                        #endif
                        
                        // Restart interpolation
                        RestartInterpolation("This block begins after propagator change from " +
                                             prevPropName + " to " + currPropName + "\n", true);
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
   
   #if DBGLVL_EPHEMFILE_PROPAGATOR_CHANGE > 1
   MessageInterface::ShowMessage
      ("EphemerisFile::HandlePropagatorChange() leaving, provider=<%p><%s>\n",
       provider, provider->GetTypeName().c_str());
   #endif
}


//------------------------------------------------------------------------------
// virtual void HandleScPropertyChange(GmatBase *originator, Real epoch,
//                                     const std::string &satName,
//                                     const std::string &desc)
//------------------------------------------------------------------------------
/**
 * @see Subscriber
 */
//------------------------------------------------------------------------------
void EphemerisFile::HandleScPropertyChange(GmatBase *originator, Real epoch,
                                           const std::string &satName,
                                           const std::string &desc)
{
   #ifdef DEBUG_EPHEMFILE_SC_PROPERTY_CHANGE
   MessageInterface::ShowMessage
      ("EphemerisFile::HandleScPropertyChange() entered, originator=<%p>, "
       "epoch=%.15f, satName='%s', desc='%s'\n", originator, epoch, satName.c_str(),
       desc.c_str());
   #endif
   
   std::string epochStr = ToUtcGregorian(epoch, true, 2);
   
   if (spacecraftName == satName)
   {      
      // Restart interpolation
      RestartInterpolation("This block begins after spacecraft setting " +
                           desc + " at " + epochStr + "\n", true);
   }
   
   #ifdef DEBUG_EPHEMFILE_SC_PROPERTY_CHANGE
   MessageInterface::ShowMessage("EphemerisFile::HandleScPropertyChange() leaving\n");
   #endif
}
