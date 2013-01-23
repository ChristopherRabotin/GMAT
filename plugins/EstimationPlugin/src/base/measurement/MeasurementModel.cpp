//$Id: MeasurementModel.cpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                          MeasurementModel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2009/06/24
//
/**
 * MeasurementModel implementation used in GMAT's estimators and simulator
 */
//------------------------------------------------------------------------------

#include "MeasurementModel.hpp"
#include "MeasurementException.hpp"
#include "MessageInterface.hpp"

#include "PhysicalMeasurement.hpp"  // For parameters that only apply to
                                    // real world physical models


//#define TEST_FIRE_MEASUREMENT
//#define DEBUG_MEASUREMENT_INITIALIZATION
//#define DEBUG_HARDWARE
//#define DEBUG_DERIVATIVES
//#define DEBUG_SET_CORRECTION

//------------------------------------------------------------------------------
// Static data initialization
//------------------------------------------------------------------------------

const std::string MeasurementModel::PARAMETER_TEXT[] =
{
   "ObservationData",
   "Type",
   "Participants",
   "Bias",
   "NoiseSigma",
   "TimeConstant",
   "Frequency"
};


const Gmat::ParameterType MeasurementModel::PARAMETER_TYPE[] =
{
   Gmat::OBJECTARRAY_TYPE,
   Gmat::OBJECT_TYPE,
   Gmat::OBJECTARRAY_TYPE,
   Gmat::RVECTOR_TYPE,
   Gmat::RVECTOR_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
};

//------------------------------------------------------------------------------
// Public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Object construction, destruction, and replication
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// MeasurementModel(const std::string &nomme)
//------------------------------------------------------------------------------
/**
 * Standard GMAT constructor
 *
 * @param nomme The new object's name
 */
//------------------------------------------------------------------------------
MeasurementModel::MeasurementModel(const std::string &nomme) :
   GmatBase                (Gmat::MEASUREMENT_MODEL, "MeasurementModel", nomme),
   measurementType         ("NoTypeSet"),
   measurement             (NULL),
   theData                 (NULL),
   theDataDerivatives      (NULL),
   timeConstant            (6000.0),
   modelID                 (-1),
   measurementNeedsObjects (false)
{
   objectTypes.push_back(Gmat::MEASUREMENT_MODEL);
   objectTypeNames.push_back("MeasurementModel");

   parameterCount = MeasurementModelParamCount;
}

//------------------------------------------------------------------------------
// ~MeasurementModel()
//------------------------------------------------------------------------------
/**
 * Measurement Model destructor
 */
//------------------------------------------------------------------------------
MeasurementModel::~MeasurementModel()
{
}

//------------------------------------------------------------------------------
// MeasurementModel(const MeasurementModel &mm) :
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param mm The model that is copied here
 */
//------------------------------------------------------------------------------
MeasurementModel::MeasurementModel(const MeasurementModel &mm) :
   GmatBase                (mm),
   observationStreamName   (mm.observationStreamName),
   participantNames        (mm.participantNames),
   participantHardwareNames(mm.participantHardwareNames),
   measurementType         (mm.measurementType),
   theData                 (NULL),
   theDataDerivatives      (NULL),
   timeConstant            (mm.timeConstant),
   modelID                 (mm.modelID),
   measurementNeedsObjects (false)
{
   if (mm.measurement != NULL)
   {
      measurement = (CoreMeasurement*)mm.measurement->Clone();
      measurementBias.SetSize(measurement->GetMeasurementSize());
      measurementBias = mm.measurementBias;
      noiseSigma.SetSize(measurement->GetMeasurementSize());
      noiseSigma = mm.noiseSigma;
   }
   if (participants.size() > 0)
         measurementNeedsObjects = true;

   for (UnsignedInt i = 0; i < participantHardwareNames.size(); ++i)
      participantHardwareNames[i] = mm.participantHardwareNames[i];
}

//------------------------------------------------------------------------------
// MeasurementModel& MeasurementModel::operator=(const MeasurementModel &mm)
//------------------------------------------------------------------------------
/**
 * Measurement model assignment operator
 *
 * @param mm The model that is copied here
 *
 * @return this measurement model, set to look like mm
 */
//------------------------------------------------------------------------------
MeasurementModel& MeasurementModel::operator=(const MeasurementModel &mm)
{
   if (&mm != this)
   {
      GmatBase::operator=(mm);
      observationStreamName   = mm.observationStreamName;
      participantNames        = mm.participantNames;
      participantHardwareNames= mm.participantHardwareNames;
      measurementType         = mm.measurementType;
      theData                 = NULL;
      theDataDerivatives      = NULL;
      timeConstant            = mm.timeConstant;
      modelID                 = mm.modelID;

      if (mm.measurement != NULL)
      {
         measurement = (CoreMeasurement*)mm.measurement->Clone();
         measurementBias.SetSize(measurement->GetMeasurementSize());
         measurementBias = mm.measurementBias;
         noiseSigma.SetSize(measurement->GetMeasurementSize());
         noiseSigma = mm.noiseSigma;
      }

      // Clear participant list so it can be filled with the local pointers
      participants.clear();

      for (UnsignedInt i = 0; i < participantHardwareNames.size(); ++i)
         participantHardwareNames[i] = mm.participantHardwareNames[i];
   }

   return *this;
}

//------------------------------------------------------------------------------
// GmatBase *MeasurementModel::Clone() const
//------------------------------------------------------------------------------
/**
 * This method calls the copy constructor to make a duplicate of this object
 *
 * @return The duplicate
 */
//------------------------------------------------------------------------------
GmatBase *MeasurementModel::Clone() const
{
   #ifdef DEBUG_MEASUREMENT_INITIALIZATION
      MessageInterface::ShowMessage("Entered MeasurementModel::Clone()\n");
   #endif

   return new MeasurementModel(*this);
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Initializes the measurement model and its owned objects
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool MeasurementModel::Initialize()
{
   #ifdef DEBUG_MEASUREMENT_INITIALIZATION
      MessageInterface::ShowMessage("Entered MeasurementModel::Initialize()\n");
   #endif

   bool retval = false;

   if (GmatBase::Initialize())
   {
      if (measurement != NULL)
      {
         // Pass in the participants and hardware lists
         for (UnsignedInt i = 0; i < participants.size(); ++i)
         {
            #ifdef DEBUG_MEASUREMENT_INITIALIZATION
               MessageInterface::ShowMessage(
                     "   Setting participant %d to %s\n", i,
                     participants[i]->GetName().c_str());
            #endif
            if (measurementNeedsObjects)
               measurement->SetRefObject(participants[i],
                     participants[i]->GetType(), participants[i]->GetName());

            #ifdef DEBUG_MEASUREMENT_INITIALIZATION
               MessageInterface::ShowMessage("   Setting hardware for %s\n",
                     participants[i]->GetName().c_str());
               MessageInterface::ShowMessage("      %d hardware name arrays\n",
                     participantHardwareNames.size());
               MessageInterface::ShowMessage(
                     "      %d hardware array has %d elements\n", i,
                     participantHardwareNames[i].size());
            #endif

            for (UnsignedInt j = 0; j < participantHardwareNames[i].size(); ++j)
            {
               if (measurement->SetParticipantHardware(participants[i],
                     participantHardwareNames[i][j], j) == false)
                  throw MeasurementException("The measurement participant " +
                        participants[i]->GetName() +
                        " does not have a hardware component named " +
                        participantHardwareNames[i][j]);
            }
         }

         // Validate CoreMeasurement member
         if (measurement->Initialize())
         {
            #ifdef DEBUG_MEASUREMENT_INITIALIZATION
               MessageInterface::ShowMessage("   Core measurement initialized\n");
            #endif

            // Set calculated data pointers
            theData              = measurement->GetMeasurementDataPointer();
            theDataDerivatives   = measurement->GetDerivativePointer();

            // Prepare the measurement noise covariance
            Integer measurementSize = measurement->GetMeasurementSize();
            measErrorCovariance.SetDimension(measurementSize);
            for (Integer i = 0; i < measurementSize; ++i)
            {
               for (Integer j = 0; j < measurementSize; ++j)
               {
                  measErrorCovariance(i,j) = (i == j ?
                           (noiseSigma[i] != 0.0 ?
                                 (noiseSigma[i] * noiseSigma[i]) : 1.0) :
                           0.0);
               }
            }
            theData->covariance = &measErrorCovariance;

            retval = true;

            #ifdef DEBUG_MEASUREMENT_INITIALIZATION
               MessageInterface::ShowMessage(
                     "Initialization complete for measurement model %s\n",
                     instanceName.c_str());
            #endif

            #ifdef TEST_FIRE_MEASUREMENT
               MessageInterface::ShowMessage("Test firing measurement model %s\n",
                     instanceName.c_str());

               CalculateMeasurement();

               MessageInterface::ShowMessage("   Calculated %s at epoch %.12lf\n",
                     measurement->GetTypeName().c_str(), theData->epoch);
               MessageInterface::ShowMessage("   FeasibilityValue = %lf\n",
                     theData->feasibilityValue);
               MessageInterface::ShowMessage("   Feasibility:  %s\n",
                     (theData->isFeasible ? "true" : "false"));
               MessageInterface::ShowMessage("   Measurement = [");
               for (RealArray::iterator i = theData->value.begin();
                     i != theData->value.end(); ++i)
                  MessageInterface::ShowMessage(" %.12lf ", (*i));
               MessageInterface::ShowMessage("]\n");
            #endif

         }
      }
   }

   return retval;
}


//------------------------------------------------------------------------------
// Parameter handling code
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//  Integer  GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter ID, given the input parameter string.
 *
 * @param str string for the requested parameter.
 *
 * @return ID for the requested parameter.
 */
//------------------------------------------------------------------------------
Integer MeasurementModel::GetParameterID(const std::string & str) const
{
   for (Integer i = GmatBaseParamCount; i < MeasurementModelParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - GmatBaseParamCount])
         return i;
   }

   // Check the GmatBase parameters
   for (Integer i = 0; i < GmatBaseParamCount; i++)
   {
      if (str == GmatBase::PARAMETER_LABEL[i])
         return i;
   }


   // Handle parameters from the CoreMeasurement
   if (measurement != NULL)
   {
      return measurement->GetParameterID(str) + MeasurementModelParamCount;
   }

   return GmatBase::GetParameterID(str);
}


//------------------------------------------------------------------------------
//  std::string  GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter text, given the input parameter ID.
 *
 * @param id Id for the requested parameter text.
 *
 * @return parameter text for the requested parameter.
 */
//------------------------------------------------------------------------------
std::string MeasurementModel::GetParameterText(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < MeasurementModelParamCount)
      return PARAMETER_TEXT[id - GmatBaseParamCount];

   // Handle parameters from the CoreMeasurement
   if (id >= MeasurementModelParamCount)
   {
      Integer newId = id - MeasurementModelParamCount;
      return measurement->GetParameterText(newId);
   }

   return GmatBase::GetParameterText(id);
}


//------------------------------------------------------------------------------
//  std::string  GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type string, given the input parameter ID.
 *
 * @param id ID for the requested parameter.
 *
 * @return parameter type string of the requested parameter.
 */
//------------------------------------------------------------------------------
std::string MeasurementModel::GetParameterTypeString(const Integer id) const
{
   // Handle parameters from the CoreMeasurement
   if (id >= MeasurementModelParamCount)
   {
      Integer newId = id - MeasurementModelParamCount;
      return measurement->GetParameterTypeString(newId);
   }


   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
}


//---------------------------------------------------------------------------
//  std::string GetParameterUnit(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieve the unit for the parameter.
 *
 * @param id The integer ID for the parameter.
 *
 * @return unit for the requested parameter.
 */
//------------------------------------------------------------------------------
std::string MeasurementModel::GetParameterUnit(const Integer id) const
{
   // Handle parameters from the CoreMeasurement
   if (id >= MeasurementModelParamCount)
   {
      Integer newId = id - MeasurementModelParamCount;
      return measurement->GetParameterUnit(newId);
   }

   return GmatBase::GetParameterUnit(id);
}


//---------------------------------------------------------------------------
//  Gmat::ParameterType GetParameterType(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieve the enumerated type of the object.
 *
 * @param id The integer ID for the parameter.
 *
 * @return The enumeration for the type of the parameter, or
 *         UNKNOWN_PARAMETER_TYPE.
 */
//------------------------------------------------------------------------------
Gmat::ParameterType MeasurementModel::GetParameterType(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < MeasurementModelParamCount)
      return PARAMETER_TYPE[id - GmatBaseParamCount];

   // Handle parameters from the CoreMeasurement
   if (id >= MeasurementModelParamCount)
   {
      Integer newId = id - MeasurementModelParamCount;
      return measurement->GetParameterType(newId);
   }

   return GmatBase::GetParameterType(id);
}


//------------------------------------------------------------------------------
// Integer GetParameterCount() const
//------------------------------------------------------------------------------
/**
 * Override the default method and retrieve the total number of parameters that
 * are scriptable for the MeasurementModel plus the CoreMeasurement object.
 *
 * @return The total number of scriptable parameters from the composite object
 *
 * @note Anyone that derives a class off of MeasurementModel will need to
 *       rework this method to accommodate any new parameters added to the
 *       derived class
 */
//------------------------------------------------------------------------------
Integer MeasurementModel::GetParameterCount() const
{
   if (measurement != NULL)
      return parameterCount + measurement->GetParameterCount();

   return parameterCount;
}


//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param id Description for the parameter.
 *
 * @return true if the parameter is read only, false (the default) if not,
 *         throws if the parameter is out of the valid range of values.
 */
//---------------------------------------------------------------------------
bool MeasurementModel::IsParameterReadOnly(const Integer id) const
{
   if (id == Frequency)
      return true;
   if (id == Bias)
      if (!measurementBias.IsSized())
         return true;
   if (id == NoiseSigma)
      if (!noiseSigma.IsSized())
         return true;

   // Handle parameters from the CoreMeasurement
   if (id >= MeasurementModelParamCount)
   {
      Integer newId = id - MeasurementModelParamCount;
      return measurement->IsParameterReadOnly(newId);
   }

   return GmatBase::IsParameterReadOnly(id);
}


//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param label Description for the parameter.
 *
 * @return true if the parameter is read only, false (the default) if not.
 */
//---------------------------------------------------------------------------
bool MeasurementModel::IsParameterReadOnly(const std::string & label) const
{
   return IsParameterReadOnly(GetParameterID(label));
}

//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the parameters used in the noise modeling for the Measurement
 *
 * @param id The ID for the parameter that is retrieved
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
Real MeasurementModel::GetRealParameter(const Integer id) const
{
   if (id == TimeConstant)
      return timeConstant;
      
   if (id == Frequency)
      if (measurement != NULL)
         if (measurement->IsOfType("PhysicalMeasurement"))
            return ((PhysicalMeasurement*)measurement)->GetConstantFrequency();

   // Handle parameters from the CoreMeasurement
   if (id >= MeasurementModelParamCount)
   {
      Integer newId = id - MeasurementModelParamCount;
      return measurement->GetRealParameter(newId);
   }

   return GmatBase::GetRealParameter(id);
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Sets the parameters used in the noise modeling for the Measurement
 *
 * @param id The ID for the parameter that is to be set
 * @param value The new value for the parameter
 *
 * @return The parameter value.  The return value is the new value if it was
 *         changed, or the value prior to the call if the new value wwas not
 *         accepted.
 */
//------------------------------------------------------------------------------
Real MeasurementModel::SetRealParameter(const Integer id, const Real value)
{
//   if (id == Bias)
//   {
//      if (value >= 0)
//         measurementBias = value;
//      return measurementBias;
//   }
//   if (id == NoiseSigma)
//   {
//      if (value >= 0)
//         noiseSigma = value;
//      return noiseSigma;
//   }
   if (id == TimeConstant)
   {
      if (value >= 0)
         timeConstant = value;
      return timeConstant;
   }

   // Handle parameters from the CoreMeasurement
   if (id >= MeasurementModelParamCount)
   {
      Integer newId = id - MeasurementModelParamCount;
      return measurement->SetRealParameter(newId, value);
   }

   if (id == Frequency)
   {
      if (measurement != NULL)
      {
         if (measurement->IsOfType("PhysicalMeasurement"))
         {
            if (value > 0.0)
            {
               ((PhysicalMeasurement*)measurement)->SetConstantFrequency(value);
            }
            return ((PhysicalMeasurement*)measurement)->GetConstantFrequency();
         }
      }
   }

   return GmatBase::SetRealParameter(id, value);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string & label) const
//------------------------------------------------------------------------------
/**
 * Retrieves the parameters used in the noise modeling for the Measurement
 *
 * @param label The text label for the parameter that is retrieved
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
Real MeasurementModel::GetRealParameter(const std::string & label) const
{
   return GetRealParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const std::string & label, const Real value)
//------------------------------------------------------------------------------
/**
 * Sets the parameters used in the noise modeling for the Measurement
 *
 * @param label The text label for the parameter that is to be set
 * @param value The new value for the parameter
 *
 * @return The parameter value.  The return value is the new value if it was
 *         changed, or the value prior to the call if the new value wwas not
 *         accepted.
 */
//------------------------------------------------------------------------------
Real MeasurementModel::SetRealParameter(const std::string & label,
      const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// Real MeasurementModel::GetRealParameter(const std::string & label,
//       const Integer index) const
//------------------------------------------------------------------------------
/**
 * This method calls the base class method.  It is provided for overload
 * compatibility.  See the base class description for a full description.
 */
//------------------------------------------------------------------------------
Real MeasurementModel::GetRealParameter(const std::string & label,
      const Integer index) const
{
   return GetRealParameter(GetParameterID(label), index);
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const std::string & label, const Real value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * This method sets a Real parameter in an array.
 *
 * @param label The text label for the parameter that is to be set
 * @param value The new value for the parameter
 * @param index The index of the parameter
 *
 * @return The parameter value.
 */
//------------------------------------------------------------------------------
Real MeasurementModel::SetRealParameter(const std::string & label,
      const Real value, const Integer index)
{
   return SetRealParameter(GetParameterID(label), value, index);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string & label, const Integer row,
//       const Integer col) const
//------------------------------------------------------------------------------
/**
 * This method retrieves a Real parameter from an matrix.
 *
 * @param label The text label for the parameter that is to be set
 * @param row The matrix row
 * @param col The column in the matrix
 *
 * @return The parameter value.
 */
//------------------------------------------------------------------------------
Real MeasurementModel::GetRealParameter(const std::string & label,
      const Integer row, const Integer col) const
{
   return GetRealParameter(GetParameterID(label), row, col);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id, const Integer row,
//       const Integer col) const
//------------------------------------------------------------------------------
/**
 * This method retrieves a Real parameter from an matrix.
 *
 * @param id The parameter ID
 * @param row The matrix row
 * @param col The column in the matrix
 *
 * @return The parameter value.
 */
//------------------------------------------------------------------------------
Real MeasurementModel::GetRealParameter(const Integer id, const Integer row,
      const Integer col) const
{
   return GmatBase::GetRealParameter(id, row, col);
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets parameter data in a vector of data
 *
 * @param id The ID for the parameter that is to be set
 * @param value The new value for the parameter
 * @param index The index into the vector
 *
 * @return The parameter value.
 */
//------------------------------------------------------------------------------
Real MeasurementModel::SetRealParameter(const Integer id, const Real value,
      const Integer index)
{
   if (id == Bias)
   {
      #ifdef DEBUG_PARAMETER_HANDLING
         MessageInterface::ShowMessage("Setting Bias[%d] to %lf\n", index,
               value);
      #endif
      if ((index >=0) && (index < measurementBias.GetSize()))
      {
         measurementBias[index] = value;
         return measurementBias[index];
      }
      else if (index == -1)
      {
         measurementBias[0] = value;
         return measurementBias[0];
      }
   }

   if (id == NoiseSigma)
   {
      #ifdef DEBUG_PARAMETER_HANDLING
         MessageInterface::ShowMessage("Setting NoiseSigma[%d] to %lf\n", index,
               value);
      #endif
      if ((index >=0) && (index < noiseSigma.GetSize()))
      {
         if (value >= 0)
            noiseSigma[index] = value;
         return noiseSigma[index];
      }
      else if (index == -1)
      {
         if (value >= 0)
            noiseSigma[0] = value;
         return noiseSigma[0];
      }
   }

   return GmatBase::SetRealParameter(id, value, index);
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value,
//       const Integer row, const Integer col)
//------------------------------------------------------------------------------
/**
 * Sets parameter data in a matrix of data
 *
 * @param id The ID for the parameter that is to be set
 * @param value The new value for the parameter
 * @param row The matrix row
 * @param col The column in the matrix
 *
 * @return The parameter value.
 */
//------------------------------------------------------------------------------
Real MeasurementModel::SetRealParameter(const Integer id, const Real value,
      const Integer row, const Integer col)
{
   return GmatBase::SetRealParameter(id, value, row, col);
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const std::string & label,
//       const Real value, const Integer row, const Integer col)
//------------------------------------------------------------------------------
/**
 * Sets parameter data in a matrix of data
 *
 * @param label The text descriptor for the parameter that is to be set
 * @param value The new value for the parameter
 * @param row The matrix row
 * @param col The column in the matrix
 *
 * @return The parameter value.
 */
//------------------------------------------------------------------------------
Real MeasurementModel::SetRealParameter(const std::string & label,
      const Real value, const Integer row, const Integer col)
{
   return SetRealParameter(GetParameterID(label), value, row, col);
}

//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * This method calls the base class method.  It is provided for overload
 * compatibility.  See the base class description for a full description.
 */
//------------------------------------------------------------------------------
Real MeasurementModel::GetRealParameter(const Integer id, const Integer index) const
{
   // todo Use [index] rather than [0] here
   if (id == Bias)
      if ((index >=0) && (index < measurementBias.GetSize()))
         return measurementBias[0];

   if (id == NoiseSigma)
      if ((index >=0) && (index < noiseSigma.GetSize()))
         return noiseSigma[0];

   return GmatBase::GetRealParameter(id, index);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method retrieves a string parameter
 *
 * @param id The ID for the parameter
 *
 * @return The string parameter
 */
//------------------------------------------------------------------------------
std::string MeasurementModel::GetStringParameter(const Integer id) const
{
   if (id == MeasurementType)
      return measurementType;

   return GmatBase::GetStringParameter(id);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string & value)
//------------------------------------------------------------------------------
/**
 * This method sets a string parameter
 *
 * @param id The ID for the parameter
 * @param value The new string value
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool MeasurementModel::SetStringParameter(const Integer id,
      const std::string & value)
{
   if (id == ObservationData)
   {
      // Only add the obs data if it is not in the list already
      if (find(observationStreamName.begin(), observationStreamName.end(),
            value) == observationStreamName.end())
      {
         observationStreamName.push_back(value);
         return true;
      }
   }
   if (id == MeasurementType)
   {
      measurementType = value;
      return true;
   }

   if (id == Participants)
   {
      UnsignedInt loc = value.find(".");
      std::string parName;
      if (loc != std::string::npos)
         parName = value.substr(0, loc);
      else
         parName = value;

      // Only add the participant if it is not in the list already
      if (find(participantNames.begin(), participantNames.end(), parName) ==
            participantNames.end())
      {
         participantNames.push_back(parName);

         // Prep the hardware list for associated named hardware
         StringArray pHardware;
         participantHardwareNames.push_back(pHardware);
      }
      // Now add hardware to the list if it was found
      if (loc != std::string::npos)
      {
         std::string hwName = value.substr(loc+1);
         #ifdef DEBUG_HARDWARE
            MessageInterface::ShowMessage("Hardware %s is being stored\n",
                  hwName.c_str());
         #endif

         // Find the participant's index
         for (UnsignedInt i = 0; i < participantNames.size(); ++i)
         {
            if (participantNames[i] == parName)
            {
               #ifdef DEBUG_HARDWARE
                  MessageInterface::ShowMessage("   Storing on %s @ index %d\n",
                        parName.c_str(), i);
                  MessageInterface::ShowMessage("   pha size is %d\n",
                        participantHardwareNames.size());
               #endif
               if (find(participantHardwareNames[i].begin(),
                     participantHardwareNames[i].end(), hwName) ==
                        participantHardwareNames[i].end())
                  participantHardwareNames[i].push_back(hwName);
            }
         }
      }
      return true;
   }

   return GmatBase::SetStringParameter(id, value);
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string & label) const
//------------------------------------------------------------------------------
/**
 * This method retrieves a string parameter
 *
 * @param label The string label for the parameter
 *
 * @return The parameter
 */
//------------------------------------------------------------------------------
std::string MeasurementModel::GetStringParameter(const std::string & label) const
{
   return GetStringParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string & label, const std::string & value)
//------------------------------------------------------------------------------
/**
 * This method sets a string parameter
 *
 * @param label The string label for the parameter
 * @param value The new string value
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool MeasurementModel::SetStringParameter(const std::string & label,
      const std::string & value)
{
   return SetStringParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id,
//       const Integer index) const
//------------------------------------------------------------------------------
/**
 * This method retrieves a string parameter from a StringArray
 *
 * @param id The ID of the parameter
 *
 * @return The parameter
 */
//------------------------------------------------------------------------------
std::string MeasurementModel::GetStringParameter(const Integer id,
      const Integer index) const
{
   if (id == Participants)
      return participantNames[index];
   if (id == ObservationData)
      return observationStreamName[index];

   return GmatBase::GetStringParameter(id, index);
}

//------------------------------------------------------------------------------
// std::string MeasurementModel::GetStringParameter(const std::string & label,
//       const Integer index) const
//------------------------------------------------------------------------------
/**
 * This method retrieves a string parameter from a StringArray
 *
 * @param label The string label for the parameter
 *
 * @return The parameter
 */
//------------------------------------------------------------------------------
std::string MeasurementModel::GetStringParameter(const std::string & label,
      const Integer index) const
{
   return GetStringParameter(GetParameterID(label), index);
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string & label,
//       const std::string & value, const Integer index)
//------------------------------------------------------------------------------
/**
 * This method calls the base class method.  It is provided for overload
 * compatibility.  See the base class description for a full description.
 */
//------------------------------------------------------------------------------
bool MeasurementModel::SetStringParameter(const std::string & label,
      const std::string & value, const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id,
//       const std::string & value, const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets a specific string in a StringArray
 *
 * This method changes a specific string in a StringArray if a string has been
 * set at the location selected by the index value.  If the index exceeds the
 * size of the name array, the participant name is added to the end of the list.
 *
 * @param id The ID for the StringArray parameter that is being changed
 * @param value The string that needs to be placed in the StringArray
 * @param index The location for the string in the list.  If index exceeds the
 *              size of the StringArray, the string is added to the end of the
 *              array
 *
 * @return true If the string was processed
 */
//------------------------------------------------------------------------------
bool MeasurementModel::SetStringParameter(const Integer id,
      const std::string & value, const Integer index)
{
   if (index < 0)
   {
      MeasurementException ex;
      ex.SetDetails("The index %d is out-of-range for field \"%s\"", index,
                    GetParameterText(id).c_str());
      throw ex;
   }

   switch (id)
   {
   case Participants:
      {
         UnsignedInt loc = value.find(".");
         std::string parName;
         std::string hwName;
         if (loc != std::string::npos)
         {
            parName = value.substr(0, loc);
            hwName  = value.substr(loc+1);
         }
         else
            parName = value;

         if (index < (Integer)participantNames.size())
            participantNames[index] = parName;
         else
            if (find(participantNames.begin(),participantNames.end(),parName) ==
                  participantNames.end())
            {
               participantNames.push_back(parName);
               StringArray pHardware;
               participantHardwareNames.push_back(pHardware);
            }

         if (loc != std::string::npos)
         {
            for (UnsignedInt j = 0; j < participants.size(); ++j)
               if (participants[j]->GetName() == parName)
                  participantHardwareNames[j].push_back(hwName);
         }
         return true;
      }

   case ObservationData:
      {
         if (index < (Integer)observationStreamName.size())
            observationStreamName[index] = value;
         else
            if (find(observationStreamName.begin(), observationStreamName.end(),
                  value) == observationStreamName.end())
               observationStreamName.push_back(value);

         return true;
      }
   default:
      return GmatBase::SetStringParameter(id, value, index);
   }
}

//------------------------------------------------------------------------------
//  const StringArray& GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Access an array of string data.
 *
 * @param id The ID for the parameter.
 *
 * @return The requested StringArray; throws if the parameter is not a
 *         StringArray.
 */
//------------------------------------------------------------------------------
const StringArray& MeasurementModel::GetStringArrayParameter(
      const Integer id) const
{
   if (id == Participants)
   {
      #ifdef DEBUG_MEASUREMENT_INITIALIZATION
         MessageInterface::ShowMessage("Reporting %d participant names\n",
               participantNames.size());
      #endif
      return participantNames;
   }
   if (id == ObservationData)
   {
      #ifdef DEBUG_MEASUREMENT_INITIALIZATION
         MessageInterface::ShowMessage("Reporting %d observation streams\n",
               observationStreamName.size());
      #endif
      return observationStreamName;
   }

   return GmatBase::GetStringArrayParameter(id);
}

//------------------------------------------------------------------------------
//  const StringArray& GetStringArrayParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Access an array of string data.
 *
 * @param label The (string) label for the parameter.
 *
 * @return The requested StringArray; throws if the parameter is not a
 *         StringArray.
 */
//------------------------------------------------------------------------------
const StringArray& MeasurementModel::GetStringArrayParameter(
      const std::string &label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id,
//       const Integer index) const;
//------------------------------------------------------------------------------
/**
 * This method calls the base class method.  It is provided for overload
 * compatibility.  See the base class description for a full description.
 */
//------------------------------------------------------------------------------
const StringArray& MeasurementModel::GetStringArrayParameter(const Integer id,
      const Integer index) const
{
   return GmatBase::GetStringArrayParameter(id, index);
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const std::string &label,
//       const Integer index) const
//------------------------------------------------------------------------------
/**
 * This method calls the base class method.  It is provided for overload
 * compatibility.  See the base class description for a full description.
 */
//------------------------------------------------------------------------------
const StringArray& MeasurementModel::GetStringArrayParameter(
      const std::string &label, const Integer index) const
{
   return GmatBase::GetStringArrayParameter(label, index);
}


//------------------------------------------------------------------------------
// const Rvector& GetRvectorParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves an Rvector parameter
 *
 * @param id The ID for the parameter
 *
 * @return The Rvector
 */
//------------------------------------------------------------------------------
const Rvector& MeasurementModel::GetRvectorParameter(const Integer id) const
{
   if (id == Bias)
      return measurementBias;

   if (id == NoiseSigma)
      return noiseSigma;

   return GmatBase::GetRvectorParameter(id);
}


//------------------------------------------------------------------------------
// const Rvector& SetRvectorParameter(const Integer id, const Rvector &value)
//------------------------------------------------------------------------------
/**
 * Sets data in an Rvector
 *
 * @param id The parameter ID for the Rvector
 * @param value The Rvector that sills in the data for the paramater
 *
 * @return The filled in Rvector
 */
//------------------------------------------------------------------------------
const Rvector& MeasurementModel::SetRvectorParameter(const Integer id,
                                         const Rvector &value)
{
   if (id == Bias)
   {
      measurementBias = value;
      return measurementBias;
   }

   if (id == NoiseSigma)
   {
      noiseSigma = value;
      return noiseSigma;
   }

   return GmatBase::SetRvectorParameter(id, value);
}


//------------------------------------------------------------------------------
// const Rvector& GetRvectorParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieves an Rvector parameter
 *
 * @param label The text descriptor for the Rvector parameter
 *
 * @return The Rvector
 */
//------------------------------------------------------------------------------
const Rvector& MeasurementModel::GetRvectorParameter(
      const std::string &label) const
{
   return GetRvectorParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// const Rvector& SetRvectorParameter(const std::string &label,
//       const Rvector &value)
//------------------------------------------------------------------------------
/**
 * Sets data in an Rvector
 *
 * @param label The text descriptor for the Rvector parameter
 * @param value The Rvector that sills in the data for the paramater
 *
 * @return The filled in Rvector
 */
//------------------------------------------------------------------------------
const Rvector& MeasurementModel::SetRvectorParameter(const std::string &label,
      const Rvector &value)
{
   return SetRvectorParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// bool RenameRefObject(const Gmat::ObjectType type,
//------------------------------------------------------------------------------
/**
 * Renames references objects
 *
 * @param type The type of object that is renamed
 * @param oldName The name of the object that is changing
 * @param newName the new object name
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool MeasurementModel::RenameRefObject(const Gmat::ObjectType type,
      const std::string & oldName, const std::string & newName)
{
   /// @todo MeasurementModel rename code needs to be implemented
   return GmatBase::RenameRefObject(type, oldName, newName);
}


//------------------------------------------------------------------------------
// bool SetRefObjectName(const Gmat::ObjectType type, const std::string & name)
//------------------------------------------------------------------------------
/**
 * identifies reference objects needed by name
 *
 * @param type The type of the reference object
 * @param name The reference object's name
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool MeasurementModel::SetRefObjectName(const Gmat::ObjectType type,
      const std::string & name)
{
   return GmatBase::SetRefObjectName(type, name);
}


//------------------------------------------------------------------------------
// const ObjectTypeArray & GetRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * Retrieves an array identifying the types of the needed reference objects
 *
 * @return The array
 */
//------------------------------------------------------------------------------
const ObjectTypeArray & MeasurementModel::GetRefObjectTypeArray()
{
   return GmatBase::GetRefObjectTypeArray();
}


//------------------------------------------------------------------------------
// const StringArray& GetRefObjectNameArray(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * Initialization method that identifies the reference objects needed
 *
 * @param type The ObjectType for the references; UNKNOWN_OBJECT retrieves all
 *
 * @return A StringArray with all of the object names.
 */
//------------------------------------------------------------------------------
const StringArray& MeasurementModel::GetRefObjectNameArray(
      const Gmat::ObjectType type)
{
   #ifdef DEBUG_MEASUREMENT_INITIALIZATION
      MessageInterface::ShowMessage(
            "MeasurementModel::GetRefObjectNameArray(%d) entered\n", type);
   #endif

   refObjectList.clear();

   if ((type == Gmat::UNKNOWN_OBJECT) || (type == Gmat::SPACE_POINT))
   {
      // Add the participants this model needs
      for (StringArray::iterator i = participantNames.begin();
            i != participantNames.end(); ++i)
      {
         #ifdef DEBUG_MEASUREMENT_INITIALIZATION
            MessageInterface::ShowMessage(
                  "   Adding: %s\n", i->c_str());
         #endif
         if (find(refObjectList.begin(), refObjectList.end(), *i) ==
               refObjectList.end())
            refObjectList.push_back(*i);
      }
   }
   else
      refObjectList = GmatBase::GetRefObjectNameArray(type);

   return refObjectList;
}


//------------------------------------------------------------------------------
// std::string GetRefObjectName(const Gmat::ObjectType type) const
//------------------------------------------------------------------------------
/**
 * Retrieves the name of a referenced object
 *
 * @param type The type of the object
 *
 * @return The corresponding name
 */
//------------------------------------------------------------------------------
std::string MeasurementModel::GetRefObjectName(
      const Gmat::ObjectType type) const
{
   return GmatBase::GetRefObjectName(type);
}


//------------------------------------------------------------------------------
// GmatBase* GetRefObject(const Gmat::ObjectType type, const std::string & name)
//------------------------------------------------------------------------------
/**
 * Retrieves a reference object
 *
 * @param type The type of the object
 * @param name The object's name
 *
 * @return A pointer to the object, or NULL if the object is not set
 */
//------------------------------------------------------------------------------
GmatBase* MeasurementModel::GetRefObject(const Gmat::ObjectType type,
      const std::string & name)
{
   if (type == Gmat::CORE_MEASUREMENT)
      return measurement;
   return GmatBase::GetRefObject(type, name);
}


//------------------------------------------------------------------------------
// GmatBase* GetRefObject(const Gmat::ObjectType type, const std::string & name,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Retrieves a reference object from an array of objects
 *
 * @param type The type of the object
 * @param name The object's name
 * @param index The index into the array
 *
 * @return A pointer to the object, or NULL if the object is not set
 */
//------------------------------------------------------------------------------
GmatBase* MeasurementModel::GetRefObject(const Gmat::ObjectType type,
      const std::string & name, const Integer index)
{
   return GmatBase::GetRefObject(type, name, index);
}


//------------------------------------------------------------------------------
// GmatBase* GetOwnedObject(Integer whichOne)
//------------------------------------------------------------------------------
/**
 * Retrieves an owned object
 *
 * @param whichOne The index to the owned object
 *
 * @return A pointer to the object
 */
//------------------------------------------------------------------------------
GmatBase* MeasurementModel::GetOwnedObject(Integer whichOne)
{
   return GmatBase::GetOwnedObject(whichOne);
}


//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//       const std::string & name)
//------------------------------------------------------------------------------
/**
 * Sets a reference object
 *
 * @param obj The reference object
 * @param type The type of the object
 * @param name The object's name
 *
 * @return true if the object was set, false if not
 */
//------------------------------------------------------------------------------
bool MeasurementModel::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
      const std::string & name)
{
   #ifdef DEBUG_MEASUREMENT_INITIALIZATION
      MessageInterface::ShowMessage("Setting ref object %s with type %s\n",
            name.c_str(), obj->GetTypeName().c_str());
   #endif

   if (obj->IsOfType(Gmat::CORE_MEASUREMENT))
   {
      CoreMeasurement *meas = (CoreMeasurement*)obj;
      measurement = meas;
      measurementType = measurement->GetTypeName();

      theData = measurement->GetMeasurementDataPointer();
      theDataDerivatives = measurement->GetDerivativePointer();
      covariance = *(measurement->GetCovariance());
      measurementBias.SetSize(measurement->GetMeasurementSize());
      for (Integer i = 0; i < measurement->GetMeasurementSize(); ++i)
         measurementBias[i] = 0.0;
      noiseSigma.SetSize(measurement->GetMeasurementSize());
      for (Integer i = 0; i < measurement->GetMeasurementSize(); ++i)
         noiseSigma[i] = 1.0;

      return true;
   }
   else if (find(participantNames.begin(), participantNames.end(), name) !=
         participantNames.end())
   {
      if (find(participants.begin(), participants.end(), obj) ==
            participants.end())
      {
         participants.push_back(obj);
         if (measurement != NULL)
         {
            bool retval = measurement->SetRefObject(obj, type, name);
            // Pass in named hardware elements

            return retval;
         }
         else
            measurementNeedsObjects = true;
      }
   }

   return GmatBase::SetRefObject(obj, type, name);
}


//------------------------------------------------------------------------------
// Integer GetOwnedObjectCount()
//------------------------------------------------------------------------------
/**
 * Retrieves the number of owned objects in this object
 *
 * @return The owned object count
 */
//------------------------------------------------------------------------------
Integer MeasurementModel::GetOwnedObjectCount()
{
   ownedObjectCount = 0;

   return GmatBase::GetOwnedObjectCount();
}


//------------------------------------------------------------------------------
// ObjectArray& GetRefObjectArray(const std::string & typeString)
//------------------------------------------------------------------------------
/**
 * Retrieves an array of reference objects of a specified type
 *
 * @param typeString The type of the objects
 *
 * @return The object array
 */
//------------------------------------------------------------------------------
ObjectArray& MeasurementModel::GetRefObjectArray(const std::string & typeString)
{
   return GmatBase::GetRefObjectArray(typeString);
}


//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//       const std::string & name, const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets referenced objects in an array
 *
 * @param obj The referenced object
 * @param type The object's type
 * @param name The object's name
 * @param index The index into the array
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool MeasurementModel::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
      const std::string & name, const Integer index)
{
   #ifdef DEBUG_MEASUREMENT_INITIALIZATION
      MessageInterface::ShowMessage(""
            "Setting indexed ref object %s with type %s\n", name.c_str(),
            obj->GetTypeName().c_str());
   #endif

   if (obj->IsOfType(Gmat::CORE_MEASUREMENT))
   {
      CoreMeasurement *meas = (CoreMeasurement*)obj;
      measurement = meas;
      measurementType = measurement->GetTypeName();

      theData = measurement->GetMeasurementDataPointer();
      theDataDerivatives = measurement->GetDerivativePointer();
      covariance = *(measurement->GetCovariance());
      measurementBias.SetSize(measurement->GetMeasurementSize());
      for (Integer i = 0; i < measurement->GetMeasurementSize(); ++i)
         measurementBias[i] = 0.0;
      noiseSigma.SetSize(measurement->GetMeasurementSize());
      for (Integer i = 0; i < measurement->GetMeasurementSize(); ++i)
         noiseSigma[i] = 1.0;

      return true;
   }

   return GmatBase::SetRefObject(obj, type, name, index);
}


//------------------------------------------------------------------------------
// ObjectArray& GetRefObjectArray(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * Retrieves an array of reference objects
 *
 * @param type The Gmat::OBJECT_TYPE requested
 *
 * @return The array of objects
 */
//------------------------------------------------------------------------------
ObjectArray& MeasurementModel::GetRefObjectArray(const Gmat::ObjectType type)
{
   return GmatBase::GetRefObjectArray(type);
}


//------------------------------------------------------------------------------
// bool IsOwnedObject(Integer id) const
//------------------------------------------------------------------------------
/**
 * Finds out if object property of given id is owned object
 * This method was added so that Interpreter can query and create
 * proper owned objects.
 *
 * Any subclass should override this method as necessary.
 *
 * @param  id   The object property id to check for owned object
 *
 * @return  true if it is owned object
 */
//------------------------------------------------------------------------------
bool MeasurementModel::IsOwnedObject(Integer id) const
{
   return GmatBase::IsOwnedObject(id);
}


//------------------------------------------------------------------------------
// Integer GetModelID()
//------------------------------------------------------------------------------
/**
 * Retrieves the ID for the measuremetn model
 *
 * @return The ID
 */
//------------------------------------------------------------------------------
Integer MeasurementModel::GetModelID()
{
   return modelID;
}


//------------------------------------------------------------------------------
// Integer GetModelTypeID()
//------------------------------------------------------------------------------
/**
 * Accesses the CoreMeasurement's type for validation purposes
 *
 * @return the CoreMeasurement type from the enumerated list in EstimationDefs
 */
//------------------------------------------------------------------------------
Integer MeasurementModel::GetModelTypeID()
{
   if (measurement != NULL)
      return measurement->GetMeasurementTypeID();
   else
      return -1;
}

//------------------------------------------------------------------------------
// void SetModelID(Integer newID)
//------------------------------------------------------------------------------
/**
 * Sets the measurement model ID
 *
 * @param newID The ID for the model
 */
//------------------------------------------------------------------------------
void MeasurementModel::SetModelID(Integer newID)
{
   modelID = newID;
   measurement->SetUniqueId(newID);
}


//------------------------------------------------------------------------------
// const MeasurementData& CalculateMeasurement(bool withEvents)
//------------------------------------------------------------------------------
/**
 * Calculates the measurement
 *
 * Calls the core measurement and retrieves the measurement data for the current
 * state of the participants.  If the measurement is not possible given that
 * state, the MeasurementData container is cleared and its isFeasible flag is
 * set to false.
 *
 * @param withEvents Flag indicating is events - if present - should be included
 *                   in the calculations
 *
 * @return A reference to the calculated MeasurementData
 */
//------------------------------------------------------------------------------
const MeasurementData& MeasurementModel::CalculateMeasurement(bool withEvents)
{
   measurement->CalculateMeasurement(withEvents);

   // Add in the Biases if the measurement was feasible
   if (theData->isFeasible)
   {
      #ifdef DEBUG_BIAS
         MessageInterface::ShowMessage("Before bias: [");
         for (UnsignedInt i = 0; i < theData->value.size(); ++i)
            MessageInterface::ShowMessage(" %.12lf ", theData->value[i]);
         MessageInterface::ShowMessage("]\n");
      #endif

      for (Integer i = 0; i < measurementBias.GetSize(); ++i)
         theData->value[i] += measurementBias[i];

      #ifdef DEBUG_BIAS
         MessageInterface::ShowMessage("      bias:  [");
         for (Integer i = 0; i < measurementBias.GetSize(); ++i)
            MessageInterface::ShowMessage(" %.12lf ", measurementBias[i]);
         MessageInterface::ShowMessage("]\n");


         MessageInterface::ShowMessage("After bias:  [");
         for (UnsignedInt i = 0; i < theData->value.size(); ++i)
            MessageInterface::ShowMessage(" %.12lf ", theData->value[i]);
         MessageInterface::ShowMessage("]\n");
      #endif
   }

   return *theData;
}


//------------------------------------------------------------------------------
// bool SetMeasurement(CoreMeasurement *meas)
//------------------------------------------------------------------------------
/**
 * Sets the core measurement for the measurement model
 *
 * The core measurement is identified by the "Type" parameter on the measurement
 * model.  The interpreter uses this identifier to pass a CoreMeasurement
 * instance that the model uses when computing the (expected) measurement value.
 *
 * @param meas The CoreMeasurement
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool MeasurementModel::SetMeasurement(CoreMeasurement *meas)
{
   bool retval = false;

   if (meas != NULL)
   {
      measurement = meas;
      measurementType = measurement->GetTypeName();
      retval = true;

      theData = measurement->GetMeasurementDataPointer();
      theDataDerivatives = measurement->GetDerivativePointer();
      covariance = *(measurement->GetCovariance());
      measurementBias.SetSize(measurement->GetMeasurementSize());
      for (Integer i = 0; i < measurement->GetMeasurementSize(); ++i)
         measurementBias[i] = 0.0;
      noiseSigma.SetSize(measurement->GetMeasurementSize());
      for (Integer i = 0; i < measurement->GetMeasurementSize(); ++i)
         noiseSigma[i] = 1.0;
   }

   return retval;
}


//------------------------------------------------------------------------------
// const MeasurementData& GetMeasurement()
//------------------------------------------------------------------------------
/**
 * Retrieves the last calculated measurement
 *
 * @return A reference to the calculated MeasurementData
 */
//------------------------------------------------------------------------------
const MeasurementData& MeasurementModel::GetMeasurement()
{
   return *theData;
}


//------------------------------------------------------------------------------
// const Rmatrix& CalculateMeasurementDerivatives(GmatBase *obj, Integer id)
//------------------------------------------------------------------------------
/**
 * Calculates the measurement and derivatives
 *
 * Calls the core measurement and retrieves the measurement data and derivatives
 * for the current state of the participants.  If the measurement is not
 * possible given that state, the MeasurementData container, theData, is cleared
 * and its isFeasible flag is set to false, and the return pointer is set to
 * NULL.
 *
 * Note that while the measurement is calculated, it is not returned.  Users of
 * this class can retrieve the measurement data using the GetMeassurement()
 * method (to get the last measurement calculated) or the CalculateMeasurement
 * method (to recalculate the measurement).
 *
 * @param obj The object that has the "with respect to" parameter
 * @param id The parameter index of the "with respect to" parameter
 *
 * @return A reference to the calculated derivatives.
 */
//------------------------------------------------------------------------------
const std::vector<RealArray>& MeasurementModel::CalculateMeasurementDerivatives(
                                             GmatBase *obj, Integer id)
{
   #ifdef DEBUG_DERIVATIVES
      MessageInterface::ShowMessage("MeasurementModel::CalculateMeasurement"
            "Derivatives(%s, %d) called; this = %p, obj = %p\n",
            obj->GetName().c_str(), id, this, obj);
      if (obj == this)
         MessageInterface::ShowMessage("This MM is the object\n");
   #endif
   return measurement->CalculateMeasurementDerivatives(obj, id);
}


//-----------------------------------------------------------------------------
// bool MeasurementModel::WriteMeasurements()
//-----------------------------------------------------------------------------
/**
 * Writes out measurement data
 *
 * This method is a place holder for data file writes.  It is likely that is
 * will be removed from later builds.
 *
 * @return false always
 */
//-----------------------------------------------------------------------------
bool MeasurementModel::WriteMeasurements()
{
   return false;
}

//-----------------------------------------------------------------------------
// bool MeasurementModel::WriteMeasurements()
//-----------------------------------------------------------------------------
/**
 * Writes out measurement data
 *
 * This method is a place holder for data file writes.  It is likely that is
 * will be removed from later builds.
 *
 * @param id The ID of the measurement that gets written
 *
 * @return false always
 */
//-----------------------------------------------------------------------------
bool MeasurementModel::WriteMeasurement(Integer id)
{
   return false;
}


//------------------------------------------------------------------------------
// Integer HasParameterCovariances(Integer parameterId)
//------------------------------------------------------------------------------
/**
 * Determines if the model has covariances for the specified parameter
 *
 * @param parameterId The parameter
 *
 * @return The size of the covariance matrix for that parameter.  The matrix is
 * square, with the returned number of rows and columns.
 */
//------------------------------------------------------------------------------
Integer MeasurementModel::HasParameterCovariances(Integer parameterId)
{
   if (parameterId == Bias)
   {
      return 1;
   }

   return GmatBase::HasParameterCovariances(parameterId);
}


//------------------------------------------------------------------------------
// Integer GetEventCount()
//------------------------------------------------------------------------------
/**
 * Returns the number of events in the CoreMeasurement associated with this
 * measurement model
 *
 * @return The number of events
 */
//------------------------------------------------------------------------------
Integer MeasurementModel::GetEventCount()
{
   return measurement->GetEventCount();
}


//------------------------------------------------------------------------------
// Event* GetEvent(Integer whichOne)
//------------------------------------------------------------------------------
/**
 * Retrieves an event
 *
 * @param whichOne The index of the requested event
 *
 * @return A pointer to the event
 */
//------------------------------------------------------------------------------
Event* MeasurementModel::GetEvent(Integer whichOne)
{
   return measurement->GetEvent(whichOne);
}


//------------------------------------------------------------------------------
// bool MeasurementModel::SetEventData(Event *locatedEvent)
//------------------------------------------------------------------------------
/**
 * Passes in a found event so that the resulting data can be processed and
 * passed to other events that need it.
 *
 * @param locatedEvent The event that has been located
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool MeasurementModel::SetEventData(Event *locatedEvent)
{
   return measurement->SetEventData(locatedEvent);
}

//Rmatrix* MeasurementModel::GetParameterCovariances(Integer parameterId)
//{
//   if (parameterId == Bias)
//   {
//      return covariance;
//   }
//
//   return GmatBase::GetParameterCovariances(parameterId);
//}


//------------------------------------------------------------------------------
// Hardware* GetHardwarebyType(GmatBase *onObject, const std::string &hwType,
//       const std::string hwName)
//------------------------------------------------------------------------------
/**
 * Retrieves hardware by type
 *
 * @note: This method is apparently not yet implemented
 *
 * @param onObject Object that has the hardware
 * @param hwType The type of the hardware
 * @param hwName The name of the hardware object
 *
 * @return The hardware object
 */
//------------------------------------------------------------------------------
Hardware* MeasurementModel::GetHardwarebyType(GmatBase *onObject,
      const std::string &hwType, const std::string hwName)
{
   Hardware *hw = NULL;

   return hw;
}


//------------------------------------------------------------------------------
// Hardware* GetHardware(GmatBase *onObject, const std::string hwName)
//------------------------------------------------------------------------------
/**
 * Retrieves a hardware object
 *
 * @note: This method is apparently not yet implemented
 *
 * @param onObject Object that has the hardware
 * @param hwName The name of the hardware object
 *
 * @return The hardware object
 */
//------------------------------------------------------------------------------
Hardware* MeasurementModel::GetHardware(GmatBase *onObject,
      const std::string hwName)
{
   Hardware *hw = NULL;

   return hw;
}


//------------------------------------------------------------------------------
// void SetCorrection(const std::string correctionName,
//       const std::string correctionType)
//------------------------------------------------------------------------------
/**
 * Passes a correction name into the owned CoreMeasurement
 *
 * @param correctionName The name of the correction
 * @param correctionType The type of correction
 *
 * @note This information is not passed via SetStringParameter because it isn't
 *       managed by scripting on MeasurementModels.  It is managed in the
 *       TrackingSystem code.  If it becomes part of the measurements, this
 *       code should move into the Get/SetStringParameter methods.
 */
//------------------------------------------------------------------------------
void MeasurementModel::SetCorrection(const std::string& correctionName,
         const std::string& correctionType)
{
   #ifdef DEBUG_SET_CORRECTION
	  MessageInterface::ShowMessage("Start MeasurementModel::SetCorrection():   correctionName = '%s',   correctionType = '%s'\n", correctionName.c_str(), correctionType.c_str());
   #endif
   
   // Only PhysicalModels receive corrections
   if (measurement->IsOfType("PhysicalMeasurement"))
   {
      // We probably ought to also send in the type here
      #ifdef DEBUG_SET_CORRECTION
	     MessageInterface::ShowMessage("correctionName = '%s' will be added.\n", correctionName.c_str(), correctionType.c_str());
      #endif
      ((PhysicalMeasurement *)(measurement))->AddCorrection(correctionName);
   }
}
