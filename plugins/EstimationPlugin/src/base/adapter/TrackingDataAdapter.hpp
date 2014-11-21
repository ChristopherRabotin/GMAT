//$Id$
//------------------------------------------------------------------------------
//                           TrackingDataAdapter
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
// Created: Feb 7, 2014
/**
 * Base class for the tracking data adapters
 */
//------------------------------------------------------------------------------

#ifndef TrackingDataAdapter_hpp
#define TrackingDataAdapter_hpp

#include "MeasurementModelBase.hpp"
#include "MeasureModel.hpp"
#include "MeasurementData.hpp"
#include "ProgressReporter.hpp"
#include "RampTableData.hpp"                         // made changes by TUAN NGUYEN
#include "ObservationData.hpp"                       // made changes by TUAN NGUYEN

// Forward reference
class SolarSystem;
class PropSetup;

/**
 * Base class for the tracking data adapters
 *
 * The tracking data adapters convert the raw data computed in measurement
 * models into the formatted data needed for the estimation and simulation
 * processes.  For many measurement types, this conversion is a very lightweight
 * process.
 *
 * The adapters also assemble the derivative data needed by these processes.
 */
class ESTIMATION_API TrackingDataAdapter: public MeasurementModelBase
{
public:
   TrackingDataAdapter(const std::string &typeStr, const std::string &name);
   virtual ~TrackingDataAdapter();
   TrackingDataAdapter(const TrackingDataAdapter& ma);
   TrackingDataAdapter& operator=(const TrackingDataAdapter& ma);

   virtual void SetSolarSystem(SolarSystem *ss);

   // Access methods derived classes can override
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;


   virtual Integer      GetIntegerParameter(const Integer id) const;
   virtual Integer      SetIntegerParameter(const Integer id,
                                            const Integer value);
   virtual Integer      GetIntegerParameter(const std::string &label) const;
   virtual Integer      SetIntegerParameter(const std::string &label,
                                            const Integer value);

   virtual Real         GetRealParameter(const Integer id) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value);
   virtual Real         GetRealParameter(const Integer id,
                                         const Integer index) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value,
                                         const Integer index);
   virtual Real         GetRealParameter(const std::string &label) const;
   virtual Real         SetRealParameter(const std::string &label,
                                         const Real value);
   virtual Real         GetRealParameter(const std::string &label,
                                         const Integer index) const;
   virtual Real         SetRealParameter(const std::string &label,
                                         const Real value,
                                         const Integer index);

   virtual bool         GetBooleanParameter(const Integer id) const;
   virtual bool         SetBooleanParameter(const Integer id,
                                            const bool value);
   virtual bool         GetBooleanParameter(const std::string &label) const;
   virtual bool         SetBooleanParameter(const std::string &label,
                                            const bool value);


   virtual std::string  GetStringParameter(const Integer id) const;
   virtual bool SetStringParameter(const Integer id,
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

   virtual const StringArray&
                        GetRefObjectNameArray(const Gmat::ObjectType type);
   virtual bool         RenameRefObject(const Gmat::ObjectType type,
                                        const std::string &oldName,
                                        const std::string &newName);
//   virtual ObjectArray& GetRefObjectArray(const Gmat::ObjectType type);     // ^^^^
   virtual bool         SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                     const std::string &name = "");
   virtual bool         SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                     const std::string &name,
                                     const Integer index);


   virtual void         SetPropagator(PropSetup *ps);
   virtual bool         Initialize();

   DEFAULT_TO_NO_CLONES

   // Preserve interfaces in the older measurement model code
   virtual const MeasurementData&
                        CalculateMeasurement(bool withEvents = false,
                              ObservationData* forObservation = NULL,
                              std::vector<RampTableData>* rampTB = NULL) = 0;
   virtual const std::vector<RealArray>&
                        CalculateMeasurementDerivatives(GmatBase *obj,
                              Integer id) = 0;

   virtual bool         SetMeasurement(MeasureModel *meas);
   void                 SetModelID(Integer newID);
   Integer              GetModelID();
   void                 SetModelTypeID(const Integer theID,
                                       const std::string type, Real mult = 1.0);
   Integer              GetModelTypeID();

   virtual const MeasurementData&
                        GetMeasurement();
   virtual bool         WriteMeasurements();
   virtual bool         WriteMeasurement(Integer id);

   // Covariance handling code
   virtual Integer      HasParameterCovariances(Integer parameterId);

   virtual Integer      GetEventCount();

   virtual void         SetCorrection(const std::string& correctionName,
                                      const std::string& correctionType);

   // Measurement Model Settings
   virtual bool         SetProgressReporter(ProgressReporter* reporter);
   virtual void         UsesLightTime(const bool tf);

   // Multiplier Factor                                                // made changes by TUAN NGUYEN
   virtual void         SetMultiplierFactor(Real mult);                // made changes by TUAN NGUYEN
   virtual Real         GetMultiplierFactor();                         // made changes by TUAN NGUYEN

   // Get measurement model object
   virtual MeasureModel * GetMeasurementModel();                       // made changes by TUAN NGUYEN
   //// Get participants' names
   //virtual const std::vector<StringArray*>                             // made changes by TUAN NGUYEN
   //   GetParticipantNameLists() { return participantLists;}            // made changes by TUAN NGUYEN

   virtual void         AddBias(bool isAdd) {addBias = isAdd;}         // made changes by TUAN NGUYEN

protected:
   /// The ordered list of participants in the measurement
   std::vector<StringArray*> participantLists;
   /// The measurement model that holds the raw data processed in this adapter
   MeasureModel              *calcData;
   /// Most recently computed measurement data
   MeasurementData           cMeasurement;
   /// Measurement derivatives
   std::vector<RealArray>    theDataDerivatives;

   // Noise model paramters                                                                         // made changes by TUAN NGUYEN
   /// Noise sigma (noiseSigma[i]) associated to measuremnet (cMeasurement.value[i])                // made changes by TUAN NGUYEN
   RealArray                 noiseSigma;                                                            // made changes by TUAN NGUYEN
   /// Measurement bias (measurementBias[i]) asociated to measurement (cMeasurement.value[i])       // made changes by TUAN NGUYEN
   RealArray                 measurementBias;                                                       // made changes by TUAN NGUYEN

   /// Reporter for the adapter
   ProgressReporter          *navLog;
   /// Current logging level for adapters
   UnsignedInt               logLevel;
   /// Solar system used in the modeling
   SolarSystem               *solarsys;
   /// Array of reference objects
   ObjectArray               refObjects;
   /// Unique measurement ID used during a run
   Integer                   modelID;
   /// Measurement type ID
   Integer                   modelTypeID;
   /// Text name of the model type
   std::string               modelType;
   /// Scaling factor used in some measurements -- e.g. 2-way range divide by 2
   Real                      multiplier;

   /// Flags controlling the internal computations and corrections
   bool                      withLighttime;
   /// Propagator used for light time solutions, when needed
   PropSetup                 *thePropagator;


   /// Constant frequency value used in a physical measurement when needed (In DSNDoppler, it is used as uplink frequency for S path
   Real                      uplinkFreq;                           // unit is MHz                        // made changes by TUAN NGUYEN
   /// Frequency band   (In DSNDoppler, it is used for S path)                                           // made changes by TUAN NGUYEN
   Integer                   freqBand;                                                                   // made changes by TUAN NGUYEN
   /// Observation data object containing an observation data record                                     // made changes by TUAN NGUYEN
   ObservationData*          obsData;                                                                    // made changes by TUAN NGUYEN

   /// Ramped frequency table used to calculate ramped frequency measurements                            // made changes by TUAN NGUYEN
   std::vector<RampTableData>*rampTB;                                                                    // made changes by TUAN NGUYEN
   UnsignedInt               beginIndex;                                                                 // made changes by TUAN NGUYEN
   UnsignedInt               endIndex;                                                                   // made changes by TUAN NGUYEN

   /// Name of the frequency ramp table that supplied or receives data                                   // made changes by TUAN NGUYEN
   StringArray               rampTableNames;                                                             // made changes by TUAN NGUYEN
   /// Add noise to measurement (used only for simulation)                                               // made changes by TUAN NGUYEN
   bool                      addNoise;                                                                   // made changes by TUAN NGUYEN
   /// Add bias to measurement                                                                           // made changes by TUAN NGUYEN
   bool                      addBias;                                                                    // made changes by TUAN NGUYEN
   
   /// Measurement error covariance matrix                                                               // made changes by TUAN NGUYEN
   Covariance                measErrorCovariance;                                                        // made changes by TUAN NGUYEN

   /// Measurement type
   std::string               measurementType;  // it's value could be "Range", "DSNRange", "Doppler", etc


   /// Parameter IDs for the TrackingDataAdapter
   enum
   {
      SIGNAL_PATH  = MeasurementModelBaseParamCount,
      OBS_DATA,                                           // made changes by TUAN NGUYEN
      RAMPTABLES,                                         // made changes by TUAN NGUYEN
      MEASUREMENT_TYPE,                                   // made changes by TUAN NGUYEN
      BIAS,                                               // made changes by TUAN NGUYEN
      //NOISE_SIGMA,                                        // made changes by TUAN NGUYEN
      //ERROR_MODEL,                                        // made changes by TUAN NGUYEN
      ADD_NOISE,                                          // made changes by TUAN NGUYEN
      UPLINK_FREQUENCY,                                   // made changes by TUAN NGUYEN
      UPLINK_BAND,                                        // made changes by TUAN NGUYEN
      AdapterParamCount,                                  // made changes by TUAN NGUYEN
   };


   /// Strings describing the TrackingDataAdapter parameters
   static const std::string PARAMETER_TEXT[AdapterParamCount -
                                           MeasurementModelBaseParamCount];                              // made changes by TUAN NGUYEN
   /// Types of the TrackingDataAdapter parameters
   static const Gmat::ParameterType PARAMETER_TYPE[AdapterParamCount -
                                                   MeasurementModelBaseParamCount];                      // made changes by TUAN NGUYEN

   StringArray*         DecomposePathString(const std::string &value);
   
   void                 ComputeMeasurementBias(const std::string biasName);                              // made changes by TUAN NGUYEN
   void                 ComputeMeasurementNoiseSigma(const std::string noiseSigmaName);                  // made changes by TUAN NGUYEN
   void                 ComputeMeasurementErrorCovarianceMatrix();                                       // made changes by TUAN NGUYEN

   void                 BeginEndIndexesOfRampTable(Integer & err);                                       // made changes by TUAN NGUYEN
   virtual Real         IntegralRampedFrequency(Real t1, Real delta_t, Integer& err);                    // made changes by TUAN NGUYEN

};

#endif /* TrackingDataAdapter_hpp */
