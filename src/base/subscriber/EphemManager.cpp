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
#include "SpiceInterface.hpp"
#include "Spacecraft.hpp"
#include "FileManager.hpp"
#include "MessageInterface.hpp"
#include "SubscriberException.hpp"
#include "FileUtil.hpp"
#include "StringUtil.hpp"

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
   spice                  (NULL),
   coordSys               (NULL),
   coordSysName           (""),
   ephemName              (""),
   ephemCount             (0),
   fileName               (""),
   recording              (false),
   deleteTmpFiles         (deleteFiles),
   cover                  (NULL)
{
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
      spice->UnloadKernel(eachFile.c_str());
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

   if (cover)
      delete cover;

   #ifdef DEBUG_EPHEM_MANAGER
      MessageInterface::ShowMessage("Destructing EphemManager ... deleting ephemFile\n");
   #endif
   // delete the current EphemerisFile
   if (ephemFile) delete ephemFile;
   #ifdef DEBUG_EPHEM_MANAGER
      MessageInterface::ShowMessage("Destructing EphemManager ... deleting spice\n");
   #endif
   // delete the SiceInterface
   if (spice)     delete spice;
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
   spice                  (NULL),
   coordSys               (NULL),
   coordSysName           (copy.coordSysName),
   ephemName              (""),
   ephemCount             (0),
   fileName               (copy.fileName),
   recording              (copy.recording),
   deleteTmpFiles         (copy.deleteTmpFiles),
   cover                  (NULL)
{
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
   fileName                 = "";
   recording                = copy.recording;
   deleteTmpFiles           = copy.deleteTmpFiles;
   cover                    = NULL;

   // @todo - what to do with the files already created here ... and what if the copy
   // is in the middle of recording?  need to check deleteTmpFiles flag?
   fileList                 = copy.fileList; // ?????

   if (spice) delete spice;
   spice                    = NULL;

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

   // Create SPICE interface
   if (!spice) spice = new SpiceInterface();

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
   // If it's already recording, continue
   if (!ephemFile)
   {
      if (theType != SPK)
         throw SubscriberException("Only SPK currently allowed for EphemManager\n");

      if (!spice)
         spice = new SpiceInterface();

      Spacecraft *theSc = (Spacecraft*) theObj;

      // Set up the name for the EphemerisFile, and the file name
      std::stringstream ss("");
      // @todo - put these files in the tmp directory (platform-dependent)
      ss << "tmp_" << theObjName << "_" << ephemCount;
      ephemName = ss.str();
      ss << ".bsp";
      fileName = ss.str();
      ephemFile         = new EphemerisFile(ephemName);

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
      spice->LoadKernel(fileName);
      // save the just written file name
      fileList.push_back(fileName);
   }
   ephemFile = NULL;
   recording = false;
}

//------------------------------------------------------------------------------
// void GetCoverageWindow(SpiceCell* w, bool includeAll = true)
//------------------------------------------------------------------------------
void EphemManager::GetCoverageWindow(SpiceCell* w, bool includeAll)
{
   // @todo - most of this should be moved to SpiceInterface and
   // made very general (for SPK, CK, etc.)
   // Set SPICEDOUBLE_CELL cover
//   if (cover) delete cover;
   if (!spice)
      spice = new SpiceInterface();
   Spacecraft *theSc = (Spacecraft*) theObj;
   Integer    forNaifId = theSc->GetIntegerParameter("NAIFId");

   // Which files do we need to check?
   StringArray inKernels = fileList;
   if (includeAll)
   {
      StringArray inputKernels = theSc->GetStringArrayParameter("OrbitSpiceKernelName");
      for (Integer ii = 0; ii < inputKernels.size(); ii++)
         inKernels.push_back(inputKernels.at(ii));
   }
   #ifdef DEBUG_EM_COVERAGE
      MessageInterface::ShowMessage("Entering GetCoverageWindow:\n");
      MessageInterface::ShowMessage("   forNaifId = %d\n", forNaifId);
      MessageInterface::ShowMessage("   kernels are:\n");
      if (inKernels.empty())  MessageInterface::ShowMessage("   EMPTY!!!!\n");
      else
      {
         for (unsigned int ii = 0; ii < inKernels.size(); ii++)
            MessageInterface::ShowMessage("   %d    %s\n", (Integer) ii, inKernels.at(ii).c_str());
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
      // start with an empty cell
      scard_c(0, &cover);   // reset the coverage cell

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
//            scard_c(0, &cover);   // reset the coverage cell
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
//            if ((firstInt) && (numInt > 0))
//            {
//               wnfetd_c(&cover, 0, &b, &e);
//               if (failed_c())
//               {
//                  ConstSpiceChar option[] = "LONG";
//                  SpiceInt       numChar  = MAX_LONG_MESSAGE_VALUE;
//                  //SpiceChar      err[MAX_LONG_MESSAGE_VALUE];
//                  SpiceChar      *err = new SpiceChar[MAX_LONG_MESSAGE_VALUE];
//                  getmsg_c(option, numChar, err);
//                  std::string errStr(err);
//                  std::string errmsg = "Error getting interval times for SPK kernel \"";
//                  errmsg += kernels.at(ii) + "\".  Message received from CSPICE is: [";
//                  errmsg += errStr + "]\n";
//                  reset_c();
//                  delete [] err;
//                  throw SubscriberException(errmsg);
//               }
//               start    = SpiceTimeToA1(b);
//               end      = SpiceTimeToA1(e);
//               firstInt = false;
//            }
//            for (SpiceInt jj = 0; jj < numInt; jj++)
//            {
//               wnfetd_c(&cover, jj, &b, &e);
//               bA1 = SpiceTimeToA1(b);
//               eA1 = SpiceTimeToA1(e);
//               if (bA1 < start)  start = bA1;
//               if (eA1 > end)    end   = eA1;
//            }
//         }

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
   copy_c(&cover, w);
}

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

////------------------------------------------------------------------------------
//// SetInitialEpoch()
////------------------------------------------------------------------------------
//void EphemManager::SetInitialEpoch(const std::string &ep)
//{
//}
//
////------------------------------------------------------------------------------
//// SetFinalEpoch()
////------------------------------------------------------------------------------
//void EphemManager::SetFinalEpoch(const std::string &ep)
//{
//}


