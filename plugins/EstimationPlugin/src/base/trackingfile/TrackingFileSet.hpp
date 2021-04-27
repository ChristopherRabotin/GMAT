//$Id$
//------------------------------------------------------------------------------
//                           TrackingFileSet
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
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
#include "SignalDataCache.hpp"

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
   virtual std::string  GetParameterUnit(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;

   virtual bool         IsParameterReadOnly(const Integer id) const;
   virtual bool         IsParameterReadOnly(const std::string &label) const;

   virtual Integer      GetIntegerParameter(const Integer id) const;
   virtual Integer      SetIntegerParameter(const Integer id,
                                            const Integer value);
   virtual Integer      GetIntegerParameter(const std::string &label) const;
   virtual Integer      SetIntegerParameter(const std::string &label,
                                            const Integer value);

   virtual Real         GetRealParameter(const Integer id) const;
   virtual Real         GetRealParameter(const Integer id,
                                         const Integer index) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value);

   virtual Real         GetRealParameter(const std::string &label) const;
   virtual Real         GetRealParameter(const std::string &label,
                                         const Integer index) const;
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

   virtual UnsignedInt
                        GetPropertyObjectType(const Integer id) const;

   DEFAULT_TO_NO_CLONES

   virtual std::string  GetRefObjectName(const UnsignedInt type) const;
   virtual const ObjectTypeArray&
                        GetRefObjectTypeArray();
   virtual const StringArray&
                        GetRefObjectNameArray(const UnsignedInt type);
   virtual bool         SetRefObjectName(const UnsignedInt type,
                                         const std::string &name);
   virtual bool         RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName);
   virtual GmatBase*    GetRefObject(const UnsignedInt type,
                                     const std::string &name);
   virtual GmatBase*    GetRefObject(const UnsignedInt type,
                                     const std::string &name,
                                     const Integer index);
   virtual bool         SetRefObject(GmatBase *obj, const UnsignedInt type,
                                     const std::string &name = "");
   virtual bool         SetRefObject(GmatBase *obj, const UnsignedInt type,
                                     const std::string &name,
                                     const Integer index);
   virtual ObjectArray& GetRefObjectArray(const UnsignedInt type);
   virtual ObjectArray& GetRefObjectArray(const std::string& typeString);

   virtual void         SetSolarSystem(SolarSystem *ss);
   virtual void         SetPropagators(std::vector<PropSetup*> *ps,
                                       std::map<std::string, StringArray> *spMap);

   /// @note:  These SetPropagator methods are only used by the API
   virtual void         SetPropagator(PropSetup *ps);
   virtual void         SetPropagator(PropSetup *ps, const std::string &forSpacecraft);

   virtual bool         Initialize();

   const StringArray&   GetParticipants() const;
   TrackingDataAdapter  *GetAdapter(Integer index);
   std::vector<TrackingDataAdapter*> *GetAdapters();

   bool                 GenerateTrackingConfigs(std::vector<StringArray> strandsList, std::vector<StringArray> sensorsList, StringArray typesList);

   void                 ClearIonosphereCache();
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
      bool        SetDefinitionString(StringArray strand, StringArray sensors, std::string measType);

      /// The strings describing signal paths
      std::vector<StringArray> strands;
      /// The strings describing sensors
      std::vector<StringArray> sensors;
      /// The measurement types associated with the signal paths
      StringArray types;
   };

   /// Vector of scripted measurement configurations
   std::vector<MeasurementDefinition> trackingConfigs;
   /// The collection of measurements in the set
   std::vector<TrackingDataAdapter*> measurements;
   /// Name of the associate tracking data file
   StringArray filenames;
   /// Name of the associate ramp table
   StringArray rampedTablenames;
   /// Flag for the inclusion of light time solution
   bool        useLighttime;

   /// Flag for the inclusion of relativity correction
   bool        useRelativityCorrection;
   /// Flag for the inclusion of ET-TAI correction
   bool        useETminusTAICorrection;
   /// Type of angular aberration correction (None, Annual, Diurnal, or AnnualAndDiurnal)
   std::string aberrationCorrection;

   /// Range modulo constant
   Real        rangeModulo;
   /// Doppler count interval
   Real        dopplerCountInterval;

   
   /// TDRS parameters for simulation
   /// Service access list. It is used in TDRS doppler measurement to specify a list of serivce access used for measurement (SA1, SA2, or MA)
   StringArray tdrsServiceAccessList;
   /// Node 4 frequency
   Real tdrsNode4Frequency;
   /// Node 4 frequency band
   Integer tdrsNode4Band;
   /// TDRS SMAR Id
   Integer tdrsSMARID;
   /// TDRS data flag
   Integer tdrsDataFlag;

   /// Data filters
   StringArray dataFilterNames;
   ObjectArray dataFilters;

   /// Solar system used in the measurements
   SolarSystem *solarsystem;

   /// Propagators used for light time solution
   std::vector<PropSetup*> *thePropagators;
   /// Mapping for propagator overrides for specific spacecraft
   std::map<std::string, StringArray> *satPropagatorMap;

   /// Pointers to ref objects that the adapters use
   ObjectArray references;

   /// Parameter IDs for the TrackingFileSets
   enum
   {
      TRACKINGCONFIG = MeasurementModelBaseParamCount,
      FILENAME,
      RAMPED_TABLENAME,
      USELIGHTTIME,
      USE_RELATIVITY,
      USE_ETMINUSTAI,
      ABERRATION_CORRECTION,
      RANGE_MODULO,
      DOPPLER_COUNT_INTERVAL,
      TDRS_SERVICE_ACCESS,
      TDRS_NODE4_FREQUENCY,
      TDRS_NODE4_BAND,
      TDRS_SMAR_ID,
      TDRS_DATA_FLAG,
      DATA_FILTERS,

      // API fields used to access calculated measurements
      API_TRACKINGCONFIGCOUNT,
      API_GET_C_VALUE,

      TrackingFileSetParamCount,
   };

   /// Strings describing the BatchEstimator parameters
   static const std::string PARAMETER_TEXT[TrackingFileSetParamCount -
                                           MeasurementModelBaseParamCount];
   /// Types of the BatchEstimator parameters
   static const Gmat::ParameterType PARAMETER_TYPE[TrackingFileSetParamCount -
                                                   MeasurementModelBaseParamCount];

   TrackingDataAdapter *BuildAdapter(const StringArray &strand, const StringArray &sensors,                    // made changes by TUAN NGUYEN
         const std::string &type, Integer configIndex);

private:

   /// Cache for ionosphere corrections
   SignalDataCache::SimpleSignalDataCache ionosphereCache;

   /// Warning messages
   StringArray mesg;

   //std::string CheckTypeDeprecation(const std::string datatype);
   bool    ParseTrackingConfig(std::string value, Integer& configIndex, bool& start);
   bool    ParseStrand(std::string value, Integer configIndex, Integer strandIndex = 0);
   bool    AddToSignalPath(std::string participantName, Integer configIndex, Integer strandIndex);

   Integer openBracketCount;
   bool    start;

   /**
    * @note:  The propvec and spm members are only used by the API.  These
    * pieces are isolated here to allow multiple propagator support for API
    * users without needing to change the GMAT flow driven through the
    * navigation Solvers.
    */
   /// API vector of propagator pointers
   std::vector<PropSetup*> propvec;
   /// API mapping of propagators to spacecraft
   std::map<std::string, StringArray> spm;
};

#endif /* TrackingFileSet_hpp */
