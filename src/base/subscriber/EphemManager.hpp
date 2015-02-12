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
 * Base class definition for the EphemManager.  The EphemManager is
 * responsible for creating, loading, and managing private/hidden EphemerisFile
 * objects associated with its specified Spacecraft object.
 * NOTE: currently, the EphemManager will only handle SPK Orbit files.
 */
//------------------------------------------------------------------------------

#ifndef EphemManager_hpp
#define EphemManager_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "CoordinateSystem.hpp"
#include "SpiceInterface.hpp"

// Declare forward reference
class EphemerisFile;

/**
 * Manager for ephemeris recording for the specified object
 */
class GMAT_API EphemManager
{
public:
   /// The types of Ephems that can be managed
   enum ManagedEphemType
   {
      SPK,
      CK,     // future
      CCSDS,  // future
   };

   EphemManager(bool deleteFiles = true);
   virtual ~EphemManager();
   EphemManager(const EphemManager& copy);
   EphemManager& operator=(const EphemManager& copy);

   /// Initialize the EphemManager
   virtual bool         Initialize();

   /// Create the EphemerisFile and set to begin recording
   virtual bool         RecordEphemerisData();
   /// Load the created file and set up to continue (with a new EphemerisFile)
   virtual bool         ProvideEphemerisData();
   /// Stop recording - load the last ephem data - this must be called
   /// at the end of the run for the last-written SPK to be loaded correctly
   virtual void         StopRecording();

   void                 GetCoverageWindow(SpiceCell* w, bool includeAll = true);

   /// Set reference objects
   virtual void         SetObject(GmatBase *obj);
   virtual void         SetEphemType(ManagedEphemType eType);
   virtual void         SetCoordinateSystem(CoordinateSystem *cs);

//   virtual void         SetInitialEpoch(const std::string &ep);
//   virtual void         SetFinalEpoch(const std::string &ep);

protected:
//   /// Epoch format
//   std::string          epochFormat;
   /// Initial epoch as input/set
   std::string          initialEpoch;
   /// Final epoch as input/set
   std::string          finalEpoch;
   /// The ephem type
   ManagedEphemType     theType;
   /// the name of the object whose ephemeris is being managed
   std::string          theObjName;
   /// pointer to the object whose ephemeris is being managed
   GmatBase             *theObj;
   /// the Subscriber to which the Ephem will be written
   EphemerisFile        *ephemFile;
   /// need a SpiceInterface to load and unload kernels
   SpiceInterface       *spice;
   /// CoordinateSystem to use for the EphemerisFile
   CoordinateSystem     *coordSys;
   /// Name of the specified CoordinateSystem
   std::string          coordSysName;
   /// Name of the currently managed EphemerisFile object
   std::string          ephemName;
   /// Number of EphemerisFiles created by this EphemManager
   Integer              ephemCount;
   /// the name of the file to which the ephem will be written
   std::string          fileName;
   /// Is the EphemManager currently recording data
   bool                 recording;
   /// Do we want to delete the files on destruction?
   bool                 deleteTmpFiles;
   /// List of created files
   StringArray          fileList;
   /// The window specifying the SPK coverage for this object
   SpiceCell            *cover;

};

#endif // EphemManager_hpp
