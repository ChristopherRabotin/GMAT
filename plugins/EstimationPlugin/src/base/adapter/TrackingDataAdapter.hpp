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

protected:
   /// The ordered list of participants in the measurement
   std::vector<StringArray*> participantLists;
   /// The measurement model that holds the raw data processed in this adapter
   MeasureModel *calcData;
   /// Most recently computed measurement data
   MeasurementData cMeasurement;
   /// Measurement derivatives
   std::vector<RealArray> theDataDerivatives;
   /// Reporter for the adapter
   ProgressReporter *navLog;
   /// Current logging level for adapters
   UnsignedInt logLevel;
   /// Solar system used in the modeling
   SolarSystem *solarsys;
   /// Array of reference objects
   ObjectArray refObjects;
   /// Unique measurement ID used during a run
   Integer modelID;
   /// Measurement type ID
   Integer modelTypeID;
   /// Text name of the model type
   std::string modelType;
   /// Scaling factor used in some measurements -- e.g. 2-way range divide by 2
   Real multiplier;

   /// Flags controlling the internal computations and corrections
   bool withLighttime;
   /// Propagator used for light time solutions, when needed
   PropSetup *thePropagator;

   /// Parameter IDs for the BatchEstimators
   enum
   {
      SIGNAL_PATH  = GmatBaseParamCount,
      AdapterParamCount,
   };

   /// Strings describing the BatchEstimator parameters
   static const std::string PARAMETER_TEXT[AdapterParamCount -
                                           GmatBaseParamCount];
   /// Types of the BatchEstimator parameters
   static const Gmat::ParameterType PARAMETER_TYPE[AdapterParamCount -
                                                   GmatBaseParamCount];

   StringArray*         DecomposePathString(const std::string &value);
};

#endif /* TrackingDataAdapter_hpp */
