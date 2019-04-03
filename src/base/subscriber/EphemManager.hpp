//$Id$
//------------------------------------------------------------------------------
//                               EphemManager
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
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
// Author: Wendy C. Shoan
// Created: 2014.10.28
//
/**
 * Base class definition for the EphemManager.  The EphemManager is
 * responsible for creating, loading, and managing private/hidden EphemerisFile
 * objects associated with its specified Spacecraft or Asset object.
 * NOTE: currently, the EphemManager will only handle SPK Orbit files, and
 * FK text files for the GroundStation.
 */
//------------------------------------------------------------------------------

#ifndef EphemManager_hpp
#define EphemManager_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "CoordinateSystem.hpp"
#include "SolarSystem.hpp"

#ifdef __USE_SPICE__
   #include "SpiceInterface.hpp"
#endif



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
      FK,
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
   virtual void         StopRecording(bool done = true);

   /// method to determine occultation intervals
   bool                 GetOccultationIntervals(const std::string &occType,
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
                                                RealArray         &ends);

   /// @YRL
   bool                 GetContactIntervals(const std::string &observerID,
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
                                            RealArray         &ends);

   bool                 GetCoverage(Real s, Real e,
                                    bool useEntireIntvl,
                                    bool includeAll,
                                    Real &intvlStart,
                                    Real &intvlStop,
                                    Real &cvrStart,
                                    Real &cvrStop);

   /// Set reference objects
   virtual void         SetObject(GmatBase *obj);
   virtual void         SetEphemType(ManagedEphemType eType);
   virtual void         SetCoordinateSystem(CoordinateSystem *cs);
   virtual void         SetSolarSystem(SolarSystem *ss);

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
   /// the solar system
   SolarSystem          *solarSys;
   /// the Subscriber to which the Ephem will be written
   EphemerisFile        *ephemFile;
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
   /// start time of the observation window
   Real                 intStart;
   /// stop time of the observation window
   Real                 intStop;
   /// start time of the actual coverage window (coverage of loaded SPKs)
   Real                 coverStart;
   /// stop time of the actual coverage window (coverage of loaded SPKs)
   Real                 coverStop;
   #ifdef __USE_SPICE__
      /// need a SpiceInterface to load and unload kernels
      SpiceInterface       *spice;
      /// The window specifying the SPK coverage for this object
      SpiceCell            *cover;
      /// The window specifying the desired observation window for this
      /// object
      SpiceCell            *window;
      /// Method to determine the coverage window(s) for the spacecraft
      void                 GetRequiredCoverageWindow(SpiceCell* w, Real s, Real e,
                                                     bool useEntireIntvl,
                                                     const std::string &abCorr     = "NONE",
                                                     bool includeAll          = true,
                                                     bool lightTimeCorrection = false,
                                                     bool transmit = false,
                                                     Real stepSize = 10.0,
                                                     Integer obsID = -999);

   #endif

};

#endif // EphemManager_hpp
