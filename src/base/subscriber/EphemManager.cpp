//$Id$
//------------------------------------------------------------------------------
//                               EphemManager
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
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
// Author: Wendy C. Shoan
// Created: 2014.10.28
//
/**
 * Base class implementation for the EphemManager.  The EphemManager is
 * responsible for creating, loading, and managing private/hidden EphemerisFile
 * objects associated with its specified Spacecraft object.
 * NOTE: currently, the EphemManager will only handle SPK Orbit files.
 * NOTE: code to get occultation and contact intervals based on
 * prototypes written by Joel Parker/GSFC and Yeerang Lim/KAIST
 */
//------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include "EphemManager.hpp"
#include "GmatBase.hpp"
#include "CoordinateSystem.hpp"
#include "EphemerisFile.hpp"
#include "Publisher.hpp"
#include "Spacecraft.hpp"
#include "FileManager.hpp"
#include "MessageInterface.hpp"
#include "SubscriberException.hpp"
#include "FileUtil.hpp"
#include "StringUtil.hpp"
#include "TimeTypes.hpp"
#include "GmatConstants.hpp"
#ifdef __USE_SPICE__
   #include "SpiceInterface.hpp"
#endif

//#define DEBUG_EPHEM_MANAGER
//#define DEBUG_EPHEM_MANAGER_FILES
//#define DEBUG_EM_COVERAGE
//#define DEBUG_OCCULTATION
//#define DEBUG_SPK_COVERAGE
//#define DEBUG_CONTACT
//#define DEBUG_EM_TIME_SPENT
//#define DEBUG_EM_TIME_ADJUST
//#define DEBUG_EM_FILENAME

#ifdef DEBUG_EM_TIME_SPENT
#include <time.h>
#endif


/**
 * Manager for ephemeris recording for the specified object
 */

//------------------------------------------------------------------------------
// default constructor
//------------------------------------------------------------------------------
EphemManager::EphemManager(bool deleteFiles) :
   initialEpoch           ("InitialSpacecraftEpoch"),
   finalEpoch             ("FinalSpacecraftEpoch"),
   theType                (SPK),
   theObjName             (""),
   theObj                 (NULL),
   solarSys               (NULL),
   ephemFile              (NULL),
   coordSys               (NULL),
   coordSysName           (""),
   ephemName              (""),
   ephemCount             (0),
   fileName               (""),
   recording              (false),
   deleteTmpFiles         (deleteFiles),
   intStart               (0.0),
   intStop                (0.0),
   coverStart             (0.0),
   coverStop              (0.0)
{
#ifdef __USE_SPICE__
   spice = NULL;
   cover = NULL;
#endif
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
EphemManager::~EphemManager()
{
   #ifdef DEBUG_EPHEM_MANAGER
      MessageInterface::ShowMessage("Destructing EphemManager ...\n");
      MessageInterface::ShowMessage("     and deleteTmpFiles = %s\n",
            (deleteTmpFiles? "true" : "false"));
   #endif
   // Stop recording
   if (recording) StopRecording(true);

   // Unsubscribe
   if (ephemFile)
   {
      Publisher *pub = Publisher::Instance();
      pub->Unsubscribe(ephemFile);
   }

   // Unload the SPK files that we have already loaded
   for (unsigned int ii = 0; ii < fileList.size(); ii++)
   {
      std::string eachFile = fileList.at(ii);
      #ifdef DEBUG_EPHEM_MANAGER
         MessageInterface::ShowMessage(
               "Destructing EphemManager ...about to unload SPK kernel %s\n",
               eachFile.c_str());
      #endif
      #ifdef __USE_SPICE__
         spice->UnloadKernel(eachFile.c_str());
      #endif
   }

   if (deleteTmpFiles)
   {
      // Remove all of the temporary SPK files
      for (unsigned int ii = 0; ii < fileList.size(); ii++)
      {
         std::string eachFile = fileList.at(ii);
         #ifdef DEBUG_EPHEM_MANAGER
            MessageInterface::ShowMessage(
                  "Destructing EphemManager ...about to delete file %s\n",
                  eachFile.c_str());
         #endif
         remove(eachFile.c_str());
      }
   }
   fileList.clear();

   #ifdef DEBUG_EPHEM_MANAGER
      MessageInterface::ShowMessage("Destructing EphemManager ... deleting ephemFile\n");
   #endif
   // delete the current EphemerisFile
   if (ephemFile) delete ephemFile;
   #ifdef DEBUG_EPHEM_MANAGER
      MessageInterface::ShowMessage("Destructing EphemManager ... deleting spice\n");
   #endif
   // delete the SpiceInterface

   #ifdef __USE_SPICE__
      if (spice)     delete spice;
   #endif
   #ifdef DEBUG_EPHEM_MANAGER
      MessageInterface::ShowMessage("Destructing EphemManager ... DONE\n");
   #endif
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
EphemManager::EphemManager(const EphemManager& copy) :
   initialEpoch           (copy.initialEpoch),
   finalEpoch             (copy.finalEpoch),
   theType                (copy.theType),
   theObjName             (copy.theObjName),
   theObj                 (NULL),
   solarSys               (NULL),
   ephemFile              (NULL),
   coordSys               (NULL),
   coordSysName           (copy.coordSysName),
   ephemName              (""),
   ephemCount             (0),
   fileName               (copy.fileName),
   recording              (copy.recording),
   deleteTmpFiles         (copy.deleteTmpFiles),
   intStart               (copy.intStart),
   intStop                (copy.intStop),
   coverStart             (copy.coverStart),
   coverStop              (copy.coverStop)
{
   #ifdef __USE_SPICE__
      spice = NULL;
      cover = NULL;
   #endif
}

//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
EphemManager& EphemManager::operator=(const EphemManager& copy)
{
   if (&copy == this)
      return *this;

   initialEpoch             = copy.initialEpoch;
   finalEpoch               = copy.finalEpoch;
   theType                  = copy.theType;
   theObjName               = copy.theObjName;
   theObj                   = copy.theObj;
   solarSys                 = copy.solarSys;
//   ephemFile                = (EphemerisFile*)(copy.ephemFile)->Clone(); // need to Subscribe here?
   ephemFile                = NULL;
   coordSys                 = copy.coordSys;
   coordSysName             = copy.coordSysName;
   ephemName                = "";                 // OR copy.ephemName;
   ephemCount               = copy.ephemCount;
   fileName                 = copy.fileName;
   recording                = copy.recording;
   deleteTmpFiles           = copy.deleteTmpFiles;
   intStart                 = copy.intStart;
   intStop                  = copy.intStop;
   coverStart               = copy.coverStart;
   coverStop                = copy.coverStop;

   #ifdef __USE_SPICE__
      if (spice) delete spice;
      spice = NULL;
      cover = NULL;
   #endif

   // @todo - what to do with the files already created here ... and what if the copy
   // is in the middle of recording?  need to check deleteTmpFiles flag?
   fileList                 = copy.fileList; // ?????

   return *this;
}

//------------------------------------------------------------------------------
// Initialize()
//------------------------------------------------------------------------------
bool EphemManager::Initialize()
{
   if (!coordSys)
      throw SubscriberException("Coordinate system for EphemManager has not been set!\n");
   if (!theObj)
      throw SubscriberException("Spacecraft for EphemManager has not been set!\n");
   if (!solarSys)
      throw SubscriberException("SolarSystem for EphemManager has not been set!\n");

   #ifdef __USE_SPICE__
      // Create SPICE interface
      if (!spice) spice = new SpiceInterface();
   #endif

   return true;
}

//------------------------------------------------------------------------------
// RecordEphemerisData()
//------------------------------------------------------------------------------
bool EphemManager::RecordEphemerisData()
{
   #ifdef DEBUG_EPHEM_MANAGER
      MessageInterface::ShowMessage(
            "Entering EphemManager::RecordEphemerisData for SC %s\n",
            theObj->GetName().c_str());
   #endif

   Spacecraft *theSc = (Spacecraft*) theObj;

   #ifndef __USE_SPICE__
      std::string errmsg = "ERROR - cannot record ephemeris data for spacecraft ";
      errmsg += theSc->GetName() + " without SPICE included in build!\n";
      throw SubscriberException(errmsg);
   #else

      // If it's already recording, continue
      if (!ephemFile)
      {
         #ifdef DEBUG_EPHEM_MANAGER_FILES
            MessageInterface::ShowMessage(
                  "In EphemManager::RecordEphemerisData for SC %s, setting up ephemFile\n",
                  theObj->GetName().c_str());
         #endif
         if (theType != SPK)
            throw SubscriberException("Only SPK currently allowed for EphemManager\n");

         if (!spice)
            spice = new SpiceInterface();

         // Set up the name for the EphemerisFile, and the file name
         std::stringstream ss("");
//         ss << "tmp_" << theObjName << "_" << ephemCount << "_" << GmatTimeUtil::FormatCurrentTime(4);
         ss << "tmp_" << theObjName << "_" << GmatTimeUtil::FormatCurrentTime(4);
         ephemName = ss.str();
         ss << ".bsp";
         fileName = ss.str();
         #ifdef DEBUG_EM_FILENAME
            MessageInterface::ShowMessage("(base) Filename for NEW ephemFile is determined to be: %s\n",
                  fileName.c_str());
         #endif
         ephemFile         = new EphemerisFile(ephemName);
         #ifdef DEBUG_EPHEM_MANAGER_FILES
            MessageInterface::ShowMessage(
                  "In EphemManager::RecordEphemerisData, ephemFile is at <%p> with name %s\n",
                  ephemFile, ephemName.c_str());
         #endif

         // For now, put it in the Output path << this should be put into the
         // appropriate TMPDIR for the platform
//         FileManager *fm = FileManager::Instance();
//         std::string spkPath = fm->GetPathname(FileManager::OUTPUT_PATH);
//         fileName = spkPath + fileName;
         std::string spkTmpPath = GmatFileUtil::GetTemporaryDirectory();
         fileName = spkTmpPath + fileName;
         #ifdef DEBUG_EM_FILENAME
            MessageInterface::ShowMessage("(full-path) Filename for NEW ephemFile is determined to be: %s\n",
                  fileName.c_str());
         #endif
         #ifdef DEBUG_EPHEM_MANAGER_FILES
            MessageInterface::ShowMessage(
                  "In EphemManager::RecordEphemerisData,  fileName (full path) = %s\n",
                  fileName.c_str());
         #endif

         // Set up the EphemerisFile to write what we need - currently only SPK Orbit
         ephemFile->SetStringParameter("FileFormat", "SPK");
         ephemFile->SetStringParameter("StateType", "Cartesian");
         ephemFile->SetStringParameter("Spacecraft", theObjName);
         ephemFile->SetStringParameter("CoordinateSystem", coordSysName); // only MJ2000Eq so far!!
         ephemFile->SetStringParameter("Filename", fileName);
         ephemFile->SetStringParameter("Interpolator", "Hermite");
         ephemFile->SetIntegerParameter(ephemFile->GetParameterID("InterpolationOrder"), 7);
//         ephemFile->SetBackgroundGeneration(true); // must be set after initialization

         ephemFile->SetInternalCoordSystem(coordSys);
         ephemFile->SetRefObject(theObj,   Gmat::SPACECRAFT,        theObjName);
         ephemFile->SetRefObject(coordSys, Gmat::COORDINATE_SYSTEM, coordSysName);

         ephemFile->Initialize();
         ephemFile->TakeAction("ToggleOn");
         ephemFile->SetBackgroundGeneration(true);

         // Subscribe to the data
         Publisher *pub = Publisher::Instance();
         #ifdef DEBUG_EPHEM_MANAGER
            MessageInterface::ShowMessage(
                  "In EphemManager::RecordEphemerisData, subscribing to publisher\n");
         #endif
         pub->Subscribe(ephemFile);

         ephemCount++;
      }
      else if (!recording)
      {
         // Set up the name for the EphemerisFile, and the file name
         std::stringstream ss("");
//         ss << "tmp_" << theObjName << "_" << ephemCount << "_" << GmatTimeUtil::FormatCurrentTime(4);
         ss << "tmp_" << theObjName << "_" << GmatTimeUtil::FormatCurrentTime(4);
         ephemName = ss.str();
         ss << ".bsp";
         fileName = ss.str();
         std::string spkTmpPath = GmatFileUtil::GetTemporaryDirectory();
         fileName = spkTmpPath + fileName;
         #ifdef DEBUG_EM_FILENAME
            MessageInterface::ShowMessage("(full path) Filename for existing ephemFile is determined to be: %s\n",
                  fileName.c_str());
         #endif
         // if it has an ephemFile but it is not recording,
         // reset the SPK filename
         ephemFile->SetStringParameter("Filename", fileName);
      }
      else
      {
         // continue recording
         #ifdef DEBUG_EPHEM_MANAGER_FILES
            MessageInterface::ShowMessage(
                  "In EphemManager::RecordEphemerisData for SC %s, ephemFile is already recording!!!\n",
                  theObj->GetName().c_str());
         #endif
      }
      recording = true;
      return true;
   #endif
}

//------------------------------------------------------------------------------
// ProvideEphemerisData()
//------------------------------------------------------------------------------
bool EphemManager::ProvideEphemerisData()
{
   #ifdef DEBUG_EPHEM_MANAGER
      MessageInterface::ShowMessage("ProvideEphemerisData called -----  fileName = %s\n",
            fileName.c_str());
   #endif
   StopRecording(true);   //  false); SPK appending turned off for now.
   RecordEphemerisData();
   return true;
}

//------------------------------------------------------------------------------
// StopRecording(bool done = true)
//------------------------------------------------------------------------------
void EphemManager::StopRecording(bool done)
{
   #ifdef DEBUG_EPHEM_MANAGER
      MessageInterface::ShowMessage("StopRecording -----  fileName = %s, done = %s\n",
            fileName.c_str(), (done? "true" : "false"));
   #endif
   // Finalize and close the SPK file
   if (done)
   {
//      // Unsubscribe
//      Publisher *pub = Publisher::Instance();
//      pub->Unsubscribe(ephemFile);
      // Delete the current ephemFile - this should finalize the SPK writing and then
      // close the <fileName> file
      #ifdef DEBUG_EM_FILENAME
         MessageInterface::ShowMessage("In StopRecording, closing ephem file ...");
      #endif
      ephemFile->CloseEphemerisFile(false, true);
      // Made general method name (LOJ: 2015.11.16)
      //bool notAllDataWritten = ephemFile->InsufficientSPKData();
      bool notAllDataWritten = ephemFile->InsufficientDataPoints();
      if (notAllDataWritten)
      {
         std::string warn = "*** WARNING *** Insufficient ephemeris data ";
         warn            += "available for spacecraft " + theObj->GetName();
         warn            += " to write last segment.  Event location may be ";
         warn            += "incomplete.  Try increasing the propagation time.\n";
         MessageInterface::ShowMessage(warn);
      }
//      delete ephemFile;
   }
   else  // appending - this is turned OFF for now
   {
      #ifdef __USE_SPICE__
         #ifdef DEBUG_EPHEM_MANAGER
            MessageInterface::ShowMessage("-==-==-= Now attempting to UNload fileList \"%s\"\n",
                  fileName.c_str());
         #endif
         if (spice->IsLoaded(fileName))
            spice->UnloadKernel(fileName);   // need to unload before re-loading?
      #endif
      // tell the EphemerisFile to close the SPK and leave ready for appending
      #ifdef DEBUG_EPHEM_MANAGER
         MessageInterface::ShowMessage("-==-==-= Calling CloseEphemerisFile\n");
      #endif
      ephemFile->CloseEphemerisFile(false, true);
      //bool notAllDataWritten = ephemFile->InsufficientSPKData();
      bool notAllDataWritten = ephemFile->InsufficientDataPoints();
      if (notAllDataWritten)
      {
         std::string warn = "*** WARNING *** Insufficient ephemeris data ";
         warn            += "available for spacecraft " + theObj->GetName();
         warn            += " to write last segment.  Event location may be ";
         warn            += "incomplete.  Try increasing the propagation time.\n";
         MessageInterface::ShowMessage(warn);
      }

   }
   // Load the current SPK file, if it has been written
   if (GmatFileUtil::DoesFileExist(fileName))
   {
      #ifdef __USE_SPICE__
         #ifdef DEBUG_EPHEM_MANAGER
            MessageInterface::ShowMessage("-==-==-= Now attempting to re-load file \"%s\"\n",
                  fileName.c_str());
         #endif
         spice->LoadKernel(fileName);
      #endif
      #ifdef DEBUG_EPHEM_MANAGER
         MessageInterface::ShowMessage("-==-==-= adding fileName %s to fileList\n",
               fileName.c_str());
      #endif
      // save the just written file name
      if (find(fileList.begin(), fileList.end(), fileName) ==
               fileList.end())
         fileList.push_back(fileName);
   }
   #ifdef DEBUG_EPHEM_MANAGER
      else
      {
      MessageInterface::ShowMessage("-==-==-= %s DOES NOT exist and so cannot be reloaded!!!\n",
            fileName.c_str());
      }
   #endif
//   if (done) ephemFile = NULL;
   recording = false;
}


//------------------------------------------------------------------------------
//    bool    GetOccultationIntervals(const std::string &occType,
//                                    const std::string &frontBody,
//                                    const std::string &frontShape,
//                                    const std::string &frontFrame,
//                                    const std::string &backBody,
//                                    const std::string &backShape,
//                                    const std::string &backFrame,
//                                    const std::string &abbCorrection,
//                                    Integer           naifId,
//                                    Integer           step,
//                                    Real              &intvlStart,
//                                    Real              &intvlStop,
//                                    Integer           &numIntervals,
//                                    RealArray         &starts,
//                                    RealArray         &ends);
//------------------------------------------------------------------------------
/**
 * This method determines the intervals of occultation given the input front
 * body, back body, abberration correction, naif ID, and step size.
 *
 * @param occType       UMBRA, PENUMBRA, ANTUMBRA, ALL
 * @param frontBody     the front body
 * @param frontShape    the shape of the front body
 * @param frontFrame    the frame of the front body
 * @param backBody      the back body
 * @param backShape     the shape of the back body
 * @param backFrame     the frame of the back body
 * @param abbCorrection the aberration correction for the operation
 * @param naifId        the NAIF ID for the spacecraft
 * @param step          step size (s)
 * @param intvlStart    start of the resulting interval
 * @param intvlStop     stop of the resulting interval
 * @param numIntervals  the number of intervals detected (output)
 * @param starts        array of start times for the intervals (output)
 * @param ends          array of end times for the intervals (output)
 *
 */
//------------------------------------------------------------------------------

/// method to determine occultation intervals
bool EphemManager::GetOccultationIntervals(const std::string &occType,
                                           const std::string &frontBody,
                                           const std::string &frontShape,
                                           const std::string &frontFrame,
                                           const std::string &backBody,
                                           const std::string &backShape,
                                           const std::string &backFrame,
                                           const std::string &abCorrection,
                                           Real              s,
                                           Real              e,
                                           bool              useEntireIntvl,
                                           Real              stepSize,
                                           Integer           &numIntervals,
                                           RealArray         &starts,
                                           RealArray         &ends)
{
   Spacecraft  *theSc       = (Spacecraft*) theObj;

   #ifndef __USE_SPICE__
      std::string errmsg = "ERROR - cannot compute occultation intervals for spacecraft ";
      errmsg += theSc->GetName() + " without SPICE included in build!\n";
      throw SubscriberException(errmsg);
   #else

      #ifdef DEBUG_OCCULTATION
         MessageInterface::ShowMessage("In GetOccultationIntervals:\n");
         MessageInterface::ShowMessage("   occType      = %s\n", occType.c_str());
         MessageInterface::ShowMessage("   frontBody    = %s\n", frontBody.c_str());
         MessageInterface::ShowMessage("   frontShape   = %s\n", frontShape.c_str());
         MessageInterface::ShowMessage("   frontFrame   = %s\n", frontFrame.c_str());
         MessageInterface::ShowMessage("   backBody     = %s\n", backBody.c_str());
         MessageInterface::ShowMessage("   backShape    = %s\n", backShape.c_str());
         MessageInterface::ShowMessage("   backFrame    = %s\n", backFrame.c_str());
         MessageInterface::ShowMessage("   abCorrection = %s\n", abCorrection.c_str());
      #endif

      Integer     theNAIFId    = theSc->GetIntegerParameter("NAIFId");
      std::string theNAIFIdStr = GmatStringUtil::ToString(theNAIFId);

      // window we want to search
      SPICEDOUBLE_CELL(window, 200000);
      scard_c(0, &window);   // reset (empty) the cell

      // Get coverage window (no light time corrections needed)
      GetRequiredCoverageWindow(&window, s, e, useEntireIntvl, abCorrection);

      SpiceInt     numInt = wncard_c(&window);

      // CSPICE data
      ConstSpiceChar   *occultationType; //   = occType.c_str();
      ConstSpiceChar   *front            = frontBody.c_str();
      ConstSpiceChar   *fshape           = frontShape.c_str();
      ConstSpiceChar   *fframe           = frontFrame.c_str();
      ConstSpiceChar   *back             = backBody.c_str();
      ConstSpiceChar   *bshape           = backShape.c_str();
      ConstSpiceChar   *bframe           = backFrame.c_str();
      ConstSpiceChar   *abcorr           = abCorrection.c_str();
      ConstSpiceChar   *obsrvr           = theNAIFIdStr.c_str();
      SpiceDouble      step              = stepSize;

      if (occType == "ALL")
         occultationType = "ANY";   // <future> find SPICE constant for this
      else if (occType == "Umbra")
         occultationType = SPICE_GF_FULL;
      else if (occType == "Penumbra")
         occultationType = SPICE_GF_PARTL;
      else // Antumbra
         occultationType = SPICE_GF_ANNULR;

      SPICEDOUBLE_CELL(result, 200000);
      scard_c(0, &result);   // reset (empty) the result cell

      #ifdef DEBUG_OCCULTATION
         MessageInterface::ShowMessage("Calling gfoclt_c with:\n");
         MessageInterface::ShowMessage("   occultationType = %s\n", occultationType);
         MessageInterface::ShowMessage("   front           = %s\n", frontBody.c_str());
         MessageInterface::ShowMessage("   fshape          = %s\n", frontShape.c_str());
         MessageInterface::ShowMessage("   fframe          = %s\n", frontFrame.c_str());
         MessageInterface::ShowMessage("   back            = %s\n", backBody.c_str());
         MessageInterface::ShowMessage("   bshape          = %s\n", backShape.c_str());
         MessageInterface::ShowMessage("   bframe          = %s\n", backFrame.c_str());
         MessageInterface::ShowMessage("   abcorr          = %s\n", abCorrection.c_str());
         MessageInterface::ShowMessage("   obsrvr          = %s\n", theNAIFIdStr.c_str());
         MessageInterface::ShowMessage("   step            = %12.10f\n", stepSize);
      #endif

      #ifdef DEBUG_EM_TIME_SPENT
      clock_t t = clock();
      #endif
      gfoclt_c(occultationType, front, fshape, fframe, back, bshape, bframe, abcorr,
               obsrvr, step, &window, &result);
      #ifdef DEBUG_EM_TIME_SPENT
      Real timeSpent = (Real) (clock() - t);
      MessageInterface::ShowMessage(" -------------- time in gfoclt_c call for %s is %12.10f (sec)\n",
            occType.c_str(), (timeSpent / CLOCKS_PER_SEC));
      #endif
      if (failed_c())
      {
         ConstSpiceChar option[] = "LONG";
         SpiceInt       numChar  = MAX_LONG_MESSAGE_VALUE;
         SpiceChar      err[MAX_LONG_MESSAGE_VALUE];
         getmsg_c(option, numChar, err);
         std::string errStr(err);
         std::string errmsg = "Error calling gfoclt_c!!!  ";
         errmsg += "Message received from CSPICE is: ";
         errmsg += errStr + "\n";
//         MessageInterface::ShowMessage("----- error message = %s\n",
//               errmsg.c_str());
         reset_c();
         throw SubscriberException(errmsg);
      }

      SpiceInt sz = wncard_c(&result);
      numIntervals = (Integer) sz;
      #ifdef DEBUG_OCCULTATION
         MessageInterface::ShowMessage("---------- For occType = %s, %d events found\n",
               occType.c_str(), numIntervals);
      #endif

      for (Integer ii = 0; ii < numIntervals; ii++)
      {
         SpiceDouble s, e;
         wnfetd_c(&result, ii, &s, &e);
         Real ss = spice->SpiceTimeToA1(s);
         Real ee = spice->SpiceTimeToA1(e);
         starts.push_back(ss); // OR starts.at(ii) = s? (and same for e)?
         ends.push_back(ee);
      }
      scard_c(0, &window);   // reset (empty) the window cell
      scard_c(0, &result);   // reset (empty) the result cell

      return true;
   #endif
}

/// @YRL
//------------------------------------------------------------------------------
//    bool    GetContactIntervals(const std::string &observerID,
//                                Real              minElevation,
//                                const std::string &obsFrameName,
//                                StringArray       &occultingBodyNames,
//                                const std::string &abCorrection,
//                                Real              s,
//                                Real              e,
//                                bool              useEntireIntvl,
//                                bool              useLightTime,
//                                bool              transmit,
//                                Real              stepSize,
//                                Integer           &numIntervals,
//                                RealArray         &starts,
//                                RealArray         &ends)
//------------------------------------------------------------------------------
/**
 * This method determines the contact intervals given the input observer,
 * abberration correction, times, and stepsize.
 *
 * @param observerID         NAIF ID of the observer
 * @param minElevation       minimum elevation of the GS
 * @param obsFrameName       frame name for the observer
 * @param occultingBodyNames array of occulting bodies
 * @param abCorrection       aberration correction
 * @param s                  start time
 * @param e                  end time
 * @param useEntireIntvl the flag to use entire available interval
 * @param useLightTime       use light time delay flag
 * @param transmit           transmit or receive
 * @param stepSize           stepsize
 * @param numIntervals       number of intervals returned (output)
 * @param starts             array of start times for the intervals (output)
 * @param ends               array of end times for the intervals (output)
 *
 * Note: initial implementation by Yeerang Lim/KAIST
 *
 */
//------------------------------------------------------------------------------
bool EphemManager::GetContactIntervals(const std::string &observerID,
                                       Real              minElevation,
                                       const std::string &obsFrameName,
                                       StringArray       &occultingBodyNames,
                                       const std::string &abCorrection,
                                       Real              s,
                                       Real              e,
                                       bool              useEntireIntvl,
                                       bool              useLightTime,
                                       bool              transmit,
                                       Real              stepSize,
                                       Integer           &numIntervals,
                                       RealArray         &starts,
                                       RealArray         &ends)
{
   Spacecraft  *theSc       = (Spacecraft*) theObj;

#ifndef __USE_SPICE__
   std::string errmsg = "ERROR - cannot compute contact intervals for spacecraft ";
   errmsg += theSc->GetName() + " without SPICE included in build!\n";
   throw SubscriberException(errmsg);
#else

   Integer     theNAIFId       = theSc->GetIntegerParameter("NAIFId");
   std::string theNAIFIdStr    = GmatStringUtil::ToString(theNAIFId);

   #ifdef DEBUG_CONTACT
      MessageInterface::ShowMessage("In GetContactIntervals:\n");
      MessageInterface::ShowMessage("   observerID      = %s\n", observerID.c_str());
      MessageInterface::ShowMessage("   theNAIFIdStr    = %s\n", theNAIFIdStr.c_str());
      MessageInterface::ShowMessage("   minElevation    = %12.10f\n", minElevation);
      MessageInterface::ShowMessage("   obsFrameName    = %s\n", obsFrameName.c_str());
      MessageInterface::ShowMessage("   abCorrection    = %s\n", abCorrection.c_str());
      MessageInterface::ShowMessage("   s               = %12.10f\n", s);
      MessageInterface::ShowMessage("   e               = %12.10f\n", e);
      MessageInterface::ShowMessage("   stepSize        = %12.10f\n", stepSize);
   #endif

//   std::string theNAIFFrameStr = theSc->GetStringParameter(theSc->GetParameterID("SpiceFrameId"));

   // window we want to search
   SPICEDOUBLE_CELL(initWindow, 200000);
   scard_c(0, &initWindow);   // reset (empty) the cell

   // window we want to search
   SPICEDOUBLE_CELL(window, 200000);
   scard_c(0, &window);   // reset (empty) the cell

   Integer obsID;
   GmatStringUtil::ToInteger(observerID, obsID);
   // NOTE we should find a way to do this once per FindEvents, before the first
   // call to this method (for the first observer).  Since I can't use SPICE
   // types in plugins (i.e. ContactLocator), I can't pass a window out of this
   // class.  The current way of calling this method each time will be a
   // performance hit if there are lots of observers.
   GetRequiredCoverageWindow(&window, s, e, useEntireIntvl, abCorrection,
                             true, useLightTime,
                             transmit, stepSize, obsID);

   std::string    theCrdSys   = "LATITUDINAL";
   std::string    theCoord    = "LATITUDE";
   std::string    theRelate   = ">";
   std::string    theOccType  = "ANY";
   std::string    theFront    = "";
   std::string    theFShape   = "ELLIPSOID";
   std::string    theFFrame   = "";
   std::string    theTShape   = "POINT";

   // CSPICE data
   ConstSpiceChar *target           = theNAIFIdStr.c_str();     // NAIF Id of the spacecraft
   ConstSpiceChar *tframe           = obsFrameName.c_str();     // SpiceFrameId for the observer OR ' '?
   ConstSpiceChar *abcorr           = abCorrection.c_str();     // Aberration correction
   ConstSpiceChar *obsrvr           = observerID.c_str();       // NAIF ID of the observer
   ConstSpiceChar *crdsys           = theCrdSys.c_str();        //
   ConstSpiceChar *coord            = theCoord.c_str();         //
   ConstSpiceChar *relate           = theRelate.c_str();        //
   ConstSpiceChar *occultationType  = theOccType.c_str();       //
   ConstSpiceChar *fshape           = theFShape.c_str();        //
   ConstSpiceChar *tshape           = theTShape.c_str();        //
   ConstSpiceChar *front;
   ConstSpiceChar *fframe;
   SpiceDouble    refval            = minElevation * GmatMathConstants::RAD_PER_DEG;
   SpiceDouble    adjust            = 0.0;
   SpiceInt       nintvls           = (SpiceInt)1e6;
   SpiceDouble    step              = stepSize;

   SPICEDOUBLE_CELL(result, 200000);
   scard_c(0, &result);   // reset (empty) the coverage cell
   SPICEDOUBLE_CELL(subtracted, 200000);
   scard_c(0, &subtracted);   // reset (empty) the coverage cell
   SPICEDOUBLE_CELL(obsResults, 200000);
   scard_c(0, &obsResults);   // reset (empty) the coverage cell
   SPICEDOUBLE_CELL(occultResults, 200000);
   scard_c(0, &occultResults);   // reset (empty) the coverage cell

#ifdef DEBUG_CONTACT
   MessageInterface::ShowMessage("In GetContactIntervals, about to call gfposc_c\n");
   MessageInterface::ShowMessage("   target      = %s\n",      theNAIFIdStr.c_str());
   MessageInterface::ShowMessage("   tframe      = %s\n",      obsFrameName.c_str());
   MessageInterface::ShowMessage("   abcorr      = %s\n",      abCorrection.c_str());
   MessageInterface::ShowMessage("   obsrvr      = %s\n",      observerID.c_str());
   MessageInterface::ShowMessage("   crdsys      = %s\n",      theCrdSys.c_str());
   MessageInterface::ShowMessage("   coord       = %s\n",      theCoord.c_str());
   MessageInterface::ShowMessage("   relate      = %s\n",      theRelate.c_str());
   MessageInterface::ShowMessage("   refval      = %12.10f\n", refval);
   MessageInterface::ShowMessage("   adjust      = %12.10f\n", (Real) adjust);
   MessageInterface::ShowMessage("   step        = %12.10f\n", stepSize);
   MessageInterface::ShowMessage("   nintvls     = %d\n",      (Integer) nintvls);
#endif


   gfposc_c(target, tframe, abcorr, obsrvr, crdsys, coord, relate,
            refval, adjust, step, nintvls, &window, &obsResults);
   if (failed_c())
   {
      ConstSpiceChar option[] = "LONG";
      SpiceInt       numChar  = MAX_LONG_MESSAGE_VALUE;
      SpiceChar      err[MAX_LONG_MESSAGE_VALUE];
      getmsg_c(option, numChar, err);
      std::string errStr(err);
      std::string errmsg = "Error calling gfposc_c!!!  ";
      errmsg += "Message received from CSPICE is: ";
      errmsg += errStr + "\n";
      reset_c();
      throw SubscriberException(errmsg);
   }

   SpiceInt szObs       = wncard_c(&obsResults);
   #ifdef DEBUG_CONTACT
      Integer numObs       = (Integer) szObs;
      MessageInterface::ShowMessage("--- size of obsResults = %d\n",
            numObs);
      for (Integer ii = 0; ii < numObs; ii++)
      {
         SpiceDouble sObs, eObs;
         wnfetd_c(&obsResults, ii, &sObs, &eObs);
         Real ssObs = spice->SpiceTimeToA1(sObs);
         Real eeObs = spice->SpiceTimeToA1(eObs);
         MessageInterface::ShowMessage("------  start %d = %12.10f\n",
               ii, ssObs);
         MessageInterface::ShowMessage("------  end %d   = %12.10f\n",
               ii, eeObs);
      }
   #endif

   if ((Integer) szObs > 0)
   {
      for (unsigned int ii = 0; ii < occultingBodyNames.size(); ii++ )
      {
         CelestialBody *body = solarSys->GetBody(occultingBodyNames.at(ii));

         theFFrame = body->GetStringParameter(body->GetParameterID("SpiceFrameId"));
         Integer bodyNaifId = body->GetIntegerParameter(body->GetParameterID("NAIFId"));
         theFront = GmatStringUtil::Trim(GmatStringUtil::ToString(bodyNaifId));

         front  = theFront.c_str();
         fframe = theFFrame.c_str();
         #ifdef DEBUG_CONTACT
            MessageInterface::ShowMessage("Calling gfoclt_c (for body %s) with:\n",
                  body->GetName().c_str());
            MessageInterface::ShowMessage("   occultationType = %s\n", theOccType.c_str());
            MessageInterface::ShowMessage("   front           = %s\n", theFront.c_str());
            MessageInterface::ShowMessage("   fshape          = %s\n", theFShape.c_str());
            MessageInterface::ShowMessage("   fframe          = %s\n", theFFrame.c_str());
            MessageInterface::ShowMessage("   target          = %s\n", theNAIFIdStr.c_str());
            MessageInterface::ShowMessage("   tshape          = %s\n", theTShape.c_str());
            MessageInterface::ShowMessage("   tframe          = \"    \"\n");
            MessageInterface::ShowMessage("   abcorr          = %s\n", abCorrection.c_str());
            MessageInterface::ShowMessage("   obsrvr          = %s\n", observerID.c_str());
            MessageInterface::ShowMessage("   step            = %12.10f\n", stepSize);
         #endif

         gfoclt_c(occultationType, front, fshape, fframe, target, tshape, " ", abcorr,
                  obsrvr, step, &obsResults, &occultResults);
         if (failed_c())
         {
            ConstSpiceChar option[] = "LONG";
            SpiceInt       numChar  = MAX_LONG_MESSAGE_VALUE;
            SpiceChar      err[MAX_LONG_MESSAGE_VALUE];
            getmsg_c(option, numChar, err);
            std::string errStr(err);
            std::string errmsg = "Error calling gfoclt_c!!!  ";
            errmsg += "Message received from CSPICE is: ";
            errmsg += errStr + "\n";
            reset_c();
            throw SubscriberException(errmsg);
         }
         #ifdef DEBUG_CONTACT
            SpiceInt szOcc       = wncard_c(&occultResults);
            Integer  numOcc      = (Integer) szOcc;
            MessageInterface::ShowMessage("--- size of occultResults = %d\n",
                  numOcc);
            for (Integer ii = 0; ii < numOcc; ii++)
            {
               SpiceDouble sOcc, eOcc;
               wnfetd_c(&occultResults, ii, &sOcc, &eOcc);
               Real ssOcc = spice->SpiceTimeToA1(sOcc);
               Real eeOcc = spice->SpiceTimeToA1(eOcc);
               MessageInterface::ShowMessage("------  start %d = %12.10f\n",
                     ii, ssOcc);
               MessageInterface::ShowMessage("------  end %d   = %12.10f\n",
                     ii, eeOcc);
            }
         #endif
         wndifd_c(&obsResults, &occultResults, &subtracted);
         copy_c(&subtracted, &obsResults);
         #ifdef DEBUG_CONTACT
            szObs       = wncard_c(&obsResults);
            numObs      = (Integer) szObs;
            MessageInterface::ShowMessage("--- After subtracting occultation results, size of obsResults = %d\n",
                  numObs);
            for (Integer ii = 0; ii < numObs; ii++)
            {
               SpiceDouble sObs, eObs;
               wnfetd_c(&obsResults, ii, &sObs, &eObs);
               Real ssObs = spice->SpiceTimeToA1(sObs);
               Real eeObs = spice->SpiceTimeToA1(eObs);
               MessageInterface::ShowMessage("------  start %d = %12.10f\n",
                     ii, ssObs);
               MessageInterface::ShowMessage("------  end %d   = %12.10f\n",
                     ii, eeObs);
            }
         #endif
//         wndifd_c(&obsResults, &occultResults, &result);
      }
   }
   copy_c(&obsResults, &result);
//   wndifd_c(&obsResults, &occultResults, &result);


   SpiceInt sz = wncard_c(&result);
   numIntervals = (Integer) sz;
   #ifdef DEBUG_CONTACT
      MessageInterface::ShowMessage("--- size of result = %d\n",
            numIntervals);
   #endif

   for (Integer ii = 0; ii < numIntervals; ii++)
   {
      SpiceDouble s, e;
      wnfetd_c(&result, ii, &s, &e);
      Real ss = spice->SpiceTimeToA1(s);
      Real ee = spice->SpiceTimeToA1(e);
      starts.push_back(ss); // OR starts.at(ii) = s? (and same for e)?
      ends.push_back(ee);
   }
   scard_c(0, &initWindow);      // reset (empty) the window cell
   scard_c(0, &window);          // reset (empty) the window cell
   scard_c(0, &result);          // reset (empty) the window cell
   scard_c(0, &subtracted);      // reset (empty) the window cell
   scard_c(0, &obsResults);      // reset (empty) the window cell
   scard_c(0, &occultResults);   // reset (empty) the window cell

   return true;
#endif
}



bool EphemManager::GetCoverage(Real s, Real e,
                               bool useEntireIntvl,
                               bool includeAll,
                               Real &intvlStart,
                               Real &intvlStop,
                               Real &cvrStart,
                               Real &cvrStop)
{
   #ifndef __USE_SPICE__
      Spacecraft *theSc = (Spacecraft*) theObj;
      std::string errmsg = "ERROR - cannot compute occultation intervals for spacecraft ";
      errmsg += theSc->GetName() + " without SPICE included in build!\n";
      throw SubscriberException(errmsg);
   #else
      SPICEDOUBLE_CELL(coverWindow, 200000);
      scard_c(0, &coverWindow);   // reset (empty) the cell

      // Get the coverage for the spacecraft (without light time corrections)
      GetRequiredCoverageWindow(&coverWindow, s, e, useEntireIntvl, "NONE", includeAll);

      // return the start and stop times of the window we are using
      intvlStart = intStart;
      intvlStop  = intStop;
      // return the start and stop times of the full coverage window
      cvrStart   = coverStart;
      cvrStop    = coverStop;

      scard_c(0, &coverWindow);   // reset (empty) the cell

      return true;
   #endif
}


#ifdef __USE_SPICE__
//------------------------------------------------------------------------------
// void GetRequiredCoverageWindow(SpiceCell* w, Real s1, Real e1,
//                                bool useEntireIntvl,
//                                const std::string &abCorr     = "NONE",
//                                bool includeAll          = true,
//                                bool lightTimeCorrection = false,
//                                bool transmitDirection = false,
//                                Real stepSize = 10.0,
//                                Integer obsID = -999);
//------------------------------------------------------------------------------
void EphemManager::GetRequiredCoverageWindow(SpiceCell* w, Real s1, Real e1,
                                             bool useEntireIntvl,
                                             const std::string &abCorr,
                                             bool includeAll,
                                             bool lightTimeCorrection,
                                             bool transmitDirection,
                                             Real stepSize,
                                             Integer obsID)
{
   #ifdef DEBUG_EM_COVERAGE
      MessageInterface::ShowMessage("In GetRequiredCoverageWindow:\n");
      MessageInterface::ShowMessage("   s1 = %12.10f\n", s1);
      MessageInterface::ShowMessage("   e1 = %12.10f\n", e1);
      MessageInterface::ShowMessage("   useEntireIntvl = %s\n",
                        (useEntireIntvl? "true":"false"));
    #endif
   // @todo - most of this should be moved to SpiceInterface and
   // made very general (for SPK, CK, etc.)
   Spacecraft *theSc = (Spacecraft*) theObj;

   Integer    forNaifId = theSc->GetIntegerParameter("NAIFId");

   if (!spice)
      spice = new SpiceInterface();
   // Which files do we need to check?
   StringArray inKernels = fileList;
   if (includeAll)
   {
      StringArray inputKernels = theSc->GetStringArrayParameter("OrbitSpiceKernelName");
      for (unsigned int ii = 0; ii < inputKernels.size(); ii++)
         inKernels.push_back(inputKernels.at(ii));
   }
   #ifdef DEBUG_EM_COVERAGE
      MessageInterface::ShowMessage("In GetRequiredCoverageWindow:\n");
      MessageInterface::ShowMessage("   forNaifId = %d\n", forNaifId);
      MessageInterface::ShowMessage("   kernels are:\n");
      if (inKernels.empty())  MessageInterface::ShowMessage("   EMPTY!!!!\n");
      else
      {
         for (unsigned int ii = 0; ii < inKernels.size(); ii++)
            MessageInterface::ShowMessage("   %d    %s\n", (Integer) ii, inKernels.at(ii).c_str());
      }
      MessageInterface::ShowMessage("   fileList are:\n");
      if (fileList.empty())  MessageInterface::ShowMessage("   EMPTY!!!!\n");
      else
      {
         for (unsigned int ii = 0; ii < fileList.size(); ii++)
            MessageInterface::ShowMessage("   %d    %s\n", (Integer) ii, fileList.at(ii).c_str());
      }
   #endif
   // first check to see if a kernel specified is not loaded; if not,
   // try to load it
   FileManager *fm = FileManager::Instance();
   for (unsigned int ii = 0; ii < inKernels.size(); ii++)
   {
      std::string kName = GmatStringUtil::Replace(inKernels.at(ii), "\\", "/");
      std::string fullPath = fm->FindPath(kName, "VEHICLE_EPHEM_SPK_PATH",
                                          true, false, true);
      if (!spice->IsLoaded(fullPath))   spice->LoadKernel(fullPath);
   }

   SpiceInt         idSpice     = forNaifId;
   SpiceInt         arclen      = 4;
   SpiceInt         typlen      = 5;
   bool             firstInt    = true;
   bool             idOnKernel  = false;
   ConstSpiceChar   *kernelName = NULL;
   SpiceInt         objId       = 0;
   SpiceInt         numInt      = 0;
   SpiceChar        *kernelType;
   SpiceChar        *arch;
   SpiceDouble      b;
   SpiceDouble      e;
   Real             bA1;
   Real             eA1;
   SPICEINT_CELL(ids, 200);
   SPICEDOUBLE_CELL(cover, 200000);
   char             kStr[5] = "    ";
   char             aStr[4] = "   ";

   // start with an empty cell
   scard_c(0, &cover);   // reset the coverage cell

   // look through each kernel
   for (unsigned int ii = 0; ii < inKernels.size(); ii++)
   {
      #ifdef DEBUG_SPK_COVERAGE
         MessageInterface::ShowMessage("Checking coverage for ID %d on kernel %s\n",
               forNaifId, (inKernels.at(ii)).c_str());
      #endif
      // SPICE expects forward slashes for directory separators
      std::string kName1 = GmatStringUtil::Replace(inKernels.at(ii), "\\", "/");
      std::string kName  = fm->FindPath(kName1, "VEHICLE_EPHEM_SPK_PATH",
                                        true, false, true);
      #ifdef DEBUG_SPK_COVERAGE
         MessageInterface::ShowMessage("--- Setting kernel name to %s\n", kName.c_str());
      #endif
      kernelName = kName.c_str();
      // check the type of kernel
      arch        = aStr;
      kernelType  = kStr;
      getfat_c(kernelName, arclen, typlen, arch, kernelType);
      if (failed_c())
      {
         ConstSpiceChar option[] = "LONG";
         SpiceInt       numChar  = MAX_LONG_MESSAGE_VALUE;
         //SpiceChar      err[MAX_LONG_MESSAGE_VALUE];
         SpiceChar      *err = new SpiceChar[MAX_LONG_MESSAGE_VALUE];
         getmsg_c(option, numChar, err);
         std::string errStr(err);
         std::string errmsg = "Error determining type of kernel \"";
         errmsg += inKernels.at(ii) + "\".  Message received from CSPICE is: [";
         errmsg += errStr + "]\n";
         reset_c();
         delete [] err;
         throw SubscriberException(errmsg);
      }

      #ifdef DEBUG_SPK_COVERAGE
         MessageInterface::ShowMessage("Kernel is of type %s\n",
               kernelType);
      #endif
      // only deal with SPK kernels
      if (eqstr_c( kernelType, "spk" ))
      {
         spkobj_c(kernelName, &ids);
         // get the list of objects (IDs) for which data exists in the SPK kernel
         for (SpiceInt jj = 0;  jj < card_c(&ids);  jj++)
         {
            objId = SPICE_CELL_ELEM_I(&ids,jj);
            #ifdef DEBUG_SPK_COVERAGE
               MessageInterface::ShowMessage("Kernel contains data for object %d\n",
                     (Integer) objId);
            #endif
            // look to see if this kernel contains data for the object we're interested in
            if (objId == idSpice)
            {
               idOnKernel = true;
               break;
            }
         }
         // only deal with kernels containing data for the object we're interested in
         if (idOnKernel)
         {
            #ifdef DEBUG_SPK_COVERAGE
               MessageInterface::ShowMessage("Checking kernel %s for data for object %d\n",
                     (inKernels.at(ii)).c_str(), (Integer) objId);
            #endif
            spkcov_c (kernelName, idSpice, &cover);
            if (failed_c())
            {
               ConstSpiceChar option[] = "LONG";
               SpiceInt       numChar  = MAX_LONG_MESSAGE_VALUE;
               //SpiceChar      err[MAX_LONG_MESSAGE_VALUE];
               SpiceChar      *err = new SpiceChar[MAX_LONG_MESSAGE_VALUE];
               getmsg_c(option, numChar, err);
               std::string errStr(err);
               std::string errmsg = "Error determining coverage for SPK kernel \"";
               errmsg += inKernels.at(ii) + "\".  Message received from CSPICE is: [";
               errmsg += errStr + "]\n";
               reset_c();
               delete [] err;
               throw SubscriberException(errmsg);
            }
            numInt = wncard_c(&cover);
            // We assume that the intervals contained in the resulting window
            // are in time order
            firstInt = false;
            #ifdef DEBUG_SPK_COVERAGE
               MessageInterface::ShowMessage("Number of intervals found =  %d\n",
                     (Integer) numInt);
            #endif
         }
      }
      if (firstInt)
      {
         std::stringstream errmsg("");
         errmsg << "Error - no data available for body with NAIF ID " << forNaifId << " on specified SPK kernels\n";
         throw SubscriberException(errmsg.str());
      }
      #ifdef DEBUG_SPK_COVERAGE
         else
         {
            MessageInterface::ShowMessage("Number of intervals found for body with NAIF ID %d =  %d\n",
                  forNaifId, (Integer) numInt);
         }
      #endif

   }
   // window we want to search
   SPICEDOUBLE_CELL(window, 200000);
   scard_c(0, &window);   // reset (empty) the window cell

   // Get the start and stop times for the complete coverage window
   SpiceInt szCoverage = wncard_c(&cover);
   #ifdef DEBUG_SPK_COVERAGE
      MessageInterface::ShowMessage("----- number of intervals in full coverage window is %d\n",
            szCoverage);
   #endif

   if (((Integer) szCoverage) > 0)
   {
      SpiceDouble s001, e001, s002, e002;
      wnfetd_c(&cover, 0, &s001, &e001);
      wnfetd_c(&cover, szCoverage-1, &s002, &e002);

      coverStart = spice->SpiceTimeToA1(s001);
      coverStop  = spice->SpiceTimeToA1(e002);
   }
   else
   {
      coverStart = 0.0;
      coverStop  = 0.0;
   }

   // Set this initially - it  will be computed later on if necessary
   intStart   = coverStart;
   intStop    = coverStop;

   // Figure out the window we want to use
   if (useEntireIntvl)
   {
      #ifdef DEBUG_SPK_COVERAGE
         MessageInterface::ShowMessage("Using ENTIRE interval and the size of the window = %d\n",
               (Integer) wncard_c(&cover));
         MessageInterface::ShowMessage("   and number of elements in the window = %d\n",
               (Integer) card_c(&cover));
         for (Integer ii = 0; ii < (Integer) card_c(&cover); ii++)
            MessageInterface::ShowMessage("     element %d = %12.10f\n",
                  ii, (Real) SPICE_CELL_ELEM_D(&cover, (SpiceInt) ii));
      #endif
      if (lightTimeCorrection)
      {
         ConstSpiceChar   *abcorrection = abCorr.c_str();
         // Shift entire window by the light time
         SpiceDouble     pos[3];
         std::string     targetID = GmatStringUtil::Trim(GmatStringUtil::ToString(forNaifId));
         std::string     obsrvrID = GmatStringUtil::Trim(GmatStringUtil::ToString(obsID));
         ConstSpiceChar* tID      = targetID.c_str();
         ConstSpiceChar* oID      = obsrvrID.c_str();
         Real            dir      = 1.0; // RECEIVE
         if (transmitDirection)   // TRANSMIT
            dir = -1.0;

         // First, get the number of elements (not intervals) in the window
         SpiceInt    numEl = card_c(&cover);
//         SpiceDouble lt[numEl];
		 SpiceDouble *lt = new SpiceDouble[numEl];
         SpiceDouble d     = 0.0;
         SpiceDouble newD  = 0.0;
         // Loop over the elements and adjust the time based on the light time
         // at that time
         for (Integer ii = 0; ii < (Integer) numEl; ii++)
         {
            d = SPICE_CELL_ELEM_D(&cover, (SpiceInt) ii);
            #ifdef DEBUG_SPK_COVERAGE
               MessageInterface::ShowMessage("Element of cover (%d) = %12.10f\n",
                     ii, (Real) d);
            #endif
            spkpos_c(oID, d, "J2000", abcorrection, tID, pos, &lt[ii]);
            if (failed_c())
            {
               ConstSpiceChar option[] = "SHORT";
               SpiceInt       numChar  = MAX_SHORT_MESSAGE_VALUE;
               SpiceChar      err[MAX_SHORT_MESSAGE_VALUE];
               getmsg_c(option, numChar, err);
               std::string errStr(err);
               std::string errmsg = "Error calling spkpos_c!!!";
               errmsg += "Message received from CSPICE is: ";
               errmsg += errStr + "\n";
               reset_c(); // reset SPICE error flags
               throw SubscriberException(errmsg);
            }
            newD = d + dir * lt[ii];
            SPICE_CELL_SET_D(newD, (SpiceInt) ii, &cover);
         }
         #ifdef DEBUG_SPK_COVERAGE
            MessageInterface::ShowMessage("After spkpos_c, the array of light time delays:\n");
            for (Integer ii = 0; ii < numEl; ii++)
               MessageInterface::ShowMessage("   lt[%d] = %12.10f\n",
                     ii, lt[ii]);
         #endif
         // Truncate the search interval (beginning for transmit, end for
         // receive)
         if (transmitDirection)
            wncond_c(lt[0], 0.0, &cover);
         else // receive
            wncond_c(0.0, lt[numEl-1], &cover);

         delete[] lt;

         // Trim to find valid endpoint
         Integer     trimIterMax  = 1000;
         Real        trimMax      = 2.0;
         Real        trimErrTol   = 1.0e-3;

         Real        trimErr      = (Real) GmatRealConstants::INTEGER_MAX;
         Real        trim         = 0.0;
         Real        trimA        = trimMax;
         Real        trimB        = trim;
         Integer     trimIter     = 0;
         Integer     testInterval = 0;
         SpiceDouble lTime;

         SPICEDOUBLE_CELL(testWindow, 200010);
         scard_c(0, &testWindow);   // reset (empty) the test window cell

         while (trimIter < trimIterMax)
         {
            #ifdef DEBUG_SPK_COVERAGE
               MessageInterface::ShowMessage("BEFORE copying, the size of the cover = %d\n",
                     (Integer) wncard_c(&cover));
               MessageInterface::ShowMessage("   and number of elements in the cover = %d\n",
                     (Integer) card_c(&cover));
            #endif
            copy_c(&cover, &testWindow);
            #ifdef DEBUG_SPK_COVERAGE
               MessageInterface::ShowMessage("After copying, the size of the testWindow = %d\n",
                     (Integer) wncard_c(&testWindow));
               MessageInterface::ShowMessage("   and number of elements in the testWindow = %d\n",
                     (Integer) card_c(&testWindow));
            #endif

            if (transmitDirection)  // transmit
            {
               wncond_c(0.0, trim, &testWindow);
               testInterval = (Integer) card_c(&testWindow) - 1;
            }
            else // receive
            {
               wncond_c(trim, 0.0, &testWindow);
               testInterval = 0;
            }
            if (failed_c())
            {
               ConstSpiceChar option[] = "SHORT";
               SpiceInt       numChar  = MAX_SHORT_MESSAGE_VALUE;
               SpiceChar      err[MAX_SHORT_MESSAGE_VALUE];
               getmsg_c(option, numChar, err);
               std::string errStr(err);
               std::string errmsg = "Error calling wncond_c for ";
               if (transmitDirection)
                  errmsg += "transmit!!!  ";
               else
                  errmsg += "receive!!!  ";
               errmsg += "Message received from CSPICE is: ";
               errmsg += errStr + "\n";
               reset_c(); // reset SPICE error flags
               throw SubscriberException(errmsg);
            }
            SPICE_CELL_GET_D(&testWindow, testInterval, &d);
            spkpos_c(tID, d, "J2000", abcorrection, oID, pos, &lTime);
            if (failed_c())
            {
               // Instead of throwing an error adjust the numbers if necessary

               ConstSpiceChar option[] = "SHORT";
               SpiceInt       numChar  = MAX_SHORT_MESSAGE_VALUE;
               SpiceChar      err[MAX_SHORT_MESSAGE_VALUE];
               getmsg_c(option, numChar, err);
               // Search the message for "Insufficient ephemeris data" to indicate that
               // specific error NOTE: this assumes that this error message text
               // will not change in the future
               if (eqstr_c(err, "SPICE(SPKINSUFFDATA)"))
               {
                  trimB = trim;
                  trim  = (trimA + trimB) / 2.0;
                  trimIter++;
                  // apply trim
                  if (transmitDirection)  // transmit
                  {
                     wncond_c(0.0, trim, &testWindow);
                     testInterval = (Integer) card_c(&testWindow) - 1;
                  }
                  else // receive
                  {
                     wncond_c(trim, 0.0, &testWindow);
                     testInterval = 0;
                  }
                  reset_c(); // reset SPICE error flags
               }
               else
               {
                  std::string errStr(err);
                  std::string errmsg = "Error calling spkpos_c!!!  ";
                  errmsg += "Message received from CSPICE is: ";
                  errmsg += errStr + "\n";
                  reset_c(); // reset SPICE error flags
                  throw SubscriberException(errmsg);
               }
            }
            else
            {
               if (trimErr <= trimErrTol)
               {
                  // Copy the test window to the window result that we want
                  copy_c(&testWindow, &window);
                  break;
               }
               trimA   = trim;
               trim    = (trimA + trimB) / 2;
               trimErr = GmatMathUtil::Abs(trimA - trimB);
            }
         }
         scard_c(0, &testWindow);   // reset (empty) the test window cell
      }
      else
      {
         copy_c(&cover, &window);
      }
   }
   else // a window only over the specified time range - no light time etc.
   {
      // create a window of the specified time span - for
      SpiceDouble s = spice->A1ToSpiceTime(s1);
      SpiceDouble e = spice->A1ToSpiceTime(e1);
      SPICEDOUBLE_CELL(timespan, 200000);
      scard_c(0, &timespan);   // reset (empty) the coverage cell
      // Get the intersection of the timespan window and the coverage window
      wninsd_c(s, e, &timespan);
      wnintd_c(&cover, &timespan, &window);
      scard_c(0, &timespan);   // reset (empty) the timespan cell
      if (failed_c())
      {
         ConstSpiceChar option[] = "LONG";
         SpiceInt       numChar  = MAX_LONG_MESSAGE_VALUE;
         SpiceChar      err[MAX_LONG_MESSAGE_VALUE];
         getmsg_c(option, numChar, err);
         std::string errStr(err);
         std::string errmsg = "Error calling wninsd_c or wnintd_c!!!  ";
         errmsg += "Message received from CSPICE is: ";
         errmsg += errStr + "\n";
         reset_c();
         throw SubscriberException(errmsg);
      }
   }
   scard_c(0, &cover);   // reset (empty) the cover cell

   #ifdef DEBUG_ECLIPSE_EVENTS
      MessageInterface::ShowMessage("Number of intervals = %d\n", (Integer) numInt);
   #endif

   // Get the window start and stop times
   SpiceInt szWin = wncard_c(&window);
   #ifdef DEBUG_SPK_COVERAGE
      MessageInterface::ShowMessage("----- number of intervals to check times for is %d\n",
            szWin);
   #endif

   if (((Integer) szWin) > 0)
   {
      SpiceDouble s01, e01, s02, e02;
      wnfetd_c(&window, 0, &s01, &e01);
      wnfetd_c(&window, szWin-1, &s02, &e02);

      intStart = spice->SpiceTimeToA1(s01);
      intStop  = spice->SpiceTimeToA1(e02);
   }
   #ifdef DEBUG_SPK_COVERAGE
      MessageInterface::ShowMessage("----- intStart =  %12.10f\n", intStart);
      MessageInterface::ShowMessage("----- intStop  =  %12.10f\n", intStop);
   #endif
   copy_c(&window, w);
   scard_c(0, &window);   // reset (empty) the window cell
}
#endif //#ifdef __USE_SPICE__

//------------------------------------------------------------------------------
// SetObject()
//------------------------------------------------------------------------------
void EphemManager::SetObject(GmatBase *obj)
{
   if (!obj->IsOfType("Spacecraft"))
   {
      throw SubscriberException("Object used for EphemManager must be a Spacecraft.\n");
   }
   theObj     = obj;
   theObjName = obj->GetName();
}

//------------------------------------------------------------------------------
// SetEphemType()
//------------------------------------------------------------------------------
void EphemManager::SetEphemType(ManagedEphemType eType)
{
   if (eType != SPK)
   {
      throw SubscriberException("Type used for EphemManager must currently be SPK.\n");
   }
   theType = eType;
}

//------------------------------------------------------------------------------
// SetCoordinateSystem()
//------------------------------------------------------------------------------
void EphemManager::SetCoordinateSystem(CoordinateSystem *cs)
{
   coordSys     = cs;
   coordSysName = cs->GetName();
}

void EphemManager::SetSolarSystem(SolarSystem *ss)
{
   solarSys = ss;
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none

