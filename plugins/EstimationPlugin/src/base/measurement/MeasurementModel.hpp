//$Id: MeasurementModel.hpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                          MeasurementModel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2009/06/24
//
/**
 * MeasurementModel declaration used in GMAT's estimators and simulator
 */
//------------------------------------------------------------------------------

#ifndef MeasurementModel_hpp
#define MeasurementModel_hpp

#include "estimation_defs.hpp"
#include "GmatBase.hpp"
#include "MeasurementModelBase.hpp"
#include "EstimationDefs.hpp"
#include "MeasurementData.hpp"
#include "CoreMeasurement.hpp"
#include "Hardware.hpp"
///// TBD: Do we want something more generic here?
#include "ObservationData.hpp"
#include "RampTableData.hpp"

/**
 * Container class that wraps CoreMeasurement objects for use in estimation and
 * simulation
 */
class ESTIMATION_API MeasurementModel : public MeasurementModelBase
{
public:
   MeasurementModel(const std::string &nomme = "");
   virtual ~MeasurementModel();
   MeasurementModel(const MeasurementModel &mm);
   MeasurementModel& operator=(const MeasurementModel &mm);

   virtual GmatBase* Clone() const;
   virtual bool Initialize();

   // Access methods derived classes can override
   virtual std::string  GetParameterText(const Integer id) const;
   virtual std::string  GetParameterUnit(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;
   virtual Integer      GetParameterCount() const;

   virtual bool         IsParameterReadOnly(const Integer id) const;
   virtual bool         IsParameterReadOnly(const std::string &label) const;

   virtual bool         IsEstimationParameterValid(const Integer id);

   virtual Real         GetRealParameter(const Integer id) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value);
   virtual Real         GetRealParameter(const Integer id,
                                         const Integer index) const;
   virtual Real         GetRealParameter(const Integer id, const Integer row,
                                         const Integer col) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value,
                                         const Integer index);
   virtual Real         SetRealParameter(const Integer id, const Real value,
                                         const Integer row, const Integer col);
   virtual Real         GetRealParameter(const std::string &label) const;
   virtual Real         SetRealParameter(const std::string &label,
                                         const Real value);
   virtual Real         GetRealParameter(const std::string &label,
                                         const Integer index) const;
   virtual Real         SetRealParameter(const std::string &label,
                                         const Real value,
                                         const Integer index);
   virtual Real         GetRealParameter(const std::string &label,
                                         const Integer row,
                                         const Integer col) const;
   virtual Real         SetRealParameter(const std::string &label,
                                         const Real value, const Integer row,
                                         const Integer col);

   virtual std::string  GetOnOffParameter(const Integer id) const;
   virtual bool         SetOnOffParameter(const Integer id,
                                         const std::string &value);

   virtual std::string  GetStringParameter(const Integer id) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value);
   virtual std::string  GetStringParameter(const Integer id,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value,
                                           const Integer index);
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value);
   virtual std::string  GetStringParameter(const std::string &label,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value,
                                           const Integer index);

   virtual const StringArray&
                        GetStringArrayParameter(const Integer id) const;
   virtual const StringArray&
                        GetStringArrayParameter(const Integer id,
                                                const Integer index) const;
   virtual const StringArray&
                        GetStringArrayParameter(const std::string &label) const;
   virtual const StringArray&
                        GetStringArrayParameter(const std::string &label,
                                                const Integer index) const;

   virtual const Rvector&
                        GetRvectorParameter(const Integer id) const;
   virtual const Rvector&
                        SetRvectorParameter(const Integer id,
                                            const Rvector &value);
   virtual const Rvector&
                        GetRvectorParameter(const std::string &label) const;
   virtual const Rvector&
                        SetRvectorParameter(const std::string &label,
                                            const Rvector &value);

   // Access methods derived classes can override on reference objects
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

   virtual bool         IsOwnedObject(Integer id) const;
   virtual Integer      GetOwnedObjectCount();
   virtual GmatBase*    GetOwnedObject(Integer whichOne);

   Integer              GetModelID();
   Integer              GetModelTypeID();
   void                 SetModelID(Integer newID);

   virtual ObjectArray&     GetParticipants();

///// TBD: Do we want something more generic here?
   virtual const MeasurementData&
                        CalculateMeasurement(bool withEvents = false,
                           ObservationData* forObservation = NULL, std::vector<RampTableData>* rampTB = NULL, 
                           bool withNoise = false);
   virtual const std::vector<RealArray>&
                        CalculateMeasurementDerivatives(GmatBase *obj,
                                                        Integer id);
   virtual bool         SetMeasurement(CoreMeasurement *meas);
   virtual const MeasurementData&
                        GetMeasurement();
   virtual bool         WriteMeasurements();
   virtual bool         WriteMeasurement(Integer id);

   // Covariance handling code
   virtual Integer      HasParameterCovariances(Integer parameterId);
//   virtual Rmatrix*        GetParameterCovariances(Integer parameterId = -1);

   virtual Integer      GetEventCount();
   virtual Event*       GetEvent(Integer whichOne);
   virtual bool         SetEventData(Event *locatedEvent = NULL);

   virtual void         SetCorrection(const std::string& correctionName,
         const std::string& correctionType);

   virtual Real*        GetEstimationParameterValue(const Integer item);
   virtual Integer      GetEstimationParameterSize(const Integer item);

   /// @todo: Check this
   DEFAULT_TO_NO_CLONES


protected:
   /// Name of the observation stream that supplied or receives data
   StringArray          observationStreamName;

///// TBD: Do we want something more generic here?
   /// Name of the frequency ramp table stream that supplied or receives data
   StringArray          rampTableStreamName;

   /// List of participants used in the contained measurement
   StringArray          participantNames;
   /// Pointers to the participants
   ObjectArray          participants;
   /// Names of the scripted participant hardware elements
   std::vector<StringArray> participantHardwareNames;
   /// The type of measurement
   std::string          measurementType;
   /// An integer representing the type of measurement
   Integer              measurementTypeID;
   /// The core measurement component
   CoreMeasurement      *measurement;
   /// Measurement data
   MeasurementData      *theData;
   /// And derivatives
   std::vector<RealArray> *theDataDerivatives;

   /// Local storage element for ref object names
   StringArray          refObjectList;

   // Noise model parameters
   /// Measurement bias
   Rvector  measurementBias;
   /// Noise sigma
   Rvector noiseSigma;

   /// Error covariance; the inherited Covariance is used for a priori data
   Covariance measErrorCovariance;

   /// Internal ID used for this measurement model
   Integer modelID;

   /// Flag that is set if participants need to be passed in later
   bool measurementNeedsObjects;

///// TBD: Do we want something more generic here?
   /// Flag indicate whether Measurement model using relativity correction or not
   bool useRelativityCorrection;

   /// Flag indicate whether Measurement model using ET-TAI correction or not
   bool useETminusTAICorrection;

//   /// Residual maximum
//   Real residualMax;

   /// Enumeration defining the MeasurementModel's scriptable parameters
   enum
   {
      ObsData = GmatBaseParamCount,
      RampTables,
      MeasurementType,
      Participants,
      Bias,
      NoiseSigma,
      Frequency,
//      RangeModuloConstant,
      RelativityCorrection,
      ETminusTAICorrection,
//      ResidualMaxLimit,
      MeasurementModelParamCount
   };

   // Start with the parameter IDs and associates strings
   /// Script labels for the MeasurementModel parameters
   static const std::string
                PARAMETER_TEXT[MeasurementModelParamCount - GmatBaseParamCount];
   /// IDs for the MeasurementModel parameters
   static const Gmat::ParameterType
                PARAMETER_TYPE[MeasurementModelParamCount - GmatBaseParamCount];

   // Hardware accessors
   virtual Hardware*          GetHardwarebyType(GmatBase *onObject,
                                    const std::string &hwType,
                                    const std::string hwName = "");
   virtual Hardware*          GetHardware(GmatBase *onObject,
                                    const std::string hwName);
};

#endif /* MeasurementModel_hpp */
