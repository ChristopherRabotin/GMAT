//$Id$
//------------------------------------------------------------------------------
//                           TrackingFileSet
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task Order 28
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Mar 11, 2014
/**
 * The class of objects that define GMAT measurement models and tracking data
 */
//------------------------------------------------------------------------------

#ifndef TrackingFileSet_hpp
#define TrackingFileSet_hpp

#include "estimation_defs.hpp"
#include "GmatBase.hpp"
#include "MeasurementModelBase.hpp"
#include "TrackingDataAdapter.hpp"
#include "DataFile.hpp"

class SolarSystem;
class PropSetup;

/**
 * User access to measurement modes and tracking data file interfaces
 *
 * The TrackingFile set defines the measurements and tracking data that GMAT
 * uses in the estimation process.
 */
class ESTIMATION_API TrackingFileSet: public MeasurementModelBase
{
public:
   TrackingFileSet(const std::string &name);
   virtual ~TrackingFileSet();
   TrackingFileSet(const TrackingFileSet& tfs);
   TrackingFileSet& operator=(const TrackingFileSet& tfs);

   virtual GmatBase* Clone() const;

   // methods overridden from GmatBase
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;

   virtual Real         GetRealParameter(const Integer id) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value);
   virtual Real         GetRealParameter(const std::string &label) const;
   virtual Real         SetRealParameter(const std::string &label,
                                         const Real value);


   virtual std::string  GetStringParameter(const Integer id) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value);
   virtual std::string  GetStringParameter(const Integer id,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value,
                                           const Integer index);
   virtual const StringArray&
                        GetStringArrayParameter(const Integer id) const;
   virtual const StringArray&
                        GetStringArrayParameter(const Integer id,
                                                const Integer index) const;
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value);
   virtual std::string  GetStringParameter(const std::string &label,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value,
                                           const Integer index);

   virtual const StringArray&
                        GetStringArrayParameter(const std::string &label) const;
   virtual const StringArray&
                        GetStringArrayParameter(const std::string &label,
                                                const Integer index) const;

   virtual bool         GetBooleanParameter(const Integer id) const;
   virtual bool         SetBooleanParameter(const Integer id,
                                            const bool value);
   virtual bool         GetBooleanParameter(const Integer id,
                                            const Integer index) const;
   virtual bool         SetBooleanParameter(const Integer id,
                                            const bool value,
                                            const Integer index);
   virtual bool         GetBooleanParameter(const std::string &label) const;
   virtual bool         SetBooleanParameter(const std::string &label,
                                            const bool value);
   virtual bool         GetBooleanParameter(const std::string &label,
                                            const Integer index) const;
   virtual bool         SetBooleanParameter(const std::string &label,
                                            const bool value,
                                            const Integer index);

   DEFAULT_TO_NO_CLONES

   virtual std::string  GetRefObjectName(const Gmat::ObjectType type) const;
   virtual const ObjectTypeArray&
                        GetRefObjectTypeArray();
   virtual const StringArray&
                        GetRefObjectNameArray(const Gmat::ObjectType type);
   virtual bool         SetRefObjectName(const Gmat::ObjectType type,
                                         const std::string &name);
   virtual bool         RenameRefObject(const Gmat::ObjectType type,
                                        const std::string &oldName,
                                        const std::string &newName);
   virtual GmatBase*    GetRefObject(const Gmat::ObjectType type,
                                     const std::string &name);
   virtual GmatBase*    GetRefObject(const Gmat::ObjectType type,
                                     const std::string &name,
                                     const Integer index);
   virtual bool         SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                     const std::string &name = "");
   virtual bool         SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                     const std::string &name,
                                     const Integer index);
   virtual ObjectArray& GetRefObjectArray(const Gmat::ObjectType type);
   virtual ObjectArray& GetRefObjectArray(const std::string& typeString);

   virtual void SetSolarSystem(SolarSystem *ss);
   virtual void SetPropagator(PropSetup *ps);
   virtual bool Initialize();
   std::vector<TrackingDataAdapter*> *GetAdapters();

protected:
   /**
    * Internal class used to match strand and model descriptions together, as
    * scripted.
    */
   class MeasurementDefinition
   {
   public:
      MeasurementDefinition();
      ~MeasurementDefinition();
      MeasurementDefinition(const MeasurementDefinition& md);
      MeasurementDefinition& operator=(const MeasurementDefinition& md);

      std::string GetDefinitionString() const;

      /// The strings describing the tracking configs
      std::vector<StringArray> strands;
      /// The measurement types associated with each tracking config
      StringArray types;
   };

   /// Vector of scripted measurement configurations
   std::vector<MeasurementDefinition> trackingConfigs;
   /// The collection of measurements in the set
   std::vector<TrackingDataAdapter*> measurements;
   /// Name of the associate tracking data file
   StringArray filenames;
   /// Name of the associate ramped table                      // made changes by TUAN NGUYEN
   StringArray rampedTablenames;                               // made changes by TUAN NGUYEN
   /// Flag for the inclusion of light time solution
   bool useLighttime;

   /// Flag for the inclusion of relativity correction         // made changes by TUAN NGUYEN
   bool useRelativityCorrection;                               // made changes by TUAN NGUYEN
   /// Flag for the inclusion of ET-TAI correction             // made changes by TUAN NGUYEN
   bool useETminusTAICorrection;                               // made changes by TUAN NGUYEN
   /// Name of troposphere model                               // made changes by TUAN NGUYEN
   std::string troposphereModel;                               // made changes by TUAN NGUYEN
   /// Name of ionosphere model                                // made changes by TUAN NGUYEN
   std::string ionosphereModel;                                // made changes by TUAN NGUYEN

   /// Noise
   Real rangeNoiseSigma;                                       // made changes by TUAN NGUYEN
   std::string rangeErrorModel;                                // made changes by TUAN NGUYEN
   Real dopplerNoiseSigma;                                     // made changes by TUAN NGUYEN
   std::string dopplerErrorModel;                              // made changes by TUAN NGUYEN

   /// Range modulo constant                                   // made changes by TUAN NGUYEN
   Real rangeModulo;                                           // made changes by TUAN NGUYEN

   // Doppler interval
   Real dopplerInterval;

   /// Solar system used in the measurements
   SolarSystem *solarsystem;
   /// @todo Adjust this code when multiple propagator support is implemented
   /// Propagator used for light time computations
   PropSetup *thePropagator;
   /// Pointers to ref objects that the adapters use
   ObjectArray references;
   /// Pointers to the Datafile Objects specified for this TFS
   std::vector<DataFile*> datafiles;

   /// Parameter IDs for the TrackingFileSets
   enum
   {
      TRACKINGCONFIG = MeasurementModelBaseParamCount,      // made changes by TUAN NGUYEN
      FILENAME,
      RAMPED_TABLENAME,                                     // made changes by TUAN NGUYEN
      USELIGHTTIME,
      USE_RELATIVITY,                                       // made changes by TUAN NGUYEN
      USE_ETMINUSTAI,                                       // made changes by TUAN NGUYEN
      TROPOSPHERE_MODEL,                                    // made changes by TUAN NGUYEN
      IONOSPHERE_MODEL,                                     // made changes by TUAN NGUYEN
      RANGE_NOISESIGMA,                                     // made changes by TUAN NGUYEN
      RANGE_ERRORMODEL,                                     // made changes by TUAN NGUYEN
      DOPPLER_NOISESIGMA,                                   // made changes by TUAN NGUYEN
      DOPPLER_ERRORMODEL,                                   // made changes by TUAN NGUYEN
      RANGE_MODULO,                                         // made changes by TUAN NGUYEN
      DOPPLER_INTERVAL,
      TrackingFileSetParamCount,
   };

   /// Strings describing the BatchEstimator parameters
   static const std::string PARAMETER_TEXT[TrackingFileSetParamCount -
                                           MeasurementModelBaseParamCount];              // made changes by TUAN NGUYEN
   /// Types of the BatchEstimator parameters
   static const Gmat::ParameterType PARAMETER_TYPE[TrackingFileSetParamCount -
                                                   MeasurementModelBaseParamCount];      // made changes by TUAN NGUYEN

   TrackingDataAdapter *BuildAdapter(const StringArray &strand,
         const std::string &type);
};

#endif /* TrackingFileSet_hpp */
