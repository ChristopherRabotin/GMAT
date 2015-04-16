//$Id$
//------------------------------------------------------------------------------
//                               EphemManager
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2014 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Wendy C. Shoan
// Created: 2014.10.28
//
/**
 * Base class implementation for the EphemManager.  The EphemManager is
 * responsible for creating, loading, and managing private/hidden EphemerisFile
 * objects associated with its specified Spacecraft object.
 * NOTE: currently, the EphemManager will only handle SPK Orbit files.
 */
//------------------------------------------------------------------------------

#include <stdio.h>
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
#ifdef __USE_SPICE__
   #include "SpiceInterface.hpp"
#endif

//#define DEBUG_EPHEM_MANAGER
//#define DEBUG_EM_COVERAGE


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
   ephemFile              (NULL),
   coordSys               (NULL),
   coordSysName           (""),
   ephemName              (""),
   ephemCount             (0),
   fileName               (""),
   recording              (false),
   deleteTmpFiles         (deleteFiles),
   intStart               (0.0),
   intStop                (0.0)
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
   #endif
   // Stop recording
   if (recording) StopRecording();
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
         remove(eachFile.c_str()); // don't want to do this for now, since we need to check the files
      }
   }
   fileList.clear();

   #ifdef __USE_SPICE__
//   if (cover)
//      delete cover;
//   if (window)
//      delete cover;
   #endif

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
   ephemFile              (NULL),
   coordSys               (NULL),
   coordSysName           (copy.coordSysName),
   ephemName              (""),
   ephemCount             (0),
   fileName               (copy.fileName),
   recording              (copy.recording),
   deleteTmpFiles         (copy.deleteTmpFiles),
   intStart               (copy.intStart),
   intStop                (copy.intStop)
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
         #ifdef DEBUG_EPHEM_MANAGER
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
         // @todo - put these files in the tmp directory (platform-dependent)
         ss << "tmp_" << theObjName << "_" << ephemCount;
         ephemName = ss.str();
         ss << ".bsp";
         fileName = ss.str();
         ephemFile         = new EphemerisFile(ephemName);
         #ifdef DEBUG_EPHEM_MANAGER
            MessageInterface::ShowMessage(
                  "In EphemManager::RecordEphemerisData, ephemFile is at <%p> with name %s\n",
                  ephemFile, ephemName.c_str());
         #endif

   //      // For now, put it in the Vehicle path
   //      FileManager *fm = FileManager::Instance();
   //      std::string spkPath = fm->GetPathname(FileManager::VEHICLE_EPHEM_SPK_PATH);
   //      fileName = spkPath + fileName;
         // For now, put it in the Output path << this should be put into the
         // appropriate TMPDIR for the platform
         FileManager *fm = FileManager::Instance();
         std::string spkPath = fm->GetPathname(FileManager::OUTPUT_PATH);
         fileName = spkPath + fileName;

   //      // Save the filename in the list
   //      fileList.push_back(fileName);

         // Set up the EphemerisFile to write what we need - currently only SPK Orbit
         ephemFile->SetStringParameter("FileFormat", "SPK");
         ephemFile->SetStringParameter("StateType", "Cartesian");
         ephemFile->SetStringParameter("Spacecraft", theObjName);
         ephemFile->SetStringParameter("CoordinateSystem", coordSysName); // only MJ2000Eq so far!!
         ephemFile->SetStringParameter("Filename", fileName);
         ephemFile->SetStringParameter("Interpolator", "Hermite");
         ephemFile->SetIntegerParameter(ephemFile->GetParameterID("InterpolationOrder"), 7);

         ephemFile->SetInternalCoordSystem(coordSys);
         ephemFile->SetRefObject(theObj,   Gmat::SPACECRAFT,        theObjName);
         ephemFile->SetRefObject(coordSys, Gmat::COORDINATE_SYSTEM, coordSysName);

         ephemFile->Initialize();

         // Subscribe to the data
         Publisher *pub = Publisher::Instance();
         #ifdef DEBUG_EPHEM_MANAGER
            MessageInterface::ShowMessage(
                  "In EphemManager::RecordEphemerisData, subscribing to publisher\n");
         #endif
         pub->Subscribe(ephemFile);

         ephemCount++;
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
   StopRecording();
   RecordEphemerisData();
   return true;
}

//------------------------------------------------------------------------------
// StopRecording()
//------------------------------------------------------------------------------
void EphemManager::StopRecording()
{
   #ifdef DEBUG_EPHEM_MANAGER
      MessageInterface::ShowMessage("StopRecording -----  fileName = %s\n",
            fileName.c_str());
   #endif
   // Finalize and close the SPK file
   // Unsubscribe
   Publisher *pub = Publisher::Instance();
   pub->Unsubscribe(ephemFile);
   // Delete the current ephemFile - this should finalize the SPK writing and then
   // close the <fileName> file
   delete ephemFile;
   // Load the current SPK file, if it has been written
   if (GmatFileUtil::DoesFileExist(fileName))
   {
      #ifdef __USE_SPICE__
         spice->LoadKernel(fileName);
      #endif
      #ifdef DEBUG_EPHEM_MANAGER
         MessageInterface::ShowMessage("-==-==-= adding fileName %s to fileList\n",
               fileName.c_str());
      #endif
      // save the just written file name
      fileList.push_back(fileName);
   }
   ephemFile = NULL;
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
                                           Integer           stepSize,
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

      Integer     theNAIFId    = theSc->GetIntegerParameter("NAIFId");
      std::string theNAIFIdStr = GmatStringUtil::ToString(theNAIFId);

//      // window we want to search
      SPICEDOUBLE_CELL(window, 2000);
      scard_c(0, &window);   // reset (empty) the cell

      GetCoverageWindow(&window, s, e, useEntireIntvl);

//      // Get the interval start and stop times
//      SpiceInt szWin = wncard_c(&window);
//      if (((Integer) szWin) > 0)
//      {
//         SpiceDouble s, e;
//         wnfetd_c(&window, 0, &s, &e);
//         intvlStart = spice->SpiceTimeToA1(s);
//         wnfetd_c(&window, szWin-1, &s, &e);
//         intvlStop  = spice->SpiceTimeToA1(e);
//      }


   //   SpiceDouble  start0, end0, startN, endN;
      SpiceInt     numInt = wncard_c(&window);
   //   wnfetd_c(&window, 0, &start0, &end0);
   //   wnfetd_c(&window, (numInt-1), &startN, &endN);
   //   findStart  = spice->SpiceTimeToA1(start0);
   //   findStop   = spice->SpiceTimeToA1(endN);

      // CSPICE data
      ConstSpiceChar   *occultationType  = occType.c_str();
      ConstSpiceChar   *front            = frontBody.c_str();
      ConstSpiceChar   *fshape           = frontShape.c_str();
      ConstSpiceChar   *fframe           = frontFrame.c_str();
      ConstSpiceChar   *back             = backBody.c_str();
      ConstSpiceChar   *bshape           = backShape.c_str();
      ConstSpiceChar   *bframe           = backFrame.c_str();
      ConstSpiceChar   *abcorr           = abCorrection.c_str();
      ConstSpiceChar   *obsrvr           = theNAIFIdStr.c_str();
      SpiceDouble      step              = stepSize;

      if (occType == "Umbra")
      {
         occultationType = SPICE_GF_FULL;
      }
      else if (occType == "Penumbra")
      {
         occultationType = SPICE_GF_PARTL;
      }
      else // Antumbra
      {
         occultationType = SPICE_GF_ANNULR;
      }

      SPICEDOUBLE_CELL(result, 2000);
      scard_c(0, &result);   // reset (empty) the result cell

      gfoclt_c(occultationType, front, fshape, fframe, back, bshape, bframe, abcorr,
               obsrvr, step, &window, &result);
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
         MessageInterface::ShowMessage("----- error message = %s\n",
               errmsg.c_str());
         reset_c();
         throw SubscriberException(errmsg);
      }

      SpiceInt sz = wncard_c(&result);
      numIntervals = (Integer) sz;

      for (Integer ii = 0; ii < numIntervals; ii++)
      {
         SpiceDouble s, e;
         wnfetd_c(&result, ii, &s, &e);
         Real ss = spice->SpiceTimeToA1(s);
         Real ee = spice->SpiceTimeToA1(e);
         starts.push_back(ss); // OR starts.at(ii) = s? (and same for e)?
         ends.push_back(ee);
      }

      return true;
   #endif
}


bool EphemManager::GetCoverageStartAndStop(Real s, Real e,
                                           bool useEntireIntvl,
                                           bool includeAll,
                                           Real &intvlStart,
                                           Real &intvlStop)
{
   #ifndef __USE_SPICE__
      Spacecraft *theSc = (Spacecraft*) theObj;
      std::string errmsg = "ERROR - cannot compute occultation intervals for spacecraft ";
      errmsg += theSc->GetName() + " without SPICE included in build!\n";
      throw SubscriberException(errmsg);
   #else
      SPICEDOUBLE_CELL(coverWindow, 2000);
      scard_c(0, &coverWindow);   // reset (empty) the cell

      GetCoverageWindow(&coverWindow, s, e, useEntireIntvl, includeAll);

      intvlStart = intStart;
      intvlStop  = intStop;
      return true;
   #endif
}

#ifdef __USE_SPICE__
//------------------------------------------------------------------------------
// void GetCoverageWindow(SpiceCell* w, Real s1, Real e1,
//                              bool useEntireIntvl, bool includeAll = true)
//------------------------------------------------------------------------------
void EphemManager::GetCoverageWindow(SpiceCell* w, Real s1, Real e1,
                                      bool useEntireIntvl, bool includeAll)
{
#ifndef __USE_SPICE__
   Spacecraft *theSc = (Spacecraft*) theObj;
   std::string errmsg = "ERROR - cannot compute occultation intervals for spacecraft ";
   errmsg += theSc->GetName() + " without SPICE included in build!\n";
   throw SubscriberException(errmsg);
#else
   // @todo - most of this should be moved to SpiceInterface and
   // made very general (for SPK, CK, etc.)
   // Set SPICEDOUBLE_CELL cover
//   if (cover) delete cover;
   Spacecraft *theSc = (Spacecraft*) theObj;

   Integer    forNaifId = theSc->GetIntegerParameter("NAIFId");

   if (!spice)
      spice = new SpiceInterface();
   // Which files do we need to check?
   StringArray inKernels = fileList;
   if (includeAll)
   {
      StringArray inputKernels = theSc->GetStringArrayParameter("OrbitSpiceKernelName");
      for (Integer ii = 0; ii < inputKernels.size(); ii++)
         inKernels.push_back(inputKernels.at(ii));
   }
   #ifdef DEBUG_EM_COVERAGE
      MessageInterface::ShowMessage("In GetCoverageWindow:\n");
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
   for (unsigned int ii = 0; ii < inKernels.size(); ii++)
      if (!spice->IsLoaded(inKernels.at(ii)))   spice->LoadKernel(inKernels.at(ii));

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
      std::string kName = GmatStringUtil::Replace(inKernels.at(ii), "\\", "/");
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
//      // start with an empty cell
//      scard_c(0, &cover);   // reset the coverage cell

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
                     (kernels.at(ii)).c_str(), (Integer) objId);
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
            firstInt = false; // ******
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
   SPICEDOUBLE_CELL(window, 2000);
   scard_c(0, &window);   // reset (empty) the coverage cell

   if (useEntireIntvl)
   {
      copy_c(&cover, &window);
   }
   else // create a window only over the specified time range
   {
      // create a window of the specified time span
      SpiceDouble s = spice->A1ToSpiceTime(s1);
      SpiceDouble e = spice->A1ToSpiceTime(e1);
      SPICEDOUBLE_CELL(timespan, 2000);
      scard_c(0, &timespan);   // reset (empty) the coverage cell
      // Get the intersection of the timespan window and the coverage window
      wninsd_c(s, e, &timespan);
      wnintd_c(&cover, &timespan, &window);
   }

   #ifdef DEBUG_ECLIPSE_EVENTS
      MessageInterface::ShowMessage("Number of intervals = %d\n", (Integer) numInt);
   #endif

   // Get the window start and stop times
   SpiceInt szWin = wncard_c(&window);
   if (((Integer) szWin) > 0)
   {
      SpiceDouble s, e;
      wnfetd_c(&window, 0, &s, &e);
      intStart = spice->SpiceTimeToA1(s);
      wnfetd_c(&window, szWin-1, &s, &e);
      intStop  = spice->SpiceTimeToA1(e);
   }
   #endif
   copy_c(&window, w);
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

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none

