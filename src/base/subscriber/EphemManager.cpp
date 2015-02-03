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

//#define DEBUG_EPHEM_MANAGER

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
   deleteTmpFiles         (deleteFiles)
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
   deleteTmpFiles         (copy.deleteTmpFiles)
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


