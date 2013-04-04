//$Id: TrackingSystem.cpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                             TrackingSystem
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Darrel J. Conway
// Created: 2010/02/18 by Darrel Conway (Thinking Systems)
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task 28
//
/**
 * Implementation for the TrackingSystem class
 */
//------------------------------------------------------------------------------


#include "TrackingSystem.hpp"
#include "TrackingSystemException.hpp"
#include <sstream>

#include "MessageInterface.hpp"

//#define DEBUG_TRACKINGSYSTEM_INITIALIZATION

//------------------------------------------------------------------------------
// Static data
//------------------------------------------------------------------------------

/// Script labels for the tracking system properties
const std::string
TrackingSystem::PARAMETER_TEXT[TrackingSystemParamCount - GmatBaseParamCount] =
   {
      "Add",
      "DataFiles",
      "TroposphereModel",
      "IonosphereModel"
   };


/// Tracking system property types
const Gmat::ParameterType
TrackingSystem::PARAMETER_TYPE[TrackingSystemParamCount - GmatBaseParamCount] =
   {
      Gmat::STRINGARRAY_TYPE,
      Gmat::STRINGARRAY_TYPE,
      Gmat::STRING_TYPE,
      Gmat::STRING_TYPE
   };


//------------------------------------------------------------------------------
// TrackingSystem(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param type The script label for the tracking system subtype
 * @param name The name of the new tracking system
 */
//------------------------------------------------------------------------------
TrackingSystem::TrackingSystem(const std::string &type,
         const std::string &name) :
   GmatBase          (Gmat::TRACKING_SYSTEM, type, name)
{
   objectTypes.push_back(Gmat::TRACKING_SYSTEM);
   objectTypes.push_back(Gmat::MEASUREMENT_MODEL);
   objectTypeNames.push_back("TrackingSystem");

   parameterCount = TrackingSystemParamCount;

   troposphereModel = "None";
   ionosphereModel  = "None";
}


//------------------------------------------------------------------------------
// ~TrackingSystem()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
TrackingSystem::~TrackingSystem()
{
}


//------------------------------------------------------------------------------
// TrackingSystem(const TrackingSystem& ts)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param ts The TrackingSystem that gets copied here
 */
//------------------------------------------------------------------------------
TrackingSystem::TrackingSystem(const TrackingSystem& ts) :
   GmatBase             (ts),
   trackingDataNames    (ts.trackingDataNames),
   trackingFiles        (ts.trackingFiles),
   allowedMeasurements  (ts.allowedMeasurements)
{
   UnsignedInt measSize, fileSize;
   measSize = trackingDataNames.size();
   fileSize = trackingFiles.size();

   measurements.clear();
   datafiles.clear();
   for (UnsignedInt i = 0; i < measSize; ++i)
      measurements.push_back(NULL);
   for (UnsignedInt i = 0; i < fileSize; ++i)
      datafiles.push_back(NULL);

   troposphereModel = ts.troposphereModel;
   ionosphereModel  = ts.ionosphereModel;
}


//------------------------------------------------------------------------------
// TrackingSystem& operator=(const TrackingSystem& ts)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param ts The TrackingSystem that gets provides data for this one
 *
 * @return This TrackingSystem, configured to match ts
 */
//------------------------------------------------------------------------------
TrackingSystem& TrackingSystem::operator=(const TrackingSystem& ts)
{
   if (this != &ts)
   {
      GmatBase::operator=(ts);

      trackingDataNames   = ts.trackingDataNames;
      trackingFiles       = ts.trackingFiles;
      allowedMeasurements = ts.allowedMeasurements;

      UnsignedInt measSize, fileSize;
      measSize = trackingDataNames.size();
      fileSize = trackingFiles.size();

      measurements.clear();
      datafiles.clear();
      for (UnsignedInt i = 0; i < measSize; ++i)
         measurements.push_back(NULL);
      for (UnsignedInt i = 0; i < fileSize; ++i)
         datafiles.push_back(NULL);

      troposphereModel = ts.troposphereModel;
      ionosphereModel  = ts.ionosphereModel;
   }

   return *this;
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string & str) const
//------------------------------------------------------------------------------
/**
 * Retrieves the ID for a scriptable property of this object
 *
 * @param str The script string used for the property
 *
 * @return The property's ID
 */
//------------------------------------------------------------------------------
Integer TrackingSystem::GetParameterID(const std::string & str) const
{
   for (Integer i = GmatBaseParamCount; i < TrackingSystemParamCount; i++)
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
 * Finds and reports the type of a scriptable property
 *
 * @param id The ID of the property
 *
 * @return The enumerated type of the property
 */
//------------------------------------------------------------------------------
Gmat::ParameterType TrackingSystem::GetParameterType(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < TrackingSystemParamCount)
      return PARAMETER_TYPE[id - GmatBaseParamCount];

   return GmatBase::GetParameterType(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Returns a string describing the type of the property with thd input ID
 *
 * @param id The ID of the property
 *
 * @return The property's type
 */
//------------------------------------------------------------------------------
std::string TrackingSystem::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the text string used to script the property with the input ID
 *
 * @param id The ID of the property
 *
 * @return The text string
 */
//------------------------------------------------------------------------------
std::string TrackingSystem::GetParameterText(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < TrackingSystemParamCount)
      return PARAMETER_TEXT[id - GmatBaseParamCount];
   return GmatBase::GetParameterText(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterUnit(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the units used for a property
 *
 * @param id The ID of the property
 *
 * @return The string describing the units of the property
 *
 * @note GMAT does not yet support units
 */
//------------------------------------------------------------------------------
std::string TrackingSystem::GetParameterUnit(const Integer id) const
{
   return GmatBase::GetParameterUnit(id);
}



//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string based property
 *
 * @param id The property's ID
 *
 * @return The property
 */
//------------------------------------------------------------------------------
std::string TrackingSystem::GetStringParameter(const Integer id) const
{
   switch (id)
   {
      case TROPOSPHERE_MODEL:
         return troposphereModel;

      case IONOSPHERE_MODEL:
         return ionosphereModel;
   }

   return GmatBase::GetStringParameter(id);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string & value)
//------------------------------------------------------------------------------
/**
 * Sets a string based property
 *
 * @param id The property's ID
 * @param value The value of the property
 *
 * @return true if the property was set, false if not
 */
//------------------------------------------------------------------------------
bool TrackingSystem::SetStringParameter(const Integer id,
         const std::string& value)
{
   switch (id)
   {
      case ADD_DATA:
         if (find(trackingDataNames.begin(), trackingDataNames.end(), value) ==
                  trackingDataNames.end())
         {
            trackingDataNames.push_back(value);
            datafiles.push_back(NULL);
         }
         return true;

      case FILELIST:
         if (find(trackingFiles.begin(), trackingFiles.end(), value) ==
                  trackingFiles.end())
         {
            trackingFiles.push_back(value);
            measurements.push_back(NULL);
         }
         return true;

      case TROPOSPHERE_MODEL:
      	troposphereModel = value;
         return true;

      case IONOSPHERE_MODEL:
      	ionosphereModel = value;
         return true;

      default:
         break;
   }

   return GmatBase::SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string property from a StringArray
 *
 * @param id The property ID
 * @param index The index of the specific string requested
 *
 * @return The string
 */
//------------------------------------------------------------------------------
std::string TrackingSystem::GetStringParameter(const Integer id,
         const Integer index) const
{
   switch (id)
   {
      case ADD_DATA:
         if ((index >= 0) && ((Integer)trackingDataNames.size() > index))
            return trackingDataNames[index];
         break;

      case FILELIST:
         if ((index >= 0) && ((Integer)trackingFiles.size() > index))
            return trackingFiles[index];
         break;

      default:
         break;
   }

   return GmatBase::GetStringParameter(id, index);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string & value,
//          const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets a string property in a StringArray
 *
 * @param id The property ID
 * @param value The value of the entry
 * @param index The index of the specific string being set or changed
 *
 * @return true if the property was changes, false if not
 */
//------------------------------------------------------------------------------
bool TrackingSystem::SetStringParameter(const Integer id,
         const std::string& value, const Integer index)
{
   switch (id)
   {
      case ADD_DATA:
         if (index >= 0)
         {
            // Only add if it is not in the list, regardless of index
            if (find(trackingDataNames.begin(), trackingDataNames.end(),
                     value) == trackingDataNames.end())
            {
               if (index == (Integer)trackingDataNames.size())
               {
                  trackingDataNames.push_back(value);
                  datafiles.push_back(NULL);
               }
               else
               {
                  if (index > (Integer)trackingDataNames.size())
                     throw TrackingSystemException("Attempting to add tracking"
                              " system data outside of the allowed range of the"
                              " data array");
                  trackingDataNames[index] = value;
               }
               return true;
            }

            // Make the addition idempotent
            if (index < (Integer)trackingDataNames.size())
               if (trackingDataNames[index] == value)
                  return true;
         }
         break;

      case FILELIST:
         if (index >= 0)
         {
            // Only add if it is not in the list, regardless of index
            if (find(trackingFiles.begin(), trackingFiles.end(),
                     value) == trackingFiles.end())
            {
               if (index == (Integer)trackingFiles.size())
               {
                  trackingFiles.push_back(value);
                  measurements.push_back(NULL);
               }
               else
               {
                  if (index > (Integer)trackingFiles.size())
                     throw TrackingSystemException("Attempting to add tracking"
                              " file name outside of the allowed range of the"
                              " data array");
                  trackingFiles[index] = value;
               }
               return true;
            }

            // Make the addition idempotent
            if (index < (Integer)trackingFiles.size())
               if (trackingFiles[index] == value)
                  return true;
         }
         break;

      default:
         break;
   }

   return GmatBase::SetStringParameter(id, value, index);
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a list of properties contained in a StringArray
 *
 * @param The ID of the StringArray
 *
 * @return The array
 */
//------------------------------------------------------------------------------
const StringArray& TrackingSystem::GetStringArrayParameter(
         const Integer id) const
{
   switch (id)
   {
      case ADD_DATA:
         return trackingDataNames;

      case FILELIST:
         return trackingFiles;

      default:
         break;
   }

   return GmatBase::GetStringArrayParameter(id);
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id,
//          const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a list of properties contained in an array of StringArrays
 *
 * @param id The ID of the StringArray
 * @param index The index of the StringArray in the containing array
 *
 * @return The StringArray
 */
//------------------------------------------------------------------------------
const StringArray& TrackingSystem::GetStringArrayParameter(const Integer id,
         const Integer index) const
{
   return GmatBase::GetStringArrayParameter(id, index);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string & label) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string property from the object
 *
 * @param label Script string used to identify the property
 *
 * @return The property
 */
//------------------------------------------------------------------------------
std::string TrackingSystem::GetStringParameter(const std::string & label) const
{
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// std::string SetStringParameter(const std::string & label,
//          const std::string & value)
//------------------------------------------------------------------------------
/**
 * Sets a string property in a StringArray from the object
 *
 * @param label Script string used to identify the property
 * @param value The value of the property
 *
 * @return true if the property was set, false if not
 */
//------------------------------------------------------------------------------
bool TrackingSystem::SetStringParameter(const std::string & label,
         const std::string & value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string & label,
//          const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string property in a StringArray from the object
 *
 * @param label Script string used to identify the property
 * @param index The index of the property in the StringArray
 *
 * @return The property
 */
//------------------------------------------------------------------------------
std::string TrackingSystem::GetStringParameter(const std::string & label,
         const Integer index) const
{
   return GetStringParameter(GetParameterID(label), index);
}


//------------------------------------------------------------------------------
// std::string SetStringParameter(const std::string & label,
//          const std::string & value, const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets a string property in a StringArray from the object
 *
 * @param label Script string used to identify the property
 * @param value The value of the property in the StringArray
 * @param index The index of the string in the array
 *
 * @return true if the property was set, false if not
 */
//------------------------------------------------------------------------------
bool TrackingSystem::SetStringParameter(const std::string & label,
         const std::string & value, const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const std::string& label) const
//------------------------------------------------------------------------------
/**
 * Retrieves a list of properties contained in a StringArray
 *
 * @param label Script string used to identify the property
 *
 * @return The StringArray
 */
//------------------------------------------------------------------------------
const StringArray& TrackingSystem::GetStringArrayParameter(
         const std::string & label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const std::string& label,
//          const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a list of properties contained in an array of StringArrays
 *
 * @param label Script string used to identify the property
 * @param index The index of the StringArray in the containing array
 *
 * @return The StringArray
 */
//------------------------------------------------------------------------------
const StringArray& TrackingSystem::GetStringArrayParameter(
         const std::string& label, const Integer index) const
{
   return GetStringArrayParameter(GetParameterID(label), index);
}


//------------------------------------------------------------------------------
// bool RenameRefObject(const Gmat::ObjectType type,
//          const std::string & oldName, const std::string & newName)
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
bool TrackingSystem::RenameRefObject(const Gmat::ObjectType type,
         const std::string & oldName, const std::string & newName)
{
   /// @todo TrackingSystem rename code needs to be implemented
   return GmatBase::RenameRefObject(type, oldName, newName);
}


//------------------------------------------------------------------------------
// bool SetRefObjectName(const Gmat::ObjectType type, const std::string & name)
//------------------------------------------------------------------------------
/**
 * This method sets a reference object's name
 *
 * @param type The type of the object
 * @param name The name of the object
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool TrackingSystem::SetRefObjectName(const Gmat::ObjectType type,
         const std::string & name)
{
   return GmatBase::SetRefObjectName(type, name);
}


//------------------------------------------------------------------------------
// const ObjectTypeArray & GetRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * This method retrieves am array of reference object types
 *
 * @return The array
 */
//------------------------------------------------------------------------------
const ObjectTypeArray & TrackingSystem::GetRefObjectTypeArray()
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
const StringArray& TrackingSystem::GetRefObjectNameArray(
         const Gmat::ObjectType type)
{
   #ifdef DEBUG_TRACKINGSYSTEM_INITIALIZATION
      MessageInterface::ShowMessage(
            "TrackingSystem::GetRefObjectNameArray(%d) entered\n", type);
   #endif

   refObjectList.clear();

   if ((type == Gmat::UNKNOWN_OBJECT) || (type == Gmat::DATASTREAM) ||
       (type == Gmat::MEASUREMENT_MODEL))
   {
      if ((type == Gmat::UNKNOWN_OBJECT) || (type == Gmat::DATASTREAM))
      {
         #ifdef DEBUG_TRACKINGSYSTEM_INITIALIZATION
            MessageInterface::ShowMessage(
                  "   Adding DataFiles\n");
         #endif
         for (UnsignedInt i = 0; i < trackingFiles.size(); ++i)
            if (find(refObjectList.begin(), refObjectList.end(),
                     trackingFiles[i]) == refObjectList.end())
               refObjectList.push_back(trackingFiles[i]);
      }

      if ((type == Gmat::UNKNOWN_OBJECT) || (type == Gmat::MEASUREMENT_MODEL))
      {
         // Add the measurements this TrackingSystem needs
         for (StringArray::iterator i = trackingDataNames.begin();
               i != trackingDataNames.end(); ++i)
         {
            #ifdef DEBUG_TRACKINGSYSTEM_INITIALIZATION
               MessageInterface::ShowMessage(
                     "   Adding measurement: %s\n", i->c_str());
            #endif
            if (find(refObjectList.begin(), refObjectList.end(), *i) ==
                  refObjectList.end())
               refObjectList.push_back(*i);
         }
      }
   }
   else
   {
      // Fill in any base class needs
      refObjectList = GmatBase::GetRefObjectNameArray(type);
   }

   return refObjectList;
}


//------------------------------------------------------------------------------
// std::string GetRefObjectName(const Gmat::ObjectType type) const
//------------------------------------------------------------------------------
/**
 * Retrieves the name of a referenced object of a given type
 *
 * @param type The object's type
 *
 * @return The name of the associated object
 */
//------------------------------------------------------------------------------
std::string TrackingSystem::GetRefObjectName(const Gmat::ObjectType type) const
{
   return GmatBase::GetRefObjectName(type);
}


//------------------------------------------------------------------------------
// GmatBase* GetRefObject(const Gmat::ObjectType type, const std::string & name)
//------------------------------------------------------------------------------
/**
 * Retrieves a pointer to a referenced object of a given type and name
 *
 * @param type The object's type
 * @param name The object's name
 *
 * @return The pointer to the associated object
 */
//------------------------------------------------------------------------------
GmatBase *TrackingSystem::GetRefObject(const Gmat::ObjectType type,
         const std::string & name)
{
   return GmatBase::GetRefObject(type, name);
}


//------------------------------------------------------------------------------
// GmatBase* GetRefObject(const Gmat::ObjectType type, const std::string & name,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Retrieves a pointer to a referenced object of a given type and name from an
 * array of reference objects
 *
 * @param type The object's type
 * @param name The object's name
 * @param index The index to the object
 *
 * @return The pointer to the associated object
 */
//------------------------------------------------------------------------------
GmatBase *TrackingSystem::GetRefObject(const Gmat::ObjectType type,
         const std::string & name, const Integer index)
{
   return GmatBase::GetRefObject(type, name, index);
}


//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//       const std::string & name)
//------------------------------------------------------------------------------
/**
 * Sets a pointer to a referenced object of a given type and name
 *
 * @param obj  The object
 * @param type The object's type
 * @param name The object's name
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool TrackingSystem::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
         const std::string & name)
{
   #ifdef DEBUG_TRACKINGSYSTEM_INITIALIZATION
      MessageInterface::ShowMessage("TrackingSystem::SetRefObject: Setting ref "
            "object %s with type %s\n",
            name.c_str(), obj->GetTypeName().c_str());
   #endif

   switch(type)
   {
      case Gmat::DATASTREAM:
         for (UnsignedInt i = 0; i < trackingFiles.size(); ++i)
            if (trackingFiles[i] == name)
            {
               datafiles[i] = (DataFile*)obj;
               return true;
            }
         break;

      case Gmat::MEASUREMENT_MODEL:
         for (UnsignedInt i = 0; i < trackingDataNames.size(); ++i)
            if (trackingDataNames[i] == name)
            {
               #ifdef DEBUG_TRACKINGSYSTEM_INITIALIZATION
                  MessageInterface::ShowMessage("   Setting MM in index %d; "
                           "array has size %d\n", i, measurements.size());
               #endif
               if (allowedMeasurements.size() > 0)
               {
                  // restrict to allowed models for this tracking system
                  MeasurementModel* model = (MeasurementModel*)obj;
                  Integer id = model->GetModelTypeID();
                  if (find(allowedMeasurements.begin(),
                        allowedMeasurements.end(), id) !=
                              allowedMeasurements.end())
                     measurements[i] = (MeasurementModel*)obj;
                  else
                  {
                     std::stringstream idStr;
                     idStr << "Measurement type " << id << " is not valid in a "
                           << typeName << " tracking system";
                     MessageInterface::ShowMessage("%s\n", idStr.str().c_str());
                     throw TrackingSystemException(idStr.str());
                  }
               }
               else
                  measurements[i] = (MeasurementModel*)obj;
               return true;
            }
         break;

      default:
         break;
   }

   return GmatBase::SetRefObject(obj, type, name);
}


//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//       const std::string &name, const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets a pointer to a referenced object of a given type and name in an array of
 * objects of that type
 *
 * @param obj  The object
 * @param type The object's type
 * @param name The object's name
 * @param index The index into the object array
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool TrackingSystem::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
         const std::string & name, const Integer index)
{
   #ifdef DEBUG_TRACKINGSYSTEM_INITIALIZATION
      MessageInterface::ShowMessage(""
            "Setting indexed ref object %s with type %s\n", name.c_str(),
            obj->GetTypeName().c_str());
   #endif

   return GmatBase::SetRefObject(obj, type, name, index);
}


//------------------------------------------------------------------------------
// ObjectArray& GetRefObjectArray(const std::string & typeString)
//------------------------------------------------------------------------------
/**
 * This method retrieves an array of reference objects of a given type
 *
 * @param typeString The type of object requested
 *
 * @return The array of objects
 */
//------------------------------------------------------------------------------
ObjectArray& TrackingSystem::GetRefObjectArray(const std::string & typeString)
{
   return GmatBase::GetRefObjectArray(typeString);
}


//------------------------------------------------------------------------------
// ObjectArray& GetRefObjectArray(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * This method retrieves an array of reference objects of a given type
 *
 * @param type The type of object requested
 *
 * @return The array of objects
 */
//------------------------------------------------------------------------------
ObjectArray& TrackingSystem::GetRefObjectArray(const Gmat::ObjectType type)
{
   return GmatBase::GetRefObjectArray(type);
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Prepares the object for use in the mission control sequence
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool TrackingSystem::Initialize()
{
   // Pass in the names of the corrections
   for (UnsignedInt i = 0; i < measurements.size(); ++i)
   {
      if (measurements[i] == NULL)
         throw TrackingSystemException("Cannot initialize; the measurement \"" +
               trackingDataNames[i] + "\" is not set.");
      measurements[i]->SetCorrection(troposphereModel, "TroposphereModel");
      measurements[i]->SetCorrection(ionosphereModel, "IonosphereModel");
   }
   return true;
}


//------------------------------------------------------------------------------
// UnsignedInt GetMeasurementCount()
//------------------------------------------------------------------------------
/**
 * Returns the number of measurements in this tracking system
 *
 * @return The number of measurements
 */
//------------------------------------------------------------------------------
UnsignedInt TrackingSystem::GetMeasurementCount()
{
   return measurements.size();
}


//------------------------------------------------------------------------------
// MeasurementModel* GetMeasurement(Integer i)
//------------------------------------------------------------------------------
/**
 * Accesses specific measurements
 *
 * @param i The index of the desired measurement
 *
 * @return The measuremetn model for the measurement
 */
//------------------------------------------------------------------------------
MeasurementModel* TrackingSystem::GetMeasurement(Integer i)
{
   return measurements[i];
}
