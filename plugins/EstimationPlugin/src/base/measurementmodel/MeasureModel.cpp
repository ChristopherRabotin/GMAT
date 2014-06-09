//$Id$
//------------------------------------------------------------------------------
//                           MeasureModel
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
// Created: Jan 31, 2014
/**
 * The Measurement Model base class for estimation
 */
//------------------------------------------------------------------------------

#include "MeasureModel.hpp"
#include "SignalBase.hpp"
#include "ProgressReporter.hpp"
#include "MeasurementException.hpp"

#include "SignalBase.hpp"
#include "PhysicalSignal.hpp"
//#include "SinglePointSignal.hpp"

#include "MessageInterface.hpp"
#include "TextParser.hpp"
#include <sstream>

//#define DEBUG_INITIALIZATION

//------------------------------------------------------------------------------
// Static data
//------------------------------------------------------------------------------

const std::string
MeasureModel::PARAMETER_TEXT[MeasurementParamCount - GmatBaseParamCount] =
{
   "SignalPath",
};


const Gmat::ParameterType
MeasureModel::PARAMETER_TYPE[MeasurementParamCount - GmatBaseParamCount] =
{
   Gmat::OBJECTARRAY_TYPE,
};



//------------------------------------------------------------------------------
// MeasureModel(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param name The scripted name (when needed) for the measurement model
 */
//------------------------------------------------------------------------------
MeasureModel::MeasureModel(const std::string &name) :
   GmatBase          (Gmat::MEASUREMENT_MODEL, "SignalBasedMeasurement", name),
   feasible          (false),
   navLog            (NULL),
   logLevel          (0),              // Default to everything
   isPhysical        (true),
   solarsys          (NULL)
{
}


//------------------------------------------------------------------------------
// ~MeasureModel()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
MeasureModel::~MeasureModel()
{
}


//------------------------------------------------------------------------------
// MeasureModel(const MeasureModel& mm)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param mm The model used as a template for this one
 */
//------------------------------------------------------------------------------
MeasureModel::MeasureModel(const MeasureModel& mm) :
   GmatBase          (mm),
   feasible          (false),
   navLog            (mm.navLog),
   logLevel          (mm.logLevel),
   isPhysical        (mm.isPhysical),
   solarsys          (mm.solarsys)
{
}


//------------------------------------------------------------------------------
// MeasureModel& operator=(const MeasureModel& mm)
//------------------------------------------------------------------------------
/**
 * Assignmant operator
 *
 * @param mm The measurement model used to configure this one
 *
 * @return This model, set to match mm
 */
//------------------------------------------------------------------------------
MeasureModel& MeasureModel::operator=(const MeasureModel& mm)
{
   if (this == &mm)
   {
      GmatBase::operator=(mm);

      theData.clear();
      feasible = false;
      navLog = mm.navLog;
      logLevel = mm.logLevel;
      isPhysical = mm.isPhysical;
      solarsys = mm.solarsys;
   }

   return *this;
}


//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Creates a copy of this model
 *
 * @return A clone of this model
 */
//------------------------------------------------------------------------------
GmatBase* MeasureModel::Clone() const
{
   return new MeasureModel(*this);
}


//------------------------------------------------------------------------------
// void SetSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
/**
 * Sets the solar system pointer
 *
 * @param ss The solar system teh model should use
 */
//------------------------------------------------------------------------------
void MeasureModel::SetSolarSystem(SolarSystem *ss)
{
   solarsys = ss;
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Returns the script name for the parameter
 *
 * @param id The id of the parameter
 *
 * @return The script name
 */
//------------------------------------------------------------------------------
std::string MeasureModel::GetParameterText(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < MeasurementParamCount)
      return PARAMETER_TEXT[id - GmatBaseParamCount];
   return GmatBase::GetParameterText(id);
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string& str) const
//------------------------------------------------------------------------------
/**
 * Retrieves the ID for a scriptable parameter
 *
 * @param str The string used for the parameter
 *
 * @return The parameter ID
 */
//------------------------------------------------------------------------------
Integer MeasureModel::GetParameterID(const std::string& str) const
{
   for (Integer i = GmatBaseParamCount; i < MeasurementParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - GmatBaseParamCount])
         return i;
   }
   return GmatBase::GetParameterID(str);
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the type for a specified parameter
 *
 * @param id The ID for the parameter
 *
 * @return The parameter's type
 */
//------------------------------------------------------------------------------
Gmat::ParameterType MeasureModel::GetParameterType(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < MeasurementParamCount)
      return PARAMETER_TYPE[id - GmatBaseParamCount];

   return GmatBase::GetParameterType(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a text description for a parameter
 *
 * @param id The ID for the parameter
 *
 * @return The description string
 */
//------------------------------------------------------------------------------
std::string MeasureModel::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value of a string parameter
 *
 * @param id The ID for the parameter
 *
 * @return The value of the parameter
 */
//------------------------------------------------------------------------------
std::string MeasureModel::GetStringParameter(const Integer id) const
{
   return GmatBase::GetStringParameter(id);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string& value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a string parameter
 *
 * @param id The ID for the parameter
 * @param value The value for the parameter
 *
 * @return true if the paramter was set, false if not
 */
//------------------------------------------------------------------------------
bool MeasureModel::SetStringParameter(const Integer id,
      const std::string& value)
{
   if (id == SIGNAL_PATH)
   {
//      StringArray *partList = DecomposePathString(value);
//      if (partList->size() > 0)
//      {
//         participantLists.push_back(partList);
//         return true;
//      }
//
//      // The list is empty
//      delete partList;
      return false;
   }

   return GmatBase::SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string parameter from a string array
 *
 * @param id The ID for the parameter
 * @param index The index into the array
 *
 * @return The value of the parameter
 */
//------------------------------------------------------------------------------
std::string MeasureModel::GetStringParameter(const Integer id,
      const Integer index) const
{
   return GmatBase::GetStringParameter(id, index);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string& value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets a string parameter in an array of strings
 *
 * @param id The ID for the parameter
 * @param value The new value for the parameter
 * @index The desired location of the parameter in the array
 *
 * @return true if the parameter was set, false if not
 */
//------------------------------------------------------------------------------
bool MeasureModel::SetStringParameter(const Integer id,
      const std::string& value, const Integer index)
{
   if ((index < 0) || (index > (Integer)participantLists.size()))
      throw MeasurementException("Index out of bounds when setting string "
            "parameter");

   if (id == SIGNAL_PATH)
   {
      if ((Integer)participantLists.size() == index)
      {
         StringArray *partList = new StringArray;
         participantLists.push_back(partList);
      }
      participantLists[index]->push_back(value);
      return true;
   }

   return GmatBase::SetStringParameter(id, value, index);
}

//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string array parameter
 *
 * @param id The ID for the parameter
 *
 * @return The string array
 */
//------------------------------------------------------------------------------
const StringArray& MeasureModel::GetStringArrayParameter(const Integer id) const
{
   return GmatBase::GetStringArrayParameter(id);
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id,
//       const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retireves a string array from an array of string arrays
 *
 * @param id The ID for the parameter
 * @param index The index into the array of arrays
 *
 * @return The requested string array
 */
//------------------------------------------------------------------------------
const StringArray& MeasureModel::GetStringArrayParameter(const Integer id,
      const Integer index) const
{
   if (id == SIGNAL_PATH)
   {
      if ((index >= 0) && ((Integer)participantLists.size() > index))
         return *(participantLists[index]);
      throw MeasurementException("Index out of bounds when accessing the "
            "signal path");
   }

   return GmatBase::GetStringArrayParameter(id, index);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string& label) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value of a string parameter
 *
 * @param label The scriptable name of the parameter
 *
 * @return The value of the parameter
 */
//------------------------------------------------------------------------------
std::string MeasureModel::GetStringParameter(const std::string& label) const
{
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string& label, const std::string& value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a string parameter
 *
 * @param label The scriptable name of the parameter
 * @param value The value for the parameter
 *
 * @return true if the paramter was set, false if not
 */
//------------------------------------------------------------------------------
bool MeasureModel::SetStringParameter(const std::string& label,
      const std::string& value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string& label,
//       const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string parameter from a string array
 *
 * @param label The scriptable name of the parameter
 * @param index The index into the array
 *
 * @return The value of the parameter
 */
//------------------------------------------------------------------------------
std::string MeasureModel::GetStringParameter(const std::string& label,
      const Integer index) const
{
   return GetStringParameter(GetParameterID(label), index);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string& label, const std::string& value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets a string parameter in an array of strings
 *
 * @param label The scriptable name of the parameter
 * @param value The new value for the parameter
 * @index The desired location of the parameter in the array
 *
 * @return true if the parameter was set, false if not
 */
//------------------------------------------------------------------------------
bool MeasureModel::SetStringParameter(const std::string& label,
      const std::string& value, const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const std::string& label) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string array parameter
 *
 * @param label The scriptable name of the parameter
 *
 * @return The string array
 */
//------------------------------------------------------------------------------
const StringArray& MeasureModel::GetStringArrayParameter(
      const std::string& label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const std::string& label,
//       const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retireves a string array from an array of string arrays
 *
 * @param label The scriptable name of the parameter
 * @param index The index into the array of arrays
 *
 * @return The requested string array
 */
//------------------------------------------------------------------------------
const StringArray& MeasureModel::GetStringArrayParameter(
      const std::string& label, const Integer index) const
{
   return GetStringArrayParameter(GetParameterID(label), index);
}


//------------------------------------------------------------------------------
// const StringArray& GetRefObjectNameArray(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * Retrieves a list of the reference objects used in the model
 *
 * @param type The type of object requested (not used)
 *
 * @return The list of names of reference objects
 */
//------------------------------------------------------------------------------
const StringArray& MeasureModel::GetRefObjectNameArray(
      const Gmat::ObjectType type)
{
   refObjectNames.clear();

   // Build the list
   for (std::vector<StringArray*>::iterator sa = participantLists.begin();
         sa != participantLists.end(); ++sa)
   {
      for (UnsignedInt i = 0; i < ((*sa)->size()); ++i)
      {
         std::string candidate = (*sa)->at(i);
         if (find(refObjectNames.begin(), refObjectNames.end(), candidate) ==
               refObjectNames.end())
            refObjectNames.push_back(candidate);
      }
   }

   return refObjectNames;
}

//------------------------------------------------------------------------------
// bool RenameRefObject(const Gmat::ObjectType type, const std::string &oldName,
//       const std::string &newName)
//------------------------------------------------------------------------------
/**
 * Renames the reference objects
 *
 * @param type The type of the object being renamed
 * @param oldName The name of the object before the rename
 * @param newName The new name of the object
 *
 * @return true if an object reference name was changed, false if not
 */
//------------------------------------------------------------------------------
bool MeasureModel::RenameRefObject(const Gmat::ObjectType type,
      const std::string &oldName, const std::string &newName)
{
   bool retval = false;

   // Handle participant name changes
   for (UnsignedInt i = 0; i < participantLists.size(); ++i)
   {
      for (UnsignedInt j = 0; j < participantLists.at(i)->size(); ++j)
      {
         if (participantLists.at(i)->at(j) == oldName)
         {
            participantLists.at(i)->at(j) = newName;
            retval = true;
         }
      }
   }

   return retval;
}


//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase* obj, const Gmat::ObjectType type,
//       const std::string& name)
//------------------------------------------------------------------------------
/**
 * Sets pointers to the model's reference objects
 *
 * @param obj The object pointer
 * @param type The type of the object (not used)
 * @param name The name of the object  (not used)
 *
 * @return true if the pointer was set, false if not
 */
//------------------------------------------------------------------------------
bool MeasureModel::SetRefObject(GmatBase* obj, const Gmat::ObjectType type,
      const std::string& name)
{
   bool retval = false;

   if (obj->IsOfType(Gmat::SPACE_POINT))
   {
      std::string name = obj->GetName();
      for (UnsignedInt i = 0; i < participantLists.size(); ++i)
      {
         if (find(participantLists[i]->begin(), participantLists[i]->end(),
               name) != participantLists[i]->end())
         {
            if (SetRefObject(obj, type, name, i))
               retval = true;
         }
      }
   }

   return retval;
}

//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase* obj, const Gmat::ObjectType type,
//       const std::string& name, const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets the object pointers used in the measurement
 *
 * @param obj   The object used in the measurement
 * @param type  The object's type (not used)
 * @param name  The object's name (not used)
 * @param index The index of participant list that receives the SpacePoint
 *              participants
 *
 * @return true is the object pointer was set anywhere; false if not
 */
//------------------------------------------------------------------------------
bool MeasureModel::SetRefObject(GmatBase* obj, const Gmat::ObjectType type,
      const std::string& name, const Integer index)
{
   bool retval = false;

   if (obj->IsOfType(Gmat::SPACE_POINT))
   {
      if (find(candidates.begin(), candidates.end(), obj) ==
            candidates.end())
      {
         #ifdef DEBUG_INITIALIZATION
            MessageInterface::ShowMessage("Adding object %s to %s\n",
                  obj->GetName().c_str(), instanceName.c_str());
         #endif
         candidates.push_back(obj);
         retval = true;
      }
   }

   return retval;
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Prepares the measurement model for use
 *
 * @return true if the initialization succeeds, false if it fails
 */
//------------------------------------------------------------------------------
bool MeasureModel::Initialize()
{
   bool retval = false;

   if (GmatBase::Initialize())
   {
      if (navLog)
      {
         logLevel = navLog->GetLogLevel("Measurement");
         #ifdef DEBUG_LOGGING
            MessageInterface::ShowMessage("Current log level for measurements "
                  "is %d\n", logLevel);
         #endif
      }

      if (participantLists.size() > 0)
      {
         std::string theMissing = "";

         // Clear stale pointers
         for (UnsignedInt i = 0; i < participants.size(); ++i)
         {
            delete participants[i];
         }
         participants.clear();
         for (UnsignedInt i = 0; i < signalPaths.size(); ++i)
            delete signalPaths[i];
         signalPaths.clear();
         theData.clear();

         for (UnsignedInt i = 0; i < participantLists.size(); ++i)
         {
            participants.push_back(new ObjectArray);
            if (participantLists[i]->size() < 2)
               throw MeasurementException("Participant list size is too short");
         }

         // Put all participants in place for the model
         for (UnsignedInt i = 0; i < participantLists.size(); ++i)
         {
            for (UnsignedInt j = 0; j < participantLists[i]->size(); ++j)
            {
               std::string pname = participantLists[i]->at(j);
               GmatBase *obj = NULL;
               for (UnsignedInt k = 0; k < candidates.size(); ++k)
                  if (candidates[k]->GetName() == pname)
                     obj = candidates[k];
               if (obj != NULL)
                  participants[i]->push_back(obj);
               else
               {
                  if (theMissing != "")
                     theMissing += ", ";
                  theMissing += pname;
               }
            }
         }

         if (theMissing == "")
         {
            // Build the signals
            for (UnsignedInt i = 0; i < participantLists.size(); ++i)
            {
               SignalBase *head = NULL;
               for (UnsignedInt j = 0; j < participantLists[i]->size()-1; ++j)
               {
                  SignalBase *sb = NULL;
                  if (isPhysical)
                  {
                     sb = new PhysicalSignal("");
                     if (navLog)
                        sb->SetProgressReporter(navLog);
                  }
                  else
                  {
                     // The signals are single point
//                     sb = new SinglePointSignal("");
//                     if (navLog)
//                        sb->SetProgressReporter(navLog);
                     throw MeasurementException("Single point signals are not "
                           "yet implemented");
                  }

                  if (sb)
                  {
                     sb->SetSolarSystem(solarsys);
                     if (sb->SetTransmitParticipantName(
                           participantLists[i]->at(j)) == false)
                     {
                        throw MeasurementException("Failed to set the name of "
                              "the transmit participant to " +
                              participantLists[i]->at(j) +
                              " on measurement model " + instanceName);
                     }
                     if (sb->SetReceiveParticipantName(
                           participantLists[i]->at(j+1)) == false)
                     {
                        throw MeasurementException("Failed to set the name of "
                              "the receive participant to " +
                              participantLists[i]->at(j+1) +
                              " on measurement model " + instanceName);
                     }

                     GmatBase *obj = participants[i]->at(j);
                     if (sb->SetRefObject(obj, obj->GetType(),
                           obj->GetName()) == false)
                     {
                        throw MeasurementException("Failed to set the transmit "
                              "participant");
                     }

                     obj = participants[i]->at(j+1);
                     if (sb->SetRefObject(obj, obj->GetType(),
                           obj->GetName()) == false)
                     {
                        throw MeasurementException("Failed to set the receive "
                              "participant\n");
                     }

                     if (!sb->Initialize())
                     {
                        throw MeasurementException("Signal initialization "
                              "failed in measurement model " + instanceName);
                     }

                     if (j == 0)
                     {
                        signalPaths.push_back(sb);
                        head = sb;
                        theData.push_back(&(sb->GetSignalData()));
                     }
                     else
                        head->Add(sb);
                  }
               }
               retval = true;
            }
         }
         else
            throw MeasurementException("Cannot configure the measurement "
                  "model " + instanceName + "; the following participants are "
                  "not in the object: " + theMissing);

         if (navLog)
         {
            std::stringstream data;
            data.precision(16);

            if (logLevel <= 1)
            {
               data << "   " << instanceName
                    << (retval ? " initialized\n" : " failed to initialize\n");
            }

            if (logLevel == 0)
            {
               data << "   For measurement model " << instanceName
                    << ", constructed the signal path(s):" << "\n";
               for (UnsignedInt i = 0; i < signalPaths.size(); ++i)
                  data << "      "
                       << (signalPaths[i]->GetPathDescription(true))
                       << "\n";
            }

            navLog->WriteData(data.str());
         }
      }
      else
      {

      }
   }

   return retval;
}

const std::vector<SignalData*>& MeasureModel::GetSignalData()
{
   return theData;
}

//------------------------------------------------------------------------------
// bool MeasureModel::IsMeasurementFeasible()
//------------------------------------------------------------------------------
/**
 * Returns the feasibility flag for the most recent measurement computation
 *
 * @return true if the measurement was feasible, false if not (or if no
 * calculation has been performed)
 */
//------------------------------------------------------------------------------
bool MeasureModel::IsMeasurementFeasible()
{
   return feasible;
}

//------------------------------------------------------------------------------
// bool SetProgressReporter(ProgressReporter* reporter)
//------------------------------------------------------------------------------
/**
 * Sets the logging object for the measurements
 *
 * @param reporter The progress reporter used for log messages
 *
 * @return true if the reporter is set
 */
//------------------------------------------------------------------------------
bool MeasureModel::SetProgressReporter(ProgressReporter* reporter)
{
   bool retval = false;

   if (reporter)
   {
      navLog = reporter;
      retval = true;
   }

   return retval;
}

//@todo: Move the calling parameters here into the adapters
//------------------------------------------------------------------------------
// bool CalculateMeasurement(bool withEvents, ObservationData* forObservation,
//       std::vector<RampTableData>* rampTB)
//------------------------------------------------------------------------------
/**
 * Fires all of the Signal objects to generate the raw measurement data
 *
 * @param withEvents Flag used to indicate if a light time solution should be
 *                   computed (not used)
 * @param forObservation An observation supplying data needed for the
 *                       calculation (not used)
 * @param rampTB A ramp table for the data (not used)
 *
 * @return true if the the calculation succeeded, false if not
 */
//------------------------------------------------------------------------------
bool MeasureModel::CalculateMeasurement(bool withEvents,
      ObservationData* forObservation, std::vector<RampTableData>* rampTB)
{
   bool retval = false;
   feasible = true;

   for (UnsignedInt i = 0; i < signalPaths.size(); ++i)
   {
      if (signalPaths[i]->ModelSignal() == false)
      {
         throw MeasurementException("Signal modeling failed in model " +
               instanceName);
      }
      feasible = feasible && signalPaths[i]->IsSignalFeasible();
   }
   retval = true;

   return retval; //theData;
}

//------------------------------------------------------------------------------
// StringArray* DecomposePathString(const std::string &value)
//------------------------------------------------------------------------------
///**
// * Breaks apart a signal path string into a participant list
// *
// * @param value The string
// *
// * @return
// */
////------------------------------------------------------------------------------
//StringArray* MeasureModel::DecomposePathString(const std::string &value)
//{
//   TextParser theParser;
//   StringArray *partList = new StringArray;
//
//   (*partList)  = theParser.SeparateBy(value, ", ");
//
//   MessageInterface::ShowMessage("Path string %s has members:\n",
//         value.c_str());
//   for (UnsignedInt i = 0; i < partList->size(); ++i)
//      MessageInterface::ShowMessage("   %s\n", partList->at(i).c_str());
//
//   return partList;
//}


const std::vector<RealArray>& MeasureModel::CalculateMeasurementDerivatives(
      GmatBase* obj, Integer id)
{
   theDataDerivatives.clear();

   // Collect the data from the signals
   for (UnsignedInt i = 0; i < signalPaths.size(); ++i)
   {
      std::vector<RealArray> pathDerivative =
            signalPaths[i]->ModelSignalDerivative(obj, id);
      for (UnsignedInt j = 0; j < pathDerivative.size(); ++j)
         theDataDerivatives.push_back(pathDerivative[j]);
   }

   return theDataDerivatives;
}
